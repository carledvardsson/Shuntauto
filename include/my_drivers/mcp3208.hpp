#pragma once
#include "my_drivers/SPIBus.hpp"
#include "esp_system.h"
enum mcp3208_kanaler_t
    {
        AD_TILL_MOTOR = 0,
        AD_FRAN_MOTOR = 1,
        AD_TEMP_RUM = 2,
        AD_TEMP_TANK = 3,
        AD_TEMP_FRAM = 4,
        AD_TEMP_RETUR = 5,
        AD_TEMP_EXT1 = 6,
        AD_TEMP_EXT2 = 7
    };
enum mcp3208_read_mode
    {
        MCP320X_READ_MODE_DIFFERENTIAL = 0,
        MCP320X_READ_MODE_SINGLE = 1
    };
    
class mcp3208{
    spi_device_handle_t spi;
    mcp3208_read_mode read_mode;
    float mVPerStep=2.5/4095;

    public:
    
    mcp3208(SPIBus *bus,int cs_pin,mcp3208_read_mode mode);

    void setReferenceVoltage(float voltage);
    float getVoltageValue(mcp3208_kanaler_t adKanal);
    u_int16_t getRawValue(mcp3208_kanaler_t adKanal);

};