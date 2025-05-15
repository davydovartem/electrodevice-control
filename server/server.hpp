#pragma once

#include <string>
#include <boost/asio.hpp>

using boost::asio::local::stream_protocol;

class ConnectionHandler;

class Server
{
public:
    Server(const std::string &socket_path, int num_channels);
    ~Server();

    void run();

private:
    friend class ConnectionHandler;

    void startAccept();
    void handleAccept(std::shared_ptr<ConnectionHandler> handler, const boost::system::error_code &error);

    boost::asio::io_context _io_context;
    stream_protocol::acceptor _acceptor;
    std::string _socket_path;
    int _num_channels;

    std::string processCommand(const std::string &command);
};

class ConnectionHandler : public std::enable_shared_from_this<ConnectionHandler>
{
public:
    ConnectionHandler(boost::asio::io_context &io_context, Server &server);
    stream_protocol::socket &socket();
    void start();

private:
    void handleRead(const boost::system::error_code &error, size_t bytes_transferred);
    void handleWrite(const boost::system::error_code &error);

    boost::asio::io_context &_io_context;
    stream_protocol::socket _socket;
    Server &_server;
    boost::asio::streambuf _buffer;
};
