/*
 * File:   main.c
 * Created on 26 de febrero de 2016, 04:35 PM
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
#pragma config CANMX = PORTC
#pragma config MCLRE = OFF
#pragma config STVREN = ON
#pragma config BBSIZ = BB1K

volatile extern struct CANStructure CAN;
volatile extern struct IOStructure IO;

/**
El primer filtro para mensajes en modo 'normal' o de 'control'.
*/
UInt16UInt8 MyCANIDControl;

/**
El segundo filtro para el control de iluminacion.
*/
UInt16UInt8 MyCANIDLighting;

/**
Un solo identificador de respuesta.
*/
UInt16UInt8 MyCANIDResp;

void Interrupt_High(void);
void Interrupt_Low(void);
void Interrupt_HighVector(void);
void Interrupt_LowVector(void);

void CAN_OnMessage(void);

void IO_OnInputChanged(void);

/**
Interrupcion alta dedicada a la administracion de cuadros CAN.
\todo Pasar a vector bajo? Romperia codigo con TECMO.
*/
#pragma interrupt Interrupt_High
void Interrupt_High(void) {

    //IO_LED2 = 1;
    CAN_OnInterrupt();
    CAN_OnMessage();
    //IO_LED2 = 0;
    IO_LED2 = !IO_LED2;
}

/**
 Interrupcion baja dedicada a interrupciones menores. Ej. bases de tiempo.
*/
#pragma interruptlow Interrupt_Low
void Interrupt_Low(void) {
    // principal base de tiempo de 16.67ms
    if(INTCONbits.TMR0IF) {
        INTCONbits.TMR0IF = FALSE;
        TMR0H = 0xF3;
        TMR0L = 0xCA; //10ms@20MHz, 1:16 prescaler

        //IO_TimerBaseOverflow();
        //eliminar antirebotes
        if(IO.SWI_FWD.DebounceTimerCounter)
            IO.SWI_FWD.DebounceTimerCounter--;

        if(IO.SWI_RWD.DebounceTimerCounter)
            IO.SWI_RWD.DebounceTimerCounter--;

        if(IO.SWI_CSL.DebounceTimerCounter)
            IO.SWI_CSL.DebounceTimerCounter--;

        if(IO.SWI_INST.DebounceTimerCounter)
            IO.SWI_INST.DebounceTimerCounter--;

        //cambio en indicadores de actividad
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
//    if(CAN.BufferReceived == FALSE)
//        return;

    if(CAN.BufferReceived == 1) { // CAN ID de control
        UInt16UInt8 action;
        action.UI8[1] = CAN.RX0.Data[0]; // elemento
        action.UI8[0] = CAN.RX0.Data[1]; // accion

        switch(action.UI16) {// ya concemos la CAN ID   
        case (UInt16)OVE_FLTDECKLT_POT_FWD_SWITCHSTAT:                   //Estado FWD
            if(!IO_ENT_SWI_FWD_ON) 
                action.UI16 = (UInt16)OVE_FLTDECKLT_POT_FWD_OFF;
            else
                action.UI16 = (UInt16)OVE_FLTDECKLT_POT_FWD_nOFF;
            CAN.TX.SID.UI16 = MyCANIDResp.UI16;
            CAN.TX.RTR = FALSE;
            CAN.TX.DLC = 2;
            CAN.TX.Data[0] = action.UI8[1]; //elemento ID
            CAN.TX.Data[1] = action.UI8[0]; //accion ID
            CAN_TXSendStdFrame();
            break;
        case (UInt16)OVE_FLTDECKLT_POT_FWD_ANTIREBOTE:               //Antirebote FWD
            IO.SWI_FWD.DebounceTimerProg = CAN.RX0.Data[2];
            break;

        case (UInt16)OVE_FLTDECKLT_POT_RWD_SWITCHSTAT:                  //Estado RWD
            if(!IO_ENT_SWI_RWD_ON) 
                action.UI16 = (UInt16)OVE_FLTDECKLT_POT_RWD_OFF;
            else 
                action.UI16 = (UInt16)OVE_FLTDECKLT_POT_RWD_nOFF;

            CAN.TX.SID.UI16 = MyCANIDResp.UI16;
            CAN.TX.RTR = FALSE;
            CAN.TX.DLC = 2;
            CAN.TX.Data[0] = action.UI8[1]; //elemento ID
            CAN.TX.Data[1] = action.UI8[0]; //accion ID
            CAN_TXSendStdFrame();
            break;
        case (UInt16)OVE_FLTDECKLT_POT_RWD_ANTIREBOTE:            //Antirebote GA
            IO.SWI_RWD.DebounceTimerProg = CAN.RX0.Data[2];
            break;
            
        case (UInt16)OVE_FLTDECKLT_POT_INST_SWITCHSTAT:              //Estado AP
            if(!IO_ENT_SWI_INST_ON) 
                action.UI16 = (UInt16)OVE_FLTDECKLT_POT_INST_OFF;
            else 
                action.UI16 = (UInt16)OVE_FLTDECKLT_POT_INST_nOFF;

            CAN.TX.SID.UI16 = MyCANIDResp.UI16;
            CAN.TX.RTR = FALSE;
            CAN.TX.DLC = 2;
            CAN.TX.Data[0] = action.UI8[1]; //elemento ID
            CAN.TX.Data[1] = action.UI8[0]; //accion ID
            CAN_TXSendStdFrame();
            break;
        case (UInt16)OVE_FLTDECKLT_POT_INST_ANTIREBOTE:            //Antirebote AP
            IO.SWI_INST.DebounceTimerProg = CAN.RX0.Data[2];
            break;
            
        case (UInt16)OVE_FLTDECKLT_POT_CSL_SWITCHSTAT:              //Estado Trim
            if(!IO_ENT_SWI_CSL_ON) 
                action.UI16 = (UInt16)OVE_FLTDECKLT_POT_CSL_OFF;
            else
                action.UI16 = (UInt16)OVE_FLTDECKLT_POT_CSL_nOFF;
            
            CAN.TX.SID.UI16 = MyCANIDResp.UI16;
            CAN.TX.RTR = FALSE;
            CAN.TX.DLC = 2;
            CAN.TX.Data[0] = action.UI8[1]; //elemento ID
            CAN.TX.Data[1] = action.UI8[0]; //accion ID
            CAN_TXSendStdFrame();
            break;
        case (UInt16)OVE_FLTDECKLT_POT_CSL_ANTIREBOTE:          //Antirebote Trim
            IO.SWI_CSL.DebounceTimerProg = CAN.RX0.Data[2];
            break;
            
        case (UInt16)OVE_FLTDECKLT_CFG_CONFIG_PING:             //piN
            action.UI16 = (UInt16)OVE_FLTDECKLT_CFG_CONFIG_PINGRESP;
            CAN.TX.SID.UI16 = MyCANIDResp.UI16;
            CAN.TX.RTR = FALSE;
            CAN.TX.DLC = 2;
            CAN.TX.Data[0] = action.UI8[1]; //elemento ID
            CAN.TX.Data[1] = action.UI8[0]; //accion ID
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
        case (UInt16)LT_UPPER_FWD_LAMPS:
            // apagar caratula y mantener lamparas al maximo
            IO_CE_POWER(0); //IO_CE_SEL = FALSE;
            IO_LAMPS_POWER(0x3FF);
            break;
        case (UInt16)LT_UPPER_FWD_CE:
            // caratula y lamparas al mismo nivel de intensidad
            pwm.UI8[1] = CAN.RX1.Data[2];
            pwm.UI8[0] = CAN.RX1.Data[3];
            IO_CE_POWER(pwm.UI16); //IO_CE_SEL = TRUE;
            IO_LAMPS_POWER(pwm.UI16);
            break;
        default:
            break;
        } // fin de switch elemento-accion
    }

    CAN.BufferReceived = 0;
}

/**
Ejecuta acciones dependiendo de la entrada digital que tenga algun cambio.
*/
void IO_OnInputChanged(void) {
    UInt16UInt8 action;

    if(IO.SWI_FWD.DebounceTimerCounter == 0) {
        IO.SWI_FWD.Switch.Bits.Input = IO_ENT_SWI_FWD_ON;
        if(IO.SWI_FWD.Switch.Bits.Input != IO.SWI_FWD.Switch.Bits.InputBack) {
            if(IO.SWI_FWD.Switch.Bits.Input == 0)
                action.UI16 = (UInt16)OVE_FLTDECKLT_POT_FWD_nOFF;
            else
                action.UI16 = (UInt16)OVE_FLTDECKLT_POT_FWD_OFF;

            IO.SWI_FWD.DebounceTimerCounter = IO.SWI_FWD.DebounceTimerProg;
            IO.SWI_FWD.Switch.Bits.InputBack = IO.SWI_FWD.Switch.Bits.Input;

            CAN.TX.SID.UI16 = MyCANIDResp.UI16;
            CAN.TX.RTR = FALSE;
            CAN.TX.DLC = 2;
            CAN.TX.Data[0] = action.UI8[1]; //elemento ID
            CAN.TX.Data[1] = action.UI8[0]; //accion ID
            CAN_TXSendStdFrame();
            IO_LED2 = !IO_LED2;
        }
    }

    if(IO.SWI_RWD.DebounceTimerCounter == 0) {
        IO.SWI_RWD.Switch.Bits.Input = IO_ENT_SWI_RWD_ON;
        if(IO.SWI_RWD.Switch.Bits.Input != IO.SWI_RWD.Switch.Bits.InputBack) {
            if(IO.SWI_RWD.Switch.Bits.Input == 0)
                action.UI16 = (UInt16)OVE_FLTDECKLT_POT_RWD_nOFF;
            else
                action.UI16 = (UInt16)OVE_FLTDECKLT_POT_RWD_OFF;

            IO.SWI_RWD.DebounceTimerCounter = IO.SWI_RWD.DebounceTimerProg;
            IO.SWI_RWD.Switch.Bits.InputBack = IO.SWI_RWD.Switch.Bits.Input;

            CAN.TX.SID.UI16 = MyCANIDResp.UI16;
            CAN.TX.RTR = FALSE;
            CAN.TX.DLC = 2;
            CAN.TX.Data[0] = action.UI8[1]; //elemento ID
            CAN.TX.Data[1] = action.UI8[0]; //accion ID
            CAN_TXSendStdFrame();
            IO_LED2 = !IO_LED2;
        }
    }

    if(IO.SWI_CSL.DebounceTimerCounter == 0) {
        IO.SWI_CSL.Switch.Bits.Input = IO_ENT_SWI_CSL_ON;
        if(IO.SWI_CSL.Switch.Bits.Input != IO.SWI_CSL.Switch.Bits.InputBack) {
            if(IO.SWI_CSL.Switch.Bits.Input == 0)
                action.UI16 = (UInt16)OVE_FLTDECKLT_POT_CSL_nOFF ;
            else
                action.UI16 = (UInt16)OVE_FLTDECKLT_POT_CSL_OFF;

            IO.SWI_CSL.DebounceTimerCounter = IO.SWI_CSL.DebounceTimerProg;
            IO.SWI_CSL.Switch.Bits.InputBack = IO.SWI_CSL.Switch.Bits.Input;

            CAN.TX.SID.UI16 = MyCANIDResp.UI16;
            CAN.TX.RTR = FALSE;
            CAN.TX.DLC = 2;
            CAN.TX.Data[0] = action.UI8[1]; //elemento ID
            CAN.TX.Data[1] = action.UI8[0]; //accion ID
            CAN_TXSendStdFrame();
            IO_LED2 = !IO_LED2;
        }
    }

    if(IO.SWI_INST.DebounceTimerCounter == 0) {
        IO.SWI_INST.Switch.Bits.Input = IO_ENT_SWI_INST_ON;
        if(IO.SWI_INST.Switch.Bits.Input != IO.SWI_INST.Switch.Bits.InputBack) {
            if(IO.SWI_INST.Switch.Bits.Input == 0)
                action.UI16 = (UInt16)OVE_FLTDECKLT_POT_INST_nOFF;
            else
                action.UI16 = (UInt16)OVE_FLTDECKLT_POT_INST_OFF;

            IO.SWI_INST.DebounceTimerCounter = IO.SWI_INST.DebounceTimerProg;
            IO.SWI_INST.Switch.Bits.InputBack = IO.SWI_INST.Switch.Bits.Input;

            CAN.TX.SID.UI16 = MyCANIDResp.UI16;
            CAN.TX.RTR = FALSE;
            CAN.TX.DLC = 2;
            CAN.TX.Data[0] = action.UI8[1]; //elemento ID
            CAN.TX.Data[1] = action.UI8[0]; //accion ID
            CAN_TXSendStdFrame();
            IO_LED2 = !IO_LED2;
        }
    }
}

/**
 Funcion principal.
 \todo rutina de encendido.
*/
void main(void) {
    UInt32UInt8 canID;

    MCU_Init();
    //PWM1_Init();
    //PWM4_Init();
    IO_Init();
    TMR0_Init();

    //tomamos el macro de respuesta PINGRESP con direccion "par" para la direccion de respuesta (base)
    canID.UI32 = OVE_FLTDECKLT_CFG_CONFIG_PINGRESP;
    MyCANIDResp.UI8[1] = canID.UI8[3];
    MyCANIDResp.UI8[0] = canID.UI8[2];

    //la direccion de "control" es la direccion base + 1
    MyCANIDControl.UI8[1] = MyCANIDResp.UI8[1];
    MyCANIDControl.UI8[0] = MyCANIDResp.UI8[0];
    MyCANIDControl.UI16++;

    //la direccion de iluminacion la tomamos del macro para control de caratula
    canID.UI32 = LT_UPPER_FWD_CE;
    MyCANIDLighting.UI8[1] = canID.UI8[3];
    MyCANIDLighting.UI8[0] = canID.UI8[2];

    CAN.Config.RX0.Mask.UI16 = 0;
    CAN.Config.RX0.Filter0.UI16 = MyCANIDControl.UI16;
    CAN.Config.RX0.Filter1.UI16 = 0;

    CAN.Config.RX1.Mask.UI16 = 0;
    CAN.Config.RX1.Filter2.UI16 = MyCANIDLighting.UI16;
    CAN.Config.RX1.Filter3.UI16 = 0;
    CAN.Config.RX1.Filter4.UI16 = 0;
    CAN.Config.RX1.Filter5.UI16 = 0;

    CAN_Init();

    MCU_EnableInterrupts();

    //empieza rutina de encendido

    IO_LED1 = 1;
    IO_LED2 = 1;

    //IO_CE_POWER(0x3FF); //IO_CE_SEL = FALSE;

    //espera para apagar todas las salidas y empezar heartbeat
    IO.LEDListTimer.Counter = 480; //x4.16ms
    while(IO.LEDListTimer.Counter);
    IO.LEDListTimer.CounterProg = 120; //x4.16ms

    IO_LED1 = 0;
    IO_LED2 = 0;
    //IO_LAMPS_POWER(0x3FF);

    while(1) {
        IO_OnInputChanged();

        if(IO.LEDListTimer.Counter == 0) {
            IO_LED1 = !IO_LED1; //heartbeat
            IO.LEDListTimer.Counter = IO.LEDListTimer.CounterProg;
        }
    }
}
