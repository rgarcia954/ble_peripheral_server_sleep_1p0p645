/**
 * @file ble_protocol_config.h
 * @brief BLE protocol layer configurations header
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

#ifndef BLE_PROTOCOL_CONFIG_H
#define BLE_PROTOCOL_CONFIG_H

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
#include <ble.h>
#include <flash_rom.h>

/* ----------------------------------------------------------------------------
 * Defines
 * --------------------------------------------------------------------------*/
/* Application controlled BLE stack variables for adjusting RAM usage */
/* Heap memory size used by kernel and BLE stack */
#define APP_HEAP_SIZE_DEFINED			1

/* Maximum number of connection */
#define APP_MAX_NB_CON                  1

/* Maximum number of activities */
#define APP_MAX_NB_ACTIVITY             2

/* Maximum number of profiles */
#define APP_MAX_NB_PROFILES				8

/* Size of environment variables in heap memory */
#define APP_RWIP_HEAP_ENV_SIZE          (600 + (APP_MAX_NB_ACTIVITY) * 230) + \
    APP_MAX_NB_CON * ((sizeof(struct gapc_env_tag)  + KE_HEAP_MEM_RESERVED)    \
                      + (sizeof(struct gattc_env_tag)  + KE_HEAP_MEM_RESERVED)   \
                      + (sizeof(struct l2cc_env_tag)   + KE_HEAP_MEM_RESERVED))  \
    + ((APP_MAX_NB_ACTIVITY)*(sizeof(struct gapm_actv_scan_tag) + KE_HEAP_MEM_RESERVED))

/* Size of data base memory in heap */
#define APP_RWIP_HEAP_DB_SIZE			(768)

/* Size of message heap memory */
#define APP_RWIP_HEAP_MSG_SIZE          (1650 + 2 * \
                                         ((16 + (APP_MAX_NB_ACTIVITY - 1) * 56) + \
                                          (58 + (APP_MAX_NB_ACTIVITY - 1) * 26) + ((APP_MAX_NB_ACTIVITY) * 66) + \
                                          ((APP_MAX_NB_ACTIVITY) * 100) + ((APP_MAX_NB_ACTIVITY) * 12))) + \
    (((BLEHL_HEAP_MSG_SIZE_PER_CON * APP_MAX_NB_CON) > BLEHL_HEAP_DATA_THP_SIZE) \
     ? (BLEHL_HEAP_MSG_SIZE_PER_CON * APP_MAX_NB_CON) : BLEHL_HEAP_DATA_THP_SIZE) \

/* Non retention memory in heap for security algorithm calculations */
#define APP_RWIP_HEAP_NON_RET_SIZE      (328 * 2)

/* Provide seed for random number from application */
#define APP_BLE_RAND_SEED_DEFINED       0

/* Generate random number from application */
#define APP_BLE_RAND_GENERATE_DEFINED   0

/* BLE public address location in MNVR */
#define BLE_PUBLIC_ADDR_LOC_MNVR        FLASH0_MNVR_BASE


/* Additional application defined BLE device parameters use by BLE stack */
/* RF Oscillator accuracy in ppm */
#define RADIO_CLOCK_ACCURACY            20

/* Lower power clock period */
#define LPCLK_PERIOD_VALUE              (float)(1000000.0 / 32768)

/* Enable RF tester pattern */
#define RF_TESTER_GENERATES_PATTERN     0

/* It should reflect GPIO configuration */
#define DTM_ANTENNA_ID_TO_PATTERN       { 0x0, 0x01, 0x02, 0x03}

/* Use application provided private_key and public_key */
#define APP_SECURELE_KEYS_FORCED        0

/* Application-provided private key */
#define APP_PRIVATE_KEY                 { 0xEC, 0x89, 0x3C, 0x11, 0xBB, 0x2E, \
                                          0xEB, 0x5C, 0x80, 0x88, 0x63, 0x57, \
                                          0xCC, 0xE2, 0x05, 0x17, 0x20, 0x75, \
                                          0x5A, 0x26, 0x3E, 0x8D, 0xCF, 0x26, \
                                          0x63, 0x1D, 0x26, 0x0B, 0xCE, 0x4D, \
                                          0x9E, 0x07 }

/* Application-provided public key */
#define APP_PUBLIC_KEY                  { 0x56, 0x09, 0x79, 0x1D, 0x5A, 0x5F, \
                                          0x4A, 0x5C, 0xFE, 0x89, 0x56, 0xEC, \
                                          0xE6, 0xF7, 0x92, 0x21, 0xAC, 0x93, \
                                          0x99, 0x10, 0x51, 0x82, 0xF4, 0xDD, \
                                          0x84, 0x07, 0x50, 0x99, 0xE7, 0xC2, \
                                          0xF1, 0xC8, \
                                          0x40, 0x84, 0xB4, 0xA6, 0x08, 0x67, \
                                          0xFD, 0xAC, 0x81, 0x5D, 0xB0, 0x41, \
                                          0x27, 0x75, 0x9B, 0xA7, 0x92, 0x57, \
                                          0x0C, 0x44, 0xB1, 0x57, 0x7C, 0x76, \
                                          0x5B, 0x56, 0xF0, 0xBA, 0x03, 0xF4, \
                                          0xAA, 0x67}

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* BLE_PROTOCOL_CONFIG_H */
