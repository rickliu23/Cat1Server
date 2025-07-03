#ifndef _SRVLTE_BASE_H_
#define _SRVLTE_BASE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdbool.h"
#include "stdint.h"
#include "Fibocom_AT.h"

class clsLteBaseIf;

typedef void (clsLteBaseIf::*MemberMemberFunPtrTemplate)(void);

#define LET_INVALID_INDEX   (-1)

#define LET_RAW_FIFO_MAX             (200)

#define LET_MSG_FIFO_MAX_COUNT       (5)
#define LET_MSG_FIFO_MAX_BYTES       (100)

#define LET_KEEPALIVE_ARRAY_COUNT       (10)

typedef struct
{
    volatile bool isLock;
    uint8_t array[LET_RAW_FIFO_MAX];
    int16_t head;
    int16_t tail;
} LteRawFifoStructure; // 存放原始数据的buffer

typedef struct
{
    volatile bool isLock;

    uint8_t msg[LET_MSG_FIFO_MAX_COUNT][LET_MSG_FIFO_MAX_BYTES];
    uint32_t msgLen[LET_MSG_FIFO_MAX_COUNT];
    int16_t timeout[LET_MSG_FIFO_MAX_COUNT]; // for recv, useless

    int16_t head;
    int16_t tail;

} LteMsgFifoStructure; // 存放完整消息的buffer，放到这里的消息，一定是完整的，不存在粘包

typedef struct
{
    volatile bool isWaitForReply;
    int16_t timeout;
} LteMsgStateMachine;

#pragma pack(4)
typedef struct
{
    int32_t step;
    bool isFirst;

    LTE_AT_INDEX cmd;
    LTE_AT_TYPE type;
    int32_t para;

    int32_t tryTimes;
    int32_t reloadTryTimes;

    int32_t timeout_ms;
    int32_t reloadTimeout_ms;

    bool isRecved;
    bool isSucceed;

    int32_t timewait_ms;
    int32_t reloadtimewait_ms;

    int32_t stepAfterSucceed;
    int32_t stepAfterFailed;

} LteKeepAliveStruct;
#pragma pack()

class clsLteBaseIf
{
public:
    clsLteBaseIf()
    {
    }

    ~clsLteBaseIf()
    {
    }

    /** @brief Get the singalton instance */
    static clsLteBaseIf *GetInstance(void);

public:
    /** @brief Lte service interface init/start/stop/100ms/1000ms tick methods */
    void Init(void);
    void Start(void);
    void Stop(void);

    void OnTimerFast(void);
    void OnTimer(void);
    void OnTimerSlow(void);

private:
    bool isEnabled; // 当前类的使能标志位，如果是false，代表用户不需要使用

    void Clear(void);

private: // 和通信总线的信息交互
    void Uart_Send(uint8_t *msg, uint32_t len);
    void Uart_Recv(uint8_t *msg, uint32_t len);
    friend void Friend_Uart_Recv(uint8_t *msg, uint32_t len);

private:
    LteRawFifoStructure m_rawDataFifo; // 收到的消息全部存这里，后续从这里来拆包

    // 发送消息队列，基础状态管理，这个队列中的消息优先发送
    LteMsgFifoStructure m_sendFifoForNet; //

    // 发送消息队列，例如MQTT、HTTP等
    LteMsgFifoStructure m_sendFifoForOther;

    // 接收消息队列
    LteMsgFifoStructure m_recvFifo; // 所有收到的消息，拆包完成之后，会先放到这里

    // 管理当前的通信状态
    LteMsgStateMachine m_msgState;

    void Msg_Clear(void);

    // 发送队列管理，定期处理，看是否需要发送
    void Msg_SendQueueMngr(uint32_t t_ms);

    // 接收队列管理，定期处理，看是否有新消息
    void Msg_RecvQueueMngr(uint32_t t_ms);

    // 原始数据处理，主要是拆包
    // 会在接收完成后处理或者rawdata满了之后处理
    void Msg_RawDataProcess();

    // 需要发送的基础函数，一律通过该函数，存放到队列中
    bool Msg_PushForNet(uint8_t *msg, uint32_t len, uint32_t timeout_ms);

    // 拆包完成后，一律通过该函数来存放消息
    bool Msg_PushToRecvQueue(uint8_t *msg, uint32_t len);


private:
    uint32_t m_nowStep;
    LteKeepAliveStruct m_keepAliveStatusArray[LET_KEEPALIVE_ARRAY_COUNT];

    void KA_StatusArrayInit(void);

    void KA_StateMachine(uint32_t t_ms);

    void KA_MsgProcess(uint8_t *msg, uint32_t len);

    //void PushMsgToMQTT(uint8_t *msg, uint32_t len);

    // 基础状态机，用来维持4G模块的网络连接

    // void LteBaseLoopStructureInit(void);



    // void BaseStateMachine(int time_ms);


    // void StateMachine_Reset(void);
    // void StateMachine_Idle(void);
    // void StateMachine_Echo(void);


    // 这里测试用，以后换掉
private:
#define LTE_TIMER_TIMEOUT   (100) // 物理总线上没消息，开始处理目前接收到的消息
    int32_t timerCount;
    void Timer_Reset(void);
    void Timer_TimeCount(uint32_t time_ms);

private:


    // LteBaseLoopStructure m_stateMachineLoop[LTE_ENUM_SM_MAX];

};

#ifdef __cplusplus
}
#endif

#endif
