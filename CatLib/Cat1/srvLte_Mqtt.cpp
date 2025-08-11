#include "srvLte_Mqtt.h"

static clsLteMqttIf m_LteMqttIfObj;

/************************************ public ************************************/
clsLteMqttIf *clsLteMqttIf::GetInstance(void)
{
    return &m_LteMqttIfObj;
}

void clsLteMqttIf::Init(void)
{

}

void clsLteMqttIf::Start(void)
{
    Clear();
}

void clsLteMqttIf::Stop(void)
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
