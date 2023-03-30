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
    IO.LEDListTimer.CounterProg = 50; //x10ms

    //evitar enviar mensajes de CAN al inicio por diferencias entre
    //el respaldo de las entradas y su estado actual
    IO.BUT_PTT.Inputs.UI8 = 0;
    IO.BUT_PTT.Inputs.Bits.BRT = IO_ENT_BUT_PTT_BRT;

    IO.BUT_PTT.Inputs.Bits.DIM = IO_ENT_BUT_PTT_DIM;
    IO.BUT_PTT.InputsBack = IO.BUT_PTT.Inputs.UI8;
    IO.BUT_PTT.DebounceTimerCounter = 0;
    IO.BUT_PTT.DebounceTimerProg = 11; //x10ms, almenos 10Hz por defecto

    IO.SWI_GA.Switch.UI8 = 0;
    IO.SWI_GA.Switch.Bits.Input = IO_ENT_GA_ON;
    IO.SWI_GA.Switch.Bits.InputBack = IO.SWI_GA.Switch.Bits.Input;
    IO.SWI_GA.DebounceTimerCounter = 0;
    IO.SWI_GA.DebounceTimerProg = 11; //x10ms, almenos 10Hz por defecto

    IO.SWI_AP.Switch.UI8 = 0;
    IO.SWI_AP.Switch.Bits.Input = IO_ENT_AP_ON;
    IO.SWI_AP.Switch.Bits.InputBack = IO.SWI_AP.Switch.Bits.Input;
    IO.SWI_AP.DebounceTimerCounter = 0;
    IO.SWI_AP.DebounceTimerProg = 11; //x10ms, almenos 10Hz por defecto

    IO.TIMM_RIGHT.Switch.UI8 = 0;
    IO.TIMM_RIGHT.Switch.Bits.Input = IO_TIMM_RIGHT_ON;
    IO.TIMM_RIGHT.Switch.Bits.InputBack = IO.TIMM_RIGHT.Switch.Bits.Input;
    IO.TIMM_RIGHT.DebounceTimerCounter = 0;
    IO.TIMM_RIGHT.DebounceTimerProg = 11; //x10ms, almenos 10Hz por defecto

    IO.TIMM_LEFT.Switch.UI8 = 0;
    IO.TIMM_LEFT.Switch.Bits.Input = IO_ENT_TIMM_LEFT_ON;
    IO.TIMM_LEFT.Switch.Bits.InputBack = IO.TIMM_LEFT.Switch.Bits.Input;
    IO.TIMM_LEFT.DebounceTimerCounter = 0;
    IO.TIMM_LEFT.DebounceTimerProg = 11; //x10ms, almenos 10Hz por defecto

    IO.TIMM_UP.Switch.UI8 = 0;
    IO.TIMM_UP.Switch.Bits.Input = IO_ENT_TIMM_UP_ON;
    IO.TIMM_UP.Switch.Bits.InputBack = IO.TIMM_UP.Switch.Bits.Input;
    IO.TIMM_UP.DebounceTimerCounter = 0;
    IO.TIMM_UP.DebounceTimerProg = 11; //x10ms, almenos 10Hz por defecto

    IO.TIMM_DOWN.Switch.UI8 = 0;
    IO.TIMM_DOWN.Switch.Bits.Input = IO_ENT_TIMM_DOWN_ON;
    IO.TIMM_DOWN.Switch.Bits.InputBack = IO.TIMM_DOWN.Switch.Bits.Input;
    IO.TIMM_DOWN.DebounceTimerCounter = 0;
    IO.TIMM_DOWN.DebounceTimerProg = 11; //x10ms, almenos 10Hz por defecto

    IO.SWI_SYN.Switch.UI8 = 0;
    IO.SWI_SYN.Switch.Bits.Input = IO_ENT_SWI_SYN_ON;
    IO.SWI_SYN.Switch.Bits.InputBack = IO.SWI_SYN.Switch.Bits.Input;
    IO.SWI_SYN.DebounceTimerCounter = 0;
    IO.SWI_SYN.DebounceTimerProg = 11; //x10ms, almenos 10Hz por defecto
 
}

/**
  Reinicia los contadores de los temporizadores de soporte.
  \note Se puede utilizar con o sin interrupcion.
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

