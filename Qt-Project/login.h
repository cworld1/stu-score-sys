#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class Login;
}
QT_END_NAMESPACE

class Login : public QMainWindow
{
    Q_OBJECT

public:
    Login(QWidget *parent = nullptr);
    ~Login();

    void open_stu_window();

private slots:
    void on_button_login_clicked();
    void on_label_register_link_linkActivated(QString);

private:
    Ui::Login *ui;
};

#endif // LOGIN_H
