#include "document.h"

Document::Document(QObject *parent)
    : QObject(parent)
    , modified_(false)
{

}

void Document::setModified(bool modified)
{
    modified_ = modified;
}
