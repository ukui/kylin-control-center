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
#include "changefacedialog.h"
#include "ui_changefacedialog.h"

#include "FlowLayout/flowlayout.h"
#include "elipsemaskwidget.h"

#include <QDebug>
#include <QMessageBox>
#include <QFile>

#define FACEPATH "/usr/share/ukui/faces/"

extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);

ChangeFaceDialog::ChangeFaceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangeFaceDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);

    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
    ui->closeBtn->setProperty("useIconHighlightEffect", true);
    ui->closeBtn->setProperty("iconHighlightEffectMode", 1);
    ui->closeBtn->setFlat(true);

    ui->closeBtn->setStyleSheet("QPushButton:hover:!pressed#closeBtn{background: #FA6056; border-radius: 4px;}"
                                "QPushButton:hover:pressed#closeBtn{background: #E54A50; border-radius: 4px;}");

//    //分割线，其颜色应由主题控制，此处设置样式仅为预览布局效果
//    ui->line_1->setStyleSheet(".QWidget{background: rgba(66,77,89,1); ipacity: 0.1;}");
//    ui->line_2->setStyleSheet(".QWidget{background: rgba(66,77,89,1); ipacity: 0.1;}");

//    ui->frame->setStyleSheet("QFrame{background: #ffffff; border: none; border-radius: 6px;}");
//    ui->closeBtn->setStyleSheet("QPushButton{background: #ffffff; border: none;}");


    ui->closeBtn->setIcon(QIcon("://img/titlebar/close.svg"));

    ui->historyFacesWidget->setContentsMargins(4,0,0,0);

    historyFacesFlowLayout = new FlowLayout(ui->historyFacesWidget);
    ui->historyFacesWidget->setLayout(historyFacesFlowLayout);

//    ElipseMaskWidget * cfMaskWidget = new ElipseMaskWidget(ui->faceLabel);
////    cfMaskWidget->setBgColor("#F4F4F4");
//    cfMaskWidget->setGeometry(0, 0, ui->faceLabel->width(), ui->faceLabel->height());

    loadSystemFaces();

    connect(ui->closeBtn, &QPushButton::clicked, [=]{
        close();
    });
    connect(ui->cancelBtn, &QPushButton::clicked, [=]{
        close();
    });
//    connect(ui->customfaceBtn, &QPushButton::clicked, [=]{
//        showLocalFaceDialog();
//    });
}

ChangeFaceDialog::~ChangeFaceDialog()
{
    delete ui;
}

void ChangeFaceDialog::loadSystemFaces(){

    ui->facesWidget->setContentsMargins(4,0,0,0);

    FlowLayout * facesFlowLayout = new FlowLayout(ui->facesWidget);
    ui->facesWidget->setLayout(facesFlowLayout);

    //遍历头像目录
    QStringList facesList;
    QDir facesDir = QDir(FACEPATH);
    foreach (QString filename, facesDir.entryList(QDir::Files)){
//        facesList.append(FACEPATH + filename);
        QString fullface = QString("%1%2").arg(FACEPATH).arg(filename);
        if (fullface.endsWith(".svg"))
            continue;
        if (fullface.endsWith("3.png"))
            continue;

        QPushButton * button = new QPushButton;
        button->setAttribute(Qt::WA_DeleteOnClose);
        button->setFixedSize(QSize(56, 56));
//        button->setStyleSheet("QPushButton{border: none;}");

        QHBoxLayout * mainHorLayout = new QHBoxLayout(button);
        mainHorLayout->setSpacing(0);
        mainHorLayout->setMargin(0);
        QLabel * iconLabel = new QLabel(button);
        iconLabel->setScaledContents(true);
        iconLabel->setPixmap(QPixmap(fullface));

        mainHorLayout->addWidget(iconLabel);

        button->setLayout(mainHorLayout);

        connect(button, &QPushButton::clicked, [=]{
            //show dialog更新头像
            setFace(fullface);
            confirmFile = fullface;
//            emit face_file_send(fullface, ui->usernameLabel->text());
        });
        connect(ui->confirmBtn, &QPushButton::clicked, [=]{
//            qDebug()<<confirmFile;
            if(confirmFile != "")emit face_file_send(confirmFile, ui->usernameLabel->text());
        });

        facesFlowLayout->addWidget(button);
    }
}

void ChangeFaceDialog::loadHistoryFaces(){
    if (historyFacesFlowLayout->layout() != NULL) {
        QLayoutItem* item;
        while ((item = historyFacesFlowLayout->layout()->takeAt( 0 )) != NULL )
        {
            delete item->widget();
            delete item;
        }
//        delete ui->availableLayout->layout();
    }
    //给每个历史头像创建组件
    for(int i = historyCount; i > 0; i--){
        QString historyface = QString("%1/%2.face").arg(historyFacesPath).arg(i);
//        qDebug()<<"load:"<<historyface;
        QPushButton * button = new QPushButton;
        button->setAttribute(Qt::WA_DeleteOnClose);
        button->setFixedSize(QSize(56, 56));
//        button->setStyleSheet("QPushButton{border: none;}");

        QHBoxLayout * mainHorLayout = new QHBoxLayout(button);
        mainHorLayout->setSpacing(0);
        mainHorLayout->setMargin(0);
        QLabel * iconLabel = new QLabel(button);
        iconLabel->setScaledContents(true);
        iconLabel->setPixmap(QPixmap(historyface));
//        ElipseMaskWidget * cpMaskWidget = new ElipseMaskWidget(button);
//        cpMaskWidget->setGeometry(0, 0, 56, 56);
        QPushButton * delBtn = new QPushButton;
        delBtn->setFixedSize(16,16);
//        delBtn->setIcon(QIcon(":/img/plugins/userinfo/delete.png"));
        QHBoxLayout * delBtnLayout = new QHBoxLayout(iconLabel);
        delBtnLayout->setSpacing(0);

        QHBoxLayout * delBtnHorLayout = new QHBoxLayout(button);
        delBtnHorLayout->setSpacing(0);
        delBtnHorLayout->setMargin(0);
        QLabel * delBtnLabel = new QLabel(delBtn);
        delBtnLabel->setScaledContents(true);
        delBtnLabel->setPixmap(QPixmap(":/img/plugins/userinfo/delete.png"));

        connect(delBtn, &QPushButton::clicked, this, [=]{
            sysinterface = new QDBusInterface("com.control.center.qt.systemdbus",
                                             "/",
                                             "com.control.center.interface",
                                             QDBusConnection::systemBus());

            if (!sysinterface->isValid()){
                qCritical() << "Create Client Interface Failed When Copy Face File: " << QDBusConnection::systemBus().lastError();
                return;
            }
            QString cmd = QString("rm %1/%2.face").arg(historyFacesPath).arg(i);
            historyCount--;
            sysinterface->call("systemRun", QVariant(cmd));
            for(int j = i + 1; j <= 4; j++){
                qDebug()<<j<<" to "<<j-1;
                cmd = QString("mv %1/%2.face %1/%3.face").arg(historyFacesPath).arg(j).arg(j-1);
                sysinterface->call("systemRun", QVariant(cmd));
            }
            loadHistoryFaces();
        });

        delBtnLayout->addStretch();
        delBtnLayout->addWidget(delBtn);
        delBtnLayout->setContentsMargins(0,0,0,50);

        mainHorLayout->addWidget(iconLabel);
        iconLabel->setLayout(delBtnLayout);

        button->setLayout(mainHorLayout);
        historyFacesFlowLayout->addWidget(button);
        connect(button, &QPushButton::clicked, this, [=]{
            setFace(historyface);
            confirmFile = historyface;
        });
    }

    //添加本地头像按钮
    QPushButton * addBtn = new QPushButton;
    addBtn->setAttribute(Qt::WA_DeleteOnClose);
    addBtn->setFixedSize(QSize(56, 56));
    addBtn->setIcon(QIcon("://img/titlebar/add.svg"));
    historyFacesFlowLayout->addWidget(addBtn);
    connect(addBtn, &QPushButton::clicked, this, [=]{
        showLocalFaceDialog();
    });
}

void ChangeFaceDialog::setFace(QString iconfile){
    ui->faceLabel->setPixmap(QPixmap(iconfile));
}

void ChangeFaceDialog::setUsername(QString username){
    ui->usernameLabel->setText(username);
}

void ChangeFaceDialog::setAccountType(QString atype){
    ui->typeLabel->setText(atype);
}

void ChangeFaceDialog::setHistoryFacesPath(QString path){
    historyFacesPath = path;
    QDir historyFacesDir;
    historyFacesDir.setPath(historyFacesPath);
    if (historyFacesDir.exists("4.face")){
        qDebug()<<"4 hisotry faces";
        historyCount = 4;
    } else if(historyFacesDir.exists("3.face")) {
        qDebug()<<"3 hisotry faces";
        historyCount = 3;
    } else if(historyFacesDir.exists("2.face")) {
        qDebug()<<"2 hisotry faces";
        historyCount = 2;
    } else if(historyFacesDir.exists("1.face")) {
        qDebug()<<"1 hisotry faces";
        historyCount = 1;
    } else {
        qDebug()<<"0 hisotry faces";
        historyCount = 0;
    }
    loadHistoryFaces();
}

void ChangeFaceDialog::showLocalFaceDialog(){
    QString filters = "Face files(*.png *.jpg *.svg)";
    QFileDialog fd;
    fd.setDirectory(QString(const_cast<char *>(g_get_user_special_dir(G_USER_DIRECTORY_PICTURES))));
    fd.setAcceptMode(QFileDialog::AcceptOpen);
    fd.setViewMode(QFileDialog::List);
    fd.setNameFilter(filters);
    fd.setFileMode(QFileDialog::ExistingFile);
    fd.setWindowTitle(tr("select custom face file"));
    fd.setLabelText(QFileDialog::Accept, tr("Select"));
    fd.setLabelText(QFileDialog::LookIn, tr("Position: "));
    fd.setLabelText(QFileDialog::FileName, tr("FileName: "));
    fd.setLabelText(QFileDialog::FileType, tr("FileType: "));
    fd.setLabelText(QFileDialog::Reject, tr("Cancel"));

    if (fd.exec() != QDialog::Accepted)
        return;

    QString selectedfile;
    selectedfile = fd.selectedFiles().first();;
//    QString fileurl = fd.selectedUrls().first().toEncoded();
//    QString filepath = fileurl.right(fileurl.length() - 7);

    QFile pic(selectedfile);
    int size = pic.size();

    if (size >= 2097152) {
        QMessageBox::warning(this, tr("Warning"), tr("The avatar is larger than 2M, please choose again"));
        return;
    }

    setFace(selectedfile);
    confirmFile = selectedfile;
    emit face_file_send(selectedfile, ui->usernameLabel->text());

    //添加本地头像后，将此头像copy到历史头像文件夹，再自动刷新历史头像列表
    sysinterface = new QDBusInterface("com.control.center.qt.systemdbus",
                                     "/",
                                     "com.control.center.interface",
                                     QDBusConnection::systemBus());

    if (!sysinterface->isValid()){
        qCritical() << "Create Client Interface Failed When Copy Face File: " << QDBusConnection::systemBus().lastError();
        return;
    }

    if(historyCount < 4){
        historyCount ++;
    } else {
        //删除最早添加的1.face文件，把2、3、4重命名为1、2、3，
        QString cmd = QString("rm %1/1.face").arg(historyFacesPath);
        sysinterface->call("systemRun", QVariant(cmd));
        cmd = QString("mv %1/2.face %1/1.face").arg(historyFacesPath);
        sysinterface->call("systemRun", QVariant(cmd));
        cmd = QString("mv %1/3.face %1/2.face").arg(historyFacesPath);
        sysinterface->call("systemRun", QVariant(cmd));
        cmd = QString("mv %1/4.face %1/3.face").arg(historyFacesPath);
        sysinterface->call("systemRun", QVariant(cmd));
    }
//    QString cp_cmd = QString("cp %1 %2/%3.face").arg(selectedfile).arg(historyFacesPath).arg(historyCount);
//    qDebug()<<cp_cmd;
//    qDebug()<<QVariant(cp_cmd);
//    sysinterface->call("systemRun", QVariant(cp_cmd));
    //由于systemRun方法传输的指令不支持中文路径，换用QFile的copy方法执行文件拷贝
    QFile *cp_file = new QFile();
    cp_file->copy(selectedfile, QString("%2/%3.face").arg(historyFacesPath).arg(historyCount));
    loadHistoryFaces();
}

void ChangeFaceDialog::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    QPainterPath rectPath;
    rectPath.addRoundedRect(this->rect().adjusted(10, 10, -10, -10), 6, 6);

    // 画一个黑底
    QPixmap pixmap(this->rect().size());
    pixmap.fill(Qt::transparent);
    QPainter pixmapPainter(&pixmap);
    pixmapPainter.setRenderHint(QPainter::Antialiasing);
    pixmapPainter.setPen(Qt::transparent);
    pixmapPainter.setBrush(Qt::black);
    pixmapPainter.setOpacity(0.65);
    pixmapPainter.drawPath(rectPath);
    pixmapPainter.end();

    // 模糊这个黑底
    QImage img = pixmap.toImage();
    qt_blurImage(img, 10, false, false);

    // 挖掉中心
    pixmap = QPixmap::fromImage(img);
    QPainter pixmapPainter2(&pixmap);
    pixmapPainter2.setRenderHint(QPainter::Antialiasing);
    pixmapPainter2.setCompositionMode(QPainter::CompositionMode_Clear);
    pixmapPainter2.setPen(Qt::transparent);
    pixmapPainter2.setBrush(Qt::transparent);
    pixmapPainter2.drawPath(rectPath);

    // 绘制阴影
    p.drawPixmap(this->rect(), pixmap, pixmap.rect());

    // 绘制一个背景
    p.save();
    p.fillPath(rectPath,palette().color(QPalette::Base));
    p.restore();
}
