// ----------------------------------------
// Filename: main.c
// Description: 
//   This file contains the high-level code for the project.
//   It initialises appropriate hardware subsystems,
//   creates application threads, and then starts the OS.
//   **************************************
//   An example of two threads communicating via a smepahore
//   is given that flashes the LED. These should be removed
//   when the use of threads and the RTOS is understood.
//   **************************************
//
// Author: PMcL
// Date: 12-May-09

#include "main.h"
#include "OS.h"
#include <mc9s12a512.h>     /* derivative information */

#pragma LINK_INFO DERIVATIVE "mc9s12a512"

#define THREAD_STACK_SIZE 100

static const UINT16 LED_TIME_ON  = 10;
static const UINT16 LED_TIME_OFF = 10;

static UINT8 LEDOnStack[THREAD_STACK_SIZE];
static UINT8 LEDOffStack[THREAD_STACK_SIZE];

// ----------------------------------------
// Global Sempahores
// ----------------------------------------

static OS_ECB *TurnLEDOn;
static OS_ECB *TurnLEDOff;

#ifndef NO_DEBUG
/**
 * \fn void LogDebug(const UINT16 lineNumber, const UINT16 err)
 * \brief Logs debug information including line number and error number.
 * \param lineNumber line number of source file
 * \param err error number refers to predefined errors
 */
void LogDebug(const UINT16 lineNumber, const UINT16 err)
{
    /* break point here */
    UNUSED(lineNumber);
    UNUSED(err);
}
#endif

// ----------------------------------------
// LEDOn
// ----------------------------------------

static void LEDOn(void *pData)
{
  for (;;)                       
  {
    // Call one of the RTOS's services:
    (void)OS_SemaphoreWait(TurnLEDOn, 0);

    // User Code
    
    // Turn LED on
    PORTE_BIT7 = 0;

    // Call more RTOS services:
    OS_TimeDelay(LED_TIME_ON);
    (void)OS_SemaphoreSignal(TurnLEDOff);
  }
} 

// ----------------------------------------
// LEDOff
// ----------------------------------------

static void LEDOff(void *pData)
{

  for (;;)                       
  {
    // Call one of the RTOS's services:
    (void)OS_SemaphoreWait(TurnLEDOff, 0);

    // User Code

    // Turn LED off
    PORTE_BIT7 = 1;

    // Call more RTOS services:
    OS_TimeDelay(LED_TIME_OFF);
    (void)OS_SemaphoreSignal(TurnLEDOn);
  }
} 

// ----------------------------------------
// main
// ----------------------------------------

void main(void)
{
  OS_ERROR error;

  // Initialise hardware  
  PORTE_BIT7 = 1; // Turn LED off
  DDRE_BIT7  = 1; // Set /XCLKS as output 

  // Initialize the RTOS
  OS_Init();         
  
  // Create semaphores for signalling between threads
  TurnLEDOn = OS_SemaphoreCreate(1);
  TurnLEDOff = OS_SemaphoreCreate(0);

  // Create threads using OS_ThreadCreate();
  error = OS_ThreadCreate(LEDOn, 
                  NULL,
                  &LEDOnStack[THREAD_STACK_SIZE - 1],
                  1);
  error = OS_ThreadCreate(LEDOff, 
                  NULL,
                  &LEDOffStack[THREAD_STACK_SIZE - 1],
                  2);

  // Start multithreading!  
  OS_Start();      
}