/**
 Configuracion del microcontrolador y sus perifericos internos.
*/

#include <p18f46k80.h>
#include "utils.h"
#include "mcu.h"

/**
 Configuracion por defecto del MCU.
 Mantiene como salidas las IO no utilizadas y en nivel bajo.
 Desactiva modulos sin utilizar mediante los registros PMDx para disminuir consumo.
 Los SFR de modulos desactivados se leen como 0x00.
*/
void MCU_Init(void) {
    INTCONbits.GIE = 0;
    
    //BIT7=ANSEL7
    //BIT6=ANSEL6
    //BIT5=ANSEL5
    //BIT4=ANSEL4
    //BIT3=ANSEL3
    //BIT2=ANSEL2
    //BIT1=ANSEL1
    //BIT0=ANSEL0
    ANCON0=0b00010111;
    
    //BIT7=UNIMPLEMENTED
    //BIT6=ANSEL14 RD3
    //BIT5=ANSEL13 RD2
    //BIT4=ANSEL12 RD1
    //BIT3=ANSEL11 RD0
    //BIT2=ANSEL11   AN10
    //BIT1=ANSEL11   AN10
    //BIT0=ANSEL11  AN10
    ANCON1=0b00000000;
    

    //RA5=CSL
    //RA3
    //RA2=FWD
    //RA1=RWD
    //RA0=INST
    TRISA = 0b00101111;
    LATA = 0;

    //RB7=PGD,DS1
    //RB6=PGC,DS2
    //RB5=STAT
    //RB4=COMM
    //RB3
    //RB2
    //RB1
    //RB0
    TRISB = 0b00000000;
    LATB = 0;

    //RC7=CANRX
    //RC6=CANTX
    //RC5=swRWD
    //RC4=swFWD
    //RC3
    //RC2
    //RC1
    //RC0
    TRISC = 0b11110000;
    LATC = 0;

    //RD7
    //RD6
    //RD5
    //RD4=PWMCE
    //RD3=swINST
    //RD2=swCSL
    //RD1
    //RD0
    TRISD = 0b00001100;
    LATD = 0;

    //RE2
    //RE1
    //RE0
    TRISE = 0b00000000;
    LATE = 0;

    // desactivar modulos sin uso
    //BIT3=MODMD, solo en dispositivos con 64 pines
    //BIT2=ECANMD
    //BIT1=CMP2MD
    //BIT0=CMP1MD
    PMD2 = 0b00001011;

    //BIT7=PSPMD
    //BIT6=CTMUMD
    //BIT5=ADCMD
    //BIT4=TMR4MD
    //BIT3=TMR3MD
    //BIT2=TMR2MD
    //BIT1=TMR1MD
    //BIT0=TMR0MD
    PMD1 = 0b11011000;

    //BIT7=CCP5MD
    //BIT6=CCP4MD
    //BIT5=CCP3MD
    //BIT4=CCP2MD
    //BIT3=CCP1MD
    //BIT2=UART2MD
    //BIT1=UART1MD
    //BIT0=SSPMD
    PMD0 = 0b11110111;
}

/**
 Activa ADC
 */

void ADC_Init(void){
    /*
     BIT7=Unimplemented
     BIT6-2=AN0 first convertion
     BIT1=NO PROGRESO
     BIT0=MODULE ON
     */
    ADCON0 = 0;
    
    /*
     BIT7-6=TIMER 1
     BIT5-4=AVDD
     BIT3=AVSS
     BIT2-0=AVSS*/
    //ADCON1 = 0b10000000;
    //ADCON1bits.TRIGSEL=2;
    ADCON1bits.VCFG=0;
    ADCON1bits.VNCFG=0;
    ADCON1bits.CHSN=0;
    
    /*
    BIT7=JUSTIFICADO DERECHA
    BIT6=UNIMPLEMENTED
    BIT5-3=4TAD
    BIT2-0=FOSC/16
     */
    ADCON2=0b10010101;
    
    ADRESH=0x000;
    ADRESL=0x000;
    
    ADCON0bits.ADON = 1;    //Turn the module
    PIR1bits.ADIF = 0;      //Limpia interrupción
    PIE1bits.ADIE = 1;       //Habilita interrupcion
    IPR1bits.ADIP = 0;        //Low priority
    
    ADCON0bits.CHS = 0;        //select chanel 0
    
    //T1GCONbits.T1GGO_NOT_T1DONE=1;  //Inicia por Timer1
    
    ADCON0bits.GO = 1;   //Inicia modulo solo
}

/**
 Activa interrupciones altas y bajas.
 \note Para que GIEL funcione, GIEH debe estar activa.
*/
void MCU_EnableInterrupts(void) {
    RCONbits.IPEN = 1; //enable priority levels
    INTCONbits.GIEH = 1; //activar interrupciones altas
    INTCONbits.GIEL = 1; //activar interrupciones bajas
}

/**
 Desactiva interrupciones altas y bajas.
 \note Para que GIEL funcione, GIEH debe estar activa.
*/
void MCU_DisableInterrupts(void) {
    INTCONbits.GIEL = 0; // interrupciones bajas
    INTCONbits.GIEH = 0; // interrupciones altas
}

/**
 Configuracion por defecto para el PWM ECCP1.
 Se usa la base de tiempo de TMR2.
*/
void PWM1_Init(void) {
    TRISDbits.TRISD4 = 1; //disable output

    // select 8-bit timer resource
    CCPTMRS = 0; // TMR2 se usa por defecto

    // load period value
    T2CON = 0x00; // tmr2 prescaler = 4
    T2CONbits.TMR2ON = 1; // tmr2 enable
    PR2 = 0xff; // 1/(pr2+1 x 4/fosc x tmr2pre) -> 4882.81Hz@20Mhz

    // configure module
    // PWM mode, single output, duty cycle bit1, bit0 (bit[5:4])
    // PWM mode: P1A and P1C are active-high; P1B and P1D are active-high
    CCP1CON = 0b00001100;   //selecciona PWM1 como pwm
    // P1A pin has the PWM waveform active high
    PSTR1CON = 0b00010001;

    // load 10-bit duty cycle value load 
    CCPR1L = 0;

    // enable PWM outputs
    TRISDbits.TRISD4 = 0;
}

/**
 Confirma que el buffer de salida ha sido actualizado.
 Evita perder valores en el buffer de salida.
*/
void PWM1_ConfirmDutyCycle(void) {
    while(CCPR1H != CCPR1L);// tmr2 and pr2 match
}

/**
Detiene la salida PWM.
*/
void PWM1_Close(void) {
    CCP1CON = 0;
    TRISBbits.TRISB4 = 1; //dejar en alta impedancia??
}

void PWM1_Write(UInt16 pwm) {
    if(pwm & 0x01)
        CCP1CONbits.DC1B0 = 1;
    else
        CCP1CONbits.DC1B0 = 0;

    if(pwm & 0x02)
        CCP1CONbits.DC1B1 = 1;
    else
        CCP1CONbits.DC1B1 = 0;

    pwm >>= 2;
    CCPR1L = pwm;
}

/**
 Configuracion por defecto para el PWM CCP3.
 Se usa la base de tiempo de ??.
 \note La resolucion se calcula como Res = log(Fosc/Fpwm) / log(2) [bits], sin embargo, el valor de
 PR2 (TMR2) limita la maxima cantidad de bits que se pueden asignar. Por ejemplo, con prescaler=4,
 si PR2=63=0b00111111 mas los dos bits DC3b1 y DC3B0, limitaran a una escritura de 8 bits pese poder
 obtener hasta 10 bits de resolucion.
 Para este caso se recomienda usar el prescaler=1 para obtener los 10 bits.
*/
void PWM3_Init(void) {
    TRISCbits.TRISC6 = 1; //disable output

    // select 8-bit timer resource
    CCPTMRS = 0; // TMR2 se usa por defecto

    // load period value
    //T2CON = 0x01; // tmr2 prescaler = 4
    T2CON = 0x00; // tmr2 prescaler = 1
    T2CONbits.TMR2ON = 1; // tmr2 enable
    //PR2 = 0xff; // 1/(pr2+1 x 4/fosc x tmr2pre) -> 4882.81 Hz @20Mhz, prescaler=4
    //PR2 = 63; // 1/(pr2+1 x 4/fosc x tmr2pre) -> 19531.25 Hz @20Mhz, prescaler=4
    PR2 = 0xFF; // 1/(pr2+1 x 4/fosc x tmr2pre) -> 19531.25 Hz @20Mhz, prescaler=1

    // configure module
    // PWM mode
    // duty cycle bit1, bit0 (bit[5:4])
    CCP3CON = 0b00001100;

    // load 10-bit duty cycle value
    CCPR3L = 0; //solo 6 bits de CCPRL influyen con prescaler=4 y PR2=63, sumando DC3Bx son 8 bits

    // enable PWM outputs
    TRISCbits.TRISC6 = 0;
}

/**
Carga los 10 bits del PWM de CCP3.
La escritura se ajusta para utilizar solo 8 bits significativos a la frecuencia de 19531.25Hz.
\param pwm [0x00,0x3FF]: el ancho de pulso de 8 bits.
\note Ver esquematico y especificaciones de MOSFET para analisis de maxima frecuencia de trabajo.
*/
void PWM3_Write(UInt16 pwm) {
    if(pwm & 0x01)
        CCP3CONbits.DC3B0 = 1;
    else
        CCP3CONbits.DC3B0 = 0;

    if(pwm & 0x02)
        CCP3CONbits.DC3B1 = 1;
    else
        CCP3CONbits.DC3B1 = 0;

    pwm >>= 2;
    CCPR3L = pwm;
}

/**
 Configuracion por defecto para el PWM CCP4.
 \note Ver PWM3_Init().
*/
void PWM4_Init(void) {
    TRISCbits.TRISC7 = 1; //disable output

    // select 8-bit timer resource
    CCPTMRS = 0; // TMR2 se usa por defecto

    // load period value
    //T2CON = 0x01; // tmr2 prescaler = 4
    T2CON = 0x00; // tmr2 prescaler = 1
    T2CONbits.TMR2ON = 1; // tmr2 enable
    //PR2 = 0xff; // 1/(pr2+1 x 4/fosc x tmr2pre) -> 4882.81 Hz @20Mhz, prescaler=4
    //PR2 = 63; // 1/(pr2+1 x 4/fosc x tmr2pre) -> 19531.25 Hz @20Mhz, prescaler=4
    PR2 = 0xFF; // 1/(pr2+1 x 4/fosc x tmr2pre) -> 19531.25 Hz @20Mhz, prescaler=1

    // configure module
    // PWM mode
    // duty cycle bit1, bit0 (bit[5:4])
    CCP4CON = 0b00001100;

    // load 10-bit duty cycle value
    CCPR4L = 0; //solo 6 bits de CCPRL influyen con prescaler=4 y PR2=63, sumando DC3Bx son 8 bits

    // enable PWM outputs
    TRISCbits.TRISC7 = 0;
}

/**
Carga los 10 bits del PWM de CCP4.
\note Ver PWM3_Init().
*/
void PWM4_Write(UInt16 pwm) {
    if(pwm & 0x01)
        CCP4CONbits.DC4B0 = 1;
    else
        CCP4CONbits.DC4B0 = 0;

    if(pwm & 0x02)
        CCP4CONbits.DC4B1 = 1;
    else
        CCP4CONbits.DC4B1 = 0;

    pwm >>= 2;
    CCPR4L = pwm;
}

/**
 Configurcion por defecto del bus SPI.
 El driver SPI esta implementado por control de software.
 \note Configuracion para los displays DOGM132 y DOGL128.
 \todo Parametros para SPI_DI, SPI_DO, SPI_CLOCK
*/
void SPI_Init(void) {
    TRISCbits.TRISC3 = 0; //define clock pin as output
    TRISCbits.TRISC5 = 0; //define SDO as output (master or slave)
    TRISCbits.TRISC4 = 1; //define SDI as input (master or slave)

    SPI_CLOCK = 1; //IDLE high
    SPI_DO = 0;
    SPI_DI = 0;
}

/**
 Escribe/lee un byte al bus SPI para los displays DOGL128 y DOGM132.
 Los LCD DOG leen bits en flanco de subida con el reloj IDLE en alto.
 \param data_out Byte a escribir al bus. Se ignora en la lectura.
 \return Byte leido del bus.
 \note Doble Nop() para 16MHz, un Nop() para 8MHz
*/
UInt8 SPI_WriteReadBus(UInt8 dataOut) {
    UInt8 dataIn;
    UInt8 i;

    dataIn = 0;
    //Delay10TCYx(1); //varia con interrupciones
    Nop(); Nop();

    for(i = 0; i < 8; i++) {
        SPI_CLOCK = 0; Nop(); Nop();
        if(dataOut & 0x80) { //envia MSB
            SPI_DO = 1;
        } else {
            SPI_DO = 0;
        }
        dataOut <<= 1;

        if(SPI_DI) {
            dataIn++; //read LSB
        }
        if(i < 7) {
            dataIn <<= 1;
        }

        SPI_CLOCK = 1; Nop(); Nop(); //Delay10TCYx(1); // 5us@8MHz
        //SPI_CLOCK = 0; Nop(); Nop(); //Delay10TCYx(1);
    }
    return (dataIn);
}

/**
\todo Ecuaciones de temporizador.
*/
void TMR0_Init(void) {
    T0CON = 0x03; // 1:16 prescaler, 16-bit, don't start it yet
//    TMR0H = 0xC2;
//    TMR0L = 0xF6; // 50ms@20MHz, 1:16 prescaler
//    TMR0H = 0x85;
//    TMR0L = 0xED; // 100ms@20MHz, 1:16 prescaler
//    TMR0H = 0x0B;
//    TMR0L = 0xDB; // 200ms@20MHz, 1:16 prescaler
//    TMR0H = 0xF3;
//    TMR0L = 0xCA; //10ms@20MHz, 1:16 prescaler

    TMR0H = 0xFB;
    TMR0L = 0xA7; //4.7ms@20MHz, 1:16 prescaler
    
//	T0CON = 0x06; // 1:128 prescaler, don't start it yet
//	TMR0H = 0x85;
//	TMR0L = 0xed; //1s@16MHz, 1:128 prescaler

    INTCONbits.TMR0IF = 0; // clear Timer0 overflow flag
    INTCON2bits.TMR0IP = 0; // baja prioridad
    INTCONbits.TMR0IE = 1; // activa interrupcion
    T0CONbits.TMR0ON = 1;
}

/**
\todo Ecuaciones de temporizador.
*/
void TMR1_Init(void) {
    T1CON = 0; //dont start yet

    T1CONbits.SOSCEN = 0; //SOSC disable
    T1CONbits.TMR1CS = 0; //clock=FOSC/4
    T1CONbits.T1CKPS = 2; //1:4 prescaler
    T1CONbits.RD16 = 1;

//    TMR1H = 0xFB;
//    TMR1L = 0x1D; //1ms@20MHz, 1:4 prescaler
//    TMR1H = 0xCF;
//    TMR1L = 0x2B; //10ms@20MHz, 1:4 prescaler
//    TMR1H = 0xE7;
//    TMR1L = 0x95; //5ms@20MHz, 1:4 prescaler
    //TMR1H = 0xF3;
    //TMR1L = 0xCA; //2.5ms@20MHz, 1:4 prescaler
    TMR1H = 0xAE;
    TMR1L = 0x99; //16.67ms@20MHz, 1:4 prescaler
    
    PIR1bits.TMR1IF = 0; // clear Timer0 overflow flag
    IPR1bits.TMR1IP = 0; // baja prioridad
    PIE1bits.TMR1IE = 0; // no activa interrupcion
    T1CONbits.TMR1ON = 0; //activar solo en cambio de entradas
}

/**
  Reinicia TMR1 sin control de interrupciones.
*/
void TMR1_Start(void) {
    PIR1bits.TMR1IF = 0; // clear Timer0 overflow flag
    TMR1H = 0xAE;
    TMR1L = 0x99; //16.67ms@20MHz, 1:4 prescaler
//    TMR1H = 0xE7;
//    TMR1L = 0x95; //5ms@20MHz, 1:4 prescaler
//    TMR1H = 0xFB;
//    TMR1L = 0x1D; //1ms@20MHz, 1:4 prescaler
//    TMR1H = 0xCF;
//    TMR1L = 0x2B; //10ms@20MHz, 1:4 prescaler
    T1CONbits.TMR1ON = 1;
}

/**
  Detiene el temporizador TMR1.
*/
void TMR1_Stop(void) {
    PIR1bits.TMR1IF = 0; // clear Timer0 overflow flag
    T1CONbits.TMR1ON = FALSE;
}

/**
  Configura la interrupcion externa INT0.
  INT0 siempre es de alta prioridad.
*/
void INT0_Init(void) {
    TRISBbits.TRISB0 = 1; // input
    INTCON2bits.INTEDG0 = 1; // rising edge
    INTCONbits.INT0IF = 0; // clear flag
    INTCONbits.INT0IE = 1; // enable interrupt
}

/**
  Configura la interrupcion externa INT1.
*/
void INT1_Init(void) {
    TRISBbits.TRISB1 = 1; // input
    INTCON2bits.INTEDG1 = 1; // rising edge
    INTCON3bits.INT1IF = 0; // clear flag
    INTCON3bits.INT1IP = 1; // high priority
    INTCON3bits.INT1IE = 1; // enable interrupt
}

/**
  Configura la interrupcion externa INT2.
*/
void INT2_Init(void) {
    TRISBbits.TRISB2 = 1; // input
    INTCON2bits.INTEDG2 = 1; // rising edge
    INTCON3bits.INT2IF = 0; // clear flag
    INTCON3bits.INT2IP = 1; // high priority
    INTCON3bits.INT2IE = 1; // enable interrupt
}

/**
  Configura la interrupcion externa INT3.
*/
void INT3_Init(void) {
    TRISBbits.TRISB3 = 1; // input
    INTCON2bits.INTEDG3 = 1; // rising edge
    INTCON3bits.INT3IF = 0; // clear flag
    INTCON2bits.INT3IP = 1; // high priority
    INTCON3bits.INT3IE = 1; // enable interrupt
}

/**
 Escribe en la memoria de datos interna.
 \note Ver datasheet para tiempo necesario de escrituta (4ms).
 \note 1024 bytes para PIC18F66K80.
*/
void EEPROM_WriteByte(UInt16 address, UInt8 value) {
    EEADRH = (address >> 8) & 0x03;
    EEADR = (address & 0x0ff);
    EEDATA = value;

    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS = 0;
    EECON1bits.WREN = 1;

    //EECON1 = 0x04;
    INTCONbits.GIE = 0;
    EECON2 = 0x55;
    EECON2 = 0xaa;
    EECON1bits.WR = 1;
    while(EECON1bits.WR); //Wait till the write completion
    EECON1bits.WREN = 0;
    //PIR2bits.EEIF = 0;
    INTCONbits.GIE = 1;

    //Delay1KTCYx(16); // 4ms@16MHz clock necesario?
}

/**
  Escribe y verifica que un byte se escribio correctamente en la EEPROM.
*/
UInt8 EEPROM_ValidWriteByte(UInt16 address, UInt8 value) {
    EEPROM_WriteByte(address, value);
    if(EEPROM_ReadByte(address) == value)
        return TRUE;
    else
        return FALSE;
}

/**
 Lee de la memoria de datos interna.
 \note 1024 bytes para PIC18F66K80.
*/
UInt8 EEPROM_ReadByte(UInt16 address) {
    EEADRH = (address >> 8) & 0x03;
    EEADR = (address & 0x0ff);
    EECON1bits.CFGS = 0;
    EECON1bits.EEPGD = 0;
    EECON1bits.RD = 1;
    Nop(); //Nop may be required for latency at high frequencies
    Nop(); //Nop may be required for latency at high frequencies
    return (EEDATA); // return with read byte
}

/**
  Refresca la memoria de datos EEPROM.
  \note 1024 bytes para PIC18F66K80.
  \todo Depurar break de lazo do.
*/
void EEPROM_Refresh(void) {
    EEADR = 0;
    EEADRH = 0;
    EECON1bits.CFGS = 0; //set for memory
    EECON1bits.EEPGD = 0; //set for data EEPROM
    INTCONbits.GIE = 0; //disable interrupts
    EECON1bits.WREN = 1; //enable writes

    //loop to refresh array
    do {
        EECON1bits.RD = 1; //read current address
        EECON2 = 0x55;
        EECON2 = 0xaa;
        EECON1bits.WR = 1; //set WR bit to begin write
        while(EECON1bits.WR); //Wait till the write completion
        EEADR++; //increment address
        if(EEADR == 0) {
          EEADRH++;
          if(EEADRH == 0)
              break;
        }
    } while(1);

    EECON1bits.WREN = 0; //disable writes
    INTCONbits.GIE = 1; //enable interrupts
}
