#include "transfer.h"
#include "srvLte.h"

clsLteIf *clsLteObj = 0;

void Main_Init(void)
{
    clsLteObj = clsLteIf::GetInstance();
    clsLteObj->Init();
    clsLteObj->Start();


}

void Main_OnTimer(int time_ms)
{
    if(clsLteObj == 0)
    {
        return;
    }

    static int time = 0;
    time += time_ms;

    if((time % 10) == 0)
    {
        clsLteObj->OnTimerFast();
    }

    if((time % 100) == 0)
    {
        clsLteObj->OnTimer();
    }

    if((time % 1000) == 0)
    {
        time = 0;

        clsLteObj->OnTimerSlow();
    }
}
