#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "document.h"
#include "../pwdlib/Pwd.h"

#include <QMessageBox>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QFileDialog>
#include <QApplication>

const std::wstring UserDataFile = L"pwd/pwd.dat";

const int DataRoleIndex = Qt::UserRole + 1;

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

    connect(ui->categoryView, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(onItemClicked(QTreeWidgetItem*,int)));
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
    if(doc_->getDataPath().isEmpty())
    {
        on_actionSaveAs_triggered();
        return;
    }

    if(!doc_->save())
    {
        QMessageBox::critical(NULL, tr("Error"), tr("Failed to save."));
        return;
    }

    doc_->setModified(false);
}

void MainWindow::on_actionSaveAs_triggered()
{
    QString path = QFileDialog::getSaveFileName(NULL, tr("Save"));
    if(path.isEmpty())
    {
        return;
    }

    if(!doc_->saveAs(path))
    {
        QMessageBox::critical(NULL, tr("Error"), tr("Failed to save."));
        return;
    }

    doc_->setModified(false);
}

void MainWindow::on_actionQuit_triggered()
{
    if(!checkModified())
    {
        return;
    }

    QApplication::quit();
}

void MainWindow::on_actionOpen_triggered()
{
    if(!checkModified())
    {
        return;
    }

    QString path = QFileDialog::getOpenFileName(NULL, tr("Open File"));
    if(!path.isEmpty())
    {
        QApplication::instance()->processEvents();

        if(!doc_->load(path))
        {
            QMessageBox::critical(NULL, tr("Error"), tr("Failed to open data."));
            return;
        }

        ui->categoryView->clear();
        QList<QTreeWidgetItem*> items;

        pwd::PwdMgr &mgr = *(doc_->getPwdMgr());
        for(const auto &pair : mgr)
        {
            const pwd::Pwd &info = pair.second;

            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, QString("%1").arg(info.id_));
            item->setText(1, QString::fromStdString(info.keyword_));
            item->setText(2, QString::fromStdString(info.name_));
            item->setData(0, DataRoleIndex, info.id_);
            items.append(item);
        }
        ui->categoryView->addTopLevelItems(items);
    }
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

void MainWindow::onItemClicked(QTreeWidgetItem * item, int /*column*/)
{
    if(!checkModified())
    {
        return;
    }

    pwd::pwdid id = item->data(0, DataRoleIndex).toUInt();

    const pwd::Pwd &info = doc_->getPwdMgr()->get(id);
    viewPwdInfo(info);
}

void MainWindow::viewPwdInfo(const pwd::Pwd &info)
{
    ui->edtID->setText(QString("%1").arg(info.id_));
    ui->edtKeyword->setText(QString::fromStdString(info.keyword_));
    ui->edtName->setText(QString::fromStdString(info.name_));
    ui->edtContent->setText(QString::fromStdString(info.desc_));
    ui->edtPassword->setText(QString::fromStdString(info.pwd_));
}
