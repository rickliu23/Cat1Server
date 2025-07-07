#ifndef _LTE_DEF_H_
#define _LTE_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

// 单条信息的最大长度
#define LTE_MSG_MAX_BYTES              (30)
    
/* 原始数据buffer大小，用于存放LTE吐出的原始数据 */
#define LTE_RAW_FIFO_MAX_BYTES             (100)


/* 消息队列大小，count用于指定消息条数上限  */
#define LTE_MSG_FIFO_MAX_COUNT              (5)



/* 物理总线上接收超时时间，超时后开始处理接收消息  */
#define LTE_MSG_INTERFACE_RECV_TIMEOUT      (5 * 10) // unit ms

    
#define LTE_TCP_ENABLE                      (0)

#define LTE_MQTT_ENABLE                     (0)



#ifdef __cplusplus
}
#endif

#endif

