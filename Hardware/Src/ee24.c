//#include "i2c.h"
#include "math.h"
#include "string.h"
#include "main.h"
#include "ee24.h"
#include "ee24Config.h"
#if (_EEPROM_USE_FREERTOS == 1)
#include "cmsis_os.h"
#define ee24_delay(x)   osDelay(x)
#else
#define ee24_delay(x)   HAL_Delay(x)
#endif

#if (_EEPROM_SIZE_KBIT == 1) || (_EEPROM_SIZE_KBIT == 2)
#define _EEPROM_PSIZE     8
#elif (_EEPROM_SIZE_KBIT == 4) || (_EEPROM_SIZE_KBIT == 8) || (_EEPROM_SIZE_KBIT == 16)
#define _EEPROM_PSIZE     16
#else
#define _EEPROM_PSIZE     32
#endif

uint8_t ee24_lock = 0;
uint8_t bytes_temp[4];
//################################################################################################################
bool ee24_isConnected(void)
{
#if (_EEPROM_USE_WP_PIN==1)
  HAL_GPIO_WritePin(_EEPROM_WP_GPIO,_EEPROM_WP_PIN,GPIO_PIN_SET);
  #endif
	if (HAL_I2C_IsDeviceReady(&_EEPROM_I2C, _EEPROM_ADDRESS, 10, 1000)
			== HAL_OK)
		return true;
	else
		return false;
}

void AT24CXX_WriteOneByte(uint16_t WriteAddr, uint8_t DataToWrite)
{
	ee24_write(WriteAddr, &DataToWrite, 1, 1000);
}

void AT24CXX_WriteLenByte(uint16_t WriteAddr, uint32_t DataToWrite, uint8_t Len)
{
	uint8_t t;
	for (t = 0; t < Len; t++)
	{
		AT24CXX_WriteOneByte(WriteAddr + t, (DataToWrite >> (8 * t)) & 0xff);
	}
}

uint8_t AT24CXX_ReadOneByte(uint16_t ReadAddr)
{
	uint8_t temp = 0;
	ee24_read(ReadAddr, &temp, 1, 1000);

	return temp;
}

uint32_t AT24CXX_ReadLenByte(uint16_t ReadAddr, uint8_t Len)
{
	uint8_t t;
	uint32_t temp = 0;
	for (t = 0; t < Len; t++)
	{
		temp <<= 8;
		temp += AT24CXX_ReadOneByte(ReadAddr + Len - t - 1);
	}
	return temp;
}
//################################################################################################################
bool ee24_write(uint16_t address, uint8_t *data, size_t len, uint32_t timeout)
{
	if (ee24_lock == 1)
		return false;
	ee24_lock = 1;
	uint16_t w;
	uint32_t startTime = HAL_GetTick();
#if	(_EEPROM_USE_WP_PIN==1)
  HAL_GPIO_WritePin(_EEPROM_WP_GPIO, _EEPROM_WP_PIN,GPIO_PIN_RESET);
  #endif
	while (1)
	{
		w = _EEPROM_PSIZE - (address % _EEPROM_PSIZE);
		if (w > len)
			w = len;
#if ((_EEPROM_SIZE_KBIT==1) || (_EEPROM_SIZE_KBIT==2))
    if (HAL_I2C_Mem_Write(&_EEPROM_I2C, _EEPROM_ADDRESS, address, I2C_MEMADD_SIZE_8BIT, data, w, 100) == HAL_OK)

//    	if(HAL_I2C_Mem_Write_DMA(&_EEPROM_I2C, _EEPROM_ADDRESS, address, I2C_MEMADD_SIZE_8BIT, data, w) == HAL_OK)

    #elif (_EEPROM_SIZE_KBIT==4)
    if (HAL_I2C_Mem_Write(&_EEPROM_I2C, _EEPROM_ADDRESS | ((address & 0x0100) >> 7), (address & 0xff), I2C_MEMADD_SIZE_8BIT, data, w, 100) == HAL_OK)
    #elif (_EEPROM_SIZE_KBIT==8)
    if (HAL_I2C_Mem_Write(&_EEPROM_I2C, _EEPROM_ADDRESS | ((address & 0x0300) >> 7), (address & 0xff), I2C_MEMADD_SIZE_8BIT, data, w, 100) == HAL_OK)
    #elif (_EEPROM_SIZE_KBIT==16)
    if (HAL_I2C_Mem_Write(&_EEPROM_I2C, _EEPROM_ADDRESS | ((address & 0x0700) >> 7), (address & 0xff), I2C_MEMADD_SIZE_8BIT, data, w, 100) == HAL_OK)		
    #else
		if (HAL_I2C_Mem_Write(&_EEPROM_I2C, _EEPROM_ADDRESS, address, I2C_MEMADD_SIZE_16BIT, data, w, 100) == HAL_OK)
#endif
		{
			ee24_delay(10);
			len -= w;
			data += w;
			address += w;
			if (len == 0)
			{
#if (_EEPROM_USE_WP_PIN==1)
        HAL_GPIO_WritePin(_EEPROM_WP_GPIO, _EEPROM_WP_PIN, GPIO_PIN_SET);
        #endif
				ee24_lock = 0;
				return true;
			}
			if (HAL_GetTick() - startTime >= timeout)
			{
				ee24_lock = 0;
				return false;
			}
		}
		else
		{
#if (_EEPROM_USE_WP_PIN==1)
      HAL_GPIO_WritePin(_EEPROM_WP_GPIO, _EEPROM_WP_PIN, GPIO_PIN_SET);
      #endif
			ee24_lock = 0;
			return false;
		}
	}
}
//################################################################################################################
bool ee24_read(uint16_t address, uint8_t *data, size_t len, uint32_t timeout)
{
	if (ee24_lock == 1)
		return false;
	ee24_lock = 1;
#if (_EEPROM_USE_WP_PIN==1)
  HAL_GPIO_WritePin(_EEPROM_WP_GPIO, _EEPROM_WP_PIN, GPIO_PIN_SET);
  #endif
#if ((_EEPROM_SIZE_KBIT==1) || (_EEPROM_SIZE_KBIT==2))
  if (HAL_I2C_Mem_Read(&_EEPROM_I2C, _EEPROM_ADDRESS, address, I2C_MEMADD_SIZE_8BIT, data, len, 100) == HAL_OK)

//	  if(HAL_I2C_Mem_Read_DMA(&_EEPROM_I2C, 0xA1, address, I2C_MEMADD_SIZE_8BIT, data, len) == HAL_OK)

  #elif (_EEPROM_SIZE_KBIT == 4)
  if (HAL_I2C_Mem_Read(&_EEPROM_I2C, _EEPROM_ADDRESS | ((address & 0x0100) >> 7), (address & 0xff), I2C_MEMADD_SIZE_8BIT, data, len, 100) == HAL_OK)
  #elif (_EEPROM_SIZE_KBIT == 8)
  if (HAL_I2C_Mem_Read(&_EEPROM_I2C, _EEPROM_ADDRESS | ((address & 0x0300) >> 7), (address & 0xff), I2C_MEMADD_SIZE_8BIT, data, len, 100) == HAL_OK)
  #elif (_EEPROM_SIZE_KBIT==16)
  if (HAL_I2C_Mem_Read(&_EEPROM_I2C, _EEPROM_ADDRESS | ((address & 0x0700) >> 7), (address & 0xff), I2C_MEMADD_SIZE_8BIT, data, len, 100) == HAL_OK)
  #else
	if (HAL_I2C_Mem_Read(&_EEPROM_I2C, _EEPROM_ADDRESS, address,
			I2C_MEMADD_SIZE_16BIT, data, len, timeout) == HAL_OK)
#endif
	{
		ee24_lock = 0;
		return true;
	}
	else
	{
		ee24_lock = 0;
		return false;
	}
}
//################################################################################################################
bool ee24_eraseChip(void)
{
	const uint8_t eraseData[32] =
	{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF\
, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	uint32_t bytes = 0;
	while (bytes < (_EEPROM_SIZE_KBIT * 256))
	{
		if (ee24_write(bytes, (uint8_t*) eraseData, sizeof(eraseData),
				100) == false)
			return false;
		bytes += sizeof(eraseData);
	}
	return true;
}
//################################################################################################################

void float2Bytes(uint8_t *ftoa_bytes_temp, float float_variable)
{
	union
	{
		float a;
		uint8_t bytes[4];
	} thing;

	thing.a = float_variable;

	for (uint8_t i = 0; i < 4; i++)
	{
		ftoa_bytes_temp[i] = thing.bytes[i];
	}

}

float Bytes2float(uint8_t *ftoa_bytes_temp)
{
	union
	{
		float a;
		uint8_t bytes[4];
	} thing;

	for (uint8_t i = 0; i < 4; i++)
	{
		thing.bytes[i] = ftoa_bytes_temp[i];
	}

	float float_variable = thing.a;
	return float_variable;
}

void long2Bytes(uint8_t *longtoa_bytes_temp, long long_variable)
{
	union
	{
		long a;
		uint8_t bytes[8];
	} thing;

	thing.a = long_variable;

	for (uint8_t i = 0; i < 8; i++)
	{
		longtoa_bytes_temp[i] = thing.bytes[i];
	}

}

long Bytes2long(uint8_t *longtoa_bytes_temp)
{
	union
	{
		long a;
		uint8_t bytes[8];
	} thing;

	for (uint8_t i = 0; i < 8; i++)
	{
		thing.bytes[i] = longtoa_bytes_temp[i];
	}

	long long_variable = thing.a;
	return long_variable;
}

/*Write the Float/Integer values to the EEPROM
 * @page is the number of the start page. Range from 0 to PAGE_NUM-1
 * @offset is the start byte offset in the page. Range from 0 to PAGE_SIZE-1
 * @data is the float/integer value that you want to write*/

//void EEPROM_Write_NUM(uint16_t address, float data, uint32_t timeout)
//{
//
//	float2Bytes(bytes_temp, data);
//
//	ee24_write(address, bytes_temp, 4, timeout);
//}

void EEPROM_Write_NUM(uint16_t address, float data, uint32_t timeout) {
    float2Bytes(bytes_temp, data);
    EEPROM_Write_PageSafe(address, bytes_temp, 4);
}

/*Reads the single Float/Integer values from the EEPROM
 * @page is the number of the start page. Range from 0 to PAGE_NUM-1
 * @offset is the start byte offset in the page. Range from 0 to PAGE_SIZE-1
 * @returns the float/integer value*/

//float EEPROM_Read_NUM(uint16_t address, uint32_t timeout)
//{
//	uint8_t buffer[4];
//
//	ee24_read(address, buffer, 4, timeout);
//
//	return (Bytes2float(buffer));
//}

float EEPROM_Read_NUM(uint16_t address, uint32_t timeout) {
    uint8_t buffer[4];
    EEPROM_Read_PageSafe(address, buffer, 4);
    return Bytes2float(buffer);
}

//void EEPROM_Write_LONG(uint16_t address, long data, uint32_t timeout)
//{
//
//	long2Bytes(bytes_temp, data);
//
//	ee24_write(address, bytes_temp, 8, timeout);
//}

void EEPROM_Write_LONG(uint16_t address, long data, uint32_t timeout) {
    long2Bytes(bytes_temp, data);
    EEPROM_Write_PageSafe(address, bytes_temp, 8);
}

//long EEPROM_Read_LONG(uint16_t address, uint32_t timeout)
//{
//	uint8_t buffer[4];
//
//	ee24_read(address, buffer, 4, timeout);
//
//	return (Bytes2long(buffer));
//}

long EEPROM_Read_LONG(uint16_t address, uint32_t timeout) {
    uint8_t buffer[8];
    EEPROM_Read_PageSafe(address, buffer, 8);
    return Bytes2long(buffer);
}

// تابع نوشتن داده در EEPROM
HAL_StatusTypeDef EEPROM_Write(uint16_t memAddress, uint8_t *data, size_t len) {
    HAL_StatusTypeDef status;
    uint8_t retries = 3;

    // EEPROM آدرس حافظه 16 بیتی برای
//    uint8_t memAddrBytes[2] = { (uint8_t)(memAddress >> 8), (uint8_t)(memAddress & 0xFF) };

    while (retries--) {
        status = HAL_I2C_Mem_Write(&_EEPROM_I2C, _EEPROM_ADDRESS, memAddress, I2C_MEMADD_SIZE_16BIT, data, len, 1000);
        if (status == HAL_OK) break;
        ee24_delay(5); // تاخیر قبل از تکرار
    }

    ee24_delay(5); // تاخیر پس از نوشتن برای اطمینان از تکمیل عملیات
    return status;
}

// تابع خواندن داده از EEPROM
HAL_StatusTypeDef EEPROM_Read(uint16_t memAddress, uint8_t *data, size_t len) {
    return HAL_I2C_Mem_Read(&_EEPROM_I2C, _EEPROM_ADDRESS, memAddress, I2C_MEMADD_SIZE_16BIT, data, len, 1000);
}


HAL_StatusTypeDef EEPROM_Write_PageSafe(uint16_t memAddress, uint8_t *data,
		size_t len)
{
	HAL_StatusTypeDef status;
	const uint16_t pageSize = 64; // اندازه صفحه AT24C256
	uint16_t bytesToWrite;

	while (len > 0)
	{
		// محاسبه فضای باقیمانده در صفحه فعلی
		bytesToWrite = pageSize - (memAddress % pageSize);
		if (bytesToWrite > len)
			bytesToWrite = len;

		status = HAL_I2C_Mem_Write(&_EEPROM_I2C,
		_EEPROM_ADDRESS, memAddress,
		I2C_MEMADD_SIZE_16BIT, data, bytesToWrite, 100);

		if (status != HAL_OK)
			return status;

		// تاخیر برای تکمیل عملیات نوشتن (با پشتیبانی از FreeRTOS)
		ee24_delay(5); // از ماکروی تعریف شده استفاده می‌کند

		// به‌روزرسانی اشاره‌گرها
		memAddress += bytesToWrite;
		data += bytesToWrite;
		len -= bytesToWrite;
	}
	return HAL_OK;
}

HAL_StatusTypeDef EEPROM_Read_PageSafe(uint16_t memAddress, uint8_t *data, size_t len) {
    // برای خواندن نیازی به شکستن به بلوک‌های کوچک نیست
    return HAL_I2C_Mem_Read(
        &_EEPROM_I2C,
        _EEPROM_ADDRESS,
        memAddress,
        I2C_MEMADD_SIZE_16BIT,
        data,
        len,
        1000
    );
}
