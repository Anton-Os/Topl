
#include "Timer.hpp"

#include <iostream>

/*int main(void){
    unsigned timerInc = 0;
    clock_t startTime = 0, midTime = 0, saveTime = 0;
	clock_t wClock = 0;

    double perSec = 0.0f;

    startTime = clock();
    while(timerInc < 20){
        // midTime = clock() - startTime - midTime;
		wClock = clock();
		midTime = wClock - startTime - saveTime;
		saveTime = midTime; // So that I can decrement

        perSec += (double)midTime / CLOCKS_PER_SEC;

        if(perSec >= 1.0f){ // Over one second has passed
            timerInc++;
            printf("Seconds: %f", timerInc);
            perSec -= 1.0f;
        }
    }

    return 0;
} */

int main(void){
    double secsBetween;
	unsigned trigCount;
    Topl_Timer timer;
    Topl_Countup tCounter_secs((unsigned long)(2)); // Sets it for 2 seconds
    Topl_Countup tCounter_frac(0.1);
    Topl_Countup tCounter_rend((double) 1 / (double) 60);

    /* timer.getSecsPassed();
    while(tCounter_secs.getEventCount() < 20){ // Twenty event triggers happens
        secsBetween = timer.getSecsPassed();
        trigCount = tCounter_secs.isTrigger(secsBetween);
		if (trigCount >= 1)
			std::cout << tCounter_secs.getEventCount() << std::endl;
    } */

	timer.getSecsPassed();
	while (tCounter_frac.getEventCount() < 5) { // Twenty event triggers happens
		secsBetween = timer.getSecsPassed();
		trigCount = tCounter_frac.isTrigger(secsBetween);
		if (trigCount >= 1)
			std::cout << tCounter_frac.getEventCount() << std::endl;
	}

	timer.getSecsPassed();
	while (tCounter_rend.getEventCount() < 1000) { // Twenty event triggers happens
		secsBetween = timer.getSecsPassed();
		trigCount = tCounter_rend.isTrigger(secsBetween);
		if (trigCount >= 1)
			std::cout << tCounter_rend.getEventCount() << " ";
	}


    return 0;
}