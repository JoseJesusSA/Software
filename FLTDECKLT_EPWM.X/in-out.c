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
    IO.LEDListTimer.CounterProg = 120; //x4.16ms ~0.5seg

    //evitar enviar mensajes de CAN al inicio por diferencias entre
    //el respaldo de las entradas y su estado actual
    IO.SWI_FWD.Switch.UI8 = 0;
    IO.SWI_FWD.Switch.Bits.Input = IO_ENT_SWI_FWD_ON;       //asignacion entrada
    IO.SWI_FWD.Switch.Bits.InputBack = IO.SWI_FWD.Switch.Bits.Input;
    IO.SWI_FWD.DebounceTimerCounter = 0;
    IO.SWI_FWD.DebounceTimerProg = 26; //x4.16ms, almenos ~10Hz por defecto

    IO.SWI_RWD.Switch.UI8 = 0;
    IO.SWI_RWD.Switch.Bits.Input = IO_ENT_SWI_RWD_ON;
    IO.SWI_RWD.Switch.Bits.InputBack = IO.SWI_RWD.Switch.Bits.Input;
    IO.SWI_RWD.DebounceTimerCounter = 0;
    IO.SWI_RWD.DebounceTimerProg = 26; //x4.16ms, almenos ~10Hz por defecto

    IO.SWI_CSL.Switch.UI8 = 0;
    IO.SWI_CSL.Switch.Bits.Input = IO_ENT_SWI_CSL_ON;
    IO.SWI_CSL.Switch.Bits.InputBack = IO.SWI_CSL.Switch.Bits.Input;
    IO.SWI_CSL.DebounceTimerCounter = 0;
    IO.SWI_CSL.DebounceTimerProg = 26; //x4.16ms, almenos ~10Hz por defecto

    IO.SWI_INST.Switch.UI8 = 0;
    IO.SWI_INST.Switch.Bits.Input = IO_ENT_SWI_INST_ON;
    IO.SWI_INST.Switch.Bits.InputBack = IO.SWI_INST.Switch.Bits.Input;
    IO.SWI_INST.DebounceTimerCounter = 0;
    IO.SWI_INST.DebounceTimerProg = 26; //x4.16ms, almenos ~10Hz por defecto
 
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

