#include "transfer.h"
#include "srvLte_Base.h"

clsLteBaseIf *clsLteBaseObj = 0;

void Main_Init(void)
{
    clsLteBaseObj = clsLteBaseIf::GetInstance();
    clsLteBaseObj->Init();
    clsLteBaseObj->Start();


}

void Main_OnTimer(int time_ms)
{
    if(clsLteBaseObj == 0)
    {
        return;
    }

    static int time = 0;
    time += time_ms;

    if((time % 10) == 0)
    {
        clsLteBaseObj->OnTimerFast();
    }

    if((time % 100) == 0)
    {
        clsLteBaseObj->OnTimer();
    }

    if((time % 1000) == 0)
    {
        time = 0;

        clsLteBaseObj->OnTimerSlow();
    }
}
