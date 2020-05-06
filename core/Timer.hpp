#include <chrono>
#include <cmath>

struct Timestamp {
    Timestamp(){}
    Timestamp(unsigned long s, unsigned long mil, unsigned long mic){
        secs = s; millisecs = mil; microsecs = mic;
    }

    unsigned long secs;
    unsigned long millisecs;
    unsigned long microsecs;
};

class Timer_Event { // Returns whether enough time has passed to trigger an event (i.e draw 60 fps)
public:
    Timer_Event(unsigned long secs){ mElapseTime = secs * 1000; }
    Timer_Event(double milSecs){ mElapseTime = milSecs; }

    unsigned long long getEventCount(){ return mEventCount; }
    unsigned isTrigger(double milSecsPass); // How many times triggered since last call
private:
    unsigned long long mEventCount = 0; // Number of times trigger went off
    double mTimeBuff = 0.0f; // Time incrementor
    double mElapseTime = 0.0; // Time needed for event to go go off
};


class Timer_Interval { // Get number of millisecs between two invocations of getSecsPassed()
public:
    Timer_Interval(){ 
        mStartSec = std::chrono::steady_clock::now();
    }
    // Timestamp getTimeAbs();
    double getSecsPassed();
private:
    std::chrono::duration<double> mSecTimeSpan;
    std::chrono::steady_clock::time_point mStartSec;
    std::chrono::steady_clock::time_point mEndSec;
};