#ifndef TIMER_H

#include <chrono>
#include <ratio>
#include <cmath>

class Timer_Ticker { // Get number of millisecs between two invocations of getSecsPassed()
public:
	Timer_Ticker() { reset(); }

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