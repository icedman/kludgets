#ifndef KLUDGETSETTINGS_H
#define KLUDGETSETTINGS_H

#include <QObject>
#include <QVariant>

class QSettings;

class KSettings : public QObject
{
    Q_OBJECT

public:
    KSettings(QObject* parent = 0, const QString &path = "");
    ~KSettings();

    QString path();
    void setPath(const QString &);
    void setRootKey(const QString &);
    void setReadOnly(bool r);
    QString prependRootKey(const QString &);
    void sync();

    public
Q_SLOTS:
    QVariant read(const QString &key, QVariant defaultValue);
    bool write(const QString &key, QVariant value);
    bool contains(const QString &key);
    void clear();
    bool isReadOnly();

private:
    QSettings *settings;
    QString settingsPath;
    QString rootKey;
    bool readOnly;
};

#endif // KLUDGETSETTINGS_H
