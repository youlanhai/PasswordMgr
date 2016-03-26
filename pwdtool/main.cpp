
#include <QApplication>
#include "mainwindow.h"

#include "../pwdlib/util.h"

int main(int argc, char **argv)
{
    pwd::test_util();

    // create the application instance
    QApplication app(argc, argv);

#ifdef Q_OS_MAC
    app.setAttribute(Qt::AA_DontShowIconsInMenus);
#endif

#if QT_VERSION >= 0x050100
    // Enable support for highres images (added in Qt 5.1, but off by default)
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    app.processEvents();

    MainWindow w;
    w.show();

    return app.exec();
}
