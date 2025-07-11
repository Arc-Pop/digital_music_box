#include <stdint.h>
#include "os.h"
#include "../inc/CortexM.h"
#include "../inc/BSP.h"


// function definitions in osasm.s
void StartOS(void);

tcbType tcbs[NUMTHREADS];
tcbType *RunPt;
int32_t Stacks[NUMTHREADS][STACKSIZE];
void static runperiodicevents(void);
// ******** OS_Init ************
// Initialize operating system, disable interrupts
// Initialize OS controlled I/O: systick, bus clock as fast as possible
// Initialize OS global variables
// Inputs:  none
// Outputs: none
void OS_Init(void){
  DisableInterrupts();
  BSP_Clock_InitFastest();// set processor clock to fastest speed
  // initialize any global variables as needed

		for(int i=0 ; i<NUMTHREADS ; i++){
		tcbs[i].sp = 0;
		tcbs[i].next = 0;
		tcbs[i].blocked = 0;
		tcbs[i].sleep = 0;	
		}
		RunPt = 0;
		BSP_PeriodicTask_Init(&runperiodicevents, 1000, 0);		
}


void SetInitialStack(int i){
  tcbs[i].sp = &Stacks[i][STACKSIZE-16]; // thread stack pointer
  Stacks[i][STACKSIZE-1] = 0x01000000;   // thumb bit
  Stacks[i][STACKSIZE-3] = 0x14141414;   // R14
  Stacks[i][STACKSIZE-4] = 0x12121212;   // R12
  Stacks[i][STACKSIZE-5] = 0x03030303;   // R3
  Stacks[i][STACKSIZE-6] = 0x02020202;   // R2
  Stacks[i][STACKSIZE-7] = 0x01010101;   // R1
  Stacks[i][STACKSIZE-8] = 0x00000000;   // R0
  Stacks[i][STACKSIZE-9] = 0x11111111;   // R11
  Stacks[i][STACKSIZE-10] = 0x10101010;  // R10
  Stacks[i][STACKSIZE-11] = 0x09090909;  // R9
  Stacks[i][STACKSIZE-12] = 0x08080808;  // R8
  Stacks[i][STACKSIZE-13] = 0x07070707;  // R7
  Stacks[i][STACKSIZE-14] = 0x06060606;  // R6
  Stacks[i][STACKSIZE-15] = 0x05050505;  // R5
  Stacks[i][STACKSIZE-16] = 0x04040404;  // R4
}


//******** OS_AddThreads ***************
// Add four main threads to the scheduler
// Inputs: function pointers to four void/void main threads
// Outputs: 1 if successful, 0 if this thread can not be added
// This function will only be called once, after OS_Init and before OS_Launch
int OS_AddThreads(void(*thread0)(void), uint32_t p0,
                  void(*thread1)(void),	uint32_t p1,
									void(*thread2)(void),	uint32_t p2,
									void(*thread3)(void),	uint32_t p3,
									void(*thread4)(void),	uint32_t p4
									){
	int32_t status;
  status = StartCritical();
  tcbs[0].next = &tcbs[1]; // 0 points to 1
  tcbs[1].next = &tcbs[2]; // 1 points to 2
	tcbs[2].next = &tcbs[3]; // 1 points to 2
	tcbs[3].next = &tcbs[4]; // 1 points to 2
	tcbs[4].next = &tcbs[0]; // 1 points to 2										

  SetInitialStack(0); Stacks[0][STACKSIZE-2] = (int32_t)(thread0); // PC
  SetInitialStack(1); Stacks[1][STACKSIZE-2] = (int32_t)(thread1); // PC
  SetInitialStack(2); Stacks[2][STACKSIZE-2] = (int32_t)(thread2); // PC
  SetInitialStack(3); Stacks[3][STACKSIZE-2] = (int32_t)(thread3); // PC
  SetInitialStack(4); Stacks[4][STACKSIZE-2] = (int32_t)(thread4); // PC
										

	for(int i=0; i<NUMTHREADS; i++){ 
		tcbs[i].blocked = 0; 
		tcbs[i].sleep = 0;
	}

  tcbs[0].priority = p0; 
  tcbs[1].priority = p1; 
  tcbs[2].priority = p2; 
  tcbs[3].priority = p3; 
  tcbs[4].priority = p4; 
  
  RunPt = &tcbs[0];       // thread 0 will run first
  EndCritical(status);									 									 
  return 1;               // successful
}

void static runperiodicevents(void){
// Handle periodic events in RealTimeEvents
		for(int i=0; i<NUMTHREADS; i++){	
				if(tcbs[i].sleep>0){tcbs[i].sleep--;}
			}			
}


//******** OS_AddPeriodicEventThreads ***************
// Add two background periodic event threads
// Typically this function receives the highest priority
// Inputs: pointers to a void/void event thread function2
//         periods given in units of OS_Launch (Lab 2 this will be msec)
// Outputs: 1 if successful, 0 if this thread cannot be added
// It is assumed that the event threads will run to completion and return
// It is assumed the time to run these event threads is short compared to 1 msec
// These threads cannot spin, block, loop, sleep, or kill
// These threads can call OS_Signal
typedef struct{
	uint16_t Interval;
	uint32_t LastTick;
	void (*Thread)(void);
}Task_t;

Task_t Tasks[2];

int OS_AddPeriodicEventThreads(void(*thread1)(void), uint32_t period1,
  void(*thread2)(void), uint32_t period2){

	Tasks[0].Thread = thread1;
	Tasks[1].Thread = thread2;

	Tasks[0].Interval = period1;
	Tasks[1].Interval = period2;

	Tasks[0].LastTick = 0;
	Tasks[1].LastTick = 0;		
		
  return 1;
}


//******** OS_Launch ***************
// Start the scheduler, enable interrupts
// Inputs: number of clock cycles for each time slice
// Outputs: none (does not return)
// Errors: theTimeSlice must be less than 16,777,216
void OS_Launch(uint32_t theTimeSlice){
  STCTRL = 0;                  // disable SysTick during setup
  STCURRENT = 0;               // any write to current clears it
  SYSPRI3 =(SYSPRI3&0x00FFFFFF)|0xE0000000; // priority 7
  STRELOAD = theTimeSlice - 1; // reload value
  STCTRL = 0x00000007;         // enable, core clock and interrupt arm
  StartOS();                   // start on the first task
}


// runs every ms
void Scheduler(void){      // every time slice
// look at all threads in TCB list choose
// highest priority thread not blocked and not sleeping 
// If there are multiple highest priority (not blocked, not sleeping) run these round robin
uint32_t max = 255; // max
tcbType *pt;
tcbType *bestPt;
pt = RunPt; // search for highest thread not blocked or sleeping
do{
pt = pt->next; // skips at least one
if((pt->priority < max)&&((pt->blocked)==0)&&((pt->sleep)==0)){
max = pt->priority;
bestPt = pt;
}
} while(RunPt != pt); // look at all possible threads
RunPt = bestPt;
}


//******** OS_Suspend ***************
// Called by main thread to cooperatively suspend operation
// Inputs: none
// Outputs: none
// Will be run again depending on sleep/block status
void OS_Suspend(void){
  STCURRENT = 0;        // any write to current clears it
  INTCTRL = 0x04000000; // trigger SysTick
// next thread gets a full time slice
}


// ******** OS_Sleep ************
// place this thread into a dormant state
// input:  number of msec to sleep
// output: none
// OS_Sleep(0) implements cooperative multitasking
void OS_Sleep(uint32_t sleepTime){
	RunPt->sleep=sleepTime;			// set sleep parameter in TCB
	OS_Suspend();								// suspend, stops running
}


// ******** OS_InitSemaphore ************
// Initialize counting semaphore
// Inputs:  pointer to a semaphore
//          initial value of semaphore
// Outputs: none
void OS_InitSemaphore(int32_t *semaPt, int32_t value){
	(*semaPt)=value;
}


// ******** OS_Wait ************
// Decrement semaphore and block if less than zero
// Lab2 spinlock (does not suspend while spinning)
// Lab3 block if less than zero
// Inputs:  pointer to a counting semaphore
// Outputs: none
void OS_Wait(int32_t *semaPt){
	DisableInterrupts();
	(*semaPt)=(*semaPt)-1;
	if((*semaPt)<0){
		RunPt->blocked=semaPt; // reason it is blocked
		EnableInterrupts();
		OS_Suspend();					// run thread switcher
	}
	EnableInterrupts();
}


// ******** OS_Signal ************
// Increment semaphore
// Lab2 spinlock
// Lab3 wakeup blocked thread if appropriate
// Inputs:  pointer to a counting semaphore
// Outputs: none
void OS_Signal(int32_t *semaPt){
	tcbType *pt;
	DisableInterrupts();
	(*semaPt)=(*semaPt)+1;
	if((*semaPt)<=0){
		pt=RunPt->next; // search for one blocked on this
		while(pt->blocked!=semaPt){
			pt=pt->next;
		}
		pt->blocked=0; // wakeup this one
	}
	EnableInterrupts();
}

#define FSIZE 10    // can be any size
uint32_t PutI;      // index of where to put next
uint32_t GetI;      // index of where to get next
uint32_t Fifo[FSIZE];
int32_t CurrentSize;// 0 means FIFO empty, FSIZE means full
uint32_t LostData;  // number of lost pieces of data


// ******** OS_FIFO_Init ************
// Initialize FIFO.  The "put" and "get" indices initially
// are equal, which means that the FIFO is empty.  Also
// initialize semaphores to track properties of the FIFO
// such as size and busy status for Put and Get operations,
// which is important if there are multiple data producers
// or multiple data consumers.
// Inputs:  none
// Outputs: none
void OS_FIFO_Init(void){
PutI = GetI = 0;
OS_InitSemaphore(&CurrentSize, 0);
LostData=0;
}

// ******** OS_FIFO_Put ************
// Put an entry in the FIFO.  Consider using a unique
// semaphore to wait on busy status if more than one thread
// is putting data into the FIFO and there is a chance that
// this function may interrupt itself.
// Inputs:  data to be stored
// Outputs: 0 if successful, -1 if the FIFO is full
int OS_FIFO_Put(uint32_t data){
if(CurrentSize==FSIZE){
	LostData++;
  return -1;   // success
}
else{
	Fifo[PutI]=data;
	PutI = (PutI+1)%FSIZE;
	OS_Signal(&CurrentSize);
	return 0;
}	
// return 0; // success
}

// ******** OS_FIFO_Get ************
// Get an entry from the FIFO.  Consider using a unique
// semaphore to wait on busy status if more than one thread
// is getting data from the FIFO and there is a chance that
// this function may interrupt itself.
// Inputs:  none
// Outputs: data retrieved
uint32_t OS_FIFO_Get(void){uint32_t data;

OS_Wait(&CurrentSize);
data = Fifo[GetI];
GetI = (GetI+1)%FSIZE;
 return data;
}

int32_t *PeriodicSemaphore0;
uint32_t Period0; // time between signals
int32_t *PeriodicSemaphore1;
uint32_t Period1; // time between signals
void RealTimeEvents(void){int flag=0;
  static int32_t realCount = -10; // let all the threads execute once
  // Note to students: we had to let the system run for a time so all user threads ran at least one
  // before signalling the periodic tasks
  realCount++;
  if(realCount >= 0){
		if((realCount%Period0)==0){
      OS_Signal(PeriodicSemaphore0);
      flag = 1;
		}
    if((realCount%Period1)==0){
      OS_Signal(PeriodicSemaphore1);
      flag=1;
		}
    if(flag){
      OS_Suspend();
    }
  }
}

// ******** OS_PeriodTrigger0_Init ************
// Initialize periodic timer interrupt to signal 
// Inputs:  semaphore to signal
//          period in ms
// priority level at 0 (highest
// Outputs: none
void OS_PeriodTrigger0_Init(int32_t *semaPt, uint32_t period){
	PeriodicSemaphore0 = semaPt;
	Period0 = period;
	BSP_PeriodicTask_InitC(&RealTimeEvents,1000,0);
}

// ******** OS_PeriodTrigger1_Init ************
// Initialize periodic timer interrupt to signal 
// Inputs:  semaphore to signal
//          period in ms
// priority level at 0 (highest
// Outputs: none
void OS_PeriodTrigger1_Init(int32_t *semaPt, uint32_t period){
	PeriodicSemaphore1 = semaPt;
	Period1 = period;
	BSP_PeriodicTask_InitC(&RealTimeEvents,1000,0);
}

