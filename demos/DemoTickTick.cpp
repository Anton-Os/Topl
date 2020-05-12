
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
    Timer_Interval timer;
    Timer_Event tCounter_secs((unsigned long)(2)); // Sets it for 2 seconds
    Timer_Event tCounter_frac(1000.0f); // 1000 Milliseconds = 1 Second
    Timer_Event tCounter_rend((double) 1 / (double) 60);

    /* timer.getRelMillsecs();
    while(tCounter_secs.getEventCount() < 20){ // Twenty event triggers happens
        secsBetween = timer.getRelMillsecs();
        trigCount = tCounter_secs.isTrigger(secsBetween);
		if (trigCount >= 1)
			std::cout << tCounter_secs.getEventCount() << std::endl;
    } */

	timer.getRelMillsecs();
	while (tCounter_frac.getEventCount() < 5) { // Twenty event triggers happens
		secsBetween = timer.getRelMillsecs();
		trigCount = tCounter_frac.isTrigger(secsBetween);
		if (trigCount >= 1)
			std::cout << tCounter_frac.getEventCount() << std::endl;
	}

	/* timer.getRelMillsecs();
	while (tCounter_rend.getEventCount() < 1000) { // Twenty event triggers happens
		secsBetween = timer.getRelMillsecs();
		trigCount = tCounter_rend.isTrigger(secsBetween);
		if (trigCount >= 1)
			std::cout << tCounter_rend.getEventCount() << " ";
	} */


    return 0;
}