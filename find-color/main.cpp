#include <QCoreApplication>
#include <QImage>
#include <QDirIterator>
#include <QDebug>
#include <vector>
#include <functional>

bool compareColor1(const QColor& c)
{
    QColor c_2(135, 117, 79);

    if (qAbs(c.red() - c_2.red()) > 3)
        return false;
    if (qAbs(c.green() - c_2.green()) > 3)
        return false;
    if (qAbs(c.blue() - c_2.blue()) > 3)
        return false;
    return true;
}


bool compareColor8001(const QColor& c)
{
    QColor c_2(117, 99, 63); // for apartment 81.01 only

    if (qAbs(c.red() - c_2.red()) > 3)
        return false;
    if (qAbs(c.green() - c_2.green()) > 3)
        return false;
    if (qAbs(c.blue() - c_2.blue()) > 3)
        return false;
    return true;
}

struct hotspot
{
    int x = 0, y = 0;
};

struct hotspot_group
{
    int x = 0, y = 0;
    std::vector<hotspot> points;
};

bool check_pos(int x1, int y1, int x2, int y2)
{
    if (qAbs(x1 - x2) < 60 && qAbs(y1 - y2) < 60)
        return true;
    return false;
}

std::vector<hotspot_group> all_hotspots;

void add_to_hotspot_group(int tx, int ty)
{
    bool found = false;
    for (hotspot_group& group : all_hotspots)
    {
        if (check_pos(group.x, group.y, tx, ty))
        {
            group.points.push_back(hotspot{tx, ty});
            found = true;
            break;
        }
    }

    if (!found)
    {
        hotspot_group my_group;
        my_group.x = tx;
        my_group.y = ty;
        my_group.points.push_back(hotspot{tx, ty});
        all_hotspots.push_back(my_group);
    }
}

void do_average(hotspot_group& g)
{
    int sumx = 0, sumy = 0;
    for (hotspot h : g.points)
    {
        sumx += h.x;
        sumy += h.y;
    }

    g.x = (sumx / g.points.size());
    g.y = (sumy / g.points.size());
}

QString find_hotspots(QString file_name, std::function<bool(const QColor&)> compareColor)
{
    all_hotspots.clear();

    QFileInfo info(file_name);
    QImage img(file_name);

    qDebug() << img.width() << img.height();

    for (int h = 0; h < img.height(); ++h)
    {
        for (int w = 0; w < img.width(); ++w)
        {
            QColor c = img.pixelColor(w, h);
            if (compareColor(c))
            {
                //qDebug () << "Pixel! {" << w << h <<"}";
                add_to_hotspot_group(w, h);
            }
        }
    }

    for (hotspot_group& g : all_hotspots)
    {
        do_average(g);

        g.x = g.x * 0.4066874;
        g.y = g.y * 0.4066874 + 100;
    }

    QString sOut = "";
    for (hotspot_group& g : all_hotspots)
    {
        //qDebug() << "(" << g.x << "," << g.y << ")";
        QTextStream s(&sOut);

        s << "{x=" << g.x << ", y=" << g.y << "}, ";
    }

    return sOut;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString input_folder = "D:\\DisplaySweet\\ipad\\onebarangaroo_staging\\project_images\\OneBarangaroo\\1.0\\ipad\\floor_plans";
    QString output_file = "C:\\temp\\hotspot.txt";

    QFile f(output_file);
    f.open(QFile::WriteOnly);
    QTextStream fout(&f);

    QDirIterator dir_it(input_folder, QStringList() << "*.jpg");

    int index = 1;
    while(dir_it.hasNext())
    {
        QString file_name = dir_it.next();

        std::function<bool(const QColor&)> comparator;
        if (file_name.contains("8101"))
            comparator = compareColor8001;
        else
            comparator = compareColor1;

        QString oneApartment = find_hotspots(file_name, comparator);
        QString final_result = QString("[%1] = { %2},").arg(index).arg(oneApartment);
        qDebug() << final_result;
        fout << final_result << "\n";
        index++;
    }
    qDebug () << "done";
    fout.flush();
    f.close();
    return a.exec();
}
