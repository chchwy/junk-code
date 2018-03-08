

#include <QCoreApplication>
#include <QTimer>
#include <QString>
#include "downloadmanager.h"


int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    DownloadManager manager;
    for (int i = 50; i < 100; ++i)
        manager.Add(QString("http://s3-ap-southeast-2.amazonaws.com/int-aplus-chinese-dev/learn-english/efa/zh_efa_%1.mp3").arg(i));
    QTimer::singleShot(0, &manager, SLOT(execute()));

    app.exec();
}

