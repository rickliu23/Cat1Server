#include "srvLte_Resource.h"
#include <string.h>

/******************************** �Զ�����ַ����������� ******************************************/

/*
�ҳ�s1�е�һ�γ���s2��λ��
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

/*
�ҳ�s1�����һ�γ���s2��λ��
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

/*
Ѱ���ַ����еĵ�n����ֵ����
    ���磬�ַ���Ϊ 99,55��Ѱ�ҵ�һ����ֵʱ������99
�����������ֵ������false
*/
bool find_nth_num(const char *s1, int lenIn, int nth, int &numOut)
{
    if(nth <= 0)
    {
        return false;
    }

    int order = 0; // ������ǵ�ǰ�ĵڼ�����ֵ

    bool isContinuous = false; // �Ƚϵ�ǰ�����������Ƿ�����
    int num = 0;

    for(int i = 0; i < lenIn; i++)
    {
        if(s1[i] >= '0' && s1[i] <= '9')
        {
            if(isContinuous == false) // ���ǵ�ǰ��ֵ�ĵ�һ������
            {
                order++;

                isContinuous = true;

                num = s1[i] - '0';
            }
            else
            {
                num = num * 10 + (s1[i] - '0');
            }
        }
        else
        {
            isContinuous = false;
        }

        if(isContinuous == false && order == nth)
        {
            numOut = num;

            return true;
        }
    }

    if(isContinuous && order == nth) // ��ǰ�ַ��������һ���ַ���Ȼ�����֣�������ѭ����û�н���
    {
        numOut = num;

        return true;
    }

    return false;
}


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

    if(msg == NULL || lenIn < LTE_MSG_MAX_BYTES)
    {
        return false;
    }

    bool isNeedClear = false;;
    char *pos = NULL;
    int32_t startIndex = 0, endInedx = 0;

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
        int16_t lenLeft = (uint32_t)m_head - (uint32_t)(m_array) + startIndex;

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

/************************* ���Ͷ��� *************************/
void LteMsgSendFifoStructure::MsgInit(void)
{
    m_isLock = false;

    memset(&m_buf, 0, sizeof(m_buf));

    m_head = 0;
    m_tail = LTE_INVALID_INDEX;
}

bool LteMsgSendFifoStructure::MsgPush(LTE_AT_CMD_TYPE cmdType, uint8_t *msg, uint32_t lenIn, int32_t timeout_ms)
{
    if(m_head == LTE_INVALID_INDEX || m_isLock == true)
    {
        return false;
    }

    if(lenIn > LTE_MSG_MAX_BYTES)
    {
        return false;
    }

    m_isLock = true;

    m_buf[m_head].cmdType = cmdType;
    memcpy(m_buf[m_head].msg, msg, lenIn);
    m_buf[m_head].len = lenIn;
    m_buf[m_head].timeout = timeout_ms;
    
    m_tail = m_tail == LTE_INVALID_INDEX ? m_head : m_tail;

    m_head += 1;
    m_head %= LTE_MSG_FIFO_MAX_COUNT;
    m_head = m_head == m_tail ? LTE_INVALID_INDEX : m_head;

    m_isLock = false;

    return true;
}

bool LteMsgSendFifoStructure::MsgPop(LTE_AT_CMD_TYPE &cmdType, uint8_t *msg, uint32_t lenIn, uint32_t &lenOut, int32_t &timeout_ms)
{
    if(m_tail == LTE_INVALID_INDEX || m_isLock == true)
    {
        return false;
    }

    if(lenIn < LTE_MSG_MAX_BYTES)
    {
        return false;
    }

    memset(msg, 0, lenIn);
    lenOut = 0;
    timeout_ms = 0;

    m_isLock = true;

    cmdType = m_buf[m_tail].cmdType;
    memcpy(msg, m_buf[m_tail].msg, m_buf[m_tail].len);
    lenOut = m_buf[m_tail].len;
    timeout_ms = m_buf[m_tail].timeout;

    m_head = m_head == LTE_INVALID_INDEX ? m_tail : m_head;

    m_tail += 1;
    m_tail %= LTE_MSG_FIFO_MAX_COUNT;
    m_tail = m_tail == m_head ? LTE_INVALID_INDEX : m_tail;

    m_isLock = false;

    return true;
}

/************************* ���ն��� *************************/
bool LteMsgRecvFifoStructure::MsgPush(LTE_AT_CMD_TYPE cmdType, uint8_t *msg, uint32_t lenIn)
{
    int32_t timeout = 0;

    return LteMsgSendFifoStructure::MsgPush(cmdType, msg, lenIn, timeout);
}

bool LteMsgRecvFifoStructure::MsgPop(LTE_AT_CMD_TYPE &cmdType, uint8_t *msg, uint32_t lenIn, uint32_t &lenOut)
{
    int32_t timeout = 0;

    return LteMsgSendFifoStructure::MsgPop(cmdType, msg, lenIn, lenOut, timeout);
}

/************************************ ��Ϣ���� END ************************************/

