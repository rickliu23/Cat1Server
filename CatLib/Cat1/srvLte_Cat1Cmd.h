#ifndef _SRVLTE_CAT1_CMD_H_
#define _SRVLTE_CAT1_CMD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdbool.h"
#include "stdlib.h"
#include "srvLte_Def.h"

#ifdef DEFINE_LTE_AT_COMMANDS
#define DECLARE_AT_COMMAND(name, value) const char name[] = value
#else
#define DECLARE_AT_COMMAND(name, value) extern const char name[]
#endif

typedef enum
{
    LTE_AT_CMD_UNKNOW = 0,
    
    // Command-Driven Queries
    LTE_AT_CMD_AT, //test AT communication
    LTE_AT_CMD_ECHO, // echo on/off
    LTE_AT_CMD_ATI, // get module info
    LTE_AT_CMD_CSQ, // get signal quality
    LTE_AT_CMD_SIM_PIN, // check SIM card status
    LTE_AT_CMD_NET_REG, // network registration status
    LTE_AT_CMD_CALL,

    // Event-Driven Notifications
    LTE_AT_CMD_MODULE_READY_RSP,

    LTE_AT_CMD_NUM
} LTE_AT_CMD_TYPE;

typedef enum
{
    LTE_AT_READ,
    LTE_AT_WRITE,
    LTE_AT_QUERY,
    LTE_AT_NUM
} LTE_AT_OPERATION_TYPE;

/************************* CMD *************************/
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





// When defining AT commands, explicitly associate them with response types.?
typedef struct
{
    LTE_AT_CMD_TYPE cmd_id;
    const char *cmd_str;
    const char *ok_rsp; // succeed response
    const char *err_rsp; // error response
    const char *data_rsp; // response with data
} Lte_AT_Response_Table_TYPE;



#ifdef DEFINE_LTE_AT_COMMANDS
#define DECLARE_CMD_REGISTRY(id, str, ok, err, data) \
    [id] = {id, str, ok, err, data}

const Lte_AT_Response_Table_TYPE Lte_AT_Response_Table[] = 
{
    // Command-Driven Queries
    DECLARE_CMD_REGISTRY(LTE_AT_CMD_AT, LTE_AT_AT, LTE_AT_OK_RSP, NULL, NULL),
    DECLARE_CMD_REGISTRY(LTE_AT_CMD_ECHO, LTE_AT_ECHO, LTE_AT_OK_RSP, LTE_AT_ERR_RSP, NULL),
    DECLARE_CMD_REGISTRY(LTE_AT_CMD_SIM_PIN, LTE_AT_SIM_PIN, LTE_AT_OK_RSP, LTE_AT_ERR_RSP, LTE_AT_SIM_PIN),
    DECLARE_CMD_REGISTRY(LTE_AT_CMD_CSQ, LTE_AT_CSQ, NULL, NULL, LTE_AT_CSQ),
    DECLARE_CMD_REGISTRY(LTE_AT_CMD_NET_REG, LTE_AT_NET_REG, LTE_AT_OK_RSP, LTE_AT_ERR_RSP, LTE_AT_NET_REG), 

    // Event-Driven Notifications
    DECLARE_CMD_REGISTRY(LTE_AT_CMD_MODULE_READY_RSP, NULL, NULL, NULL, LTE_AT_MODULE_READY_RSP), 
};

const int Lte_AT_Response_Table_Size = sizeof(Lte_AT_Response_Table)/sizeof(Lte_AT_Response_Table_TYPE);
#else
    extern const Lte_AT_Response_Table_TYPE Lte_AT_Response_Table[];
    extern const int Lte_AT_Response_Table_Size;
#endif

bool Lte_AT_Assemble_Basic(LTE_AT_CMD_TYPE id, LTE_AT_OPERATION_TYPE type, int param, char cmdBuf[], int bufMaxLen, int *usefulLen);

#ifdef __cplusplus
}
#endif

#endif
