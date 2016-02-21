#include "progressivevolume.h"
#include <QtGui/QApplication>

void customMessageHandler(QtMsgType type, const char *msg)
{
    QString dt = QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss");
    QString txt = QString("[%1] ").arg(dt);

    switch(type) {
    case QtDebugMsg:
        txt += QString("{Debug} \t\t %1").arg(msg);
        break;
    case QtWarningMsg:
        txt += QString("{Warning} \t %1").arg(msg);
        break;
    case QtCriticalMsg:
        txt += QString("{Critical} \t %1").arg(msg);
        break;
    case QtFatalMsg:
        txt += QString("{Fatal} \t\t %1").arg(msg);
        abort();
        break;
    }

    QFile outFile("LogFile.log");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);

    QTextStream textStream(&outFile);
    textStream << txt << endl;
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qInstallMsgHandler(customMessageHandler);

    ProgressiveVolume w;
    w.show();
    return a.exec();
}
