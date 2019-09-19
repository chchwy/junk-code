#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QDebug>
#include <time.h>


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

QString MainWindow::randomText(int n)
{
    QString result;
    QTextStream sout(&result, QIODevice::WriteOnly);
    char base[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    int len = strlen(base);
    for (int i = 0; i < n; ++i)
    {
        int index = rand() % len
                ;
        qDebug() << index;
        sout << base[index];
    }
    qDebug() << result;
    return result;
}

void MainWindow::Go()
{
    std::srand(time(nullptr));

    QFile fin(":/rsc/fp_source.gltf");
    bool ok = fin.open(QFile::ReadOnly);
    qDebug() << "ok?" << ok;

    QString sourceText = fin.readAll();
    QString destFolder = "D:/salesapp/apps/3dplans/public/doma-3d-scene/assets/floorplan";
    for (int i = 1; i <= 21; ++i)
    {
        QString gltfJson = sourceText;
        QString imageName = QString("fp/A_%1.png").arg(i, 3, 10, QChar('0'));
        gltfJson.replace("##image_url##", imageName);

        QString materialName = QString("Mtl_appt%1_%2").arg(i, 3, 10, QChar('0')).arg(randomText(7));
        gltfJson.replace("##material_name##", materialName);
        qDebug() << materialName;

        QString fileName = QString("apartment%1.gltf").arg(i, 3, 10, QChar('0'));
        qDebug() << fileName;
        QFile fout("D:/salesapp/apps/3dplans/public/doma-3d-scene/assets/floorplan/" + fileName);
        fout.open(QFile::WriteOnly);
        QTextStream tout(&fout);
        tout << gltfJson;
    }
}
