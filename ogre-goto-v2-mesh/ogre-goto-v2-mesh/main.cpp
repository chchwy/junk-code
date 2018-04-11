#include <QCoreApplication>
#include <QDirIterator>
#include <QDebug>
#include <QProcess>

bool executeCommand( QString strCmd )
{
    qDebug() << strCmd;

    QProcess ogreMeshTool;
    ogreMeshTool.start( strCmd );
    if (ogreMeshTool.waitForStarted() == true)
    {
        if (ogreMeshTool.waitForFinished() == true)
        {
            qDebug() << "stdout: " + ogreMeshTool.readAllStandardOutput();
            qDebug() << "stderr: " + ogreMeshTool.readAllStandardError();
        }
        else
        {
            qDebug() << "ERROR: FFmpeg did not finish executing.";
            return false;
        }
    }
    else
    {
        qDebug() << "ERROR: Could not execute FFmpeg.";
        return false;
    }
    return true;
}

void Go()
{
    QString inFolder = "C:/Users/Matt/Desktop/Crown_3/script/mesh/others";
    QString outFolder = "C:/Users/Matt/Desktop/Crown_3/script/mesh/others/out";
    QString sOgreMeshTool = "C:/SDK/OgreSDK/Ogre/Build/sdk/bin/release/OgreMeshTool.exe";

    QDirIterator it(inFolder, QDir::Files);

    while(it.hasNext())
    {
        QString fileFullPath = it.next();
        qDebug() << "file: " << fileFullPath;

        QString fileName = QFileInfo(fileFullPath).fileName();
        QString outFile = QDir(outFolder).filePath(fileName);

        QString strCmd2 = QString("\"%1\"").arg(sOgreMeshTool);
        strCmd2 += " -v2 -e -t -ts 4 -O puqs";
        strCmd2 += QString(" \"%1\"").arg(fileFullPath);
        strCmd2 += QString(" \"%1\"").arg(outFile);

        executeCommand(strCmd2);
    }
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Go();

    return a.exec();
}
