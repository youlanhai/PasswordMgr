#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class Document;
class QTreeWidgetItem;

namespace pwd
{
    class Pwd;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool checkModified();

private slots:
    void on_btnSearch_clicked();

    void on_cobSearch_activated(int index);

    void on_actionSave_triggered();

    void on_actionSaveAs_triggered();

    void on_actionQuit_triggered();

    void on_actionOpen_triggered();

    void on_actionPwdNew_triggered();

    void on_actionPwdDelete_triggered();

    void on_actionPwdModify_triggered();

    void onItemClicked(QTreeWidgetItem * item, int column);

private:
    void viewPwdInfo(const pwd::Pwd &info);

    Ui::MainWindow *ui;
    Document*       doc_;
};

#endif // MAINWINDOW_H
