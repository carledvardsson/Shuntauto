
#include "my_drivers/SPIBus.hpp"
#include "my_drivers/MAX6675.hpp"
#include <stdio.h>


MAX6675::MAX6675(SPIBus *bus,int cs_pin)
{
    //Konfigurerar SPI device för att läsa K-element
    spi_device_interface_config_t devcfg={0};
    devcfg.clock_speed_hz=1*1000*1000;           //Clock out at 1 MHz
    devcfg.mode=0;                               //SPI mode 0
    devcfg.spics_io_num=cs_pin;                  //CS pin
    devcfg.queue_size=3;

    ESP_ERROR_CHECK(spi_bus_add_device(bus->getHostDevice(), &devcfg, &spi));
}

float MAX6675::spi_therm_read() {
    uint16_t bits = 0;
    spi_transaction_t tM;
    tM.tx_buffer = NULL;
    tM.rx_buffer = &bits;
    tM.length = 16;
    tM.rxlength = 16;

    spi_device_acquire_bus(spi, portMAX_DELAY);
    spi_device_transmit(spi, &tM);
    spi_device_release_bus(spi);
    
    uint16_t res = SPI_SWAP_DATA_RX(bits,16);
    if (res & 0x0004){
        printf("Thermocouple open");
    }
    res >>= 3;

    return res*0.25;
}