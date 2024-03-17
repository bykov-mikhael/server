/*
 * Подключение заголовочного файла
 */
#include "server.h"
/*
 * Подключение библиотек для работы с Qt
 */
#include <QLabel>
#include <QMessageBox>
#include <QTcpSocket>
#include <QTime>
#include <QVBoxLayout>

/**
 * @brief Server::Server ctor
 * @param m_port номер порта для подключения
 * @param parent родитель (по умолчанию =0)
 */
Server::Server(quint16 m_port, QWidget *parent) : QWidget(parent) {
  /*
   * Создание объекта QTcpServer *localServer
   * Выделение памяти происходит из кучи оператором new
   */
  localServer = new QTcpServer(this);

  if (!localServer->listen(QHostAddress::Any, m_port)) {
    QMessageBox::critical(
        0, "Server error",
        "Unable to start server:" + localServer->errorString());
    localServer->close();
    return;
  }

  // Соединяем сигнал сервера о наличии нового подключения с обработчиком нового
  // клиентского подключения
  QObject::connect(localServer, SIGNAL(newConnection()), this,
                   SLOT(slotNewConnection()));

  // Формируем окно для просмотра текстовых сообщений от клиента
  m_pteMessage = new QTextEdit;
  m_pteMessage->setReadOnly(true);

  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(new QLabel("Server: " + m_port));
  layout->addWidget(m_pteMessage);

  setLayout(layout);
}
/**
 * @brief Server::~Server dtor
 */
Server::~Server() {}
/**
 * @brief Server::slotNewConnection слот обработки нового клиентского
 * подключения
 */
void Server::slotNewConnection() {
  // Получаем сокет, подключённый к серверу
  QTcpSocket *localSocket = localServer->nextPendingConnection();

  // Соединяем сигнал отключения сокета с обработчиком удаления сокета
  connect(localSocket, SIGNAL(disconnected()),
          localSocket, SLOT(deleteLater()));

  // Соединяем сигнал сокета о готовности передачи данных с обработчиком данных
  connect(localSocket, SIGNAL(readyRead()),
          this, SLOT(slotReadClient()));

  // Отправляем информацию клиенту о соединении с сервером
  sendToClient(localSocket, "Server response: Connected!");
}
/**
 * @brief Server::slotReadClient слот чтения информации от клиента
 */
void Server::slotReadClient() {
  // Получаем QLocalSocket после срабатывания сигнала о готовности передачи
  // данных
  QTcpSocket *localSocket = (QTcpSocket *)sender();

  // Создаём входной поток получения данных на основе сокета
  QDataStream in(localSocket);

  // Устанавливаем версию сериализации данных потока. У клиента и сервера они
  // должны быть одинаковыми
  in.setVersion(QDataStream::Qt_5_3);

  // Бесконечный цикл нужен для приёма блоков данных разных размеров, от двух
  // байт и выше
  for (;;) {
    // Если размер блока равен нулю
    if (!nextBlockSize) {
      // Если размер передаваемого блока меньше двух байт, выйти из цикла
      if (localSocket->bytesAvailable() < (int)sizeof(quint16)) break;

      // Извлекаем из потока размер блока данных
      in >> nextBlockSize;
    }

    // Извлекаем из потока время и строку
    QTime time;
    QString string;
    in >> time >> string;

    // Преобразуем полученные данные и показываем их в виджете

    QString message = time.toString() + " " + "Client has sent - " + string;
    /*
     * вызов функции обмена данными с jira по протоколу Rest API
     */
    if (message.contains("jira")) {
      message = "get data from jira";
    }
    /*
     * вызов функции обмена данными с 1C по протоколу Rest API
     */
    if (message.contains("1c")) {
      message = "get data from 1C";
    }
    /*
     * вызов функции обмена данными с сетевой папкой по протоколу samba
     */
    if (message.contains("dir")) {
      message = "get data from shared folder";
    }

    m_pteMessage->append(message);

    nextBlockSize = 0;

    // Отправляем ответ клиенту
    sendToClient(localSocket, "Server response: received \"" + string + "\"");
  }
}
/**
 * @brief Server::sendToClient метод для отправки клиенту подтверждения о приёме
 * информации
 * @param localSocket
 * @param string
 */
void Server::sendToClient(QTcpSocket *localSocket, const QString &string) {
  // Поскольку заранее размер блока неизвестен (параметр string может быть любой
  // длины), вначале создаём объект array класса QByteArray
  QByteArray array;
  // На его основе создаём выходной поток
  QDataStream out(&array, QIODevice::WriteOnly);
  // Устанавливаем версию сериализации данных потока
  out.setVersion(QDataStream::Qt_5_3);
  // Записываем в поток данные для отправки. На первом месте идёт нулевой размер
  // блока
  out << quint16(0) << QTime::currentTime() << string;

  // Перемещаем указатель на начало блока
  out.device()->seek(0);
  // Записываем двухбайтное значение действительного размера блока без учёта
  // пересылаемого размера блока
  out << quint16(array.size() - sizeof(quint16));

  // Отправляем получившийся блок клиенту
  localSocket->write(array);
}
