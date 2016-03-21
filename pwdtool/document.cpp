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
    if(modified == modified_)
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

bool Document::load(const QString &path)
{
    if(pwdmgr_->load(path.toStdString()))
    {
        dataPath_ = path;
        refreshTitle();

        PWD_LOG_INFO("Load data file '%s'", path.toUtf8().data());
        return true;
    }
    return false;
}

bool Document::save()
{
    if(dataPath_.isEmpty())
    {
        return false;
    }

    if(!pwdmgr_->save(dataPath_.toStdString()))
    {
        return false;
    }

    setModified(false);
    return true;
}

bool Document::saveAs(const QString &path)
{
    dataPath_ = path;
    return save();
}
