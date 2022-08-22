#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#include "skype_gui.hpp"
#include "job_bus.hpp"

#include <thread>
#include <QApplication>
#include <iostream>
#include <QFile>

int main(int argc, char *argv[])
{
    std::thread bus_loop(&JobBus::main_loop);
    Client client;
    
    QApplication a(argc, argv);
    QFile stylesheetFile("/home/khalil/Documents/Skype/src/client/gui/stylesheet/Hookmark.qss");
    stylesheetFile.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(stylesheetFile.readAll());
    a.setStyleSheet(styleSheet);

    SkypeGui welcome;
    welcome.show();

    a.exec();

    JobBus::set_exit();
    bus_loop.join();
    return 0;
}
