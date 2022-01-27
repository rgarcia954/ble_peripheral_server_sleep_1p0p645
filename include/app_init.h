/**
 * @file app_init.h
 * @brief Application initialization header
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

#ifndef APP_INIT_H
#define APP_INIT_H

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

void DeviceInit(void);
void AppMsgHandlersInit(void);
void BatteryServiceServerInit(void);
void CustomServiceServerInit(void);
void IRQPriorityInit(void);
void BLEStackInit(void);
void DisableAppInterrupts(void);
void EnableAppInterrupts(void);
void App_GPIO_Config(void);
void App_Clock_Config(void);
void App_Sleep_Initialization(void);
uint32_t Power_Down_FPU(void);
uint32_t Power_Down_Debug(void);
/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* APP_INIT_H */
