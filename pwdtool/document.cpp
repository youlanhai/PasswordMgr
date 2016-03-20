#include "document.h"

Document::Document(QObject *parent)
    : QObject(parent)
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
    modified_ = modified;
}

bool Document::load(const QString &path)
{
    dataPath_ = path;
    return pwdmgr_->load(path.toStdString());
}

bool Document::save()
{
    if(dataPath_.isEmpty())
    {
        return false;
    }
    return pwdmgr_->save(dataPath_.toStdString());
}

bool Document::saveAs(const QString &path)
{
    dataPath_ = path;
    return save();
}
