#ifndef _SRVLTE_KEEPER_H_
#define _SRVLTE_KEEPER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "srvLte_Type.h"
#include "srvLte_Def.h"
    
#include "srvLte_Interface.h"

#include "srvLte_Cat1Cmd.h"



/************************************ 模块基础状态维护区 ************************************/
#pragma pack(4)
typedef struct
{
    bool flag;

    int32_t retryTimesLeft;

    bool isExecPeriodically;
    int32_t periodLeft_s;

    int32_t timeoutReloadValue;
    int32_t retryTimesReloadValue;
    int32_t periodReloadValue_s;

} LteModuleStatusStruct;
#pragma pack()



#pragma pack(4)
typedef struct
{
    LteModuleStatusStruct isPowerOn;

    LteModuleStatusStruct isModuleReachable; // 和模块是否成功通信

    LteModuleStatusStruct isEchoOn; // 回显是否打开

    LteModuleStatusStruct isSimExist; // sim卡是否存在

    LteModuleStatusStruct isCSQ; // 信号状态

    LteModuleStatusStruct isReg; // 注册状态

    LteModuleStatusStruct end; // 仅用作结尾标志

    LTE_RSSI rssi; // 当前信号状况

} LteModuleStatus;
#pragma pack()


/************************************ 模块基础状态维护类 ************************************/
class clsLteKeeperIf
{
public:
    clsLteKeeperIf()
    {
    }

    ~clsLteKeeperIf()
    {
    }

    /** @brief Get the singalton instance */
    static clsLteKeeperIf *GetInstance(void);

public:
    /** @brief Lte service interface init/start/stop/100ms/1000ms tick methods */
    void Init(void);
    void Start(void);
    void Stop(void);

    void OnTimerFast(void);
    void OnTimer(void);
    void OnTimerSlow(void);

    // Regularly retrieve data from this class to determine if sending is required.
    bool MsgPop(LTE_AT_INDEX &cmdType, uint8_t *msg, uint32_t lenIn, uint32_t &lenOut, int32_t &timeout_ms);

    // 收到消息，需要这个类来处理
    void MsgProcess(LTE_AT_INDEX cmdType, uint8_t *msg, uint32_t lenIn);

    bool GetConnectedStatus() { return isConnected; }

private:
    void Clear(void);

    void PowerOff(void);
    void PowerOn(void);

    void ModuleStatusInit(void);

    /*  */
    void ModuleStatusResetPeriodically(uint32_t t_s);

    void WaitAndFindNextMsg(uint32_t time_ms);

    void AT_CsqProcess(uint8_t *msg, uint32_t lenIn);
    bool AT_RegProcess(uint8_t *msg, uint32_t lenIn);

private:
    bool isConnected; // whether is cat1 is connected to net

    LteMsgStatus m_msgStatus;

    LteModuleStatus m_moduleStatus; // 模块状态保存

private:


};

#ifdef __cplusplus
}
#endif

#endif
