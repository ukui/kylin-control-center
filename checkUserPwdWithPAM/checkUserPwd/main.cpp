#include "widget.h"

#include <QCoreApplication>

#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Widget w;
    if (argc == 3){
        w.pwdCheck(argv[1], argv[2]);
    }

    return a.exec();
}
