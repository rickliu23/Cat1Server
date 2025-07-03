#include "srvLte_Base.h"
#include "LteUartPort.h"
#include "Fibocom_AT.h"

#include <stdio.h>
#include <string.h>

/******************************** �Զ�����ַ����������� ******************************************/

// �ҳ�s1�е�һ�γ���s2��λ��
/*
s1: ��Ѱ��Χ
len: s1�ĳ���
s2: �������Ӵ�
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

// �ҳ�s1�����һ�γ���s2��λ��
/*
s1: ��Ѱ��Χ
len: s1�ĳ���
s2: �������Ӵ�
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

        // �ƶ�ָ�뵽��һ���ַ�������������һ��ƥ��λ��
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
    HW_Printf("init\r\n");

    Clear();
}

void clsLteBaseIf::Start(void)
{
    isEnabled = true;

    KA_StatusArrayInit();

    HW_Printf("start\r\n");

    // /*************************************/
    // char msg[20] = { 0 } ;
    // int len;

    // Fibocom_AT_Assemble_Basic(LTE_AT_INDEX_AT, LTE_AT_READ, 0, msg, sizeof(msg), &len);
    // Msg_PushForNet((uint8_t *)msg, len, 300);
    // Msg_PushForNet((uint8_t *)msg, len, 300);

    // /*************************************/
}

void clsLteBaseIf::Stop(void)
{
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

    KA_StateMachine(100);
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
    // ����ģ���ʧ��
    isEnabled = false;

    timerCount = 0x7FFFFFFF;

    // m_statusMngr.smNowStep = LTE_ENUM_SM_RESET;

    Msg_Clear();
}

/******************************** �����ߵ���Ϣ�շ� ******************************************/
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

            // �����������ˣ�˵��rawdata���Ȳ�������Ҫ����
        }
    }

    m_rawDataFifo.isLock = false;

#if 0
    if(isFull)
    {
        // fifo ���ˣ����������ﴦ��һ�£����fifo���������Ƽ������ﴦ��
        Msg_RawDataProcess();
    }
#endif
}

void Friend_Uart_Recv(uint8_t *msg, uint32_t len)
{
    m_LteBaseIfObj.Uart_Recv(msg, len);
}

/******************************** ��Ϣ������ ******************************************/

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
    if(m_recvFifo.tail == LET_INVALID_INDEX || m_recvFifo.isLock == true)
    {
        return;
    }

    m_recvFifo.isLock = true;

    KA_MsgProcess(m_recvFifo.msg[m_recvFifo.tail], m_recvFifo.msgLen[m_recvFifo.tail]);

    m_recvFifo.head = m_recvFifo.head == LET_INVALID_INDEX ? m_recvFifo.tail : m_recvFifo.head;

    m_recvFifo.tail++;
    m_recvFifo.tail %= LET_MSG_FIFO_MAX_COUNT;
    m_recvFifo.tail = m_recvFifo.tail == m_recvFifo.head ? LET_INVALID_INDEX : m_recvFifo.tail;

    m_recvFifo.isLock = false;
}

void clsLteBaseIf::Msg_RawDataProcess()
{
    if(m_rawDataFifo.tail == LET_INVALID_INDEX)
    {
        return;
    }

    // ������ϣ����Ͷ˿��Լ�������
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
        /********** ������ **********/
        pos = my_strstr((char *)(m_rawDataFifo.array + m_rawDataFifo.tail), m_rawDataFifo.head - m_rawDataFifo.tail, cmdStart);
        if(pos == NULL)
        {
            // û�����
            goto FINISH;
        }

        startIndex = (uint32_t)pos - (uint32_t)m_rawDataFifo.array;

        /********** ����յ� **********/
        int16_t lenLeft = (uint32_t)m_rawDataFifo.head - (uint32_t)(m_rawDataFifo.array + startIndex + 1);

        // �ȳ��ԣ����Ƿ���ڵڶ�������
        pos = my_strstr((char *)(m_rawDataFifo.array + startIndex + 1), lenLeft, cmdStart);
        if(pos) // ���ڵڶ�������
        {
            endInedx = (uint32_t)pos - (uint32_t)m_rawDataFifo.array;

            // �ڶ�������֮ǰ�����ݣ�����Ϊ��һ�����ݣ�copy����
            cmdBufLen = endInedx - startIndex;
            memcpy(cmdBufTemp, m_rawDataFifo.array + startIndex, cmdBufLen);
        }
        else // �����ڵڶ�������ʱ���ҽ�β����
        {
            pos = my_strrstr((char *)(m_rawDataFifo.array + startIndex + 1), lenLeft, cmdEnd);
            if(pos == NULL)
            {
                // û�н�β
                goto FINISH;
            }
            endInedx = (uint32_t)pos - (uint32_t)m_rawDataFifo.array + strlen(cmdEnd);

            // copy��������
            cmdBufLen = endInedx - startIndex;
            memcpy(cmdBufTemp, m_rawDataFifo.array + startIndex, cmdBufLen);
        }

        Msg_PushToRecvQueue((uint8_t *)cmdBufTemp, cmdBufLen); // ������Ϣ������

        m_rawDataFifo.tail = endInedx;

        if(m_rawDataFifo.tail >= m_rawDataFifo.head)
        {
            // �������
            m_rawDataFifo.tail = LET_INVALID_INDEX;
            break;
        }
    }

    // �ɽ��ն���ȥ����Ϣ�ַ�

FINISH:
    /********** clear all **********/
    // �ܲ�Ķ�������ˣ��𲻳�����һ������
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

    if(m_recvFifo.head == LET_INVALID_INDEX || m_recvFifo.isLock == true)
    {
        return false;
    }

    m_recvFifo.isLock = true;

    memcpy(m_recvFifo.msg + m_recvFifo.head, msg, len);
    m_recvFifo.msgLen[m_recvFifo.head] = len;

    m_recvFifo.tail = m_recvFifo.tail == LET_INVALID_INDEX ? m_recvFifo.head : m_recvFifo.tail;

    m_recvFifo.head += 1;
    m_recvFifo.head %= LET_MSG_FIFO_MAX_COUNT;
    m_recvFifo.head = m_recvFifo.head == m_recvFifo.tail ? LET_INVALID_INDEX : m_recvFifo.head;

    m_recvFifo.isLock = false;

    return true;
}

/******************************** ����״̬ά�ֺ��� ******************************************/
void clsLteBaseIf::KA_StatusArrayInit(void)
{
    LteKeepAliveStruct keepAliveStatusArray[LET_KEEPALIVE_ARRAY_COUNT] =
    {
      //index   isFirst  cmd                  type           para   tryTimes  reload  timeout   reload   recv   succeed   wait  reload   succeed step,  failed step
        {0,     true,    LTE_AT_INDEX_UNKNOW,  LTE_AT_READ,    0,     0,        0,      0,       200,     false,  false,    0,     0,       1,          0}, // disableģ��
        {1,     true,    LTE_AT_INDEX_UNKNOW,  LTE_AT_READ,    0,     0,        0,      0,       5100,    false,  false,    0,     0,       2,          0}, // enableģ��
        {2,     true,    LTE_AT_INDEX_AT,      LTE_AT_READ,    0,     0,        2,      0,       1100,    false,  false,    0,     0,       3,          0}, // ���Է�AT
        {3,     true,    LTE_AT_INDEX_ECHO,    LTE_AT_WRITE,   1,     0,        5,      0,       2000,    false,  false,    0,     0,       4,          0}, // �򿪻���
        {4,     true,    LTE_AT_INDEX_SIM_PIN, LTE_AT_READ,    1,     0,        5,      0,       2000,    false,  false,    0,     5000,    4,          4}, // ��ѯ�Ƿ����sim��
    };

    memcpy(m_keepAliveStatusArray, keepAliveStatusArray, sizeof(keepAliveStatusArray));

    m_nowStep = 0;
}

void clsLteBaseIf::KA_StateMachine(uint32_t t_ms)
{
    if(m_keepAliveStatusArray[m_nowStep].isRecved == false)
    {
        if(m_keepAliveStatusArray[m_nowStep].timeout_ms > 0)
        {
            m_keepAliveStatusArray[m_nowStep].timeout_ms -= t_ms;
            return;
        }
    }

    if(m_keepAliveStatusArray[m_nowStep].timewait_ms > 0)
    {
        m_keepAliveStatusArray[m_nowStep].timewait_ms -= t_ms;

        return;
    }

    // ��ǰ����ִ�гɹ�
    if(m_keepAliveStatusArray[m_nowStep].isSucceed == true)
    {
        // ȥԤ������һ��
        m_nowStep = m_keepAliveStatusArray[m_nowStep].stepAfterSucceed;

        // ��һ���ǵ�һ��ִ��
        m_keepAliveStatusArray[m_nowStep].isFirst = true;

        // ���صȴ�ʱ��
        m_keepAliveStatusArray[m_nowStep].timewait_ms = m_keepAliveStatusArray[m_nowStep].reloadtimewait_ms;
    }
    else // ��ǰ����ִ��ʧ��
    {
        // ���Դ�������
        if(m_keepAliveStatusArray[m_nowStep].tryTimes <= 0)
        {
            // ȥԤ������һ��
            m_nowStep = m_keepAliveStatusArray[m_nowStep].stepAfterFailed;

            // ��һ��ִ��
            m_keepAliveStatusArray[m_nowStep].isFirst = true;

            // ���صȴ�ʱ��
            m_keepAliveStatusArray[m_nowStep].timewait_ms = m_keepAliveStatusArray[m_nowStep].reloadtimewait_ms;
        }
        else // ��������
        {
            m_keepAliveStatusArray[m_nowStep].tryTimes -= 1;
        }
    }

    // �˴���λfalse�����յ���Ϣ���������Ԥ�ڣ�����Ϊtrue
    m_keepAliveStatusArray[m_nowStep].isRecved = false;
    m_keepAliveStatusArray[m_nowStep].isSucceed = false;

    if(m_nowStep == 0)
    {
        HW_Printf("disable\r\n");

        HW_LTE_Disable();

        // ֱ������Ϊ�ɹ����ȴ���ʱ����
        m_keepAliveStatusArray[m_nowStep].isSucceed = true;
    }
    else if(m_nowStep == 1)
    {
        HW_Printf("enable\r\n");

        HW_LTE_Enable();
    }
    else
    {
        char msg[20] = { 0 };
        int len;
        Fibocom_AT_Assemble_Basic(m_keepAliveStatusArray[m_nowStep].cmd, m_keepAliveStatusArray[m_nowStep].type, m_keepAliveStatusArray[m_nowStep].para, msg, sizeof(msg), &len);
        Msg_PushForNet((uint8_t *)msg, len, m_keepAliveStatusArray[m_nowStep].timeout_ms - 100);
    }

    // ���¼��س�ʱʱ��
    m_keepAliveStatusArray[m_nowStep].timeout_ms = m_keepAliveStatusArray[m_nowStep].reloadTimeout_ms;

    // ����ǵ�һ��ִ�У���Ҫ���¼������Դ���
    if(m_keepAliveStatusArray[m_nowStep].isFirst == true)
    {
        m_keepAliveStatusArray[m_nowStep].tryTimes = m_keepAliveStatusArray[m_nowStep].reloadTryTimes;

        m_keepAliveStatusArray[m_nowStep].isFirst = false;
    }
}

void clsLteBaseIf::KA_MsgProcess(uint8_t *msg, uint32_t len)
{
    char *pos = NULL;
    LTE_AT_INDEX atCmdIndex = m_keepAliveStatusArray[m_nowStep].cmd;

    m_keepAliveStatusArray[m_nowStep].isRecved = true;

    switch(atCmdIndex)
    {
        case LTE_AT_INDEX_UNKNOW:
        {
            pos = my_strstr((char *)msg, len, LTE_AT_MODULE_READY_RSP);
            if(pos)
            {
                m_keepAliveStatusArray[m_nowStep].isSucceed = true;
            }
        }
        break;

        case LTE_AT_INDEX_AT:
        {
            pos = my_strstr((char *)msg, len, LTE_AT_OK_RSP);
            if(pos)
            {
                m_keepAliveStatusArray[m_nowStep].isSucceed = true;
            }
        }
        break;

        case LTE_AT_INDEX_ECHO:
        {
            pos = my_strstr((char *)msg, len, LTE_AT_OK_RSP);
            if(pos)
            {
                m_keepAliveStatusArray[m_nowStep].isSucceed = true;
            }
        }
        break;

        case LTE_AT_INDEX_SIM_PIN:
        {
            pos = my_strstr((char *)msg, len, LTE_AT_SIM_READY_RSP);
            if(pos)
            {
                m_keepAliveStatusArray[m_nowStep].isSucceed = true;
            }
        }
        break;

        default:
        {
            HW_Printf("unknow msg\r\n");
        }
        break;
    }
}

/******************************** ����״̬ά�ֺ��� ******************************************/

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

    Msg_RawDataProcess(); // ��Ϣ����ֹͣһ��ʱ�����Ϊ��Ϣ���ս�������ʼ���в������
}
