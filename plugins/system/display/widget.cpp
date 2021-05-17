#include "widget.h"
#include "controlpanel.h"
#include "declarative/qmloutput.h"
#include "declarative/qmlscreen.h"
#include "utils.h"
#include "ui_display.h"
#include "displayperformancedialog.h"
#include "colorinfo.h"
#include "../../../shell/utils/utils.h"

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
    initBrightnessUI();

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

void Widget::setConfig(const KScreen::ConfigPtr &config)
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
    if (!mIsWayland) {
        connect(mConfig.data(), &KScreen::Config::primaryOutputChanged,
                this, &Widget::primaryOutputChanged);
    }

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

    if (mFirstLoad && isCloneMode()) {
        mUnifyButton->blockSignals(true);
        mUnifyButton->setChecked(true);
        mUnifyButton->blockSignals(false);
        slotUnifyOutputs();
    }
    mFirstLoad = false;
    QtConcurrent::run(std::mem_fn(&Widget::setBrightnesSldierValue), this);

    if (mIsWayland) {
        mScreenId = getPrimaryScreenID();
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

void Widget::slotOutputConnectedChanged()
{
    resetPrimaryCombo();
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
        KScreen::OutputList screens = mPrevConfig->connectedOutputs();

        QMap<int, KScreen::OutputPtr>::iterator preIt = screens.begin();
        QMap<int, KScreen::OutputPtr>::iterator nowIt = screens.begin();
        nowIt++;
        while (nowIt != screens.end()) {
            nowIt.value()->setPos(QPoint(preIt.value()->pos().x() + preIt.value()->size().width(), 0));
            KScreen::ModeList modes = preIt.value()->modes();
            Q_FOREACH (const KScreen::ModePtr &mode, modes) {
                if (preIt.value()->currentModeId() == mode->id()) {
                    if (preIt.value()->rotation() != KScreen::Output::Rotation::Left && preIt.value()->rotation() != KScreen::Output::Rotation::Right) {
                        nowIt.value()->setPos(QPoint(preIt.value()->pos().x() + mode->size().width(), 0));
                    } else {
                        nowIt.value()->setPos(QPoint(preIt.value()->pos().x() + mode->size().height(), 0));
                    }
                }
            }
            preIt = nowIt;
            nowIt++;
        }
        setConfig(mPrevConfig);

        ui->primaryCombo->setEnabled(true);
        mCloseScreenButton->setEnabled(true);
        ui->showMonitorframe->setVisible(true);
        ui->brightnessframe->setVisible(isVisibleBrightness());
        ui->primaryCombo->setEnabled(true);
    } else if (!base->isCloneMode() && mUnifyButton->isChecked()) {
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

        // 关闭开关
        mCloseScreenButton->setEnabled(false);
        ui->showMonitorframe->setVisible(false);
        ui->brightnessframe->setVisible(false);
        ui->primaryCombo->setEnabled(false);
        ui->mainScreenButton->setEnabled(false);
        mControlPanel->setUnifiedOutput(base->outputPtr());
    }
    Q_EMIT changed();
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
    QFont font;
    font.setPixelSize(18);
    ui->titleLabel->setFont(font);

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

    QByteArray powerId(POWER_SCHMES);
    if (QGSettings::isSchemaInstalled(powerId)) {
        mPowerGSettings = new QGSettings(powerId, QByteArray(), this);
        mPowerKeys = mPowerGSettings->keys();
        connect(mPowerGSettings, &QGSettings::changed, this, [=](QString key) {
            if ("brightnessAc" == key || "brightnessBat" == key) {
                int value = mPowerGSettings->get(key).toInt();
                if (mIsWayland && !mIsBattery) {
                    value = (value == 0 ? 0 : value / 10);
                }
                ui->brightnessSlider->blockSignals(true);
                ui->brightValueLabel->setText(QString::number(value));
                ui->brightnessSlider->setValue(value);
                ui->brightnessSlider->blockSignals(false);
            }
        });
    }

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
    int ret;
    QRect rect = this->topLevelWidget()->geometry();
    QMessageBox msg;
    int msgX = 0, msgY = 0;
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
        msgX = rect.x() + rect.width()/2 - 500/2 - 2;
        msgY = rect.y() + rect.height()/2 - 145/2 + 35;
        msg.move(msgX, msgY);
        ret = msg.exec();
    }

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
            || (mIsWayland && !mIsBattery)) {
        return true;
    }
    return false;
}

int Widget::getDDCBrighthess()
{
    QString type = getMonitorType();
    QDBusInterface ukccIfc("com.control.center.qt.systemdbus",
                           "/",
                           "com.control.center.interface",
                           QDBusConnection::systemBus());

    QDBusReply<int> reply = ukccIfc.call("getDDCBrightness", type);

    if (reply.isValid()) {
        return reply.value();
    }
    return 0;
}

int Widget::getLaptopBrightness() const
{
    return mPowerGSettings->get(POWER_KEY).toInt();
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

void Widget::outputAdded(const KScreen::OutputPtr &output)
{
    connect(output.data(), &KScreen::Output::isConnectedChanged,
            this, &Widget::slotOutputConnectedChanged);
    connect(output.data(), &KScreen::Output::isEnabledChanged,
            this, &Widget::slotOutputEnabledChanged);
    connect(output.data(), &KScreen::Output::posChanged,
            this, &Widget::changed);

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
    Q_EMIT changed();
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

void Widget::setDDCBrighthessSlot(int brightnessValue)
{
    QString type = getMonitorType();
    QDBusInterface ukccIfc("com.control.center.qt.systemdbus",
                           "/",
                           "com.control.center.interface",
                           QDBusConnection::systemBus());


    if (mLock.tryLock()) {
        ukccIfc.call("setDDCBrightness", QString::number(brightnessValue), type);
        mLock.unlock();
    }
}

void Widget::kdsScreenchangeSlot()
{
    connect(new KScreen::GetConfigOperation(), &KScreen::GetConfigOperation::finished,
            [&](KScreen::ConfigOperation *op) {
        bool cloneMode = true;
        KScreen::ConfigPtr config = qobject_cast<KScreen::GetConfigOperation *>(op)->config();
        KScreen::OutputPtr output = config->primaryOutput();
        if (config->connectedOutputs().count() >= 2) {
            foreach (KScreen::OutputPtr secOutput, config->connectedOutputs()) {
                if (secOutput->geometry() != output->geometry() || !secOutput->isEnabled()) {
                    cloneMode = false;
                }
            }
        } else {
            cloneMode = false;
        }
        mUnifyButton->setChecked(cloneMode);
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
    } else if (((ui->opHourCom->currentIndex() < ui->clHourCom->currentIndex())
                || (ui->opHourCom->currentIndex() == ui->clHourCom->currentIndex()
                    && ui->opMinCom->currentIndex() <= ui->clMinCom->currentIndex()))
               && CUSTOM == singleButton->checkedId() && mNightButton->isChecked()) {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Open time should be earlier than close time!"));
        mCloseScreenButton->setChecked(true);
        return;
    }

    writeScale(this->mScreenScale);
    setNightMode(mNightButton->isChecked());

    if (!KScreen::Config::canBeApplied(config)) {
        QMessageBox::information(this,
                                 tr("Warnning"),
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
            QtConcurrent::run(std::mem_fn(&Widget::setBrightnesSldierValue), this);
            QString hash = config->connectedOutputsHash();
            writeFile(mDir % hash);
        }
        mBlockChanges = false;
    });

    int enableScreenCount = 0;
    KScreen::OutputPtr enableOutput;
    for (const KScreen::OutputPtr &output : mConfig->outputs()) {
        if (output->isEnabled()) {
            enableOutput = output;
            enableScreenCount++;
        }
    }

    if (mIsWayland && -1 != mScreenId) {
        if (enableScreenCount >= 2 && !config.isNull()) {
            config->output(mScreenId)->setPrimary(true);
            callMethod(config->primaryOutput()->geometry(), config->primaryOutput()->name());
            if (mScreen->primaryOutput()) {
                mScreen->primaryOutput()->setIsCloneMode(mUnifyButton->isChecked());
            }
        } else if (!enableOutput.isNull()) {
            enableOutput->setPrimary(true);
            callMethod(enableOutput->geometry(), enableOutput->name());
        }
    }

    mScreen->updateOutputsPlacement();

    QTimer::singleShot(500, this, [=]() {
        if (isRestoreConfig()) {
            if (mIsWayland && -1 != mScreenId) {
                mPrevConfig->output(mScreenId)->setPrimary(true);
                callMethod(mPrevConfig->output(mScreenId)->geometry(), mPrevConfig->output(mScreenId)->name());
            }
            auto *op = new KScreen::SetConfigOperation(mPrevConfig);
            op->exec();

            // 无法知道什么时候执行完操作
            QTimer::singleShot(1000, this, [=]() {
                writeFile(mDir % mPrevConfig->connectedOutputsHash());
            });
        } else {
            writeScreenXml();
        }
    });

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
    QtConcurrent::run(std::mem_fn(&Widget::setBrightSliderVisible), this);

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

    ui->brightnessframe->setVisible(newPrimary->isEnabled() && isVisibleBrightness());
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

    mScreenId = newPrimary->id();

    Q_EMIT changed();
}

void Widget::checkOutputScreen(bool judge)
{
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
    Q_EMIT changed();
}

// 亮度调节UI
void Widget::initBrightnessUI()
{
    ui->brightnessSlider->setRange(0, 100);
    if (mIsWayland && !mIsBattery) {
        connect(ui->brightnessSlider, &QSlider::valueChanged, this, &Widget::setDDCBrightness);
    } else {
        connect(ui->brightnessSlider, &QSlider::valueChanged, this, &Widget::setBrightnessScreen);
    }

    if (mIsWayland) {
        ui->darkLabel->setVisible(false);
        ui->brihghtLabel->setVisible(false);
    } else {
        ui->brightValueLabel->setVisible(false);
    }
}

void Widget::initConnection()
{
    connect(mNightButton, SIGNAL(checkedChanged(bool)), this, SLOT(showNightWidget(bool)));
    connect(mThemeButton, SIGNAL(checkedChanged(bool)), this, SLOT(slotThemeChanged(bool)));
    connect(singleButton, SIGNAL(buttonClicked(int)), this, SLOT(showCustomWiget(int)));
    connect(ui->primaryCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &Widget::mainScreenButtonSelect);

    connect(ui->mainScreenButton, SIGNAL(clicked(bool)), this, SLOT(primaryButtonEnable(bool)));
    mControlPanel = new ControlPanel(this);
    connect(mControlPanel, &ControlPanel::changed, this, &Widget::changed);
    connect(this, &Widget::changed, this, &Widget::changedSlot);
    connect(mControlPanel, &ControlPanel::scaleChanged, this, &Widget::scaleChangedSlot);

    ui->controlPanelLayout->addWidget(mControlPanel);

    connect(ui->applyButton, &QPushButton::clicked, this, [=]() {
        save();
    });

    connect(ui->advancedBtn, &QPushButton::clicked, this, [=] {
        DisplayPerformanceDialog *dialog = new DisplayPerformanceDialog;
        dialog->exec();
    });

    connect(mUnifyButton, &SwitchButton::checkedChanged,
            [this] {
        slotUnifyOutputs();
    });

    connect(mCloseScreenButton, &SwitchButton::checkedChanged,
            this, [=](bool checked) {
        checkOutputScreen(checked);
    });

    connect(QApplication::desktop(), &QDesktopWidget::resized, this, [=] {
        QTimer::singleShot(1500, this, [=]{
            kdsScreenchangeSlot();
        });
    });

    connect(QApplication::desktop(), &QDesktopWidget::screenCountChanged, this, [=] {
        QTimer::singleShot(1500, this, [=] {
            kdsScreenchangeSlot();
        });
    });

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
}

void Widget::setBrightnessScreen(int value)
{
    qDebug() << Q_FUNC_INFO << value;
    ui->brightValueLabel->setText(QString::number(value));
    mPowerGSettings->set(POWER_KEY, value);
}

void Widget::setDDCBrightness(int value)
{
    qDebug() << Q_FUNC_INFO << value;
    ui->brightValueLabel->setText(QString::number(value));
    if (!isLaptopScreen()) {
        setDDCBrighthessSlot(value);
    } else {
        setBrightnessScreen(value);
    }
}

void Widget::setBrightSliderVisible()
{
    int value;
    if (mIsBattery && !mUnifyButton->isChecked()) {
        ui->brightnessframe->setVisible(isVisibleBrightness());
        if (isLaptopScreen()) {
            value = getLaptopBrightness();
        }
    } else {
        value = getDDCBrighthess();
    }
    ui->brightValueLabel->setText(QString::number(value));
    ui->brightnessSlider->blockSignals(true);
    ui->brightnessSlider->setValue(value);
    ui->brightnessSlider->blockSignals(false);
}

// 滑块改变
void Widget::setBrightnesSldierValue()
{
    int value = 99;
    value = getLaptopBrightness();

    if (mIsWayland && !mIsBattery) {
        int realValue = getDDCBrighthess();
        ui->brightValueLabel->setText(QString::number(realValue));
        ui->brightnessSlider->setValue(realValue);
    } else {
        ui->brightValueLabel->setText(QString::number(value));
        ui->brightnessSlider->setValue(value);
    }
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
        mNightConfig["Mode"] = 2;

        if (ui->sunradioBtn->isChecked()) {
            mNightConfig["EveningBeginFixed"] = "17:55:00";
            mNightConfig["MorningBeginFixed"] = "05:55:04";
        } else if (ui->customradioBtn->isChecked()) {
            mNightConfig["EveningBeginFixed"] = ui->opHourCom->currentText() + ":"
                                                + ui->opMinCom->currentText() + ":00";
            mNightConfig["MorningBeginFixed"] = ui->clHourCom->currentText() + ":"
                                                + ui->clMinCom->currentText() + ":00";
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

    QDBusReply<QVariant> briginfo;
    briginfo = brightnessInterface.call("Get", "org.freedesktop.UPower.Device", "PowerSupply");
    mIsBattery = briginfo.value().toBool();
    ui->brightnessframe->setVisible(isVisibleBrightness());

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
                                                 SLOT(propertiesChangedSlot(QString,QMap<QString,
                                                                                         QVariant>,
                                                                            QStringList)));
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
    if (0 == mNightConfig["Mode"].toInt()) {
        ui->sunradioBtn->setChecked(true);
    } else if (2 == mNightConfig["Mode"].toInt()) {
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
