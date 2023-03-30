/**
 Configuracion del modulo ECAN.
 \note Para referencia vea "ECAN Mode 0 with Baud" de los ejemplos de codigo de
 Microchip.
*/

#include <p18f46k80.h>
#include "utils.h"
#include "can.h"

volatile struct CANStructure CAN;

/**
  Configuracion por defecto del modulo CAN.
  Establece la configuracion de los errores, recepcion con mascaras y filtros, y
  transmision de cuadros con sus respectivas prioridades.
  Las mascaras y filtros deben estar previamente establecidos.
*/
void CAN_Init(void) {
    UInt16UInt8 tmpUi16;
    //detener cualquier peticion pendiente
    CAN.BufferReceived = FALSE;

    //modo config, no alvidar cambiar a modo normal
    CAN_SetConfigMode();

    ECANCON = 0; //legacy Mode0

    //interrupciones de error en el modulo CAN, multiples fuentes
    PIR5bits.ERRIF = 0;
    PIE5bits.ERRIE = 0;
    IPR5bits.ERRIP = 1; //alta prioridad

    //interrupcion por recepcion de mensaje no valido
    PIR5bits.IRXIF = 0;
    PIE5bits.IRXIE = 0;
    IPR5bits.IRXIP = 1; //alta prioridad

    //interrupcion TXB0 por mensaje completo
    PIR5bits.TXB0IF = 0;
    PIE5bits.TXB0IE = 0; //administrar sin interrupcion
    IPR5bits.TXB0IP = 1; //alta prioridad
    TXB0CON = 0x03; //TXREQ clear, highest priority 3

    //interrupcion TXB1 por mensaje completo
    PIR5bits.TXB1IF = 0;
    PIE5bits.TXB1IE = 0; //administrar sin interrupcion
    IPR5bits.TXB1IP = 1; //alta prioridad
    TXB1CON = 0x02; //TXREQ clear, priority level 2

    //interrupcion TXB2 por mensaje completo
    PIR5bits.TXB2IF = 0;
    PIE5bits.TXB2IE = 0; //administrar sin interrupcion
    IPR5bits.TXB2IP = 1; //alta prioridad
    TXB2CON = 0x01; //TXREQ clear, priority level 1

    //interrupcion RXB0 por nuevo mensaje recibido
    PIR5bits.RXB0IF = 0;
    PIE5bits.RXB0IE = 1;
    IPR5bits.RXB0IP = 1; //alta prioridad
    RXB0CON = 0b00000000;   //open to receive, standar ID, write buffer 1 NO overflow buffer 2
                            //solo por filtros
    tmpUi16.UI16 = CAN.Config.RX0.Mask.UI16 << 5;
    RXM0SIDH = tmpUi16.UI8[1]; //mask buffer 0
    RXM0SIDL = tmpUi16.UI8[0];
    RXM0SIDH = 0xFF; // Standard ID FILTER
    RXM0SIDL = 0xE0;
    tmpUi16.UI16 = CAN.Config.RX0.Filter0.UI16 << 5;
    RXF0EIDH = 0x00;
    RXF0EIDL = 0x00;
    RXF0SIDH = tmpUi16.UI8[1]; //filtro 0 buffer 0, only standar
    RXF0SIDL = tmpUi16.UI8[0];
    tmpUi16.UI16 = CAN.Config.RX0.Filter1.UI16 << 5;
    RXF1EIDH = 0x00;
    RXF1EIDL = 0x00;
    RXF1SIDH = tmpUi16.UI8[1]; //filtro 1 buffer 0, only standar
    RXF1SIDL = tmpUi16.UI8[0];

    //interrupcion RXB1 por nuevo mensaje recibido
    //los filtros 0 y 1 estan disponibles solo si RXB0CON.RXB0DBEN esta activo
    PIR5bits.RXB1IF = 0;
    PIE5bits.RXB1IE = 1;
    IPR5bits.RXB1IP = 1; //alta prioridad
    RXB1CON = 0b00000000; //open to receive, standar ID, solo por filtros
    tmpUi16.UI16 = CAN.Config.RX1.Mask.UI16 << 5;
    RXM1SIDH = tmpUi16.UI8[1]; //mask buffer 1
    RXM1SIDL = tmpUi16.UI8[0];
    RXM1SIDH = 0xFF;
    RXM1SIDL = 0xE0;
    tmpUi16.UI16 = CAN.Config.RX1.Filter2.UI16 << 5;
    RXF2EIDH = 0x00;
    RXF2EIDL = 0x00;
    RXF2SIDH = tmpUi16.UI8[1]; //filtro 2 buffer 1, only standar
    RXF2SIDL = tmpUi16.UI8[0];
    tmpUi16.UI16 = CAN.Config.RX1.Filter3.UI16 << 5;
    RXF3EIDH = 0x00;
    RXF3EIDL = 0x00;
    RXF3SIDH = tmpUi16.UI8[1]; //filtro 3 buffer 1, only standar
    RXF3SIDL = tmpUi16.UI8[0];
    tmpUi16.UI16 = CAN.Config.RX1.Filter4.UI16 << 5;
    RXF4EIDH = 0x00;
    RXF4EIDL = 0x00;
    RXF4SIDH = tmpUi16.UI8[1]; //filtro 4 buffer 1, only standar
    RXF4SIDL = tmpUi16.UI8[0];
    tmpUi16.UI16 = CAN.Config.RX1.Filter5.UI16 << 5;
    RXF5EIDH = 0x00;
    RXF5EIDL = 0x00;
    RXF5SIDH = tmpUi16.UI8[1]; //filtro 5 buffer 1, only standar
    RXF5SIDL = tmpUi16.UI8[0];

    // Enable Filters
    // Only using two filters
    RXFCON0 = 0x03; //Disable all
    RXFCON1 = 0x00; //Disable all

    //baudrate 500 kb
//    BRGCON1 = 0b00000000; //SJW=1Tq, BRP<5:0>=0, Tq=2*1/FOSC=100ns
//    BRGCON2 = 0b10111110; //sample once, 7Tq, 8Tq
//    BRGCON3 = 0b10000011; //no wake-up, 4Tq
    BRGCON1 = 0b00000001; //SJW=1Tq, BRP<5:0>=1=Tq=2x2/FOSC=200ns
    BRGCON2 = 0b10011010; //sample once, PhaseSeg1=4Tq, Prop.Time=3Tq
    BRGCON3 = 0b10000001; //no wake-up, PhaseSeg2=2Tq

    TRISBbits.TRISB3 = 1; //CANRX entrada
    LATBbits.LATB3 = 1; //evitamos envios al start-up
    TRISBbits.TRISB2 = 0; //CANTX salida
    //CANTX2 IO, CANTX drive VDD when recessive (differential bus),
    //RC2/CCP1 input
    //oscillator as source of CAN clock
    CIOCON = 0b00100001;

    CANCON = 0b00000000; //normal mode, TX normal
    while(CANSTATbits.OPMODE == 0x00);
//    CANCON = 0b01000000; //loop mode, TX normal
}

/**
  Inicializa los buffers y la configuracion CAN de usuario.
*/
void CAN_InitBuffers(void) {

}

/**
  Establece el Bit Timing del CAN a 500k
  FOSC=20MHz, NominalBitTime=10Tq, Tq=200ns
  Synchronization Segment: 1
  Propagation Segment: 3
  Phase Segment 1: 4
  Phase Segment 2: 2
  \note Valores anteriores:
  FOSC=20MHz, NominalBitTime=20Tq
  Synchronization Segment: 1
  Propagation Segment: 7
  Phase Segment 1: 8
  Phase Segment 2: 4
*/
void CAN_SetFreq500k(void) {
    CAN_SetConfigMode();
//    BRGCON1 = 0b00000000; //SJW=1Tq, BRP<5:0>=0, Tq=2*1/FOSC=100ns
//    BRGCON2 = 0b10111110; //sample once, Prop. Time: 7Tq, Phase Seg.1 8Tq
//    BRGCON3 = 0b10000011; //no wake-up, 4Tq

    BRGCON1 = 0b00000001; //SJW=1Tq, BRP<5:0>=1=Tq=2x2/FOSC=200ns
    BRGCON2 = 0b10011010; //sample once, PhaseSeg1=4Tq, Prop.Time=3Tq
    BRGCON3 = 0b10000001; //no wake-up, PhaseSeg2=2Tq
}

/**
  Establece el Bit Timing del CAN a 100k
  FOSC=20MHz, NominalBitTime=20Tq
*/
void CAN_SetFreq100k(void) {
    CAN_SetConfigMode();
    BRGCON1 = 0b00000100; //SJW=1Tq, BRP<5:0>=4, Tq=2*5/FOSC=0.5us
    BRGCON2 = 0b10111110; //sample once, 7tq, 8tq
    BRGCON3 = 0b00000011; //no wake-up, 4Tq
}

/**
  Manda a modo de configuracion al modulo CAN.
  No se honra inmediatamente la peticion.
  El modulo esperara al que el CAN bus este IDLE para hacer el cambio.
*/
void CAN_SetConfigMode(void) {
    CANCON = 0x80; //hacer peticion
    while((CANSTAT & 0x80) == 0); //esperar bus IDLE
}

/**
  Actualiza la estructura global 'CAN' con los datos del buffer RxB0.
  \note Liberar desde aqui el buffer??
*/
void CAN_RX0GetStdFrame(void) {
    CAN.RX0.SID.UI8[1] = RXB0SIDH;
    CAN.RX0.SID.UI8[0] = RXB0SIDL;
    CAN.RX0.SID.UI16 = CAN.RX0.SID.UI16 >> 5;

    CAN.RX0.DLC = RXB0DLC & 0x0f;
    if(RXB0CONbits.RXRTRRO)
        CAN.RX0.RTR = 1;
    else {
        CAN.RX0.RTR = 0;

        CAN.RX0.Data[0] = RXB0D0;
        CAN.RX0.Data[1] = RXB0D1;
        CAN.RX0.Data[2] = RXB0D2;
        CAN.RX0.Data[3] = RXB0D3;
        CAN.RX0.Data[4] = RXB0D4;
        CAN.RX0.Data[5] = RXB0D5;
        CAN.RX0.Data[6] = RXB0D6;
        CAN.RX0.Data[7] = RXB0D7;
    }
    CAN_RX0_FREE();
}

/**
  Actualiza la estructura global 'CAN' con los datos del buffer RxB1.
  \note Liberar desde aqui el buffer??
*/
void CAN_RX1GetStdFrame(void) {
    CAN.RX1.SID.UI8[1] = RXB1SIDH;
    CAN.RX1.SID.UI8[0] = RXB1SIDL;
    CAN.RX1.SID.UI16 = CAN.RX1.SID.UI16 >> 5;

    CAN.RX1.DLC = RXB1DLC & 0x0f;
    if(RXB1CONbits.RXRTRRO)
        CAN.RX1.RTR = 1;
    else {
        CAN.RX1.RTR = 0;

        CAN.RX1.Data[0] = RXB1D0;
        CAN.RX1.Data[1] = RXB1D1;
        CAN.RX1.Data[2] = RXB1D2;
        CAN.RX1.Data[3] = RXB1D3;
        CAN.RX1.Data[4] = RXB1D4;
        CAN.RX1.Data[5] = RXB1D5;
        CAN.RX1.Data[6] = RXB1D6;
        CAN.RX1.Data[7] = RXB1D7;
    }
    CAN_RX1_FREE();
}

/**
  Revisa interrupciones por error y accede al banco correcto de datos para
  procesar los cuadros recibidos correctamente.
*/
void CAN_OnInterrupt(void) {
    UInt8 tmpCANCON;
    UInt8 tmpCANSTAT;

    // Save CANCON.WIN bits. This is required to prevent CANCON from
    // corrupting CAN buffer access in-progress while this interrupt occurred
    tmpCANCON = CANCON;
    // Save CANSTAT register. This is required to make sure that we use same
    // CANSTAT value rather than one changed by another CAN interrupt.
    tmpCANSTAT = CANSTAT;
    // Retrieve ICODE bits
    tmpCANSTAT &= 0b00001110;
    tmpCANSTAT = tmpCANSTAT >> 1;
    // Perform corresponding interrupt cause
    switch(tmpCANSTAT) {
    case 1: { // error interrupt
        //TODO analizar fuentes de error
        PIR5bits.ERRIF = 0;
    } break; // sin cambiar CANCON o CANSTAT
    case 2: { // TXB2 interrupt
        PIR5bits.TXB2IF = 0;
    } break;
    case 3: { // TXB1 interrupt
        PIR5bits.TXB1IF = 0;
    } break;
    case 4: { // TXB0 interrupt
        PIR5bits.TXB0IF = 0;
    } break;
    case 5: { // RXB1 interrupt
        if(CAN.BufferReceived == 0) {
            //TODO identificar el filtro que activo el mensaje
            //TODO Remote Transmission Request??
            CAN_RX1GetStdFrame();
            CAN.BufferReceived = 2;
        } else {
            CAN_RX1_FREE();
        }
        PIR5bits.RXB1IF = 0;
    } break;
    case 6: { // RXB0 interrupt
        if(CAN.BufferReceived == 0) {
            //TODO identificar el filtro que activo el mensaje
            //TODO Remote Transmission Request??
            CAN_RX0GetStdFrame();
            CAN.BufferReceived = 1;
        } else {
            CAN_RX0_FREE();
        }
        PIR5bits.RXB0IF = 0;
    } break;
    case 7: { // Wake-up on interrupt
        //TODO hacer algo
        PIR5bits.WAKIF = 0;
    } break;
    default: break;
    } // fin switch interrupt
}

/**
  Transmite un cuadro 'estandar' CAN usando el buffer TxB0.
  Utiliza la estructura de datos global 'CAN' para construir el mensaje.
  Incluye el envio de cuadros Remote Frame.
  Los datos DATA[0,7] deben estar previamente estabecidos.
*/
void CAN_TX0SendStdFrame(void) {
    TXB0EIDH = 0x00;
    TXB0EIDL = 0x00;

    CAN.TX.SID.UI16 = CAN.TX.SID.UI16 << 5;
    TXB0SIDH = CAN.TX.SID.UI8[1];
    TXB0SIDL = CAN.TX.SID.UI8[0];

    TXB0DLC = CAN.TX.DLC;
    if(CAN.TX.RTR) //remote transfer request?
        TXB0DLCbits.TXRTR = 1;
    else {
        TXB0DLCbits.TXRTR = 0;

        TXB0D0 = CAN.TX.Data[0];
        TXB0D1 = CAN.TX.Data[1];
        TXB0D2 = CAN.TX.Data[2];
        TXB0D3 = CAN.TX.Data[3];
        TXB0D4 = CAN.TX.Data[4];
        TXB0D5 = CAN.TX.Data[5];
        TXB0D6 = CAN.TX.Data[6];
        TXB0D7 = CAN.TX.Data[7];
    }

    //Now that all data bytes are loaded, mark it for transmission.
    TXB0CON = 0b00001011; //Highest priority, Request transmission
}


/**
  Transmite un cuadro 'estandar' CAN usando el buffer TxB1.
  Utiliza la estructura de datos global 'CAN' para construir el mensaje.
  Incluye el envio de cuadros Remote Frame.
  Los datos DATA[0,7] deben estar previamente estabecidos.
*/
void CAN_TX1SendStdFrame(void) {
    TXB1EIDH = 0x00;
    TXB1EIDL = 0x00;

    CAN.TX.SID.UI16 = CAN.TX.SID.UI16 << 5;
    TXB1SIDH = CAN.TX.SID.UI8[1];
    TXB1SIDL = CAN.TX.SID.UI8[0];

    TXB1DLC = CAN.TX.DLC;
    if(CAN.TX.RTR) //remote transfer request?
        TXB1DLCbits.TXRTR = 1;
    else {
        TXB1DLCbits.TXRTR = 0;

        TXB1D0 = CAN.TX.Data[0];
        TXB1D1 = CAN.TX.Data[1];
        TXB1D2 = CAN.TX.Data[2];
        TXB1D3 = CAN.TX.Data[3];
        TXB1D4 = CAN.TX.Data[4];
        TXB1D5 = CAN.TX.Data[5];
        TXB1D6 = CAN.TX.Data[6];
        TXB1D7 = CAN.TX.Data[7];
    }

    //Now that all data bytes are loaded, mark it for transmission.
    TXB1CON = 0b00001010; //priority level 2, Request transmission
}

/**
  Transmite un cuadro 'estandar' CAN usando el buffer TxB2.
  Utiliza la estructura de datos global 'CAN' para construir el mensaje.
  Incluye el envio de cuadros Remote Frame.
  Los datos DATA[0,7] deben estar previamente estabecidos.
*/
void CAN_TX2SendStdFrame(void) {
    TXB2EIDH = 0x00;
    TXB2EIDL = 0x00;

    CAN.TX.SID.UI16 = CAN.TX.SID.UI16 << 5;
    TXB2SIDH = CAN.TX.SID.UI8[1];
    TXB2SIDL = CAN.TX.SID.UI8[0];

    TXB2DLC = CAN.TX.DLC;
    if(CAN.TX.RTR) //remote transfer request?
        TXB2DLCbits.TXRTR = 1;
    else {
        TXB2DLCbits.TXRTR = 0;

        TXB2D0 = CAN.TX.Data[0];
        TXB2D1 = CAN.TX.Data[1];
        TXB2D2 = CAN.TX.Data[2];
        TXB2D3 = CAN.TX.Data[3];
        TXB2D4 = CAN.TX.Data[4];
        TXB2D5 = CAN.TX.Data[5];
        TXB2D6 = CAN.TX.Data[6];
        TXB2D7 = CAN.TX.Data[7];
    }

    //Now that all data bytes are loaded, mark it for transmission.
    TXB2CON = 0b00001001; //priority level 1, Request transmission
}

/**
  Decide cual buffer de transmision esta libre para enviar mensajes.
  \returns [0,3]: 1-buffer0, 2-buffer1, 3-buffer2, 0 sin disponibilidad
*/
//UInt8 CAN_TXGetFreeBuffer(void) {
//    if(TXB0CONbits.TXREQ == FALSE)
//        return 1;
//    if(TXB1CONbits.TXREQ == FALSE)
//        return 2;
//    if(TXB2CONbits.TXREQ == FALSE)
//        return 3;
//    return 0;
//}

/**
  Envia el un cuadro estandar en el primer buffer disponible.
  El buffer TXB0 tiene mayor prioridad igual a la forma en que se programaron.
*/
void CAN_TXSendStdFrame(void) {
    if(TXB0CONbits.TXREQ == FALSE) {
        CAN_TX0SendStdFrame();
        return;
    }
    if(TXB1CONbits.TXREQ == FALSE) {
        CAN_TX1SendStdFrame();
        return;
    }
    if(TXB2CONbits.TXREQ == FALSE) {
        CAN_TX2SendStdFrame();
        return;
    }
}
