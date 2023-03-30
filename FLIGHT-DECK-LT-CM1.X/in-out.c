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

    IO.PotSwitchs.Inst.Switch.UI8 = 0;
    IO.PotSwitchs.Inst.Switch.Bits.Input = IO_ENT_SW_INST;
    IO.PotSwitchs.Inst.Switch.Bits.InputBack = IO.PotSwitchs.Inst.Switch.Bits.Input;
    IO.PotSwitchs.Inst.DebounceTimerCounter = 0;
    IO.PotSwitchs.Inst.DebounceTimerProg = 18; //x5.56ms

    IO.PotSwitchs.LatlCsl.Switch.UI8 = 0;
    IO.PotSwitchs.LatlCsl.Switch.Bits.Input = IO_ENT_SW_LATL_CSL;
    IO.PotSwitchs.LatlCsl.Switch.Bits.InputBack = IO.PotSwitchs.LatlCsl.Switch.Bits.Input;
    IO.PotSwitchs.LatlCsl.DebounceTimerCounter = 0;
    IO.PotSwitchs.LatlCsl.DebounceTimerProg = 18; //x5.56ms

    IO.PotSwitchs.Panel.Switch.UI8 = 0;
    IO.PotSwitchs.Panel.Switch.Bits.Input = IO_ENT_SW_PANEL;
    IO.PotSwitchs.Panel.Switch.Bits.InputBack = IO.PotSwitchs.Panel.Switch.Bits.Input;
    IO.PotSwitchs.Panel.DebounceTimerCounter = 0;
    IO.PotSwitchs.Panel.DebounceTimerProg = 18; //x5.56ms

    IO.Pots.Inst.Value.UI16 = 0;
    IO.Pots.Inst.ParamMax.UI16 = 0; //TODO: leer de eeprom
    IO.Pots.Inst.ParamMin.UI16 = 0;
    IO.Pots.Inst.Counter = 0;
    IO.Pots.Inst.CounterProg = 0; //x5.56ms

    IO.Pots.LatlCsl.Value.UI16 = 0;
    IO.Pots.LatlCsl.ParamMax.UI16 = 0; //TODO: leer de eeprom
    IO.Pots.LatlCsl.ParamMin.UI16 = 0;
    IO.Pots.LatlCsl.Counter = 0;
    IO.Pots.LatlCsl.CounterProg = 0; //x5.56ms

    IO.Pots.Panel.Value.UI16 = 0;
    IO.Pots.Panel.ParamMax.UI16 = 0; //TODO: leer de eeprom
    IO.Pots.Panel.ParamMin.UI16 = 0;
    IO.Pots.Panel.Counter = 0;
    IO.Pots.Panel.CounterProg = 0; //x5.56ms

    IO.Pots.Channel = 1;
    IO.Pots.MsgCANFlag = 0;
    IO.Pots.MsgCANState = 0;

    IO.Switches.FloorPedal.Switch.UI8 = 0;
    IO.Switches.FloorPedal.Switch.Bits.Input = IO_ENT_SW_FLOOR_PEDAL;
    IO.Switches.FloorPedal.Switch.Bits.InputBack = IO.Switches.FloorPedal.Switch.Bits.Input;
    IO.Switches.FloorPedal.DebounceTimerCounter = 0;
    IO.Switches.FloorPedal.DebounceTimerProg = 18; //x5.56ms

    IO.Switches.Map.Switch.UI8 = 0;
    IO.Switches.Map.Switch.Bits.Input = IO_ENT_SW_MAP;
    IO.Switches.Map.Switch.Bits.InputBack = IO.Switches.Map.Switch.Bits.Input;
    IO.Switches.Map.DebounceTimerCounter = 0;
    IO.Switches.Map.DebounceTimerProg = 18; //x5.56ms

    IO.Switches.Reading.Switch.UI8 = 0;
    IO.Switches.Reading.Switch.Bits.Input = IO_ENT_SW_READING;
    IO.Switches.Reading.Switch.Bits.InputBack = IO.Switches.Reading.Switch.Bits.Input;
    IO.Switches.Reading.DebounceTimerCounter = 0;
    IO.Switches.Reading.DebounceTimerProg = 18; //x5.56ms
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

