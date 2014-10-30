/**
 * \file main.c
 * \brief
 * \author
 * \date
 */
#include "main.h"
#include "analog.h"

const sineWaveLookupTable[1024] =
{
0x400,0x440,0x480,0x4c0,0x4ff,0x53c,0x579,0x5b4,
0x5ed,0x625,0x65a,0x68d,0x6bd,0x6ea,0x715,0x73c,
0x761,0x781,0x79f,0x7b8,0x7ce,0x7e0,0x7ee,0x7f8,
0x7fe,0x800,0x7fe,0x7f8,0x7ee,0x7e0,0x7ce,0x7b8,
0x79f,0x781,0x761,0x73c,0x715,0x6ea,0x6bd,0x68d,
0x65a,0x625,0x5ed,0x5b4,0x579,0x53c,0x4ff,0x4c0,
0x480,0x440,0x400,0x3c0,0x380,0x340,0x301,0x2c4,
0x287,0x24c,0x213,0x1db,0x1a6,0x173,0x143,0x116,
0xeb,0xc4,0x9f,0x7f,0x61,0x48,0x32,0x20,
0x12,0x8,0x2,0x0,0x2,0x8,0x12,0x20,
0x32,0x48,0x61,0x7f,0x9f,0xc4,0xeb,0x116,
0x143,0x173,0x1a6,0x1db,0x213,0x24c,0x287,0x2c4,
0x301,0x340,0x380,0x3c0,0x400,
};

// 100 sample every 1ms

const V_MIN = 1023;

void main(void)
{
  static UINT32 count = 0;
  static UINT16 index = 0;
  //static BOOL toggle = bFALSE;
  const UINT32 period_100hz = 13; // 0x0400 for 1ms
  const UINT32 period_0dot1hz = 0x00004F00;
  
  for(;;)
  {
    if (++count >= period_0dot1hz)
    {
      //if (toggle)
      //{        
      //  Analog_Set(ANALOG_OUTPUT_Ch1, 1023);
      //}
      //else
      //{
      //  Analog_Set(ANALOG_OUTPUT_Ch1, 3071);
      //}
      //toggle = !toggle;
      Analog_Set(ANALOG_OUTPUT_Ch1, sineWaveLookupTable[index++] + V_MIN);
      if (index == 100)
      {
        index = 0;
      }
      
      count = 0;      
    }
  }
}

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

//#include "main.h"
//#include "CRG.h"
//#include "analog.h"
//#include "OS.h"
//#include <mc9s12a512.h>     /* derivative information */

//#pragma LINK_INFO DERIVATIVE "mc9s12a512"

//#define THREAD_STACK_SIZE 100

//static const UINT16 LED_TIME_ON  = 1;
//static const UINT16 LED_TIME_OFF = 1;

//static UINT8 LEDOnStack[THREAD_STACK_SIZE];
//static UINT8 LEDOffStack[THREAD_STACK_SIZE];

// ----------------------------------------
// Global Sempahores
// ----------------------------------------

//static OS_ECB *TurnLEDOn;
//static OS_ECB *TurnLEDOff;

//#ifndef NO_DEBUG
/**
 * \fn void LogDebug(const UINT16 lineNumber, const UINT16 err)
 * \brief Logs debug information including line number and error number.
 * \param lineNumber line number of source file
 * \param err error number refers to predefined errors
 */
//void LogDebug(const UINT16 lineNumber, const UINT16 err)
//{
    /* break point here */
//    UNUSED(lineNumber);
//    UNUSED(err);
//}
//#endif

// ----------------------------------------
// LEDOn
// ----------------------------------------
//static Vout = 4095;

//static void LEDOn(void *pData)
//{
  
//  for (;;)                       
//  {
    // Call one of the RTOS's services:
//    (void)OS_SemaphoreWait(TurnLEDOn, 0);

    // User Code
    
    // Turn LED on
    //PORTE_BIT7 = 0;

    //Analog_Set(ANALOG_OUTPUT_Ch1, Vout);
//    Analog_Set(ANALOG_OUTPUT_Ch1, 3071);
    
    //Vout -= 1000;

    // Call more RTOS services:
//    OS_TimeDelay(LED_TIME_ON);
//    (void)OS_SemaphoreSignal(TurnLEDOff);
//  }
//} 

// ----------------------------------------
// LEDOff
// ----------------------------------------

//static void LEDOff(void *pData)
//{

//  for (;;)                       
//  {
    // Call one of the RTOS's services:
//    (void)OS_SemaphoreWait(TurnLEDOff, 0);

    // User Code

    // Turn LED off
    //PORTE_BIT7 = 1;

    //Analog_Set(ANALOG_OUTPUT_Ch1, Vout);
//    Analog_Set(ANALOG_OUTPUT_Ch1, 1023);

    //Vout -= 1000;
    
    // Call more RTOS services:
//    OS_TimeDelay(LED_TIME_OFF);
//    (void)OS_SemaphoreSignal(TurnLEDOn);
//  }
//} 

//BOOL Initialize(void)
//{
//  if (!CRG_SetupPLL(CONFIG_BUSCLK, CONFIG_OSCCLK, CONFIG_REFCLK))
//  {
//#ifndef NO_DEBUG
//    DEBUG(__LINE__, ERR_CRGPLL_SETUP);
//#endif
//    return bFALSE;
//  }

//  Analog_Setup(CONFIG_BUSCLK);

  // Initialize the RTOS
//  OS_Init();         

//  return bTRUE;
//}

// ----------------------------------------
// main
// ----------------------------------------

//void main(void)
//{
//  OS_ERROR error;

  // Initialise hardware  
//  PORTE_BIT7 = 1; // Turn LED off
//  DDRE_BIT7  = 1; // Set /XCLKS as output 
  
//  UNUSED(Initialize());
    
  // Create semaphores for signalling between threads
//  TurnLEDOn = OS_SemaphoreCreate(1);
//  TurnLEDOff = OS_SemaphoreCreate(0);

  // Create threads using OS_ThreadCreate();
//  error = OS_ThreadCreate(LEDOn, 
//                  NULL,
//                  &LEDOnStack[THREAD_STACK_SIZE - 1],
//                  1);
//  error = OS_ThreadCreate(LEDOff, 
//                  NULL,
//                  &LEDOffStack[THREAD_STACK_SIZE - 1],
//                  2);

  // Start multithreading!  
//  OS_Start();      
//}
