#pragma once
#include "driver/spi_master.h"

enum SPI_BUS {HSPI,VSPI};

//Generell konfiguration för SPI buss
class SPIBus{
    
    SPI_BUS selectedBus;

    public:
        SPIBus(SPI_BUS bus,int miso, int mosi,int clock,int max_size);
        ~SPIBus();
        spi_host_device_t getHostDevice();
};

