#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QDebug>
#include <QDir>
#include "gltfpicker.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setAcceptDrops(true);
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
    QList<QUrl> urls = event->mimeData()->urls();

    QList<QString> localFiles;
    for (QUrl& url : urls)
    {
        localFiles.append(url.toLocalFile());
    }
    qDebug() << localFiles;

    for (const QString& filePath : localFiles)
    {
        ProcessGltf(filePath);
    }
}

void MainWindow::ProcessGltf(const QString& filePath)
{
    QFileInfo info(filePath);
    if (info.suffix() != "gltf") return;

    QString fileNameWithoutExt = info.completeBaseName();
    //qDebug() << fileName;

    QString baseFolder = info.path();
    //qDebug() << baseFolder;

    QDir dir(baseFolder);

    if (!dir.exists(fileNameWithoutExt)) {
        dir.mkpath(fileNameWithoutExt);
    }
    dir.cd(fileNameWithoutExt);

    QString gltf = info.absoluteFilePath();

    qDebug() << "GLTF:" << gltf;
    qDebug() << "Dest:" << dir.absolutePath();

    GLTFPicker picker;
    bool ok = picker.Run(gltf, dir.absolutePath());
    if (ok)
    {
        qInfo() << "Done.";
    }
    else
    {
        qInfo() << "Oooops! something went wrong.";
        qInfo() << "=============================";
        qInfo() << picker.error();
        qInfo() << "=============================";
    }
}
