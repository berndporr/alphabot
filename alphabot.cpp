#include "alphabot.h"

void AlphaBot::start(long _samplingInterval)
{
        samplingInterval = _samplingInterval;
        CppTimer::start(samplingInterval);
}

void AlphaBot::stop()
{
        CppTimer::stop();
}

void AlphaBot::timerEvent() {

}