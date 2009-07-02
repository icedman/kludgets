#ifndef KSYSTEM_H
#define KSYSTEM_H

#include <QObject>
#include <QProcess>
#include <QStringList>
#include <QMap>

class KSettings;
class KWindow;

class KSystem : public QObject
{
    Q_OBJECT

public:
    KSystem(QObject* parent = 0);
    ~KSystem();

    void setShellPath(const QString path);
    void setEnableCommands(bool enable);

    public
Q_SLOTS:
    void log(QString message);
    void openURL(QString url);
    int execute(QString cmd);
    void close(long pid = 0);
    void write(QString string, long pid = 0);
    void cancel(long pid = 0, int wait = 0);
    QString error(long pid = 0);
    QString output(long pid = 0);
    int status(long pid = 0);
    bool hasError(long pid = 0);
    bool hasOutput(long pid = 0);

    QString version();

Q_SIGNALS:
    void execUpdate(long);
    void execFinish(long);

    private
Q_SLOTS:
    void readyReadStandardError();
    void readyReadStandardOutput();
    void finished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    KSystem *getProcess(long pid);

private:
    bool runSystemCommands;
    bool runInShell;
    QString shellPath;

    QProcess *process;
    int exitStatus;
    long pid;

    QStringList errorList;
    QStringList outputList;

    QMap<long, KSystem*> processes;
    QString tempFile;
};

#endif
