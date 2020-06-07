#include "3140_concur.h"
#include <stdlib.h>
#include <MK64F12.h>
#include "realtime.h"


// Our PCB
struct process_state {
	unsigned int* original_sp;
	unsigned int* sp;
	unsigned int size;
	process_t* next_process;
    realtime_t* start;
    realtime_t* deadline;
    int realTime;
};

// Function Prototypes
int compare_currentTime(realtime_t* first, realtime_t second);
void enqueue_ready(process_t* new_process);
void enqueue_notReady(process_t* new_process);
void update_ready_queue();
int process_rt_create(void (*f)(void), int n, realtime_t *start, realtime_t *deadline);
int compare_time_d(process_t* first, process_t* second);
int compare_time(realtime_t* first, realtime_t* second);
realtime_t current_time;
process_t* process_queue = NULL;
process_t* current_process = NULL;
process_t* readyRT_queue = NULL;
process_t* notReadyRT_queue = NULL;
int process_deadline_met = 0;
int process_deadline_miss = 0;

// Function implementations

void enqueue(process_t* new_process){
	if (process_queue == NULL){
    	process_queue = new_process;
	}
	else{
    	process_t* tmp = process_queue;
    	while (tmp->next_process != NULL) {
        	tmp = tmp->next_process;
    	}
    	tmp->next_process = new_process;
	}
	new_process->next_process = NULL;
}

process_t* dequeue()
{
	if (process_queue == NULL) // If nothing in process queue, we cannot dequeue anything
	  return NULL;
	process_t* temp = process_queue;
	process_queue = process_queue->next_process;
	temp->next_process = NULL; // breaks off a process ( represented by a node in the linked list ) from the process_queue.
	return temp;
}

//1. initialize a null stack everytime
//2. In general how to implement a linked list
int process_create (void (*f)(void), int n){
	process_t* new_process = malloc(sizeof(process_t));
	unsigned int* stack_p = process_stack_init(f, n);
	if (new_process == NULL || stack_p == NULL) return -1;
	new_process->original_sp = stack_p;
	new_process->sp = stack_p;
	new_process->size = n;
  new_process->realTime = 0;
	enqueue(new_process);
	return 0;


	//In process create you put it in process queue_start
	//Process select move queue from current
	//(only place that can mofidy current process and process queue)


	//enqueue(*process_t proc) ->
	//pop - move ptr to point to ptr.next

}

void process_start (void){
	NVIC_EnableIRQ(PIT0_IRQn);
  NVIC_EnableIRQ(PIT1_IRQn);
	NVIC_SetPriority(SVCall_IRQn, 1);
  NVIC_SetPriority(PIT1_IRQn, 0);
  NVIC_SetPriority(PIT0_IRQn, 1);
	SIM->SCGC6 = SIM_SCGC6_PIT_MASK; // Enable clock to PIT module
	PIT->MCR = 0;
	PIT->CHANNEL[0].LDVAL = 200000; // Set load value of zeroth PIT (0x001FCA05)
  PIT->CHANNEL[0].TCTRL |= 0x00000003; // Enable Timer & Timer Interupts ?????
  PIT->CHANNEL[1].LDVAL = SystemCoreClock/1000; // Set load value of zeroth PIT (0x001FCA05)
  PIT->CHANNEL[1].TCTRL |= 0x00000003; // Enable Timer & Timer Interupts ?????
	process_begin();
}
//process start responsible for set up the timer, enable interrupt, call process begin

//current_proc(cursp)
//current_proc is a global variable, you have to update it manually
//if not finish put in pcb then add to queue
//process begin is provided, process start calls process begin which
//enable the timmer and IRQ calls process select

void PIT1_IRQHandler(void)
{
	  
    if (current_time.msec == 999){
        current_time.sec += 1;
        current_time.msec = 0;
    }
    else{
        current_time.msec += 1;
    }
    PIT->CHANNEL[1].TFLG |= 1;
}

int process_rt_create(void (*f)(void), int n, realtime_t *start, realtime_t *deadline){
  process_t* new_process = malloc(sizeof(process_t));
	unsigned int* stack_p = process_stack_init(f, n);
	if (new_process == NULL || stack_p == NULL) return -1;
	new_process->original_sp = stack_p;
	new_process->sp = stack_p;
	new_process->size = n;
  new_process->start = start;
  new_process->deadline = deadline;
  new_process->realTime = 1;
  if (compare_currentTime(start, current_time)) {
      enqueue_ready(new_process);
	    return 0;
	}
  else {
    enqueue_notReady(new_process);
		return 0;
		}
	
}
int compare_time_d(process_t* first, process_t* second){
    int firstTime = first->deadline->sec*1000+first->deadline->msec+first->start->sec*1000+first->start->msec;
    int secondTime = second->deadline->sec*1000+second->deadline->msec+second->start->sec*1000+second->start->msec;
    if (firstTime <= secondTime)
        return 1;
    else
        return 0;
}

void enqueue_ready(process_t* new_process){
	if (readyRT_queue == NULL){
    	readyRT_queue = new_process;
	}
	else{
				if (compare_time_d(new_process, readyRT_queue)) {
					new_process->next_process = readyRT_queue;
					readyRT_queue = new_process;
				}
				else{
				process_t* tmp = readyRT_queue;
        process_t* tmpNext;
        int update = 1;
        while (update){
            tmpNext = tmp->next_process;
            if (tmpNext == NULL){
                tmp->next_process = new_process;
                new_process->next_process = NULL;
                update = 0;
            }
            else if (compare_time_d(tmp, new_process)
            && compare_time_d(new_process, tmpNext)){
                new_process->next_process = tmpNext;
                tmp->next_process = new_process;
                update = 0;
            }
            else
                tmp = tmp->next_process;
        }
			}
    }
}

void enqueue_notReady(process_t* new_process){
    if (notReadyRT_queue == NULL){
    	notReadyRT_queue = new_process;
	}
    else{
				if (compare_time(new_process->start, notReadyRT_queue->start)) {
					new_process->next_process = notReadyRT_queue;
					notReadyRT_queue = new_process;
				}
				else{
				process_t* tmp = notReadyRT_queue;
        process_t* tmpNext;
        int update = 1;
        while (update){
            tmpNext = tmp->next_process;
            if (tmpNext == NULL){
                tmp->next_process = new_process;
                new_process->next_process = NULL;
                update = 0;
            }
            else if (compare_time(tmp->start, new_process->start)
            && compare_time(new_process->start, tmpNext->start)){
                new_process->next_process = tmpNext;
                tmp->next_process = new_process;
                update = 0;
            }
            else
                tmp = tmp->next_process;
        }
			}
    }
}


//return 1 is the first time is leass or equal to second time
int compare_time(realtime_t* first, realtime_t* second){
    int firstTime = 1000*first->sec+first->msec;
    int secondTime = 1000*second->sec+second->msec;
    if (firstTime <= secondTime)
        return 1;
    else
        return 0;
}

int compare_currentTime(realtime_t* first, realtime_t second){
    int firstTime = 1000*first->sec+first->msec;
    int secondTime = 1000*second.sec+second.msec;
    if (firstTime <= secondTime)
        return 1;
    else
        return 0;		
}

unsigned int * process_select(unsigned int * cursp){
    //check processes in not ready queue becomes ready
  update_ready_queue();
	if (current_process != NULL && cursp == NULL) {
        if ( (current_time.sec*1000+current_time.msec <= current_process->deadline->sec*1000+current_process->deadline->msec+current_process->start->sec*1000+current_process->start->msec)  
					&& ( current_process->realTime == 1 ) )
            process_deadline_met++;
        else if (current_process->realTime == 1 )
            process_deadline_miss++;
    	process_stack_free(current_process->original_sp, current_process->size);
    	free(current_process);
	}
	else if(current_process != NULL && cursp != NULL) {
    	  current_process->sp = cursp;
        if (current_process->realTime)
            enqueue_ready(current_process);
        else
            enqueue(current_process);
	}
	if (readyRT_queue == NULL && notReadyRT_queue == NULL && process_queue == NULL)
    	return NULL;
	else{
        if (readyRT_queue != NULL) {
          current_process = readyRT_queue;
          readyRT_queue = readyRT_queue->next_process;
					current_process->next_process = NULL;
        }
        else if (process_queue != NULL){
          current_process = process_queue;
        	process_queue = process_queue->next_process;
					current_process->next_process = NULL;
        }
        else {
					__enable_irq();
            while(!(compare_currentTime(notReadyRT_queue->start, current_time)));
					__disable_irq();
            current_process = notReadyRT_queue;
            notReadyRT_queue = notReadyRT_queue->next_process;
            current_process->next_process = NULL;
        }
	}
  return current_process == NULL ? NULL: current_process->sp;
}

void update_ready_queue(){
	__disable_irq();
    while (notReadyRT_queue != NULL && (compare_currentTime(notReadyRT_queue->start, current_time))) {
        process_t* tmp = notReadyRT_queue;
        notReadyRT_queue = notReadyRT_queue->next_process;
        tmp->next_process = NULL;
        enqueue_ready(tmp);
    }
		__enable_irq();
}
