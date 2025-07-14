#include "srvLte_Basic.h"
#include <string.h>

/******************************** �Զ�����ַ����������� ******************************************/

// �ҳ�s1�е�һ�γ���s2��λ��
/*
s1: ��Ѱ��Χ
len: s1�ĳ���
s2: �������Ӵ�
*/
char *my_strstr(const char *s1, int len, const char *s2)
{
    if(*s2 == '\0')
    {
        return (char *)s1;
    }
    const char *startPos = s1;

    for(; *s1 != '\0'; s1++)
    {
        const char *p1 = s1, *p2 = s2;
        while(*p1 != '\0' && *p2 != '\0' && *p1 == *p2)
        {
            p1++;
            p2++;
        }
        if(*p2 == '\0')
        {
            if((int)s1 - (int)startPos + strlen(s2) > len)
            {
                return NULL;
            }
            return (char *)s1;
        }
    }

    return NULL;
}

// �ҳ�s1�����һ�γ���s2��λ��
/*
s1: ��Ѱ��Χ
len: s1�ĳ���
s2: �������Ӵ�
*/
char *my_strrstr(const char *s1, int len, const char *s2)
{
    if(s1 == NULL || s2 == NULL || *s2 == '\0')
    {
        return NULL;
    }
    int lenLeft = len;

    char *last_occurrence = NULL;
    char *current = my_strstr(s1, lenLeft, s2);

    while(current != NULL)
    {
        last_occurrence = current;

        lenLeft = len - ((int)current - (int)s1);

        // �ƶ�ָ�뵽��һ���ַ�������������һ��ƥ��λ��
        current = my_strstr(current + 1, lenLeft - 1, s2);
    }

    return last_occurrence;
}

bool find_nth_num(const char *s1, int lenIn, int nth, int &numOut)
{
    if(nth <= 0)
    {
        return false;
    }

    int order = 0; // ������ǵ�ǰ�ĵڼ�����ֵ

    bool isContinuous = false; // �Ƚϵ�ǰ�����������Ƿ�����
    int num = 0;

    for(int i = 0; i < lenIn; i++)
    {
        if(s1[i] >= '0' && s1[i] <= '9')
        {
            if(isContinuous == false) // ���ǵ�ǰ��ֵ�ĵ�һ������
            {
                order++;

                isContinuous = true;

                num = s1[i] - '0';
            }
            else
            {
                num = num * 10 + (s1[i] - '0');
            }
        }
        else
        {
            isContinuous = false;
        }

        if(isContinuous == false && order == nth)
        {
            numOut = num;

            return true;
        }
    }

    if(isContinuous && order == nth) // ��ǰ�ַ��������һ���ַ���Ȼ�����֣�������ѭ����û�н���
    {
        numOut = num;

        return true;
    }

    return false;
}
