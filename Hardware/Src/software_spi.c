//#include "software_spi.h"
//
//void SOFT_SPI_Init(void) {
//    GPIO_InitTypeDef GPIO_InitStruct = {0};
//
//    // پیکربندی SCLK، MOSI و CS به عنوان خروجی
//    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//
//    GPIO_InitStruct.Pin = SOFT_SPI_SCLK_PIN;
//    HAL_GPIO_Init(SOFT_SPI_SCLK_PORT, &GPIO_InitStruct);
//
//    GPIO_InitStruct.Pin = SOFT_SPI_MOSI_PIN;
//    HAL_GPIO_Init(SOFT_SPI_MOSI_PORT, &GPIO_InitStruct);
//
//    GPIO_InitStruct.Pin = SOFT_SPI_CS_PIN;
//    HAL_GPIO_Init(SOFT_SPI_CS_PORT, &GPIO_InitStruct);
//
//    // پیکربندی MISO به عنوان ورودی
//    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//    GPIO_InitStruct.Pin = SOFT_SPI_MISO_PIN;
//    HAL_GPIO_Init(SOFT_SPI_MISO_PORT, &GPIO_InitStruct);
//
//    // حالت اولیه پینها
//    HAL_GPIO_WritePin(SOFT_SPI_SCLK_PORT, SOFT_SPI_SCLK_PIN, GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(SOFT_SPI_CS_PORT, SOFT_SPI_CS_PIN, GPIO_PIN_SET);
//}
//
//void SOFT_SPI_WriteByte(uint8_t data) {
//    for (uint8_t i = 0; i < 8; i++) {
//        // ارسال بیت از MSB به LSB
//        HAL_GPIO_WritePin(SOFT_SPI_MOSI_PORT, SOFT_SPI_MOSI_PIN, (data & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//        data <<= 1;
//
//        // تولید پالس کلاک
//        HAL_GPIO_WritePin(SOFT_SPI_SCLK_PORT, SOFT_SPI_SCLK_PIN, GPIO_PIN_SET);
//        SOFT_SPI_DELAY();
//        HAL_GPIO_WritePin(SOFT_SPI_SCLK_PORT, SOFT_SPI_SCLK_PIN, GPIO_PIN_RESET);
//        SOFT_SPI_DELAY();
//    }
//}
//
//uint8_t SOFT_SPI_ReadByte(void) {
//    uint8_t data = 0;
//    for (uint8_t i = 0; i < 8; i++) {
//        data <<= 1;
//
//        // تولید پالس کلاک
//        HAL_GPIO_WritePin(SOFT_SPI_SCLK_PORT, SOFT_SPI_SCLK_PIN, GPIO_PIN_SET);
//        SOFT_SPI_DELAY();
//        data |= HAL_GPIO_ReadPin(SOFT_SPI_MISO_PORT, SOFT_SPI_MISO_PIN);
//        HAL_GPIO_WritePin(SOFT_SPI_SCLK_PORT, SOFT_SPI_SCLK_PIN, GPIO_PIN_RESET);
//        SOFT_SPI_DELAY();
//    }
//    return data;
//}
//
//void SOFT_SPI_CS_High(void) {
//    HAL_GPIO_WritePin(SOFT_SPI_CS_PORT, SOFT_SPI_CS_PIN, GPIO_PIN_SET);
//}
//
//void SOFT_SPI_CS_Low(void) {
//    HAL_GPIO_WritePin(SOFT_SPI_CS_PORT, SOFT_SPI_CS_PIN, GPIO_PIN_RESET);
//}
//
//
//



#include "software_spi.h"

void SOFT_SPI_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // پیکربندی پینهای SCLK, MOSI, CS به عنوان خروجی
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    
    GPIO_InitStruct.Pin = SOFT_SPI_SCLK_PIN;
    HAL_GPIO_Init(SOFT_SPI_SCLK_PORT, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = SOFT_SPI_MOSI_PIN;
    HAL_GPIO_Init(SOFT_SPI_MOSI_PORT, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = SOFT_SPI_CS_PIN;
    HAL_GPIO_Init(SOFT_SPI_CS_PORT, &GPIO_InitStruct);
    
    // پیکربندی MISO به عنوان ورودی
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pin = SOFT_SPI_MISO_PIN;
    HAL_GPIO_Init(SOFT_SPI_MISO_PORT, &GPIO_InitStruct);
    
    // حالت اولیه پینها
    HAL_GPIO_WritePin(SOFT_SPI_SCLK_PORT, SOFT_SPI_SCLK_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SOFT_SPI_CS_PORT, SOFT_SPI_CS_PIN, GPIO_PIN_SET);
}

void SOFT_SPI_WriteByte(uint8_t data) {
    for (uint8_t i = 0; i < 8; i++) {
        HAL_GPIO_WritePin(SOFT_SPI_MOSI_PORT, SOFT_SPI_MOSI_PIN, (data & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        data <<= 1;
        HAL_GPIO_WritePin(SOFT_SPI_SCLK_PORT, SOFT_SPI_SCLK_PIN, GPIO_PIN_SET);
        __asm volatile("nop");
        HAL_GPIO_WritePin(SOFT_SPI_SCLK_PORT, SOFT_SPI_SCLK_PIN, GPIO_PIN_RESET);
    }
}

uint8_t SOFT_SPI_ReadByte(void) {
    uint8_t data = 0;
    for (uint8_t i = 0; i < 8; i++) {
        data <<= 1;
        HAL_GPIO_WritePin(SOFT_SPI_SCLK_PORT, SOFT_SPI_SCLK_PIN, GPIO_PIN_SET);
        if (HAL_GPIO_ReadPin(SOFT_SPI_MISO_PORT, SOFT_SPI_MISO_PIN)) data |= 0x01;
        HAL_GPIO_WritePin(SOFT_SPI_SCLK_PORT, SOFT_SPI_SCLK_PIN, GPIO_PIN_RESET);
    }
    return data;
}

void SOFT_SPI_CS_High(void) {
    HAL_GPIO_WritePin(SOFT_SPI_CS_PORT, SOFT_SPI_CS_PIN, GPIO_PIN_SET);
}

void SOFT_SPI_CS_Low(void) {
    HAL_GPIO_WritePin(SOFT_SPI_CS_PORT, SOFT_SPI_CS_PIN, GPIO_PIN_RESET);
}
