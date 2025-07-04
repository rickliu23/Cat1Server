#ifndef _LTE_INTERFACE_H_
#define _LTE_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Lte_def.h"

#include "stdbool.h"
#include "stdint.h"

/* 消息队列使用，当头指针或者尾指针无效时，置为 INVALID */
#define LTE_INVALID_INDEX   (-1)

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
#pragma pack(4)
typedef struct
{
    volatile bool m_isLock;

    uint8_t m_msg[LTE_MSG_FIFO_MAX_COUNT][LTE_MSG_FIFO_MAX_BYTES];
    uint32_t m_msgLen[LTE_MSG_FIFO_MAX_COUNT];

    /* 针对发送消息使用，不同的消息，预期回复时间有差异 */
    uint32_t m_timeout[LTE_MSG_FIFO_MAX_COUNT];

    int32_t m_head;
    int32_t m_tail;

    void MsgInit(void);

    // 针对接收消息队列
    bool MsgPush(uint8_t *msg, uint32_t lenIn);
    bool MsgPop(uint8_t *msg, uint32_t lenIn, uint32_t &lenOut);

    // 针对发送消息队列
    bool MsgPush(uint8_t *msg, uint32_t lenIn, uint32_t timeout_ms);
    bool MsgPop(uint8_t *msg, uint32_t lenIn, uint32_t &lenOut, uint32_t &timeout_ms);

} LteMsgFifoStructure; // 存放完整消息的buffer，放到这里的消息，一定是完整的，不存在粘包
#pragma pack()


/************************************ 交互接口 ************************************/
typedef enum
{
    Enum_LteNetInfo,
    Enum_LteOtherInfo,
} Enum_LteMsgType;

class clsLteInterfaceIf
{
public:
    clsLteInterfaceIf()
    {
    }

    ~clsLteInterfaceIf()
    {
    }

    /** @brief Get the singalton instance */
    static clsLteInterfaceIf *GetInstance(void);

    /** @brief Lte service interface init/start/stop/100ms/1000ms tick methods */
    void Init(void);
    void Start(void);
    void Stop(void);

    void OnTimerFast(void);
    void OnTimer(void);
    void OnTimerSlow(void);

    bool MsgPush(Enum_LteMsgType type, uint8_t *msg, uint32_t lenIn, uint32_t timeout_ms);

private:
    void Clear(void);

    uint8_t m_sendCmdBuf[LTE_MSG_FIFO_MAX_BYTES];
    uint32_t m_sendCmdBufLen;
    uint32_t timeout_ms;
    void SendDataProcess(void);

    void RawDataRecv(uint8_t *msg, uint32_t lenIn);
    friend void friend_RawDataRecv(uint8_t *msg, uint32_t lenIn);

    uint8_t m_recvCmdBuf[LTE_MSG_FIFO_MAX_BYTES];
    uint32_t m_recvCmdBufLen;
    void RawDataProcess(void);

private:
    LteRawFifoStructure m_rawData;

    LteMsgFifoStructure m_netSendFifo;
    LteMsgFifoStructure m_otherSendFifo;

    LteMsgFifoStructure m_recvFifo;



    // 这里测试用，以后换掉
private:
#define LTE_TIMER_TIMEOUT   (100) // 物理总线上没消息，开始处理目前接收到的消息
    int32_t timerCount;
    void Timer_Reset(void);
    void Timer_TimeCount(uint32_t time_ms);
};

#ifdef __cplusplus
}
#endif

#endif

