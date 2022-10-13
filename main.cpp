#include "mainwindow.h"

#include <QApplication>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.read_start_path();
    //设置无窗口框架边界和隐藏任务栏图标
    //w.setAttribute(Qt::WA_TranslucentBackground);//设置背景透明
    w.setWindowFlags(Qt::FramelessWindowHint|Qt::Tool);
    //隐藏任务栏图标
    QString x = w.readini(w.ini,"/settings/x");
    QString y = w.readini(w.ini,"/settings/y");
    w.move(x.toInt(),y.toInt());
    w.setAttribute(Qt::WA_QuitOnClose,true);
    w.get_user_info();
    w.show();
    return a.exec();
}
