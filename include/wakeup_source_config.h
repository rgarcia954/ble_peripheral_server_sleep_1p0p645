/**
 * @file wakeup_source_config.h
 * @brief Wakeup source configuration header file
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

#ifndef WAKEUP_SOURCE_CONFIG_H_
#define WAKEUP_SOURCE_CONFIG_H_

/* ----------------------------------------------------------------------------
 * Include files
 * --------------------------------------------------------------------------*/
#include "hw.h"

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/* Clear all sticky wake up flags */
#define WAKEUP_FLAGS_CLEAR()                 ACS->WAKEUP_CTRL = (uint32_t)(0xFFFF)

/* Clear sticky wake up RTC alarm flag */
#define WAKEUP_RTC_ALARM_FLAG_CLEAR()        ACS->WAKEUP_CTRL |= WAKEUP_RTC_ALARM_EVENT_CLEAR

/* Clear sticky wake up GPIO0 flag */
#define WAKEUP_GPIO0_FLAG_CLEAR()            ACS->WAKEUP_CTRL |= WAKEUP_GPIO0_EVENT_CLEAR

/* Clear sticky wake up GPIO1 flag */
#define WAKEUP_GPIO1_FLAG_CLEAR()            ACS->WAKEUP_CTRL |= WAKEUP_GPIO1_EVENT_CLEAR

/* Clear sticky wake up GPIO2 flag */
#define WAKEUP_GPIO2_FLAG_CLEAR()            ACS->WAKEUP_CTRL |= WAKEUP_GPIO2_EVENT_CLEAR

/* Clear sticky wake up GPIO3 flag */
#define WAKEUP_GPIO3_FLAG_CLEAR()            ACS->WAKEUP_CTRL |= WAKEUP_GPIO3_EVENT_CLEAR

/* Clear sticky wake up BB Timer flag */
#define WAKEUP_BB_TIMER_FLAG_CLEAR()         ACS->WAKEUP_CTRL |= WAKEUP_BB_TIMER_CLEAR

/* Clear sticky wake up SENSOR DETECT flag */
#define WAKEUP_SENSOR_DETECT_FLAG_CLEAR()    ACS->WAKEUP_CTRL |= WAKEUP_SENSOR_DET_EVENT_CLEAR

/* Clear sticky wake up FIFO FULL flag */
#define WAKEUP_FIFO_FULL_FLAG_CLEAR()        ACS->WAKEUP_CTRL |= WAKEUP_FIFO_FULL_EVENT_CLEAR

/* Clear sticky wake up THRESHOLD FULL flag */
#define WAKEUP_THRESHOLD_FULL_FLAG_CLEAR()   ACS->WAKEUP_CTRL |= THRESHOLD_FULL_EVENT_CLEAR

/* Clear sticky wake up NFC FIELD flag */
#define WAKEUP_NFC_FIELD_FLAG_CLEAR()        ACS->WAKEUP_CTRL |= WAKEUP_NFC_FIELD_EVENT_CLEAR

/* Clock source for sensor
 * Possible options:
 *   - SENSOR_CLK_RTC: RTC clock
 *   - SENSOR_CLK_SLOWCLK: slow clock */
#define SENSOR_CLK_SRC                  SENSOR_CLK_RTC

/* Differential mode enable, this mode use difference value between a pair of samples
 * Possible options:
 *   - SENSOR_DIFF_MODE_DISABLED: At the end of the Pulse Count state and Pre Count state
 *                                a sample is generated (2 samples per cycle).
 *   - SENSOR_DIFF_MODE_ENABLED: At the end of the Pulse Count state  a sample is generated.
 *                               The sample generated is equal to Pulse_count_sample less
 *                               Pre_count_sample (1 sample per cycle). */
#define SENSOR_DIFF_MODE                SENSOR_DIFF_MODE_DISABLED

/* The number of ADC samples stored before waking up the core
 * Possible values:
 *   - SENSOR_FIFO_SIZE1: SIZE = 1
 *   - SENSOR_FIFO_SIZE2: SIZE = 2
 *   - etc. */
#define FIFO_SIZE_VALUE                 SENSOR_FIFO_SIZE1

/* The number of samples used by summation and threshold mode
 * Possible values:
 *   - SENSOR_NBR_SAMPLES_1: 1 sample or 1 pair used
 *   - SENSOR_NBR_SAMPLES_2: 2 sample or 2 pair used
 *   - ((uint32_t)(0x2U << SENSOR_PROCESSING_NBR_SAMPLES_Pos))
 *   - etc. */
#define NBR_SAMPLES_VALUE               SENSOR_NBR_SAMPLES_1;

/* The sensor data value threshold for wake up
 * Possible values:
 *   - SENSOR_THRESHOLD_DISABLED: Sensor threshold functionality is disabled
 *   - SENSOR_THRESHOLD_1: Increments counter if samples (LSBs) is >=0x0001
 *   - ((uint32_t)(0x2 << SENSOR_PROCESSING_THRESHOLD_Pos))
 *   - etc. */
#define ADC_THRESHOLD_VALUE             ((uint32_t)(0x01 << SENSOR_PROCESSING_THRESHOLD_Pos))

/* Absolute Value of main counter to trigger the change of "Pre Count Sample"
 * state
 * Possible values:
 *   - PRE_COUNT_INT_0: Number of periods for "Pre Count Int State" = 1
 *                      (Duration of the state is 0.976 ms)
 *   - PRE_COUNT_INT_1: Number of periods for "Pre Count Int State" = 2
 *                      (Duration of the state is 1.953 ms)
 *   - ((uint32_t)(0x2U << SENSOR_INT_CFG_PRE_COUNT_INT_Pos))
 *   - etc. */
#define PRE_COUNT_INT_VALUE             ((uint32_t)(0xFF << SENSOR_INT_CFG_PRE_COUNT_INT_Pos))

/* ---------------------------------------------------------------------------
* Function prototype definitions
* --------------------------------------------------------------------------*/

void GPIO_Wakeup_Init(void);

void Sensor_Init(void);

void ADC_FIFO_Init(void);

void SensorFIFO_Reset(uint8_t fifo_level);

void Wakeup_Source_Config(void);

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif /* WAKEUP_SOURCE_CONFIG_H_ */
