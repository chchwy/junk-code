#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>

namespace Ui { class MainWindow; }
class QPushButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void makeEnvButtons();
    void buttonClicked();
    void refresh();
private:
    QString baseFolder;
    QList<QPushButton*> buttonList;
    Ui::MainWindow* ui = nullptr;
};
#endif // MAINWINDOW_H
