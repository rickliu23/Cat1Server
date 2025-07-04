#ifndef _LTE_BASIC_H_
#define _LTE_BASIC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdbool.h"
#include "stdbool.h"
#include "stdint.h"


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
    bool hasSim;
    int32_t rssi;

} LteAliveFlag;
#pragma pack()

#ifdef __cplusplus
}
#endif

#endif

