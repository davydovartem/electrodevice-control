#include "command.hpp"
#include "server.hpp"
#include <random>
#include <charconv>

std::unique_ptr<Command> CommandFactory::create(Server &server, const std::string &commandStr)
{
    std::istringstream iss(commandStr);
    std::string cmd;
    iss >> cmd;

    std::string channel;
    iss >> channel;
    int ch = parseChannel(channel);
    if (ch == -1)
    {
        return nullptr;
    }

    if (cmd == "start_measure")
    {
        return std::make_unique<StartMeasureCommand>(server, ch);
    }
    if (cmd == "stop_measure")
    {
        return std::make_unique<StopMeasureCommand>(server, ch);
    }
    if (cmd == "get_result")
    {
        return std::make_unique<GetResultCommand>(server, ch);
    }
    if (cmd == "get_status")
    {
        return std::make_unique<GetStatusCommand>(server, ch);
    }

    return nullptr;
}

std::string StartMeasureCommand::execute()
{
    if (!validate())
        return "fail\n";

    auto &channel = _invoker.getChannelState(_channel);

    static thread_local std::mt19937 rng(std::random_device{}());
    // 5% chance of error
    if (std::bernoulli_distribution(0.05)(rng))
    {
        channel.setState(ChannelState::ERROR);
        return "fail\n";
    }

    channel.setState(ChannelState::MEASURE);
    return "ok\n";
}

std::string StopMeasureCommand::execute()
{
    if (!validate())
        return "fail\n";

    auto &channel = _invoker.getChannelState(_channel);

    static thread_local std::mt19937 rng(std::random_device{}());

    if (channel.getState() != ChannelState::MEASURE)
    {
        return "fail\n";
    }

    // 10% chance of error
    if (std::bernoulli_distribution(0.1)(rng))
    {
        channel.setState(ChannelState::ERROR);
        return "fail\n";
    }

    channel.setState(ChannelState::IDLE);
    return "ok\n";
}

std::string GetResultCommand::execute()
{
    if (!validate())
        return "fail\n";

    auto &channel = _invoker.getChannelState(_channel);

    static thread_local std::mt19937 rng(std::random_device{}());
    // 10% chance of error
    if (std::bernoulli_distribution(0.1)(rng))
    {
        channel.setState(ChannelState::ERROR);
        return "fail\n";
    }

    if(channel.getState() == ChannelState::ERROR )
    {
        return "fail\n";
    }
    std::stringstream ss;
    ss << "ok, " << channel.makeRandomValue() << "\n";

    return ss.str();
}

std::string GetStatusCommand::execute()
{
    if (!validate())
        return "fail\n";

    auto &channel = _invoker.getChannelState(_channel);

    if(channel.getState() == ChannelState::ERROR )
    {
        return "fail\n";
    }

    std::stringstream ss;
    switch(channel.getState())
    {
        case ChannelState::IDLE:
            ss << "ok, idle_state\n";
            break;
        case ChannelState::MEASURE:
            ss << "ok, measure_state\n";
            break;
        case ChannelState::ERROR:
            ss << "ok, error_state\n";
            break;
        case ChannelState::BUSY:
            ss << "ok, busy_state\n";
            break;
        default:
            ss << "fail\n";
            break;
    }

    return ss.str();
}

int CommandFactory::parseChannel(const std::string &channelStr)
{
    if (channelStr.rfind("channel", 0) != 0)
        return -1;
    size_t pos = channelStr.size() - 1;
    int channel = -1;
    auto result = std::from_chars(
        channelStr.data() + pos,
        channelStr.data() + channelStr.size(),
        channel);

    if (result.ec != std::errc() || channel < 0)
        return -1;

    return channel;
}

int CommandFactory::parseRange(const std::string &rangeStr)
{
    if (rangeStr.rfind("range", 0) != 0)
        return -1;
    size_t pos = rangeStr.size() - 1;
    int range = -1;
    auto result = std::from_chars(
        rangeStr.data() + pos,
        rangeStr.data() + rangeStr.size(),
        range);
    if (result.ec != std::errc() || range < 0)
        return -1;

    return (range >= 0 && range < ChannelState::RANGE_MAX) ? range : -1;
}