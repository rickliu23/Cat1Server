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

    HW_Printf("Lte_Init\r\n");
}

void clsLteIf::Start(void)
{
    Clear();

    if(m_pInterfaceObj == NULL)
    {
        return;
    }

    m_pInterfaceObj->Init();
    m_pInterfaceObj->Start();

     char msg[20] = { 0 } ;
     int len;

     isEnabled = true;
     
     HW_Printf("Lte_Start\r\n");

     Fibocom_AT_Assemble_Basic(LTE_AT_INDEX_AT, LTE_AT_READ, 0, msg, sizeof(msg), &len);
     m_pInterfaceObj->MsgPush(Enum_LteNetInfo, (uint8_t *)msg, len, 300);
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
}

void clsLteIf::OnTimer(void)
{
    if(isEnabled != true)
    {
        return;
    }

    m_pInterfaceObj->OnTimer();
}

void clsLteIf::OnTimerSlow(void)
{
    if(isEnabled != true)
    {
        return;
    }

    m_pInterfaceObj->OnTimerSlow();
}


/************************************ private ************************************/
void clsLteIf::Clear(void)
{

}
