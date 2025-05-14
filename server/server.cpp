#include "server.hpp"
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <iostream>

ConnectionHandler::ConnectionHandler(boost::asio::io_context &io_context, Server &server)
    : _io_context(io_context), _server(server), _socket(io_context)
{
}

stream_protocol::socket &ConnectionHandler::socket()
{
    return _socket;
}

void ConnectionHandler::start()
{
    _socket.async_read_some(boost::asio::buffer(_buffer),
                            boost::bind(&ConnectionHandler::handleRead, shared_from_this(),
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
}

void ConnectionHandler::handleRead(const boost::system::error_code &error, size_t bytes_transferred)
{
    if (!error)
    {
        std::string command(_buffer.data(), bytes_transferred);
        std::cout << "Received command: " << command << std::endl;
        std::string response = _server.processCommand(command);
        boost::asio::async_write(_socket, boost::asio::buffer(response),
                                 boost::bind(&ConnectionHandler::handleWrite, shared_from_this(),
                                             boost::asio::placeholders::error));
    }
}

void ConnectionHandler::handleWrite(const boost::system::error_code &error)
{
    if (!error)
    {
        start();
    }
}

Server::Server(const std::string &socket_path, int num_channels)
    : _socket_path(socket_path), _num_channels(num_channels),
      _io_context(), _acceptor(_io_context, stream_protocol::endpoint(socket_path))
{
    startAccept();
}

Server::~Server()
{
    _io_context.stop();
    unlink(_socket_path.c_str());
    std::cout << "Server stopped" << std::endl;
}

void Server::run()
{
    std::cout << "Server::run()" << std::endl;
    _io_context.run();
}

void Server::startAccept()
{
    try
    {
        auto handler = std::make_shared<ConnectionHandler>(_io_context, *this);
        _acceptor.async_accept(handler->socket(),
                               boost::bind(&Server::handleAccept, this, handler, boost::asio::placeholders::error));
        std::cout << "Server started acceptor" << std::endl;
    }
    catch (const boost::system::system_error &e)
    {
        std::cerr << "Error in StartAccept(): " << e.what() << std::endl;
        throw;
    }
}

void Server::handleAccept(std::shared_ptr<ConnectionHandler> handler, const boost::system::error_code &error)
{
    if (!error)
    {
        std::cout << "Accepted connection from client" << std::endl;
        handler->start();
    }

    startAccept();
}

std::string Server::processCommand(const std::string &command)
{
    return "Command received: " + command + "\n";
}