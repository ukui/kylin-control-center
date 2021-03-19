#include "leftmenulist.h"

#include <QDebug>

LeftMenuList::LeftMenuList(QWidget *parent) : QListWidget(parent) {

}

void LeftMenuList::resizeEvent(QResizeEvent *event) {
    int maxItemWidth = 0;
    for (int i = 0; i < this->count(); i++) {
        QWidget *item = this->itemWidget(this->item(i));
        if (item->width() > maxItemWidth) {
            maxItemWidth = item->width();
        }
    };
    if (maxItemWidth < 100) {
        this->setMinimumWidth(maxItemWidth);
    }

    QListWidget::resizeEvent(event);
}
