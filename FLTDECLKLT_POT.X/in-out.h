/**
 Administracion de componentes de entradas y salidas de la tarjeta.
*/

// indicadores LED activos en alto
#define IO_LED2 LATBbits.LATB4
#define IO_LED1 LATBbits.LATB5

//interruptores activos en bajo, 5V/10kohm=500uA
#define IO_swFWD PORTCbits.RC4
#define IO_swRWD PORTCbits.RC5
#define IO_swCSL PORTDbits.RD2
#define IO_swINST PORTDbits.RD3


//salidas a colector abierto, consumo en base 5V/2.7kohm=1.85mA


// macros que definen las acciones de los elementos
#define OVE_FLTDECKLT_POT_FWD_OFF 0x01720101
#define OVE_FLTDECKLT_POT_FWD_nOFF 0x01720102
#define OVE_FLTDECKLT_POT_FWD_VALUE 0x01720103
#define OVE_FLTDECKLT_POT_FWD_ANTIREBOTE 0x01730104
#define OVE_FLTDECKLT_POT_FWD_TIMER 0x01730105
#define OVE_FLTDECKLT_POT_FWD_SWITCHSTAT 0x01730106
#define OVE_FLTDECKLT_POT_RWD_OFF 0x01720201
#define OVE_FLTDECKLT_POT_RWD_nOFF 0x01720202
#define OVE_FLTDECKLT_POT_RWD_VALUE 0x01720203
#define OVE_FLTDECKLT_POT_RWD_ANTIREBOTE 0x01730204
#define OVE_FLTDECKLT_POT_RWD_TIMER 0x01730205
#define OVE_FLTDECKLT_POT_RWD_SWITCHSTAT 0x01730206
#define OVE_FLTDECKLT_POT_INST_OFF 0x01720301
#define OVE_FLTDECKLT_POT_INST_nOFF 0x01720302
#define OVE_FLTDECKLT_POT_INST_VALUE 0x01720303
#define OVE_FLTDECKLT_POT_INST_ANTIREBOTE 0x01730304
#define OVE_FLTDECKLT_POT_INST_TIMER 0x01730305
#define OVE_FLTDECKLT_POT_INST_SWITCHSTAT 0x01730306
#define OVE_FLTDECKLT_POT_CSL_OFF 0x01720401
#define OVE_FLTDECKLT_POT_CSL_nOFF 0x01720402
#define OVE_FLTDECKLT_POT_CSL_VALUE 0x01720403
#define OVE_FLTDECKLT_POT_CSL_ANTIREBOTE 0x01730404
#define OVE_FLTDECKLT_POT_CSL_TIMER 0x01730405
#define OVE_FLTDECKLT_POT_CSL_SWITCHSTAT 0x01730406
#define OVE_FLTDECKLT_CFG_CONFIG_PING 0x01730501
#define OVE_FLTDECKLT_CFG_CONFIG_PINGRESP 0x01720502
#define LT_UPPER_FWD_LAMPS 0x01FB0001
#define LT_UPPER_FWD_CE 0x01FB0002

#define IO_CE_POWER(x) PWM1_Write(x)
#define IO_LAMPS_POWER(x) PWM3_Write(x)

#define IO_ENT_SWI_FWD_ON   IO_swFWD
#define IO_ENT_SWI_RWD_ON   IO_swRWD
#define IO_ENT_SWI_CSL_ON   IO_swCSL
#define IO_ENT_SWI_INST_ON  IO_swINST

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
    struct SwitchStructure SWI_FWD;
    struct SwitchStructure SWI_RWD;
    struct SwitchStructure SWI_CSL;
    struct SwitchStructure SWI_INST;
    
};

void IO_Init(void);
void IO_TimerBaseOverflow(void);
