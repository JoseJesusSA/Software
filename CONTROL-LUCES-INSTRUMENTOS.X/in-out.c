/**
 Administracion de entradas-salidas para perifericos del simulador.
*/

#include <p18f46k80.h>
#include "utils.h"
#include "mcu.h"
#include "in-out.h"

volatile struct IOStructure IO;

/**
  Configuracion por defecto de las variables para IOs.
*/
void IO_Init(void) {
    IO.LEDListTimer.Counter = 0;
    IO.LEDListTimer.CounterProg = 90; //x5.56ms

    IO.Pots.Channel = 1;
    IO.Pots.MsgCANFlag = 0;
    IO.Pots.MsgCANState = 0;
}

/**
  Mantiene actualizado el buffer de TURN.
  \todo Aplicar parametros de calibracion.
*/
void IO_PotsListUpdate(void) {
    if(ADC_IsBussy() == FALSE) {
        switch(IO.Pots.Channel) {
        case 1:
            IO.Pots.Panel.Value.UI16 = ADC_Read();
            IO.Pots.Channel++;
            break;
        case 2:
            IO.Pots.LatlCsl.Value.UI16 = ADC_Read();
            IO.Pots.Channel++;
            break;
        case 3:
            IO.Pots.Inst.Value.UI16 = ADC_Read();
            IO.Pots.Channel = 1;
            break;
        default: break;
        }
        ADC_SetChannel(IO.Pots.Channel);
        ADC_WaitForNextSample();
        ADC_START();
    }
}

/**
  Reinicia los contadores de los temporizadores de soporte.
  \note Se puede utilizar con o sin interrupcion.
  \todo Turn con su propio timer?
*/
void IO_TimerBaseOverflow(void) {
    UInt8 i;

    //las siguientes 3 lineas en caso de usar TMR1 sin interrupcion, pasar a main()
//    if(TMR1_OVERFLOW() == FALSE)
//        return;
//    TMR1_Start();

    //actualizar indicadores
    if(IO.LEDListTimer.Counter)
        IO.LEDListTimer.Counter--;
}

