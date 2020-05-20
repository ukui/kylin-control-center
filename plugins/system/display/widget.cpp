#include "widget.h"
#include "controlpanel.h"
#include "declarative/qmloutput.h"
#include "declarative/qmlscreen.h"
#include "utils.h"
#include "ui_display.h"

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
#include <QStyledItemDelegate>

#include <KPluginFactory>
#include <KAboutData>
#include <KMessageBox>
#include <QStyledItemDelegate>
//#include <KLocalizedString>
#include <KF5/KScreen/kscreen/output.h>
#include <KF5/KScreen/kscreen/edid.h>
#include <KF5/KScreen/kscreen/mode.h>
#include <KF5/KScreen/kscreen/config.h>
#include <KF5/KScreen/kscreen/getconfigoperation.h>
#include <KF5/KScreen/kscreen/configmonitor.h>
#include <KF5/KScreen/kscreen/setconfigoperation.h>
#include <KF5/KScreen/kscreen/edid.h>


#define QML_PATH "kcm_kscreen/qml/"
#define UKUI_CONTORLCENTER_PANEL_SCHEMAS "org.ukui.control-center.panel.plugins"

#define NIGHT_MODE_KEY "nightmodestatus"
#define SCRENN_SCALE_SCHMES "org.ukui.session"
#define GDK_SCALE_KEY "gdk-scale"
#define QT_SCALE_KEY "qt-scale-factor"
#define USER_SACLE_KEY "hidpi"

#define POWER_SCHMES "org.ukui.power-manager"
#define POWER_KEY "brightness-ac"

Q_DECLARE_METATYPE(KScreen::OutputPtr)


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DisplayWindow()), slider(new Slider())
{
    qRegisterMetaType<QQuickView*>();
    itemDelege= new QStyledItemDelegate(this);

    ui->setupUi(this);
    ui->quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);

#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)

#else
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
    nightLabel = new QLabel(tr("night mode"));
    nightButton = new SwitchButton;
    nightLayout->addWidget(nightLabel);
    nightLayout->addStretch();
    nightLayout->addWidget(nightButton);


    initUiQss();
    initTemptSlider();
    initConfigFile();
    initUiComponent();
    initNightStatus();

//    nightButton->setVisible(this->m_redshiftIsValid);
      ui->nightframe->setVisible(this->m_redshiftIsValid);

//    qDebug()<<"set night mode here ---->"<<this->m_isNightMode<<endl;
    nightButton->setChecked(this->m_isNightMode);


//    connect(this,&Widget::nightModeChanged,nightButton,&SwitchButton::setChecked);
//    connect(this,&Widget::redShiftValidChanged,nightButton,&SwitchButton::setVisible);
    connect(nightButton,SIGNAL(checkedChanged(bool)),this,SLOT(showNightWidget(bool)));
    connect(singleButton, SIGNAL(buttonClicked(int)), this, SLOT(showCustomWiget(int)));


    //这里是设置主显示器(已经废弃重写了)
//    connect(ui->primaryCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
//            this, &Widget::primaryOutputSelected);


    //是否禁用主显示器确认按钮
    connect(ui->primaryCombo, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &Widget::mainScreenButtonSelect);


    //主屏确认按钮
    connect(ui->mainScreenButton, SIGNAL(clicked()),
            this, SLOT(primaryButtonEnable()));


    mControlPanel = new ControlPanel(this);
    connect(mControlPanel, &ControlPanel::changed,
            this, &Widget::changed);

    connect(mControlPanel, &ControlPanel::scaleChanged,
            this, &Widget::scaleChangedSlot);

    //ui->formLayout_2->addWidget(mControlPanel);
    ui->controlPanelLayout->addWidget(mControlPanel);


    //ui->controlPanelLayout->setContentsMargins(0,0,60,10);


    connect(ui->applyButton,SIGNAL(clicked()),this,SLOT(save()));
//    connect(ui->applyButton,SIGNAL(clicked()),this,SLOT(saveBrigthnessConfig()));



    //统一输出按钮
//    connect(ui->unifyButton, &QPushButton::released,
//            [this]{
//                slotUnifyOutputs();
//            });
    connect(m_unifybutton,&SwitchButton::checkedChanged,
            [this]{
//                  if(checked)
                    slotUnifyOutputs();
            });


//    connect(ui->checkBox, &QCheckBox::clicked,
//            this, [=](bool checked) {
//                checkOutputScreen(checked);
//            });

    //TODO----->bug
//    ui->showMonitorwidget->setVisible(false);
    connect(closeScreenButton,&SwitchButton::checkedChanged,
            this,[=](bool checked){
                checkOutputScreen(checked);
        });

    //缩放按钮注释
//    connect(ui->scaleAllOutputsButton, &QPushButton::released,
//            [this] {
//                QPointer<ScalingConfig> dialog = new ScalingConfig(mConfig->outputs(), this);
//                dialog->exec();
//                delete dialog;
//            });

    mOutputTimer = new QTimer(this);
    connect(mOutputTimer, &QTimer::timeout,
            this, &Widget::clearOutputIdentifiers);

    initGSettings();
    loadQml();
    setBrigthnessFile();
    //亮度调节UI
//    initBrightnessUI();


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

    //上面屏幕拿取配置
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

    //这里的qml路径还需要更改
    //auto tmpfile = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);

//    QString tmpfile = QCoreApplication::applicationDirPath();

//    qDebug()<<"路径----------------->";
//    const QString file = QStringLiteral("/home/kylin/zhoubin/ukui/ukui-control-center/plugins/system/display/qml/main.qml");

    ui->quickWidget->setSource(QUrl("qrc:/qml/main.qml"));

    QQuickItem* rootObject = ui->quickWidget->rootObject();
    mScreen = rootObject->findChild<QMLScreen*>(QStringLiteral("outputView"));
    if (!mScreen) {
        return;
    }
    connect(mScreen, &QMLScreen::focusedOutputChanged,
            this, &Widget::slotFocusedOutputChanged);

    //识别按钮注释
//    connect(rootObject->findChild<QObject*>(QStringLiteral("identifyButton")), SIGNAL(clicked()),
//            this, SLOT(slotIdentifyButtonClicked()));
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
       //注释后让他显示全部屏幕下拉框
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
    //这里为点击禁用屏幕输出后的改变
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

float Widget::scaleRet() {
    QString filepath = getenv("HOME");
    QString scale;
    filepath += "/.profile";
    QStringList res = this->readFile(filepath);
    QRegExp re("export( GDK_SCALE)?=(.*)$");
    for(int i = 0; i < res.length(); i++) {
        int pos = 0;
//        qDebug()<<res.at(i)<<endl;
        QString str = res.at(i);
        while ((pos = re.indexIn(str, pos)) != -1) {
            scale = re.cap(2);
            pos += re.matchedLength();
        }
    }
    return scale.toFloat();
}

void Widget::writeScale(float scale) {
    QString strGDK = "export GDK_SCALE=";
    QString strQT  = "export QT_SCALE_FACTOR=";
    QString strAutoQT  = "export QT_AUTO_SCREEN_SET_FACTOR=0";
    bool judge = false;
    QString filepath = getenv("HOME");
    filepath += "/.profile";
    for(int i = 0; i < proRes.length(); i++) {
        QString tmpstr =  proRes.at(i);
        if(tmpstr.contains(strGDK)) {
            proRes[i] = strGDK + QString::number(scale);
            judge = true;
        }
        if(tmpstr.contains(strQT)) {
            proRes[i] = strQT + QString::number(scale);
        }
    }
    if(!judge) {
        this->proRes.append(strGDK + QString::number(scale));
        this->proRes.append(strQT + QString::number(scale));
        this->proRes.append(strAutoQT);
    }

    writeFile(filepath, this->proRes);
    setSessionScale(static_cast<int>(scale));
}


void Widget::initGSettings() {
    QByteArray id(UKUI_CONTORLCENTER_PANEL_SCHEMAS);
    if(QGSettings::isSchemaInstalled(id)) {
//        qDebug()<<"initGSettings-------------------->"<<endl;
        m_gsettings = new QGSettings(id)        ;
    } else {
        return ;
    }

    QByteArray scaleId(SCRENN_SCALE_SCHMES);
    if(QGSettings::isSchemaInstalled(scaleId)) {
//        qDebug()<<"initGSettings-------------------->"<<endl;
        scaleGSettings = new QGSettings(scaleId)        ;
    } else {
        qDebug()<<"org.ukui.session schemas not installed"<<endl;
        return ;
    }
}

bool Widget::getNightModeGSetting(const QString &key) {
    if (!m_gsettings) {
        return "";
    }
    const QStringList list = m_gsettings->keys();
    if (!list.contains(key)) {
        return "";
    }
    bool res = m_gsettings->get(key).toBool();
    return res;
}

void Widget::setNightModebyPanel(bool judge) {
//    QProcess *process = new QProcess;
//    if(judge == true) {
//        process->startDetached("redshift -t 5700:3600 -g 0.8 -m randr -v");
//    }  else {
//        QProcess::execute("killall redshift");
//    }
}

void Widget::setSessionScale(int scale) {

    if (!scaleGSettings) {
        return;
    }    
    QStringList keys = scaleGSettings->keys();   
    if (keys.contains("hidpi")){
        scaleGSettings->set(USER_SACLE_KEY, true);
    }
    if (keys.contains("gdkScale")){

        scaleGSettings->set(GDK_SCALE_KEY, scale);
    }
    if (keys.contains("qtScaleFactor")) {
        scaleGSettings->set(QT_SCALE_KEY, scale);
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

void Widget::primaryOutputSelected(int index)
{
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
    //readScreenXml();
    //qDebug()<<"save------------>"<<endl;
    if (!this) {
        return;
    }


    const KScreen::ConfigPtr &config = this->currentConfig();

    const int countOutput = config->connectedOutputs().count();


    bool atLeastOneEnabledOutput = false;
    int i = 0;
    int connectedScreen = 0;
    Q_FOREACH(const KScreen::OutputPtr &output, config->outputs()) {
        KScreen::ModePtr mode = output->currentMode();
        if (output->isEnabled()) {
//            qDebug()<<"atLeastOneEnabledOutput------------>"<<endl;
            atLeastOneEnabledOutput = true;
            connectedScreen++;
        }
        if (!output->isConnected())
            continue;

        QMLOutput *base = mScreen->primaryOutput();
//        qDebug()<<"primaryOutput---->"<<base<<endl;

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
//        qDebug()<<" clone mode--------->"<<endl;
        inputXml[i].isClone = base->isCloneMode() == true?"true":"false";
        inputXml[i].outputName = output->name();

        inputXml[i].widthValue = QString::number(mode->size().width());
        inputXml[i].heightValue = QString::number(mode->size().height());
        inputXml[i].rateValue = QString::number(mode->refreshRate());
        inputXml[i].posxValue = QString::number(output->pos().x());
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

    int scale = static_cast<int>(this->scaleRet());
    initScreenXml(countOutput);
    writeScreenXml(countOutput);
    writeScale(static_cast<float>(this->screenScale));
    writeConfigFile();
    setNightMode(nightButton->isChecked());


    if (!KScreen::Config::canBeApplied(config)) {
        KMessageBox::information(this,
            tr("Sorry, your configuration could not be applied.\nCommon reasons are that the overall screen size is too big, or you enabled more displays than supported by your GPU."),
                 tr("@title:window", "Unsupported Configuration"));
        return;
    }


//    qDebug()<<"scale ann screenScale is -------->"<<this->scaleRet()<<" "<<this->screenScale<<endl;

    if (scale != this->screenScale) {
        KMessageBox::information(this,tr("Some applications need to be restarted to take effect"));
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
    //设置是否勾选
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
    ui->mainScreenButton->setEnabled(false);
    const KScreen::OutputPtr newPrimary = mConfig->output(ui->primaryCombo->itemData(index).toInt());
   // qDebug()<<"按下主屏按钮---->"<<newPrimary<<"index ----"<<index<<endl;
    mConfig->setPrimaryOutput(newPrimary);

    Q_EMIT changed();
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

QStringList Widget::getscreenBrightnesName(){
    QByteArray ba;
    FILE * fp = NULL;
    char cmd[1024];
    char buf[1024];

    sprintf(cmd, "xrandr | grep \" connected\"  | awk '{ print$1 }'");
    if ((fp = popen(cmd, "r")) != NULL){
        rewind(fp);
        while(!feof(fp)){
            fgets(buf, sizeof (buf), fp);
            ba.append(buf);
        }
        pclose(fp);
        fp = NULL;

    }else{
        qDebug()<<"popen文件打开失败"<<endl;
    }
    QString str =  QString(ba);
//    qDebug()<<"strlist------>"<<str<<endl;
    QStringList strlist = str.split("\n");

    return strlist;
}


QStringList Widget::getscreenBrightnesValue(){
    QByteArray ba;
    FILE * fp = NULL;
    char cmd[1024];
    char buf[1024];
//    const char * cmdstr = ;


    sprintf(cmd, "xrandr --verbose | grep Brightness |cut -f2 -d :");
    if ((fp = popen(cmd, "r")) != NULL){
        rewind(fp);
        while(!feof(fp)){
            fgets(buf, sizeof (buf), fp);
            ba.append(buf);
        }
        pclose(fp);
        fp = NULL;
    }else{
        qDebug()<<"popen文件打开失败"<<endl;
    }
    QString str =  QString(ba);
//    qDebug()<<"strlist  value------>"<<str<<endl;
    str = str.mid(1,str.length())+" ";
    QStringList strlist = str.split("\n ");


    return strlist;
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
//    QStringList nameList = getscreenBrightnesName();
//    QString sliderValue = QString::number(ui->brightnessSlider->value()/100.0);

//    QString screenName =  getScreenName();

//    float value = index/100.0 >0.2?index/100.0:0.2;
//    QString brightnessValue = QString::number(value);

//    QProcess *process = new QProcess;
//    QMLOutput *base = mScreen->primaryOutput();
//    //qDebug()<<"primaryOutput---->"<<base<<endl;
//    if (!base) {

//        for (QMLOutput *output: mScreen->outputs()) {
//            if (output->output()->isConnected() && output->output()->isEnabled()) {
//                base = output;
//                break;
//            }
//        }

//        if (!base) {
//            // WTF?
//            return;
//        }
//    }
//    if(base->isCloneMode() == false) {
//        process->start("xrandr",QStringList()<<"--output"<<screenName<<"--brightness"<< brightnessValue);
//        process->waitForFinished();
//        const QString &cmd = "xrandr --output "+ screenName+" --brightness "+ brightnessValue;
//    } else {
//        for(int i = 0; i < nameList.length(); i++ ){
//            if(nameList.at(i) != ""){
//                process->start("xrandr",QStringList()<<"--output"<<nameList.at(i)<<"--brightness"<< sliderValue);
//                process->waitForFinished();
//            }
//        }
//    }
}


//保存屏幕亮度配置
void Widget::saveBrigthnessConfig(){

    QStringList cmdList;
    QStringList nameList = getscreenBrightnesName();
    QStringList valueList = getscreenBrightnesValue();
    QString sliderValue = QString::number(ui->brightnessSlider->value()/100.0);
    int len = std::min(nameList.length(),valueList.length());

//    qDebug()<<"QStringList------------------>"<<nameList<<" "<<valueList<<endl;
    for(int i = 0;i < len;i++){
        //qDebug()<<"亮度值---》"<<valueList.at(i)<<endl;
        if("" == nameList.at(i) || "" == valueList.at(i)){
            continue;
        }
        //非统一输出模式
        QString tmpcmd = nullptr;
        if(inputXml->isClone == "false"){
            tmpcmd = "xrandr --output "+ nameList.at(i)+" --brightness "+ valueList.at(i);
        } else {
            tmpcmd = "xrandr --output "+ nameList.at(i)+" --brightness "+ sliderValue;
        }

        cmdList.append(tmpcmd);
    }

    QFile fp(brightnessFile);
    if(!fp.open(QIODevice::WriteOnly)){
        qDebug()<<"写入文件失败"<<endl;
        return ;
    }
    QTextStream cmdOuput(&fp);
    for(int i=0;i<cmdList.length();i++){
        cmdOuput<<cmdList.at(i)<<endl;
    }
    fp.close();
}

//滑块改变
void Widget::setBrightnesSldierValue(QString name){
//   // qDebug()<<"setBrightnesSldierValue---->"<<endl;
//    QString screename = getScreenName(name);
//    QStringList nameList = getscreenBrightnesName();
//    QStringList valueList = getscreenBrightnesValue();
////    qDebug()<<"nameList and valueList is--------->"<<nameList<<" \n"<<valueList<<endl;
//    int len = std::min(nameList.length(),valueList.length()) -1;
//    QMap<QString,float> brightnessMap;

//    for(int i = 0;i < len;i++){
//        brightnessMap.insert(nameList.at(i).trimmed(),valueList.at(i).toFloat());
//    }

//    ui->brightnessSlider->setValue(brightnessMap[screename]*100);

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

//亮度配置文件位置
void Widget::setBrigthnessFile(){
    brightnessFile = getenv("HOME");
    brightnessFile += "/.xprofile";
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

    if ("" != optime){
        QString ophour = optime.split(":").at(0);
        QString opmin = optime.split(":").at(1);

//        qDebug()<<"optime is----->"<<ophour.toInt()<<" "<<opmin.toInt()<<endl;

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



void Widget::getEdidInfo(QString monitorName,xmlFile *xml){
    int index = monitorName.indexOf('-');
    monitorName = monitorName.mid(0,index);\

    QString cmdGrep  = "ls /sys/class/drm/ | grep " +monitorName;
    QByteArray tmpBa = cmdGrep.toLatin1();
    const char *cmdfile =tmpBa.data();

    QByteArray ba;
    FILE * fp = NULL;
    char cmd[128];
    char buf[1024];

    sprintf(cmd, "%s", cmdfile);
    if ((fp = popen(cmd, "r")) != NULL){
        fgets(buf, sizeof (buf), fp);
        ba.append(buf);
        pclose(fp);
        fp = NULL;
    }else{
        qDebug()<<"popen文件打开失败"<<endl;
    }
    QString fileName = QString(ba);
    fileName = fileName.mid(0,fileName.length()-1);

//    qDebug()<<"file name is----------------------->"<<fileName<<endl;

    QString edidPath = "cat /sys/class/drm/"+fileName+"/edid | edid-decode | grep Manufacturer";
    QByteArray tmpEdit = edidPath.toLatin1();
    const char *runCmd = tmpEdit.data();

    QByteArray edidBa;
    FILE * fpEdid = NULL;
    char cmdEdid[128];
    char bufEdid[1024];
    sprintf(cmdEdid, "%s", runCmd);
    if ((fpEdid = popen(cmdEdid, "r")) != NULL){
        fgets(bufEdid, sizeof (bufEdid), fpEdid);
        edidBa.append(bufEdid);
        pclose(fpEdid);
        fpEdid = NULL;
    }else{
        qDebug()<<"popen文件打开失败"<<endl;
    }

    QString res = QString(edidBa);
    res = res.mid(0,res.length()-1);

    int modelIndex = res.indexOf("Model");
    int serialIndex =  res.indexOf("Serial Number");
    QString modelStr= res.mid(modelIndex+6,serialIndex-modelIndex-7);
    int modelDec = modelStr.toInt();
    xml->productName = "0x"+QString("%1").arg(modelDec,4,16,QLatin1Char('0'));


    QString serialStr = res.mid(serialIndex+14,res.length()-serialIndex-14);
    int serialDec = serialStr.toInt();
    xml->serialNum = "0x"+QString("%1").arg(serialDec,4,16,QLatin1Char('0'));
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

QStringList Widget::readFile(const QString& filepath) {
    QStringList fileCont;
    QFile file(filepath);
    if(file.exists()) {
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "ReadFile() failed to open" << filepath;
            return QStringList();
        }
        QTextStream textStream(&file);
        while(!textStream.atEnd()) {
            QString line= textStream.readLine();
            line.remove('\n');
            fileCont<<line;
        }
        file.close();
        return fileCont;
    } else {
        qWarning() << filepath << " not found"<<endl;
        return QStringList();
    }
}

void Widget::writeFile(const QString &filepath, const QStringList &content) {
//    qDebug()<<"witeFile--------->"<<endl;
    QFile file(filepath);

    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            qWarning() << "writeFile() failed to open" << filepath;
    }
    QTextStream textStream(&file);
    for(int i = 0; i < content.length(); i++) {

        textStream<<content.at(i);
        textStream<<"\n";
    }
    file.close();
}

void Widget::initUiQss() {

//    ui->mainwidget->setStyleSheet("background-color:#F4F4F4;border-radius:6px");
//    ui->screenwidget->setStyleSheet("background-color:#F4F4F4;border-radius:6px");
//    ui->nightwidget->setStyleSheet("background-color:#F4F4F4;border-radius:6px");
//    ui->unionwidget->setStyleSheet("background-color:#F4F4F4;border-radius:6px");

//    ui->nightwidget->setStyleSheet("background-color:#F4F4F4;border-radius:6px");
//    ui->sunwidget->setStyleSheet("background-color:#F4F4F4;border-radius:6px");
//    ui->customwidget->setStyleSheet("background-color:#F4F4F4;border-radius:6px");
//    ui->opwidget->setStyleSheet("background-color:#F4F4F4;border-radius:6px");
//    ui->clswidget->setStyleSheet("background-color:#F4F4F4;border-radius:6px");
//    ui->temptwidget->setStyleSheet("background-color:#F4F4F4;border-radius:6px");

//    ui->mainScreenButton->setStyleSheet("QPushButton{background-color:#F8F9F9;border-radius:6px;font-size:14px;}"
//                                   "QPushButton:hover{background-color: #3D6BE5;};border-radius:6px");

//    ui->primaryCombo->setStyleSheet("background-color:#F8F9F9");
//    ui->primaryCombo->setMaxVisibleItems(1);

//    ui->showMonitorwidget->setStyleSheet("background-color:#F4F4F4;border-radius:6px");


//    ui->applyButton->setStyleSheet("QPushButton{background-color:#F4F4F4;border-radius:6px}"
//                                   "QPushButton:hover{background-color: #3D6BE5;color:white};border-radius:6px");

//    ui->primaryCombo->setContentsMargins(8,0,0,0);
//    ui->primaryCombo->setItemDelegate(new QStyledItemDelegate());

//    ui->opHourCom->setItemDelegate(new QStyledItemDelegate());
//    ui->opHourCom->setMaxVisibleItems(5);
//    ui->opMinCom->setItemDelegate(new QStyledItemDelegate());
//    ui->opMinCom->setMaxVisibleItems(5);
//    ui->clHourCom->setItemDelegate(new QStyledItemDelegate());
//    ui->clHourCom->setMaxVisibleItems(5);
//    ui->clMinCom->setItemDelegate(new QStyledItemDelegate());
//    ui->clMinCom->setMaxVisibleItems(5);
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

void Widget::setRedShiftIsValid(bool redshiftIsValid){
    if(m_redshiftIsValid == redshiftIsValid) {
        return ;
    }

    m_redshiftIsValid = redshiftIsValid;

    emit redShiftValidChanged(redshiftIsValid);
}


void Widget::initNightStatus(){

    QProcess *process = new QProcess;
    const bool isRedShiftValid  = (0 == process->execute("which",QStringList() << "redshift"));
//    qDebug()<<"isRedshitValid-------------->"<<isRedShiftValid<<endl;


    QProcess *process_2 = new QProcess;
    process_2->start("systemctl", QStringList() << "--user" << "is-active" << "redshift.service");
    process_2->waitForFinished();

    QByteArray qbaOutput = process_2->readAllStandardOutput();

    QString tmpNight = qbaOutput;
    m_isNightMode = (tmpNight=="active\n" ? true : false);
//    qDebug()<<"m_isNightMode is------------->"<<tmpNight<<endl;


    if (isRedShiftValid){
        updateNightStatus();
    }
    setRedShiftIsValid(isRedShiftValid);

}
