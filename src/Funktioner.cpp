#include "Funktioner.h"
#include <cmath>

// Rundar till en rak kurva till lite av en tangensform, så ändringen blir lite större i början och slutet eftersom shunten inte är linjär
float Funktioner::TanKurva(float percentOpen)
{
    const float MinLimit = -2.3311;
	const float MaxLimit = 2.3311;
	float inValue = percentOpen;
	if (inValue < 0)
		inValue = 0;
	else if (inValue > 100)
		inValue = 100;

	float scaledInValue = ((inValue / 100) * MaxLimit * 2) - MaxLimit;
	float result = ((tan((float)0.5 * scaledInValue) + MaxLimit) / MaxLimit * 2) * 25;
	return result;
}

// Beräkna framtemp utifrån utetemp
float Funktioner::FramledningsTemp(float K, float Tu)
{
	// Tf = Framledningstemp
	// Tu = Utetemp
	// K = Lutning

	float Tf = -0.2 * K * (Tu - 20) + 20;
	return Tf;

}

// korrigerar utetemp beroende på vindhastighet
float Funktioner::VindkorrigeraTemp(float Ws, float Tu)
{
	// Teff = Effektiv temperatur
	// Ws = Vindhastighet
	// https://sv.wikipedia.org/wiki/Vindavkylning#:~:text=Vindavkylning%2C%20%C3%A4ven%20k%C3%B6ldindex%2C%20%C3%A4r%20en,den%20en%20mycket%20liten%20betydelse.
	if (Ws < 0.5)
		return Tu;
	float Teff = 13.126667 + 0.6215 * Tu - 13.924748 * pow(Ws, 0.16) + 0.4875195 * Tu * pow(Ws, 0.16);
	return Teff;
}

// beräknar procentuell nivå mellan två tempar, retur (låg) fram (mellan) och tank (hög)
float Funktioner::ProcentTemp(float retur,float aktuell, float tank)
{
    float procent=0;
    if(retur <= aktuell && aktuell <=tank){
        procent=(aktuell-retur)/(tank-retur)*100;
    }
    else if (aktuell >= tank){
        procent=100;
    }
    else
    {
        procent=0;
    }
    return procent;
}

// Beräknar procent till värdet som ska skickas till DAC
u_int16_t Funktioner::DAC_Ut(float procent)
{
    // 16 bitars DAC: 0-4095
    // Spänning ut: 0-10V (0-2.5*4), motor vill ha 2-10V, 0% är på 20% av intervallet
    int low=(int)0.2*4095;
    int val=low+((4095-low)*procent)/100;
    u_int16_t ut;
    if (val < low){
        ut=low;
    }
    else if (val>4095){
        ut=4095;
    }
    else{
        ut=val;
    }
    return ut;
}

// Räknar om värdet från ADC till procent
float Funktioner::ADC_In(u_int16_t ADCIn)
{
    // 16 bitar ADC: 0-4095
    // Spänning in: 0-2.5V, med kretsar för att hantera överspänning levererar motorn 0-2V, 0.5V går åt för att säkra mot överspänning.
    // 0.4-2V använder vi i intervallet (motor levererar 2-10V ut, delat på 5 ger 0.4-2V) 
    float procent;
    int minLimit=655;//(int) (0.4/2.5)*4095;
    int maxLimit=3276;//(int) (2/2.5)*4095;
    if (minLimit <= ADCIn && ADCIn <= maxLimit){
        procent=100.0*(ADCIn-minLimit)/(maxLimit-minLimit);
    }
    else if (ADCIn >maxLimit){
        procent=100;
    }
    else{
        procent=0;
    }
    return procent;
}

// beräknar temp från värde in, kommer nog inte användas
float Funktioner::ADC_InTemp(u_int16_t tempSensor)
{
    // LM35, 10mV/Grad celsius, 0-2.5V
    float temp;
    if (tempSensor < (int)(250.0/4095*100)){
        temp=250.0/4095*tempSensor;
    }
    else{
        temp=101;
    }
    return temp;
}

// beräknar temp från spänning in, enklare än förra metoden
float Funktioner::ADC_InTempFromVoltage(float tempVoltage)
{
    // LM35, 10mV/Grad celsius, 0-2.5V
    float temp= tempVoltage/0.01;
    return temp;
}