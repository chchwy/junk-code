#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDirIterator>
#include <QDebug>
#include <QImage>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setAcceptDrops(true);
    //perforemResizing();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    // You must necessarily accept the data transfer event in the application window area
    event->accept();
}

void MainWindow::dropEvent(QDropEvent* event)
{
    // When we drop the file into the application area, we take the path to the file from the MIME data
    ui->logWidget->clear();
    QList<QUrl> urls = event->mimeData()->urls();

    QList<QString> localFiles;
    for (QUrl& url : urls)
    {
        localFiles.append(url.toLocalFile());
    }
    qDebug() << localFiles;
    resizeImages(localFiles);
}

void MainWindow::resizeImages(const QList<QString>& imgFiles)
{
    for (QString imgPath : imgFiles)
    {
        QImage img(imgPath);
        qDebug() << imgPath << ", " << img.width() << "x" << img.height();

        int originalWidth = img.width();
        int originalHeight = img.height();

        int size = ui->imgSizeSpinBox->value();
        int w = (size < originalWidth)? size : originalWidth;
        int h = (size < originalHeight)? size : originalHeight;
        QImage img2 = img.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        img2.save(imgPath);

        ui->logWidget->addItem(QString("Resize %1 to %2x%2").arg(imgPath).arg(w).arg(h));
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }
}

void MainWindow::quickAndDirtyResizing()
{
    QString path = "C:\\Users\\Matt\\Desktop\\ok\\building1_level4";
    QDirIterator it(path, QStringList() << "*.jpg", QDir::Files);

    qDebug() << "test!";
    while(it.hasNext())
    {
        QString imgPath = it.next();
        QImage img(imgPath);
        qDebug() << imgPath << ", " << img.width() << "x" << img.height();

        int w = 512;
        int h = 512;
        if (img.width() >= 2000) {
            w = h = 1024;
        } else if (img.width() > 1000) {
            w = h = 256;
        }
        QImage img2 = img.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        img2.save(imgPath);
    }
}
