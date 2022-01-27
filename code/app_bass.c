/**
 * @file app_bass.c
 * @brief BASS Application-specific source file
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
#include <app_bass.h>
#include <hw.h>
#include <swmTrace_api.h>
#include <ble_bass.h>

/* ----------------------------------------------------------------------------
 * Function      : void APP_BASS_ReadBatteryLevel(uint8_t bas_nb)
 * ----------------------------------------------------------------------------
 * Description   : Calculate the current battery level in a scale of [0,100],
 *                 where 0% = 1.1V and 100% = 1.4V. The LSAD measurements are
 *                 averaged 16 times with a 5ms delay in between.
 * Inputs        : uint8_t bas_nb   - Battery instance [0,1].
 * Outputs       : An integer in the [0,100] range.
 * Assumptions   : Return the same battery value for any bas_nb argument.
 * ------------------------------------------------------------------------- */
uint8_t APP_BASS_ReadBatteryLevel(uint8_t bas_nb)
{
    uint32_t lsad_avg = 0;
    uint8_t battLevelPercent;

    /* Perform 16 LSAD reads */
    for(uint8_t i = 0; i < 16; i++)
    {
        lsad_avg += LSAD->DATA_TRIM_CH[LSAD_BATMON_CH];

        /* 5ms delay */
        Sys_Delay((uint32_t)(0.005 * SystemCoreClock));
    }

    lsad_avg = lsad_avg >> 4; /* Average 16 reads */

    /* Calculate percentage battery level */
    battLevelPercent = (uint8_t)(((lsad_avg - VBAT_1p1V_MEASURED) * 100) /
                                 (VBAT_1p4V_MEASURED - VBAT_1p1V_MEASURED));

    battLevelPercent = (battLevelPercent <= 100) ? battLevelPercent : 100;

    swmLogInfo("Read battery level = %d%%\r\n",battLevelPercent);
    return battLevelPercent;
}
