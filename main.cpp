#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include <QtSql>
#include <QtDebug>
#include "keyBoard.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    keyBoard keyBoard;
    keyBoard.hide();
    //QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    MainWindow w;
    w.show();

    return a.exec();
}
