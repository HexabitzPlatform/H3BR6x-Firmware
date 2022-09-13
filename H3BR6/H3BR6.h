/*
 BitzOS (BOS) V0.2.7 - Copyright (C) 2017-2022 Hexabitz
 All rights reserved
 
 File Name     : H3BR6.h
 Description   : Header file for module H3BR6.
 	 	 	 	 (Description_of_module)

(Description of Special module peripheral configuration):
>>
>>
>>

 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef H3BR6_H
#define H3BR6_H

/* Includes ------------------------------------------------------------------*/
#include "BOS.h"
#include "H3BR6_MemoryMap.h"
#include "H3BR6_uart.h"
#include "H3BR6_gpio.h"
#include "H3BR6_dma.h"
#include "H3BR6_inputs.h"
#include "H3BR6_eeprom.h"
/* Exported definitions -------------------------------------------------------*/

#define	modulePN		_H3BR6


/* Port-related definitions */
#define	NumOfPorts			5

#define P_PROG 				P2						/* ST factory bootloader UART */

/* Define available ports */
#define _P1 
#define _P2 
#define _P3 
#define _P4 
#define _P5 


/* Define available USARTs */
#define _Usart1 1
#define _Usart2 1
#define _Usart3 1
#define _Usart5 1
#define _Usart6	1


/* Port-UART mapping */

#define P1uart &huart6
#define P2uart &huart2
#define P3uart &huart3
#define P4uart &huart1
#define P5uart &huart5


/* Port Definitions */
#define	USART1_TX_PIN		GPIO_PIN_9
#define	USART1_RX_PIN		GPIO_PIN_10
#define	USART1_TX_PORT		GPIOA
#define	USART1_RX_PORT		GPIOA
#define	USART1_AF			GPIO_AF1_USART1

#define	USART2_TX_PIN		GPIO_PIN_2
#define	USART2_RX_PIN		GPIO_PIN_3
#define	USART2_TX_PORT		GPIOA
#define	USART2_RX_PORT		GPIOA
#define	USART2_AF			GPIO_AF1_USART2

#define	USART3_TX_PIN		GPIO_PIN_10
#define	USART3_RX_PIN		GPIO_PIN_11
#define	USART3_TX_PORT		GPIOB
#define	USART3_RX_PORT		GPIOB
#define	USART3_AF			GPIO_AF4_USART3



#define	USART5_TX_PIN		GPIO_PIN_3
#define	USART5_RX_PIN		GPIO_PIN_2
#define	USART5_TX_PORT		GPIOD
#define	USART5_RX_PORT		GPIOD
#define	USART5_AF			GPIO_AF3_USART5

#define	USART6_TX_PIN		GPIO_PIN_8
#define	USART6_RX_PIN		GPIO_PIN_9
#define	USART6_TX_PORT		GPIOB
#define	USART6_RX_PORT		GPIOB
#define	USART6_AF			GPIO_AF8_USART6

/* Module-specific Definitions */

#define NUM_MODULE_PARAMS						1


/*  Pins For SevenSegment*/
#define Seven_seg_a_Pin 				GPIO_PIN_1
#define Seven_seg_a_GPIO_Port 			GPIOA

#define Seven_seg_b_Pin 				GPIO_PIN_5
#define Seven_seg_b_GPIO_Port 			GPIOA

#define Seven_seg_c_Pin 				GPIO_PIN_6
#define Seven_seg_c_GPIO_Port	 		GPIOA

#define Seven_seg_d_Pin 				GPIO_PIN_1
#define Seven_seg_d_GPIO_Port 			GPIOB

#define Seven_seg_e_Pin 				GPIO_PIN_2
#define Seven_seg_e_GPIO_Port 			GPIOB

#define Seven_seg_f_Pin 				GPIO_PIN_4
#define Seven_seg_f_GPIO_Port 			GPIOA

#define Seven_seg_g_Pin 				GPIO_PIN_7
#define Seven_seg_g_GPIO_Port 			GPIOA

#define Seven_seg_DP_Pin 				GPIO_PIN_0
#define Seven_seg_DP_GPIO_Port 			GPIOB

/* Enable Pin For SevenSegment*/
#define Seven_seg_Enable_1_Pin 			GPIO_PIN_12
#define Seven_seg_Enable_1_GPIO_Port 	GPIOB

#define Seven_seg_Enable_2_Pin 			GPIO_PIN_13
#define Seven_seg_Enable_2_GPIO_Port 	GPIOB

#define Seven_seg_Enable_3_Pin 			GPIO_PIN_11
#define Seven_seg_Enable_3_GPIO_Port 	GPIOA

#define Seven_seg_Enable_4_Pin 			GPIO_PIN_8
#define Seven_seg_Enable_4_GPIO_Port 	GPIOA

#define Seven_seg_Enable_5_Pin 			GPIO_PIN_15
#define Seven_seg_Enable_5_GPIO_Port 	GPIOB

#define Seven_seg_Enable_6_Pin 			GPIO_PIN_12
#define Seven_seg_Enable_6_GPIO_Port 	GPIOA

#define C_LED_Pin 			            GPIO_PIN_14
#define C_LED_GPIO_Port 	            GPIOB

/* Module EEPROM Variables */

// Module Addressing Space 500 - 599
#define _EE_MODULE							500		

/* Module_Status Type Definition */
typedef enum {
	H3BR6_OK =0,
	H3BR6_ERR_UnknownMessage,
	H3BR6_ERR_WrongParams,
	H3BR6_ERROR =255
} Module_Status;

/* Indicator LED */
#define _IND_LED_PORT			GPIOB
#define _IND_LED_PIN			GPIO_PIN_7

/* Export UART variables */
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart6;

/* Define UART Init prototypes */
extern void MX_USART1_UART_Init(void);
extern void MX_USART2_UART_Init(void);
extern void MX_USART3_UART_Init(void);
extern void MX_USART5_UART_Init(void);
extern void MX_USART6_UART_Init(void);
extern void SystemClock_Config(void);
extern void ExecuteMonitor(void);

/* -----------------------------------------------------------------------
 |								  APIs							          |  																 	|
/* -----------------------------------------------------------------------
 */

void SetupPortForRemoteBootloaderUpdate(uint8_t port);
void remoteBootloaderUpdate(uint8_t src,uint8_t dst,uint8_t inport,uint8_t outport);

/* -----------------------------------------------------------------------
 |								Commands							      |															 	|
/* -----------------------------------------------------------------------
 */


#endif /* H3BR6_H */

/************************ (C) COPYRIGHT HEXABITZ *****END OF FILE****/
