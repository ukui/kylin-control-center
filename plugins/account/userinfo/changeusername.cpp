#include "changeusername.h"
#include "ui_changeusername.h"

#include <QDebug>


extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);

ChangeUserName::ChangeUserName(QStringList usernames, QStringList realnames, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangeUserName),
    _usernames(usernames),
    _realnames(realnames)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);

    ui->saveBtn->setEnabled(false);

    connect(ui->lineEdit, &QLineEdit::textChanged, this, [=](QString txt){

        if (_usernames.contains(txt) || _realnames.contains(txt)){
            ui->tipLabel->setText(tr("Name already in use, change another one."));
        } else {
            ui->tipLabel->setText(tr(""));
        }


        if (!txt.isEmpty() && ui->tipLabel->text().isEmpty()){
            ui->saveBtn->setEnabled(true);
        } else {
            ui->saveBtn->setEnabled(false);
        }
    });


    connect(ui->cancelBtn, &QPushButton::clicked, [=]{
        close();
    });

    connect(ui->saveBtn, &QPushButton::clicked, [=]{
        emit sendNewName(ui->lineEdit->text());
        close();
    });
}

ChangeUserName::~ChangeUserName()
{
    delete ui;
}

void ChangeUserName::paintEvent(QPaintEvent *event) {
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
