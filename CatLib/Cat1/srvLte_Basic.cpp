#include "srvLte_Basic.h"
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

bool find_nth_num(const char *s1, int lenIn, int nth, int &numOut)
{
    if(nth <= 0)
    {
        return false;
    }

    int order = 0; // 标记这是当前的第几个数值

    bool isContinuous = false; // 比较当前碰到的数字是否连续
    int num = 0;

    for(int i = 0; i < lenIn; i++)
    {
        if(s1[i] >= '0' && s1[i] <= '9')
        {
            if(isContinuous == false) // 这是当前数值的第一个数字
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

    if(isContinuous && order == nth) // 当前字符串的最后一个字符仍然是数字，所以在循环内没有结束
    {
        numOut = num;

        return true;
    }

    return false;
}


/************************************ 接收数据 raw data 存储区 ************************************/
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

    // 目前不做循环队列，默认每次都会在满之前把数据处理掉
    m_tail = m_tail == LTE_INVALID_INDEX ? m_head : m_tail; // 尾指针更新

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

    /********** 拆出起点 **********/
    pos = my_strstr((char *)(m_array + m_tail), m_head - m_tail, cmdStart);
    if(pos != NULL) // 存在起点
    {
        // 起点下标
        startIndex = (uint32_t)pos - (uint32_t)m_array;

//        char info[20];
//        sprintf(info, "start: %d\r\n", startIndex);
//        HW_Printf(info);

        // 剩余的有效字节数量
        int16_t lenLeft = (uint32_t)m_head - (uint32_t)(m_array) + startIndex;

        /********** 尝试拆出终点 **********/

        // 先尝试，看是否存在第二包数据
        pos = my_strstr((char *)(m_array + startIndex + 1), lenLeft - 1, cmdStart);
        if(pos != NULL) // 存在第二包数据
        {
            endInedx = (uint32_t)pos - (uint32_t)m_array;

            // 第二包数据之前的内容，均认为是一包数据，copy数据
            lenOut = endInedx - startIndex;
            memcpy(msg, m_array + startIndex, lenOut);

            m_tail = endInedx;
        }
        else // 不存在第二包数据时，找结尾符号
        {
            isNeedClear = true;

            pos = my_strrstr((char *)(m_array + startIndex + 1), lenLeft - 1, cmdEnd);
            if(pos != NULL) // 存在结尾符号
            {
                endInedx = (uint32_t)pos - (uint32_t)m_array + strlen(cmdEnd); // 有效字符需要加上结尾符号

                // copy整包数据
                lenOut = endInedx - startIndex;
                memcpy(msg, m_array + startIndex, lenOut);

                m_tail = endInedx;
            }
        }

//        sprintf(info, "end: %d\r\n", endInedx);
//        HW_Printf(info);
    }
    else // 无起点
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

/************************************ 消息队列 ************************************/

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


/************************************ 消息队列 END ************************************/

