#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Go();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Go()
{
    QFile fin(":/rsc/fp_source.gltf");
    bool ok = fin.open(QFile::ReadOnly);
    qDebug() << "ok?" << ok;

    QString sourceText = fin.readAll();
    QString destFolder = "D:/salesapp/apps/3dplans/public/doma-3d-scene/assets/floorplan";
    for (int i = 1; i <= 21; ++i)
    {
        QString gltfJson = sourceText;
        QString imageName = QString("fp/A_%1.png").arg(i, 3, 10, QChar('0'));
        gltfJson.replace("#####", imageName);

        QString fileName = QString("fp_b23_a_%1.gltf").arg(i, 3, 10, QChar('0'));
        qDebug() << fileName;
        QFile fout("D:/salesapp/apps/3dplans/public/doma-3d-scene/assets/floorplan/" + fileName);
        fout.open(QFile::WriteOnly);
        QTextStream tout(&fout);
        tout << gltfJson;
    }
}
