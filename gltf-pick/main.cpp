#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include "gltfpicker.h"
#include "mainwindow.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("gltf-picker");
    QApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("glTF-picker");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("source", QCoreApplication::translate("main", "Source file to copy."));
    parser.addPositionalArgument("destination", QCoreApplication::translate("main", "Destination directory."));
    parser.process(a.arguments());
    const QStringList args = parser.positionalArguments();
    if (args.size() >= 2)
    {
        // gltf-picker *.gltf -o folder
        // source is args.at(0), destination is args.at(1)
        GLTFPicker picker;
        bool ok = picker.Run(args[0], args[1]);
        if (ok)
        {
            qInfo() << "Done.";
        }
        else
        {
            qInfo() << "Oooops! something went wrong.";
            qInfo() << "=============================";
            qInfo() << picker.error();
            qInfo() << "=============================";
        }
        QApplication::exit(0);
        return 0;
    }

    MainWindow w;
    w.show();

    return a.exec();
}
