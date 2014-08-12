/**
 * \author Xu Waycell
 * \date
 */
#include "packet.h"
#include "SCI.h"

UINT8 Packet_Command = 0, Packet_Parameter1 = 0, Packet_Parameter2 = 0, Packet_Parameter3 = 0;
TUINT16 ModConNumber;

UINT8 Packet_Checksum(const UINT8 command, const UINT8 parameter1, 
  const UINT8 parameter2, const UINT8 parameter3) {
    return (command ^ parameter1 ^ parameter2 ^ parameter3); 
}

BOOL Packet_Setup(const UINT32 baudRate, const UINT32 busClk) {
    SCI_Setup(baudRate, busClk);
    return Packet_Put_ModCon_Startup();
}

BOOL Packet_Get(void) {
    UINT8 checksum = 0;
    
    SCI_Poll();
    if (SCI_InChar(&Packet_Command)) { /* if we got first byte then we're gonna poll out the reset of them */
        while(!SCI_InChar(&Packet_Parameter1))
            SCI_Poll();
        while(!SCI_InChar(&Packet_Parameter2))
            SCI_Poll();
        while(!SCI_InChar(&Packet_Parameter3))
            SCI_Poll();
        while(!SCI_InChar(&checksum))
            SCI_Poll();    
        if (checksum == (Packet_Command ^ Packet_Parameter1 ^ Packet_Parameter2 ^ Packet_Parameter3)) {
            return bTRUE;
        }
    }
    return bFALSE;   
}

BOOL Packet_Put(const UINT8 command, const UINT8 parameter1, 
  const UINT8 parameter2, const UINT8 parameter3) { 
    return SCI_OutChar(command) &&
           SCI_OutChar(parameter1) &&
           SCI_OutChar(parameter2) &&
           SCI_OutChar(parameter3) &&
           SCI_OutChar(Packet_Checksum(command, parameter1, parameter2, parameter3));
}

BOOL Packet_Put_ModCon_Startup(void) {
    return Packet_Put(MODCON_COMMAND_STARTUP, 0, 0, 0) &&
           Packet_Put_ModCon_Version() &&
           Packet_Put_ModCon_Number_Get();    
}

BOOL Packet_Put_ModCon_Version(void) {
    return Packet_Put(MODCON_COMMAND_SPECIAL, MODCON_VERSION_INITIAL, MODCON_VERSION_MAJOR, MODCON_VERSION_MINOR);    
}

BOOL Packet_Put_ModCon_Number_Get(void) {
    return Packet_Put(MODCON_COMMAND_NUMBER, MODCON_NUMBER_GET, ModConNumberLSB, ModConNumberMSB);
}
