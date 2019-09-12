#include "gltfpicker.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFileInfo>

GLTFPicker::GLTFPicker()
{

}

bool GLTFPicker::Run(const QString glTFPath, const QString destFolder)
{
    QFile file(glTFPath);
    if (file.open(QFile::Text | QFile::ReadOnly) == false)
    {
        mErrors = "Cannot open the file at " + glTFPath;
        return false;
    }

    QByteArray content = file.readAll();

    QJsonDocument doc = QJsonDocument::fromBinaryData(content);
    if (doc.isNull())
    {
        mErrors = "Invalid JSON";
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



    return true;
}
