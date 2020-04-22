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
#include "shortcut.h"
#include "ui_shortcut.h"

#include <QListWidgetItem>
#include <QDebug>

#include "customlineedit.h"

#include "realizeshortcutwheel.h"
#include "defineshortcutitem.h"

#define ITEMHEIGH 36

//快捷键屏蔽键
QStringList forbiddenKeys = {
    //Navigation keys
    "Home",
    "Left",
    "Up",
    "Right",
    "Down",
    "Page_Up",
    "Page_Down",
    "End",
    "Tab",

    // Return
    "Return",
    "Enter",

    "Space",
};

QList<KeyEntry *> generalEntries;
QList<KeyEntry *> customEntries;

Shortcut::Shortcut()
{
    ui = new Ui::Shortcut;
    pluginWidget = new QWidget;
    pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(pluginWidget);

    pluginName = tr("Shortcut");
    pluginType = DEVICES;

    ui->titleLabel->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");
    ui->title2Label->setStyleSheet("QLabel{font-size: 18px; color: palette(windowText);}");

//    pluginWidget->setStyleSheet("background: #ffffff;");

//    ui->generalListWidget->setStyleSheet("QListWidget#generalListWidget{background: #ffffff; border: none;}");

//    ui->showBtn->setStyleSheet("QPushButton{background: #E9E9E9; border-radius: 4px;}"
//                               "QPushButton:hover:!pressed{background: #3d6be5; border-radius: 4px;}"
//                               "QPushButton:hover:pressed{background: #415FC4; border-radius: 4px;}");

//    ui->resetBtn->setStyleSheet("QPushButton{background: #E9E9E9; border-radius: 4px;}"
//                               "QPushButton:hover:!pressed{background: #3d6be5; border-radius: 4px;}"
//                               "QPushButton:hover:pressed{background: #415FC4; border-radius: 4px;}");

//    ui->customListWidget->setStyleSheet("QListWidget#customListWidget{background: #ffffff; border: none;}");

//    ui->addWidget->setStyleSheet("QWidget{background: #F4F4F4; border-radius: 6px;}");

    pKeyMap = new KeyMap;
    addDialog = new addShortcutDialog();
    showDialog = new ShowAllShortcut();

    showList << "terminal" << "screenshot" << "area-screenshot" << "peony-qt" << "logout" << "screensaver";

    setupComponent();
    setupConnect();
    initFunctionStatus();
}

Shortcut::~Shortcut()
{
    delete ui;
    delete pKeyMap;
    delete addDialog;
    delete showDialog;
}

QString Shortcut::get_plugin_name(){
    return pluginName;
}

int Shortcut::get_plugin_type(){
    return pluginType;
}

QWidget *Shortcut::get_plugin_ui(){
    return pluginWidget;
}

void Shortcut::plugin_delay_control(){

}

void Shortcut::setupComponent(){
    ui->addLabel->setPixmap(QPixmap("://img/plugins/printer/add.png"));

    ui->generalListWidget->setFocusPolicy(Qt::NoFocus);
    ui->generalListWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->generalListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->generalListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->generalListWidget->setSpacing(1);

    ui->customListWidget->setFocusPolicy(Qt::NoFocus);
    ui->customListWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->customListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->customListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->customListWidget->setSpacing(0);
//    ui->customListWidget->setFixedHeight((showList.length() * ITEMHEIGH));

    ui->addFrame->installEventFilter(this);
    ui->generalListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->resetBtn->hide();

}

void Shortcut::setupConnect(){
    connect(ui->showBtn, &QPushButton::clicked, [=]{
        QMap<QString, QString> systemMap;
        QMap<QString, QString> desktopMap;

        //最新快捷键数据
        for (int i = 0; i < generalEntries.count(); i++){
            if (generalEntries[i]->gsSchema == KEYBINDINGS_DESKTOP_SCHEMA){
                desktopMap.insert(generalEntries[i]->keyStr, generalEntries[i]->valueStr);
            } else if (generalEntries[i]->gsSchema == KEYBINDINGS_SYSTEM_SCHEMA){
                systemMap.insert(generalEntries[i]->keyStr, generalEntries[i]->valueStr);
            }

        }
        QMap<QString, QMap<QString, QString>> generalMaps;
        generalMaps.insert(tr("Desktop"), desktopMap);
        generalMaps.insert(tr("System"), systemMap);
        showDialog->buildComponent(generalMaps);
        showDialog->show();
    });

    connect(addDialog, &addShortcutDialog::shortcutInfoSignal, [=](QString path, QString name, QString exec){
        createNewShortcut(path, name, exec);
    });

    connect(ui->generalListWidget, &QListWidget::itemSelectionChanged, [=]{
        QList<QListWidgetItem *> selectedItems = ui->generalListWidget->selectedItems();
        if (selectedItems.count() == 1){
            ui->resetBtn->show();
        } else{
            ui->resetBtn->hide();
        }
    });

    connect(ui->resetBtn, &QPushButton::clicked, [=]{
        QList<QListWidgetItem *> selectedItems = ui->generalListWidget->selectedItems();
        if (!selectedItems.isEmpty()){
            QListWidgetItem * sItem = selectedItems.first();
            DefineShortcutItem * wItem = dynamic_cast<DefineShortcutItem *>(ui->generalListWidget->itemWidget(sItem));
            KeyEntry * currentEntry = dynamic_cast<KeyEntry *>(wItem->userData(Qt::UserRole));

            const QByteArray id(currentEntry->gsSchema.toLatin1().data());
            QGSettings * settings = new QGSettings(id);

            settings->reset(currentEntry->keyStr);

            QString value = settings->get(currentEntry->keyStr).toString();
            wItem->setShortcutBinding(value);

            delete settings;
        }
    });
}

void Shortcut::initFunctionStatus(){
    generalEntries.clear();
    customEntries.clear();

    //使用线程获取快捷键
    pThread = new QThread;
    pWorker = new GetShortcutWorker;
    connect(pWorker, &GetShortcutWorker::generalShortcutGenerate, this, [=](QString schema, QString key, QString value){
//        qDebug() << "general shortcut" << schema << key << value;
        KeyEntry * generalKeyEntry = new KeyEntry;
        generalKeyEntry->gsSchema = schema;
        generalKeyEntry->keyStr = key;
        generalKeyEntry->valueStr = value;
        generalEntries.append(generalKeyEntry);

    });
    connect(pWorker, &GetShortcutWorker::customShortcutGenerate, this, [=](QString path, QString name, QString binding, QString action){
//        qDebug() << "custom shortcut" << path << name << binding;
        KeyEntry * customKeyEntry = new KeyEntry;
        customKeyEntry->gsSchema = KEYBINDINGS_CUSTOM_SCHEMA;
        customKeyEntry->gsPath = path;
        customKeyEntry->nameStr = name;
        customKeyEntry->bindingStr = binding;
        customKeyEntry->actionStr = action;
        customEntries.append(customKeyEntry);

    });
    connect(pWorker, &GetShortcutWorker::workerComplete, this, [=]{
        pThread->quit(); //退出事件循环
        pThread->wait(); //释放资源
    });

    pWorker->moveToThread(pThread);
    connect(pThread, &QThread::started, pWorker, &GetShortcutWorker::run);
    connect(pThread, &QThread::finished, this, [=]{
        //系统快捷键
        appendGeneralItems();
        ui->generalListWidget->setFixedHeight(ui->generalListWidget->count() * ITEMHEIGH);
        initGeneralItemsStyle();

        //自定义快捷键
        appendCustomItems();
        ui->customListWidget->setFixedHeight(ui->customListWidget->count() * ITEMHEIGH);
        initCustomItemsStyle();
    });
    connect(pThread, &QThread::finished, pWorker, &GetShortcutWorker::deleteLater);

    pThread->start();

}

void Shortcut::appendGeneralItems(){
    for (KeyEntry * gkeyEntry: generalEntries){
        if (showList.contains(gkeyEntry->keyStr)){
            DefineShortcutItem * singleWidget = new DefineShortcutItem(gkeyEntry->keyStr, gkeyEntry->valueStr);
            singleWidget->setFrameShape(QFrame::Shape::Box);
            singleWidget->setUserData(Qt::UserRole, gkeyEntry);

            CustomLineEdit * line = singleWidget->lineeditComponent();
            connect(line, &CustomLineEdit::shortcutCodeSignals, this, [=](QList<int> keyCode){
                newBindingRequest(keyCode);
            });

            QListWidgetItem * item = new QListWidgetItem(ui->generalListWidget);
            item->setSizeHint(QSize(ui->generalListWidget->width(), ITEMHEIGH));
            item->setData(Qt::UserRole, "");
            ui->generalListWidget->setItemWidget(item, singleWidget);
        }
    }
}

void Shortcut::appendCustomItems(){
    for (KeyEntry * ckeyEntry : customEntries){
        buildCustomItem(ckeyEntry);
    }
}

void Shortcut::buildCustomItem(KeyEntry * nkeyEntry){
    DefineShortcutItem * singleWidget = new DefineShortcutItem(nkeyEntry->nameStr, nkeyEntry->bindingStr);
    singleWidget->setDeleteable(true);
    singleWidget->setUpdateable(true);
    singleWidget->setUserData(Qt::UserRole, nkeyEntry);
    connect(singleWidget, &DefineShortcutItem::updateShortcutSignal, [=]{
        addDialog->setTitleText(QObject::tr("Update Shortcut"));
        addDialog->setUpdateEnv(nkeyEntry->gsPath, nkeyEntry->nameStr, nkeyEntry->actionStr);
        addDialog->exec();

    });

    CustomLineEdit * line = singleWidget->lineeditComponent();
    connect(line, &CustomLineEdit::shortcutCodeSignals, this, [=](QList<int> keyCode){
        newBindingRequest(keyCode);
    });

    QPushButton * button = singleWidget->btnComponent();


    QListWidgetItem * item = new QListWidgetItem(ui->customListWidget);
    item->setSizeHint(QSize(ui->customListWidget->width(), ITEMHEIGH));
    item->setData(Qt::UserRole, nkeyEntry->gsPath);
    ui->customListWidget->setItemWidget(item, singleWidget);


    connect(button, &QPushButton::clicked, [=]{
        int row = ui->customListWidget->row(item);
        QListWidgetItem * obItem =  ui->customListWidget->takeItem(row);

        delete obItem;

        ui->customListWidget->setFixedHeight(ui->customListWidget->count() * ITEMHEIGH);

        initCustomItemsStyle();

        deleteCustomShortcut(nkeyEntry->gsPath);
    });
}

void Shortcut::initItemsStyle(QListWidget *listWidget){
    int total = listWidget->count();
    for (int row = 0; row < total; row++){
        QString style;
        QString subStyle;
        if (1 == total){ //总数为1
            style = "QWidget{background: #F4F4F4; border: none; border-radius: 6px;}";
            subStyle = "background: #F4F4F4; border: none; border-radius: 4px;";
        } else if (0 == row && (row % 2 == 0)){ //首位
            style = "QWidget{background: #F4F4F4; border: none; border-top-left-radius: 6px; border-top-right-radius: 6px;}";
            subStyle = "background: #F4F4F4; border: none; border-radius: 4px;";
        } else if (total - 1 == row){ //末位
            if (0 == row % 2){
                style = "QWidget{background: #F4F4F4; border: none; border-bottom-left-radius: 6px; border-bottom-right-radius: 6px;}";
                subStyle = "background: #F4F4F4; border: none; border-radius: 4px;";
            } else {
                style = "QWidget{background: #EEEEEE; border: none; border-bottom-left-radius: 6px; border-bottom-right-radius: 6px;}";
                subStyle = "background: #EEEEEE; border: none; border-radius: 4px;";
            }
        } else if (row % 2 == 0){
            style = "QWidget{background: #F4F4F4; border: none;}";
            subStyle = "background: #F4F4F4; border: none; border-radius: 4px;";
        } else if (row % 2 != 0){
            style = "QWidget{background: #EEEEEE; border: none;}";
            subStyle = "background: #EEEEEE; border: none; border-radius: 4px;";
        }

        QWidget * widget = listWidget->itemWidget(listWidget->item(row));
        DefineShortcutItem * pShortcutItem = dynamic_cast<DefineShortcutItem *>(widget);
//        pShortcutItem->widgetComponent()->setStyleSheet(style);
//        pShortcutItem->btnComponent()->setStyleSheet(subStyle);
    }
}

void Shortcut::initGeneralItemsStyle(){
    initItemsStyle(ui->generalListWidget);
}

void Shortcut::initCustomItemsStyle(){
    initItemsStyle(ui->customListWidget);
}

void Shortcut::createNewShortcut(QString path, QString name, QString exec){
    QString availablepath;
    if (path.isEmpty()){
        availablepath = findFreePath(); //创建快捷键

        //更新数据
        KeyEntry * nKeyentry = new KeyEntry;
        nKeyentry->gsPath = availablepath;
        nKeyentry->nameStr = name;
        nKeyentry->bindingStr = DEFAULT_BINDING;
        nKeyentry->actionStr = exec;

        customEntries.append(nKeyentry);

        /*********刷新界面(添加)******/
        buildCustomItem(nKeyentry);

        ui->customListWidget->setFixedHeight(ui->customListWidget->count() * ITEMHEIGH);

        initCustomItemsStyle();
        /******************/


    } else {
        availablepath = path; //更新快捷键

        //更新数据
        int i = 0;
        for (; i < customEntries.count(); i++){
            if (customEntries[i]->gsPath == availablepath){
                customEntries[i]->nameStr = name;
                customEntries[i]->actionStr = exec;
                break;
            }
        }

        //刷新界面(更新)
        for (int row = 0; row < ui->customListWidget->count(); row++){
            QListWidgetItem * item = ui->customListWidget->item(row);
            if (!QString::compare(item->data(Qt::UserRole).toString(), availablepath)){
                DefineShortcutItem * widgetItem = dynamic_cast<DefineShortcutItem *>(ui->customListWidget->itemWidget(item));
                widgetItem->setShortcutName(name);
                KeyEntry * uKeyentry = customEntries.at(i);
                widgetItem->setUserData(Qt::UserRole, uKeyentry);
            }
        }
    }

//    if (availablepath.isEmpty()){
//        qDebug() << "add custom shortcut failed";
//        return;
//    }

    const QByteArray id(KEYBINDINGS_CUSTOM_SCHEMA);
    const QByteArray idd(availablepath.toLatin1().data());
    QGSettings * settings = new QGSettings(id, idd);

    settings->set(BINDING_KEY, DEFAULT_BINDING);
    settings->set(NAME_KEY, name);
    settings->set(ACTION_KEY, exec);

    delete settings;
}

void Shortcut::deleteCustomShortcut(QString path){
    if (path.isEmpty())
        return;

    gboolean ret;
    GError ** error = NULL;

    char * fullpath = path.toLatin1().data();

    DConfClient * client = dconf_client_new ();

    ret = dconf_client_write_sync (client, fullpath, NULL, NULL, NULL, error);

    if (!ret)
        qDebug() << "Delete Custom ShortCut Failed!";

    g_object_unref (client);
}

void Shortcut::newBindingRequest(QList<int> keyCode){
    QObject * object = QObject::sender();

    CustomLineEdit * current = qobject_cast<CustomLineEdit *>(object); //孙子
    QWidget * widget = current->parentWidget();
    DefineShortcutItem * widgetItem = dynamic_cast<DefineShortcutItem *>(widget->parentWidget());
    KeyEntry * nkeyEntry = dynamic_cast<KeyEntry *>(widgetItem->userData(Qt::UserRole));

    QString shortcutString = getBindingName(keyCode);

    //check for unmodified keys
    if (keyCode.count() == 1){
        if (shortcutString.contains(QRegExp("[a-z]")) ||
                shortcutString.contains(QRegExp("[0-9]")) ||
                keyIsForbidden(shortcutString)){
            qDebug() << "Please try with a key such as Control, Alt or Shift at the same time.";
            return;
        }
    }

    /* flag to see if the new accelerator was in use by something */
    for (KeyEntry * ckeyEntry : generalEntries){
        if (shortcutString == ckeyEntry->valueStr){
            qDebug() << QString("The shortcut \"%1\" is already used for\n\"%2\",please reset!!!").arg(shortcutString).arg(ckeyEntry->keyStr);
            return;
        }
    }

    current->setText(shortcutString);

    //已经设置了值，清除焦点不再监听
    current->clearFocus();

    if (nkeyEntry->gsPath.isEmpty()){ //非自定义快捷键的修改
        const QByteArray iid(nkeyEntry->gsSchema.toLatin1().data());
        QGSettings * settings = new QGSettings(iid);

        settings->set(nkeyEntry->keyStr, shortcutString);

        delete settings;

        //更新
        for (int index = 0; index < generalEntries.count(); index++){
            if (nkeyEntry->keyStr == generalEntries[index]->keyStr){
                generalEntries[index]->valueStr = shortcutString;
            }
        }

    } else { //自定义快捷键的修改
        const QByteArray id(KEYBINDINGS_CUSTOM_SCHEMA);
        const QByteArray idd(nkeyEntry->gsPath.toLatin1().data());
        QGSettings * settings = new QGSettings(id, idd);

        settings->set(BINDING_KEY, shortcutString);

        delete settings;

        //更新
        for (int index = 0; index < customEntries.count(); index++){
            if (nkeyEntry->nameStr == customEntries[index]->nameStr){
                customEntries[index]->bindingStr = shortcutString;
            }
        }
    }
}

QString Shortcut::getBindingName(QList<int> keyCode){
    QStringList tmpList;
    for (int keycode : keyCode){
        if (keycode >= 16777216 && keycode <= 16777254){ //1677216=Escape; 16777254=ScrollLock
            tmpList.append(QString("<%1>").arg(pKeyMap->keycodeTokeystring(keycode)));
        }
        else if (keycode >= 48 && keycode <= 57){ // 48 = 0; 57 = 9
            QString str = pKeyMap->keycodeTokeystring(keycode);
            tmpList.append(str.split("K_").at(1));
        }
        else if (keycode >= 65 && keycode <= 90){
            QString str = pKeyMap->keycodeTokeystring(keycode);
            tmpList.append(str.toLower());
        }
        else
            tmpList.append(pKeyMap->keycodeTokeystring(keycode));
    }
    return tmpList.join("");
}

bool Shortcut::keyIsForbidden(QString key){
    for (int i = 0; i < forbiddenKeys.length(); i++){
        if (key == forbiddenKeys[i])
            return true;
    }
    return false;
}

bool Shortcut::eventFilter(QObject *watched, QEvent *event){
    if (watched == ui->addFrame){
        if (event->type() == QEvent::MouseButtonPress){
            QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton){
                addDialog->setTitleText(QObject::tr("Add Shortcut"));
                addDialog->show();
                return true;
            } else
                return false;
        }
    }
    return QObject::eventFilter(watched, event);
}

//bool Shortcut::event(QEvent *event){
//    qDebug() << "---111--->";
//    if (event->type() == QEvent::MouseButtonPress){
//        qDebug() << "-------------->";
//        QMouseEvent * mouseEvent = dynamic_cast<QMouseEvent *>(event);
//        if (!ui->customListWidget->geometry().contains(mouseEvent->globalX(), mouseEvent->globalY())){
//            qDebug() << "=============>";
//            emit hideDelBtn();
//        }
//    }
//    return QObject::event(event);
//}

