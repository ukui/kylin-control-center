#include "item_list.h"

item_list::item_list(QListWidget *parent,int itemssize) : QListWidget(parent) {
    this->parent();
    itemsize = varmap.size();
    for(int cur_ptr = 0; cur_ptr < itemsize; cur_ptr ++) {
        itempack[cur_ptr] = new network_item;
        items[cur_ptr] = new QListWidgetItem;
        items[cur_ptr]->setSizeHint(QSize(200, 50));
        items[cur_ptr]->setFlags(items[cur_ptr]->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
        itempack[cur_ptr]->set_itemname(varmap[cur_ptr]);
        itempack[cur_ptr]->get_swbtn()->set_id(cur_ptr);
        QSize size = items[cur_ptr]->sizeHint();
        this->addItem(items[cur_ptr]);
        itempack[cur_ptr]->get_widget()->setSizeIncrement(size.width(), 56);
        setItemWidget(items[cur_ptr], itempack[cur_ptr]->get_widget());
    }
    setFrameShape(QListWidget::NoFrame);
    //customize the script on/off area

    //decorate the widget
    this->setSpacing(1);
}

network_item*  item_list::get_item(int cur) {
    return itempack[cur];
}

void item_list::add_item(QString item_name) {
    itemsize = itemsize + 1;
    itempack[itemsize - 1] = new network_item;
    items[itemsize - 1] = new QListWidgetItem;
    items[itemsize - 1]->setSizeHint(QSize(200, 50));
    items[itemsize - 1]->setFlags(items[itemsize - 1]->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
    itempack[itemsize - 1]->set_itemname(item_name);
    QSize size = items[itemsize - 1]->sizeHint();
    this->addItem(items[itemsize - 1]);
    itempack[itemsize  - 1]->get_widget()->setSizeIncrement(size.width(), 56);
    setItemWidget(items[itemsize - 1], itempack[itemsize - 1]->get_widget());
}


QStringList item_list::get_list() {
    return varmap;
}
