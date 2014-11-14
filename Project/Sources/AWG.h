#ifndef AWG_H
#define AWG_H

#include "global.h"

#ifdef NO_INTERRUPT
#error "AWG module depends on interrupt feature enabled."
#endif

#define NB_AWG_CHANNELS                 2
#define AWG_ARBITRARY_WAVE_SIZE         256
#define AWG_ANALOG_OUTPUT_SAMPLING_RATE 1000 /* 1000 micromseconds */

typedef enum
{
  AWG_WAVEFORM_DC,
  AWG_WAVEFORM_SINE,
  AWG_WAVEFORM_SQUARE,
  AWG_WAVEFORM_TRIANGLE,
  AWG_WAVEFORM_SAWTOOTH,
  AWG_WAVEFORM_NOISE,
  AWG_WAVEFORM_ARBITRARY
} TAWGWaveformType;

typedef enum
{
  AWG_Ch1 = 0x00,
  AWG_Ch2 = 0x01,
  AWG_Ch3 = 0x02,
  AWG_Ch4 = 0x03
} TAWGChannel;

typedef void(*TAWGPostProcessRoutine)(TAWGChannel);

typedef struct
{
  TAWGWaveformType waveformType;
        
  BOOL isActive;
  BOOL isEnabled;
  
  UINT16 frequency; /* 1 will be 0.1hz and 1000 will be 100hz */
  UINT16 amplitude;
  INT16 offset;
    
} TAWGEntry;

extern TAWGEntry AWG_Channel[NB_AWG_CHANNELS];

extern UINT16 AWG_ARBITRARY_WAVE[AWG_ARBITRARY_WAVE_SIZE];

void AWG_Setup(const UINT32 busClk);

void AWG_Update(TAWGChannel channelNb);

void AWG_Enable(TAWGChannel channelNb, BOOL enable);

void AWG_AttachPostProcessRoutine(TAWGPostProcessRoutine routine);

void AWG_DetachPostProcessRoutine(void);

#endif
