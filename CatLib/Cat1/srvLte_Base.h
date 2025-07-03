#ifndef _SRVLTE_BASE_H_
#define _SRVLTE_BASE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdbool.h"
#include "stdint.h"
#include "Fibocom_AT.h"

class clsLteBaseIf;

typedef void (clsLteBaseIf::*MemberMemberFunPtrTemplate)(void);

#define LET_INVALID_INDEX   (-1)

#define LET_RAW_FIFO_MAX             (200)

#define LET_MSG_FIFO_MAX_COUNT       (5)
#define LET_MSG_FIFO_MAX_BYTES       (100)

#define LET_KEEPALIVE_ARRAY_COUNT       (10)

typedef struct
{
    volatile bool isLock;
    uint8_t array[LET_RAW_FIFO_MAX];
    int16_t head;
    int16_t tail;
} LteRawFifoStructure; // ���ԭʼ���ݵ�buffer

typedef struct
{
    volatile bool isLock;

    uint8_t msg[LET_MSG_FIFO_MAX_COUNT][LET_MSG_FIFO_MAX_BYTES];
    uint32_t msgLen[LET_MSG_FIFO_MAX_COUNT];
    int16_t timeout[LET_MSG_FIFO_MAX_COUNT]; // for recv, useless

    int16_t head;
    int16_t tail;

} LteMsgFifoStructure; // ���������Ϣ��buffer���ŵ��������Ϣ��һ���������ģ�������ճ��

typedef struct
{
    volatile bool isWaitForReply;
    int16_t timeout;
} LteMsgStateMachine;

#pragma pack(4)
typedef struct
{
    int32_t step;
    bool isFirst;

    LTE_AT_INDEX cmd;
    LTE_AT_TYPE type;
    int32_t para;

    int32_t tryTimes;
    int32_t reloadTryTimes;

    int32_t timeout_ms;
    int32_t reloadTimeout_ms;

    bool isRecved;
    bool isSucceed;

    int32_t timewait_ms;
    int32_t reloadtimewait_ms;

    int32_t stepAfterSucceed;
    int32_t stepAfterFailed;

} LteKeepAliveStruct;
#pragma pack()

class clsLteBaseIf
{
public:
    clsLteBaseIf()
    {
    }

    ~clsLteBaseIf()
    {
    }

    /** @brief Get the singalton instance */
    static clsLteBaseIf *GetInstance(void);

public:
    /** @brief Lte service interface init/start/stop/100ms/1000ms tick methods */
    void Init(void);
    void Start(void);
    void Stop(void);

    void OnTimerFast(void);
    void OnTimer(void);
    void OnTimerSlow(void);

private:
    bool isEnabled; // ��ǰ���ʹ�ܱ�־λ�������false�������û�����Ҫʹ��

    void Clear(void);

private: // ��ͨ�����ߵ���Ϣ����
    void Uart_Send(uint8_t *msg, uint32_t len);
    void Uart_Recv(uint8_t *msg, uint32_t len);
    friend void Friend_Uart_Recv(uint8_t *msg, uint32_t len);

private:
    LteRawFifoStructure m_rawDataFifo; // �յ�����Ϣȫ����������������������

    // ������Ϣ���У�����״̬������������е���Ϣ���ȷ���
    LteMsgFifoStructure m_sendFifoForNet; //

    // ������Ϣ���У�����MQTT��HTTP��
    LteMsgFifoStructure m_sendFifoForOther;

    // ������Ϣ����
    LteMsgFifoStructure m_recvFifo; // �����յ�����Ϣ��������֮�󣬻��ȷŵ�����

    // ����ǰ��ͨ��״̬
    LteMsgStateMachine m_msgState;

    void Msg_Clear(void);

    // ���Ͷ��й������ڴ������Ƿ���Ҫ����
    void Msg_SendQueueMngr(uint32_t t_ms);

    // ���ն��й������ڴ������Ƿ�������Ϣ
    void Msg_RecvQueueMngr(uint32_t t_ms);

    // ԭʼ���ݴ�����Ҫ�ǲ��
    // ���ڽ�����ɺ������rawdata����֮����
    void Msg_RawDataProcess();

    // ��Ҫ���͵Ļ���������һ��ͨ���ú�������ŵ�������
    bool Msg_PushForNet(uint8_t *msg, uint32_t len, uint32_t timeout_ms);

    // �����ɺ�һ��ͨ���ú����������Ϣ
    bool Msg_PushToRecvQueue(uint8_t *msg, uint32_t len);


private:
    uint32_t m_nowStep;
    LteKeepAliveStruct m_keepAliveStatusArray[LET_KEEPALIVE_ARRAY_COUNT];

    void KA_StatusArrayInit(void);

    void KA_StateMachine(uint32_t t_ms);

    void KA_MsgProcess(uint8_t *msg, uint32_t len);

    //void PushMsgToMQTT(uint8_t *msg, uint32_t len);

    // ����״̬��������ά��4Gģ�����������

    // void LteBaseLoopStructureInit(void);



    // void BaseStateMachine(int time_ms);


    // void StateMachine_Reset(void);
    // void StateMachine_Idle(void);
    // void StateMachine_Echo(void);


    // ��������ã��Ժ󻻵�
private:
#define LTE_TIMER_TIMEOUT   (100) // ����������û��Ϣ����ʼ����Ŀǰ���յ�����Ϣ
    int32_t timerCount;
    void Timer_Reset(void);
    void Timer_TimeCount(uint32_t time_ms);

private:


    // LteBaseLoopStructure m_stateMachineLoop[LTE_ENUM_SM_MAX];

};

#ifdef __cplusplus
}
#endif

#endif
