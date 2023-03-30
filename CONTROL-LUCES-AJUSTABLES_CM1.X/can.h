/**
  \file TODO
*/

#define CAN_TX0_SEND() TXB0CONbits.TXREQ = 1
#define CAN_TX1_SEND() TXB1CONbits.TXREQ = 1
#define CAN_TX2_SEND() TXB2CONbits.TXREQ = 1

#define CAN_TX0_ABORT() TXB0CONbits.TXREQ = 0
#define CAN_TX1_ABORT() TXB1CONbits.TXREQ = 0
#define CAN_TX2_ABORT() TXB2CONbits.TXREQ = 0

#define CAN_NORMAL() CANCON = 0x00
#define CAN_RX0_FREE() RXB0CONbits.RXFUL = 0
#define CAN_RX1_FREE() RXB1CONbits.RXFUL = 0

/**
  Acceso especial a buffer TX/RX.
  \note Ver datasheet de PIC18F66K80 pg. 396.
*/
// This is either TX or RX interrupt
// Clear CANCON.WIN bits before copying new ones. Use previously saved
// CANCON value to make sure same value.
// Copy masked value back to TempCANCON
// Retrieve ICODE bits. Use previously saved CANSTAT value to make sure
// same value.
// Copy ICODE bits to WIN bits. Copy the result to actual CANCON
#define CAN_ACCESS_BUFFER(tmpCANCON, tmpSTATUS) { \
    tmpCANCON &= 0b11110001;        \
    tmpCANSTAT = tmpCANSTAT << 1;   \
    tmpCANCON |= tmpCANSTAT;        \
    CANCON = tmpCANCON;             \
    }

/**
  Restaura CANCON despues de acceder a buffer TX/RX.
  \note Ver datasheet de PIC18F66K80 pg. 396.
*/
// Restore CANCON.WIN bits. Preserve current non WIN bits.
// Restore original WIN bits
// Do not need to restore CANSTAT - it is read-only register.
// Return from interrupt or check for another module interrupt source
#define CAN_UNACCESS_BUFFER(tmpCANCON) { \
    CANCON &= 0b11110001;   \
    CANCON |= tmpCANCON;    \
    }


/**
  Buffer de campos para cuadros estandar CAN.
*/
struct CANTXStructure {
    UInt16UInt8 SID;
    UInt16 SIDBack;
    UInt8 DLC;
    UInt8 RTR;
    UInt8 TXRequest;
    UInt8 Data[8];
};

/**
  Buffer de campos para cuadros estandar CAN.
*/
struct CANRXStructure {
    UInt16UInt8 SID;
    UInt8 DLC;
    UInt8 RTR;
    UInt8 TXRequest;
    UInt8 Data[8];
};

/**
  Mascaras y filtros de configuracion para buffers RX.
*/
struct CANRXConfigStructure{
    struct RX0Config {
        UInt16UInt8 Mask;
        UInt16UInt8 Filter0;
        UInt16UInt8 Filter1;
    } RX0;
    struct RX1Config {
        UInt16UInt8 Mask;
        UInt16UInt8 Filter2;
        UInt16UInt8 Filter3;
        UInt16UInt8 Filter4;
        UInt16UInt8 Filter5;
    } RX1;
};

/**
  Estructura compartida global CAN con los datos de todos los buffers Tx y RX.
*/
struct CANStructure {
    struct CANTXStructure TX; //solo usar de forma secuencial, sin interrupciones

    struct CANRXStructure RX0;
    struct CANRXStructure RX1;

    struct CANRXConfigStructure Config;

    UInt8 BufferReceived; //TODO Eliminar, pasar _OnInterrupt a main()
};

void CAN_Init(void);
void CAN_SetFreq500k(void);
void CAN_SetFreq100k(void);
void CAN_SetConfigMode(void);
void CAN_RX0GetStdFrame(void);
void CAN_RX1GetStdFrame(void);
void CAN_RequestConfigCheck(void);
void CAN_TX0SendStdFrame(void);
void CAN_TX1SendStdFrame(void);
void CAN_TX2SendStdFrame(void);
void CAN_OnInterrupt(void);
//UInt8 CAN_TXGetFreeBuffer(void);
void CAN_TXSendStdFrame(void);
