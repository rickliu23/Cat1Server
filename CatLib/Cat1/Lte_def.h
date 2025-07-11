#ifndef _LTE_DEF_H_
#define _LTE_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

// ������Ϣ����󳤶�
#define LTE_MSG_MAX_BYTES              (30)

/* ԭʼ����buffer��С�����ڴ��LTE�³���ԭʼ���� */
#define LTE_RAW_FIFO_MAX_BYTES             (100)


/* ��Ϣ���д�С��count����ָ����Ϣ��������  */
#define LTE_MSG_FIFO_MAX_COUNT              (5)



/* ���ڽ��ճ�ʱʱ�䣬��ʱ��ʼ���������Ϣ  */
#define LTE_MSG_INTERFACE_RECV_TIMEOUT_MS   (50)







/* ��ü��һ��sim���Ƿ���� */
#define LTE_SIM_CHECK_TIME_S                (30)

/* ��ü��һ���ź�  */
#define LTE_CSQ_CHECK_TIME_S                (30)

/* ��ü��һ��ע��״̬  */
#define LTE_REG_CHECK_TIME_S                (30)







#define LTE_TCP_ENABLE                      (0)

#define LTE_MQTT_ENABLE                     (0)



#ifdef __cplusplus
}
#endif

#endif

