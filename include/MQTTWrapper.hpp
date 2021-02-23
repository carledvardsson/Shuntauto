#pragma once
#include "mqtt_client.h"
#include "esp_event.h"
#include "freertos/queue.h"
#ifndef MQTTWRAPPER
#define MQTTWRAPPER
namespace  MQTTWrapper{
        extern QueueHandle_t MQTTSendQueue;
        extern QueueHandle_t MQTTRecQueue;
        extern const char *TAG;
        extern esp_mqtt_client_handle_t client;

        void init();
        void publish(MQTTMessage *msg);
        QueueHandle_t getRecQueue();
        void log_error_if_nonzero(const char * message, int error_code);
        void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
        esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event);
        void PostFromQueue(void * parameters);
        void Subscribe(char * topic,int QoS);

};

#endif