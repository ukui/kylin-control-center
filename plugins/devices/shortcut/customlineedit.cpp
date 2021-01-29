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
#include "customlineedit.h"

#include <QDebug>

CustomLineEdit::CustomLineEdit(QString shortcut, QWidget *parent) :
    QLineEdit(parent),
    _oldshortcut(shortcut)
{
    _wait = tr("New Shortcut...");

    flag = true;
    setFocusPolicy(Qt::ClickFocus);
}

CustomLineEdit::~CustomLineEdit()
{
}


void CustomLineEdit::focusInEvent(QFocusEvent *evt){
    this->setText(_wait);
    flag = true;
}

void CustomLineEdit::focusOutEvent(QFocusEvent *evt){
    if (this->text() == _wait)
        this->setText(_oldshortcut);
    flag = false;
}

void CustomLineEdit::setFlagStatus(bool checked){
    flag = checked;
}

void CustomLineEdit::keyReleaseEvent(QKeyEvent *evt){
    QList<int> tmpList;

    if (evt->key() == Qt::Key_Escape){
        this->clearFocus();
    }

    if (int(evt->modifiers()) == Qt::NoModifier && evt->key() != 0 && flag){ //判断当前text，屏蔽掉多余的keyRelease事件触发
        tmpList.append(evt->key());
//        qDebug() << evt->key() << evt->text();
    }
    else if (evt->modifiers() == Qt::ControlModifier && evt->key() != 0 && flag){
        tmpList.append(Qt::Key_Control);
        tmpList.append(evt->key());
//        qDebug() << "Ctr + " << evt->key() << evt->text() << (int)Qt::ControlModifier << (int)Qt::Key_Control;
    }
    else if (evt->modifiers() == Qt::AltModifier && evt->key() != 0 && flag){
        tmpList.append(Qt::Key_Alt);
        tmpList.append(evt->key());
//        qDebug() << "Alt + " << evt->key() << evt->text();
    }
    else if (evt->modifiers() == Qt::ShiftModifier && evt->key() != 0 && flag){
        tmpList.append(Qt::Key_Shift);
        tmpList.append(evt->key());
//        qDebug() << "Shift + " << evt->key() << evt->text();
    }
    else if ((evt->modifiers() == (Qt::ControlModifier | Qt::AltModifier)) && evt->key() != 0 && flag){
        tmpList.append(Qt::Key_Control);
        tmpList.append(Qt::Key_Alt);
        tmpList.append(evt->key());
//        qDebug() << "Ctr + Alt" << evt->key() << evt->text();
    }
    else if ((evt->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier)) && evt->key() != 0 && flag){
        tmpList.append(Qt::Key_Control);
        tmpList.append(Qt::Key_Shift);
        tmpList.append(evt->key());
//        qDebug() << "Ctr + shift" << evt->key() << evt->text();
    }
    else if ((evt->modifiers() == (Qt::AltModifier | Qt::ShiftModifier)) && evt->key() != 0 && flag){
        tmpList.append(Qt::Key_Alt);
        tmpList.append(Qt::Key_Shift);
        tmpList.append(evt->key());
//        qDebug() << "Alt + shift" << evt->key() << evt->text();
    }
    else if ((evt->modifiers() == (Qt::ControlModifier | Qt::AltModifier | Qt::ShiftModifier) && evt->key() != 0 && flag)){
        tmpList.append(Qt::Key_Control);
        tmpList.append(Qt::Key_Alt);
        tmpList.append(Qt::Key_Shift);
        tmpList.append(evt->key());
    }

    if (tmpList.length() > 0){
        emit shortcutCodeSignals(tmpList);
//        this->clearFocus();
    }

}

void CustomLineEdit::updateOldShow(QString newStr){
    _oldshortcut = newStr;
}
