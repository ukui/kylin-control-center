#include "delegate.h"

Delegate::Delegate(QObject *parent):QStyledItemDelegate(parent)
{

}

Delegate::~Delegate()
{

}

void Delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {


    auto value = index.data(Qt::UserRole);
    QStyleOptionViewItem optionVI = option;
    if ((option.state & QStyle::State_MouseOver) && "title" == value.toString()) {
        optionVI.state &= (~QStyle::State_MouseOver);
    }

//    if ((option.state & QStyle::State_MouseOver) == QStyle::State_MouseOver && value.toString() == "title"){
//        auto text = index.data().toString();
//        qDebug() << "this is title"  << text << endl;
//        painter->save();
//        painter->setRenderHint(QPainter::Antialiasing);  // 反锯齿;
//        painter->setBrush(QBrush(Qt::transparent));
//        painter->drawText(option.rect.adjusted(3,0,0,0), text);
//        painter->restore();
//        return ;
//    }
    QStyledItemDelegate::paint(painter, optionVI, index);
}
