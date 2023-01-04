#include <QDebug>
#include <QMessageBox>
#include <QTableWidgetItem>
// SQL 相关
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include "stuscore.h"
#include "ui_stuscore.h"

QSqlQuery *sql;

StuScore::StuScore(QWidget *parent) : QMainWindow(parent), ui(new Ui::StuScore)
{
    ui->setupUi(this);
    SortUporDown = true; // 默认升序

    // 初始化数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./stu-score.db");
    if (db.open())
    {
        qDebug() << "打开数据库成功";

        sql = new QSqlQuery(db);
        QString cmd = "create table stu(id primary key,name,class,chinese,math,english);";
        if (sql->exec(cmd))
            qDebug() << "创建表格成功";
        else
            qDebug() << "创建表格失败：" << sql->lastError().text();
    }
    else
        QMessageBox::warning(this, tr("数据信息"), tr("数据文件被占用或无权访问，请检查！"), QMessageBox::Ok);

    // 表格控件初始化
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);             // 禁用编辑
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 自适应
    this->show_table();                                                              // 填充数据
    connect(ui->tableWidget->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(sort_by_col(int)));
}

StuScore::~StuScore()
{
    delete ui;
}

// 获取数据库中的行数
int StuScore::get_sql_row()
{
    int row = 0;
    // 获取数据库中的行数
    QString cmd = "select count(*) from stu;";
    if (sql->exec(cmd))
    {
        while (sql->next())
            row = sql->value(0).toInt();
    }
    return row;
}

// 从数据库中填充表格
void StuScore::show_table()
{
    // 设置正确行数
    ui->tableWidget->setRowCount(this->get_sql_row());

    // 获取数据库
    QString cmd = "select * from stu;";
    if (sql->exec(cmd))
    {
        int row = 0;
        while (sql->next())
        {
            // 获取数据库内容
            QString id = sql->value(0).toString();
            QString name = sql->value(1).toString();
            QString clas = sql->value(2).toString();
            QString chinese = sql->value(3).toString();
            QString math = sql->value(4).toString();
            QString english = sql->value(5).toString();
            float averange = (chinese.toInt() + math.toInt() + english.toInt()) / 3;
            // 展示表格内容
            ui->tableWidget->setItem(row, 0, new QTableWidgetItem(id));
            ui->tableWidget->setItem(row, 1, new QTableWidgetItem(name));
            ui->tableWidget->setItem(row, 2, new QTableWidgetItem(clas));
            ui->tableWidget->setItem(row, 3, new QTableWidgetItem(chinese));
            ui->tableWidget->setItem(row, 4, new QTableWidgetItem(math));
            ui->tableWidget->setItem(row, 5, new QTableWidgetItem(english));
            ui->tableWidget->setItem(row, 6, new QTableWidgetItem(QString::number(averange)));
            row++;
        }
    }
    else
        qDebug() << "数据获取失败" << sql->lastError().text();
}

// 保存表格到数据库
void StuScore::save_table()
{
    int row = this->get_sql_row();

    // 清空数据库
    QString cmd = "delete from stu;";
    if (sql->exec(cmd))
        qDebug() << "清空数据库成功";
    else
        qDebug() << "清空数据库失败";

    // 重新插入数据
    for (int i = 0; i < row; i++)
    {
        QString id = ui->tableWidget->item(i, 0)->text();
        QString name = ui->tableWidget->item(i, 1)->text();
        QString clas = ui->tableWidget->item(i, 2)->text();
        QString chinese = ui->tableWidget->item(i, 3)->text();
        QString math = ui->tableWidget->item(i, 4)->text();
        QString english = ui->tableWidget->item(i, 5)->text();
        QString cmd = QString("insert into stu values('%1','%2','%3',%4,%5,%6);").arg(id, name, clas, chinese, math, english);
        qDebug() << cmd;
        if (!sql->exec(cmd))
        {
            qDebug() << "数据更新失败：" << sql->lastError().text();
            return;
        }
    }
    qDebug() << "数据更新成功";
}

// 按列排序
void StuScore::sort_by_col(int arg)
{
    if (SortUporDown)
    {
        ui->tableWidget->sortItems(arg, Qt::AscendingOrder);
        SortUporDown = false;
    }
    else
    {
        ui->tableWidget->sortItems(arg, Qt::DescendingOrder);
        SortUporDown = true;
    }
    qDebug() << "排序方式：" << (SortUporDown ? "逆序" : "顺序") << "；指定列：" << arg;
}

// 编辑按钮事件
void StuScore::on_button_edit_clicked()
{
    ui->tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked);
    ui->label->setText("当前为：编辑模式");
    ui->label->setStyleSheet("color: rgb(255, 0, 0);");
}

// 保存按钮事件
void StuScore::on_button_save_clicked()
{
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->save_table();
    this->show_table();
    ui->label->setText("当前为：浏览模式");
    ui->label->setStyleSheet("color: rgb(0, 100, 0);");
}

// 插入数据按钮事件
void StuScore::on_button_insert_clicked()
{
    int row = this->get_sql_row();
    if (ui->button_insert->text() == "插入数据")
    {
        ui->tableWidget->setRowCount(row + 1);
        // 设置为编辑模式
        this->on_button_edit_clicked();
        ui->button_insert->setText("确定插入");
        return;
    }
    else if (ui->button_insert->text() == "确定插入")
    {
        // 检查是否有空项
        if (ui->tableWidget->item(row, 0) == nullptr ||
            ui->tableWidget->item(row, 1) == nullptr ||
            ui->tableWidget->item(row, 2) == nullptr ||
            ui->tableWidget->item(row, 3) == nullptr ||
            ui->tableWidget->item(row, 4) == nullptr ||
            ui->tableWidget->item(row, 5) == nullptr)
        {
            QMessageBox::warning(this, tr("插入信息"), tr("内容有误，请检查！"), QMessageBox::Ok);
            return;
        }

        QString id = ui->tableWidget->item(row, 0)->text();
        QString name = ui->tableWidget->item(row, 1)->text();
        QString clas = ui->tableWidget->item(row, 2)->text();
        QString chinese = ui->tableWidget->item(row, 3)->text();
        QString math = ui->tableWidget->item(row, 4)->text();
        QString english = ui->tableWidget->item(row, 5)->text();

        QString sql_inser = QString("insert into stu values('%1','%2','%3',%4,%5,%6);").arg(id, name, clas, chinese, math, english);
        qDebug() << sql_inser;
        if (sql->exec(sql_inser))
        {
            ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
            ui->label->setText("当前为：浏览模式");
            ui->label->setStyleSheet("color: rgb(0, 100, 0);");
            ui->button_insert->setText("插入数据");
        }
        else
            qDebug() << "插入数据失败：" << sql->lastError().text();
    }

    // 更新表格
    this->show_table();
}

// 删除数据按钮事件
void StuScore::on_button_delete_clicked()
{
    QString input = ui->lineEdit->text();
    QString cmd = QString("delete from stu where id='%1';").arg(input);
    if (input.length() > 0)
    {
        if (sql->exec(cmd))
        {
            // 成功才会更新展示并返回，否则都会收到提示框
            this->show_table();
            return;
        }
        else
            qDebug() << "数据删除失败" << sql->lastError().text();
    }
    QMessageBox::warning(this, tr("删除信息"), tr("输入有误，请检查！"), QMessageBox::Ok);
}

// 统计成绩按钮事件
void StuScore::on_button_summary_clicked()
{
    // 获取数据库
    QString cmd = "select * from stu;";
    if (sql->exec(cmd))
    {
        int stu = 0;
        int chineseTotal = 0, mathTotal = 0, englishTotal = 0;
        int chineseMax = 0, mathMax = 0, englishMax = 0;
        int chineseMin = 150, mathMin = 150, englishMin = 150;
        while (sql->next())
        {
            // 获取数据库内容
            chineseTotal += sql->value(3).toInt();
            mathTotal += sql->value(4).toInt();
            englishTotal += sql->value(5).toInt();

            if (sql->value(3).toInt() > chineseMax)
                chineseMax = sql->value(3).toInt();
            if (sql->value(4).toInt() > mathMax)
                mathMax = sql->value(4).toInt();
            if (sql->value(5).toInt() > englishMax)
                englishMax = sql->value(5).toInt();
            if (sql->value(3).toInt() < chineseMin)
                chineseMin = sql->value(3).toInt();
            if (sql->value(4).toInt() < mathMin)
                mathMin = sql->value(4).toInt();
            if (sql->value(5).toInt() < englishMin)
                englishMin = sql->value(5).toInt();
            stu++;
        }
        if (stu == 0)
        {
            QMessageBox::warning(this, tr("统计信息"), tr("当前没有任何数据可供统计！"), QMessageBox::Ok);
            return;
        }
        float chineseAvg = chineseTotal / stu;
        float mathAvg = mathTotal / stu;
        float englishAvg = englishTotal / stu;
        QString info = QString("总人数：%1 人\n").arg(QString::number(stu));
        info += QString("语文：平均分 %1 分，最高分 %2 分，最低分 %3 分\n")
                    .arg(QString::number(chineseAvg), QString::number(chineseMax), QString::number(chineseMin));
        info += QString("数学：平均分 %1 分，最高分 %2 分，最低分 %3 分\n")
                    .arg(QString::number(mathAvg), QString::number(mathMax), QString::number(mathMin));
        info += QString("英语：平均分 %1 分，最高分 %2 分，最低分 %3 分")
                    .arg(QString::number(englishAvg), QString::number(englishMax), QString::number(englishMin));
        QMessageBox::information(this, tr("统计信息"), info, QMessageBox::Ok);
    }
    else
        qDebug() << "数据获取失败" << sql->lastError().text();
}

// 退出按钮事件
void StuScore::on_button_exit_clicked()
{
    this->parentWidget()->show();
    this->hide();
}
