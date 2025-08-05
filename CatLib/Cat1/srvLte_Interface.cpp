#include "srvLte_Interface.h"
#include <string.h>
#include "srvLte_Basic.h"
#include "LteUartPort.h"
#include <stdio.h>

#define LTE_INTERFACE_TIME_MAX  (0x7FFFFFFF)

/************************************ public ************************************/
void clsLteInterfaceIf::OnTimerFast(void)
{
    Timer_TimeCount(LTE_TIMEFAST_TIMEGAP_MS);
}

void clsLteInterfaceIf::OnTimer(void)
{
    SendDataProcess(LTE_TIME_TIMEGAP_MS);
}

void clsLteInterfaceIf::OnTimerSlow(void)
{

}

bool clsLteInterfaceIf::MsgPush(Enum_LteMsgType type, uint8_t *msg, uint32_t lenIn, int32_t timeout_ms)
{
    if(type == Enum_LteNetInfo)
    {
        return m_netSendFifo.MsgPush(msg, lenIn, timeout_ms);
    }
    else
    {
        return m_otherSendFifo.MsgPush(msg, lenIn, timeout_ms);
    }
}

bool clsLteInterfaceIf::MsgPop(uint8_t *msg, uint32_t lenIn, uint32_t &lenOut)
{
    bool res;
    res = m_recvFifo.MsgPop(msg, lenIn, lenOut);

    return res;
}

/************************************ private ************************************/
void clsLteInterfaceIf::Clear(void)
{
    m_timeout = 0;

    m_rawData.MsgInit();

    m_netSendFifo.MsgInit();
    m_otherSendFifo.MsgInit();

    m_recvFifo.MsgInit();

    timerCount = LTE_INTERFACE_TIME_MAX;
}

void clsLteInterfaceIf::SendDataProcess(uint32_t time_ms)
{
    // waiting for response
    if(m_timeout > 0)
    {
        m_timeout -= time_ms;
        return;
    }

    // try to find msg
    if(m_netSendFifo.MsgPop(m_sendCmdBuf, LTE_MSG_MAX_BYTES, m_sendCmdBufLen, m_timeout) != true)
    {
        if(m_otherSendFifo.MsgPop(m_sendCmdBuf, LTE_MSG_MAX_BYTES, m_sendCmdBufLen, m_timeout) != true)
        {
            return;
        }
    }

    HW_Printf("Send cmd:\r\n");
    for(int i = 0; i < m_sendCmdBufLen; i++)
    {
        HW_DEBUG_Transmit((uint8_t *)(m_sendCmdBuf + i), 1);
    }

    // send by uart
    HW_UART_Transmit(m_sendCmdBuf, m_sendCmdBufLen);
}

void clsLteInterfaceIf::RawDataRecv(uint8_t *msg, uint32_t lenIn)
{
    Timer_Reset();
    m_rawData.MsgPush(msg, lenIn);
}

void friend_RawDataRecv(uint8_t *msg, uint32_t lenIn)
{
    clsLteInterfaceIf::GetInstance()->RawDataRecv(msg, lenIn);
}

void clsLteInterfaceIf::RawDataProcess(void)
{
    // 拆包
    while(m_rawData.MsgPop(m_recvCmdBuf, LTE_MSG_MAX_BYTES, m_recvCmdBufLen) == true)
    {
        HW_Printf("Recv cmd:\r\n");
        for(int i = 0; i < m_recvCmdBufLen; i++)
        {
            HW_DEBUG_Transmit((uint8_t *)(m_recvCmdBuf + i), 1);
        }

        // 存到接收消息队列
        if(m_recvFifo.MsgPush(m_recvCmdBuf, m_recvCmdBufLen) != true)
        {
            break;
        }
    }
}

/******************************** 基础状态维持函数 ******************************************/

void clsLteInterfaceIf::Timer_Reset(void)
{
    timerCount = LTE_MSG_INTERFACE_RECV_TIMEOUT_MS;
}

void clsLteInterfaceIf::Timer_TimeCount(uint32_t time_ms)
{
    timerCount -= time_ms;

    if(timerCount > 0)
    {
        return;
    }

    timerCount = LTE_INTERFACE_TIME_MAX;

    RawDataProcess(); // 消息接收停止一段时间后，认为消息接收结束，开始进行拆包操作
}
