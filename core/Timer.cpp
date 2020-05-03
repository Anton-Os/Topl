#include "Timer.hpp"

#include <ratio>

unsigned Topl_Countup::isTrigger(double milSecsPass) { // Sees if over milliseconds some event triggered
    mTimeBuff += milSecsPass;

    if(mTimeBuff < mElapseTime) return 0; // Nothing triggered, not enough time passed

    unsigned triggerCount = 0;
    while(mTimeBuff >= mElapseTime){
        mTimeBuff -= mElapseTime;
        triggerCount++;
    }

    mEventCount += triggerCount; // Stores total number of occurances
    return triggerCount; // Number of times 
}

using namespace std::chrono;

/* Timestamp Topl_Timer::getTimeAbs(){
    auto time = steady_clock::now();
    //duration<long, std::deca> durSecs = time;
	seconds durSecs = time;
	duration<unsigned long, std::milli> durMilli = time;
    duration<unsigned long, std::micro> durMicro = time;

    Timestamp timestamp(durSecs, durMilli, durMicro);
    return timestamp;
} */

double Topl_Timer::getSecsPassed(){ // Retrieves seconds passed since last invocation
    mEndSec = steady_clock::now(); // Gets current time
    mSecTimeSpan = duration_cast<duration<double>>(mEndSec - mStartSec); // Computes difference
    mStartSec = mEndSec; // Sets the new start time
    return mSecTimeSpan.count();
}