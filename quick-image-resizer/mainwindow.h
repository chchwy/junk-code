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
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

    void processImages(const QList<QString>& imgFiles);
    void resizeImage(QString imgFile);
    void resizeImageKeepAspectRatio(QString imgFile);

    void imgSizeComboChanged(QString s);

private:
    void writeSizeToJson(QString imgPath, int size);
    void quickAndDirtyResizing();
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
