#include "AWG.h"
#include "timer.h"
#include "analog.h"

#ifdef NO_INTERRUPT
#error "AWG module depends on interrupt feature enabled."
#endif

#define DAC_ZERO_VOLTAGE 2047

typedef struct
{
  UINT32 cycles;
  UINT32 halfCycles;
  UINT32 harmonyCycles;
  UINT32 count;
  UINT16 step;
  INT16 voltageScale;
  INT16 sampleFactor;
  INT16 value;
} TAWGEntryContext;

TAWGEntry AWG_Channel[NB_AWG_CHANNELS] = {0};
TAWGEntryContext AWGChannelContext[NB_AWG_CHANNELS] = {0};

UINT16 AWG_ARBITRARY_WAVE[AWG_ARBITRARY_WAVE_SIZE] = {0};

static UINT16 AWGRoutinePeriod = 0; /* delay period of signal generating process */
static TAWGPostProcessRoutine channelPostProcessRoutinePtr = (TAWGPostProcessRoutine) 0x0000;

/* url: http://www.meraman.com/htmls/en/sinTableOld.html */
const INT16 AWG_SINE_WAVE[] =
{
  0, 129, 257, 386, 515, 643, 772, 900, 1029, 1157, 1286,
  1414, 1543, 1671, 1799, 1927, 2055, 2183, 2311, 2439, 2567,
  2694, 2822, 2949, 3077, 3204, 3331, 3458, 3584, 3711, 3838,
  3964, 4090, 4216, 4342, 4468, 4593, 4718, 4843, 4968, 5093,
  5218, 5342, 5466, 5590, 5714, 5837, 5960, 6083, 6206, 6329,
  6451, 6573, 6695, 6816, 6937, 7058, 7179, 7299, 7419, 7539,
  7659, 7778, 7897, 8015, 8134, 8252, 8369, 8486, 8603, 8720,
  8836, 8952, 9068, 9183, 9298, 9412, 9526, 9640, 9753, 9866,
  9979, 10091, 10203, 10314, 10425, 10536, 10646, 10756, 10865,
  10974, 11082, 11190, 11298, 11405, 11511, 11618, 11723, 11829,
  11934, 12038, 12142, 12245, 12348, 12450, 12552, 12654, 12755,
  12855, 12955, 13054, 13153, 13252, 13350, 13447, 13544, 13640,
  13736, 13831, 13925, 14020, 14113, 14206, 14298, 14390, 14482,
  14572, 14662, 14752, 14841, 14929, 15017, 15104, 15191, 15277,
  15362, 15447, 15531, 15615, 15698, 15780, 15862, 15943, 16023,
  16103, 16182, 16261, 16339, 16416, 16493, 16569, 16644, 16719,
  16793, 16866, 16939, 17011, 17082, 17153, 17223, 17292, 17360,
  17428, 17496, 17562, 17628, 17693, 17758, 17821, 17884, 17947,
  18008, 18069, 18130, 18189, 18248, 18306, 18363, 18420, 18476,
  18531, 18585, 18639, 18692, 18744, 18796, 18846, 18896, 18946,
  18994, 19042, 19089, 19135, 19181, 19225, 19269, 19312, 19355,
  19397, 19437, 19478, 19517, 19556, 19593, 19631, 19667, 19702,
  19737, 19771, 19804, 19837, 19868, 19899, 19929, 19958, 19987,
  20014, 20041, 20067, 20093, 20117, 20141, 20164, 20186, 20207,
  20228, 20248, 20267, 20285, 20302, 20319, 20334, 20349, 20363,
  20377, 20389, 20401, 20412, 20422, 20431, 20440, 20447, 20454,
  20460, 20465, 20470, 20474, 20476, 20478, 20480, 20480, 20480,
  20478, 20476, 20474, 20470, 20465, 20460, 20454, 20447, 20440,
  20431, 20422, 20412, 20401, 20389, 20377, 20363, 20349, 20334,
  20319, 20302, 20285, 20267, 20248, 20228, 20207, 20186, 20164,
  20141, 20117, 20093, 20067, 20041, 20014, 19987, 19958, 19929,
  19899, 19868, 19837, 19804, 19771, 19737, 19702, 19667, 19631,
  19593, 19556, 19517, 19478, 19437, 19397, 19355, 19312, 19269,
  19225, 19181, 19135, 19089, 19042, 18994, 18946, 18896, 18846,
  18796, 18744, 18692, 18639, 18585, 18531, 18476, 18420, 18363,
  18306, 18248, 18189, 18130, 18069, 18008, 17947, 17884, 17821,
  17758, 17693, 17628, 17562, 17496, 17428, 17360, 17292, 17223,
  17153, 17082, 17011, 16939, 16866, 16793, 16719, 16644, 16569,
  16493, 16416, 16339, 16261, 16182, 16103, 16023, 15943, 15862,
  15780, 15698, 15615, 15531, 15447, 15362, 15277, 15191, 15104,
  15017, 14929, 14841, 14752, 14662, 14572, 14482, 14390, 14298,
  14206, 14113, 14020, 13925, 13831, 13736, 13640, 13544, 13447,
  13350, 13252, 13153, 13054, 12955, 12855, 12755, 12654, 12552,
  12450, 12348, 12245, 12142, 12038, 11934, 11829, 11723, 11618,
  11511, 11405, 11298, 11190, 11082, 10974, 10865, 10756, 10646,
  10536, 10425, 10314, 10203, 10091, 9979, 9866, 9753, 9640, 9526,
  9412, 9298, 9183, 9068, 8952, 8836, 8720, 8603, 8486, 8369, 8252,
  8134, 8015, 7897, 7778, 7659, 7539, 7419, 7299, 7179, 7058, 6937,
  6816, 6695, 6573, 6451, 6329, 6206, 6083, 5960, 5837, 5714, 5590,
  5466, 5342, 5218, 5093, 4968, 4843, 4718, 4593, 4468, 4342, 4216,
  4090, 3964, 3838, 3711, 3584, 3458, 3331, 3204, 3077, 2949, 2822,
  2694, 2567, 2439, 2311, 2183, 2055, 1927, 1799, 1671, 1543, 1414,
  1286, 1157, 1029, 900, 772, 643, 515, 386, 257, 129, 0, -129, -257,
  -386, -515, -643, -772, -900, -1029, -1157, -1286, -1414, -1543, -1671,
  -1799, -1927, -2055, -2183, -2311, -2439, -2567, -2694, -2822, -2949, -3077,
  -3204, -3331, -3458, -3584, -3711, -3838, -3964, -4090, -4216, -4342, -4468,
  -4593, -4718, -4843, -4968, -5093, -5218, -5342, -5466, -5590, -5714, -5837,
  -5960, -6083, -6206, -6329, -6451, -6573, -6695, -6816, -6937, -7058, -7179,
  -7299, -7419, -7539, -7659, -7778, -7897, -8015, -8134, -8252, -8369, -8486,
  -8603, -8720, -8836, -8952, -9068, -9183, -9298, -9412, -9526, -9640, -9753,
  -9866, -9979, -10091, -10203, -10314, -10425, -10536, -10646, -10756, -10865,
  -10974, -11082, -11190, -11298, -11405, -11511, -11618, -11723, -11829, -11934,
  -12038, -12142, -12245, -12348, -12450, -12552, -12654, -12755, -12855, -12955,
  -13054, -13153, -13252, -13350, -13447, -13544, -13640, -13736, -13831, -13925,
  -14020, -14113, -14206, -14298, -14390, -14482, -14572, -14662, -14752, -14841,
  -14929, -15017, -15104, -15191, -15277, -15362, -15447, -15531, -15615, -15698,
  -15780, -15862, -15943, -16023, -16103, -16182, -16261, -16339, -16416, -16493,
  -16569, -16644, -16719, -16793, -16866, -16939, -17011, -17082, -17153, -17223,
  -17292, -17360, -17428, -17496, -17562, -17628, -17693, -17758, -17821, -17884,
  -17947, -18008, -18069, -18130, -18189, -18248, -18306, -18363, -18420, -18476,
  -18531, -18585, -18639, -18692, -18744, -18796, -18846, -18896, -18946, -18994,
  -19042, -19089, -19135, -19181, -19225, -19269, -19312, -19355, -19397, -19437,
  -19478, -19517, -19556, -19593, -19631, -19667, -19702, -19737, -19771, -19804,
  -19837, -19868, -19899, -19929, -19958, -19987, -20014, -20041, -20067, -20093,
  -20117, -20141, -20164, -20186, -20207, -20228, -20248, -20267, -20285, -20302,
  -20319, -20334, -20349, -20363, -20377, -20389, -20401, -20412, -20422, -20431,
  -20440, -20447, -20454, -20460, -20465, -20470, -20474, -20476, -20478, -20480,
  -20480, -20480, -20478, -20476, -20474, -20470, -20465, -20460, -20454, -20447,
  -20440, -20431, -20422, -20412, -20401, -20389, -20377, -20363, -20349, -20334,
  -20319, -20302, -20285, -20267, -20248, -20228, -20207, -20186, -20164, -20141,
  -20117, -20093, -20067, -20041, -20014, -19987, -19958, -19929, -19899, -19868,
  -19837, -19804, -19771, -19737, -19702, -19667, -19631, -19593, -19556, -19517,
  -19478, -19437, -19397, -19355, -19312, -19269, -19225, -19181, -19135, -19089,
  -19042, -18994, -18946, -18896, -18846, -18796, -18744, -18692, -18639, -18585,
  -18531, -18476, -18420, -18363, -18306, -18248, -18189, -18130, -18069, -18008,
  -17947, -17884, -17821, -17758, -17693, -17628, -17562, -17496, -17428, -17360,
  -17292, -17223, -17153, -17082, -17011, -16939, -16866, -16793, -16719, -16644,
  -16569, -16493, -16416, -16339, -16261, -16182, -16103, -16023, -15943, -15862,
  -15780, -15698, -15615, -15531, -15447, -15362, -15277, -15191, -15104, -15017,
  -14929, -14841, -14752, -14662, -14572, -14482, -14390, -14298, -14206, -14113,
  -14020, -13925, -13831, -13736, -13640, -13544, -13447, -13350, -13252, -13153,
  -13054, -12955, -12855, -12755, -12654, -12552, -12450, -12348, -12245, -12142,
  -12038, -11934, -11829, -11723, -11618, -11511, -11405, -11298, -11190, -11082,
  -10974, -10865, -10756, -10646, -10536, -10425, -10314, -10203, -10091, -9979,
  -9866, -9753, -9640, -9526, -9412, -9298, -9183, -9068, -8952, -8836, -8720, -8603,
  -8486, -8369, -8252, -8134, -8015, -7897, -7778, -7659, -7539, -7419, -7299, -7179,
  -7058, -6937, -6816, -6695, -6573, -6451, -6329, -6206, -6083, -5960, -5837, -5714,
  -5590, -5466, -5342, -5218, -5093, -4968, -4843, -4718, -4593, -4468, -4342, -4216,
  -4090, -3964, -3838, -3711, -3584, -3458, -3331, -3204, -3077, -2949, -2822, -2694,
  -2567, -2439, -2311, -2183, -2055, -1927, -1799, -1671, -1543, -1414, -1286, -1157,
  -1029, -900, -772, -643, -515, -386, -257, -129
};

static TAnalogChannel outputChannelNumberLookupTable[NB_OUTPUT_CHANNELS] = { ANALOG_OUTPUT_Ch1,
                                                                             ANALOG_OUTPUT_Ch2,
                                                                             ANALOG_OUTPUT_Ch3,
                                                                             ANALOG_OUTPUT_Ch4 };

void AWGRoutine(TTimerChannel channelNb);

void AWGRoutine(TTimerChannel channelNb)
{
  //static UINT16 cycles = 1000;

  /* NOTE: channel enums might not be in numeric order */
  static TAWGChannel channelNumberLookupTable[NB_AWG_CHANNELS] = { AWG_Ch1,
                                                                   AWG_Ch2 };
  
  /* TODO: use modulus count for small voltage */
  
  UINT16 index = 0;
  UINT16 sampleIndex = 0;
  INT16 analogValue = 0;

  Timer_ScheduleRoutine(channelNb, AWGRoutinePeriod);

    
  for (index = 0; index < NB_AWG_CHANNELS; ++index) 
  {
    if (AWG_Channel[index].isEnabled)
    {
      switch(AWG_Channel[index].waveformType)
      {
        case AWG_WAVEFORM_SINE:
          /* TODO: replace fixed value table with fators and replace this implementation */          
          /* TODO: use half sine wave sample */
          if (AWGChannelContext[index].cycles < 1000)
          {
            sampleIndex = (UINT16)(AWGChannelContext[index].count * AWGChannelContext[index].sampleFactor);            
          }
          else
          {
            sampleIndex = (UINT16)(AWGChannelContext[index].count / AWGChannelContext[index].sampleFactor);
          }
          sampleIndex = sampleIndex % 1000;
          analogValue = DAC_ZERO_VOLTAGE + AWG_SINE_WAVE[sampleIndex] / AWGChannelContext[index].voltageScale;
          break;
        case AWG_WAVEFORM_SQUARE:
          /* if I could get square wave percisely in 0.1hz step then reset of them will be no problem */
          if (AWGChannelContext[index].count < AWGChannelContext[index].halfCycles)
          {
            analogValue = DAC_ZERO_VOLTAGE + AWG_Channel[index].amplitude;
          }
          else
          {
            analogValue = DAC_ZERO_VOLTAGE - AWG_Channel[index].amplitude;
          }
          break;
        case AWG_WAVEFORM_TRIANGLE:
          AWGChannelContext[index].value += (AWGChannelContext[index].step * 2);
          
          if (AWGChannelContext[index].count == 0 || AWGChannelContext[index].count == AWGChannelContext[index].halfCycles)
          {
            AWGChannelContext[index].value = 0;
          }
          
          if (AWGChannelContext[index].count < AWGChannelContext[index].halfCycles)
          {            
            analogValue = (DAC_ZERO_VOLTAGE + AWG_Channel[index].amplitude) - AWGChannelContext[index].value;
          }
          else
          {
            analogValue = (DAC_ZERO_VOLTAGE - AWG_Channel[index].amplitude) + AWGChannelContext[index].value;
          }
          break;
        case AWG_WAVEFORM_SAWTOOTH:
          AWGChannelContext[index].value += AWGChannelContext[index].step;
          
          if (AWGChannelContext[index].count == 0)
          {
            AWGChannelContext[index].value = 0;
          }
          
          if (AWGChannelContext[index].count == (AWGChannelContext[index].cycles - 1))
          {
            analogValue = DAC_ZERO_VOLTAGE - AWG_Channel[index].amplitude;
          }
          else
          {            
            analogValue = (DAC_ZERO_VOLTAGE + AWG_Channel[index].amplitude) - AWGChannelContext[index].value;
          }
          break;
        case AWG_WAVEFORM_NOISE:
          break;
        case AWG_WAVEFORM_ARBITRARY:
          break;
        case AWG_WAVEFORM_DC:
        default:
          break;
      }
      
      if (++AWGChannelContext[index].count >= AWGChannelContext[index].cycles)
      {
        AWGChannelContext[index].count = AWGChannelContext[index].count % AWGChannelContext[index].cycles;
      }
      
      //if (++AWGChannelContext[index].count >= AWGChannelContext[index].harmonyCycles)
      //{
      //  AWGChannelContext[index].count = AWGChannelContext[index].count % AWGChannelContext[index].harmonyCycles;
      //}
      
      analogValue -= AWG_Channel[index].offset;
      
      if (analogValue > 4095)
      {
        analogValue = 4095;
      }
      else if (analogValue < 0)
      {
        analogValue = 0;
      }
      
      Analog_Put(outputChannelNumberLookupTable[index], analogValue);
      
      if (channelPostProcessRoutinePtr)
      {
        channelPostProcessRoutinePtr(channelNumberLookupTable[index]);
      }
      
    }
  }
}

void AWG_Update(void)
{
  UINT16 index = 0;
  for (index = 0; index < NB_AWG_CHANNELS; ++index)
  {
    /* TODO: use larger cycles so we could produce 99.9hz in about 10 seconds or 100 seconds*/
    AWGChannelContext[index].cycles = 10000 / AWG_Channel[index].frequency;
    AWGChannelContext[index].halfCycles = AWGChannelContext[index].cycles / 2;
      
    AWGChannelContext[index].step = (UINT16)(AWG_Channel[index].amplitude / AWGChannelContext[index].halfCycles);  
    
    if (AWG_Channel[index].waveformType == AWG_WAVEFORM_SAWTOOTH)
    {      
      if (AWG_Channel[index].amplitude % AWGChannelContext[index].halfCycles != 0)
      {
        AWGChannelContext[index].step ++;
      }
    }
    
    /* make sure it is not divided by zero */
    if (AWG_Channel[index].amplitude > 0)
    {      
      AWGChannelContext[index].voltageScale = 20480 / AWG_Channel[index].amplitude;
    }
    else
    {
      AWGChannelContext[index].voltageScale = 0;
    }
    
    if (AWGChannelContext[index].cycles < 1000)
    {
      AWGChannelContext[index].sampleFactor = (UINT16)(1000 / AWGChannelContext[index].cycles);
    }
    else
    {
      AWGChannelContext[index].sampleFactor = (UINT16)(AWGChannelContext[index].cycles / 1000);
    }
    
    AWGChannelContext[index].harmonyCycles = AWGChannelContext[index].sampleFactor * 1000;
    
    if (AWG_Channel[index].waveformType == AWG_WAVEFORM_SINE)
    {
      AWGChannelContext[index].cycles = AWGChannelContext[index].harmonyCycles;
    }
            
    AWGChannelContext[index].value = 0;
    
    AWGChannelContext[index].count = 0;
  }
}

void AWG_Setup(const UINT32 busClk)
{
  TTimerSetup timerCh5;
  UINT16 index = 0xFF;
  
  timerCh5.outputCompare    = bTRUE;
  timerCh5.outputAction     = TIMER_OUTPUT_DISCONNECT;
  timerCh5.inputDetection   = TIMER_INPUT_OFF;
  timerCh5.toggleOnOverflow = bFALSE;
  timerCh5.interruptEnable  = bFALSE;
  timerCh5.pulseAccumulator = bFALSE;
  timerCh5.routine          = &AWGRoutine;
  
  Timer_Init(TIMER_Ch5, &timerCh5);
  
  AWGRoutinePeriod = (UINT16)((busClk / MATH_1_MEGA) * AWG_ANALOG_OUTPUT_SAMPLING_RATE);

  for (index = 0; index < NB_AWG_CHANNELS; ++index)
  {    
    Analog_Put(outputChannelNumberLookupTable[index], DAC_ZERO_VOLTAGE);
    /* hot fix for analog sampling issue */
    Analog_Output[index].OldValue.l = Analog_Output[index].Value.l;  
  }
  
  /* kick start our hitchhiker here */
  Timer_Set(TIMER_Ch5, AWGRoutinePeriod);
  Timer_Enable(TIMER_Ch5, bTRUE);  
}

void AWG_AttachPostProcessRoutine(TAWGPostProcessRoutine routine)
{
  channelPostProcessRoutinePtr = routine;
}

void AWG_DetachPostProcessRoutine(void)
{
  channelPostProcessRoutinePtr = (TAWGPostProcessRoutine) 0x0000;
}
