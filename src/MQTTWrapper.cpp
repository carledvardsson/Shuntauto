#include "MQTTMessage.hpp"
#include "mqtt_client.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_event.h"
#include "MQTTWrapper.hpp"

// Anpassat standardexempel från Esspressif, ändrat med namespace för att justera för C++


namespace MQTTWrapper{
#if CONFIG_FREERTOS_UNICORE
    static const BaseType_t app_cpu=0;
#else
    static const BaseType_t app_cpu=1;
#endif

    QueueHandle_t MQTTSendQueue=NULL;
    QueueHandle_t MQTTRecQueue=NULL;
    const char *TAG;
    esp_mqtt_client_handle_t client;

    void init()
    {
        TAG = "MQTT_WRAPPER";
        MQTTSendQueue=xQueueCreate(50,sizeof(MQTTMessage));

        //Skapa köer för meddelanden in och ut
        if (MQTTSendQueue==NULL)
            ESP_LOGE(TAG, "MQTTSendQueue is null");
        MQTTRecQueue=xQueueCreate(10,sizeof(MQTTMessage));
        if (MQTTRecQueue==NULL)
            ESP_LOGE(TAG, "MQTTRecQueue is null");
        
        //Inställningar för MQTT
        esp_mqtt_client_config_t mqtt_cfg = {0};
        mqtt_cfg.uri = "mqtt://username:password@<ip/namn till mqtts erver>";

        
        client = esp_mqtt_client_init(&mqtt_cfg);

        //Registrera funktion som hanterar händelser på MQTT
        esp_mqtt_client_register_event(client, MQTT_EVENT_ANY, mqtt_event_handler, client);
        //Starta MQTT
        esp_mqtt_client_start(client);

        // Egen task som publicerar meddelanden så att den inte behöver vänta i huvudloopen
        xTaskCreatePinnedToCore(PostFromQueue,"MQTTPublish",2048,NULL,1,NULL,app_cpu);
    }

    esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
    {
        esp_mqtt_client_handle_t client = event->client;
        int msg_id;
        // your_context_t *context = event->context;
        switch (event->event_id) {
            case MQTT_EVENT_CONNECTED:
                ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");

                msg_id = esp_mqtt_client_subscribe(client, "/smhi/current/WindSpeed", 0);
                ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

                msg_id = esp_mqtt_client_subscribe(client, "/topic/qos0", 1);
                ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

                break;
            case MQTT_EVENT_DISCONNECTED:
                ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
                break;

            case MQTT_EVENT_SUBSCRIBED:
                ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
                msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
                ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
                break;
            case MQTT_EVENT_UNSUBSCRIBED:
                ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
                break;
            case MQTT_EVENT_PUBLISHED:
                ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
                break;
            case MQTT_EVENT_DATA:
                ESP_LOGI(TAG, "MQTT_EVENT_DATA");
                printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
                printf("DATA=%.*s\r\n", event->data_len, event->data);
                break;
            case MQTT_EVENT_ERROR:
                ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
                if (event->error_handle->error_type == MQTT_ERROR_TYPE_ESP_TLS || event->error_handle->error_type ==MQTT_ERROR_TYPE_CONNECTION_REFUSED ) {
                    log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
                    log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
                    log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_tls_last_esp_err);
                    ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_tls_stack_err));

                }
                break;
            default:
                ESP_LOGI(TAG, "Other event id:%d", event->event_id);
                break;
        }
        return ESP_OK;
    }
    void PostFromQueue(void * parameters)
    {
        //Task för att skicka meddelanden via MQTT, tar från kön och skickar (kan ta tid, därför egen tråd)
        MQTTMessage msg;
        while(1){
            if (xQueueReceive(MQTTSendQueue,(void *)&msg,1000/portTICK_PERIOD_MS) == pdTRUE){
                esp_mqtt_client_publish(client,(const char *)&msg.topic,(const char *)&msg.payload,0,1,1);
            }
        }
    }
    void log_error_if_nonzero(const char * message, int error_code)
    {
        if (error_code != 0) {
            ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
        }
    }

    QueueHandle_t getRecQueue()
    {
        return MQTTRecQueue;
    }

    //Event handler, om det är ett meddelande in, så hamnar det på kö in som Controllern läser
    void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
        esp_mqtt_event_handle_t event=(esp_mqtt_event_handle_t)event_data;
        ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
        mqtt_event_handler_cb(event);

        if (event->event_id==MQTT_EVENT_DATA){
            MQTTMessage *msg=new MQTTMessage(event->topic,event->topic_len,event->data,event->data_len);
            int res=xQueueSend(MQTTRecQueue,(void *)msg,0);
            delete msg;
        }
    }

    //Lägger ett meddelande på sänd kön, som skickas i den egna tråden sedan
    void publish(MQTTMessage *msg)
    {
        xQueueSend(MQTTSendQueue,(void *)msg,0);
    }
}