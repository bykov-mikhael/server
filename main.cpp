/*
 * TODO перенос на boost::asio; принцип работы одинаковый, сначала запускается
 * сервер - listen(номер порта), затем к нему подключаются клиенты через socket
 * TODO перенос в visual studio code с изменением архзитектуры проекта (папки и
 * классы)
 * TODO разобраться с cmake при подключении библиотек из класса окна
 * BUG не работают кастомные слоты через сигналы и отдельный класс
 * графической оболочки
 * NOTE реализовать обмен с информационными системами по
 * протоколоам REST API и samba
 */

#include <QApplication>
#include <QDebug>

#include "server.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  Server server(2323);
  server.show();

  return app.exec();
}
