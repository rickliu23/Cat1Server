#ifndef _LTE_MODULE_H_
#define _LTE_MODULE_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "stdbool.h"
#include "stdint.h"



class clsLteModuleIf
{
public:
    clsLteModuleIf() { };
    ~clsLteModuleIf() { };
    
private:
    clsLteModuleIf(const clsLteModuleIf&){  }; // ���ÿ������캯��
    clsLteModuleIf& operator=(const clsLteModuleIf&){ return *this; }; // ���ø�ֵ����

public:
    void PowerOff(void);
    void PowerOn(void);

    
};

#ifdef __cplusplus
}
#endif

#endif

