#include "Timer.hpp"

#include <ratio>

using namespace std::chrono;

void Timer_Ticker::reset(){
    _relSecsElapsed = milliseconds(0);
    _absSecsElapsed = milliseconds(0);
    _startSecs = steady_clock::now();
}

double Timer_Ticker::getRelMillisecs() {
    updateTimer();
    return _relSecsElapsed.count();
}

double Timer_Ticker::getAbsMillisecs() {
    updateTimer();
    return _absSecsElapsed.count();
}

void Timer_Ticker::updateTimer(){
    _endSecs = steady_clock::now(); // Gets current time
    _relSecsElapsed = duration_cast<duration<double>>(_endSecs - _startSecs);
    _absSecsElapsed += _relSecsElapsed;

    for(std::vector<Timer_PeriodicEvent>::iterator currentEvent = _periodicEvents.begin(); currentEvent != _periodicEvents.end(); currentEvent++)
        currentEvent->addTime(_relSecsElapsed.count()); // update time elapsed of all periodic events
    for(std::vector<Timer_RecurringEvent>::iterator currentEvent = _recurringEvents.begin(); currentEvent != _recurringEvents.end(); currentEvent++)
        currentEvent->addTime(_relSecsElapsed.count()); // update time elapsed of all periodic events

    _startSecs = _endSecs; // internally adjusting timing functions
}