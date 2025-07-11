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

    uint8_t m_msg[LTE_MSG_FIFO_MAX_COUNT][LTE_MSG_MAX_BYTES];
    uint32_t m_msgLen[LTE_MSG_FIFO_MAX_COUNT];

    /* 针对发送消息使用，不同的消息，预期回复时间有差异 */
    int32_t m_timeout[LTE_MSG_FIFO_MAX_COUNT];

    int32_t m_head;
    int32_t m_tail;

    void MsgInit(void);

    // 针对接收消息队列
    bool MsgPush(uint8_t *msg, uint32_t lenIn);
    bool MsgPop(uint8_t *msg, uint32_t lenIn, uint32_t &lenOut);

    // 针对发送消息队列
    bool MsgPush(uint8_t *msg, uint32_t lenIn, int32_t timeout_ms);
    bool MsgPop(uint8_t *msg, uint32_t lenIn, uint32_t &lenOut, int32_t &timeout_ms);

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

    // 要发送的数据，一律通过该函数塞进来
    bool MsgPush(Enum_LteMsgType type, uint8_t *msg, uint32_t lenIn, int32_t timeout_ms);

    bool MsgPop(uint8_t *msg, uint32_t lenIn, uint32_t &lenOut);

private:
    void Clear(void);



    // 看是否有数据需要发送
    void SendDataProcess(uint32_t time_ms);

    // 从硬件总线接收数据用
    void RawDataRecv(uint8_t *msg, uint32_t lenIn);
    friend void friend_RawDataRecv(uint8_t *msg, uint32_t lenIn);

    // 数据接收接收后，在此处拆包
    void RawDataProcess(void);

private:
    // 只有在time out之后，才会去发送下一包数据
    int32_t m_timeout;

    // buffer, 存放接收的数据用，此处存的是没拆包的数据
    LteRawFifoStructure m_rawData;

    // 存放接收的命令，此处存的是单条的数据，是拆包之后的
    LteMsgFifoStructure m_recvFifo;

    // 存放发送的命令，这里是基础数据，用来保障连接的，有最高优先级
    LteMsgFifoStructure m_netSendFifo;

    // 存放发送的命令，其它数据类型，比如MQTT之类，优先级低
    LteMsgFifoStructure m_otherSendFifo;

    // 数据暂存区，用来从发送FIFO中获取命令
    uint8_t m_sendCmdBuf[LTE_MSG_MAX_BYTES];
    uint32_t m_sendCmdBufLen;
    uint32_t timeout_ms;

    // 数据暂存区，用来从接收FIFO中获取命令
    uint8_t m_recvCmdBuf[LTE_MSG_MAX_BYTES];
    uint32_t m_recvCmdBufLen;

    // 这里测试用，以后换掉
private:

    int32_t timerCount;
    void Timer_Reset(void);
    void Timer_TimeCount(uint32_t time_ms);
};

#ifdef __cplusplus
}
#endif

#endif

