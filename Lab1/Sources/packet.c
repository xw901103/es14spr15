#include "packet.h"
#include "SCI.h"

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
