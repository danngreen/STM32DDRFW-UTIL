/**
  ******************************************************************************
  * @file    ddr_tool_util.h
  * @author  MCD Application Team
  * @brief   Header for ddr_tool_util.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DDR_TOOL_UTIL_H
#define __DDR_TOOL_UTIL_H

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>


extern bool silent_console;
extern char remote_argv0[];
extern char remote_argv1[];
extern int remote_argc;
extern int remote_cmd;
extern uint32_t remote_impedance;
extern uint32_t save_test_id;
extern uint32_t save_idx;
extern uint32_t stage_in_test;
extern uint32_t back_to_test;

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void UART_Config(void);
uint32_t Serial_Scanf(uint32_t value);
void Serial_Putchar(char value);
void Serial_Printf(char *value, int len);
void Error_Handler(void);
void valid_delay_us(unsigned long delay_us);
uint32_t DDR_Test_All(uint32_t loop, uint32_t size, uint32_t addr);

#endif /* __DDR_TOOL_UTIL_H */
