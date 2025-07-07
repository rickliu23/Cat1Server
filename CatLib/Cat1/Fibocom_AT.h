#ifndef _FIBOCOM_AT_H_
#define _FIBOCOM_AT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdbool.h"






/************************* RSP *************************/

// 模块复位结束之后主动发送该指令
extern char LTE_AT_MODULE_READY_RSP[];

/************************* Basic CMD *************************/
extern char LTE_AT_AT[];
extern char LTE_AT_ECHO[];
extern char LTE_AT_SIM_PIN[];
extern char LTE_AT_CSQ[] ;
extern char LTE_AT_NET_REG[];

/************************* RSP *************************/


extern char LTE_AT_OK_RSP[];
extern char LTE_AT_ERR_RSP[];
extern char LTE_AT_SIM_READY_RSP[];
extern char LTE_AT_CSQ_RSP[];


typedef enum _LTE_AT_TYPE
{
    LTE_AT_READ,
    LTE_AT_WRITE,
    LTE_AT_QUERY,
    LTE_AT_NUM
} LTE_AT_TYPE;


typedef enum _LTE_AT_INDEX
{
    LTE_AT_INDEX_UNKNOW = 0,

    LTE_AT_INDEX_AT = 1, // AT测试通信
    LTE_AT_INDEX_ECHO, // 打开回显
    LTE_AT_INDEX_SIM_PIN, // 查询sim卡是否存在
    LTE_AT_INDEX_CSQ, // 查询信号强度
    LTE_AT_INDEX_NET_REG, // 网络注册

    // LTE_AT_INDEX_ATI,
    // LTE_AT_INDEX_FILE,
    // LTE_AT_INDEX_FILE_DEL,

    // LTE_AT_INDEX_FUNC,

    // LTE_AT_INDEX_NET_REG,
    // LTE_AT_INDEX_LOCATOR,
    // // LTE_AT_INDEX_ACTIVE,
    // LTE_AT_INDEX_CALL,
    // LTE_AT_INDEX_UNCALL,
    // LTE_AT_INDEX_MQTT_RECV_MODE,
    // LTE_AT_INDEX_MQTT_SSL_MODE,
    // LTE_AT_INDEX_MQTT_SAVE_CA,
    // LTE_AT_INDEX_MQTT_SAVE_CC,
    // LTE_AT_INDEX_MQTT_SAVE_CK,
    // LTE_AT_INDEX_MQTT_CFG_CA,
    // LTE_AT_INDEX_MQTT_CFG_CC,
    // LTE_AT_INDEX_MQTT_CFG_CK,
    // LTE_AT_INDEX_MQTT_CFG_SSL_LEVEL,
    // LTE_AT_INDEX_MQTT_CFG_SSL_VERSION,
    // LTE_AT_INDEX_MQTT_CFG_SSL_CIPHER,
    // LTE_AT_INDEX_MQTT_CFG_SSL_SNI,
    // LTE_AT_INDEX_MQTT_CFG_SSL_IGNORE_TIME,
    // LTE_AT_INDEX_MQTT_USER,
    // LTE_AT_INDEX_MQTT_WILL,
    // LTE_AT_INDEX_MQTT_OPEN,
    // LTE_AT_INDEX_MQTT_CONN,
    // LTE_AT_INDEX_MQTT_DISC,
    // LTE_AT_INDEX_MQTT_CLOSE,
    // LTE_AT_INDEX_MQTT_SUB,
    // LTE_AT_INDEX_MQTT_PUB,
    // LTE_AT_INDEX_MQTT_RECV,
    // LTE_AT_INDEX_MQTT_BREAK,
    // LTE_AT_INDEX_TCP_FMT,
    // LTE_AT_INDEX_TCP_OPEN,
    // LTE_AT_INDEX_TCP_CLOSE,
    // LTE_AT_INDEX_TCP_SEND,
    // LTE_AT_INDEX_TCP_RECV,
    // LTE_AT_INDEX_SLEEP_SWITCH,
    // LTE_AT_INDEX_SLEEP_MODE,
    // LTE_AT_INDEX_NTP,
    // LTE_AT_INDEX_TIME,
    // LTE_AT_INDEX_DNS,
    LTE_AT_INDEX_NUM
} LTE_AT_INDEX;


bool Fibocom_AT_Assemble_Basic(LTE_AT_INDEX id, LTE_AT_TYPE type, int param, char cmdBuf[], int bufMaxLen, int *usefulLen);

#ifdef __cplusplus
}
#endif

#endif
