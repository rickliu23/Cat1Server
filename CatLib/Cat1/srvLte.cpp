#include "srvLte.h"

#include <stddef.h>
#include "stdbool.h"
#include "stdint.h"
#include "Fibocom_AT.h"
#include "Lte_Basic.h"
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

    HW_Printf("Lte_Init\r\n");
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

//    char msg[20] = { 0 } ;
//    int len;

    isEnabled = true;

    HW_Printf("Lte_Start\r\n");

//    Fibocom_AT_Assemble_Basic(LTE_AT_INDEX_AT, LTE_AT_READ, 0, msg, sizeof(msg), &len);
//    m_pInterfaceObj->MsgPush(Enum_LteNetInfo, (uint8_t *)msg, len, 300);
//    m_pInterfaceObj->MsgPush(Enum_LteNetInfo, (uint8_t *)msg, len, 300);
//    m_pInterfaceObj->MsgPush(Enum_LteNetInfo, (uint8_t *)msg, len, 300);
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

    if(m_pKeeperObj->MsgPop(m_CmdBuf, sizeof(LTE_MSG_MAX_BYTES), m_CmdBufLen, m_timeout))
    {
        m_pInterfaceObj->MsgPush(Enum_LteNetInfo, (uint8_t *)m_CmdBuf, m_CmdBufLen, m_timeout);
    }

    Enum_LteMsgType type;
    if(m_pInterfaceObj->MsgPop(type, (uint8_t *)m_CmdBuf, sizeof(m_CmdBuf), m_CmdBufLen))
    {

        HW_Printf("recv :\r\n");
        for(int i = 0; i < m_CmdBufLen; i++)
        {
            HW_DEBUG_Transmit((uint8_t *)(m_CmdBuf + i), 1);
        }


        m_pKeeperObj->MsgProcess(m_CmdBuf, m_CmdBufLen);
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
