/**
 * @file app_bass.h
 * @brief Bluetooth battery service header
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

#ifndef APP_BASS_H
#define APP_BASS_H

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

#include <stdint.h>
#include <ke_msg.h>

/* ----------------------------------------------------------------------------
 * Defines
 * --------------------------------------------------------------------------*/

#define CONCAT(x, y)                     x##y
#define BATMON_CH(x)                     CONCAT(BATMON_CH, x)

#define APP_BAS_NB                       1  /* Number of batteries (1 or 2) */
#define BATT_LEVEL_LOW_THRESHOLD_PERCENT 15 /* Battery level low at 15% of 1.1V to 1.4V range */

/* LSAD, VBAT and BATMON alarm configuration */
#define VBAT_1p1V_MEASURED               0x11BF
#define VBAT_1p4V_MEASURED               0x168C
#define BATMON_SUPPLY_THRESHOLD_CFG      (((VBAT_1p4V_MEASURED - VBAT_1p1V_MEASURED) * \
                                           BATT_LEVEL_LOW_THRESHOLD_PERCENT / 100 +   \
                                           VBAT_1p1V_MEASURED) * 256 / 0x4000 + 1)
#define LSAD_BATMON_CH                    6
#define LSAD_GND_CH                       0

void APP_BASS_SetBatMonAlarm(uint32_t supplyThresholdCfg);

uint8_t APP_BASS_ReadBatteryLevel(uint8_t bas_nb);

void APP_BASS_BattLevelLow_Handler(ke_msg_id_t const msg_id,
                                   void const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id);

extern void LSAD_BATMON_IRQHandler(void);

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* APP_H */
