 /* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 *
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */
#include "projection.h"
#include "ui_projection.h"

#include <QProcess>
#include <QMessageBox>

#include <QDebug>
#include <QMouseEvent>

#define ITEMFIXEDHEIGH 58
#define THEME_QT_SCHEMA                  "org.ukui.style"
#define MODE_QT_KEY                      "style-name"
Projection::Projection()
{
    pluginName = tr("Projection");
    //~ contents_path /bluetooth/Bluetooth
    pluginType = DEVICES;
    ui = new Ui::Projection;
    pluginWidget = new QWidget;
    pluginWidget->setAttribute(Qt::WA_StyledBackground,true);
    pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(pluginWidget);
    projectionBtn = new SwitchButton(pluginWidget);
    int result = system("checkDaemonRunning.sh");
    if (result != 0) {
        projectionBtn->setChecked(true);
    }
    ui->pinframe->hide();


    connect(projectionBtn, SIGNAL(checkedChanged(bool)), this, SLOT(projectionButtonClickSlots(bool)));
    m_pin = new QLabel(pluginWidget);
    ui->label->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
    //~ contents_path /bluetooth/Open Bluetooth
    ui->titleLabel->setText(tr("Open Projection"));
    ui->titleLabel->setStyleSheet("QLabel{font-size: 14px; color: palette(windowText);}");
    ui->namelabel->setText(tr("Projection Name"));
    ui->namelabel->setStyleSheet("QLabel{font-size: 14px; color: palette(windowText);}");
    ui->pronamelabel->setStyleSheet("QLineEdit{background-color:transparent;"
                                              "border-width:0;"
                                              "border-style:outset}");
    m_pServiceInterface = new QDBusInterface("org.freedesktop.miracleagent",
                                             "/org/freedesktop/miracleagent",
                                             "org.freedesktop.miracleagent.op",
                                             QDBusConnection::sessionBus());

    QString path=QDir::homePath()+"/.config/miracast.ini";
    QSettings *setting=new QSettings(path,QSettings::IniFormat);
    setting->beginGroup("projection");
    bool bo=setting->contains("host");
    qDebug()<<bo<<"bo";
    if (!bo) {
        QDBusInterface *hostInterface = new QDBusInterface("org.freedesktop.hostname1",
                                                          "/org/freedesktop/hostname1",
                                                          "org.freedesktop.hostname1",
                                                          QDBusConnection::systemBus());
        hostName = hostInterface->property("Hostname").value<QString>();
        setting->setValue("host",hostName);
        setting->sync();
        setting->endGroup();
    } else {
        hostName = setting->value("host").toString();
    }
    ui->pronamelabel->setText(hostName);
    ui->pronamelabel->setMaxLength(16);
    ui->pronamelabel->setAlignment(Qt::AlignRight);
    ui->pronamelabel->installEventFilter(this);


    ui->horizontalLayout->addWidget(projectionBtn);
    ui->horizontalLayout_21->addWidget(m_pin);


    initComponent();
}

bool Projection::eventFilter(QObject *watched, QEvent *event){
    if (watched == ui->pronamelabel)
        {
            if (event->type() == QEvent::KeyPress)
            {
                QKeyEvent* keyevt = static_cast<QKeyEvent*>(event);
                if ((keyevt->key() == Qt::Key_Return) ||
                    (keyevt->key() == Qt::Key_Escape) ||
                    (keyevt->key() == Qt::Key_Enter))   // Qt::Key_Return是大键盘的回车键 Qt::Key_Enter是小键盘的回车键
                {
                    QString str_name =ui->pronamelabel->text().remove(QRegExp("\\s"));
                    QString path=QDir::homePath()+"/.config/miracast.ini";
                    QSettings *setting=new QSettings(path,QSettings::IniFormat);
                    setting->beginGroup("projection");

                    if (str_name != NULL) {
                        setting->setValue("host",str_name);
                        setting->sync();
                        setting->endGroup();
                        m_pServiceInterface->call("UiSetName",str_name);
                        ui->pronamelabel->clearFocus();
                    } else {
                        qDebug()<<"回车";
                        enter = true;
                        QMessageBox::information(NULL, QStringLiteral("提示"), QStringLiteral("投屏端名不能为空"));
                        hostName = setting->value("host").toString();                       
                        ui->pronamelabel->setText(hostName);
                        ui->pronamelabel->clearFocus();
                    }
                }
            }
            else if (event->type() == QEvent::FocusOut)
            {
                QString str_name =ui->pronamelabel->text().remove(QRegExp("\\s"));
                QString path=QDir::homePath()+"/.config/miracast.ini";
                QSettings *setting=new QSettings(path,QSettings::IniFormat);
                setting->beginGroup("projection");
                if (str_name != NULL) {
                    setting->setValue("host",str_name);
                    setting->sync();
                    setting->endGroup();
                    m_pServiceInterface->call("UiSetName",str_name);
                    ui->pronamelabel->deselect();
                } else {
                    qDebug()<<"失去焦点";
                    if (enter) {
                        enter = false;
                    } else {
                        QMessageBox::information(NULL, QStringLiteral("提示"), QStringLiteral("投屏端名不能为空"));
                        hostName = setting->value("host").toString();
                        ui->pronamelabel->setText(hostName);
                        ui->pronamelabel->deselect();
                    }
                }
            }
        }
    return QObject::eventFilter(watched, event);
}

void Projection::catchsignal()
{
    while (1)
    {
        m_pServiceInterface = new QDBusInterface("org.freedesktop.miracle.wifi",
                                                 "/org/freedesktop/miracle/wifi/ui",
                                                 "org.freedesktop.miracle.wifi.ui",
                                                 QDBusConnection::systemBus());
        if (m_pServiceInterface->isValid()) {
            connect(m_pServiceInterface,SIGNAL(PinCode(QString, QString)),this,SLOT(projectionPinSlots(QString,QString)));
            return;
        }else {
            qDebug()<<"失败";
            delete m_pServiceInterface;
            delaymsec(1000);
        }
    }
\
}
void Projection::delaymsec(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

Projection::~Projection()
{
    delete ui;
    delete m_pServiceInterface;
}

QString Projection::get_plugin_name(){

    return pluginName;
}

int Projection::get_plugin_type(){
    return pluginType;
}

QWidget *Projection::get_plugin_ui(){
    QDBusMessage result = m_pServiceInterface->call("PreCheck");
    QList<QVariant> outArgs = result.arguments();
    int projectionstatus = outArgs.at(0).value<int>();
    qDebug() << "---->" << projectionstatus;
    if (projectionstatus != 1) {
        QMessageBox::information(NULL, QStringLiteral("提示"), QStringLiteral("投屏无法使用"));
        ui->pronamelabel->setEnabled(false);
        projectionBtn->setEnabled(false);
    }

    return pluginWidget;
}

void Projection::plugin_delay_control(){

}

const QString Projection::name() const {

    return QStringLiteral("projection");
}

void Projection::projectionPinSlots(QString type, QString pin) {
    if (type.contains("clear")) {
        m_pin->clear();
    } else {
        qDebug()<<pin;
        m_pin->setText(pin);
    }
}

void Projection::projectionButtonClickSlots(bool status) {

    qDebug() << "aaaaaa";
    if (status){        
        m_pServiceInterface->call("Start",ui->pronamelabel->text(),"");
    } else {
        m_pServiceInterface->call("Stop");
    }
}

void Projection::initComponent(){

    addWgt = new HoverWidget("");
    addWgt->setObjectName("addwgt");
    addWgt->setMinimumSize(QSize(580, 64));
    addWgt->setMaximumSize(QSize(16777215, 64));
    addWgt->setStyleSheet("HoverWidget#addwgt{background: palette(base); border-radius: 4px;}HoverWidget:hover:!pressed#addwgt{background: #2FB3E8; border-radius: 4px;}");

    QHBoxLayout *addLyt = new QHBoxLayout;

    QLabel * iconLabel = new QLabel();
    //~ contents_path /bluetooth/Add Bluetooths
    QLabel * textLabel = new QLabel(tr("Add Bluetooths"));
    QPixmap pixgray = ImageUtil::loadSvg(":/img/titlebar/add.svg", "black", 12);
    iconLabel->setPixmap(pixgray);
    addLyt->addItem(new QSpacerItem(8,10,QSizePolicy::Fixed));
    addLyt->addWidget(iconLabel);
    addLyt->addItem(new QSpacerItem(16,10,QSizePolicy::Fixed));
    addLyt->addWidget(textLabel);
    addLyt->addStretch();
    addWgt->setLayout(addLyt);
    addWgt->hide();
}

