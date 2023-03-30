#ifndef UTILS_H
#define UTILS_H

/**
 \file Utilerias de software.
*/

/**
 \page UTILS
 Esta seccion contiene rutinas que solo funcionan a nivel de software.
 Por ejemplo rutinas para cambiar entre datos binarios, BCD, ASCII, etc.
 Tambien contiene las definiciones para nuestros tipos de datos especiales.
 Es una de las secciones que puede permanecer casi sin cambios e
 independiente del tipo de aplicacion o hardware que se utilice.
*/

/**
 Siempre usar TRUE o FALSE para datos booleanos.
*/
#ifndef _BOOL
typedef enum _BOOL { FALSE = 0, TRUE } BOOL;    /* Undefined size */
#define _BOOL _BOOL
#endif

/**
 Usar nombres cortos UInt8, UInt16, Int24, Real32, etc.
*/
typedef char Int8;
typedef int Int16;
typedef short long Int24;
typedef unsigned char UInt8;
typedef unsigned int UInt16;
typedef unsigned short long UInt24;
typedef unsigned long UInt32;
typedef float Real32;
typedef union UInt32UInt8 UInt32UInt8;
typedef union UInt24UInt8 UInt24UInt8;
typedef union UInt16UInt8 UInt16UInt8;
typedef union UInt8Bit UInt8Bit;
typedef const rom char ROMInt8;

/**
 Union de 4 bytes.
 Util para evitar corrimientos cuando se lee un byte que necesita
 acupar el espacio en una variable de mas de un byte.
*/
union UInt32UInt8 {
    UInt32 UI32;
    UInt8 UI8[4];
};

/**
 Union de 3 bytes.
 Util para evitar corrimientos cuando se lee un byte que necesita
 acupar el espacio en una variable de mas de un byte.
*/
union UInt24UInt8 {
    UInt24 UI24;
    UInt8 UI8[3];
};

/**
 Union de 2 bytes.
 Util para evitar corrimientos cuando se lee un byte que necesita
 ocupar el espacio en una variable de mas de un byte.
*/
union UInt16UInt8 {
    UInt16 UI16;
    UInt8 UI8[2];
};

/**
  Union de 1 byte y 8 bits.
*/
union UInt8Bit {
    UInt8 UI8;
    struct{
        unsigned B0:1;
        unsigned B1:1;
        unsigned B2:1;
        unsigned B3:1;
        unsigned B4:1;
        unsigned B5:1;
        unsigned B6:1;
        unsigned B7:1;
    } Bits;
};


/**
Timers virtuales controlados por software.
Levantan una bandera cuando pasan de 0xf...f a 0
(por ejemplo de 255 a 0).
Estos timers son usados para eventos de tiempo que no sean criticos
pero necesarios.
\todo Variable CountProg. Eliminar banderas y Exec?
*/
struct TimerOldStructure {
    UInt8 Count; //se incrementa de uno en uno
    struct{
        unsigned Flag:1; // don't clean it up on Exec()
        unsigned Enable:1;
    } Bits;
    void (*Exec)(void);
};


UInt24 Utils_BinToBCD(UInt24 binaryData);
UInt8 Utils_BCDToASCII(UInt8 bcd);
UInt8 Utils_SevenCodeToASCII(UInt8 sevenSegmentsCode);
UInt8 Utils_BCDByteToBin(UInt8 bcd);
UInt8 Utils_GetChecksum(UInt8 data[], UInt8 dataSize);
UInt8 Utils_VerifyChecksum(UInt8 data[], UInt8 dataSize);
void Timer0_Exec(void); // \todo ELIMINAR DE AQUIĄĄĄĄ
void Timer0_Init(UInt8 time);
void Timer0_Start(void);
void Timer0_Stop(void);
void Timer0_Wait(void);
void Timer0_Interrupt(void);


#endif // UTILS_H
