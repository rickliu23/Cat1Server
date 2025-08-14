#include "srvLte_Interface.h"
#include <string.h>
#include "srvLte_Resource.h"
#include "LteUartPort.h"
#include <stdio.h>


static clsLteInterfaceIf m_LteInterfaceIfObj;

/************************************ public ************************************/
clsLteInterfaceIf *clsLteInterfaceIf::GetInstance(void)
{
    return &m_LteInterfaceIfObj;
}

void clsLteInterfaceIf::Init(void)
{

}

void clsLteInterfaceIf::Start(void)
{
    Clear();
}

void clsLteInterfaceIf::Stop(void)
{

}

void clsLteInterfaceIf::OnTimerFast(void)
{
    Timer_TimeCount(10);
}

void clsLteInterfaceIf::OnTimer(void)
{
    SendDataProcess(100);
}

void clsLteInterfaceIf::OnTimerSlow(void)
{

}

bool clsLteInterfaceIf::MsgPush(Enum_LteMsgType type, LTE_AT_CMD_TYPE cmdType, uint8_t *msg, uint32_t lenIn, int32_t timeout_ms)
{
    if(type == Enum_LteNetInfo)
    {
        return m_netSendFifo.MsgPush(cmdType, msg, lenIn, timeout_ms);
    }
    else
    {
        return m_otherSendFifo.MsgPush(cmdType, msg, lenIn, timeout_ms);
    }
}

bool clsLteInterfaceIf::MsgPop(LTE_AT_CMD_TYPE &cmdType, uint8_t *msg, uint32_t lenIn, uint32_t &lenOut)
{
    bool res;
    res = m_recvFifo.MsgPop(cmdType, msg, lenIn, lenOut);

    return res;
}

/************************************ private ************************************/
void clsLteInterfaceIf::Clear(void)
{
    m_operateCmd.timeout = 0;

    m_rawData.MsgInit();

    m_netSendFifo.MsgInit();
    m_otherSendFifo.MsgInit();

    m_recvFifo.MsgInit();

    timerCount = 0x7FFFFFFF;
}

void clsLteInterfaceIf::SendDataProcess(uint32_t time_ms)
{
    if(!m_operateCmd.isRpyRecved)
    {
        if(m_operateCmd.timeout > 0)
        {
            m_operateCmd.timeout -= time_ms;
            return;
        }
    }

    if(m_netSendFifo.MsgPop(m_operateCmd.cmdType, m_operateCmd.cmdBuf, LTE_MSG_MAX_BYTES, m_operateCmd.bufLen, m_operateCmd.timeout) != true)
    {
        if(m_otherSendFifo.MsgPop(m_operateCmd.cmdType, m_operateCmd.cmdBuf, LTE_MSG_MAX_BYTES, m_operateCmd.bufLen, m_operateCmd.timeout) != true)
        {
            return;
        }
    }

    HW_Printf("Send cmd:\r\n");
    for(int i = 0; i < m_operateCmd.bufLen; i++)
    {
        HW_DEBUG_Transmit((uint8_t *)(m_operateCmd.cmdBuf + i), 1);
    }

    m_operateCmd.isRpyRecved = false;
    HW_UART_Transmit(m_operateCmd.cmdBuf, m_operateCmd.bufLen);
}

void clsLteInterfaceIf::RawDataRecv(uint8_t *msg, uint32_t lenIn)
{
    Timer_Reset();
    m_rawData.MsgPush(msg, lenIn);
}

void friend_RawDataRecv(uint8_t *msg, uint32_t lenIn)
{
    m_LteInterfaceIfObj.RawDataRecv(msg, lenIn);
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

        // 做分类
        cmdType = MsgClassify(m_recvCmdBuf, m_recvCmdBufLen);

        // 存到接收消息队列
        if(m_recvFifo.MsgPush(cmdType, m_recvCmdBuf, m_recvCmdBufLen) != true)
        {
            break;
        }
    }
}

LTE_AT_CMD_TYPE clsLteInterfaceIf::MsgClassify(uint8_t *msg, uint32_t lenIn)
{
    return LTE_AT_CMD_UNKNOW;
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

    timerCount = 0x7FFFFFFF;

    RawDataProcess(); // 消息接收停止一段时间后，认为消息接收结束，开始进行拆包操作
}
