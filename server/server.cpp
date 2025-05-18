#include "server.hpp"
#include <boost/asio.hpp>
#include <iostream>
#include "command.hpp"

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
    std::cout << "Recieving commands from client" << std::endl;

    boost::asio::async_read_until(_socket, _buffer, '\n',
                                  [self = shared_from_this()](const boost::system::error_code &error, size_t bytes_transferred)
                                  {
                                      if (error)
                                      {
                                          std::cerr << "Read error: " << error.message() << std::endl;
                                          return;
                                      }
                                      self->handleRead(error, bytes_transferred);
                                  });
}

void ConnectionHandler::handleRead(const boost::system::error_code &error, size_t bytes_transferred)
{
    std::istream stream(&_buffer);
    std::string command;
    std::getline(stream, command);
    _buffer.consume(bytes_transferred);

    std::string response = _server.processCommand(command);

    boost::asio::async_write(_socket, boost::asio::buffer(response),
                             [self = shared_from_this()](const boost::system::error_code &error, size_t)
                             {
                                 if (error)
                                 {
                                     std::cerr << "Write error: " << error.message() << std::endl;
                                     return;
                                 }
                                 self->handleWrite(error);
                             });
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
    for (int i = 0; i < num_channels; ++i)
    {
        _channels.emplace(i, ChannelState{});
    }
    startAccept();
}
static std::mt19937 rng(std::random_device{}());

Server::~Server()
{
    _io_context.stop();
    unlink(_socket_path.c_str());
    std::cout << "Server stopped" << std::endl;
}

void Server::run()
{
    std::cout << "Server run" << std::endl;
    _io_context.run();
}

void Server::startAccept()
{
    std::cout << "Server starting accept commands" << std::endl;
    auto handler = std::make_shared<ConnectionHandler>(_io_context, *this);
    _acceptor.async_accept(handler->socket(),
                           [this, handler](const boost::system::error_code &error)
                           {
                               if (error)
                               {
                                   std::cerr << "Error in StartAccept(): " << error.message() << std::endl;
                                   return;
                               }
                               handleAccept(handler, error);
                           });
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

ChannelState &Server::getChannelState(int channel)
{
    auto it = _channels.find(channel);
    if (it == _channels.end())
    {
        throw std::out_of_range("Invalid channel number");
    }
    return it->second;
}

bool Server::isChannelExists(int channel) const
{
    return _channels.find(channel) != _channels.end();
}

std::string Server::processCommand(const std::string &command)
{
    std::cout << "Command received: " << command << std::endl;

    auto cmd = CommandFactory::create(*this, command);
    if (!cmd)
        return "fail\n";

    try
    {
        return cmd->execute();
    }
    catch (const std::exception &e)
    {
        return "fail\n";
    }
}