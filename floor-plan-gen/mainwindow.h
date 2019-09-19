#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString randomText(int n);
    void Go();
private:
    QMap<int, int> m;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
