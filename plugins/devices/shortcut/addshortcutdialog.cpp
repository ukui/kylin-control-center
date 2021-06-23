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

#include "addshortcutdialog.h"
#include "ui_addshortcutdialog.h"
#include "CloseButton/closebutton.h"
#include "realizeshortcutwheel.h"

#define DEFAULTPATH "/usr/share/applications/"

extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);

addShortcutDialog::addShortcutDialog(QList<KeyEntry *> generalEntries,
                                     QList<KeyEntry *> customEntries, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addShortcutDialog),
    gsPath(""),
    systemEntry(generalEntries),
    customEntry(customEntries),
    keyIsAvailable(false)
{
    ui->setupUi(this);

    editSeq  = QKeySequence("");
    editName = "";
    keyIsAvailable  = 0;
    execIsAvailable = false;
    nameIsAvailable = false;
    initSetup();
    slotsSetup();
    limitInput();

}

addShortcutDialog::~addShortcutDialog()
{
    delete ui;
    ui = nullptr;
}

void addShortcutDialog::initSetup()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Add custom shortcut"));

    ui->titleLabel->setStyleSheet("QLabel{color: palette(windowText);}");
    ui->noteLabel->setPixmap(QPixmap("://img/plugins/shortcut/note.png"));
    ui->execLineEdit->setReadOnly(true);

    ui->noteLabel->setVisible(false);
    ui->label_4->setStyleSheet("color: red");
    ui->label_4->setText("");

    ui->certainBtn->setDisabled(true);
    shortcutLine = new ShortcutLine(systemEntry, customEntry);
    ui->horizontalLayout_2->addWidget(shortcutLine);
    shortcutLine->setFixedWidth(302);
    shortcutLine->setPlaceholderText(tr("Please enter a shortcut"));

    connect(shortcutLine, &ShortcutLine::shortCutAvailable, this, [=](const int &flag){
        if (flag == 0 || (flag == -2 && editSeq == shortcutLine->keySequence())) {  //快捷键正常
            keyIsAvailable = 3;
        } else if(flag == -2) { //快捷键冲突
            keyIsAvailable = 1;
        } else {               //快捷键不可用
            keyIsAvailable = 2;
        }
        refreshCertainChecked(3);
    });
}

void addShortcutDialog::slotsSetup()
{
    connect(ui->openBtn, &QPushButton::clicked, [=](bool checked){
        Q_UNUSED(checked)
        openProgramFileDialog();
    });

    connect(ui->execLineEdit, &QLineEdit::textChanged, [=](QString text){
        if (text.endsWith("desktop")
            || (!g_file_test(text.toLatin1().data(),
                             G_FILE_TEST_IS_DIR)
                && g_file_test(text.toLatin1().data(), G_FILE_TEST_IS_EXECUTABLE))) {
            execIsAvailable = true;
        } else {
            execIsAvailable = false;
        }
        refreshCertainChecked(1);
    });

    connect(ui->nameLineEdit, &QLineEdit::textChanged, [=](){
        QStringList customName;
        QString text = ui->nameLineEdit->text();
        for (KeyEntry *ckeyEntry : customEntry) {
            customName << ckeyEntry->nameStr;
            if (customName.contains(text) && text != editName) {
                nameIsAvailable = false;
            } else {
                nameIsAvailable = true;
            }
        }
        refreshCertainChecked(2);
    });

    connect(ui->cancelBtn, &QPushButton::clicked, [=] {
        close();
    });
    connect(ui->certainBtn, &QPushButton::clicked, [=] {
        emit shortcutInfoSignal(gsPath, ui->nameLineEdit->text(), selectedfile,
                                shortcutLine->keySequence().toString());

        close();
    });

    connect(this, &addShortcutDialog::finished, [=] {
        gsPath = "";
        ui->nameLineEdit->clear();
        ui->execLineEdit->clear();
        ui->nameLineEdit->setFocus(Qt::ActiveWindowFocusReason);
    });
}

void addShortcutDialog::setTitleText(QString text)
{
    ui->titleLabel->setText(text);
}

void addShortcutDialog::setUpdateEnv(QString path, QString name, QString exec)
{
    gsPath = path;
    ui->nameLineEdit->setText(name);
    ui->execLineEdit->setText(exec);
}

void addShortcutDialog::limitInput()
{
    // 大小写字母数字中文
    QRegExp rx("[a-zA-Z0-9\u4e00-\u9fa5]+");
    QRegExpValidator *regValidator = new QRegExpValidator(rx);
    // 输入限制
    ui->nameLineEdit->setValidator(regValidator);
    // 字符长度限制
    // ui->nameLineEdit->setMaxLength(10);
}

QString addShortcutDialog::keyToLib(QString key)
{
    if (key.contains("+")) {
        QStringList keys = key.split("+");
        if (keys.count() == 2) {
            QString lower = keys.at(1);
            QString keyToLib = "<" + keys.at(0) + ">" + lower.toLower();

            return keyToLib;
        } else if (keys.count() == 3) {
            QString lower = keys.at(2);
            QString keyToLib = "<" + keys.at(0) + ">" + "<" + keys.at(1) + ">" + lower.toLower();

            return keyToLib;
        }
    }

    return key;
}

void addShortcutDialog::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
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

void addShortcutDialog::openProgramFileDialog()
{
    QString filters = tr("Desktop files(*.desktop)");
    QFileDialog fd(this);
    fd.setDirectory(DEFAULTPATH);
    fd.setAcceptMode(QFileDialog::AcceptOpen);
    fd.setViewMode(QFileDialog::List);
    fd.setNameFilter(filters);
    fd.setFileMode(QFileDialog::ExistingFile);
    fd.setWindowTitle(tr("select desktop"));
    fd.setLabelText(QFileDialog::Reject, tr("Cancel"));

    if (fd.exec() != QDialog::Accepted)
        return;

    selectedfile = fd.selectedFiles().first();

    QString exec = selectedfile.section("/", -1, -1);
    ui->execLineEdit->setText(exec);
}

void addShortcutDialog::refreshCertainChecked(int triggerFlag)
{
    ui->label_4->setText("");
    if (!execIsAvailable || keyIsAvailable != 3 || !nameIsAvailable) {
        ui->noteLabel->setVisible(true);
        ui->certainBtn->setDisabled(true);

        switch (triggerFlag) {
        case 1:
            if (!execIsAvailable) {
                ui->label_4->setText(tr("Invalid application"));  //程序无效
            } else if (keyIsAvailable == 1 && !shortcutLine->text().isEmpty()) {
                ui->label_4->setText(tr("Shortcut conflict"));  //快捷键冲突
            } else if (keyIsAvailable == 2 && !shortcutLine->text().isEmpty()) {
                ui->label_4->setText(tr("Invalid shortcut"));  //快捷键无效
            } else if (!nameIsAvailable && !ui->nameLineEdit->text().isEmpty()) {
                ui->label_4->setText(tr("Name repetition"));  //名称重复
            } else {
                ui->noteLabel->setVisible(false);
            }
            break;
        case 2:
            if (!nameIsAvailable) {
                ui->label_4->setText(tr("Name repetition"));  //名称重复
            } else if (keyIsAvailable == 1 && !shortcutLine->text().isEmpty()) {
                ui->label_4->setText(tr("Shortcut conflict"));  //快捷键冲突
            } else if (keyIsAvailable == 2 && !shortcutLine->text().isEmpty()) {
                ui->label_4->setText(tr("Invalid shortcut"));  //快捷键无效
            } else if (!execIsAvailable && !ui->execLineEdit->text().isEmpty()) {
                ui->label_4->setText(tr("Invalid application"));  //程序无效
            } else {
                ui->noteLabel->setVisible(false);
            }
            break;
        case 3:
            if (keyIsAvailable == 1) {
                ui->label_4->setText(tr("Shortcut conflict"));  //快捷键冲突
            } else if (keyIsAvailable == 2) {
                ui->label_4->setText(tr("Invalid shortcut"));  //快捷键无效
            } else if (!execIsAvailable && !ui->execLineEdit->text().isEmpty()) {
                ui->label_4->setText(tr("Invalid application"));  //程序无效
            } else if (!nameIsAvailable && !ui->nameLineEdit->text().isEmpty()) {
                ui->label_4->setText(tr("Name repetition"));  //名称重复
            } else {
                ui->noteLabel->setVisible(false);
            }
            break;
        default:
            ui->label_4->setText(tr("Unknown error"));  //未知问题，不会触发
            break;
        }

    } else {
        ui->noteLabel->setVisible(false);
        ui->certainBtn->setDisabled(false);
    }
}

bool addShortcutDialog::conflictWithGlobalShortcuts(const QKeySequence &keySequence)
{
    QHash<QKeySequence, QList<KGlobalShortcutInfo>> clashing;
    for (int i = 0; i < keySequence.count(); ++i) {
        QKeySequence keys(keySequence[i]);

        if (!KGlobalAccel::isGlobalShortcutAvailable(keySequence)) {
            clashing.insert(keySequence, KGlobalAccel::getGlobalShortcutsByKey(keys));
        }
    }

    if (clashing.isEmpty()) {
        return false;
    } else {
        qDebug() << "conflict With Global Shortcuts";
    }

    return true;
}

bool addShortcutDialog::conflictWithStandardShortcuts(const QKeySequence &seq)
{
    KStandardShortcut::StandardShortcut ssc = KStandardShortcut::find(seq);
    if (ssc != KStandardShortcut::AccelNone) {
        qDebug() << "conflict With Standard Shortcuts";
        return true;
    }

    return false;
}

bool addShortcutDialog::conflictWithSystemShortcuts(const QKeySequence &seq)
{
    QString systemKeyStr = keyToLib(seq.toString());

    if (systemKeyStr.contains("Ctrl")) {
        systemKeyStr.replace("Ctrl", "Control");
    }
    for (KeyEntry *ckeyEntry : systemEntry) {
        if (systemKeyStr == ckeyEntry->valueStr) {
            qDebug() << "conflictWithSystemShortcuts" << seq;
            return true;
        }
    }
    return false;
}

bool addShortcutDialog::conflictWithCustomShortcuts(const QKeySequence &seq)
{
    QString customKeyStr = keyToLib(seq.toString());

    for (KeyEntry *ckeyEntry : customEntry) {
        if (customKeyStr == ckeyEntry->bindingStr) {
            qDebug() << "conflictWithCustomShortcuts" << seq;
            return true;
        }
    }
    return false;
}

bool addShortcutDialog::isKeyAvailable(const QKeySequence &seq)
{
    QString keyStr = seq.toString();

    if (!keyStr.contains("+")) {
        qDebug() << "is not Available";
        return false;
    } else if (keyStr.contains("Num") || keyStr.contains("Space")
               || keyStr.contains("Meta") || keyStr.contains("Ins") || keyStr.contains("Home")
               || keyStr.contains("PgUp") || keyStr.contains("Del") || keyStr.contains("End")
               || keyStr.contains("PgDown") || keyStr.contains("Print")
               || keyStr.contains("Backspace") || keyStr.contains("ScrollLock")
               || keyStr.contains("Return") || keyStr.contains("Enter")
               || keyStr.contains("Tab") || keyStr.contains("CapsLock")
               || keyStr.contains("Left") || keyStr.contains("Right")
               || keyStr.contains("Up") || keyStr.contains("Down")
               || keyStr.contains("Clear Grab")) {
        qDebug() << "is not Available";
        return false;
    } else {
        QStringList keys = keyStr.split("+");
        if (keys.count() == 4) {
            qDebug() << "is not Available";
            return false;
        } else {
            QString key = keys.at(keys.count() - 1);
            if (!key.contains(QRegExp("[A-Z]")) && !key.contains(QRegExp("[a-z]"))
                && !key.contains(QRegExp("[0-9]"))) {
                qDebug() << "is not Available";
                return false;
            }
        }
    }

    return true;
}

void addShortcutDialog::setExecText(const QString &text)
{
    selectedfile = text;
    QString exec = selectedfile.section("/", -1, -1);
    ui->execLineEdit->setText(exec);
}

void addShortcutDialog::setNameText(const QString &text)
{
    editName = text;
    ui->nameLineEdit->setText(text);
}

void addShortcutDialog::setKeyText(const QString &text)
{

    QString showText = text;
    showText = showText.replace("<","");
    showText = showText.replace(">"," + ");
    QString endStr = showText.mid(showText.length() - 1, 1);
    showText = showText.mid(0, showText.length() - 1) + endStr.toUpper();

    shortcutLine->setText(showText);

    QKeySequence seq(showText.replace(" ", "")); //去掉空格
    editSeq = seq;
    shortcutLine->setKeySequence(seq);
}
