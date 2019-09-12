#include <QCoreApplication>
#include <QCommandLineParser>

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("gltf-picker");
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("glTF-picker");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("source", QCoreApplication::translate("main", "Source file to copy."));
    parser.addPositionalArgument("destination", QCoreApplication::translate("main", "Destination directory."));

    // gltf-picker *.gltf -o folder
    parser.process(app.arguments());

    const QStringList args = parser.positionalArguments();
    // source is args.at(0), destination is args.at(1)



    return app.exec();
}
