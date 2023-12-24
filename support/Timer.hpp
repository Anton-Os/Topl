#ifndef TIMER_H

#include <chrono>
#include <ratio>
#include <cmath>
#include <vector>

#define INVALID_TIME -1.0

#define MICROSEC_IN_SEC 1000000.0
#define MILLISEC_IN_SEC 1000.0
#define MICROSEC_IN_MILLISEC 1000.0

#define MILILISEC_FPS30 1000.0 / 60.0 // fps in millisecs
#define MILILISEC_FPS60 1000.0 / 60.0 // fps in millisecs
#define MILILISEC_FPS120 1000.0 / 60.0 // fps in millisecs

typedef double millisec_t; // timer calculations in milliseconds

// Periodic Event

typedef void (*periodicCallback)(void);

class Timer_PeriodicEvent {
public:
	Timer_PeriodicEvent(millisec_t period, periodicCallback callback) : period(period) { callbackTrigger = callback; }
	void addTime(unsigned long microsecs);
private:
	const millisec_t period;
	periodicCallback callbackTrigger;
	millisec_t _millisecsElapsed = 0.0;
};

// Recurring Event

typedef void (*recurringCallback)(millisec_t);

class Timer_RecurringEvent {
public:
	Timer_RecurringEvent(recurringCallback callback){ callbackTrigger = callback; }
	void addTime(unsigned long microsecs);
private:
	recurringCallback callbackTrigger;
	millisec_t _millisecsElapsed = 0.0;
};

// Timer

class Timer_Ticker { // Get number of milliss between two invocations of getSecsPassed()
public:
	Timer_Ticker(){ reset(); }

	void reset();
	void addPeriodicEvent(unsigned period, periodicCallback callback){ 
		_periodicEvents.push_back(Timer_PeriodicEvent(period, callback));
	}
	void addRecurringEvent(recurringCallback callback){
		_recurringEvents.push_back(Timer_RecurringEvent(callback));
	}

	void updateTimer();
	double getRelMicrosecs();
	millisec_t getRelMillisecs() { return getRelMicrosecs() / MICROSEC_IN_MILLISEC; } // gets millisonds secs since last invocation
	double getRelSecs(){ return getRelMicrosecs() / MICROSEC_IN_SEC; } // gets seconds secs since last invocation
	double getAbsMicrosecs();
	millisec_t getAbsMillisecs() { return getAbsMicrosecs() / MICROSEC_IN_MILLISEC; } // gets millisonds since timer creation
	double getAbsSecs(){ return getAbsMicrosecs() / MICROSEC_IN_SEC; } // gets seconds since timer creation
protected:
	std::chrono::duration<double, std::micro> _relMicrosElapsed = std::chrono::microseconds(0); // relative time since last call
	std::chrono::duration<double, std::micro> _absMicrosElapsed = std::chrono::microseconds(0); // absolute time since creation
	std::chrono::steady_clock::time_point _startSecs; // helper variable for adjusting time
	std::chrono::steady_clock::time_point _endSecs; // helper variable for adjusting time

	std::vector<Timer_PeriodicEvent> _periodicEvents;
	std::vector<Timer_RecurringEvent> _recurringEvents;
};

struct Timer_Dynamic : public Timer_Ticker {
	Timer_Dynamic(millisec_t time) : Timer_Ticker(){ setTimer(time); }

	void setTimer(millisec_t time);
};

#define TIMER_H
#endif