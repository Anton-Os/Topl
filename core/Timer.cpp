#include "Timer.hpp"

#include <ratio>

unsigned Timer_Event::isTrigger(double milSecsPass) { // Sees if over milliseconds some event triggered
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

/* Timestamp Timer_Interval::getTimeAbs(){
    Timestamp tStamp;
    steady_clock::time_point timePt = steady_clock::now();
    
    // std::chrono::duration<double> secs = duration_cast<duration<double>>(time_point_cast<seconds>(timePt));
    //steady_clock::time_point secs = time_point_cast<seconds>(timePt);

    //duration<double, std::deca> secs = timePt;
    //seconds secs;
    return tStamp;
} */

/* Timestamp Timer_Interval::getTimeAbs(){
    auto time = steady_clock::now();
    //duration<long, std::deca> durSecs = time;
	seconds durSecs = time;
	duration<unsigned long, std::milli> durMilli = time;
    duration<unsigned long, std::micro> durMicro = time;

    Timestamp timestamp(durSecs, durMilli, durMicro);
    return timestamp;
} */

/* double Timer_Interval::getRelMillsecs(){ // Retrieves seconds passed since last invocation
    mEndSec = steady_clock::now(); // Gets current time
    mRelTimeSpan = duration_cast<duration<double>>(mEndSec - mStartSec); // Computes difference
    mAbsTimeSpan += mRelTimeSpan;
    mStartSec = mEndSec; // Sets the new start time
    return mRelTimeSpan.count();
} */

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