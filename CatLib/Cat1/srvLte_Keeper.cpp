#include "srvLte_Keeper.h"

#include <string.h>
#include <stddef.h>
#include "stdbool.h"
#include "stdint.h"
#include "stdlib.h"
#include <stdio.h>

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
    ModuleStatusResetPeriodically(1);
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
    if(m_nowCmd == LTE_AT_INDEX_UNKNOW && my_strstr((char *)msg, lenIn, LTE_AT_MODULE_READY_RSP))
    {
        m_moduleStatus.isPowerOn.flag = true;
    }
    else if(m_nowCmd == LTE_AT_INDEX_AT)
    {
        if(my_strstr((char *)msg, lenIn, LTE_AT_OK_RSP))
        {
            m_moduleStatus.isModuleReachable.flag = true;
            m_moduleStatus.isModuleReachable.retryTimesLeft = m_moduleStatus.isModuleReachable.retryTimesReloadValue;
        }
    }
    else if(m_nowCmd == LTE_AT_INDEX_ECHO && my_strstr((char *)msg, lenIn, LTE_AT_OK_RSP))
    {
        if(my_strstr((char *)msg, lenIn, LTE_AT_OK_RSP))
        {
            m_moduleStatus.isEchoOn.flag = true;
            m_moduleStatus.isEchoOn.retryTimesLeft = m_moduleStatus.isEchoOn.retryTimesReloadValue;
        }
    }
    else if(m_nowCmd == LTE_AT_INDEX_SIM_PIN)
    {
        if(my_strstr((char *)msg, lenIn, LTE_AT_SIM_READY_RSP))
        {
            m_moduleStatus.isSimExist.flag = true;
            m_moduleStatus.isSimExist.retryTimesLeft = m_moduleStatus.isSimExist.retryTimesReloadValue;
            m_moduleStatus.isSimExist.periodLeft_s = m_moduleStatus.isSimExist.periodReloadValue_s;
        }
    }
    else if(m_nowCmd == LTE_AT_INDEX_CSQ)
    {
        AT_CsqProcess(msg, lenIn);

        if(m_moduleStatus.rssi.level == LTE_RSSI_LEVEL_0 || m_moduleStatus.rssi.level == LTE_RSSI_LEVEL_1
                || m_moduleStatus.rssi.level == LTE_RSSI_LEVEL_2 || m_moduleStatus.rssi.level == LTE_RSSI_LEVEL_3
          )
        {
            m_moduleStatus.isCSQ.flag = true;
            m_moduleStatus.isCSQ.retryTimesLeft = m_moduleStatus.isCSQ.retryTimesReloadValue;
            m_moduleStatus.isCSQ.periodLeft_s = m_moduleStatus.isCSQ.periodReloadValue_s;
        }
    }
    else if(m_nowCmd == LTE_AT_INDEX_NET_REG)
    {
        if(AT_RegProcess(msg, lenIn) == true)
        {
            m_moduleStatus.isReg.flag = true;
            m_moduleStatus.isReg.retryTimesLeft = m_moduleStatus.isReg.retryTimesReloadValue;
            m_moduleStatus.isReg.periodLeft_s = m_moduleStatus.isReg.periodReloadValue_s;
        }
    }

    m_nowCmd = LTE_AT_INDEX_UNKNOW;
}

/************************************ private ************************************/
void clsLteKeeperIf::Clear(void)
{
    LteModuleStatus temp =
    {
        // flag  retryTimes  isPeriodically   periodLeft_s   Reloade:timeout   retryTimes  period_s
        {false,     2,          false,          0,              4000,           2,          0}, // isPowerOn
        {false,     5,          false,          0,              1000,           5,          1}, // isModuleReachable
        {false,     5,          false,          0,              1000,           5,          1}, // isEchoOn
        {false,     10,         true,           0,              1000,           10,         LTE_SIM_CHECK_TIME_S}, // isSimExist
        {false,     5,          true,           0,              1000,           5,          LTE_CSQ_CHECK_TIME_S}, // isCSQ
        {false,     5,          true,           0,              1000,           5,          LTE_REG_CHECK_TIME_S}, // isReg
        {false,     2,          false,          0,              1000,           2,          30}, // end
    };

    memcpy(&m_moduleStatus, &temp, sizeof(LteModuleStatus));

    m_moduleStatus.rssi.level = LTE_RSSI_LEVEL_INVALID;
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

void clsLteKeeperIf::ModuleStatusResetPeriodically(uint32_t t_s)
{
    LteModuleStatusStruct *ptr = &m_moduleStatus.isPowerOn;
    for ( ; ptr < &m_moduleStatus.end; ptr += 1)
    {
        if (ptr->isExecPeriodically != true)
        {
            continue;
        }

        if (ptr->flag != true)
        {
            continue;
        }

        ptr->periodLeft_s -= t_s;

        if (ptr->periodLeft_s <= 0)
        {
            ptr->flag = false;
        }
    }
}

void clsLteKeeperIf::WaitAndFindNextMsg(uint32_t time_ms)
{
    bool isFind = false;
    LteModuleStatusStruct *ptr = NULL;

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
    m_nowCmd = LTE_AT_INDEX_UNKNOW;

    // 找下一条消息
    if(m_moduleStatus.isPowerOn.flag == false)
    {
        PowerOn();

        m_nowCmd = LTE_AT_INDEX_UNKNOW;

        ptr = &m_moduleStatus.isPowerOn;
    }
    else if(m_moduleStatus.isModuleReachable.flag == false)
    {
        m_nowCmd = LTE_AT_INDEX_AT;

        isFind = Fibocom_AT_Assemble_Basic(LTE_AT_INDEX_AT, LTE_AT_READ, 0, (char *)m_msgStatus.msgBuf, sizeof(m_msgStatus.msgBuf), (int *)&m_msgStatus.msgLen);

        ptr = &m_moduleStatus.isModuleReachable;
    }
    else if(m_moduleStatus.isEchoOn.flag == false)
    {
        m_nowCmd = LTE_AT_INDEX_ECHO;

        isFind = Fibocom_AT_Assemble_Basic(LTE_AT_INDEX_ECHO, LTE_AT_WRITE, 1, (char *)m_msgStatus.msgBuf, sizeof(m_msgStatus.msgBuf), (int *)&m_msgStatus.msgLen);

        ptr = &m_moduleStatus.isEchoOn;
    }
    else if(m_moduleStatus.isSimExist.flag == false)
    {
        m_nowCmd = LTE_AT_INDEX_SIM_PIN;

        isFind = Fibocom_AT_Assemble_Basic(LTE_AT_INDEX_SIM_PIN, LTE_AT_READ, 0, (char *)m_msgStatus.msgBuf, sizeof(m_msgStatus.msgBuf), (int *)&m_msgStatus.msgLen);

        ptr = &m_moduleStatus.isSimExist;
    }
    else if(m_moduleStatus.isCSQ.flag == false)
    {
        m_nowCmd = LTE_AT_INDEX_CSQ;

        isFind = Fibocom_AT_Assemble_Basic(LTE_AT_INDEX_CSQ, LTE_AT_READ, 0, (char *)m_msgStatus.msgBuf, sizeof(m_msgStatus.msgBuf), (int *)&m_msgStatus.msgLen);

        ptr = &m_moduleStatus.isCSQ;
    }
    else if(m_moduleStatus.isReg.flag == false)
    {
        m_nowCmd = LTE_AT_INDEX_NET_REG;

        isFind = Fibocom_AT_Assemble_Basic(LTE_AT_INDEX_NET_REG, LTE_AT_READ, 0, (char *)m_msgStatus.msgBuf, sizeof(m_msgStatus.msgBuf), (int *)&m_msgStatus.msgLen);

        ptr = &m_moduleStatus.isReg;
    }

    if(isFind)
    {
        m_msgStatus.isWaitingForPop = true;

        if (ptr)
        {
            m_msgStatus.timeout = ptr->timeoutReloadValue;
            ptr->retryTimesLeft -= 1;

            if (ptr->retryTimesLeft < 0) // 超出尝试次数，重启模块
            {
                Clear();

                PowerOff();
            }
        }
    }
}

void clsLteKeeperIf::AT_CsqProcess(uint8_t *msg, uint32_t lenIn)
{
    int csq = 0;
    int8_t rssiValue;

    if(my_strstr((char *)msg, lenIn, LTE_AT_CSQ_RSP) == NULL)
    {
        return;
    }

    if(find_nth_num((char *)msg, lenIn, 1, csq) == false)
    {
        return;
    }

    rssiValue = -113 + csq * 2;

    if(rssiValue >= 0 || rssiValue == -113)
    {
        m_moduleStatus.rssi.level = LTE_RSSI_LEVEL_INVALID;
        HW_Printf("RSSI LEVEL INVALID\r\n");
    }
    else if(rssiValue > -67)
    {
        m_moduleStatus.rssi.level = LTE_RSSI_LEVEL_3;
        HW_Printf("RSSI LEVEL 3\r\n");
    }
    else if(rssiValue > -83)
    {
        m_moduleStatus.rssi.level = LTE_RSSI_LEVEL_2;
        HW_Printf("RSSI LEVEL 2\r\n");
    }
    else if(rssiValue > -99)
    {
        m_moduleStatus.rssi.level = LTE_RSSI_LEVEL_1;
        HW_Printf("RSSI LEVEL 1\r\n");
    }
    else if(rssiValue > -111)
    {
        m_moduleStatus.rssi.level = LTE_RSSI_LEVEL_0;
        HW_Printf("RSSI LEVEL 0\r\n");
    }

    m_moduleStatus.rssi.value = rssiValue;

    char info[20] = { 0 };
    sprintf(info, "rssi value: %d\r\n", rssiValue);
    HW_Printf(info);
}

bool clsLteKeeperIf::AT_RegProcess(uint8_t *msg, uint32_t lenIn)
{
    if(my_strstr((char *)msg, lenIn, LTE_AT_REG_RSP) == NULL)
    {
        return false;
    }

    int stat;

    if(find_nth_num((char *)msg, lenIn, 2, stat) == false)
    {
        return false;
    }

    if(stat == 1)
    {
        return true;
    }

    return false;
}
