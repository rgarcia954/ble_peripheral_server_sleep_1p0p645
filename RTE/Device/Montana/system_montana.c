/**
 * @file system_montana.c
 * @brief CMSIS Cortex-M33 Device Peripheral Access Layer Source File
 * 
 * Provides the required system_<Device>.c implementation for CMSIS
 * compatibility
 * This trace feature allows the selection of tracing to multiple different
 * output devices by linking in separate libraries rather than changing the
 * application code.
 *
 * @copyright @parblock
 * Copyright (c) 2020 Semiconductor Components Industries, LLC (d/b/a
 * ON Semiconductor), All Rights Reserved
 *
 * This code is the property of ON Semiconductor and may not be redistributed
 * in any form without prior written permission from ON Semiconductor.
 * The terms of use and warranty for this code are covered by contractual
 * agreements between ON Semiconductor and the licensee.
 *
 * This is Reusable Code.
 * @endparblock
 */
 
#include <stdint.h>
#include <hw.h>
#include "system_montana.h"
#include "flash_rom.h"
#if defined (__ARM_FEATURE_CMSE) &&  (__ARM_FEATURE_CMSE == 3U)
  #include "partition_ARMCM33.h"
#endif


/* ----------------------------------------------------------------------------
 * Variables
 * ------------------------------------------------------------------------- */
/* Required system clock frequency variable (Cortex-M33 clock frequency) */
uint32_t SystemCoreClock __attribute__((section(".systemclock"))) = DEFAULT_FREQ;


/*----------------------------------------------------------------------------
  Forward Declarations for helper functions
 *----------------------------------------------------------------------------*/
static uint32_t GetSystemClockfromRCCLK(void);
static uint32_t GetSystemClockfromSTANDBYCLK(void);
static uint32_t GetSystemClockfromRFCLK(void);
static uint32_t GetSystemClockfromJTCK(void);
static uint32_t GetSystemClockfromUnknownCLK(void);


/*----------------------------------------------------------------------------
  System initialization function
 *----------------------------------------------------------------------------*/
#ifndef NON_SECURE
void SystemInit(void)
{
    Sys_NVIC_DisableAllInt(); // Disable all interrupts
    Sys_NVIC_ClearAllPendingInt(); // Clear all pending interrupts

    __set_PRIMASK(PRIMASK_DISABLE_INTERRUPTS);
    __set_FAULTMASK(FAULTMASK_DISABLE_INTERRUPTS);

#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
    SCB->VTOR = (unsigned int) (&ISR_Vector_Table);
#endif

#if defined (__FPU_USED) && (__FPU_USED == 1U)
    SCB->CPACR |= ((3U << 10U*2U) |           /* enable CP10 Full Access */
                 (3U << 11U*2U)  );           /* enable CP11 Full Access */
#endif

#ifdef UNALIGNED_SUPPORT_DISABLE
    SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
#endif

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
    TZ_SAU_Setup();
#endif

    /* Update SystemCoreClock by reading system registers 
       also update the flash registers */
    SystemCoreClockUpdate();
}
#endif

#ifdef NON_SECURE
void SystemInit(void)
{
	__set_PRIMASK(PRIMASK_DISABLE_INTERRUPTS);
	__set_FAULTMASK(FAULTMASK_DISABLE_INTERRUPTS);

#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
	SCB->VTOR = (unsigned int) (&ISR_Vector_Table);
#endif

#if defined (__FPU_USED) && (__FPU_USED == 1U)
	SCB->CPACR |= ((3U << 10U*2U) |           /* enable CP10 Full Access */
				   (3U << 11U*2U));           /* enable CP11 Full Access */
#endif
}
#endif

/*----------------------------------------------------------------------------
  System Core Clock update function
 *----------------------------------------------------------------------------*/

void SystemCoreClockUpdate(void)
{
    uint32_t clk_src_sel = (CLK->SYS_CFG & CLK_SYS_CFG_SYSCLK_SRC_SEL_Mask);
    
    switch (clk_src_sel)
    {
        case SYSCLK_CLKSRC_RCCLK:
            SystemCoreClock = GetSystemClockfromRCCLK();
        break;
        case SYSCLK_CLKSRC_STANDBYCLK:
            SystemCoreClock = GetSystemClockfromSTANDBYCLK();
        break;
        case SYSCLK_CLKSRC_RFCLK:
            SystemCoreClock = GetSystemClockfromRFCLK();
        break;
        case SYSCLK_CLKSRC_JTCK:
            SystemCoreClock = GetSystemClockfromJTCK();
        break;
        default:
             SystemCoreClock = GetSystemClockfromUnknownCLK();
        break;
    }

    /* Initialize the delay settings for both flash interfaces (if they are
     * enabled), to ensure proper operation when using the new clock setting. */
    for(int i = 0; i < 2; i++)
    {
        if ((FLASH[i].IF_STATUS & (0x1U << FLASH_IF_STATUS_ISOLATE_STATUS_Pos)) !=
             FLASH_ISOLATE)
        {
             Flash_Initialize(i, SystemCoreClock);
        }
    }
    return;
}

/* Get clock frequency from internal RC oscillator (3/12/24/48 MHz steps)  */
static uint32_t GetSystemClockfromRCCLK(void)
{
    uint32_t clk = DEFAULT_FREQ;
    uint32_t rc_fsel = ACS->RCOSC_CTRL & ACS_RCOSC_CTRL_RC_FSEL_Mask;
    if(rc_fsel == RC_OSC_3MHZ)
    {
        clk = 3000000;
    }
    else if(rc_fsel == RC_OSC_12MHZ)
    {
        clk = 12000000;
    }
    else if(rc_fsel == RC_OSC_24MHZ)
    {
        clk = 24000000;
    }
    else if(rc_fsel == RC_OSC_48MHZ)
    {
        clk = 48000000;
    }
    else
    {
        /* Nothing to do here */
    }
    return clk;
}

/* Get clock frequency from low frequency 32 Khz internal oscillator */
static uint32_t GetSystemClockfromSTANDBYCLK(void)
{
    /* SYSCLK is source from the standby oscillator or 32 kHz crystal.
     * We assume 32768 is close enough in this case, since there isn't
     * significant variation for this value. */
    return STANDBYCLK_DEFAULT_FREQ;
}

static uint32_t GetSystemClockfromRFCLK(void)
{
    uint32_t clk = DEFAULT_FREQ;
    if(RF0_REG33->CK_DIV_1_6_CK_DIV_1_6_BYTE > 0 )
    {
        clk = RFCLK_BASE_FREQ /
            (RF0_REG33->CK_DIV_1_6_CK_DIV_1_6_BYTE);
    }
    else
    {
        /* Nothing to do here. Clock is not generated from RFCLK */
    }
    return clk;
}

/* Get clock external JTAG clock oscillator */
static uint32_t GetSystemClockfromJTCK(void)
{
    /* @ref JTCK_MAX_FREQ is assumed to be the JTCK/SWCLK clock
       update this definition for a different clock */
    return JTCK_MAX_FREQ / (CLK_SYS_CFG->SWCLK_PRESCALE_BYTE + 1);
}

static uint32_t GetSystemClockfromUnknownCLK(void)
{
    return DEFAULT_FREQ;
}
