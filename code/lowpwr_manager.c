/**
 * @file lowpwr_manager.c
 *
 * @copyright @parblock
 * Copyright (c) 2021 Semiconductor Components Industries, LLC (d/b/a
 * onsemi), All Rights Reserved
 *
 * This code is the property of onsemi and may not be redistributed
 * in any form without prior written permission from onsemi.
 * The terms of use and warranty for this code are covered by contractual
 * agreements between onsemi and the licensee.
 *
 * This is Reusable Code.
 * @endparblock
 */

#include "app.h"

sleep_mode_cfg app_sleep_mode_cfg;

void SOC_Sleep(void)
{
	/* Initialize sleep before entering sleep */
	Sys_PowerModes_Sleep_Init(&app_sleep_mode_cfg);

#if DEBUG_SLEEP_GPIO
	/* Set power mode GPIO to indicate power mode */
	Sys_GPIO_Set_High(POWER_MODE_GPIO);
#endif

#if defined(CFG_REDUCED_DRAM)
	/* Enable the required amount of DRAMs */
	SYSCTRL_MEM_POWER_CFG->DRAM_POWER_BYTE = DRAM0_POWER_ENABLE_BYTE | DRAM1_POWER_ENABLE_BYTE |
	                                             DRAM2_POWER_ENABLE_BYTE | DRAM3_POWER_ENABLE_BYTE;
#endif

	/* Power Mode enter sleep with core retention */
	Sys_PowerModes_Sleep_Enter(&app_sleep_mode_cfg, SLEEP_CORE_RETENTION);
}

/**
 * @brief FIFO Wakeup Handler routine
 */
void FIFO_Wakeup_Handler(void)
{
    WAKEUP_FIFO_FULL_FLAG_CLEAR();

#if DEBUG_SLEEP_GPIO
    Sys_GPIO_Set_Low(WAKEUP_ACTIVITY_SENSOR);
#endif    /* DEBUG_SLEEP_GPIO */

    /* The FIFO_LEVEL is asynchronous to the system clock it is recommended to
     * read two consecutive times to check the consistency of the value */
    uint8_t fifo_level = 0;
    uint8_t previous_fifo_level = 0;
    uint8_t i = 0;
    uint8_t read_flag = false;

    /* Read the first FIFO level to check if the level is equal to the FIFO size configured. */
    fifo_level = (uint8_t)((SENSOR->FIFO_CFG & SENSOR_FIFO_CFG_FIFO_LEVEL_Mask) >> SENSOR_FIFO_CFG_FIFO_LEVEL_Pos);


    do
    {
        previous_fifo_level = fifo_level;

        /* Read FIFO_LEVEL for second time to check the consistency */
        fifo_level = (uint8_t)(SENSOR->FIFO_CFG & SENSOR_FIFO_CFG_FIFO_LEVEL_Mask) >> SENSOR_FIFO_CFG_FIFO_LEVEL_Pos;

        /* Compare if consecutive read are equal */
        if(previous_fifo_level == fifo_level)
        {
            read_flag = true;
    }

        i++;

    }while((i<10) && (!read_flag));

	/* Force to reset FIFO in here */
    SensorFIFO_Reset(fifo_level);

#if DEBUG_SLEEP_GPIO
    /* Toggle this pin 6 times to emulate application operations
     * for FIFO wakeup in run mode */
    for(uint8_t i = 0; i < 6; i++)
    {
        Sys_GPIO_Toggle(WAKEUP_ACTIVITY_SENSOR);
    }
#endif    /* DEBUG_SLEEP_GPIO */
}

/**
 * @brief GPIO1 wakeup Handler routine
 */
void GPIO1_Wakeup_Handler(void)
{
    WAKEUP_GPIO1_FLAG_CLEAR();

#if DEBUG_SLEEP_GPIO

    Sys_GPIO_Set_Low(WAKEUP_ACTIVITY_GPIO);

    /* Toggle this pin 6 times to emulate application operations
     * for FIFO wakeup in run mode */
    for(uint8_t i = 0; i < 6; i++)
    {
        Sys_GPIO_Toggle(WAKEUP_ACTIVITY_GPIO);
    }
#endif    /* DEBUG_SLEEP_GPIO */
}

/**
 * @brief   Wakeup interrupt handler routine for VDDC in Retention
 */
void WAKEUP_IRQHandler(void)
{
    SYS_WATCHDOG_REFRESH();

    /* Check if FIFO FULL wakeup event set */
    if(ACS->WAKEUP_CTRL & WAKEUP_FIFO_FULL_EVENT_SET)
    {
        /* Call FIFO Wakeup Handler */
        FIFO_Wakeup_Handler();
    }
    /* Check if GPIO1 wakeup event set */
    if(ACS->WAKEUP_CTRL & WAKEUP_GPIO1_EVENT_SET)
    {
        /* Call GPIO1 Wakeup Handler */
        GPIO1_Wakeup_Handler();
    }
    if(ACS->WAKEUP_CTRL & WAKEUP_BB_TIMER_EVENT_SET)
    {
        /* Clear the BB Timer sticky flag */
        WAKEUP_BB_TIMER_FLAG_CLEAR();
    }

    /* If there is an pending wakeup event set during the execution of this
     * Wakeup interrupt handler. Set the NVIC WAKEUP_IRQn so that the pending wakeup
     * event will be serviced again */
    if(ACS->WAKEUP_CTRL)
    {
        if(!NVIC_GetPendingIRQ(WAKEUP_IRQn))
        {
            NVIC_SetPendingIRQ(WAKEUP_IRQn);
        }
    }
}
