#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../pwdlib/pwdconfig.h"

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
    void on_actionSave_triggered();

    void on_actionSaveAs_triggered();

    void on_actionQuit_triggered();

    void on_actionOpen_triggered();

    void on_actionPwdNew_triggered();

    void on_actionPwdDelete_triggered();

    void onCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previos);
    void onContextMenuRequested(const QPoint &pt);

    void doSearch();
    void refreshCategoryView();
    void onContentModified(const QString &text = QString());

private:
    virtual void closeEvent(QCloseEvent * event) override;
    void viewPwdInfo(const pwd::Pwd &info);
    void savePwdInfo();
    QTreeWidgetItem* createTreeItem(const pwd::Pwd &info);
    QTreeWidgetItem* findTreeItem(pwd::pwdid id);

    Ui::MainWindow *ui;
    Document*       doc_;
    QString         defaultDataPath_;
    bool            isSynchronized_;

    pwd::pwdid      currentPwdID_;
    QMenu*          categoryMenu_;
};

#endif // MAINWINDOW_H
