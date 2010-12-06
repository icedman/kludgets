#ifndef DATAMINER_DATA_H
#define DATAMINER_DATA_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QMap>

class Data : public QObject
{
    Q_OBJECT

public:
    Data(QObject *parent = 0);

    public
Q_SLOTS:

    QVariant value(QString name);
    void insert(QString name, QVariant);
    bool contains(QString name);
    bool isValid();

public:
    void setValid(bool);

private:
    QMap<QString, QVariant> data;
    bool valid;
};

QString convertToBytesString(double bytes);

#endif // DATAMINER_DATA_H
