/**
  ******************************************************************************
  * @file    ddr_tests.c
  * @author  MCD Application Team
  * @brief   This file provides all the HAL DDR test functions.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32_device_hal.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "log.h"
#include "ddr_tests.h"

#include "stm32mp_util_conf.h"
#include "stm32mp_util_ddr_conf.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DDR_BASE_ADDR                        0x80000000

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
bool silent_console = false;
char remote_argv0[20] = "\0";
char remote_argv1[20] = "\0";
int remote_argc = 0;
int remote_cmd = 0;
uint32_t remote_impedance = 0;

uint32_t save_test_id = 0;
uint32_t save_idx = 0;
uint32_t save_tx_idx = 0;
uint32_t save_odt_idx = 0;
uint32_t initial_tx = 0;
uint32_t initial_odt = 0;
#if STM32MP_DDR3_TYPE || STM32MP_DDR4_TYPE
uint32_t save_rttnom_idx = 0;
uint32_t initial_rttnom = 0;
#endif /* STM32MP_DDR3_TYPE || STM32MP_DDR4_TYPE */
#if STM32MP_DDR3_TYPE
uint32_t save_ron_idx = 0;
uint32_t initial_ron = 0;
#endif /* STM32MP_DDR3_TYPE */
#if STM32MP_DDR4_TYPE
uint32_t save_odi_idx = 0;
uint32_t initial_odi = 0;
#endif /* STM32MP_DDR4_TYPE */
#if STM32MP_LPDDR4_TYPE
uint32_t save_dqodt_idx = 0;
uint32_t save_pdds_idx = 0;
uint32_t initial_dqodt = 0;
uint32_t initial_pdds = 0;
#endif
uint32_t stage_in_test = 0;
uint32_t back_to_test = 0;
bool initial_done = false;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static int get_addr(unsigned long addr_in, uintptr_t **addr)
{
  if (addr_in != 0UL)
  {
    if (addr_in < DDR_MEM_BASE)
    {
      if (!silent_console)
      {
        printf("Address too low: 0x%lx\n\r", addr_in);
      }
      return -1;
    }

    if ((addr_in & 0x3UL) != 0UL)
    {
      if (!silent_console)
      {
        printf("Unaligned address: 0x%lx\n\r", addr_in);
      }
      return -1;
    }

    *addr = (uintptr_t *)addr_in;
  }
  else
  {
    *addr = (uintptr_t *)DDR_MEM_BASE;
  }

  return 0;
}

static void get_nb_loop(unsigned long loop_in, uint32_t *nb_loop,
                        uint32_t default_nb_loop)
{
  if (loop_in != 0)
  {
    if (loop_in == 0xFFFFFFFF)
    {
      if (!silent_console)
      {
        printf("Warning: infinite loop requested\n\r");
      }
    }

    if (loop_in > 0xFFFFFFFF)
    {
      if (!silent_console)
      {
        printf("Warning: incorrect loop_number, forced to default value\n\r");
      }
      *nb_loop = default_nb_loop;
    }
    else
    {
      *nb_loop = (uint32_t)loop_in;
    }
  }
  else
  {
    *nb_loop = default_nb_loop;
  }
}

static int get_buf_size(unsigned long size_in, unsigned long *size,
                        unsigned long default_size, unsigned long min_size)
{
  if (size_in != 0)
  {
    if ((size_in < min_size) || (size_in > (unsigned long)DDR_MEM_SIZE))
    {
      if (!silent_console)
      {
        printf("Invalid size: 0x%lx\n\r", size_in);
        printf("  (range = 0x%lx..0x%lx)\n\r", min_size, (unsigned long)DDR_MEM_SIZE);
      }
      return -1;
    }

    if ((size_in & (min_size - 1)) != 0)
    {
      if (!silent_console)
      {
        printf("Unaligned size: 0x%lx (min=0x%lx)\n\r", size_in, min_size);
      }
      return -1;
    }

    *size = size_in;
  }
  else
  {
    *size = default_size;
  }

  return 0;
}

static bool is_power_of_2(unsigned long n)
{
  return ((n != 0) && ((n & (n - 1)) == 0));
}

static void get_pattern(unsigned long pattern_in, unsigned long *pattern,
		unsigned long default_pattern)
{
  if (pattern_in != 0)
  {
    *pattern = pattern_in;
  }
  else
  {
    *pattern = default_pattern;
  }
}

static int test_loop_end(uint32_t *loop, uint32_t nb_loop)
{
  (*loop)++;
  if (nb_loop && *loop >= nb_loop)
  {
    return 1;
  }

  return 0;
}

/**
* @brief test_databus.
* @par Test Description
*   Verifies each data line by walking 1 on fixed address.
*   Test the data bus wiring in a memory region by performing
*   a walking 1's test at a fixed address within that region.
* @par Test Hardware Connection
* - None
* @par Required preconditions
* - None
* @par Expected result
* - None
* @par Called functions
* - None
* @par Used Peripherals
* - None,...
* @retval
*  0: Test passed
*  Value different from 0: Test failed
*  None(0xFF): if the result is deduced by the user: waveform, event...
*/
uint32_t DDR_Test_Databus(unsigned long addr_in)
{
  unsigned long pattern;
  uintptr_t *addr = NULL;

  if (get_addr(addr_in, &addr) != 0)
  {
    return 1;
  }

  for (pattern = 1U; pattern != 0U; pattern <<= 1)
  {
    *addr = pattern;

    if (*addr != pattern)
    {
      if (!silent_console)
      {
        printf("  %s KO @ 0x%lx \n\r", __func__,  (unsigned long)addr);
      }
      return 2;
    }
  }

  return 0;
}

static int test_databuswalk(uint8_t mode, unsigned long loop_in, unsigned long addr_in)
{
  int i;
  uintptr_t *addr = NULL;
  unsigned long data;
  unsigned long error = 0U;
  uint32_t loop = 0;
  uint32_t nb_loop;

  get_nb_loop(loop_in, &nb_loop, 100);

  if (get_addr(addr_in, &addr) != 0)
  {
    return 1;
  }

  while (error == 0U)
  {
    for (i = 0; i < (int)(sizeof(unsigned long) * 8); i++)
    {
      unsigned long pattern = mode ? (1 << i) : ~(1 << i);

      *(addr + sizeof(unsigned long) * i) = pattern;
    }

    for (i = 0; i < (int)(sizeof(unsigned long) * 8); i++)
    {
      unsigned long pattern = mode ? (1 << i) : ~(1 << i);

      data = *(addr + sizeof(unsigned long) * i);
      if (pattern !=  data)
      {
        error |= 1 << i;
        if (!silent_console)
        {
          printf("  0x%lx: error 0x%lx expected 0x%lx => error:0x%lx\n\r",
                 (unsigned long)(addr + sizeof(unsigned long) * i), data, pattern, error);
        }
      }
    }

    if (test_loop_end(&loop, nb_loop))
    {
      break;
    }

    for (i = 0; i < (int)(sizeof(unsigned long) * 8); i++)
    {
      *(addr + sizeof(unsigned long) * i) = 0;
    }
  }

  if (error != 0U)
  {
    if (!silent_console)
    {
      printf("  %s%d KO\n\r", __func__, mode);
    }
    return 2;
  }

  return 0;
}

/**
* @brief test_databuswalk0.
* @par Test Description
*   Verifies each data bus signal can be driven low (32 word burst).
* @par Test Hardware Connection
* - None
* @par Required preconditions
* - None
* @par Expected result
* - None
* @par Called functions
* - xxx
* @par Used Peripherals
* - None,...
* @retval
*  0: Test passed
*  Value different from 0: Test failed
*  None(0xFF): if the result is deduced by the user: waveform, event...
*/
uint32_t DDR_Test_DatabusWalk0(unsigned long loop, unsigned long addr)
{
  return test_databuswalk(0, loop, addr);
}

/**
* @brief test_databuswalk1.
* @par Test Description
*   Verifies each data bus signal can be driven high (32 word burst).
* @par Test Hardware Connection
* - None
* @par Required preconditions
* - None
* @par Expected result
* - None
* @par Called functions
* - xxx
* @par Used Peripherals
* - None,...
* @retval
*  0: Test passed
*  Value different from 0: Test failed
*  None(0xFF): if the result is deduced by the user: waveform, event...
*/
uint32_t DDR_Test_DatabusWalk1(unsigned long loop, unsigned long addr)
{
  return test_databuswalk(1, loop, addr);
}

/**
* @brief test_addrbus.
* @par Test Description
*   Verifies each relevant bits of the address and checking for aliasing.
*   Test the address bus wiring in a memory region by performing
*   a walking 1's test on the relevant bits of the address and
*   checking for aliasing. This test will find single-bit address
*   failures such as stuck -high, stuck-low, and shorted pins.
*   For best results, the selected base address should have enough
*   LSB 0's to guarantee single address bit changes.  For example,
*   to test a 64-Kbyte region, select a base address on a 64-Kbyte
*   boundary.  Also, select the region size as a power-of-two
*   --if at all possible.
* @par Test Hardware Connection
* - None
* @par Required preconditions
* - None
* @par Expected result
* - None
* @par Called functions
* - xxx
* @par Used Peripherals
* - None,...
* @retval
*  0: Test passed
*  Value different from 0: Test failed
*  None(0xFF): if the result is deduced by the user: waveform, event...
*/
uint32_t DDR_Test_AddressBus(unsigned long size_in, unsigned long addr_in)
{
  uintptr_t *addr = NULL;
  unsigned long size;
  unsigned long dflt_size;
  unsigned long addressmask;
  unsigned long offset;
  unsigned long testoffset = 0;
  unsigned long pattern     = 0xAAAAAAAAAAAAAAAA;
  unsigned long antipattern = 0x5555555555555555;
  unsigned long data;

  if (!is_power_of_2(DDR_MEM_SIZE))
  {
    /* Get the bigger 2^n value of DDR_MEM_SIZE */
    if ((DDR_MEM_SIZE & 0x100000000) == 0x100000000)
    {
      dflt_size = 0x100000000;
    }
    else if ((DDR_MEM_SIZE & 0x80000000) == 0x80000000)
    {
      dflt_size = 0x80000000;
    }
    else if ((DDR_MEM_SIZE & 0x40000000) == 0x40000000)
    {
      dflt_size = 0x40000000;
    }
    else if ((DDR_MEM_SIZE & 0x20000000) == 0x20000000)
    {
      dflt_size = 0x20000000;
    }
    else if ((DDR_MEM_SIZE & 0x10000000) == 0x10000000)
    {
      dflt_size = 0x10000000;
    }
    else
    {
      if (!silent_console)
      {
        printf("DDR size too low for this test (0x%lx)\n\r",
               (unsigned long)DDR_MEM_SIZE);
      }
      return 2;
    }
  }
  else
  {
    dflt_size = DDR_MEM_SIZE;
  }

  if (get_buf_size(size_in, &size, dflt_size, sizeof(unsigned long)) != 0)
  {
    return 1;
  }

  if (!is_power_of_2(size))
  {
    if (!silent_console)
    {
      printf("size 0x%lx is not a power of 2\n\r", size);
    }
    return 2;
  }

  if (get_addr(addr_in, &addr) != 0)
  {
    return 3;
  }

  addressmask = size - 1;

  /* Write the default pattern at each of the power-of-two offsets. */
  for (offset = 1U;
       ((offset & addressmask) != 0U) &&
       ((offset * sizeof(unsigned long)) != dflt_size);
       offset <<= 1)
  {
    *(addr + offset) = pattern;
  }

  /* Check for address bits stuck high. */
  *(addr + testoffset) = antipattern;

  for (offset = 1U;
       ((offset & addressmask) != 0U) &&
       ((offset * sizeof(unsigned long)) != dflt_size);
       offset <<= 1)
  {
    data = *(addr + offset);
    if (data != pattern)
    {
      if (!silent_console)
      {
        printf("  %s KO @ 0x%lx \n\r", __func__, (unsigned long)(addr + offset));
        printf("  read 0x%lx instead of 0x%lx\n\r", data, pattern);
      }
      return 4;
    }
  }

  *(addr + testoffset) = pattern;

  /* Check for address bits stuck low or shorted. */
  for (testoffset = 1U;
       ((testoffset & addressmask) != 0U) &&
       ((testoffset * sizeof(unsigned long)) != dflt_size);
       testoffset <<= 1)
  {
    *(addr + testoffset) = antipattern;

    data = *addr;
    if (data != pattern)
    {
      if (!silent_console)
      {
        printf("  %s KO @ 0x%lx \n\r", __func__, (unsigned long)(addr + testoffset));
        printf("  read 0x%lx instead of 0x%lx\n\r", data, pattern);
      }
      return 5;
    }

    for (offset = 1U;
         ((offset & addressmask) != 0U) &&
         ((offset * sizeof(unsigned long)) != dflt_size);
         offset <<= 1)
    {
     data = *(addr + offset);
     if ((data != pattern) && (offset != testoffset))
      {
        if (!silent_console)
        {
          printf("  %s KO @ 0x%lx \n\r", __func__, (unsigned long)(addr + offset));
          printf("  read 0x%lx instead of 0x%lx\n\r", data, pattern);
        }
        return 6;
      }
    }

    *(addr + testoffset) = pattern;
  }

  return 0;
}

/**
* @brief test_memdevice.
* @par Test Description
*   Test the integrity of a physical memory device (test every storage
*   bit in the region) by performing an increment/decrement test over
*   the entire region. In the process every storage bit in the device
*   is tested as a zero and a one.
* @par Test Hardware Connection
* - None
* @par Required preconditions
* - None
* @par Expected result
* - None
* @par Called functions
* - xxx
* @par Used Peripherals
* - None,...
* @retval
*  0: Test passed
*  Value different from 0: Test failed
*  None(0xFF): if the result is deduced by the user: waveform, event...
*/
uint32_t DDR_Test_MemDevice(unsigned long size_in, unsigned long addr_in)
{
  uintptr_t *addr = NULL;
  unsigned long size;
  unsigned long nb_words;
  unsigned long offset;
  unsigned long pattern;
  unsigned long antipattern;

  if (get_buf_size(size_in, &size, 4 * 1024, sizeof(unsigned long)) != 0)
  {
    return 1;
  }

  if (get_addr(addr_in, &addr) != 0)
  {
    return 2;
  }

  nb_words = size / sizeof(unsigned long);

  /* Fill memory with a known pattern. */
  for (pattern = 1, offset = 0; offset < nb_words;
       pattern++, offset += sizeof(unsigned long))
  {
    *(addr + offset) = pattern;
  }

  /* Check each location and invert it for the second pass. */
  for (pattern = 1, offset = 0; offset < nb_words;
       pattern++, offset += sizeof(unsigned long))
  {
    if (*(addr + offset) != pattern)
    {
      if (!silent_console)
      {
        printf("  %s KO @ 0x%lx \n\r", __func__, (unsigned long)(addr + offset));
      }
      return 3;
    }

    antipattern = ~pattern;
    *(addr + offset) = antipattern;
  }

  /* Check each location for the inverted pattern and zero it. */
  for (pattern = 1, offset = 0; offset < nb_words;
       pattern++, offset += sizeof(unsigned long))
  {
    antipattern = ~pattern;
    if (*(addr + offset) != antipattern)
    {
      if (!silent_console)
      {
        printf("  %s KO @ 0x%lx \n\r", __func__, (unsigned long)(addr + offset));
      }
      return 4;
    }
  }

  return 0;
}

/**
* @brief test_sso.
* @par Test Description
*   Test the Simultaneous Switching Output.
*   Verifies succes sive reads and writes to the same memory word,
*   holding one bit constant while toggling all other data bits
*   simultaneously
*   => stress the data bus over an address range
*   The CPU writes to each address in the given range.
*   For each bit, first the CPU holds the bit at 1 while toggling the
*   other bits, and then the CPU holds the bit at 0 while toggling
*   the other bits.
*   After each write, the CPU reads the address that was written
*   to verify that it contains the correct data.
* @par Test Hardware Connection
* - None
* @par Required preconditions
* - None
* @par Expected result
* - None
* @par Called functions
* - xxx
* @par Used Peripherals
* - None,...
* @retval
*  0: Test passed
*  Value different from 0: Test failed
*  None(0xFF): if the result is deduced by the user: waveform, event...
*/
uint32_t DDR_Test_SimultaneousSwitchingOutput(unsigned long size_in,
                                              unsigned long addr_in)
{
  int i, j;
  uintptr_t *addr = NULL;
  unsigned long size;
  unsigned long remaining;
  unsigned long offset;
  unsigned long data = 0;

  if (get_buf_size(size_in, &size, 4 * 1024, sizeof(unsigned long)) != 0)
  {
    return 1;
  }

  if (get_addr(addr_in, &addr) != 0)
  {
    return 2;
  }

  //printf("running sso at 0x%lx length 0x%lx\n\r", (uint32_t)addr, size);
  offset = 0;
  remaining = size;

  while (remaining) {
    for (i = 0; i < (int)(sizeof(unsigned long) * 8); i++) {
      /* write pattern. */
      for (j = 0; j < 6; j++) {
        switch (j)
        {
          case 0:
          case 2:
            data = 1 << i;
            break;
          case 3:
          case 5:
            data = ~(1 << i);
            break;
          case 1:
            data = ~0x0;
            break;
          case 4:
            data = 0x0;
            break;
        }

        *(addr + offset) = data;

        if (*(addr + offset) != data)
        {
          if (!silent_console)
          {
            printf("  %s KO @ 0x%lx \n\r", __func__, (unsigned long)(addr + offset));
          }
          return 3;
        }
      }
    }
    offset ++;
    remaining -= sizeof(unsigned long);
  }

  return 0;
}

static void do_noise(unsigned long addr, unsigned long pattern,
                     unsigned long *result)
{
  __asm volatile (
//                "PUSH {x2-x10}            \n"
                  "STP x2, x3, [sp, #-16]!  \n"
                  "STP x4, x5, [sp, #-16]!  \n"
                  "STP x6, x7, [sp, #-16]!  \n"
                  "STP x8, x9, [sp, #-16]!  \n"
                  "STP x10, x11, [sp, #-16]!\n"

                  "MOV x0, %[addr]          \n"
                  "MOV x1, %[pattern]       \n"
                  "MOV x12, %[result]       \n"

                  "MVN x2, x1               \n"

                  "STR x1, [x0]             \n"
                  "LDR x3, [x0]             \n"
                  "STR x2, [x0]             \n"
                  "LDR x4, [x0]             \n"

                  "STR x1, [x0]             \n"
                  "LDR x5, [x0]             \n"
                  "STR x2, [x0]             \n"
                  "LDR x6, [x0]             \n"

                  "STR x1, [x0]             \n"
                  "LDR x7, [x0]             \n"
                  "STR x2, [x0]             \n"
                  "LDR x8, [x0]             \n"

                  "STR x1, [x0]             \n"
                  "LDR x9, [x0]             \n"
                  "STR x2, [x0]             \n"
                  "LDR x10, [x0]            \n"

//                "STMIA x12!, {x3-x10}     \n"
                  "STP x3, x4, [x12], #16   \n"
                  "STP x5, x6, [x12], #16   \n"
                  "STP x7, x8, [x12], #16   \n"
                  "STP x9, x10, [x12], #16  \n"

//                "POP {x2-x10}             \n"
                  "LDP x2, x3, [sp], #16    \n"
                  "LDP x4, x5, [sp], #16    \n"
                  "LDP x6, x7, [sp], #16    \n"
                  "LDP x8, x9, [sp], #16    \n"
                  "LDP x10, x11, [sp], #16  \n"
                  ::[addr]    "r" (addr),
                    [pattern] "r" (pattern),
                    [result]  "r" (result)
                  : "x0", "x1", "x12");
}


/**
* @brief test_noise.
* @par Test Description
*   Verifies r/w while forcing switching of all data bus lines.
*   optimised 4 iteration write/read/write/read cycles...
*   for pattern and inversed pattern.
* @par Test Hardware Connection
* - None
* @par Required preconditions
* - None
* @par Expected result
* - None
* @par Called functions
* - xxx
* @par Used Peripherals
* - None,...
* @retval
*  0: Test passed
*  Value different from 0: Test failed
*  None(0xFF): if the result is deduced by the user: waveform, event...
*/
uint32_t DDR_Test_Noise(unsigned long pattern_in, unsigned long addr_in)
{
  unsigned long pattern;
  unsigned long dflt_pattern = 0xFFFFFFFFFFFFFFFF;
  uintptr_t *addr = NULL;
  unsigned long result[8] = {0,0,0,0,0,0,0,0};
  int i;

  get_pattern(pattern_in, &pattern, dflt_pattern);

  if (get_addr(addr_in, &addr) != 0)
  {
    return 1;
  }

  do_noise((unsigned long)addr, pattern, result);

  for (i = 0; i < 8;)
  {
    if (*(&result[i++]) != pattern)
    {
      if (!silent_console)
      {
        printf("  %s KO @ 0x%lx \n\r", __func__, result[i - 1]);
      }
      return 2;
    }

    if (*(&result[i++]) != ~pattern)
    {
      if (!silent_console)
      {
        printf("  %s KO @ 0x%lx \n\r", __func__, result[i - 1]);
      }
      return 3;
    }
  }

  return 0;
}

static void do_noiseburst(unsigned long addr, unsigned long pattern,
                          unsigned long bufsize)
{
  __asm volatile (
//                "PUSH {x2-x8}             \n"
                  "STP x2, x3, [sp, #-16]!  \n"
                  "STP x4, x5, [sp, #-16]!  \n"
                  "STP x6, x7, [sp, #-16]!  \n"
                  "STP x8, x9, [sp, #-16]!  \n"

                  "MOV x0, %[addr]          \n"
                  "MOV x1, %[pattern]       \n"
                  "MOV x10, %[bufsize]      \n"

                  "MVN x2, x1               \n"
                  "MOV x3, x1               \n"
                  "MOV x4, x2               \n"
                  "MOV x5, x1               \n"
                  "MOV x6, x2               \n"
                  "MOV x7, x1               \n"
                  "MOV x8, x2               \n"

                  "loop1:                   \n"

//                "STMIA x0!, {x1-x8}       \n"
                  "STP x1, x2, [x0], #16    \n"
                  "STP x3, x4, [x0], #16    \n"
                  "STP x5, x6, [x0], #16    \n"
                  "STP x7, x8, [x0], #16    \n"

//                "STMIA x0!, {x1-x8}       \n"
                  "STP x1, x2, [x0], #16    \n"
                  "STP x3, x4, [x0], #16    \n"
                  "STP x5, x6, [x0], #16    \n"
                  "STP x7, x8, [x0], #16    \n"

//                "STMIA x0!, {x1-x8}       \n"
                  "STP x1, x2, [x0], #16    \n"
                  "STP x3, x4, [x0], #16    \n"
                  "STP x5, x6, [x0], #16    \n"
                  "STP x7, x8, [x0], #16    \n"

//                "STMIA x0!, {x1-x8}       \n"
                  "STP x1, x2, [x0], #16    \n"
                  "STP x3, x4, [x0], #16    \n"
                  "STP x5, x6, [x0], #16    \n"
                  "STP x7, x8, [x0], #16    \n"

                  "SUBS x10, x10, #256      \n"
                  "BGE loop1                \n"

//                "POP {x2-x8}              \n"
                  "LDP x2, x3, [sp], #16    \n"
                  "LDP x4, x5, [sp], #16    \n"
                  "LDP x6, x7, [sp], #16    \n"
                  "LDP x8, x9, [sp], #16    \n"
                  ::[addr]    "r" (addr),
                    [pattern] "r" (pattern),
                    [bufsize] "r" (bufsize)
                  : "x0", "x1", "x10");
}

/**
* @brief test_noiseburst.
* @par Test Description
*   Verifies r/w while forcing switching of all data bus lines.
*   optimised write loop witrh store multiple to use burst
*   for pattern and inversed pattern.
* @par Test Hardware Connection
* - None
* @par Required preconditions
* - None
* @par Expected result
* - None
* @par Called functions
* - xxx
* @par Used Peripherals
* - None,...
* @retval
*  0: Test passed
*  Value different from 0: Test failed
*  None(0xFF): if the result is deduced by the user: waveform, event...
*/
uint32_t DDR_Test_NoiseBurst(unsigned long size_in, unsigned long pattern_in,
                             unsigned long addr_in)
{
  unsigned long pattern;
  uintptr_t *addr = NULL;
  unsigned long bufsize;
  unsigned long data;
  unsigned long i;

  if (get_buf_size(size_in, &bufsize, 4 * 1024, 128) != 0)
  {
    return 1;
  }

  get_pattern(pattern_in, &pattern, 0xFFFFFFFF);

  if (get_addr(addr_in, &addr) != 0)
  {
    return 2;
  }

  do_noiseburst((unsigned long)addr, pattern, bufsize);

  for (i = 0; i < bufsize / sizeof(unsigned long);)
  {
    data = *(addr + i);
    if (data != pattern)
    {
      if (!silent_console)
      {
        printf("  %s KO @ 0x%lx\n\r", __func__, (unsigned long)(addr + i));
        printf("  read 0x%lx instead of 0x%lx\n\r", data, pattern);
      }
      return 3;
    }

    i++;

    data = *(addr + i);
    if (data != ~pattern)
    {
      if (!silent_console)
      {
        printf("  %s KO @ 0x%lx\n\r", __func__, (unsigned long)(addr + i));
        printf("  read 0x%lx instead of 0x%lx\n\r", data, pattern);
      }
      return 4;
    }

    i++;
  }

  return 0;
}

/**
* @brief test_random.
* @par Test Description
*   Verifies r/w with pseudo-ramdom value on one region
*   + write the region (individual access)
*   + memcopy to the 2nd region (try to use burst)
*   + verify the 2 regions
* @par Test Hardware Connection
* - None
* @par Required preconditions
* - None
* @par Expected result
* - None
* @par Called functions
* - xxx
* @par Used Peripherals
* - None,...
* @retval
*  0: Test passed
*  Value different from 0: Test failed
*  None(0xFF): if the result is deduced by the user: waveform, event...
*/
uint32_t DDR_Test_Random(unsigned long size_in, unsigned long loop_in,
                         unsigned long addr_in)
{
  unsigned long value = 0;
  unsigned long data;
  unsigned long offset;
  uintptr_t *addr = NULL;
  unsigned long error = 0U;
  uint32_t loop = 0;
  uint32_t nb_loop;
  unsigned long bufsize_bytes;
  unsigned long bufsize_words;
  unsigned int seed;

  if (get_buf_size(size_in, &bufsize_bytes, 4 * 1024, 8) != 0)
  {
    return 1;
  }

  get_nb_loop(loop_in, &nb_loop, 10);

  if (get_addr(addr_in, &addr) != 0)
  {
    return 2;
  }

  bufsize_bytes /= 2;
  bufsize_words = bufsize_bytes/sizeof(unsigned long);

  while (error == 0U)
  {
    seed = rand();
    srand(seed);

    for (offset = 0; offset < bufsize_words; offset ++)
    {
      data = rand();
      *(addr + offset) = data;
    }

    memcpy((void *)((unsigned long)addr + bufsize_bytes), addr, bufsize_bytes);

    srand(seed);

    for (offset = 0; offset < (2 * bufsize_words); offset ++)
    {
      if (offset == bufsize_words)
      {
        srand(seed);
      }

      value = rand();
      data = *(addr + offset);
      if (data != value)
      {
        error++;
        if (!silent_console)
        {
          printf("  loop %d: error @ 0x%lx: 0x%lx expected 0x%lx\n\r",
                 loop, (unsigned long)(addr + offset), data, value);
	}
        break;
      }
    }

    if (test_loop_end(&loop, nb_loop))
    {
      break;
    }
  }

  if (error != 0U)
  {
    if (!silent_console)
    {
      printf("  %s KO\n\r", __func__);
    }
    return 3;
  }

  return 0;
}

#define DDR_PATTERN_SIZE  8

/* pattern test, optimized loop for read/write pattern (array of 8 u32) */
static void test_loop_in(const unsigned long *pattern, unsigned long offset,
                         unsigned long testsize)
{
  __asm volatile (
//                "PUSH {x3-x10}            \n"
                  "STP x3, x4, [sp, #-16]!  \n"
                  "STP x5, x6, [sp, #-16]!  \n"
                  "STP x7, x8, [sp, #-16]!  \n"
                  "STP x9, x10, [sp, #-16]! \n"

                  "MOV x0, %[pattern]       \n"
                  "MOV x1, %[offset]        \n"
                  "MOV x2, %[testsize]      \n"

//                "LDMIA x0!, {x3-x10}      \n"
                  "LDP x3, x4, [x0], #16    \n"
                  "LDP x5, x6, [x0], #16    \n"
                  "LDP x7, x8, [x0], #16    \n"
                  "LDP x9, x10, [x0], #16   \n"

                  "loop2:                   \n"

//                "STMIA x1!, {x3-x10}      \n"
                  "STP x3, x4, [x1], #16    \n"
                  "STP x5, x6, [x1], #16    \n"
                  "STP x7, x8, [x1], #16    \n"
                  "STP x9, x10, [x1], #16   \n"

//                "STMIA x1!, {x3-x10}      \n"
                  "STP x3, x4, [x1], #16    \n"
                  "STP x5, x6, [x1], #16    \n"
                  "STP x7, x8, [x1], #16    \n"
                  "STP x9, x10, [x1], #16   \n"

//                "STMIA x1!, {x3-x10}      \n"
                  "STP x3, x4, [x1], #16    \n"
                  "STP x5, x6, [x1], #16    \n"
                  "STP x7, x8, [x1], #16    \n"
                  "STP x9, x10, [x1], #16   \n"

//                "STMIA x1!, {x3-x10}      \n"
                  "STP x3, x4, [x1], #16    \n"
                  "STP x5, x6, [x1], #16    \n"
                  "STP x7, x8, [x1], #16    \n"
                  "STP x9, x10, [x1], #16   \n"

                  "SUBS x2, x2, #256        \n"
                  "BGE loop2                \n"

//                "POP {x3-x10}             \n"
                  "LDP x3, x4, [sp], #16    \n"
                  "LDP x5, x6, [sp], #16    \n"
                  "LDP x7, x8, [sp], #16    \n"
                  "LDP x9, x10, [sp], #16   \n"
                    ::[pattern]  "r" (pattern),
                      [offset]   "r" (offset),
                      [testsize] "r" (testsize)
                    : "x0", "x1", "x2");
}

static int test_loop(const unsigned long *pattern, uintptr_t *address,
                     const unsigned long bufsize)
{
  unsigned long i;
  int j;
  unsigned long offset;
  unsigned long testsize;
  unsigned long remaining;

  offset = (unsigned long)address;
  remaining = bufsize;

  while (remaining)
  {
    testsize = bufsize > 0x1000000 ? 0x1000000 : bufsize;

    test_loop_in(pattern, offset, testsize);

    offset += testsize;
    remaining -= testsize;
  }

  offset = 0;

  for (i = 0; i < bufsize / sizeof(unsigned long); i += DDR_PATTERN_SIZE)
  {
    for (j = 0; j < DDR_PATTERN_SIZE; j++, offset++)
    {
      if (*(address + offset) != pattern[j])
      {
        if (!silent_console)
        {
          printf("  %s KO @ 0x%lx\n\r", __func__, (unsigned long)(address + offset));
	}
        return 1;
      }
    }
  }

  return 0;
}

const unsigned long pattern_div1_x16[DDR_PATTERN_SIZE] = {
  0x0000FFFF0000FFFF, 0x0000FFFF0000FFFF, 0x0000FFFF0000FFFF, 0x0000FFFF0000FFFF,
  0x0000FFFF0000FFFF, 0x0000FFFF0000FFFF, 0x0000FFFF0000FFFF, 0x0000FFFF0000FFFF
};

const unsigned long pattern_div2_x16[DDR_PATTERN_SIZE] = {
  0xFFFFFFFFFFFFFFFF, 0x0000000000000000, 0xFFFFFFFFFFFFFFFF, 0x0000000000000000,
  0xFFFFFFFFFFFFFFFF, 0x0000000000000000, 0xFFFFFFFFFFFFFFFF, 0x0000000000000000
};

const unsigned long pattern_div4_x16[DDR_PATTERN_SIZE] = {
  0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0x0000000000000000, 0x0000000000000000,
  0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0x0000000000000000, 0x0000000000000000
};

const unsigned long pattern_div4_x32[DDR_PATTERN_SIZE] = {
  0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF,
  0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000
};

const unsigned long pattern_mostly_zero_x16[DDR_PATTERN_SIZE] = {
  0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000FFFF0000FFFF,
  0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000
};

const unsigned long pattern_mostly_zero_x32[DDR_PATTERN_SIZE] = {
  0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0xFFFFFFFFFFFFFFFF,
  0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000
};

const unsigned long pattern_mostly_one_x16[DDR_PATTERN_SIZE] = {
  0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0x0000FFFF0000FFFF,
  0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF
};

const unsigned long pattern_mostly_one_x32[DDR_PATTERN_SIZE] = {
  0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0x0000000000000000,
  0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF
};

#define DDR_NB_PATTERN                      5
#define DDRCTRL_MSTR_DATA_BUS_WIDTH_HALF    DDRC_MSTR_DATA_BUS_WIDTH_0
#define DDRCTRL_MSTR_DATA_BUS_WIDTH_QUARTER DDRC_MSTR_DATA_BUS_WIDTH_1

/**
* @brief test_freqpattern.
* @par Test Description
*   Frequency Selective Pattern Test.
*   Stress data bus by performing successive write 8-word burst
*   operations using mostly zero/one patterns and frequency divider
*   patterns (F/1, F/2, F/4) for 16 and 32 data bus width.
* @par Test Hardware Connection
* - None
* @par Required preconditions
* - None
* @par Expected result
* - None
* @par Called functions
* - xxx
* @par Used Peripherals
* - None,...
* @retval
*  0: Test passed
*  Value different from 0: Test failed
*  None(0xFF): if the result is deduced by the user: waveform, event...
*/
uint32_t DDR_Test_FrequencySelectivePattern(unsigned long size,
                                            unsigned long addr_in)
{
  const unsigned long * const patterns_x16[DDR_NB_PATTERN] = {
    pattern_div1_x16,
    pattern_div2_x16,
    pattern_div4_x16,
    pattern_mostly_zero_x16,
    pattern_mostly_one_x16,
  };
  const unsigned long * const patterns_x32[DDR_NB_PATTERN] = {
    pattern_div2_x16,
    pattern_div4_x16,
    pattern_div4_x32,
    pattern_mostly_zero_x32,
    pattern_mostly_one_x32
  };

  int i;
  int bus_width;
  int ret = 0;
  const unsigned long **patterns;
  unsigned long bufsize;
  uintptr_t *addr = NULL;

  if (get_buf_size(size, &bufsize, 4 * 1024, 128) != 0)
  {
    return 1;
  }

  if (get_addr(addr_in, &addr) != 0)
  {
    return 2;
  }

  switch (READ_REG(DDRC->MSTR) & DDRC_MSTR_DATA_BUS_WIDTH_Msk)
  {
    case DDRCTRL_MSTR_DATA_BUS_WIDTH_HALF:
    case DDRCTRL_MSTR_DATA_BUS_WIDTH_QUARTER:
      bus_width = 16;
      break;
    default:
      bus_width = 32;
      break;
  }

  patterns = (const unsigned long **)(bus_width == 16 ? patterns_x16 : patterns_x32);

  for (i = 0; i < DDR_NB_PATTERN; i++)
  {
    ret = test_loop(patterns[i], addr, bufsize);
    if (ret != 0)
    {
      if (!silent_console)
      {
        printf("  %s KO\n\r", __func__);
      }
      return 3;
    }
  }

  return 0;
}

/* pattern test with size, loop for write pattern */
static int test_loop_size(const unsigned long *pattern, unsigned long size,
                          uintptr_t *address, const unsigned long bufsize,
                          __attribute__((unused))uint32_t loop_nb,
                          __attribute__((unused))uint32_t loop)
{
  unsigned long i, j;
  uintptr_t *addr = (uintptr_t *)address;

  for (i = 0; i < bufsize / sizeof(unsigned long); i += size)
  {
    for (j = 0; j < size ; j++, addr++)
    {
      *addr = pattern[j];
    }

  }

  addr = address;
  for (i = 0; i < bufsize / sizeof(unsigned long); i += size)
  {
    for (j = 0; j < size; j++, addr++)
    {
      if (*addr != pattern[j])
      {
        if (!silent_console)
        {
          printf("  test KO @ 0x%lx\n\r", (unsigned long)addr);
	}
        return 1;
      }
    }
  }

  return 0;
}

/**
* @brief test_blockseq.
* @par Test Description
*   Block Sequential Test.
*   Test incremental pattern.
* @par Test Hardware Connection
* - None
* @par Required preconditions
* - None
* @par Expected result
* - None
* @par Called functions
* - xxx
* @par Used Peripherals
* - None,...
* @retval
*  0: Test passed
*  Value different from 0: Test failed
*  None(0xFF): if the result is deduced by the user: waveform, event...
*/
uint32_t DDR_Test_BlockSequential(unsigned long size, unsigned long loop_in,
                                  unsigned long addr_in)
{
  unsigned long bufsize;
  uint32_t nb_loop;
  uint32_t loop = 0;
  uintptr_t *addr = NULL;
  unsigned long value;
  unsigned long i;
  int ret;

  if (get_buf_size(size, &bufsize, 4 * 1024, sizeof(unsigned long)) != 0)
  {
    return 1;
  }

  get_nb_loop(loop_in, &nb_loop, 1);

  if (get_addr(addr_in, &addr) != 0)
  {
    return 2;
  }

  while (1)
  {
    for (i = 0; i < 256; i++)
    {
      value = i | i << 8 | i << 16 | i << 24 | i << 32 | i << 40 | i << 48 | i << 56;
      ret = test_loop_size(&value, 1, addr, bufsize, 256, i);
      if (ret != 0)
      {
        if (!silent_console)
        {
          printf("  %s KO\n\r", __func__);
	}
        return 3;
      }
    }

    if (test_loop_end(&loop, nb_loop))
    {
      break;
    }
  }

  return 0;
}

/**
* @brief test_checkboard.
* @par Test Description
*   Checker board test.
*   Test checker pattern.
* @par Test Hardware Connection
* - None
* @par Required preconditions
* - None
* @par Expected result
* - None
* @par Called functions
* - xxx
* @par Used Peripherals
* - None,...
* @retval
*  0: Test passed
*  Value different from 0: Test failed
*  None(0xFF): if the result is deduced by the user: waveform, event...
*/
uint32_t DDR_Test_Checkerboard(unsigned long size, unsigned long loop_in,
                               unsigned long addr_in)
{
  unsigned long bufsize;
  uint32_t nb_loop;
  uint32_t loop = 0;
  uintptr_t *addr = NULL;
  unsigned long checkboard[2];
  int i;
  int ret;

  checkboard[0] = 0x5555555555555555;
  checkboard[1] = 0xAAAAAAAAAAAAAAAA;

  if (get_buf_size(size, &bufsize, 4 * 1024, 8) != 0)
  {
    return 1;
  }

  get_nb_loop(loop_in, &nb_loop, 1);

  if (get_addr(addr_in, &addr) != 0)
  {
    return 2;
  }

  while (1)
  {
    for (i = 0; i < 2; i++)
    {
      ret = test_loop_size(checkboard, 2, addr, bufsize, 2, i);
      if (ret != 0)
      {
        if (!silent_console)
        {
          printf("  %s KO\n\r", __func__);
	}
        return 3;
      }

      checkboard[0] = ~checkboard[0];
      checkboard[1] = ~checkboard[1];
    }

    if (test_loop_end(&loop, nb_loop))
    {
      break;
    }
  }

  return 0;
}

/**
* @brief test_bitspread.
* @par Test Description
*   Test Bit Spread pattern.
*   Try to catch bad bits which are dependent on the current values of
*   surrounding bits in either the same word32.
* @par Test Hardware Connection
* - None
* @par Required preconditions
* - None
* @par Expected result
* - None
* @par Called functions
* - xxx
* @par Used Peripherals
* - None,...
* @retval
*  0: Test passed
*  Value different from 0: Test failed
*  None(0xFF): if the result is deduced by the user: waveform, event...
*/
uint32_t DDR_Test_BitSpread(unsigned long size, unsigned long loop_in,
                            unsigned long addr_in)
{
  unsigned long bufsize;
  uint32_t nb_loop;
  uint32_t loop = 0;
  uintptr_t *addr = NULL;
  unsigned long bitspread[4];
  int i;
  int j;
  int ret;

  if (get_buf_size(size, &bufsize, 4 * 1024, 32) != 0)
  {
    return 1;
  }

  get_nb_loop(loop_in, &nb_loop, 1);

  if (get_addr(addr_in, &addr) != 0)
  {
    return 2;
  }

  while (1)
  {
    for (i = 1; i < (int)(sizeof(unsigned long) * 8); i++)
    {
      for (j = 0; j < i; j++)
      {
        if (i < 32)
        {
          bitspread[0] = (1 << i) | (1 << j);
        }
        else
        {
          bitspread[0] = (1 << (63 - i)) | (1 << (63 - j));
        }

        bitspread[1] = bitspread[0];
        bitspread[2] = ~bitspread[0];
        bitspread[3] = ~bitspread[0];

        ret = test_loop_size(bitspread, 4, addr, bufsize, 32, i);
        if (ret != 0)
        {
          if (!silent_console)
          {
            printf("  %s KO\n\r", __func__);
          }
          return 3;
        }
      }
    }

    if (test_loop_end(&loop, nb_loop))
    {
      break;
    }
  }

  return 0;
}

/**
* @brief test_bitflip.
* @par Test Description
*   Test Bit Flip pattern.
* @par Test Hardware Connection
* - None
* @par Required preconditions
* - None
* @par Expected result
* - None
* @par Called functions
* - xxx
* @par Used Peripherals
* - None,...
* @retval
*  0: Test passed
*  Value different from 0: Test failed
*  None(0xFF): if the result is deduced by the user: waveform, event...
*/
uint32_t DDR_Test_BitFlip(unsigned long size, unsigned long loop_in,
                          unsigned long addr_in)
{
  unsigned long bufsize;
  uint32_t nb_loop;
  uint32_t loop = 0;
  uintptr_t *addr = NULL;
  unsigned long bitflip[4];
  int i;
  int ret;

  if (get_buf_size(size, &bufsize, 4 * 1024, 32) != 0)
  {
    return 1;
  }

  get_nb_loop(loop_in, &nb_loop, 1);

  if (get_addr(addr_in, &addr) != 0)
  {
    return 2;
  }

  while (1)
  {
    for (i = 0; i < (int)(sizeof(unsigned long) * 8); i++)
    {
      bitflip[0] = 1 << i;
      bitflip[1] = bitflip[0];
      bitflip[2] = ~bitflip[0];
      bitflip[3] = bitflip[2];

      ret = test_loop_size(bitflip, 4, addr, bufsize, 32, i);
      if (ret != 0)
      {
        if (!silent_console)
        {
          printf("  %s KO\n\r", __func__);
        }
        return 3;
      }
    }

    if (test_loop_end(&loop, nb_loop))
    {
      break;
    }
  }

  return 0;
}

/**
* @brief test_walkbit0.
* @par Test Description
*   Test Walking Zeroes pattern
* @par Test Hardware Connection
* - None
* @par Required preconditions
* - None
* @par Expected result
* - None
* @par Called functions
* - xxx
* @par Used Peripherals
* - None,...
* @retval
*  0: Test passed
*  Value different from 0: Test failed
*  None(0xFF): if the result is deduced by the user: waveform, event...
*/
uint32_t DDR_Test_WalkingZeroes(unsigned long size, unsigned long loop_in,
                                unsigned long addr_in)
{
  unsigned long bufsize;
  uint32_t nb_loop;
  uint32_t loop = 0;
  uintptr_t *addr = NULL;
  unsigned long value;
  int i;
  int ret;
  int depth;

  if (get_buf_size(size, &bufsize, 4 * 1024, sizeof(unsigned long)) != 0)
  {
    return 1;
  }

  get_nb_loop(loop_in, &nb_loop, 1);

  if (get_addr(addr_in, &addr) != 0)
  {
    return 2;
  }

  depth = sizeof(unsigned long) * 8;

  while (1)
  {
    for (i = 0; i < depth * 2 ; i++)
    {
      if (i < depth)
      {
        value = 1 << i;
      }
      else
      {
        value = 1 << ((depth * 2) - 1 - i);
      }

      ret = test_loop_size(&value, 1, addr, bufsize, (depth * 2) -1, i);
      if (ret != 0)
      {
        if (!silent_console)
        {
          printf("  %s KO\n\r", __func__);
        }
        return 3;
      }
    }

    if (test_loop_end(&loop, nb_loop))
    {
      break;
    }
  }

  return 0;
}

/**
* @brief test_walkbit1.
* @par Test Description
*   Test Walking Ones pattern.
* @par Test Hardware Connection
* - None
* @par Required preconditions
* - None
* @par Expected result
* - None
* @par Called functions
* - xxx
* @par Used Peripherals
* - None,...
* @retval
*  0: Test passed
*  Value different from 0: Test failed
*  None(0xFF): if the result is deduced by the user: waveform, event...
*/
uint32_t DDR_Test_WalkingOnes(unsigned long size, unsigned long loop_in,
                              unsigned long addr_in)
{
  unsigned long bufsize;
  uint32_t nb_loop;
  uint32_t loop = 0;
  uintptr_t *addr = NULL;
  unsigned long value;
  int i;
  int ret;
  int depth;

  if (get_buf_size(size, &bufsize, 4 * 1024, sizeof(unsigned long)) != 0)
  {
    return 1;
  }

  get_nb_loop(loop_in, &nb_loop, 1);

  if (get_addr(addr_in, &addr) != 0)
  {
    return 2;
  }

  depth = sizeof(unsigned long) * 8;

  while (1)
  {
    for (i = 0; i < depth * 2; i++)
    {
      if (i < depth)
      {
        value = ~(1 << i);
      }
      else
      {
        value = ~(1 << ((depth * 2) - 1 - i));
      }

      ret = test_loop_size(&value, 1, addr, bufsize, (depth * 2) - 1, i);
      if (ret != 0)
      {
        if (!silent_console)
        {
          printf("  %s KO\n\r", __func__);
        }
        return 3;
      }
    }

    if (test_loop_end(&loop, nb_loop))
    {
      break;
    }
  }

  return 0;
}

#ifdef TEST_INFINITE_ENABLE
/**
* @brief test infinite write access to DDR
* @par Test Description
*   Continuous write the same pattern at the same address
* @par Test Hardware Connection
* - None
* @par Required preconditions
* - None
* @par Expected result
* - None
* @par Called functions
* - xxx
* @par Used Peripherals
* - None,...
* @retval
*  0: Test passed
*  Value different from 0: Test failed
*  None(0xFF): if the result is deduced by the user: waveform, event...
*/
uint32_t DDR_Test_Infinite_write(unsigned long pattern_in, unsigned long addr_in)
{
  uintptr_t *addr = NULL;
  unsigned long data;
  uint32_t loop = 0;
  uint32_t nb_loop = 0xFFFFFFFFU;
  int i, size = 1024 * 1024;
  bool random = false;
  volatile uint32_t go_loop = 1U;
  unsigned long dflt_pattern = 0xA5A5AA55AAAA5555;

  if (get_addr(addr_in, &addr) != 0)
  {
    return 1;
  }

  get_pattern(pattern_in, &data, dflt_pattern);

  if ((unsigned long)addr == 0xC8888888)
  {
    printf("running random\n\r");
    random = true;
  }
  else
  {
    printf("running at 0x%lx with pattern 0x%lx\n\r", (unsigned long)addr, data);
  }

  while (go_loop != 0U)
  {
    for (i = 0; i < size; i++)
    {
      if (random)
      {
        addr = (uintptr_t *)(unsigned long)((rand() & (DDR_MEM_SIZE - 1) & ~0x3));
        data = rand();
      }

      *addr = data;
    }

    if (test_loop_end(&loop, nb_loop))
    {
      break;
    }
  }

  return 0;
}

/**
* @brief test infinite read access to DDR
* @par Test Description
*   Continuous read the same pattern at the same address
* @par Test Hardware Connection
* - None
* @par Required preconditions
* - None
* @par Expected result
* - None
* @par Called functions
* - xxx
* @par Used Peripherals
* - None,...
* @retval
*  0: Test passed
*  Value different from 0: Test failed
*  None(0xFF): if the result is deduced by the user: waveform, event...
*/
uint32_t DDR_Test_Infinite_read(unsigned long pattern_in, unsigned long addr_in)
{
  uintptr_t *addr = NULL;
  unsigned long data;
  uint32_t loop = 0;
  uint32_t nb_loop = 0xFFFFFFFFU;
  int i, size = 1024 * 1024;
  bool random = false;
  volatile uint32_t go_loop = 1U;
  unsigned long dflt_pattern = 0xA5A5AA55AAAA5555;

  if (get_addr(addr_in, &addr) != 0)
  {
    return 1;
  }

  get_pattern(pattern_in, &data, dflt_pattern);

  if ((unsigned long)addr == 0xC8888888)
  {
    printf("running random\n\r");
    random = true;
  }
  else
  {
    printf("running at 0x%lx with pattern 0x%lx\n\r", (unsigned long)addr, data);
    *addr = data;
  }

  while (go_loop != 0U)
  {
    for (i = 0; i < size; i++)
    {
      if (random)
        addr = (uintptr_t *)(unsigned long)((rand() & (DDR_MEM_SIZE - 1) & ~0x3));

      data = *addr;
      printf("data @ address 0x%lx = 0x%lx \n\r", (unsigned long)addr, data);
    }

    if (test_loop_end(&loop, nb_loop))
    {
      break;
    }
  }

  return 0;
}
#endif

/*
 * Some impedance values can be excluded from DDR_Test_TXComputeDelayMargins
 * and DDR_Test_RXComputeDelayMargins scenarios.
 * Globally extreme values that brings high/low current intensities.
 * Min/Max indexes correspond to constant arrays defined in the DDR driver.
 */
#define MIN_TX_IDX 2 /* 30 Ohms */
#define MAX_TX_IDX 4 /* 60 Ohms */
#define MAX_TX_CASES (MAX_TX_IDX - MIN_TX_IDX + 1)
#define MIN_ODT_IDX 4  /* 37 Ohms */
#define MAX_ODT_IDX 14 /* 160 Ohms */
#define MAX_ODT_CASES (MAX_ODT_IDX - MIN_ODT_IDX + 1)
#if STM32MP_DDR3_TYPE || STM32MP_DDR4_TYPE
#if STM32MP_DDR3_TYPE
#define MIN_RTTNOM_IDX 0 /* disabled */
#define MAX_RTTNOM_IDX 5 /* 120 Ohms */
#else /* STM32MP_DDR4_TYPE */
#define MIN_RTTNOM_IDX 1 /* 34 Ohms */
#define MAX_RTTNOM_IDX 5 /* 80 Ohms */
#endif /* STM32MP_DDR3_TYPE */
#define MAX_RTTNOM_CASES (MAX_RTTNOM_IDX - MIN_RTTNOM_IDX + 1)
#define MAX_DATA_TX_CASES (MAX_TX_CASES * MAX_RTTNOM_CASES)
uint32_t data_tx_delay_margins[MAX_TX_CASES][MAX_RTTNOM_CASES] = { 0 };
#endif /* STM32MP_DDR3_TYPE || STM32MP_DDR4_TYPE */
#if STM32MP_DDR3_TYPE
#define MIN_RON_IDX 0 /* 34 Ohms */
#define MAX_RON_IDX 1 /* 40 Ohms */
#define MAX_RON_CASES (MAX_RON_IDX - MIN_RON_IDX + 1)
#define MAX_DATA_RX_CASES (MAX_ODT_CASES * MAX_RON_CASES)
uint32_t data_rx_delay_margins[MAX_ODT_CASES][MAX_RON_CASES] = { 0 };
#endif /* STM32MP_DDR3_TYPE */
#if STM32MP_DDR4_TYPE
#define MIN_ODI_IDX 0 /* 34 Ohms */
#define MAX_ODI_IDX 1 /* 48 Ohms */
#define MAX_ODI_CASES (MAX_ODI_IDX - MIN_ODI_IDX + 1)
#define MAX_DATA_RX_CASES (MAX_ODT_CASES * MAX_ODI_CASES)
uint32_t data_rx_delay_margins[MAX_ODT_CASES][MAX_ODI_CASES] = { 0 };
#endif /* STM32MP_DDR3_TYPE */
#if STM32MP_LPDDR4_TYPE
#define MIN_DQODT_IDX 1 /* 40 Ohms */
#define MAX_DQODT_IDX 4 /* 80 Ohms */
#define MAX_DQODT_CASES (MAX_DQODT_IDX - MIN_DQODT_IDX + 1)
#define MAX_DATA_TX_CASES (MAX_TX_CASES * MAX_DQODT_CASES)
uint32_t data_tx_delay_margins[MAX_TX_CASES][MAX_DQODT_CASES] = { 0 };
#define MIN_PDDS_IDX 0 /* 40 Ohms */
#define MAX_PDDS_IDX 3 /* 80 Ohms */
#define MAX_PDDS_CASES (MAX_PDDS_IDX - MIN_PDDS_IDX + 1)
#define MAX_DATA_RX_CASES (MAX_ODT_CASES * MAX_PDDS_CASES)
uint32_t data_rx_delay_margins[MAX_ODT_CASES][MAX_PDDS_CASES] = { 0 };
#endif /* STM32MP_LPDDR4_TYPE */

#ifdef STM32MP_DDR_16_BIT_INTERFACE
#define NB_DQS 2
#else /* !STM32MP_DDR_16_BIT_INTERFACE */
#define NB_DQS 4
#endif /* STM32MP_DDR_16_BIT_INTERFACE */

#define DDRPHYC_RXCLKDLY_OFFSET        0x40230
#define DDRPHYC_TXDQSDLY_OFFSET        0x40340
#define DDRPHYC_R_OFFSET               0x400
#define DDRPHYC_DBYTE_OFFSET           0x4000
#define DDRPHYC_MICROCONTMUXSEL_OFFSET 0x340000

static void Remote_Do_Step(HAL_DDR_InteractStepTypeDef step)
{
  /* Only manages STEP_DDR_READY <-> STEP_DDR_RESET transitions */
  remote_argc = 2;
  remote_cmd = 9; /* DDR_CMD_STEP */
  if (step == STEP_DDR_RESET)
  {
    remote_argv0[0] = '0'; /* STEP_DDR_RESET */
  }
  else
  {
    remote_argv0[0] = '3'; /* STEP_DDR_READY */
  }
  remote_argv0[1] = '\0';
  remote_argv1[0] = '\0';
}

static bool Remote_Do_Impedance_Write(char *name, uint32_t value)
{
  remote_argc = 3;
  remote_cmd = 5; /* DDR_CMD_IMPEDANCE */
  sprintf(remote_argv0, "%s%c", name, (char)'\0');
  sprintf(remote_argv1, "0x%08X", value);
  return HAL_DDR_Interactive(STEP_DDR_RESET);
}

static bool Remote_Do_Impedance_Read(char *name)
{
  remote_argc = 2;
  remote_cmd = 5; /* DDR_CMD_IMPEDANCE */
  sprintf(remote_argv0, "%s%c", name, (char)'\0');
  sprintf(remote_argv1, "%c", (char)'\0');
  return HAL_DDR_Interactive(STEP_DDR_READY);
}

static uint32_t read_txdqsdly(uint32_t idx)
{
  uint32_t result;
  WRITE_REG(*(volatile uint32_t *)(DDRPHYC_BASE + DDRPHYC_MICROCONTMUXSEL_OFFSET), 0U);
  result = READ_REG(*(volatile uint32_t *)(DDRPHYC_BASE + DDRPHYC_TXDQSDLY_OFFSET +
                                           (idx * DDRPHYC_DBYTE_OFFSET)));
  WRITE_REG(*(volatile uint32_t *)(DDRPHYC_BASE + DDRPHYC_MICROCONTMUXSEL_OFFSET), 1U);

  return result;
}

static void write_txdqsdly(uint32_t idx, uint32_t value)
{
  WRITE_REG(*(volatile uint32_t *)(DDRPHYC_BASE + DDRPHYC_MICROCONTMUXSEL_OFFSET), 0U);
  WRITE_REG(*(volatile uint32_t *)(DDRPHYC_BASE + DDRPHYC_TXDQSDLY_OFFSET +
                                   (idx * DDRPHYC_DBYTE_OFFSET)), value);
  WRITE_REG(*(volatile uint32_t *)(DDRPHYC_BASE + DDRPHYC_MICROCONTMUXSEL_OFFSET), 1U);
}

static uint32_t read_rxclkdly(uint32_t idx)
{
  uint32_t result;
  WRITE_REG(*(volatile uint32_t *)(DDRPHYC_BASE + DDRPHYC_MICROCONTMUXSEL_OFFSET), 0U);
  result = READ_REG(*(volatile uint32_t *)(DDRPHYC_BASE + DDRPHYC_RXCLKDLY_OFFSET +
                                           (idx * DDRPHYC_DBYTE_OFFSET)));
  WRITE_REG(*(volatile uint32_t *)(DDRPHYC_BASE + DDRPHYC_MICROCONTMUXSEL_OFFSET), 1U);

  return result;
}

static void write_rxclkdly(uint32_t idx, uint32_t value)
{
  WRITE_REG(*(volatile uint32_t *)(DDRPHYC_BASE + DDRPHYC_MICROCONTMUXSEL_OFFSET), 0U);
  WRITE_REG(*(volatile uint32_t *)(DDRPHYC_BASE + DDRPHYC_RXCLKDLY_OFFSET +
                                   (idx * DDRPHYC_DBYTE_OFFSET)), value);
  WRITE_REG(*(volatile uint32_t *)(DDRPHYC_BASE + DDRPHYC_MICROCONTMUXSEL_OFFSET), 1U);
}

/**
* @brief test_txcomputedelaymargins
* @par Test Description
*   Compute TX DQS delay margins with present impedances.
* @par Test Hardware Connection
* - None
* @par Required preconditions
* - None
* @par Expected result
* - None
* @par Called functions
* - xxx
* @par Used Peripherals
* - None,...
* @retval
*  0: Test passed
*  Value different from 0: Test failed
*  None(0xFF): if the result is deduced by the user: waveform, event...
*/
uint32_t DDR_Test_TXComputeDelayMargins(void)
{
  uint32_t idx = 0;
  uint32_t tx_idx = 0;
#if STM32MP_DDR3_TYPE || STM32MP_DDR4_TYPE
  uint32_t rttnom_idx = 0;
#else /* STM32MP_LPDDR4_TYPE */
  uint32_t dqodt_idx = 0;
#endif
  uint32_t dqs_idx;
  uint32_t ret = 0;

  save_test_id = 1;

  /*
   * Check if test has been interrupted to process step/impedance commands.
   * If yes, restore indexes and jump to continue processing.
   */
  if (stage_in_test != 0)
  {
    idx = save_idx;
    tx_idx = save_tx_idx;
#if STM32MP_DDR3_TYPE || STM32MP_DDR4_TYPE
    rttnom_idx = save_rttnom_idx;
#else /* STM32MP_LPDDR4_TYPE */
    dqodt_idx = save_dqodt_idx;
#endif
    if (stage_in_test == 1)
    {
      /* Jump after first step transition (STEP_DDR_READY -> STEP_DDR_RESET) */
      goto stage_in_tx_test_1;
    }
    if (stage_in_test == 2)
    {
      /* Jump after second step transition (STEP_DDR_RESET -> STEP_DDR_READY) */
      goto stage_in_tx_test_2;
    }
    if (stage_in_test == 3)
    {
      /* Jump after third step transition (STEP_DDR_READY -> STEP_DDR_RESET) */
      goto stage_in_tx_test_3;
    }
    if (stage_in_test == 4)
    {
      /* Jump after fourth step transition (STEP_DDR_RESET -> STEP_DDR_READY) */
      goto stage_in_tx_test_4;
    }
  }

  silent_console = true;

  /*
   * Read impedance values before starting the algorithm.
   * They will be restored after the processing.
   */
  if (!initial_done)
  {
    if (Remote_Do_Impedance_Read("TX"))
    {
      printf("impedance TX read error\n\r");
      ret = 1;
      goto txcomputedelaymargins_end;
    }
    initial_tx = remote_impedance;

#if STM32MP_DDR3_TYPE || STM32MP_DDR4_TYPE
    if (Remote_Do_Impedance_Read("RTTNOM"))
    {
      printf("impedance RTTNOM read error\n\r");
      ret = 1;
      goto txcomputedelaymargins_end;
    }
    initial_rttnom = remote_impedance;
#else /* STM32MP_LPDDR4_TYPE */
    if (Remote_Do_Impedance_Read("DQODT"))
    {
      printf("impedance DQODT read error\n\r");
      ret = 1;
      goto txcomputedelaymargins_end;
    }
    initial_dqodt = remote_impedance;
#endif /* STM32MP_DDR3_TYPE || STM32MP_DDR4_TYPE */

    initial_done = true;
  }

  for (tx_idx = MIN_TX_IDX; tx_idx <= MAX_TX_IDX; tx_idx++)
  {
#if STM32MP_DDR3_TYPE || STM32MP_DDR4_TYPE
    for (rttnom_idx = MIN_RTTNOM_IDX; rttnom_idx <= MAX_RTTNOM_IDX; rttnom_idx++)
#else /* STM32MP_LPDDR4_TYPE */
    for (dqodt_idx = MIN_DQODT_IDX; dqodt_idx <= MAX_DQODT_IDX; dqodt_idx++)
#endif
    {
#if STM32MP_DDR3_TYPE || STM32MP_DDR4_TYPE
      idx = ((tx_idx - MIN_TX_IDX) * MAX_RTTNOM_CASES) + (rttnom_idx - MIN_RTTNOM_IDX);
      printf("case %02u/%02u:  TX = %03u Ohms, RTTNOM = %03u Ohms\n\r",
	     idx + 1, MAX_DATA_TX_CASES, tx_ohms[tx_idx], rttnom_ohms[rttnom_idx]);
#else /* STM32MP_LPDDR4_TYPE */
      idx = ((tx_idx - MIN_TX_IDX) * MAX_DQODT_CASES) + (dqodt_idx - MIN_DQODT_IDX);
      printf("case %02u/%02u:  TX = %03u Ohms, DQODT = %03u Ohms\n\r",
	     idx + 1, MAX_DATA_TX_CASES, tx_ohms[tx_idx], dqodt_ohms[dqodt_idx]);
#endif

      /* Go to RESET state, necessary to change settings */
      Remote_Do_Step(STEP_DDR_RESET);

      /* Save indexes before the next execution break during step change */
      save_idx = idx;
      save_tx_idx = tx_idx;
#if STM32MP_DDR3_TYPE || STM32MP_DDR4_TYPE
      save_rttnom_idx = rttnom_idx;
#else /* STM32MP_LPDDR4_TYPE */
      save_dqodt_idx = dqodt_idx;
#endif
      stage_in_test = 1;
      return 0;

stage_in_tx_test_1:
      stage_in_test = 0;

      /* Set all DATA TX impedances according to loop indexes */
#if STM32MP_DDR3_TYPE || STM32MP_DDR4_TYPE
      if (!Remote_Do_Impedance_Write("TX", tx_ohms[tx_idx]) ||
          !Remote_Do_Impedance_Write("RTTNOM", rttnom_ohms[rttnom_idx]))
#else /* STM32MP_LPDDR4_TYPE */
      if (!Remote_Do_Impedance_Write("TX", tx_ohms[tx_idx]) ||
          !Remote_Do_Impedance_Write("DQODT", dqodt_ohms[dqodt_idx]))
#endif /* STM32MP_DDR3_TYPE || STM32MP_DDR4_TYPE */
      {
        printf("impedance write error\n\r");
        ret = 1;
        goto txcomputedelaymargins_end;
      }

      /* Go back to READY state, necessary to make tests */
      Remote_Do_Step(STEP_DDR_READY);

      /* New execution break during step change */
      stage_in_test = 2;
      return 0;

stage_in_tx_test_2:
      stage_in_test = 0;

      if (DDR_Test_All(0, 0, 0) != 0)
      {
        printf("current impedance configuration not functional\n\r");
        continue;
      }

#if STM32MP_DDR3_TYPE || STM32MP_DDR4_TYPE
      data_tx_delay_margins[tx_idx - MIN_TX_IDX][rttnom_idx - MIN_RTTNOM_IDX] = 0;
#else /* STM32MP_LPDDR4_TYPE */
      data_tx_delay_margins[tx_idx - MIN_TX_IDX][dqodt_idx - MIN_DQODT_IDX] = 0;
#endif

      for (dqs_idx = 0; dqs_idx < NB_DQS; dqs_idx++)
      {
        uint32_t txdqsdly;
        uint32_t inc_dly_offset = 0;
        uint32_t dec_dly_offset = 0;
        bool failure_detected = false;

        /* read initial DQS delay generated by the training */
        txdqsdly = read_txdqsdly(dqs_idx);

        /* Get delay margin by incrementation */
        do
        {
          inc_dly_offset += 2;
          write_txdqsdly(dqs_idx, txdqsdly + inc_dly_offset);

          valid_delay_us(1000);

          if (DDR_Test_All(0, 0, 0) != 0)
          {
            inc_dly_offset--;
            write_txdqsdly(dqs_idx, txdqsdly + inc_dly_offset);

            if (DDR_Test_All(0, 0, 0) != 0)
            {
              inc_dly_offset--;
            }

            failure_detected = true;
          }
        }
        while (!failure_detected);

        /* restore initial DQS delay generated by the training */
        write_txdqsdly(dqs_idx, txdqsdly);

        failure_detected = false;

        /* Get delay margin by decrementation */
        do
        {
          dec_dly_offset += 2;
          write_txdqsdly(dqs_idx, txdqsdly - dec_dly_offset);

          valid_delay_us(1000);

          if (DDR_Test_All(0, 0, 0) != 0)
          {
            dec_dly_offset--;
            write_txdqsdly(dqs_idx, txdqsdly - dec_dly_offset);

            if (DDR_Test_All(0, 0, 0) != 0)
            {
              dec_dly_offset--;
            }

            failure_detected = true;
          }
        }
        while (!failure_detected);

        /* restore initial DQS delay generated by the training */
        write_txdqsdly(dqs_idx, txdqsdly);

#if STM32MP_DDR3_TYPE || STM32MP_DDR4_TYPE
        if (inc_dly_offset > dec_dly_offset)
        {
          data_tx_delay_margins[tx_idx - MIN_TX_IDX][rttnom_idx - MIN_RTTNOM_IDX] +=
		  dec_dly_offset;
        }
        else
        {
          data_tx_delay_margins[tx_idx - MIN_TX_IDX][rttnom_idx - MIN_RTTNOM_IDX] +=
		  inc_dly_offset;
        }
#else /* STM32MP_LPDDR4_TYPE */
        if (inc_dly_offset > dec_dly_offset)
        {
          data_tx_delay_margins[tx_idx - MIN_TX_IDX][dqodt_idx - MIN_DQODT_IDX] +=
		  dec_dly_offset;
        }
        else
        {
          data_tx_delay_margins[tx_idx - MIN_TX_IDX][dqodt_idx - MIN_DQODT_IDX] +=
		  inc_dly_offset;
        }
#endif /* STM32MP_DDR3_TYPE || STM32MP_DDR4_TYPE */
      } /* dqs_idx */
    } /* rttnom_idx or dqodt_idx */
  } /* tx_idx */

  /* Display results in a 2D eye diagram */
  printf ("\n\r TX  |\n\r");
  for (tx_idx = MIN_TX_IDX; tx_idx <= (MAX_TX_IDX + 3); tx_idx++)
  {
    char output[64] = "\0";

    if (tx_idx >= (MAX_TX_IDX + 1))
    {
      sprintf(output, "     ");

      if (tx_idx >= (MAX_TX_IDX + 2))
      {
        sprintf(output + strlen(output), " ");
      }
      else
      {
        sprintf(output + strlen(output), "|");
      }
    }
    else
    {
      sprintf(output, " %03u |", tx_ohms[tx_idx]);
    }

#if STM32MP_DDR3_TYPE || STM32MP_DDR4_TYPE
    for (rttnom_idx = MIN_RTTNOM_IDX; rttnom_idx <= MAX_RTTNOM_IDX; rttnom_idx++)
    {
      if (tx_idx == (MAX_TX_IDX + 3))
      {
        sprintf(output + strlen(output), " RTTNOM");
	break;
      }
      else if (tx_idx == (MAX_TX_IDX + 2))
      {
        sprintf(output + strlen(output), " %03u  ", rttnom_ohms[rttnom_idx]);
      }
      else if (tx_idx == (MAX_TX_IDX + 1))
      {
        sprintf(output + strlen(output), "______");
      }
      else
      {
        sprintf(output + strlen(output), "  %02u  ",
                data_tx_delay_margins[tx_idx - MIN_TX_IDX][rttnom_idx - MIN_RTTNOM_IDX]);
      }
    } /* rttnom_idx */
#else /* STM32MP_LPDDR4_TYPE */
    for (dqodt_idx = MIN_DQODT_IDX; dqodt_idx <= MAX_DQODT_IDX; dqodt_idx++)
    {
      if (tx_idx == (MAX_TX_IDX + 3))
      {
        sprintf(output + strlen(output), " DQODT");
	break;
      }
      else if (tx_idx == (MAX_TX_IDX + 2))
      {
        sprintf(output + strlen(output), " %03u  ", dqodt_ohms[dqodt_idx]);
      }
      else if (tx_idx == (MAX_TX_IDX + 1))
      {
        sprintf(output + strlen(output), "______");
      }
      else
      {
        sprintf(output + strlen(output), "  %02u  ",
                data_tx_delay_margins[tx_idx - MIN_TX_IDX][dqodt_idx - MIN_DQODT_IDX]);
      }
    } /* dqodt_idx */
#endif
    printf("%s\n\r", output);
  } /* tx_idx */

  /* Restore initial impedance values */
  /* Go to RESET state, necessary to change settings */
  Remote_Do_Step(STEP_DDR_RESET);

  stage_in_test = 3;
  return 0;

stage_in_tx_test_3:
  stage_in_test = 0;

#if STM32MP_DDR3_TYPE || STM32MP_DDR4_TYPE
  if (!Remote_Do_Impedance_Write("TX", initial_tx) ||
      !Remote_Do_Impedance_Write("RTTNOM", initial_rttnom))
#else /* STM32MP_LPDDR4_TYPE */
  if (!Remote_Do_Impedance_Write("TX", initial_tx) ||
      !Remote_Do_Impedance_Write("DQODT", initial_dqodt))
#endif /* STM32MP_DDR3_TYPE || STM32MP_DDR4_TYPE */
  {
    printf("impedance write error\n\r");
    ret = 1;
    goto txcomputedelaymargins_end;
  }

  /* Go back to READY state, necessary to make tests */
  Remote_Do_Step(STEP_DDR_READY);

  /* New execution break during step change */
  stage_in_test = 4;
  return 0;

stage_in_tx_test_4:
  stage_in_test = 0;

txcomputedelaymargins_end:
  silent_console = false;
  save_test_id = 0;
  initial_done = false;

  return ret;
}

/**
 * @brief test_rxcomputedelaymargins
 * @par Test Description
 *   Compute RX DQS to RxClk delay margins with present impedances.
 * @par Test Hardware Connection
 * - None
 * @par Required preconditions
 * - None
 * @par Expected result
 * - None
 * @par Called functions
 * - xxx
 * @par Used Peripherals
 * - None,...
 * @retval
 *  0: Test passed
 *  Value different from 0: Test failed
 *  None(0xFF): if the result is deduced by the user: waveform, event...
*/
uint32_t DDR_Test_RXComputeDelayMargins(void)
{
  uint32_t idx = 0;
  uint32_t odt_idx = 0;
#if STM32MP_DDR3_TYPE
  uint32_t ron_idx = 0;
#endif /* STM32MP_DDR3_TYPE */
#if STM32MP_DDR4_TYPE
  uint32_t odi_idx = 0;
#endif /* STM32MP_DDR4_TYPE */
#if STM32MP_LPDDR4_TYPE
  uint32_t pdds_idx = 0;
#endif /* STM32MP_LPDDR4_TYPE */
  uint32_t dqs_idx;
  uint32_t ret = 0;

  save_test_id = 2;

  /*
   * Check if test has been interrupted to process step/impedance commands.
   * If yes, restore indexes and jump to continue processing.
   */
  if (stage_in_test != 0)
  {
    idx = save_idx;
    odt_idx = save_odt_idx;
#if STM32MP_DDR3_TYPE
    ron_idx = save_ron_idx;
#endif /* STM32MP_DDR3_TYPE */
#if STM32MP_DDR4_TYPE
    odi_idx = save_odi_idx;
#endif /* STM32MP_DDR4_TYPE */
#if STM32MP_LPDDR4_TYPE
    pdds_idx = save_pdds_idx;
#endif /* STM32MP_LPDDR4_TYPE */
    if (stage_in_test == 1)
    {
      /* Jump after first step transition (STEP_DDR_READY -> STEP_DDR_RESET) */
      goto stage_in_rx_test_1;
    }
    if (stage_in_test == 2)
    {
      /* Jump after second step transition (STEP_DDR_RESET -> STEP_DDR_READY) */
      goto stage_in_rx_test_2;
    }
    if (stage_in_test == 3)
    {
      /* Jump after third step transition (STEP_DDR_READY -> STEP_DDR_RESET) */
      goto stage_in_rx_test_3;
    }
    if (stage_in_test == 4)
    {
      /* Jump after fourth step transition (STEP_DDR_RESET -> STEP_DDR_READY) */
      goto stage_in_rx_test_4;
    }
  }

  silent_console = true;

  /*
   * Read impedance values before starting the algorithm.
   * They will be restored after the processing.
   */
  if (!initial_done)
  {
    if (Remote_Do_Impedance_Read("ODT"))
    {
      printf("impedance ODT read error\n\r");
      ret = 1;
      goto rxcomputedelaymargins_end;
    }
    initial_odt = remote_impedance;

#if STM32MP_DDR3_TYPE
    if (Remote_Do_Impedance_Read("RON"))
    {
      printf("impedance RON read error\n\r");
      ret = 1;
      goto rxcomputedelaymargins_end;
    }
    initial_ron = remote_impedance;
#endif /* STM32MP_DDR3_TYPE */
#if STM32MP_DDR4_TYPE
    if (Remote_Do_Impedance_Read("ODI"))
    {
      printf("impedance ODI read error\n\r");
      ret = 1;
      goto rxcomputedelaymargins_end;
    }
    initial_odi = remote_impedance;
#endif /* STM32MP_DDR4_TYPE */
#if STM32MP_LPDDR4_TYPE
    if (Remote_Do_Impedance_Read("PDDS"))
    {
      printf("impedance PDDS read error\n\r");
      ret = 1;
      goto rxcomputedelaymargins_end;
    }
    initial_pdds = remote_impedance;
#endif /* STM32MP_LPDDR4_TYPE */

    initial_done = true;
  }

  for (odt_idx = MIN_ODT_IDX; odt_idx <= MAX_ODT_IDX; odt_idx++)
  {
#if STM32MP_DDR3_TYPE
    for (ron_idx = MIN_RON_IDX; ron_idx <= MAX_RON_IDX; ron_idx++)
#endif /* STM32MP_DDR3_TYPE */
#if STM32MP_DDR4_TYPE
    for (odi_idx = MIN_ODI_IDX; odi_idx <= MAX_ODI_IDX; odi_idx++)
#endif /* STM32MP_DDR4_TYPE */
#if STM32MP_LPDDR4_TYPE
    for (pdds_idx = MIN_PDDS_IDX; pdds_idx <= MAX_PDDS_IDX; pdds_idx++)
#endif /* STM32MP_LPDDR4_TYPE */
    {
#if STM32MP_DDR3_TYPE
      idx = ((odt_idx - MIN_ODT_IDX) * MAX_RON_CASES) + (ron_idx - MIN_RON_IDX);
      printf("case %02u/%02u:  ODT = %03u Ohms, RON = %03u Ohms\n\r",
	     idx + 1, MAX_DATA_RX_CASES, odt_ohms[odt_idx], ron_ohms[ron_idx]);
#endif /* STM32MP_DDR3_TYPE */
#if STM32MP_DDR4_TYPE
      idx = ((odt_idx - MIN_ODT_IDX) * MAX_ODI_CASES) + (odi_idx - MIN_ODI_IDX);
      printf("case %02u/%02u:  ODT = %03u Ohms, ODI = %03u Ohms\n\r",
	     idx + 1, MAX_DATA_RX_CASES, odt_ohms[odt_idx], odi_ohms[odi_idx]);
#endif /* STM32MP_DDR4_TYPE */
#if STM32MP_LPDDR4_TYPE
      idx = ((odt_idx - MIN_ODT_IDX) * MAX_PDDS_CASES) + (pdds_idx - MIN_PDDS_IDX);
      printf("case %02u/%02u:  ODT = %03u Ohms, PDDS = %03u Ohms\n\r",
	     idx + 1, MAX_DATA_RX_CASES, odt_ohms[odt_idx], pdds_ohms[pdds_idx]);
#endif /* STM32MP_LPDDR4_TYPE */

      /* Go to RESET state, necessary to change settings */
      Remote_Do_Step(STEP_DDR_RESET);

      /* Save indexes before the next execution break during step change */
      save_idx = idx;
      save_odt_idx = odt_idx;
#if STM32MP_DDR3_TYPE
      save_ron_idx = ron_idx;
#endif /* STM32MP_DDR3_TYPE */
#if STM32MP_DDR4_TYPE
      save_odi_idx = odi_idx;
#endif /* STM32MP_DDR4_TYPE */
#if STM32MP_LPDDR4_TYPE
      save_pdds_idx = pdds_idx;
#endif /* STM32MP_LPDDR4_TYPE */
      stage_in_test = 1;
      return 0;

stage_in_rx_test_1:
      stage_in_test = 0;

      /* Set all DATA RX impedances according to loop indexes */
#if STM32MP_DDR3_TYPE
      if (!Remote_Do_Impedance_Write("ODT", odt_ohms[odt_idx]) ||
          !Remote_Do_Impedance_Write("RON", ron_ohms[ron_idx]))
#endif /* STM32MP_DDR3_TYPE */
#if STM32MP_DDR4_TYPE
      if (!Remote_Do_Impedance_Write("ODT", odt_ohms[odt_idx]) ||
          !Remote_Do_Impedance_Write("ODI", odi_ohms[odi_idx]))
#endif /* STM32MP_DDR4_TYPE */
#if STM32MP_LPDDR4_TYPE
      if (!Remote_Do_Impedance_Write("ODT", odt_ohms[odt_idx]) ||
          !Remote_Do_Impedance_Write("PDDS", pdds_ohms[pdds_idx]))
#endif /* STM32MP_LPDDR4_TYPE */
      {
        printf("impedance write error\n\r");
        ret = 1;
        goto rxcomputedelaymargins_end;
      }

      /* Go back to READY state, necessary to make tests */
      Remote_Do_Step(STEP_DDR_READY);

      /* New execution break during step change */
      stage_in_test = 2;
      return 0;

stage_in_rx_test_2:
      stage_in_test = 0;

      if (DDR_Test_All(0, 0, 0) != 0)
      {
        printf("current impedance configuration not functional\n\r");
        continue;
      }

#if STM32MP_DDR3_TYPE
      data_rx_delay_margins[odt_idx - MIN_ODT_IDX][ron_idx - MIN_RON_IDX] = 0;
#endif /* STM32MP_DDR3_TYPE */
#if STM32MP_DDR4_TYPE
      data_rx_delay_margins[odt_idx - MIN_ODT_IDX][odi_idx - MIN_ODI_IDX] = 0;
#endif /* STM32MP_DDR4_TYPE */
#if STM32MP_LPDDR4_TYPE
      data_rx_delay_margins[odt_idx - MIN_ODT_IDX][pdds_idx - MIN_PDDS_IDX] = 0;
#endif /* STM32MP_LPDDR4_TYPE */

      for (dqs_idx = 0; dqs_idx < NB_DQS; dqs_idx++)
      {
        uint32_t rxclkdly;
        uint32_t inc_dly_offset = 0;
        uint32_t dec_dly_offset = 0;
        bool failure_detected = false;

        /* read initial DQS delay generated by the training */
        rxclkdly = read_rxclkdly(dqs_idx);

        /* Get delay margin by incrementation */
        do
        {
          inc_dly_offset += 2;
          write_rxclkdly(dqs_idx, rxclkdly + inc_dly_offset);

          valid_delay_us(1000);

          if (DDR_Test_All(0, 0, 0) != 0)
          {
            inc_dly_offset--;
            write_rxclkdly(dqs_idx, rxclkdly + inc_dly_offset);

            if (DDR_Test_All(0, 0, 0) != 0)
            {
              inc_dly_offset--;
            }

            failure_detected = true;
          }
        }
        while (!failure_detected);

        /* restore initial DQS delay generated by the training */
        write_rxclkdly(dqs_idx, rxclkdly);

        failure_detected = false;

        /* Get delay margin by decrementation */
        do
        {
          dec_dly_offset += 2;
          write_rxclkdly(dqs_idx, rxclkdly - dec_dly_offset);

          valid_delay_us(1000);

          if (DDR_Test_All(0, 0, 0) != 0)
          {
            dec_dly_offset--;
            write_rxclkdly(dqs_idx, rxclkdly - dec_dly_offset);

            if (DDR_Test_All(0, 0, 0) != 0)
            {
              dec_dly_offset--;
            }

            failure_detected = true;
          }
        }
        while (!failure_detected);

        /* restore initial DQS delay generated by the training */
        write_rxclkdly(dqs_idx, rxclkdly);

#if STM32MP_DDR3_TYPE
        if (inc_dly_offset > dec_dly_offset)
        {
          data_rx_delay_margins[odt_idx - MIN_ODT_IDX][ron_idx - MIN_RON_IDX] +=
		  dec_dly_offset;
        }
        else
        {
          data_rx_delay_margins[odt_idx - MIN_ODT_IDX][ron_idx - MIN_RON_IDX] +=
		  inc_dly_offset;
        }
#endif /* STM32MP_DDR3_TYPE */
#if STM32MP_DDR4_TYPE
        if (inc_dly_offset > dec_dly_offset)
        {
          data_rx_delay_margins[odt_idx - MIN_ODT_IDX][odi_idx - MIN_ODI_IDX] +=
		  dec_dly_offset;
        }
        else
        {
          data_rx_delay_margins[odt_idx - MIN_ODT_IDX][odi_idx - MIN_ODI_IDX] +=
		  inc_dly_offset;
        }
#endif /* STM32MP_DDR4_TYPE */
#if STM32MP_LPDDR4_TYPE
        if (inc_dly_offset > dec_dly_offset)
        {
          data_rx_delay_margins[odt_idx - MIN_ODT_IDX][pdds_idx - MIN_PDDS_IDX] +=
		  dec_dly_offset;
        }
        else
        {
          data_rx_delay_margins[odt_idx - MIN_ODT_IDX][pdds_idx - MIN_PDDS_IDX] +=
		  inc_dly_offset;
        }
#endif /* STM32MP_LPDDR4_TYPE */
      } /* dqs_idx */
    } /* ron_idx or odi_idx or pdds_idx */
  } /* odt_idx */

  /* Display results in a 2D eye diagram */
  printf ("\n\r ODT |\n\r");
  for (odt_idx = MIN_ODT_IDX; odt_idx <= (MAX_ODT_IDX + 3); odt_idx++)
  {
    char output[64] = "\0";

    if (odt_idx >= (MAX_ODT_IDX + 1))
    {
      sprintf(output, "     ");

      if (odt_idx >= (MAX_ODT_IDX + 2))
      {
        sprintf(output + strlen(output), " ");
      }
      else
      {
        sprintf(output + strlen(output), "|");
      }
    }
    else
    {
      sprintf(output, " %03u |", odt_ohms[odt_idx]);
    }

#if STM32MP_DDR3_TYPE
    for (ron_idx = MIN_RON_IDX; ron_idx <= MAX_RON_IDX; ron_idx++)
    {
      if (odt_idx == (MAX_ODT_IDX + 3))
      {
        sprintf(output + strlen(output), " RON");
	break;
      }
      else if (odt_idx == (MAX_ODT_IDX + 2))
      {
        sprintf(output + strlen(output), " %03u  ", ron_ohms[ron_idx]);
      }
      else if (odt_idx == (MAX_ODT_IDX + 1))
      {
        sprintf(output + strlen(output), "______");
      }
      else
      {
        sprintf(output + strlen(output), "  %02u  ",
                data_rx_delay_margins[odt_idx - MIN_ODT_IDX][ron_idx - MIN_RON_IDX]);
      }
    } /* ron_idx */
#endif /* STM32MP_DDR3_TYPE */
#if STM32MP_DDR4_TYPE
    for (odi_idx = MIN_ODI_IDX; odi_idx <= MAX_ODI_IDX; odi_idx++)
    {
      if (odt_idx == (MAX_ODT_IDX + 3))
      {
        sprintf(output + strlen(output), " ODI");
	break;
      }
      else if (odt_idx == (MAX_ODT_IDX + 2))
      {
        sprintf(output + strlen(output), " %03u  ", odi_ohms[odi_idx]);
      }
      else if (odt_idx == (MAX_ODT_IDX + 1))
      {
        sprintf(output + strlen(output), "______");
      }
      else
      {
        sprintf(output + strlen(output), "  %02u  ",
                data_rx_delay_margins[odt_idx - MIN_ODT_IDX][odi_idx - MIN_ODI_IDX]);
      }
    } /* odi_idx */
#endif /* STM32MP_DDR4_TYPE */
#if STM32MP_LPDDR4_TYPE
    for (pdds_idx = MIN_PDDS_IDX; pdds_idx <= MAX_PDDS_IDX; pdds_idx++)
    {
      if (odt_idx == (MAX_ODT_IDX + 3))
      {
        sprintf(output + strlen(output), " PDDS");
	break;
      }
      else if (odt_idx == (MAX_ODT_IDX + 2))
      {
        sprintf(output + strlen(output), " %03u  ", pdds_ohms[pdds_idx]);
      }
      else if (odt_idx == (MAX_ODT_IDX + 1))
      {
        sprintf(output + strlen(output), "______");
      }
      else
      {
        sprintf(output + strlen(output), "  %02u  ",
                data_rx_delay_margins[odt_idx - MIN_ODT_IDX][pdds_idx - MIN_PDDS_IDX]);
      }
    } /* pdds_idx */
#endif /* STM32MP_LPDDR4_TYPE */
    printf("%s\n\r", output);
  } /* odt_idx */

  /* Restore initial impedance values */
  /* Go to RESET state, necessary to change settings */
  Remote_Do_Step(STEP_DDR_RESET);

  stage_in_test = 3;
  return 0;

stage_in_rx_test_3:
  stage_in_test = 0;

#if STM32MP_DDR3_TYPE
  if (!Remote_Do_Impedance_Write("ODT", initial_odt) ||
      !Remote_Do_Impedance_Write("RON", initial_ron))
#endif /* STM32MP_DDR3_TYPE */
#if STM32MP_DDR4_TYPE
  if (!Remote_Do_Impedance_Write("ODT", initial_odt) ||
      !Remote_Do_Impedance_Write("ODI", initial_odi))
#endif /* STM32MP_DDR4_TYPE */
#if STM32MP_LPDDR4_TYPE
  if (!Remote_Do_Impedance_Write("ODT", initial_odt) ||
      !Remote_Do_Impedance_Write("PDDS", initial_pdds))
#endif /* STM32MP_LPDDR4_TYPE */
  {
    printf("impedance write error\n\r");
    ret = 1;
    goto rxcomputedelaymargins_end;
  }

  /* Go back to READY state, necessary to make tests */
  Remote_Do_Step(STEP_DDR_READY);

  /* New execution break during step change */
  stage_in_test = 4;
  return 0;

stage_in_rx_test_4:
  stage_in_test = 0;

rxcomputedelaymargins_end:
  silent_console = false;
  save_test_id = 0;
  initial_done = false;

  return ret;
}
