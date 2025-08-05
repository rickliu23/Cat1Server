#ifndef _LTE_INTERFACE_H_
#define _LTE_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "srvLte_Type.h"
#include "srvLte_Def.h"
#include "srvLte_Basic.h"

#include "stdbool.h"
#include "stdint.h"



/************************************ �����ӿ� ************************************/
typedef enum
{
    Enum_LteNetInfo,
    Enum_LteOtherInfo,
} Enum_LteMsgType;

class clsLteInterfaceIf
{
public:
    /** @brief Get the singalton instance */
    static clsLteInterfaceIf *GetInstance(void) 
    {
        static clsLteInterfaceIf instance;
        return &instance;
    }

    /** @brief Lte service interface 10ms/100ms/1000ms tick methods */

    void OnTimerFast(void);
    void OnTimer(void);
    void OnTimerSlow(void);

    // Ҫ���͵����ݣ�һ��ͨ���ú���������
    bool MsgPush(Enum_LteMsgType type, uint8_t *msg, uint32_t lenIn, int32_t timeout_ms);

    bool MsgPop(uint8_t *msg, uint32_t lenIn, uint32_t &lenOut);

private:
     // disable certain functions
    clsLteInterfaceIf() { /* add init function here */}
    ~clsLteInterfaceIf() { }
    clsLteInterfaceIf(const clsLteInterfaceIf &) { };
    clsLteInterfaceIf &operator=(const clsLteInterfaceIf &){ return *this; };

    void Clear(void);

    // ���Ƿ���������Ҫ����
    void SendDataProcess(uint32_t time_ms);

    // ��Ӳ�����߽���������
    void RawDataRecv(uint8_t *msg, uint32_t lenIn);
    friend void friend_RawDataRecv(uint8_t *msg, uint32_t lenIn);

    // ���ݽ��ս��պ��ڴ˴����
    void RawDataProcess(void);

private:
    // ֻ����time out֮�󣬲Ż�ȥ������һ������
    int32_t m_timeout;

    // buffer, ��Ž��յ������ã��˴������û���������
    LteRawFifoStructure m_rawData;

    // ��Ž��յ�����˴�����ǵ��������ݣ��ǲ��֮���
    LteMsgFifoStructure m_recvFifo;

    // ��ŷ��͵���������ǻ������ݣ������������ӵģ���������ȼ�
    LteMsgFifoStructure m_netSendFifo;

    // ��ŷ��͵���������������ͣ�����MQTT֮�࣬���ȼ���
    LteMsgFifoStructure m_otherSendFifo;

    // �����ݴ����������ӷ���FIFO�л�ȡ����
    uint8_t m_sendCmdBuf[LTE_MSG_MAX_BYTES];
    uint32_t m_sendCmdBufLen;
    uint32_t timeout_ms;

    // �����ݴ����������ӽ���FIFO�л�ȡ����
    uint8_t m_recvCmdBuf[LTE_MSG_MAX_BYTES];
    uint32_t m_recvCmdBufLen;

    // ��������ã��Ժ󻻵�
private:

    int32_t timerCount;
    void Timer_Reset(void);
    void Timer_TimeCount(uint32_t time_ms);
};

#ifdef __cplusplus
}
#endif

#endif

