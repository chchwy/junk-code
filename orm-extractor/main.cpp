#include <QCoreApplication>
#include <QImage>
#include <iostream>
#include <QFile>

bool extractChannels(const QString& inputFile)
{
    QImage img(inputFile);
    QImage ao(img.size(), QImage::Format_Grayscale8);
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
    ao.save(aoPath);

    QString roughnessPath = inputFile;
    roughnessPath.replace("occlusionRoughnessMetallic", "R");
    roughness.save(roughnessPath);

    QString metallicPath = inputFile;
    metallicPath.replace("occlusionRoughnessMetallic", "M");
    metallic.save(metallicPath);

    return true;
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

    extractChannels(inputFile);

    return a.exec();
}
