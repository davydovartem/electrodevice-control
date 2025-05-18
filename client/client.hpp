#include <QObject>
#include <QLocalSocket>
#include <QTimer>

class SocketClient : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(int channelNumber READ channelNumber WRITE setChannelNumber NOTIFY channelNumberChanged)
public:
    explicit SocketClient(QObject* parent = nullptr);
    
    Q_INVOKABLE void connectToServer(const QString& socketPath);
    Q_INVOKABLE void sendCommand(const QString& command);
    Q_INVOKABLE void requestStatus();
    Q_INVOKABLE void disconnectFromServer();
    
signals:
    void channelNumberChanged(int newNumber);
    void statusChanged(const QString& status);
    void connectedChanged(bool connected);
    void responseReceived(const QString& response);

private slots:
    void onConnected();
    void onDisconnected();
    void onCommandReceived();
    void onStatusReceived();

private:
    int m_channelNumber = 0;
    int channelNumber() const;
    void setChannelNumber(int number);
    bool m_connected = false;
    QString m_status;
    bool isConnected() const;
    QString status() const;
    QLocalSocket* m_commandSocket;
    QLocalSocket* m_statusSocket;
    QTimer* m_statusTimer;
};

Q_DECLARE_METATYPE(QLocalSocket::LocalSocketError)