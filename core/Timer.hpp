#ifndef TIMER_H

#include <chrono>
#include <ratio>
#include <cmath>
#include <vector>

#define TIME_MOTION_CALLBACK 30 // 30 millisonds

typedef void (*periodicCallback)(void);

class Timer_PeriodicEvent {
public:
	Timer_PeriodicEvent(unsigned period, periodicCallback callback) : millisPeriod(period){
		callbackTrigger = callback;
	}
	void addTime(unsigned micros){
		_secsElapsed += static_cast<double>(micros / 1000000.0); // conversion from microsecs to seconds
		while(_secsElapsed >= static_cast<double>(millisPeriod / 1000.0)){
			_secsElapsed -= static_cast<double>(millisPeriod / 1000.0);
			callbackTrigger();
		}
	}
private:
	const unsigned millisPeriod;
	periodicCallback callbackTrigger;
	double _secsElapsed = 0.0;
};

typedef void (*recurringCallback)(double);

class Timer_RecurringEvent {
public:
	Timer_RecurringEvent(recurringCallback callback){
		callbackTrigger = callback;
	}
	void addTime(unsigned micros){
		_secsElapsed += static_cast<double>(micros / 1000000.0); // conversion from microsecs to seconds
		callbackTrigger(_secsElapsed);
	}
private:
	recurringCallback callbackTrigger;
	double _secsElapsed = 0.0;
};

class Timer_Ticker { // Get number of milliss between two invocations of getSecsPassed()
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
	double getRelMicrosecs();
	double getRelMillisecs() { return getRelMicrosecs() / 1000.0; } // gets millisonds secs since last invocation
	double getRelSecs(){ return getRelMillisecs() / 1000.0; } // gets seconds secs since last invocation
	double getAbsMicrosecs();
	double getAbsMillisecs() { return getAbsMicrosecs() / 1000.0; } // gets millisonds since timer creation
	double getAbsSecs(){ return getAbsMillisecs() / 1000.0; } // gets seconds since timer creation
private:
	std::chrono::duration<double, std::micro> _relMicrosElapsed = std::chrono::microseconds(0); // relative time since last call
	std::chrono::duration<double, std::micro> _absMicrosElapsed = std::chrono::microseconds(0); // absolute time since creation
	std::chrono::steady_clock::time_point _startSecs; // helper variable for adjusting time
	std::chrono::steady_clock::time_point _endSecs; // helper variable for adjusting time

	std::vector<Timer_PeriodicEvent> _periodicEvents;
	std::vector<Timer_RecurringEvent> _recurringEvents;
};

#define TIMER_H
#endif