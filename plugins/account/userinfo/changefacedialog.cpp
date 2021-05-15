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
#include "CloseButton/closebutton.h"

#include <QDebug>
#include <QMessageBox>
#include <QFileSystemWatcher>

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
    // ui->closeBtn->setProperty("useIconHighlightEffect", true);
    // ui->closeBtn->setProperty("iconHighlightEffectMode", 1);
    // ui->closeBtn->setFlat(true);

    // ui->closeBtn->setStyleSheet("QPushButton:hover:!pressed#closeBtn{background: #FA6056; border-radius: 4px;}"
    // "QPushButton:hover:pressed#closeBtn{background: #E54A50; border-radius: 4px;}");

// ui->frame->setStyleSheet("QFrame{background: #ffffff; border: none; border-radius: 6px;}");
// ui->closeBtn->setStyleSheet("QPushButton{background: #ffffff; border: none;}");

    ui->customfaceBtn->setStyleSheet("background: transparent; text-align:left");

    selectedFaceIcon = "";

    btnsGroup = new QButtonGroup;

    ElipseMaskWidget *cfMaskWidget = new ElipseMaskWidget(ui->faceLabel);
// cfMaskWidget->setBgColor("#F4F4F4");
    cfMaskWidget->setGeometry(0, 0, ui->faceLabel->width(), ui->faceLabel->height());

    ui->saveBtn->setEnabled(false);

    loadSystemFaces();

    connect(ui->cancelBtn, &QPushButton::clicked, [=]{
        close();
    });
    connect(ui->customfaceBtn, &QPushButton::clicked, [=]{
        showLocalFaceDialog();
    });

    connect(ui->saveBtn, &QPushButton::clicked, [=]{
        emit face_file_send(selectedFaceIcon);
        close();
    });
}

ChangeFaceDialog::~ChangeFaceDialog()
{
    delete ui;
    ui = nullptr;
}

void ChangeFaceDialog::loadSystemFaces()
{
    FlowLayout *facesFlowLayout = new FlowLayout(ui->facesWidget, 0, 5, 5);
    ui->facesWidget->setLayout(facesFlowLayout);

    // 遍历头像目录
    QStringList facesList;
    QDir facesDir = QDir(FACEPATH);
    foreach (QString filename, facesDir.entryList(QDir::Files)) {
// facesList.append(FACEPATH + filename);
        QString fullface = QString("%1%2").arg(FACEPATH).arg(filename);
        if (fullface.endsWith(".svg"))
            continue;
        if (fullface.endsWith("3.png"))
            continue;

        QPushButton *button = new QPushButton;
        button->setCheckable(true);
        button->setAttribute(Qt::WA_DeleteOnClose);
        button->setFixedSize(QSize(64, 64));
// button->setStyleSheet("QPushButton{border: none;}");

        btnsGroup->addButton(button);

        QHBoxLayout *mainHorLayout = new QHBoxLayout(button);
        mainHorLayout->setSpacing(0);
        mainHorLayout->setMargin(0);
        QLabel *iconLabel = new QLabel(button);
        iconLabel->setScaledContents(true);
        iconLabel->setPixmap(QPixmap(fullface));

        mainHorLayout->addWidget(iconLabel);

        button->setLayout(mainHorLayout);

        connect(button, &QPushButton::clicked, [=]{
            // show dialog更新头像
            setFace(fullface);

            selectedFaceIcon = fullface;

            if (!ui->saveBtn->isEnabled())
                ui->saveBtn->setEnabled(true);

// emit face_file_send(fullface, ui->usernameLabel->text());
        });

        facesFlowLayout->addWidget(button);
    }
}

void ChangeFaceDialog::setFace(QString iconfile)
{
    ui->faceLabel->setPixmap(QPixmap(iconfile));
}

void ChangeFaceDialog::setUsername(QString username)
{
    ui->usernameLabel->setText(username);
}

void ChangeFaceDialog::setAccountType(QString atype)
{
    ui->typeLabel->setText(atype);
}

void ChangeFaceDialog::showLocalFaceDialog()
{
    QString filters = "Face files(*.jpg *.jpeg *.png *.svg)";
    QFileDialog fd;
    QList<QUrl> usb_list = fd.sidebarUrls();
    int sidebarNum = 8;// 最大添加U盘数，可以自己定义
    QString home_path = QDir::homePath().section("/", -1, -1);
    QString mnt = "/media/" + home_path + "/";
    QDir mntDir(mnt);
    mntDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    QFileInfoList file_list = mntDir.entryInfoList();
    QList<QUrl> mntUrlList;
    for (int i = 0; i < sidebarNum && i < file_list.size(); ++i) {
        QFileInfo fi = file_list.at(i);
        mntUrlList << QUrl("file://" + fi.filePath());
    }

    QFileSystemWatcher m_fileSystemWatcher(&fd);
    m_fileSystemWatcher.addPath("/media/" + home_path + "/");
    connect(&m_fileSystemWatcher, &QFileSystemWatcher::directoryChanged, &fd,
            [=, &sidebarNum, &mntUrlList, &usb_list, &fd](const QString path) {
        QDir m_wmntDir(path);
        m_wmntDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
        QFileInfoList m_wfilist = m_wmntDir.entryInfoList();
        mntUrlList.clear();
        for (int i = 0; i < sidebarNum && i < m_wfilist.size(); ++i) {
            QFileInfo m_fi = m_wfilist.at(i);
            mntUrlList << QUrl("file://" + m_fi.filePath());
        }
        fd.setSidebarUrls(usb_list + mntUrlList);
        fd.update();
    });

    connect(&fd, &QFileDialog::finished, &fd, [=, &usb_list, &fd]() {
        fd.setSidebarUrls(usb_list);
    });

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

    fd.setSidebarUrls(usb_list + mntUrlList);

    if (fd.exec() != QDialog::Accepted)
        return;

    QString selectedfile;
    selectedfile = fd.selectedFiles().first();

    QFile pic(selectedfile);
    int size = pic.size();

    qDebug() << "size is"  << size;
    if (size >= 1048576) {
        QMessageBox::warning(this, tr("Warning"),
                             tr("The avatar is larger than 1M, please choose again"));
        return;
    }

    setFace(selectedfile);

    selectedFaceIcon = selectedfile;

    if (!ui->saveBtn->isEnabled())
        ui->saveBtn->setEnabled(true);
// emit face_file_send(selectedfile);
}

void ChangeFaceDialog::paintEvent(QPaintEvent *event)
{
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
    p.fillPath(rectPath, palette().color(QPalette::Base));
    p.restore();
}
