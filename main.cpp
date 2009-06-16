#include "config.h"
#include "kapp.h"
#include "kclient.h"
#include "kserver.h"
#include "util.h"

#include <QApplication>

int main(int argc, char *argv[])
{    
    KApp *app = new KApp(argc, argv);

    if (argc > 1)
    {
        if (!app->startClient(argv[argc-1]))
            return 0;
    }
    else
    {
        if (!app->startServer())
            return 0;
    }

    return app->exec();
}
