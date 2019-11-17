#include "config.h"
#include "kapp.h"
#include "kclient.h"
#include "kserver.h"
#include "kutil.h"
// #include "theme.h"

#include <QApplication>
int main(int argc, char *argv[])
{
	
    // QApplication::setGraphicsSystem("software"); // opengl,raster

    KApp app(argc, argv);

    //Theme::loadTheme("skulpture");

    if (argc > 1)
    {
#if !defined(WIN32)
        if (QString(argv[1]) == "-l")
        {
            if (!app.startHotKeyListener())
                return 0;
        }
#endif
        if (QString(argv[1]) == "-p")
        {
            app.startServer();
            app.exec();
            return 0;
        }
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
