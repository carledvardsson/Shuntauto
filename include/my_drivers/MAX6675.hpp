#ifndef  __MAX6675_H__
#define  __MAX6675_H__
#include "driver/spi_master.h"
#include "my_drivers/SPIBus.hpp"


class MAX6675{
    spi_device_handle_t spi;

    public:
        MAX6675(SPIBus *bus,int cs_pin);
        float spi_therm_read();
};

#endif