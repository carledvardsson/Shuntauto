#include "project_pins.h"
#include "my_drivers/mcp3208.hpp"
#include "my_drivers/SPIBus.hpp"
#include "driver/spi_master.h"
#include "driver/gpio.h"


mcp3208::mcp3208(SPIBus *bus,int cs_pin,mcp3208_read_mode mode)
{
    //Konfigurerar SPI device för att skriva till DAC
    read_mode=mode;
    spi_device_interface_config_t devcfg={0};
    devcfg.clock_speed_hz=1*500*1000;           //Clock out at 500kHz   1 MHz
    devcfg.mode=0;                               //SPI mode 0
    devcfg.spics_io_num=cs_pin;                  //CS pin
    devcfg.queue_size=3;

    ESP_ERROR_CHECK(spi_bus_add_device(bus->getHostDevice(), &devcfg, &spi));
}

void mcp3208::setReferenceVoltage(float voltage)
{
    mVPerStep=voltage/4095;
}

float mcp3208::getVoltageValue(mcp3208_kanaler_t adKanal)
{
    return getRawValue(adKanal)*mVPerStep;
}

u_int16_t  mcp3208::getRawValue(mcp3208_kanaler_t adKanal)
{
    u_int16_t value;
    spi_transaction_t transaction = {
        .flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA,
        .length = 24, // 24 bits.
    };

    transaction.tx_data[0] = (1 << 2) | (read_mode << 1) | ((adKanal & 4) >> 2);
    transaction.tx_data[1] = adKanal << 6;
    transaction.tx_data[2] = 0;

    spi_device_transmit(spi, &transaction);

    value = ((transaction.rx_data[1] & 15) << 8) | transaction.rx_data[2];

    return value;
}