#ifndef _srvLte_Basic_H_
#define _srvLte_Basic_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "srvLte_Cat1Cmd.h"
    
#include "srvLte_Def.h"

#include "stdbool.h"
#include "stdint.h"

/*
找出s1中第一次出现s2的位置
如果不存在数值，返回NULL
*/
char *my_strstr(const char *s1, int len, const char *s2);

/*
找出s1中最后一次出现s2的位置
如果不存在数值，返回NULL
*/
char *my_strrstr(const char *s1, int len, const char *s2);

/*
寻找字符串中的第n个数值（）
    例如，字符串为 99,55，寻找第一个数值时，返回99
如果不存在数值，返回false
*/
bool find_nth_num(const char *s1, int lenIn, int nth, int &numOut);


/************************************ 接收数据 raw data 存储区 ************************************/
#pragma pack(4)
typedef struct
{
    volatile bool m_isLock;

    uint8_t m_array[LTE_RAW_FIFO_MAX_BYTES];

    int32_t m_head;
    int32_t m_tail;
    uint32_t m_freeBytes;

    void MsgInit(void);
    bool MsgPush(uint8_t *msg, uint32_t lenIn);
    bool MsgPop(uint8_t *msg, uint32_t lenIn, uint32_t &lenOut);

} LteRawFifoStructure; // 存放原始数据的buffer
#pragma pack()


/************************************ 消息队列 ************************************/
/* 消息队列使用，当头指针或者尾指针无效时，置为 INVALID */
#define LTE_INVALID_INDEX   (-1)

/************************* 发送队列 *************************/
#pragma pack(4)
typedef struct
{
private:
    volatile bool m_isLock;

    struct
    {
        LTE_AT_INDEX cmdType;
        uint8_t msg[LTE_MSG_MAX_BYTES];
        uint32_t len;
        /* 针对发送消息使用，不同的消息，预期回复时间有差异 */
        int32_t timeout;
    } m_buf[LTE_MSG_FIFO_MAX_COUNT];

    int32_t m_head;
    int32_t m_tail;

public:
    void MsgInit(void);

    bool MsgPush(LTE_AT_INDEX cmdType, uint8_t *msg, uint32_t lenIn, int32_t timeout_ms);
    bool MsgPop(LTE_AT_INDEX &cmdType, uint8_t *msg, uint32_t lenIn, uint32_t &lenOut, int32_t &timeout_ms);

} LteMsgSendFifoStructure; // 存放完整消息：放到这里的消息，一定是完整的，不存在粘包
#pragma pack()

/************************* 接收队列 *************************/
#pragma pack(4)
typedef struct : public LteMsgSendFifoStructure
{
    bool MsgPush(LTE_AT_INDEX cmdType, uint8_t *msg, uint32_t lenIn);
    bool MsgPop(LTE_AT_INDEX &cmdType, uint8_t *msg, uint32_t lenIn, uint32_t &lenOut);
} LteMsgRecvFifoStructure;
#pragma pack()


/************************************ 当前信息发送状态保存 ************************************/
#pragma pack(4)
typedef struct
{
    bool isWaitingForPop;
    bool isWaitingForReply;

    LTE_AT_INDEX nowCmd;
    int32_t timeout;

    uint8_t msgBuf[LTE_MSG_MAX_BYTES];
    uint32_t msgLen;

} LteMsgStatus;
#pragma pack()

#ifdef __cplusplus
}
#endif

#endif

