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

#include "touchserialquery.h"
#include "xinputmanager.h"


#ifdef signals
#undef signals
#endif

#define QML_PATH "kcm_kscreen/qml/"

#define UKUI_CONTORLCENTER_PANEL_SCHEMAS "org.ukui.control-center.panel.plugins"

#define FONT_RENDERING_DPI               "org.ukui.SettingsDaemon.plugins.xsettings"

#define ADVANCED_SCHEMAS                 "org.ukui.session.required-components"
#define ADVANCED_KEY                     "windowmanager"
#define TOUCHSCREEN_CFG_PATH             "/.config/touchcfg.ini"

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
    CurTouchScreenName= ui->touchscreenCombo->itemText(ui->touchscreenCombo->currentIndex());
    CurDevicesId=ui->touchscreenCombo->itemText(ui->touchscreenCombo->currentIndex()).toInt();
    CurDevicesName=findTouchScreenName(CurDevicesId);
    ui->touchnameContent->setText(CurDevicesName);
}

//触摸映射
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
            if(!ret){

                save(CurDevicesName,CurTouchScreenName,CurMonitorName); //保存映射关系

            }else{
                qDebug("MapToOutput exe failed ! ret=%d\n",ret);
            }
        }

        lib.unload();

    }else{
        qDebug("/usr/lib/libkysset.so not found!\n");
    }

    XCloseDisplay(dpy);

}

/*触摸校准
 * 通过dbus信号与kylin-xinput-calibration应用交互
 * 发送触摸校准事件并传递相关参数
*/
void Widget::CalibratTouch() {

    QDBusMessage msg =QDBusMessage::createSignal("/com/control/center/calibrator",  "com.control.center.calibrator.interface", "calibratorEvent");
    msg<<(CurTouchScreenName+","+CurMonitorName);
    QDBusConnection::systemBus().send(msg);
}


void Widget::addTouchScreenToTouchCombo(const QString touchscreenname ){

    ui->touchscreenCombo->addItem(touchscreenname);
}

//识别触摸屏设备
bool Widget::findTouchScreen(){

    int  ndevices = 0;
    bool retval=false;
    CurTouchscreenNum=0;
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
                CurTouchscreenNum++;
            }
        }
    }

    XIFreeDeviceInfo(info);
    XCloseDisplay(dpy);

    return retval;
}

//获取触摸屏名称
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
    return devicesname;
}

/*
  *判断映射关系保存时，屏幕是否已更换，不同屏幕通过touch serial区分
  *通过配置中保存的touch name及touch id获取对应touch serial
  *然后用该touch serial与配置文件中的touch serial作比较，如果相同则触摸屏设备没有更换
  *否则清空配置文件重新记录
*/
int Widget::compareserial(int touchcount){

    for(int i=1;i<=touchcount;i++)
    {
        QString str = QString::number(i);
        QString mapoption = "MAP"+str;
        QString serial = mapoption+"/serial";
        QString name = mapoption+"/name";
        QString id = mapoption+"/id";
        QString touchname = configIni->value(name).toString();
        QString touchserial = configIni->value(serial).toString();
        if( (touchname == "") && (touchserial == "") )
            continue;
        int touchid = configIni->value(id).toInt();
        char _touchserial[32]={0};
        std::string namestr = touchname.toStdString();
        char * _touchname=(char *)namestr.c_str();
        findSerialFromId(touchid,_touchname,_touchserial,32);
        //qDebug("_touchserial=%s\n",_touchserial);
        QString Qtouchserial(_touchserial);
        //qDebug("Qtouchserial=%s\n",Qtouchserial.toStdString().data());
        //qDebug("touchserial=%s\n",touchserial.toStdString().data());
        if(Qtouchserial!=touchserial){
            return -1;
        }
    }

    return Success;
}

/*
 *比较配置文件中同一触摸屏与显示器的映射关系
 *如不同则保存最新的映射关系
*/
int Widget::comparescreenname(QString _touchserial,QString _screenname){

    int touchcount=configIni->value("COUNT/num").toInt();

    for(int i=1;i<=touchcount;i++)
    {
        QString str = QString::number(i);
        QString mapoption = "MAP"+str;
        QString serial = mapoption+"/serial";
        QString scrname = mapoption+"/scrname";
        QString screenname = configIni->value(scrname).toString();
        QString touchserial = configIni->value(serial).toString();

        //qDebug("Qtouchserial=%s\n",screenname.toStdString().data());
        //qDebug("touchserial=%s\n",touchserial.toStdString().data());
        if(_touchserial==touchserial){
            if(screenname!=_screenname){
                configIni->remove(mapoption);
            }
        }
    }

    return Success;
}

//清空配置文件
void Widget::cleanTouchConfig(int touchcount){

    configIni->setValue("COUNT/num",0);
    for(int i=1;i<=touchcount;i++)
    {
        QString str = QString::number(i);
        QString mapoption = "MAP"+str;
        configIni->remove(mapoption);
    }
}

//对配置文件进行预处理
void Widget::initTouchConfig(QString touchserial,QString screenname) {
    qdir = new QDir;
    QString homepath = qdir->homePath();
    QString touchcfgpath = homepath + TOUCHSCREEN_CFG_PATH; //触摸屏映射关系配置文件路径
    configIni = new QSettings(touchcfgpath, QSettings::IniFormat);

    int touchcount = configIni->value("COUNT/num").toInt();
    int devicecount = configIni->value("COUNT/device_num").toInt();

    if(!touchcount)
        return ;

    if(devicecount != CurTouchscreenNum)
        cleanTouchConfig(touchcount);

    if(compareserial(touchcount)!=0){

        cleanTouchConfig(touchcount);
        qDebug("compareserial cleanTouchConfig\n");
    }
    comparescreenname(touchserial,screenname);
}

/*
 *判断配置文件中是否已有该触摸屏配置
 * 并返回相应状态
*/
bool Widget::Configserialisexit(QString touchserial){

    bool devicesisexit=0;
    int touchcount=configIni->value("COUNT/num").toInt();

    for(int i=0;i<=touchcount;i++){

        QString numstr = QString::number(i);
        QString mapoption = "MAP"+numstr;
        QString serial = mapoption+"/serial";
        QString _touchserial = configIni->value(serial).toString();
        if(_touchserial == touchserial){
            devicesisexit=1;
            break;
        }

    }
    if(devicesisexit)
        return TRUE;
    else
        return FALSE;

}

//写入配置文件，保存触摸映射关系
void Widget::writeTouchConfig(QString touchname,QString touchid,QString touchserial,QString screenname) {

    int touchcount = configIni->value("COUNT/num").toInt();
    bool devicesisexit = Configserialisexit(touchserial);
    if(devicesisexit && touchcount)      //如果配置文件中已存在该触摸屏配置，则不重复写入
        return;

    QString str = QString::number(touchcount+1);
    QString mapoption = "MAP"+str;
    QString serial = mapoption+"/serial";
    QString name = mapoption+"/name";
    QString id = mapoption+"/id";
    QString scrname = mapoption+"/scrname";

    configIni->setValue( "COUNT/num" ,touchcount+1);
    configIni->setValue( "COUNT/device_num" ,CurTouchscreenNum);
    configIni->setValue( name ,touchname);
    configIni->setValue( id ,touchid);
    configIni->setValue( serial ,touchserial);
    configIni->setValue( scrname ,screenname);

}

/*保存触摸映射关系
 *对保存过程中的各种异常情况做处理
 *如避免重复保存、更换屏幕后删除原映射关系、多屏情况下各屏映射关系保存
 */
void Widget::save(QString touchname,QString touchid,QString screenname) {

    char _touchserial[32]={0};
    std::string str = touchname.toStdString();
    char * _touchname=(char *)str.c_str();
    findSerialFromId(touchid.toInt(),_touchname,_touchserial,32);

    QString touchserial(_touchserial);
    initTouchConfig(touchserial,screenname); //保存之前先对配置文集进行处理
    writeTouchConfig(touchname,touchid,touchserial,screenname);//将触摸映射关系写入配置文件
}
