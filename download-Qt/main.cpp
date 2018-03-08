

#include <QCoreApplication>
#include <QTimer>
#include "downloadmanager.h"


int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    DownloadManager manager;
    QTimer::singleShot(0, &manager, SLOT(execute()));

    app.exec();
}

