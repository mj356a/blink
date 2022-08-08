/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define led_on __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1, 499)
#define led_off __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1, 0) 
#define key_push HAL_GPIO_ReadPin(key1_GPIO_Port,key1_Pin)==0
#define key_pull HAL_GPIO_ReadPin(key1_GPIO_Port,key1_Pin)==1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t ModeSet=0;
uint8_t key_state=0;//0:未按下；1：短按；2：短按两次：3：长按
uint16_t pwmVal=50;   //mode2的PWM计数  
uint8_t dir=1; 
uint16_t counter1=0;//mode1的计数
uint8_t frequence=0;//mode1的闪烁时间

void scan();
void Meun();
void breath();

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */


  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM3_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);//初始化定时器3，用于mode2
	HAL_TIM_Base_Start_IT(&htim2);//初始化定时器2，用于mode1


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		scan();
		HAL_Delay(200);
		Meun();
		


  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 144;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void scan()
{
	if(key_push)
	{
		HAL_Delay(20);//按键消抖
		if(key_push)//第一次按下
		{
			HAL_Delay(200);
			if(key_pull)//中途没有按
			{
				HAL_Delay(20);//按键消抖
				if(key_pull)
				{
					HAL_Delay(300);
					if(key_push)//连续按两次
					{
						HAL_Delay(20);//按键消抖
						if(key_push)
							key_state=2;//按两次			
					}else if(key_pull)
						key_state=1;//短按一下
				}	
			}	else	if(key_push)//中途按下了	
				{
					HAL_Delay(300);
					if(key_push)
						key_state=3;//长按			
				}
		}	
	}
}

void Meun()
{
//	if(key_state==3)
//		ModeSet=(ModeSet+1)%3;
		switch(ModeSet)
		{
			case 0:
				led_on;
			if(key_state==3)//模式转换
			{
				ModeSet=(ModeSet+1)%3;
				key_state=0;
			}
				break;
			case 1:
				if(counter1<=frequence)
					led_on;
				else
					led_off;
				if(key_state==1)
				{
					frequence=(frequence+1)%10;	
					key_state=0;//按键复位
				}else if(key_state==3)//mode转换
				{
					ModeSet=(ModeSet+1)%3;
					key_state=0;
				}			
				break;
			case 2:
				led_off;
				breath();
				if(key_state==1)
				{
					if(pwmVal<450)
					{
						pwmVal+=50;
						key_state=0;
						breath();
					}else if(pwmVal>=450)
					{
						pwmVal=50;
						key_state=0;
						breath();						
					}
				}else if(key_state==2)
					{
						key_state=0;
						breath();
						if(pwmVal<=60)
						{
							pwmVal=50;//小于50限幅操作
						}else
							pwmVal-=50;
					}else if(key_state==3)
					{
						ModeSet=(ModeSet+1)%3;
						key_state=0;
					}		
				break;			
			default:
				break;
		}	
	}


/*定时器2，每100ms产生一次中断*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == (&htim2))
    {
			counter1=(counter1+1)%10;
    }
}
void breath()
{
		TIM3->CCR1 = pwmVal;   //改变占空比 
	  HAL_Delay(1);		
	
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

