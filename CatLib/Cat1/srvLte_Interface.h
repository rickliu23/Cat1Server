#ifndef _LTE_INTERFACE_H_
#define _LTE_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "srvLte_Type.h"
#include "srvLte_Def.h"
#include "srvLte_Basic.h"

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
    /** @brief Get the singalton instance */
    static clsLteInterfaceIf *GetInstance(void) 
    {
        static clsLteInterfaceIf instance;
        return &instance;
    }

    /** @brief Lte service interface 10ms/100ms/1000ms tick methods */

    void OnTimerFast(void);
    void OnTimer(void);
    void OnTimerSlow(void);

    // 要发送的数据，一律通过该函数塞进来
    bool MsgPush(Enum_LteMsgType type, uint8_t *msg, uint32_t lenIn, int32_t timeout_ms);

    bool MsgPop(uint8_t *msg, uint32_t lenIn, uint32_t &lenOut);

private:
     // disable certain functions
    clsLteInterfaceIf() { /* add init function here */}
    ~clsLteInterfaceIf() { }
    clsLteInterfaceIf(const clsLteInterfaceIf &) { };
    clsLteInterfaceIf &operator=(const clsLteInterfaceIf &){ return *this; };

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

