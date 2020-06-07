// Lab 5 Test 2 - Written by Kofi A. Efah & Yuyi He
// This test investiagtes if the EDF algorithm is working as expected.
// Expected Result: Blue Blue Red(x7) Blue Green(x2)
 
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
realtime_t t_20sec = {9, 0};
realtime_t t_10sec = {4, 0};

/* Process start time */
realtime_t t_pRT1 = {0, 1};
realtime_t t_pRT2 = {2, 0};

 
/*------------------*/
/* Helper functions */
/*------------------*/
void shortDelay(){delay();}
void mediumDelay() {delay(); delay();}




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

void pRT2(void) {
	int i;
	for (i=0; i<7;i++){
	LEDRed_On();
	mediumDelay();
	LEDRed_Toggle();
	mediumDelay();
	}
}


/*--------------------------------------------*/
/* Main function - start concurrent execution */
/*--------------------------------------------*/
int main(void) {	
	 
	LED_Initialize();

    /* Create processes */ 
    if (process_rt_create(pRT1, RT_STACK, &t_pRT1, &t_20sec) < 0) { return -1; } 
		if (process_rt_create(pRT2, RT_STACK, &t_pRT2, &t_10sec) < 0) { return -1; } 
   
    /* Launch concurrent execution */
	process_start();

  LED_Off();
  while(process_deadline_miss>0) {
		LEDGreen_On();
		shortDelay();
		LED_Off();
		shortDelay();
		process_deadline_miss--;
	}
	
	/* Hang out in infinite loop (so we can inspect variables if we want) */ 
	while (1);
	return 0;
}
