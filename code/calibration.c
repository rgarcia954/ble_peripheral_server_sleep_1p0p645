/**
 * @file calibration.c
 * @brief calibration source file
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

#include "calibration.h"

int8_t tx_power_level_dbm = 0;

#if ((CALIB_RECORD == SUPPLEMENTAL_CALIB) || (CALIB_RECORD == MANU_CALIB))

/**
 * @brief         Load trim values and calibrate the module to target voltages
 *                using pre-loaded calibration records
 *
 * @param[in]     None
 * @param[out]    None
 * @assumptions   Calibration records were loaded in NVR7 during manufacturing
 *                or Calibration records were saved in NVR4 region by user
 */
uint8_t Load_Trim_Values(void)
{
    uint8_t error_code;

    error_code = VOLTAGES_CALIB_NO_ERROR;

#if (CALIB_RECORD == SUPPLEMENTAL_CALIB)
    TRIM_Type *trim_region = TRIM_SUPPLEMENTAL;
#elif (CALIB_RECORD == MANU_CALIB)
    TRIM_Type *trim_region = TRIM;
#endif
    /* -------------- Load pre-calculated VCC trim value -------------- */
    if (Sys_Trim_LoadDCDC(trim_region, VCC_TARGET) != ERRNO_NO_ERROR)
    {
        error_code |= VCC_CALIB_ERROR;
    }

    /* -------------- Load pre-calculated VDDC trim value ------------- */
    if (Sys_Trim_LoadVDDC(trim_region, VDDC_TARGET,TARGET_VDDC_STANDBY) != ERRNO_NO_ERROR)
    {
        error_code |= VDDC_CALIB_ERROR;
    }

    /* ------------- Load pre-calculated VDDM trim value -------------- */
    if (Sys_Trim_LoadVDDM(trim_region, VDDM_TARGET, TARGET_VDDM_STANDBY) != ERRNO_NO_ERROR)
    {
        error_code |= VDDM_CALIB_ERROR;
    }

    /* ------------- Load pre-calculated VDDM trim value -------------- */
    if (Sys_Trim_LoadVDDRF(trim_region, VDDRF_TARGET) != ERRNO_NO_ERROR)
    {
        error_code |= VDDM_CALIB_ERROR;
    }

    /* ------------- Load pre-calculated VDDM trim value -------------- */
    if (Sys_Trim_LoadVDDFLASH(trim_region, VDDFLASH_TARGET) != ERRNO_NO_ERROR)
    {
        error_code |= VDDM_CALIB_ERROR;
    }

    return (error_code);
}

#elif (CALIB_RECORD == USER_CALIB)

/**
 * @brief         Calculate trim values and calibrate the module
 *                to target voltages
 *
 * @param[in]     None
 * @param[out]    None
 * @return        error code
 * @assumptions   Target values defined in calibration.h
 */
uint8_t Calculate_Trim_Values_And_Calibrate(void)
{
    /* Initialize variables */
    CalPower_Type pwr_results[1] = { 0 };

    uint32_t result = ERROR_NO_ERROR;

    CalSetting cal_values = { 0 };

    uint16_t *init_ptr;

    uint8_t error_code;

    error_code = VOLTAGES_CALIB_NO_ERROR;

    /* Initialize cal_values struct to 0xffff. The for loop is configured as
     * follows: Start:     a pointer to the start of the cal_values struct
     *          End:       The memory address at the end of the cal_values
     *                     struct, calculated by adding the size of cal_values
     *                     to the base address of cal_values. The division by
     *                     two is because the sizeof operator returns the size
     *                     in 8 bits while the pointer we're using to increment
     *                     is 16 bits.
     *          Increment: Increment the pointer by one, which amounts to 16
     *                     bits since we're using a 16 bit pointer. */
    for (init_ptr = (uint16_t *)(&cal_values);
         init_ptr < ((uint16_t *)(sizeof(cal_values) / 2 +
                                  (uint16_t *)&cal_values));
         init_ptr++)
    {
        *init_ptr = 0xffff;
    }

    /* Initializes the system state */
    Calibrate_Power_Initialize();

    /* Before calibrating VDDRF, we must set VCC to a value high enough to
     * support our desired VDDRF. This is because VCC is the supply for VDDRF.
     * VCC = max(VDDRF, VDDM, VDDC) + 50mV margin*/
    /* ------------------------ Calibrate DCDC ------------------------ */
    result = Calibrate_Power_DCDC(LSAD_CALIB_CHANNEL,
                                   &(LSAD->DATA_TRIM_CH[LSAD_CALIB_CHANNEL]),
                                   LSAD_VCC_TARGET, pwr_results);

    /* Update the errors array if calibration failed */
    if (result != ERRNO_NO_ERROR)
    {
        error_code |= VCC_CALIB_ERROR;
    }
    else    /* No error occurred, save the calibration data */
    {
        /* Save the DCDC target value in the cal_values struct */
        cal_values.DCDC_CAL_TARGET = LSAD_VCC_TARGET;

        /* Save the VDDM trim value in the cal_values struct */
        cal_values.DCDC_CAL_TRIM_VALUE = pwr_results[0].trim_setting;
    }


    /* ----------------------- Calibrate VDDRF ------------------------ */
    result = Calibrate_Power_VDDRF(LSAD_CALIB_CHANNEL,
                                    &(LSAD->DATA_TRIM_CH[LSAD_CALIB_CHANNEL]),
                                    LSAD_VDDRF_TARGET, pwr_results);

    /* Update the errors array if calibration failed */
    if (result != ERRNO_NO_ERROR)
    {
        error_code |= VDDRF_CALIB_ERROR;
    }
    else    /* No error occurred, save the calibration data */
    {
        /* Save the VDDRF target value in the cal_values struct */
        cal_values.VDDRF_CAL_TARGET = LSAD_VDDRF_TARGET;

        /* Save the VDDRF trim value in the cal_values struct */
        cal_values.VDDRF_CAL_TRIM_VALUE = pwr_results[0].trim_setting;
    }


    /* ------------------------ Calibrate VDDC ------------------------ */
    result = Calibrate_Power_VDDC(LSAD_CALIB_CHANNEL,
                                   &(LSAD->DATA_TRIM_CH[LSAD_CALIB_CHANNEL]),
                                   LSAD_VDDC_TARGET, pwr_results);

    /* Update the errors array if calibration failed */
    if (result != ERRNO_NO_ERROR)
    {
        error_code |= VDDC_CALIB_ERROR;
    }
    else    /* No error occurred, save the calibration data */
    {
        /* Save the VDDC target value in the cal_values struct */
        cal_values.VDDC_CAL_TARGET = LSAD_VDDC_TARGET;

        /* Save the VDDC trim value in the cal_values struct */
        cal_values.VDDC_CAL_TRIM_VALUE = pwr_results[0].trim_setting;
    }

    /* ------------------------ Calibrate VDDM ------------------------ */
    result = Calibrate_Power_VDDM(LSAD_CALIB_CHANNEL,
                                   &(LSAD->DATA_TRIM_CH[LSAD_CALIB_CHANNEL]),
                                   LSAD_VDDM_TARGET, pwr_results);

    /* Update the errors array if calibration failed */
    if (result != ERRNO_NO_ERROR)
    {
        error_code |= VDDM_CALIB_ERROR;
    }
    else    /* No error occurred, save the calibration data */
    {
        /* Save the VDDM target value in the cal_values struct */
        cal_values.VDDM_CAL_TARGET = LSAD_VDDM_TARGET;

        /* Save the VDDM trim value in the cal_values struct */
        cal_values.VDDM_CAL_TRIM_VALUE = pwr_results[0].trim_setting;
    }

    /* ------------------------ Calibrate VDDFLASH ------------------------ */
    result = Calibrate_Power_VDDFLASH(LSAD_CALIB_CHANNEL,
                                   &(LSAD->DATA_TRIM_CH[LSAD_CALIB_CHANNEL]),
                                   LSAD_VDDFLASH_TARGET, pwr_results);

    /* Update the errors array if calibration failed */
    if (result != ERRNO_NO_ERROR)
    {
        error_code |= VDDFLASH_CALIB_ERROR;
    }
    else    /* No error occurred, save the calibration data */
    {
        /* Save the VDDM target value in the cal_values struct */
        cal_values.VDDFLASH_CAL_TARGET = LSAD_VDDFLASH_TARGET;

        /* Save the VDDM trim value in the cal_values struct */
        cal_values.VDDFLASH_CAL_TRIM_VALUE = pwr_results[0].trim_setting;
    }


    /* It may be desirable to set a breakpoint over here during testing and
     * check the errors array to make sure no calibration failure have occurred
     */

    return (error_code);
}

#endif    /* CALIB_RECORD */
