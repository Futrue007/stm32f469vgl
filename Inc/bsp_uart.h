#ifndef _PACE_BSP_UART_H_
#define _PACE_BSP_UART_H_
#include <stdio.h>
#include <string.h>
#include "stm32f4xx_hal.h"


#define USARTx                           USART6
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART6_CLK_ENABLE();
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE() 

#define USARTx_FORCE_RESET()             __HAL_RCC_USART6_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __HAL_RCC_USART6_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_6
#define USARTx_TX_GPIO_PORT              GPIOC  

#define USARTx_RX_PIN                    GPIO_PIN_7
#define USARTx_RX_GPIO_PORT              GPIOC 
#define USARTx_RX_AF                     GPIO_AF7_USART2

#define USARTx_IRQn                      USART6_IRQn


void uart6_init(void);
#endif //_PACE_BSP_UART_H_
