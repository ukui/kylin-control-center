
extern "C" {
#define MATE_DESKTOP_USE_UNSTABLE_API
#include <libmate-desktop/mate-rr.h>
#include <libmate-desktop/mate-rr-config.h>
#include <libmate-desktop/mate-rr-labeler.h>
#include <libmate-desktop/mate-desktop-utils.h>
}

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
#include <QDir>
#include <QStandardPaths>
#include <QComboBox>
#include <QMessageBox>

#include <QLibrary>
#include <iostream>
#include <cstring>


#include <KF5/KScreen/kscreen/output.h>
#include <KF5/KScreen/kscreen/edid.h>
#include <KF5/KScreen/kscreen/mode.h>
#include <KF5/KScreen/kscreen/config.h>
#include <KF5/KScreen/kscreen/getconfigoperation.h>
#include <KF5/KScreen/kscreen/configmonitor.h>
#include <KF5/KScreen/kscreen/setconfigoperation.h>
#include <KF5/KScreen/kscreen/edid.h>

#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>
#include <X11/Xutil.h>


#include "declarative/qmloutput.h"
#include "declarative/qmlscreen.h"
#include "utils.h"
#include "ui_touchscreen.h"
#include "widget.h"

#include "xinputmanager.h"


#ifdef signals
#undef signals
#endif

#define QML_PATH "kcm_kscreen/qml/"

#define UKUI_CONTORLCENTER_PANEL_SCHEMAS "org.ukui.control-center.panel.plugins"

#define FONT_RENDERING_DPI               "org.ukui.SettingsDaemon.plugins.xsettings"

#define ADVANCED_SCHEMAS                 "org.ukui.session.required-components"
#define ADVANCED_KEY                     "windowmanager"

Q_DECLARE_METATYPE(KScreen::OutputPtr)

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TouchScreen())
{
    qRegisterMetaType<QQuickView*>();
    gdk_init(NULL, NULL);
      
    m_pXinputManager=new XinputManager;

    m_pXinputManager->start();


    ui->setupUi(this);
    ui->touchscreenLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");

#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)
    oriApply = true;
#else
    mOriApply = false;

#endif


    initConnection();

    initui();

    loadQml();

}

void Widget::initui(){

    if (findTouchScreen()){
        qDebug() << "Touch Screen Devices Available";
        ui->tipLabel->hide();
        ui->screenFrame->show();
        ui->touchscreenFrame->show();
        ui->deviceinfoFrame->show();
        ui->mapButton->show();
        ui->CalibrationButton->show();
        //initTouchScreenStatus();
    } else {
        qDebug() << "Touch Screen Devices Unavailable";
        ui->screenFrame->hide();
        ui->touchscreenFrame->hide();
        ui->deviceinfoFrame->hide();
        ui->mapButton->hide();
        ui->CalibrationButton->hide();
        ui->tipLabel->show();
    }
}

void Widget::loadQml() {

}

Widget::~Widget() {
    //clearOutputIdentifiers();
    delete ui;
}

//接收触摸事件
bool Widget::event(QEvent *event)
{

    switch( event->type() )
    {
    case QEvent::TouchBegin:
    {
        QTouchEvent* touch = static_cast<QTouchEvent*>(event);
        QList<QTouchEvent::TouchPoint> touch_list = touch->touchPoints();
        touch_list.at(0).pos().x();
        touch_list.at(0).pos().y();
        event->accept();
        return true;

    }

    case QEvent::TouchUpdate:
    {
        QTouchEvent* touch = static_cast<QTouchEvent*>(event);

        if(touch->touchPointStates() & Qt::TouchPointPressed){
            //判断是否有触摸点处于TouchPointPressed或TouchPointMoved或TouchPointStationary或TouchPointReleased
        }
        event->accept();
        return true;
    }
    case QEvent::TouchEnd:
    {
        //QTouchEvent* touch = static_cast<QTouchEvent*>(event);
        event->accept();
        return true;
    }
    default:break;

    }
    return QWidget::event(event);
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
    resettouchscreenCombo();
    connect(mConfig.data(), &KScreen::Config::outputAdded,
            this, &Widget::outputAdded);
    connect(mConfig.data(), &KScreen::Config::outputRemoved,
            this, &Widget::outputRemoved);
    for (const KScreen::OutputPtr &output : mConfig->outputs()) {
        outputAdded(output);
    }
}

KScreen::ConfigPtr Widget::currentConfig() const {
    return mConfig;
}


void Widget::resetPrimaryCombo() {
#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)

#else
    bool isPrimaryDisplaySupported = mConfig->supportedFeatures().testFlag(KScreen::Config::Feature::PrimaryDisplay);
    ui->monitorLabel->setVisible(isPrimaryDisplaySupported);
    ui->monitorCombo->setVisible(isPrimaryDisplaySupported);
#endif
    // Don't emit currentIndexChanged when resetting
    bool blocked = ui->monitorCombo->blockSignals(true);
    ui->monitorCombo->clear();
    ui->monitorCombo->blockSignals(blocked);

    if (!mConfig) {
        return;
    }

    for (auto &output: mConfig->outputs()) {
        addOutputToMonitorCombo(output);
    }

}

void Widget::resettouchscreenCombo() {
#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)

#else
    ui->touchscreenLabel->setVisible(true);
    ui->touchscreenCombo->setVisible(true);
#endif

    // Don't emit currentIndexChanged when resetting
    bool blocked = ui->touchscreenCombo->blockSignals(true);
    ui->touchscreenCombo->clear();
    ui->touchscreenCombo->blockSignals(blocked);

    findTouchScreen();

}

void Widget::addOutputToMonitorCombo(const KScreen::OutputPtr &output) {
    // 注释后让他显示全部屏幕下拉框
    if (!output->isConnected()) {
        return;
    }

    ui->monitorCombo->addItem(Utils::outputName(output), output->id());
    if (output->isPrimary()) {
        Q_ASSERT(mConfig);
        int lastIndex = ui->monitorCombo->count() - 1;
        ui->monitorCombo->setCurrentIndex(lastIndex);
    }
}

//这里从屏幕点击来读取输出
void Widget::slotFocusedOutputChanged(QMLOutput *output) {

    //读取屏幕点击选择下拉框
    Q_ASSERT(mConfig);
    int index = output->outputPtr().isNull() ? 0 : ui->monitorCombo->findData(output->outputPtr()->id());
    if (index == -1 || index == ui->monitorCombo->currentIndex()) {
        return;
    }
    ui->monitorCombo->setCurrentIndex(index);
}

void Widget::slotOutputConnectedChanged() {
    resetPrimaryCombo();
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

void Widget::outputAdded(const KScreen::OutputPtr &output) {
    connect(output.data(), &KScreen::Output::isConnectedChanged,
            this, &Widget::slotOutputConnectedChanged);

    //addOutputToMonitorCombo(output);
}

void Widget::outputRemoved(int outputId) {
    KScreen::OutputPtr output = mConfig->output(outputId);
    if (!output.isNull()) {
        output->disconnect(this);
    }

    const int index = ui->monitorCombo->findData(outputId);
    if (index == -1) {
        return;
    }

    if (index == ui->monitorCombo->currentIndex()) {
        // We'll get the actual primary update signal eventually
        // Don't emit currentIndexChanged
        const bool blocked = ui->monitorCombo->blockSignals(true);
        ui->monitorCombo->setCurrentIndex(0);
        ui->monitorCombo->blockSignals(blocked);
    }
    ui->monitorCombo->removeItem(index);
}

void Widget::touchscreenAdded() {
    initui();
    resettouchscreenCombo();
}

void Widget::touchscreenRemoved() {
    initui();
    resettouchscreenCombo();

}

void Widget::primaryOutputSelected(int index) {
    if (!mConfig) {
        return;
    }

    const KScreen::OutputPtr newPrimary = index == 0 ? KScreen::OutputPtr() : mConfig->output(ui->monitorCombo->itemData(index).toInt());
    if (newPrimary == mConfig->primaryOutput()) {
        return;
    }

    mConfig->setPrimaryOutput(newPrimary);
    Q_EMIT changed();
}

void Widget::initConnection() {

    connect(ui->monitorCombo, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &Widget::curOutoutChanged);

    connect(ui->touchscreenCombo, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &Widget::curTouchScreenChanged);

    // TODO: Find out why adjusting the screen orientation does not take effect
    connect(ui->mapButton, &QPushButton::clicked, this, [=]() {
        maptooutput();
    });
    connect(ui->CalibrationButton, &QPushButton::clicked, this, [=]() {
        CalibratTouch();
    });

    connect(m_pXinputManager, &XinputManager::xinputSlaveAdded, this, &Widget::touchscreenAdded);
    connect(m_pXinputManager, &XinputManager::xinputSlaveRemoved, this, &Widget::touchscreenRemoved);

}

void Widget::curOutoutChanged(int index)
{
    const KScreen::OutputPtr &output=mConfig->output(ui->monitorCombo->itemData(index).toInt());
    CurMonitorName = output.data()->name();
}

void Widget::curTouchScreenChanged(int index)
{
    int CurDevicesId;
    QString CurDevicesName="";
    CurTouchScreenName= ui->touchscreenCombo->itemText(ui->touchscreenCombo->currentIndex());
    CurDevicesId=ui->touchscreenCombo->itemText(ui->touchscreenCombo->currentIndex()).toInt();
    CurDevicesName=findTouchScreenName(CurDevicesId);
    ui->touchnameContent->setText(CurDevicesName);
}

void Widget::maptooutput() {

    Display *dpy=XOpenDisplay(NULL);

    QLibrary lib("/usr/lib/libkysset.so");

    std::string touchstr = CurTouchScreenName.toStdString();
    std::string monitorstr = CurMonitorName.toStdString();
    const char* _CurTouchScreenName = touchstr.c_str();
    const char* _CurMonitorName = monitorstr.c_str();

    if(lib.load()){

        typedef int(*MapToOutput)(Display *,const char *,const char *);
        MapToOutput _maptooutput=(MapToOutput)lib.resolve("MapToOutput");

        if(!_maptooutput){
            qDebug("maptooutput resolve failed!\n");
        }else{
            int ret=_maptooutput(dpy,_CurTouchScreenName,_CurMonitorName);
            if(ret!=0){
                qDebug("MapToOutput exe failed ! ret=%d\n",ret);
            }
        }

        lib.unload();

    }else{
        qDebug("/usr/lib/libkysset.so not found!\n");
    }

    XCloseDisplay(dpy);
}

//todo:完善触摸校准代码
void Widget::CalibratTouch() {

    QDBusMessage msg =QDBusMessage::createSignal("/com/control/center/calibrator",  "com.control.center.calibrator.interface", "calibratorEvent");
    msg<<CurTouchScreenName;
    QDBusConnection::systemBus().send(msg);
}


void Widget::addTouchScreenToTouchCombo(const QString touchscreenname ){

    ui->touchscreenCombo->addItem(touchscreenname);
}

bool Widget::findTouchScreen(){

    int  ndevices = 0;
    bool retval=false;
    Display *dpy = XOpenDisplay(NULL);
    XIDeviceInfo *info = XIQueryDevice(dpy, XIAllDevices, &ndevices);
    QString devicesid="";

    for (int i = 0; i < ndevices; i++)
    {
        XIDeviceInfo* dev = &info[i];
        // 判断当前设备是不是触摸屏
        if(dev->use != XISlavePointer) continue;
        if(!dev->enabled) continue;
        for (int j = 0; j < dev->num_classes; j++)
        {
            if (dev->classes[j]->type == XITouchClass)
            {
                devicesid = tr("%1").arg(dev->deviceid);
                addTouchScreenToTouchCombo(devicesid);
                retval = true;
            }
        }
    }

    XIFreeDeviceInfo(info);
    XCloseDisplay(dpy);

    return retval;
}

QString Widget::findTouchScreenName(int devicesid){

    int  ndevices = 0;
    Display *dpy = XOpenDisplay(NULL);
    XIDeviceInfo *info = XIQueryDevice(dpy, XIAllDevices, &ndevices);
    QString devicesname="";

    for (int i = 0; i < ndevices; i++)
    {
        XIDeviceInfo* dev = &info[i];
        // 判断当前设备是不是触摸屏
        if(dev->use != XISlavePointer) continue;
        if(!dev->enabled) continue;
        for (int j = 0; j < dev->num_classes; j++)
        {
            if (dev->classes[j]->type == XITouchClass)
            {
                if(dev->deviceid==devicesid)
                {
                    devicesname=dev->name;
                    return devicesname;
                }
            }
        }
    }
}
