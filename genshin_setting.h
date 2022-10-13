#ifndef GENSHIN_SETTING_H
#define GENSHIN_SETTING_H

#include <QWidget>

namespace Ui {
class genshin_setting;
}

class genshin_setting : public QWidget
{
    Q_OBJECT

public:
    explicit genshin_setting(QWidget *parent = nullptr);
    ~genshin_setting();

private slots:
    void on_pushButton_3_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();

private:
    Ui::genshin_setting *ui;
};

#endif // GENSHIN_SETTING_H
