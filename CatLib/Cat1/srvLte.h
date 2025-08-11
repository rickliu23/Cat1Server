#ifndef _SRVLTE_H_
#define _SRVLTE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "srvLte_Basic.h"

#include "srvLte_Interface.h"
#include "srvLte_Keeper.h"

typedef enum
{
    enum_lteKeeper = 0,
    enum_lteUdp,
    enum_lteTcp,
    enum_lteMqtt,
} LTE_ENUM_MSG_TYPE;

class clsLteIf
{
public:
    clsLteIf()
    {
    }

    ~clsLteIf()
    {
    }

    /** @brief Get the singalton instance */
    static clsLteIf *GetInstance(void);

public:
    /** @brief Lte service interface init/start/stop/100ms/1000ms tick methods */
    void Init(void);
    void Start(void);
    void Stop(void);

    void OnTimerFast(void);
    void OnTimer(void);
    void OnTimerSlow(void);

private:
    void Clear(void);

    void GetMsgAndProcess(void);

    LTE_ENUM_MSG_TYPE MsgClassify(uint8_t *msg, uint32_t lenIn);

private:
    bool isEnabled; // 当前类的使能标志位，如果是false，代表用户不需要使用

    clsLteInterfaceIf *m_pInterfaceObj;
    clsLteKeeperIf *m_pKeeperObj;

    // 数据暂存区
    uint8_t m_CmdBuf[LTE_MSG_MAX_BYTES];
    uint32_t m_CmdBufLen;
    int32_t m_timeout;
};

#ifdef __cplusplus
}
#endif

#endif
