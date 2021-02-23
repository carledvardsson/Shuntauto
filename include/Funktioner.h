#pragma once
#include "freertos/FreeRTOS.h"
class Funktioner
{
private:

public:
	static  float TanKurva(float percentOpen);

	static  float FramledningsTemp(float K, float Tu);

	static  float VindkorrigeraTemp(float Ws, float Tu);

    static  float ProcentTemp(float retur,float aktuell, float tank);

    static u_int16_t DAC_Ut(float procent);

    static float ADC_In(u_int16_t procent);

    static float ADC_InTemp(u_int16_t tempSensor);

    static float ADC_InTempFromVoltage(float tempVoltage);
};