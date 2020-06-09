
#include "Timer.hpp"

#include <iostream>

void timerCallback1() {
	std::cout << "Cmon!" << std::endl;
}

int main(void){
    double secsBetween = 0.0;
	unsigned trigCount;
    Timer_Ticker timer;


	timer.reset();
	while (secsBetween < 4000.0)
    {
        secsBetween = timer.getAbsMillsecs();
    }

	Timer_DiscreteEvent event1(1000.0, timerCallback1);

	while (event1.getCallCount() < 10) event1.update();

    return 0;
}