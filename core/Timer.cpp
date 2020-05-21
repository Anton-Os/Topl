#include "Timer.hpp"

#include <ratio>

using namespace std::chrono;

void Timer_Interval::reset(){
    mRelTimeSpan = milliseconds(0);
    mAbsTimeSpan = milliseconds(0);
    mStartSec = steady_clock::now();
}

double Timer_Interval::getRelMillsecs(){
    updateTimer();
    return mRelTimeSpan.count();
}

double Timer_Interval::getAbsMillsecs(){
    updateTimer();
    return mAbsTimeSpan.count();
}

void Timer_Interval::updateTimer(){
    mEndSec = steady_clock::now(); // Gets current time
    mRelTimeSpan = duration_cast<duration<double>>(mEndSec - mStartSec);
    mAbsTimeSpan += mRelTimeSpan;
    mStartSec = mEndSec;
}