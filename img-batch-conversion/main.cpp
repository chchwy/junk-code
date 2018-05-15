#include <QCoreApplication>
#include <QDirIterator>
#include <QDebug>
#include <QImage>
#include <QPainter>

bool white9x9(const QImage& img, int x, int y)
{
    QRgb white = qRgb(255, 255, 255);
    const int radius = 50;
    for (int i = x - radius; i <= x + radius; ++i)
    {
        for (int j = y - radius; j <= y + radius; ++j)
        {
            QRgb rgb = img.pixel(i, j);
            if (rgb != white)
            {
                return false;
            }
        }
    }
    return true;
}

void ProcessImage(const QString& img_path)
{
    QImage img1(img_path);
    //qDebug() << img1.width() << "x" << img1.height();

    //QColor c = img1.pixelColor(3052, 149);
    //qDebug() << c;
    bool is_landscape = white9x9(img1, 3059, 149);
    qDebug() << QFileInfo(img_path).fileName() << is_landscape;

    if (!is_landscape)
    {
        return;
        auto left = 2750;
        auto right = 3450;
        auto height = img1.height();

        QImage info_side(right - left, img1.height(), QImage::Format_ARGB32);
        QPainter painter(&info_side);
        {
            painter.drawImage(QPoint(0, 0), img1, QRect(left, 0, right, height));
        }
        painter.end();

        QFileInfo info(img_path);
        QString folder = info.absolutePath().append("_bg");
        QString fileName = info.completeBaseName().append(".png");

        fileName.replace("_4k", "");

        QString sRealName = QDir(folder).filePath(fileName);
        //qDebug() << sRealName;
        //qDebug() << info_side.save(sRealName);

        QImage img2(img1.width(), img1.height(), QImage::Format_ARGB32);
        img2.fill(QColor(255, 255, 255, 0));

        auto offset = 400;
        left += offset;
        right += offset;

        QPainter painter2(&img2);
        {
            painter2.drawImage(left, 0, info_side);
        }
        painter2.end();

        img2.save(sRealName, "PNG");
    }
    else
    {
        QImage img_land(img_path);

        auto top = 1570;
        auto bottom = img_land.height();
        auto width = img_land.width();

        // keep the bottom part
        QImage img_bottom(width, bottom - top, QImage::Format_ARGB32);
        QPainter paint(&img_bottom);
        {
            paint.drawImage(QPoint(0, 0), img_land, QRect(0, top, width, bottom));
        }
        paint.end();

        // keep the apartment name
        QRect rect(450, 110, 1130 - 450, 390 - 110);
        QImage img_name(rect.size(), QImage::Format_ARGB32);
        QPainter p3(&img_name);
        {
            p3.drawImage(QPoint(0, 0), img_land, rect);
        }
        p3.end();

        // combine the final results
        QImage img_land2(img_land.width(), img_land.height(), QImage::Format_ARGB32);
        img_land2.fill(QColor(255, 255, 255, 0));
        QPainter paint2(&img_land2);
        {
            paint2.drawImage(0, top, img_bottom);
            rect.moveTop(50);
            paint2.drawImage(rect, img_name);
        }
        paint2.end();

        QFileInfo info(img_path);
        QString sDest = "C:\\temp\\";
        sDest.append(info.completeBaseName());
        sDest.append(".png");
        sDest.replace("_4k", "");

        img_land2.save(sDest, "PNG");
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString sInputFolder = "D:\\DisplaySweet\\tv\\Crown_TV\\project_images\\OneBarangaroo\\1.0\\tv\\floor_plans";
    QStringList filters { "*.jpg" };
    QDirIterator dir_it(sInputFolder, filters, QDir::Files);

    while (dir_it.hasNext())
    {
        QString imgPath = dir_it.next();
        //qDebug() << imgPath;
        ProcessImage(imgPath);
        //break;
    }

    return a.exec();
}
