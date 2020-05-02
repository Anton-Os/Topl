
#include "Timer.hpp"

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
    Topl_Timer timer;
    Topl_Countup tCounter((unsigned long)(2)); // Sets it for 2 seconds

    return 0;
}