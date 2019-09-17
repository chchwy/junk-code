#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QCheckBox>
#include <QMimeData>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


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
    ui->logWidget->clear();
    QList<QUrl> urls = event->mimeData()->urls();

    QList<QString> localFiles;
    for (QUrl& url : urls)
    {
        QString s = url.toLocalFile();
        if (s.endsWith(".gltf"))
            localFiles.append(s);
    }
    qDebug() << localFiles;
    processAllGltf(localFiles);
}

void MainWindow::processAllGltf(QList<QString> gltfFiles)
{
    for (const QString& s : gltfFiles)
    {
        bool ok = processOneGltf(s);
        log(QString("Processing %1").arg(s));
        if (!ok)
        {
            log(QString("Error: %1").arg(mErrors));
        }

        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }
    log("Done!");
}

bool MainWindow::processOneGltf(QString glTFPath)
{
    QFile file(glTFPath);
    if (file.open(QFile::Text | QFile::ReadOnly) == false)
    {
        mErrors = "Cannot open the file at " + glTFPath;
        return false;
    }

    QByteArray content = file.readAll();
    file.close();

    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(content, &jsonError);
    if (doc.isNull())
    {
        mErrors = "Invalid JSON: " + jsonError.errorString();
        return false;
    }

    if (doc.isObject() == false)
    {
        mErrors = "Invalid glTF. A glTF document must be an Json object.";
        return false;
    }

    QJsonObject rootObject = doc.object();
    QJsonObject asset = rootObject["asset"].toObject();
    if (asset["version"].toString() != "2.0")
    {
        mErrors = "Incorrect version number. Only support glTF 2.0.";
        return false;
    }

    // remove animation
    if (ui->removeAnimationCheckBox->isChecked())
    {
        rootObject["animations"] = QJsonArray();
    }

    if (ui->removeNormalTexCheckBox->isChecked())
    {
        QJsonArray materials = rootObject["materials"].toArray();
        for (int i = 0; i < materials.size(); ++i)
        {
            QJsonObject material = materials[i].toObject();
            material.remove("normalTexture");
            materials[i] = material;
        }

        rootObject["materials"] = materials;
    }

    qDebug() << rootObject["materials"];
    writeJson(rootObject, glTFPath);

    return true;
}

bool MainWindow::writeJson(QJsonObject rootObject, QString outPath)
{
    QJsonDocument newDoc;
    newDoc.setObject(rootObject);

    auto minify = (ui->minifyJsonCheckBox->isChecked()) ? QJsonDocument::Compact : QJsonDocument::Indented;
    QByteArray newContent = newDoc.toJson(minify);

    QFile file(outPath);
    if (file.open(QFile::WriteOnly | QFile::Text) == false)
    {
        return false;
    }

    file.write(newContent);
    file.close();
    return true;
}

void MainWindow::log(QString msg)
{
    ui->logWidget->addItem(msg);
}
