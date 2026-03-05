#include <QApplication>
#include <QDebug>
#include <fstream>
#include <filesystem>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    QString filePath = "C:/Temp/cjk/許功蓋/test2.txt";

    std::filesystem::path path1(filePath.toStdWString());

    std::ofstream file(path1);

    file << "WTF!" << std::endl;
    file.flush();
    file.close();
    return 0;
}
