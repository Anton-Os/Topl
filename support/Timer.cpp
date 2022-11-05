#include "Timer.hpp"

#include <ratio>

using namespace std::chrono;

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
        currentEvent->addTime(_relMicrosElapsed.count()); // update time elapsed of all periodic events

    _startSecs = _endSecs; // internally adjusting timing functions
}