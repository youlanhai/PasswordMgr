#include "pwdsettingdialog.h"
#include "ui_pwdsettingdialog.h"

#include <QMessageBox>

PwdSettingDialog::PwdSettingDialog(const std::string &oldPassword, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PwdSettingDialog),
    oldPassword_(QString::fromStdString(oldPassword))
{
    ui->setupUi(this);
    ui->edtOldPwd->setEnabled(!oldPassword_.isEmpty());
}

PwdSettingDialog::~PwdSettingDialog()
{
    delete ui;
}

void PwdSettingDialog::on_btnOK_clicked()
{
    QString oldPwd = ui->edtOldPwd->text();
    if(!oldPassword_.isEmpty() && oldPwd.isEmpty())
    {
        QMessageBox::critical(nullptr, tr("Error"), tr("Please input current password."));
        return;
    }

    QString newPwd = ui->edtNewPwd->text();
    if(newPwd.isEmpty())
    {
        QMessageBox::critical(nullptr, tr("Error"), tr("Please input new password."));
        return;
    }

    QString repeatPwd = ui->edtRepeatPwd->text();
    if(repeatPwd.isEmpty())
    {
        QMessageBox::critical(nullptr, tr("Error"), tr("Please input the repeated password."));
        return;
    }

    if(newPwd != repeatPwd)
    {
        QMessageBox::critical(nullptr, tr("Error"), tr("The new password and repeated password was not equal."));
        return;
    }

    if(oldPwd != oldPassword_)
    {
        QMessageBox::critical(nullptr, tr("Error"), tr("The current password was incorrect."));
        return;
    }

    newPassword_ = newPwd.toStdString();
    accept();
}

void PwdSettingDialog::on_btnCancel_clicked()
{
    reject();
}
