#include "Timer.hpp"

#include <ratio>

using namespace std::chrono;

// Periodic Event

void PeriodicEvent::addTime(unsigned long microsecs){
    _millisecsElapsed += static_cast<double>(microsecs / MICROSEC_IN_MILLISEC);
    while(_millisecsElapsed >= period){
        _millisecsElapsed -= period;
        callbackTrigger();
    }
}

// Recurring Event

void RecurringEvent::addTime(unsigned long microsecs){
    _millisecsElapsed += static_cast<double>(microsecs / MICROSEC_IN_MILLISEC); // conversion from microsecsecs to seconds
    callbackTrigger(_millisecsElapsed);
}

// Timer

void Timer_Persist::reset(){
    _relMicrosElapsed = microseconds(0);
    _absMicrosElapsed = microseconds(0);
    _startSecs = steady_clock::now();
}

double Timer_Persist::getRelMicrosecs() {
    updateTimer();
    return _relMicrosElapsed.count();
}

double Timer_Persist::getAbsMicrosecs() {
    updateTimer();
    return _absMicrosElapsed.count();
}

void Timer_Persist::updateTimer(){
    _endSecs = steady_clock::now(); // gets current time
    _relMicrosElapsed = duration_cast<duration<double>>(_endSecs - _startSecs);
    _absMicrosElapsed += _relMicrosElapsed;

    for(std::vector<PeriodicEvent>::iterator currentEvent = _periodicEvents.begin(); currentEvent != _periodicEvents.end(); currentEvent++)
        currentEvent->addTime(_relMicrosElapsed.count()); // update time elapsed of all periodic events
    for(std::vector<RecurringEvent>::iterator currentEvent = _recurringEvents.begin(); currentEvent != _recurringEvents.end(); currentEvent++)
        currentEvent->addTime(_relMicrosElapsed.count()); // update time elapsed of all recurring events

    _startSecs = _endSecs; // internally adjusting timing functions
}

void Timer_Dynamic::updateTimer(){ 
    if(!isPaused) Timer_Persist::updateTimer();
    else{
        _endSecs = steady_clock::now(); // gets current time
        _startSecs = _endSecs;
    }
}

void Timer_Dynamic::setTime(millisec_t millisecs){
    _relMicrosElapsed = microseconds(0);
    _absMicrosElapsed = std::chrono::duration<double>((double)millisecs);
}

void Timer_Dynamic::updateTime(millisec_t millisecs){
    _relMicrosElapsed = microseconds(0);
    _absMicrosElapsed += std::chrono::duration<double>((double)millisecs);
    if(_absMicrosElapsed < microseconds(0)) _absMicrosElapsed = microseconds(0); // clamp to lower bound
}