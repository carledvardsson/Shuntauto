#include "Controller.h"
#include "Funktioner.h"
#include "freertos/FreeRTOS.h"
#include "MQTTWrapper.hpp"


Controller::Controller()
{
    //Startar MQTT
    MQTTWrapper::init();

    //Skapa upp sensorerna så vi får data och kan styra shuntmotor
    sensorer=new Sensorer();

    //Vårt dataobjekt, där finns all data
    data= new AktuellData();

    //PID kontrollern för att styra motorn
    pid = new PID(30, 5, -5, data->PID_Kp, data->PID_Kd, data->PID_Ki);
}

//Huvudloopen anropar denna metod, som är den som gör allt jobb
// tick har värde mellan 0-29, ensekunds intervall
void Controller::tick(int tick)
{
    if (tick==0  || tick==10 || tick==20)
    {
        //Starta mätning från 1-wire buss... tar ca. 750mS
        sensorer->DS18S20_meassure_all();
    }
    if ( tick==1 || tick==11 || tick==21 )
    {
        //Läsa från alla sensorer
        sensorer->ReadAll(data);
        //publicera all data vi vill till MQTT
        PubliceraMQTT();
    }
    if (tick==5)
    {
        //Beräkna och sätta procent till shuntmotor
        doPID();
        //Publicera PID relaterad data till MQTT, efter beräkningen
        PubliceraPidMQTT();
    }

    ReadMQTT();
}

void Controller::doPID()
{
    //Vindkorrigera utetemp
    float uteTempVindKorr=Funktioner::VindkorrigeraTemp(data->Vind , data->UteTemp);
    data->UtetempVindkorr=uteTempVindKorr;

    //Beräkna framtemp utifrån utetemp
    float framTemp=Funktioner::FramledningsTemp(data->KurvKonstant , uteTempVindKorr);
    data->BerFramtemp=framTemp;

    //Kolla att den önskade framtempen inte ligger över MaxFram
    if (framTemp > data->MaxFram){
        framTemp=data->MaxFram;
    }
    
    //Beräkna önskad procent fram
    float framOnskadTempProcent=Funktioner::ProcentTemp(data->ReturTemp , framTemp , data->TankTemp);
    data->FramOnskadTempProcent=framOnskadTempProcent;

    //Beräkna aktuell procent fram
    float framAktuellTempProcent=Funktioner::ProcentTemp(data->ReturTemp , data->FramTemp , data->TankTemp);
    data->ShuntAktuell=framAktuellTempProcent;

    //Beräkna justering med PID
    float justering=pid->calculate(framOnskadTempProcent , framAktuellTempProcent);
    data->PidJustering=justering;

    //Justera procent fram med PID
    framOnskadTempProcent=framOnskadTempProcent + justering;
    data->FramOnskadTempProcentJusterad=framOnskadTempProcent;

    //Kolla om vattnet från tanken är för kallt, om det är för kallt så är max öppning på shunt 30% (finns i AktuellData)
    //Sätter shuntens öppningsgrad via DAC
    if (data->TankTemp < data->LagTempBegr_limit && framOnskadTempProcent > data->LagTempBegr_shunt){
        sensorer->SetDAC(Funktioner::DAC_Ut(data->LagTempBegr_shunt));
        data->ShuntSet=data->LagTempBegr_shunt;
    }
    else{
        sensorer->SetDAC(Funktioner::DAC_Ut(Funktioner::TanKurva(framOnskadTempProcent)));
        data->ShuntSet=framOnskadTempProcent;
    }
}
void Controller::PubliceraPidMQTT()
{
    //Publicera PID saker på MQTT
    PubliceraMQTT(MQTT_TOPIC_SHUNT_AKTUELL,data->ShuntAktuell);
    PubliceraMQTT(MQTT_TOPIC_SHUNT_SET,data->ShuntSet);
    PubliceraMQTT(MQTT_TOPIC_SHUNT_JUSTERING,data->PidJustering);

    PubliceraMQTT(MQTT_TOPIC_UTETEMP_VINDKORR,data->UtetempVindkorr);
    PubliceraMQTT(MQTT_TOPIC_ONSKADTEMP,data->BerFramtemp);
}
void Controller::PubliceraMQTT()
{
    //Publicera data på MQTT
    PubliceraMQTT(MQTT_TOPIC_BORTEMP,data->BorTemp);
    PubliceraMQTT(MQTT_TOPIC_INNETEMP,data->AktuellTemp);
    PubliceraMQTT(MQTT_TOPIC_UTETEMP,data->UteTemp);
    PubliceraMQTT(MQTT_TOPIC_VIND,data->Vind);
    PubliceraMQTT(MQTT_TOPIC_FRAMTEMP,data->FramTemp);
    PubliceraMQTT(MQTT_TOPIC_RETURTEMP,data->ReturTemp);
    PubliceraMQTT(MQTT_TOPIC_TANKTEMP,data->TankTemp);
    PubliceraMQTT(MQTT_TOPIC_PANNAGASTEMP,data->RokTemp);
    PubliceraMQTT(MQTT_TOPIC_KURVA_K,data->KurvKonstant);
}

void Controller::PubliceraMQTT(const char * topic, float dataValue)
{
    MQTTMessage msg;
    char buffer[10];
    FormateraTillMQTT(dataValue,buffer);

    printf("Topic:%s , Payload:%s",topic,buffer);
    msg.set((char *)topic,buffer);
    MQTTWrapper::publish(&msg);
}

void Controller::FormateraTillMQTT(float x,char *buffer)
{
    // En decimal på all data ut
    sprintf(buffer,"%0.1f",x);
}

float Controller::ParseFloat(char *buffer)
{
    return atof(buffer);
}

void Controller::ReadMQTT()
{
    //För varje meddelande på kön, läsaer och kollar ifall det är det något vi vill ha
    while(uxQueueMessagesWaiting(MQTTWrapper::MQTTRecQueue)){
        MQTTMessage msg;

        if (xQueueReceive(MQTTWrapper::MQTTRecQueue,(void *)&msg,0) == pdTRUE){
            if ( msg.payload !=NULL){
                // Vi bryr oss bara om det vi vill ta in...
                if (strcmp(MQTT_TOPIC_VIND_IN,msg.topic)==0){
                    data->Vind = ParseFloat(msg.payload);
                }
            }
        }
    }
}