#include "mainwindow.h"

#include <QApplication>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.initialize(argc, argv);
    //设置无窗口框架边界和隐藏任务栏图标
    //w.setAttribute(Qt::WA_TranslucentBackground);//设置背景透明
    w.setWindowFlags(Qt::FramelessWindowHint|Qt::Tool);
    //隐藏任务栏图标
    w.setAttribute(Qt::WA_QuitOnClose,true);
    w.show();
    return a.exec();
}
