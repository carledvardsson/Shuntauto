#include "MQTTMessage.hpp"
#include <string.h>

MQTTMessage::MQTTMessage(){}
MQTTMessage::MQTTMessage(char * topicIn, int tLength, char * payloadIn, int pLength)
{
    if(tLength>0)
    {
        memcpy(topic,topicIn,tLength);
        topic[tLength]='\0';
    }
    if(pLength>0)
    {
        memcpy(payload,payloadIn,pLength);
        payload[pLength]='\0';
    }
}

void MQTTMessage::set(char* topicIn,char* payloadIn)
{
    for (int i=0;i<50;i++)
        topic[i]='\0';
    for (int i=0;i<10;i++)
        payload[i]='\0';
    strcpy(topic,topicIn);
    strcpy(payload,payloadIn);
}


