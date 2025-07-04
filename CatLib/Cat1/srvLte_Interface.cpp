#include "srvLte_Interface.h"
#include <string.h>
#include "Lte_Basic.h"
#include "LteUartPort.h"

/************************************ �������� raw data �洢�� ************************************/
void LteRawFifoStructure::MsgInit(void)
{
    m_isLock = false;
    memset(m_array, 0, sizeof(m_array));
    m_head = 0;
    m_tail = LTE_INVALID_INDEX;
}

bool LteRawFifoStructure::MsgPush(uint8_t *msg, uint32_t lenIn)
{
    if(m_head == LTE_INVALID_INDEX || m_isLock == true)
    {
        return false;
    }

    if(lenIn > (LTE_RAW_FIFO_MAX_BYTES - m_head))
    {
        return false;
    }

    m_isLock = true;

    // Ŀǰ����ѭ�����У�Ĭ��ÿ�ζ�������֮ǰ�����ݴ����
    m_tail = m_tail == LTE_INVALID_INDEX ? m_head : m_tail; // βָ�����

    memcpy(m_array + m_head, msg, lenIn);
    m_head += lenIn;

    m_isLock = false;

    return true;
}

bool LteRawFifoStructure::MsgPop(uint8_t *msg, uint32_t lenIn, uint32_t &lenOut)
{
    memset(msg, 0, lenIn);
    lenOut = 0;

    if(m_tail == LTE_INVALID_INDEX || m_isLock == true)
    {
        return false;
    }

    if(msg == NULL || lenIn < LTE_MSG_FIFO_MAX_BYTES)
    {
        return false;
    }

    bool isNeedClear = false;;
    char *pos = NULL;
    int16_t startIndex = 0, endInedx = 0;

    char cmdStart[] = "AT";
    char cmdEnd[] = "\r\n";

    m_isLock = true;

    /********** ������ **********/
    pos = my_strstr((char *)(m_array + m_tail), m_head - m_tail, cmdStart);
    if(pos != NULL) // �������
    {
        // ����±�
        startIndex = (uint32_t)pos - (uint32_t)m_array;

        // ʣ�����Ч�ֽ�����
        int16_t lenLeft = (uint32_t)m_head - (uint32_t)(m_array + startIndex);

        /********** ���Բ���յ� **********/

        // �ȳ��ԣ����Ƿ���ڵڶ�������
        pos = my_strstr((char *)(m_array + startIndex + 1), lenLeft - 1, cmdStart);
        if(pos != NULL) // ���ڵڶ�������
        {
            endInedx = (uint32_t)pos - (uint32_t)m_array;

            // �ڶ�������֮ǰ�����ݣ�����Ϊ��һ�����ݣ�copy����
            lenOut = endInedx - startIndex;
            memcpy(msg, m_array + startIndex, lenOut);

            m_tail = endInedx;
        }
        else // �����ڵڶ�������ʱ���ҽ�β����
        {
            isNeedClear = true;

            pos = my_strrstr((char *)(m_array + startIndex + 1), lenLeft - 1, cmdEnd);
            if(pos != NULL) // ���ڽ�β����
            {
                endInedx = (uint32_t)pos - (uint32_t)m_array + strlen(cmdEnd); // ��Ч�ַ���Ҫ���Ͻ�β����

                // copy��������
                lenOut = endInedx - startIndex;
                memcpy(msg, m_array + startIndex, lenOut);

                m_tail = endInedx;
            }
        }
    }
    else // �����
    {
        isNeedClear = true;
    }

    if(isNeedClear)
    {
        MsgInit();
    }

    m_isLock = false;

    if(lenOut)
    {
        return true;
    }

    return false;
}

/************************************ ��Ϣ���� ************************************/

void LteMsgFifoStructure::MsgInit(void)
{
    m_isLock = false;

    memset(m_msg, 0, sizeof(m_msg));
    memset(m_msgLen, 0, sizeof(m_msgLen));
    memset(m_timeout, 0, sizeof(m_timeout));

    m_head = 0;
    m_tail = LTE_INVALID_INDEX;
}

bool LteMsgFifoStructure::MsgPush(uint8_t *msg, uint32_t lenIn)
{
    uint32_t timeout = 0;

    return MsgPush(msg, lenIn, timeout);
}

bool LteMsgFifoStructure::MsgPop(uint8_t *msg, uint32_t lenIn, uint32_t &lenOut)
{
    uint32_t timeout = 0;

    return MsgPop(msg, lenIn, lenOut, timeout);
}

bool LteMsgFifoStructure::MsgPush(uint8_t *msg, uint32_t lenIn, uint32_t timeout_ms)
{
    if(m_head == LTE_INVALID_INDEX || m_isLock == true)
    {
        return false;
    }

    if(lenIn > LTE_MSG_FIFO_MAX_BYTES)
    {
        return false;
    }

    m_isLock = true;

    memcpy(m_msg + m_head, msg, lenIn);
    m_msgLen[m_head] = lenIn;
    m_timeout[m_head] = timeout_ms;

    m_tail = m_tail == LTE_INVALID_INDEX ? m_head : m_tail;

    m_head += 1;
    m_head %= LTE_MSG_FIFO_MAX_COUNT;
    m_head = m_head == m_tail ? LTE_INVALID_INDEX : m_head;

    m_isLock = false;

    return true;
}

bool LteMsgFifoStructure::MsgPop(uint8_t *msg, uint32_t lenIn, uint32_t &lenOut, uint32_t &timeout_ms)
{
    if(m_tail == LTE_INVALID_INDEX || m_isLock == true)
    {
        return false;
    }

    if(lenIn < LTE_MSG_FIFO_MAX_BYTES)
    {
        return false;
    }

    memset(msg, 0, lenIn);
    lenOut = 0;
    timeout_ms = 0;

    m_isLock = true;

    memcpy(msg, m_msg[m_tail], m_msgLen[m_tail]);
    lenOut = m_msgLen[m_tail];
    timeout_ms = m_timeout[m_tail];

    m_head = m_head == LTE_INVALID_INDEX ? m_tail : m_head;

    m_tail += 1;
    m_tail %= LTE_MSG_FIFO_MAX_COUNT;
    m_tail = m_tail == m_head ? LTE_INVALID_INDEX : m_tail;

    m_isLock = false;

    return true;
}
/************************************ ��Ϣ���� END ************************************/


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
    SendDataProcess();
}

void clsLteInterfaceIf::OnTimerSlow(void)
{

}

bool clsLteInterfaceIf::MsgPush(Enum_LteMsgType type, uint8_t *msg, uint32_t lenIn, uint32_t timeout_ms)
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
/************************************ private ************************************/
void clsLteInterfaceIf::Clear(void)
{
    m_rawData.MsgInit();
    
    m_netSendFifo.MsgInit();
    m_otherSendFifo.MsgInit();
    
    m_recvFifo.MsgInit();
    
    timerCount = 0x7FFFFFFF;
}

void clsLteInterfaceIf::SendDataProcess(void)
{
    if(m_netSendFifo.MsgPop(m_sendCmdBuf, LTE_MSG_FIFO_MAX_BYTES, m_sendCmdBufLen, timeout_ms) != true)
    {
        if(m_otherSendFifo.MsgPop(m_sendCmdBuf, LTE_MSG_FIFO_MAX_BYTES, m_sendCmdBufLen, timeout_ms) != true)
        {
            return;
        }
    }
    
    HW_Printf("Send to module\r\n");
    for(int i = 0; i < m_sendCmdBufLen; i++)
    {
        HW_DEBUG_Transmit((uint8_t *)(m_sendCmdBuf + i), 1);
    }

    HW_UART_Transmit(m_sendCmdBuf, m_sendCmdBufLen);
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
    // ���
    while(m_rawData.MsgPop(m_recvCmdBuf, LTE_MSG_FIFO_MAX_BYTES, m_recvCmdBufLen) == true)
    {
        for(int i = 0; i < m_recvCmdBufLen; i++)
        {
            HW_DEBUG_Transmit((uint8_t *)(m_recvCmdBuf + i), 1);
        }

        // �浽������Ϣ����
        if(m_recvFifo.MsgPush(m_recvCmdBuf, m_recvCmdBufLen) != true)
        {
            break;
        }
    }
}

/******************************** ����״̬ά�ֺ��� ******************************************/

void clsLteInterfaceIf::Timer_Reset(void)
{
    timerCount = LTE_TIMER_TIMEOUT;
}

void clsLteInterfaceIf::Timer_TimeCount(uint32_t time_ms)
{
    timerCount -= time_ms;

    if(timerCount > 0)
    {
        return;
    }

    timerCount = 0x7FFFFFFF;

    RawDataProcess(); // ��Ϣ����ֹͣһ��ʱ�����Ϊ��Ϣ���ս�������ʼ���в������
}
