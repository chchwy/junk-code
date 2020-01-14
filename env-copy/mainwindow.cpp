#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDirIterator>
#include <QPushButton>
#include <QFileInfo>
#include <QDebug>
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    baseFolder = "C:/Users/Matt/Desktop/HDRi";
    makeEnvButtons();

    connect(ui->refreshButton, &QPushButton::clicked, this, &MainWindow::refresh);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::makeEnvButtons()
{
    QStringList envMapList;

    QDirIterator it(baseFolder);
    while(it.hasNext())
    {
        QString path = it.next();
        if (path.contains("SpecularHDR.dds"))
        {
            envMapList.append(path);
            qDebug() << "Add " << path;
            continue;
        }
    }

    for (QString path : envMapList)
    {
        QFileInfo info(path);
        QPushButton* button = new QPushButton;
        button->setText(info.fileName());
        button->setObjectName(path);
        ui->groupBox->layout()->addWidget(button);

        connect(button, &QPushButton::clicked, this, &MainWindow::buttonClicked);

        //buttonList.append(button);
    }
    auto vbox = static_cast<QVBoxLayout*>(ui->groupBox->layout());
    vbox->addStretch();
}

void MainWindow::buttonClicked()
{
    const QString dest1 = "D:/Github/ogre-v2-mesh-viewer/common/resources/env.dds";
    const QString dest2 = "D:/Github/ogre-v2-mesh-viewer/common/resources/envSpecularHDR.dds";

    QPushButton* button = static_cast<QPushButton*>(QObject::sender());
    const QString src = button->objectName();

    QFile::remove(dest1);
    QFile::remove(dest2);

    QFile::copy(src, dest1);
    QFile::copy(src, dest2);
}

void MainWindow::refresh()
{
    QLayout* layout = ui->groupBox->layout();
    QLayoutItem* child = nullptr;
    while ((child = layout->takeAt(0)) != 0) {
        delete child;
    }

    makeEnvButtons();
}

