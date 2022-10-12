#include "mainwindow.h"

#include <QApplication>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //设置无窗口框架边界和隐藏任务栏图标
    w.setWindowFlags(Qt::FramelessWindowHint|Qt::Tool);
    //w.setAttribute(Qt::WA_TranslucentBackground);//设置背景透明
    //隐藏任务栏图标
    QString x = w.readini(w.ini,"/settings/x");
    QString y = w.readini(w.ini,"/settings/y");
    w.move(x.toInt(),y.toInt());
    w.setAttribute(Qt::WA_QuitOnClose,true);
    w.show();
    return a.exec();
}
