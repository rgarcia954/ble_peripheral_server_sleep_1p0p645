/**
 * @file app.h
 * @brief Main application header
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

#ifndef APP_H
#define APP_H

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
 * --------------------------------------------------------------------------*/
/* Device and libraries headers */
#include <hw.h>
#include <swmTrace_api.h>
#include <flash_rom.h>
#include <ble_protocol_support.h>
#include <ble_abstraction.h>

/* Application headers */
#include <app_customss.h>
#include <app_bass.h>
#include <app_init.h>
#include <app_msg_handler.h>
#include "calibration.h"
#include "wakeup_source_config.h"

/* APP Task messages */
enum appm_msg
{
    APPM_DUMMY_MSG = TASK_FIRST_MSG(TASK_ID_APP),
    BLE_STATES_TIMEOUT
};

/* ----------------------------------------------------------------------------
 * Defines
 * --------------------------------------------------------------------------*/
/* Power Reduction Defines Begin */
/* Set this to 1 to Power Down Sensor Interface */
#define SENSOR_POWER_DISABLE            0

/* Set this to 1 to Power Down CryptoCell
 * notes: settings this to 1 will lock debug port and JTAG debug will not work */
#define CC312AO_POWER_DISABLE           0

/* Set this to 1 if you want to have debug GPIO capability for sleep mode or run mode
 * Set this to 0 if you want to reduce power consumption */
#define DEBUG_SLEEP_GPIO                1

/* Set this to 1 to Power Down FPU
 * note: If FPU is used during run mode this should be left 0 */
#define POWER_DOWN_FPU                  0

/* Set this to 1 to Power Down Debug Unit
 * note: If Debug Port is used during run mode this should be left 0 */
#define POWER_DOWN_DBG                  0

/* Disable VDDIF interface regulator */
#define VDDIF_POWER_DOWN                1

/* Power Reduction Defines End */

/* Sensor Calibration mode */
#define SENSOR_CALIB                     1

#define APP_BLE_DEV_PARAM_SOURCE        APP_PROVIDED /* or APP_PROVIDED FLASH_PROVIDED_or_DFLT */

/* Location of BLE public address
 *   - BLE public address location in MNVR is used as a default value;
 *   - Any other valid locations can be used as needed.
 */
#define APP_BLE_PUBLIC_ADDR_LOC			BLE_PUBLIC_ADDR_LOC_MNVR

/* Advertising channel map - 37, 38, 39 */
#define APP_ADV_CHMAP                   0x07

/* Advertisement connectable mode */
#define ADV_CONNECTABLE_MODE            0

/* Advertisement non-connectable mode */
#define ADV_NON_CONNECTABLE_MODE        1

/* Advertisement connectability mode
 * Options are:
 *     - ADV_CONNECTABLE_MODE
 *     - ADV_NON_CONNECTABLE_MODE */
#ifndef APP_ADV_CONNECTABILITY_MODE
#define APP_ADV_CONNECTABILITY_MODE     ADV_CONNECTABLE_MODE
#endif

#if defined(CFG_REDUCED_DRAM)
#define CFG_ADV_INTERVAL_MS             5000
#endif

/* Define the advertisement interval for connectable mode (units of 625us)
 * Notes: the interval can be 20ms up to 10.24s */
#ifdef CFG_ADV_INTERVAL_MS
#define ADV_INT_CONNECTABLE_MODE        (CFG_ADV_INTERVAL_MS / 0.625)
#else    /* ifdef CFG_ADV_INTERVAL_MS */
#define ADV_INT_CONNECTABLE_MODE        64
#endif    /* ifdef CFG_ADV_INTERVAL_MS */

/* Define the advertisement interval for non-connectable mode (units of 625us)
 * Notes: the minimum interval for non-connectable advertising should be 100ms */
#ifdef CFG_ADV_INTERVAL_MS
#define ADV_INT_NON_CONNECTABLE_MODE    (CFG_ADV_INTERVAL_MS / 0.625)
#else    /* ifdef CFG_ADV_INTERVAL_MS */
#define ADV_INT_NON_CONNECTABLE_MODE    160
#endif    /* ifdef CFG_ADV_INTERVAL_MS */

/* Set the advertisement interval */
#if (APP_ADV_CONNECTABILITY_MODE == ADV_CONNECTABLE_MODE)
#define APP_ADV_INT_MIN                 ADV_INT_CONNECTABLE_MODE
#define APP_ADV_INT_MAX                 ADV_INT_CONNECTABLE_MODE
#define APP_ADV_DISCOVERY_MODE          GAPM_ADV_MODE_GEN_DISC
#define APP_ADV_PROPERTIES              GAPM_ADV_PROP_UNDIR_CONN_MASK
#elif (APP_ADV_CONNECTABILITY_MODE == ADV_NON_CONNECTABLE_MODE)
#define APP_ADV_INT_MIN                 ADV_INT_NON_CONNECTABLE_MODE
#define APP_ADV_INT_MAX                 ADV_INT_NON_CONNECTABLE_MODE
#define APP_ADV_DISCOVERY_MODE          GAPM_ADV_MODE_NON_DISC
#define APP_ADV_PROPERTIES              GAPM_ADV_PROP_NON_CONN_NON_SCAN_MASK
#endif    /* if (APP_ADV_CONNECTABILITY_MODE== ADV_CONNECTABLE_MODE) */

#define APP_PUBLIC_ADDRESS              { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB }

#define GAPM_CFG_ADDR_PUBLIC            (0 << GAPM_PRIV_CFG_PRIV_ADDR_POS)
#define GAPM_CFG_ADDR_PRIVATE           (1 << GAPM_PRIV_CFG_PRIV_ADDR_POS)

#define GAPM_CFG_HOST_PRIVACY           (0 << GAPM_PRIV_CFG_PRIV_EN_POS)
#define GAPM_CFG_CONTROLLER_PRIVACY     (1 << GAPM_PRIV_CFG_PRIV_EN_POS)

#define GAPM_ADDRESS_TYPE               GAPM_CFG_ADDR_PRIVATE
#define GAPM_PRIVACY_TYPE               GAPM_CFG_HOST_PRIVACY

#define APP_BD_RENEW_DUR                150 /* in seconds */

#define GAPM_OWN_ADDR_TYPE              GAPM_STATIC_ADDR /* GAPM_STATIC_ADDR, GAPM_GEN_NON_RSLV_ADDR */

#define APP_BLE_PRIVATE_ADDR            { 0x94, 0x11, 0x22, 0xff, 0xbb, 0xD5 } /* BLE private address of local device */

#define SECURE_CONNECTION               1  /* set 0 for LEGACY_CONNECTION or 1 for SECURE_CONNECTION */

/* The number of standard profiles and custom services added in this application */
#define APP_NUM_STD_PRF                 1
#define APP_NUM_CUST_SVC                1

#if    (VDDIF_POWER_DOWN == 0)
#define TWOSC                           1700 /* us */
#elif  (VDDIF_POWER_DOWN == 1)
#define TWOSC                           2700 /* us */
#endif

#define MAX_SLEEP_DURATION              96000 /* 30s */
#define MIN_SLEEP_DURATION              3500 /* 3500us */

/* Defines the Low power clock accuracy in ppm */
#define LOW_POWER_CLOCK_ACCURACY        500

#define SYSTEM_CLK                      8000000

/* Set UART peripheral clock */
#define UART_CLK                        8000000

/* Set sensor clock */
#define SENSOR_CLK                      32768

/* Set user clock */
#define USER_CLK                        1000000

/* Enable/disable buck converter
 * Options: VCC_BUCK or VCC_LDO
 */
#define VCC_BUCK_LDO_CTRL               VCC_BUCK

/* The GPIO pin to use for TX when using the UART mode */
#define UART_TX_GPIO                    (6)

/* The GPIO pin to use for RX when using the UART mode */
#define UART_RX_GPIO                    (5)

/* The selected baud rate for the application when using UART mode */
#define UART_BAUD       				(115200)

/* GPIO number that is used for easy re-flashing (recovery mode) */
#define RECOVERY_GPIO                   0

/* GPIO used to output the sysclock */
#define SYSCLK_GPIO                     2

/* GPIO used to output SENSOR wakeup */
#define WAKEUP_ACTIVITY_SENSOR          4

/* GPIO used to output GPIO1 wakeup activity */
#define WAKEUP_ACTIVITY_GPIO            3

/* GPIO wakeup pin */
#define GPIO_WAKEUP_PIN                 1

/* GPIO used to indicate run and power mode
 *   - Run mode: low
 *   - Power mode (sleep or storage): high */ 
#define POWER_MODE_GPIO                 7

/* Default LSAD channel for SetTxPower */
#define LSAD_TXPWR_DEF                  1

/* Default RF maximum TX power */
#define DEF_TX_POWER                    0

/* Timer setting in units of 1ms (kernel timer resolution) */
#define TIMER_SETTING_MS(MS)            MS
#define TIMER_SETTING_S(S)              (S * 1000)

/* Advertising data is composed by device name and company id */
#if defined(CFG_REDUCED_DRAM)
#define APP_DEVICE_NAME                 ""
#else
#define APP_DEVICE_NAME                 "ble_periph_srv_sleep"
#endif
#define APP_DEVICE_NAME_LEN             sizeof(APP_DEVICE_NAME)-1

/* Manufacturer info (onsemi Company ID) */
#define APP_COMPANY_ID                  { 0x62, 0x3 }
#define APP_COMPANY_ID_LEN              2

#define APP_DEVICE_APPEARANCE           0
#define APP_PREF_SLV_MIN_CON_INTERVAL   8
#define APP_PREF_SLV_MAX_CON_INTERVAL   10
#define APP_PREF_SLV_LATENCY            0
#define APP_PREF_SLV_SUP_TIMEOUT        200

/* Application-provided IRK */
#define APP_IRK                         { 0x01, 0x23, 0x45, 0x68, 0x78, 0x9a, \
                                          0xbc, 0xde, 0x01, 0x23, 0x45, 0x68, \
                                          0x78, 0x9a, 0xbc, 0xde }

/* Application-provided CSRK */
#define APP_CSRK                        { 0x01, 0x23, 0x45, 0x68, 0x78, 0x9a, \
                                          0xbc, 0xde, 0x01, 0x23, 0x45, 0x68, \
                                          0x78, 0x9a, 0xbc, 0xde }

#define AOUT_ENABLE_DELAY               SystemCoreClock / 100    /* delay set to 10ms */
#define AOUT_GPIO                       2

/* ---------------------------------------------------------------------------
* Function prototype definitions
* --------------------------------------------------------------------------*/
void Main_Loop(void);
void SOC_Sleep(void);
void WAKEUP_IRQHandler(void);
void FIFO_Wakeup_Handler(void);
void GPIO1_Wakeup_Handler(void);
/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* APP_H */

