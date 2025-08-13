#include "srvLte.h"

#include <stddef.h>
#include "stdbool.h"
#include "stdint.h"
#include "Fibocom_AT.h"
#include "srvLte_Resource.h"
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

    GetMsgAndProcess();

    if (m_pKeeperObj->GetConnectedStatus())
    {
        
    }
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
    if(m_pKeeperObj->MsgPop(cmdType, m_CmdBuf, LTE_MSG_MAX_BYTES, m_CmdBufLen, m_timeout))
    {
        m_pInterfaceObj->MsgPush(Enum_LteNetInfo, cmdType, (uint8_t *)m_CmdBuf, m_CmdBufLen, m_timeout);
    }

    if (m_pKeeperObj->GetConnectedStatus())
    {
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

    if (m_pKeeperObj->GetConnectedStatus())
    {
    }
}


/************************************ private ************************************/
void clsLteIf::Clear(void)
{

}

void clsLteIf::GetMsgAndProcess(void)
{
    // 从接口类中获取数据
    if(m_pInterfaceObj->MsgPop(cmdType, (uint8_t *)m_CmdBuf, sizeof(m_CmdBuf), m_CmdBufLen))
    {
        m_pKeeperObj->MsgProcess(cmdType, m_CmdBuf, m_CmdBufLen);
    }
}
