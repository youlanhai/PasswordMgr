#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QObject>
#include "../pwdlib/PwdMgr.h"

class Document : public QObject
{
    Q_OBJECT
public:
    explicit Document(QWidget *widget, QObject *parent = 0);
    ~Document();

    pwd::LoaderError load(const QString &path);
    pwd::LoaderError save(const QString &path);

    pwd::PwdMgr* getPwdMgr(){ return pwdmgr_; }
    const QString& getDataPath() const { return dataPath_; }

    bool isModified() const { return modified_; }
    void setModified(bool modified);

    void refreshTitle();
signals:

public slots:

private:
    QWidget*        widget_;
    bool            modified_;

    QString         dataPath_;
    pwd::PwdMgr*    pwdmgr_;
};

#endif // DOCUMENT_H
