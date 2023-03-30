/**
 Administracion de componentes de entradas y salidas de la tarjeta.
*/

// indicadores LED activos en alto
#define IO_LED1 LATBbits.LATB7
#define IO_LED2 LATBbits.LATB6

//interruptores activos en bajo, 5V/10kohm=500uA
#define IO_ENT1 PORTAbits.RA0
#define IO_ENT2 PORTAbits.RA1
#define IO_ENT3 PORTAbits.RA2
#define IO_ENT4 PORTAbits.RA3
#define IO_ENT5 PORTAbits.RA5
#define IO_ENT6 PORTEbits.RE0
#define IO_ENT7 PORTEbits.RE1
#define IO_ENT8 PORTEbits.RE2
#define IO_ENT9 PORTCbits.RC3
#define IO_ENT10 PORTCbits.RC2
#define IO_ENT11 PORTCbits.RC1
#define IO_ENT12 PORTCbits.RC0

//salidas a colector abierto, consumo en base 5V/2.7kohm=1.85mA
#define IO_SAL1 LATBbits.LATB1
#define IO_SAL2 LATBbits.LATB0
#define IO_SAL3 LATDbits.LATD7
#define IO_SAL4 LATDbits.LATD6
#define IO_SAL5 LATDbits.LATD5
#define IO_SAL6 LATDbits.LATD4
#define IO_SAL7 LATCbits.LATC5
#define IO_SAL8 LATCbits.LATC4
#define IO_SAL9 LATDbits.LATD3
#define IO_SAL10 LATDbits.LATD2
#define IO_SAL11 LATDbits.LATD1
#define IO_SAL12 LATDbits.LATD0

// macros que definen las acciones de los elementos
#define FLI_CM2WHEEL_BUT_GA_PUSH 0x01440101
#define FLI_CM2WHEEL_BUT_GA_nPUSH 0x01440102
#define FLI_CM2WHEEL_BUT_GA_PUSHSTAT 0x01450103
#define FLI_CM2WHEEL_BUT_GA_ANTIREBOTE 0x01450104
#define FLI_CM2WHEEL_BUT_AP_PUSH 0x01440201
#define FLI_CM2WHEEL_BUT_AP_nPUSH 0x01440202
#define FLI_CM2WHEEL_BUT_AP_PUSHSTAT 0x01450203
#define FLI_CM2WHEEL_BUT_AP_ANTIREBOTE 0x01450204
#define FLI_CM2WHEEL_BUT_SYN_PUSH 0x01440301
#define FLI_CM2WHEEL_BUT_SYN_nPUSH 0x01440302
#define FLI_CM2WHEEL_BUT_SYN_PUSHSTAT 0x01450303
#define FLI_CM2WHEEL_BUT_SYN_ANTIREBOTE 0x01450304
#define FLI_CM2WHEEL_BUT_PTT_FWD 0x01440401
#define FLI_CM2WHEEL_BUT_PTT_OFF 0x01440402
#define FLI_CM2WHEEL_BUT_PTT_BWD 0x01440403
#define FLI_CM2WHEEL_BUT_PTT_STATUS 0x01450404
#define FLI_CM2WHEEL_BUT_PTT_ANTIREBOTE 0x01450405
#define FLI_CM2WHEEL_SEL_TRIM_RIGHT 0x01440501
#define FLI_CM2WHEEL_SEL_TRIM_LEFT 0x01440502
#define FLI_CM2WHEEL_SEL_TRIM_UP 0x01440503
#define FLI_CM2WHEEL_SEL_TRIM_DOWN 0x01440504
#define FLI_CM2WHEEL_SEL_TRIM_CENTER 0x01440505
#define FLI_CM2WHEEL_SEL_TRIM_STATUS 0x01450506
#define FLI_CM2WHEEL_SEL_TRIM_ANTIREBOTE 0x01450507
#define FLI_CM2WHEEL_LIN_MAPS_ON 0x01450601
#define FLI_CM2WHEEL_LIN_MAPS_nON 0x01450602
#define FLI_CM2WHEEL_CFG_CONFIG_PING 0x01450701
#define FLI_CM2WHEEL_CFG_CONFIG_PINGRESP 0x01440702
#define LT_CM2_INST_LAMPS 0x01F80001
#define LT_CM2_INST_CE 0x01F80002

#define IO_LAMPS_POWER(x) PWM3_Write(x)
//Luz de Mapas
#define IO_CE_POWER(x) PWM4_Write(x)

//Botón SYN
#define IO_ENT_SWI_SYN_ON   IO_ENT9

//Tri-estado PJT
#define IO_ENT_BUT_PTT_DIM  IO_ENT8     //FWD
#define IO_ENT_BUT_PTT_BRT  IO_ENT7     //BWD

//Botón GA
#define IO_ENT_GA_ON        IO_ENT6

//Botón AP
#define IO_ENT_AP_ON        IO_ENT5

//Roll Trim
#define IO_TIMM_RIGHT_ON    IO_ENT4
#define IO_ENT_TIMM_LEFT_ON IO_ENT3
#define IO_ENT_TIMM_UP_ON   IO_ENT2
#define IO_ENT_TIMM_DOWN_ON IO_ENT1



/**
Timers por software.
*/
struct TimerStructure {
    UInt8 Counter; //Counter=0 => rebote terminado
    UInt8 CounterProg; //asignado desde CAN u otro
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
  Entradas de selectores o interruptores de 3 o mas estados.
*/
union SelectorUnionBUTPTT {
    UInt8 UI8;
    struct {
        unsigned BRT:1;
        unsigned DIM:1;
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
        union SelectorUnionBUTPTT Inputs;
        UInt8 InputsBack;
        UInt8 DebounceTimerCounter;
        UInt8 DebounceTimerProg;
    } BUT_PTT;
    
    struct SwitchStructure SWI_GA;
    struct SwitchStructure SWI_AP;
    struct SwitchStructure TIMM_RIGHT;
    struct SwitchStructure TIMM_LEFT;
    struct SwitchStructure TIMM_UP;
    struct SwitchStructure TIMM_DOWN;
    struct SwitchStructure SWI_SYN;
    
};

void IO_Init(void);
void IO_TimerBaseOverflow(void);
