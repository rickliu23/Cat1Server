#ifndef _SRVLTE_H_
#define _SRVLTE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Lte_Basic.h"
#include "srvLte_Interface.h"



class clsLteIf
{
public:
    clsLteIf()
    {
    }

    ~clsLteIf()
    {
    }

    /** @brief Get the singalton instance */
    static clsLteIf *GetInstance(void);

public:
    /** @brief Lte service interface init/start/stop/100ms/1000ms tick methods */
    void Init(void);
    void Start(void);
    void Stop(void);

    void OnTimerFast(void);
    void OnTimer(void);
    void OnTimerSlow(void);

private:
    void Clear(void);

private:
    bool isEnabled; // 当前类的使能标志位，如果是false，代表用户不需要使用


    clsLteInterfaceIf *m_pInterfaceObj;


};

#ifdef __cplusplus
}
#endif

#endif
