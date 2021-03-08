#include "bluetoothnamelabel.h"

BluetoothNameLabel::BluetoothNameLabel(QWidget *parent, int x, int y):
    QWidget(parent)
{
//    qDebug() << Q_FUNC_INFO << x << y;

    this->setAutoFillBackground(true);
    this->setStyleSheet("QWidget{border: none;border-radius:2px;}");
    this->setFixedSize(x,y);

    m_label = new QLabel(this);
    m_label->setAlignment(Qt::AlignCenter);
    m_label->setGeometry(2,2,this->width()-3,this->height()-3);
    m_label->setStyleSheet("QLabel{\
                             width: 214px;\
                             height: 20px;\
                             font-size: 14px;\
                             font-family: PingFangSC-Regular, PingFang SC;\
                             font-weight: 400;\
                             line-height: 20px;}");

    m_lineedit = new QLineEdit(this);
    m_lineedit->setEchoMode(QLineEdit::Normal);
    m_lineedit->setAlignment(Qt::AlignCenter);
    connect(m_lineedit,&QLineEdit::editingFinished,this,&BluetoothNameLabel::LineEdit_Input_Complete);
    m_lineedit->setGeometry(2,2,this->width()-3,this->height()-3);
    m_lineedit->setVisible(false);

    if(QGSettings::isSchemaInstalled("org.ukui.style")){
        settings = new QGSettings("org.ukui.style");
        if(settings->get("style-name").toString() == "ukui-black" || settings->get("style-name").toString() == "ukui-dark")
            style_flag = true;
        else
            style_flag = false;
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
    QString text = fm.elidedText(dev_name, Qt::ElideRight, 150);
    m_label->setText(tr("Can now be found as ")+"\""+text+"\"");

    m_label->update();

    device_name = dev_name;
}

void BluetoothNameLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    m_label->setVisible(false);

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
        this->setStyleSheet("QWidget{background-color:black;border:none;border-radius:2px;}");
    else
        this->setStyleSheet("QWidget{background-color:white;border:none;border-radius:2px;}");
}

void BluetoothNameLabel::LineEdit_Input_Complete()
{
    qDebug() << Q_FUNC_INFO;
    if(device_name == m_lineedit->text()){
        set_label_text(device_name);
    }else{
        device_name = m_lineedit->text();
        emit this->send_adapter_name(m_lineedit->text());
    }
    this->setStyleSheet("QWidget{border:none;border-radius:2px;}");
}

void BluetoothNameLabel::set_label_text(const QString &value)
{
    m_lineedit->setVisible(false);

    QFont ft;
    QFontMetrics fm(ft);
    QString text = fm.elidedText(m_lineedit->text(), Qt::ElideRight, 150);
    m_label->setText(tr("Can now be found as ")+"\""+text+"\"");
    m_label->setVisible(true);
}

void BluetoothNameLabel::settings_changed(const QString &key)
{
    qDebug() << Q_FUNC_INFO <<key;
    if(key == "styleName"){
        if(settings->get("style-name").toString() == "ukui-black" || settings->get("style-name").toString() == "ukui-dark")
            style_flag = true;
        else
            style_flag = false;
    }
}
