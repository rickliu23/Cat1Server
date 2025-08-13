#ifndef _LTE_INTERFACE_H_
#define _LTE_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "srvLte_Type.h"
#include "srvLte_Def.h"
#include "srvLte_Resource.h"

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
    clsLteInterfaceIf()
    {
    }

    ~clsLteInterfaceIf()
    {
    }

    /** @brief Get the singalton instance */
    static clsLteInterfaceIf *GetInstance(void);

    /** @brief Lte service interface init/start/stop/100ms/1000ms tick methods */
    void Init(void);
    void Start(void);
    void Stop(void);

    void OnTimerFast(void);
    void OnTimer(void);
    void OnTimerSlow(void);

    // Ҫ���͵����ݣ�һ��ͨ���ú���������
    bool MsgPush(Enum_LteMsgType type, LTE_AT_INDEX cmdType, uint8_t *msg, uint32_t lenIn, int32_t timeout_ms);

    bool MsgPop(LTE_AT_INDEX &cmdType, uint8_t *msg, uint32_t lenIn, uint32_t &lenOut);

private:
    void Clear(void);

    // ���Ƿ���������Ҫ����
    void SendDataProcess(uint32_t time_ms);

    // ��Ӳ�����߽���������
    void RawDataRecv(uint8_t *msg, uint32_t lenIn);
    friend void friend_RawDataRecv(uint8_t *msg, uint32_t lenIn);

    // ���ݽ��ս��պ��ڴ˴����
    void RawDataProcess(void);

    LTE_AT_INDEX MsgClassify(uint8_t *msg, uint32_t lenIn);

private:
    // buffer, ��Ž��յ������ã��˴������û���������
    LteRawFifoStructure m_rawData;

    // ��Ž��յ�����˴�����ǵ��������ݣ��ǲ��֮���
    LteMsgRecvFifoStructure m_recvFifo;

    // ��ŷ��͵���������ǻ������ݣ������������ӵģ���������ȼ�
    LteMsgSendFifoStructure m_netSendFifo;

    // ��ŷ��͵���������������ͣ�����MQTT֮�࣬���ȼ���
    LteMsgSendFifoStructure m_otherSendFifo;

    // �����ݴ����������洢��ǰ���Ͳ������ڵȴ��ظ�������
    struct
    {
        LTE_AT_INDEX cmdType;
        uint8_t cmdBuf[LTE_MSG_MAX_BYTES];
        uint32_t bufLen;

        bool isRpyRecved; // ����յ��ظ������������ȴ�
        int32_t timeout; // ���û�յ��ظ���ֻ����time out֮�󣬲Ż�ȥ������һ������
    } m_operateCmd;

    // �����ݴ����������ӽ���FIFO�л�ȡ����
    LTE_AT_INDEX cmdType;
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

