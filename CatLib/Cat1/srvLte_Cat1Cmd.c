#define DEFINE_LTE_AT_COMMANDS
#include "srvLte_Cat1Cmd.h"
#undef DEFINE_LTE_AT_COMMANDS

#include <string.h>
#include <stdio.h>


//static const char LTE_AT_AT[] = "AT";

//static const char LTE_AT_OK_RSP[] = "OK";
//static const char LTE_AT_ERR_RSP[] = "ERROR";

//static const char LTE_AT_INFO[] = "I";
//static const char LTE_AT_MODULE_READY_RSP[] = "AT command ready";
//static const char LTE_AT_SIM_READY_RSP[] = "READY";//"+SIM READY";
//static const char LTE_AT_ECHO[] = "E";
//static const char LTE_AT_ATI[] = "I";
//static const char LTE_AT_CSQ[] = "CSQ";
//static const char LTE_AT_FUNC[] = "CFUN";
//static const char LTE_AT_SIM_PIN[] = "CPIN";
//static const char LTE_AT_NET_REG[] = "CEREG";
//static const char LTE_AT_CALL[] = "MIPCALL";
//static const char LTE_AT_MQTT_USER[] = "MQTTUSER";
//static const char LTE_AT_MQTT_WILL[] = "MQTTWILL";
//static const char LTE_AT_MQTT_OPEN[] = "MQTTOPEN";
//static const char LTE_AT_MQTT_CLOSE[] = "MQTTCLOSE";
//static const char LTE_AT_MQTT_SUB[] = "MQTTSUB";
//static const char LTE_AT_MQTT_MSG_RSP[] = "MQTTMSG";
//static const char LTE_AT_MQTT_PUB[] = "MQTTPUB";
////static const char LTE_AT_MQTT_CLOSE[] = "MQTTCLOSE";
//static const char LTE_AT_MQTT_BREAK_RSP[] = "MQTTBREAK";
//static const char LTE_AT_TCP_FMT[] = "GTSET";
//static const char LTE_AT_TCP_OPEN[] = "MIPOPEN";
//static const char LTE_AT_TCP_CLOSE[] = "MIPCLOSE";
//static const char LTE_AT_TCP_STATE[] = "MIPSTAT";
//static const char LTE_AT_TCP_SEND[] =  "MIPSEND";
//static const char LTE_AT_SEND_SYMBOL[] = ">";
//static const char LTE_AT_TCP_RECV[] = "MIPRTCP";
//static const char LTE_AT_SLEEP_SWITCH[] = "CSCLK";
//static const char LTE_AT_SLEEP_MODE[] = "GTLPMMODE";

//static const char LTE_AT_LOCATOR[] = "GTGIS";
//static const char LTE_AT_LOCATOR_RSP_ERR[] = "NETWORK ERROR";

//static const char LTE_AT_NTP[] = "MIPNTP";
//static const char LTE_AT_TIME[] = "CCLK";

//static const char LTE_AT_FILE[] = "GTSSLFILE";
//static const char LTE_AT_DNS[] = "MIPDNS";

//static const char LTE_AT_MQTT_CFG_SSL_VERSION[] = "GTSSLVER";

/*
id: cmd type
type: read or write or QUERY
param: used when type is write
cmdBuf: store the cmd to this buffer
bufMaxLen: max len of cmdBuf
usefulLen: the len of cmdBuf which actually used
*/
bool Lte_AT_Assemble_Basic(LTE_AT_CMD_TYPE id, LTE_AT_OPERATION_TYPE type, int param, char cmdBuf[], int bufMaxLen, int *usefulLen)
{
    memset(cmdBuf, 0, bufMaxLen);
    *usefulLen = 0;

    if(id == LTE_AT_CMD_AT)
    {
        sprintf(cmdBuf, "%s\r\n", LTE_AT_AT);
    }
    else if(id == LTE_AT_CMD_ECHO)
    {
        if(param > 1)
            return NULL;
        sprintf(cmdBuf, "%s%s%d\r\n", LTE_AT_AT, LTE_AT_ECHO, param);
    }
    else if(id == LTE_AT_CMD_SIM_PIN)
    {
        if(type == LTE_AT_READ)
            sprintf(cmdBuf, "%s+%s?\r\n", LTE_AT_AT, LTE_AT_SIM_PIN);
        else if(type == LTE_AT_WRITE)
        {
            if(param > 9999)
                return NULL;
            sprintf(cmdBuf, "%s+%s=\"%d\"\r\n", LTE_AT_AT, LTE_AT_SIM_PIN, param);
        }
        else if(type == LTE_AT_QUERY)
            sprintf(cmdBuf, "%s+%s=?\r\n", LTE_AT_AT, LTE_AT_SIM_PIN);
    }
    else if(id == LTE_AT_CMD_CSQ)
    {
        if(type == LTE_AT_READ)
            sprintf(cmdBuf, "%s+%s\r\n", LTE_AT_AT, LTE_AT_CSQ);
        else if(type == LTE_AT_QUERY)
            sprintf(cmdBuf, "%s+%s=?\r\n", LTE_AT_AT, LTE_AT_CSQ);
    }
    else if(id == LTE_AT_CMD_NET_REG)
    {
        if(type == LTE_AT_READ)
            sprintf(cmdBuf, "%s+%s?\r\n", LTE_AT_AT, LTE_AT_NET_REG);
        else if(type == LTE_AT_WRITE)
        {
            if(param > 5)
                return NULL;
            sprintf(cmdBuf, "%s+%s=%d\r\n", LTE_AT_AT, LTE_AT_NET_REG, param);
        }
        else if(type == LTE_AT_QUERY)
            sprintf(cmdBuf, "%s+%s=?\r\n", LTE_AT_AT, LTE_AT_NET_REG);
    }
    else
    {
        return false;
    }

    *usefulLen = strlen(cmdBuf);

    return true;
}
