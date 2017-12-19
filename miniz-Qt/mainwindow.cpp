#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>

#include "miniz.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->openFileButton, &QPushButton::clicked, this, &MainWindow::openFileClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFileClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "", "D:/");
    qDebug() << fileName;

    ui->fileNameLabel->setText(fileName);
}
