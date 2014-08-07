/**
 * \author Xu Waycell
 * \date
 */
#include "packet.h"
#include "SCI.h"

UINT8 Packet_Command = 0, Packet_Parameter1 = 0, Packet_Parameter2 = 0, Packet_Parameter3 = 0;

UINT8 Packet_Checksum(const TPACKET* const PACKET) {
    if (PACKET) {
        return (PACKET->Command ^ PACKET->Parameter1 ^ PACKET->Parameter2 ^ PACKET->Parameter3); 
    }
#ifndef NO_DEBUG
    DEBUG(__LINE__, ERR_INVALID_POINTER);    
#endif
    return 0;
}

BOOL Packet_Setup(const UINT32 baudRate, const UINT32 busClk) {
    SCI_Setup(baudRate, busClk);
    return Packet_Put(MODCON_COMMAND_STARTUP, 0, 0, 0) &&
           Packet_Put(MODCON_COMMAND_VERSION, MODCON_VERSION_INITIAL, MODCON_VERSION_MAJOR, MODCON_VERSION_MINOR) &&
           Packet_Put(MODCON_COMMAND_NUMBER, MODCON_NUMBER_GET, ModConNumberLSB, ModConNumberMSB);
}

BOOL Packet_Get() {
    UINT8 Checksum = 0;
    if (SCI_InChar(&Packet_Command)) { /* if we got first byte then we're gonna poll out the reset of them */
        while(!SCI_InChar(&Packet_Parameter1))
            SCI_Poll();
        while(!SCI_InChar(&Packet_Parameter2))
            SCI_Poll();
        while(!SCI_InChar(&Packet_Parameter3))
            SCI_Poll();
        while(!SCI_InChar(&Checksum))
            SCI_Poll();    
        if (Checksum == (Packet_Command ^ Packet_Parameter1 ^ Packet_Parameter2 ^ Packet_Parameter3)) {
            return bTRUE;
        }
    }
    return bFALSE;   
}

BOOL Packet_Put(const UINT8 command, const UINT8 parameter1, 
  const UINT8 parameter2, const UINT8 parameter3) {
    UINT8 Checksum = 0;
    
    Checksum = (command ^ parameter1 ^ parameter2 ^ parameter3); 
    
    return SCI_OutChar(command) && SCI_OutChar(parameter1) && SCI_OutChar(parameter2) && SCI_OutChar(parameter3) && SCI_OutChar(Checksum);
}
