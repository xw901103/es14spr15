/**
 * \author Xu Waycell
 * \date 06-August-2014
 */
#include "packet.h"
#include "SCI.h"

UINT8 Packet_Command = 0, Packet_Parameter1 = 0, Packet_Parameter2 = 0, Packet_Parameter3 = 0;

typedef enum {
    STATE_0,
    STATE_1,
    STATE_2,
    STATE_3,
    STATE_4,
    STATE_5        
} PACKET_STATE;
static UINT8 _TEMP_Packet_Command = 0, _TEMP_Packet_Parameter1 = 0, _TEMP_Packet_Parameter2 = 0, _TEMP_Packet_Parameter3 = 0, _TEMP_Packet_Checksum = 0;
static PACKET_STATE _TEMP_Packet_State = STATE_1;

UINT8 Packet_Checksum(const UINT8 command, const UINT8 parameter1, 
  const UINT8 parameter2, const UINT8 parameter3) {
    return (command ^ parameter1 ^ parameter2 ^ parameter3); 
}

BOOL Packet_Setup(const UINT32 baudRate, const UINT32 busClk) {
    SCI_Setup(baudRate, busClk);
    return bTRUE;
}

BOOL Packet_Get(void) {
    SCI_Poll();
    switch(_TEMP_Packet_State) {
        case STATE_0:
            if (SCI_InChar(&_TEMP_Packet_Command)) {
                _TEMP_Packet_State = STATE_1;
            }
            break;
        case STATE_1:
            if (SCI_InChar(&_TEMP_Packet_Parameter1)) {
                _TEMP_Packet_State = STATE_2;
            }
            break;
        case STATE_2:
            if (SCI_InChar(&_TEMP_Packet_Parameter2)) {
                _TEMP_Packet_State = STATE_3;
            }
            break;
        case STATE_3:
            if (SCI_InChar(&_TEMP_Packet_Parameter3)) {
                _TEMP_Packet_State = STATE_4;
            }
            break;            
        case STATE_4:
            if (SCI_InChar(&_TEMP_Packet_Checksum)) {
                _TEMP_Packet_State = STATE_5;
            }
            break;
        case STATE_5:
                if (_TEMP_Packet_Checksum != Packet_Checksum(_TEMP_Packet_Command, _TEMP_Packet_Parameter1, _TEMP_Packet_Parameter2, _TEMP_Packet_Parameter3)) {
                    _TEMP_Packet_Command = _TEMP_Packet_Parameter1;
                    _TEMP_Packet_Parameter1 = _TEMP_Packet_Parameter2;
                    _TEMP_Packet_Parameter2 = _TEMP_Packet_Parameter3;
                    _TEMP_Packet_Parameter3 = _TEMP_Packet_Checksum;
                    _TEMP_Packet_State = STATE_4;                    
                } else {
                    Packet_Command = _TEMP_Packet_Command;
                    Packet_Parameter1 = _TEMP_Packet_Parameter1;
                    Packet_Parameter2 = _TEMP_Packet_Parameter2;
                    Packet_Parameter3 = _TEMP_Packet_Parameter3;
                    _TEMP_Packet_State = STATE_0;
                    return bTRUE;
                }
            break;
        default:
            break;
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
