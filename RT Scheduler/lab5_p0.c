/**************************************************************
 * Test Case 1: To check the working of periodic real-time process scheduling.
 *
 * In this test case, one periodic process is created with a period of 6 seconds.
 * Process: The process created blinks the BLUE LED 3 times every time it executes.
 *
 * Expected Output: GREEN LED should never turn on, if it turns on then the it means test failed.
 * Also, the BLUE LED should keep blinking(3 times every time the process executes)
 * this should happen with period of about 6 seconds.
 * If after some time BLUE LED stops blinking, it indicates test failed.
 *
 *
***************************************************************/

#include "utils.h"
#include "3140_concur.h"
#include "realtime.h"

/*--------------------------*/
/* Parameters for test case */
/*--------------------------*/

/* Stack space for processes */
#define NRT_STACK 80
#define RT_STACK  80



/*--------------------------------------*/
/* Time structs for real-time processes */
/*--------------------------------------*/

/* Constants used for 'work' and 'deadline's */
realtime_t t_1msec = {0, 1}; // deadline of the first periodic process
realtime_t t_pRT1 = {1, 0}; // initial start time of the first Periodic Process
realtime_t t_period1 = {6, 0}; // Time period of the first periodic process

/*------------------*/
/* Helper functions */
/*------------------*/
void shortDelay(){delay();}
void mediumDelay() {delay(); delay();}


/*-------------------
 * 1st Periodic Real-time process
 * Blinks Blue LED
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

/* Main function */
int main(void) {

	LED_Initialize();

	/* Create process (real_time and periodic) */
	if(process_rt_periodic(pRT1, RT_STACK, &t_pRT1, &t_1msec, &t_period1) < 0){ 
		return -1; 
	}
	
	/* Launch concurrent execution */
	process_start();
	
	LED_Off();
	mediumDelay();
	LEDGreen_On();
  
	/* Hang out in infinite loop (so we can inspect variables if we want) */
	while (1);
	return 0;
}
