#ifndef TIMER_H

#include <chrono>
#include <ratio>
#include <cmath>
#include <vector>

#define INVALID_TIME -1.0

#define MICROSEC_IN_SEC 1000000.0
#define MILLISEC_IN_SEC 1000.0
#define MICROSEC_IN_MILLISEC 1000.0

#define MILILISEC_FirstPerson30 1000.0 / 60.0 // fps in millisecs
#define MILILISEC_FirstPerson60 1000.0 / 60.0 // fps in millisecs
#define MILILISEC_FirstPerson120 1000.0 / 60.0 // fps in millisecs

typedef double millisec_t; // timer calculations in milliseconds

// Periodic Event

typedef void (*periodicCallback)(void);

class PeriodicEvent {
public:
	PeriodicEvent(millisec_t period, periodicCallback callback) : period(period) { callbackTrigger = callback; }
	void addTime(unsigned long microsecs);
private:
	const millisec_t period;
	periodicCallback callbackTrigger;
	millisec_t _millisecsElapsed = 0.0;
};

// Recurring Event

typedef void (*recurringCallback)(millisec_t); // passes absolute seconds in callback

class RecurringEvent {
public:
	RecurringEvent(recurringCallback callback){ callbackTrigger = callback; }
	void addTime(unsigned long microsecs);
private:
	recurringCallback callbackTrigger;
	millisec_t _millisecsElapsed = 0.0;
};

// Timer

class Timer_Persist {
public:
	Timer_Persist(){ reset(); }

	void reset();
	void addPeriodicEvent(unsigned period, periodicCallback callback){ 
		_periodicEvents.push_back(PeriodicEvent(period, callback));
	}
	void addRecurringEvent(recurringCallback callback){
		_recurringEvents.push_back(RecurringEvent(callback));
	}

	virtual void updateTimer();
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

	std::vector<PeriodicEvent> _periodicEvents;
	std::vector<RecurringEvent> _recurringEvents;
};

struct Timer_Dynamic : public Timer_Persist {
	Timer_Dynamic(millisec_t time) : Timer_Persist(){ setTime(time); }

	virtual void updateTimer() override;

	double getRelMillisecs(){
		if(Timer_Persist::getAbsMicrosecs() > range.second * MICROSEC_IN_SEC || Timer_Persist::getAbsMicrosecs() < range.first * MICROSEC_IN_SEC)
			return 0.0; // if out of bounds return 0 by default
		else return Timer_Persist::getRelMicrosecs();
	}

	double getAbsMicrosecs(){
		if(Timer_Persist::getAbsMicrosecs() > range.second * MICROSEC_IN_SEC) return range.second * MICROSEC_IN_SEC;
		else if(Timer_Persist::getAbsMicrosecs() < range.first * MICROSEC_IN_SEC) return range.first * MICROSEC_IN_SEC;
		else return Timer_Persist::getAbsMicrosecs();
	}

	void setTime(millisec_t time); // set absolute
	void updateTime(millisec_t time); // increment or decrement time
	std::pair<double, double> range = std::make_pair(0.0, 60.0);
	bool isPaused = false; // start or stop to handle events
};

#define TIMER_H
#endif