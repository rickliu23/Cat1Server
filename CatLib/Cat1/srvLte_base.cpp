#include "srvLte_Base.h"
#include "LteUartPort.h"
#include "Fibocom_AT.h"

#include <stdio.h>
#include <string.h>

/******************************** 自定义的字符串搜索函数 ******************************************/

// 找出s1中第一次出现s2的位置
/*
s1: 搜寻范围
len: s1的长度
s2: 被搜索子串
*/
char *my_strstr(const char *s1, int len, const char *s2)
{
    if(*s2 == '\0')
    {
        return (char *)s1;
    }
    const char *startPos = s1;

    for(; *s1 != '\0'; s1++)
    {
        const char *p1 = s1, *p2 = s2;
        while(*p1 != '\0' && *p2 != '\0' && *p1 == *p2)
        {
            p1++;
            p2++;
        }
        if(*p2 == '\0')
        {
            if((int)s1 - (int)startPos + strlen(s2) > len)
            {
                return NULL;
            }
            return (char *)s1;
        }
    }

    return NULL;
}

// 找出s1中最后一次出现s2的位置
/*
s1: 搜寻范围
len: s1的长度
s2: 被搜索子串
*/
char *my_strrstr(const char *s1, int len, const char *s2)
{
    if(s1 == NULL || s2 == NULL || *s2 == '\0')
    {
        return NULL;
    }
    int lenLeft = len;

    char *last_occurrence = NULL;
    char *current = my_strstr(s1, lenLeft, s2);

    while(current != NULL)
    {
        last_occurrence = current;

        lenLeft = len - ((int)current - (int)s1);

        // 移动指针到下一个字符，继续查找下一个匹配位置
        current = my_strstr(current + 1, lenLeft - 1, s2);
    }

    return last_occurrence;
}

/********************************  ******************************************/

static clsLteBaseIf m_LteBaseIfObj;

clsLteBaseIf *clsLteBaseIf::GetInstance(void)
{
    return &m_LteBaseIfObj;
}

void clsLteBaseIf::Init(void)
{
    Clear();

    // LteBaseLoopStructureInit();
}

void clsLteBaseIf::Start(void)
{
    HW_LTE_Disable();
    HAL_Delay(100);
    HW_LTE_Enable();
    HAL_Delay(3000);

    isEnabled = true;

    /*************************************/
    char msg[20] = { 0 } ;
    int len;

    Fibocom_AT_Assemble_Basic(LTE_AT_INDEX_AT, LTE_AT_READ, 0, msg, sizeof(msg), &len);
    Msg_PushForNet((uint8_t *)msg, len, 300);
    Msg_PushForNet((uint8_t *)msg, len, 300);

    /*************************************/
}

void clsLteBaseIf::Stop(void)
{
    HW_LTE_Disable();

    isEnabled = false;
}

void clsLteBaseIf::OnTimerFast(void)
{
    if(isEnabled == false)
    {
        return;
    }

    Timer_TimeCount(10);
}

void clsLteBaseIf::OnTimer(void)
{
    if(isEnabled == false)
    {
        return;
    }

    Msg_SendQueueMngr(100);
    Msg_RecvQueueMngr(100);

    // BaseStateMachine(100);
}

void clsLteBaseIf::OnTimerSlow(void)
{
    if(isEnabled == false)
    {
        return;
    }
}

void clsLteBaseIf::Clear(void)
{
    // 整个模块均失能
    isEnabled = false;

    timerCount = 0x7FFFFFFF;

    // m_statusMngr.smNowStep = LTE_ENUM_SM_RESET;

    Msg_Clear();
}

/******************************** 和总线的消息收发 ******************************************/
void clsLteBaseIf::Uart_Send(uint8_t *msg, uint32_t len)
{
    HW_UART_Transmit(msg, len);
}

void clsLteBaseIf::Uart_Recv(uint8_t *msg, uint32_t len)
{
    if(len == 0)
    {
        return;
    }

    if(m_rawDataFifo.isLock == true)
    {
        return;
    }

    Timer_Reset();

    m_rawDataFifo.isLock = true;

    m_rawDataFifo.tail = m_rawDataFifo.tail == LET_INVALID_INDEX ? m_rawDataFifo.head : m_rawDataFifo.tail;

    for(uint32_t i = 0; i < len; i++)
    {
        m_rawDataFifo.array[m_rawDataFifo.head++] = msg[i++];
        m_rawDataFifo.head %= LET_RAW_FIFO_MAX;

        if(m_rawDataFifo.tail == m_rawDataFifo.head)
        {
            m_rawDataFifo.tail++;
            m_rawDataFifo.tail %= LET_RAW_FIFO_MAX;
            
            // 如果这个触发了，说明rawdata长度不够，需要增加
        }
    }

    m_rawDataFifo.isLock = false;

#if 0
    if (isFull)
    {
        // fifo 满了，所以在这里处理一下，如果fifo不满，不推荐在这里处理
        Msg_RawDataProcess();
    }
#endif
}

void Friend_Uart_Recv(uint8_t *msg, uint32_t len)
{
    m_LteBaseIfObj.Uart_Recv(msg, len);
}

/******************************** 消息处理函数 ******************************************/

void clsLteBaseIf::Msg_Clear(void)
{
    m_rawDataFifo.head = 0;
    m_rawDataFifo.tail = LET_INVALID_INDEX;
    m_rawDataFifo.isLock = false;

    m_sendFifoForNet.head = 0;
    m_sendFifoForNet.tail = LET_INVALID_INDEX;
    m_sendFifoForNet.isLock = false;

    m_sendFifoForOther.head = 0;
    m_sendFifoForOther.tail = LET_INVALID_INDEX;
    m_sendFifoForOther.isLock = false;

    m_recvFifo.head = 0;
    m_recvFifo.tail = LET_INVALID_INDEX;
    m_recvFifo.isLock = false;

    m_msgState.isWaitForReply = false;
    m_msgState.timeout = 0;
}

void clsLteBaseIf::Msg_SendQueueMngr(uint32_t t_ms)
{
    if(m_msgState.isWaitForReply == true)
    {
        m_msgState.timeout -= t_ms;

        if(m_msgState.timeout > 0)
        {
            return;
        }

        m_msgState.isWaitForReply = false;
    }

    if(m_sendFifoForNet.tail != LET_INVALID_INDEX && m_sendFifoForNet.isLock == false)
    {
        m_sendFifoForNet.isLock = true;

        Uart_Send(m_sendFifoForNet.msg[m_sendFifoForNet.tail], m_sendFifoForNet.msgLen[m_sendFifoForNet.tail]);

        m_msgState.isWaitForReply = true;
        m_msgState.timeout = m_sendFifoForNet.timeout[m_sendFifoForNet.tail];

        m_sendFifoForNet.head = m_sendFifoForNet.head == LET_INVALID_INDEX ? m_sendFifoForNet.tail : m_sendFifoForNet.head;

        m_sendFifoForNet.tail += 1;
        m_sendFifoForNet.tail %= LET_MSG_FIFO_MAX_COUNT;
        m_sendFifoForNet.tail = m_sendFifoForNet.tail == m_sendFifoForNet.head ? LET_INVALID_INDEX : m_sendFifoForNet.tail;

        m_sendFifoForNet.isLock = false;

        return;
    }

    if(m_sendFifoForOther.tail != LET_INVALID_INDEX && m_sendFifoForOther.isLock == false)
    {
        m_sendFifoForOther.isLock = true;

        Uart_Send(m_sendFifoForOther.msg[m_sendFifoForOther.tail], m_sendFifoForOther.msgLen[m_sendFifoForOther.tail]);

        m_msgState.isWaitForReply = true;
        m_msgState.timeout = m_sendFifoForOther.timeout[m_sendFifoForNet.tail];

        m_sendFifoForOther.head = m_sendFifoForOther.head == LET_INVALID_INDEX ? m_sendFifoForOther.tail : m_sendFifoForOther.head;

        m_sendFifoForOther.tail += 1;
        m_sendFifoForOther.tail %= LET_MSG_FIFO_MAX_COUNT;
        m_sendFifoForOther.tail = m_sendFifoForOther.tail == m_sendFifoForOther.head ? LET_INVALID_INDEX : m_sendFifoForOther.tail;

        m_sendFifoForOther.isLock = false;

        return;
    }
}

void clsLteBaseIf::Msg_RecvQueueMngr(uint32_t t_ms)
{

}

void clsLteBaseIf::Msg_RawDataProcess()
{
    if(m_rawDataFifo.tail == LET_INVALID_INDEX)
    {
        return;
    }

    // 接收完毕，发送端可以继续发送
    m_msgState.isWaitForReply = false;

    if(m_rawDataFifo.isLock == true)
    {
        return;
    }

    char cmdBufTemp[LET_MSG_FIFO_MAX_BYTES] = { 0 };
    int16_t cmdBufLen = 0;

    char *pos = NULL;
    int16_t startIndex = 0, endInedx = 0;

    char cmdStart[] = "AT";
    char cmdEnd[] = "\r\n";

    m_rawDataFifo.isLock = true;

    while(true)
    {
        /********** 拆出起点 **********/
        pos = my_strstr((char *)(m_rawDataFifo.array + m_rawDataFifo.tail), m_rawDataFifo.head - m_rawDataFifo.tail, cmdStart);
        if(pos == NULL)
        {
            // 没有起点
            goto FINISH;
        }

        startIndex = (uint32_t)pos - (uint32_t)m_rawDataFifo.array;

        /********** 拆出终点 **********/
        // 先尝试，看是否存在第二包数据
        pos = my_strstr((char *)(m_rawDataFifo.array + m_rawDataFifo.tail + 1), m_rawDataFifo.head - (m_rawDataFifo.tail + 1), cmdStart);
        if(pos) // 存在第二包数据
        {
            endInedx = (uint32_t)pos - (uint32_t)m_rawDataFifo.array;

            // 第二包数据之前的内容，均认为是一包数据，copy数据
            cmdBufLen = endInedx - startIndex;
            memcpy(cmdBufTemp, m_rawDataFifo.array + startIndex, cmdBufLen);
        }
        else // 不存在第二包数据时，找结尾符号
        {
            pos = my_strrstr((char *)(m_rawDataFifo.array + m_rawDataFifo.tail + 1), m_rawDataFifo.head - (m_rawDataFifo.tail + 1), cmdEnd);
            if(pos == NULL)
            {
                // 没有结尾
                goto FINISH;
            }
            endInedx = (uint32_t)pos - (uint32_t)m_rawDataFifo.array + strlen(cmdEnd);

            // copy整包数据
            cmdBufLen = endInedx - startIndex;
            memcpy(cmdBufTemp, m_rawDataFifo.array + startIndex, cmdBufLen);
        }

        Msg_PushToRecvQueue((uint8_t *)cmdBufTemp, cmdBufLen); // 塞到消息队列中

        m_rawDataFifo.tail = endInedx;

        if(m_rawDataFifo.tail >= m_rawDataFifo.head)
        {
            // 拆包结束
            m_rawDataFifo.tail = LET_INVALID_INDEX;
            break;
        }
    }

    // 由接收队列去做消息分发

FINISH:
    /********** clear all **********/
    // 能拆的都拆出来了，拆不出来的一律清理
    m_rawDataFifo.head = 0;
    m_rawDataFifo.tail = LET_INVALID_INDEX;
    memset(m_rawDataFifo.array, 0, sizeof(m_rawDataFifo.array));

    m_rawDataFifo.isLock = false;
}

bool clsLteBaseIf::Msg_PushForNet(uint8_t *msg, uint32_t len, uint32_t timeout_ms)
{
    if(m_sendFifoForNet.head == LET_INVALID_INDEX || m_sendFifoForNet.isLock == true)
    {
        return false;
    }

    m_sendFifoForNet.isLock = true;

    memcpy(m_sendFifoForNet.msg + m_sendFifoForNet.head, msg, len);
    m_sendFifoForNet.msgLen[m_sendFifoForNet.head] = len;
    m_sendFifoForNet.timeout[m_sendFifoForNet.head] = timeout_ms;

    m_sendFifoForNet.tail = m_sendFifoForNet.tail == LET_INVALID_INDEX ? m_sendFifoForNet.head : m_sendFifoForNet.tail;

    m_sendFifoForNet.head += 1;
    m_sendFifoForNet.head %= LET_MSG_FIFO_MAX_COUNT;
    m_sendFifoForNet.head = m_sendFifoForNet.head == m_sendFifoForNet.tail ? LET_INVALID_INDEX : m_sendFifoForNet.head;

    m_sendFifoForNet.isLock = false;

    return true;
}
bool clsLteBaseIf::Msg_PushToRecvQueue(uint8_t *msg, uint32_t len)
{
    for(int i = 0; i < len; i++)
    {
        HW_DEBUG_Transmit((uint8_t *)(msg + i), 1);
    }

    return true;
}

//void clsLteBaseIf::LteBaseLoopStructureInit(void)
//{
// LteBaseLoopStructure loop[LTE_ENUM_SM_MAX] =
// {
// {LTE_ENUM_SM_RESET, &clsLteBaseIf::StateMachine_Reset, 1000, 2, 0, LTE_ENUM_SM_IDLE, 0, LTE_ENUM_SM_RESET},
// {LTE_ENUM_SM_IDLE, &clsLteBaseIf::StateMachine_Idle, 1000, 2, 0, LTE_ENUM_SM_ECHO, 0, LTE_ENUM_SM_RESET},
// {LTE_ENUM_SM_ECHO, &clsLteBaseIf::StateMachine_Echo, 1000, 2, 10000, LTE_ENUM_SM_ECHO, 0, LTE_ENUM_SM_RESET},
// };

// memcpy(m_stateMachineLoop, loop, sizeof(m_stateMachineLoop));
//}



//void clsLteBaseIf::BaseStateMachine(int time_ms)
//{
// if(m_statusMngr.waitBeforNextStep_ms > 0)
// {
// m_statusMngr.waitBeforNextStep_ms -= time_ms;
// return;
// }

// if(m_statusMngr.msgStatus == LTE_ENUM_MSG_IDLE)
// {
// // 执行当前步骤
// }
// else if(m_statusMngr.msgStatus == LTE_ENUM_MSG_RECVED)
// {
// // 处理消息
// }
// else if(m_statusMngr.msgStatus == LTE_ENUM_MSG_WAIT_REPLY)
// {
// if(m_statusMngr.timeout_ms > 0)
// {
// m_statusMngr.timeout_ms -= time_ms;
// }
// else
// {
// m_statusMngr.msgStatus = LTE_ENUM_MSG_TIMEOUT;
// }
// }
// else if(m_statusMngr.msgStatus == LTE_ENUM_MSG_TIMEOUT)
// {
// // 超时机制
// }
//}

//void clsLteBaseIf::StateMachine_Reset(void)
//{
//}
//void clsLteBaseIf::StateMachine_Idle(void)
//{
//}
//void clsLteBaseIf::StateMachine_Echo(void)
//{
//}

void clsLteBaseIf::Timer_Reset(void)
{
    timerCount = LTE_TIMER_TIMEOUT;
}

void clsLteBaseIf::Timer_TimeCount(uint32_t time_ms)
{
    timerCount -= time_ms;

    if(timerCount > 0)
    {
        return;
    }

    timerCount = 0x7FFFFFFF;

    Msg_RawDataProcess(); // 消息接收停止一段时间后，认为消息接收结束，开始进行拆包操作
}
