#include "theme.h"
#include "skulpture.h"

#include <QApplication>

void computeColorGroups(QPalette &palette, bool kdeMode = false, bool makeDisabledWidgetsTransparent = true);

bool Theme::loadTheme(const QString &filename)
{
    QApplication *app = (QApplication*)QApplication::instance();
    app->setStyle(new SkulptureStyle);
    // app->setFont(font());

    QPalette p;
    p.setBrush(QPalette::Base, QColor(255,255,255));
    p.setBrush(QPalette::Background, QColor(240, 240, 240));
    p.setBrush(QPalette::Button, QColor(220, 220, 220));
    p.setBrush(QPalette::Highlight, QColor(80, 80, 80));
    p.setBrush(QPalette::Foreground, QColor(60,60,60));
    p.setBrush(QPalette::ButtonText, QColor(40,40,40));
    p.setBrush(QPalette::Text, QColor(40,40,40));
    computeColorGroups(p);
    app->setPalette(p);

    return true;
}
