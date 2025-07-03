#include "LteUartPort.h"
#include <stdio.h>
#include <string.h>

#define HW_UART_TIMEOUT         (100)

extern void Friend_Uart_Recv(uint8_t *msg, uint32_t len);

static uint8_t uart1Data, uart2Data;

bool HW_UART_Transmit(uint8_t *pData, uint16_t len)
{
    HAL_UART_Transmit(&huart2, pData, len, HW_UART_TIMEOUT);

    return true;
}

void HW_UART_Start_Recv(void)
{
    HAL_UART_Receive_IT(&huart2, &uart2Data, 1);
}


static uint32_t recv_timeout;
void HW_UART_Timer_Reset(void)
{
    recv_timeout = 0;
}

void HW_UART_Timer_Count(uint32_t time_ms)
{
    recv_timeout += time_ms;
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1) // 检查触发的串口
    {
        // 重新启用接收中断（持续接收）
        HAL_UART_Receive_IT(huart, &uart1Data, 1);
    }
    else if(huart->Instance == USART2) // 检查触发的串口
    {
        Friend_Uart_Recv(&uart2Data, 1);

        // 重新启用接收中断（持续接收）
        HAL_UART_Receive_IT(huart, &uart2Data, 1);
    }
}

/******************************************************************************/

void HW_LTE_Disable(void)
{
    HAL_GPIO_WritePin(GPIOB, RST_4G_Pin, GPIO_PIN_SET);
}

void HW_LTE_Enable(void)
{
    HAL_GPIO_WritePin(GPIOB, RST_4G_Pin, GPIO_PIN_RESET);
}

/******************************************************************************/


bool HW_DEBUG_Transmit(uint8_t *pData, uint16_t len)
{
    HAL_UART_Transmit(&huart1, pData, len, HW_UART_TIMEOUT);

    return true;
}

void HW_Printf(char *info)
{
    int len = strlen(info);

    for(int i = 0; i < len; i++)
    {
        HW_DEBUG_Transmit((uint8_t *)(info + i), 1);
    }
}
