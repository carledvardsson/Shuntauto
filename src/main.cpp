#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "project_pins.h"
#include "Controller.h"
#include "AktuellData.h"
#include "nvs_flash.h"
#include "WiFi.h"
#include "ExternCStuff.h"



void initStuff()
{
    // initierar nvs flash, behövs för WiFi
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    //Starta WiFi, standard kod från EsspressIF exempel (ändrat nätverksinformation)
    wifi_init_sta();
}

int app_main() 
{
    initStuff();
    
    //Skapa kontroller, den gör allt jobb
    Controller controller;

    //Räkna ut fördröjningen för en sekund
    const TickType_t xPeriod = 1000 / portTICK_PERIOD_MS;

    //Pausa en sekund, så allt hinner starta
    vTaskDelay(xPeriod);

    //Variabel att Spara nuvarande tid i (används för att pausa 1s - tiden det tar för controller.tick() att utföras)
    TickType_t xLastWakeTime;

    //Sätter min tick, den räknar upp ett steg per sekund, nollas vid 30.
    //Används i controllern för att välja vad som ska göras
    int tick=0;

    //Oändlig loop
    while (1){
        //Spara nuvarande tid (används för att pausa 1s - tiden det tar för controller.tick() att utföras)
        xLastWakeTime= xTaskGetTickCount();

        //Kör ett tick i controllern
        controller.tick(tick);

        // pausa resten av tiden av sekunden, alltså en sekund från senaste start av 
		vTaskDelayUntil( &xLastWakeTime, xPeriod );

        //Räkna upp tick och slå runt vid 30
        tick++;
        tick = tick % 30;
    }
    //Hit kommer vi aldrig...
    return 0;
}