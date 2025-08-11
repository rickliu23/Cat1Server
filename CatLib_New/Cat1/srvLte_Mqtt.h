#ifndef _SRVLTE_MQTT_H_
#define _SRVLTE_MQTT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "srvLte_Type.h"
#include "srvLte_Def.h"
#include "srvLte_Basic.h"

#include "srvLte_Interface.h"

#include "Fibocom_AT.h"

typedef enum
{
    Enum_LteMqtt_WES = 1,
    Enum_LteMqtt_TP = 2, // third party
} Enum_LteMqttConnID;

class clsLteMqttConnIf;

/************************************ ģ��MQTTά���� ************************************/
class clsLteMqttIf
{
public:
    clsLteMqttIf()
    {
    }

    ~clsLteMqttIf()
    {
    }

    /** @brief Get the singalton instance */
    static clsLteMqttIf *GetInstance(void);

public: // for state machine
    /** @brief Lte service interface init/start/stop/100ms/1000ms tick methods */
    void Init(void);
    void Start(void);
    void Stop(void);

    void OnTimerFast(void);
    void OnTimer(void);
    void OnTimerSlow(void);

    // Regularly retrieve data from this class to determine if sending is required.
    bool MsgPop(uint8_t *msg, uint32_t lenIn, uint32_t &lenOut, int32_t &timeout_ms);

    // �յ���Ϣ����Ҫ�����������
    void MsgProcess(uint8_t *msg, uint32_t lenIn);

public: // for user

    void SetUser(Enum_LteMqttConnID connID, uint8_t *userName, uint8_t nameLen, uint8_t *password, uint8_t passwordLen);


private:
    void Clear(void);

private:

    LTE_AT_INDEX m_nowCmd;

    // ����û��������ݷ���ʱʹ��
    LteMsgFifoStructure m_msgFifo;

    // mqtt id
    uint32_t m_id;
};

#ifdef __cplusplus
}
#endif

/************************************ ģ��MQTT������ ************************************/
class clsLteMqttConnIf
{
public:
    clsLteMqttConnIf()
    {
    }

    ~clsLteMqttConnIf()
    {
    }

public: // for state machine
    /** @brief Lte service interface init/start/stop/100ms/1000ms tick methods */
    void Init(uint8_t id);

    void OnTimerFast(void);
    void OnTimer(void);
    void OnTimerSlow(void);

    // Regularly retrieve data from this class to determine if sending is required.
    bool MsgPop(uint8_t *msg, uint32_t lenIn, uint32_t &lenOut, int32_t &timeout_ms);

    // �յ���Ϣ����Ҫ�����������
    void MsgProcess(uint8_t *msg, uint32_t lenIn);

public: // for user

    void SetUser(uint8_t *userName, uint8_t *password);
    void Connect(void);
    void Disconnect(void);

    void Subscribe(uint8_t *topic);
    void Unsubscribe(uint8_t *topic);

    void Publish(uint8_t *topic, uint8_t *payload, uint32_t len);

private:
    void Clear(void);

private:
    Enum_LteMqttConnID m_connID;

    LTE_AT_INDEX m_nowCmd;

    // ����û��������ݷ���ʱʹ��
    LteMsgFifoStructure m_msgFifo;

    // mqtt id
    uint8_t m_id;
};

#endif
