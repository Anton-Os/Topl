#include "Timer.hpp"

#include <ratio>

using namespace std::chrono;

// Periodic Event

void Timer_PeriodicEvent::addTime(unsigned long microsecs){
    _millisecsElapsed += static_cast<double>(microsecs / MICROSEC_IN_MILLISEC);
    while(_millisecsElapsed >= period){
        _millisecsElapsed -= period;
        callbackTrigger();
    }
}

// Recurring Event

void Timer_RecurringEvent::addTime(unsigned long microsecs){
    _millisecsElapsed += static_cast<double>(microsecs / MICROSEC_IN_MILLISEC); // conversion from microsecsecs to seconds
    callbackTrigger(_millisecsElapsed);
}

// Timer

void Timer_Ticker::reset(){
    _relMicrosElapsed = microseconds(0);
    _absMicrosElapsed = microseconds(0);
    _startSecs = steady_clock::now();
}

double Timer_Ticker::getRelMicrosecs() {
    updateTimer();
    return _relMicrosElapsed.count();
}

double Timer_Ticker::getAbsMicrosecs() {
    updateTimer();
    return _absMicrosElapsed.count();
}

void Timer_Ticker::updateTimer(){
    _endSecs = steady_clock::now(); // Gets current time
    _relMicrosElapsed = duration_cast<duration<double>>(_endSecs - _startSecs);
    _absMicrosElapsed += _relMicrosElapsed;

    for(std::vector<Timer_PeriodicEvent>::iterator currentEvent = _periodicEvents.begin(); currentEvent != _periodicEvents.end(); currentEvent++)
        currentEvent->addTime(_relMicrosElapsed.count()); // update time elapsed of all periodic events
    for(std::vector<Timer_RecurringEvent>::iterator currentEvent = _recurringEvents.begin(); currentEvent != _recurringEvents.end(); currentEvent++)
        currentEvent->addTime(_relMicrosElapsed.count()); // update time elapsed of all recurring events

    _startSecs = _endSecs; // internally adjusting timing functions
}

void Timer_Dynamic::setTimer(millisec_t millisecs){
    _relMicrosElapsed = microseconds(0);
    _absMicrosElapsed = std::chrono::duration<double>((double)millisecs * (double)MICROSEC_IN_MILLISEC);
}