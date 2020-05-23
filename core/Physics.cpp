#include "Physics.hpp"

void Physics_MoveAbs::updatePos(unsigned reps){
    unsigned r = 0;
    while(r != reps){
        mPos = mAbsMove(mPos); // Call callback r times!
        r++;
        // r = (r < reps) r++ : r--; // If reps is negative we traverse other way
    }
}

void Physics_MoveTimed::updatePos(const Timer_EventRatio& timeEvent){
    mPos = mTimedMove(mPos, timeEvent);
}