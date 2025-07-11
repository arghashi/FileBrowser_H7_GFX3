#include "sdram.h"
#include "delay.h"
//#include "stm32h7xx_hal_sdram.h"
//////////////////////////////////////////////////////////////////////////////////	 
//This program is for learning and use only, and cannot be used for any other purpose without the author's permission
//ALIENTEK STM32F7 development board
//SDRAM driver code
//Punctual Atom @ALIENTEK
//Technical Forum: www.openedv.com
//Creation Date: 2015/11/27
//Version: V1.0
//Copyright, piracy must be investigated.
//Copyright(C) Guangzhou Xingyi Electronic Technology Co., Ltd. 2014-2024
//All rights reserved
///////////////////////////////////////////////// ////////////////////////////////
SDRAM_HandleTypeDef SDRAM_Handler;   //SDRAM handle

static FMC_SDRAM_CommandTypeDef Command;

//SDRAM initialization
void SDRAM_Init(void)
{
    FMC_SDRAM_TimingTypeDef SDRAM_Timing;
                                                     
    SDRAM_Handler.Instance=FMC_SDRAM_DEVICE;                          	 //SDRAM in BANK5,6
    SDRAM_Handler.Init.SDBank=FMC_SDRAM_BANK1;                           //SDRAM is connected to BANK5
    SDRAM_Handler.Init.ColumnBitsNumber=FMC_SDRAM_COLUMN_BITS_NUM_9;     //Number of columns
    SDRAM_Handler.Init.RowBitsNumber=FMC_SDRAM_ROW_BITS_NUM_13;          //Number of rows
    SDRAM_Handler.Init.MemoryDataWidth=FMC_SDRAM_MEM_BUS_WIDTH_16;       //Data width is 16 bits
    SDRAM_Handler.Init.InternalBankNumber=FMC_SDRAM_INTERN_BANKS_NUM_4;  //A total of 4 BANK
    SDRAM_Handler.Init.CASLatency=FMC_SDRAM_CAS_LATENCY_2;               //CAS is 2
    SDRAM_Handler.Init.WriteProtection=FMC_SDRAM_WRITE_PROTECTION_DISABLE;//Disable write protection
    SDRAM_Handler.Init.SDClockPeriod=FMC_SDRAM_CLOCK_PERIOD_2;           //The SDRAM clock is PLL2_DIVR2/2=240M/2=120M=8.3ns
    SDRAM_Handler.Init.ReadBurst=FMC_SDRAM_RBURST_ENABLE;                //Enable burst
    SDRAM_Handler.Init.ReadPipeDelay=FMC_SDRAM_RPIPE_DELAY_0;            //Read channel delay
    
    SDRAM_Timing.LoadToActiveDelay=2;                                   //The delay from loading the mode register to the activation time is 2 clock cycles
    SDRAM_Timing.ExitSelfRefreshDelay=8;                                //Exit self-refresh delay is 8 clock cycles
    SDRAM_Timing.SelfRefreshTime=5;                                     //Self-refresh time is 6 clock cycles
    SDRAM_Timing.RowCycleDelay=7;                                       //Line cycle delay is 6 clock cycles
    SDRAM_Timing.WriteRecoveryTime=4;                                   //Recovery delay is 2 clock cycles
    SDRAM_Timing.RPDelay=2;                                             //Line precharge delay is 2 clock cycles
    SDRAM_Timing.RCDDelay=2;                                            //Row to column delay is 2 clock cycles
    HAL_SDRAM_Init(&SDRAM_Handler,&SDRAM_Timing);  
	
	SDRAM_Initialization_Sequence(&SDRAM_Handler);//Send SDRAM initialization sequence
}

//Send SDRAM initialization sequence
void SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram)
{
//	uint32_t temp=0;
//
//    //After /SDRAM controller initialization is completed, SDRAM needs to be initialized in the following order
//    SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_CLK_ENABLE,1,0); //Clock configuration enable
////    delay_us(500);                                  //At least 200us delay
//	SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_PALL,1,0);       //Precharge all storage areas
//    SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_AUTOREFRESH_MODE,8,0);//Set the number of self-refresh
//    //Configuration mode register, bit0~bit2 of SDRAM are the length of specified burst access,
////bit3 is the type of specified burst access, bit4~bit6 are CAS values, bit7 and bit8 are operating modes
////bit9 is the specified write burst mode, bit10 and bit11 are reserved bits
//	temp=(uint32_t)SDRAM_MODEREG_BURST_LENGTH_1          |	//Set burst length: 1 (can be 1/2/4/8)
//              SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |	//Set burst type: continuous (can be continuous/interleaved)
//              SDRAM_MODEREG_CAS_LATENCY_2           |	//Set CAS value: 3 (can be 2/3)
//              SDRAM_MODEREG_OPERATING_MODE_STANDARD |   //Set operation mode: 0, standard mode
//              SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;     //Set burst write mode: 1, single point access
//    SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_LOAD_MODE,1,temp);   //Set the mode register of SDRAM
//
//    //Refresh frequency counter (counting at SDCLK frequency), calculation method:
////COUNT=SDRAM refresh cycle/number of rows-20=SDRAM refresh cycle (us)*SDCLK frequency (Mhz)/number of rows
//     //The SDRAM refresh cycle we use is 64ms, SDCLK=240/2=120Mhz, and the number of rows is 8192 (2^13).
////So, COUNT=64*1000*120/8192-20=780
//	HAL_SDRAM_ProgramRefreshRate(&SDRAM_Handler,918);

	/* Step 3:  Configure a clock configuration enable command */
	Command.CommandMode      	     = FMC_SDRAM_CMD_CLK_ENABLE;
	Command.CommandTarget 	     = FMC_SDRAM_CMD_TARGET_BANK1;
	Command.AutoRefreshNumber      = 1;
	Command.ModeRegisterDefinition = 0;

	/* Send the command */
	HAL_SDRAM_SendCommand(&SDRAM_Handler, &Command, 0x1000);

	/* Step 4: Insert 100 ms delay */
//	HAL_Delay(1);

	/* Step 5: Configure a PALL (precharge all) command */
	Command.CommandMode 			 = FMC_SDRAM_CMD_PALL;
	Command.CommandTarget 	     = FMC_SDRAM_CMD_TARGET_BANK1;
	Command.AutoRefreshNumber 	 = 1;
	Command.ModeRegisterDefinition = 0;

	/* Send the command */
	HAL_SDRAM_SendCommand(&SDRAM_Handler, &Command, SDRAM_TIMEOUT);

	/* Step 6 : Configure a Auto-Refresh command */
	Command.CommandMode 			 = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
	Command.CommandTarget 		 = FMC_SDRAM_CMD_TARGET_BANK1;
	Command.AutoRefreshNumber 	 = 8;
	Command.ModeRegisterDefinition = 0;

	/* Send the command */
	HAL_SDRAM_SendCommand(&SDRAM_Handler, &Command, 0x1000);

	Command.CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
	Command.CommandTarget 		 = FMC_SDRAM_CMD_TARGET_BANK1;
	Command.AutoRefreshNumber 	 = 1;
	Command.ModeRegisterDefinition = 0x0220;

	/* Send the command */
	HAL_SDRAM_SendCommand(&SDRAM_Handler, &Command, 0x1000);

	/* Step 8: Set the refresh rate counter */
	/* (15.62 us x Freq) - 20 */
	/* Set the device refresh counter */
	HAL_SDRAM_ProgramRefreshRate(&SDRAM_Handler, REFRESH_COUNT);
}	
//SDRAM bottom driver, pin configuration, clock enable
//This function will be called by HAL_SDRAM_Init()
//hsdram: SDRAM handle
//void HAL_SDRAM_MspInit(SDRAM_HandleTypeDef *hsdram)
//{
//    GPIO_InitTypeDef GPIO_Initure;
//
//	__HAL_RCC_SYSCFG_CLK_ENABLE();				//Enable SYSCFG clock
//    __HAL_RCC_FMC_CLK_ENABLE();                 //Enable FMC clock
//    __HAL_RCC_GPIOC_CLK_ENABLE();               //Enable GPIOC clock
//    __HAL_RCC_GPIOD_CLK_ENABLE();               //Enable GPIOD clock
//    __HAL_RCC_GPIOE_CLK_ENABLE();               //Enable GPIOE clock
//    __HAL_RCC_GPIOF_CLK_ENABLE();               //Enable GPIOF clock
//    __HAL_RCC_GPIOG_CLK_ENABLE();               //Enable GPIOG clock
//
//
//	//PC2,3 pin analog switch is off! ! ! ! !
//	HAL_SYSCFG_AnalogSwitchConfig(SYSCFG_SWITCH_PC2,SYSCFG_SWITCH_PC2_CLOSE);
//	HAL_SYSCFG_AnalogSwitchConfig(SYSCFG_SWITCH_PC3,SYSCFG_SWITCH_PC3_CLOSE);
//
//    //Initialize PC0,2,3
//    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_2|GPIO_PIN_3;
//    GPIO_Initure.Mode=GPIO_MODE_AF_PP;          //Push-pull multiplexing
//    GPIO_Initure.Pull=GPIO_PULLUP;              //pull up
//    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;         //high speed
//    GPIO_Initure.Alternate=GPIO_AF12_FMC;       //Multiplexed as FMC
//    HAL_GPIO_Init(GPIOC,&GPIO_Initure);         //initialization
//
//
//    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_14|GPIO_PIN_15;
//    HAL_GPIO_Init(GPIOD,&GPIO_Initure); //Initialize PD0,1,8,9,10,14,15
//
//    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
//    HAL_GPIO_Init(GPIOE,&GPIO_Initure); //Initialize PE0,1,7,8,9,10,11,12,13,14,15
//
//    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
//    HAL_GPIO_Init(GPIOF,&GPIO_Initure); //Initialize PF0,1,2,3,4,5,11,12,13,14,15
//
//    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_8|GPIO_PIN_15;
//    HAL_GPIO_Init(GPIOG,&GPIO_Initure);	//Initialize PG0,1,2,4,5,8,15
//}

//Send command to SDRAM
//bankx:0, send instructions to SDRAM on BANK5
// 1, send instructions to SDRAM on BANK6
//cmd: command (0, normal mode/1, clock configuration enable/2, precharge all storage areas/3, auto refresh/4, load mode register/5, self refresh/6, power down)
//refresh: number of self-refresh
//regval: definition of mode register
//Return value: 0, normal; 1, failure.
uint8_t SDRAM_Send_Cmd(uint8_t bankx,uint8_t cmd,uint8_t refresh,uint16_t regval)
{
    uint32_t target_bank=0;
    FMC_SDRAM_CommandTypeDef Command;
    
    if(bankx==0) target_bank=FMC_SDRAM_CMD_TARGET_BANK1;       
    else if(bankx==1) target_bank=FMC_SDRAM_CMD_TARGET_BANK2;   
    Command.CommandMode=cmd;                //command
    Command.CommandTarget=target_bank;      //Target SDRAM storage area
    Command.AutoRefreshNumber=refresh;      //Self-refresh times
    Command.ModeRegisterDefinition=regval;  //Value to be written to the mode register
    if(HAL_SDRAM_SendCommand(&SDRAM_Handler,&Command,0XFFFF)==HAL_OK) //Send commands to SDRAM
    {
        return 0;  
    }
    else return 1;    
}

// Start at the specified address (WriteAddr+Bank5_SDRAM_ADDR) and write n bytes continuously.
//pBuffer: byte pointer
//WriteAddr: the address to be written
//n: the number of bytes to be written
void FMC_SDRAM_WriteBuffer(uint8_t *pBuffer,uint32_t WriteAddr,uint32_t n)
{
	for(;n!=0;n--)
	{
		*(volatile uint8_t*)(Bank5_SDRAM_ADDR+WriteAddr)=*pBuffer;
		WriteAddr++;
		pBuffer++;
	}
}

// Starting at the specified address ((WriteAddr+Bank5_SDRAM_ADDR)), read n bytes continuously.
//pBuffer: byte pointer
//ReadAddr: the starting address to be read
//n: the number of bytes to be written
void FMC_SDRAM_ReadBuffer(uint8_t *pBuffer,uint32_t ReadAddr,uint32_t n)
{
	for(;n!=0;n--)
	{
		*pBuffer++=*(volatile uint8_t*)(Bank5_SDRAM_ADDR+ReadAddr);
		ReadAddr++;
	}
}
