#pragma once
#include "AktuellData.h"
#include "freertos/FreeRTOS.h"
#include "my_drivers/SPIBus.hpp"
#include "my_drivers/mcp3208.hpp"
#include "my_drivers/DS18S20.hpp"
#include "my_drivers/MCP492X.hpp"
#include "my_drivers/MAX6675.hpp"
#include "MQTTMessage.hpp"

class Sensorer
{
private:
    SPIBus *SPI_buss;
    MAX6675 *KElement;
    mcp3208 *ADC;
    MCP492X *DAC;
    DS18S20 *ds1820_a;
    DS18S20 *ds1820_b;
public:
    Sensorer();
    void DS18S20_meassure_all();
    void ReadAll(AktuellData *data);
    void SetDAC(u_int16_t pos);
};