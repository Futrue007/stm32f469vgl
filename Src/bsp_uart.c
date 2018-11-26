/***************************************************************************/
/*                                                           Date:08/2017  */
/*                        Pace Uart Driver                                 */
/*             (c)copyright Pacewear                                       */
/*-------------------------------------------------------------------------*/
/*   Author:     future                                                    */
/*   Reference documents:                                                  */
/*-------------------------------------------------------------------------*/
/*  Comments:                                                              */
/*   File :    pace_uart.c                                                 */
/*   Labels :                                                              */
/*   Brief    Uart Driver                                                  */
/*=========================================================================*/
/*  Modifications                                                          */
/*-------------------------------------------------------------------------*/
/*  date    | author  |modification                                        */
/*----------|---------|----------------------------------------------------*/
/*11/08/2017|   |                                                          */
/*----------|---------|----------------------------------------------------*/

/*=========================================================================*/
/*  Problems Report                                                        */
/*-------------------------------------------------------------------------*/
/*  date    | author  |modification                                        */
/*----------|---------|----------------------------------------------------*/
/***************************************************************************/


/**********include files************/
#include "bsp_uart.h"

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart6;


#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)

PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART2 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart6, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}


void uart6_init(void)
{

  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    //Error_Handler();
  }

 
}


void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
    GPIO_InitTypeDef GPIO_InitStruct;
 
    if(huart->Instance == USART6) {
		__HAL_RCC_USART6_CLK_ENABLE();
		__HAL_RCC_GPIOC_CLK_ENABLE();

 
		//PC7 ------------>USART6_RX
		//PC6 ------------>USART6_TX
		GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_6;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		//HAL_NVIC_SetPriority(USART6_IRQn, 12, 0);
       // HAL_NVIC_EnableIRQ(USART6_IRQn);
	}
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{


    if(huart->Instance == USART6) {
		__HAL_RCC_USART6_CLK_DISABLE();
		__HAL_RCC_USART6_FORCE_RESET();
		__HAL_RCC_USART6_RELEASE_RESET();
	//	HAL_GPIO_DeInit(GPIOC, GPIO_PIN_7|GPIO_PIN_6);
	//	HAL_NVIC_DisableIRQ(USART6_IRQn);
    }

}



