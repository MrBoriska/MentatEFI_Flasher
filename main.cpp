#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setStyle("Fusion");

    QCoreApplication::setOrganizationName("MEMO ECU");
    QCoreApplication::setOrganizationDomain("vk.com/club184036019");
    QCoreApplication::setApplicationName("Flasher");

    MainWindow w;
    w.show();

    return a.exec();
}
