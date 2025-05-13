#pragma once

#include <string>

class Server {
public:
    Server(const std::string& socketPath, int numChannels);
    ~Server();
    
    void run();
    
private:
    void handleConnection(int clientFd);
    std::string processCommand(const std::string& command);
    
    int _serverFd;
    std::string _socketPath;
    int _numChannels;
};