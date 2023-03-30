/**
 \page "Autor"
 \version 2, 2016.10
*/

/**
\mainpage "Firmware para la tarjeta FLIGHT-DECK-LT-CM1 para el simulador CASA-295."
Caracteristicas de la tarjeta:
- 6 entradas activas en bajo para interruptores.
- 3 canalES ADC con acople de impedancia.
- 1 canal PWM para control de iluminacion en caratula.
- Comunicacion CAN con transceiver High-Speed.
- Almacenamiento de configuracion en memoria EEPROM?
===
---
 #Lista general por hacer:#
- Hacer que la recepcion CAN responda hasta 500kBit sin mensajes perdidos por
el tiempo de procesamiento. OK
\note El header de este MCU maneja "grupos de bits" en las estructuras de sus
registros especiales; distinto a lo tradicional de tener bits independientes.
*/

/** \file
Contiene la configuracion por defecto del MCU y las "rutinas de aplicacion"
del equipo: administracion de comandos CAN e interfaz de usuario.
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

extern volatile struct CANStructure CAN;
extern volatile struct IOStructure IO;
int GEN;

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
void IO_OnPotSrchGainRequest(void);
void IO_OnPotTiltRequest(void);
void IO_OnPotBCNGainRequest(void);
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

        //IO_TimerBaseOverflow();
        //eliminar rebotes
        if(IO.PotSwitchs.Inst.DebounceTimerCounter)
            IO.PotSwitchs.Inst.DebounceTimerCounter--;

        if(IO.PotSwitchs.LatlCsl.DebounceTimerCounter)
            IO.PotSwitchs.LatlCsl.DebounceTimerCounter--;

        if(IO.PotSwitchs.Panel.DebounceTimerCounter)
            IO.PotSwitchs.Panel.DebounceTimerCounter--;

        if(IO.Switches.FloorPedal.DebounceTimerCounter)
            IO.Switches.FloorPedal.DebounceTimerCounter--;

        if(IO.Switches.Map.DebounceTimerCounter)
            IO.Switches.Map.DebounceTimerCounter--;

        if(IO.Switches.Reading.DebounceTimerCounter)
            IO.Switches.Reading.DebounceTimerCounter--;

        //evitamos saturar los 3 buffers TX CAN desplazando 5.56ms cada mensaje
        switch (IO.Pots.MsgCANState) {
        case 0:
            if(IO.Pots.Inst.Counter)
                IO.Pots.Inst.Counter--;
            IO.Pots.MsgCANState++;
            //IO.Pots.MsgCANFlag = 1;
            break;
        case 1:
            if(IO.Pots.LatlCsl.Counter)
                IO.Pots.LatlCsl.Counter--;
            IO.Pots.MsgCANState++;
            //IO.Pots.MsgCANFlag = 1;
            break;
        case 2:
            if(IO.Pots.Panel.Counter)
                IO.Pots.Panel.Counter--;
            IO.Pots.MsgCANState = 0;
            //IO.Pots.MsgCANFlag = 1;
            break;
        default:
            IO.Pots.MsgCANState = 0;
            //IO.Pots.MsgCANFlag = 0;
            break;
        }

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

        switch(action.UI16) {// ya concemos la CAN ID
        //------------------------------------------------------------------------------------------
        case (UInt16)C1I_FLIDECLT_SWI_FLOORPED_ANTIREBOTE:
            IO.Switches.FloorPedal.DebounceTimerProg = CAN.RX0.Data[2];
            break;
        //------------------------------------------------------------------------------------------
        case (UInt16)C1I_FLIDECLT_SWI_FLOORPED_STAT:
            if(!IO_ENT_SW_FLOOR_PEDAL) action.UI16 = (UInt16)C1I_FLIDECLT_SWI_FLOORPED_FLOPED;
            else action.UI16 = (UInt16)C1I_FLIDECLT_SWI_FLOORPED_OFF;

            CAN.TX.SID.UI16 = MyCANIDResp.UI16;
            CAN.TX.RTR = FALSE;
            CAN.TX.DLC = 2;
            CAN.TX.Data[0] = action.UI8[1]; //elemento ID
            CAN.TX.Data[1] = action.UI8[0]; //accion ID
            CAN_TXSendStdFrame();
            break;
        //------------------------------------------------------------------------------------------
        case (UInt16)C1I_FLIDECLT_SWI_MAP_ANTIREBOTE:
            IO.Switches.Map.DebounceTimerProg = CAN.RX0.Data[2];
            break;
        //------------------------------------------------------------------------------------------
        case (UInt16)C1I_FLIDECLT_SWI_MAP_STAT:
            if(!IO_ENT_SW_MAP) action.UI16 = (UInt16)C1I_FLIDECLT_SWI_MAP_MAP;
            else action.UI16 = (UInt16)C1I_FLIDECLT_SWI_MAP_OFF;

            CAN.TX.SID.UI16 = MyCANIDResp.UI16;
            CAN.TX.RTR = FALSE;
            CAN.TX.DLC = 2;
            CAN.TX.Data[0] = action.UI8[1]; //elemento ID
            CAN.TX.Data[1] = action.UI8[0]; //accion ID
            CAN_TXSendStdFrame();
            break;
        //------------------------------------------------------------------------------------------
        case (UInt16)C1I_FLIDECLT_SWI_READING_ANTIREBOTE:
            IO.Switches.Reading.DebounceTimerProg = CAN.RX0.Data[2];
            break;
        case (UInt16)C1I_FLIDECLT_SWI_READING_STAT:
            if(!IO_ENT_SW_READING) action.UI16 = (UInt16)C1I_FLIDECLT_SWI_READING_READ;
            else action.UI16 = (UInt16)C1I_FLIDECLT_SWI_READING_OFF;

            CAN.TX.SID.UI16 = MyCANIDResp.UI16;
            CAN.TX.RTR = FALSE;
            CAN.TX.DLC = 2;
            CAN.TX.Data[0] = action.UI8[1]; //elemento ID
            CAN.TX.Data[1] = action.UI8[0]; //accion ID
            CAN_TXSendStdFrame();
            break;
        //------------------------------------------------------------------------------------------
        case (UInt16)C1I_FLIDECLT_POT_INST_ANTIREBOTE:
            IO.PotSwitchs.Inst.DebounceTimerProg = CAN.RX0.Data[2];
            break;
        //------------------------------------------------------------------------------------------
        case (UInt16)C1I_FLIDECLT_POT_INST_TIMER:
            IO.Pots.Inst.CounterProg = CAN.RX0.Data[2]; //Counter=0 detiene el envio
            break;
        //------------------------------------------------------------------------------------------
        case (UInt16)C1I_FLIDECLT_POT_INST_SWITCHSTAT:
            if(!IO_ENT_SW_INST) action.UI16 = (UInt16)C1I_FLIDECLT_POT_INST_nOFF;
            else action.UI16 = (UInt16)C1I_FLIDECLT_POT_INST_OFF;

            CAN.TX.SID.UI16 = MyCANIDResp.UI16;
            CAN.TX.RTR = FALSE;
            CAN.TX.DLC = 2;
            CAN.TX.Data[0] = action.UI8[1]; //elemento ID
            CAN.TX.Data[1] = action.UI8[0]; //accion ID
            CAN_TXSendStdFrame();
            break;
        //------------------------------------------------------------------------------------------
        case (UInt16)C1I_FLIDECLT_POT_LATLCSL_ANTIREBOTE:
            IO.PotSwitchs.LatlCsl.DebounceTimerProg = CAN.RX0.Data[2];
            break;
        //------------------------------------------------------------------------------------------
        case (UInt16)C1I_FLIDECLT_POT_LATLCSL_TIMER:
            IO.Pots.LatlCsl.CounterProg = CAN.RX0.Data[2]; //Counter=0 detiene el envio
            break;
        //------------------------------------------------------------------------------------------
        case (UInt16)C1I_FLIDECLT_POT_LATLCSL_SWITCHSTAT:
            if(!IO_ENT_SW_LATL_CSL) action.UI16 = (UInt16)C1I_FLIDECLT_POT_LATLCSL_nOFF;
            else action.UI16 = (UInt16)C1I_FLIDECLT_POT_LATLCSL_OFF;

            CAN.TX.SID.UI16 = MyCANIDResp.UI16;
            CAN.TX.RTR = FALSE;
            CAN.TX.DLC = 2;
            CAN.TX.Data[0] = action.UI8[1]; //elemento ID
            CAN.TX.Data[1] = action.UI8[0]; //accion ID
            CAN_TXSendStdFrame();
            break;
        //------------------------------------------------------------------------------------------
        case (UInt16)C1I_FLIDECLT_POT_PANEL_ANTIREBOTE:
            IO.PotSwitchs.Panel.DebounceTimerProg = CAN.RX0.Data[2];
            break;
        //------------------------------------------------------------------------------------------
        case (UInt16)C1I_FLIDECLT_POT_PANEL_TIMER:
            IO.Pots.Panel.CounterProg = CAN.RX0.Data[2]; //Counter=0 detiene el envio
            break;
        //------------------------------------------------------------------------------------------
        case (UInt16)C1I_FLIDECLT_POT_PANEL_SWITCHSTAT:
            if(!IO_ENT_SW_PANEL) action.UI16 = (UInt16)C1I_FLIDECLT_POT_PANEL_nOFF;
            else action.UI16 = (UInt16)C1I_FLIDECLT_POT_PANEL_OFF;

            CAN.TX.SID.UI16 = MyCANIDResp.UI16;
            CAN.TX.RTR = FALSE;
            CAN.TX.DLC = 2;
            CAN.TX.Data[0] = action.UI8[1]; //elemento ID
            CAN.TX.Data[1] = action.UI8[0]; //accion ID
            CAN_TXSendStdFrame();
            break;
        //------------------------------------------------------------------------------------------
        case(UInt16) C1I_FLIDECLT_MSW_GENBUS1_ON:
            GEN=1;
            break;
        //------------------------------------------------------------------------------------------    
        case(UInt16) C1I_FLIDECLT_MSW_GENBUS1_OFF:
            GEN=0;
            break;
        //------------------------------------------------------------------------------------------
        case (UInt16)C1I_FLIDECLT_CFG_CONFIG_PING:
            action.UI16 = (UInt16)C1I_FLIDECLT_CFG_CONFIG_PINGRESP;
            CAN.TX.SID.UI16 = MyCANIDResp.UI16;
            CAN.TX.RTR = FALSE;
            CAN.TX.DLC = 4;
            CAN.TX.Data[0] = action.UI8[1]; //elemento ID
            CAN.TX.Data[1] = action.UI8[0]; //accion ID
            CAN.TX.Data[2] = TXERRCNT;
            CAN.TX.Data[3] = RXERRCNT;
            CAN_TXSendStdFrame();
            break;
        //------------------------------------------------------------------------------------------
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
        case (UInt16)LT_CM1_INST_LAMPS:
            // apagar caratula y mantener lamparas al maximo
            //IO_CE_SEL = FALSE;
            IO_CE_POWER(0);
            break;
        //------------------------------------------------------------------------------------------
        case (UInt16)LT_CM1_INST_CE:
            // caratula y lamparas al mismo nivel de intensidad
            pwm.UI8[1] = CAN.RX1.Data[2];
            pwm.UI8[0] = CAN.RX1.Data[3];
            IO_CE_POWER(pwm.UI16);
            //IO_CE_SEL = TRUE;
            break;
        //------------------------------------------------------------------------------------------
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

    if(IO.Switches.FloorPedal.DebounceTimerCounter == 0 && GEN == 0) {
        IO.Switches.FloorPedal.Switch.Bits.Input = IO_ENT_SW_FLOOR_PEDAL;
        IO.Switches.FloorPedal.Switch.Bits.InputBack = IO.Switches.FloorPedal.Switch.Bits.Input;
    }
    if(IO.Switches.Map.DebounceTimerCounter == 0  && GEN == 0) {
        IO.Switches.Map.Switch.Bits.Input = IO_ENT_SW_MAP;
        IO.Switches.Map.Switch.Bits.InputBack = IO.Switches.Map.Switch.Bits.Input;
    }
    if(IO.Switches.Reading.DebounceTimerCounter == 0 && GEN == 0) {
        IO.Switches.Reading.Switch.Bits.Input = IO_ENT_SW_READING;
        IO.Switches.Reading.Switch.Bits.InputBack = IO.Switches.Reading.Switch.Bits.Input;
    }
    
    if(IO.Switches.FloorPedal.DebounceTimerCounter == 0 && GEN==1) {
        IO.Switches.FloorPedal.Switch.Bits.Input = IO_ENT_SW_FLOOR_PEDAL;
        if(IO.Switches.FloorPedal.Switch.Bits.Input != IO.Switches.FloorPedal.Switch.Bits.InputBack) {
            if(IO.Switches.FloorPedal.Switch.Bits.Input == 0)
                action.UI16 = (UInt16)C1I_FLIDECLT_SWI_FLOORPED_FLOPED;
            else
                action.UI16 = (UInt16)C1I_FLIDECLT_SWI_FLOORPED_OFF;

            IO.Switches.FloorPedal.DebounceTimerCounter = IO.Switches.FloorPedal.DebounceTimerProg;
            IO.Switches.FloorPedal.Switch.Bits.InputBack = IO.Switches.FloorPedal.Switch.Bits.Input;

            CAN.TX.SID.UI16 = MyCANIDResp.UI16;
            CAN.TX.RTR = FALSE;
            CAN.TX.DLC = 2;
            CAN.TX.Data[0] = action.UI8[1]; //elemento ID
            CAN.TX.Data[1] = action.UI8[0]; //accion ID
            CAN_TXSendStdFrame();
            IO_LED_STATUS = !IO_LED_STATUS;
        }
    }

    if(IO.Switches.Map.DebounceTimerCounter == 0 && GEN==1) {
        IO.Switches.Map.Switch.Bits.Input = IO_ENT_SW_MAP;
        if(IO.Switches.Map.Switch.Bits.Input != IO.Switches.Map.Switch.Bits.InputBack) {
            if(IO.Switches.Map.Switch.Bits.Input == 0)
                action.UI16 = (UInt16)C1I_FLIDECLT_SWI_MAP_MAP;
            else
                action.UI16 = (UInt16)C1I_FLIDECLT_SWI_MAP_OFF;

            IO.Switches.Map.DebounceTimerCounter = IO.Switches.Map.DebounceTimerProg;
            IO.Switches.Map.Switch.Bits.InputBack = IO.Switches.Map.Switch.Bits.Input;

            CAN.TX.SID.UI16 = MyCANIDResp.UI16;
            CAN.TX.RTR = FALSE;
            CAN.TX.DLC = 2;
            CAN.TX.Data[0] = action.UI8[1]; //elemento ID
            CAN.TX.Data[1] = action.UI8[0]; //accion ID
            CAN_TXSendStdFrame();
            IO_LED_STATUS = !IO_LED_STATUS;
        }
    }

    if(IO.Switches.Reading.DebounceTimerCounter == 0 && GEN==1) {
        IO.Switches.Reading.Switch.Bits.Input = IO_ENT_SW_READING;
        if(IO.Switches.Reading.Switch.Bits.Input != IO.Switches.Reading.Switch.Bits.InputBack) {
            if(IO.Switches.Reading.Switch.Bits.Input == 0)
                action.UI16 = (UInt16)C1I_FLIDECLT_SWI_READING_READ;
            else
                action.UI16 = (UInt16)C1I_FLIDECLT_SWI_READING_OFF;

            IO.Switches.Reading.DebounceTimerCounter = IO.Switches.Reading.DebounceTimerProg;
            IO.Switches.Reading.Switch.Bits.InputBack = IO.Switches.Reading.Switch.Bits.Input;

            CAN.TX.SID.UI16 = MyCANIDResp.UI16;
            CAN.TX.RTR = FALSE;
            CAN.TX.DLC = 2;
            CAN.TX.Data[0] = action.UI8[1]; //elemento ID
            CAN.TX.Data[1] = action.UI8[0]; //accion ID
            CAN_TXSendStdFrame();
            IO_LED_STATUS = !IO_LED_STATUS;
        }
    }

    if(IO.PotSwitchs.Inst.DebounceTimerCounter == 0) {
        IO.PotSwitchs.Inst.Switch.Bits.Input = IO_ENT_SW_INST;
        if(IO.PotSwitchs.Inst.Switch.Bits.Input != IO.PotSwitchs.Inst.Switch.Bits.InputBack) {
            IO.PotSwitchs.Inst.DebounceTimerCounter = IO.PotSwitchs.Inst.DebounceTimerProg;
            IO.PotSwitchs.Inst.Switch.Bits.InputBack = IO.PotSwitchs.Inst.Switch.Bits.Input;
            IO_LED_STATUS = !IO_LED_STATUS;
        }
    }

    if(IO.PotSwitchs.LatlCsl.DebounceTimerCounter == 0) {
        IO.PotSwitchs.LatlCsl.Switch.Bits.Input = IO_ENT_SW_LATL_CSL;
        if(IO.PotSwitchs.LatlCsl.Switch.Bits.Input != IO.PotSwitchs.LatlCsl.Switch.Bits.InputBack) {
            IO.PotSwitchs.LatlCsl.DebounceTimerCounter = IO.PotSwitchs.LatlCsl.DebounceTimerProg;
            IO.PotSwitchs.LatlCsl.Switch.Bits.InputBack = IO.PotSwitchs.LatlCsl.Switch.Bits.Input;
            IO_LED_STATUS = !IO_LED_STATUS;
        }
    }

    if(IO.PotSwitchs.Panel.DebounceTimerCounter == 0) {
        IO.PotSwitchs.Panel.Switch.Bits.Input = IO_ENT_SW_PANEL;
        if(IO.PotSwitchs.Panel.Switch.Bits.Input != IO.PotSwitchs.Panel.Switch.Bits.InputBack) {
            IO.PotSwitchs.Panel.DebounceTimerCounter = IO.PotSwitchs.Panel.DebounceTimerProg;
            IO.PotSwitchs.Panel.Switch.Bits.InputBack = IO.PotSwitchs.Panel.Switch.Bits.Input;
            IO_LED_STATUS = !IO_LED_STATUS;
        }
    }

}

/**
  Envia de forma continua la posicion de Pedestal.
*/
void IO_OnPotSrchGainRequest(void) {
    UInt32UInt8 action;
    UInt16UInt8 tmpAdc;

    if(IO.Pots.Inst.CounterProg) { //enviar mensajes solo si se ha inciado el temporizador
        if(IO.Pots.Inst.Counter == 0) {
            if(IO.PotSwitchs.Inst.Switch.Bits.Input == 0) {
                action.UI32 = LT_CM1_INST_CE;
                tmpAdc.UI16 = IO.Pots.Inst.Value.UI16 >> 2;
                CAN.TX.DLC = 4;
                CAN.TX.Data[2] = tmpAdc.UI8[1];
                CAN.TX.Data[3] = tmpAdc.UI8[0];
            } else {
                action.UI32 = LT_CM1_INST_LAMPS;
                CAN.TX.DLC = 2;
            }

            CAN.TX.SID.UI8[1] = action.UI8[3];
            CAN.TX.SID.UI8[0] = action.UI8[2];
            CAN.TX.RTR = FALSE;
            CAN.TX.Data[0] = action.UI8[1]; //elemento ID
            CAN.TX.Data[1] = action.UI8[0]; //accion ID
            CAN_TXSendStdFrame();
            IO_LED_ERROR = !IO_LED_ERROR;
            IO.Pots.Inst.Counter = IO.Pots.Inst.CounterProg; //x16.67ms
        }
    }
}

/**
  Envia de forma continua la posicion de Pedestal.
*/
void IO_OnPotTiltRequest(void) {
    UInt32UInt8 action;
    UInt16UInt8 tmpAdc;

    if(IO.Pots.LatlCsl.CounterProg) { //enviar mensajes solo si se ha inciado el temporizador
        if(IO.Pots.LatlCsl.Counter == 0) {
            if(IO.PotSwitchs.LatlCsl.Switch.Bits.Input == 0) {
                action.UI32 = LT_CM1_LATL_CE;
                tmpAdc.UI16 = IO.Pots.LatlCsl.Value.UI16 >> 2;
                CAN.TX.DLC = 4;
                CAN.TX.Data[2] = tmpAdc.UI8[1];
                CAN.TX.Data[3] = tmpAdc.UI8[0];
            } else {
                action.UI32 = LT_CM1_LATL_LAMPS;
                CAN.TX.DLC = 2;
            }

            CAN.TX.SID.UI8[1] = action.UI8[3];
            CAN.TX.SID.UI8[0] = action.UI8[2];
            CAN.TX.RTR = FALSE;
            CAN.TX.Data[0] = action.UI8[1]; //elemento ID
            CAN.TX.Data[1] = action.UI8[0]; //accion ID
            CAN_TXSendStdFrame();
            IO_LED_ERROR = !IO_LED_ERROR;
            IO.Pots.LatlCsl.Counter = IO.Pots.LatlCsl.CounterProg; //x16.67ms
        }
    }
}

/**
  Envia de forma continua la posicion de Panel.
*/
void IO_OnPotBCNGainRequest(void) {
    UInt32UInt8 action;
    UInt16UInt8 tmpAdc;

    if(IO.Pots.Panel.CounterProg) { //enviar mensajes solo si se ha inciado el temporizador
        if(IO.Pots.Panel.Counter == 0) {
            if(IO.PotSwitchs.Panel.Switch.Bits.Input == 0) {
                action.UI32 = LT_CM1_PANEL_CE;
                tmpAdc.UI16 = IO.Pots.Panel.Value.UI16 >> 2;
                CAN.TX.DLC = 4;
                CAN.TX.Data[2] = tmpAdc.UI8[1];
                CAN.TX.Data[3] = tmpAdc.UI8[0];
                IO_CE_POWER(tmpAdc.UI16); //no captura su propio mensaje CAN
            } else {
                action.UI32 = LT_CM1_PANEL_LAMPS;
                CAN.TX.DLC = 2;
                IO_CE_POWER(0); //no captura su propio mensaje CAN
            }

            CAN.TX.SID.UI8[1] = action.UI8[3];
            CAN.TX.SID.UI8[0] = action.UI8[2];
            CAN.TX.RTR = FALSE;
            CAN.TX.Data[0] = action.UI8[1]; //elemento ID
            CAN.TX.Data[1] = action.UI8[0]; //accion ID
            CAN_TXSendStdFrame();
            IO_LED_ERROR = !IO_LED_ERROR;
            IO.Pots.Panel.Counter = IO.Pots.Panel.CounterProg; //x16.67ms
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
    PWM5_Init();
    ADC_Init();
    IO_Init();
    TMR0_Init();

    //tomamos el macro de respuesta PINGRESP con direccion "par" para la direccion de respuesta (base)
    canID.UI32 = C1I_FLIDECLT_CFG_CONFIG_PINGRESP;
    MyCANIDResp.UI8[1] = canID.UI8[3];
    MyCANIDResp.UI8[0] = canID.UI8[2];

    //la direccion de "control" es la direccion base + 1
    MyCANIDControl.UI8[1] = MyCANIDResp.UI8[1];
    MyCANIDControl.UI8[0] = MyCANIDResp.UI8[0];
    MyCANIDControl.UI16++;

    //la direccion de iluminacion la tomamos del macro para control de caratula
    canID.UI32 = LT_CM1_INST_CE;
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

    IO_LED_STATUS = 1;
    IO_LED_ERROR = 1;

    IO_CE_POWER(0);


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
        //TODO eliminar parpadeo en bajas potencias debido a sensibilidad de LEDS
        //usar filtros en ADC o menos bits?
        IO_OnInputChanged();

        IO_PotsListUpdate();

        //se puede pasar a interrupciones la siguiente maquina de estados
        //y si se ase, se debe eliminar CANFlag
//        if(IO.Pots.MsgCANFlag) {
//            switch (IO.Pots.MsgCANState) {
//            case 0:
//                IO_OnPotInstRequest();
//                break;
//            case 1:
//                IO_OnPotLatlCslRequest();
//                break;
//            case 2:
//                IO_OnPotPanelRequest();
//                break;
//            default:
//                break;
//            }
//            IO.Pots.MsgCANFlag = 0;
//        }
        IO_OnPotSrchGainRequest();
        IO_OnPotTiltRequest();
        IO_OnPotBCNGainRequest();

        if(IO.LEDListTimer.Counter == 0) {
            IO_LED_STATUS = !IO_LED_STATUS; //heartbeat
            IO.LEDListTimer.Counter = IO.LEDListTimer.CounterProg;
        }
    }
}

