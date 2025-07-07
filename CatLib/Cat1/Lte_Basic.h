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


typedef enum
{
    LTE_RSSI_LEVEL_0,
    LTE_RSSI_LEVEL_1,
    LTE_RSSI_LEVEL_2,
    LTE_RSSI_LEVEL_3,
    LTE_RSSI_LEVEL_NUM,
    LTE_RSSI_LEVEL_INVALID
} LTE_RSSI_LEVEL;

#ifdef __cplusplus
}
#endif

#endif

