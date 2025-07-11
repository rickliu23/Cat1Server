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

/*
Ѱ���ַ����еĵ�n����ֵ����
    ���磬�ַ���Ϊ 99,55��Ѱ�ҵ�һ����ֵʱ������99
�����������ֵ������false
*/
bool find_nth_num(const char *s1, int lenIn, int nth, int &numOut);

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

#ifdef __cplusplus
}
#endif

#endif

