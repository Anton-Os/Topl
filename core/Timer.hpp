#ifndef TIMER_H

#include <chrono>
#include <ratio>
#include <cmath>
#include <vector>

typedef void (*periodicCallback)(void);
// typedef void (*periodicCallback)(double);

struct Timer_PeriodicEvent {
	Timer_PeriodicEvent(unsigned period, periodicCallback callback) : millisecPeriod(period){
		callbackTrigger = callback;
	}
	void addTime(unsigned millisecs){
		millisecElapsed += millisecs;
		while(millisecElapsed >= millisecPeriod){
			millisecElapsed -= millisecPeriod;
			double secs = static_cast<double>(millisecs) / 1000.0;
			callbackTrigger();
			// callbackTrigger(secs); // callback function triggers once period is exceeded
		}
	}
	unsigned millisecElapsed = 0;
	const unsigned millisecPeriod;	
	periodicCallback callbackTrigger;
};

class Timer_Ticker { // Get number of millisecs between two invocations of getSecsPassed()
public:
	Timer_Ticker() { reset(); }

	void reset();
	void addPeriodicEvent(unsigned period, periodicCallback callback){ 
		_periodicEvents.push_back(Timer_PeriodicEvent(period, callback));
	}

	void updateTimer();
	double getRelMillisecs(); // Gets milliseconds secs since last invocation
	double getRelSecs(){ return getRelMillisecs() / 1000.0; } // Gets seconds secs since last invocation
	double getAbsMillisecs(); // Gets milliseconds since timer creation
	double getAbsSecs(){ return getAbsMillisecs() / 1000.0; } // Gets seconds since timer creation
private:
	std::chrono::duration<double, std::milli> _relTimeSpan = std::chrono::milliseconds(0);
	std::chrono::duration<double, std::milli> _absTimeSpan = std::chrono::milliseconds(0);
	std::chrono::steady_clock::time_point _startSecs; // helper variable for adjusting time
	std::chrono::steady_clock::time_point _endSecs; // helper variable for adjusting time

	std::vector<Timer_PeriodicEvent> _periodicEvents;
};

#define TIMER_H
#endif