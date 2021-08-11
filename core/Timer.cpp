#include "Timer.hpp"

#include <ratio>

using namespace std::chrono;

void Timer_Ticker::reset(){
    _relSecSpan = milliseconds(0);
    _absSecSpan = milliseconds(0);
    _startSecs = steady_clock::now();
}

double Timer_Ticker::getRelMillisecs() {
    updateTimer();
    return _relSecSpan.count();
}

double Timer_Ticker::getAbsMillisecs() {
    updateTimer();
    return _absSecSpan.count();
}

void Timer_Ticker::updateTimer(){
    _endSecs = steady_clock::now(); // Gets current time
    _relSecSpan = duration_cast<duration<double>>(_endSecs - _startSecs);
    _absSecSpan += _relSecSpan;

    for(std::vector<Timer_PeriodicEvent>::iterator currentEvent = _periodicEvents.begin(); currentEvent != _periodicEvents.end(); currentEvent++)
        currentEvent->addTime(_relSecSpan.count()); // update time elapsed of all periodic events

    _startSecs = _endSecs; // internally adjusting timing functions
}