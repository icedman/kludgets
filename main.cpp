#include "config.h"
#include "kapp.h"
#include "kclient.h"
#include "kserver.h"
#include "kutil.h"

#include <QApplication>

int main(int argc, char *argv[])
{    
    KApp app(argc, argv);

    if (argc > 1)
    {
#if !defined(WIN32)
        if (QString(argv[1]) == "-l")
        {
            if (!app.startHotKeyListener())
                return 0;
        }
#endif
        if (!app.startClient(argv[argc-1]))
            return 0;
    }
    else
    {
        if (!app.startServer())
            return 0;
    }

    app.exec();

    return 0;
}
