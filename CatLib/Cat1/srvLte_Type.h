#ifndef _srvLte_Type_H_
#define _srvLte_Type_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdbool.h"

typedef enum
{
    LTE_MODEL_INVALID,

    LTE_MODEL_FIBOCOM_MC661,

    LTE_LTE_MODULE_MODEL_MAX
} LTE_MODULE_MODEL;
    
typedef enum
{
    LTE_RSSI_LEVEL_0,
    LTE_RSSI_LEVEL_1,
    LTE_RSSI_LEVEL_2,
    LTE_RSSI_LEVEL_3,
    LTE_RSSI_LEVEL_NUM,
    LTE_RSSI_LEVEL_INVALID
} LTE_RSSI_LEVEL;

typedef struct
{
    int8_t value;
    LTE_RSSI_LEVEL level;
} LTE_RSSI;

typedef enum
{
    LTE_STATUS_NOMODULE,
    LTE_STATUS_NOSIM,
    LTE_STATUS_REGERR, // register error
} LTE_STATUS;


#ifdef __cplusplus
}
#endif

#endif

