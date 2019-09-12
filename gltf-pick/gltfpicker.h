#ifndef GLTFPICKER_H
#define GLTFPICKER_H

#include <QString>
#include <QStringList>


class GLTFPicker
{
public:
    GLTFPicker();
    bool Run(const QString glTFPath, const QString destFolder);

    QStringList mBuffers;
    QStringList mImages;
    QString mErrors;
};

#endif // GLTFPICKER_H
