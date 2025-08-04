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
    clsLteModuleIf(const clsLteModuleIf&){  }; // 禁用拷贝构造函数
    clsLteModuleIf& operator=(const clsLteModuleIf&){ return *this; }; // 禁用赋值操作

public:
    void PowerOff(void);
    void PowerOn(void);

    
};

#ifdef __cplusplus
}
#endif

#endif

