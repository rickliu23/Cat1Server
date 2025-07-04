#ifndef _LTE_DEF_H_
#define _LTE_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif


/* ԭʼ����buffer��С�����ڴ��LTE�³���ԭʼ���� */
#define LTE_RAW_FIFO_MAX_BYTES             (200)


/* ��Ϣ���д�С��count����ָ����Ϣ�������ޣ�max_bytee����ָ��ÿ����Ϣ�Ĵ�С����  */
#define LTE_MSG_FIFO_MAX_COUNT              (5)
#define LTE_MSG_FIFO_MAX_BYTES              (100)


/* ���������Ͻ��ճ�ʱʱ�䣬��ʱ��ʼ���������Ϣ  */
#define LTE_MSG_INTERFACE_RECV_TIMEOUT      (5 * 10) // unit ms

    
#define LTE_TCP_ENABLE                      (0)

#define LTE_MQTT_ENABLE                     (0)



#ifdef __cplusplus
}
#endif

#endif

