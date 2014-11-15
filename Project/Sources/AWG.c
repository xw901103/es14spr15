#include "AWG.h"
#include "timer.h"
#include "analog.h"

#include "AWG.h"
#include "timer.h"
#include "analog.h"

#ifdef NO_INTERRUPT
#error "AWG module depends on interrupt feature enabled."
#endif

#define DAC_ZERO_VOLTAGE 2047

//typedef struct
//{
//  UINT32 cycles;
//  UINT32 halfCycles;
//  UINT32 harmonyCycles;
//  UINT32 count;
//  UINT16 step;
//  INT16 voltageScale;
//  INT16 sampleFactor;
//  INT16 value;
//} TAWGEntryContext;

typedef struct
{
  BOOL isRunning;

  //UINT32 frequency; /* frequency period in microseconds */
  TAWGWaveformType waveformType;
  UINT16 amplitude;
  INT16 offset;

	UINT32 frequencyPeriod;
	UINT32 halfFrequencyPeriod;
	UINT32 time;
	UINT32 timeStep;

  //UINT32 period2;
  //UINT32 period10;
  //UINT32 period100;
  //UINT32 period1000;
  
  //UINT16 cyclePeriod;
  //UINT16 cycleBoundary;
  //UINT16 halfCycleBoundary;
  
  //UINT16 cycleCount;
  
  //UINT16 sampleFactor;  
  
  //INT16 voltageScale;
  //INT16 voltageStep;
  
  //INT16 voltageValue;
    
} TAWGEntryContext;

typedef struct
{
  UINT32 busClk;  
  UINT16 scale;  
  UINT16 sampleRate;  
  UINT16 routinePeriod;
  
} TAWGRuntimeContext;

TAWGEntry AWG_Channel[NB_AWG_CHANNELS] = {0};
TAWGEntryContext AWGChannelContext[NB_AWG_CHANNELS] = {0};
TAWGRuntimeContext AWGRuntimeContext = {0};

UINT16 AWG_ARBITRARY_WAVE[AWG_ARBITRARY_WAVE_SIZE] = {0};

//static UINT16 AWGRoutinePeriod = 0; /* delay period of signal generating process */
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

void AWGUpdateContext(TAWGEntry* const, TAWGEntryContext* const);

void AWGRoutine(TTimerChannel channelNb)
{
  static TAWGChannel channelNumberLookupTable[NB_AWG_CHANNELS] = { AWG_Ch1,
                                                                   AWG_Ch2 };
  UINT16 index = 0xFFFF, sampleIndex = 0xFFFF;
  INT16 analogValue = 0;
  //TAnalogChannel analogChannel;
  
	Timer_ScheduleRoutine(channelNb, AWGRuntimeContext.routinePeriod); 
    
  //if (AWGChannelContext[index].isRunning)
  //{    
  //  Timer_ScheduleRoutine(channelNb, AWGChannelContext[index].cyclePeriod);  
  //}
  //else
  //{
  //  Timer_Enable(channelNb, bFALSE);
  //  return;
  //}
  
  for (index = 0; index < NB_AWG_CHANNELS; ++index) 
  {
    if (AWG_Channel[index].isEnabled)
    {
    
    	if (AWGChannelContext[index].time >= AWGChannelContext[index].frequencyPeriod)
    	{
    		AWGChannelContext[index].time = AWGChannelContext[index].time % AWGChannelContext[index].frequencyPeriod;
    	}
    
		  switch(AWGChannelContext[index].waveformType)
  		{
    		case AWG_WAVEFORM_SINE:
      //if (AWGChannelContext[index].cycleBoundary < 1000)
      //{
      //  sampleIndex = AWGChannelContext[index].cycleCount * AWGChannelContext[index].sampleFactor;
      //}
      //else
      //{
      //  sampleIndex = AWGChannelContext[index].cycleCount / AWGChannelContext[index].sampleFactor;
      //}
      //sampleIndex = sampleIndex % 1000;
      //analogValue = DAC_ZERO_VOLTAGE + AWG_SINE_WAVE[sampleIndex] / AWGChannelContext[index].voltageScale;
      		break;
    		case AWG_WAVEFORM_SQUARE:
      		if (AWGChannelContext[index].time < AWGChannelContext[index].halfFrequencyPeriod)
      		{
      			analogValue = DAC_ZERO_VOLTAGE - AWGChannelContext[index].amplitude;
      		}
      		else
      		{
      			analogValue = DAC_ZERO_VOLTAGE + AWGChannelContext[index].amplitude;
      		}
      		break;
    		case AWG_WAVEFORM_TRIANGLE:
      //AWGChannelContext[index].voltageValue += (AWGChannelContext[index].voltageStep * 2);
       
      //if (AWGChannelContext[index].cycleCount == 0 || AWGChannelContext[index].cycleCount == AWGChannelContext[index].halfCycleBoundary)
      //{
      //  AWGChannelContext[index].voltageValue = 0;
      //}
      //if (AWGChannelContext[index].cycleCount < AWGChannelContext[index].halfCycleBoundary)
      //{            
      //  analogValue = (DAC_ZERO_VOLTAGE + AWGChannelContext[index].amplitude) - AWGChannelContext[index].voltageValue;
      //}
      //else
      //{
      //  analogValue = (DAC_ZERO_VOLTAGE - AWGChannelContext[index].amplitude) + AWGChannelContext[index].voltageValue;
      //}      
      		break;
    		case AWG_WAVEFORM_SAWTOOTH:
      		//AWGChannelContext[index].voltageValue += AWGChannelContext[index].voltageStep;
          
          //if (AWGChannelContext[index].voltageValue >= AWGChannelContext[index].amplitude)
          //{
          //	AWGChannelContext[index].voltageValue = 0 - AWGChannelContext[index].amplitude;	
          //}
          analogValue = (DAC_ZERO_VOLTAGE + AWGChannelContext[index].amplitude) - (AWGChannelContext[index].amplitude * 2 / (UINT16)(AWGChannelContext[index].frequencyPeriod / AWGChannelContext[index].time));
      		//if (AWGChannelContext[index].cycleCount == 0)
      		//{
      	  //	AWGChannelContext[index].voltageValue = 0;
      		//}

      		//if (AWGChannelContext[index].cycleCount == (AWGChannelContext[index].cycleBoundary - 1))
      		//{
        	//	analogValue = DAC_ZERO_VOLTAGE - AWGChannelContext[index].amplitude;
      		//}
      		//else
      		//{            
        	//	analogValue = (DAC_ZERO_VOLTAGE + AWGChannelContext[index].amplitude) - AWGChannelContext[index].voltageValue;
      		//}
      		break;
    		case AWG_WAVEFORM_NOISE:
      		break;
    		case AWG_WAVEFORM_ARBITRARY:
      		break;
    		case AWG_WAVEFORM_DC:
    		default:
      		break;
  		}

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

			AWGChannelContext[index].time += AWGChannelContext[index].timeStep;
			
	  	if (channelPostProcessRoutinePtr)
  		{
    		channelPostProcessRoutinePtr(channelNumberLookupTable[index]);
  		} 
  		   		
    }
  }
  
  //if (++AWGChannelContext[index].cycleCount == AWGChannelContext[index].cycleBoundary)
  //{
  //  AWGChannelContext[index].cycleCount = 0;
  //}
      
};

void AWGUpdateContext(TAWGEntry* const entryPtr, TAWGEntryContext* const contextPtr)
{
  UINT16 frequency = 0;

  if (entryPtr && contextPtr)
  {
  
	  frequency = (UINT16)(entryPtr->frequency % 256); /* XX.F*/

  	switch(frequency)
  	{
    	case 25:  /* 0.1 */
      	frequency = 1;
      	break;
	    case 51:  /* 0.2 */
  	    frequency = 2;
 	      break;
    	case 76:  /* 0.3 */
      	frequency = 3;
      	break;
  	  case 102: /* 0.4 */
	      frequency = 4;
    	  break;
	    case 128: /* 0.5 */
  	    frequency = 5;
    	  break;
   	 	case 153: /* 0.6 */
      	frequency = 6;
      	break;
    	case 179: /* 0.7 */
      	frequency = 7;
      	break;
    	case 204: /* 0.8 */
      	frequency = 8;
      	break;
    	case 230: /* 0.9 */
      	frequency = 9;
      	break;
    	default:
      	break;
  	};
  
  	frequency += (entryPtr->frequency / 256) * 10;
      
    contextPtr->frequencyPeriod = MATH_1_MEGA * 10 / frequency;
    contextPtr->halfFrequencyPeriod = contextPtr->frequencyPeriod / 2;
    contextPtr->timeStep = AWG_ANALOG_OUTPUT_SAMPLING_RATE;
   
    contextPtr->waveformType = entryPtr->waveformType;
    contextPtr->amplitude = entryPtr->amplitude;
    contextPtr->offset = entryPtr->offset;
    
    //contextPtr->voltageValue = 0;
    
    //contextPtr->halfCycleBoundary = contextPtr->cycleBoundary / 2;        

    //if (contextPtr->cycleBoundary < 1000)
    //{
    //  contextPtr->sampleFactor = (UINT16)(1000 / contextPtr->cycleBoundary);
    //}
    //else
    //{
    //  contextPtr->sampleFactor = (UINT16)(contextPtr->cycleBoundary / 1000);
    //}

    //contextPtr->voltageStep = (INT16)(contextPtr->amplitude * 1000 / contextPtr->halfFrequencyPeriod);  
    
    //if (contextPtr->amplitude > 0)
    //{      
    //  contextPtr->voltageScale = 20480 / contextPtr->amplitude;
    //}
    //else
    //{
    //  contextPtr->voltageScale = 0;
    //}
        
  }  
}

void AWG_Update(TAWGChannel channelNb)
{
  UINT16 index = 0xFFFF;
  //TTimerChannel timerChannel;
 
  switch(channelNb)
  {
    case AWG_Ch1:
      index = 0;
  //    timerChannel = TIMER_Ch0;
      break;
    case AWG_Ch2:
      index = 1;
  //    timerChannel = TIMER_Ch1;
      break;
    case AWG_Ch3:
      index = 2;
  //    timerChannel = TIMER_Ch2;
      break;
    case AWG_Ch4:
      index = 3;
  //    timerChannel = TIMER_Ch3;
      break;      
    default:
      return;
      break;
  }
  
  if (AWGChannelContext[index].isRunning)
  {
    //Timer_Enable(timerChannel, bFALSE);
    AWGUpdateContext(&AWG_Channel[index], &AWGChannelContext[index]);  
    //Timer_Set(timerChannel, AWGChannelContext[index].cyclePeriod);
    //Timer_Enable(timerChannel, bTRUE);  
  }
}

void AWG_Enable(TAWGChannel channelNb, BOOL enable)
{
  UINT16 index = 0xFFFF;
  //TTimerChannel timerChannel;
  
  switch(channelNb)
  {
    case AWG_Ch1:
      index = 0;
  //    timerChannel = TIMER_Ch0;
      break;
    case AWG_Ch2:
      index = 1;
  //    timerChannel = TIMER_Ch1;
      break;
    case AWG_Ch3:
      index = 2;
  //    timerChannel = TIMER_Ch2;
      break;
    case AWG_Ch4:
      index = 3;
  //    timerChannel = TIMER_Ch3;
      break;      
    default:
      return;
      break;
  }
  
  if (AWG_Channel[index].isEnabled && enable)
  {    
    AWGUpdateContext(&AWG_Channel[index], &AWGChannelContext[index]);  
    AWGChannelContext[index].isRunning = bTRUE;  
    //Timer_Set(timerChannel, AWGChannelContext[index].cyclePeriod);
    //Timer_Enable(timerChannel, bTRUE);    
  }
  else
  {
    AWGChannelContext[index].isRunning = bFALSE;
    Analog_Put(outputChannelNumberLookupTable[index], DAC_ZERO_VOLTAGE);  
    //Timer_Enable(timerChannel, bFALSE);
  }
}

//void AWGRoutine(TTimerChannel channelNb)
//{

  /* NOTE: channel enums might not be in numeric order */
//  static TAWGChannel channelNumberLookupTable[NB_AWG_CHANNELS] = { AWG_Ch1,
//                                                                   AWG_Ch2 };
  
  /* TODO: use modulus count for small voltage */
  
//  UINT16 index = 0;
//  UINT16 sampleIndex = 0;
//  INT16 analogValue = 0;

//  Timer_ScheduleRoutine(channelNb, AWGRoutinePeriod);

    
//  for (index = 0; index < NB_AWG_CHANNELS; ++index) 
//  {
//    if (AWG_Channel[index].isEnabled)
//    {
//      switch(AWG_Channel[index].waveformType)
//      {
//        case AWG_WAVEFORM_SINE:
//          /* TODO: replace fixed value table with fators and replace this implementation */          
//          /* TODO: use half sine wave sample */
//          if (AWGChannelContext[index].cycles < 1000)
//          {
//            sampleIndex = (UINT16)(AWGChannelContext[index].count * AWGChannelContext[index].sampleFactor);            
//          }
//          else
//          {
//            sampleIndex = (UINT16)(AWGChannelContext[index].count / AWGChannelContext[index].sampleFactor);
//          }
//          sampleIndex = sampleIndex % 1000;
//          analogValue = DAC_ZERO_VOLTAGE + AWG_SINE_WAVE[sampleIndex] / AWGChannelContext[index].voltageScale;
//          break;
//        case AWG_WAVEFORM_SQUARE:
//          /* if I could get square wave percisely in 0.1hz step then reset of them will be no problem */
//          if (AWGChannelContext[index].count < AWGChannelContext[index].halfCycles)
//          {
//            analogValue = DAC_ZERO_VOLTAGE + AWG_Channel[index].amplitude;
//          }
//          else
//          {
//            analogValue = DAC_ZERO_VOLTAGE - AWG_Channel[index].amplitude;
//          }
//          break;
//        case AWG_WAVEFORM_TRIANGLE:
//          AWGChannelContext[index].value += (AWGChannelContext[index].step * 2);
//          
//          if (AWGChannelContext[index].count == 0 || AWGChannelContext[index].count == AWGChannelContext[index].halfCycles)
//          {
//            AWGChannelContext[index].value = 0;
//          }
//          
//          if (AWGChannelContext[index].count < AWGChannelContext[index].halfCycles)
//          {            
//            analogValue = (DAC_ZERO_VOLTAGE + AWG_Channel[index].amplitude) - AWGChannelContext[index].value;
//          }
//          else
//          {
//            analogValue = (DAC_ZERO_VOLTAGE - AWG_Channel[index].amplitude) + AWGChannelContext[index].value;
//          }
//          break;
//        case AWG_WAVEFORM_SAWTOOTH:
//          AWGChannelContext[index].value += AWGChannelContext[index].step;
//          
//          if (AWGChannelContext[index].count == 0)
//          {
//            AWGChannelContext[index].value = 0;
//          }
//          
//          if (AWGChannelContext[index].count == (AWGChannelContext[index].cycles - 1))
//          {
//            analogValue = DAC_ZERO_VOLTAGE - AWG_Channel[index].amplitude;
//          }
//          else
//          {            
//            analogValue = (DAC_ZERO_VOLTAGE + AWG_Channel[index].amplitude) - AWGChannelContext[index].value;
//          }
//          break;
//        case AWG_WAVEFORM_NOISE:
//          break;
//        case AWG_WAVEFORM_ARBITRARY:
//          break;
//        case AWG_WAVEFORM_DC:
//        default:
//          break;
//      }
//      
//      if (++AWGChannelContext[index].count >= AWGChannelContext[index].cycles)
//      {
//        AWGChannelContext[index].count = AWGChannelContext[index].count % AWGChannelContext[index].cycles;
//      }
//            
//      analogValue -= AWG_Channel[index].offset;
//      
//      if (analogValue > 4095)
//      {
//        analogValue = 4095;
//      }
//      else if (analogValue < 0)
//      {
//        analogValue = 0;
//      }
//      
//      Analog_Put(outputChannelNumberLookupTable[index], analogValue);
//      
//      if (channelPostProcessRoutinePtr)
//      {
//        channelPostProcessRoutinePtr(channelNumberLookupTable[index]);
//      }
//      
//    }
//  }
//}

//void AWG_Update(void)
//{
//  UINT16 index = 0;
//  for (index = 0; index < NB_AWG_CHANNELS; ++index)
//  {
//    /* TODO: use larger cycles so we could produce 99.9hz in about 10 seconds or 100 seconds*/
//    AWGChannelContext[index].cycles = 10000 / AWG_Channel[index].frequency;
//    AWGChannelContext[index].halfCycles = AWGChannelContext[index].cycles / 2;
//      
//    AWGChannelContext[index].step = (UINT16)(AWG_Channel[index].amplitude / AWGChannelContext[index].halfCycles);  
//    
//    if (AWG_Channel[index].waveformType == AWG_WAVEFORM_SAWTOOTH)
//    {      
//      if (AWG_Channel[index].amplitude % AWGChannelContext[index].halfCycles != 0)
//      {
//        AWGChannelContext[index].step ++;
//      }
//    }
//    
//    /* make sure it is not divided by zero */
//    if (AWG_Channel[index].amplitude > 0)
//    {      
//      AWGChannelContext[index].voltageScale = 20480 / AWG_Channel[index].amplitude;
//    }
//    else
//    {
//      AWGChannelContext[index].voltageScale = 0;
//    }
//    
//    if (AWGChannelContext[index].cycles < 1000)
//    {
//      AWGChannelContext[index].sampleFactor = (UINT16)(1000 / AWGChannelContext[index].cycles);
//    }
//    else
//    {
//      AWGChannelContext[index].sampleFactor = (UINT16)(AWGChannelContext[index].cycles / 1000);
//    }
//    
//    AWGChannelContext[index].harmonyCycles = AWGChannelContext[index].sampleFactor * 1000;
//    
//    if (AWG_Channel[index].waveformType == AWG_WAVEFORM_SINE)
//    {
//      AWGChannelContext[index].cycles = AWGChannelContext[index].harmonyCycles;
//    }
//            
//    AWGChannelContext[index].value = 0;
//    
//    AWGChannelContext[index].count = 0;
//  }
//}

//void AWG_Setup(const UINT32 busClk)
//{
//  TTimerSetup timerCh5;
//  UINT16 index = 0xFF;
//  
//  timerCh5.outputCompare    = bTRUE;
//  timerCh5.outputAction     = TIMER_OUTPUT_DISCONNECT;
//  timerCh5.inputDetection   = TIMER_INPUT_OFF;
//  timerCh5.toggleOnOverflow = bFALSE;
//  timerCh5.interruptEnable  = bFALSE;
//  timerCh5.pulseAccumulator = bFALSE;
//  timerCh5.routine          = &AWGRoutine;
//  
//  Timer_Init(TIMER_Ch5, &timerCh5);
//  
//  AWGRoutinePeriod = (UINT16)((busClk / MATH_1_MEGA) * AWG_ANALOG_OUTPUT_SAMPLING_RATE);
//
//  for (index = 0; index < NB_AWG_CHANNELS; ++index)
//  {    
//    Analog_Put(outputChannelNumberLookupTable[index], DAC_ZERO_VOLTAGE);
//    /* hot fix for analog sampling issue */
//    Analog_Output[index].OldValue.l = Analog_Output[index].Value.l;  
//  }
//  
//  /* kick start our hitchhiker here */
//  Timer_Set(TIMER_Ch5, AWGRoutinePeriod);
//  Timer_Enable(TIMER_Ch5, bTRUE);  
//}

void AWG_Setup(const UINT32 busClk)
{
  TTimerSetup timerCh5 = {
                           bTRUE,                   /* outputCompare    */
                           TIMER_OUTPUT_DISCONNECT, /* outputAction     */
                           TIMER_INPUT_OFF,         /* inputDetection   */
                           bFALSE,                  /* toggleOnOverflow */
                           bFALSE,                  /* interruptEnable  */
                           bFALSE,                  /* pulseAccumulator */
                           &AWGRoutine              /* routine          */
                         };
  UINT16 index = 0xFF;

  AWGRuntimeContext.busClk = busClk;
  AWGRuntimeContext.scale = AWGRuntimeContext.busClk / MATH_1_MEGA;
  AWGRuntimeContext.routinePeriod = (UINT16)(AWGRuntimeContext.scale * AWG_ANALOG_OUTPUT_SAMPLING_RATE);
    
  Timer_Init(TIMER_Ch5, &timerCh5);

  for (index = 0; index < NB_AWG_CHANNELS; ++index)
  {    
    Analog_Put(outputChannelNumberLookupTable[index], DAC_ZERO_VOLTAGE);
    /* hot fix for analog sampling issue */
    Analog_Output[index].OldValue.l = Analog_Output[index].Value.l;  
  }  

  Timer_Set(TIMER_Ch5, AWGRuntimeContext.routinePeriod);
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
