#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDirIterator>
#include <QDebug>
#include <QImage>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QPainter>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setAcceptDrops(true);
    //quickAndDirtyResizing();
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

    processImages(localFiles);
}

void MainWindow::processImages(const QList<QString>& imgFiles)
{
    bool keepAspectRatio = ui->keepAspectCheckBox->isChecked();
    for (QString imgPath : imgFiles)
    {
        if (keepAspectRatio)
            resizeImageKeepAspectRatio(imgPath);
        else
            resizeImage(imgPath);
    }
}

void MainWindow::resizeImage(QString imgPath)
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

void MainWindow::resizeImageKeepAspectRatio(QString imgPath)
{
    QImage img(imgPath);
    qDebug() << imgPath << ", " << img.width() << "x" << img.height();

    int oldWidth = img.width();
    int oldHeight = img.height();
    int targetSize = ui->imgSizeSpinBox->value();

    QImage img2;
    if (oldWidth > oldHeight)
    {
        img2 = img.scaledToWidth(targetSize, Qt::SmoothTransformation);
    }
    else
    {
        img2 = img.scaledToHeight(targetSize, Qt::SmoothTransformation);
    }

    QPoint drawPoint;
    drawPoint.setX((targetSize - img2.width()) / 2);
    drawPoint.setY((targetSize - img2.height()) / 2);

    QImage img3(QSize(targetSize, targetSize), QImage::Format_ARGB32);
    {
        QPainter painter(&img3);
        painter.drawImage(drawPoint, img2);
        painter.end();
    }
    img3.save(imgPath);

    ui->logWidget->addItem(QString("Resize %1 to %2x%2").arg(imgPath).arg(targetSize).arg(targetSize));
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

}

void MainWindow::quickAndDirtyResizing()
{
    QString path = "C:\\Temp\\fp\\fp";
    QDirIterator it(path, QStringList() << "*.jpg" << "*.png", QDir::Files);

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

        //QImage img2 = img.mirrored(true, false);
        img2.save(imgPath);
    }
}
