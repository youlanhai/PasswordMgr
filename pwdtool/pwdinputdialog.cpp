#include "pwdinputdialog.h"
#include "ui_pwdinputdialog.h"
#include <QMessageBox>

PwdInputDialog::PwdInputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PwdInputDialog)
{
    ui->setupUi(this);
}

PwdInputDialog::~PwdInputDialog()
{
    delete ui;
}

void PwdInputDialog::on_btnOK_clicked()
{
    QString text = ui->edtPassword->text();
    if(text.isEmpty())
    {
        QMessageBox::critical(nullptr, tr("Error"), tr("Please input password"));
        return;
    }

    password_ = text.toStdString();

    accept();
}

void PwdInputDialog::on_btnCancel_clicked()
{
    reject();
}
