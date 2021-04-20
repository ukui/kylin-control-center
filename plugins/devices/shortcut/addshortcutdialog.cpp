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

#include <QDebug>

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

    initSetup();
    slotsSetup();
    limitInput();
    refreshCertainChecked();
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

    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
    ui->noteLabel->setPixmap(QPixmap("://img/plugins/shortcut/note.png"));
    ui->execLineEdit->setReadOnly(true);

    ui->stackedWidget->setCurrentIndex(1);
    ui->kkeysequencewidget->setClearButtonShown(false);
    ui->kkeysequencewidget->setMultiKeyShortcutsAllowed(false);
    ui->kkeysequencewidget->setModifierlessAllowed(false);
    ui->kkeysequencewidget->setCheckForConflictsAgainst(KKeySequenceWidget::None);
}

void addShortcutDialog::slotsSetup()
{
    connect(ui->kkeysequencewidget, &KKeySequenceWidget::keySequenceChanged, this,
            [=](QKeySequence seq){
        qDebug() << seq.toString() << keyToLib(seq.toString());
        if (ui->kkeysequencewidget->isKeySequenceAvailable(seq)) {
            if (conflictWithGlobalShortcuts(seq) || conflictWithStandardShortcuts(seq)
                || conflictWithSystemShortcuts(seq) || conflictWithCustomShortcuts(seq)) {
                ui->kkeysequencewidget->clearKeySequence();
                ui->kkeysequencewidget->clearFocus();
                ui->label_4->setText(tr("shortcut conflict"));
                ui->stackedWidget->setCurrentIndex(0);
                keyIsAvailable = false;
                refreshCertainChecked();
            } else if (!isKeyAvailable(seq)) {
                ui->kkeysequencewidget->clearKeySequence();
                ui->kkeysequencewidget->clearFocus();
                ui->label_4->setText(tr("invaild shortcut"));
                ui->stackedWidget->setCurrentIndex(0);
                keyIsAvailable = false;
                refreshCertainChecked();
            } else {
                ui->stackedWidget->setCurrentIndex(1);
                keyIsAvailable = true;
                refreshCertainChecked();
            }
        }
        qDebug() << "keySequenceChanged" << seq <<
            ui->kkeysequencewidget->isKeySequenceAvailable(seq);
    });
    connect(ui->openBtn, &QPushButton::clicked, [=](bool checked){
        Q_UNUSED(checked)
        openProgramFileDialog();
    });

    connect(ui->execLineEdit, &QLineEdit::textChanged, [=](QString text){
        if (text.endsWith("desktop")
            || (!g_file_test(text.toLatin1().data(),
                             G_FILE_TEST_IS_DIR)
                && g_file_test(text.toLatin1().data(), G_FILE_TEST_IS_EXECUTABLE))) {
            ui->certainBtn->setChecked(true);
            ui->stackedWidget->setCurrentIndex(1);
        } else {
            ui->certainBtn->setChecked(false);
            ui->stackedWidget->setCurrentIndex(0);
        }

        refreshCertainChecked();
    });

    connect(ui->nameLineEdit, &QLineEdit::textChanged, [=](QString text){
        QStringList customName;
        for (KeyEntry *ckeyEntry : customEntry) {
            customName << ckeyEntry->nameStr;
            if (customName.contains(text)) {
                ui->stackedWidget->setCurrentIndex(0);
                ui->label_4->setText(tr("repeated naming"));
            } else {
                ui->stackedWidget->setCurrentIndex(1);
            }
        }
        refreshCertainChecked();
    });

    connect(ui->cancelBtn, &QPushButton::clicked, [=] {
        close();
    });
    connect(ui->certainBtn, &QPushButton::clicked, [=] {
        emit shortcutInfoSignal(gsPath, ui->nameLineEdit->text(), selectedfile,
                                ui->kkeysequencewidget->keySequence().toString());

        close();
    });

    connect(this, &addShortcutDialog::finished, [=] {
        gsPath = "";
        ui->nameLineEdit->clear();
        ui->execLineEdit->clear();
        ui->stackedWidget->setCurrentIndex(1);
        ui->nameLineEdit->setFocus(Qt::ActiveWindowFocusReason);

        refreshCertainChecked();
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
            qDebug() << "count = 2,keyToLib = " << keyToLib;
            return keyToLib;
        } else if (keys.count() == 3) {
            QString lower = keys.at(2);
            QString keyToLib = "<" + keys.at(0) + ">" + "<" + keys.at(1) + ">" + lower.toLower();
            qDebug() << "count = 3,keyToLib = " << keyToLib;
            return keyToLib;
        }
    }
    qDebug() << "count = 1,keyToLib = " << key;
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
    QFileDialog fd;
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

void addShortcutDialog::refreshCertainChecked()
{
    if (ui->nameLineEdit->text().isEmpty() || ui->execLineEdit->text().isEmpty()
        || ui->stackedWidget->currentIndex() == 0
        || !keyIsAvailable) {
        ui->certainBtn->setDisabled(true);
    } else {
        ui->certainBtn->setDisabled(false);
    }
}

bool addShortcutDialog::conflictWithGlobalShortcuts(const QKeySequence &keySequence)
{
    qDebug() << "conflictWithGlobalShortcuts" << keySequence;

    QHash<QKeySequence, QList<KGlobalShortcutInfo> > clashing;
    for (int i = 0; i < keySequence.count(); ++i) {
        QKeySequence keys(keySequence[i]);
        qDebug() << "KGlobalAccel::isGlobalShortcutAvailable ::"
                 << keySequence
                 << KGlobalAccel::isGlobalShortcutAvailable(keySequence);
        if (!KGlobalAccel::isGlobalShortcutAvailable(keySequence)) {
            clashing.insert(keySequence, KGlobalAccel::getGlobalShortcutsByKey(keys));
        }
    }

    if (clashing.isEmpty()) {
        qDebug() << "not conflict With Global Shortcuts";
        return false;
    } else {
        qDebug() << "conflict With Global Shortcuts";
    }

    return true;
}

bool addShortcutDialog::conflictWithStandardShortcuts(const QKeySequence &seq)
{
    qDebug() << "conflictWithStandardShortcuts" << seq;

    KStandardShortcut::StandardShortcut ssc = KStandardShortcut::find(seq);
    if (ssc != KStandardShortcut::AccelNone) {
        qDebug() << "conflict With Standard Shortcuts";
        return true;
    } else {
        qDebug() << "not conflict With Standard Shortcuts";
    }
    return false;
}

bool addShortcutDialog::conflictWithSystemShortcuts(const QKeySequence &seq)
{
    QString systemKeyStr = keyToLib(seq.toString());

    for (KeyEntry *ckeyEntry : systemEntry) {
        if (systemKeyStr == ckeyEntry->valueStr) {
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
            return true;
        }
    }
    return false;
}

bool addShortcutDialog::isKeyAvailable(const QKeySequence &seq)
{
    QString keyStr = seq.toString();

    if (!keyStr.contains("+")) {
        return false;
    } else if (keyStr.contains("Num") || keyStr.contains("Space") || keyStr.contains("Meta")) {
        return false;
    } else {
        QStringList keys = keyStr.split("+");
        if (keys.count() == 4) {
            return false;
        }
    }
    return true;
}
