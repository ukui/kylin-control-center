#include "dataformat.h"
#include "ui_dataformat.h"

#include <QDateTime>
#include <QDebug>

#define PANEL_GSCHEMAL   "org.ukui.panel.plugins"
#define CALENDAR_KEY     "calendar"
#define DAY_KEY          "firstday"
#define DATE_FORMATE_KEY "date"
#define TIME_KEY         "time"

DataFormat::DataFormat(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataFormat)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    QByteArray id(PANEL_GSCHEMAL);
    if(QGSettings::isSchemaInstalled(id)) {
        m_gsettings = new QGSettings(id)        ;
    }

    QFile QssFile("://combox.qss");
    QssFile.open(QFile::ReadOnly);

    if (QssFile.isOpen()){
        qss = QLatin1String(QssFile.readAll());
        QssFile.close();
    }


    initUi();
    initConnect();
    initComponent();
}

DataFormat::~DataFormat()
{
    delete ui;
    delete m_itimer;
}


void DataFormat::initUi() {    
//    this->setStyleSheet("background: #ffffff;");

    ui->calendarBox->setStyleSheet(qss);;
    ui->calendarBox->setView(new QListView());
    ui->calendarBox->setMaxVisibleItems(5);

    ui->dayBox->setStyleSheet(qss);;
    ui->dayBox->setView(new QListView());
    ui->dayBox->setMaxVisibleItems(5);

    ui->dateBox->setStyleSheet(qss);;
    ui->dateBox->setView(new QListView());
    ui->dateBox->setMaxVisibleItems(5);

    ui->timeBox->setStyleSheet(qss);;
    ui->timeBox->setView(new QListView());
    ui->timeBox->setMaxVisibleItems(5);


    ui->frame->setStyleSheet("QFrame{background: #ffffff; border: none; border-radius: 6px;}");
    //关闭按钮在右上角，窗体radius 6px，所以按钮只得6px
    ui->closeBtn->setIcon(QIcon("://img/titlebar/close.png"));
    ui->closeBtn->setStyleSheet("QPushButton#closeBtn{background: #ffffff; border: none; border-radius: 6px;}"
                                "QPushButton:hover:!pressed#closeBtn{background: #FA6056; border: none; border-top-left-radius: 2px; border-top-right-radius: 6px; border-bottom-left-radius: 2px; border-bottom-right-radius: 2px;}"
                                "QPushButton:hover:pressed#closeBtn{background: #E54A50; border: none; border-top-left-radius: 2px; border-top-right-radius: 6px; border-bottom-left-radius: 2px; border-bottom-right-radius: 2px;}");


    ui->calendarLabel->setStyleSheet("QLabel#calendarLabel{background: #F4F4F4;}");
    ui->dayLabel->setStyleSheet("QLabel#dayLabel{background: #F4F4F4;}");
    ui->dateLabel->setStyleSheet("QLabel#dateLabel{background: #F4F4F4;}");
    ui->timeLabel->setStyleSheet("QLabel#timeLabel{background: #F4F4F4;}");

    ui->calendarwidget->setStyleSheet("QWidget#calendarwidget{background: #F4F4F4; border-radius: 6px;}");
    ui->daywidget->setStyleSheet("QWidget#daywidget{background: #F4F4F4; border-radius: 6px;}");
    ui->datewidget->setStyleSheet("QWidget#datewidget{background: #F4F4F4; border-radius: 6px;}");
    ui->timewidget->setStyleSheet("QWidget#timewidget{background: #F4F4F4; border-radius: 6px;}");


    ui->cancelButton->setStyleSheet("QPushButton{background-color:#E5E7E9;border-radius:4px}"
                                   "QPushButton:hover{background-color: #3D6BE5;color:white;};border-radius:4px");
    ui->confirmButton->setStyleSheet("QPushButton{background-color:#E5E7E9;border-radius:4px}"
                                   "QPushButton:hover{background-color: #3D6BE5;color:white;};border-radius:4px");

    ui->calendarLabel->setText(tr("calendar"));
    ui->dayLabel->setText(tr("first day of week"));
    ui->dateLabel->setText(tr("date"));
    ui->timeLabel->setText(tr("time"));

    ui->calendarBox->addItem(tr("lunar"));
    ui->calendarBox->addItem(tr("solar calendar"));

    ui->dayBox->addItem(tr("monday"));
    ui->dayBox->addItem(tr("sunday"));        

    QString currentsecStr;
    QDateTime current = QDateTime::currentDateTime();

    currentsecStr = current.toString("yyyy/MM/dd ");
    ui->dateBox->addItem(currentsecStr);

    currentsecStr = current.toString("yyyy-MM-dd ");
    ui->dateBox->addItem(currentsecStr);

    QString timeStr;
    timeStr = current.toString("hh: mm : ss");
    ui->timeBox->addItem(timeStr);

    timeStr = current.toString("AP hh: mm : ss");
    ui->timeBox->addItem(timeStr);
}

void DataFormat::initConnect() {
    m_itimer = new QTimer();
    m_itimer->start(1000);

    connect(m_itimer,SIGNAL(timeout()), this, SLOT(datetime_update_slot()));
    connect(ui->calendarBox, SIGNAL(currentIndexChanged(int)), SLOT(calendar_change_slot(int)));
    connect(ui->dayBox, SIGNAL(currentIndexChanged(int)), SLOT(day_change_slot(int)));
    connect(ui->dateBox, SIGNAL(currentIndexChanged(int)), SLOT(date_change_slot(int)));
    connect(ui->timeBox, SIGNAL(currentIndexChanged(int)), SLOT(time_change_slot(int)));
    connect(ui->confirmButton, SIGNAL(clicked(bool)), SLOT(confirm_btn_slot()));
    connect(ui->cancelButton, SIGNAL(clicked()), SLOT(close()));
    connect(ui->closeBtn, &QPushButton::clicked, [=](bool checked){
        Q_UNUSED(checked)
        close();
    });

}

void DataFormat::initComponent() {
    if (!m_gsettings) {
        return ;
    }
    const QStringList list = m_gsettings->keys();

    if (!list.contains("calendar") || !list.contains("firstday")){
        return ;
    }

    QString clac = m_gsettings->get(CALENDAR_KEY).toString();
    if ("lunar" == clac) {
        ui->calendarBox->setCurrentIndex(0);
    } else {
        ui->calendarBox->setCurrentIndex(1);
    }

    QString day = m_gsettings->get(DAY_KEY).toString();
    if ("monday" == day) {
        ui->dayBox->setCurrentIndex(0);
    } else {
        ui->dayBox->setCurrentIndex(1);
    }

    QString dateFormat = m_gsettings->get(DATE_FORMATE_KEY).toString();
    if ("cn" == dateFormat) {
       ui->timeBox->setCurrentIndex(0);
    } else {
       ui->timeBox->setCurrentIndex(1);
    }
}

void DataFormat::writeGsettings(const QString &key, const QString &value) {
    if(!m_gsettings) {
        return ;
    }

    const QStringList list = m_gsettings->keys();
    if (!list.contains(key)) {
        return ;
    }
    m_gsettings->set(key,value);
}

void DataFormat::datetime_update_slot() {
    QString timeStr;
    QDateTime current = QDateTime::currentDateTime();

    timeStr = current.toString("hh: mm : ss");
    ui->timeBox->setItemText(0,timeStr);

    timeStr = current.toString("AP hh: mm : ss");
    ui->timeBox->setItemText(1,timeStr);
}

void DataFormat::calendar_change_slot(int index) {

}

void DataFormat::day_change_slot(int index) {

}

void DataFormat::date_change_slot(int index) {

}

void DataFormat::time_change_slot(int index) {

}

void DataFormat::confirm_btn_slot() {
    QString calendarValue;
    QString dayValue;
    QString dateValue;
    QString timeValue;

    if ( 0 == ui->calendarBox->currentIndex()) {
        calendarValue = "lunar";
    } else {
        calendarValue = "solarLunar";
    }

    if ( 0 == ui->dayBox->currentIndex()) {
        dayValue = "monday";
    } else {
        dayValue = "sunday";
    }

    if ( 0 == ui->dateBox->currentIndex()) {
        dateValue = "cn";
    } else {
        dateValue = "en";
    }

    if ( 0 == ui->timeBox->currentIndex()) {
        timeValue = "24";
    } else {
        timeValue = "12";
    }

    writeGsettings("calendar", calendarValue);
    writeGsettings("firstday", dayValue);
    writeGsettings("date", dateValue);
    writeGsettings("time", timeValue);

    emit this->dataChangedSignal();
}
