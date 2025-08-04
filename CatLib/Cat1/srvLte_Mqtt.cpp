#include "srvLte_Interface.h"
#include <string.h>
#include "srvLte_Basic.h"
#include "LteUartPort.h"
#include <stdio.h>

/************************************ public ************************************/

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

bool clsLteMqttIf::MsgPush(uint8_t *msg, uint32_t lenIn, int32_t timeout_ms)
{

}


bool clsLteMqttIf::MsgPop(uint8_t *msg, uint32_t lenIn, uint32_t &lenOut)
{

    return true;
}

/************************************ private ************************************/
void clsLteMqttIf::Clear(void)
{

}
