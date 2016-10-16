#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "document.h"
#include "pwdsettingdialog.h"
#include "pwdinputdialog.h"

#include "../pwdlib/Pwd.h"
#include "../pwdlib/pwdlog.h"

#include <QMessageBox>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QFileDialog>
#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QCloseEvent>
#include <QMenu>
#include <openssl/ssl.h>

const char *DefaultDataPath = ".config/pwd";
const char *UserDataFile = "pwd.dat";

const int DataRoleIndex = Qt::UserRole + 1;

namespace SearchType
{
    const int Keyword = 0;
    const int Id = 1;
    const int Name = 2;
    const int Describe = 3;
    const int Password = 4;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    doc_(new Document(this)),
    isSynchronized_(false),
    currentPwdID_(0),
    categoryMenu_(new QMenu(this))
{
    ui->setupUi(this);

    QStringList searchTypes;
    searchTypes << tr("key") << tr("id") << tr("name") << tr("content") << tr("password");
    ui->cobSearch->addItems(searchTypes);

    QStringList categoryTitles;
    categoryTitles << tr("id") << tr("keyword") << tr("name");
    ui->categoryView->setHeaderLabels(categoryTitles);

    categoryMenu_->addAction(ui->actionPwdNew);
    categoryMenu_->addAction(ui->actionPwdDelete);

    connect(ui->categoryView, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(onCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
    connect(ui->categoryView, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(onContextMenuRequested(QPoint)));

    connect(ui->btnSearch, SIGNAL(clicked(bool)), this, SLOT(doSearch()));
    connect(ui->edtSearch, SIGNAL(returnPressed()), this, SLOT(doSearch()));
    connect(ui->cobSearch, SIGNAL(currentIndexChanged(int)), this, SLOT(doSearch()));
    connect(ui->btnShowAll, SIGNAL(clicked(bool)), this, SLOT(refreshCategoryView()));

    connect(ui->edtContent, SIGNAL(textChanged()), this, SLOT(onContentModified()));
    connect(ui->edtKeyword, SIGNAL(textChanged(QString)), this, SLOT(onContentModified(QString)));
    connect(ui->edtPassword, SIGNAL(textChanged(QString)), this, SLOT(onContentModified(QString)));
    connect(ui->edtName, SIGNAL(textChanged(QString)), this, SLOT(onContentModified(QString)));

    ui->actionHtmlMode->setChecked(true);

    defaultDataPath_ = QDir::home().absoluteFilePath(QString(DefaultDataPath));
    QDir dir(defaultDataPath_);
    if(!dir.exists())
    {
        dir.mkpath(defaultDataPath_);
        PWD_LOG_INFO("Create path '%s'", defaultDataPath_.toUtf8().data());
    }
}

MainWindow::~MainWindow()
{
    delete ui;

    //消除openssl链接错误
    OPENSSL_cleanse(0, 0);
}

void MainWindow::closeEvent(QCloseEvent * event)
{
    if(!checkModified())
    {
        event->ignore();
        return;
    }

    QMainWindow::closeEvent(event);
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
            on_actionSave_triggered();
        }
        else if(QMessageBox::Discard == ret)
        {
            doc_->setModified(false);
        }
    }
    return true;
}

void MainWindow::on_actionSave_triggered()
{
    if(doc_->getDataPath().isEmpty())
    {
        on_actionSaveAs_triggered();
        return;
    }

    doSave(doc_->getDataPath());
}

void MainWindow::on_actionSaveAs_triggered()
{
    QString path = QFileDialog::getSaveFileName(NULL, tr("Save"), defaultDataPath_);
    if(path.isEmpty())
    {
        return;
    }

    doSave(path);
}

void MainWindow::doSave(const QString &path)
{
    if(doc_->getPwdMgr()->getEncryptKey().empty())
    {
        PwdSettingDialog dialog("");
        if(QDialog::Accepted != dialog.exec())
        {
            return;
        }

        doc_->getPwdMgr()->setEncryptKey(dialog.getNewPassword());
    }

    if(doc_->isModified())
    {
        savePwdInfo();
    }

    pwd::LoaderError ret = doc_->save(path);
    if(ret != pwd::LoaderError::NoError)
    {
        QMessageBox::critical(NULL, tr("Error"), tr("Failed to save (%1).").arg((int)ret));
        return;
    }
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

    QString path = QFileDialog::getOpenFileName(NULL, tr("Open File"), defaultDataPath_);
    if(!path.isEmpty())
    {
        // clear current data
        ui->categoryView->clear();
        viewPwdInfo(pwd::Pwd());
        doc_->getPwdMgr()->setEncryptKey("");

        QApplication::instance()->processEvents();

        pwd::LoaderError ret = doc_->load(path);
        if(ret == pwd::LoaderError::EmptyPassword)
        {
            PwdInputDialog dialog;
            if(QDialog::Accepted != dialog.exec())
            {
                return;
            }

            doc_->getPwdMgr()->setEncryptKey(dialog.getPassword());
            ret = doc_->load(path);
        }

        if(ret != pwd::LoaderError::NoError)
        {
            QString msg = QString::fromUtf8(pwd::getErrorStr(ret));
            QMessageBox::critical(NULL, tr("Error"), tr("Failed to open data. Error:'%1'.").arg(msg));
            return;
        }

        refreshCategoryView();
    }
}

void MainWindow::on_actionPwdNew_triggered()
{
    if(!checkModified())
    {
        return;
    }

    viewPwdInfo(pwd::Pwd());
}

void MainWindow::on_actionPwdDelete_triggered()
{
    QTreeWidgetItem *item = ui->categoryView->currentItem();
    if(!item)
    {
        return;
    }

    pwd::pwdid id = item->data(0, DataRoleIndex).toUInt();

    const pwd::Pwd &info = doc_->getPwdMgr()->get(id);
    if(info.id_ != id)
    {
        QMessageBox::critical(NULL, tr("Error"), tr("The key '%1' doesn't exist").arg(id));
        return;
    }

    QString text = tr("Are you sure to delete '[%1]%2'").arg(QString::number(info.id_), QString::fromStdString(info.keyword_));
    if(QMessageBox::question(NULL, tr("Confirm"), text) == QMessageBox::Yes)
    {
        doc_->getPwdMgr()->del(id);
        doc_->setModified(true);

        delete item;
    }
}

void MainWindow::onCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem * previos)
{
    if(current == NULL)
    {
        return;
    }

    pwd::pwdid id = current->data(0, DataRoleIndex).toUInt();
    if(id == currentPwdID_)
    {
        return;
    }

    if(!checkModified())
    {
        ui->categoryView->setCurrentItem(previos);
        return;
    }

    const pwd::Pwd &info = doc_->getPwdMgr()->get(id);
    viewPwdInfo(info);
}

void MainWindow::onContextMenuRequested(const QPoint &pt)
{
    if(ui->categoryView->currentItem() != NULL &&
            !doc_->isModified())
    {
        categoryMenu_->exec(ui->categoryView->mapToGlobal(pt));
    }
}

void MainWindow::refreshCategoryView()
{
    ui->categoryView->clear();
    QList<QTreeWidgetItem*> items;

    pwd::PwdMgr &mgr = *(doc_->getPwdMgr());
    for(const auto &pair : mgr)
    {
        QTreeWidgetItem *item = createTreeItem(pair.second);
        items.append(item);
    }
    ui->categoryView->addTopLevelItems(items);
}

void MainWindow::viewPwdInfo(const pwd::Pwd &info)
{
    currentPwdID_ = info.id_;
    isSynchronized_ = true;

    ui->edtID->setText(QString::number(info.id_));
    ui->edtKeyword->setText(QString::fromStdString(info.keyword_));
    ui->edtName->setText(QString::fromStdString(info.name_));
    ui->edtPassword->setText(QString::fromStdString(info.pwd_));

    setDetailText(QString::fromStdString(info.desc_));

    isSynchronized_ = false;
}

bool MainWindow::savePwdInfo()
{
    pwd::Pwd info;
    info.id_ = currentPwdID_;
    info.keyword_ = ui->edtKeyword->text().toStdString();
    if(info.keyword_.empty())
    {
        QMessageBox::critical(NULL, tr("Error"), tr("The keyword must not be empty"));
        return false;
    }

    info.name_ = ui->edtName->text().toStdString();
    info.pwd_ = ui->edtPassword->text().toStdString();
    info.desc_ = ui->edtContent->toPlainText().toStdString();

    if(currentPwdID_ == 0)
    {
        currentPwdID_ = doc_->getPwdMgr()->add(info);
        info.id_ = currentPwdID_;

        ui->edtID->setText(QString::number(currentPwdID_));

        QTreeWidgetItem *item = createTreeItem(info);
        ui->categoryView->addTopLevelItem(item);
    }
    else
    {
        doc_->getPwdMgr()->modify(currentPwdID_, info);

        QTreeWidgetItem *item = findTreeItem(currentPwdID_);
        if(item == NULL)
        {
            QMessageBox::critical(NULL, tr("Error"), tr("The id[%1] was not found").arg(currentPwdID_));
        }
        else
        {
            item->setText(1, QString::fromStdString(info.keyword_));
            item->setText(2, QString::fromStdString(info.name_));
        }
    }
    return true;
}

void MainWindow::doSearch()
{
    ui->categoryView->clear();

    QString text = ui->edtSearch->text();
    if(text.isEmpty())
    {
        return;
    }

    pwd::IdVector ids;
    pwd::PwdMgr &pwdmgr = *(doc_->getPwdMgr());

    std::string strSearch = text.toStdString();
    int searchType = ui->cobSearch->currentIndex();
    switch(searchType)
    {
    case SearchType::Keyword:
        pwdmgr.searchByKeyword(ids, strSearch);
        break;

    case SearchType::Name:
        pwdmgr.searchByName(ids, strSearch);
        break;

    case SearchType::Describe:
        pwdmgr.searchByDesc(ids, strSearch);
        break;

    case SearchType::Password:
        pwdmgr.searchByPwd(ids, strSearch);
        break;

    case SearchType::Id:
        {
            pwd::pwdid pid = atoi(strSearch.c_str());
            if(pwdmgr.exist(pid))
            {
                ids.push_back(pid);
            }
            break;
        }

    default:
        break;
    }

    QList<QTreeWidgetItem*> items;
    for(pwd::IdVector::iterator it = ids.begin(); it != ids.end(); ++it)
    {
        const pwd::Pwd & info = pwdmgr.get(*it);

        QTreeWidgetItem *item = createTreeItem(info);
        items.append(item);
    }
    ui->categoryView->addTopLevelItems(items);
}

void MainWindow::onContentModified(const QString & /*text*/)
{
    if(!isSynchronized_)
    {
        doc_->setModified(true);
    }
}

QTreeWidgetItem* MainWindow::createTreeItem(const pwd::Pwd &info)
{
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, QString::number(info.id_));
    item->setText(1, QString::fromStdString(info.keyword_));
    item->setText(2, QString::fromStdString(info.name_));
    item->setData(0, DataRoleIndex, info.id_);
    return item;
}

QTreeWidgetItem* MainWindow::findTreeItem(pwd::pwdid id)
{
    int n = ui->categoryView->topLevelItemCount();
    for(int i = 0; i < n; ++i)
    {
        QTreeWidgetItem *item = ui->categoryView->topLevelItem(i);
        if(item->data(0, DataRoleIndex) == id)
        {
            return item;
        }
    }
    return NULL;
}

void MainWindow::on_actionChangePassword_triggered()
{
    PwdSettingDialog dialog(doc_->getPwdMgr()->getEncryptKey());
    if(QDialog::Accepted == dialog.exec())
    {
        doc_->getPwdMgr()->setEncryptKey(dialog.getNewPassword());
        doc_->setModified(true);
    }
}

void MainWindow::on_actionHtmlMode_triggered(bool checked)
{
    isSynchronized_ = true;

    QString text = ui->edtContent->toPlainText();
    setDetailText(text);

    isSynchronized_ = false;
}

void MainWindow::setDetailText(const QString &text)
{
    if(ui->actionHtmlMode->isChecked())
    {
        ui->edtContent->setText(text);
    }
    else
    {
        ui->edtContent->setPlainText(text);
    }
}
