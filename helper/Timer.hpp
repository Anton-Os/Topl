#ifndef TIMER_H

#include <chrono>
#include <ratio>
#include <cmath>
#include <vector>

#define FPS_60 1000.0 / 60.0 // fps in millisecs
#define BAD_EVENT_TIME -1.0f // indicates that event timestamp is invalid
#define CONCURRENT_THRESH 1000.0 / 240.0 // used for concurrent events
#define MICROSEC_SEC_CONVERT 1000000.0
#define MICROSEC_MILLI_CONVERT 1000.0

typedef void (*periodicCallback)(void);

class Timer_PeriodicEvent {
public:
	Timer_PeriodicEvent(double period, periodicCallback callback) : periodMillisec(period){
		callbackTrigger = callback;
	}
	void addTime(double micros){
		_secsElapsed += static_cast<double>(micros / MICROSEC_SEC_CONVERT);
		while(_secsElapsed >= static_cast<double>(periodMillisec / MICROSEC_MILLI_CONVERT)){
			_secsElapsed -= static_cast<double>(periodMillisec / MICROSEC_MILLI_CONVERT);
			callbackTrigger();
		}
	}
private:
	const double periodMillisec;
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
		_secsElapsed += static_cast<double>(micros / MICROSEC_SEC_CONVERT); // conversion from microsecs to seconds
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
	double getRelMillisecs() { return getRelMicrosecs() / MICROSEC_MILLI_CONVERT; } // gets millisonds secs since last invocation
	double getRelSecs(){ return getRelMicrosecs() / MICROSEC_SEC_CONVERT; } // gets seconds secs since last invocation
	double getAbsMicrosecs();
	double getAbsMillisecs() { return getAbsMicrosecs() / MICROSEC_MILLI_CONVERT; } // gets millisonds since timer creation
	double getAbsSecs(){ return getAbsMicrosecs() / MICROSEC_SEC_CONVERT; } // gets seconds since timer creation
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