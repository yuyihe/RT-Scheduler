 
#include "utils.h"
#include "3140_concur.h"
#include "realtime.h"

/*--------------------------*/
/* Parameters for test case */
/*--------------------------*/


 
/* Stack space for processes */
#define NRT_STACK 80
#define RT_STACK  80

//We want to test the code's functionality when there are no real time processes created.
//We are doing this to confirm that the addition of support for real time processes will not affect the original functionality of the code.
//Expected result: Red and Blue LED toggles alternatively.

 
/*------------------*/
/* Helper functions */
/*------------------*/
void shortDelay(){delay();}
void mediumDelay() {delay(); delay();}



/*----------------------------------------------------
 * Non real-time process
 *   Blinks red LED 10 times. 
 *   Should be blocked by real-time process at first.
 *----------------------------------------------------*/
 
void pNRT1(void) {
	int i;
	for (i=0; i<4;i++){
	LEDRed_On();
	shortDelay();
	LEDRed_Toggle();
	shortDelay();
	}
	
}

void pNRT2(void) {
	int i;
	for (i=0; i<4;i++){
	LEDBlue_On();
	shortDelay();
	LEDBlue_Toggle();
	shortDelay();
	}
	
}


/*--------------------------------------------*/
/* Main function - start concurrent execution */
/*--------------------------------------------*/
int main(void) {	
	 
	LED_Initialize();

    /* Create processes */ 
    if (process_create(pNRT1, NRT_STACK) < 0) { return -1; }
		if (process_create(pNRT2, NRT_STACK) < 0) { return -1; }
   
    /* Launch concurrent execution */
	process_start();

  LED_Off();
	
	/* Hang out in infinite loop (so we can inspect variables if we want) */ 
	while (1);
	return 0;
}
