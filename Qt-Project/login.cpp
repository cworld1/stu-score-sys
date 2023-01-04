#include <QDebug>
#include <QMessageBox>
#include <QFile>

#include "login.h"
#include "ui_login.h"
#include "stuscore.h"
#include "md5.h"

Login::Login(QWidget *parent) : QMainWindow(parent), ui(new Ui::Login)
{
    ui->setupUi(this);
}

Login::~Login()
{
    delete ui;
}

void Login::open_stu_window()
{
    StuScore *stuwindow = new StuScore(this);
    this->hide();
    stuwindow->show();
}

void Login::on_button_login_clicked()
{
    MD5 md5;
    QString input = QString("%1%2").arg(ui->input_name->text(), ui->input_pswd->text());
    QString encode = md5.encode(input); // 对密码进行加密

    QFile file("admin.txt");
    // 如果没有该文件
    if (file.open(QIODevice::ReadWrite | QIODevice::NewOnly))
    {
        file.write(encode.toUtf8());
        file.close();
        qDebug() << "账号文件创建成功";
        this->open_stu_window();
    }
    // 如果有文件
    else if (file.open(QIODevice::ReadWrite))
    {
        QString account = file.readAll();
        // 输入与获取的内容相同
        if (encode == account)
            this->open_stu_window();
        else
            QMessageBox::warning(this, tr("登录信息"), tr("账号或密码错误！"), QMessageBox::Ok);
    }
    else
        QMessageBox::warning(this, tr("登录信息"), tr("数据文件被占用或无权访问，请检查！"), QMessageBox::Ok);
}

void Login::on_label_register_link_linkActivated(QString)
{
    QMessageBox::information(this, tr("注册信息"), tr("第一次登陆即为管理员，\n所以账号和密码将在第一次登录时被设置。"), QMessageBox::Ok);
}
