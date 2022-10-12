#include "internetwindow.h"
#include <QWebEngineCookieStore>
#include "ui_internetwindow.h"

InternetWindow::InternetWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::InternetWindow)
{
    ui->setupUi(this);
    //注：这里Debug模式可能会加载不出来网页，要换成Release模式
    ui->myWeb->setUrl(QUrl("https://bbs.mihoyo.com/ys/"));
}

InternetWindow::~InternetWindow()
{
    delete ui;
}

void InternetWindow::on_pushButton_clicked()
{

    //QWebEngineCookieStore *browser_cookie = ui->myWeb->page()->profile();
    ui->myWeb->page()->runJavaScript("function GetCookie(){return document.cookie}");
    ui->myWeb->page()->runJavaScript("GetCookie();",[this](const QVariant &v) {
        //qDebug() << v.toString();
        SendCookie(v);
    });
}

void InternetWindow::closeEvent(QCloseEvent *event)
{
    delete ui->myWeb;
}

void InternetWindow::SendCookie(QVariant a)
{
    emit sendCookieData(a.toByteArray());
    this->close();
    //qDebug() << a.toString();
}
