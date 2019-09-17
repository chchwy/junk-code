#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

    void processAllGltf(QList<QString> gltfFiles);
    bool processOneGltf(QString gltfPath);

    bool writeJson(QJsonObject rootObject, QString outPath);

    void log(QString msg);
private:
    QString mErrors;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
