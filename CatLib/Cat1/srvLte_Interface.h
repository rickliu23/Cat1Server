#ifndef _LTE_INTERFACE_H_
#define _LTE_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "srvLte_Type.h"
#include "srvLte_Def.h"
#include "srvLte_Resource.h"

#include "stdbool.h"
#include "stdint.h"



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
    bool MsgPush(Enum_LteMsgType type, LTE_AT_INDEX cmdType, uint8_t *msg, uint32_t lenIn, int32_t timeout_ms);

    bool MsgPop(LTE_AT_INDEX &cmdType, uint8_t *msg, uint32_t lenIn, uint32_t &lenOut);

private:
    void Clear(void);

    // 看是否有数据需要发送
    void SendDataProcess(uint32_t time_ms);

    // 从硬件总线接收数据用
    void RawDataRecv(uint8_t *msg, uint32_t lenIn);
    friend void friend_RawDataRecv(uint8_t *msg, uint32_t lenIn);

    // 数据接收接收后，在此处拆包
    void RawDataProcess(void);

    LTE_AT_INDEX MsgClassify(uint8_t *msg, uint32_t lenIn);

private:
    // buffer, 存放接收的数据用，此处存的是没拆包的数据
    LteRawFifoStructure m_rawData;

    // 存放接收的命令，此处存的是单条的数据，是拆包之后的
    LteMsgRecvFifoStructure m_recvFifo;

    // 存放发送的命令，这里是基础数据，用来保障连接的，有最高优先级
    LteMsgSendFifoStructure m_netSendFifo;

    // 存放发送的命令，其它数据类型，比如MQTT之类，优先级低
    LteMsgSendFifoStructure m_otherSendFifo;

    // 数据暂存区，用来存储当前发送并且正在等待回复的命令
    struct
    {
        LTE_AT_INDEX cmdType;
        uint8_t cmdBuf[LTE_MSG_MAX_BYTES];
        uint32_t bufLen;

        bool isRpyRecved; // 如果收到回复，立即结束等待
        int32_t timeout; // 如果没收到回复，只有在time out之后，才会去发送下一包数据
    } m_operateCmd;

    // 数据暂存区，用来从接收FIFO中获取命令
    LTE_AT_INDEX cmdType;
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

