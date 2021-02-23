#include <math.h>
#include <esp_log.h>
#include "my_drivers/DS18S20.hpp"





static const char *TAG = "ds18x20";

DS18S20::DS18S20(int BusPin,int maxDevices)
{
    pin=(gpio_num_t)BusPin;
    addr_list=new ds18x20_addr_t[maxDevices];
    totalDevicesFound=ds18x20_scan_devices();
    if (totalDevicesFound>maxDevices)
        deviceCount=maxDevices;
    else
        deviceCount=totalDevicesFound;

}

esp_err_t DS18S20::ds18x20_measure(bool wait)
{
    return ds18x20_measure(ds18x20_ANY, wait);
}
esp_err_t DS18S20::ds18x20_measure(ds18x20_addr_t addr, bool wait)
{
    if (!OneWire::onewire_reset(pin))
        return ESP_ERR_INVALID_RESPONSE;

    if (addr == ds18x20_ANY)
        OneWire::onewire_skip_rom(pin);
    else
        OneWire::onewire_select(pin, addr);

    PORT_ENTER_CRITICAL;
    OneWire::onewire_write(pin, ds18x20_CONVERT_T);
    // For parasitic devices, power must be applied within 10us after issuing
    // the convert command.
    OneWire::onewire_power(pin);
    PORT_EXIT_CRITICAL;

    if (wait)
    {
        SLEEP_MS(750);
        OneWire::onewire_depower(pin);
    }

    return ESP_OK;
}

esp_err_t DS18S20::ds18x20_read_scratchpad(ds18x20_addr_t addr, uint8_t *buffer)
{
    CHECK_ARG(buffer);

    uint8_t crc;
    uint8_t expected_crc;

    if (!OneWire::onewire_reset(pin))
        return ESP_ERR_INVALID_RESPONSE;

    if (addr == ds18x20_ANY)
        OneWire::onewire_skip_rom(pin);
    else
        OneWire::onewire_select(pin, addr);
    OneWire::onewire_write(pin, ds18x20_READ_SCRATCHPAD);

    for (int i = 0; i < 8; i++)
        buffer[i] = OneWire::onewire_read(pin);
    crc = OneWire::onewire_read(pin);

    expected_crc = OneWire::onewire_crc8(buffer, 8);
    if (crc != expected_crc)
    {
        ESP_LOGE(TAG, "CRC check failed reading scratchpad: %02x %02x %02x %02x %02x %02x %02x %02x : %02x (expected %02x)", buffer[0], buffer[1],
                buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], crc, expected_crc);
        return ESP_ERR_INVALID_CRC;
    }

    return ESP_OK;
}

esp_err_t DS18S20::ds18x20_read_temperature(ds18x20_addr_t addr, float *temperature)
{
    CHECK_ARG(temperature);

    uint8_t scratchpad[8];
    int16_t temp;

    CHECK(ds18x20_read_scratchpad(addr, scratchpad));

    temp = scratchpad[1] << 8 | scratchpad[0];

    if ((uint8_t)addr == DS18B20_FAMILY_ID)
        *temperature = ((float)temp * 625.0) / 10000;
    else
    {
        temp = ((temp & 0xfffe) << 3) + (16 - scratchpad[6]) - 4;
        *temperature = ((float)temp * 625.0) / 10000 - 0.25;
    }

    return ESP_OK;
}

esp_err_t DS18S20::ds18x20_measure_and_read(ds18x20_addr_t addr, float *temperature)
{
    CHECK_ARG(temperature);

    CHECK(ds18x20_measure( addr, true));
    return ds18x20_read_temperature( addr, temperature);
}

esp_err_t DS18S20::ds18x20_measure_and_read_multi(ds18x20_addr_t *addr_list, int addr_count, float *result_list)
{
    CHECK_ARG(result_list);

    CHECK(ds18x20_measure( ds18x20_ANY, true));

    return ds18x20_read_temp_multi( addr_list, addr_count, result_list);
}

int DS18S20::ds18x20_scan_devices()
{
    CHECK_ARG(addr_list);

    onewire_search_t search;
    onewire_addr_t addr;
    int found = 0;

    OneWire::onewire_search_start(&search);
    while ((addr = OneWire::onewire_search_next(&search, pin)) != ONEWIRE_NONE)
    {
        uint8_t family_id = (uint8_t)addr;
        if (family_id == DS18B20_FAMILY_ID || family_id == DS18S20_FAMILY_ID)
        {
            if (found < addr_count)
                addr_list[found] = addr;
            found++;
        }
    }
    return found;
}

esp_err_t DS18S20::ds18x20_read_temp_multi(ds18x20_addr_t *addr_list, int addr_count, float *result_list)
{
    CHECK_ARG(result_list);

    esp_err_t res = ESP_OK;
    for (int i = 0; i < addr_count; i++)
    {
        esp_err_t tmp = ds18x20_read_temperature( addr_list[i], &result_list[i]);
        if (tmp != ESP_OK)
            res = tmp;
    }
    return res;
}

void DS18S20::ds1820_readAll()
{
    ds18x20_read_temp_multi(addr_list, deviceCount, temp_list);
}