#include "login.h"
#include <QApplication>
#include <QMutex>
#include <QFile>
#include <QDateTime>

#ifdef QT_NO_DEBUG
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // 加锁
    static QMutex mutex;
    mutex.lock();

    QByteArray localMsg = msg.toLocal8Bit();

    QString strMsg("");
    switch(type)
    {
    case QtDebugMsg:
        strMsg = QString("Debug");
        break;
    case QtWarningMsg:
        strMsg = QString("Warning");
        break;
    case QtCriticalMsg:
        strMsg = QString("Critical");
        break;
    case QtFatalMsg:
        strMsg = QString("Fatal");
        break;
    default:;
    }

    // 设置输出信息格式
    QString strDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString strMessage = QString("[%1] %2: %3   (File: %4 Line: %5 Function: %6)")
            .arg(strMsg, strDateTime, localMsg.constData(), context.file, QString::number(context.line), context.function);

    // 输出信息至文件中（读写、追加形式）
    QFile file("log.txt");
    file.open(QIODevice::ReadWrite | QIODevice::Append);
    QTextStream stream(&file);
    stream << strMessage << "\r\n";
    file.flush();
    file.close();

    // 解锁
    mutex.unlock();
}

#endif

int main(int argc, char *argv[])
{
    #ifdef QT_NO_DEBUG
    qInstallMessageHandler(myMessageOutput);
    #endif
    QApplication app(argc, argv);
    Login loginwindow;
    loginwindow.show();
    return app.exec();
}
