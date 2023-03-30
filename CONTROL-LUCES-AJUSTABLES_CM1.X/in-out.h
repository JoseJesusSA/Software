#ifndef IN_OUT_H_
#define IN_OUT_H_

#include "utils.h"

/**
 Administracion de componentes de entradas y salidas de la tarjeta.
*/

//indicadores LED activos en alto
#define IO_LED_STATUS LATBbits.LATB4
#define IO_LED_ERROR LATBbits.LATB5
#define PWMDIM LATCbits.LATC2

#define OVE_LUZAJUST1_CFG_CONFIG_PING 0x01B90101
#define OVE_LUZAJUST1_CFG_CONFIG_PINGRESP 0x01B80102
//Macro Luz DOME GENERAL
#define OVE_INTLT_SWI_DOME_ON 0x01820701
#define OVE_INTLT_SWI_DOME_OFF 0x01820702
//Macro Luz READING CM-2
#define C1I_FLIDECLT_SWI_READING_READ 0x01860301
#define C1I_FLIDECLT_SWI_READING_OFF 0x01860302

#define IO_LAMP_INST_POWER_2(x) PWM2_Write(x)
#define IO_LAMP_INST_POWER_3(x) PWM3_Write(x)

/**
Timers por software.
*/
struct TimerStructure {
    UInt8 Counter; //Counter=0 => rebote terminado
    UInt8 CounterProg; //asignado desde CAN u otro
};

/**
Variables de soporte para el formato de potenciometros.
*/
struct PotStructure {
    UInt16UInt8 Value;
    UInt16UInt8 ParamMin;
    UInt16UInt8 ParamMax;
    //variables relacionadas al periodo de transmision
    UInt8 Counter; //Counter=0 = periodo terminado
    UInt8 CounterProg; //asignado desde CAN
};

/**
  Estado de interruptores simples.
*/
union SwitchUnion {
    UInt8 UI8;
    struct {
        unsigned Input:1; //bit0
        unsigned InputBack:1;
    } Bits;
};

/**
  Variables para interruptores simples de 2 posiciones.
  \note Para elementos con 3 o mas posiciones usar estructuras con una union UIntXBit
  de acuerdo a la cantidad de posiciones necesarias. Un UIntX para el respaldo de entradas y
  un solo DebounceTimerCounter y DebounceTimerProg.
*/
struct SwitchStructure {
    union SwitchUnion Switch;
    UInt8 DebounceTimerCounter;
    UInt8 DebounceTimerProg;
};

/*
Datos que conforman los modulos de entradas/salidas electricas o para interfaz
de usuario.
*/
struct IOStructure {
    struct TimerStructure LEDListTimer;

    struct {
        struct SwitchStructure Inst;
        struct SwitchStructure LatlCsl;
        struct SwitchStructure Panel;
    } PotSwitchs;

    struct {
        struct PotStructure Inst;
        struct PotStructure LatlCsl;
        struct PotStructure Panel;
        UInt8 Channel;
        UInt8 MsgCANState;
        UInt8 MsgCANFlag;
    } Pots;

    struct {
        struct SwitchStructure FloorPedal;
        struct SwitchStructure Map;
        struct SwitchStructure Reading;
    } Switches;
};

void IO_Init(void);
void IO_Set(UInt8 output, UInt8 status);
void IO_TimerBaseOverflow(void);
void IO_PotsListUpdate(void);

#endif // IN_OUT_H_
