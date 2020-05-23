
#include "Timer.hpp"

#include <iostream>

int main(void){
    double secsBetween = 0.0;
	unsigned trigCount;
    Timer_Interval timer;
    /* Timer_EventRatio tCounter_secs((unsigned long)(2)); // Sets it for 2 seconds
    Timer_EventRatio tCounter_frac(1000.0f); // 1000 Milliseconds = 1 Second
    Timer_EventRatio tCounter_rend((double) 1 / (double) 60); */


	timer.reset();
	while (secsBetween < 4000.0)
    {
        secsBetween = timer.getAbsMillsecs();
    }
    
    /* while (tCounter_frac.getEventCount() < 5.0f) {
		secsBetween = timer.getRelMillsecs();
		trigCount = tCounter_frac.isTrigger(secsBetween);
		if (trigCount >= 1) // Event has gone off at least once
			std::cout << "Good day sir" << std::endl;
			// std::cout << "Count: " << tCounter_frac.getEventCount() << std::endl;
    } */

	std::cout << "Cmon!" << std::endl;

    return 0;
}