#include <presslabel.h>
#include "screensaver.h"

PressLabel::PressLabel(QWidget *parent)
    :QLabel(parent)
{

}

PressLabel::~PressLabel()
{

}

void PressLabel::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if(event->button() == Qt::LeftButton) {
        PreviewWindow::previewScreensaver();
    }
}
