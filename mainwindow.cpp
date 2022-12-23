#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QMouseEvent>
#include <QMenu>
#include "internetwindow.h"
#include "genshin_setting.h"
#include <QSettings>
#include <QFile>
#include <QDateTime>
#include <QRandomGenerator>
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#define AUTORUN "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"
#define KEY "GenshinDesktop"
int max_resin = 160;
int resintime = 99999; //回满体力的时间戳
bool resin_full = false ;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_TranslucentBackground);
    //托盘操作
    trayIcon = new QSystemTrayIcon(this);
    trayIcon -> setIcon(QIcon(":/img/settings.ico"));
    /*给是否移动的标志初始化为false*/
    m_move = false;

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    //painter.
}
//初始化
void MainWindow::initialize(int argc, char *argv[])
{
    qDebug() << argc;
    for(int i = 0; i < argc; i++)
    {
        if(i == 0)
            this->arg << QString::fromLocal8Bit(argv[i]);//防止中文路径报错
        else
            this->arg << argv[i];
    }
    //traytoast("11", arg.at(0));
    this -> start_path = this->arg.at(0).left(this->arg.at(0).lastIndexOf("\\") + 1);
    this -> ini = this -> start_path + "setting";
    this -> user_data = this -> start_path + "user.data" ;//设置cookie路径
    move(readini(this->ini,"/settings/x").toInt(), readini(this->ini,"/settings/y").toInt());
    get_user_info();
    qDebug() << start_path << Qt::endl
                << ini << Qt::endl
                << user_data << Qt::endl;
}


void MainWindow::mousePressEvent(QMouseEvent *event)
{
    //窗口移动
    if (event->button() == Qt::LeftButton)
    {
       m_move = true;
       m_startPoint = event->globalPos();
       m_windowPoint = this->frameGeometry().topLeft();
    }
    //这里是右键菜单
    else if(event->button() == Qt::RightButton)
    {
        QMenu menu;
        QAction* act1 = menu.addAction("登录");
        connect(act1, &QAction::triggered, this, &MainWindow::login_window);
        QAction* act2 = menu.addAction("刷新");
        connect(act2, &QAction::triggered, this, &MainWindow::get_user_info);
        QAction* act3 = menu.addAction("设置");
        connect(act3, &QAction::triggered, this, &MainWindow::settings_window);
        QAction* act4 = menu.addAction("退出");
        connect(act4, &QAction::triggered, this, [=]()
        {
            //记住退出位置
            int x = this->geometry().x();
            int y = this->geometry().y();
            writeini(ini,"/settings/x",QString::number(x));
            writeini(ini,"/settings/y",QString::number(y));
            this->close();
        });
        menu.exec(QCursor::pos());
    }
}
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        QPoint relativePos = event->globalPos() - m_startPoint;
        this->move(m_windowPoint + relativePos );
        int x = this->geometry().x();
        int y = this->geometry().y();
        writeini(ini,"/settings/x",QString::number(x));
        writeini(ini,"/settings/y",QString::number(y));
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_move = false;
    }
}
//窗口移动结束
//登录窗口
void MainWindow::login_window()
{
    InternetWindow *cookieWindow = new InternetWindow;
    cookieWindow->setAttribute(Qt::WA_QuitOnClose,false);
    cookieWindow->show();
    connect(cookieWindow,&InternetWindow::sendCookieData,this,&MainWindow::cookieDatas);
}
//设置窗口
void MainWindow::settings_window()
{
    genshin_setting *settingwindow = new genshin_setting;
    settingwindow->setAttribute(Qt::WA_QuitOnClose,false);
    settingwindow->show();
}
//托盘toast
void MainWindow::traytoast(QString title,QString toast)
{
    trayIcon -> show();
    trayIcon -> showMessage(title,toast);
    trayIcon -> hide();
}
//接受cookie
void MainWindow::cookieDatas(QByteArray cookie)
{
    QFile file(user_data);
    //先写进文件
    //qDebug()<<cookie;
    file.open(QIODevice::WriteOnly);
    file.write(cookie);
    file.close();
    qDebug()<<QSslSocket::sslLibraryBuildVersionString();
    get_user(cookie);
}
//
QString MainWindow::get_DS(QString region , QString uid)
{
    QString salt = "xV8v4Qu54lUKrEYFZkJhB8cuOh9Asafs";
    QString t = QString::number(QDateTime::currentSecsSinceEpoch());
    int random = QRandomGenerator::global()->bounded(100000);
    QString r = QString::number(random + 100000);
    QString q = "role_id="+ uid + "&server=" + region;
    QString DS = "salt=" + salt + "&t=" + t + "&r=" + r +"&b=&q=" + q;
    QString DSMD5 = QCryptographicHash::hash(DS.toLatin1(),QCryptographicHash::Md5).toHex();
    QString DSresult = t + "," + r + "," + DSMD5;
    return DSresult;
}
//写到ini
void MainWindow::writeini(QString path,QString name,QString value)
{
    /*
     *path:文件路径
     *name:节 例如/country/city
     *value:值
     */
    QSettings *Write = new QSettings(path, QSettings::IniFormat);
    Write->setValue(name, value);
    delete Write;
}
//读取ini文件
QString MainWindow::readini(QString path, QString name)
{
   /*
    *path:文件路径
    *name:节 例如/country/city
    */
   QSettings *read = new QSettings(path, QSettings::IniFormat);
   //将读取到的ini文件保存在QString中，先取值，然后通过toString()函数转换成QString类型
   QString result = read->value(name).toString();
   delete read;
   return result;
}
//进行get获取个人信息 cookie需要cookie_token_v2
void MainWindow::get_user(QByteArray cookies)
{
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    QNetworkRequest request;
    connect(manager,&QNetworkAccessManager::finished,this,&MainWindow::get_user_reply);
    request.setUrl(QUrl("https://api-takumi.mihoyo.com/binding/api/getUserGameRolesByCookie?game_biz=hk4e_cn"));
    request.setRawHeader("Cookie",cookies);
    qDebug() << cookies ;
    manager->get(request);
}
void MainWindow::get_user_reply(QNetworkReply* reply)
{
    QString a = reply->readAll().data();
    if(get_uid(a))
    {
        get_user_info();
    }
}
//这里是获取uid和名称
bool MainWindow::get_uid(QString data)
{
    if(data.contains("game_uid",Qt::CaseSensitive) && data.contains("nickname",Qt::CaseSensitive))
    {
        //int uid , name ;
        QString region , uid , name;
        region = data.mid(data.indexOf("region")+9,data.indexOf("game_uid")-12-data.indexOf("region"));
        uid = data.mid(data.indexOf("game_uid")+11,data.indexOf("nickname")-14-data.indexOf("game_uid"));
        name  = data.mid(data.indexOf("nickname") + 11,data.indexOf("level") - 14 - data.indexOf("nickname"));
        writeini(ini,"/user/region",region);
        writeini(ini,"/user/uid",uid);
        writeini(ini,"/user/name",name);
        traytoast("绑定成功 服务器:" + region,"uid:" + uid + " " + name);
        return true;
    }
    else
    {
        traytoast("绑定失败","数据错误，请重新登录");
        return false;
    }
}
//获取树脂方面的信息
void MainWindow::get_user_info()
{
    qDebug() << "get_user_info";
    QByteArray cookie;
    QString region , uid , strurl , DS;
    QFile file(user_data);
    file.open(QFile::ReadOnly);
    cookie = file.readAll();
    file.close();
    region = readini(ini , "/user/region");
    uid = readini(ini,"/user/uid");
    qDebug() << cookie;
    //进行get操作
    strurl = "https://api-takumi-record.mihoyo.com/game_record/app/genshin/api/dailyNote?role_id=" + uid + "&server=" + region;
    DS = get_DS(region , uid);
    qDebug() << DS;
    QNetworkAccessManager* Qmanager = new QNetworkAccessManager(this);
    QNetworkRequest Qrequest;
    connect(Qmanager,&QNetworkAccessManager::finished,this,&MainWindow::update_user_data);
    Qrequest.setUrl(QUrl(strurl));
    //Header大队
    Qrequest.setRawHeader("Access-Control-Request-Headers","ds,x-rpc-app_version,x-rpc-client_type,x-rpc-page");
    Qrequest.setRawHeader("Access-Control-Request-Method","GET");
    Qrequest.setRawHeader("Accept","application/json, text/plain, */*");
    Qrequest.setRawHeader("Accept-Encoding","deflate");
    Qrequest.setRawHeader("Accept-Language","zh-CN,zh;q=0.9,en-US;q=0.8,en;q=0.7");
    Qrequest.setRawHeader("Connection","keep-alive");
    Qrequest.setRawHeader("DS",DS.toLocal8Bit());
    Qrequest.setRawHeader("Host","api-takumi-record.mihoyo.com");
    Qrequest.setRawHeader("Origin","https://webstatic.mihoyo.com");
    Qrequest.setRawHeader("Referer","https://webstatic.mihoyo.com/");
    Qrequest.setRawHeader("Sec-Fecth-Dest","empty");
    Qrequest.setRawHeader("Sec-Fecth-Mode","cors");
    Qrequest.setRawHeader("Sec-Fetch-Site","cookiessame-site");
    Qrequest.setRawHeader("X-Requested-With","com.mihoyo.hyperion");
    Qrequest.setRawHeader("x-rpc-app_version","2.35.2");
    Qrequest.setRawHeader("x-rpc-client_type","5");
    Qrequest.setRawHeader("x-rpc-page","/ys/daily/");
    Qrequest.setRawHeader("Cookie",cookie);
    Qmanager->get(Qrequest);
}
//更新数据
void MainWindow::update_user_data(QNetworkReply* reply)
{
    QByteArray data = reply->readAll();

    if(this->arg.contains("-debug"))
    {
        out_log(data);
    }
    qDebug() << Qt::endl << data ;

    //重写
    int current_resin,   finished_task_num, total_task_num;
    QString resin_recovery_time;
    int current_expedition_num, max_expedition_num, current_home_coin, max_home_coin;
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject jsonObj = doc.object();
    if (jsonObj.value("message").toString() == "OK")
    {
        QJsonObject dataObj = jsonObj.value("data").toObject();
        current_resin = dataObj.value("current_resin").toInt();
        max_resin = dataObj.value("max_resin").toInt();
        resin_recovery_time = dataObj.value("resin_recovery_time").toString();
        finished_task_num = dataObj.value("finished_task_num").toInt();
        total_task_num = dataObj.value("total_task_num").toInt();
        current_expedition_num = dataObj.value("current_expedition_num").toInt();
        max_expedition_num = dataObj.value("max_expedition_num").toInt();
        current_home_coin = dataObj.value("current_home_coin").toInt();
        max_home_coin = dataObj.value("max_home_coin").toInt();
        ui->label_5->setText(QString::number(current_resin) + "/" + QString::number(max_resin));
        ui->label_6->setText(QString::number(current_expedition_num) + "/" + QString::number(max_expedition_num));
        ui->label_7->setText(QString::number(finished_task_num) + "/" + QString::number(total_task_num));
        ui->label_8->setText(QString::number(current_home_coin) + "/" + QString::number(max_home_coin));
        resintime = resin_recovery_time.toInt() + QDateTime::currentSecsSinceEpoch();
        qDebug() << "1111"<< Qt::endl<<Qt::endl<<dataObj.value("current_resin").toInt() << Qt::endl<< resintime;
        resin_time();
    }
    else
    {
        traytoast("错误", jsonObj.value("message").toString());
    }
}
//体力倒计时
void MainWindow::resin_time()
{
    resin_timer = new QTimer(this);
    resin_timer->setTimerType(Qt::PreciseTimer);
    connect(resin_timer, &QTimer::timeout, this, &MainWindow::resin_update);
    resin_timer->start( 1000 );
}
//体力更新
void MainWindow::resin_update()
{
    QString label_resin;
    //qDebug() << resintime << QDateTime::currentSecsSinceEpoch();
    if(resintime > QDateTime::currentSecsSinceEpoch())
    {
        int max = max_resin;
        int resin_time = resintime - QDateTime::currentSecsSinceEpoch();
        int last_resin = resin_time / 480;
        int resin = max - last_resin - 1;
        label_resin = QString::number(resin) + "/" + QString::number(max_resin);
    }
    else
        label_resin = "160/160";
    if(label_resin != ui->label_5->text())
        ui->label_5->setText(label_resin);
    //qDebug() << resintime << Qt::endl;
}

void MainWindow::out_log(QByteArray log)
{
    QDateTime dateTime(QDateTime::currentDateTime());
    QString path = this->start_path + "log";
    QString path_2 = path + "\\" + dateTime.toString("yy-MM-dd");
    qDebug() << path;
    QDir dir1(path);
    if(!dir1.exists())
        dir1.mkdir(path);
    QDir dir2(path_2);
    if(!dir2.exists())
        dir2.mkdir(path_2);
    QFile file(path_2 + "\\" + dateTime.toString("hh-mm-ss-") + QString::number(QDateTime::currentSecsSinceEpoch()) + ".txt");
    file.open(QIODevice::ReadWrite|QIODevice::Text);
    file.write(log);
    file.close();
}
