/**
 * \file AWG.c
 * \brief Implementation of AWG
 * \author Xu Waycell
 */
#include <stdlib.h>
#include <math.h>
#include <mc9s12a512.h>

#include "AWG.h"
#include "timer.h"
#include "analog.h"

#ifdef NO_INTERRUPT
#error "AWG module depends on interrupt feature enabled."
#endif

#define DAC_ZERO_VOLTAGE 2047
#define DAC_MAXIMUM      4095
#define DAC_MINIMUM      0

#define AWG_ARBITRARY_WAVE_INITIAL_VOLTAGE 0

#define AWG_SINE_WAVE_SIZE  1000

typedef struct
{
  BOOL isRunning;

  TAWGWaveformType waveformType;
  UINT16 amplitude;
  INT16 offset;

	UINT32 frequencyPeriod; /* frequency period in microseconds */
	UINT32 halfFrequencyPeriod;
	UINT32 time;
	UINT32 timeStep;

    
  INT16 voltage;
  INT16 voltageScale;
  INT16 voltageOffset;
    
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

INT32 AWG_ARBITRARY_WAVE[AWG_ARBITRARY_WAVE_SIZE] = {0};

static TAWGPostProcessRoutine channelPostProcessRoutinePtr = (TAWGPostProcessRoutine) 0x0000;

/* url: http://www.meraman.com/htmls/en/sinTableOld.html */
const INT16 AWG_SINEWAVE[AWG_SINE_WAVE_SIZE] =
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
 
 /* 
 0, 13, 26, 39, 51, 64, 77, 90, 103, 116, 129, 141, 154, 167, 180, 193, 206, 218,
 231, 244, 257, 269, 282, 295, 308, 320, 333, 346, 358, 371, 384, 396, 409, 422, 434,
 447, 459, 472, 484, 497, 509, 522, 534, 547, 559, 571, 584, 596, 608, 621, 633, 645,
 657, 669, 682, 694, 706, 718, 730, 742, 754, 766, 778, 790, 802, 813, 825, 837, 849,
 860, 872, 884, 895, 907, 918, 930, 941, 953, 964, 975, 987, 998, 1009, 1020, 1031, 1043,
 1054, 1065, 1076, 1086, 1097, 1108, 1119, 1130, 1140, 1151, 1162, 1172, 1183, 1193, 1204,
 1214, 1225, 1235, 1245, 1255, 1265, 1275, 1286, 1296, 1305, 1315, 1325, 1335, 1345, 1354,
 1364, 1374, 1383, 1393, 1402, 1411, 1421, 1430, 1439, 1448, 1457, 1466, 1475, 1484, 1493,
 1502, 1510, 1519, 1528, 1536, 1545, 1553, 1561, 1570, 1578, 1586, 1594, 1602, 1610, 1618,
 1626, 1634, 1642, 1649, 1657, 1664, 1672, 1679, 1687, 1694, 1701, 1708, 1715, 1722, 1729,
 1736, 1743, 1750, 1756, 1763, 1769, 1776, 1782, 1788, 1795, 1801, 1807, 1813, 1819, 1825,
 1831, 1836, 1842, 1848, 1853, 1859, 1864, 1869, 1874, 1880, 1885, 1890, 1895, 1899, 1904,
 1909, 1914, 1918, 1923, 1927, 1931, 1935, 1940, 1944, 1948, 1952, 1956, 1959, 1963, 1967,
 1970, 1974, 1977, 1980, 1984, 1987, 1990, 1993, 1996, 1999, 2001, 2004, 2007, 2009, 2012,
 2014, 2016, 2019, 2021, 2023, 2025, 2027, 2028, 2030, 2032, 2033, 2035, 2036, 2038, 2039,
 2040, 2041, 2042, 2043, 2044, 2045, 2045, 2046, 2047, 2047, 2047, 2048, 2048, 2048, 2048,
 2048, 2048, 2048, 2047, 2047, 2047, 2046, 2045, 2045, 2044, 2043, 2042, 2041, 2040, 2039,
 2038, 2036, 2035, 2033, 2032, 2030, 2028, 2027, 2025, 2023, 2021, 2019, 2016, 2014, 2012,
 2009, 2007, 2004, 2001, 1999, 1996, 1993, 1990, 1987, 1984, 1980, 1977, 1974, 1970, 1967,
 1963, 1959, 1956, 1952, 1948, 1944, 1940, 1935, 1931, 1927, 1923, 1918, 1914, 1909, 1904,
 1899, 1895, 1890, 1885, 1880, 1874, 1869, 1864, 1859, 1853, 1848, 1842, 1836, 1831, 1825,
 1819, 1813, 1807, 1801, 1795, 1788, 1782, 1776, 1769, 1763, 1756, 1750, 1743, 1736, 1729,
 1722, 1715, 1708, 1701, 1694, 1687, 1679, 1672, 1664, 1657, 1649, 1642, 1634, 1626, 1618,
 1610, 1602, 1594, 1586, 1578, 1570, 1561, 1553, 1545, 1536, 1528, 1519, 1510, 1502, 1493,
 1484, 1475, 1466, 1457, 1448, 1439, 1430, 1421, 1411, 1402, 1393, 1383, 1374, 1364, 1354,
 1345, 1335, 1325, 1315, 1305, 1296, 1286, 1275, 1265, 1255, 1245, 1235, 1225, 1214, 1204,
 1193, 1183, 1172, 1162, 1151, 1140, 1130, 1119, 1108, 1097, 1086, 1076, 1065, 1054, 1043,
 1031, 1020, 1009, 998, 987, 975, 964, 953, 941, 930, 918, 907, 895, 884, 872, 860, 849,
 837, 825, 813, 802, 790, 778, 766, 754, 742, 730, 718, 706, 694, 682, 669, 657, 645,
 633, 621, 608, 596, 584, 571, 559, 547, 534, 522, 509, 497, 484, 472, 459, 447, 434,
 422, 409, 396, 384, 371, 358, 346, 333, 320, 308, 295, 282, 269, 257, 244, 231, 218,
 206, 193, 180, 167, 154, 141, 129, 116, 103, 90, 77, 64, 51, 39, 26, 13, 0, -13, -26,
 -39, -51, -64, -77, -90, -103, -116, -129, -141, -154, -167, -180, -193, -206, -218,
 -231, -244, -257, -269, -282, -295, -308, -320, -333, -346, -358, -371, -384, -396,
 -409, -422, -434, -447, -459, -472, -484, -497, -509, -522, -534, -547, -559, -571,
 -584, -596, -608, -621, -633, -645, -657, -669, -682, -694, -706, -718, -730, -742,
 -754, -766, -778, -790, -802, -813, -825, -837, -849, -860, -872, -884, -895, -907,
 -918, -930, -941, -953, -964, -975, -987, -998, -1009, -1020, -1031, -1043, -1054,
 -1065, -1076, -1086, -1097, -1108, -1119, -1130, -1140, -1151, -1162, -1172, -1183,
 -1193, -1204, -1214, -1225, -1235, -1245, -1255, -1265, -1275, -1286, -1296, -1305,
 -1315, -1325, -1335, -1345, -1354, -1364, -1374, -1383, -1393, -1402, -1411, -1421,
 -1430, -1439, -1448, -1457, -1466, -1475, -1484, -1493, -1502, -1510, -1519, -1528,
 -1536, -1545, -1553, -1561, -1570, -1578, -1586, -1594, -1602, -1610, -1618, -1626,
 -1634, -1642, -1649, -1657, -1664, -1672, -1679, -1687, -1694, -1701, -1708, -1715,
 -1722, -1729, -1736, -1743, -1750, -1756, -1763, -1769, -1776, -1782, -1788, -1795,
 -1801, -1807, -1813, -1819, -1825, -1831, -1836, -1842, -1848, -1853, -1859, -1864,
 -1869, -1874, -1880, -1885, -1890, -1895, -1899, -1904, -1909, -1914, -1918, -1923,
 -1927, -1931, -1935, -1940, -1944, -1948, -1952, -1956, -1959, -1963, -1967, -1970,
 -1974, -1977, -1980, -1984, -1987, -1990, -1993, -1996, -1999, -2001, -2004, -2007,
 -2009, -2012, -2014, -2016, -2019, -2021, -2023, -2025, -2027, -2028, -2030, -2032,
 -2033, -2035, -2036, -2038, -2039, -2040, -2041, -2042, -2043, -2044, -2045, -2045,
 -2046, -2047, -2047, -2047, -2048, -2048, -2048, -2048, -2048, -2048, -2048, -2047,
 -2047, -2047, -2046, -2045, -2045, -2044, -2043, -2042, -2041, -2040, -2039, -2038,
 -2036, -2035, -2033, -2032, -2030, -2028, -2027, -2025, -2023, -2021, -2019, -2016,
 -2014, -2012, -2009, -2007, -2004, -2001, -1999, -1996, -1993, -1990, -1987, -1984,
 -1980, -1977, -1974, -1970, -1967, -1963, -1959, -1956, -1952, -1948, -1944, -1940,
 -1935, -1931, -1927, -1923, -1918, -1914, -1909, -1904, -1899, -1895, -1890, -1885,
 -1880, -1874, -1869, -1864, -1859, -1853, -1848, -1842, -1836, -1831, -1825, -1819,
 -1813, -1807, -1801, -1795, -1788, -1782, -1776, -1769, -1763, -1756, -1750, -1743,
 -1736, -1729, -1722, -1715, -1708, -1701, -1694, -1687, -1679, -1672, -1664, -1657,
 -1649, -1642, -1634, -1626, -1618, -1610, -1602, -1594, -1586, -1578, -1570, -1561,
 -1553, -1545, -1536, -1528, -1519, -1510, -1502, -1493, -1484, -1475, -1466, -1457,
 -1448, -1439, -1430, -1421, -1411, -1402, -1393, -1383, -1374, -1364, -1354, -1345,
 -1335, -1325, -1315, -1305, -1296, -1286, -1275, -1265, -1255, -1245, -1235, -1225,
 -1214, -1204, -1193, -1183, -1172, -1162, -1151, -1140, -1130, -1119, -1108, -1097,
 -1086, -1076, -1065, -1054, -1043, -1031, -1020, -1009, -998, -987, -975, -964, -953,
 -941, -930, -918, -907, -895, -884, -872, -860, -849, -837, -825, -813, -802, -790,
 -778, -766, -754, -742, -730, -718, -706, -694, -682, -669, -657, -645, -633, -621,
 -608, -596, -584, -571, -559, -547, -534, -522, -509, -497, -484, -472, -459, -447,
 -434, -422, -409, -396, -384, -371, -358, -346, -333, -320, -308, -295, -282, -269,
 -257, -244, -231, -218, -206, -193, -180, -167, -154, -141, -129, -116, -103, -90,
 -77, -64, -51, -39, -26, -13   
 */
};

const INT16 AWG_SINE_TABLE[256] = 
{
  0, 503, 1005, 1507, 2007, 2507, 3005,
  3501, 3995, 4487, 4976, 5462, 5945, 6424,
  6900, 7371, 7837, 8299, 8756, 9208, 9654,
  10095, 10529, 10957, 11378, 11793, 12200, 12600,
  12992, 13377, 13754, 14122, 14482, 14833, 15175,
  15508, 15831, 16145, 16450, 16744, 17028, 17303,
  17566, 17819, 18062, 18293, 18514, 18723, 18921,
  19108, 19283, 19446, 19598, 19738, 19866, 19982,
  20086, 20178, 20258, 20326, 20381, 20425, 20455,
  20474, 20480, 20474, 20455, 20425, 20381, 20326,
  20258, 20178, 20086, 19982, 19866, 19738, 19598,
  19446, 19283, 19108, 18921, 18723, 18514, 18293,
  18062, 17819, 17566, 17303, 17028, 16744, 16450,
  16145, 15831, 15508, 15175, 14833, 14482, 14122,
  13754, 13377, 12992, 12600, 12200, 11793, 11378,
  10957, 10529, 10095, 9654, 9208, 8756, 8299,
  7837, 7371, 6900, 6424, 5945, 5462, 4976,
  4487, 3995, 3501, 3005, 2507, 2007, 1507,
  1005, 503, 0, -503, -1005, -1507, -2007,
  -2507, -3005, -3501, -3995, -4487, -4976,
  -5462, -5945, -6424, -6900, -7371, -7837,
  -8299, -8756, -9208, -9654, -10095, -10529,
  -10957, -11378, -11793, -12200, -12600, -12992,
  -13377, -13754, -14122, -14482, -14833, -15175,
  -15508, -15831, -16145, -16450, -16744, -17028,
  -17303, -17566, -17819, -18062, -18293, -18514,
  -18723, -18921, -19108, -19283, -19446, -19598,
  -19738, -19866, -19982, -20086, -20178, -20258,
  -20326, -20381, -20425, -20455, -20474, -20480,
  -20474, -20455, -20425, -20381, -20326, -20258,
  -20178, -20086, -19982, -19866, -19738, -19598,
  -19446, -19283, -19108, -18921, -18723, -18514,
  -18293, -18062, -17819, -17566, -17303, -17028,
  -16744, -16450, -16145, -15831, -15508, -15175,
  -14833, -14482, -14122, -13754, -13377, -12992,
  -12600, -12200, -11793, -11378, -10957, -10529,
  -10095, -9654, -9208, -8756, -8299, -7837,
  -7371, -6900, -6424, -5945, -5462, -4976,
  -4487, -3995, -3501, -3005, -2507, -2007, 
  -1507, -1005, -503
};

static TAnalogChannel outputChannelNumberLookupTable[NB_OUTPUT_CHANNELS] = { ANALOG_OUTPUT_Ch1,
                                                                             ANALOG_OUTPUT_Ch2,
                                                                             ANALOG_OUTPUT_Ch3,
                                                                             ANALOG_OUTPUT_Ch4 };

void AWGRoutine(TTimerChannel channelNb);

float AWGGenerateAWGN(void);

void AWGOutAnalog(TAnalogChannel channelNb, INT16 analogValue)
{
  if (analogValue > DAC_MAXIMUM)
  {
  	analogValue = DAC_MAXIMUM;
  }
  else if (analogValue < DAC_MINIMUM)
  {
  	analogValue = DAC_MINIMUM;
  }
      
  Analog_Put(channelNb, analogValue);  
}

float AWGGenerateAWGN(void)
{
  /* generates additive white gaussian noise samples with zero mean and a standard deviation of 1 */ 
  float cache1 = 0.0f;
  float cache2 = 0.0f;
  float result = 0.0f;
  INT16 p = 1;

  while( p > 0 )
  {
    cache2 = (rand() / (float)RAND_MAX); /*  rand() function generates an
                                             integer between 0 and  RAND_MAX,
                                             which is defined in stdlib.h.
                                          */

    if (cache2 == 0 )
    {
      /* cache2 is >= (RAND_MAX / 2) */
      p = 1;
    }
    else
    {
      /* cache2 is < (RAND_MAX / 2) */
      p = -1;
    }

  }

  cache1 = cosf((2.0f * _M_PI) * rand() / (float)RAND_MAX);
  result = sqrtf(-2.0 * logf( cache2 ) ) * cache1;

  return result;        // return the generated random sample to the caller  
}

void AWGUpdateContext(TAWGEntry* const, TAWGEntryContext* const);

void AWGRoutine(TTimerChannel channelNb)
{
  static TAWGChannel channelNumberLookupTable[NB_AWG_CHANNELS] = { AWG_Ch1,
                                                                   AWG_Ch2 };
  UINT16 index = 0xFFFF, sampleIndex = 0xFFFF;
  INT16 analogValue = 0;
  
	Timer_ScheduleRoutine(channelNb, AWGRuntimeContext.routinePeriod); 
      
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
      	  
      	  sampleIndex = (UINT16)((AWG_SINE_WAVE_SIZE * (AWGChannelContext[index].time / 10)) / (AWGChannelContext[index].frequencyPeriod / 10));
      		
      		sampleIndex = sampleIndex % AWG_SINE_WAVE_SIZE;
				
					AWGChannelContext[index].voltage = AWG_SINEWAVE[sampleIndex] / AWGChannelContext[index].voltageScale;

      		analogValue = DAC_ZERO_VOLTAGE + AWGChannelContext[index].voltage;      		
      		      		
      		break;
    		case AWG_WAVEFORM_SQUARE:
      		if (AWGChannelContext[index].time > AWGChannelContext[index].halfFrequencyPeriod)
      		{
      			analogValue = DAC_ZERO_VOLTAGE + AWGChannelContext[index].amplitude;
      		}
      		else
      		{
      			analogValue = DAC_ZERO_VOLTAGE - AWGChannelContext[index].amplitude;
      		}
      		break;
    		case AWG_WAVEFORM_TRIANGLE:
					AWGChannelContext[index].voltage = (UINT16)((AWGChannelContext[index].amplitude * 4 * (AWGChannelContext[index].time / 10)) / (AWGChannelContext[index].frequencyPeriod / 10));
					
      		if (AWGChannelContext[index].time < AWGChannelContext[index].halfFrequencyPeriod)
      		{
      			analogValue = DAC_ZERO_VOLTAGE + (AWGChannelContext[index].amplitude - AWGChannelContext[index].voltage);
      		}
      		else
      		{
      			analogValue = DAC_ZERO_VOLTAGE + (AWGChannelContext[index].voltage - AWGChannelContext[index].amplitude * 3);
      		}
      		break;
    		case AWG_WAVEFORM_SAWTOOTH:
      		
      		if (AWGChannelContext[index].time == 0)
      		{
      		  /* hotfix: resolve that end frequency period peak */
        		analogValue = DAC_ZERO_VOLTAGE - AWGChannelContext[index].amplitude;
        		
        		analogValue -= AWG_Channel[index].offset;
        		
        		AWGOutAnalog(outputChannelNumberLookupTable[index], analogValue);
      		  
      		  AWGChannelContext[index].voltage = 0;      		  
      		  
      		}
      		else
      		{      		  
      		  AWGChannelContext[index].voltage = (UINT16)((AWGChannelContext[index].amplitude * 2 * (AWGChannelContext[index].time / 10)) / (AWGChannelContext[index].frequencyPeriod / 10));
      		}
      		      		      		
      		analogValue = DAC_ZERO_VOLTAGE + AWGChannelContext[index].amplitude - AWGChannelContext[index].voltage;
      		
      		break;
    		case AWG_WAVEFORM_NOISE:
    		  
    		  //analogValue = DAC_ZERO_VOLTAGE + AWGChannelContext[index].amplitude - (rand() % (AWGChannelContext[index].amplitude * 2));
    		  //analogValue = DAC_ZERO_VOLTAGE + AWGChannelContext[index].amplitude - (INT16)(AWGGenerateAWGN() * AWGChannelContext[index].amplitude);
    		  analogValue = DAC_ZERO_VOLTAGE + (INT16)(AWGGenerateAWGN() * AWGChannelContext[index].amplitude);
      		break;
    		case AWG_WAVEFORM_ARBITRARY:
      	  sampleIndex = (UINT16)((AWG_ARBITRARY_WAVE_SIZE * (AWGChannelContext[index].time / 10)) / (AWGChannelContext[index].frequencyPeriod / 10));
      		
      		sampleIndex = sampleIndex % AWG_ARBITRARY_WAVE_SIZE;
				
					AWGChannelContext[index].voltage = (UINT16)(AWG_ARBITRARY_WAVE[sampleIndex] / AWGChannelContext[index].voltageScale);

      		analogValue = DAC_ZERO_VOLTAGE + AWGChannelContext[index].voltage;
      		break;
    		case AWG_WAVEFORM_DC:
    		default:
      		break;
  		}
  		
  		analogValue -= AWG_Channel[index].offset;
  		
  		AWGOutAnalog(outputChannelNumberLookupTable[index], analogValue);
      
			AWGChannelContext[index].time += AWGChannelContext[index].timeStep;
			
	  	if (channelPostProcessRoutinePtr)
  		{
    		channelPostProcessRoutinePtr(channelNumberLookupTable[index]);
  		} 
  		   		
    }
  }
        
};

void AWGUpdateContext(TAWGEntry* const entryPtr, TAWGEntryContext* const contextPtr)
{
  UINT32 frequency = 0;

  if (entryPtr && contextPtr)
  {
  
	  frequency = entryPtr->frequency % 256; /* XX.F*/

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
    
    frequency = MATH_1_MEGA * 10 / frequency; /* in 1us unit */
    
    if (contextPtr->frequencyPeriod != frequency)
    {
    	contextPtr->time = 0;
    }
    
    contextPtr->frequencyPeriod = frequency;
    
    contextPtr->halfFrequencyPeriod = contextPtr->frequencyPeriod / 2;
    contextPtr->timeStep = AWG_ANALOG_OUTPUT_SAMPLING_RATE;
   
    contextPtr->waveformType = entryPtr->waveformType;
    contextPtr->amplitude = entryPtr->amplitude;
    contextPtr->offset = entryPtr->offset;

    /* make sure it is not divided by zero */
    if (contextPtr->amplitude > 0)
    {      
      contextPtr->voltageScale = 20480 / contextPtr->amplitude; /* use 20480 to increase accuracy */
    }
    else
    {
      contextPtr->voltageScale = 0;
    }
            
  }  
}

void AWG_Update(TAWGChannel channelNb)
{
  UINT16 index = 0xFFFF;
 
  switch(channelNb)
  {
    case AWG_Ch1:
      index = 0;
      break;
    case AWG_Ch2:
      index = 1;
      break;
    case AWG_Ch3:
      index = 2;
      break;
    case AWG_Ch4:
      index = 3;
      break;      
    default:
      return;
      break;
  }
  
  if (AWGChannelContext[index].isRunning)
  {
    AWGUpdateContext(&AWG_Channel[index], &AWGChannelContext[index]);  
  }
}

void AWG_Enable(TAWGChannel channelNb, BOOL enable)
{
  UINT16 index = 0xFFFF;
  
  switch(channelNb)
  {
    case AWG_Ch1:
      index = 0;
      break;
    case AWG_Ch2:
      index = 1;
      break;
    case AWG_Ch3:
      index = 2;
      break;
    case AWG_Ch4:
      index = 3;
      break;      
    default:
      return;
      break;
  }
  
  if (AWG_Channel[index].isEnabled && enable)
  {    
    AWGUpdateContext(&AWG_Channel[index], &AWGChannelContext[index]);  
    AWGChannelContext[index].isRunning = bTRUE;  
  }
  else
  {
    AWGChannelContext[index].isRunning = bFALSE;
    Analog_Put(outputChannelNumberLookupTable[index], DAC_ZERO_VOLTAGE);  
  }
}

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
    /* hotfix for analog sampling issue */
    Analog_Output[index].OldValue.l = Analog_Output[index].Value.l;  
  }  

  AWG_ResetArbitraryWave();

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

void AWG_ApplyArbitraryPhasor(UINT8 harmonicNb, UINT16 magnitude, INT16 angle)
{
  UINT32 sineIndexOffset = 0, sineIndex = 0;
  UINT32 index = 0;
  
  sineIndexOffset = (angle + 90) * 25 / 9;
  
  for (index = 0; index < AWG_ARBITRARY_WAVE_SIZE; ++index)
  {
    sineIndex = (sineIndexOffset + (index * harmonicNb * AWG_SINE_WAVE_SIZE / AWG_ARBITRARY_WAVE_SIZE)) % AWG_SINE_WAVE_SIZE;   
    AWG_ARBITRARY_WAVE[index] = AWG_ARBITRARY_WAVE[index] + (0 - AWG_SINEWAVE[sineIndex] / (INT16)magnitude);
    __RESET_WATCHDOG();
  }
}

void AWG_ResetArbitraryWave(void)
{
  UINT16 index = 0;
  
  for (index = 0; index < AWG_ARBITRARY_WAVE_SIZE; ++index)
  {
    AWG_ARBITRARY_WAVE[index] = AWG_ARBITRARY_WAVE_INITIAL_VOLTAGE; /* match our scale */
  }
}