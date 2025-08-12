#include "srvLte_Mqtt.h"
#include <stdio.h>
#include "Fibocom_AT.h"

/************************************ 模块MQTT连接类 ************************************/

/************************************ public ************************************/

void clsLteMqttConnIf::Init(uint8_t id)
{
}

void clsLteMqttConnIf::OnTimerFast(void)
{
}
void clsLteMqttConnIf::OnTimer(void)
{
}
void clsLteMqttConnIf::OnTimerSlow(void)
{
}

// Regularly retrieve data from this class to determine if sending is required.
bool clsLteMqttConnIf::MsgPop(uint8_t *msg, uint32_t lenIn, uint32_t &lenOut, int32_t &timeout_ms)
{
}

// 收到消息，需要这个类来处理
void clsLteMqttConnIf::MsgProcess(uint8_t *msg, uint32_t lenIn)
{
    
}

void clsLteMqttConnIf::SetUser(uint8_t *userName, uint8_t *password, uint8_t* clientID)
{
    char params[4][LTE_PARAM_MAX_LEN] = { 0 };
    LteMsgStructure msg = { 0 };

    sprintf(params[0], "%d", m_connID);
    sprintf(params[1], "%s", userName);
    sprintf(params[2], "%s", password);
    sprintf(params[3], "%s", clientID);

    if (Fibocom_AT_Assemble_Mqtt(LTE_AT_INDEX_MQTT_USER, (char*)msg.msg, LTE_MSG_MAX_BYTES, &msg.len, 4, params, NULL) != true)
    {
        return;
    }

    m_msgFifo.MsgPush(msg.msg, msg.len, LTE_AT_MQTT_USER_TIMEOUT_MS);
}

void clsLteMqttConnIf::Connect(uint8_t *remoteIpOrUrl, uint16_t remotePort, uint8_t CleansessionFlag, uint16_t keepAliveTime, uint8_t usetls)
{
    char params[6][LTE_PARAM_MAX_LEN] = { 0 };
    LteMsgStructure msg = { 0 };
    
    sprintf(params[0], "%d", m_connID);
    sprintf(params[1], "%s", remoteIpOrUrl);
    sprintf(params[2], "%d", remotePort);
    sprintf(params[3], "%d", CleansessionFlag);
    sprintf(params[4], "%d", keepAliveTime);
    sprintf(params[5], "%d", usetls);

    if (Fibocom_AT_Assemble_Mqtt(LTE_AT_INDEX_MQTT_OPEN, (char*)msg.msg, LTE_MSG_MAX_BYTES, &msg.len, 6, params, NULL) != true)
    {
        return;
    }

    m_msgFifo.MsgPush(msg.msg, msg.len, LTE_AT_MQTT_USER_TIMEOUT_MS);
}

void clsLteMqttConnIf::Disconnect(void)
{
    char params[1][LTE_PARAM_MAX_LEN] = { 0 };
    LteMsgStructure msg = { 0 };
    
    sprintf(params[0], "%d", m_connID);

    if (Fibocom_AT_Assemble_Mqtt(LTE_AT_INDEX_MQTT_CLOSE, (char*)msg.msg, LTE_MSG_MAX_BYTES, &msg.len, 1, params, NULL) != true)
    {
        return;
    }

    m_msgFifo.MsgPush(msg.msg, msg.len, LTE_AT_MQTT_USER_TIMEOUT_MS);
}

void clsLteMqttConnIf::Subscribe(uint8_t *topic)
{
    
}

void clsLteMqttConnIf::Unsubscribe(uint8_t *topic)
{
}

void clsLteMqttConnIf::Publish(uint8_t *topic, uint8_t *payload, uint32_t len)
{
}

/************************************ private ************************************/
void clsLteMqttConnIf::Clear(void)
{
}

/************************************ 模块MQTT维护类 ************************************/

static clsLteMqttIf m_LteMqttIfObj;

/************************************ public ************************************/
clsLteMqttIf *clsLteMqttIf::GetInstance(void)
{
    return &m_LteMqttIfObj;
}

void clsLteMqttIf::Init(void)
{
    m_connMngr.isUsed[0] = false;
    m_connMngr.id[0] = Enum_LteMqtt_WES;
    m_connMngr.connObj[0].Init(Enum_LteMqtt_WES);
    
    m_connMngr.isUsed[1] = false;
    m_connMngr.id[1] = Enum_LteMqtt_TP;
    m_connMngr.connObj[1].Init(Enum_LteMqtt_TP);
}

void clsLteMqttIf::OnTimerFast(void)
{

}

void clsLteMqttIf::OnTimer(void)
{

}

void clsLteMqttIf::OnTimerSlow(void)
{

}

bool clsLteMqttIf::MsgPop(uint8_t *msg, uint32_t lenIn, uint32_t &lenOut, int32_t &timeout_ms)
{
    return true;
}

void clsLteMqttIf::SetUser(Enum_LteMqttConnID connID, uint8_t *userName, uint8_t nameLen, uint8_t *password, uint8_t passwordLen)
{

}
void clsLteMqttIf::Connect(Enum_LteMqttConnID connID, uint8_t *remoteIpOrUrl, uint16_t remotePort, uint8_t CleansessionFlag, uint16_t keepAliveTime, uint8_t usetls)
{

}
void clsLteMqttIf::Disconnect(Enum_LteMqttConnID connID)
{

}

/************************************ private ************************************/
void clsLteMqttIf::Clear(void)
{

}
