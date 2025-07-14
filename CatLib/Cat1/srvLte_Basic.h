#ifndef _srvLte_Basic_H_
#define _srvLte_Basic_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "srvLte_Def.h"

#include "stdbool.h"
#include "stdint.h"

/*
Ѱ��s1�ַ����е�s2��һ�γ��ֵ�λ��
�����������ֵ������NULL
*/
char *my_strstr(const char *s1, int len, const char *s2);

/*
Ѱ��s1�ַ����е�s2���һ�γ��ֵ�λ��
�����������ֵ������NULL
*/
char *my_strrstr(const char *s1, int len, const char *s2);

/*
Ѱ���ַ����еĵ�n����ֵ����
    ���磬�ַ���Ϊ 99,55��Ѱ�ҵ�һ����ֵʱ������99
�����������ֵ������false
*/
bool find_nth_num(const char *s1, int lenIn, int nth, int &numOut);

/************************************ ��Ϣ���� ************************************/
#pragma pack(4)
typedef struct
{
    volatile bool m_isLock;

    uint8_t m_msg[LTE_MSG_FIFO_MAX_COUNT][LTE_MSG_MAX_BYTES];
    uint32_t m_msgLen[LTE_MSG_FIFO_MAX_COUNT];

    /* ��Է�����Ϣʹ�ã���ͬ����Ϣ��Ԥ�ڻظ�ʱ���в��� */
    int32_t m_timeout[LTE_MSG_FIFO_MAX_COUNT];

    int32_t m_head;
    int32_t m_tail;

    void MsgInit(void);

    // ��Խ�����Ϣ����
    bool MsgPush(uint8_t *msg, uint32_t lenIn);
    bool MsgPop(uint8_t *msg, uint32_t lenIn, uint32_t &lenOut);

    // ��Է�����Ϣ����
    bool MsgPush(uint8_t *msg, uint32_t lenIn, int32_t timeout_ms);
    bool MsgPop(uint8_t *msg, uint32_t lenIn, uint32_t &lenOut, int32_t &timeout_ms);

} LteMsgFifoStructure; // ���������Ϣ��buffer���ŵ��������Ϣ��һ���������ģ�������ճ��
#pragma pack()

#ifdef __cplusplus
}
#endif

#endif

