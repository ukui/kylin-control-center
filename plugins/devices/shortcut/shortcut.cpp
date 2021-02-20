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


/* qt会将glib里的signals成员识别为宏，所以取消该宏
 * 后面如果用到signals时，使用Q_SIGNALS代替即可
 **/
#ifdef signals
#undef signals
#endif

extern "C" {
#include <glib.h>
#include <glib/gi18n.h>
}


#define ITEMHEIGH 36
#define TITLEWIDGETHEIGH 40
#define SYSTEMTITLEWIDGETHEIGH 50

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

Shortcut::Shortcut() : mFirstLoad(true)
{
    pluginName = tr("Shortcut");
    pluginType = DEVICES;
}

Shortcut::~Shortcut()
{
    if (!mFirstLoad) {
        delete ui;
        ui = nullptr;
        delete pKeyMap;
        pKeyMap = nullptr;
        delete addDialog;
        addDialog = nullptr;
    }
}

QString Shortcut::get_plugin_name(){
    return pluginName;
}

int Shortcut::get_plugin_type(){
    return pluginType;
}

QWidget *Shortcut::get_plugin_ui(){
    if (mFirstLoad) {
        mFirstLoad = false;

        ui = new Ui::Shortcut;
        pluginWidget = new QWidget;
        pluginWidget->setAttribute(Qt::WA_DeleteOnClose);
        ui->setupUi(pluginWidget);

        pKeyMap = new KeyMap;
        addDialog = new addShortcutDialog();

        isCloudService = false;

        initTitleLabel();
        setupComponent();
        setupConnect();
        initFunctionStatus();
        connectToServer();
    }
    return pluginWidget;
}

void Shortcut::plugin_delay_control(){

}

const QString Shortcut::name() const {

    return QStringLiteral("shortcut");
}

void Shortcut::initTitleLabel() {
    QFont font;
    font.setPixelSize(18);
    ui->systemLabel->setFont(font);
    ui->customLabel->setFont(font);
}

void Shortcut::connectToServer(){
    cloudInterface = new QDBusInterface("org.kylinssoclient.dbus",
                                          "/org/kylinssoclient/path",
                                          "org.freedesktop.kylinssoclient.interface",
                                          QDBusConnection::sessionBus());
    if (!cloudInterface->isValid())
    {
        qDebug() << "fail to connect to service";
        qDebug() << qPrintable(QDBusConnection::systemBus().lastError().message());
        return;
    }
    QDBusConnection::sessionBus().connect(QString(), QString("/org/kylinssoclient/path"), QString("org.freedesktop.kylinssoclient.interface"), "shortcutChanged", this, SLOT(shortcutChangedSlot()));
    // 将以后所有DBus调用的超时设置为 milliseconds
    cloudInterface->setTimeout(2147483647); // -1 为默认的25s超时
}

void Shortcut::setupComponent(){
    ui->systemLabel->setText(tr("System Shortcut"));
    ui->customLabel->setText(tr("Customize Shortcut"));

    ui->customListWidget->setFocusPolicy(Qt::NoFocus);
    ui->customListWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->customListWidget->setSpacing(0);
    ui->customListWidget->setMaximumWidth(960);

    QWidget * systemTitleWidget = new QWidget;
    QVBoxLayout * systemVerLayout = new QVBoxLayout(systemTitleWidget);

    systemTitleWidget->setFixedHeight(SYSTEMTITLEWIDGETHEIGH);
    systemTitleWidget->setStyleSheet("QWidget{background: palette(window); border: none; border-radius: 4px}");
    systemVerLayout->setSpacing(0);
    systemVerLayout->setContentsMargins(16, 15, 19, 0);

    QLabel * titleLabel  = new QLabel(systemTitleWidget);
    titleLabel->setText(tr("System Shortcut"));

    systemVerLayout->addWidget(titleLabel);
    systemVerLayout->addStretch();
    systemTitleWidget->setLayout(systemVerLayout);

    addWgt = new HoverWidget("");
    addWgt->setObjectName("addwgt");
    addWgt->setMinimumSize(QSize(580, 50));
    addWgt->setMaximumSize(QSize(960, 50));
    addWgt->setStyleSheet("HoverWidget#addwgt{background: palette(button); border-radius: 4px;}HoverWidget:hover:!pressed#addwgt{background: #3D6BE5; border-radius: 4px;}");

    QHBoxLayout *addLyt = new QHBoxLayout;

    QLabel * iconLabel = new QLabel();
    QLabel * textLabel = new QLabel(tr("Add custom shortcut"));
    QPixmap pixgray = ImageUtil::loadSvg(":/img/titlebar/add.svg", "black", 12);
    iconLabel->setPixmap(pixgray);
    iconLabel->setProperty("useIconHighlightEffect", true);
    iconLabel->setProperty("iconHighlightEffectMode", 1);
    addLyt->addWidget(iconLabel);
    addLyt->addWidget(textLabel);
    addLyt->addStretch();
    addWgt->setLayout(addLyt);

    ui->addLyt->addWidget(addWgt);
}

void Shortcut::setupConnect(){

    connect(addWgt, &HoverWidget::widgetClicked, this, [=](QString mname){
        addDialog->setTitleText(QObject::tr("Customize Shortcut"));
        addDialog->exec();
    });

    connect(addDialog, &addShortcutDialog::shortcutInfoSignal, [=](QString path, QString name, QString exec){
        createNewShortcut(path, name, exec);
    });
}

void Shortcut::initFunctionStatus(){
    generalEntries.clear();
    customEntries.clear();

    //使用线程获取快捷键
    pThread = new QThread;
    pWorker = new GetShortcutWorker;
    if(isCloudService == false) {
        connect(pWorker, &GetShortcutWorker::generalShortcutGenerate, this, [=](QString schema, QString key, QString value){
            //qDebug() << "general shortcut" << schema << key << value;
            KeyEntry * generalKeyEntry = new KeyEntry;
            generalKeyEntry->gsSchema = schema;
            generalKeyEntry->keyStr = key;
            generalKeyEntry->valueStr = value;
            generalEntries.append(generalKeyEntry);

        });
    }
    connect(pWorker, &GetShortcutWorker::customShortcutGenerate, this, [=](QString path, QString name, QString binding, QString action){
        //qDebug() << "custom shortcut" << path << name << binding;
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
        if (desktopMap.count() != 0) {
            generalMaps.insert("Desktop", desktopMap);
        }
        //系统快捷键
        appendGeneralItems(generalMaps);

        //自定义快捷键
        appendCustomItems();
        ui->customListWidget->setFixedHeight((ui->customListWidget->count() + 1) * ITEMHEIGH);
        initCustomItemsStyle();
        isCloudService = false;
    });
    connect(pThread, &QThread::finished, pWorker, &GetShortcutWorker::deleteLater);

    pThread->start();
}

QWidget * Shortcut::buildGeneralWidget(QString schema, QMap<QString, QString> subShortcutsMap){

    GSettingsSchema * pSettings;
    QString domain;

    if (schema == "Desktop"){
        pSettings = g_settings_schema_source_lookup(g_settings_schema_source_new_from_directory("/usr/share/glib-2.0/schemas/", g_settings_schema_source_get_default(), FALSE, NULL),
                                                    KEYBINDINGS_DESKTOP_SCHEMA,
                                                    FALSE);
        domain = "ukui-settings-daemon";

    } else if (schema == "System"){
        pSettings = g_settings_schema_source_lookup(g_settings_schema_source_new_from_directory("/usr/share/glib-2.0/schemas/", g_settings_schema_source_get_default(), FALSE, NULL),
                                                    KEYBINDINGS_SYSTEM_SCHEMA,
                                                    FALSE);
        domain = "gsettings-desktop-schemas";
    } else {
        return NULL;
    }

    QWidget * pWidget = new QWidget;
    pWidget->setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout * pVerLayout = new QVBoxLayout(pWidget);
    pVerLayout->setSpacing(2);
    //pVerLayout->setMargin(0);
    pVerLayout->setContentsMargins(0,0,0,16);

    pWidget->setLayout(pVerLayout);

    QMap<QString, QString>::iterator it = subShortcutsMap.begin();

    for (; it != subShortcutsMap.end(); it++){

        QWidget * gWidget = new QWidget;
        gWidget->setFixedHeight(TITLEWIDGETHEIGH);
        gWidget->setStyleSheet("QWidget{background: palette(window); border: none; border-radius: 4px}");

        QHBoxLayout * gHorLayout = new QHBoxLayout(gWidget);
        gHorLayout->setSpacing(0);
        gHorLayout->setContentsMargins(16, 0, 19, 0);

        QByteArray ba = domain.toLatin1();
        QByteArray ba1 = it.key().toLatin1();

        GSettingsSchemaKey * keyObj = g_settings_schema_get_key(pSettings, ba1.data());

        char * i18nKey;
        QLabel * nameLabel  = new QLabel(gWidget);
        i18nKey = const_cast<char *>(g_dgettext(ba.data(), g_settings_schema_key_get_summary(keyObj)));
        nameLabel->setText(QString(i18nKey));

        QLabel * bindingLabel = new QLabel(gWidget);
        bindingLabel->setText(it.value());

        gHorLayout->addWidget(nameLabel);
        gHorLayout->addStretch();
        gHorLayout->addWidget(bindingLabel);

        gWidget->setLayout(gHorLayout);


        pVerLayout->addWidget(gWidget);

        g_settings_schema_key_unref(keyObj);
    }

    g_settings_schema_unref(pSettings);

    return pWidget;

}

void Shortcut::appendGeneralItems(QMap<QString, QMap<QString, QString> > shortcutsMap){

    QMap<QString, QMap<QString, QString>>::iterator it = shortcutsMap.begin();
    for (; it != shortcutsMap.end(); it++){
        QWidget * gWidget = buildGeneralWidget(it.key(), it.value());
        if (gWidget != NULL) {
            gWidget->setMaximumWidth(960);
            ui->verticalLayout->addWidget(gWidget);
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
//    singleWidget->setUserData(Qt::UserRole, nkeyEntry);
    singleWidget->setProperty("userData", QVariant::fromValue(nkeyEntry));
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
//    item->setSizeHint(QSize(ui->customListWidget->width() - 4, ITEMHEIGH));
    item->setSizeHint(QSize(QSizePolicy::Expanding, ITEMHEIGH));
    item->setData(Qt::UserRole, nkeyEntry->gsPath);
    ui->customListWidget->setItemWidget(item, singleWidget);


    connect(button, &QPushButton::clicked, [=]{
        int row = ui->customListWidget->row(item);
        QListWidgetItem * obItem =  ui->customListWidget->takeItem(row);

        delete obItem;
        obItem = nullptr;

        ui->customListWidget->setFixedHeight((ui->customListWidget->count() + 1) * ITEMHEIGH);

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
    }
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
        nKeyentry->bindingStr = tr("disable");
        nKeyentry->actionStr = exec;

        customEntries.append(nKeyentry);

        /*********刷新界面(添加)******/
        buildCustomItem(nKeyentry);

        ui->customListWidget->setFixedHeight((ui->customListWidget->count() + 1) * ITEMHEIGH);

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
//                widgetItem->setUserData(Qt::UserRole, uKeyentry);
                widgetItem->setProperty("userData", QVariant::fromValue(uKeyentry));
            }
        }
    }

    const QByteArray id(KEYBINDINGS_CUSTOM_SCHEMA);
    const QByteArray idd(availablepath.toLatin1().data());
    QGSettings * settings = new QGSettings(id, idd);

    settings->set(BINDING_KEY, tr("disable"));
    settings->set(NAME_KEY, name);
    settings->set(ACTION_KEY, exec);

    delete settings;
    settings = nullptr;
}

void Shortcut::deleteCustomShortcut(QString path){
    if (path.isEmpty())
        return;

//    gboolean ret;
//    GError ** error = NULL;
    QProcess p(0);
    QStringList args;

    char * fullpath = path.toLatin1().data();
    QString cmd = "dconf";

    args.append("reset");
    args.append("-f");
    args.append(fullpath);
    p.execute(cmd,args);//command是要执行的命令,args是参数
    qDebug()<<"wait for finish";
    p.waitForFinished(-1);
    qDebug()<<QString::fromLocal8Bit(p.readAllStandardError());
//    DConfClient * client = dconf_client_new ();

//    ret = dconf_client_write_sync (client, fullpath, NULL, NULL, NULL, error);

//    if (!ret)
//        qDebug() << "Delete Custom ShortCut Failed!";

//    g_object_unref (client);
}

void Shortcut::newBindingRequest(QList<int> keyCode){
    QObject * object = QObject::sender();

    CustomLineEdit * current = qobject_cast<CustomLineEdit *>(object); //孙子
    QWidget * widget = current->parentWidget();
    DefineShortcutItem * widgetItem = dynamic_cast<DefineShortcutItem *>(widget->parentWidget());
//    KeyEntry * nkeyEntry = dynamic_cast<KeyEntry *>(widgetItem->userData(Qt::UserRole));
    KeyEntry * nkeyEntry = widgetItem->property("userData").value<KeyEntry *>();

    QString shortcutString = getBindingName(keyCode);
    int len = shortcutString.length();
    //check for unmodified keys
    if (nkeyEntry->gsPath.isEmpty()){ //非自定义快捷键的修改
        if (keyCode.count() == 1 && shortcutString.length() <= 1){
            if (shortcutString.contains(QRegExp("[a-z]")) ||
                    shortcutString.contains(QRegExp("[0-9]")) ||
                    keyIsForbidden(shortcutString)){
                const QByteArray iid(nkeyEntry->gsSchema.toLatin1().data());
                QGSettings * settings = new QGSettings(iid);
                current->setText(settings->get(nkeyEntry->keyStr).toString());
                current->updateOldShow(settings->get(nkeyEntry->keyStr).toString());
                current->clearFocus();
                qDebug() << "Please try with a key such as Control, Alt or Shift at the same time.";
                delete settings;
                settings = nullptr;
                return;
            }
        }

        if(shortcutString.isEmpty()){   //fn
            qDebug() << "the key is null";
            const QByteArray iid(nkeyEntry->gsSchema.toLatin1().data());
            QGSettings * settings = new QGSettings(iid);

            current->setText(settings->get(nkeyEntry->keyStr).toString());
            current->updateOldShow(settings->get(nkeyEntry->keyStr).toString());
            current->clearFocus();
            delete settings;
            settings = nullptr;
            return;
        }
        if(shortcutString.endsWith(">")){   //special key
            qDebug() << "end with >";
            const QByteArray iid(nkeyEntry->gsSchema.toLatin1().data());
            QGSettings * settings = new QGSettings(iid);

            current->setText(settings->get(nkeyEntry->keyStr).toString());
            current->updateOldShow(settings->get(nkeyEntry->keyStr).toString());
            current->clearFocus();
            delete settings;
            settings = nullptr;
            return;
        }
        /* flag to see if the new accelerator was in use by something */
        for (KeyEntry * ckeyEntry : generalEntries){
            if (shortcutString == ckeyEntry->valueStr){
                const QByteArray iid(nkeyEntry->gsSchema.toLatin1().data());
                QGSettings * settings = new QGSettings(iid);

                current->setText(settings->get(nkeyEntry->keyStr).toString());
                current->updateOldShow(settings->get(nkeyEntry->keyStr).toString());
                current->clearFocus();
                qDebug() << QString("The shortcut \"%1\" is already used for\n\"%2\",please reset!!!").arg(shortcutString).arg(ckeyEntry->keyStr);
                delete settings;
                settings = nullptr;
                return;
            }
        }
        const QByteArray iid(nkeyEntry->gsSchema.toLatin1().data());
        QGSettings * settings = new QGSettings(iid);

        settings->set(nkeyEntry->keyStr, shortcutString);
        delete settings;
        settings = nullptr;

        //更新
        for (int index = 0; index < generalEntries.count(); index++){
            if (nkeyEntry->keyStr == generalEntries[index]->keyStr){
                generalEntries[index]->valueStr = shortcutString;
            }
        }
    }else { //自定义快捷键的修改
        qDebug() << "custom key";

        if(shortcutString.isEmpty()){   //fn
            current->setText(nkeyEntry->bindingStr);
            current->updateOldShow(nkeyEntry->bindingStr);
            current->clearFocus();
            qDebug() << "Please try with a valid key.";
            return;
        }
        if(shortcutString.endsWith(">")){   //special key
            current->setText(nkeyEntry->bindingStr);
            current->updateOldShow(nkeyEntry->bindingStr);
            current->clearFocus();
            qDebug() << "Please try with a valid key value";
            return;
        }
        /* flag to see if the new accelerator was in use by something */
        for (KeyEntry * ckeyEntry : generalEntries){
            if (shortcutString == ckeyEntry->valueStr){
                current->setText(nkeyEntry->bindingStr);
                current->updateOldShow(nkeyEntry->bindingStr);
                current->clearFocus();
                qDebug() << QString("The shortcut \"%1\" is already used for\n\"%2\",please reset!!!").arg(shortcutString).arg(ckeyEntry->keyStr);
                return;
            }
        }
        if (keyCode.count() == 1){
            if (shortcutString.contains(QRegExp("[a-z]")) ||
                    shortcutString.contains(QRegExp("[0-9]")) ||
                    keyIsForbidden(shortcutString)){
                current->setText(nkeyEntry->bindingStr);
                current->updateOldShow(nkeyEntry->bindingStr);
                current->clearFocus();
                qDebug() << "Please try with a key such as Control, Alt or Shift at the same time.";
                return;
            }
        }
        if (keyCode.count() == 2){
            if (shortcutString == "<Control>F1"){
                current->setText(nkeyEntry->bindingStr);
                current->updateOldShow(nkeyEntry->bindingStr);
                current->clearFocus();
                qDebug() << "Please try with a valid key.";
                return;
            } else if (shortcutString.startsWith("<")){
                int len = shortcutString.length();
                for(int i = 0; i < len; i++){
                    if(shortcutString.at(i) == '>'){
                        len = len - i - 1;
                        break;
                    }
                }
                if(len > 3)
                {
                    current->setText(nkeyEntry->bindingStr);
                    current->updateOldShow(nkeyEntry->bindingStr);
                    current->clearFocus();
                    return;
                }
            }
        }
        if(keyCode.count() == 3){
            int len = shortcutString.length();
            int count = 0;
            for(int i = 0; i < len; i++){
                if(shortcutString.at(i) == '>'){
                    count += 1;
                    if(count == 2){
                        len = len - i - 1;
                        break;
                    }
                }
            }
            if(len > 3)
            {
                current->setText(nkeyEntry->bindingStr);
                current->updateOldShow(nkeyEntry->bindingStr);
                current->clearFocus();
                return;
            }
        }

        const QByteArray id(KEYBINDINGS_CUSTOM_SCHEMA);
        const QByteArray idd(nkeyEntry->gsPath.toLatin1().data());
        QGSettings * settings = new QGSettings(id, idd);
        settings->set(BINDING_KEY, shortcutString);
        delete settings;
        settings = nullptr;

        //更新
        for (int index = 0; index < customEntries.count(); index++){
            if (nkeyEntry->nameStr == customEntries[index]->nameStr){
                customEntries[index]->bindingStr = shortcutString;
            }
        }
    }

    current->setText(shortcutString);
    current->updateOldShow(shortcutString);
    //已经设置了值，清除焦点不再监听
    current->clearFocus();
}

/**
 * @brief Shortcut::getBindingName
 * @param keyCode
 * @return
 */
QString Shortcut::getBindingName(QList<int> keyCode){
    QStringList tmpList;
    for (int keycode : keyCode){
        if (keycode >= 16777216 && keycode <= 16777254){ //1677216=Escape; 16777254=ScrollLock
            if(keycode == 16777223 || keycode == 16777225){ // 16777223=Delete  16777225=Print
                tmpList.append(pKeyMap->keycodeTokeystring(keycode));
            }else {
                tmpList.append(QString("<%1>").arg(pKeyMap->keycodeTokeystring(keycode)));
            }
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

void Shortcut::shortcutChangedSlot(){
    qDebug() << "receive cloud service signal";
    for(int i = 0; i < ui->customListWidget->count(); i++){
        QListWidgetItem * obItem =  ui->customListWidget->takeItem(i);
        delete obItem;
        obItem = nullptr;
    }
    isCloudService = true;
    initFunctionStatus();
}


