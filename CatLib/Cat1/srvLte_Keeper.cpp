#include "srvLte_Keeper.h"

#include <string.h>
#include <stddef.h>
#include "stdbool.h"
#include "stdint.h"
#include "Fibocom_AT.h"
#include "Lte_Basic.h"
#include "srvLte_Interface.h"
#include "LteUartPort.h"

static clsLteKeeperIf m_LteKeeperIfObj;


/************************************ public ************************************/
clsLteKeeperIf *clsLteKeeperIf::GetInstance(void)
{
    return &m_LteKeeperIfObj;
}

void clsLteKeeperIf::Init(void)
{
    PowerOff();
}

void clsLteKeeperIf::Start(void)
{
    Clear();
}

void clsLteKeeperIf::Stop(void)
{
    PowerOff();
}

void clsLteKeeperIf::OnTimerFast(void)
{

}

void clsLteKeeperIf::OnTimer(void)
{
    WaitAndFindNextMsg(100);
}

void clsLteKeeperIf::OnTimerSlow(void)
{

}

bool clsLteKeeperIf::MsgPop(uint8_t *msg, uint32_t lenIn, uint32_t &lenOut, int32_t &timeout_ms)
{
    if(m_msgStatus.isWaitingForPop != true)
    {
        return false;
    }

    if(lenIn < m_msgStatus.msgLen)
    {
        return false;
    }

    m_msgStatus.isWaitingForPop = false;

    memcpy(msg, m_msgStatus.msgBuf, m_msgStatus.msgLen);
    lenOut = m_msgStatus.msgLen;
    timeout_ms = m_msgStatus.timeout;

    m_msgStatus.isWaitingForReply = true;
    m_msgStatus.timeout += 50;

    return true;
}

void clsLteKeeperIf::MsgProcess(uint8_t *msg, uint32_t lenIn)
{
    if(my_strstr((char *)msg, lenIn, LTE_AT_MODULE_READY_RSP))
    {
        m_moduleStatus.isPowerOn.flag = true;
    }
}

/************************************ private ************************************/
void clsLteKeeperIf::Clear(void)
{
    LteModuleStatus temp =
    {
    // flag  retryTimes  isPeriodically   periodLeft_s   Reloade:timeout   retryTimes  period_s
        {false,     0,     false,              0,          4000,            0,          0}, // isPowerOn
        {false,     0,     false,              0,          1000,            2,          1}, // isModuleReachable
        {false,     0,     false,              0,          1000,            2,          1}, // isEchoOn
        {false,     0,     true,               0,          1000,            2,          30}, // isSimExist
        {false,     0,     true,               0,          1000,            2,          30}, // isCSQ
        {false,     0,     true,               0,          1000,            2,          30}, // isReg
    };

    memcpy(&m_moduleStatus, &temp, sizeof(LteModuleStatus));
}

void clsLteKeeperIf::PowerOff(void)
{
    HW_LTE_Disable();
}

void clsLteKeeperIf::PowerOn(void)
{
    Clear();

    HW_LTE_Enable();
}

void clsLteKeeperIf::ModuleStatusInit(void)
{
}

void clsLteKeeperIf::WaitAndFindNextMsg(uint32_t time_ms)
{
    bool isFind = false;

    // 正在等待消息被取走
    if(m_msgStatus.isWaitingForPop == true)
    {
        return;
    }

    // 正在等待回复
    if(m_msgStatus.isWaitingForReply == true)
    {
        m_msgStatus.timeout -= time_ms;
        if(m_msgStatus.timeout > 0) // 等待消息回复
        {
            return;
        }

        // 超时未收到回复
        m_msgStatus.isWaitingForReply = false;
    }

    // 找下一条消息
    if(m_moduleStatus.isPowerOn.flag == false)
    {
        PowerOn();
        m_msgStatus.timeout = m_moduleStatus.isPowerOn.timeoutReloadValue;
    }
    else if(m_moduleStatus.isModuleReachable.flag == false)
    {
        isFind = Fibocom_AT_Assemble_Basic(LTE_AT_INDEX_AT, LTE_AT_READ, 0, (char *)m_msgStatus.msgBuf, sizeof(m_msgStatus.msgBuf), (int *)&m_msgStatus.msgLen);
        m_msgStatus.timeout = m_moduleStatus.isModuleReachable.timeoutReloadValue;
    }
    else if(m_moduleStatus.isEchoOn.flag == false)
    {
        isFind = Fibocom_AT_Assemble_Basic(LTE_AT_INDEX_ECHO, LTE_AT_WRITE, 1, (char *)m_msgStatus.msgBuf, sizeof(m_msgStatus.msgBuf), (int *)&m_msgStatus.msgLen);
        m_msgStatus.timeout = m_moduleStatus.isEchoOn.timeoutReloadValue;
    }
    else if(m_moduleStatus.isSimExist.flag == false)
    {
        isFind = Fibocom_AT_Assemble_Basic(LTE_AT_INDEX_SIM_PIN, LTE_AT_READ, 0, (char *)m_msgStatus.msgBuf, sizeof(m_msgStatus.msgBuf), (int *)&m_msgStatus.msgLen);
        m_msgStatus.timeout = m_moduleStatus.isSimExist.timeoutReloadValue;
    }
    else if(m_moduleStatus.isCSQ.flag == false)
    {
        isFind = Fibocom_AT_Assemble_Basic(LTE_AT_INDEX_CSQ, LTE_AT_READ, 0, (char *)m_msgStatus.msgBuf, sizeof(m_msgStatus.msgBuf), (int *)&m_msgStatus.msgLen);
        m_msgStatus.timeout = m_moduleStatus.isCSQ.timeoutReloadValue;
    }
    else if(m_moduleStatus.isReg.flag == false)
    {
        isFind = Fibocom_AT_Assemble_Basic(LTE_AT_INDEX_NET_REG, LTE_AT_READ, 0, (char *)m_msgStatus.msgBuf, sizeof(m_msgStatus.msgBuf), (int *)&m_msgStatus.msgLen);
        m_msgStatus.timeout = m_moduleStatus.isReg.timeoutReloadValue;
    }

    if(isFind)
    {
        m_msgStatus.isWaitingForPop = true;
    }
}
