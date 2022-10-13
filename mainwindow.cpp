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
#define AUTORUN "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"
#define KEY "GenshinDesktop"
QString max_resin = "160";
int resintime = 99999;
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
//进行get获取个人信息
void MainWindow::get_user(QByteArray cookies)
{
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    QNetworkRequest request;
    connect(manager,&QNetworkAccessManager::finished,this,&MainWindow::get_user_reply);
    request.setUrl(QUrl("https://api-takumi.mihoyo.com/binding/api/getUserGameRolesByCookie?game_biz=hk4e_cn"));
    request.setRawHeader("Cookie",cookies);
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
    qDebug() << data;
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
    QByteArray cookie;
    QString region , uid , strurl , DS;
    QFile file(user_data);
    file.open(QFile::ReadOnly);
    cookie = file.readAll();
    file.close();
    region = readini(ini , "/user/region");
    uid = readini(ini,"/user/uid");
    //进行get操作
    strurl = "https://api-takumi-record.mihoyo.com/game_record/app/genshin/api/dailyNote?role_id=" + uid + "&server=" + region;
    DS = get_DS(region , uid);
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
    QString data = reply->readAll().data();
    qDebug() << Qt::endl << data ;
    QString current_resin,  resin_recovery_time, finished_task_num, total_task_num;
    QString current_expedition_num, max_expedition_num, current_home_coin, max_home_coin;
    //name  = data.mid(data.indexOf("nickname") + 11,data.indexOf("level") - 14 - data.indexOf("nickname"));
    QString message = data.mid(data.indexOf("message") + 10,data.indexOf("data\"") - 13 - data.indexOf("message"));
    qDebug()<< message;
    if (message == "OK")
    {
        current_resin = data.mid(data.indexOf("current_resin") + 15,data.indexOf("max_resin") - 17 - data.indexOf("current_resin"));
        max_resin = data.mid(data.indexOf("max_resin") + 11,data.indexOf("resin_recovery_time") - 13 - data.indexOf("max_resin"));//这个在全局变量里了
        resin_recovery_time = data.mid(data.indexOf("resin_recovery_time") + 22,data.indexOf("finished_task_num") - 25 - data.indexOf("resin_recovery_time"));
        finished_task_num = data.mid(data.indexOf("finished_task_num") + 19,data.indexOf("total_task_num") - 21 - data.indexOf("finished_task_num"));
        total_task_num = data.mid(data.indexOf("total_task_num") + 16,data.indexOf("is_extra_task_reward_received") - 18 - data.indexOf("total_task_num"));
        current_expedition_num = data.mid(data.indexOf("current_expedition_num") + 24,data.indexOf("max_expedition_num") - 26 - data.indexOf("current_expedition_num"));
        max_expedition_num = data.mid(data.indexOf("max_expedition_num") + 20,data.indexOf("expeditions") - 22 - data.indexOf("max_expedition_num"));
        current_home_coin = data.mid(data.indexOf("current_home_coin") + 19,data.indexOf("max_home_coin") - 21 - data.indexOf("current_home_coin"));
        max_home_coin = data.mid(data.indexOf("max_home_coin") + 15,data.indexOf("home_coin_recovery_time") - 17 - data.indexOf("max_home_coin"));
        ui->label_5->setText(current_resin + "/" + max_resin);
        ui->label_6->setText(current_expedition_num + "/" + max_expedition_num);
        ui->label_7->setText(finished_task_num + "/" + total_task_num);
        ui->label_8->setText(current_home_coin + "/" + max_home_coin);
        resintime = resin_recovery_time.toInt();
        qDebug() <<current_resin << Qt::endl<< resintime;
        resin_time();
    }
    else
        traytoast("注意","获取信息失败，请刷新或重新登录");
}
//体力倒计时
void MainWindow::resin_time()
{
    resin_timer = new QTimer(this);
    connect(resin_timer, &QTimer::timeout, this, &MainWindow::resin_update);
    resin_timer->start( 1000 );
}
//体力更新
void MainWindow::resin_update()
{
    resintime = resintime - 1;
    int max = max_resin.toInt();
    int last_resin = resintime / 480;
    int resin = max - last_resin - 1;
    QString label_resin = QString::number(resin) + "/" + max_resin;
    if(label_resin != ui->label_5->text())
        ui->label_5->setText(label_resin);
    //qDebug() << resintime << Qt::endl;
}
//找到运行目录
void MainWindow::read_start_path()
{
    QSettings *Reg=new QSettings(AUTORUN,QSettings::NativeFormat);
    QStringList keys = Reg->allKeys();

    for(int i  = 0 ; i < keys.size();i++)
    {
        QString id = keys.at(i);
        if(id.contains(KEY))
        {
            QString a;
            QVariant v = Reg -> value(KEY);
            a = v.toString();
            start_path = a.right(a.length() - 1);
            start_path.chop(19);
            break;
        }
        else
        {
            start_path = QCoreApplication::applicationDirPath();
        }
    }
    ini = start_path + "setting";
    user_data = start_path + "user.data" ;//设置cookie路径
    qDebug() << start_path << Qt::endl
                << ini << Qt::endl
                << user_data << Qt::endl;
    delete Reg;
}
