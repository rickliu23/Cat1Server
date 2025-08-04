#ifndef _SRVLTE_MQTT_H_
#define _SRVLTE_MQTT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "srvLte_Type.h"
#include "srvLte_Def.h"
#include "srvLte_Basic.h"

#include "srvLte_Interface.h"

#include "Fibocom_AT.h"


/************************************ 模块基础状态维护类 ************************************/
class clsLteMqttIf
{
public:
    clsLteMqttIf()
    {
    }

    ~clsLteMqttIf()
    {
    }

public: // for state machine
    /** @brief Lte service interface init/start/stop/100ms/1000ms tick methods */
    void Init(void);
    void Start(void);
    void Stop(void);

    void OnTimerFast(void);
    void OnTimer(void);
    void OnTimerSlow(void);

    // Regularly retrieve data from this class to determine if sending is required.
    bool MsgPop(uint8_t *msg, uint32_t lenIn, uint32_t &lenOut, int32_t &timeout_ms);

    // 收到消息，需要这个类来处理
    void MsgProcess(uint8_t *msg, uint32_t lenIn);

public: // for user




private:
    void Clear(void);

private:

    LTE_AT_INDEX m_nowCmd;

    // 针对用户多条数据发送时使用
    LteMsgFifoStructure m_msgFifo;

    // mqtt id
    uint32_t m_id;
};

#ifdef __cplusplus
}
#endif

#endif
