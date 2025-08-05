#ifndef _LTE_MODULE_GENERAL_H_
#define _LTE_MODULE_GENERAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "srvLte_Type.h"
#include "srvLte_Def.h"
#include "srvLte_Interface.h"

class clsLteModuleGeneralIf
{
public:
    /** @brief Get the singalton instance */
    static clsLteModuleGeneralIf *GetInstance(void)
    {
        static clsLteModuleGeneralIf instance;
        return &instance;
    }

private: // disable certain functions
    clsLteModuleGeneralIf()
    {
        m_pInterfaceObj = clsLteInterfaceIf::GetInstance();
    };
    ~clsLteModuleGeneralIf() { };
    clsLteModuleGeneralIf(const clsLteModuleGeneralIf &) { };
    clsLteModuleGeneralIf &operator=(const clsLteModuleGeneralIf &) { return *this; };

public:
    void PowerOff(void);
    void PowerOn(void);

    LTE_MODULE_MODEL getModel(void);

private:
    clsLteInterfaceIf *m_pInterfaceObj; // physical interface handler
};

#ifdef __cplusplus
}
#endif

#endif

