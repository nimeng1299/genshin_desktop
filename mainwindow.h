#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSystemTrayIcon>
#include <QTimer>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void paintEvent(QPaintEvent *);
    void writeini(QString,QString,QString);
    QString readini(QString,QString);
    void get_user(QByteArray);
    void get_user_reply(QNetworkReply* reply);
    bool get_uid(QString);
    void traytoast(QString,QString);
    void login_window();
    void settings_window();
    void get_user_info();
    void update_user_data(QNetworkReply* reply);
    void resin_time();
    void resin_update();
    QString ini = "setting";

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

protected slots:
    void cookieDatas(QByteArray);

private:
    Ui::MainWindow *ui;
    QString get_DS(QString,QString);
    bool m_move;
    QPoint m_startPoint;
    QPoint m_windowPoint;
    QSystemTrayIcon *trayIcon;
    QTimer *resin_timer;
};
#endif // MAINWINDOW_H
