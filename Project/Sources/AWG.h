/**
 * \file AWG.h
 * \author Xu Waycell
 */
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
  
  INT32 arbitraryWave[AWG_ARBITRARY_WAVE_SIZE]; /* arbitrary wave buffer */
    
} TAWGEntry;

/**
 * \brief channel setting
 */
extern TAWGEntry AWG_Channel[NB_AWG_CHANNELS];

/**
 * \brief arbitrary wave buffer
 */
//extern INT32 AWG_ARBITRARY_WAVE[AWG_ARBITRARY_WAVE_SIZE];

/**
 * \fn void AWG_Setup(const UINT32 busClk)
 * \brief Setup arbitrary waveform generator runtime configurations
 * \param busClk bus clock
 */
void AWG_Setup(const UINT32 busClk);

/**
 * \fn void AWG_Update(TAWGChannel channelNb)
 * \brief Update runtime context of given AWG channel
 * \param channelNb AWG channel number
 */
void AWG_Update(TAWGChannel channelNb);

/**
 * \fn void AWG_Enable(TAWGChannel channelNb, BOOL enable)
 * \brief Enables/Disables given AWG channel
 * \param channelNb AWG channel number
 * \param enable TRUE if channel should be enable or FALSE otherwise
 */
void AWG_Enable(TAWGChannel channelNb, BOOL enable);

/**
 * \fn void AWG_ApplyArbitraryPhasor(UINT8 harmonicNb, UINT16 magnitude, INT16 angle)
 * \brief Apply phasor to arbitrary wave sample buffer
 * \param harmonicNb it represents nth phasor harmonic
 * \param magnitude phasor magnitude
 * \param angle phasor angle 
 */
void AWG_ApplyArbitraryPhasor(UINT8 harmonicNb, UINT16 magnitude, INT16 angle);

/**
 * \fn void AWG_ResetArbitraryWave(void)
 * \brief Clear arbitrary wave buffer
 */
void AWG_ResetArbitraryWave(void);

/**
 * \fn void AWG_AttachPostProcessRoutine(TAWGPostProcessRoutine routine)
 * \brief Attaches a routine for post analog process 
 * \param routine
 */
void AWG_AttachPostProcessRoutine(TAWGPostProcessRoutine routine);

/** 
 * \fn void AWG_DetachPostProcessRoutine(void)
 * \brief Removes attached post analog process routine
 */
void AWG_DetachPostProcessRoutine(void);

#endif
