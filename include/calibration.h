/**
 * @file calibration.h
 * @brief calibration header file
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

#ifndef CALIBRATION_H_
#define CALIBRATION_H_

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/* ----------------------------------------------------------------------------
 * Include files
 * ------------------------------------------------------------------------- */
#include <calibrate.h>

/* ----------------------------------------------------------------------------
 * Defines
 * ------------------------------------------------------------------------- */

/* RF output transmission power level (in unit of dBm)
 * Options: [0 | 3 | 6 ] for 0, 3, 6 dBm respectively */
#ifndef RF_TX_POWER_LEVEL_DBM
#define RF_TX_POWER_LEVEL_DBM           0
#endif

/* This sample application considers three possible cases regarding the
 * calibrations of supply voltages:
 *  - (1) Trim values for desired voltages were loaded in the calibration
 * records during manufacturing (NVR7). This application simply reads them
 * from NVR7 and load them into corresponding trim registers to calibrate
 * the board. In this case, define CALIB_RECORD as MANU_CALIB.
 *  - (2) Not case (1) and supplemental_calibrate sample application was
 * used to calculate and store those trim values in the calibration information
 * region of NVR4. During the system boot process, the user-defined initialization
 * function reads and loads the supplemental calibrated values from NVR4 into
 * corresponding trim registers to calibrate the board. In this case,
 * define CALIB_RECORD as SUPPLEMENTAL_CALIB.
 *  - (3) Neither case (1) nor (2). This application needs to calculate
 * trim values for desired voltages and load them into corresponding trim
 * registers to calibrate the board. In this case, define CALIB_RECORD as
 * USER_CALIB.
 */
#define MANU_CALIB                      1
#define SUPPLEMENTAL_CALIB              2
#define USER_CALIB                      3

/* Calibration records to be loaded for voltage calibration
 * Options: - MANU_CALIB
 *          - SUPPLEMENTAL_CALIB
 *          - USER_CALIB */
#ifndef CALIB_RECORD
#define CALIB_RECORD                    MANU_CALIB
#endif

/* Hold the application to verify if calibrated voltages agree with
 * target voltages */

/* #define VOLTAGES_CALIB_VERIFY */

/* To check if there are any errors when calibrating supply voltages */
#define VOLTAGES_CALIB_NO_ERROR         (uint8_t)(0x0)
#define VCC_CALIB_ERROR                 (uint8_t)(0x1 << 0)
#define VDDRF_CALIB_ERROR               (uint8_t)(0x1 << 1)
#define VDDPA_CALIB_ERROR               (uint8_t)(0x1 << 2)
#define VDDC_CALIB_ERROR                (uint8_t)(0x1 << 3)
#define VDDM_CALIB_ERROR                (uint8_t)(0x1 << 4)
#define VDDFLASH_CALIB_ERROR            (uint8_t)(0x1 << 5)

/* If trim values for desired voltages were loaded in
 * the calibration records during manufacturing (NVR7) */
#if (CALIB_RECORD == MANU_CALIB)

/* Calibrated voltage targets [10 * mV] */

#define VDDRF_TARGET                    TARGET_VDDRF_1100       /* Target for 1.10 V for VDDRF regulator */
#define VDDC_TARGET                     TARGET_VDDC_1150        /* Target for 1.15 V for VDDC regulator */
#define VDDM_TARGET                     TARGET_VDDM_1150        /* Target for 1.15 V for VDDM regulator */
#define VDDFLASH_TARGET                 TARGET_FLASH_1600       /* Target for 1.60 V for VDDFLASH regulator */

/* RF output transmission power level = 0 dBm */
#if (RF_TX_POWER_LEVEL_DBM == 0)
#define VCC_TARGET                      (uint8_t)(TARGET_DCDC_1200)

/* RF output transmission power level = 3 dBm */
#elif (RF_TX_POWER_LEVEL_DBM == 3)
#define VCC_TARGET                      (uint8_t)(TARGET_DCDC_1200)

/* RF output transmission power level = 6 dBm */
#elif (RF_TX_POWER_LEVEL_DBM == 6)
#define VCC_TARGET                      (uint8_t)(TARGET_DCDC_1200)

#endif    /* RF_TX_POWER_LEVEL_DBM */

/* Else if supplemental_calibrate already calculated and saved
 * trim values in NVR4 */
#elif (CALIB_RECORD == SUPPLEMENTAL_CALIB)
/* Calibrated voltage targets [10 * mV] */

#define VDDRF_TARGET                    TARGET_VDDRF_1100       /* Target for 1.10 V for VDDRF regulator */
#define VDDC_TARGET                     TARGET_VDDC_1150        /* Target for 1.15 V for VDDC regulator */
#define VDDM_TARGET                     TARGET_VDDM_1150        /* Target for 1.15 V for VDDM regulator */
#define VDDFLASH_TARGET                 TARGET_FLASH_1600       /* Target for 1.60 V for VDDFLASH regulator */

/* RF output transmission power level = 0 dBm */
#if (RF_TX_POWER_LEVEL_DBM == 0)
#define VCC_TARGET                      (uint8_t)(TARGET_DCDC_1200)

/* RF output transmission power level = 3 dBm */
#elif (RF_TX_POWER_LEVEL_DBM == 3)
#define VCC_TARGET                      (uint8_t)(TARGET_DCDC_1200)

/* RF output transmission power level = 6 dBm */
#elif (RF_TX_POWER_LEVEL_DBM == 6)
#define VCC_TARGET                      (uint8_t)(TARGET_DCDC_1200)

#endif    /* RF_TX_POWER_LEVEL_DBM */

/* If supplemental_calibrate has not calculated and saved
 * supply voltage trim values in NVR4 */
#elif (CALIB_RECORD == USER_CALIB)

#define LSAD_CALIB_CHANNEL              6

/* Calibrated voltage targets [10 * mV] */

#define LSAD_VDDC_TARGET                (uint8_t)(110)
#define LSAD_VDDM_TARGET                (uint8_t)(110)
#define LSAD_VDDFLASH_TARGET            (uint8_t)(160)

/* RF output transmission power level = 0 dBm */
#if (RF_TX_POWER_LEVEL_DBM == 0)
#define LSAD_VDDRF_TARGET               (uint8_t)(107)
#define LSAD_VCC_TARGET                 (uint8_t)(120)

/* RF output transmission power level = 3 dBm */
#elif (RF_TX_POWER_LEVEL_DBM == 3)
#define LSAD_VDDRF_TARGET               (uint8_t)(126)
#define LSAD_VCC_TARGET                 (uint8_t)(131)

/* RF output transmission power level = 6 dBm */
#elif (RF_TX_POWER_LEVEL_DBM == 6)
#define LSAD_VDDRF_TARGET               (uint8_t)(110)
#define LSAD_VCC_TARGET                 (uint8_t)(120)

#endif    /* RF_TX_POWER_LEVEL_DBM */

/* ----------------------------------------------------------------------------
 * Structure that holds calibration values to be written in NVR4
 * ------------------------------------------------------------------------- */
typedef struct
{
    uint16_t VDDRF_CAL_TRIM_VALUE;
    uint16_t VDDRF_CAL_TARGET;
    uint16_t VDDC_CAL_TRIM_VALUE;
    uint16_t VDDC_CAL_TARGET;
    uint16_t VDDM_CAL_TRIM_VALUE;
    uint16_t VDDM_CAL_TARGET;
    uint16_t DCDC_CAL_TRIM_VALUE;
    uint16_t DCDC_CAL_TARGET;
    uint16_t VDDFLASH_CAL_TRIM_VALUE;
    uint16_t VDDFLASH_CAL_TARGET;
} CalSetting;

#endif    /* CALIB_RECORD */

/* ----------------------------------------------------------------------------
 * Global Variables
 * ------------------------------------------------------------------------- */
extern int8_t tx_power_level_dbm;

/* ----------------------------------------------------------------------------
 * Function prototype definitions
 * ------------------------------------------------------------------------- */

#if ((CALIB_RECORD == SUPPLEMENTAL_CALIB) || (CALIB_RECORD == MANU_CALIB))
uint8_t Load_Trim_Values(void);

#elif (CALIB_RECORD == USER_CALIB)
uint8_t Calculate_Trim_Values_And_Calibrate(void);

#endif    /* if ((CALIB_RECORD == SUPPLEMENTAL_CALIB) || (CALIB_RECORD == MANU_CALIB)) */

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* CALIBRATION_H_ */
