#ifndef KLUDGETHOTKEY_H
#define KLUDGETHOTKEY_H

#include <QWidget>
#include <QMap>

class HotKeyPrivate;
class HotKey : public QWidget
{
    Q_OBJECT
public:

    HotKey(QObject *parent = 0);

    void registerHotKey(const QString key, int id = 1);
    void registerHotKey(Qt::Key key, Qt::KeyboardModifier modifier, int id = 1);
    void unregisterHotKey(int id = 1);
    void unregisterAll();

#if defined(WIN32)
    bool winEvent(MSG *message, long *result);
#else
    bool x11Event(XEvent * ev);
#endif
    static QString keyName(Qt::Key key);

Q_SIGNALS:
    void hotKeyPressed(Qt::Key, Qt::KeyboardModifier);

private:
    QMap<int, QString> registeredKeys;
    HotKeyPrivate *hotkey_p;

};

#endif // KLUDGETHOTKEY_H
