#include "server.hpp"

class ChannelState
{
public:
    enum State
    {
        IDLE,
        MEASURE,
        BUSY,
        ERROR
    };

    ChannelState() : _state(IDLE),
                     _current_range(0),
                     _current_value(0.0f) {}

    const static int RANGE_MAX = 4;
    
    void setState(State newState)
    {
        _state = newState;
    }

    State getState() const
    {
        return _state;
    }

    void setRange(int range)
    {
        _current_range = range;
    }
    int getRange()
    {
        return _current_range;
    }

    float getValue() const
    {
        return _current_value;
    }
    float makeRandomValue()
    {
        static thread_local std::mt19937 gen(std::random_device{}());
        static constexpr std::array ranges = {
            std::pair{0.0000001f, 0.001f},
            std::pair{0.001f, 1.0f},
            std::pair{1.0f, 1000.0f},
            std::pair{1000.0f, 1000000.0f}};
    
        std::uniform_real_distribution<float> dist(
            ranges[_current_range].first,
            ranges[_current_range].second);
        return _current_value = dist(gen);
    }
private:
    State _state;
    int _current_range;
    float _current_value;
};
