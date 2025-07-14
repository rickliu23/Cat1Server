#include "srvLte.h"

#include <stddef.h>
#include "stdbool.h"
#include "stdint.h"
#include "Fibocom_AT.h"
#include "srvLte_Basic.h"
#include "srvLte_Interface.h"
#include "LteUartPort.h"

static clsLteIf m_LteIfObj;


/************************************ public ************************************/
clsLteIf *clsLteIf::GetInstance(void)
{
    return &m_LteIfObj;
}

void clsLteIf::Init(void)
{
    m_pInterfaceObj = clsLteInterfaceIf::GetInstance();
    m_pKeeperObj = clsLteKeeperIf::GetInstance();
}

void clsLteIf::Start(void)
{
    Clear();

    if(m_pInterfaceObj == NULL || m_pKeeperObj == NULL)
    {
        while(1);
    }

    m_pInterfaceObj->Init();
    m_pInterfaceObj->Start();

    m_pKeeperObj->Init();
    m_pKeeperObj->Start();

    isEnabled = true;

    HW_Printf("Lte_Start\r\n");
}

void clsLteIf::Stop(void)
{
    m_pInterfaceObj->Stop();
    isEnabled = false;
}

void clsLteIf::OnTimerFast(void)
{
    if(isEnabled != true)
    {
        return;
    }

    m_pInterfaceObj->OnTimerFast();
    m_pKeeperObj->OnTimerFast();
}

void clsLteIf::OnTimer(void)
{
    if(isEnabled != true)
    {
        return;
    }

    m_pInterfaceObj->OnTimer();
    m_pKeeperObj->OnTimer();

    // 获取keeper中需要发送的信息
    if(m_pKeeperObj->MsgPop(m_CmdBuf, LTE_MSG_MAX_BYTES, m_CmdBufLen, m_timeout))
    {
        m_pInterfaceObj->MsgPush(Enum_LteNetInfo, (uint8_t *)m_CmdBuf, m_CmdBufLen, m_timeout);
    }

    // 从接口类中获取数据
    if(m_pInterfaceObj->MsgPop((uint8_t *)m_CmdBuf, sizeof(m_CmdBuf), m_CmdBufLen))
    {
        if (MsgClassify(m_CmdBuf, m_CmdBufLen) == enum_lteKeeper)
        {
            m_pKeeperObj->MsgProcess(m_CmdBuf, m_CmdBufLen);
        }
    }
}

void clsLteIf::OnTimerSlow(void)
{
    if(isEnabled != true)
    {
        return;
    }

    m_pInterfaceObj->OnTimerSlow();
    m_pKeeperObj->OnTimerSlow();
}


/************************************ private ************************************/
void clsLteIf::Clear(void)
{

}

LTE_ENUM_MSG_TYPE clsLteIf::MsgClassify(uint8_t *msg, uint32_t lenIn)
{
    
    return enum_lteKeeper;
}
