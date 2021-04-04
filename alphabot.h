#ifndef ALPHABOT_H
#define ALPHABOT_H

#include <thread>
#include "CppTimer.h"

#define DEFAULT_SAMPLING_INTERVAL_NS (10 * 1000 * 1000) // 10ms

class AlphaBot : public CppTimer
{
public:
        void start(long _samplingInterval = DEFAULT_SAMPLING_INTERVAL_NS);
        void stop();

private:
        long samplingInterval;

        virtual void timerEvent();
};

#endif
