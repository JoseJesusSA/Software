/**
 \page "Autor"
 \author Jj Sa
 \version 2, 2016.10
*/

#include <p18f46k80.h>
#include "utils.h"
#include "mcu.h"
#include "can.h"
#include "in-out.h"

#pragma config RETEN = OFF
#pragma config INTOSCSEL = HIGH
#pragma config SOSCSEL = DIG
#pragma config XINST = OFF
#pragma config FOSC = HS2
#pragma config PLLCFG = OFF
#pragma config FCMEN = OFF
#pragma config IESO = OFF
#pragma config PWRTEN = ON
#pragma config BOREN = SBORDIS
#pragma config BORV = 0
#pragma config BORPWR = HIGH
#pragma config WDTEN = OFF
#pragma config WDTPS = 524288
#pragma config CANMX = PORTB
#pragma config MCLRE = OFF
#pragma config STVREN = ON
#pragma config BBSIZ = BB1K

extern volatile struct CANStructure CAN;
extern volatile struct IOStructure IO;

/**
El primer filtro para mensajes en modo 'normal' o de 'control'.
*/
UInt16UInt8 MyCANIDControl;

/**
El segundo filtro para el control de iluminacion.
*/
UInt16UInt8 MyCANIDDome;

/**
El Tercer filtro para el control de iluminacion.
*/
UInt16UInt8 MyCANIDRead;
/**
Un solo identificador de respuesta.
*/
UInt16UInt8 MyCANIDResp;

void Interrupt_High(void);
void Interrupt_Low(void);
void Interrupt_HighVector(void);
void Interrupt_LowVector(void);
void CAN_OnMessage(void);
void IO_OnPotCentralInstRequest(void);

/**
Interrupcion alta dedicada a la administracion de cuadros CAN.
*/
#pragma interrupt Interrupt_High
void Interrupt_High(void) {
    CAN_OnInterrupt();
    CAN_OnMessage();
    IO_LED_ERROR = !IO_LED_ERROR;
}

/**
 Interrupcion baja dedicada a interrupciones menores. Ej. bases de tiempo.
*/
#pragma interruptlow Interrupt_Low
void Interrupt_Low(void) {
    // principal base de tiempo de 16.67ms
    if(INTCONbits.TMR0IF) {
        INTCONbits.TMR0IF = FALSE;
//        TMR0H = 0xEB;
//        TMR0L = 0xA6; //16.6688ms@20MHz, 59.99Hz 1:16 prescaler
//        TMR0H = 0xFA;
//        TMR0L = 0xE8; //4.17ms@20MHz, 59.99Hz x 4 1:16 prescaler
        TMR0H = 0xF9;
        TMR0L = 0x35; //5.56ms@20MHz, 59.93Hz x 3 1:16 prescaler

        //actualizacion de indicadores
        if(IO.LEDListTimer.Counter)
            IO.LEDListTimer.Counter--;
    }
}

/**
 Asigna un vector para la interrupcion alta.
*/
#pragma code Interrupt_HighVector = 0x08 //recordar modo boot
void Interrupt_HighVector(void) {
  _asm
    goto Interrupt_High
  _endasm
}
#pragma code

/**
 \fn
 Asigna un vector para la interrupcion baja.
*/
#pragma code Interrupt_LowVector = 0x18 //recordar modo boot
void Interrupt_LowVector(void) {
  _asm
    goto Interrupt_Low
  _endasm
}
#pragma code

/**
Administra las peticiones del bus CAN.
*/
void CAN_OnMessage(void) {
    if(CAN.BufferReceived == 1) { // CAN ID de control
        UInt16UInt8 action;
        action.UI8[1] = CAN.RX0.Data[0]; // elemento
        action.UI8[0] = CAN.RX0.Data[1]; // accion
        //------------------------------------------------------------------------------------------
        switch(action.UI16) {
        //------------------------------------------------------------------------------------------
        case (UInt16)OVE_LUZAJUST1_CFG_CONFIG_PING:
            action.UI16 = (UInt16)OVE_LUZAJUST1_CFG_CONFIG_PINGRESP;
            CAN.TX.SID.UI16 = MyCANIDResp.UI16;
            CAN.TX.RTR = FALSE;
            CAN.TX.DLC = 4;
            CAN.TX.Data[0] = action.UI8[1]; //elemento ID
            CAN.TX.Data[1] = action.UI8[0]; //accion ID
            CAN.TX.Data[2] = TXERRCNT;
            CAN.TX.Data[3] = RXERRCNT;
            CAN_TXSendStdFrame();
            break; 
            
        default:
            break;
        } // fin de switch elemento-accion
       
    } else if(CAN.BufferReceived == 2) { // CAN ID de iluminacion
        UInt16UInt8 action;
        UInt16UInt8 pwm;
        action.UI8[1] = CAN.RX1.Data[0]; // elemento
        action.UI8[0] = CAN.RX1.Data[1]; // accion

        switch(action.UI16) {
        //------------------------------------------------------------------------------------------
        case (UInt16)OVE_INTLT_SWI_DOME_ON:
            // Actualizar lamparas tubulares. Variar Intensidad.
            IO_LAMP_INST_POWER_3(0x3FF);

            break;
        //------------------------------------------------------------------------------------------
        case (UInt16)OVE_INTLT_SWI_DOME_OFF:
            // Actualizar lamparas tubulares
            IO_LAMP_INST_POWER_3(0x00);
            break;
        //------------------------------------------------------------------------------------------
        case (UInt16)C1I_FLIDECLT_SWI_READING_READ:
            // Encender lampara de pedales
            IO_LAMP_INST_POWER_2(0x3FF);
            break;
        //-----------------------------------------------------------------------------------------    
        case (UInt16)C1I_FLIDECLT_SWI_READING_OFF:
            // Apagar lampara de pedales
            IO_LAMP_INST_POWER_2(0x00);
            break;
        //-----------------------------------------------------------------------------------------    
        default:
            break;
        } // fin de switch elemento-accion
    }
    CAN.BufferReceived = 0;
}

/**
 Funcion principal.
 \todo rutina de encendido.
*/
void main(void) {
    UInt32UInt8 canID;
    PWMDIM=0;
    MCU_Init();
    PWM2_Init();
    PWM3_Init();
    IO_Init();
    TMR0_Init();

    //tomamos el macro de respuesta PINGRESP con direccion "par" para la direccion de respuesta (base)
    canID.UI32 = OVE_LUZAJUST1_CFG_CONFIG_PINGRESP;
    MyCANIDResp.UI8[1] = canID.UI8[3];
    MyCANIDResp.UI8[0] = canID.UI8[2];

    //la direccion de "control" es la direccion base + 1
    MyCANIDControl.UI8[1] = MyCANIDResp.UI8[1];
    MyCANIDControl.UI8[0] = MyCANIDResp.UI8[0];
    MyCANIDControl.UI16++;

    //la direccion de iluminacion la tomamos del macro DOME
    canID.UI32 = OVE_INTLT_SWI_DOME_ON;
    MyCANIDDome.UI8[1] = canID.UI8[3];
    MyCANIDDome.UI8[0] = canID.UI8[2];
    
    //la direccion de iluminacion la tomamos del macro READ CM-1
    canID.UI32 = C1I_FLIDECLT_SWI_READING_READ;
    MyCANIDRead.UI8[1] = canID.UI8[3];
    MyCANIDRead.UI8[0] = canID.UI8[2];

    CAN.Config.RX0.Mask.UI16 = 0;
    CAN.Config.RX0.Filter0.UI16 = MyCANIDControl.UI16;
    CAN.Config.RX0.Filter1.UI16 = 0;

    CAN.Config.RX1.Mask.UI16 = 0;
    CAN.Config.RX1.Filter2.UI16 = MyCANIDDome.UI16;
    CAN.Config.RX1.Filter3.UI16 = MyCANIDRead.UI16;
    CAN.Config.RX1.Filter4.UI16 = 0;
    CAN.Config.RX1.Filter5.UI16 = 0;
    
    CAN_Init();

    MCU_EnableInterrupts();

    //empieza rutina de encendido
    IO_LED_STATUS = 1;
    IO_LED_ERROR = 1;
    
    IO_LAMP_INST_POWER_2(0);
    IO_LAMP_INST_POWER_3(0);

    //esperar mas que el resto de modulos para garantizar
    //una inicialización correcta
    IO.LEDListTimer.Counter = 180; //x5.56ms
    while(IO.LEDListTimer.Counter);
    IO.LEDListTimer.Counter = 180; //x5.56ms
    while(IO.LEDListTimer.Counter);

    //inicializar heartbeat
    IO.LEDListTimer.CounterProg = 90; //x5.56ms

    IO_LED_STATUS = 0;
    IO_LED_ERROR = 0;

    while(1) {
        if(IO.LEDListTimer.Counter == 0) {
            IO_LED_STATUS = !IO_LED_STATUS; //heartbeat
            IO.LEDListTimer.Counter = IO.LEDListTimer.CounterProg;
        }
    }
}