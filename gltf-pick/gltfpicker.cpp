#include "gltfpicker.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFileInfo>
#include <QDir>
#include <QDebug>


GLTFPicker::GLTFPicker()
{
}

bool GLTFPicker::Run(const QString glTFPath, const QString destFolderPath)
{
    QFile file(glTFPath);
    if (file.open(QFile::Text | QFile::ReadOnly) == false)
    {
        mErrors = "Cannot open the file at " + glTFPath;
        return false;
    }

    QByteArray content = file.readAll();

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

    // Create dest folder
    QDir destDir(destFolderPath);
    if (!destDir.exists())
    {
        bool ok = destDir.mkpath(".");
        if (!ok)
        {
            mErrors = "Cannot create the dest folder: " + destFolderPath;
            return false;
        }
    }

    QFileInfo info(glTFPath);
    QString basePath = info.path();
    //qDebug() << "BasePath:" << basePath;

    QJsonArray bufferArray = rootObject["buffers"].toArray();
    if (!processBuffers(bufferArray, basePath, destFolderPath))
    {
        return false;
    }

    QJsonArray imageArray = rootObject["images"].toArray();
    if (!processImages(imageArray, basePath, destFolderPath))
    {
        return false;
    }

    // process the json
    QString newGltfJsonLocation = destDir.filePath(info.fileName());
    qInfo() << newGltfJsonLocation;

    rootObject["animations"] = QJsonArray();
    if (!writeJson(rootObject, newGltfJsonLocation))
    {
        return false;
    }

    return true;
}

bool GLTFPicker::processBuffers(QJsonArray bufferArray, QString basePath, QString destFolderPath)
{
    QDir srcDir(basePath);
    QDir destDir(destFolderPath);

    for (QJsonValue i : bufferArray)
    {
        QJsonObject buffer = i.toObject();
        //qInfo() << buffer;

        QString bufferUri = buffer["uri"].toString();
        QString srcFile = srcDir.filePath(bufferUri);
        QString destFile = destDir.filePath(bufferUri);

        //qInfo() << "Write to" << destFile;
        bool ok = QFile::copy(srcFile, destFile);
        qInfo() << "Process" << bufferUri << ":" << ((ok) ? "Succeeded" : "Failed");
    }
    return true;
}

bool GLTFPicker::processImages(QJsonArray imageArray, QString basePath, QString destFolderPath)
{
    QDir srcDir(basePath);
    QDir destDir(destFolderPath);

    for (QJsonValue i : imageArray)
    {
        QJsonObject image = i.toObject();
        //qInfo() << image;

        QString imgUri = image["uri"].toString();
        QString srcFile = srcDir.filePath(imgUri);
        QString destFile = destDir.filePath(imgUri);

        //qInfo() << "From " << srcFile;
        //qInfo() << "Write to" << destFile;
        bool ok = QFile::copy(srcFile, destFile);
        qInfo() << "Process" << imgUri << ":" << ((ok) ? "Succeeded" : "Failed");
    }
    return true;
}

bool GLTFPicker::writeJson(QJsonObject rootObject, QString outPath)
{
    QJsonDocument newDoc;
    newDoc.setObject(rootObject);
    QByteArray newContent = newDoc.toJson(QJsonDocument::Compact);

    QFile file(outPath);
    if (file.open(QFile::WriteOnly | QFile::Text) == false)
    {
        return false;
    }

    file.write(newContent);
    file.close();
    return true;
}
