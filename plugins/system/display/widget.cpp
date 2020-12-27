#include "widget.h"
#include "controlpanel.h"
#include "declarative/qmloutput.h"
#include "declarative/qmlscreen.h"
#include "utils.h"
#include "ui_display.h"
#include "displayperformancedialog.h"

#include <QHBoxLayout>
#include <QTimer>
#include <QLabel>
#include <QVBoxLayout>
#include <QSplitter>
#include <QtGlobal>
#include <QQuickView>
#include <qquickitem.h>
#include <QDebug>
#include <QPushButton>
#include <QProcess>
#include <QtAlgorithms>
#include <QtXml>
#include <QDomDocument>
#include <QDir>
#include <QStandardPaths>
#include <QComboBox>
#include <QQuickWidget>
#include <QMessageBox>

#include <KF5/KScreen/kscreen/output.h>
#include <KF5/KScreen/kscreen/edid.h>
#include <KF5/KScreen/kscreen/mode.h>
#include <KF5/KScreen/kscreen/config.h>
#include <KF5/KScreen/kscreen/getconfigoperation.h>
#include <KF5/KScreen/kscreen/configmonitor.h>
#include <KF5/KScreen/kscreen/setconfigoperation.h>
#include <KF5/KScreen/kscreen/edid.h>

#ifdef signals
#undef signals
#endif

extern "C" {
#define MATE_DESKTOP_USE_UNSTABLE_API
#include <libmate-desktop/mate-rr.h>
#include <libmate-desktop/mate-rr-config.h>
#include <libmate-desktop/mate-rr-labeler.h>
#include <libmate-desktop/mate-desktop-utils.h>
}

#define QML_PATH "kcm_kscreen/qml/"

#define UKUI_CONTORLCENTER_PANEL_SCHEMAS "org.ukui.control-center.panel.plugins"
#define NIGHT_MODE_KEY                   "nightmodestatus"
#define THEME_NIGHT_KEY                  "themebynight"

#define FONT_RENDERING_DPI               "org.ukui.SettingsDaemon.plugins.xsettings"
#define SCALE_KEY                        "scaling-factor"

#define MOUSE_SIZE_SCHEMAS               "org.ukui.peripherals-mouse"
#define CURSOR_SIZE_KEY                  "cursor-size"

#define POWER_SCHMES                     "org.ukui.power-manager"
#define POWER_KEY                        "brightness-ac"
#define POWER_BAT_KEY                    "brightness-bat"

#define ADVANCED_SCHEMAS                 "org.ukui.session.required-components"
#define ADVANCED_KEY                     "windowmanager"

const QString kCpu = "ZHAOXIN";

Q_DECLARE_METATYPE(KScreen::OutputPtr)

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DisplayWindow()), slider(new Slider())
{
    qDebug("enter open ui DisplayWindow\n");
    qRegisterMetaType<QQuickView*>();
    gdk_init(NULL, NULL);

    ui->setupUi(this);
    ui->monitorLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
    ui->quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);

#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)
    oriApply = true;
#else
    mOriApply = false;
    if (!getCpuInfo().startsWith(kCpu, Qt::CaseInsensitive)) {
        ui->quickWidget->setAttribute(Qt::WA_AlwaysStackOnTop);
        ui->quickWidget->setClearColor(Qt::transparent);
    }

#endif

    ui->quickWidget->setContentsMargins(0,0,0,9);

    mCloseScreenButton = new SwitchButton(this);
    ui->showScreenLayout->addWidget(mCloseScreenButton);

    mUnifyButton = new SwitchButton(this);
    ui->unionLayout->addWidget(mUnifyButton);

    initNightUI();

    QProcess * process = new QProcess;
    process->start("lsb_release -r");
    process->waitForFinished();

    QByteArray ba = process->readAllStandardOutput();
    QString osReleaseCrude = QString(ba.data());
    QStringList res = osReleaseCrude.split(":");
    QString osRelease = res.length() >= 2 ?  res.at(1) : "";
    osRelease = osRelease.simplified();

    const QByteArray idd(ADVANCED_SCHEMAS);
    if (QGSettings::isSchemaInstalled(idd) && osRelease == "V10"){
        ui->advancedBtn->show();
        ui->advancedHorLayout->setContentsMargins(9, 8, 9, 32);
    } else {
        ui->advancedBtn->hide();
        ui->advancedHorLayout->setContentsMargins(9, 0, 9, 0);
    }

    initGSettings();
    initTemptSlider();
    initConfigFile(false, false);
    initUiComponent();
    initNightStatus();
    initBrightnessUI();

#if QT_VERSION <= QT_VERSION_CHECK(5,12,0)
    ui->nightframe->setVisible(false);
#else
    ui->nightframe->setVisible(this->mRedshiftIsValid);
#endif

    mNightButton->setChecked(this->mIsNightMode);
    showNightWidget(mNightButton->isChecked());

    initConnection();
    loadQml();
}

Widget::~Widget() {
    clearOutputIdentifiers();
    delete ui;
}

bool Widget::eventFilter(QObject* object, QEvent* event) {
    if (event->type() == QEvent::Resize) {
        if (mOutputIdentifiers.contains(qobject_cast<QQuickView*>(object))) {
            QResizeEvent *e = static_cast<QResizeEvent*>(event);
            const QRect screenSize = object->property("screenSize").toRect();
            QRect geometry(QPoint(0, 0), e->size());
            geometry.moveCenter(screenSize.center());
            static_cast<QQuickView*>(object)->setGeometry(geometry);
            // Pass the event further
        }
    }
    return QObject::eventFilter(object, event);
}


void Widget::setConfig(const KScreen::ConfigPtr &config) {
    if (mConfig) {
        KScreen::ConfigMonitor::instance()->removeConfig(mConfig);
        for (const KScreen::OutputPtr &output : mConfig->outputs()) {
            output->disconnect(this);
        }
        mConfig->disconnect(this);
    }

    mConfig = config;
    mPrevConfig = config->clone();

    KScreen::ConfigMonitor::instance()->addConfig(mConfig);
    resetPrimaryCombo();
    connect(mConfig.data(), &KScreen::Config::outputAdded,
            this, &Widget::outputAdded);
    connect(mConfig.data(), &KScreen::Config::outputRemoved,
            this, &Widget::outputRemoved);
    connect(mConfig.data(), &KScreen::Config::primaryOutputChanged,
            this, &Widget::primaryOutputChanged);

    // 上面屏幕拿取配置
    mScreen->setConfig(mConfig);
    mControlPanel->setConfig(mConfig);
    mUnifyButton->setEnabled(mConfig->outputs().count() > 1);
    ui->unionframe->setVisible(mConfig->outputs().count() > 1);

    for (const KScreen::OutputPtr &output : mConfig->outputs()) {
        outputAdded(output);
    }

    // 选择主屏幕输出
    QMLOutput *qmlOutput = mScreen->primaryOutput();
    if (qmlOutput) {
        mScreen->setActiveOutput(qmlOutput);
    } else {
        if (!mScreen->outputs().isEmpty()) {
            mScreen->setActiveOutput(mScreen->outputs().at(0));
            //选择一个主屏幕，避免闪退现象
            primaryButtonEnable(true);
        }
    }
    slotOutputEnabledChanged();

    if (isCloneMode()) {
        mUnifyButton->setChecked(true);
        slotUnifyOutputs();
    }
}

KScreen::ConfigPtr Widget::currentConfig() const {
    return mConfig;
}

void Widget::loadQml() {
    qmlRegisterType<QMLOutput>("org.kde.kscreen", 1, 0, "QMLOutput");
    qmlRegisterType<QMLScreen>("org.kde.kscreen", 1, 0, "QMLScreen");

    qmlRegisterType<KScreen::Output>("org.kde.kscreen", 1, 0, "KScreenOutput");
    qmlRegisterType<KScreen::Edid>("org.kde.kscreen", 1, 0, "KScreenEdid");
    qmlRegisterType<KScreen::Mode>("org.kde.kscreen", 1, 0, "KScreenMode");

    ui->quickWidget->setSource(QUrl("qrc:/qml/main.qml"));

    QQuickItem* rootObject = ui->quickWidget->rootObject();
    mScreen = rootObject->findChild<QMLScreen*>(QStringLiteral("outputView"));
    if (!mScreen) {
        return;
    }
    connect(mScreen, &QMLScreen::focusedOutputChanged,
            this, &Widget::slotFocusedOutputChanged);
}

void Widget::resetPrimaryCombo() {
#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)

#else
    bool isPrimaryDisplaySupported = mConfig->supportedFeatures().testFlag(KScreen::Config::Feature::PrimaryDisplay);
    ui->primaryLabel->setVisible(isPrimaryDisplaySupported);
    ui->primaryCombo->setVisible(isPrimaryDisplaySupported);
#endif

    // Don't emit currentIndexChanged when resetting
    bool blocked = ui->primaryCombo->blockSignals(true);
    ui->primaryCombo->clear();
    ui->primaryCombo->blockSignals(blocked);

    if (!mConfig) {
        return;
    }

    for (auto &output: mConfig->outputs()) {
        addOutputToPrimaryCombo(output);
    }
}

void Widget::addOutputToPrimaryCombo(const KScreen::OutputPtr &output) {
    // 注释后让他显示全部屏幕下拉框
    if (!output->isConnected()) {
        return;
    }

    ui->primaryCombo->addItem(Utils::outputName(output), output->id());
    if (output->isPrimary()) {
        Q_ASSERT(mConfig);
        int lastIndex = ui->primaryCombo->count() - 1;
        ui->primaryCombo->setCurrentIndex(lastIndex);
    }
}

//这里从屏幕点击来读取输出
void Widget::slotFocusedOutputChanged(QMLOutput *output) {
    mControlPanel->activateOutput(output->outputPtr());

    //读取屏幕点击选择下拉框
    Q_ASSERT(mConfig);
    int index = output->outputPtr().isNull() ? 0 : ui->primaryCombo->findData(output->outputPtr()->id());
    if (index == -1 || index == ui->primaryCombo->currentIndex()) {
        return;
    }
    ui->primaryCombo->setCurrentIndex(index);
}

void Widget::slotFocusedOutputChangedNoParam() {
    mControlPanel->activateOutput(res);
}


void Widget::slotOutputEnabledChanged() {
    // 这里为点击禁用屏幕输出后的改变
    resetPrimaryCombo();
    int enabledOutputsCount = 0;
    Q_FOREACH (const KScreen::OutputPtr &output, mConfig->outputs()) {
        if (output->isEnabled()) {
            ++enabledOutputsCount;
        }
        if (enabledOutputsCount > 1) {
            break;
        }
    }
    mUnifyButton->setEnabled(enabledOutputsCount > 1);
    ui->unionframe->setVisible(enabledOutputsCount > 1);
}

void Widget::slotOutputConnectedChanged() {
    resetPrimaryCombo();
}

void Widget::slotUnifyOutputs() {

    QMLOutput *base = mScreen->primaryOutput();

    QList<int> clones;

    if (!base) {
        for (QMLOutput *output: mScreen->outputs()) {
            if (output->output()->isConnected() && output->output()->isEnabled()) {
                base = output;
                break;
            }
        }
        if (!base) {
            // WTF?
            return;
        }
    }

    if (base->isCloneMode() && !mUnifyButton->isChecked()) {
        QPoint secPoint;
        KScreen::OutputList screens =  mPrevConfig->connectedOutputs();

        QMap<int, KScreen::OutputPtr>::iterator it = screens.begin();
        while (it != screens.end()) {

            KScreen::OutputPtr screen= it.value();
            if (screen->isPrimary()) {
                secPoint = QPoint(screen->size().width(),0);
            }
            it++;
        }

        QMap<int, KScreen::OutputPtr>::iterator secIt = screens.begin();
        while (secIt != screens.end()) {
            KScreen::OutputPtr screen= secIt.value();
            if (!screen->isPrimary()) {
                screen->setPos(secPoint);
            }
            secIt++;
        }

        setConfig(mPrevConfig);
        mPrevConfig.clear();

        ui->primaryCombo->setEnabled(true);
        mCloseScreenButton->setEnabled(true);
        ui->primaryCombo->setEnabled(true);
    } else if (!base->isCloneMode() && mUnifyButton->isChecked()){
        // Clone the current config, so that we can restore it in case user
        // breaks the cloning
        mPrevConfig = mConfig->clone();

        for (QMLOutput *output: mScreen->outputs()) {
            if (output != mScreen->primaryOutput()) {
                output->output()->setRotation(mScreen->primaryOutput()->output()->rotation());
            }

            if (!output->output()->isConnected()) {
                continue;
            }

            if (!output->output()->isEnabled()) {

                output->setVisible(false);
                continue;
            }

            if (!base) {
                base = output;
            }

            output->setOutputX(0);
            output->setOutputY(0);
            output->output()->setPos(QPoint(0, 0));
            output->output()->setClones(QList<int>());

            if (base != output) {
                clones << output->output()->id();
                output->setCloneOf(base);
                output->setVisible(false);
            }
        }

        base->output()->setClones(clones);

        base->setIsCloneMode(true);

        mScreen->updateOutputsPlacement();

        //关闭开关
        mCloseScreenButton->setEnabled(false);
        ui->primaryCombo->setEnabled(false);
        ui->mainScreenButton->setEnabled(false);

        mControlPanel->setUnifiedOutput(base->outputPtr());
    }
    Q_EMIT changed();
}

// FIXME: Copy-pasted from KDED's Serializer::findOutput()
KScreen::OutputPtr Widget::findOutput(const KScreen::ConfigPtr &config, const QVariantMap &info) {
    KScreen::OutputList outputs = config->outputs();
    Q_FOREACH(const KScreen::OutputPtr &output, outputs) {
        if (!output->isConnected()) {
            continue;
        }

        const QString outputId = (output->edid() && output->edid()->isValid()) ? output->edid()->hash() : output->name();
        if (outputId != info[QStringLiteral("id")].toString()) {
            continue;
        }

        QVariantMap posInfo = info[QStringLiteral("pos")].toMap();
        QPoint point(posInfo[QStringLiteral("x")].toInt(), posInfo[QStringLiteral("y")].toInt());
        output->setPos(point);
        output->setPrimary(info[QStringLiteral("primary")].toBool());
        output->setEnabled(info[QStringLiteral("enabled")].toBool());
        output->setRotation(static_cast<KScreen::Output::Rotation>(info[QStringLiteral("rotation")].toInt()));

        QVariantMap modeInfo = info[QStringLiteral("mode")].toMap();
        QVariantMap modeSize = modeInfo[QStringLiteral("size")].toMap();
        QSize size(modeSize[QStringLiteral("width")].toInt(), modeSize[QStringLiteral("height")].toInt());

        const KScreen::ModeList modes = output->modes();
        Q_FOREACH(const KScreen::ModePtr &mode, modes) {
            if (mode->size() != size) {
                continue;
            }
            if (QString::number(mode->refreshRate()) != modeInfo[QStringLiteral("refresh")].toString()) {
                continue;
            }

            output->setCurrentModeId(mode->id());
            break;
        }
        return output;
    }

    return KScreen::OutputPtr();
}

void Widget::writeScale(int scale) {
    if (mIsScaleChanged) {
        QMessageBox::information(this, tr("Information"), tr("Some applications need to be logouted to take effect"));
    } else {
        return;
    }
    mIsScaleChanged = false;
    int cursize;
    QByteArray iid(MOUSE_SIZE_SCHEMAS);
    if (QGSettings::isSchemaInstalled(MOUSE_SIZE_SCHEMAS)) {
        QGSettings cursorSettings(iid);

        if (1 == scale)  {
            cursize = 24;
        } else if (2 == scale) {
            cursize = 48;
        } else if (3 == scale) {
            cursize = 96;
        } else {
            scale = 1;
            cursize = 24;
        }

        QStringList keys = scaleGSettings->keys();
        if (keys.contains("scalingFactor")) {
            scaleGSettings->set(SCALE_KEY, scale);
        }
        cursorSettings.set(CURSOR_SIZE_KEY, cursize);
    }
}

void Widget::initGSettings() {
    QByteArray id(UKUI_CONTORLCENTER_PANEL_SCHEMAS);
    if(QGSettings::isSchemaInstalled(id)) {
        mGsettings = new QGSettings(id, QByteArray(), this);
        if (mGsettings->keys().contains(THEME_NIGHT_KEY)) {
            mThemeButton->setChecked(mGsettings->get(THEME_NIGHT_KEY).toBool());
        }
    } else {
        qDebug() << Q_FUNC_INFO << "org.ukui.control-center.panel.plugins not install";
        return;
    }

    connect(mGsettings, &QGSettings::changed, this, [=](QString key) {
        if (static_cast<QString>(NIGHT_MODE_KEY) == key) {
            bool status =  mGsettings->get(key).toBool();
            initConfigFile(true, status);
        }
    });

    QByteArray powerId(POWER_SCHMES);
    if (QGSettings::isSchemaInstalled(powerId)) {
        mPowerGSettings = new QGSettings(powerId, QByteArray(), this);
        mPowerKeys = mPowerGSettings->keys();
        connect(mPowerGSettings, &QGSettings::changed, this, [=](QString key) {
            if ("brightnessAc" == key || "brightnessBat" == key) {
                ui->brightnessSlider->setValue(mPowerGSettings->get(key).toInt());
            }
        });
    }

    QByteArray scaleId(FONT_RENDERING_DPI);
    if (QGSettings::isSchemaInstalled(scaleId)) {
        scaleGSettings = new QGSettings(scaleId, QByteArray(), this);
    }
}

void Widget::writeConfigFile() {

    if (mGsettings) {
        mGsettings->set(NIGHT_MODE_KEY, mNightButton->isChecked());
    }

    mQsettings->beginGroup("redshift");
    QString optime = ui->opHourCom->currentText() + ":" + ui->opMinCom->currentText();
    QString cltime = ui->clHourCom->currentText() + ":" + ui->clMinCom->currentText();
    QString value = QString::number(ui->temptSlider->value());


    if ( !ui->customradioBtn->isChecked()) {
        optime = "17:55";
        cltime = "05:04";
    }

    mQsettings->setValue("dawn-time", cltime);
    mQsettings->setValue("dusk-time", optime);
    mQsettings->setValue("temp-day", tempDayBrig);
    mQsettings->setValue("temp-night", value);

    mQsettings->endGroup();

    mQsettings->beginGroup("switch");
    mQsettings->setValue("nightjudge", mNightButton->isChecked());
    mQsettings->setValue("sunjudge", ui->sunradioBtn->isChecked());
    mQsettings->setValue("manualjudge", ui->customradioBtn->isChecked());
    mQsettings->setValue("nightStatus", mNightButton->isChecked());

    mQsettings->endGroup();
    mQsettings->sync();
}

void Widget::setcomBoxScale() {
    int scale = 1;
    QComboBox *scaleCombox = findChild<QComboBox*>(QString("scaleCombox"));
    if (scaleCombox) {
        scale = ("100%" == scaleCombox->currentText() ? 1 : 2);
    }
    writeScale(scale);
}

void Widget::initNightUI() {
    //~ contents_path /display/unify output
    ui->unifyLabel->setText(tr("unify output"));

    QHBoxLayout *nightLayout = new QHBoxLayout(ui->nightframe);
    //~ contents_path /display/night mode
    nightLabel = new QLabel(tr("night mode"), this);
    mNightButton = new SwitchButton(this);
    nightLayout->addWidget(nightLabel);
    nightLayout->addStretch();
    nightLayout->addWidget(mNightButton);

    QHBoxLayout *themeLayout = new QHBoxLayout(ui->themeFrame);
    mThemeButton = new SwitchButton(this);
    themeLayout->addWidget(new QLabel(tr("Theme follow night mode")));
    themeLayout->addStretch();
    themeLayout->addWidget(mThemeButton);
}

bool Widget::isRestoreConfig() {
    int cnt = 10;
    int ret;
    QMessageBox msg;
    if (mConfigChanged) {
        msg.setWindowTitle(tr("Hint"));
        msg.setText(tr("After modifying the resolution or refresh rate, "
                       "due to compatibility issues between the display device and the graphics card, "
                       "the display may be abnormal or unable to display\n"
                       "If something goes wrong, the settings will be restored after 10 seconds"));
        msg.addButton(tr("Save Config"), QMessageBox::AcceptRole);
        msg.addButton(tr("Restore Config"), QMessageBox::RejectRole);

        QTimer cntDown;
        QObject::connect(&cntDown, &QTimer::timeout, [&msg,&cnt, &cntDown, &ret]()->void{
            if (--cnt < 0) {
                cntDown.stop();
                msg.close();
            } else {
                msg.setText(QString(tr("After modifying the resolution or refresh rate, "
                                    "due to compatibility issues between the display device and the graphics card, "
                                    "the display may be abnormal or unable to display \n"
                                    "If something goes wrong, the settings will be restored after %1 seconds")).arg(cnt));
            }
        });
        cntDown.start(1000);
        ret = msg.exec();
    }

    bool res = true;
    switch (ret) {
      case QMessageBox::AcceptRole:
          res = false;
          break;
      case QMessageBox::RejectRole:
          res = true;
          break;
    }
    return res;
}

QString Widget::getCpuInfo() {
    QDBusInterface youkerInterface("com.kylin.assistant.systemdaemon",
                                   "/com/kylin/assistant/systemdaemon",
                                   "com.kylin.assistant.systemdaemon",
                                   QDBusConnection::systemBus());
    if (!youkerInterface.isValid()) {
        qCritical() << "Create youker Interface Failed When Get Computer info: " << QDBusConnection::systemBus().lastError();
        return QString();
    }

    QDBusReply<QMap<QString, QVariant>> cpuinfo;
    QString cpuType;
    cpuinfo  = youkerInterface.call("get_cpu_info");
    if (!cpuinfo.isValid()) {
        qDebug() << "cpuinfo is invalid" << endl;
    } else {
        QMap<QString, QVariant> res = cpuinfo.value();
        cpuType = res["CpuVersion"].toString();
    }
    return cpuType;
}

bool Widget::isCloneMode()
{
    MateRRScreen *rr_screen;
    MateRRConfig *rr_config;

    rr_screen = mate_rr_screen_new (gdk_screen_get_default (), NULL); /* NULL-GError */
    if (!rr_screen)
            return false;

    rr_config = mate_rr_config_new_current (rr_screen, NULL);

    return mate_rr_config_get_clone(rr_config);
}


void Widget::showNightWidget(bool judge) {
    if (judge) {
        ui->sunframe->setVisible(true);
        ui->customframe->setVisible(true);
        ui->temptframe->setVisible(true);
        ui->themeFrame->setVisible(true);
    } else {
        ui->sunframe->setVisible(false);
        ui->customframe->setVisible(false);
        ui->temptframe->setVisible(false);
        ui->themeFrame->setVisible(false);
    }

   if (judge && ui->customradioBtn->isChecked()) {
       showCustomWiget(CUSTOM);
   } else {
       showCustomWiget(SUN);
   }
}

void Widget::showCustomWiget(int index) {
    if (SUN == index) {
        ui->opframe->setVisible(false);
        ui->clsframe->setVisible(false);
    } else if (CUSTOM == index) {
        ui->opframe->setVisible(true);;
        ui->clsframe->setVisible(true);
    }
}

void Widget::slotThemeChanged(bool judge) {
    if (mGsettings->keys().contains(THEME_NIGHT_KEY)) {
        mGsettings->set(THEME_NIGHT_KEY, judge);
    }
}

void Widget::clearOutputIdentifiers() {
    mOutputTimer->stop();
    qDeleteAll(mOutputIdentifiers);
    mOutputIdentifiers.clear();
}

void Widget::outputAdded(const KScreen::OutputPtr &output) {
    connect(output.data(), &KScreen::Output::isConnectedChanged,
            this, &Widget::slotOutputConnectedChanged);
    connect(output.data(), &KScreen::Output::isEnabledChanged,
            this, &Widget::slotOutputEnabledChanged);
    connect(output.data(), &KScreen::Output::posChanged,
            this, &Widget::changed);

    addOutputToPrimaryCombo(output);
}

void Widget::outputRemoved(int outputId) {
    KScreen::OutputPtr output = mConfig->output(outputId);
    if (!output.isNull()) {
        output->disconnect(this);
    }

    const int index = ui->primaryCombo->findData(outputId);
    if (index == -1) {
        return;
    }

    if (index == ui->primaryCombo->currentIndex()) {
        // We'll get the actual primary update signal eventually
        // Don't emit currentIndexChanged
        const bool blocked = ui->primaryCombo->blockSignals(true);
        ui->primaryCombo->setCurrentIndex(0);
        ui->primaryCombo->blockSignals(blocked);
    }
    ui->primaryCombo->removeItem(index);
}

void Widget::primaryOutputSelected(int index) {
    if (!mConfig) {
        return;
    }

    const KScreen::OutputPtr newPrimary = index == 0 ? KScreen::OutputPtr() : mConfig->output(ui->primaryCombo->itemData(index).toInt());
    if (newPrimary == mConfig->primaryOutput()) {
        return;
    }

    mConfig->setPrimaryOutput(newPrimary);
    Q_EMIT changed();
}

//主输出
void Widget::primaryOutputChanged(const KScreen::OutputPtr &output) {
    Q_ASSERT(mConfig);
    int index = output.isNull() ? 0 : ui->primaryCombo->findData(output->id());
    if (index == -1 || index == ui->primaryCombo->currentIndex()) {
        return;
    }
    ui->primaryCombo->setCurrentIndex(index);
}

void Widget::slotIdentifyButtonClicked(bool checked) {
    Q_UNUSED(checked);
    connect(new KScreen::GetConfigOperation(), &KScreen::GetConfigOperation::finished,
            this, &Widget::slotIdentifyOutputs);
}

void Widget::slotIdentifyOutputs(KScreen::ConfigOperation *op) {
    if (op->hasError()) {
        return;
    }

    const KScreen::ConfigPtr config = qobject_cast<KScreen::GetConfigOperation*>(op)->config();

    const QString qmlPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral(QML_PATH "OutputIdentifier.qml"));

    mOutputTimer->stop();
    clearOutputIdentifiers();

    /* Obtain the current active configuration from KScreen */
    Q_FOREACH (const KScreen::OutputPtr &output, config->outputs()) {
        if (!output->isConnected() || !output->currentMode()) {
            continue;
        }

        const KScreen::ModePtr mode = output->currentMode();

        QQuickView *view = new QQuickView();

        view->setFlags(Qt::X11BypassWindowManagerHint | Qt::FramelessWindowHint);
        view->setResizeMode(QQuickView::SizeViewToRootObject);
        view->setSource(QUrl::fromLocalFile(qmlPath));
        view->installEventFilter(this);

        QQuickItem *rootObj = view->rootObject();
        if (!rootObj) {
            qWarning() << "Failed to obtain root item";
            continue;
        }

        QSize deviceSize, logicalSize;
        if (output->isHorizontal()) {
            deviceSize = mode->size();
        } else {
            deviceSize = QSize(mode->size().height(), mode->size().width());
        }

#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)
#else
        if (config->supportedFeatures() & KScreen::Config::Feature::PerOutputScaling) {
            // no scale adjustment needed on Wayland
            logicalSize = deviceSize;
        } else {
            logicalSize = deviceSize / devicePixelRatioF();
        }
#endif

        rootObj->setProperty("outputName", Utils::outputName(output));
        rootObj->setProperty("modeName", Utils::sizeToString(deviceSize));

#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)
        view->setProperty("screenSize", QRect(output->pos(), deviceSize));
#else
        view->setProperty("screenSize", QRect(output->pos(), logicalSize));
#endif

        mOutputIdentifiers << view;
    }

    for (QQuickView *view: mOutputIdentifiers) {
        view->show();
    }

    mOutputTimer->start(2500);
}


void Widget::save() {
    if (!this) {
        return;
    }

    const KScreen::ConfigPtr &config = this->currentConfig();

    bool atLeastOneEnabledOutput = false;
    Q_FOREACH(const KScreen::OutputPtr &output, config->outputs()) {
        if (output->isEnabled()) {
            atLeastOneEnabledOutput = true;
        }
        if (!output->isConnected())
            continue;

        QMLOutput *base = mScreen->primaryOutput();
        if (!base) {
            for (QMLOutput *output: mScreen->outputs()) {
                if (output->output()->isConnected() && output->output()->isEnabled()) {
                    base = output;
                    break;
                }
            }

            if (!base) {
                // WTF?
                return;
            }
        }
    }

    if (!atLeastOneEnabledOutput ) {
        QMessageBox::warning(this, tr("Warning"), tr("please insure at least one output!"));
        mCloseScreenButton->setChecked(true);
        return ;
    } else if (((ui->opHourCom->currentIndex() < ui->clHourCom->currentIndex()) ||
               (ui->opHourCom->currentIndex() == ui->clHourCom->currentIndex()  &&
                ui->opMinCom->currentIndex() <= ui->clMinCom->currentIndex()))  &&
               CUSTOM == singleButton->checkedId() && mNightButton->isChecked()) {
        QMessageBox::warning(this, tr("Warning"), tr("Morning time should be earlier than evening time!"));
        mCloseScreenButton->setChecked(true);
        return ;
    }

    writeScale(this->screenScale);
    writeConfigFile();
    setNightMode(mNightButton->isChecked());

    if (!KScreen::Config::canBeApplied(config)) {
        QMessageBox::information(this,
            tr("Sorry, your configuration could not be applied.\nCommon reasons are that the overall screen size is too big, or you enabled more displays than supported by your GPU."),
                 tr("@title:window", "Unsupported Configuration"));
        return;
    }

    m_blockChanges = true;
    /* Store the current config, apply settings */
    auto *op = new KScreen::SetConfigOperation(config);

    /* Block until the operation is completed, otherwise KCMShell will terminate
     * before we get to execute the Operation */
    op->exec();

    // The 1000ms is a bit "random" here, it's what works on the systems I've tested, but ultimately, this is a hack
    // due to the fact that we just can't be sure when xrandr is done changing things, 1000 doesn't seem to get in the way
    QTimer::singleShot(1000, this,
        [this] () {
            m_blockChanges = false;
        }
    );

    if (isRestoreConfig()) {
        auto *op = new KScreen::SetConfigOperation(mPrevConfig);
        op->exec();
    } else {
        mPrevConfig = config->clone();
        writeScreenXml();
    }
}

void Widget::scaleChangedSlot(int index) {
    switch (index) {
    case 0:
        this->screenScale = 1;
        break;
    case 1:
        this->screenScale = 2;
        break;
    case 2:
        this->screenScale = 3;
        break;
    default:
        this->screenScale = 1;
        break;
    }
    if (scaleGSettings->get(SCALE_KEY).toInt() != this->screenScale) {
        mIsScaleChanged = true;
    } else {
        mIsScaleChanged = false;
    }
}

void Widget::changedSlot() {
    mConfigChanged = true;
}

void Widget::propertiesChangedSlot(QString property, QMap<QString, QVariant> propertyMap, QStringList propertyList) {
    Q_UNUSED(property);
    Q_UNUSED(propertyList);
    if (propertyMap.keys().contains("OnBattery")) {
        mOnBattery = propertyMap.value("OnBattery").toBool();
    }
}

// 是否禁用主屏按钮
void Widget::mainScreenButtonSelect(int index) {
    if (!mConfig) {
        return;
    }

    const KScreen::OutputPtr newPrimary = mConfig->output(ui->primaryCombo->itemData(index).toInt());
    if (newPrimary == mConfig->primaryOutput()) {
        ui->mainScreenButton->setEnabled(false);
    } else {
        ui->mainScreenButton->setEnabled(true);
    }
    // 设置是否勾选
    mCloseScreenButton->setEnabled(true);

    // 初始化时不要发射信号
    const bool blockded = mCloseScreenButton->blockSignals(true);
    mCloseScreenButton->setChecked(newPrimary->isEnabled());
    mCloseScreenButton->blockSignals(blockded);
    mControlPanel->activateOutput(newPrimary);
}


// 设置主屏按钮
void Widget::primaryButtonEnable(bool status) {
    Q_UNUSED(status);
    if (!mConfig) {
        return;
    }
    int index  = ui->primaryCombo->currentIndex();
    ui->mainScreenButton->setEnabled(false);
    const KScreen::OutputPtr newPrimary = mConfig->output(ui->primaryCombo->itemData(index).toInt());
    mConfig->setPrimaryOutput(newPrimary);
    Q_EMIT changed();
}

void Widget::checkOutputScreen(bool judge) {
   int index  = ui->primaryCombo->currentIndex();
   const KScreen::OutputPtr newPrimary = mConfig->output(ui->primaryCombo->itemData(index).toInt());

   KScreen::OutputPtr mainScreen = mConfig->primaryOutput();
   if (!mainScreen) {
       mConfig->setPrimaryOutput(newPrimary);
   }

   newPrimary->setEnabled(judge);

   int enabledOutput = 0;
   Q_FOREACH(KScreen::OutputPtr outptr, mConfig->outputs()) {
       if (outptr->isEnabled()) {
           enabledOutput++;
       }

       if (enabledOutput >= 2) {
           if (!mainScreen && outptr != newPrimary) {
               mainScreen = outptr;
           }
           // 设置副屏在主屏右边
           newPrimary->setPos(QPoint(mainScreen->pos().x() + mainScreen->size().width(),
                               mainScreen->pos().y()));
       }
   }

   ui->primaryCombo->setCurrentIndex(index);
   Q_EMIT changed();
}

// 亮度调节UI
void Widget::initBrightnessUI() {
    ui->brightnessSlider->setRange(0, 100);
    ui->brightnessSlider->setTracking(true);

    setBrightnesSldierValue();

    connect(ui->brightnessSlider, &QSlider::valueChanged, this, &Widget::setBrightnessScreen);
}

void Widget::initConnection() {
    connect(mNightButton, SIGNAL(checkedChanged(bool)), this, SLOT(showNightWidget(bool)));
    connect(mThemeButton, SIGNAL(checkedChanged(bool)), this, SLOT(slotThemeChanged(bool)));
    connect(singleButton, SIGNAL(buttonClicked(int)), this,  SLOT(showCustomWiget(int)));
    //是否禁用主显示器确认按钮
    connect(ui->primaryCombo, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &Widget::mainScreenButtonSelect);
    //主屏确认按钮
    connect(ui->mainScreenButton, SIGNAL(clicked(bool)), this, SLOT(primaryButtonEnable(bool)));
    mControlPanel = new ControlPanel(this);
    connect(mControlPanel, &ControlPanel::changed, this, &Widget::changed);
    connect(this, &Widget::changed, this, &Widget::changedSlot);
    connect(mControlPanel, &ControlPanel::scaleChanged, this, &Widget::scaleChangedSlot);

    ui->controlPanelLayout->addWidget(mControlPanel);

    // TODO: Find out why adjusting the screen orientation does not take effect
    connect(ui->applyButton, &QPushButton::clicked, this, [=]() {
        save();
    });

    connect(ui->advancedBtn, &QPushButton::clicked, this, [=] {
        DisplayPerformanceDialog * dialog = new DisplayPerformanceDialog;
        dialog->exec();
    });

    connect(mUnifyButton,&SwitchButton::checkedChanged,
            [this]{
        slotUnifyOutputs();
    });

    //TODO----->bug
//    ui->showMonitorwidget->setVisible(false);
    connect(mCloseScreenButton, &SwitchButton::checkedChanged,
            this, [=](bool checked) {
        checkOutputScreen(checked);
    });

    mOutputTimer = new QTimer(this);
    connect(mOutputTimer, &QTimer::timeout,
            this, &Widget::clearOutputIdentifiers);
}


void Widget::setBrightnessScreen(int value) {

    if (mPowerKeys.contains("brightnessBat") && mOnBattery) {
        mPowerGSettings->set(POWER_BAT_KEY, value);
    } else {
        mPowerGSettings->set(POWER_KEY, value);
    }
}

//滑块改变
void Widget::setBrightnesSldierValue() {
    int value = 99;
    if (mPowerKeys.contains("brightnessBat") && mOnBattery) {
        value = mPowerGSettings->get(POWER_BAT_KEY).toInt();
    } else {
        value = mPowerGSettings->get(POWER_KEY).toInt();
    }

    ui->brightnessSlider->setValue(value);
}

void Widget::initTemptSlider() {
    ui->temptSlider->setRange(1.1*1000, 6500);
    ui->temptSlider->setTracking(true);

    for (int i = 0; i < 24; i++) {
        ui->opHourCom->addItem(QString::number(i));
        ui->clHourCom->addItem(QString::number(i));
    }

    for (int i = 0; i < 60; i++) {
        ui->opMinCom->addItem(QString::number(i));
        ui->clMinCom->addItem(QString::number(i));
    }
}

void Widget::initConfigFile(bool changed, bool status) {
    QString filename = QDir::homePath() + "/.config/redshift.conf";
    mQsettings = new QSettings(filename, QSettings::IniFormat,this);

    mQsettings->beginGroup("redshift");

    QString optime = mQsettings->value("dusk-time", "").toString();
    QString cltime = mQsettings->value("dawn-time", "").toString();
    QString temptValue = mQsettings->value("temp-night", "").toString();

    if ("" != optime) {
        QString ophour = optime.split(":").at(0);
        QString opmin = optime.split(":").at(1);

        ui->opHourCom->setCurrentIndex(ophour.toInt());
        ui->opMinCom->setCurrentIndex(opmin.toInt());
    }

    if ("" != cltime) {
        QString clhour = cltime.split(":").at(0);
        QString clmin = cltime.split(":").at(1);

        ui->clHourCom->setCurrentIndex(clhour.toInt());
        ui->clMinCom->setCurrentIndex(clmin.toInt());
    }

    if ("" != temptValue) {
        int value = temptValue.toInt();
        ui->temptSlider->setValue(value);
    }

    mQsettings->endGroup();

    mQsettings->beginGroup("switch");;

    bool sunjudge    = false;
    bool manualjudge = false;
    bool nightjudge  = mQsettings->value("nightjudge", nightjudge).toBool();
    manualjudge      = mQsettings->value("manualjudge", manualjudge).toBool();
    sunjudge         = mQsettings->value("sunjudge", sunjudge).toBool();

    mNightButton->setChecked(nightjudge);

    if (sunjudge || manualjudge) {
        ui->sunradioBtn->setChecked(sunjudge);
        ui->customradioBtn->setChecked(manualjudge);
    } else {
        ui->sunradioBtn->setChecked(true);
        ui->temptSlider->setValue(3500);
    }

    mQsettings->endGroup();

    if (changed) {
        mNightButton->setChecked(status);
    }
}

void Widget::writeScreenXml() {
    MateRRScreen *rr_screen;
    MateRRConfig *rr_config;

    /* Normally, mate_rr_config_save() creates a backup file based on the
     * old monitors.xml.  However, if *that* file didn't exist, there is
     * nothing from which to create a backup.  So, here we'll save the
     * current/unchanged configuration and then let our caller call
     * mate_rr_config_save() again with the new/changed configuration, so
     * that there *will* be a backup file in the end.
     */

    rr_screen = mate_rr_screen_new (gdk_screen_get_default (), NULL); /* NULL-GError */
    if (!rr_screen)
            return;

    rr_config = mate_rr_config_new_current (rr_screen, NULL);
    mate_rr_config_save (rr_config, NULL); /* NULL-GError */

    char *backup_filename = mate_rr_config_get_backup_filename();
    unlink(backup_filename);

    g_object_unref (rr_config);
    g_object_unref (rr_screen);
}


void Widget::setNightMode(const bool nightMode) {
    QProcess process;
    QString cmd;
    QString serverCmd;

    if(nightMode) {
        cmd = "restart";
        serverCmd = "enable";
    } else {
        cmd = "stop";
        serverCmd = "disable";
    }

    process.startDetached("systemctl", QStringList() << "--user" << serverCmd << "redshift.service");

    process.startDetached("systemctl", QStringList() << "--user" << cmd << "redshift.service");
    updateNightStatus();

    QGSettings theme("org.ukui.style");
    if (nightMode && mThemeButton->isChecked()) {
        theme.set("style-name", "ukui-dark");
    } else if (nightMode && !mThemeButton->isChecked()) {
        theme.set("style-name", "ukui-default");
    }
}


void Widget::updateNightStatus() {
    QProcess *process = new QProcess;

    connect(process, &QProcess::readyRead, this, [=] {
        setIsNightMode(process->readAll().replace("\n","") == "active");

        process->deleteLater();
    });

    process->start("systemctl", QStringList() << "--user" << "is-active" << "redshift.service");
    process->close();
}


void Widget::setIsNightMode(bool isNightMode) {
    if(mIsNightMode == isNightMode){
        return ;
    }
    mIsNightMode = isNightMode;
   // emit nightModeChanged(isNightMode);
}

void Widget::initUiComponent() {
    singleButton = new QButtonGroup();
    singleButton->addButton(ui->sunradioBtn);
    singleButton->addButton(ui->customradioBtn);

    singleButton->setId(ui->sunradioBtn, SUN);
    singleButton->setId(ui->customradioBtn, CUSTOM);

    MODE value = ui->customradioBtn->isChecked() == SUN ? SUN : CUSTOM;
    showNightWidget(mNightButton->isChecked());
    if (mNightButton->isChecked()) {
        showCustomWiget(value);
    }

    QDBusInterface brightnessInterface("org.freedesktop.UPower",
                                     "/org/freedesktop/UPower/devices/DisplayDevice",
                                     "org.freedesktop.DBus.Properties",
                                     QDBusConnection::systemBus());
    if (!brightnessInterface.isValid()) {
        qDebug() << "Create UPower Interface Failed : " << QDBusConnection::systemBus().lastError();
        return;
    }

    QDBusReply<QVariant> briginfo;
    briginfo  = brightnessInterface.call("Get", "org.freedesktop.UPower.Device", "PowerSupply");
    if (!briginfo.value().toBool()) {
        ui->brightnessframe->setVisible(false);
    } else {
        ui->brightnessframe->setVisible(true);
    }

    mUPowerInterface = QSharedPointer<QDBusInterface>(
                new QDBusInterface("org.freedesktop.UPower",
                                   "/org/freedesktop/UPower",
                                   "org.freedesktop.DBus.Properties",
                                   QDBusConnection::systemBus()));

    if (!mUPowerInterface.get()->isValid()) {
        qDebug() << "Create UPower Battery Interface Failed : " << QDBusConnection::systemBus().lastError();
        return;
    }

    QDBusReply<QVariant> batteryInfo;
    batteryInfo  = mUPowerInterface.get()->call("Get", "org.freedesktop.UPower", "OnBattery");
    if (batteryInfo.isValid()) {
        mOnBattery = batteryInfo.value().toBool();
    }

    mUPowerInterface.get()->connection().connect("org.freedesktop.UPower",
                                                 "/org/freedesktop/UPower",
                                                 "org.freedesktop.DBus.Properties",
                                                 "PropertiesChanged",
                                                 this,
                                                 SLOT(propertiesChangedSlot(QString, QMap<QString, QVariant>, QStringList)));
}

void Widget::setRedShiftIsValid(bool redshiftIsValid) {
    if(mRedshiftIsValid == redshiftIsValid) {
        return ;
    }

    mRedshiftIsValid = redshiftIsValid;
    emit redShiftValidChanged(redshiftIsValid);
}

void Widget::initNightStatus() {
    QProcess *process = new QProcess;
    const bool isRedShiftValid  = (0 == process->execute("which",QStringList() << "redshift"));

    QProcess *process_2 = new QProcess;
    process_2->start("systemctl", QStringList() << "--user" << "is-active" << "redshift.service");
    process_2->waitForFinished();

    QByteArray qbaOutput = process_2->readAllStandardOutput();

    QString tmpNight = qbaOutput;
    mIsNightMode = (tmpNight=="active\n" ? true : false);

    if (isRedShiftValid) {
        updateNightStatus();
    }
    setRedShiftIsValid(isRedShiftValid);
}
