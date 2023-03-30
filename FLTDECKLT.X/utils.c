/**
 \file Utilerias de software.
*/

#include "utils.h"

//VARIABLES GLOBALES
struct TimerOldStructure Timer0;

//FUNCIONES

/**
 Transforma un binario a BCD.
 El algoritmo puede transformar 24 bits de entrada pero la
 salida esta limitada a 3 bytes equivalentes en BCD a 999,999.

 \return 3 bytes con un BCD cada 4 bits. Valor maximo de 999,999.
 \param binaryData Dato binario de 3 bytes.
 \note Para informacion del algoritmo visitar
 http://www.mcgourty.com/designfiles/0603df04.html
*/
UInt24 Utils_BinToBCD(UInt24 binaryData)
{
    UInt24UInt8 bundleBCD;
	UInt8 i;

    bundleBCD.UI24 = 0;

	for (i = 0; i < 24; i++) //dimension de dataBin
	{
//		if (((dataBcd + 0x03000000) & 0x08000000) == 0x08000000)
//		dataBcd += 0x03000000;
//		if (((dataBcd + 0x30000000) & 0x80000000) == 0x80000000)
//		dataBcd += 0x30000000;
		if (((bundleBCD.UI8[2] + 0x03) & 0x08))
		bundleBCD.UI8[2] += 0x03;
		if (((bundleBCD.UI8[2] + 0x30) & 0x80))
		bundleBCD.UI8[2] += 0x30;
		if (((bundleBCD.UI8[1] + 0x03) & 0x08))
		bundleBCD.UI8[1] += 0x03;
		if (((bundleBCD.UI8[1] + 0x30) & 0x80))
		bundleBCD.UI8[1] += 0x30;
		if (((bundleBCD.UI8[0] + 0x03) & 0x08))
		bundleBCD.UI8[0] += 0x03;
		if (((bundleBCD.UI8[0] + 0x30) & 0x80))
		bundleBCD.UI8[0] += 0x30;
		bundleBCD.UI24 <<= 1;
		if ((binaryData & 0x800000))
		bundleBCD.UI8[0] += 1;
		binaryData <<= 1;
	}
	return bundleBCD.UI24;
}

/**
 Transforma un byte BCD a ASCII.
 Toma el codigo BCD en el nibble menos significativo
 de un byte y lo pasa a ASCII.
 \param bcd Byte en BCD.
 \return Byte en codigo ASCII.
 \todo Convertir varios al mismo tiempo.
*/
UInt8 Utils_BCDToASCII(UInt8 bcd) {
	UInt8 lowNibble;

	lowNibble = bcd & 0x0f;
	return (0x30 + lowNibble);
}

/**
 Convierte un dato 7 segmentos a ASCII.
 \param sevenSegmentsCode Byte en codigo de 7 segmentos.
 \return Byte en codigo ASCII.
*/
UInt8 Utils_SevenCodeToASCII(UInt8 sevenSegmentsCode) {
	if(sevenSegmentsCode < 10) {//es un numero?
		return (sevenSegmentsCode + 48);
	}
	else {
		if(sevenSegmentsCode < 36) {
			return (sevenSegmentsCode + 55); //es letra mayuscula 10+55=65 OJO
		} else {
			return ' '; //espacio => unico caracter especial
		}
	}
}

/**
 Transforma un solo BCD a un byte en binario.
 \param bcd Byte en BCD.
 \return Byte en binario.
*/
UInt8 Utils_BCDByteToBin(UInt8 bcd) {
	return (((bcd >> 4) * 10) + (bcd & 0x0f));
}

//void DelayByHalfSecond(UInt8 factor) {
//	UInt8 i;
//	for(i = 0; i < factor; i++) {
//		Delay10KTCYx(50); // 0.5 seg@4MHz
//	}
//}

/**
 Configuracion por defecto y segura del timer.
 Tiempo total para desborde: (255-time)*50ms
 \param time Factor de espera por 50ms.
 \todo Que pueda ser sobreescrita por otras funciones.
*/
void Timer0_Init(UInt8 time) {
	Timer0.Bits.Enable = FALSE; // stop timer
	Timer0.Bits.Flag = FALSE;
	Timer0.Count = 255 - time;
	Timer0.Exec = Timer0_Exec;
}

/**
 Secuencia de ejecucion para los vectores de interrupcion.
*/
void Timer0_Interrupt(void) {
	if(Timer0.Bits.Enable) {
		Timer0.Count++;
		if(Timer0.Count == 0) {
			Timer0.Bits.Flag = TRUE;
			//ejecutamos aqui para ser lo mas preciso posible
			Timer0.Exec();
		}
	}
}


/**
 Inicia el contador del timer.
 \note Funcion provisional.
*/
void Timer0_Start(void) {
	Timer0.Bits.Flag = FALSE;
	Timer0.Bits.Enable = TRUE;
}

/**
 Inicia el contador del timer.
 \note Funcion provisional.
*/
void Timer0_Stop(void) {
	Timer0.Bits.Enable = FALSE;
}

/**
 Espera a que se desborde Timer0.
 \note Funcion provisional.
*/
void Timer0_Wait(void) {
	while(Timer0.Bits.Flag == FALSE);
}

/**
 Funcion provisional para rutina de bienvenida.
 Restart Timer0 each 250ms
*/
void Timer0_Exec(void) {
	Timer0.Count = 215; // 40*50ms=2s
}


/**
 Calcula el checksum de una cadena de bytes.
 Para calcular: sumar todos los bytes manteniendo solo los
 8 bits mas bajos del resultado y substraer de 0xff.
 \param data[] Arreglo de bytes.
 \param dataSize Dimension del arreglo.
 \return Retorna el checksum.
*/
//UInt8 Utils_GetChecksum(UInt8 data[], UInt8 dataSize) {
//	UInt8 i;
//	UInt8 sum = 0;
//
//	for(i = 0; i < dataSize; i++)
//		sum += data[i];
//
//	return 0xff - sum;
//}

/**
 Verifica el checksum de una cadena de bytes.
 Para verificar: la suma de todos los bytes (incluyendo el checksum)
 debe ser igual a 0xff.
 \param data[] Arreglo de bytes.
 \param dataSize Dimension del arreglo.
 \return [TRUE, FALSE] TRUE si el checksum es valido, FALSE en otro caso.
*/
//UInt8 Utils_VerifyChecksum(UInt8 data[], UInt8 dataSize) {
//	UInt8 i;
//	UInt8 sum = 0;
//
//	for(i = 0; i < dataSize; i++)
//		sum += data[i];
//
//	if(sum == 0xff)
//		return TRUE;
//	else
//		return FALSE;
//}
