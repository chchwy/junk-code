#include <QCoreApplication>
#include <QString>
#include <QFile>
#include <QDebug>

class Townhouse
{
public:
    QString name;
    QString type;
    int levelNum = 0;

    Townhouse(QString n, QString t)
    {
        name = n;
        type = t;
        levelNum = getLevelNum(type);
    }

    int getLevelNum(QString type) {
        static QList<QString> threeType = {"4", "6", "8", "10"};
        if (threeType.contains(type)) {
            return 3;
        }
        return 2;
    }
};

std::vector<Townhouse> townhouses = {
    { "26/4", "3a" },
    { "26/5", "2" },
    { "61/1", "6" },
    { "61/2", "6" },
    { "61/3", "4" },
    { "61/4", "4" },
    { "61/5", "4" },
    { "61/6", "4" },
    { "61/7", "4" },
    { "61/10", "1" },
    { "61/11", "1" },
    { "61/12", "1" },
    { "61/13", "1" },
    { "61/14", "1" },
    { "61/15", "3" },
    { "31/67", "10" },
    { "31/68", "8" },
    { "31/69", "8" },
    { "31/70", "8" },
    { "31/71", "8" },
    { "31/71", "8" },
    { "63/8", "11" },
    { "63/9", "12" },
    { "63/10", "12" },
    { "63/11", "12" },
    { "63/12", "11" },
    { "63/13", "11a" },
};

void doWork()
{
    for (const Townhouse& i : townhouses)
    {
        QString templateFileName = (i.levelNum == 2) ? ":/townhouse_template2.gltf" : ":/townhouse_template3.gltf";
        QFile fin(templateFileName);
        bool ok = fin.open(QFile::ReadOnly);
        qDebug() << "ok?" << ok;

        QString sourceText = fin.readAll();
        QString destFolder = "C:/temp/townhouse/";

        QString gltfJson = sourceText;
        gltfJson.replace("###TYPE###", i.type);

        QString name = i.name;
        QStringList tokens = name.split('/');

        QString fileName = QString("townhouse_%1_%2.gltf").arg(tokens[0]).arg(tokens[1]);
        qDebug() << fileName;
        QFile fout(destFolder + fileName);
        fout.open(QFile::WriteOnly);
        QTextStream tout(&fout);
        tout << gltfJson;
        fout.close();
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    doWork();
    return a.exec();
}
