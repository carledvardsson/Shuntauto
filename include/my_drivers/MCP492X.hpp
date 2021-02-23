#pragma once
#include "esp_system.h"
#include "my_drivers/SPIBus.hpp"

class MCP492X{
    
    public:
        enum Model {
            MCP4921,     /* single, 12-bit */
            MCP4922      /* dual, 12-bit */
        };

        enum Channel {
            CHANNEL_A = 0,
            CHANNEL_B = 1
        };

        MCP492X(SPIBus *bus,Model _model, int cs_pin);
        void outputA(u_int16_t  _out); 
        void outputB(u_int16_t  _out);
    private:
        Model model;
        spi_device_handle_t spi;
        void output(u_int16_t  _out, Channel channel);
};