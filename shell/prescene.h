#ifndef PRESCENE_H
#define PRESCENE_H

#include <QSize>
#include <QWidget>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>


class PreScene : public QWidget
{
public:
    PreScene(QLabel *label, QSize size, QWidget * parent = nullptr);
    ~PreScene();

private:
    QSize m_size;
    QHBoxLayout * m_hlayout;

    QVBoxLayout * m_vlayout;
    QWidget * titlebar;

    QLabel  * mTitleIcon;
    QLabel  * titleLabel;
    QLabel  * logoLabel;
    QHBoxLayout * m_logoLayout = nullptr;

protected:
    void paintEvent(QPaintEvent *event);
private:
    const QPixmap loadSvg(const QString &fileName);
};

#endif // PRESCENE_H
