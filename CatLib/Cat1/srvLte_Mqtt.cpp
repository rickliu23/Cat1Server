#include "srvLte_Mqtt.h"



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



void clsLteMqttConnIf::SetUser(uint8_t *userName, uint8_t *password)
{
}
void clsLteMqttConnIf::Connect(void)
{
}
void clsLteMqttConnIf::Disconnect(void)
{
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

/************************************ private ************************************/
void clsLteMqttIf::Clear(void)
{

}
