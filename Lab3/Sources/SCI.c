/**
 * \author Xu Waycell 
 * \date 06-August-2014
 */
#include "SCI.h"
#include "FIFO.h"
#ifndef NO_INTERRUPT
#include "timer.h"
#endif
#include <mc9s12a512.h>

static TFIFO RxFIFO, TxFIFO; /* no one can touch them except SCI_ calls */

#ifndef NO_INTERRUPT

#define INITIAL_SCI0_TX_ISR_DELAY DONT_PANIC

static UINT16 SCI0TxRoutinePeriod = 0; /* delay period of transmission process */

/**
 * \fn void interrupt VectorNumber_Vsci0 SCI0RxISR(void)
 * \brief SCI0 receive interrupt service routine. This routine will be executed once the receive data register is full.
 */
void interrupt VectorNumber_Vsci0 SCI0RxISR(void);

/**
 * \fn void SCI0TxISR(const TTimerChannel channelNb)
 * \brief SCI0 transmission interrupt service routine. This routine will be executed once the timer channel output compare interrupt triggered.
 * \param channelNb timer channel number
 */
void SCI0TxISR(const TTimerChannel channelNb);

void interrupt VectorNumber_Vsci0 SCI0RxISR(void)
{
  /* handle receive interrupts */
  if (SCI0CR2_RIE)
  {
    /* check and clear receive data register full flag */
    if (SCI0SR1_RDRF)
    { 
      /* put it to receive buffer for later use */
      if (!FIFO_Put(&RxFIFO, SCI0DRL))
      { 
#ifndef NO_DEBUG
        /* generally, it should not be full. if it does, there is a design issue. */
        DEBUG(__LINE__, ERR_FIFO_PUT);
#endif
      }                                    
    }    
  }
}

void SCI0TxISR(const TTimerChannel channelNb)
{ 
  /* check if transmission data register is empty */ 
  if (SCI0SR1_TDRE)
  { 
    /* try to obtain one byte from transmission buffer */
    if (!FIFO_Get(&TxFIFO, &SCI0DRL) || TxFIFO.NbBytes == 0)
    {       
      /* disable output compare interrupt of timer channel 7 due to empty buffer */
      Timer_Enable(TIMER_Ch7, bFALSE);
      return;
    }
  }
  /* set up delay for next time */
  Timer_ScheduleRoutine(channelNb, SCI0TxRoutinePeriod);
}

#endif

void SCI_Setup(const UINT32 baudRate, const UINT32 busClk)
{
#ifndef NO_INTERRUPT

  TTimerSetup timerCh7;
  
  timerCh7.outputCompare    = bTRUE;
  timerCh7.outputAction     = TIMER_OUTPUT_DISCONNECT;
  timerCh7.inputDetection   = TIMER_INPUT_OFF;
  timerCh7.toggleOnOverflow = bFALSE;
  timerCh7.interruptEnable  = bFALSE;
  timerCh7.pulseAccumulator = bFALSE;
  timerCh7.routine          = &SCI0TxISR;
  
  /* fuel up our transmission ship docking on channel 7 */
  Timer_Init(TIMER_Ch7, &timerCh7);
  
  /* calculate bus clock cycles that is required to transmit one byte */
  SCI0TxRoutinePeriod = (UINT16)(10*busClk/baudRate);

#endif
  SCI0BD = (word)(busClk / baudRate / 16); /* baud rate */
  
  /* SCI control register 1 */
  SCI0CR1_PT      = 0; /* Parity Type                   0=event                1=odd                        */
  SCI0CR1_PE      = 0; /* Parity Enable                 0=off                  1=on                         */
  SCI0CR1_ILT     = 0; /* Idle Line Type                0=idle after start bit 1=idle after stop bit        */
  SCI0CR1_WAKE    = 0; /* Wakeup Condition              0=idle line wakeup     1=address mark wakeup        */
  SCI0CR1_M       = 0; /* Data Format Mode              0=8bits data           1=9bits data                 */
  SCI0CR1_RSRC    = 0; /* Receiver Source               0=internal             1=external                   */
  SCI0CR1_SCISWAI = 0; /* SCI Stop in Wait Mode         0=off                  1=on                         */
  SCI0CR1_LOOPS   = 0; /* Loop Select                   0=normal               1=loop                       */
  
  /* SCI control register 2 */
#ifndef NO_INTERRUPT
  SCI0CR2_SCTIE = 0;   /* Tx Interrupt Eanbled          0=off                  1=on                         */
  SCI0CR2_RIE   = 1;   /* Rx Full Interrupt Enabled     0=off                  1=on                         */
#else
  SCI0CR2_SCTIE = 0;   /* Tx Interrupt Eanbled          0=off                  1=on                         */
  SCI0CR2_RIE   = 0;   /* Rx Full Interrupt Enabled     0=off                  1=on                         */
#endif
  SCI0CR2_TCIE  = 0;   /* Tx Complete Interrupt Enabled 0=off                  1=on                         */
  SCI0CR2_ILIE  = 0;   /* Idle Line Interrupt Enabled   0=off                  1=on                         */
  SCI0CR2_TE    = 1;   /* Tx Enabled                    0=off                  1=on                         */
  SCI0CR2_RE    = 1;   /* Rx Enabled                    0=off                  1=on                         */
  SCI0CR2_RWU   = 0;   /* Rx Wakeup                     0=normal               1=enable the wakeup function */
  SCI0CR2_SBK   = 0;   /* Send Break Bit                0=off                  1=on                         */
     
  FIFO_Init(&RxFIFO); /* initialize receive buffer */    
  FIFO_Init(&TxFIFO); /* initialize transmit buffer */   
}

void SCI_Poll(void)
{
  /* check receive data register full flag */
  if (SCI0SR1_RDRF)
  { 
    /* put it to receive buffer for later use */
    if (!FIFO_Put(&RxFIFO, SCI0DRL))
    { 
#ifndef NO_DEBUG
      DEBUG(__LINE__, ERR_FIFO_PUT); /* generally, it should not be full. if it does, there is a design issue. */
#endif
    }                                    
  }
  
  /* check transmit data register empty flag */
  if (SCI0SR1_TDRE)
  { 
    /* try to get one byte from transmit buffer */
    (void)FIFO_Get(&TxFIFO, &SCI0DRL);
  }    
}

BOOL SCI_InChar(UINT8 * const dataPtr)
{
  if (dataPtr)
  {
    return FIFO_Get(&RxFIFO, dataPtr);   
  }
#ifndef NO_DEBUG
  DEBUG(__LINE__, ERR_INVALID_POINTER);
#endif    
  return bFALSE;
}

BOOL SCI_OutChar(const UINT8 data)
{
  /* simple wrap of transmit FIFO buffer */
  if (FIFO_Put(&TxFIFO, data))
  {
#ifndef NO_INTERRUPT  
    /* kick start our hitchhiker here by 42 */
    Timer_Set(TIMER_Ch7, INITIAL_SCI0_TX_ISR_DELAY);
    Timer_Enable(TIMER_Ch7, bTRUE);
#endif
    return bTRUE;
  }
  return bFALSE;
}
