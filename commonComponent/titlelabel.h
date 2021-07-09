#ifndef TITLE_LABEL_H
#define TITLE_LABEL_H

#include <QLabel>

class TitleLabel : public QLabel
{
    Q_OBJECT
public:
    TitleLabel(QWidget *parent = nullptr);
    ~TitleLabel();
};

#endif // TITLE_LABEL_H
