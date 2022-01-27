/**
 * @file app_msg_handler.h
 * @brief Application message handlers header
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

#ifndef APP_MSG_HANDLER_H
#define APP_MSG_HANDLER_H

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

#include <ke_msg.h>

void BLE_ConfigHandler(ke_msg_id_t const msg_id, void const *param,
                            ke_task_id_t const dest_id,
                            ke_task_id_t const src_id);
void BLE_ActivityHandler(ke_msg_id_t const msg_id, void const *param,
                            ke_task_id_t const dest_id,
                            ke_task_id_t const src_id);
void BLE_ConnectionHandler(ke_msg_id_t const msg_id, void const *param,
                            ke_task_id_t const dest_id,
                            ke_task_id_t const src_id);
void BLE_PairingHandler(ke_msg_id_t const msg_id, void const *param,
                            ke_task_id_t const dest_id,
                            ke_task_id_t const src_id);


void PrepareAdvScanData(void);

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* APP_MSG_HANDLER_H */
