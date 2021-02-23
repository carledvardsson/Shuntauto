#include "Sensorer.h"
#include "project_pins.h"
#include "AktuellData.h"
#include "Funktioner.h"
#include "MQTTWrapper.hpp"



Sensorer::Sensorer()
{
    //Initierar sensorer

    // Konfiguration för den ena SPI bussen (VSPI)
    SPI_buss=new SPIBus(SPI_BUS::VSPI,PP_VSPI_MISO,PP_VSPI_MOSI,PP_VSPI_SCK,10);

    //ADC (Analog till digital omvandlare), signal från shuntmotor och temperaturer
    ADC=new mcp3208(SPI_buss,PP_CS_ADC,mcp3208_read_mode::MCP320X_READ_MODE_SINGLE);

    //DAC (Digital till analog omvandlare), signal till shuntmotor
    DAC=new MCP492X(SPI_buss,MCP492X::Model::MCP4921,PP_CS_DAC);

    //K-Element, för att läsa temperatur från pannans rökgaser
    KElement=new MAX6675(SPI_buss,PP_CS_KTemp);

    //Två 1-wire bussar, den första har utetemp, den kommer att ha flera sensorer för att mäta olika temperaturer
    ds1820_a=new DS18S20(PP_OneW_A,1);
    //ds1820_b=new DS18S20(PP_OneW_B,10);
}

void Sensorer::DS18S20_meassure_all()
{
    //1-Wire sensorerna behöver ca 750mS för att läsa, anropas och triggas först
    ds1820_a->ds18x20_measure(false);
    //ds1820_b->ds18x20_measure(false);
}

// Läser in all data från sensorerna
void Sensorer::ReadAll(AktuellData *data)
{
    data->ShuntSet =Funktioner::ADC_In(ADC->getRawValue(mcp3208_kanaler_t::AD_TILL_MOTOR));
    data->ShuntAktuell =Funktioner::ADC_In(ADC->getRawValue(mcp3208_kanaler_t::AD_FRAN_MOTOR));
    data->AktuellTemp = Funktioner::ADC_InTempFromVoltage(ADC->getVoltageValue(mcp3208_kanaler_t::AD_TEMP_RUM));
    data->TankTemp = Funktioner::ADC_InTempFromVoltage(ADC->getVoltageValue(mcp3208_kanaler_t::AD_TEMP_TANK));
    data->FramTemp = Funktioner::ADC_InTempFromVoltage(ADC->getVoltageValue(mcp3208_kanaler_t::AD_TEMP_FRAM));
    data->ReturTemp= Funktioner::ADC_InTempFromVoltage(ADC->getVoltageValue(mcp3208_kanaler_t::AD_TEMP_RETUR));


    //DS18S20 tempar, läser bara från buss a just nu.
    ds1820_a->ds18x20_read_temperature(ds1820_a->addr_list[0],&data->UteTemp);
}

void Sensorer::SetDAC(u_int16_t pos)
{
    //Sätt signalen till shuntmotorn
    DAC->outputA(Funktioner::DAC_Ut(pos));
}