#include "SDL.h"
#include "config.hpp"
#include "call_gui.hpp"
#include "logger.hpp"
#include "menu_gui.hpp"
#include <qthread.h>
#include <unistd.h>
#define DOCTEST_CONFIG_IMPLEMENT
#include "audio_device_config.hpp"
#include "doctest.h"
#include "program.hpp"
#include "tester.hpp"

#include <QApplication>
#include <QFile>
#include <iostream>
#include <thread>

#define WRAPPING 1

/* / *****!SECTION
Fix bug when hanging up while handshaking to reset on call boolean in call_gui class
******/

static Config *conf = Config::get_instance();

int main(int argc, char *argv[]) {

	Logger::set_priority(conf->get<int>("LOGGER_LEVEL"));
	Logger::debug_enable(conf->get<int>("DEBUG_ENABLE"));
	// This will run tests only when --test is passed to client

	LOG_INFO("Starting my_skype!")
	int res = Tester::test(argc, argv);

	//don't launch guy when testing.
	if (res > 0 || Tester::did_test(argc, argv)) {
		return res;
	}

	AudioDevice::start_sdl(); // SDL init

	QApplication a(argc, argv);
	QFile stylesheetFile("../misc/stylesheet/stylesheet.qss");
	stylesheetFile.open(QFile::ReadOnly);
	QString styleSheet = QLatin1String(stylesheetFile.readAll());
	a.setStyleSheet(styleSheet);

	Program *program = new Program();
// 	CallGui *call = new CallGui();
// 	call->show();
	res = a.exec();

	delete program;

	SDL_Quit();

	QThread::sleep(WRAPPING);

	return res;
}
