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

    for (int i = 1; i <= 68; ++i)
    {
        m[i] = i;
    }

    m[13] = 8;
    m[14] = 9;
    m[15] = 10;
    m[11] = m[16] = 6;
    m[12] = m[17] = 7;

    m[30] = m[34] = 26;
    m[27] = m[31] = m[35] = 23;
    m[28] = m[32] = m[36] = 24;
    m[33] = m[37] = 29;

    m[47] = m[53] = m[59] = 41;
    m[48] = m[54] = m[60] = 42;
    m[49] = m[55] = m[61] = 43;
    m[50] = m[56] = m[62] = 44;
    m[51] = m[57] = m[63] = 45;
    m[58] = m[64] = 52;

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
    for (int i = 1; i <= 68; ++i)
    {
        QString gltfJson = sourceText;
        QString imageName = QString("fp/A_%1.png").arg(m[i], 3, 10, QChar('0'));
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
