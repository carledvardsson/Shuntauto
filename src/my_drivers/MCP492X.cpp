#include "my_drivers/MCP492X.hpp"
#include "driver/spi_master.h"


MCP492X::MCP492X(SPIBus *bus,Model _model, int cs_pin)
{
    //Konfigurerar SPI device för att läsa ADC
    model=_model;
    spi_device_interface_config_t devcfg={0};
    devcfg.clock_speed_hz=1*500*1000;           //Clock out at 500kHz   1 MHz
    devcfg.mode=0;                               //SPI mode 0
    devcfg.spics_io_num=cs_pin;                  //CS pin
    devcfg.queue_size=3;

    ESP_ERROR_CHECK(spi_bus_add_device(bus->getHostDevice(), &devcfg, &spi));
}
void MCP492X::outputA(u_int16_t out)
{
    output(out,Channel::CHANNEL_A);
}
void MCP492X::outputB(u_int16_t out)
{
    output(out,Channel::CHANNEL_B);
}

void MCP492X::output(u_int16_t out, Channel channel)
{
    u_int16_t data=out & 0xfff;
    uint16_t output=0x0000;
    if (model == Model::MCP4922)
         output= (channel << 15) | ((1 << 13) | (1 << 12) | data );
    else
        output =  ((1 << 13) | (1 << 12) | data );
    
    spi_transaction_t transaction = {
        .flags =   SPI_TRANS_USE_TXDATA,
        .length = 16, // 16 bits.
    };
    transaction.tx_data[0] = (output >> 8) & 0xff;
    transaction.tx_data[1] = output  & 0xff;
    

    spi_device_transmit(spi, &transaction);

}