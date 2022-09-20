#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#include "tester.hpp"
#include "program.hpp"

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

  QApplication a(argc, argv);
  QFile stylesheetFile("../misc/stylesheet/stylesheet.qss");
  stylesheetFile.open(QFile::ReadOnly);
  QString styleSheet = QLatin1String(stylesheetFile.readAll());
  a.setStyleSheet(styleSheet);
  
  Program *program = new Program();

  res = a.exec();

  delete program;

  return res;
}
