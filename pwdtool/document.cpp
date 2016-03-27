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
    dataPath_ = path;
    refreshTitle();

    return pwdmgr_->load(path.toStdString());
}

pwd::LoaderError Document::save(const QString &path)
{
    dataPath_ = path;
    refreshTitle();
    return pwdmgr_->save(dataPath_.toStdString());
}
