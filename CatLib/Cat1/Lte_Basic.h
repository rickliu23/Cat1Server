#ifndef _LTE_BASIC_H_
#define _LTE_BASIC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Lte_def.h"

#include "stdbool.h"
#include "stdint.h"

/*
Ѱ��s1�ַ����е�s2��һ�γ��ֵ�λ��
�����������ֵ������NULL
*/
char *my_strstr(const char *s1, int len, const char *s2);

/*
Ѱ��s1�ַ����е�s2���һ�γ��ֵ�λ��
�����������ֵ������NULL
*/
char *my_strrstr(const char *s1, int len, const char *s2);

/*
Ѱ���ַ����еĵ�n����ֵ����
    ���磬�ַ���Ϊ 99,55��Ѱ�ҵ�һ����ֵʱ������99
�����������ֵ������false
*/
bool find_nth_num(const char *s1, int lenIn, int nth, int &numOut);


#ifdef __cplusplus
}
#endif

#endif

