/*************************************************************************
 * Lab 5 "Easy test" used for grading
 * 
 * The test cases consists of 2 processes. One a normal process which blinks
 * RED LED 4 times and a real time periodic process which blinks the blue LED
 * 3 times.
 *	 
 *	 So when program runs, both processes should run and then after a 10 seconds
 *	 gap, blue LED should blink again and this continues every 10 seconds.
 *	 
 *	 If the green LED truns on, then that means test case has failed.
 *	 If nothing is happening, then most probably its in the hard fault handler
 * 
 ************************************************************************/
 
#include "utils.h"
#include "3140_concur.h"
#include "realtime.h"

/*--------------------------*/
/* Parameters for test case */
/*--------------------------*/


 
/* Stack space for processes */
#define NRT_STACK 40
#define RT_STACK  40
 


/*--------------------------------------*/
/* Time structs for real-time processes */
/*--------------------------------------*/

/* Constants used for 'work' and 'deadline's */
realtime_t t_1msec = {2, 1};
realtime_t t_6sec = {6, 0};

/* Process start time */
realtime_t t_pRT1 = {10, 0};

 
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
 
void pNRT(void) {
	int i;
	for (i=0; i<4;i++){
	LEDRed_On();
	mediumDelay();
	LEDRed_Toggle();
	mediumDelay();
	}
	
}

/*-------------------
 * Real-time process
 *-------------------*/

void pRT1(void) {
	int i;
	for (i=0; i<3;i++){
	LEDBlue_On();
	mediumDelay();
	LEDBlue_Toggle();
	mediumDelay();
	}
}


/*--------------------------------------------*/
/* Main function - start concurrent execution */
/*--------------------------------------------*/
int main(void) {	
	 
	LED_Initialize();

    /* Create processes */ 
		if (process_create(pNRT, NRT_STACK) < 0) { return -1; }
    if (process_rt_periodic(pRT1, RT_STACK, &t_pRT1, &t_1msec, &t_6sec) < 0) { return -1; } 
   
    /* Launch concurrent execution */
	process_start();
		
	LEDGreen_On();
	
	
	/* Hang out in infinite loop (so we can inspect variables if we want) */ 
	while (1);
	return 0;
}
