#ifndef PWDINPUTDIALOG_H
#define PWDINPUTDIALOG_H

#include <QDialog>

namespace Ui {
    class PwdInputDialog;
}

class PwdInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PwdInputDialog(QWidget *parent = 0);
    ~PwdInputDialog();

    const std::string& getPassword() const { return password_; }
    void setDescription(const QString &path);

private slots:
    void on_btnOK_clicked();

    void on_btnCancel_clicked();

private:
    Ui::PwdInputDialog *ui;
    std::string  	password_;
};

#endif // PWDINPUTDIALOG_H
