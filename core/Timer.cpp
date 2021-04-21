#include "Timer.hpp"

#include <ratio>

using namespace std::chrono;

void Timer_Ticker::reset(){
    mRelTimeSpan = milliseconds(0);
    mAbsTimeSpan = milliseconds(0);
    mStartSec = steady_clock::now();
}

double Timer_Ticker::getRelMillisecs() {
    updateTimer();
    return mRelTimeSpan.count();
}

double Timer_Ticker::getAbsMillisecs() {
    updateTimer();
    return mAbsTimeSpan.count();
}

void Timer_Ticker::updateTimer(){
    mEndSec = steady_clock::now(); // Gets current time
    mRelTimeSpan = duration_cast<duration<double>>(mEndSec - mStartSec);
    mAbsTimeSpan += mRelTimeSpan;
    mStartSec = mEndSec;
}

void Timer_DiscreteEvent::update() {
	unsigned callCountTmp = mCallCount;
	mCallCount = (unsigned)( floor(mTicker.getAbsMillisecs() / mTrigTime) );
	
	if (mCallCount > callCountTmp) mCallback();
}