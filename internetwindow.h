#ifndef INTERNETWINDOW_H
#define INTERNETWINDOW_H

#include <QMainWindow>
#include <QWebEngineView>
namespace Ui {
class InternetWindow;
}

class InternetWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit InternetWindow(QWidget *parent = nullptr);
    ~InternetWindow();

protected slots:
    void on_pushButton_clicked();

signals:
    void sendCookieData(QByteArray);
private:
    Ui::InternetWindow *ui;
    void closeEvent(QCloseEvent* event);
    void SendCookie(QVariant);
};

#endif // INTERNETWINDOW_H
