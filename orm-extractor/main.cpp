#include <QCoreApplication>
#include <QImage>
#include <iostream>
#include <QFile>

bool extractChannels(const QString& inputFile)
{
    QImage img(inputFile);
    QImage ao(img.size(), QImage::Format_ARGB32_Premultiplied);
    QImage roughness(img.size(), QImage::Format_Grayscale8);
    QImage metallic(img.size(), QImage::Format_Grayscale8);

    for (int y = 0; y < img.height(); ++y)
    {
        for (int x = 0; x < img.width(); ++x)
        {
            QRgb c = img.pixel(x, y);

            ao.setPixel(x, y, qRgb(qRed(c), qRed(c), qRed(c)));
            roughness.setPixel(x, y, qRgb(qGreen(c), qGreen(c), qGreen(c)));
            metallic.setPixel(x, y, qRgb(qBlue(c), qBlue(c), qBlue(c)));
        }
    }

    QString aoPath = inputFile;
    aoPath.replace("occlusionRoughnessMetallic", "O");
    bool ok1 = ao.save(aoPath);

    QString roughnessPath = inputFile;
    roughnessPath.replace("occlusionRoughnessMetallic", "R");
    bool ok2 = roughness.save(roughnessPath);

    QString metallicPath = inputFile;
    metallicPath.replace("occlusionRoughnessMetallic", "M");
    bool ok3 = metallic.save(metallicPath);

    return ok1 && ok2 && ok3;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if (argc < 2)
    {
        std::cout << "Error: need a input file!" << std::endl;
        return 1;
    }

    QString inputFile(argv[1]);

    if (!QFile::exists(inputFile))
    {
        std::cout << "Error: the file doesn't exists!" << std::endl;
        return 1;
    }

    bool ok = extractChannels(inputFile);
    if (ok)
        std::cout << "Done: " << inputFile.toStdString() << std::endl;
    else
        std::cout << "Failed: " << inputFile.toStdString() << std::endl;
    return 0;
}
