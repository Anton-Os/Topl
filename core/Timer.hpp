#ifndef TIMER_H

#include <chrono>
#include <ratio>
#include <cmath>
#include <vector>

#define TIME_MOTION_CALLBACK 30 // 30 milliseconds

typedef void (*periodicCallback)(void);
// typedef void (*periodicCallback)(double);

class Timer_PeriodicEvent {
public:
	Timer_PeriodicEvent(unsigned period, periodicCallback callback) : millisecPeriod(period){
		callbackTrigger = callback;
	}
	void addTime(unsigned millisecs){
		_secsElapsed += static_cast<double>(millisecs / 1000.0);
		while(_secsElapsed >= static_cast<double>(millisecPeriod / 1000.0)){
			_secsElapsed -= static_cast<double>(millisecPeriod / 1000.0);
			// callbackTrigger(_absSecsElapsed);
			callbackTrigger();
		}
	}
private:
	const unsigned millisecPeriod;
	periodicCallback callbackTrigger;
	double _secsElapsed = 0.0;
};

typedef void (*recurringCallback)(double);

class Timer_RecurringEvent {
public:
	Timer_RecurringEvent(recurringCallback callback){
		callbackTrigger = callback;
	}
	void addTime(unsigned millisecs){
		_secsElapsed += static_cast<double>(millisecs / 1000.0);
		callbackTrigger(_secsElapsed);
	}
private:
	recurringCallback callbackTrigger;
	double _secsElapsed = 0.0;
};

class Timer_Ticker { // Get number of millisecs between two invocations of getSecsPassed()
public:
	Timer_Ticker() { reset(); }

	void reset();
	void addPeriodicEvent(unsigned period, periodicCallback callback){ 
		_periodicEvents.push_back(Timer_PeriodicEvent(period, callback));
	}
	void addRecurringEvent(recurringCallback callback){
		_recurringEvents.push_back(Timer_RecurringEvent(callback));
	}

	void updateTimer();
	double getRelMillisecs(); // Gets milliseconds secs since last invocation
	double getRelSecs(){ return getRelMillisecs() / 1000.0; } // Gets seconds secs since last invocation
	double getAbsMillisecs(); // Gets milliseconds since timer creation
	double getAbsSecs(){ return getAbsMillisecs() / 1000.0; } // Gets seconds since timer creation
private:
	std::chrono::duration<double, std::milli> _relSecsElapsed = std::chrono::milliseconds(0); // relative time since last call
	std::chrono::duration<double, std::milli> _absSecsElapsed = std::chrono::milliseconds(0); // absolute time since creation
	std::chrono::steady_clock::time_point _startSecs; // helper variable for adjusting time
	std::chrono::steady_clock::time_point _endSecs; // helper variable for adjusting time

	std::vector<Timer_PeriodicEvent> _periodicEvents;
	std::vector<Timer_RecurringEvent> _recurringEvents;
};

#define TIMER_H
#endif