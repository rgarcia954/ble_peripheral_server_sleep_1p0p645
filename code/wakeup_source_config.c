/**
 * @file wakeup_source_config.c
 * @brief Wakeup source configuration file
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

#include "wakeup_source_config.h"
#include "sensor.h"
#include "app.h"

uint32_t fifo_size = FIFO_SIZE_VALUE;
uint32_t number_of_samples = NBR_SAMPLES_VALUE;
uint32_t adc_threshold = ADC_THRESHOLD_VALUE;
uint32_t diff_mode = SENSOR_DIFF_MODE;

void Wakeup_Source_Config(void)
{
    /* Configure and enable FIFO */
    ADC_FIFO_Init();

    /* Configure and enable GPIO wakeup source */
    GPIO_Wakeup_Init();

    /* Clear GPIO1 sticky wakeup flag */
    WAKEUP_GPIO1_FLAG_CLEAR();

    /* Disable NVIC GPIO1 interrupt if the GPIO1 is used as wakeup source
     * GPIO1 NVIC IRQ is recommended to disable if GPIO1 is used as wakeup
     * source. WAKEUP_IRQn is used to capture GPIO1 wakeup event */
    NVIC_DisableIRQ(GPIO1_IRQn);

    /* Clear NVIC Wakeup interrupt IRQ */
    NVIC_ClearPendingIRQ(WAKEUP_IRQn);

    /* Enable the Wakeup interrupt */
    NVIC_EnableIRQ(WAKEUP_IRQn);

}

/* Configure GPIO inputs and interrupts */
void GPIO_Wakeup_Init()
{
    SYS_GPIO_CONFIG(GPIO_WAKEUP_PIN, (GPIO_MODE_DISABLE | GPIO_LPF_DISABLE |
                                      GPIO_WEAK_PULL_UP  | GPIO_6X_DRIVE));
}


/* Configure and enable sensor interface and FIFO wake up source */
void Sensor_Init(void)
{

    uint32_t sensor_cfg = SENSOR_ENABLED |
#if SENSOR_CALIB == 1
                          SENSOR_CALIB_ENABLED |
                          SENSOR_IOFFSET_20NA |
                          SENSOR_IRANGE_80NA |
#else
                          SENSOR_CALIB_DISABLED |
                          SENSOR_IOFFSET_0NA |
                          SENSOR_IRANGE_240NA |
#endif
                          SENSOR_AMP_ENABLED;


    /* Enable the sensor clock */
    CLK->DIV_CFG1 = SENSOR_CLK_ENABLE;

    /* - Enable sensor interface
     * - Set sensor interface offset current to 20 nA
     * - Set sensor interface measurement range to 80 nA
     * - Sensor amplifier enabled
     * - Sensor Calibration enabled
     * - Set WEDAC output voltage to 0.6 V
     * - Set RTC clock as source for sensor */
    Sys_Sensor_ADCConfig(sensor_cfg, SENSOR_WEDAC_HIGH_0600, SENSOR_WEDAC_LOW_0600,
                         SENSOR_CLK_SRC);

    /* Reset the sensor timer */
    Sys_Sensor_TimerReset();

    /* Configure sensor timer and specify which state(s) will be used */
    Sys_Sensor_TimerConfig(SENSOR_TIMER_ENABLED,
                           RE_CONNECTED_BYTE);

    /* Configure length of pre integration state */
    SENSOR->INT_CFG = PRE_COUNT_INT_VALUE;

    /* Read FIFO ADC data to reset the FIFO */
    SensorFIFO_Reset((uint8_t)FIFO_SIZE_VALUE);

    /* Configure sample storage FIFO
     * Set ADC threshold and number of samples */
    Sys_Sensor_StorageConfig(diff_mode, SENSOR_SUMMATION_DISABLED,
                             number_of_samples, adc_threshold,
                             SENSOR_FIFO_STORE_ENABLED, fifo_size);

}


/* Configure ADC FIFO Interrupts */
void ADC_FIFO_Init(void)
{
    /* Configure sensor interface */
    Sensor_Init();

    /* Clear sticky wake up FIFO FULL flag */
    WAKEUP_FIFO_FULL_FLAG_CLEAR();

    /* Disable NVIC FIFO IRQ
     * NVIC IRQ is recommended to disable if FIFO is used as wakeup
     * source. WAKEUP_IRQn is used to capture FIFO wakeup event */
    NVIC_DisableIRQ(FIFO_IRQn);

    /* Disable ADC threshold */
    SENSOR->PROCESSING = SENSOR_THRESHOLD_DISABLED;
}

/**
 * @brief Reset FIFO
 */
void SensorFIFO_Reset(uint8_t fifo_level)
{
    uint8_t i =0;

    do
    {
        (void)SENSOR->ADC_DATA[i];
        i++;
    }
    while(i<=fifo_level);
}

