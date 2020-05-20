#include <chrono>
#include <ratio>
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

    double getEventCount(){ return mEventCount; }
    unsigned isTrigger(double milSecsPass); // How many times triggered since last call
private:
    double mEventCount = 0.0f; // Number of times trigger went off
    double mTimeBuff = 0.0f; // Time incrementor
    double mElapseTime = 0.0f; // Time needed for event to go go off
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