#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDirIterator>
#include <QDebug>
#include <QImage>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    perforemResizing();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::perforemResizing()
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
