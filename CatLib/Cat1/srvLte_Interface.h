#ifndef _LTE_INTERFACE_H_
#define _LTE_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Lte_def.h"

#include "stdbool.h"
#include "stdint.h"

/* ��Ϣ����ʹ�ã���ͷָ�����βָ����Чʱ����Ϊ INVALID */
#define LTE_INVALID_INDEX   (-1)

/************************************ �������� raw data �洢�� ************************************/
#pragma pack(4)
typedef struct
{
    volatile bool m_isLock;

    uint8_t m_array[LTE_RAW_FIFO_MAX_BYTES];

    int32_t m_head;
    int32_t m_tail;
    uint32_t m_freeBytes;

    void MsgInit(void);
    bool MsgPush(uint8_t *msg, uint32_t lenIn);
    bool MsgPop(uint8_t *msg, uint32_t lenIn, uint32_t &lenOut);

} LteRawFifoStructure; // ���ԭʼ���ݵ�buffer
#pragma pack()

/************************************ ��Ϣ���� ************************************/
#pragma pack(4)
typedef struct
{
    volatile bool m_isLock;

    uint8_t m_msg[LTE_MSG_FIFO_MAX_COUNT][LTE_MSG_MAX_BYTES];
    uint32_t m_msgLen[LTE_MSG_FIFO_MAX_COUNT];

    /* ��Է�����Ϣʹ�ã���ͬ����Ϣ��Ԥ�ڻظ�ʱ���в��� */
    int32_t m_timeout[LTE_MSG_FIFO_MAX_COUNT];

    int32_t m_head;
    int32_t m_tail;

    void MsgInit(void);

    // ��Խ�����Ϣ����
    bool MsgPush(uint8_t *msg, uint32_t lenIn);
    bool MsgPop(uint8_t *msg, uint32_t lenIn, uint32_t &lenOut);

    // ��Է�����Ϣ����
    bool MsgPush(uint8_t *msg, uint32_t lenIn, int32_t timeout_ms);
    bool MsgPop(uint8_t *msg, uint32_t lenIn, uint32_t &lenOut, int32_t &timeout_ms);

} LteMsgFifoStructure; // ���������Ϣ��buffer���ŵ��������Ϣ��һ���������ģ�������ճ��
#pragma pack()


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
    bool MsgPush(Enum_LteMsgType type, uint8_t *msg, uint32_t lenIn, int32_t timeout_ms);

    bool MsgPop(uint8_t *msg, uint32_t lenIn, uint32_t &lenOut);

private:
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

