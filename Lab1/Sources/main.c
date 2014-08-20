/**
 * \file main.c
 * \brief Program main entry file.
 * \author Xu Waycell
 * \date 05-August-2014
 */

#include "main.h"
#include "packet.h"

#ifndef NO_DEBUG
/* this will log errors from other functions, make sure you have a break point here */
void LogDebug(const UINT16 lineNumber, const UINT16 err) 
{
  /* break point here */
  UNUSED(lineNumber);
  UNUSED(err);
}
#endif

BOOL Handle_ModCon_Startup(void) 
{
  return Packet_Put(MODCON_COMMAND_STARTUP, 0, 0, 0) &&
         Handle_ModCon_Version() &&
         Handle_ModCon_Number_Get();    
}

BOOL Handle_ModCon_Version(void) 
{
  return Packet_Put(MODCON_COMMAND_SPECIAL, MODCON_VERSION_INITIAL, MODCON_VERSION_MAJOR, MODCON_VERSION_MINOR);    
}

BOOL Handle_ModCon_Number_Get(void)
{
  return Packet_Put(MODCON_COMMAND_NUMBER, MODCON_NUMBER_GET, ModConNumberLSB, ModConNumberMSB);
}

void Initialize(void) 
{
  ModConNumber.l = 7229;
    
  /* initialize ModCon packet communication module */
  if (!Packet_Setup(CONFIG_BAUDRATE, CONFIG_BUSCLK)) 
  {
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_PACKET_SETUP);
#endif
  }
}

void Routine(void) 
{
  UINT8 ack = 0;      /* abbreviation of acknowledgment */
  BOOL deny = bFALSE; /* DENY is using for acknowledgment purpose.                */
                        /* if DENY is true then acknowledgment bit will be removed */
    
  if (Packet_Get()) 
  { 
    ack = Packet_Command & MODCON_COMMAND_ACK_MASK; /* detect ACK mask from command */
    Packet_Command &= ~MODCON_COMMAND_ACK_MASK;     /* clear ACK mask from command */
        
    switch(Packet_Command) 
    {
      case MODCON_COMMAND_STARTUP:
        if (!Packet_Parameter1 && !Packet_Parameter2 && !Packet_Parameter3) 
        {                    
          if (!Handle_ModCon_Startup()) 
          { /* push first three packets of ModCon startup figures */
#ifndef NO_DEBUG
            DEBUG(__LINE__, ERR_PACKET_PUT);
#endif
          }
        }
        break;
      case MODCON_COMMAND_SPECIAL:
        /* make sure it is the version special query not some others */
        if (Packet_Parameter1 == MODCON_VERSION_INITIAL && Packet_Parameter2 == MODCON_VERSION_TOKEN && Packet_Parameter3 == CONTROL_CR) 
        {                    
          if (!Handle_ModCon_Version()) 
          {
#ifndef NO_DEBUG
            DEBUG(__LINE__, ERR_PACKET_PUT);
#endif
          }
        }
        else 
        {
          deny = bTRUE;
        }
        break;
      case MODCON_COMMAND_NUMBER:
        if (Packet_Parameter1 == MODCON_NUMBER_GET) 
        { /* send back current ModCon number after this */                
          if (!Handle_ModCon_Number_Get()) 
          {
#ifndef NO_DEBUG
            DEBUG(__LINE__, ERR_PACKET_PUT);
#endif
          }
        }
        else if (Packet_Parameter1 == MODCON_NUMBER_SET) 
        { /* we set our new number after this */
          ModConNumberLSB = Packet_Parameter2;
          ModConNumberMSB = Packet_Parameter3;
        }
        break;
      default:
        deny = bTRUE; /* not acceptable command so we deny */
        break;
    }
        
    if (ack) 
    {
      if (!deny) 
      {                
        if (!Packet_Put(Packet_Command | MODCON_COMMAND_ACK_MASK, Packet_Parameter1, Packet_Parameter2, Packet_Parameter3)) 
        {
#ifndef NO_DEBUG
          DEBUG(__LINE__, ERR_PACKET_PUT);
#endif
        }
      }
      else
      { /* NOTE: ACK mask has been cleared already */
        if (!Packet_Put(Packet_Command, Packet_Parameter1, Packet_Parameter2, Packet_Parameter3))
        {
#ifndef NO_DEBUG
          DEBUG(__LINE__, ERR_PACKET_PUT);
#endif
        }                
      }
    }
  }
}

void main(void)
{			  
  Initialize();                       /* initialize packet communication module */
  if (!Handle_ModCon_Startup())
  { /* queue startup packets for transmission */
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_PACKET_PUT);
#endif      
  }  
  for (;;)
  {
    Routine();                      /* ModCon command poll and execution routine */
  }
}
