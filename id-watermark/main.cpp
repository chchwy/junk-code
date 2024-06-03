#include <QGuiApplication>
#include <QImage>
#include <QDebug>
#include <QPainter>

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);

    QImage img("/Users/matt/Documents/03062024105535-0001.jpg");
    qDebug() << img.isNull() << img.size();

    QPainter painter(&img);

    painter.setPen(QPen(QColor(30, 30, 30, 90)));
    QString s = QStringLiteral("僅供深智數位出版合約使用");

    for (int i = 0; i < 4; ++i)
    {
        painter.drawText(i * 500, i * 200 + 300, s);
    }

    bool ok = img.save("/Users/matt/github/id-watermark/out.jpg");
    qDebug() << ok;
    return 0;
}
