#ifndef _LTE_BASIC_H_
#define _LTE_BASIC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Lte_def.h"

#include "stdbool.h"
#include "stdint.h"

/*
寻找s1字符串中的s2第一次出现的位置
如果不存在数值，返回NULL
*/
char *my_strstr(const char *s1, int len, const char *s2);

/*
寻找s1字符串中的s2最后一次出现的位置
如果不存在数值，返回NULL
*/
char *my_strrstr(const char *s1, int len, const char *s2);

/*
寻找字符串中的第n个数值（）
    例如，字符串为 99,55，寻找第一个数值时，返回99
如果不存在数值，返回false
*/
bool find_nth_num(const char *s1, int lenIn, int nth, int &numOut);


#ifdef __cplusplus
}
#endif

#endif

