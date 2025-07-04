#include "LteUartPort.h"
#include <stdio.h>
#include <string.h>

#define HW_UART_TIMEOUT         (100)

extern void friend_RawDataRecv(uint8_t *msg, uint32_t lenIn);

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


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1) // 检查触发的串口
    {
        // 重新启用接收中断（持续接收）
        HAL_UART_Receive_IT(huart, &uart1Data, 1);
    }
    else if(huart->Instance == USART2) // 检查触发的串口
    {
        friend_RawDataRecv(&uart2Data, 1);

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
