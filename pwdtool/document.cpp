#include "document.h"
#include "../pwdlib/pwdlog.h"

#include <QWidget>

Document::Document(QWidget *widget, QObject *parent)
    : QObject(parent)
    , widget_(widget)
    , modified_(false)
    , pwdmgr_(new pwd::PwdMgr())
{

}

Document::~Document()
{
    delete pwdmgr_;
}

void Document::setModified(bool modified)
{
    if(modified_ == modified)
    {
        return;
    }
    modified_ = modified;
    refreshTitle();
}

void Document::refreshTitle()
{
    QString title = tr("Password Manager");
    if(!dataPath_.isEmpty())
    {
        title += "|";
        title += dataPath_;
    }
    if(modified_)
    {
        title += tr("(* modified)");
    }
    widget_->setWindowTitle(title);
}

pwd::LoaderError Document::load(const QString &path)
{
    dataPath_.clear();

    pwd::LoaderError ret = pwdmgr_->load(path.toStdString());
    if(ret == pwd::LoaderError::NoError)
    {
        dataPath_ = path;
    }

    refreshTitle();
    return ret;
}

pwd::LoaderError Document::save(const QString &path)
{
    pwd::LoaderError ret = pwdmgr_->save(path.toStdString());
    if(ret == pwd::LoaderError::NoError)
    {
        dataPath_ = path;
        setModified(false);
    }
    return ret;
}
