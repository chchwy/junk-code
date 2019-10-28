#include <QCoreApplication>
#include <QImage>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QImage img1("D:/salesapp/apps/3dplans/public/doma-3d-scene/project_images/TheParks/1.0/3dassets/vegetation/CypressOak_Low_Leaves_Mat1_baseColor_src.png");
    QString outPath = "D:/salesapp/apps/3dplans/public/doma-3d-scene/project_images/TheParks/1.0/3dassets/vegetation/CypressOak_Low_Leaves_Mat1_baseColor.png";

    int w = img1.width();
    int h = img1.height();

    int64_t r = 0;
    int64_t g = 0;
    int64_t b = 0;
    int count = 0;

    for (int x = 0; x < w; ++x) {
        for (int y = 0; y < h; ++y) {
            QColor color = img1.pixelColor(x, y);
            if (color.alpha() == 255) {
                //qDebug() << "Color:" << color;
                r += color.red();
                g += color.green();
                b += color.blue();
                count += 1;
            }
        }
    }

    r = double(r) / count;
    g = double(g) / count;
    b = double(b) / count;

    //QColor avgColor(r, g, b);

    QImage img2(w, h, QImage::Format_ARGB32);

    for (int x = 0; x < w; ++x) {
        for (int y = 0; y < h; ++y) {
            QColor color = img1.pixelColor(x, y);
            if (color.alpha() < 30) {
                color.setRgb(r, g, b, 0);
            } else {
                color.setAlpha(255);
            }
            img2.setPixelColor(x, y, color);
        }
    }

    img2.save(outPath);
    qDebug() << "Done!";
    QCoreApplication::exit();
    return a.exec();
}
