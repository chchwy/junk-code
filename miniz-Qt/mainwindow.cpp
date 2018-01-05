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
    connect(ui->compressFolderButton, &QPushButton::clicked, this, &MainWindow::compressFolderClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFileClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "", "D:/");
    qDebug() << fileName;

    QString sBaseDir = QFileInfo(fileName).absolutePath();

    ui->fileNameLabel->setText(fileName);

    mz_zip_archive* mz = new mz_zip_archive;
    mz_zip_zero_struct(mz);

    mz_bool ok = mz_zip_reader_init_file(mz, fileName.toUtf8().data(), 0);
    qDebug("Open ok=%d\n", ok);

    int num = mz_zip_reader_get_num_files(mz);
	qDebug("num=%d\n", num);

    mz_zip_archive_file_stat* stat = new mz_zip_archive_file_stat;
    for (int i = 0; i < num; ++i)
    {
        ok = mz_zip_reader_file_stat(mz, i, stat);
		qDebug(" item=%s\n", stat->m_filename);

        if (stat->m_is_directory)
        {
            QString sFolderName = QString::fromUtf8(stat->m_filename);
            QString sFullPath = QDir(sBaseDir).filePath(sFolderName);

            QDir d(sBaseDir);
            bool mkdirOK = d.mkdir(sFolderName);
            qDebug() << "mkdirOK=" << mkdirOK;
        }
        else
        {
            QDir d(sBaseDir);
            bool b = d.makeAbsolute();
            Q_ASSERT(b);
            QString sFullPath = d.filePath(QString::fromUtf8(stat->m_filename));
            ok = mz_zip_reader_extract_to_file(mz, i, sFullPath.toUtf8(), 0);
            qDebug("extract ok=%d\n", ok);
        }
    }
}

void MainWindow::compressFolderClicked()
{
    qDebug() << "Button clicked";

    mz_zip_archive* mz = new mz_zip_archive;
    mz_zip_zero_struct(mz);

    bool b = mz_zip_writer_init_file(mz, "D:/abc.zip", 0);
    qDebug("Writer init ok = %d", b);

    b = mz_zip_writer_add_file(mz, "aa/aa.txt", "D:/XYZ.jpg", "", 0, MZ_DEFAULT_COMPRESSION);
    qDebug("Writer add file ok = %d", b);

    b = mz_zip_writer_finalize_archive(mz);
    qDebug("Writer finalize = %d", b);
    mz_zip_writer_end(mz);
}
