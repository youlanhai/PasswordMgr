#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "document.h"
#include "../pwdlib/Pwd.h"

#include <QMessageBox>
#include <QTreeWidget>
#include <QTreeWidgetItem>

const std::wstring UserDataFile = L"pwd/pwd.dat";

namespace SearchType
{
    const int Keyword = 0;
    const int Id = 1;
    const int Name = 2;
    const int Describe = 3;
    const int Password = 4;
}

namespace EditMode
{
    const int None = -1;
    const int Read = 0;
    const int New = 1;
    const int Edit = 2;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    doc_(new Document(this))
{
    ui->setupUi(this);

    QStringList searchTypes;
    searchTypes << tr("key") << tr("id") << tr("name") << tr("content") << tr("password");
    ui->cobSearch->addItems(searchTypes);

    QStringList categoryTitles;
    categoryTitles << tr("id") << tr("keyword") << tr("name");
    ui->categoryView->setHeaderLabels(categoryTitles);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::checkModified()
{
    if(doc_->isModified())
    {
        int ret = QMessageBox::question(
            nullptr, tr("Question"),
            tr("The file has been modified, would you like to save?"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Abort,
            QMessageBox::Abort);

        if(QMessageBox::Abort == ret)
        {
            return false;
        }
        else if(QMessageBox::Save == ret)
        {
            //do save
        }
    }
    return true;
}

void MainWindow::on_btnSearch_clicked()
{

}

void MainWindow::on_cobSearch_activated(int index)
{

}

void MainWindow::on_actionSave_triggered()
{
    doc_->setModified(false);
}

void MainWindow::on_actionSaveAs_triggered()
{

}

void MainWindow::on_actionQuit_triggered()
{
    if(!checkModified())
    {
        return;
    }
}

void MainWindow::on_actionOpen_triggered()
{

}

void MainWindow::on_actionPwdNew_triggered()
{
    if(!checkModified())
    {
        return;
    }

    doc_->setModified(true);
}

void MainWindow::on_actionPwdDelete_triggered()
{

}

void MainWindow::on_actionPwdModify_triggered()
{

}
