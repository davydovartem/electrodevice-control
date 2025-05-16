#pragma once

#include <string>
#include "server.hpp"

class Command
{
protected:
    Server &_invoker;
    int _channel;

public:
    explicit Command(Server &server, int channel = 0) : _invoker(server), _channel(channel) {}
    virtual ~Command() = default;

    virtual std::string execute() = 0;
    virtual bool validate() { return _invoker.isChannelExists(_channel); }
};

class StartMeasureCommand : public Command
{
public:
    using Command::Command;
    std::string execute() override;
};

class StopMeasureCommand : public Command
{
public:
    using Command::Command;
    std::string execute() override;
};

class GetResultCommand : public Command
{
public:
    using Command::Command;
    std::string execute() override;
};

class CommandFactory
{
public:
    static std::unique_ptr<Command> create(Server &server, const std::string &commandStr);
    static int parseChannel(const std::string &channelStr);
    static int parseRange(const std::string &rangeStr);
};