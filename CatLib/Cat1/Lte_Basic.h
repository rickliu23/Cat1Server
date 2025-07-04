#ifndef _LTE_BASIC_H_
#define _LTE_BASIC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Lte_def.h"
    
#include "stdbool.h"
#include "stdint.h"

char *my_strstr(const char *s1, int len, const char *s2);
char *my_strrstr(const char *s1, int len, const char *s2);
    

/************************************ 模块基础状态维护区 ************************************/
#pragma pack(4)
typedef struct
{
    bool flag;

    int32_t timeout_ms;
    int32_t retryTimes;

    bool isExecPeriodically;
    int32_t period_ms;
    
    int32_t timeoutReloadValue;
    int32_t retryTimesReloadValue;
    int32_t periodReloadValue;

}LteModuleStatusStruct;
#pragma pack()

typedef enum
{
    LTE_RSSI_LEVEL_0,
    LTE_RSSI_LEVEL_1,
    LTE_RSSI_LEVEL_2,
    LTE_RSSI_LEVEL_3,
    LTE_RSSI_LEVEL_NUM,
    LTE_RSSI_LEVEL_INVALID
} LTE_RSSI_LEVEL;

#pragma pack(4)
typedef struct
{
    LteModuleStatusStruct isModuleReachable; // 和模块是否成功通信

    LteModuleStatusStruct isEchoOn; // 回显是否打开

    LteModuleStatusStruct isSimExist; // sim卡是否存在

    LteModuleStatusStruct isReg; // 注册状态

    
    LTE_RSSI_LEVEL rssi;

} LteModuleStatus;
#pragma pack()

#ifdef __cplusplus
}
#endif

#endif

