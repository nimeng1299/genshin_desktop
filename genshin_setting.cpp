#include "genshin_setting.h"
#include "ui_genshin_setting.h"
#include <QDesktopServices>
#include <ShlObj.h>
#include <QDir>
#include <QSettings>
#define AUTORUN "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"
#define KEY "GenshinDesktop"
QString path;

genshin_setting::genshin_setting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::genshin_setting)
{
    ui->setupUi(this);
    path = QDir::toNativeSeparators(QApplication::applicationFilePath());

    QSettings *Reg=new QSettings(AUTORUN,QSettings::NativeFormat);
    QStringList keys = Reg->allKeys();

    for(int i  = 0 ; i < keys.size();i++)
    {
        QString id = keys.at(i);
        if(id.contains(KEY))
        {
            QSettings *ReadReg=new QSettings(AUTORUN,QSettings::NativeFormat);
            QString value=ReadReg->value(KEY).toString();
            if(value == path)
            {
                ui->pushButton->setEnabled(false);
                ui->pushButton->setText("已设置开机启动");
            }
            else
                IsAdminRun();

            delete ReadReg;
            break;
        }
        else
            IsAdminRun();
    }
    delete Reg;

}

void genshin_setting::IsAdminRun()
{
    BOOL bIsAdmin = IsUserAnAdmin();

    if(bIsAdmin)
    {
       qDebug() << "Run As administrator";
       ui->pushButton->setEnabled(true);
       ui->pushButton->setText("设置开机启动");
    }
    else
    {
       qDebug() << "Run As user";
       ui->pushButton->setEnabled(false);
       ui->pushButton->setText("请以管理员身份运行");
    }
}
genshin_setting::~genshin_setting()
{
    delete ui;
}


void genshin_setting::on_pushButton_clicked()
{
    QSettings *writeReg=new QSettings(AUTORUN,QSettings::NativeFormat);
    writeReg->setValue(KEY,path);
    ui->pushButton->setEnabled(false);
    ui->pushButton->setText("已设置开机启动");
    delete writeReg;
}



void genshin_setting::on_pushButton_2_clicked()
{
    QString URL = "https://github.com/nimeng1299/genshin_desktop/releases";
    QDesktopServices::openUrl(QUrl(URL.toLatin1()));
}


void genshin_setting::on_pushButton_3_clicked()
{
    QString URL = "https://github.com/nimeng1299/genshin_desktop";
    QDesktopServices::openUrl(QUrl(URL.toLatin1()));
}



