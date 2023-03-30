#ifndef IN_OUT_H_
#define IN_OUT_H_

#include "utils.h"

/**
 Administracion de componentes de entradas y salidas de la tarjeta.
*/

//indicadores LED activos en alto
#define IO_LED_STATUS LATBbits.LATB2
#define IO_LED_ERROR LATBbits.LATB3

//entradas activas en bajo para interruptores, 5V/10kohm=500uA
#define IO_ENT_SW_MAP PORTDbits.RD1
#define IO_ENT_SW_FLOOR_PEDAL PORTDbits.RD0
#define IO_ENT_SW_INST PORTCbits.RC3
#define IO_ENT_SW_LATL_CSL PORTCbits.RC2
#define IO_ENT_SW_READING PORTDbits.RD2
#define IO_ENT_SW_PANEL PORTCbits.RC1

// macros que definen las acciones de los elementos
#define C1I_FLIDECLT_SWI_FLOORPED_FLOPED 0x01860101
#define C1I_FLIDECLT_SWI_FLOORPED_OFF 0x01860102
#define C1I_FLIDECLT_SWI_FLOORPED_ANTIREBOTE 0x01870103
#define C1I_FLIDECLT_SWI_FLOORPED_STAT 0x01870104
#define C1I_FLIDECLT_SWI_MAP_MAP 0x01860201
#define C1I_FLIDECLT_SWI_MAP_OFF 0x01860202
#define C1I_FLIDECLT_SWI_MAP_ANTIREBOTE 0x01870203
#define C1I_FLIDECLT_SWI_MAP_STAT 0x01870204
#define C1I_FLIDECLT_SWI_READING_READ 0x01860301
#define C1I_FLIDECLT_SWI_READING_OFF 0x01860302
#define C1I_FLIDECLT_SWI_READING_ANTIREBOTE 0x01870303
#define C1I_FLIDECLT_SWI_READING_STAT 0x01870304
#define C1I_FLIDECLT_POT_INST_OFF 0x01860401
#define C1I_FLIDECLT_POT_INST_nOFF 0x01860402
#define C1I_FLIDECLT_POT_INST_VALUE 0x01860403
#define C1I_FLIDECLT_POT_INST_ANTIREBOTE 0x01870404
#define C1I_FLIDECLT_POT_INST_TIMER 0x01870405
#define C1I_FLIDECLT_POT_INST_SWITCHSTAT 0x01870406
#define C1I_FLIDECLT_POT_LATLCSL_OFF 0x01860501
#define C1I_FLIDECLT_POT_LATLCSL_nOFF 0x01860502
#define C1I_FLIDECLT_POT_LATLCSL_VALUE 0x01860503
#define C1I_FLIDECLT_POT_LATLCSL_ANTIREBOTE 0x01870504
#define C1I_FLIDECLT_POT_LATLCSL_TIMER 0x01870505
#define C1I_FLIDECLT_POT_LATLCSL_SWITCHSTAT 0x01870506
#define C1I_FLIDECLT_POT_PANEL_OFF 0x01860601
#define C1I_FLIDECLT_POT_PANEL_nOFF 0x01860602
#define C1I_FLIDECLT_POT_PANEL_VALUE 0x01860603
#define C1I_FLIDECLT_POT_PANEL_ANTIREBOTE 0x01870604
#define C1I_FLIDECLT_POT_PANEL_TIMER 0x01870605
#define C1I_FLIDECLT_POT_PANEL_SWITCHSTAT 0x01870606
#define C1I_FLIDECLT_CFG_CONFIG_PING 0x01870801
#define C1I_FLIDECLT_CFG_CONFIG_PINGRESP 0x01860802
#define LT_CM1_INST_LAMPS 0x01F50001
#define LT_CM1_INST_CE 0x01F50002

//lista completa de paneles iluminados
#define LT_CM1_INST_LAMPS 0x01F50001
#define LT_CM1_INST_CE 0x01F50002
#define LT_CM1_LATL_LAMPS 0x01F60001
#define LT_CM1_LATL_CE 0x01F60002
#define LT_CM1_PANEL_LAMPS 0x01F70001
#define LT_CM1_PANEL_CE 0x01F70002
#define LT_CM2_INST_LAMPS 0x01F80001
#define LT_CM2_INST_CE 0x01F80002
#define LT_CM2_LATL_LAMPS 0x01F90001
#define LT_CM2_LATL_CE 0x01F90002
#define LT_CM2_PANEL_LAMPS 0x01FA0001
#define LT_CM2_PANEL_CE 0x01FA0002
#define LT_UPPER_FWD_LAMPS 0x01FB0001
#define LT_UPPER_FWD_CE 0x01FB0002
#define LT_UPPER_RWD_LAMPS 0x01FC0001
#define LT_UPPER_RWD_CE 0x01FC0002
#define LT_CENTRAL_INST_LAMPS 0x01FD0001
#define LT_CENTRAL_INST_CE 0x01FD0002
#define LT_CENTRAL_CSL_LAMPS 0x01FE0001
#define LT_CENTRAL_CSL_CE 0x01FE0002

//GEN-BUS
#define C1I_FLIDECLT_MSW_GENBUS1_ON 0x01870701
#define C1I_FLIDECLT_MSW_GENBUS1_OFF 0x01870702

#define IO_CE_POWER(x) PWM5_Write(x)

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
