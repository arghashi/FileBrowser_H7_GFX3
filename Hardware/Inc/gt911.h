/**
 ******************************************************************************
 * @file    gt911.h
 * @author  Joao Costa - costasjf@gmail.com
 * @brief   This file contains all the functions prototypes for the
 *          gt911.c Touch screen driver.
 ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GT911_H
#define __GT911_H

#ifdef __cplusplus
extern "C" {
#endif

/************ INCLUDES ************/
#include "main.h"
#include "stm32h7xx_hal.h"
#include "stm32h7xx.h"
#include "stdbool.h"


#define TOUCH_INT_Port       GPIOH
#define TOUCH_INT_Pin        GPIO_PIN_7
#define TOUCH_INT_Clock()    __HAL_RCC_GPIOH_CLK_ENABLE()

#define TOUCH_RST_Port       GPIOI
#define TOUCH_RST_Pin        GPIO_PIN_8
#define TOUCH_RST_Clock()    __HAL_RCC_GPIOI_CLK_ENABLE()

extern	I2C_HandleTypeDef hi2c1;

#define	 _GT911_I2C       hi2c1

//extern void MX_I2C3_Init();

//static void attachInterrupt(GPIO_TypeDef *PORTx,uint8_t pin);

//static uint8_t TP_START_FLAG=0;



/************ DEFINITIONS ************/
#define GT911_ADDR 					0XBA
#define GT911_Read_Address        	0x29 //0xbb//
#define GT911_Write_Address       	0x28 //0xba//


#define SENS_LOW     40   // حساسیت کم (فشار بیشتر نیاز دارد)
#define SENS_MEDIUM  80   // حساسیت متوسط
#define SENS_HIGH    120  // حساسیت بالا


/* GT911 registers used in this driver: */
#define GT911_CONFIG_END_REG        	0x80FE  // پایان محدوده پیکربندی

/* GT911 definitions used in this driver: */
#define GT911_STATUS_OK					((uint8_t)0x00)			//:-)
#define GT911_STATUS_NOT_OK				((uint8_t)0x01)			//There's something wrong! :-(
#define GT911_I2C_NOT_INITIALIZED		((uint8_t)0x00)			//I2C not initiated
#define GT911_I2C_INITIALIZED			((uint8_t)0x01)			//I2C already initiated
#define GT911_4LSB_MASK					((uint8_t)0x0F)			//Mask for four last bit use only
#define GT911_TP_MAX_WIDTH				((uint16_t)800)			//Touchscreen pad max width
#define GT911_TP_MAX_HEIGHT				((uint16_t)480)			//Touchscreen pad max height
#define GT911_TOUCH_POS_MSB_MASK		((uint8_t)0x0F)
#define GT911_TOUCH_POS_LSB_MASK		((uint8_t)0xFF)
#define GT911_TOUCH_POS_MSB_SHIFT		((uint8_t)0x00)
#define GT911_TOUCH_POS_LSB_SHIFT		((uint8_t)0x00)
#define GT911_VENDOR_ID_VALUE			((uint8_t)0x00)

/* Real-time Command Registers (Write Only) */
#define GT911_COMMAND_REG          ((uint16_t)0x8040)
#define GT911_ESD_CHECK_REG        ((uint16_t)0x8041)
#define GT911_COMMAND_CHECK_REG    ((uint16_t)0x8046)

/* Configuration Registers (R/W) */
#define GT911_CONFIG_VERSION_REG   ((uint16_t)0x8047)
#define GT911_X_RESOLUTION_L       ((uint16_t)0x8048)
#define GT911_X_RESOLUTION_H       ((uint16_t)0x8049)
#define GT911_Y_RESOLUTION_L       ((uint16_t)0x804A)
#define GT911_Y_RESOLUTION_H       ((uint16_t)0x804B)
#define GT911_TOUCH_NUMBER_REG     ((uint16_t)0x804C)
#define GT911_MODULE_SWITCH1_REG   ((uint16_t)0x804D)
#define GT911_MODULE_SWITCH2_REG   ((uint16_t)0x804E)
#define GT911_SHAKE_COUNT_REG      ((uint16_t)0x804F)
#define GT911_FILTER_REG           ((uint16_t)0x8050)
#define GT911_LARGE_TOUCH_REG      ((uint16_t)0x8051)
#define GT911_NOISE_REDUCTION_REG  ((uint16_t)0x8052)
#define GT911_SCREEN_TOUCH_LEVEL   ((uint16_t)0x8053)
#define GT911_SCREEN_LEAVE_LEVEL   ((uint16_t)0x8054)
#define GT911_LOW_POWER_CTRL_REG   ((uint16_t)0x8055)
#define GT911_REFRESH_RATE_REG     ((uint16_t)0x8056)
#define GT911_X_THRESHOLD_REG      ((uint16_t)0x8057)
#define GT911_Y_THRESHOLD_REG      ((uint16_t)0x8058)
#define GT911_CONFIG_CHKSUM_REG    ((uint16_t)0x80FF)
#define GT911_CONFIG_FRESH_REG     ((uint16_t)0x8100)

/* Border Space Configuration */
#define GT911_BORDER_SPACE_TOP      0x805B
#define GT911_BORDER_SPACE_BOTTOM   0x805B
#define GT911_BORDER_SPACE_LEFT     0x805C
#define GT911_BORDER_SPACE_RIGHT    0x805C

/* Stretch Configuration */
#define GT911_STRETCH_R0            0x805E
#define GT911_STRETCH_R1            0x805F
#define GT911_STRETCH_R2            0x8060
#define GT911_STRETCH_RM            0x8061

/* Driver/Sensor Configuration */
#define GT911_DRIVER_GROUP_A_NUM    0x8062
#define GT911_DRIVER_GROUP_B_NUM    0x8063
#define GT911_SENSOR_NUM            0x8064
#define GT911_FREQ_A_FACTOR         0x8065
#define GT911_FREQ_B_FACTOR         0x8066
#define GT911_PANEL_BITFREQ_L       0x8067
#define GT911_PANEL_BITFREQ_H       0x8068
#define GT911_PANEL_TX_GAIN         0x806B
#define GT911_PANEL_RX_GAIN         0x806C
#define GT911_PANEL_DUMP_SHIFT      0x806D

/* Gesture Configuration */
#define GT911_GESTURE_DIS           0x8071
#define GT911_GESTURE_LONG_PRESS    0x8072
#define GT911_XY_SLOPE_ADJUST       0x8073
#define GT911_GESTURE_CONTROL       0x8074
#define GT911_GESTURE_SWITCH1       0x8075
#define GT911_GESTURE_SWITCH2       0x8076
#define GT911_GESTURE_REFRESH_RATE  0x8077
#define GT911_GESTURE_TOUCH_LEVEL   0x8078

/* Frequency Hopping */
#define GT911_FREQ_HOPPING_START    0x807A
#define GT911_FREQ_HOPPING_END      0x807B
#define GT911_NOISE_DETECT_TIMES    0x807C
#define GT911_HOPPING_FLAG          0x807D
#define GT911_HOPPING_THRESHOLD     0x807E
#define GT911_NOISE_THRESHOLD       0x807F
#define GT911_NOISE_MIN_THRESHOLD   0x8080
#define GT911_HOPPING_SENSOR_GROUP  0x8082

/* Touch Key Configuration */
#define GT911_KEY1_ADDRESS          0x8093
#define GT911_KEY2_ADDRESS          0x8094
#define GT911_KEY3_ADDRESS          0x8095
#define GT911_KEY4_ADDRESS          0x8096
#define GT911_KEY_AREA              0x8097
#define GT911_KEY_TOUCH_LEVEL       0x8098
#define GT911_KEY_LEAVE_LEVEL       0x8099
#define GT911_KEY_SENS12            0x809A
#define GT911_KEY_SENS34            0x809B
#define GT911_KEY_RESTRAIN          0x809C

/* Coordinate Information Registers (Read Only) */
#define GT911_PRODUCT_ID1_REG      0x8140
#define GT911_PRODUCT_ID2_REG      0x8141
#define GT911_PRODUCT_ID3_REG      0x8142
#define GT911_PRODUCT_ID4_REG      0x8143
#define GT911_FIRMWARE_VER_L       0x8144
#define GT911_FIRMWARE_VER_H       0x8145
#define GT911_CURRENT_X_RES_L      0x8146
#define GT911_CURRENT_X_RES_H      0x8147
#define GT911_CURRENT_Y_RES_L      0x8148
#define GT911_CURRENT_Y_RES_H      0x8149
#define GT911_VENDOR_ID_REG        0x814A

#define GT911_GESTURE_TYPE_REG     0x814B

#define GT911_BUFFER_STATUS_REG    0x814E
#define GT911_TRACK_ID_REG         0x814F
#define GT911_POINT1_X_L           0x8150
#define GT911_POINT1_X_H           0x8151
#define GT911_POINT1_Y_L           0x8152
#define GT911_POINT1_Y_H           0x8153
#define GT911_POINT1_SIZE_L        0x8154
#define GT911_POINT1_SIZE_H        0x8155

/* Command Status Registers */
#define GT911_STATUS_REG           0x81A8
#define GT911_STATUS_BAK_REG       0x81A9

/* HotKnot Registers */
#define GT911_HOTKNOT_SEND_STATUS  0xAB10
#define GT911_HOTKNOT_RECV_STATUS  0xAB11
#define GT911_HOTKNOT_TX_BUF_LEN   0xAC90
#define GT911_HOTKNOT_TX_BUF_DATA  0xAC91
#define GT911_HOTKNOT_TX_BUF_FRESH 0xAD91
#define GT911_HOTKNOT_RX_BUF_STAT  0xAE10
#define GT911_HOTKNOT_RX_BUF_LEN   0xAE11

/* Bit Masks for Critical Registers */
/* Buffer Status Register GT911_BUFFER_STATUS_REG (0x814E) */
#define GT911_BUF_STAT_READY       (1 << 7)
#define GT911_BUF_STAT_LARGE_TOUCH (1 << 6)
#define GT911_BUF_STAT_PROX_VALID  (1 << 5)
#define GT911_BUF_STAT_HAVE_KEY    (1 << 4)
#define GT911_BUF_STAT_TOUCH_MASK  0x0F

/* Module Switch1 Register GT911_MODULE_SWITCH1_REG (0x804D) */
#define GT911_SW1_Y_REV            (1 << 7)
#define GT911_SW1_X_REV            (1 << 6)
#define GT911_SW1_INT_TRIG_MASK    0x0C  // Bits 3-2

/* Module Switch2 Register GT911_MODULE_SWITCH2_REG (0x804E) */
#define GT911_SW2_APPROACH_EN      (1 << 2)
#define GT911_SW2_HOTKNOT_EN       (1 << 1)

/* Gesture Coordinates Base Address */
#define GT911_GESTURE_COORD_BASE    0x9420

/* ماکروی دسترسی به مختصات ژست */
#define GT911_GESTURE_POINT_X_LOW(n)   (0x9420 + 4*(n))
#define GT911_GESTURE_POINT_Y_LOW(n)   (0x9420 + 4*(n) + 1)
#define GT911_GESTURE_POINT_X_HIGH(n)  (0x9420 + 4*(n) + 2)
#define GT911_GESTURE_POINT_Y_HIGH(n)  (0x9420 + 4*(n) + 3)

/* مثال: دسترسی به مختصات نقطه ۵ */
#define GT911_GEST_P5_X_LOW   GT911_GESTURE_POINT_X_LOW(4)
#define GT911_GEST_P5_Y_HIGH  GT911_GESTURE_POINT_Y_HIGH(4)

/* HotKnot Transmit Buffer */
#define GT911_HK_TX_DATA_LENGTH     0xAC90
#define GT911_HK_TX_BUFFER_BASE     0xAC91

/* ماکروی دسترسی به داده‌های بافر */
#define GT911_HK_TX_DATA(n)         (0xAC91 + (n))

#define GT911_HK_TX_CHECKSUM        0xAD11  /* آدرس متغیر بر اساس طول داده */
#define GT911_HK_TX_DATA_FRESH      0xAD91

/* Command Values */
#define GT911_CMD_READ_COORDS      0x00
#define GT911_CMD_SCREEN_OFF       0x05
#define GT911_CMD_ENTER_GESTURE    0x08

#define GT911_GESTURE_DOUBLE_TAP   0xCC

#define GT911_CMD_HOTKNOT_SLAVE    0x20
#define GT911_CMD_HOTKNOT_MASTER   0x21
#define GT911_CMD_ESD_ENABLE       0xAA

/* Status Values GT911_STATUS_REG (0x81A8) */
#define GT911_STAT_NORMAL           0x00
#define GT911_STAT_SLAVE_APPROACH   0x88
#define GT911_STAT_MASTER_APPROACH  0x99
#define GT911_STAT_RECEIVE_MODE     0xAA
#define GT911_STAT_SEND_MODE        0xBB



/* Add these error codes */
typedef enum {
    GT911_OK = 0,                   // بدون خطا
    GT911_I2C_COMM_ERROR,           // خطا در ارتباط I2C
    GT911_DEVICE_NOT_FOUND,         // دستگاه پاسخ نمی‌دهد
    GT911_ID_MISMATCH,              // شناسه محصول نامعتبر
    GT911_CONFIG_CHECKSUM_ERROR,    // خطای چکسام کانفیگ
    GT911_FIRMWARE_VERSION_ERROR,   // نسخه فرمور نامعتبر
    GT911_VENDOR_ID_ERROR,          // شناسه فروشنده نامعتبر
    GT911_SENSOR_INIT_ERROR,        // خطا در مقداردهی اولیه سنسور
    GT911_TOUCH_DATA_ERROR          // خطا در داده‌های لمسی
} GT911_Status;


typedef struct //GT911 Handle definition type definition
{
	uint8_t i2cInitialized;		//I2C already initialized?
	uint8_t currActiveTouchNb;	//Current number of simultaneous active touches
} gt911_handle_TypeDef;



typedef struct {
	void       (*Init)(uint16_t);
	uint8_t   (*ReadID)(uint16_t);
	void       (*Start)(uint16_t);
	uint8_t    (*DetectTouch)(uint16_t, uint16_t*, uint16_t*);
}TS_DrvTypeDef;

typedef struct
{
  uint16_t TouchDetected;
  uint16_t x;
  uint16_t y;
  uint16_t z;
}TS_StateTypeDef;


typedef struct {
    uint16_t x;
    uint16_t y;
    uint8_t track_id;
    uint32_t start_time;
    bool is_active;
} GT911_TouchPoint;

//static TS_StateTypeDef TS_State;


//---------------transfer from ardu----------------------------

//static uint8_t goodix_config[] = {
//
//        0x42,
////        0xD0, 0x02, 0x00, 0x05, 0x05, 0x75, 0x01, 0x01, 0x0F, 0x24,
////        0x0F, 0x64, 0x3C, 0x03, 0x05, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
////        0x00, 0x16, 0x19, 0x1C, 0x14, 0x8C, 0x0E, 0x0E, 0x24, 0x00, 0x31,
////        0x0D, 0x00, 0x00, 0x00, 0x83, 0x33, 0x1D, 0x00, 0x41, 0x00, 0x00,
////        0x00, 0x00, 0x00, 0x08, 0x0A, 0x00, 0x2B, 0x1C, 0x3C, 0x94, 0xD5,
////        0x03, 0x08, 0x00, 0x00, 0x04, 0x93, 0x1E, 0x00, 0x82, 0x23, 0x00,
////        0x74, 0x29, 0x00, 0x69, 0x2F, 0x00, 0x5F, 0x37, 0x00, 0x5F, 0x20,
////        0x40, 0x60, 0x00, 0xF0, 0x40, 0x30, 0x55, 0x50, 0x27, 0x00, 0x00,
////        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
////        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x19, 0x00, 0x00,
////        0x50, 0x50, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E, 0x10, 0x12,
////        0x14, 0x16, 0x18, 0x1A, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
////        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1D,
////        0x1E, 0x1F, 0x20, 0x21, 0x22, 0x24, 0x26, 0x28, 0x29, 0x2A, 0x1C,
////        0x18, 0x16, 0x14, 0x13, 0x12, 0x10, 0x0F, 0x0C, 0x0A, 0x08, 0x06,
////        0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
////        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9C, 0x01};
//
//0x41,0x2A,0x03,0xEA,0x01,0x01,0x3D,0x00,0x01,0x08,
//0x28,0x08,0x28,0x32,0x03,0x0B,0x00,0x00,0x00,0x00,
//0x00,0x00,0x00,0x10,0x11,0x14,0x0F,0x89,0x2A,0x09,
//0x4B,0x50,0xB5,0x06,0x00,0x00,0x00,0x00,0x02,0x1D,
//0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//0x00,0x3C,0x78,0x94,0xC5,0x02,0x07,0x00,0x00,0x04,
//0x90,0x40,0x00,0x81,0x4A,0x00,0x75,0x55,0x00,0x6B,
//0x61,0x00,0x62,0x70,0x00,0x62,0x00,0x00,0x00,0x00,
//0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//0x00,0x00,0x02,0x04,0x06,0x08,0x0A,0x0C,0x10,0x12,
//0x14,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,
//0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//0x00,0x00,0x00,0x02,0x04,0x06,0x08,0x0A,0x0F,0x10,
//0x12,0x16,0x18,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,
//0x24,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
//0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//0x00,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x00,0x00,
//0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//0x00,0x00,0x00};


//-----------------------------------------------






 typedef  struct  {
  // 0x8140-0x814A 
  char productId[4];
  uint16_t fwId;
  uint16_t xResolution;
  uint16_t yResolution;
  uint8_t vendorId;
}GTInfo ;

typedef struct  {
  // 0x814F-0x8156, ... 0x8176 (5 points) 
  uint8_t trackId;
  uint16_t x;
  uint16_t y;
  uint16_t area;
  uint8_t reserved;
}GTPoint ;

typedef struct  {
  uint8_t touch; // Threshold of touch grow out of nothing
  uint8_t leave; // Threshold of touch decrease to nothing
}GTLevelConfig;

typedef struct  {
  uint8_t txGain;
  uint8_t rxGain;
  uint8_t dumpShift;
  GTLevelConfig level;
  uint8_t control;     //Pen mode escape time out period (Unit: Sec)
}GTStylusConfig ;

typedef struct  {
  uint16_t hoppingBitFreq;
  uint8_t hoppingFactor;
}GTFreqHoppingConfig ;

typedef struct  {
  // Key position: 0-255 valid 
  // 0 means no touch, it means independent touch key when 4 of the keys are 8 multiples
  uint8_t pos1;
  uint8_t pos2;
  uint8_t pos3;
  uint8_t pos4;
  uint8_t area;
  GTLevelConfig level;
  uint8_t sens12;
  uint8_t sens34;
  uint8_t restrain;
}GTKeyConfig ;

typedef struct  {
  // start at 0x8047
  uint8_t configVersion;
  uint16_t xResolution;
  uint16_t yResolution;  
  // 0x804C
  uint8_t touchNumber;   // 3:0 Touch No.: 1~10
  
  // 7:6 Reserved, 5:4 Stretch rank, 3 X2Y, 2 Sito
  // 1:0 INT trig method: 00-rising, 01-falling, 02-low level, 03-high level enquiry
  uint8_t moduleSwitch1;   
  uint8_t moduleSwitch2;  // bit0 TouchKey
  uint8_t shakeCount;     // 3:0 Finger shake count
  // 0x8050
  // 7:6 First filter, 5:0 Normal filter (filtering value of original coordinate window, coefficiency is 1)
  uint8_t filter;
  uint8_t largeTouch;
  uint8_t noiseReduction;
  GTLevelConfig screenLevel;

  uint8_t lowPowerControl;  // Time to low power consumption (0~15s)
  uint8_t refreshRate;      // Coordinate report rate (Cycle: 5+N ms)
  uint8_t xThreshold; //res
  uint8_t yThreshold; //res
  uint8_t xSpeedLimit; //res
  uint8_t ySpeedLimit; //res
  uint8_t vSpace; // 4bit top/bottom  (coefficient 32)
  uint8_t hSpace; // 4bit left/right 
  //0x805D-0x8061
  uint8_t stretchRate; //Level of weak stretch (Strtch X/16 Pitch)
  uint8_t stretchR0;   // Interval 1 coefficient
  uint8_t stretchR1;   // Interval 2 coefficient
  uint8_t stretchR2;   // Interval 3 coefficient
  uint8_t stretchRM;   // All intervals base number
   
  uint8_t drvGroupANum;
  uint8_t drvGroupBNum;
  uint8_t sensorNum;
  uint8_t freqAFactor;
  uint8_t freqBFactor;
  // 0x8067
  uint16_t pannelBitFreq;      //Baseband of Driver group A\B (1526HZ<baseband<14600Hz)
  uint16_t pannelSensorTime;  //res
  uint8_t pannelTxGain;
  uint8_t pannelRxGain;
  uint8_t pannelDumpShift;
  uint8_t drvFrameControl;
  // 0x806F - 0x8071
  uint8_t NC_2[3];
  GTStylusConfig stylusConfig;
  // 0x8078-0x8079
  uint8_t NC_3[2];
  uint8_t freqHoppingStart; // Frequency hopping start frequency (Unit: 2KHz, 50 means 100KHz )
  uint8_t freqHoppingEnd;   // Frequency hopping stop frequency (Unit: 2KHz, 150 means 300KHz )
  uint8_t noiseDetectTims; 
  uint8_t hoppingFlag;
  uint8_t hoppingThreshold;  

  uint8_t noiseThreshold;  
  uint8_t NC_4[2];
  // 0x8082
  GTFreqHoppingConfig hoppingSegments[5];
  // 0x8091
  uint8_t NC_5[2];
  GTKeyConfig keys;
  
  
}GTConfig ;


bool Gt911_isConnected(void);

uint8_t TS_IO_Read( uint16_t Reg)	;	
void TS_IO_Write(uint16_t Reg, uint8_t Value);
void show_config(void);
void  readInfo(GTInfo *infor);
uint8_t  fwResolution(uint16_t maxX, uint16_t maxY);

/************ PUBLIC FUNCTIONS PROTOTYPES ************/

/**
 * @brief  Initialize the gt911 communication bus
 *         from MCU to GT911 : ie I2C channel initialization (if required).
 * @param  DeviceAddr: Device address on communication Bus (I2C slave address of GT911).
 * @retval None
 */
void gt911_Init(void);

/**
 * @brief  Software Reset the gt911.
 * @param  DeviceAddr: Device address on communication Bus (I2C slave address of GT911).
 * @retval None
 */
void gt911_Reset(void);

/**
 * @brief  Read the gt911 device ID, pre initialize I2C in case of need to be
 *         able to read the GT911 device ID, and verify this is a GT911.
 * @param  DeviceAddr: I2C GT911 Slave address.
 * @retval The Device ID (two bytes).
 */
uint8_t gt911_ReadID(void);

/**
 * @brief  Configures the touch Screen IC device to start detecting touches
 * @param  DeviceAddr: Device address on communication Bus (I2C slave address).
 * @retval None.
 */
void gt911_TS_Start(void);

/**
 * @brief  Return if there is touches detected or not.
 *         Try to detect new touches and forget the old ones (reset internal global
 *         variables).
 * @param  DeviceAddr: Device address on communication Bus.
 * @retval : Number of active touches detected (can be 0, 1 or 2).
 */
uint8_t gt911_TS_DetectTouch( uint16_t *X, uint16_t *Y);


/************ IMPORTED ************/

//Imported TouchScreen (TS) external IO functions:
//extern void    	TS_IO_Init(void);
//extern void    	TS_IO_Write(uint8_t Addr, uint16_t Reg, uint8_t Value);
//extern uint8_t	TS_IO_Read(uint8_t Addr, uint16_t Reg);
//extern void    	TS_IO_Delay(uint32_t Delay);





//Imported global touch variables screen driver structure
extern TS_DrvTypeDef gt911_ts_drv;



void print_details(void);
uint8_t  calcChecksum(uint8_t* buf, uint8_t len);
uint8_t  readChecksum();
uint8_t  fwResolution(uint16_t maxX, uint16_t maxY);
void readConfig(GTConfig *config );
void  readInfo(GTInfo *infor);
void show_config(void);
void gt911_SetResolution(uint16_t width, uint16_t height);
void gt911_SetMaxTouchPoints(uint8_t points);
void gt911_EnableTouchKeys(bool enable);
GTInfo gt911_ReadHardwareInfo(void);
uint8_t gt911_ReadAllTouchPoints(GTPoint* points, uint8_t maxPoints);
void gt911_SetInterruptMode(uint8_t mode);
bool gt911_IsInterruptOccurred(void);
void gt911_SetRefreshRate(uint8_t rate);
void gt911_EnableLowPowerMode(bool enable);
void gt911_StartCalibration(void);
void gt911_HardReset(void);
void gt911_SetTouchSensitivity(uint8_t touchThreshold, uint8_t leaveThreshold);
void gt911_GetTouchSensitivity(uint8_t *touchThreshold, uint8_t *leaveThreshold);

void GT911_Enable_Touch_Features(void);
void GT911_Process_Touch_Event(uint32_t current_time);
void handle_double_tap(void);
void update_touch_point(uint8_t index, uint16_t x, uint16_t y, uint8_t track_id, uint32_t current_time);
void handle_long_press(uint16_t x, uint16_t y);
void GT911_Clear_Touch_Points(void);

GT911_Status gt911_CheckHealth(void);
void CheckTouchSensorHealth(void);

#ifdef __cplusplus
}
#endif
#endif /* __GT911_H */
