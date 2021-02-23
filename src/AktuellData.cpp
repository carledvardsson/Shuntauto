#include "AktuellData.h"

AktuellData::AktuellData() {
	MinFram=25;
	MaxFram=60;
	BorTemp=21.5;
    LagTempBegr_limit=40;
    LagTempBegr_shunt=30;
    KurvKonstant=3.5;
    
    PID_Kp=1;
    PID_Ki=0;
    PID_Kd=3;

    // Tempar in:
	AktuellTemp=0;
	UteTemp=0;
	Vind=0;
	FramTemp=0;
	ReturTemp=0;
	TankTemp=0;
    RokTemp=0;

    ShuntSet=0;
    ShuntSetVer=0;
    ShuntAktuell=0;

    //PID/shuntdata... för att hålla koll...
    UtetempVindkorr=0;
	BerFramtemp=0;
	FramOnskadTempProcent=0;
	
	PidJustering=0;
	FramOnskadTempProcentJusterad=0;
}