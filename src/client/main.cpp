#define DOCTEST_CONFIG_IMPLEMENT
#include "audio_device_config.hpp"
#include "doctest.h"
#include "program.hpp"
#include "tester.hpp"

#include <QApplication>
#include <QFile>
#include <iostream>
#include <thread>

int main(int argc, char *argv[]) {

  // This will run tests only when --test is passed to client
  int res = Tester::test(argc, argv);

  if (res > 0) {
    return res;
  }

  Uint32 subsystem_init = 0;

  if (SDL_WasInit(subsystem_init & SDL_INIT_AUDIO) <= 0) {
    SDL_Init(SDL_INIT_AUDIO);
  }

  QApplication a(argc, argv);
  QFile stylesheetFile("../misc/stylesheet/stylesheet.qss");
  stylesheetFile.open(QFile::ReadOnly);
  QString styleSheet = QLatin1String(stylesheetFile.readAll());
  a.setStyleSheet(styleSheet);

  Program *program = new Program();

  res = a.exec();

  delete program;

  SDL_Quit();

  return res;
}
