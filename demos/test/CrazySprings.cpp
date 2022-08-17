#include "Timer.hpp"
#include "Physics.h"

int main(void){
    Timer_Ticker ticker = Timer_Ticker();

    Phys_Connector connector1;
    Phys_Connector connector2 = Phys_Connector(1.0, 10.0);

    while(ticker.getAbsMillisecs() < 20000){
        // TODO: Calculate Physics Here!

        ticker.updateTimer();
    }
}