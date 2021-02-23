#include "my_drivers/SPIBus.hpp"


//Generell konfiguration för SPI buss
SPIBus::SPIBus(SPI_BUS bus,int miso_pin, int mosi_pin,int clock_pin,int max_size){
    spi_bus_config_t buscfg={0};
    buscfg.miso_io_num=miso_pin;
    buscfg.mosi_io_num=mosi_pin;
    buscfg.sclk_io_num=clock_pin;
    buscfg.quadwp_io_num=-1;
    buscfg.quadhd_io_num=-1;
    buscfg.max_transfer_sz=max_size;
    selectedBus=bus;
  
    ESP_ERROR_CHECK(spi_bus_initialize(getHostDevice(), &buscfg, 1));
}

spi_host_device_t SPIBus::getHostDevice()
{
    return (selectedBus==VSPI) ? VSPI_HOST:HSPI_HOST;
}

SPIBus::~SPIBus(){
    
}