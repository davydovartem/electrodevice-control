#include "client.hpp"
#include <QDebug>
#include <QLocalSocket>

SocketClient::SocketClient(QObject *parent) : QObject(parent)
{
    m_commandSocket = new QLocalSocket(this);
    m_statusSocket = new QLocalSocket(this);
    m_statusTimer = new QTimer(this);

    connect(m_commandSocket, &QLocalSocket::connected, this, &SocketClient::onConnected);
    connect(m_commandSocket, &QLocalSocket::disconnected, this, &SocketClient::onDisconnected);
    connect(m_commandSocket, &QLocalSocket::readyRead, this, &SocketClient::onCommandReceived);

    connect(m_statusSocket, &QLocalSocket::readyRead, this, &SocketClient::onStatusReceived);

    m_statusTimer->setInterval(1000);
    connect(m_statusTimer, &QTimer::timeout, this, &SocketClient::requestStatus);
}

void SocketClient::connectToServer(const QString &socketPath)
{
    m_commandSocket->connectToServer(socketPath);
    if (m_commandSocket->state() == QLocalSocket::ConnectedState)
    {
        m_connected = true;
        emit connectedChanged(true);
    }
    m_statusSocket->connectToServer(socketPath);
}

void SocketClient::disconnectFromServer()
{
    if (m_commandSocket->state() == QLocalSocket::ConnectedState)
    {
        m_commandSocket->disconnectFromServer();
        m_connected = false;
        emit connectedChanged(false);
    }
    if (m_statusSocket->state() == QLocalSocket::ConnectedState)
    {
        m_statusSocket->disconnectFromServer();
    }
}

void SocketClient::onConnected()
{
    m_statusTimer->start();
    m_connected = true;
    emit connectedChanged(m_connected);
}

void SocketClient::onDisconnected()
{
    m_statusTimer->stop();
    m_connected = false;
    emit connectedChanged(m_connected);
}

bool SocketClient::isConnected() const
{
    return m_connected;
}

void SocketClient::sendCommand(const QString &command)
{
    if (m_commandSocket->state() == QLocalSocket::ConnectedState)
    {
        m_commandSocket->write(command.toUtf8() + "\n");
    }
}

void SocketClient::onCommandReceived()
{
    while (m_commandSocket->canReadLine())
    {
        QString response = QString::fromUtf8(m_commandSocket->readLine()).trimmed();
        emit responseReceived(response);
    }
}

void SocketClient::onStatusReceived()
{
    while (m_statusSocket->canReadLine())
    {
        QString response = QString::fromUtf8(m_statusSocket->readLine()).trimmed();

        QStringList parts = response.split(", ");
        if (parts.size() >= 2)
        {
            QString state = parts[1];
            if (m_status != state)
            {
                m_status = state;
                emit statusChanged(m_status);
            }
        }
    }
}

void SocketClient::requestStatus()
{

    QString command = QString("get_status channel%1").arg(m_channelNumber);
    if (m_statusSocket->state() == QLocalSocket::ConnectedState)
    {
        m_statusSocket->write(command.toUtf8() + "\n");
    }
}

QString SocketClient::status() const
{
    return m_status;
}

int SocketClient::channelNumber() const
{
    return m_channelNumber;
}

void SocketClient::setChannelNumber(int number)
{
    if (m_channelNumber != number)
    {
        m_channelNumber = number;
        emit channelNumberChanged(m_channelNumber);
    }
}