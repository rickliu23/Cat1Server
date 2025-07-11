#include "Fibocom_AT.h"
#include <string.h>
#include <stdio.h>






/************************* RSP *************************/

// 模块复位结束之后主动发送该指令
char LTE_AT_MODULE_READY_RSP[] = "AT command ready";

/************************* Basic CMD *************************/
char LTE_AT_AT[] = "AT";
char LTE_AT_ECHO[] = "E";
char LTE_AT_SIM_PIN[] = "CPIN";
char LTE_AT_CSQ[] = "CSQ";
char LTE_AT_NET_REG[] = "CEREG";

char LTE_AT_FUNC[] = "CFUN";



char LTE_AT_CALL[] = "MIPCALL";


/************************* RSP *************************/


char LTE_AT_OK_RSP[] = "OK";
char LTE_AT_ERR_RSP[] = "ERROR";
char LTE_AT_SIM_READY_RSP[] = "READY";//"+SIM READY";
char LTE_AT_CSQ_RSP[] = "+CSQ:";
char LTE_AT_REG_RSP[] = "+CEREG:";


#if 0
/************************* MQTT CMD*************************/
char LTE_AT_MQTT_USER[] = "MQTTUSER";
char LTE_AT_MQTT_WILL[] = "MQTTWILL";
char LTE_AT_MQTT_OPEN[] = "MQTTOPEN";
char LTE_AT_MQTT_CLOSE[] = "MQTTCLOSE";
char LTE_AT_MQTT_SUB[] = "MQTTSUB";
char LTE_AT_MQTT_MSG_RSP[] = "MQTTMSG";
char LTE_AT_MQTT_PUB[] = "MQTTPUB";
// char LTE_AT_MQTT_CLOSE[] = "MQTTCLOSE";
char LTE_AT_MQTT_BREAK_RSP[] = "MQTTBREAK";

/************************* TCP CMD *************************/
char LTE_AT_TCP_FMT[] = "GTSET";
char LTE_AT_TCP_OPEN[] = "MIPOPEN";
char LTE_AT_TCP_CLOSE[] = "MIPCLOSE";
char LTE_AT_TCP_STATE[] = "MIPSTAT";
char LTE_AT_TCP_SEND[] = "MIPSEND";
char LTE_AT_SEND_SYMBOL[] = ">";
char LTE_AT_TCP_RECV[] = "MIPRTCP";
char LTE_AT_SLEEP_SWITCH[] = "CSCLK";
char LTE_AT_SLEEP_MODE[] = "GTLPMMODE";

char LTE_AT_LOCATOR[] = "GTGIS";
char LTE_AT_LOCATOR_RSP_ERR[] = "NETWORK ERROR";

char LTE_AT_NTP[] = "MIPNTP";
char LTE_AT_TIME[] = "CCLK";

char LTE_AT_FILE[] = "GTSSLFILE";
char LTE_AT_DNS[] = "MIPDNS";

char LTE_AT_MQTT_CFG_SSL_VERSION[] = "GTSSLVER";

#endif

/*
id: cmd type
type: read or write or QUERY
param: used when type is write
cmdBuf: store the cmd to this buffer
bufMaxLen: max len of cmdBuf
usefulLen: the len of cmdBuf which actually used
*/
bool Fibocom_AT_Assemble_Basic(LTE_AT_INDEX id, LTE_AT_TYPE type, int param, char cmdBuf[], int bufMaxLen, int *usefulLen)
{
    memset(cmdBuf, 0, bufMaxLen);
    *usefulLen = 0;

    if(id == LTE_AT_INDEX_AT)
    {
        sprintf(cmdBuf, "%s\r\n", LTE_AT_AT);
    }
    else if(id == LTE_AT_INDEX_ECHO)
    {
        if(param > 1)
            return NULL;
        sprintf(cmdBuf, "%s%s%d\r\n", LTE_AT_AT, LTE_AT_ECHO, param);
    }
    else if(id == LTE_AT_INDEX_SIM_PIN)
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
    else if(id == LTE_AT_INDEX_CSQ)
    {
        if(type == LTE_AT_READ)
            sprintf(cmdBuf, "%s+%s\r\n", LTE_AT_AT, LTE_AT_CSQ);
        else if(type == LTE_AT_QUERY)
            sprintf(cmdBuf, "%s+%s=?\r\n", LTE_AT_AT, LTE_AT_CSQ);
    }
    else if(id == LTE_AT_INDEX_NET_REG)
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
