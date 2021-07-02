#include "widget.h"
#include "controlpanel.h"
#include "declarative/qmloutput.h"
#include "declarative/qmlscreen.h"
#include "utils.h"
#include "ui_display.h"
#include "displayperformancedialog.h"
#include "colorinfo.h"
#include "../../../shell/utils/utils.h"
#include "../../../shell/mainwindow.h"

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
#include <QDBusConnection>
#include <QJsonDocument>
#include <QtConcurrent>
#include <QVariantMap>
#include <QDBusMetaType>

#include <KF5/KScreen/kscreen/output.h>
#include <KF5/KScreen/kscreen/edid.h>
#include <KF5/KScreen/kscreen/mode.h>
#include <KF5/KScreen/kscreen/config.h>
#include <KF5/KScreen/kscreen/getconfigoperation.h>
#include <KF5/KScreen/kscreen/configmonitor.h>
#include <KF5/KScreen/kscreen/setconfigoperation.h>
#include <KF5/KScreen/kscreen/edid.h>
#include <KF5/KScreen/kscreen/types.h>

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
#define THEME_NIGHT_KEY                  "themebynight"

#define FONT_RENDERING_DPI               "org.ukui.SettingsDaemon.plugins.xsettings"
#define SCALE_KEY                        "scaling-factor"

#define MOUSE_SIZE_SCHEMAS               "org.ukui.peripherals-mouse"
#define CURSOR_SIZE_KEY                  "cursor-size"

#define POWER_SCHMES                     "org.ukui.power-manager"
#define POWER_KEY                        "brightness-ac"

#define ADVANCED_SCHEMAS                 "org.ukui.session.required-components"
#define ADVANCED_KEY                     "windowmanager"

const QString kCpu = "ZHAOXIN";
const QString kLoong = "Loongson";
const QString tempDayBrig = "6500";

Q_DECLARE_METATYPE(KScreen::OutputPtr)

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DisplayWindow())
{
    qRegisterMetaType<QQuickView *>();
    gdk_init(NULL, NULL);

    ui->setupUi(this);
    ui->quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    ui->quickWidget->setContentsMargins(0, 0, 0, 9);

    mCloseScreenButton = new SwitchButton(this);
    ui->showScreenLayout->addWidget(mCloseScreenButton);

    mUnifyButton = new SwitchButton(this);
    ui->unionLayout->addWidget(mUnifyButton);

    qDBusRegisterMetaType<ScreenConfig>();

    setHideModuleInfo();
    initNightUI();
    isWayland();

    QProcess *process = new QProcess;
    process->start("lsb_release -r");
    process->waitForFinished();

    QByteArray ba = process->readAllStandardOutput();
    QString osReleaseCrude = QString(ba.data());
    QStringList res = osReleaseCrude.split(":");
    QString osRelease = res.length() >= 2 ? res.at(1) : "";
    osRelease = osRelease.simplified();

    const QByteArray idd(ADVANCED_SCHEMAS);
    if (QGSettings::isSchemaInstalled(idd) && osRelease == "V10") {
        ui->advancedBtn->show();
        ui->advancedHorLayout->setContentsMargins(9, 8, 9, 32);
    } else {
        ui->advancedBtn->hide();
        ui->advancedHorLayout->setContentsMargins(9, 0, 9, 0);
    }

    setTitleLabel();
    initGSettings();
    initTemptSlider();
    initUiComponent();
    initNightStatus();

#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)
    ui->nightframe->setVisible(false);
#else
    ui->nightframe->setVisible(this->mRedshiftIsValid);
#endif

    mNightButton->setChecked(this->mIsNightMode);
    showNightWidget(mNightButton->isChecked());

    initConnection();
    loadQml();

    mScreenScale = scaleGSettings->get(SCALE_KEY).toDouble();
}

Widget::~Widget()
{
    clearOutputIdentifiers();
    delete ui;
    ui = nullptr;
}

bool Widget::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::Resize) {
        if (mOutputIdentifiers.contains(qobject_cast<QQuickView *>(object))) {
            QResizeEvent *e = static_cast<QResizeEvent *>(event);
            const QRect screenSize = object->property("screenSize").toRect();
            QRect geometry(QPoint(0, 0), e->size());
            geometry.moveCenter(screenSize.center());
            static_cast<QQuickView *>(object)->setGeometry(geometry);
            // Pass the event further
        }
    }
    return QObject::eventFilter(object, event);
}

void Widget::setConfig(const KScreen::ConfigPtr &config, bool showBrightnessFrameFlag)
{
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
    mUnifyButton->setEnabled(mConfig->connectedOutputs().count() > 1);
    ui->unionframe->setVisible(mConfig->outputs().count() > 1);

    for (const KScreen::OutputPtr &output : mConfig->outputs()) {
        outputAdded(output);
    }

    // 择主屏幕输出
    QMLOutput *qmlOutput = mScreen->primaryOutput();

    if (qmlOutput) {
        mScreen->setActiveOutput(qmlOutput);
    } else {
        if (!mScreen->outputs().isEmpty()) {
            mScreen->setActiveOutput(mScreen->outputs().at(0));
            // 择一个主屏幕，避免闪退现象
            primaryButtonEnable(true);
        }
    }
    slotOutputEnabledChanged();

    if (mFirstLoad) {
        if (isCloneMode()) {
            mUnifyButton->blockSignals(true);
            mUnifyButton->setChecked(true);
            mUnifyButton->blockSignals(false);
            slotUnifyOutputs();
        }
    }
    mFirstLoad = false;

    if (showBrightnessFrameFlag == true) {
        showBrightnessFrame();   //初始化的时候，显示
    }
}

KScreen::ConfigPtr Widget::currentConfig() const
{
    return mConfig;
}

void Widget::loadQml()
{
    qmlRegisterType<QMLOutput>("org.kde.kscreen", 1, 0, "QMLOutput");
    qmlRegisterType<QMLScreen>("org.kde.kscreen", 1, 0, "QMLScreen");

    qmlRegisterType<KScreen::Output>("org.kde.kscreen", 1, 0, "KScreenOutput");
    qmlRegisterType<KScreen::Edid>("org.kde.kscreen", 1, 0, "KScreenEdid");
    qmlRegisterType<KScreen::Mode>("org.kde.kscreen", 1, 0, "KScreenMode");

    ui->quickWidget->setSource(QUrl("qrc:/qml/main.qml"));

    QQuickItem *rootObject = ui->quickWidget->rootObject();
    mScreen = rootObject->findChild<QMLScreen *>(QStringLiteral("outputView"));

    connect(mScreen, &QMLScreen::released, this, [=]() {
        delayApply();
    });

    if (!mScreen) {
        return;
    }
    connect(mScreen, &QMLScreen::focusedOutputChanged,
            this, &Widget::slotFocusedOutputChanged);
}

void Widget::resetPrimaryCombo()
{
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

void Widget::addOutputToPrimaryCombo(const KScreen::OutputPtr &output)
{
    // 注释后让他显示全部屏幕下拉框
    if (!output->isConnected()) {
        return;
    }

    ui->primaryCombo->addItem(Utils::outputName(output), output->id());
    if (output->isPrimary() && !mIsWayland) {
        Q_ASSERT(mConfig);
        int lastIndex = ui->primaryCombo->count() - 1;
        ui->primaryCombo->setCurrentIndex(lastIndex);
    }
}

// 这里从屏幕点击来读取输出
void Widget::slotFocusedOutputChanged(QMLOutput *output)
{
    mControlPanel->activateOutput(output->outputPtr());

    // 读取屏幕点击选择下拉框
    Q_ASSERT(mConfig);
    int index = output->outputPtr().isNull() ? 0 : ui->primaryCombo->findData(output->outputPtr()->id());
    if (index == -1 || index == ui->primaryCombo->currentIndex()) {
        return;
    }
    ui->primaryCombo->setCurrentIndex(index);
}

void Widget::slotFocusedOutputChangedNoParam()
{
    mControlPanel->activateOutput(res);
}

void Widget::slotOutputEnabledChanged()
{
    // 点击禁用屏幕输出后的改变
    resetPrimaryCombo();
    setActiveScreen(mKDSCfg);
    int enabledOutputsCount = 0;
    Q_FOREACH (const KScreen::OutputPtr &output, mConfig->outputs()) {
        if (output->isEnabled()) {
            ++enabledOutputsCount;
            for (int i = 0; i < BrightnessFrameV.size(); ++i) {
                if (BrightnessFrameV[i]->getOutputName() == Utils::outputName(output) && !BrightnessFrameV[i]->getSliderEnable()) {
                    BrightnessFrameV[i]->runConnectThread(true);
                }
            }
        }
        if (enabledOutputsCount > 1) {
            break;
        }
    }
    mUnifyButton->setEnabled(enabledOutputsCount > 1);
    ui->unionframe->setVisible(enabledOutputsCount > 1);
}

void Widget::slotOutputConnectedChanged()
{
    resetPrimaryCombo();
    for (auto &output: mConfig->outputs()) {
        bool frameExist = false;
        for (int i = 0; i < BrightnessFrameV.size(); ++i) {
            if (BrightnessFrameV[i]->getOutputName() == Utils::outputName(output)) {
                frameExist = true;
                if (!output->isConnected()) {
                    delete BrightnessFrameV[i];
                    BrightnessFrameV[i] = nullptr;
                    BrightnessFrameV.remove(i);
                }
            }
        }

        if (false == frameExist && output->isConnected()) {
            if (output->isConnected()) {
                QString name = Utils::outputName(output);
                addBrightnessFrame(name, output->isEnabled(), output->edid()->serial());
            }
        }
    }

}

void Widget::slotUnifyOutputs()
{
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

    // 取消统一输出
    if (base->isCloneMode() && !mUnifyButton->isChecked()) {

        if (mKDSCfg.isEmpty()) {
            KScreen::OutputList screens = mPrevConfig->connectedOutputs();
            QList<ScreenConfig> preScreenCfg = getPreScreenCfg();
            Q_FOREACH(ScreenConfig cfg, preScreenCfg) {
                Q_FOREACH(KScreen::OutputPtr output, screens) {
                    if (!cfg.screenId.compare(output->name())) {
                        output->setCurrentModeId(cfg.screenModeId);
                        output->setPos(QPoint(cfg.screenPosX, cfg.screenPosY));
                    }
                }
            }
        }
        setConfig(mPrevConfig);

        ui->primaryCombo->setEnabled(true);
        mCloseScreenButton->setEnabled(true);
        ui->showMonitorframe->setVisible(true);
        ui->primaryCombo->setEnabled(true);
    } else if (!base->isCloneMode() && mUnifyButton->isChecked()) {
        // Clone the current config, so that we can restore it in case user
        // breaks the cloning
        mPrevConfig = mConfig->clone();

        if (!mFirstLoad) {
            setPreScreenCfg(mPrevConfig->connectedOutputs());
        }

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

        // 关闭开关
        mCloseScreenButton->setEnabled(false);
        ui->showMonitorframe->setVisible(false);
        ui->primaryCombo->setEnabled(false);
        ui->mainScreenButton->setEnabled(false);
        mControlPanel->setUnifiedOutput(base->outputPtr());
    }
}

// FIXME: Copy-pasted from KDED's Serializer::findOutput()
KScreen::OutputPtr Widget::findOutput(const KScreen::ConfigPtr &config, const QVariantMap &info)
{
    KScreen::OutputList outputs = config->outputs();
    Q_FOREACH (const KScreen::OutputPtr &output, outputs) {
        if (!output->isConnected()) {
            continue;
        }

        const QString outputId
            = (output->edid()
               && output->edid()->isValid()) ? output->edid()->hash() : output->name();
        if (outputId != info[QStringLiteral("id")].toString()) {
            continue;
        }

        QVariantMap posInfo = info[QStringLiteral("pos")].toMap();
        QPoint point(posInfo[QStringLiteral("x")].toInt(), posInfo[QStringLiteral("y")].toInt());
        output->setPos(point);
        output->setPrimary(info[QStringLiteral("primary")].toBool());
        output->setEnabled(info[QStringLiteral("enabled")].toBool());
        output->setRotation(static_cast<KScreen::Output::Rotation>(info[QStringLiteral("rotation")].
                                                                   toInt()));

        QVariantMap modeInfo = info[QStringLiteral("mode")].toMap();
        QVariantMap modeSize = modeInfo[QStringLiteral("size")].toMap();
        QSize size(modeSize[QStringLiteral("width")].toInt(),
                   modeSize[QStringLiteral("height")].toInt());

        const KScreen::ModeList modes = output->modes();
        Q_FOREACH (const KScreen::ModePtr &mode, modes) {
            if (mode->size() != size) {
                continue;
            }
            if (QString::number(mode->refreshRate())
                != modeInfo[QStringLiteral("refresh")].toString()) {
                continue;
            }

            output->setCurrentModeId(mode->id());
            break;
        }
        return output;
    }

    return KScreen::OutputPtr();
}

void Widget::setHideModuleInfo()
{
    mCPU = getCpuInfo();
    if (!mCPU.startsWith(kCpu, Qt::CaseInsensitive)) {
        ui->quickWidget->setAttribute(Qt::WA_AlwaysStackOnTop);
        ui->quickWidget->setClearColor(Qt::transparent);
    }
}

void Widget::setTitleLabel()
{
    //~ contents_path /display/monitor
    ui->primaryLabel->setText(tr("monitor"));
}

void Widget::writeScale(double scale)
{
    if (scale != scaleGSettings->get(SCALE_KEY).toDouble()) {
        mIsScaleChanged = true;
    }

    if (mIsScaleChanged) {
        QMessageBox::information(this, tr("Information"),
                                 tr("Some applications need to be logouted to take effect"));
    } else {
        return;
    }

    mIsScaleChanged = false;
    int cursize;
    QByteArray iid(MOUSE_SIZE_SCHEMAS);
    if (QGSettings::isSchemaInstalled(MOUSE_SIZE_SCHEMAS)) {
        QGSettings cursorSettings(iid);

        if (1.0 == scale) {
            cursize = 24;
        } else if (2.0 == scale) {
            cursize = 48;
        } else if (3.0 == scale) {
            cursize = 96;
        } else {
            cursize = 24;
        }

        QStringList keys = scaleGSettings->keys();
        if (keys.contains("scalingFactor")) {

            scaleGSettings->set(SCALE_KEY, scale);
        }
        cursorSettings.set(CURSOR_SIZE_KEY, cursize);
        Utils::setKwinMouseSize(cursize);
    }
}

void Widget::initGSettings()
{
    QByteArray id(UKUI_CONTORLCENTER_PANEL_SCHEMAS);
    if (QGSettings::isSchemaInstalled(id)) {
        mGsettings = new QGSettings(id, QByteArray(), this);
        if (mGsettings->keys().contains(THEME_NIGHT_KEY)) {
            mThemeButton->setChecked(mGsettings->get(THEME_NIGHT_KEY).toBool());
        }
    } else {
        qDebug() << Q_FUNC_INFO << "org.ukui.control-center.panel.plugins not install";
        return;
    }

//    QByteArray powerId(POWER_SCHMES);
//    if (QGSettings::isSchemaInstalled(powerId)) {
//        mPowerGSettings = new QGSettings(powerId, QByteArray(), this);
//        mPowerKeys = mPowerGSettings->keys();
//        connect(mPowerGSettings, &QGSettings::changed, this, [=](QString key) {
//            if ("brightnessAc" == key || "brightnessBat" == key) {
//                int value = mPowerGSettings->get(key).toInt();
//                if (mIsWayland && !mIsBattery) {
//                    value = (value == 0 ? 0 : value / 10);
//                }

//                for (int i = 0; i < BrightnessFrameV.size(); ++i) {
//                    if (BrightnessFrameV[i]->getOutputName() == "eDP") {
//                       BrightnessFrameV[i]->slider->blockSignals(true);
//                       BrightnessFrameV[i]->setTextLabelValue(QString::number(value));
//                       BrightnessFrameV[i]->slider->setValue(value);
//                       BrightnessFrameV[i]->slider->blockSignals(false);
//                    }
//                }
//            }
//        });
//    }

    QByteArray scaleId(FONT_RENDERING_DPI);
    if (QGSettings::isSchemaInstalled(scaleId)) {
        scaleGSettings = new QGSettings(scaleId, QByteArray(), this);
    }
}

void Widget::setcomBoxScale()
{
    int scale = 1;
    QComboBox *scaleCombox = findChild<QComboBox *>(QString("scaleCombox"));
    if (scaleCombox) {
        scale = ("100%" == scaleCombox->currentText() ? 1 : 2);
    }
    writeScale(scale);
}

void Widget::initNightUI()
{
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

bool Widget::isRestoreConfig()
{
    int cnt = 15;
    int ret = -100;
    MainWindow *mainWindow = static_cast<MainWindow*>(this->topLevelWidget());
    QMessageBox msg;
    connect(mainWindow, &MainWindow::posChanged, this, [=,&msg]() {
        QTimer::singleShot(8, this, [=,&msg]() { //窗管会移动窗口，等待8ms,确保在窗管移动之后再move，时间不能太长，否则会看到移动的路径
            QRect rect = this->topLevelWidget()->geometry();
            int msgX = 0, msgY = 0;
            msgX = rect.x() + rect.width()/2 - 380/2;
            msgY = rect.y() + rect.height()/2 - 130/2;
            msg.move(msgX, msgY);
        });
    });
    if (mConfigChanged) {
        msg.setWindowTitle(tr("Hint"));
        msg.setText(tr("After modifying the resolution or refresh rate, "
                       "due to compatibility issues between the display device and the graphics card, "
                       "the display may be abnormal or unable to display\n"
                       "the settings will be saved after 14 seconds"));
        msg.addButton(tr("Save Config"), QMessageBox::RejectRole);
        msg.addButton(tr("Restore Config"), QMessageBox::AcceptRole);

        QTimer cntDown;
        QObject::connect(&cntDown, &QTimer::timeout, [&msg, &cnt, &cntDown, &ret]()->void {
            if (--cnt < 0) {
                cntDown.stop();
                msg.close();
            } else {
                msg.setText(QString(tr("After modifying the resolution or refresh rate, "
                                       "due to compatibility issues between the display device and the graphics card, "
                                       "the display may be abnormal or unable to display \n"
                                       "the settings will be saved after %1 seconds")).arg(cnt));
            }
        });
        cntDown.start(1000);
        QRect rect = this->topLevelWidget()->geometry();
        int msgX = 0, msgY = 0;
        msgX = rect.x() + rect.width()/2 - 380/2;
        msgY = rect.y() + rect.height()/2 - 130/2;
        msg.move(msgX, msgY);
        ret = msg.exec();
    }
    disconnect(mainWindow, &MainWindow::posChanged, 0, 0);
    bool res = false;
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

QString Widget::getCpuInfo()
{
    QDBusInterface youkerInterface("com.kylin.assistant.systemdaemon",
                                   "/com/kylin/assistant/systemdaemon",
                                   "com.kylin.assistant.systemdaemon",
                                   QDBusConnection::systemBus());
    if (!youkerInterface.isValid()) {
        qCritical() << "Create youker Interface Failed When Get Computer info: " <<
            QDBusConnection::systemBus().lastError();
        return QString();
    }

    QDBusReply<QMap<QString, QVariant> > cpuinfo;
    QString cpuType;
    cpuinfo = youkerInterface.call("get_cpu_info");
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
    KScreen::OutputPtr output = mConfig->primaryOutput();
    if (mConfig->connectedOutputs().count() >= 2) {
        foreach (KScreen::OutputPtr secOutput, mConfig->connectedOutputs()) {
            if (secOutput->geometry() != output->geometry() || !secOutput->isEnabled()) {
                return false;
            }
        }
    } else {
        return false;
    }
    return true;
}

bool Widget::isBacklight()
{
    QString cmd = "ukui-power-backlight-helper --get-max-brightness";
    QProcess process;
    process.start(cmd);
    process.waitForFinished();
    QString result = process.readAllStandardOutput().trimmed();

    QString pattern("^[0-9]*$");
    QRegExp reg(pattern);

    return reg.exactMatch(result);
}

QString Widget::getMonitorType()
{
    QString monitor = ui->primaryCombo->currentText();
    QString type;
    if (monitor.contains("VGA", Qt::CaseInsensitive)) {
        type = "4";
    } else {
        type = "8";
    }
    return type;
}

bool Widget::isLaptopScreen()
{
    int index = ui->primaryCombo->currentIndex();
    KScreen::OutputPtr output = mConfig->output(ui->primaryCombo->itemData(index).toInt());
    if (output->type() == KScreen::Output::Type::Panel) {
        return true;
    }
    return false;
}

bool Widget::isVisibleBrightness()
{
    if ((mIsBattery && isLaptopScreen())
            || (mIsWayland && !mIsBattery)
            || (!mIsWayland && mIsBattery)) {
        return true;
    }
    return false;
}


int Widget::getPrimaryScreenID()
{
    QString primaryScreen = getPrimaryWaylandScreen();
    int screenId;
    for (const KScreen::OutputPtr &output : mConfig->outputs()) {
        if (!output->name().compare(primaryScreen, Qt::CaseInsensitive)) {
            screenId = output->id();
        }
    }
    return screenId;
}

void Widget::showNightWidget(bool judge)
{
    if (judge) {
        ui->sunframe->setVisible(true);
        ui->customframe->setVisible(true);
        ui->temptframe->setVisible(true);
        ui->themeFrame->setVisible(false);
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

void Widget::showCustomWiget(int index)
{
    if (SUN == index) {
        ui->opframe->setVisible(false);
        ui->clsframe->setVisible(false);
    } else if (CUSTOM == index) {
        ui->opframe->setVisible(true);
        ui->clsframe->setVisible(true);
    }
}

void Widget::slotThemeChanged(bool judge)
{
    if (mGsettings->keys().contains(THEME_NIGHT_KEY)) {
        mGsettings->set(THEME_NIGHT_KEY, judge);
    }
}

void Widget::clearOutputIdentifiers()
{
    mOutputTimer->stop();
    qDeleteAll(mOutputIdentifiers);
    mOutputIdentifiers.clear();
}

void Widget::addBrightnessFrame(QString name, bool openFlag, QString serialNum)
{
    if (mIsBattery && name != "eDP")  //笔记本非内置
        return;
    for (int i = 0; i < BrightnessFrameV.size(); ++i) {  //已经有了
        if (name == BrightnessFrameV[i]->getOutputName())
            return;
    }

    BrightnessFrame *frame = nullptr;
    if (mIsBattery && name == "eDP") {
        frame = new BrightnessFrame(name, true, serialNum);
    } else if(!mIsBattery) {
        frame = new BrightnessFrame(name, false, serialNum);
    }
    BrightnessFrameV.push_back(frame);

    ui->unifyBrightLayout->addWidget(frame);
    frame->runConnectThread(openFlag);

}

void Widget::outputAdded(const KScreen::OutputPtr &output)
{
    if (output->isConnected()) {
        QString name = Utils::outputName(output);
        addBrightnessFrame(name, output->isEnabled(), output->edid()->serial());
    }

    connect(output.data(), &KScreen::Output::isConnectedChanged,
            this, &Widget::slotOutputConnectedChanged);
    connect(output.data(), &KScreen::Output::isEnabledChanged,
            this, &Widget::slotOutputEnabledChanged);

    addOutputToPrimaryCombo(output);

    // 检查统一输出-防止多显示屏幕
    if (mUnifyButton->isChecked()) {
        for (QMLOutput *qmlOutput: mScreen->outputs()) {
            if (!qmlOutput->output()->isConnected()) {
                continue;
            }
            if (!qmlOutput->isCloneMode()) {
                qmlOutput->blockSignals(true);
                qmlOutput->setVisible(false);
                qmlOutput->blockSignals(false);
            }
        }
    }

    ui->unionframe->setVisible(mConfig->connectedOutputs().count() > 1);
    mUnifyButton->setEnabled(mConfig->connectedOutputs().count() > 1);

    if (!mFirstLoad) {
        QTimer::singleShot(2000, this, [=] {
            mainScreenButtonSelect(ui->primaryCombo->currentIndex());
        });
    }
    showBrightnessFrame();
}

void Widget::outputRemoved(int outputId)
{
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

    QString name = ui->primaryCombo->itemText(index);
    for (int i = 0; i < BrightnessFrameV.size(); ++i) {
        if (BrightnessFrameV[i]->getOutputName() == name) {
            delete BrightnessFrameV[i];
            BrightnessFrameV[i] = nullptr;
            BrightnessFrameV.remove(i);
        }
    }
    ui->primaryCombo->removeItem(index);

    // 检查统一输出-防止移除后没有屏幕可显示
    if (mUnifyButton->isChecked()) {
        for (QMLOutput *qmlOutput: mScreen->outputs()) {
            if (!qmlOutput->output()->isConnected()) {
                continue;
            }
            qmlOutput->setIsCloneMode(false);
            qmlOutput->blockSignals(true);
            qmlOutput->setVisible(true);
            qmlOutput->blockSignals(false);
        }
    }
    ui->unionframe->setVisible(mConfig->connectedOutputs().count() > 1);
    mUnifyButton->blockSignals(true);
    mUnifyButton->setChecked(mConfig->connectedOutputs().count() > 1);
    mUnifyButton->blockSignals(false);
    mainScreenButtonSelect(ui->primaryCombo->currentIndex());
	showBrightnessFrame();
}

void Widget::primaryOutputSelected(int index)
{
    if (!mConfig) {
        return;
    }

    const KScreen::OutputPtr newPrimary = index == 0 ? KScreen::OutputPtr() : mConfig->output(ui->primaryCombo->itemData(index).toInt());
    if (newPrimary == mConfig->primaryOutput()) {
        return;
    }

    mConfig->setPrimaryOutput(newPrimary);
}

// 主输出
void Widget::primaryOutputChanged(const KScreen::OutputPtr &output)
{
    Q_ASSERT(mConfig);
    int index = output.isNull() ? 0 : ui->primaryCombo->findData(output->id());
    if (index == -1 || index == ui->primaryCombo->currentIndex()) {
        return;
    }
    ui->primaryCombo->setCurrentIndex(index);
}

void Widget::slotIdentifyButtonClicked(bool checked)
{
    Q_UNUSED(checked);
    connect(new KScreen::GetConfigOperation(), &KScreen::GetConfigOperation::finished,
            this, &Widget::slotIdentifyOutputs);
}

void Widget::slotIdentifyOutputs(KScreen::ConfigOperation *op)
{
    if (op->hasError()) {
        return;
    }

    const KScreen::ConfigPtr config = qobject_cast<KScreen::GetConfigOperation *>(op)->config();

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

void Widget::callMethod(QRect geometry, QString name)
{
    auto scale = 1;
    QDBusInterface waylandIfc("org.ukui.SettingsDaemon",
                              "/org/ukui/SettingsDaemon/wayland",
                              "org.ukui.SettingsDaemon.wayland",
                              QDBusConnection::sessionBus());

    QDBusReply<int> reply = waylandIfc.call("scale");
    if (reply.isValid()) {
        scale = reply.value();
    }

    QDBusMessage message = QDBusMessage::createMethodCall("org.ukui.SettingsDaemon",
                                                          "/org/ukui/SettingsDaemon/wayland",
                                                          "org.ukui.SettingsDaemon.wayland",
                                                          "priScreenChanged");
    message << geometry.x() / scale << geometry.y() / scale << geometry.width() / scale <<
        geometry.height() / scale << name;
    QDBusConnection::sessionBus().send(message);
}

QString Widget::getPrimaryWaylandScreen()
{
    QDBusInterface screenIfc("org.ukui.SettingsDaemon",
                             "/org/ukui/SettingsDaemon/wayland",
                             "org.ukui.SettingsDaemon.wayland",
                             QDBusConnection::sessionBus());
    QDBusReply<QString> screenReply = screenIfc.call("priScreenName");
    if (screenReply.isValid()) {
        return screenReply.value();
    }
    return QString();
}

void Widget::isWayland()
{
    QString sessionType = getenv("XDG_SESSION_TYPE");

    if (!sessionType.compare(kSession, Qt::CaseSensitive)) {
        mIsWayland = true;
    } else {
        mIsWayland = false;
    }
}

void Widget::applyNightModeSlot()
{
    if (((ui->opHourCom->currentIndex() < ui->clHourCom->currentIndex())
         || (ui->opHourCom->currentIndex() == ui->clHourCom->currentIndex()
             && ui->opMinCom->currentIndex() <= ui->clMinCom->currentIndex()))
            && CUSTOM == singleButton->checkedId() && mNightButton->isChecked()) {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Open time should be earlier than close time!"));
        return;
    }
    setNightMode(mNightButton->isChecked());
}


void Widget::setScreenKDS(QString kdsConfig)
{
    KScreen::OutputList screens = mConfig->connectedOutputs();
    if (kdsConfig == "expand") {
        Q_FOREACH(KScreen::OutputPtr output, screens) {
            if (!output.isNull() && !mUnifyButton->isChecked()) {
                output->setEnabled(true);
                output->setCurrentModeId("0");
            }
        }

        KScreen::OutputList screensPre = mPrevConfig->connectedOutputs();

        KScreen::OutputPtr mainScreen = mPrevConfig->primaryOutput();
        mainScreen->setPos(QPoint(0, 0));

        KScreen::OutputPtr preIt = mainScreen;
        QMap<int, KScreen::OutputPtr>::iterator nowIt = screensPre.begin();

        while (nowIt != screensPre.end()) {
            if (nowIt.value() != mainScreen) {
                nowIt.value()->setPos(QPoint(preIt->pos().x() + preIt->size().width(), 0));
                KScreen::ModeList modes = preIt->modes();
                Q_FOREACH (const KScreen::ModePtr &mode, modes) {
                    if (preIt->currentModeId() == mode->id()) {
                        if (preIt->rotation() != KScreen::Output::Rotation::Left && preIt->rotation() != KScreen::Output::Rotation::Right) {
                            nowIt.value()->setPos(QPoint(preIt->pos().x() + mode->size().width(), 0));
                        } else {
                            nowIt.value()->setPos(QPoint(preIt->pos().x() + mode->size().height(), 0));
                        }
                    }
                }
                preIt = nowIt.value();
            }
            nowIt++;
        }
    } else if (kdsConfig == "first") {
        for (int i = 0; i < screens.size(); i++) {
            if (!screens[i].isNull()) {
                screens[i]->setEnabled((i == 0));
            }
        }
    } else if (kdsConfig == "second") {
        for (int i = 0; i < screens.size(); i++) {
            if (!screens[i].isNull()) {
                screens[i]->setEnabled((i != 0));
            }
        }
    } else {
        Q_FOREACH(KScreen::OutputPtr output, screens) {
            if (!output.isNull()) {
                output->setEnabled(true);
            }
        }
    }
}

void Widget::setActiveScreen(QString status)
{
    int activeScreenId = 1;
    int enableCount = 0;
    int connectCount = 0;
    Q_FOREACH(const KScreen::OutputPtr &output, mConfig->connectedOutputs()) {
        connectCount++;
        enableCount = (output->isEnabled() ? (++enableCount) : enableCount);
    }

    if (status == "second") {
        activeScreenId = connectCount;
    }

    for (int index = 0; index <= ui->primaryCombo->count(); index++) {
        KScreen::OutputPtr output = mConfig->output(ui->primaryCombo->itemData(index).toInt());
        if (status.isEmpty() && connectCount > enableCount && !output.isNull() && output->isEnabled()) {
            ui->primaryCombo->setCurrentIndex(index);
        }

        if (!status.isEmpty() && !output.isNull() && activeScreenId == output->id()) {
            ui->primaryCombo->setCurrentIndex(index);
        }
    }

}

// 等相关包上传
//通过win+p修改，不存在按钮影响亮度显示的情况，直接就应用了，此时每个屏幕的openFlag是没有修改的，需要单独处理(setScreenKDS)
void Widget::kdsScreenchangeSlot(QString status)
{
    bool isCheck = (status == "copy") ? true : false;
    mKDSCfg = status;
    setScreenKDS(mKDSCfg);
    if (mConfig->connectedOutputs().count() >= 2) {
        mUnifyButton->setChecked(isCheck);
    }
}

void Widget::delayApply()
{
    QTimer::singleShot(200, this, [=]() {
        if (mKDSCfg.isEmpty()) {
            save();
        }
        mKDSCfg.clear();
    });
}

void Widget::save()
{
    if (!this) {
        return;
    }

    const KScreen::ConfigPtr &config = this->currentConfig();

    bool atLeastOneEnabledOutput = false;
    Q_FOREACH (const KScreen::OutputPtr &output, config->outputs()) {
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

    if (!atLeastOneEnabledOutput) {
        QMessageBox::warning(this, tr("Warning"), tr("please insure at least one output!"));
        mCloseScreenButton->setChecked(true);
        return;
    }

    if (!KScreen::Config::canBeApplied(config)) {
        QMessageBox::information(this,
                                 tr("Warning"),
                                 tr("Sorry, your configuration could not be applied.\nCommon reasons are that the overall screen size is too big, or you enabled more displays than supported by your GPU."));
        return;
    }
    mBlockChanges = true;
    /* Store the current config, apply settings */
    auto *op = new KScreen::SetConfigOperation(config);

    /* Block until the operation is completed, otherwise KCMShell will terminate
     * before we get to execute the Operation */
    op->exec();

    // The 1000ms is a bit "random" here, it's what works on the systems I've tested, but ultimately, this is a hack
    // due to the fact that we just can't be sure when xrandr is done changing things, 1000 doesn't seem to get in the way
    QTimer::singleShot(1000, this,
                       [=]() {
        if (mIsWayland) {
            QString hash = config->connectedOutputsHash();
            writeFile(mDir % hash);
        }
        mBlockChanges = false;
        mConfigChanged = false;
    });

    int enableScreenCount = 0;
    KScreen::OutputPtr enableOutput;
    for (const KScreen::OutputPtr &output : mConfig->outputs()) {
        if (output->isEnabled()) {
            enableOutput = output;
            enableScreenCount++;
        }
    }

    if (isRestoreConfig()) {
        auto *op = new KScreen::SetConfigOperation(mPrevConfig);
        op->exec();

    } else {
        mPrevConfig = mConfig->clone();
        writeScreenXml();
    }

    setActiveScreen();

	for (int i = 0; i < BrightnessFrameV.size(); ++i) {   //应用成功再更新屏幕是否开启的状态，判断亮度条是否打开
        for (KScreen::OutputPtr output : mConfig->outputs()) {
            if (BrightnessFrameV[i]->getOutputName() == Utils::outputName(output)) {
                BrightnessFrameV[i]->setOutputEnable(output->isEnabled());
            }
        }
    }
    int flag = mUnifyButton->isChecked() ? 1 : 2;
    showBrightnessFrame(flag);  //成功应用之后，重新显示亮度条,传入是否统一输出,1表示打开，2表示关闭
	
}

QVariantMap metadata(const KScreen::OutputPtr &output)
{
    QVariantMap metadata;
    metadata[QStringLiteral("name")] = output->name();
    if (!output->edid() || !output->edid()->isValid()) {
        return metadata;
    }

    metadata[QStringLiteral("fullname")] = output->edid()->deviceId();
    return metadata;
}

QString Widget::globalFileName(const QString &hash)
{
    QString s_dirPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)
                        %QStringLiteral("/kscreen/");
    QString dir = s_dirPath  % QStringLiteral("outputs/");
    if (!QDir().mkpath(dir)) {
        return QString();
    }
    return QString();
}

QVariantMap Widget::getGlobalData(KScreen::OutputPtr output)
{
    QFile file(globalFileName(output->hashMd5()));
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file" << file.fileName();
        return QVariantMap();
    }
    QJsonDocument parser;
    return parser.fromJson(file.readAll()).toVariant().toMap();
}

void Widget::writeGlobal(const KScreen::OutputPtr &output)
{
    // get old values and subsequently override
    QVariantMap info = getGlobalData(output);
    if (!writeGlobalPart(output, info, nullptr)) {
        return;
    }
    QFile file(globalFileName(output->hashMd5()));
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open global output file for writing! " << file.errorString();
        return;
    }

    file.write(QJsonDocument::fromVariant(info).toJson());
    return;
}

bool Widget::writeGlobalPart(const KScreen::OutputPtr &output, QVariantMap &info,
                             const KScreen::OutputPtr &fallback)
{
    info[QStringLiteral("id")] = output->hash();
    info[QStringLiteral("metadata")] = metadata(output);
    info[QStringLiteral("rotation")] = output->rotation();

    // Round scale to four digits
    info[QStringLiteral("scale")] = int(output->scale() * 10000 + 0.5) / 10000.;

    QVariantMap modeInfo;
    float refreshRate = -1.;
    QSize modeSize;
    if (output->currentMode() && output->isEnabled()) {
        refreshRate = output->currentMode()->refreshRate();
        modeSize = output->currentMode()->size();
    } else if (fallback && fallback->currentMode()) {
        refreshRate = fallback->currentMode()->refreshRate();
        modeSize = fallback->currentMode()->size();
    }

    if (refreshRate < 0 || !modeSize.isValid()) {
        return false;
    }

    modeInfo[QStringLiteral("refresh")] = refreshRate;

    QVariantMap modeSizeMap;
    modeSizeMap[QStringLiteral("width")] = modeSize.width();
    modeSizeMap[QStringLiteral("height")] = modeSize.height();
    modeInfo[QStringLiteral("size")] = modeSizeMap;

    info[QStringLiteral("mode")] = modeInfo;

    return true;
}

bool Widget::writeFile(const QString &filePath)
{
    const KScreen::OutputList outputs = mConfig->outputs();
    const auto oldConfig = mPrevConfig;
    KScreen::OutputList oldOutputs;
    if (oldConfig) {
        oldOutputs = oldConfig->outputs();
    }
    QVariantList outputList;
    for (const KScreen::OutputPtr &output : outputs) {
        QVariantMap info;
        const auto oldOutputIt = std::find_if(oldOutputs.constBegin(), oldOutputs.constEnd(),
                                              [output](const KScreen::OutputPtr &out) {
            return out->hashMd5() == output->hashMd5();
        });
        const KScreen::OutputPtr oldOutput = oldOutputIt != oldOutputs.constEnd() ? *oldOutputIt
                                             : nullptr;
        if (!output->isConnected()) {
            continue;
        }

        writeGlobalPart(output, info, oldOutput);
        info[QStringLiteral("primary")] = !output->name().compare(
            getPrimaryWaylandScreen(), Qt::CaseInsensitive);
        info[QStringLiteral("enabled")] = output->isEnabled();

        auto setOutputConfigInfo = [&info](const KScreen::OutputPtr &out) {
                                       if (!out) {
                                           return;
                                       }

                                       QVariantMap pos;
                                       pos[QStringLiteral("x")] = out->pos().x();
                                       pos[QStringLiteral("y")] = out->pos().y();
                                       info[QStringLiteral("pos")] = pos;
                                   };
        setOutputConfigInfo(output->isEnabled() ? output : oldOutput);

        if (output->isEnabled()) {
            // try to update global output data
            writeGlobal(output);
        }
        outputList.append(info);
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open config file for writing! " << file.errorString();
        return false;
    }
    file.write(QJsonDocument::fromVariant(outputList).toJson());
    qDebug() << "Config saved on: " << file.fileName();

    return true;
}

void Widget::scaleChangedSlot(double scale)
{
    this->mScreenScale = scale;
    if (scaleGSettings->get(SCALE_KEY).toDouble() != this->mScreenScale) {
        mIsScaleChanged = true;
    } else {
        mIsScaleChanged = false;
    }

    writeScale(this->mScreenScale);
}

void Widget::changedSlot()
{
    mConfigChanged = true;
}

void Widget::propertiesChangedSlot(QString property, QMap<QString, QVariant> propertyMap,
                                   QStringList propertyList)
{
    Q_UNUSED(property);
    Q_UNUSED(propertyList);
    if (propertyMap.keys().contains("OnBattery")) {
        mOnBattery = propertyMap.value("OnBattery").toBool();
    }
}

// 是否禁用主屏按钮
void Widget::mainScreenButtonSelect(int index)
{
    if (!mConfig || ui->primaryCombo->count() <= 0) {
        return;
    }

    const KScreen::OutputPtr newPrimary = mConfig->output(ui->primaryCombo->itemData(index).toInt());
    int connectCount = mConfig->connectedOutputs().count();

    if (mIsWayland) {
        if (!getPrimaryWaylandScreen().compare(newPrimary->name(), Qt::CaseInsensitive)) {
            ui->mainScreenButton->setEnabled(false);
        } else {
            ui->mainScreenButton->setEnabled(true);
        }
    } else {
        if (newPrimary == mConfig->primaryOutput()) {
            ui->mainScreenButton->setEnabled(false);
        } else {
            ui->mainScreenButton->setEnabled(true);
        }
    }

    // 设置是否勾选
    mCloseScreenButton->setEnabled(true);
    ui->showMonitorframe->setVisible(connectCount > 1 && !mUnifyButton->isChecked());

    // 初始化时不要发射信号
    mCloseScreenButton->blockSignals(true);
    mCloseScreenButton->setChecked(newPrimary->isEnabled());
    mCloseScreenButton->blockSignals(false);

    mControlPanel->activateOutput(newPrimary);

    mScreen->setActiveOutputByCombox(newPrimary->id());
}

// 设置主屏按钮
void Widget::primaryButtonEnable(bool status)
{
    Q_UNUSED(status);
    if (!mConfig) {
        return;
    }
    int index = ui->primaryCombo->currentIndex();
    ui->mainScreenButton->setEnabled(false);
    const KScreen::OutputPtr newPrimary = mConfig->output(ui->primaryCombo->itemData(index).toInt());
    mConfig->setPrimaryOutput(newPrimary);
}

void Widget::checkOutputScreen(bool judge)
{
    ui->primaryCombo->blockSignals(true);
    int index = ui->primaryCombo->currentIndex();
    KScreen::OutputPtr newPrimary = mConfig->output(ui->primaryCombo->itemData(index).toInt());

    KScreen::OutputPtr mainScreen = mConfig->primaryOutput();

    if (!mainScreen) {
        mConfig->setPrimaryOutput(newPrimary);
    }
    mainScreen = mConfig->primaryOutput();

    newPrimary->setEnabled(judge);

    int enabledOutput = 0;
    Q_FOREACH (KScreen::OutputPtr outptr, mConfig->outputs()) {
        if (outptr->isEnabled()) {
            enabledOutput++;
        }
        if (mainScreen != outptr && outptr->isConnected()) {
            newPrimary = outptr;
        }

        if (enabledOutput >= 2) {
            // 设置副屏在主屏右边
            newPrimary->setPos(QPoint(mainScreen->pos().x() + mainScreen->geometry().width(),
                                      mainScreen->pos().y()));
        }
    }

    ui->primaryCombo->setCurrentIndex(index);
	ui->primaryCombo->blockSignals(false);
}


void Widget::initConnection()
{
    connect(mThemeButton, SIGNAL(checkedChanged(bool)), this, SLOT(slotThemeChanged(bool)));
    connect(ui->primaryCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &Widget::mainScreenButtonSelect);

    connect(ui->mainScreenButton, &QPushButton::clicked, this, [=](bool status){
        primaryButtonEnable(status);
        delayApply();
    });

    mControlPanel = new ControlPanel(this);
    connect(mControlPanel, &ControlPanel::changed, this, &Widget::changed);
    connect(this, &Widget::changed, this, [=]() {
            changedSlot();
            delayApply();
    });
    connect(mControlPanel, &ControlPanel::scaleChanged, this, &Widget::scaleChangedSlot);

    ui->controlPanelLayout->addWidget(mControlPanel);

    connect(ui->advancedBtn, &QPushButton::clicked, this, [=] {
        DisplayPerformanceDialog *dialog = new DisplayPerformanceDialog(this);
        dialog->exec();
    });

    connect(mUnifyButton, &SwitchButton::checkedChanged,
            [this] {
        slotUnifyOutputs();
        delayApply();
		showBrightnessFrame();
    });

    connect(mCloseScreenButton, &SwitchButton::checkedChanged,
            this, [=](bool checked) {
        checkOutputScreen(checked);
        delayApply();
    });

    connect(mNightButton, &SwitchButton::checkedChanged, this, [=](bool status){
        showNightWidget(status);
        applyNightModeSlot();
    });

    connect(ui->opHourCom, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=]{
        applyNightModeSlot();;
    });

    connect(ui->opMinCom, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=]{
        applyNightModeSlot();
    });

    connect(ui->clHourCom, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=]{
        applyNightModeSlot();;
    });

    connect(ui->clMinCom, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=]{
        applyNightModeSlot();
    });

    connect(ui->temptSlider, &QSlider::valueChanged, this, [=]{
        applyNightModeSlot();
    });

    connect(singleButton, QOverload<int>::of(&QButtonGroup::buttonClicked), this, [=](int index){
        showCustomWiget(index);
        applyNightModeSlot();
    });

    QDBusConnection::sessionBus().connect(QString(),
                                              QString("/"),
                                              "org.ukui.ukcc.session.interface",
                                              "screenChanged",
                                              this,
                                              SLOT(kdsScreenchangeSlot(QString)));


    QDBusConnection::sessionBus().connect(QString(),
                                          QString("/ColorCorrect"),
                                          "org.ukui.kwin.ColorCorrect",
                                          "nightColorConfigChanged",
                                          this,
                                          SLOT(nightChangedSlot(QHash<QString,QVariant>)));

    mOutputTimer = new QTimer(this);
    connect(mOutputTimer, &QTimer::timeout,
            this, &Widget::clearOutputIdentifiers);

    mApplyShortcut = new QShortcut(QKeySequence("Ctrl+A"), this);
    connect(mApplyShortcut, SIGNAL(activated()), this, SLOT(save()));

    connect(ui->primaryCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),////////////////
            this, [=](int index) {
       // mainScreenButtonSelect(index);
        showBrightnessFrame();  //当前屏幕框变化的时候，显示，此时不判断
    });
}


void Widget::initTemptSlider()
{
    ui->temptSlider->setRange(1.1*1000, 6500);
    ui->temptSlider->setTracking(true);

    for (int i = 0; i < 24; i++) {
        ui->opHourCom->addItem(QStringLiteral("%1").arg(i, 2, 10, QLatin1Char('0')));
        ui->clHourCom->addItem(QStringLiteral("%1").arg(i, 2, 10, QLatin1Char('0')));
    }

    for (int i = 0; i < 60; i++) {
        ui->opMinCom->addItem(QStringLiteral("%1").arg(i, 2, 10, QLatin1Char('0')));
        ui->clMinCom->addItem(QStringLiteral("%1").arg(i, 2, 10, QLatin1Char('0')));
    }
}

void Widget::writeScreenXml()
{
    MateRRScreen *rr_screen;
    MateRRConfig *rr_config;

    /* Normally, mate_rr_config_save() creates a backup file based on the
     * old monitors.xml.  However, if *that* file didn't exist, there is
     * nothing from which to create a backup.  So, here we'll save the
     * current/unchanged configuration and then let our caller call
     * mate_rr_config_save() again with the new/changed configuration, so
     * that there *will* be a backup file in the end.
     */

    rr_screen = mate_rr_screen_new(gdk_screen_get_default(), NULL);   /* NULL-GError */
    if (!rr_screen)
        return;

    rr_config = mate_rr_config_new_current(rr_screen, NULL);
    mate_rr_config_save(rr_config, NULL);  /* NULL-GError */

    char *backup_filename = mate_rr_config_get_backup_filename();
    unlink(backup_filename);

    g_object_unref(rr_config);
    g_object_unref(rr_screen);
}

void Widget::setNightMode(const bool nightMode)
{
    QDBusInterface colorIft("org.ukui.KWin",
                            "/ColorCorrect",
                            "org.ukui.kwin.ColorCorrect",
                            QDBusConnection::sessionBus());
    if (!colorIft.isValid()) {
        qWarning() << "create org.ukui.kwin.ColorCorrect failed";
        return;
    }

    if (!nightMode) {
        mNightConfig["Active"] = false;
    } else {
        mNightConfig["Active"] = true;
        if (ui->sunradioBtn->isChecked()) {
            mNightConfig["EveningBeginFixed"] = "17:55:00";
            mNightConfig["MorningBeginFixed"] = "05:55:04";
            mNightConfig["Mode"] = 0;
        } else if (ui->customradioBtn->isChecked()) {
            mNightConfig["EveningBeginFixed"] = ui->opHourCom->currentText() + ":"
                                                + ui->opMinCom->currentText() + ":00";
            mNightConfig["MorningBeginFixed"] = ui->clHourCom->currentText() + ":"
                                                + ui->clMinCom->currentText() + ":00";
            mNightConfig["Mode"] = 2;
        }
        mNightConfig["NightTemperature"] = ui->temptSlider->value();
    }

    colorIft.call("setNightColorConfig", mNightConfig);
}

void Widget::initUiComponent()
{
    mDir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)
            %QStringLiteral("/kscreen/")
            %QStringLiteral("" /*"configs/"*/);

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

    mIsBattery = isBacklight();

    mUPowerInterface = QSharedPointer<QDBusInterface>(
        new QDBusInterface("org.freedesktop.UPower",
                           "/org/freedesktop/UPower",
                           "org.freedesktop.DBus.Properties",
                           QDBusConnection::systemBus()));

    if (!mUPowerInterface.get()->isValid()) {
        qDebug() << "Create UPower Battery Interface Failed : " <<
            QDBusConnection::systemBus().lastError();
        return;
    }

    QDBusReply<QVariant> batteryInfo;
    batteryInfo = mUPowerInterface.get()->call("Get", "org.freedesktop.UPower", "OnBattery");
    if (batteryInfo.isValid()) {
        mOnBattery = batteryInfo.value().toBool();
    }

    mUPowerInterface.get()->connection().connect("org.freedesktop.UPower",
                                                 "/org/freedesktop/UPower",
                                                 "org.freedesktop.DBus.Properties",
                                                 "PropertiesChanged",
                                                 this,
                                                 SLOT(propertiesChangedSlot(QString, QMap<QString,QVariant>, QStringList)));

    mUkccInterface = QSharedPointer<QDBusInterface>(
                new QDBusInterface("org.ukui.ukcc.session",
                                   "/",
                                   "org.ukui.ukcc.session.interface",
                                   QDBusConnection::sessionBus()));
}

void Widget::initNightStatus()
{
    QDBusInterface colorIft("org.ukui.KWin",
                            "/ColorCorrect",
                            "org.ukui.kwin.ColorCorrect",
                            QDBusConnection::sessionBus());
    if (colorIft.isValid() && Utils::isExistEffect() && !mIsWayland) {
        this->mRedshiftIsValid = true;
    } else {
        qWarning() << "create org.ukui.kwin.ColorCorrect failed";
        return;
    }

    QDBusMessage result = colorIft.call("nightColorInfo");

    QList<QVariant> outArgs = result.arguments();
    QVariant first = outArgs.at(0);
    QDBusArgument dbvFirst = first.value<QDBusArgument>();
    QVariant vFirst = dbvFirst.asVariant();
    const QDBusArgument &dbusArgs = vFirst.value<QDBusArgument>();

    QVector<ColorInfo> nightColor;

    dbusArgs.beginArray();
    while (!dbusArgs.atEnd()) {
        ColorInfo color;
        dbusArgs >> color;
        nightColor.push_back(color);
    }
    dbusArgs.endArray();

    for (ColorInfo it : nightColor) {
        mNightConfig.insert(it.arg, it.out.variant());
    }

    this->mIsNightMode = mNightConfig["Active"].toBool();
    ui->temptSlider->setValue(mNightConfig["CurrentColorTemperature"].toInt());
    if (mNightConfig["Mode"].toInt() != 2) {
        ui->sunradioBtn->setChecked(true);
    } else {
        ui->customradioBtn->setChecked(true);
        QString openTime = mNightConfig["EveningBeginFixed"].toString();
        QString ophour = openTime.split(":").at(0);
        QString opmin = openTime.split(":").at(1);

        ui->opHourCom->setCurrentIndex(ophour.toInt());
        ui->opMinCom->setCurrentIndex(opmin.toInt());

        QString cltime = mNightConfig["MorningBeginFixed"].toString();
        QString clhour = cltime.split(":").at(0);
        QString clmin = cltime.split(":").at(1);

        ui->clHourCom->setCurrentIndex(clhour.toInt());
        ui->clMinCom->setCurrentIndex(clmin.toInt());
    }
}

void Widget::nightChangedSlot(QHash<QString, QVariant> nightArg)
{
    if (this->mRedshiftIsValid) {
        mNightButton->setChecked(nightArg["Active"].toBool());
    }
}


/* 总结: 亮度条怎么显示和实际的屏幕状态有关,与按钮选择状态关系不大:
 * 实际为镜像模式，就显示所有屏幕的亮度(笔记本外显除外，笔记本外显任何情况均隐藏，这里未涉及)。
 * 实际为扩展模式，就显示当前选中的屏幕亮度，如果当前选中复制模式，则亮度条隐藏不显示，应用之后再显示所有亮度条;
 * 实际为单屏模式，即另一个屏幕关闭，则显示打开屏幕的亮度，关闭的显示器不显示亮度
 *
 *ps: by feng chao
*/

void Widget::showBrightnessFrame(const int flag)
{
    int *pFlag = new int(flag);
    QObject::connect(new KScreen::GetConfigOperation(), &KScreen::GetConfigOperation::finished,
                     [&, pFlag](KScreen::ConfigOperation *op) {
        bool allShowFlag = true;

        KScreen::ConfigPtr config = qobject_cast<KScreen::GetConfigOperation *>(op)->config();


        KScreen::OutputPtr output = config->primaryOutput();

        if (mConfig->connectedOutputs().count() >= 2) {
            foreach (KScreen::OutputPtr secOutput, config->connectedOutputs()) {
                if (!output || secOutput->geometry() != output->geometry() || !secOutput->isEnabled()) {
                    allShowFlag = false;
                }
                for (int i = 0; i < BrightnessFrameV.size(); ++i) { //检查其它显示屏是否实际打开，否则关闭，适用于显示器插拔
                    if (BrightnessFrameV[i]->getOutputName() == Utils::outputName(secOutput) && !secOutput->isEnabled()){
                        BrightnessFrameV[i]->setOutputEnable(false);
                    }
                }
            }
        } else {  //只有一个屏幕，把它亮度条打开，防止remove出问题
            allShowFlag = false;
            for (int i = 0; i < BrightnessFrameV.size(); ++i) {
                if (BrightnessFrameV[i]->getOutputName() == Utils::outputName(output)) {
                    BrightnessFrameV[i]->setOutputEnable(true);
                }
            }
        }

        ui->unifyBrightFrame->setFixedHeight(0);
        if (*pFlag == 0 && allShowFlag == false && mUnifyButton->isChecked()) {  //选中了镜像模式，实际是扩展模式

        } else if ((allShowFlag == true && *pFlag == 0) || *pFlag == 1) { //镜像模式/即将成为镜像模式
            ui->unifyBrightFrame->setFixedHeight(BrightnessFrameV.size() * (50 + 2 + 2));
            for (int i = 0; i < BrightnessFrameV.size(); ++i) {
                BrightnessFrameV[i]->setOutputEnable(true);
                BrightnessFrameV[i]->setTextLabelName(tr("Brightness") + QString("(") + BrightnessFrameV[i]->getOutputName() + QString(")"));
                BrightnessFrameV[i]->setVisible(true);
            }
        } else {
            for (int i = 0; i < BrightnessFrameV.size(); ++i) {
                if (ui->primaryCombo->currentText() == BrightnessFrameV[i]->getOutputName() && BrightnessFrameV[i]->getOutputEnable()) {
                    ui->unifyBrightFrame->setFixedHeight(52);
                    BrightnessFrameV[i]->setTextLabelName(tr("Brightness"));
                    BrightnessFrameV[i]->setVisible(true);
                    //不能break，要把其他的frame隐藏
                } else {
                    BrightnessFrameV[i]->setVisible(false);
                }
            }
        }
        if (ui->unifyBrightFrame->height() > 0) {
            ui->unifyBrightFrame->setVisible(true);
        } else {
            ui->unifyBrightFrame->setVisible(false);
        }
        delete pFlag;
    });
}

QList<ScreenConfig> Widget::getPreScreenCfg()
{
    QDBusMessage msg = mUkccInterface.get()->call("getPreScreenCfg");
    if(msg.type() == QDBusMessage::ErrorMessage) {
        qWarning() << "get pre screen cfg failed";
    }
    QDBusArgument argument = msg.arguments().at(0).value<QDBusArgument>();
    QList<QVariant> infos;
    argument >> infos;

    QList<ScreenConfig> preScreenCfg;
    for (int i = 0; i < infos.size(); i++){
        ScreenConfig cfg;
        infos.at(i).value<QDBusArgument>() >> cfg;
        preScreenCfg.append(cfg);
    }

    return preScreenCfg;
}

void Widget::setPreScreenCfg(KScreen::OutputList screens)
{
    QMap<int, KScreen::OutputPtr>::iterator nowIt = screens.begin();

    QVariantList retlist;
    while (nowIt != screens.end()) {
        ScreenConfig cfg;
        cfg.screenId = nowIt.value()->name();
        cfg.screenModeId = nowIt.value()->currentModeId();
        cfg.screenPosX = nowIt.value()->pos().x();
        cfg.screenPosY = nowIt.value()->pos().y();

        QVariant variant = QVariant::fromValue(cfg);
        retlist << variant;
        nowIt++;
    }

    mUkccInterface.get()->call("setPreScreenCfg", retlist);

    QVariantList outputList;
    Q_FOREACH(QVariant variant, retlist) {
        ScreenConfig screenCfg = variant.value<ScreenConfig>();
        QVariantMap map;
        map["id"] = screenCfg.screenId;
        map["modeid"] = screenCfg.screenModeId;
        map["x"] = screenCfg.screenPosX;
        map["y"] = screenCfg.screenPosY;
        outputList << map;
    }

    QString filePath = QDir::homePath() + "/.config/ukui/ukcc-screenPreCfg.json";
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open config file for writing! " << file.errorString();

    }
    file.write(QJsonDocument::fromVariant(outputList).toJson());
}
