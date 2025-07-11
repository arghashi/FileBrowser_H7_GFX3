/**
 ******************************************************************************
 * @file    gt911.c
 * @author  Joao Costa - costasjf@gmail.com
 * @brief   This file provides a set of functions needed to manage the GT911
 *          touch screen devices. This driver works with TouchGFX, using single
 *          point detection scheme.
 ******************************************************************************/

/************ INCLUDES ************/

#include "gt911.h"
#include "main.h"
#include "stdbool.h"
#include "stdio.h"
#include "string.h"




/* Touch screen driver structure initialization */
//TS_DrvTypeDef gt911_ts_drv = {
//		gt911_Init,
//		gt911_ReadID,
//		gt911_TS_Start,
//		gt911_TS_DetectTouch,
//};

// متغیرهای سراسری
GT911_TouchPoint active_touches[5]; // پشتیبانی از ۵ نقطه لمسی همزمان
const uint32_t LONG_PRESS_DURATION = 2000; // 2


/************ PRIVATE VARIABLES ************/

static gt911_handle_TypeDef gt911_ts_handle; //GT911 touch handle variable


static void attachInterrupt(GPIO_TypeDef *PORTx,uint8_t pin);
static bool  pinCheck(GPIO_TypeDef *PORTx,uint8_t pin, uint8_t level);
static void  pinHold(GPIO_TypeDef *PORTx,uint8_t pin);
static void  pinSet(GPIO_TypeDef *PORTx,uint8_t pin, uint8_t level);
static void  usSleep(uint16_t microseconds);
static void  msSleep(uint16_t milliseconds);
static bool  reset(void);
static void  pinIn(GPIO_TypeDef *PORTx,uint16_t pin);
static void  pinOut(GPIO_TypeDef *PORTx,uint16_t pin);

/************ PUBLIC VARIABLES ************/


/************ PRIVATE FUNCTIONS PROTOTYPES ************/

/**
 * @brief  Returns if I2C was previous initialized or not.
 * @param  None.
 * @retval : I2C initialization status.
 */
static uint8_t gt911_Get_I2C_InitializedStatus(void);

/**
 * @brief  I2C initialize if needed.
 * @param  None.
 * @retval : None.
 */
static void gt911_I2C_InitializeIfRequired(void);

/**
 * @brief  Basic static configuration of TouchScreen
 * @param  DeviceAddr: GT911 Device address for communication on I2C Bus.
 * @retval Status GT911_STATUS_OK or GT911_STATUS_NOT_OK.
 */
static uint32_t gt911_TS_Configure();


bool Gt911_isConnected(void)
{
  if (HAL_I2C_IsDeviceReady(&_GT911_I2C, GT911_Read_Address, 10, 1000)==HAL_OK)
    return true;
  else
    return false;
}

/************ PUBLIC FUNCTIONS DEFINITION ************/

/**
 * @brief  Initializes the gt911 i2c communication bus from MCU to GT911
 * @param  DeviceAddr: gt911 address for i2c communication
 * @retval None
 */
void gt911_Init(void)
{
    TOUCH_INT_Clock();
    TOUCH_RST_Clock();
    // Take chip some time to start
    msSleep(300);
    reset();
    msSleep(200);
	/* Initialize I2C link if needed */
//	gt911_I2C_InitializeIfRequired();
    gt911_ts_handle.i2cInitialized = GT911_I2C_INITIALIZED;
//	gt911_ts_handle.i2cInitialized = GT911_I2C_NOT_INITIALIZED;
	gt911_ts_handle.currActiveTouchNb = 0;

//	GT911_Enable_Touch_Features();
//	memset(active_touches, 0, sizeof(active_touches));
}


bool  reset()
{
    msSleep(1);
    pinOut(TOUCH_INT_Port, TOUCH_INT_Pin);
    pinOut(TOUCH_RST_Port, TOUCH_RST_Pin);
    pinHold(TOUCH_INT_Port, TOUCH_INT_Pin);
    pinHold(TOUCH_RST_Port, TOUCH_RST_Pin);

    /* begin select I2C slave addr */
    /* T2: > 10ms */
    msSleep(11);
    /* HIGH: 0x28/0x29 (0x14 7bit), LOW: 0xBA/0xBB (0x5D 7bit) */
    pinSet(TOUCH_INT_Port, TOUCH_INT_Pin, 1 );//i2cAddr == GOODIX_I2C_ADDR_28
    /* T3: > 100us */
    usSleep(110);
    pinIn(TOUCH_RST_Port, TOUCH_RST_Pin);
//  if (!pinCheck(rstPin, HIGH))
//  return false;
    /* T4: > 5ms */
    msSleep(6);
    pinHold(TOUCH_INT_Port,TOUCH_INT_Pin);
    /* end select I2C slave addr */
    /* T5: 50ms */
    msSleep(51);
    pinIn(TOUCH_INT_Port,TOUCH_INT_Pin); // INT pin has no pullups so simple set to floating input
//    attachInterrupt(TOUCH_INT_Pin, _goodix_irq_handler, RISING);
//    detachInterrupt(TOUCH_INT_Pin, _goodix_irq_handler);
//    attachInterrupt(TOUCH_INT_Port,TOUCH_INT_Pin);
    msSleep(51);
    return true;
}


/**
 * @brief  Read the gt911 device ID, pre initialize I2C in case of need to be
 *         able to read the GT911 device ID, and verify this is a GT911.
 * @param  DeviceAddr: I2C GT911 Slave address.
 * @retval The Device ID (two bytes).
 */
uint8_t gt911_ReadID()
{
	uint8_t attemptscount = 0;
	uint8_t devicefounded = 0;

	//Initialize I2C link if needed
	gt911_I2C_InitializeIfRequired();

	//Maximum of 4 attempts to read ID
	for(attemptscount = 0; ((attemptscount < 3) && !(devicefounded)); attemptscount++)
	{
		//Read gt911 ID registers: ASCII -> "9110"
			
		if((TS_IO_Read(GT911_PRODUCT_ID1_REG) == 0x39) && \
				(TS_IO_Read(GT911_PRODUCT_ID2_REG) == 0x31) &&\
				(TS_IO_Read(GT911_PRODUCT_ID3_REG) == 0x31) &&\
				(TS_IO_Read(GT911_PRODUCT_ID4_REG) == 0x00))
		{
			/* Set device as found */
			devicefounded = 1;
		}
	}
	/* Return the device ID value */
	return devicefounded;
}

/**
 * @brief  Configures the touch Screen IC device to start detecting touches
 * @param  DeviceAddr: Device address on communication Bus (I2C slave address).
 * @retval None.
 */
void gt911_TS_Start()
{
	
	//uint8_t teste[2] = {0, 0};
	/* Minimum static configuration of GT911 */
	gt911_TS_Configure();

	/* By default set GT911 IC in Polling mode : no INT generation on GT911 for new touch available */
	/* Note TS_INT is active low                                                                      */
//	gt911_TS_DisableIT(DeviceAddr);

	uint16_t chksum = 0;
	uint8_t checksum = 0;
	//	uint8_t touch_number = 0;

	//Reads current checksum value:
	checksum = TS_IO_Read(GT911_CONFIG_CHKSUM_REG);
	//	//Reads current touch number register velue:
	//	touch_number = TS_IO_Read(0x804C);


//	for(int i=0;i<168;i++)
//{
//	TS_IO_Write( GT911_CONFIG_VERSION_REG+i, goodix_config[i]);
//}
	
	
	//Writes the number of touch points config register:
	TS_IO_Write( 0x804C, 0x01); //call	I2Cx_WriteMultiple(&hI2cAudioHandler, Addr, Reg, I2C_MEMADD_SIZE_16BIT,(uint8_t*)&Value, 1);
	//Calculates new checksum:
	

 //TS_IO_Write( GT911_REFRESH_RATE_REG, 200);
//TS_IO_Write( GT911_X_THRESHOLD_REG, 0X00);
//TS_IO_Write( GT911_Y_THRESHOLD_REG, 0X00);

//TS_IO_Write( GT911_SCREEN_TOUCH_LEVEL, 0);
//TS_IO_Write( GT911_SCREEN_LEAVE_LEVEL, 0);

	TS_IO_Write( GT911_CONFIG_FRESH_REG, 0x01);

	TS_IO_Write( GT911_ESD_CHECK_REG, 0x00);

	for(uint16_t reg = GT911_CONFIG_VERSION_REG; reg <= 0x80FE; ++reg)
	{
		chksum += TS_IO_Read( reg);
	}
	chksum = ~chksum +1;
	checksum = (uint8_t)chksum;

	TS_IO_Write( GT911_CONFIG_CHKSUM_REG, checksum);
	
	

	

//	int reg=TS_IO_Read( GT911_MODULE_SWITCH1_REG);
		
//  TS_IO_Write( GT911_COMMAND_REG, 0x05);
//	TS_IO_Write( GT911_COMMAND_CHECK_REG, 0x2A);
	
//	TS_IO_Write( GT911_LOW_POWER_CTRL_REG, 3);

//TS_IO_Write( GT911_MODULE_SWITCH2_REG, 4);


//    printf("0x%x=0x%x \r\n",i,reg_value);
	printf("REFRESH=%d \r\n",TS_IO_Read( GT911_REFRESH_RATE_REG));
	printf("time=%d ",TS_IO_Read( GT911_MODULE_SWITCH1_REG));
	
	
	vTaskDelay(10);

	//Confere valores escritos
	//teste[0] = TS_IO_Read(0x804C);
	//teste[1] = TS_IO_Read(GT911_CONFIG_CHKSUM_REG);

	//Cleans the status register for the new touch read
	
	TS_IO_Write( GT911_BUFFER_STATUS_REG, 0);


	/* By default set GT911 IC in Polling mode : no INT generation on GT911 for new touch available */
	/* Note TS_INT is active low                                                                      */
//	gt911_TS_DisableIT(DeviceAddr);
}

/**
 * @brief  Return if there is touches detected or not.
 *         Try to detect new touches and forget the old ones (reset internal global
 *         variables).
 * @param  DeviceAddr: Device address on communication Bus.
 * @retval : Number of active touches detected (can be 0, 1 or 2).
 */

uint8_t coordinates[4] = {0, 0, 0, 0};

uint8_t gt911_TS_DetectTouch(uint16_t *X, uint16_t *Y)
{
	volatile uint8_t nbTouch = 0;
	volatile uint8_t touch_status = 0;
	volatile uint8_t ucReadData = 0;
	static uint16_t coord;

	//Read register GT911_TD_STAT_REG to check number of touches detection
	touch_status = TS_IO_Read( GT911_BUFFER_STATUS_REG);

	//touch_status = TS_IO_Read(GT911_STATUS_REG);
	nbTouch = touch_status & GT911_4LSB_MASK; //Just the 4 LSB refers to active touch points

	if (touch_status)
	{
//		printf("\r\n 0x%X \r\n",touch_status);
		ucReadData = TS_IO_Read(GT911_POINT1_X_L);
		coord = (ucReadData & GT911_TOUCH_POS_LSB_MASK)
				>> GT911_TOUCH_POS_LSB_SHIFT;
		/* Read high part of X position */
		ucReadData = TS_IO_Read( GT911_POINT1_X_H);
		coord |= ((ucReadData & GT911_TOUCH_POS_MSB_MASK)
				>> GT911_TOUCH_POS_MSB_SHIFT) << 8;
		/* Send back ready X position to caller */
		*X = coord;

		/* Read low part of Y position */
		ucReadData = TS_IO_Read(GT911_POINT1_Y_L);
		coord = (ucReadData & GT911_TOUCH_POS_LSB_MASK)
				>> GT911_TOUCH_POS_LSB_SHIFT;
		/* Read high part of Y position */
		ucReadData = TS_IO_Read(GT911_POINT1_Y_H);
		coord |= ((ucReadData & GT911_TOUCH_POS_MSB_MASK)
				>> GT911_TOUCH_POS_MSB_SHIFT) << 8;
		/* Send back ready Y position to caller */
		*Y = coord;

		//Cleans the status register for the new touch read
		TS_IO_Write( GT911_BUFFER_STATUS_REG, 0);
		//Is data ready for reading and touch detected?
		if (nbTouch > 1)
		{
			nbTouch = 0;
		}
	}
	TS_IO_Write( GT911_BUFFER_STATUS_REG, 0);
	/* Update gt911 driver internal global : current number of active touches */
	gt911_ts_handle.currActiveTouchNb = nbTouch;

	return (nbTouch);
}


/* Static functions bodies-----------------------------------------------*/


/**
 * @brief  Return the status of I2C was initialized or not.
 * @param  None.
 * @retval : I2C initialization status.
 */
static uint8_t gt911_Get_I2C_InitializedStatus(void)
{
	return(gt911_ts_handle.i2cInitialized);
}

/**
 * @brief  I2C initialize if needed.
 * @param  None.
 * @retval : None.
 */
static void gt911_I2C_InitializeIfRequired(void)
{
	if(gt911_Get_I2C_InitializedStatus() == GT911_I2C_NOT_INITIALIZED)
	{
		/* Initialize TS IO BUS layer (I2C) */
		//TS_IO_Init();
//		MX_I2C1_Init();
		/* Set state to initialized */
		gt911_ts_handle.i2cInitialized = GT911_I2C_INITIALIZED;
	}
}

/**
 * @brief  Basic static configuration of TouchScreen
 * @param  DeviceAddr: GT911 Device address for communication on I2C Bus.
 * @retval Status GT911_STATUS_OK or GT911_STATUS_NOT_OK.
 */
 uint32_t gt911_TS_Configure(void)
{
	uint32_t status = GT911_STATUS_OK;

	/* Nothing special to be done for GT911 */

	return(status);
}



 volatile uint8_t i2cRxComplete = 0;
 volatile uint8_t i2cTxComplete = 0;
		
		
void TS_IO_Write(uint16_t Reg, uint8_t Value)
{
	uint8_t data[3];
	 data[0]=Reg>>8;
	 data[1]=Reg;
	 data[2]=Value;
  if( HAL_I2C_Master_Transmit(&_GT911_I2C, GT911_Write_Address,  (unsigned char*) data, 3, 1000)==HAL_ERROR)
	{
      vTaskDelay(100);
      HAL_GPIO_WritePin(TOUCH_RST_GPIO_Port, TOUCH_RST_Pin, RESET);
	  vTaskDelay(100);
	  gt911_Init();
	  gt911_TS_Start();
	}
}


/**
  * @brief  Reads a single data.
  * @param  Addr: I2C address
  * @param  Reg: Reg address
  * @retval Data to be read
  */
uint8_t TS_IO_Read(uint16_t Reg)
{
	uint8_t data[3];
	uint8_t read_value = 0;

	data[0] = Reg >> 8;
	data[1] = Reg;
	HAL_I2C_Master_Transmit(&_GT911_I2C, GT911_Write_Address,
			(unsigned char*) data, 2, 1000);

	HAL_I2C_Master_Receive(&_GT911_I2C, GT911_Read_Address, &read_value, 1,
			1000);

	return read_value;
}



static void  pinOut(GPIO_TypeDef *PORTx,uint16_t pin) {
  //pinMode(pin, OUTPUT);
  
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  HAL_GPIO_WritePin(PORTx, pin, GPIO_PIN_RESET);
  
  GPIO_InitStruct.Pin = pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(PORTx, &GPIO_InitStruct);
  
}

static void  pinIn(GPIO_TypeDef *PORTx,uint16_t pin) {
//  pinMode(pin, INPUT);
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  HAL_GPIO_WritePin(PORTx, pin, GPIO_PIN_RESET);
  
  GPIO_InitStruct.Pin = pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(PORTx, &GPIO_InitStruct);

}

static void  pinSet(GPIO_TypeDef *PORTx,uint8_t pin, uint8_t level) {
  //digitalWrite(pin, level);
  HAL_GPIO_WritePin(PORTx, pin, level);
}

static void  pinHold(GPIO_TypeDef *PORTx,uint8_t pin) {
  //pinSet(pin, LOW);
   HAL_GPIO_WritePin(PORTx, pin, GPIO_PIN_RESET);
}

static bool  pinCheck(GPIO_TypeDef *PORTx,uint8_t pin, uint8_t level) {
  //return digitalRead(pin) == level;

  if(HAL_GPIO_ReadPin(PORTx, pin))
  return 1;
  else
  return 0;

}

static void attachInterrupt(GPIO_TypeDef *PORTx,uint8_t pin)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(PORTx, &GPIO_InitStruct);

    /* EXTI interrupt init*/
    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}


static void  msSleep(uint16_t milliseconds)
{
    //delay(milliseconds);
//    osDelay(milliseconds);
//	delay_ms(milliseconds);
	vTaskDelay(milliseconds);
}

static void  usSleep(uint16_t microseconds)
{
    // delayMicroseconds(microseconds);
//    osDelay(microseconds);
	delay_us(microseconds);
}





void print_details(void)
{
	#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
    #define BYTE_TO_BINARY(byte)  \
     (byte & 0x80 ? '1' : '0'), \
     (byte & 0x40 ? '1' : '0'), \
     (byte & 0x20 ? '1' : '0'), \
     (byte & 0x10 ? '1' : '0'), \
     (byte & 0x08 ? '1' : '0'), \
     (byte & 0x04 ? '1' : '0'), \
     (byte & 0x02 ? '1' : '0'), \
     (byte & 0x01 ? '1' : '0')
	
    uint8_t reg_value;
    for (int i=GT911_CONFIG_VERSION_REG;i<=GT911_STATUS_BAK_REG;i++)
    {
    	reg_value=TS_IO_Read( i);
    	printf("0x%x=0x%x \r\n",i,reg_value);
    	printf("0x%X=  HEX:[0x%02X] or DEC:[%03d]  or BIN:[0b"BYTE_TO_BINARY_PATTERN"]\r\n", i,reg_value,reg_value,BYTE_TO_BINARY(reg_value));
    	printf("0x%00X,",reg_value);
    }
}

//----------------------------

uint8_t  calcChecksum(uint8_t* buf, uint8_t len) {
  uint8_t ccsum = 0;
  for (uint8_t i = 0; i < len; i++) {
    ccsum += buf[i];
  }
  //ccsum %= 256;
  ccsum = (~ccsum) + 1;
  return ccsum;
}

uint8_t  readChecksum() {
  uint16_t aStart = GT911_CONFIG_VERSION_REG;
  uint16_t aStop = 0x80FE;
  uint8_t len = aStop - aStart + 1;
  uint8_t buf[len];

 // read(aStart, buf, len);
	for(int i=0;i<len;i++)
	buf[i]=TS_IO_Read(GT911_CONFIG_VERSION_REG+i);
	
  return calcChecksum(buf, len);
}

uint8_t  fwResolution(uint16_t maxX, uint16_t maxY)
{
  uint8_t len = GT911_CONFIG_FRESH_REG - GT911_CONFIG_VERSION_REG + 1;
  uint8_t cfg[len];
	
//  read(GT911_CONFIG_VERSION_REG, cfg, len);
	for(int i=0;i<len;i++)
	cfg[i]=TS_IO_Read(GT911_CONFIG_VERSION_REG+i);
	
  cfg[1] = (maxX & 0xff);
  cfg[2] = (maxX >> 8);
  cfg[3] = (maxY & 0xff);
  cfg[4] = (maxY >> 8);
  cfg[len - 2] = calcChecksum(cfg, len - 2);
  cfg[len - 1] = 1;

  //write(GT911_CONFIG_VERSION_REG, cfg, len);
	
	for(int i=0;i<len;i++)
	TS_IO_Write(GT911_CONFIG_VERSION_REG+i, cfg[i]);
}

void readConfig(GTConfig *config)
{
	//read(GT911_CONFIG_VERSION_REG, (uint8_t *) config, sizeof(GTConfig));
	uint8_t buf[sizeof(GTConfig)];
	for (int i = 0; i < sizeof(GTConfig); i++)
		buf[i] = TS_IO_Read(GT911_CONFIG_VERSION_REG + i);
	memcpy((uint8_t*) config, buf, sizeof(GTConfig));
}

void readInfo(GTInfo *infor)
{
//  read(GT911_PRODUCT_ID1_REG, (uint8_t *) infor, sizeof(GTInfo));
	uint8_t buf[sizeof(GTInfo)];
	for (int i = 0; i < sizeof(GTInfo); i++)
		buf[i] = TS_IO_Read(GT911_PRODUCT_ID1_REG + i);
	memcpy((uint8_t*) infor, buf, sizeof(GTInfo));
}

void show_config(void)
{
	GTConfig cfg;
	GTInfo info;
	readConfig(&cfg);
	readInfo(&info) ;
	//cfg.hoppingFlag
	
	printf("PRODUCT ID :GT%s \r\n",(uint8_t *)info.productId);
	printf("framware ID : %X \r\n",info.fwId);
	printf(" xResolution : %d \r\n",info.xResolution);
	printf(" yResolution : %d \r\n",info.yResolution);
	printf(" vendor Id : %d \r\n",info.vendorId);
	printf("configVersion : 0x%X \r\n",cfg.configVersion);
	printf("drvFrameControl : 0x%X \r\n",cfg.drvFrameControl);
	printf("drvGroupANum : 0x%X \r\n",cfg.drvGroupANum);
	printf("drvGroupBNum : 0x%X \r\n",cfg.drvGroupBNum);
	printf("filter : 0x%X \r\n",cfg.filter);
	printf("freqAFactor : 0x%X \r\n",cfg.freqAFactor);
	printf("freqBFactor : 0x%X \r\n",cfg.freqBFactor);
	printf("freqHoppingEnd : 0x%X \r\n",cfg.freqHoppingEnd);
    printf("freqHoppingStart : 0x%X \r\n",cfg.freqHoppingStart);
	printf("hoppingFlag : 0x%X \r\n",cfg.hoppingFlag);
    printf("refreshRate : %d \r\n",cfg.refreshRate);

}

// تنظیم رزولوشن صفحه
void gt911_SetResolution(uint16_t width, uint16_t height) {
    TS_IO_Write(GT911_X_RESOLUTION_L, width & 0xFF);
    TS_IO_Write(GT911_X_RESOLUTION_H, (width >> 8) & 0xFF);
    TS_IO_Write(GT911_Y_RESOLUTION_L, height & 0xFF);
    TS_IO_Write(GT911_Y_RESOLUTION_H, (height >> 8) & 0xFF);
    TS_IO_Write(GT911_CONFIG_FRESH_REG, 0x01); // اعمال تغییرات
}

// تنظیم تعداد نقاط لمسی پشتیبانی شده
void gt911_SetMaxTouchPoints(uint8_t points) {
    uint8_t config = TS_IO_Read(0x804C);
    config = (config & 0xF0) | (points & 0x0F);
    TS_IO_Write(GT911_TOUCH_NUMBER_REG, config);
    TS_IO_Write(GT911_CONFIG_FRESH_REG, 0x01); // اعمال تغییرات
}

// فعال/غیرفعال کردن حالت کلید لمسی
void gt911_EnableTouchKeys(bool enable) {
    uint8_t config = TS_IO_Read(GT911_MODULE_SWITCH1_REG);
    if(enable) config |= 0x01;
    else config &= ~0x01;
    TS_IO_Write(GT911_MODULE_SWITCH1_REG, config);
    TS_IO_Write(GT911_CONFIG_FRESH_REG, 0x01);
}


// خواندن اطلاعات سخت‌افزاری
GTInfo gt911_ReadHardwareInfo(void) {
    GTInfo info;
    info.productId[0] = TS_IO_Read(GT911_PRODUCT_ID1_REG);
    info.productId[1] = TS_IO_Read(GT911_PRODUCT_ID2_REG);
    info.productId[2] = TS_IO_Read(GT911_PRODUCT_ID3_REG);
    info.productId[3] = TS_IO_Read(GT911_PRODUCT_ID4_REG);
    info.fwId = (TS_IO_Read(GT911_FIRMWARE_VER_H) << 8) | TS_IO_Read(GT911_FIRMWARE_VER_L);
    info.xResolution = (TS_IO_Read(GT911_CURRENT_X_RES_H) << 8) | TS_IO_Read(GT911_CURRENT_X_RES_L);
    info.yResolution = (TS_IO_Read(GT911_CURRENT_Y_RES_H) << 8) | TS_IO_Read(GT911_CURRENT_Y_RES_L);
    info.vendorId = TS_IO_Read(GT911_VENDOR_ID_REG);
    return info;
}

// خواندن تمام نقاط لمسی (پشتیبانی چندلمسی)
uint8_t gt911_ReadAllTouchPoints(GTPoint* points, uint8_t maxPoints) {
    uint8_t status = TS_IO_Read(GT911_BUFFER_STATUS_REG);
    uint8_t touchCount = status & 0x0F;

    if(touchCount > maxPoints) touchCount = maxPoints;

    for(uint8_t i = 0; i < touchCount; i++) {
        uint16_t baseAddr = GT911_POINT1_X_L + (i * 8);
        points[i].x = (TS_IO_Read(baseAddr + 1) << 8) | TS_IO_Read(baseAddr);
        points[i].y = (TS_IO_Read(baseAddr + 3) << 8) | TS_IO_Read(baseAddr + 2);
        points[i].area = (TS_IO_Read(baseAddr + 5) << 8) | TS_IO_Read(baseAddr + 4);
        points[i].trackId = TS_IO_Read(baseAddr + 6);
    }

    TS_IO_Write(GT911_BUFFER_STATUS_REG, 0); // ریست وضعیت
    return touchCount;
}


// تنظیم حالت وقفه
void gt911_SetInterruptMode(uint8_t mode) {
    uint8_t config = TS_IO_Read(GT911_MODULE_SWITCH1_REG);
    config = (config & 0xFC) | (mode & 0x03);
    TS_IO_Write(GT911_MODULE_SWITCH1_REG, config);
    TS_IO_Write(GT911_CONFIG_FRESH_REG, 0x01);
}

// خواندن وضعیت وقفه
bool gt911_IsInterruptOccurred(void) {
    return (TS_IO_Read(GT911_BUFFER_STATUS_REG) & 0x80) != 0;
}


// تنظیم نرخ رفرش
void gt911_SetRefreshRate(uint8_t rate) {
    TS_IO_Write(GT911_REFRESH_RATE_REG, rate);
    TS_IO_Write(GT911_CONFIG_FRESH_REG, 0x01);
}

// فعال‌سازی حالت کم‌مصرف
void gt911_EnableLowPowerMode(bool enable) {
    uint8_t config = TS_IO_Read(GT911_MODULE_SWITCH2_REG);
    if(enable) config |= 0x01;
    else config &= ~0x01;
    TS_IO_Write(GT911_MODULE_SWITCH2_REG, config);
    TS_IO_Write(GT911_CONFIG_FRESH_REG, 0x01);
}


// کالیبراسیون خودکار
void gt911_StartCalibration(void) {
    TS_IO_Write(GT911_COMMAND_REG, 0x04);
    vTaskDelay(100);
    TS_IO_Write(GT911_COMMAND_REG, 0x00);
}

// ریست سخت‌افزاری
void gt911_HardReset(void) {
    HAL_GPIO_WritePin(TOUCH_RST_GPIO_Port, TOUCH_RST_Pin, GPIO_PIN_RESET);
    vTaskDelay(100);
    HAL_GPIO_WritePin(TOUCH_RST_GPIO_Port, TOUCH_RST_Pin, GPIO_PIN_SET);
    vTaskDelay(300);
    gt911_Init();
}



/**
  * @brief  تنظیم حساسیت لمسی (نسخه اصلاح شده)
  * @param  touchThreshold: مقدار آستانه لمس (0-255)
  * @param  leaveThreshold: مقدار آستانه ترک (0-255)
  * @retval None
  */
void gt911_SetTouchSensitivity(uint8_t touchThreshold, uint8_t leaveThreshold) {
    uint16_t chksum = 0;

    // خواندن تنظیمات فعلی
    uint8_t current_touch = TS_IO_Read(GT911_SCREEN_TOUCH_LEVEL);
    uint8_t current_leave = TS_IO_Read(GT911_SCREEN_LEAVE_LEVEL);

    if((current_touch != touchThreshold) || (current_leave != leaveThreshold)) {
        // نوشتن مقادیر جدید
        TS_IO_Write(GT911_SCREEN_TOUCH_LEVEL, touchThreshold);
        TS_IO_Write(GT911_SCREEN_LEAVE_LEVEL, leaveThreshold);

        // محاسبه مجدد checksum
        for(uint16_t reg = GT911_CONFIG_VERSION_REG; reg <= GT911_CONFIG_END_REG; reg++) {
            chksum += TS_IO_Read(reg);
        }
        chksum = (~chksum) + 1;

        // به‌روزرسانی checksum و فعال‌سازی تنظیمات
        TS_IO_Write(GT911_CONFIG_CHKSUM_REG, (uint8_t)chksum);
        TS_IO_Write(GT911_CONFIG_FRESH_REG, 0x01);

        // تاخیر برای اعمال تغییرات
        vTaskDelay(100);
    }
}

/**
  * @brief  خواندن تنظیمات حساسیت فعلی (نسخه اصلاح شده)
  * @param  touchThreshold: اشاره‌گر به متغیر ذخیره‌سازی آستانه لمس
  * @param  leaveThreshold: اشاره‌گر به متغیر ذخیره‌سازی آستانه ترک
  * @retval None
  */
void gt911_GetTouchSensitivity(uint8_t *touchThreshold, uint8_t *leaveThreshold) {
    *touchThreshold = TS_IO_Read(GT911_SCREEN_TOUCH_LEVEL);
    *leaveThreshold = TS_IO_Read(GT911_SCREEN_LEAVE_LEVEL);
}




// تابع فعال‌سازی قابلیت‌های لمسی
void GT911_Enable_Touch_Features(void) {
    // فعال‌سازی حالت ژست
    TS_IO_Write(GT911_COMMAND_CHECK_REG, GT911_CMD_ENTER_GESTURE);
    TS_IO_Write(GT911_COMMAND_REG, GT911_CMD_ENTER_GESTURE);

    // تنظیم زمان بی‌اعتباری دابل‌تپ (پیش‌فرض 1.5 ثانیه)
    TS_IO_Write(GT911_GESTURE_CONTROL, 15); // 15 * 100ms = 1.5s

    // فعال‌سازی دابل‌تپ در تمام صفحه
    uint8_t gesture_switch = TS_IO_Read(GT911_GESTURE_SWITCH1);
    gesture_switch |= 0x01; // فعال‌سازی بیت 0
    TS_IO_Write(GT911_GESTURE_SWITCH1, gesture_switch);

    // تنظیم زمان لمس طولانی برای کلیدها (اختیاری)
    TS_IO_Write(GT911_KEY_AREA, 0x0F); // 1.5 ثانیه
}

// تابع پردازش وقفه لمسی
void GT911_Process_Touch_Event(uint32_t current_time) {
    uint8_t buf_status = TS_IO_Read(GT911_BUFFER_STATUS_REG);

    // بررسی آماده‌بودن داده‌ها
    if (!(buf_status & GT911_BUF_STAT_READY)) {
        return;
    }

    // 1. بررسی دابل‌تپ
    uint8_t gesture_type = TS_IO_Read(GT911_GESTURE_TYPE_REG);
    if (gesture_type == GT911_GESTURE_DOUBLE_TAP) {
        handle_double_tap();
        TS_IO_Write(GT911_GESTURE_TYPE_REG, 0x00); // ریست ریجستر
    }

    // 2. پردازش نقاط لمسی
    uint8_t touch_count = buf_status & GT911_BUF_STAT_TOUCH_MASK;
    for (uint8_t i = 0; i < touch_count; i++) {
        uint16_t x = (TS_IO_Read(GT911_POINT1_X_H + i*8) << 8)
                   | TS_IO_Read(GT911_POINT1_X_L + i*8);

        uint16_t y = (TS_IO_Read(GT911_POINT1_Y_H + i*8) << 8)
                   | TS_IO_Read(GT911_POINT1_Y_L + i*8);

        uint8_t track_id = TS_IO_Read(GT911_TRACK_ID_REG + i*8);

        update_touch_point(i, x, y, track_id, current_time);
    }

    // ریست وضعیت بافر
    TS_IO_Write(GT911_BUFFER_STATUS_REG, 0x00);
}

// --- توابع کمکی ---

// پردازش دابل‌تپ
void handle_double_tap(void) {
    // پیاده‌سازی عملیات مورد نیاز
    // مثلاً: بیدار کردن دستگاه، اجرای عملیات خاص
    printf("Double Tap Detected!\n");
}

// به‌روزرسانی وضعیت نقاط لمسی
void update_touch_point(uint8_t index, uint16_t x, uint16_t y, uint8_t track_id, uint32_t current_time) {
    // جستجوی نقطه لمسی براساس track_id
    for (int i = 0; i < 5; i++) {
        if (active_touches[i].track_id == track_id) {
            // بررسی لمس طولانی
            if (active_touches[i].is_active &&
                (current_time - active_touches[i].start_time) > LONG_PRESS_DURATION) {
                handle_long_press(x, y);
                active_touches[i].is_active = false; // جلوگیری از فعال‌سازی مکرر
            }
            return;
        }
    }

    // نقطه لمسی جدید
    for (int i = 0; i < 5; i++) {
        if (!active_touches[i].is_active) {
            active_touches[i].x = x;
            active_touches[i].y = y;
            active_touches[i].track_id = track_id;
            active_touches[i].start_time = current_time;
            active_touches[i].is_active = true;
            break;
        }
    }
}

// پردازش لمس طولانی
void handle_long_press(uint16_t x, uint16_t y) {
    // پیاده‌سازی عملیات مورد نیاز
    printf("Long Press at (%d, %d)\n", x, y);
}

// ریست نقاط لمسی هنگام رها شدن
void GT911_Clear_Touch_Points(void) {
    for (int i = 0; i < 5; i++) {
        active_touches[i].is_active = false;
    }
}


/**
  * @brief  بررسی سلامت سنسور GT911 و وضعیت اتصال
  * @retval کد وضعیت (GT911_Status)
  */
GT911_Status gt911_CheckHealth(void) {
    uint8_t id1, id2, id3, id4;
    uint8_t checksum, calculated_checksum = 0;
    uint16_t chksum = 0;
    uint8_t touch_status;
    uint16_t x, y;

    // 1. بررسی ارتباط I2C
    if (HAL_I2C_IsDeviceReady(&_GT911_I2C, GT911_Write_Address, 3, 100) != HAL_OK) {
        return GT911_I2C_COMM_ERROR;
    }

    // 2. بررسی وجود دستگاه
    if (HAL_I2C_IsDeviceReady(&_GT911_I2C, GT911_Write_Address, 3, 100) != HAL_OK) {
        return GT911_DEVICE_NOT_FOUND;
    }

    // 3. بررسی شناسه محصول
    id1 = TS_IO_Read(GT911_PRODUCT_ID1_REG);
    id2 = TS_IO_Read(GT911_PRODUCT_ID2_REG);
    id3 = TS_IO_Read(GT911_PRODUCT_ID3_REG);
    id4 = TS_IO_Read(GT911_PRODUCT_ID4_REG);

    if (id1 != 0x39 || id2 != 0x31 || id3 != 0x31 || id4 != 0x00) {
        return GT911_ID_MISMATCH;
    }

    // 4. بررسی چکسام کانفیگ
    checksum = TS_IO_Read(GT911_CONFIG_CHKSUM_REG);
    for(uint16_t reg = GT911_CONFIG_VERSION_REG; reg <= GT911_CONFIG_END_REG; reg++) {
        chksum += TS_IO_Read(reg);
    }
    chksum = (~chksum) + 1;
    calculated_checksum = (uint8_t)chksum;

    if (checksum != calculated_checksum) {
        return GT911_CONFIG_CHECKSUM_ERROR;
    }

    // 5. بررسی نسخه فرمور
    uint8_t fw_ver_low = TS_IO_Read(GT911_FIRMWARE_VER_L);
    uint8_t fw_ver_high = TS_IO_Read(GT911_FIRMWARE_VER_H);
    uint16_t fw_version = (fw_ver_high << 8) | fw_ver_low;

    if (fw_version == 0x0000 || fw_version == 0xFFFF) {
        return GT911_FIRMWARE_VERSION_ERROR;
    }

    // 6. بررسی شناسه فروشنده
    uint8_t vendor_id = TS_IO_Read(GT911_VENDOR_ID_REG);
    if (vendor_id != GT911_VENDOR_ID_VALUE) {
        return GT911_VENDOR_ID_ERROR;
    }

    // 7. بررسی وضعیت سنسور
    touch_status = TS_IO_Read(GT911_STATUS_REG);
    if (touch_status == 0xFF) { // مقدار نامعتبر
        return GT911_SENSOR_INIT_ERROR;
    }

    // 8. تست عملکرد لمسی
    TS_IO_Write(GT911_STATUS_REG, 0x00); // ریست وضعیت
    uint8_t touchCount = gt911_TS_DetectTouch(&x, &y);

    // اگر سنسور مدام در حالت لمس باشد
    if (touchCount > 0 && (x == 0 || y == 0 || x == 0x0FFF || y == 0x0FFF)) {
        return GT911_TOUCH_DATA_ERROR;
    }

    return GT911_OK;
}


/**
  * @brief  دریافت توضیح خطا به صورت متن
  * @param  status: کد خطا
  * @retval رشته توصیف خطا
  */
const char* gt911_GetErrorString(GT911_Status status) {
    switch(status) {
        case GT911_OK:
            return "No error";
        case GT911_I2C_COMM_ERROR:
            return "I2C communication error";
        case GT911_DEVICE_NOT_FOUND:
            return "Device not responding";
        case GT911_ID_MISMATCH:
            return "Invalid product ID";
        case GT911_CONFIG_CHECKSUM_ERROR:
            return "Configuration checksum error";
        case GT911_FIRMWARE_VERSION_ERROR:
            return "Invalid firmware version";
        case GT911_VENDOR_ID_ERROR:
            return "Invalid vendor ID";
        case GT911_SENSOR_INIT_ERROR:
            return "Sensor initialization error";
        case GT911_TOUCH_DATA_ERROR:
            return "Invalid touch data";
        default:
            return "Unknown error";
    }
}


void CheckTouchSensorHealth(void) {
    GT911_Status status = gt911_CheckHealth();

    if (status != GT911_OK) {
        const char* errorMsg = gt911_GetErrorString(status);
        printf("Touch Sensor Error: %s\n", errorMsg);

        // اقدامات اصلاحی
        switch(status) {
            case GT911_CONFIG_CHECKSUM_ERROR:
                printf("Recalibrating touch sensor...\r\n");
//                gt911_Reset();
                gt911_HardReset();
                gt911_Init();
                gt911_TS_Start();
                break;

            case GT911_DEVICE_NOT_FOUND:
            case GT911_I2C_COMM_ERROR:
                printf("Checking I2C connections...\r\n");
                // ریست سخت‌افزاری
                HAL_GPIO_WritePin(TOUCH_RST_GPIO_Port, TOUCH_RST_Pin, GPIO_PIN_RESET);
                HAL_Delay(100);
                HAL_GPIO_WritePin(TOUCH_RST_GPIO_Port, TOUCH_RST_Pin, GPIO_PIN_SET);
                HAL_Delay(300);
                gt911_Init();
                gt911_TS_Start();
                break;

            default:
                // سایر خطاها نیاز به اقدامات خاص دارند
                printf("Critical error, please restart system\r\n");
                break;
        }
    } else {
        printf("Touch sensor is healthy\r\n");
    }
}
