#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QObject>

class Document : public QObject
{
    Q_OBJECT
public:
    explicit Document(QObject *parent = 0);

    bool isModified() const { return modified_; }
    void setModified(bool modified);

signals:

public slots:

private:
    bool    modified_;
};

#endif // DOCUMENT_H
