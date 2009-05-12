#ifndef KLOG_H
#define KLOG_H

#include <QObject>

class KLog : public QObject
{
    Q_OBJECT
public:

    static KLog* instance();

    KLog();
    void setPath(const QString &path);
    void clear();
    void write(const QString &message, bool clear = false);

private:

    QString path;

};

#endif // KLOG_H
