#pragma once
#include "PID.h"
#include "Sensorer.h"
#include "AktuellData.h"

class Controller
{
private:
    const char * MQTT_TOPIC_1WIRE_B2_BASE ="/hus/shuntauto/1Wire_b2/";

    const char * MQTT_TOPIC_BORTEMP ="/hus/shuntauto/bortemp";
    const char * MQTT_TOPIC_INNETEMP ="/hus/shuntauto/innetemp";
    const char * MQTT_TOPIC_UTETEMP ="/hus/shuntauto/utetemp";
    const char * MQTT_TOPIC_VIND ="/hus/shuntauto/vind";
    const char * MQTT_TOPIC_FRAMTEMP ="/hus/shuntauto/framtemp";
    const char * MQTT_TOPIC_RETURTEMP ="/hus/shuntauto/returtemp";
    const char * MQTT_TOPIC_TANKTEMP ="/hus/shuntauto/tanktemp";
    const char * MQTT_TOPIC_PANNAGASTEMP ="/hus/panna/gastemp";

    const char * MQTT_TOPIC_SHUNT_SET ="/hus/shuntauto/shuntver";
    const char * MQTT_TOPIC_SHUNT_AKTUELL ="/hus/shuntauto/shuntaktuell";
    const char * MQTT_TOPIC_SHUNT_JUSTERING ="/hus/shuntauto/shuntjustering";

    const char * MQTT_TOPIC_UTETEMP_VINDKORR ="/hus/shuntauto/utetempvindkorr";
    const char * MQTT_TOPIC_ONSKADTEMP ="/hus/shuntauto/berframtemp";
    const char * MQTT_TOPIC_KURVA_K ="/hus/shuntauto/K";
    const char * MQTT_TOPIC_KURVA_OFFSET ="/hus/shuntauto/shunt";

    //Subscribe
    const char * MQTT_TOPIC_VIND_IN ="/smhi/current/WindSpeed";

    PID *pid;
    Sensorer *sensorer;
    AktuellData *data;
    void doPID();
    void FormateraTillMQTT(float x,char *buffer);
    float ParseFloat(char *buffer);
    void PubliceraMQTT();
    void PubliceraPidMQTT();
    void PubliceraMQTT(const char * topic, float data);
    void ReadMQTT();
public:
    Controller();
    void tick(int tick);
};