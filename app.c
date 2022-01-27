/**
 * @file app.c
 * @brief Main application source file
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

#include <app.h>

struct ble_sleep_api_param_tag ble_sleep_api_param =
{
    .app_sleep_request = 1,
    .max_sleep_duration = MAX_SLEEP_DURATION,
	.min_sleep_duration = MIN_SLEEP_DURATION,
};

int main()
{
    /* Disable all interrupts */
    DisableAppInterrupts();

    /* Configure clocks, GPIOs, trace interface and load calibration data */
    DeviceInit();

    swmLogInfo("__ble_peripheral_server has started.\r\n");

    /* Initialize the Kernel and create application task */
    BLEStackInit();

    /* Subscribe application callback handlers to BLE events */
    AppMsgHandlersInit();

    /* Prepare advertising and scan response data (device name + company ID) */
    PrepareAdvScanData();

    /* Send a message to the BLE stack requesting a reset.
     * The stack returns a GAPM_CMT_EVT / GAPM_RESET event upon completion.
     * See DatabaseSetupHandler to follow what happens next. */
    GAPM_SoftwareReset(); /* Step 1 */

    /* Enable all interrupts */
    EnableAppInterrupts();

    /* Execute main loop */
    Main_Loop();
}

void Main_Loop(void)
{
    while(1)
    {
        /* Refresh the watchdog timers */
        SYS_WATCHDOG_REFRESH();

        if(BLE_Baseband_Is_Awake())
        {
            BLE_Kernel_Process();

                /* Checks for sleep have to be done with interrupt disabled */
                GLOBAL_INT_DISABLE();

                /* Check if processor clock can be gated */
                switch(BLE_Baseband_Sleep(&ble_sleep_api_param))
                {
                    case RWIP_DEEP_SLEEP:
                    {
                        SOC_Sleep();
                        break;
                    }
                    case RWIP_CPU_SLEEP:
                    {
                        /* Wait for interrupt */
                        __WFI();
                        break;
                    }
                    case RWIP_ACTIVE:
                    {
                        break;
                    }
                    default:
                    {
                    }
                }

                /* Checks for sleep have to be done with interrupt disabled */
                GLOBAL_INT_RESTORE();
        }
        else
        {
            /* Wait for interrupt */
            __WFI();
        }
    }
}
