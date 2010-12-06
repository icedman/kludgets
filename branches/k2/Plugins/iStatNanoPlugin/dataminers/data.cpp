#include "data.h"

QString convertToBytesString(double value)
{
    if (value < 1024)
        return QString::number(value) + "KB";

    value = value / 1024;
    QString types[] = {"KB", "MB", "GB", "TB"};

    int i = 0;
    if (value < 1024)
    {
        return QString::number(value, 'f', 2) + types[i];
    }
    float output = value;
    while (output > 1000)
    {
        output = output / 1024;
        i++;
    }
    if (i > 4)
        return QString::number(value, 'f', 2) + "KB";

    return QString::number(output, 'f', 2) + types[i];
}

Data::Data(QObject *parent) :
        QObject(parent),
        valid(false)
{
    setObjectName("data");
}

bool Data::contains(QString name)
{
    return value(name).isValid();
}

QVariant Data::value(QString name)
{
    return property(name.toAscii());
}

void Data::insert(QString name, QVariant value)
{
    setProperty(name.toAscii(), value);
}

bool Data::isValid()
{
    return valid;
}

void Data::setValid(bool v)
{
    valid = v;
}
