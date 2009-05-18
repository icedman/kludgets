#ifndef KLUDGETHOTKEY_H
#define KLUDGETHOTKEY_H

#include <QWidget>

class HotKey : public QWidget
{
    Q_OBJECT
public:

    HotKey(QObject *parent = 0);

    void registerHotKey(Qt::Key key, Qt::KeyboardModifier modifier, int id = 1);
    void unregisterHotKey(Qt::Key key, Qt::KeyboardModifier modifier, int id = 1);
    bool winEvent(MSG *message, long *result);

    static QString keyName(Qt::Key key);

Q_SIGNALS:
    void hotKeyPressed(Qt::Key, Qt::KeyboardModifier);

};

#endif // KLUDGETHOTKEY_H
