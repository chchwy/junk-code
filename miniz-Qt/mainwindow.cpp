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

    mz_zip_archive* mz = new mz_zip_archive;
    mz_zip_zero_struct(mz);

    mz_bool ok = mz_zip_reader_init_file(mz, L"D:\\新宿で20年.zip", 0);
    printf("Open ok=%d\n", ok);

    int num = mz_zip_reader_get_num_files(mz);
    printf("num=%d\n", num);

    mz_zip_archive_file_stat* stat = new mz_zip_archive_file_stat;
    for (int i = 0; i < num; ++i)
    {
        ok = mz_zip_reader_file_stat(mz, i, stat);
        printf(" item=%s\n", stat->m_filename);

        if (stat->m_is_directory)
        {
            //CreateDirectoryA(stat->m_filename, NULL);
        }
        ok = mz_zip_reader_extract_to_file(mz, i, stat->m_filename, 0);
        printf("extract ok=%d\n", ok);
    }
}
