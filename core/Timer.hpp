#ifndef TIMER_H

#include <chrono>
#include <ratio>
#include <cmath>

struct Timer_EventRatio {
	double currentTime;
	double triggerTime;
};

class Timer_Interval { // Get number of millisecs between two invocations of getSecsPassed()
public:
    Timer_Interval(){ 
        mStartSec = std::chrono::steady_clock::now();
    }
    // Timestamp getTimeAbs();
    void reset();
    double getRelMillsecs(); // Gets secs since last call
    double getAbsMillsecs(); // Gets secs since timer object construction
private:
    void updateTimer();

    std::chrono::duration<double, std::milli> mRelTimeSpan = std::chrono::milliseconds(0);
    std::chrono::duration<double, std::milli> mAbsTimeSpan = std::chrono::milliseconds(0);
    std::chrono::steady_clock::time_point mStartSec;
    std::chrono::steady_clock::time_point mEndSec;
};

#define TIMER_H
#endif