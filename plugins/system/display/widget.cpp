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

#include <KAboutData>
#include <KMessageBox>
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

#define FONT_RENDERING_DPI               "org.ukui.SettingsDaemon.plugins.xsettings"
#define SCALE_KEY                        "scaling-factor"

#define DPI_SCHEMAS                      "org.ukui.font-rendering"
#define DPI_KEY                          "dpi"

#define MOUSE_SIZE_SCHEMAS               "org.ukui.peripherals-mouse"
#define CURSOR_SIZE_KEY                  "cursor-size"

#define POWER_SCHMES                     "org.ukui.power-manager"
#define POWER_KEY                        "brightness-ac"

#define ADVANCED_SCHEMAS                 "org.ukui.session.required-components"
#define ADVANCED_KEY                     "windowmanager"

Q_DECLARE_METATYPE(KScreen::OutputPtr)

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DisplayWindow()), slider(new Slider())
{
    qRegisterMetaType<QQuickView*>();
    gdk_init(NULL, NULL);

    ui->setupUi(this);
    ui->quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);

#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)
    oriApply = true;
#else
    oriApply = false;
    ui->quickWidget->setAttribute(Qt::WA_AlwaysStackOnTop);
    ui->quickWidget->setClearColor(Qt::transparent);
#endif

    ui->quickWidget->setContentsMargins(0,0,0,9);

    closeScreenButton = new SwitchButton;
    ui->showScreenLayout->addWidget(closeScreenButton);

    m_unifybutton = new SwitchButton;
//    m_unifybutton->setEnabled(false);
    ui->unionLayout->addWidget(m_unifybutton);


    QHBoxLayout *nightLayout = new QHBoxLayout(ui->nightframe);

    //~ contents_path /display/unify output
    ui->unifyLabel->setText(tr("unify output"));

    //~ contents_path /display/night mode
    nightLabel = new QLabel(tr("night mode"));
    nightButton = new SwitchButton;
    nightLayout->addWidget(nightLabel);
    nightLayout->addStretch();
    nightLayout->addWidget(nightButton);

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

    initTemptSlider();
    initConfigFile();
    initUiComponent();
    initNightStatus();
    initBrightnessUI();

//    nightButton->setVisible(this->m_redshiftIsValid);

#if QT_VERSION <= QT_VERSION_CHECK(5,12,0)
    ui->nightframe->setVisible(false);
#else
    ui->nightframe->setVisible(this->m_redshiftIsValid);
#endif

    nightButton->setChecked(this->m_isNightMode);
    showNightWidget(nightButton->isChecked());

    connect(nightButton,SIGNAL(checkedChanged(bool)),this,SLOT(showNightWidget(bool)));
    connect(singleButton, SIGNAL(buttonClicked(int)), this, SLOT(showCustomWiget(int)));
    //是否禁用主显示器确认按钮
    connect(ui->primaryCombo, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &Widget::mainScreenButtonSelect);
    //主屏确认按钮
    connect(ui->mainScreenButton, SIGNAL(clicked()),
            this, SLOT(primaryButtonEnable()));
    //ui->mainScreenButton->setFocusPolicy(Qt::NoFocus);
    mControlPanel = new ControlPanel(this);
    connect(mControlPanel, &ControlPanel::changed, this, &Widget::changed);
    connect(mControlPanel, &ControlPanel::scaleChanged, this, &Widget::scaleChangedSlot);

    ui->controlPanelLayout->addWidget(mControlPanel);

//    connect(ui->applyButton,SIGNAL(clicked()),this,SLOT(save()));
    // TODO: Find out why adjusting the screen orientation does not take effect
    connect(ui->applyButton, &QPushButton::clicked, this, [=](){
       save();
       if (oriApply) {
           QTimer::singleShot(1000, this,
               [this] () {
                  save();
               }
           );
       }
    });

    connect(ui->advancedBtn, &QPushButton::clicked, this, [=]{
        DisplayPerformanceDialog * dialog = new DisplayPerformanceDialog;
        dialog->exec();
    });

    connect(m_unifybutton,&SwitchButton::checkedChanged,
            [this]{
//                  if(checked)
                    slotUnifyOutputs();
            });

    //TODO----->bug
//    ui->showMonitorwidget->setVisible(false);
    connect(closeScreenButton,&SwitchButton::checkedChanged,
            this,[=](bool checked){
                checkOutputScreen(checked);
        });

    mOutputTimer = new QTimer(this);
    connect(mOutputTimer, &QTimer::timeout,
            this, &Widget::clearOutputIdentifiers);

    initGSettings();
    loadQml();
}

Widget::~Widget()
{
    clearOutputIdentifiers();
    delete ui;
}

bool Widget::eventFilter(QObject* object, QEvent* event)
{
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
//    ui->unifyButton->setEnabled(mConfig->outputs().count() > 1);
    m_unifybutton->setEnabled(mConfig->outputs().count() > 1);

    //ui->scaleAllOutputsButton->setVisible(!mConfig->supportedFeatures().testFlag(KScreen::Config::Feature::PerOutputScaling));

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
            primaryButtonEnable();
        }
    }
    slotOutputEnabledChanged();
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

    QQuickItem* rootObject = ui->quickWidget->rootObject();
    mScreen = rootObject->findChild<QMLScreen*>(QStringLiteral("outputView"));
    if (!mScreen) {
        return;
    }
    connect(mScreen, &QMLScreen::focusedOutputChanged,
            this, &Widget::slotFocusedOutputChanged);
}

void Widget::resetPrimaryCombo()
{
    //qDebug()<<"resetPrimaryCombo----->"<<endl;
#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)

#else
    bool isPrimaryDisplaySupported = mConfig->supportedFeatures().testFlag(KScreen::Config::Feature::PrimaryDisplay);
    ui->primaryLabel->setVisible(isPrimaryDisplaySupported);
    ui->primaryCombo->setVisible(isPrimaryDisplaySupported);
#endif

    // Don't emit currentIndexChanged when resetting
    bool blocked = ui->primaryCombo->blockSignals(true);
    ui->primaryCombo->clear();
    //ui->primaryCombo->addItem(i18n("无主显示输出"));
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
    if (output->isPrimary()) {
        Q_ASSERT(mConfig);
        int lastIndex = ui->primaryCombo->count() - 1;
        ui->primaryCombo->setCurrentIndex(lastIndex);
    }
}

//这里从屏幕点击来读取输出
void Widget::slotFocusedOutputChanged(QMLOutput *output)
{
    mControlPanel->activateOutput(output->outputPtr());

    //读取屏幕点击选择下拉框
    Q_ASSERT(mConfig);
    int index = output->outputPtr().isNull() ? 0 : ui->primaryCombo->findData(output->outputPtr()->id());
    if (index == -1 || index == ui->primaryCombo->currentIndex()) {
        return;
    }
    //qDebug()<<"下拉框id----->"<<index<<endl;
    ui->primaryCombo->setCurrentIndex(index);
}

void Widget::slotFocusedOutputChangedNoParam()
{
    //qDebug()<<"slotFocusedOutputChangedNoParam-------->"<<res<<endl;
    mControlPanel->activateOutput(res);
}


void Widget::slotOutputEnabledChanged()
{
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
//    ui->unifyButton->setEnabled(enabledOutputsCount > 1);
    m_unifybutton->setEnabled(enabledOutputsCount > 1);
}

void Widget::slotOutputConnectedChanged()
{
    resetPrimaryCombo();
}

void Widget::slotUnifyOutputs()
{
    QMLOutput *base = mScreen->primaryOutput();
    //qDebug()<<"primaryOutput---->"<<base<<endl;
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

    if (base->isCloneMode() && !m_unifybutton->isChecked()) {
        qDebug()<<"取消clone------------>"<<endl;

        QPoint secPoint;
        KScreen::OutputList screens =  mPrevConfig->connectedOutputs();

        QMap<int, KScreen::OutputPtr>::iterator it = screens.begin();
        while (it != screens.end()) {

            KScreen::OutputPtr screen= it.value();
//            qDebug()<<"screens is-------->"<<screen<<endl;
            if (screen->isPrimary()) {

                secPoint = QPoint(screen->size().width(),0);
            }
            it++;
        }

        QMap<int, KScreen::OutputPtr>::iterator secIt = screens.begin();
        while (secIt != screens.end()) {
            KScreen::OutputPtr screen= secIt.value();
            qDebug()<<"screens is-------->"<<screen<<endl;
            if (!screen->isPrimary()) {
                screen->setPos(secPoint);
            }
            secIt++;
        }

        setConfig(mPrevConfig);
        mPrevConfig.clear();

        ui->primaryCombo->setEnabled(true);
        //开启开关
//        ui->checkBox->setEnabled(true);
        closeScreenButton->setEnabled(true);
        ui->primaryCombo->setEnabled(true);
//        ui->unifyButton->setText(tr("统一输出"));
    } else if (!base->isCloneMode() && m_unifybutton->isChecked()){
        // Clone the current config, so that we can restore it in case user
        // breaks the cloning
        qDebug()<<"点击统一输出---->"<<endl;
        //保存之前的配置
        mPrevConfig = mConfig->clone();

        for (QMLOutput *output: mScreen->outputs()) {

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
//        ui->checkBox->setEnabled(false);
        closeScreenButton->setEnabled(false);
        ui->primaryCombo->setEnabled(false);
        ui->mainScreenButton->setEnabled(false);

        //qDebug()<<"输出---->"<<base->outputPtr()<<endl;
        mControlPanel->setUnifiedOutput(base->outputPtr());

//        ui->unifyButton->setText(tr("取消统一输出"));
    }
    Q_EMIT changed();
}

// FIXME: Copy-pasted from KDED's Serializer::findOutput()
KScreen::OutputPtr Widget::findOutput(const KScreen::ConfigPtr &config, const QVariantMap &info)
{
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
    if (isScaleChanged) {
        KMessageBox::information(this,tr("Some applications need to be logouted to take effect"));
    }
    isScaleChanged = false;
    int cursize;
    QGSettings * dpiSettings;
    QGSettings * cursorSettings;
    QGSettings * fontSettings;
    QByteArray id(FONT_RENDERING_DPI);
    QByteArray iid(MOUSE_SIZE_SCHEMAS);
    QByteArray iiid(DPI_SCHEMAS);
    if (QGSettings::isSchemaInstalled(FONT_RENDERING_DPI) && QGSettings::isSchemaInstalled(MOUSE_SIZE_SCHEMAS)
            && QGSettings::isSchemaInstalled(DPI_SCHEMAS)) {
        dpiSettings = new QGSettings(id);
        cursorSettings = new QGSettings(iid);
        fontSettings = new QGSettings(iiid);

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

        QStringList keys = dpiSettings->keys();
        if (keys.contains("scalingFactor")) {
            fontSettings->set(DPI_KEY, 96);
            dpiSettings->set(SCALE_KEY, scale);
        }
        cursorSettings->set(CURSOR_SIZE_KEY, cursize);

        delete dpiSettings;
        delete cursorSettings;
    }
}


void Widget::initGSettings() {
    QByteArray id(UKUI_CONTORLCENTER_PANEL_SCHEMAS);
    if(QGSettings::isSchemaInstalled(id)) {
        m_gsettings = new QGSettings(id)        ;
    } else {
        return ;
    }
}

void Widget::writeConfigFile() {

    if (m_gsettings) {
        m_gsettings->set(NIGHT_MODE_KEY, nightButton->isChecked());
    }

    m_qsettings->beginGroup("redshift");
    QString optime = ui->opHourCom->currentText() + ":" + ui->opMinCom->currentText();
    QString cltime = ui->clHourCom->currentText() + ":" + ui->clMinCom->currentText();
    QString value = QString::number(ui->temptSlider->value());


    if ( !ui->customradioBtn->isChecked()) {
        optime = "17:55";
        cltime = "05:04";
    }

    m_qsettings->setValue("dawn-time", cltime);
    m_qsettings->setValue("dusk-time", optime);
    m_qsettings->setValue("temp-day", tempDayBrig);
    m_qsettings->setValue("temp-night", value);

    m_qsettings->endGroup();

    m_qsettings->beginGroup("switch");
    m_qsettings->setValue("unionswitch", m_unifybutton->isChecked());
    m_qsettings->setValue("nightjudge", nightButton->isChecked());
    m_qsettings->setValue("sunjudge", ui->sunradioBtn->isChecked());
    m_qsettings->setValue("manualjudge", ui->customradioBtn->isChecked());
    m_qsettings->setValue("nightStatus", nightButton->isChecked());

    m_qsettings->endGroup();
    m_qsettings->sync();
}

void Widget::showNightWidget(bool judge) {
    if (judge) {
        ui->sunframe->setVisible(true);
        ui->customframe->setVisible(true);
//        ui->temptwidget->setVisible(true);
        ui->temptframe->setVisible(true);
    } else {
        ui->sunframe->setVisible(false);
        ui->customframe->setVisible(false);

//        ui->temptwidget->setVisible(false);
        ui->temptframe->setVisible(false);
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
}

void Widget::primaryOutputSelected(int index) {
    //qDebug()<<"选中主显示器--->"<<index<<endl;
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


void Widget::save()
{
    if (!this) {
        return;
    }

    const KScreen::ConfigPtr &config = this->currentConfig();
    const int countOutput = config->connectedOutputs().count();

    bool atLeastOneEnabledOutput = false;
    int i = 0;
    int connectedScreen = 0;
    int outputCount = 0;
    Q_FOREACH(const KScreen::OutputPtr &output, config->outputs()) {
        KScreen::ModePtr mode = output->currentMode();
        if (output->isEnabled()) {
            atLeastOneEnabledOutput = true;
            connectedScreen++;
        }
        if (!output->isConnected())
            continue;
        outputCount++;

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
        inputXml[i].isClone = base->isCloneMode() == true? "yes" : "no";
        inputXml[i].outputName = output->name();

        inputXml[i].widthValue = QString::number(mode->size().width());
        inputXml[i].heightValue = QString::number(mode->size().height());
        inputXml[i].rateValue = QString::number(mode->refreshRate());

        inputXml[i].posxValue = (outputCount == 2 && connectedScreen == 1) ? "0" : QString::number(output->pos().x());
        inputXml[i].posyValue = QString::number(output->pos().y());
        inputXml[i].vendorName = output->edid()->pnpId();

        auto rotation = [&] ()->QString {
                if(1 == output->rotation())
                    return "normal";
                else if(2 == output->rotation())
                    return "left";
                else if(4 == output->rotation())
                    return "upside_down";
                else if(8 == output->rotation())
                    return "right";
        };

        inputXml[i].rotationValue = rotation();
        inputXml[i].isPrimary = (output->isPrimary() == true?"yes":"no");
        inputXml[i].isEnable = output->isEnabled();

        getEdidInfo(output->name(),&inputXml[i]);
        i++;
    }

    if (!atLeastOneEnabledOutput ) {
        qDebug()<<"atLeastOneEnabledOutput---->"<<connectedScreen<<endl;
        KMessageBox::error(this,tr("please insure at least one output!"),
                           tr("Warning"),KMessageBox::Notify);
        closeScreenButton->setChecked(true);
        return ;
    } else if( ((ui->opHourCom->currentIndex() < ui->clHourCom->currentIndex()) ||
               (ui->opHourCom->currentIndex() == ui->clHourCom->currentIndex()  &&
                ui->opMinCom->currentIndex() <= ui->clMinCom->currentIndex()))  &&
               CUSTOM == singleButton->checkedId() && nightButton->isChecked()) {
        KMessageBox::error(this,tr("Morning time should be earlier than evening time!"),
                           tr("Warning"),KMessageBox::Notify);
        closeScreenButton->setChecked(true);
        return ;
    }

//    int scale = static_cast<int>(this->scaleRet());
    initScreenXml(countOutput);
    writeScreenXml(countOutput);
#if QT_VERSION < QT_VERSION_CHECK(5, 7, 0)
#else
    writeScale(static_cast<float>(this->screenScale));
#endif
    writeConfigFile();
    setNightMode(nightButton->isChecked());


    if (!KScreen::Config::canBeApplied(config)) {
        KMessageBox::information(this,
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
}

void Widget::scaleChangedSlot(int index) {
    qDebug()<<"scale changed----------->"<<index<<endl;
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
    isScaleChanged = true;
}


//是否禁用主屏按钮
void Widget::mainScreenButtonSelect(int index){
    //qDebug()<<"index is----->"<<index<<" "<<mConfig->primaryOutput()<<endl;
    if (!mConfig) {
        return;
    }

    const KScreen::OutputPtr newPrimary = mConfig->output(ui->primaryCombo->itemData(index).toInt());
    //qDebug()<<"newPrimary----->"<<newPrimary<<" "<< index<<endl;
    if (newPrimary == mConfig->primaryOutput()) {
        ui->mainScreenButton->setEnabled(false);
    }else{
        ui->mainScreenButton->setEnabled(true);
    }
//    if(index == 0){
//        ui->mainScreenButton->setEnabled(false);
//        ui->checkBox->setEnabled(false);
//        return ;
//    }
    // 设置是否勾选
//   ui->checkBox->setEnabled(true);
    closeScreenButton->setEnabled(true);
//   ui->checkBox->setChecked(newPrimary->isEnabled());
    closeScreenButton->setChecked(newPrimary->isEnabled());
   mControlPanel->activateOutput(newPrimary);
}


//设置主屏按钮
void Widget::primaryButtonEnable(){
    if (!mConfig) {
        return;
    }
    int index  = ui->primaryCombo->currentIndex();    ;
    ui->mainScreenButton->setEnabled(false);//连接外显后点击设置为主屏后会发生页面自动向下跳，注释掉没有这个问题
    const KScreen::OutputPtr newPrimary = mConfig->output(ui->primaryCombo->itemData(index).toInt());
   // qDebug()<<"按下主屏按钮---->"<<newPrimary<<"index ----"<<index<<endl;
    mConfig->setPrimaryOutput(newPrimary);

    Q_EMIT changed();
   // return;
}

void Widget::checkOutputScreen(bool judge){
//   qDebug()<<"is enable screen---->"<<judge<<endl;
   int index  = ui->primaryCombo->currentIndex();
   const KScreen::OutputPtr newPrimary = mConfig->output(ui->primaryCombo->itemData(index).toInt());
//   if(ui->primaryCombo->count()<=1&&judge ==false)
//       return ;
//   qDebug()<<"newPrimary---------->"<<newPrimary<<endl;

   KScreen::OutputPtr  mainScreen=  mConfig->primaryOutput();
//   qDebug()<<"mainScreen is------------>"<<mainScreen<<endl;
   if (!mainScreen) {
       mConfig->setPrimaryOutput(newPrimary);
   }
   newPrimary->setEnabled(judge);
   ui->primaryCombo->setCurrentIndex(index);
   Q_EMIT changed();
}

//亮度调节UI
void Widget::initBrightnessUI(){
    //亮度调节
//    ui->brightnesswidget->setStyleSheet("background-color:#F4F4F4;border-radius:6px");

    ui->brightnessSlider->setRange(0.2*100,100);
    ui->brightnessSlider->setTracking(true);

    QString screenName = getScreenName();

    setBrightnesSldierValue(screenName);

    connect(ui->brightnessSlider,&QSlider::valueChanged,
            this,&Widget::setBrightnessScreen);

    connect(ui->primaryCombo, &QComboBox::currentTextChanged,
            this, &Widget::setBrightnesSldierValue);
}


QString Widget::getScreenName(QString screenname){
    if("" == screenname )
        screenname = ui->primaryCombo->currentText();
    int startPos = screenname.indexOf('(');
    int endPos = screenname.indexOf(')');
    return screenname.mid(startPos+1,endPos-startPos-1);
}

void Widget::setBrightnessScreen(int index){
    QGSettings *powerSettings;
    if (QGSettings::isSchemaInstalled(POWER_SCHMES)) {
        QByteArray id(POWER_SCHMES);
        powerSettings = new QGSettings(id);
        QStringList keys = powerSettings->keys();
        if (keys.contains("brightnessAc")) {
            powerSettings->set(POWER_KEY, index);
        }
    }
    if (!powerSettings) {
        delete powerSettings;
    }
}

//滑块改变
void Widget::setBrightnesSldierValue(QString name){
    Q_UNUSED(name)
    QGSettings *powerSettings;
    int value = 99;
    if (QGSettings::isSchemaInstalled(POWER_SCHMES)) {
        QByteArray id(POWER_SCHMES);
        powerSettings = new QGSettings(id);
        QStringList keys = powerSettings->keys();
        if (keys.contains("brightnessAc")) {
            value = powerSettings->get(POWER_KEY).toInt();
        }
    }
    if (!powerSettings) {
        delete powerSettings;
    }
    ui->brightnessSlider->setValue(value);
}

void Widget::initTemptSlider() {
    ui->temptSlider->setRange(1.1*1000,6500);
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

void Widget::initConfigFile() {
    QString filename = QDir::homePath() + "/.config/redshift.conf";
    m_qsettings = new QSettings(filename, QSettings::IniFormat);

    m_qsettings->beginGroup("redshift");

    QString optime = m_qsettings->value("dusk-time", "").toString();
    QString cltime = m_qsettings->value("dawn-time", "").toString();
    QString temptValue = m_qsettings->value("temp-night", "").toString();

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

    m_qsettings->endGroup();


    m_qsettings->beginGroup("switch");;

    bool unionjudge = m_qsettings->value("unionswitch", unionjudge).toBool();
    bool nightjudge = m_qsettings->value("nightjudge", nightjudge).toBool();
    bool sunjudge = m_qsettings->value("sunjudge", sunjudge).toBool();
    bool manualjudge = m_qsettings->value("manualjudge", manualjudge).toBool();

//    qDebug()<<"unionjudge is-------->"<<unionjudge<<endl;
    m_unifybutton->setChecked(unionjudge);
    nightButton->setChecked(nightjudge);

    if (!(sunjudge && manualjudge)) {
        ui->sunradioBtn->setChecked(sunjudge);
        ui->customradioBtn->setChecked(manualjudge);
    } else {
        ui->sunradioBtn->setCheckable(true);
    }

    m_qsettings->endGroup();
}

void Widget::writeScreenXml(int count){
    Q_UNUSED(count)

    QString homePath = getenv("HOME");
    QString monitorFile = homePath+"/.config/monitors.xml";
    //qDebug()<<monitorFile<<endl;
    QFile file(monitorFile);
    QDomDocument doc;

    if (!file.open(QIODevice::ReadOnly)){
        qDebug()<<"open file failed"<<endl;
        return ;
    }

    // 将文件内容读到doc中
    if (!doc.setContent(&file)) {
        qDebug()<<"open file failed"<<endl;
        file.close();
        return ;
    }
    // 关闭文件
    file.close();

    // 获得doc的第一个结点
    QDomElement rootElem = doc.documentElement();
    QDomNode firstNode = doc.firstChild();

    if(firstNode.isElement()){
       // qDebug()<<"attr--->"<<endl;
         QDomElement felm = firstNode.toElement();
//         qDebug() << (felm.tagName())
//             << (felm.attribute("version"));
    }

    // 返回根元素
    QDomElement docElem = doc.documentElement();
    // 返回根节点的第一个子结点
    QDomNode n = docElem.firstChild();
    if(n.isElement()){
//        qDebug() <<"  "<< qPrintable(n.toElement().tagName());
    }
    // 如果结点不为空，则转到下一个节点
    while(!n.isNull())
    {
        // 如果结点是元素
        if (n.isElement())
        {
            // 获得元素e的所有子结点的列表
            QDomElement e = n.toElement();
            QDomNodeList list = e.childNodes();
//            qDebug()<<"list.count()---->"<<list.count();

            // 遍历该列表
            for(int i=0; i<list.count(); i++)
            {
                QDomNode node = list.at(i);
                    if(node.isElement()){

                        QDomNodeList e2 = node.childNodes();
                        if (node.toElement().tagName() == "clone") {
                             node.toElement().firstChild().setNodeValue(inputXml[i].isClone);
                             qDebug() << "    "<< (node.toElement().tagName())
                                      <<(node.toElement().text());

                        }else if("output" == node.toElement().tagName()
                            &&inputXml[i-1].outputName == node.toElement().attribute("name")
                            &&inputXml[i-1].isEnable
                            &&e2.count() > 3){

//                            qDebug()<<"e2  count is---->"<<e2.count()<<endl;
                            qDebug() << "    "<< (node.toElement().tagName())
                                    <<(node.toElement().attribute("name"));
                            for(int j=0;j<e2.count();j++){
                                QDomNode node2 = e2.at(j);
                                if(node2.toElement().tagName() == "width"){
                                    node2.toElement().firstChild().setNodeValue(inputXml[i-1].widthValue);
                                    qDebug() << "         "<< node2.toElement().tagName()
                                             <<node2.toElement().text();
                                }else if(node2.toElement().tagName() == "height"){
                                    node2.toElement().firstChild().setNodeValue(inputXml[i-1].heightValue);
                                    qDebug() << "         "<< node2.toElement().tagName()
                                             <<node2.toElement().text();
                                }else if(node2.toElement().tagName() == "rate"){
                                    node2.toElement().firstChild().setNodeValue(inputXml[i-1].rateValue);
                                    qDebug() << "         "<< node2.toElement().tagName()
                                             <<node2.toElement().text();
                                }else if(node2.toElement().tagName() == "x"){
                                    node2.toElement().firstChild().setNodeValue(inputXml[i-1].posxValue);
                                    qDebug() << "         "<< node2.toElement().tagName()
                                             <<node2.toElement().text();
                                }else if(node2.toElement().tagName() == "y"){
                                    node2.toElement().firstChild().setNodeValue(inputXml[i-1].posyValue);
                                    qDebug() << "         "<< node2.toElement().tagName()
                                             <<node2.toElement().text();
                                }else if(node2.toElement().tagName() == "rotation"){
                                    node2.toElement().firstChild().setNodeValue(inputXml[i-1].rotationValue);
                                    qDebug() << "         "<< node2.toElement().tagName()
                                             <<node2.toElement().text();
                                }else if(node2.toElement().tagName() == "primary"){
                                    node2.toElement().firstChild().setNodeValue(inputXml[i-1].isPrimary);
                                    qDebug() << "         "<< node2.toElement().tagName()
                                             <<node2.toElement().text();
                                }
                            }
                         }else if("output" == node.toElement().tagName()
                                   &&inputXml[i-1].outputName == node.toElement().attribute("name")
                                   &&!inputXml[i-1].isEnable
                                   &&e2.count() > 3) {
                            for(; e2.count()>3;){
                                QDomNode node2 = e2.at(2);
                                qDebug() << "         "<< node2.toElement().tagName()
                                         <<node2.toElement().text();
                                node.removeChild(e2.at(3));
                            }
                         }else if ("output" == node.toElement().tagName()
                                   &&inputXml[i-1].outputName == node.toElement().attribute("name")
                                   &&inputXml[i-1].isEnable
                                   &&e2.count() <= 3){

//                            qDebug()<<"增加节点---->"<<endl;
                            QDomElement width  = doc.createElement("width");
                            QDomText widthtext = doc.createTextNode(inputXml[i-1].widthValue);
                            width.appendChild(widthtext);
                            node.appendChild(width);

                            QDomElement height  = doc.createElement("height");
                            QDomText heightext = doc.createTextNode(inputXml[i-1].heightValue);
                            height.appendChild(heightext);
                            node.appendChild(height);

                            QDomElement rate  = doc.createElement("rate");
                            QDomText ratetext = doc.createTextNode(inputXml[i-1].rateValue);
                            rate.appendChild(ratetext);
                            node.appendChild(rate);

                            QDomElement x  = doc.createElement("x");
                            QDomText xtext = doc.createTextNode(inputXml[i-1].posxValue);
                            x.appendChild(xtext);
                            node.appendChild(x);

                            QDomElement y  = doc.createElement("y");
                            QDomText ytext = doc.createTextNode(inputXml[i-1].posyValue);
                            y.appendChild(ytext);
                            node.appendChild(y);

                            QDomElement rotation  = doc.createElement("rotation");
                            QDomText rotationtext = doc.createTextNode(inputXml[i-1].rotationValue);
                            rotation.appendChild(rotationtext);
                            node.appendChild(rotation);

                            QDomElement reflect_x  = doc.createElement("reflect_x");
                            QDomText reflect_xtext = doc.createTextNode("no");
                            reflect_x.appendChild(reflect_xtext);
                            node.appendChild(reflect_x);

                            QDomElement reflect_y  = doc.createElement("reflect_y");
                            QDomText reflect_ytext = doc.createTextNode("no");
                            reflect_y.appendChild(reflect_ytext);
                            node.appendChild(reflect_y);

                            QDomElement primary  = doc.createElement("primary");
                            QDomText primarytext = doc.createTextNode(inputXml[i-1].isPrimary);
                            primary.appendChild(primarytext);
                            node.appendChild(primary);

                         }else if (node.toElement().tagName() == "clone") {
                            node.toElement().firstChild().setNodeValue(inputXml[i-1].isClone);
                            qDebug() << "    "<< (node.toElement().tagName())
                                     <<(node.toElement().text());
                        }
                    }
                }
            }
        // 转到下一个兄弟结点
        n = n.nextSibling();
    }

    if(!file.open(QFile::WriteOnly | QFile::Truncate)){
        qDebug()<<"save file failed"<<endl;
        return ;
    }

    QTextStream out_stream(&file);
    doc.save(out_stream,4);
    file.close();
}


void Widget::initScreenXml(int count){
    QString homePath = getenv("HOME");
    QString monitorFile = homePath+"/.config/monitors.xml";
    //qDebug()<<monitorFile<<endl;
    QFile file(monitorFile);
    QDomDocument doc;

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        qDebug()<<"open file failed"<<endl;
        return ;
    }
    QXmlStreamWriter xmlWriter(&file);

    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartElement("monitors");
    xmlWriter.writeAttribute("version","1");
    xmlWriter.writeStartElement("configuration");
    xmlWriter.writeTextElement("clone","no");

    for(int i = 0; i < count; ++i){
        xmlWriter.writeStartElement("output");
        xmlWriter.writeAttribute("name",inputXml[i].outputName);
        xmlWriter.writeTextElement("vendor",inputXml[i].vendorName);
        xmlWriter.writeTextElement("product",inputXml[i].productName);
        xmlWriter.writeTextElement("serial",inputXml[i].serialNum);
        xmlWriter.writeTextElement("width",inputXml[i].widthValue);
        xmlWriter.writeTextElement("height",inputXml[i].heightValue);
        xmlWriter.writeTextElement("rate",inputXml[i].rateValue);
        xmlWriter.writeTextElement("x",inputXml[i].posxValue);
        xmlWriter.writeTextElement("y",inputXml[i].posyValue);
        xmlWriter.writeTextElement("rotation",inputXml[i].rotationValue);
        xmlWriter.writeTextElement("reflect_x","no");
        xmlWriter.writeTextElement("reflect_y","no");
        xmlWriter.writeTextElement("primary",inputXml[i].isPrimary);
        xmlWriter.writeEndElement();
    }
    xmlWriter.writeEndElement();
    xmlWriter.writeEndElement();
}

void Widget::getEdidInfo(QString monitorName,xmlFile *xml) {
    int i;
    int modelDec;
    int serialDec;

    GList *justTurnedOn;

    MateRRScreen *rwScreen;
    MateRRConfig *config;
    MateRROutputInfo **outputs;

    rwScreen = mate_rr_screen_new (gdk_screen_get_default (), NULL);
    config = mate_rr_config_new_current (rwScreen, NULL);
    justTurnedOn = NULL;
    outputs = mate_rr_config_get_outputs (config);

    for (i = 0; outputs[i] != NULL; i++) {
        MateRROutputInfo *output = outputs[i];
        if (mate_rr_output_info_is_connected (output) && !mate_rr_output_info_is_active (output)) {
            justTurnedOn = g_list_prepend (justTurnedOn, GINT_TO_POINTER (i));
        }
    }

    for (i = 0; outputs[i] != NULL; i++){
        MateRROutputInfo *output = outputs[i];
        if (g_list_find (justTurnedOn, GINT_TO_POINTER (i)))  {
            continue;
        }

        if (mate_rr_output_info_is_active (output)){
            g_assert (mate_rr_output_info_is_connected (output));
            char *name = mate_rr_output_info_get_name(output);;
            unsigned int product = mate_rr_output_info_get_product(output);
            unsigned int serial = mate_rr_output_info_get_serial(output);
            qDebug()<<"the product and serial is------->"<<name<<" "<<product<<" "<<serial<<endl;
            if (monitorName == QString(QLatin1String(name))) {
               modelDec = product;
               serialDec = serial;
            }
        }
    }

    for (GList *l = justTurnedOn; l; l = l->next) {
        MateRROutputInfo *output;
        i = GPOINTER_TO_INT (l->data);
        output = outputs[i];
        if (mate_rr_output_info_is_active(output)) {
            g_assert (mate_rr_output_info_is_connected (output));
            char *name = mate_rr_output_info_get_name(output);
            unsigned int product = mate_rr_output_info_get_product(output);
            unsigned int serial = mate_rr_output_info_get_serial(output);
            qDebug()<<"the product and serial is------->"<<name<<" "<<product<<" "<<serial<<endl;
            if (monitorName == QString(QLatin1String(name))) {
               modelDec = product;
               serialDec = serial;
            }
        }
    }

    xml->productName = "0x"+QString("%1").arg(modelDec,4,16,QLatin1Char('0'));
    xml->serialNum = "0x"+QString("%1").arg(serialDec,4,16,QLatin1Char('0'));

    g_list_free (justTurnedOn);
    g_object_unref (config);
}

void Widget::setNightMode(const bool nightMode){
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
}


void Widget::updateNightStatus(){
    QProcess *process = new QProcess;

    connect(process, &QProcess::readyRead, this, [=] {
        setIsNightMode(process->readAll().replace("\n","") == "active");

        process->deleteLater();
    });

    process->start("systemctl", QStringList() << "--user" << "is-active" << "redshift.service");
    process->close();
}


void Widget::setIsNightMode(bool isNightMode) {
    if(m_isNightMode == isNightMode){
        return ;
    }
    qDebug()<<"isNightMode----->"<<isNightMode<<endl;
    m_isNightMode = isNightMode;
   // emit nightModeChanged(isNightMode);
}

void Widget::initUiComponent() {
    singleButton = new QButtonGroup();
    singleButton->addButton(ui->sunradioBtn);
    singleButton->addButton(ui->customradioBtn);

    singleButton->setId(ui->sunradioBtn, SUN);
    singleButton->setId(ui->customradioBtn, CUSTOM);


    MODE value = ui->customradioBtn->isChecked() == SUN ? SUN : CUSTOM;
    showNightWidget(nightButton->isChecked());
    if (nightButton->isChecked()) {
        showCustomWiget(value);
    }

    //ubuntukylin youker DBus interface
    QDBusInterface *brightnessInterface = new QDBusInterface("org.freedesktop.UPower",
                                     "/org/freedesktop/UPower/devices/DisplayDevice",
                                     "org.freedesktop.DBus.Properties",
                                     QDBusConnection::systemBus());
    if (!brightnessInterface->isValid()) {
        qDebug() << "Create UPower Interface Failed : " << QDBusConnection::systemBus().lastError();
        return;
    }

    QDBusReply<QVariant> briginfo;
    briginfo  = brightnessInterface ->call("Get", "org.freedesktop.UPower.Device", "PowerSupply");
    if (!briginfo.isValid()) {
        qDebug()<<"brightness info is invalid"<<endl;
        ui->brightnessframe->setVisible(false);
    } else {
        bool status = briginfo.value().toBool();
        ui->brightnessframe->setVisible(status);
    }
}

void Widget::setRedShiftIsValid(bool redshiftIsValid) {
    if(m_redshiftIsValid == redshiftIsValid) {
        return ;
    }

    m_redshiftIsValid = redshiftIsValid;
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
    m_isNightMode = (tmpNight=="active\n" ? true : false);

    if (isRedShiftValid){
        updateNightStatus();
    }
    setRedShiftIsValid(isRedShiftValid);
}
