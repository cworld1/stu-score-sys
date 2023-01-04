#ifndef STUSCORE_H
#define STUSCORE_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class StuScore;
}
QT_END_NAMESPACE

class StuScore : public QMainWindow
{
    Q_OBJECT

public:
    explicit StuScore(QWidget *parent = nullptr);
    ~StuScore();

    int get_sql_row();
    void show_table();
    void save_table();

private slots:
    void sort_by_col(int);
    void on_button_edit_clicked();
    void on_button_save_clicked();
    void on_button_insert_clicked();
    void on_button_delete_clicked();
    void on_button_summary_clicked();
    void on_button_exit_clicked();

private:
    Ui::StuScore *ui;
    bool SortUporDown; // 排序规则
};

#endif // STUSCORE_H
