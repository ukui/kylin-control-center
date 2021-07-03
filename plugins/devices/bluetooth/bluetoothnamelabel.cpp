#include "bluetoothnamelabel.h"

BluetoothNameLabel::BluetoothNameLabel(QWidget *parent, int x, int y):
    QWidget(parent)
{
//    qDebug() << Q_FUNC_INFO << x << y;

    this->setAutoFillBackground(true);
    this->setObjectName("BluetoothNameLabel");
    this->setStyleSheet("QWidget{border: none;border-radius:2px;}");
    this->setFixedSize(x,y);
    hLayout = new QHBoxLayout(this);
    hLayout->setContentsMargins(5,0,5,0);
    hLayout->setSpacing(0);

    m_label = new QLabel(this);
    m_label->resize(10,10);
    m_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    //m_label->setGeometry(2,2,this->width()-3,this->height()-3);
//    m_label->setStyleSheet("QLabel{\
//                             width: 250px;\
//                             height: 20px;\
//                             font-family: PingFangSC-Regular, PingFang SC;\
//                             font-weight: 420;\
//                             line-height: 20px;}");
//    m_label->setIndent(2);
//    m_label->setStyleSheet("QLabel{\
//                             width: 280px;\
//                             height: 20px;\
//                             font-family: PingFangSC-Regular, PingFang SC;\
//                             font-weight: 420;\
//                             line-height: 20px;}");
    hLayout->addWidget(m_label);
    icon_pencil = new QLabel(this);
    icon_pencil->setGeometry(this->width()-200,2,43,this->height()-3);
    icon_pencil->setPixmap(QIcon::fromTheme("document-edit-symbolic").pixmap(20,20));
    icon_pencil->setToolTip(tr("Double-click to change the device name"));
    hLayout->addWidget(icon_pencil);
    hLayout->addStretch(1);

    m_lineedit = new QLineEdit(this);
    m_lineedit->setEchoMode(QLineEdit::Normal);
    //m_lineedit->setAlignment(Qt::AlignCenter);
    m_lineedit->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    connect(m_lineedit,&QLineEdit::editingFinished,this,&BluetoothNameLabel::LineEdit_Input_Complete);
    m_lineedit->setGeometry(2,2,this->width()-3,this->height()-3);
    m_lineedit->setVisible(false);

    if(QGSettings::isSchemaInstalled("org.ukui.style")){
        settings = new QGSettings("org.ukui.style");
        if(settings->get("style-name").toString() == "ukui-black" || settings->get("style-name").toString() == "ukui-dark")
            style_flag = true;
        else
            style_flag = false;

        switch (settings->get("systemFontSize").toInt()) {
        case 11:
        case 12:
        case 13:
            font_width = 100;
            break;
        case 14:
            font_width = 70;
            break;
        case 15:
        case 16:
            font_width = 50;
            break;
        default:
            break;
        }
        qDebug() << Q_FUNC_INFO << connect(settings,&QGSettings::changed,this,&BluetoothNameLabel::settings_changed);
    }
}

BluetoothNameLabel::~BluetoothNameLabel()
{

}

void BluetoothNameLabel::set_dev_name(const QString &dev_name)
{
    QFont ft;
    QFontMetrics fm(ft);
    //QString text = fm.elidedText(dev_name, Qt::ElideMiddle, font_width);
    QString text = fm.elidedText(dev_name, Qt::ElideMiddle, this->width());
    //m_label->setText(tr("Can now be found as \"%1\"").arg(text));
    m_label->setText(text);
    m_label->setToolTip(tr("Can now be found as \"%1\"").arg(dev_name));
    //m_label->adjustSize();
    m_label->update();

    device_name = dev_name;
}

void BluetoothNameLabel::dev_name_limit_fun()
{
    if (!messagebox) {
        messagebox = new QMessageBox(QMessageBox::NoIcon,
                                     tr("Tip"),
                                     tr("The length of the device name does not exceed %1 characters !").arg(QString::number(DEVNAMELENGTH)),
                                     QMessageBox::Ok);

        if (messagebox->exec() == QMessageBox::Ok || messagebox->exec() == QMessageBox::Close) {
            set_label_text(device_name);
            delete messagebox;
            messagebox = NULL;
        }
    }
}

void BluetoothNameLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    m_label->setVisible(false);
    icon_pencil->setVisible(false);

    m_lineedit->setText(device_name);
    m_lineedit->setVisible(true);
    m_lineedit->setFocus();
}

void BluetoothNameLabel::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);

    if(!m_lineedit->isVisible())
        this->setStyleSheet("QWidget{border:none;border-radius:2px;}");
}

void BluetoothNameLabel::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
//    QPalette palette;
//    palette.setColor(QPalette::Background, QColor(Qt::white));
//    this->setPalette(palette);
//    this->update();

    if(style_flag)
        this->setStyleSheet("QWidget#BluetoothNameLabel{background-color:black;border:none;border-radius:2px;}");
    else
        this->setStyleSheet("QWidget#BluetoothNameLabel{background-color:white;border:none;border-radius:2px;}");
}

void BluetoothNameLabel::LineEdit_Input_Complete()
{
    qDebug() << Q_FUNC_INFO;
    if (m_lineedit->text().isEmpty())
    {
        m_lineedit->setText(device_name);
        m_lineedit->update();
        this->setStyleSheet("QWidget{border:none;border-radius:2px;}");
    }

    if(device_name == m_lineedit->text()){
        set_label_text(device_name);
    }else{
        if (m_lineedit->text().length() > DEVNAMELENGTH) {
            dev_name_limit_fun();
        } else {
            device_name = m_lineedit->text();
            emit this->send_adapter_name(m_lineedit->text());
        }
    }
    this->setStyleSheet("QWidget{border:none;border-radius:2px;}");
}

void BluetoothNameLabel::set_label_text(const QString &value)
{
    m_lineedit->setVisible(false);

    QFont ft;
    QFontMetrics fm(ft);
    //QString text = fm.elidedText(m_lineedit->text(), Qt::ElideMiddle, font_width);
    QString text = fm.elidedText(value, Qt::ElideMiddle, this->width());
    //m_label->setText(tr("Can now be found as \"%1\"").arg(text));
    m_label->setText(text);
    m_label->setToolTip(tr("Can now be found as \"%1\"").arg(device_name));

    m_label->setVisible(true);
    icon_pencil->setVisible(true);
}

void BluetoothNameLabel::settings_changed(const QString &key)
{
    qDebug() << Q_FUNC_INFO <<key;
    if(key == "styleName"){
        if(settings->get("style-name").toString() == "ukui-black" || settings->get("style-name").toString() == "ukui-dark")
            style_flag = true;
        else
            style_flag = false;
    }else if(key == "systemFontSize"){
        QFont ft;
        ft.setPixelSize(settings->get("systemFontSize").toInt());
        switch (settings->get("systemFontSize").toInt()) {
        case 11:
        case 12:
        case 13:
            font_width = 100;
            break;
        case 14:
            font_width = 70;
            break;
        case 15:
        case 16:
            font_width = 50;
            break;
        default:
            break;
        }
        QFontMetrics fm(ft);
        //QString text = fm.elidedText(device_name, Qt::ElideMiddle, font_width);
        QString text = fm.elidedText(device_name, Qt::ElideMiddle,this->width());
        m_label->setText(text);
        //m_label->setText(tr("Can now be found as \"%1\"").arg(text));
        m_label->setVisible(true);
        icon_pencil->setVisible(true);
    }
}
