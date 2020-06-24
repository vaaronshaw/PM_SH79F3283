/*
*********************************************************************************************************
*                                         SH79F326Demo code 
*
*                                        (c) Copyright 2014-11-21
*                                           All Rights Reserved
*
* File    : API_EXT.H
* By      : Cathy.Feng
* Version : V0.0
*********************************************************************************************************
*/
#ifndef _API_EXT_H 
#define _API_EXT_H



extern void UART_TEST();
extern void Sysclk_Test();
extern void Led_Test();
extern void Lcd_Test();
extern void Pca0_Test();
extern void Twi_Test();
extern void Spi_Test();
extern void Uart_Test();
extern void Adc_Test();
extern void Eeprom_Test();
extern void crc_test();
extern void SetClk(); 
extern void select_bank1();
extern void select_bank0();
extern void init_Pca0cpm0();
extern void init_lcd();
extern void Delay();
extern void init_spi();
extern void SpiMasterTran();
extern void SpiSlaveTran();

extern void Timer_Test();
extern void init_timer3();

extern void init_timer4();
extern void init_timer5();
//extern  void display();
extern void init_led();
extern void init_adc();
extern void init_pwm0();
extern void PWM_Test();
extern void init_pwm1();
extern void LPD_Test();
extern void CRC_Test();
#endif