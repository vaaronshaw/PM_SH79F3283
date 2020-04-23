#ifndef _DEFINES_H
#define _DEFINES_H

//#include "mcu/base_types.h"
/* exact-width unsigned integer types */
typedef unsigned char       uchar;
typedef signed char         schar;
typedef unsigned int        uint;
typedef signed int          sint;

typedef unsigned long       ulong;
typedef signed long         slong;

typedef unsigned char       BOOL;   ///< Standard library type

#define DEF_SET(VAR1,VAR2)  ( (VAR1)|=(VAR2) )
#define DEF_RES(VAR1,VAR2)  ( (VAR1)&=~(VAR2) )
#define DEF_TEST(VAR1,VAR2) ( (VAR1)&(VAR2))

#ifndef FALSE
#define FALSE          ((BOOL)0)       //!< logical FALSE
#endif

#ifndef TRUE
#define TRUE           ((BOOL)1)       //!< logical TRUE
#endif

#ifndef OFF
#define OFF          ((BOOL)0)       //!< logical FALSE
#endif

#ifndef ON
#define ON           ((BOOL)1)       //!< logical TRUE
#endif

#define NIBBLE_SIZE      ((uchar)0x04U)      //!< Number of bits in a nibble
#define BYTE_SIZE        ((uchar)0x08U)      //!< Number of bits in a byte

#define INVALID		0xFF
#define VALID		1



#ifndef NULL
#define NULL ((void *)0)
#endif // !NULL








#define __LITTLE_ENDIAN_ADAPTATION	1 //notice this

/* DEFINITIONS ****************************************************************************************/
#ifdef __BIG_ENDIAN_ADAPTATION
#define HIGH_ADR_OFFSET  0 ///< Address offset for high byte/word (big endian)
#define LOW_ADR_OFFSET   1 ///< Address offset for low byte/word (big endian)
#else
#if   __LITTLE_ENDIAN_ADAPTATION
#define HIGH_ADR_OFFSET  1 ///< Address offset for high byte/word (little endian)
#define LOW_ADR_OFFSET   0 ///< Address offset for low byte/word (litle endian)
#endif
#endif
#define HSUP_CHECK_SIZE(hsup_var, hsup_size)  ((1/(int)(sizeof(hsup_var)==(hsup_size)))-1)
#define HSUP_HIGH_BYTE(word) ( ((uchar *)((void *)&(word)))[HIGH_ADR_OFFSET + HSUP_CHECK_SIZE(word, 2)] ) /* PRQA S 310, 3198 */
#define HSUP_LOW_BYTE(word)  ( ((uchar *)((void *)&(word)))[LOW_ADR_OFFSET + HSUP_CHECK_SIZE(word, 2)] ) /* PRQA S 310, 3198 */
#define HSUP_uiProcToBigEndian(uiValue)  ((uint)(uiValue<<8)|(uiValue>>8))
//#define HSUP_ulProcToBigEndian32bits(ulValue)  ((ulValue<<24)|((ulValue&0xFF00)<<8)|((ulValue&0xFF0000)>>8)|(ulValue>>24))
//#define HSUP_ulBigLittleEndianSwitch32bits(ulValue)  ((ulValue<<24)|((ulValue&0xFF00)<<8)|((ulValue&0xFF0000)>>8)|(ulValue>>24))


#define MK_BIT(BitType,BitPos) ((BitType)(((BitType)(1))<<(BitPos)))

#define BIT_0  MK_BIT(uchar, 0)  //!< Bit  0
#define BIT_1  MK_BIT(uchar, 1)  //!< Bit  1
#define BIT_2  MK_BIT(uchar, 2)  //!< Bit  2
#define BIT_3  MK_BIT(uchar, 3)  //!< Bit  3
#define BIT_4  MK_BIT(uchar, 4)  //!< Bit  4
#define BIT_5  MK_BIT(uchar, 5)  //!< Bit  5
#define BIT_6  MK_BIT(uchar, 6)  //!< Bit  6
#define BIT_7  MK_BIT(uchar, 7)  //!< Bit  7
#define BIT_8  MK_BIT(uint,  8)  //!< Bit  8
#define BIT_9  MK_BIT(uint,  9)  //!< Bit  9
#define BIT_10 MK_BIT(uint, 10)  //!< Bit 10
#define BIT_11 MK_BIT(uint, 11)  //!< Bit 11
#define BIT_12 MK_BIT(uint, 12)  //!< Bit 12
#define BIT_13 MK_BIT(uint, 13)  //!< Bit 13
#define BIT_14 MK_BIT(uint, 14)  //!< Bit 14
#define BIT_15 MK_BIT(uint, 15)  //!< Bit 15
#define BIT_16 MK_BIT(ulong,16)  //!< Bit 16
#define BIT_17 MK_BIT(ulong,17)  //!< Bit 17
#define BIT_18 MK_BIT(ulong,18)  //!< Bit 18
#define BIT_19 MK_BIT(ulong,19)  //!< Bit 19
#define BIT_20 MK_BIT(ulong,20)  //!< Bit 20
#define BIT_21 MK_BIT(ulong,21)  //!< Bit 21
#define BIT_22 MK_BIT(ulong,22)  //!< Bit 22
#define BIT_23 MK_BIT(ulong,23)  //!< Bit 23
#define BIT_24 MK_BIT(ulong,24)  //!< Bit 24
#define BIT_25 MK_BIT(ulong,25)  //!< Bit 25
#define BIT_26 MK_BIT(ulong,26)  //!< Bit 26
#define BIT_27 MK_BIT(ulong,27)  //!< Bit 27
#define BIT_28 MK_BIT(ulong,28)  //!< Bit 28
#define BIT_29 MK_BIT(ulong,29)  //!< Bit 29
#define BIT_30 MK_BIT(ulong,30)  //!< Bit 30
#define BIT_31 MK_BIT(ulong,31)  //!< Bit 31

#endif




