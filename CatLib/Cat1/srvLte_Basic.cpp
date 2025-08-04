#include "srvLte_Basic.h"
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

//        char info[20];
//        sprintf(info, "start: %d\r\n", startIndex);
//        HW_Printf(info);

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

//        sprintf(info, "end: %d\r\n", endInedx);
//        HW_Printf(info);
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

//    char info[20];
//    sprintf(info, "len: %d\r\n", lenOut);
//    HW_Printf(info);

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
    int32_t timeout = 0;

    return MsgPush(msg, lenIn, timeout);
}

bool LteMsgFifoStructure::MsgPop(uint8_t *msg, uint32_t lenIn, uint32_t &lenOut)
{
    int32_t timeout = 0;

    return MsgPop(msg, lenIn, lenOut, timeout);
}

bool LteMsgFifoStructure::MsgPush(uint8_t *msg, uint32_t lenIn, int32_t timeout_ms)
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

bool LteMsgFifoStructure::MsgPop(uint8_t *msg, uint32_t lenIn, uint32_t &lenOut, int32_t &timeout_ms)
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

