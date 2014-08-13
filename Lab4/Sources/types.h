// ----------------------------------------
// Filename: types.h
// Description: Declares new type definitions
// Author: PMcL
// Date: 29-Mar-07

#ifndef TYPES_H
#define TYPES_H

// Signed types
typedef char INT8;
typedef int  INT16;
typedef long INT32;

// Unsigned types
typedef unsigned char UINT8;
typedef unsigned int  UINT16;
typedef unsigned long UINT32;

// Unions to efficiently access hi and lo parts of integers and words
typedef union
{
  INT16 l;
  struct
  {
    INT8 Hi;
    INT8 Lo;
  } s;
} TINT16;

typedef union
{
  UINT16 l;
  struct
  {
    UINT8 Hi;
    UINT8 Lo;
  } s;
} TUINT16;

// Union to efficiently access hi and lo parts of a long integer
typedef union
{
  UINT32 l;
  struct
  {
    UINT16 Hi;
    UINT16 Lo;
  } s;
} TUINT32;

// Macros for critical sections
#define OldEnterCritical() { asm pshc; asm sei; asm leas 1,sp; }
#define OldExitCritical()  { asm leas -1,sp; asm pulc; }

#define EnterCritical() { asm tfr ccr,a; asm staa savedCCR; asm sei; }
#define ExitCritical()  { asm ldaa savedCCR; asm tfr a,ccr; }

// Boolean definition that includes type and value
typedef enum
{
  bFALSE = 0,
  bTRUE = 1
} BOOL;

#endif