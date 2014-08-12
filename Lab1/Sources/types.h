/**
 * \file types.h
 * \brief Declares new type definitions
 * \author Peter McLean
 * \date 03-April-2006
 */
#ifndef TYPES_H
#define TYPES_H

/**
 * \brief Signed 8 bits integer
 */
typedef char INT8;
/**
 * \brief Signed 16 bits integer
 */
typedef int INT16;
/**
 * \brief Signed 32 bits integer
 */
typedef long INT32;

/**
 * \brief Unsigned 8 bits integer
 */
typedef unsigned char UINT8;
/**
 * \brief Unsigned 16 bits integer
 */
typedef unsigned int UINT16;
/**
 * \brief Unsigned 32 bits integer
 */
typedef unsigned long UINT32;

/**
 * \brief Union to efficiently access hi and lo parts of a signed 16 bits integer
 */
typedef union
{
  INT16 l;
  struct
  {
    INT8 Hi;
    INT8 Lo;
  } s;
} TINT16;

/**
 * \brief Union to efficiently access hi and lo parts of a unsigned 16 bits integer
 */
typedef union
{
  UINT16 l;
  struct
  {
    UINT8 Hi;
    UINT8 Lo;
  } s;
} TUINT16;

/**
 * \brief Union to efficiently access hi and lo parts of a unsigned 32 bits integer
 */
typedef union
{
  UINT32 l;
  struct
  {
    UINT16 Hi;
    UINT16 Lo;
  } s;
} TUINT32;

/** 
 * \brief Union to efficiently access individual bytes of a float
 */
typedef union
{
  float d;
  struct
  {
    TUINT16 dHi;
    TUINT16 dLo;
  } dParts;
} TFloat;

/**
 * \brief Macros for enter critical section
 */
#define EnterCritical() { asm pshc; asm sei; asm leas 1,sp; }
/**
 * \brief Macros for exit critical section
 */
#define ExitCritical()  { asm leas -1,sp; asm pulc; }

/**
 * \brief Boolean definition that includes type and value
 */
typedef enum
{
  bFALSE = 0,
  bTRUE = 1
} BOOL;

#endif