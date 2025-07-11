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
#include "cmsis_os.h"
#include "fatfs.h"
#include "libjpeg.h"
#include "app_touchgfx.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "file_browser.h"
#include <inttypes.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
buzzer_t Buz;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//#define RX_BUFFER_SIZE 			256

//#define REFRESH_COUNT                            ((uint32_t)918)       /*SDRAM refresh counter (120MHz SDRAM clock)*/
//#define SDRAM_TIMEOUT                            ((uint32_t)0xFFFF)
//#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
//#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
//#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
//#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
//#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
//#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
//#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
//#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
//#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
//#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
//#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
RTC_TimeTypeDef sTime;
RTC_DateTypeDef sDate;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc3;
DMA_HandleTypeDef hdma_adc3;

CRC_HandleTypeDef hcrc;

DMA2D_HandleTypeDef hdma2d;

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c3;
DMA_HandleTypeDef hdma_i2c1_rx;
DMA_HandleTypeDef hdma_i2c1_tx;
DMA_HandleTypeDef hdma_i2c3_rx;
DMA_HandleTypeDef hdma_i2c3_tx;

JPEG_HandleTypeDef hjpeg;
MDMA_HandleTypeDef hmdma_jpeg_infifo_th;
MDMA_HandleTypeDef hmdma_jpeg_outfifo_th;

LPTIM_HandleTypeDef hlptim1;

LTDC_HandleTypeDef hltdc;

RNG_HandleTypeDef hrng;

RTC_HandleTypeDef hrtc;

SD_HandleTypeDef hsd1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim8;
DMA_HandleTypeDef hdma_tim2_up;
DMA_HandleTypeDef hdma_tim3_ch2;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart1_rx;

SDRAM_HandleTypeDef hsdram1;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow7,
};
/* Definitions for TouchGFXTask */
osThreadId_t TouchGFXTaskHandle;
const osThreadAttr_t TouchGFXTask_attributes = {
  .name = "TouchGFXTask",
  .stack_size = 10000 * 4,
  .priority = (osPriority_t) osPriorityNormal1,
};
/* Definitions for brightnessLcdTa */
osThreadId_t brightnessLcdTaHandle;
const osThreadAttr_t brightnessLcdTa_attributes = {
  .name = "brightnessLcdTa",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for VideoTask */
osThreadId_t VideoTaskHandle;
const osThreadAttr_t VideoTask_attributes = {
  .name = "VideoTask",
  .stack_size = 4096 * 4,
  .priority = (osPriority_t) osPriorityLow1,
};
/* Definitions for uSDTask */
osThreadId_t uSDTaskHandle;
const osThreadAttr_t uSDTask_attributes = {
  .name = "uSDTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityLow6,
};
/* Definitions for TouchTask */
osThreadId_t TouchTaskHandle;
const osThreadAttr_t TouchTask_attributes = {
  .name = "TouchTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal7,
};
/* Definitions for TouchCalibKeyTa */
osThreadId_t TouchCalibKeyTaHandle;
const osThreadAttr_t TouchCalibKeyTa_attributes = {
  .name = "TouchCalibKeyTa",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal1,
};
/* Definitions for touch_mutex */
osMutexId_t touch_mutexHandle;
const osMutexAttr_t touch_mutex_attributes = {
  .name = "touch_mutex"
};
/* Definitions for xDataMutex */
osMutexId_t xDataMutexHandle;
const osMutexAttr_t xDataMutex_attributes = {
  .name = "xDataMutex"
};
/* Definitions for spi_mutex */
osMutexId_t spi_mutexHandle;
const osMutexAttr_t spi_mutex_attributes = {
  .name = "spi_mutex"
};
/* Definitions for xTPMutex */
osMutexId_t xTPMutexHandle;
const osMutexAttr_t xTPMutex_attributes = {
  .name = "xTPMutex"
};
/* USER CODE BEGIN PV */
uint8_t receivedChar;
//static FMC_SDRAM_CommandTypeDef Command;
/* -------------------------------------------------------------------------- */
/*                              Version variables                             */
/* -------------------------------------------------------------------------- */
const uint8_t Soft_Version[4] = { MAJOR, MINOR, PATCH , BUILD};
const uint8_t UI_Version[4] = { UIMAJOR, UIMINOR, UIPATCH , UIBUILD};
const uint8_t Board_Version[4] = { PCBMAJOR, PCBMINOR, PCBPATCH , PCBRELEASE};

/* -------------------------------------------------------------------------- */
/*                              RTC variables                                 */
/* -------------------------------------------------------------------------- */
char time[10];
char date[10];
uint8_t alarm =0;

/* -------------------------------------------------------------------------- */
/*                              Display variables                             */
/* -------------------------------------------------------------------------- */
uint32_t Brightness_LCD = 255;




/* -------------------------------------------------------------------------- */
/*                      Definitions Vendor_ID Queue                           */
/* -------------------------------------------------------------------------- */
/* Definitions for serialIDQueue */
osMessageQueueId_t serialIDQueueHandle;
const osMessageQueueAttr_t serialIDQueue_attributes = {
  .name = "serialIDQueue"
};



/* -------------------------------------------------------------------------- */
/*                         Definitions XPT2046 Queue                          */
/* -------------------------------------------------------------------------- */
#ifdef XPT2046_ENABLE
/* Definitions for uiQueue */
osMessageQueueId_t uiQueueHandle;
const osMessageQueueAttr_t uiQueue_attributes = {
  .name = "uiQueue"
};
#endif /* XPT2046_ENABLE */


/* -------------------------------------------------------------------------- */
/*                     Definitions File Browser Queue                         */
/* -------------------------------------------------------------------------- */
#ifdef FILE_BROWSER_ENABLE
//osMessageQueueId_t dirNoQueueHandle;								/* Definitions for dirNoQueue Directory amont*/
//const osMessageQueueAttr_t dirNoQueue_attributes = {
//  .name = "dirNoQueue"
//};
//
//osMessageQueueId_t fileNameQueueHandle;								/* Definitions for fileNameQueue File Name*/
//const osMessageQueueAttr_t fileNameQueue_attributes = {
//  .name = "fileNameQueue"
//};
//
//osMessageQueueId_t fileSizeQueueHandle;								/* Definitions for fileSizeQueue File Size*/
//const osMessageQueueAttr_t fileSizeQueue_attributes = {
//  .name = "fileSizeQueue"
//};
//
//osMessageQueueId_t dirNameQueueHandle;								/* Definitions for dirNameQueue Directory Name*/
//const osMessageQueueAttr_t dirNameQueue_attributes = {
//  .name = "dirNameQueue"
//};
//
//osMessageQueueId_t fileNoQueueHandle;								/* Definitions for fileNoQueue File amont*/
//const osMessageQueueAttr_t fileNoQueue_attributes = {
//  .name = "fileNoQueue"
//};
//
//osMessageQueueId_t openDirQueueHandle;								/* Definitions for openDirQueue Open Directory from GUI*/
//const osMessageQueueAttr_t openDirQueue_attributes = {
//  .name = "openDirQueue"
//};
//
//osMessageQueueId_t pathQueueHandle;									/* Definitions for pathQueue Address Path*/
//const osMessageQueueAttr_t pathQueue_attributes = {
//  .name = "pathQueue"
//};
//
//osMessageQueueId_t openFileCmdQueueHandle;							/* Definitions for readFileQueue Open File CMD from GUI*/
//const osMessageQueueAttr_t openFileCmdQueue_attributes = {
//  .name = "openFileCmdQueue"
//};
//
//osMessageQueueId_t sendFileNameQueueHandle;								/* Definitions for sendFileNameQueue send File Name to GUI*/
//const osMessageQueueAttr_t sendFileNameQueue_attributes = {
//  .name = "sendFileName"
//};
//
//osMessageQueueId_t readFileQueueHandle;								/* Definitions for readFileQueue Read File*/
//const osMessageQueueAttr_t readFileQueue_attributes = {
//  .name = "readFileQueue"
//};
//
//osMessageQueueId_t storageUsageQueueHandle;								/* Definitions for storageUsageQueue Get Storage Capacity*/
//const osMessageQueueAttr_t storageUsageQueue_attributes = {
//  .name = "storageUsageQueue"
//};
//
//osMessageQueueId_t eofQueueHandle;									/* Definitions for eofQueue End of File Read*/
//const osMessageQueueAttr_t eofQueue_attributes = {
//  .name = "eofQueue"
//};
//
//osMessageQueueId_t mkDirQueueHandle;									/* Definitions for mkDirQueue Make New Folder*/
//const osMessageQueueAttr_t mkDirQueue_attributes = {
//  .name = "mkDirQueue"
//};
//
//osMessageQueueId_t mkFilQueueHandle;									/* Definitions for mkDirQueue Make New File*/
//const osMessageQueueAttr_t mkFilQueue_attributes = {
//  .name = "mkFilQueue"
//};
//
//osMessageQueueId_t writeFileQueueHandle;							/* Definitions for writeFileQueue Write File*/
//const osMessageQueueAttr_t writeFileQueue_attributes = {
//  .name = "writeFileQueue"
//};
//
//osMessageQueueId_t writeFileDataQueueHandle;							/* Definitions for  writeFileDataQueue Write File Data*/
//const osMessageQueueAttr_t  writeFileDataQueue_attributes = {
//  .name = " writeFileDataQueue"
//};
//
//osMessageQueueId_t removeDirQueueHandle;									/* Definitions for removeDirQueue remove Folder*/
//const osMessageQueueAttr_t removeDirQueue_attributes = {
//  .name = "removeDirQueue"
//};
//
//osMessageQueueId_t errorQueueHandle;									/* Definitions for errorQueue send Error Dialog*/
//const osMessageQueueAttr_t errorQueue_attributes = {
//  .name = "errorQueue"
//};
//
//osMessageQueueId_t successDialogQueueHandle;							/* Definitions for successDialogQueue send Success Dialog*/
//const osMessageQueueAttr_t successDialogQueue_attributes = {
//  .name = "successDialogQueue"
//};


osMessageQueueId_t fileInfoQueueHandle;
const osMessageQueueAttr_t fileInfoQueue_attributes = {
  .name = "fileInfoQueue"
};

osMessageQueueId_t pathQueueHandle;
const osMessageQueueAttr_t pathQueue_attributes = {
  .name = "pathQueue"
};

osMessageQueueId_t storageUsageQueueHandle;
const osMessageQueueAttr_t storageUsageQueue_attributes = {
  .name = "storageUsageQueue"
};

osMessageQueueId_t fileContentQueueHandle;
const osMessageQueueAttr_t fileContentQueue_attributes = {
  .name = "fileContentQueue"
};

osMessageQueueId_t systemMessageQueueHandle;
const osMessageQueueAttr_t systemMessageQueue_attributes = {
  .name = "systemMessageQueue"
};

osMessageQueueId_t fileCommandQueueHandle;
const osMessageQueueAttr_t fileCommandQueue_attributes = {
  .name = "fileCommandQueue"
};

#endif /* FILE_BROWSER_ENABLE */


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_MDMA_Init(void);
static void MX_CRC_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_RTC_Init(void);
static void MX_DMA2D_Init(void);
static void MX_ADC3_Init(void);
static void MX_I2C3_Init(void);
static void MX_LPTIM1_Init(void);
static void MX_TIM3_Init(void);
static void MX_I2C1_Init(void);
static void MX_FMC_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_LTDC_Init(void);
static void MX_JPEG_Init(void);
static void MX_SDMMC1_SD_Init(void);
static void MX_TIM8_Init(void);
static void MX_TIM2_Init(void);
static void MX_RNG_Init(void);
void StartDefaultTask(void *argument);
extern void TouchGFX_Task(void *argument);
void StartTaskBrightnessLCD(void *argument);
extern void videoTaskFunc(void *argument);
void StartTaskuSD(void *argument);
void StartTouchTask(void *argument);
void StartTouchCalibKeyTask(void *argument);

/* USER CODE BEGIN PFP */
void Shutdown();
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
uint32_t getSerialNumber(void);
uint32_t get_random_number(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	for(uint8_t i = 0; i < 8; i++)
	{
		NVIC->ICER[i]=0xFFFFFFFF;
		NVIC->ICPR[i]=0xFFFFFFFF;
	}
	RCC->CFGR = 0;
	SCB->VTOR = QSPI_BASE;
	__enable_irq();
	__set_PRIMASK(0);

	*((uint32_t*)0x51046108) = 1; /* DMA2D issue in cut 1.1 */
	*((uint32_t*)0x51008108) = 1; /* AXI SRAM issue in cut 1.2 */
  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_MDMA_Init();
  MX_CRC_Init();
  MX_USART1_UART_Init();
  MX_RTC_Init();
  MX_DMA2D_Init();
  MX_ADC3_Init();
  MX_I2C3_Init();
  MX_LPTIM1_Init();
  MX_TIM3_Init();
  MX_I2C1_Init();
  MX_FMC_Init();
  MX_USART2_UART_Init();
  MX_LTDC_Init();
  MX_JPEG_Init();
  MX_LIBJPEG_Init();
  MX_SDMMC1_SD_Init();
  MX_FATFS_Init();
  MX_TIM8_Init();
  MX_TIM2_Init();
  MX_RNG_Init();
  MX_TouchGFX_Init();
  /* Call PreOsInit function */
  MX_TouchGFX_PreOSInit();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim2);
  delayInit();
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();
  /* Create the mutex(es) */
  /* creation of touch_mutex */
  touch_mutexHandle = osMutexNew(&touch_mutex_attributes);

  /* creation of xDataMutex */
  xDataMutexHandle = osMutexNew(&xDataMutex_attributes);

  /* creation of spi_mutex */
  spi_mutexHandle = osMutexNew(&spi_mutex_attributes);

  /* creation of xTPMutex */
  xTPMutexHandle = osMutexNew(&xTPMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */

//  metricsMutexHandle = osMutexNew(&metricsMutex_attributes);
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */

  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* creation of adcQueue */
//  adcQueueHandle = osMessageQueueNew (4, sizeof(ADC_Data_t), &adcQueue_attributes);
//
//  batteryDataQueueHandle = (4, sizeof(BatteryState), &batteryDataQueue_attributes);		/* Definitions for mAhBatteryQueue send Battery mAh*/

  /* creation of serialIDQueue */
  serialIDQueueHandle = osMessageQueueNew (1, sizeof(int), &serialIDQueue_attributes);


#ifdef XPT2046_ENABLE
  /* creation of uiQueue */
  uiQueueHandle = osMessageQueueNew (10, sizeof(UI_Command_t), &uiQueue_attributes);
#endif /* XPT2046_ENABLE */

#ifdef FILE_BROWSER_ENABLE
//  dirNoQueueHandle = osMessageQueueNew (1, sizeof(int), &dirNoQueue_attributes);  						/* creation of dirNoQueue Directory amont */
//
//  fileNameQueueHandle = osMessageQueueNew (1, sizeof(char[MAX_FILE_NAME_LEN]), &fileNameQueue_attributes);  			/* creation of fileNameQueue File Name*/
//
//  fileSizeQueueHandle = osMessageQueueNew (1, sizeof(int), &fileSizeQueue_attributes);  				/* creation of fileSizeQueue File Size*/
//
//  dirNameQueueHandle = osMessageQueueNew (1, sizeof(char[MAX_DIR_NAME_LEN]), &dirNameQueue_attributes);  			/* creation of dirNameQueue Directory Name*/
//
//  fileNoQueueHandle = osMessageQueueNew (1, sizeof(int), &fileNoQueue_attributes);  					/* creation of fileNoQueue File amont*/
//
//  openDirQueueHandle = osMessageQueueNew (1, sizeof(char[MAX_OPEN_DIR_NAME_LEN]), &openDirQueue_attributes);  			/* creation of openDirQueue Open Directory from GUI*/
//
//  pathQueueHandle = osMessageQueueNew (1, sizeof(char[100]), &pathQueue_attributes);					/* creation of pathQueue Address Path*/
//
//  openFileCmdQueueHandle = osMessageQueueNew (1, sizeof(char[100]), &openFileCmdQueue_attributes);		/* creation of openFileCmdQueue Open File CMD from GUI*/
//
//  sendFileNameQueueHandle = osMessageQueueNew (1, sizeof(char[100]), &sendFileNameQueue_attributes);	/* Definitions for sendFileNameQueue send File Name to GUI*/
//
//  readFileQueueHandle = osMessageQueueNew (1, sizeof(char[MAX_READ_BUFF_SIZE]), &readFileQueue_attributes);			/* creation of readFileQueue Read File*/
//
//  storageUsageQueueHandle = osMessageQueueNew (2, sizeof(long), &storageUsageQueue_attributes);			/* creation of readFileQueue Read File*/
//
//  eofQueueHandle = osMessageQueueNew (1, sizeof(bool), &eofQueue_attributes);							/* creation of eofQueue End of File Read*/
//
//  mkDirQueueHandle = osMessageQueueNew (1, sizeof(char[20]), &mkDirQueue_attributes);					/* Definitions for mkDirQueue Make New Folder*/
//
//  mkFilQueueHandle = osMessageQueueNew (1, sizeof(char[20]), &mkDirQueue_attributes);					/* Definitions for mkDirQueue Make New File*/
//
//  writeFileQueueHandle = osMessageQueueNew (1, sizeof(char[20]), &writeFileQueue_attributes);			/* Definitions for writeFileQueue Write File*/
//
//  writeFileDataQueueHandle = osMessageQueueNew (1, sizeof(char[MAX_WRITE_BUFF_SIZE]), &writeFileDataQueue_attributes);			/* Definitions for writeFileQueue Write File*/
//
//  removeDirQueueHandle = osMessageQueueNew (1, sizeof(char[100]), &removeDirQueue_attributes);			/* Definitions for removeDirQueue Remove Folder*/
//
//  errorQueueHandle = osMessageQueueNew (1, sizeof(char[50]), &errorQueue_attributes);					/* Definitions for errorQueue send Error Dialog*/
//
//  successDialogQueueHandle = osMessageQueueNew (1, sizeof(char[50]), &successDialogQueue_attributes);	/* Definitions for successDialogQueue send Success Dialog*/

  // Create queues
  fileInfoQueueHandle = osMessageQueueNew(20, sizeof(FileInfo), &fileInfoQueue_attributes);
  pathQueueHandle = osMessageQueueNew(5, MAX_PATH_LEN, &pathQueue_attributes);
  storageUsageQueueHandle = osMessageQueueNew(3, 3*sizeof(uint32_t), &storageUsageQueue_attributes);
  fileContentQueueHandle = osMessageQueueNew(10, MAX_READ_BUFF_SIZE, &fileContentQueue_attributes);
  systemMessageQueueHandle = osMessageQueueNew(10, sizeof(SystemMessage), &systemMessageQueue_attributes);
  fileCommandQueueHandle = osMessageQueueNew(10, sizeof(FileCommand), &fileCommandQueue_attributes);

#endif /* FILE_BROWSER_ENABLE */

  /* creation of hx711Queue */
//  hx711QueueHandle = osMessageQueueNew (12, sizeof(HX711_QueueData_t), &hx711Queue_attributes);
  /* creation of metricsQueue */
//  metricsQueueHandle = osMessageQueueNew (10, sizeof(SystemMetrics_t), &metricsQueue_attributes);

  /* creation of weightsQueue */
//  weightsQueueHandle = osMessageQueueNew (10, sizeof(ChannelWeights_t), &weightsQueue_attributes);

  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of TouchGFXTask */
  TouchGFXTaskHandle = osThreadNew(TouchGFX_Task, NULL, &TouchGFXTask_attributes);

  /* creation of brightnessLcdTa */
  brightnessLcdTaHandle = osThreadNew(StartTaskBrightnessLCD, NULL, &brightnessLcdTa_attributes);

  /* creation of VideoTask */
  VideoTaskHandle = osThreadNew(videoTaskFunc, NULL, &VideoTask_attributes);

  /* creation of uSDTask */
  uSDTaskHandle = osThreadNew(StartTaskuSD, NULL, &uSDTask_attributes);

  /* creation of TouchTask */
  TouchTaskHandle = osThreadNew(StartTouchTask, NULL, &TouchTask_attributes);

  /* creation of TouchCalibKeyTa */
  TouchCalibKeyTaHandle = osThreadNew(StartTouchCalibKeyTask, NULL, &TouchCalibKeyTa_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_HIGH);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSI
                              |RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  RCC_OscInitStruct.PLL.PLLR = 4;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CKPER;
  PeriphClkInitStruct.CkperClockSelection = RCC_CLKPSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC3_Init(void)
{

  /* USER CODE BEGIN ADC3_Init 0 */

  /* USER CODE END ADC3_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC3_Init 1 */

  /* USER CODE END ADC3_Init 1 */

  /** Common config
  */
  hadc3.Instance = ADC3;
  hadc3.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV6;
  hadc3.Init.Resolution = ADC_RESOLUTION_16B;
  hadc3.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc3.Init.LowPowerAutoWait = DISABLE;
  hadc3.Init.ContinuousConvMode = DISABLE;
  hadc3.Init.NbrOfConversion = 3;
  hadc3.Init.DiscontinuousConvMode = DISABLE;
  hadc3.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T8_TRGO;
  hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc3.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
  hadc3.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc3.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  hadc3.Init.OversamplingMode = ENABLE;
  hadc3.Init.Oversampling.Ratio = 1024;
  hadc3.Init.Oversampling.RightBitShift = ADC_RIGHTBITSHIFT_10;
  hadc3.Init.Oversampling.TriggeredMode = ADC_TRIGGEREDMODE_SINGLE_TRIGGER;
  hadc3.Init.Oversampling.OversamplingStopReset = ADC_REGOVERSAMPLING_CONTINUED_MODE;
  if (HAL_ADC_Init(&hadc3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_VBAT;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_64CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  sConfig.OffsetSignedSaturation = DISABLE;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_VREFINT;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC3_Init 2 */

  /* USER CODE END ADC3_Init 2 */

}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
  hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
  hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/**
  * @brief DMA2D Initialization Function
  * @param None
  * @retval None
  */
static void MX_DMA2D_Init(void)
{

  /* USER CODE BEGIN DMA2D_Init 0 */

  /* USER CODE END DMA2D_Init 0 */

  /* USER CODE BEGIN DMA2D_Init 1 */

  /* USER CODE END DMA2D_Init 1 */
  hdma2d.Instance = DMA2D;
  hdma2d.Init.Mode = DMA2D_M2M;
  hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB565;
  hdma2d.Init.OutputOffset = 0;
  hdma2d.LayerCfg[1].InputOffset = 0;
  hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_RGB565;
  hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  hdma2d.LayerCfg[1].InputAlpha = 0;
  hdma2d.LayerCfg[1].AlphaInverted = DMA2D_REGULAR_ALPHA;
  hdma2d.LayerCfg[1].RedBlueSwap = DMA2D_RB_REGULAR;
  hdma2d.LayerCfg[1].ChromaSubSampling = DMA2D_NO_CSS;
  if (HAL_DMA2D_Init(&hdma2d) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_DMA2D_ConfigLayer(&hdma2d, 1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DMA2D_Init 2 */

  /* USER CODE END DMA2D_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00602173;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.Timing = 0x00602173;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_ENABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c3, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c3, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

}

/**
  * @brief JPEG Initialization Function
  * @param None
  * @retval None
  */
static void MX_JPEG_Init(void)
{

  /* USER CODE BEGIN JPEG_Init 0 */

  /* USER CODE END JPEG_Init 0 */

  /* USER CODE BEGIN JPEG_Init 1 */

  /* USER CODE END JPEG_Init 1 */
  hjpeg.Instance = JPEG;
  if (HAL_JPEG_Init(&hjpeg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN JPEG_Init 2 */

  /* USER CODE END JPEG_Init 2 */

}

/**
  * @brief LPTIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_LPTIM1_Init(void)
{

  /* USER CODE BEGIN LPTIM1_Init 0 */

  /* USER CODE END LPTIM1_Init 0 */

  /* USER CODE BEGIN LPTIM1_Init 1 */

  /* USER CODE END LPTIM1_Init 1 */
  hlptim1.Instance = LPTIM1;
  hlptim1.Init.Clock.Source = LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC;
  hlptim1.Init.Clock.Prescaler = LPTIM_PRESCALER_DIV32;
  hlptim1.Init.Trigger.Source = LPTIM_TRIGSOURCE_SOFTWARE;
  hlptim1.Init.OutputPolarity = LPTIM_OUTPUTPOLARITY_HIGH;
  hlptim1.Init.UpdateMode = LPTIM_UPDATE_IMMEDIATE;
  hlptim1.Init.CounterSource = LPTIM_COUNTERSOURCE_INTERNAL;
  hlptim1.Init.Input1Source = LPTIM_INPUT1SOURCE_GPIO;
  hlptim1.Init.Input2Source = LPTIM_INPUT2SOURCE_GPIO;
  if (HAL_LPTIM_Init(&hlptim1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LPTIM1_Init 2 */

  /* USER CODE END LPTIM1_Init 2 */

}

/**
  * @brief LTDC Initialization Function
  * @param None
  * @retval None
  */
static void MX_LTDC_Init(void)
{

  /* USER CODE BEGIN LTDC_Init 0 */

  /* USER CODE END LTDC_Init 0 */

  LTDC_LayerCfgTypeDef pLayerCfg = {0};

  /* USER CODE BEGIN LTDC_Init 1 */

  /* USER CODE END LTDC_Init 1 */
  hltdc.Instance = LTDC;
  hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  hltdc.Init.HorizontalSync = 39;
  hltdc.Init.VerticalSync = 9;
  hltdc.Init.AccumulatedHBP = 127;
  hltdc.Init.AccumulatedVBP = 29;
  hltdc.Init.AccumulatedActiveW = 1151;
  hltdc.Init.AccumulatedActiveH = 629;
  hltdc.Init.TotalWidth = 1191;
  hltdc.Init.TotalHeigh = 649;
  hltdc.Init.Backcolor.Blue = 0;
  hltdc.Init.Backcolor.Green = 0;
  hltdc.Init.Backcolor.Red = 0;
  if (HAL_LTDC_Init(&hltdc) != HAL_OK)
  {
    Error_Handler();
  }
  pLayerCfg.WindowX0 = 0;
  pLayerCfg.WindowX1 = 1024;
  pLayerCfg.WindowY0 = 0;
  pLayerCfg.WindowY1 = 600;
  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
  pLayerCfg.Alpha = 255;
  pLayerCfg.Alpha0 = 0;
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
  pLayerCfg.FBStartAdress = 0;
  pLayerCfg.ImageWidth = 1024;
  pLayerCfg.ImageHeight = 600;
  pLayerCfg.Backcolor.Blue = 255;
  pLayerCfg.Backcolor.Green = 0;
  pLayerCfg.Backcolor.Red = 0;
  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LTDC_Init 2 */

  /* USER CODE END LTDC_Init 2 */

}

/**
  * @brief RNG Initialization Function
  * @param None
  * @retval None
  */
static void MX_RNG_Init(void)
{

  /* USER CODE BEGIN RNG_Init 0 */

  /* USER CODE END RNG_Init 0 */

  /* USER CODE BEGIN RNG_Init 1 */

  /* USER CODE END RNG_Init 1 */
  hrng.Instance = RNG;
  hrng.Init.ClockErrorDetection = RNG_CED_ENABLE;
  if (HAL_RNG_Init(&hrng) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RNG_Init 2 */

  /* USER CODE END RNG_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
  if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != 0x32F2)
    {
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x20;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */
    }
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x32F2);  // backup register
  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SDMMC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDMMC1_SD_Init(void)
{

  /* USER CODE BEGIN SDMMC1_Init 0 */

  /* USER CODE END SDMMC1_Init 0 */

  /* USER CODE BEGIN SDMMC1_Init 1 */

  /* USER CODE END SDMMC1_Init 1 */
  hsd1.Instance = SDMMC1;
  hsd1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
  hsd1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd1.Init.BusWide = SDMMC_BUS_WIDE_4B;
  hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd1.Init.ClockDiv = 8;
  /* USER CODE BEGIN SDMMC1_Init 2 */

  /* USER CODE END SDMMC1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 240-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 0xffffffff-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 255;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM8 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM8_Init(void)
{

  /* USER CODE BEGIN TIM8_Init 0 */

  /* USER CODE END TIM8_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM8_Init 1 */

  /* USER CODE END TIM8_Init 1 */
  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 24000-1;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 1000-1;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM8_Init 2 */

  /* USER CODE END TIM8_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_8;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_8;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
  /* DMA1_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
  /* DMA1_Stream4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
  /* DMA1_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
  /* DMA1_Stream7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream7_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream7_IRQn);
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  /* DMA2_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);

}

/**
  * Enable MDMA controller clock
  */
static void MX_MDMA_Init(void)
{

  /* MDMA controller clock enable */
  __HAL_RCC_MDMA_CLK_ENABLE();
  /* Local variables */

  /* MDMA interrupt initialization */
  /* MDMA_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(MDMA_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(MDMA_IRQn);

}

/* FMC initialization function */
static void MX_FMC_Init(void)
{

  /* USER CODE BEGIN FMC_Init 0 */

  /* USER CODE END FMC_Init 0 */

  FMC_SDRAM_TimingTypeDef SdramTiming = {0};

  /* USER CODE BEGIN FMC_Init 1 */

  /* USER CODE END FMC_Init 1 */

  /** Perform the SDRAM1 memory initialization sequence
  */
  hsdram1.Instance = FMC_SDRAM_DEVICE;
  /* hsdram1.Init */
  hsdram1.Init.SDBank = FMC_SDRAM_BANK1;
  hsdram1.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_9;
  hsdram1.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_13;
  hsdram1.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_16;
  hsdram1.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
  hsdram1.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_2;
  hsdram1.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  hsdram1.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_3;
  hsdram1.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;
  hsdram1.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_0;
  /* SdramTiming */
  SdramTiming.LoadToActiveDelay = 2;
  SdramTiming.ExitSelfRefreshDelay = 7;
  SdramTiming.SelfRefreshTime = 4;
  SdramTiming.RowCycleDelay = 7;
  SdramTiming.WriteRecoveryTime = 3;
  SdramTiming.RPDelay = 2;
  SdramTiming.RCDDelay = 2;

  if (HAL_SDRAM_Init(&hsdram1, &SdramTiming) != HAL_OK)
  {
    Error_Handler( );
  }

  /* USER CODE BEGIN FMC_Init 2 */
  SDRAM_Initialization_Sequence(&hsdram1);
  /* USER CODE END FMC_Init 2 */
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOI, TOUCH_RST_Pin|T_MOSI_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED_GR_Pin|LED_RD_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(T_CLK_GPIO_Port, T_CLK_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : Buzzer_Pin */
  GPIO_InitStruct.Pin = Buzzer_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Buzzer_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : TOUCH_RST_Pin */
  GPIO_InitStruct.Pin = TOUCH_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(TOUCH_RST_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : CARD_DETECT_Pin */
  GPIO_InitStruct.Pin = CARD_DETECT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(CARD_DETECT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PF6 PF7 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : PF8 PF9 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pin : KEY2_Pin */
  GPIO_InitStruct.Pin = KEY2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(KEY2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_GR_Pin LED_RD_Pin */
  GPIO_InitStruct.Pin = LED_GR_Pin|LED_RD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : T_CLK_Pin */
  GPIO_InitStruct.Pin = T_CLK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(T_CLK_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : TOUCH_INT_Pin */
  GPIO_InitStruct.Pin = TOUCH_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(TOUCH_INT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : T_MISO_Pin */
  GPIO_InitStruct.Pin = T_MISO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(T_MISO_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : T_MOSI_Pin */
  GPIO_InitStruct.Pin = T_MOSI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(T_MOSI_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(TOUCH_INT_EXTI_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(TOUCH_INT_EXTI_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/* -------------------------------------------------------------------------- */
/*                 Print the characters to UART (printf)                      */
/* -------------------------------------------------------------------------- */
/**
  * @brief Print the characters to UART (printf).
  * @retval int
  */
#ifdef __GNUC__
  /* With GCC, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
int __io_putchar(int ch)
#else
int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the UART3 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
//  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
  return ch;
}





void Shutdown()
{
	saveBatteryPercentage();
	PreSleepProcessing(0);
}
volatile uint32_t userButtonPressed = 0;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_3)
    {
    	userButtonPressed = 1;
    }

//    if (GPIO_Pin == GPIO_PIN_4)
//    {
//    	polarity = 1;
//    }
//    else
//    {
//    	polarity = 0;
//    }
}

//void HAL_GPIO_EXTI_Callback2(uint16_t GPIO_Pin)
//{
//    if (GPIO_Pin == GPIO_PIN_4)
//    {
//    	polarity = 0;
//    }
//}



uint32_t getSerialNumber(void)
{
	// آدرس UID برای STM32H743
//	uint32_t *UID_ptr = (uint32_t*) 0x1FF1E800;
//	char uid_str[50];  // رشته نهایی قابل ارسال
//	snprintf(uid_str, sizeof(uid_str), "CHIP_ID: %08lX%08lX%08lX\r\n",
//			UID_ptr[0], UID_ptr[1], UID_ptr[2]);
//
//	// ارسال از طریق UART
//	HAL_UART_Transmit(&huart1, (uint8_t*) uid_str, strlen(uid_str),
//			HAL_MAX_DELAY);

	uint32_t serialNumber = *(uint32_t*)(0x1FF1E800);    /*Get Serial Number from chip*/
//	char uid_str[50];
//	snprintf(uid_str, sizeof(uid_str), "CHIP_ID: %08lX%08lX%08lX\r\n",
//			serialNumber[0], serialNumber[1], serialNumber[2]);
	return serialNumber;
}



uint32_t get_random_number(void) {
  uint32_t random = 0;
  if (HAL_RNG_GenerateRandomNumber(&hrng, &random) != HAL_OK) {
    // در صورت خطا، از روش fallback استفاده کنید
    return HAL_GetTick(); // مثال ساده
  }
  return random;
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 5 */
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();

	printf("Software Version:(%d.%d.%d.%d)\r\n", Soft_Version[0], Soft_Version[1], Soft_Version[2], Soft_Version[3]);
	printf("UI Version:(%d.%d.%d.%d)\r\n", UI_Version[0], UI_Version[1], UI_Version[2], UI_Version[3]);
	printf("Board Version:(%d.%d.%d.%d)\r\n", Board_Version[0], Board_Version[1], Board_Version[2], Board_Version[3]);



	BuzzerInit(&Buz, Buzzer_GPIO_Port, Buzzer_Pin);
  /* Infinite loop */
  for(;;)
  {
	  vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1));
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartTaskBrightnessLCD */
/**
* @brief Function implementing the brightnessLcdTa thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskBrightnessLCD */
void StartTaskBrightnessLCD(void *argument)
{
  /* USER CODE BEGIN StartTaskBrightnessLCD */
//	uint32_t lcdBrightnessValue= 255;
//	uint16_t pwmData[255] = {255};
	HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_2, (uint32_t*)Brightness_LCD, 1);
//	HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_2, (uint32_t*)lcdBrightnessValue, 1);

  /* Infinite loop */
  for(;;)
  {
	  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, Brightness_LCD);
//	osMessageQueuePut(brtlcdQueueHandle, &lcdBrightnessValue, 0, 0);
//	if (osMessageQueueGetCount(brtlcdQueueHandle)>0)  //  if there is some msg in the queue
//	{
//		if (osMessageQueueGet(brtlcdQueueHandle, &lcdBrightnessValue, 0, 0)== osOK)
//		{
//			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, lcdBrightnessValue);
//		}
//	}
    osDelay(1);
  }
  /* USER CODE END StartTaskBrightnessLCD */
}

/* USER CODE BEGIN Header_StartTaskuSD */
/**
* @brief Function implementing the uSDTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskuSD */
void StartTaskuSD(void *argument)
{
  /* USER CODE BEGIN StartTaskuSD */
	FileCommand cmd;
	SystemInfo status;

//	SD_init();
	/* Infinite loop */
	for (;;)
	{
        if (osMessageQueueGet(fileCommandQueueHandle, &cmd, NULL, osWaitForever) == osOK) {
            switch (cmd.type) {
                case CMD_ENTER_FOLDER:
                    enterFolder(cmd.name);
                    break;
                case CMD_EXIT_FOLDER:
                    exitFolder();
                    break;
                case CMD_GO_TO_ROOT:
                	goToRoot();
                    break;
                case CMD_NEW_FOLDER:
                	createFolder(cmd.name);
                    break;
                case CMD_DELETE_ITEM:
                	deleteItem(cmd.name);
                    break;
                case CMD_NEW_FILE:
                	createFile(cmd.name);
                    break;
                case CMD_RENAME_ITEM:
                	renameItem(cmd.name, cmd.newName);
                    break;
                case CMD_GET_STORAGE_INFO:
                	getStorageInfo();
                    break;
                case CMD_FORMAT_SD:
                    formatSD_Card();
                    break;
                case CMD_READ_FILE:
                	readFile(cmd.name);
                    break;
                case CMD_WRITE_FILE:
                	break;

            }

            // به‌روزرسانی وضعیت سیستم
//            getCurrentPath(status.path, sizeof(status.path));
//            getStorageInfo(&status.totalKB, &status.usedKB, &status.freeKB);
//            osMessageQueuePut(fileToUiQueue, &status, 0, 0);
        }

		osDelay(1);
	}
  /* USER CODE END StartTaskuSD */
}

/* USER CODE BEGIN Header_StartTouchTask */
/**
* @brief Function implementing the TouchTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTouchTask */
void StartTouchTask(void *argument)
{
  /* USER CODE BEGIN StartTouchTask */
#ifdef GT911_ENABLE
	  gt911_Init();
	  GTInfo infor;
//	  fwResolution(1024, 600);
	  gt911_SetResolution(1024, 600);
	  readInfo(&infor);
	  printf("x=%d  y=%d\r\n",infor.xResolution,infor.yResolution);
	  if(gt911_ReadID())
	  printf("device found!! \r\n");
	  gt911_TS_Start();
//	  show_config();
//	    if (Gt911_isConnected()) {
//	        printf("GT911 Connected!\n");
//	        gt911_Init();
//	        gt911_TS_Start();
//	    } else {
//	        printf("GT911 Not Found!\n");
////	        while(1);
//	    }


	  // حساسیت بالا (برای صفحات محافظ دار)
	  gt911_SetTouchSensitivity(5, 40);

	  // حساسیت متوسط (پیش‌فرض)
//	  gt911_SetTouchSensitivity(20, 20);

	  // حساسیت پایین (برای محیط‌های مرطوب)
//	  gt911_SetTouchSensitivity(40, 10);

	  gt911_StartCalibration();
	    CheckTouchSensorHealth();

#endif /*GT911_ENABLE*/
	  /* Infinite loop */
	    while(1) {
#ifdef XPT2046_ENABLE
	        if (osMutexAcquire(touch_mutexHandle, osWaitForever) == osOK) {
	            TP_Scan(0);  // اسکن مختصات
	            osMutexRelease(touch_mutexHandle);
	        }
#endif /*XPT2046_ENABLE*/
//	    	vTaskDelay(pdMS_TO_TICKS(1)); // ۱۰۰ هرتز
	    osDelay(1);
	    }
  /* USER CODE END StartTouchTask */
}

/* USER CODE BEGIN Header_StartTouchCalibKeyTask */
/**
* @brief Function implementing the TouchCalibKeyTa thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTouchCalibKeyTask */
void StartTouchCalibKeyTask(void *argument)
{
  /* USER CODE BEGIN StartTouchCalibKeyTask */
#ifdef XPT2046_ENABLE
	uint32_t pressStartTime = 0;
	const uint32_t LONG_PRESS_TIME = 3000; // 3 ثانیه
#endif /* XPT2046_ENABLE */
	/* Infinite loop */
	while (1)
	{
#ifdef XPT2046_ENABLE
		if (HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_3) == GPIO_PIN_RESET)
		{ // دکمه فشرده شده
			if (pressStartTime == 0)
			{
				pressStartTime = osKernelGetTickCount();
			}
			else
			{
				if (osKernelGetTickCount() - pressStartTime >= LONG_PRESS_TIME)
				{

					HAL_GPIO_TogglePin(LED_GR_GPIO_Port, LED_GR_Pin);

//					hx711_t loadcell_CH1;
//					hx711_init(&loadcell_CH1, HX711_CLK_CH1_GPIO_Port, HX711_CLK_CH1_Pin, HX711_DATA_CH1_GPIO_Port, HX711_DATA_CH1_Pin);
//					vTaskResume(calibrationTaskHandle);
//					vTaskSuspend(Weights_TaskHandle);
//					osMutexAcquire(metricsMutexHandle, osWaitForever);
//					hx711_update_calibration(1);
//					hx711_update_calibration_one(&loadcell_CH1);
//					osMutexRelease(metricsMutexHandle);

//					hx711_update_calibration(2);

//                	if (CalibrationActive != 0)
//                	{
//					vTaskSuspend(TouchTaskHandle);
//                		vTaskSuspend(taskAnalogInputHandle);
					//		vTaskSuspend(calibrationTaskHandle);
//                	}

					TP_Adjust();

//                    // ارسال پیام برای ورود به کالیبراسیون
//                    char msg[] = "Entering Calibration Mode";
//                    xQueueSend(xCalibrationQueue, msg, portMAX_DELAY);
//
//                    // تغییر صفحه به calibrationScreen
//                    requestShowCalibrationScreen(); // تابع رابط TouchGFX
					pressStartTime = 0;
				}
			}
		}
		else
		{
			pressStartTime = 0;
		}
#endif /*XPT2046_ENABLE*/
		vTaskDelay(pdMS_TO_TICKS(10));
//	osDelay(10);
	}
  /* USER CODE END StartTouchCalibKeyTask */
}

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x24000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.BaseAddress = 0xC0000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_512MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER2;
  MPU_InitStruct.Size = MPU_REGION_SIZE_32MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER3;
  MPU_InitStruct.BaseAddress = 0x90000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_512MB;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER4;
  MPU_InitStruct.Size = MPU_REGION_SIZE_32MB;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER5;
  MPU_InitStruct.Size = MPU_REGION_SIZE_1MB;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER6;
  MPU_InitStruct.BaseAddress = 0x30000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_64KB;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER7;
  MPU_InitStruct.BaseAddress = 0x38000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_32KB;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
