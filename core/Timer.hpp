#include <chrono>
#include <cmath>

class Topl_Countup { // Returns whether enough time has passed to trigger an event (i.e draw 60 fps)
public:
    Topl_Countup(unsigned long secs){ mElapseTime = secs * 1000; }
    Topl_Countup(double milSecs){ mElapseTime = milSecs; }
    unsigned isTrigger(double milSecsPass); // How many times triggered since last call
private:
    unsigned long long mEventCount = 0; // Number of times trigger went off
    double mTimeBuff = 0.0f; // Time incrementor
    double mElapseTime = 0.0; // Time needed for event to go go off
};


class Topl_Timer { // Get number of millisecs between two invocations of getSecsPassed()
public:
    Topl_Timer(){ 
        mStartSec = std::chrono::high_resolution_clock::now();
    }
    double getSecsPassed();
private:
    std::chrono::duration<double> mSecTimeSpan;
    std::chrono::high_resolution_clock::time_point mStartSec;
    std::chrono::high_resolution_clock::time_point mEndSec;
};