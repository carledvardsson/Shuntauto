#pragma once

class MQTTMessage{
    public:
        char topic[50];
        char payload[10];

        MQTTMessage(char * topicIn, int tLength, char * payloadIn, int pLength);
        MQTTMessage();
        void set(char* topic,char* payload);
};