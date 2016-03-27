#ifndef PWDSETTINGDIALOG_H
#define PWDSETTINGDIALOG_H

#include <QDialog>

namespace Ui {
    class PwdSettingDialog;
}

class PwdSettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PwdSettingDialog(const std::string &oldPassword, QWidget *parent = 0);
    ~PwdSettingDialog();

    const std::string& getNewPassword() const { return newPassword_; }

private slots:
    void on_btnOK_clicked();

    void on_btnCancel_clicked();

private:
    Ui::PwdSettingDialog *ui;
    std::string 	newPassword_;
    QString 		oldPassword_;
};

#endif // PWDSETTINGDIALOG_H
