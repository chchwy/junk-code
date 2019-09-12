#ifndef GLTFPICKER_H
#define GLTFPICKER_H

#include <QString>
#include <QStringList>
#include <QJsonArray>

class GLTFPicker
{
public:
    GLTFPicker();
    bool Run(const QString glTFPath, const QString destFolderPath);

    QString error() { return mErrors; }

    bool processBuffers(QJsonArray bufferArray, QString basePath, QString destFolderPath);
    bool processImages(QJsonArray imageArray, QString basePath, QString destFolderPath);
    bool writeJson(QJsonObject rootObject, QString outPath);

    QStringList mBuffers;
    QStringList mImages;
    QString mErrors;
};

#endif // GLTFPICKER_H
