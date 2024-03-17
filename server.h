#include <QWidget>
#include <QLocalServer>
#include <QTcpServer>
#include <QTextEdit>
#include <QPushButton>

class Server : public QWidget
{
    Q_OBJECT

public:
    Server(quint16 m_port, QWidget* parent = 0);
    ~Server();

private:
    // Указатель на QLocalServer
    QTcpServer *localServer;

    // Указатель на QTextEdit, в котором будет показываться поступающая
    // от клиента информация
    QTextEdit *m_pteMessage;

    // Переменная для хранения размера получаемого от клиента блока
    quint16 nextBlockSize;

    qint16 m_port;

    // Метод для отправки клиенту подтверждения о приёме информации
    void sendToClient(QTcpSocket* localSocket, const QString& string);

public slots:
    // Слот обработки нового клиентского подключения
    virtual void slotNewConnection();

    // Слот чтения информации от клиента
    void slotReadClient();
};
