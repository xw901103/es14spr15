#ifndef AWG_H
#define AWG_H

#include "global.h"

#ifdef NO_INTERRUPT
#error "AWG module depends on interrupt feature enabled."
#endif

#define NB_AWG_CHANNELS                 2
#define AWG_ARBITRARY_WAVE_SIZE         256
#define AWG_ANALOG_OUTPUT_SAMPLING_RATE 1000 /* 1ms in micromseconds */

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

typedef struct
{
  TAWGWaveformType waveformType;

  UINT16 sample;  
  
  BOOL isActive;
  BOOL isEnabled;
  
  UINT16 frequency;
  UINT16 amplitude;
  INT16 offset;
    
} TAWGChannel;

extern TAWGChannel AWG_Channel[NB_AWG_CHANNELS];

extern UINT16 AWG_ARBITRARY_WAVE[AWG_ARBITRARY_WAVE_SIZE];

void AWG_Setup(const UINT32 busClk);

#endif