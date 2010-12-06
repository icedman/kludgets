#ifndef KLUDGETHOTKEY_H
#define KLUDGETHOTKEY_H

#include <QWidget>
#include <QMap>

class QProcess;
class HotKey : public QWidget
{
    Q_OBJECT
public:

    HotKey(QObject *parent = 0);
    ~HotKey();

    void registerHotKey(const QString key, int id = 1);
    void unregisterHotKey(int id = 1);
    void unregisterAll();

#if defined(WIN32)
    bool winEvent(MSG *message, long *result);
#else
    bool run();
    bool createExternalListener();
    void destroyExternalListener();

    private
Q_SLOTS:
    void externalListenerError();
    void externalListenerOutput();
#endif

public:
    static QString keyName(Qt::Key key);

Q_SIGNALS:
    void hotKeyPressed(Qt::Key, Qt::KeyboardModifier);

private:

    void registerHotKey(Qt::Key key, Qt::KeyboardModifier modifier, int id = 1);

private:
    QMap<int, QString> registeredKeys;
    QProcess *externalListener;

};

#endif // KLUDGETHOTKEY_H
