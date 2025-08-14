#ifndef _SRVLTE_CAT1_CMD_H_
#define _SRVLTE_CAT1_CMD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdbool.h"
#include "srvLte_Def.h"

#ifdef DEFINE_LTE_AT_COMMANDS
    #define DECLARE_AT_COMMAND(name, value) const char name[] = value
#else
    #define DECLARE_AT_COMMAND(name, value) extern const char name[]
#endif

typedef enum _LTE_AT_TYPE
{
    LTE_AT_READ,
    LTE_AT_WRITE,
    LTE_AT_QUERY,
    LTE_AT_NUM
} LTE_AT_TYPE;

/* CMD */
DECLARE_AT_COMMAND(LTE_AT_AT, "AT");
DECLARE_AT_COMMAND(LTE_AT_ECHO, "E");
DECLARE_AT_COMMAND(LTE_AT_SIM_PIN, "CPIN");
DECLARE_AT_COMMAND(LTE_AT_CSQ, "CSQ");
DECLARE_AT_COMMAND(LTE_AT_NET_REG, "CEREG");


/************************* RSP *************************/
// 模块复位结束之后主动发送该指令
DECLARE_AT_COMMAND(LTE_AT_MODULE_READY_RSP, "AT command ready");

DECLARE_AT_COMMAND(LTE_AT_OK_RSP, "OK");
DECLARE_AT_COMMAND(LTE_AT_ERR_RSP, "ERROR");
DECLARE_AT_COMMAND(LTE_AT_READY_RSP, "READY");


typedef enum _LTE_AT_INDEX
{
		LTE_AT_INDEX_UNKNOW,
    LTE_AT_INDEX_AT, //test AT communication
    LTE_AT_INDEX_ECHO, // echo on/off
    LTE_AT_INDEX_ATI, // get module info
    LTE_AT_INDEX_CSQ, // get signal quality
    LTE_AT_INDEX_FUNC, // flight mode on/off
    LTE_AT_INDEX_SIM_PIN, // check SIM card status
    LTE_AT_INDEX_NET_REG, // network registration status

    
    LTE_AT_INDEX_CALL,
    LTE_AT_INDEX_UNCALL,



    LTE_AT_INDEX_NUM
} LTE_AT_INDEX;



bool Lte_AT_Assemble_Basic(LTE_AT_INDEX id, LTE_AT_TYPE type, int param, char cmdBuf[], int bufMaxLen, int *usefulLen);
//// 公共结构体定义
//typedef struct  {
//    _LTE_AT_INDEX at_index;
//    double timeout;
//    const char* server_name;
//} GlobalConfig;

//// 根据宏定义判断声明或定义
//#ifdef DEFINE_GLOBALS
//    #define EXTERN // 空定义
//    #define INIT(...) = __VA_ARGS__
//#else
//    #define EXTERN extern
//    #define INIT(...) // 空初始化
//#endif

//// 全局配置实例（宏控制定义或声明）
//EXTERN GlobalConfig g_config INIT({
//    .at_index = LTE_AT_INDEX_AT, 
//    .timeout = 5.0,
//    .server_name = "default"
//});

//#undef EXTERN
//#undef INIT

#ifdef __cplusplus
}
#endif

#endif
