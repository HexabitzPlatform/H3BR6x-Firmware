/*
 BitzOS (BOS) V0.2.7 - Copyright (C) 2017-2022 Hexabitz
 All rights reserved

 File Name     : H3BR6.c
 Description   : Source code for module H3BR6.
 	 	 	 	 (Description_of_module)

(Description of Special module peripheral configuration):
>>
>>
>>

 */

/* Includes ------------------------------------------------------------------*/
#include "BOS.h"
#include "H3BR6_inputs.h"
/* Define UART variables */
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart6;

/* Exported variables */
extern FLASH_ProcessTypeDef pFlash;
extern uint8_t numOfRecordedSnippets;


Segment_Codes Digit[6] = {Empty}; //Digit[0]: LSD, Digit[5]: MSD
/* Module exported parameters ------------------------------------------------*/
module_param_t modParam[NUM_MODULE_PARAMS] ={{.paramPtr = NULL, .paramFormat =FMT_FLOAT, .paramName =""}};

/* Private variables ---------------------------------------------------------*/
uint8_t Res_it;          //A global variable to specify the index of the comma
uint8_t StartSevSeg_it;  //A global variable to specify the index of the comma
int Comma_flag=0;        //Activate a flag when a float number is shown


uint8_t  Moving_sentence_buffer[MOVING_SENTENCE_MAX_LENGTH + 6] = {0};
uint8_t  Moving_sentence_length = 0;
uint8_t  Moving_sentence_flag = 0;
uint32_t Moving_sentence_counter = 0;
uint8_t  Moving_sentence_index = 0;

/* Private function prototypes -----------------------------------------------*/
void ExecuteMonitor(void);

/* Create CLI commands --------------------------------------------------------*/

/*-----------------------------------------------------------*/

/* -----------------------------------------------------------------------
 |												 Private Functions	 														|
 ----------------------------------------------------------------------- 
 */

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow : 
 *            System Clock source            = PLL (HSE)
 *            SYSCLK(Hz)                     = 48000000
 *            HCLK(Hz)                       = 48000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 1
 *            HSE Frequency(Hz)              = 8000000
 *            PREDIV                         = 1
 *            PLLMUL                         = 6
 *            Flash Latency(WS)              = 1
 * @param  None
 * @retval None
 */
void SystemClock_Config(void){
	  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	  /** Configure the main internal regulator output voltage
	  */
	  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
	  /** Initializes the RCC Oscillators according to the specified parameters
	  * in the RCC_OscInitTypeDef structure.
	  */
	  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
	  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
	  RCC_OscInitStruct.PLL.PLLN = 12;
	  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  /** Initializes the CPU, AHB and APB buses clocks
	  */
	  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
	                              |RCC_CLOCKTYPE_PCLK1;
	  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

	  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  /** Initializes the peripherals clocks
	  */
	  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_USART2;
	  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
	  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
	  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	  {
	    Error_Handler();
	  }

	  HAL_NVIC_SetPriority(SysTick_IRQn,0,0);
	
}

/*-----------------------------------------------------------*/


/* --- Save array topology and Command Snippets in Flash RO --- 
 */
uint8_t SaveToRO(void){
	BOS_Status result =BOS_OK;
	HAL_StatusTypeDef FlashStatus =HAL_OK;
	uint16_t add =2, temp =0;
	uint8_t snipBuffer[sizeof(snippet_t) + 1] ={0};
	
	HAL_FLASH_Unlock();
	
	/* Erase RO area */
	FLASH_PageErase(FLASH_BANK_1,RO_START_ADDRESS);
	//TOBECHECKED
	FlashStatus =FLASH_WaitForLastOperation((uint32_t ) HAL_FLASH_TIMEOUT_VALUE);
	if(FlashStatus != HAL_OK){
		return pFlash.ErrorCode;
	}
	else{
		/* Operation is completed, disable the PER Bit */
		CLEAR_BIT(FLASH->CR,FLASH_CR_PER);
	}
	
	/* Save number of modules and myID */
	if(myID){
		temp =(uint16_t )(N << 8) + myID;
		//HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,RO_START_ADDRESS,temp);
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,RO_START_ADDRESS,temp);
		//TOBECHECKED
		FlashStatus =FLASH_WaitForLastOperation((uint32_t ) HAL_FLASH_TIMEOUT_VALUE);
		if(FlashStatus != HAL_OK){
			return pFlash.ErrorCode;
		}
		else{
			/* If the program operation is completed, disable the PG Bit */
			CLEAR_BIT(FLASH->CR,FLASH_CR_PG);
		}
		
		/* Save topology */
		for(uint8_t i =1; i <= N; i++){
			for(uint8_t j =0; j <= MaxNumOfPorts; j++){
				if(array[i - 1][0]){
					HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,              //HALFWORD
						//TOBECHECKED
					RO_START_ADDRESS + add,array[i - 1][j]);
					add +=2;
					FlashStatus =FLASH_WaitForLastOperation((uint32_t ) HAL_FLASH_TIMEOUT_VALUE);
					if(FlashStatus != HAL_OK){
						return pFlash.ErrorCode;
					}
					else{
						/* If the program operation is completed, disable the PG Bit */
						CLEAR_BIT(FLASH->CR,FLASH_CR_PG);
					}
				}
			}
		}
	}
	
	// Save Command Snippets
	int currentAdd = RO_MID_ADDRESS;
	for(uint8_t s =0; s < numOfRecordedSnippets; s++){
		if(snippets[s].cond.conditionType){
			snipBuffer[0] =0xFE;		// A marker to separate Snippets
			memcpy((uint8_t* )&snipBuffer[1],(uint8_t* )&snippets[s],sizeof(snippet_t));
			// Copy the snippet struct buffer (20 x numOfRecordedSnippets). Note this is assuming sizeof(snippet_t) is even.
			for(uint8_t j =0; j < (sizeof(snippet_t) / 2); j++){
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,currentAdd,*(uint16_t* )&snipBuffer[j * 2]);
				//HALFWORD
				//TOBECHECKED
				FlashStatus =FLASH_WaitForLastOperation((uint32_t ) HAL_FLASH_TIMEOUT_VALUE);
				if(FlashStatus != HAL_OK){
					return pFlash.ErrorCode;
				}
				else{
					/* If the program operation is completed, disable the PG Bit */
					CLEAR_BIT(FLASH->CR,FLASH_CR_PG);
					currentAdd +=2;
				}
			}
			// Copy the snippet commands buffer. Always an even number. Note the string termination char might be skipped
			for(uint8_t j =0; j < ((strlen(snippets[s].cmd) + 1) / 2); j++){
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,currentAdd,*(uint16_t* )(snippets[s].cmd + j * 2));
				//HALFWORD
				//TOBECHECKED
				FlashStatus =FLASH_WaitForLastOperation((uint32_t ) HAL_FLASH_TIMEOUT_VALUE);
				if(FlashStatus != HAL_OK){
					return pFlash.ErrorCode;
				}
				else{
					/* If the program operation is completed, disable the PG Bit */
					CLEAR_BIT(FLASH->CR,FLASH_CR_PG);
					currentAdd +=2;
				}
			}
		}
	}
	
	HAL_FLASH_Lock();
	
	return result;
}

/* --- Clear array topology in SRAM and Flash RO --- 
 */
uint8_t ClearROtopology(void){
	// Clear the array 
	memset(array,0,sizeof(array));
	N =1;
	myID =0;
	
	return SaveToRO();
}
/*-----------------------------------------------------------*/

/* --- Trigger ST factory bootloader update for a remote module.
 */
void remoteBootloaderUpdate(uint8_t src,uint8_t dst,uint8_t inport,uint8_t outport){

	uint8_t myOutport =0, lastModule =0;
	int8_t *pcOutputString;

	/* 1. Get route to destination module */
	myOutport =FindRoute(myID,dst);
	if(outport && dst == myID){ /* This is a 'via port' update and I'm the last module */
		myOutport =outport;
		lastModule =myID;
	}
	else if(outport == 0){ /* This is a remote update */
		if(NumberOfHops(dst)== 1)
		lastModule = myID;
		else
		lastModule = route[NumberOfHops(dst)-1]; /* previous module = route[Number of hops - 1] */
	}

	/* 2. If this is the source of the message, show status on the CLI */
	if(src == myID){
		/* Obtain the address of the output buffer.  Note there is no mutual
		 exclusion on this buffer as it is assumed only one command console
		 interface will be used at any one time. */
		pcOutputString =FreeRTOS_CLIGetOutputBuffer();

		if(outport == 0)		// This is a remote module update
			sprintf((char* )pcOutputString,pcRemoteBootloaderUpdateMessage,dst);
		else
			// This is a 'via port' remote update
			sprintf((char* )pcOutputString,pcRemoteBootloaderUpdateViaPortMessage,dst,outport);

		strcat((char* )pcOutputString,pcRemoteBootloaderUpdateWarningMessage);
		writePxITMutex(inport,(char* )pcOutputString,strlen((char* )pcOutputString),cmd50ms);
		Delay_ms(100);
	}

	/* 3. Setup my inport and outport for bootloader update */
	SetupPortForRemoteBootloaderUpdate(inport);
	SetupPortForRemoteBootloaderUpdate(myOutport);


	/* 5. Build a DMA stream between my inport and outport */
	StartScastDMAStream(inport,myID,myOutport,myID,BIDIRECTIONAL,0xFFFFFFFF,0xFFFFFFFF,false);
}

/*-----------------------------------------------------------*/

/* --- Setup a port for remote ST factory bootloader update:
 - Set baudrate to 57600
 - Enable even parity
 - Set datasize to 9 bits
 */
void SetupPortForRemoteBootloaderUpdate(uint8_t port){
	UART_HandleTypeDef *huart =GetUart(port);

	huart->Init.BaudRate =57600;
	huart->Init.Parity = UART_PARITY_EVEN;
	huart->Init.WordLength = UART_WORDLENGTH_9B;
	HAL_UART_Init(huart);

	/* The CLI port RXNE interrupt might be disabled so enable here again to be sure */
	__HAL_UART_ENABLE_IT(huart,UART_IT_RXNE);
}

/* --- H3BR6 module initialization.
 */
void Module_Peripheral_Init(void){

	/* Array ports */
	MX_USART1_UART_Init();
	MX_USART2_UART_Init();
	MX_USART3_UART_Init();
	MX_USART5_UART_Init();
	MX_USART6_UART_Init();

	//Init a timer for 7-seg:
	MX_TIM6_Init();

		//seven segment GPIO Init:
	seven_seg_gpio_init();


	/* Create module special task (if needed) */
}

/*-----------------------------------------------------------*/
/* --- H3BR6 message processing task.
 */
Module_Status Module_MessagingTask(uint16_t code,uint8_t port,uint8_t src,uint8_t dst,uint8_t shift){
	Module_Status result =H3BR6_OK;


	switch(code){

		default:
			result =H3BR6_ERR_UnknownMessage;
			break;
	}
	
	return result;
}
/* --- Get the port for a given UART. 
 */
uint8_t GetPort(UART_HandleTypeDef *huart){

	if(huart->Instance == USART6)
		return P1;
	else if(huart->Instance == USART2)
		return P2;
	else if(huart->Instance == USART3)
		return P3;
	else if(huart->Instance == USART1)
		return P4;
	else if(huart->Instance == USART5)
		return P5;
	

	return 0;
}

/*-----------------------------------------------------------*/

/* --- Register this module CLI Commands
 */
void RegisterModuleCLICommands(void){

}

/*-----------------------------------------------------------*/


/* Module special task function (if needed) */
//void Module_Special_Task(void *argument){
//
//	/* Infinite loop */
//	uint8_t cases; // Test variable.
//	for(;;){
//		/*  */
//		switch(cases){
//
//
//			default:
//				osDelay(10);
//				break;
//		}
//
//		taskYIELD();
//	}
//
//}


/*-----------------------------------------------------------*/
Segment_Codes get_number_code(uint8_t digit)
{
	Segment_Codes status = H3BR6_OK;

	Segment_Codes code;
	switch(digit)
	{
	case 0:
		code = zero_number;
		break;
	case 1:
		code = one_number;
		break;
	case 2:
		code = two_number;
		break;
	case 3:
		code = three_number;
		break;
	case 4:
		code = four_number;
		break;
	case 5:
		code = five_number;
		break;
	case 6:
		code = six_number;
		break;
	case 7:
		code = seven_number;
		break;
	case 8:
		code = eight_number;
		break;
	case 9:
		code = nine_number;
		break;

	default:
		code = Empty;
		break;

	}
	return code;
}

/*-----------------------------------------------------------*/
Segment_Codes get_letter_code(char letter){
	Segment_Codes status = H3BR6_OK;

	Segment_Codes letter_code;
	switch(letter){

	case 'A':
		letter_code=A_letter;
		break;
	case 'B':
		letter_code=B_letter;
		break;
	case'C':
		letter_code=C_letter;
		break;
	case'D':
		letter_code=D_letter;
		break;
	case'E':
		letter_code=E_letter;
		break;
	case'F':
		letter_code=F_letter;
		break;
	case'G':
		letter_code=G_letter;
		break;
	case'H':
		letter_code=H_letter;
		break;
	case'I':
		letter_code=I_letter;
		break;
	case'J':
		letter_code=J_letter;
		break;
	case'K':
		letter_code=K_letter;
		break;
	case'L':
		letter_code=L_letter;
		break;
	case'M':
		letter_code=M_letter;
		break;
	case'N':
		letter_code=N_letter;
		break;
	case'O':
		letter_code=O_letter;
		break;
	case'P':
		letter_code=P_letter;
		break;
	case'Q':
		letter_code=Q_letter;
		break;
	case'R':
		letter_code=R_letter;
		break;
	case'S':
		letter_code=S_letter;
		break;
	case'T':
		letter_code=T_letter;
		break;
	case'U':
		letter_code=U_letter;
		break;
	case'V':
		letter_code=V_letter;
		break;
	case'W':
		letter_code=W_letter;
		break;
	case'X':
		letter_code=X_letter;
		break;
	case'Y':
		letter_code=Y_letter;
		break;
	case'Z':
		letter_code=Z_letter;
		break;

	case 'a':
		letter_code=a_letter;
		break;
	case 'b':
		letter_code=b_letter;
		break;
	case'c':
		letter_code=c_letter;
		break;
	case'd':
		letter_code=d_letter;
		break;
	case'e':
		letter_code=e_letter;
		break;
	case'f':
		letter_code=f_letter;
		break;
	case'g':
		letter_code=g_letter;
		break;
	case'h':
		letter_code=h_letter;
		break;
	case'i':
		letter_code=i_letter;
		break;
	case'j':
		letter_code=j_letter;
		break;
	case'k':
		letter_code=k_letter;
		break;
	case'l':
		letter_code=l_letter;
		break;
	case'm':
		letter_code=m_letter;
		break;
	case'n':
		letter_code=n_letter;
		break;
	case'o':
		letter_code=o_letter;
		break;
	case'p':
		letter_code=p_letter;
		break;
	case'q':
		letter_code=q_letter;
		break;
	case'r':
		letter_code=r_letter;
		break;
	case's':
		letter_code=s_letter;
		break;
	case't':
		letter_code=t_letter;
		break;
	case'u':
		letter_code=u_letter;
		break;
	case'v':
		letter_code=v_letter;
		break;
	case'w':
		letter_code=w_letter;
		break;
	case'x':
		letter_code=x_letter;
		break;
	case'y':
		letter_code=y_letter;
		break;
	case'z':
		letter_code=z_letter;
		break;

	default: break;

	}
	return letter_code;
}
/*-----------------------------------------------------------*/

Segment_Codes clear_all_digits(void){
	Segment_Codes status = H3BR6_OK;
	for(int i=0;i<6;i++) Digit[i] = Empty;
	Comma_flag = 0;
	Moving_sentence_flag = 0;
	Moving_sentence_counter = 0;
}
/*-----------------------------------------------------------*/




/*-----------------------------------------------------------*/

/* -----------------------------------------------------------------------
 |								  APIs							          | 																 	|
/* -----------------------------------------------------------------------
 */
Module_Status SevenDisplayNumber(int32_t Number, uint8_t StartSevSeg)
{
	Module_Status status = H3BR6_OK;
	clear_all_digits();   //Seven segment display off

	int32_t max_value, min_value;
	uint8_t index_digit_last; // the index of the last used digit int 7-segment.
	uint8_t signal = 0; //0 for Positive numbers, and 1 for negative numbers.
	uint8_t length;


	if( !(StartSevSeg >= 0 && StartSevSeg <= 5) )
	{
		status = H3BR6_ERR_WrongParams;
		return status;
	}


	switch(StartSevSeg)
	{
	case 0:
		max_value = 999999;
		min_value = -99999;
		break;
	case 1:
		max_value = 99999;
		min_value = -9999;
		break;
	case 2:
		max_value = 9999;
		min_value = -999;
		break;
	case 3:
		max_value = 999;
		min_value = -99;
		break;
	case 4:
		max_value = 99;
		min_value = -9;
		break;
	case 5:
		max_value = 9;
		min_value = 0;
		break;
		// Case 5 is a special case.
	default: break;
	}


	if(StartSevSeg == 5 && (Number < 0 || Number > 9) )
	{
		status = H3BR6_NUMBER_IS_OUT_OF_RANGE;
		return status;
	}
   if(Number > max_value || Number < min_value)
	{
		status = H3BR6_NUMBER_IS_OUT_OF_RANGE;
		return status;
	}

	if(Number < 0)
		{
			signal = 1;
			Number *= -1;
		}


			if(Number>0 && Number<=9)
			{
				length= 1;
			}

			if(Number>9 && Number<=99)
			{
				length= 2;
			}

			if(Number>99 && Number<=999)
			{
				length= 3;
			}

			if(Number>999 && Number<=9999)
			{
				length= 4;
			}

			if(Number>9999 && Number<=99999)
			{
				length= 5;
			}

			if(Number>99999 && Number<=999999)
			{
				length= 6;
			}

	index_digit_last = length+StartSevSeg;
	if(signal==1)
	{
		Digit[index_digit_last] = Symbol_minus;
		Digit[index_digit_last+1]=Empty;
	}

	for(int i = StartSevSeg; i < 6;i++)
	{
		if(i == index_digit_last && signal == 1) continue;
		Digit[i] = get_number_code(Number % 10);
		Number /= 10;
	}



		for(int x=index_digit_last; x<6; x++)
		{
			if(signal==1 && x==index_digit_last)continue;
			Digit[x]=Empty;
		}


	return status;

}
/* ----------------------------------------------------------------------------*/

/*-----------------------------------------------------------*/


/* -----------------------------------------------------------------------
 |								Commands							      |
   -----------------------------------------------------------------------
 */



/*-----------------------------------------------------------*/

/************************ (C) COPYRIGHT HEXABITZ *****END OF FILE****/
