#include "mainwindow.h"
#include <QApplication>
#include <QSurfaceFormat>
#include <QColorDialog>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  QSurfaceFormat glFormat;
  glFormat.setProfile(QSurfaceFormat::CoreProfile);
  glFormat.setVersion(4, 1);
  glFormat.setOption(QSurfaceFormat::DebugContext);
  QSurfaceFormat::setDefaultFormat(glFormat);

  MainWindow app;
  app.show();

  return a.exec();
}
