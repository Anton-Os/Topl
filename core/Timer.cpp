#include "Timer.hpp"

#include <ratio>

using namespace std::chrono;

void Timer_Ticker::reset(){
    _relTimeSpan = milliseconds(0);
    _absTimeSpan = milliseconds(0);
    _startSecs = steady_clock::now();
}

double Timer_Ticker::getRelMillisecs() {
    updateTimer();
    return _relTimeSpan.count();
}

double Timer_Ticker::getAbsMillisecs() {
    updateTimer();
    return _absTimeSpan.count();
}

void Timer_Ticker::updateTimer(){
    _endSecs = steady_clock::now(); // Gets current time
    _relTimeSpan = duration_cast<duration<double>>(_endSecs - _startSecs);
    _absTimeSpan += _relTimeSpan;

    for(std::vector<Timer_PeriodicEvent>::iterator currentEvent = _periodicEvents.begin(); currentEvent != _periodicEvents.end(); currentEvent++)
        currentEvent->addTime(_relTimeSpan.count()); // update time elapsed of all periodic events

    _startSecs = _endSecs; // internally adjusting timing functions
}