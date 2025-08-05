#ifndef _LTE_MODULE_H_
#define _LTE_MODULE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "srvLte_Type.h"
#include "srvLte_Def.h"

class clsLteModuleIf
{
public:
    clsLteModuleIf() { };
    ~clsLteModuleIf() { };

private:
    clsLteModuleIf(const clsLteModuleIf &) { }; // 禁用拷贝构造函数
    clsLteModuleIf &operator=(const clsLteModuleIf &){return *this;}; // 禁用赋值操作

public: // NET
    virtual void KeepAlive(uint32_t time_ms) = 0;

    bool isModuleAvailable(void);
    bool isSimExist(void);
    LTE_RSSI getRssi(void);
    bool isRegisterSucceed(void);

#if LTE_TCP_ENABLE
public: // TCP

#endif

#if LTE_MQTT_ENABLE
public: // MQTT

#endif

};

#ifdef __cplusplus
}
#endif

#endif

