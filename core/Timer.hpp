#ifndef TIMER_H

#include <chrono>
#include <ratio>
#include <cmath>

class Timer_Ticker { // Get number of millisecs between two invocations of getSecsPassed()
public:
	Timer_Ticker() { reset(); }

	void reset();
	double getRelMillisecs(); // Gets milliseconds secs since last invocation
	double getRelSecs(){ return getRelMillisecs() / 1000.0; } // Gets seconds secs since last invocation
	double getAbsMillisecs(); // Gets milliseconds since timer creation
	double getAbsSecs(){ return getAbsMillisecs() / 1000.0; } // Gets seconds since timer creation
private:
	void updateTimer();

	std::chrono::duration<double, std::milli> mRelTimeSpan = std::chrono::milliseconds(0);
	std::chrono::duration<double, std::milli> mAbsTimeSpan = std::chrono::milliseconds(0);
	std::chrono::steady_clock::time_point mStartSec;
	std::chrono::steady_clock::time_point mEndSec;
};

typedef void (*timerCallback)(void);

class Timer_DiscreteEvent {
public:
	Timer_DiscreteEvent(double triggerTime, timerCallback callback) {
		mTrigTime = triggerTime;
		mCallback = callback;
	}
	void update();

	unsigned getCallCount(void){ return mCallCount; }
private:
	double mTrigTime;
	timerCallback mCallback;
	unsigned mCallCount = 0;
	Timer_Ticker mTicker;
};

#define TIMER_H
#endif