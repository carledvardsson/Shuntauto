#pragma once

//Klass för att spara data som vi behöver
class AktuellData
{
public:

	float MinFram;
	float MaxFram;
	float BorTemp;
    float LagTempBegr_limit;
    float LagTempBegr_shunt;
    float KurvKonstant;

    float PID_Kp;
    float PID_Ki;
    float PID_Kd;

	float AktuellTemp;
	float UteTemp;
	float Vind;
	float FramTemp;
	float ReturTemp;
	float TankTemp;
    float RokTemp;

    float ShuntSet;
    float ShuntSetVer;
    float ShuntAktuell;

	//shuntdata, för att hålla koll
	float UtetempVindkorr;
	float BerFramtemp;
	float FramOnskadTempProcent;
	//float FramAktuellTempProcent;   ShuntAktuell
	float PidJustering;
	float FramOnskadTempProcentJusterad;

	AktuellData();
};

