#include <QCoreApplication>
#include <QFile>
#include <QDirIterator>
#include <QDebug>
#include <QFileInfo>
#include "json.hpp"

using json = nlohmann::json;


json getJsonFromFile(const QString fileName)
{
    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly)) {
        return false;
    }

    QTextStream fin(&f);
    QString content = fin.readAll();
    std::string s = content.toStdString();

    auto j3 = json::parse(s);
    f.close();
    return j3;
}

bool writeJsonToFile(const json& j3, const QString fileName)
{
    std::string serialized_string = j3.dump();

    QFile f2(fileName);
    if (!f2.open(QIODevice::WriteOnly)) {
        return false;
    }

    QTextStream fout(&f2);
    fout.setCodec("UTF-8");
    fout << QString::fromUtf8(serialized_string.c_str());
    fout.flush();
    f2.close();
    return true;
}

bool parseGLTF(QString fileName)
{
    qDebug() << "Processing" << fileName;

    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly)) {
        return false;
    }

    QTextStream fin(&f);
    QString content = fin.readAll();
    std::string s = content.toStdString();

    auto j3 = json::parse(s);

    int defaultSceneIndex = j3["scene"].get<int>();
    auto defaultScene = j3["scenes"][defaultSceneIndex];

    int rootNodeIndex = defaultScene["nodes"][0];
    //qDebug() << "RootNodeIndex" << rootNodeIndex;

    auto nodes = j3["nodes"];

    // get down 2 levels
    auto rootNode = nodes[rootNodeIndex];

    //qDebug() << "Level1:";
    //auto lv1Nodes = json::array();
    for (json i : rootNode["children"]) {
        int index = i.get<int>();
        json childNode = nodes[index];

        QString s = childNode["name"].get<std::string>().c_str();
        /*
        for (json k : childNode["children"]) {
            int index2 = k.get<int>();
            json childNode2 = nodes[index2];
            qDebug() << childNode2["name"].get<std::string>().c_str();
        }
        */
        qDebug() << QString("[path.join(baseDir, buildingFileName), '-nodeName', '%1'],").arg(s);
    }
    return true;
}

void getLevelNodeNames()
{
    QString baseFolder = "C:/Temp/ParksRedHill_2019-10-15_townhouse_interiors_groups";
    QDirIterator it(baseFolder, {"*.gltf"}, QDir::Files);

    while(it.hasNext())
    {
        QString filePath = it.next();
        parseGLTF(filePath);
    }
}

void renameGLTFFiles()
{
    QString newfolder = "C:/Temp/townhouse_interior";
    QDirIterator it2(newfolder, {"*.gltf"}, QDir::Files);
    while(it2.hasNext())
    {
        QString filePath = it2.next();
        QFileInfo info(filePath);

        QString baseName = info.completeBaseName();
        QStringList tokens = baseName.split('_');
        tokens[0];

        QString newBaseName = QString("townhouse_%1_%2").arg(tokens[2]).arg(tokens[1]);
        QString newFilePath = info.path() + "/" + newBaseName + ".gltf";
        qDebug() << newFilePath;

        QFile::rename(filePath, newFilePath);
    }
}

bool setAlphaMode(QString filePath)
{
    qDebug() << "Processing" << filePath;
    QString format;
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly)) {
        return false;
    }

    QTextStream fin(&f);
    QString content = fin.readAll();
    std::string s = content.toStdString();
    f.close();

    auto j3 = json::parse(s);
    auto materials = j3["materials"];
    for (auto& i : materials)
    {
        if (i["alphaMode"].is_string())
        {
            std::string mode = i["alphaMode"].get<std::string>();
            if (mode == "BLEND" || mode == "MASK")
            {
                i["alphaMode"] = "MASK";
                i["alphaCutoff"] = 0.2;
            }
        }

        std::string name;
        if (i["name"].is_string())
            name = i["name"].get<std::string>();

        if (name == "CypressOak_Low_Leaves_Mat1")
        {
            i.erase("normalTexture");
        }
        else if (name == "BlackGum_Low_FacingLeaves_Mat1")
        {
            i.erase("normalTexture");
        }
        else if (name == "JapaneseMaple_Low_Branches_Mat1")
        {
            i.erase("normalTexture");
        }
        else if (name == "Derevo_horror_Branches_2_Mat1")
        {
            i.erase("normalTexture");
        }
    }
    j3["materials"] = materials;

    auto materials2 = j3["materials"];
    for (auto i : materials2)
    {
        if (i["alphaMode"].is_string())
        {
            float cutoff = 1.0;
            if (i["alphaCutoff"].is_number_float())
            {
                cutoff = i["alphaCutoff"].get<float>();
            }
            //qDebug() << i["alphaMode"].get<std::string>().c_str() << cutoff;
        }
    }

    std::string serialized_string = j3.dump();
    
    QFile f2(filePath);
    if (!f2.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    QTextStream fout(&f2);
    fout.setCodec("UTF-8");
    fout << QString::fromUtf8(serialized_string.c_str());
    fout.flush();
    f2.close();
    
    return true;
}

void applyAlphaCutoff()
{
    QString baseFolder = "D:/salesapp/apps/3dplans/public/doma-3d-scene/project_images/TheParks/1.0/3dassets/exterior";
    QDirIterator it(baseFolder, {"*.gltf"}, QDir::Files);

    while(it.hasNext())
    {
        QString filePath = it.next();
        setAlphaMode(filePath);
    }
}

bool removeEmissive(QString filePath)
{
    qDebug() << "Processing" << filePath;
    
    auto j3 = getJsonFromFile(filePath);
    auto materials = j3["materials"];
    for (auto& i : materials)
    {
        if (i.find("emissiveFactor") != i.end())
        {
            //qDebug() << i["emissiveFactor"].is_array();
            auto& emi = i["emissiveFactor"];
            if (emi[0] == 0 && emi[1] == 0 && emi[2] == 0)
            {
                i.erase("emissiveFactor");
            }
        }
    }
    j3["materials"] = materials;
    j3.erase("animations");

    return writeJsonToFile(j3, filePath);
}

void removeEmissiveFactors()
{
    QString baseFolder = "D:/salesapp/apps/3dplans/public/doma-3d-scene/project_images/TheParks/1.0/3dassets/exterior";
    QDirIterator it(baseFolder, { "*.gltf" }, QDir::Files);

    while (it.hasNext())
    {
        QString filePath = it.next();
        removeEmissive(filePath);
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //qDebug() << "Hello!";

    //getLevelNodeNames()
    //renameGLTFFiles();
    //applyAlphaCutoff();
    removeEmissiveFactors();

    QCoreApplication::exit();
    return a.exec();
}
