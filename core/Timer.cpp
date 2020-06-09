#include "Timer.hpp"

#include <ratio>

using namespace std::chrono;

void Timer_Ticker::reset(){
    mRelTimeSpan = milliseconds(0);
    mAbsTimeSpan = milliseconds(0);
    mStartSec = steady_clock::now();
}

double Timer_Ticker::getRelMillsecs(){
    updateTimer();
    return mRelTimeSpan.count();
}

double Timer_Ticker::getAbsMillsecs(){
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
	mCallCount = (unsigned)( floor(mTicker.getAbsMillsecs() / mTrigTime) );
	
	if (mCallCount > callCountTmp) mCallback();
}