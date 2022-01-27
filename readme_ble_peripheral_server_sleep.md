Bluetooth Low Energy Peripheral Server Sleep Sample Code
========================================================

NOTE: If you use this sample application for your own purposes, follow the
      licensing agreement specified in `Software_Use_Agreement.rtf` in the
      home directory of the installed Software Development Kit (SDK).

Overview
--------

This Bluetooth Low Energy application demonstrates a peripheral device with one 
standard service (Battery service server) and one application-defined (custom) service 
server. 

Application allows to configure BLE address type (public or private) and location to read the BLE address from. 
�	By default, private address type is selected (`GAPM_ADDRESS_TYPE` is configured as 
		`GAPM_CFG_ADDR_PRIVATE` in `app.h`), and the application will use `APP_BLE_PRIVATE_ADDR' 
		defined in `app.h` as the device private address.
�	BLE address type can be re-configured to public by changing `GAPM_ADDRESS_TYPE`
		from `GAPM_CFG_ADDR_PRIVATE` to `GAPM_CFG_ADDR_PUBLIC` in `app.h`
�	When public address type is selected, application will read the public address from a 
		location defined by `APP_BLE_PUBLIC_ADDR_LOC` in `app.h`. By default, `APP_BLE_PUBLIC_ADDR_LOC` 
		is configured to `BLE_PUBLIC_ADDR_LOC_MNVR` which is a location in MNVR. 
		However, any other valid locations can be used as needed.

Key operations performed by the application:

1. Generates battery service and custom service
2. Performs undirected connectable advertising
3. By default, it supports up to four simultaneous connections. This can be configured in 
`app.h` (the Bluetooth Low Energy stack currently supports ten connections)
4. Any central device can:  
    * Scan, connect, pair/bond/encrypt (legacy or secure bond)
    * Perform service discovery
    * Read characteristic values from both battery and custom services
5. The application sends periodic notifications of the battery level and custom service 
   characteristics to the connected peer devices (clients)

The Sleep Mode of the device is supported by the Bluetooth Low Energy
library and the system library. In each loop of the main
application, after routine operations (including battery level readings
and service notifications) are performed, if the system can switch to Sleep
Mode, Bluetooth Low Energy configurations and states are saved
and the system is put into Sleep Mode. The system is then wakened up by the
Bluetooth Low Energy baseband timer. On waking up, configurations
and states are restored; therefore, the Bluetooth Low Energy connection with 
the central device (established before going to Sleep Mode) and normal 
operations of the application are resumed.

This sample app demontrate the core retention during sleep. FIFO wakeup is configured every 
250 msec. GPIO1 wakeup can also be executed by applying rising edge on GPIO1 Pin.

The default TRIM values for VDDC and VDDM has been set to 1.15V and 1.10V 
respectively in order to support reliable operation during extended temperature.
This values can be further reduced depending on the operating temperature of
the device to reduce the overall power consumption.
    
Verification
------------
To verify that this application is functioning correctly, use a central
device to scan and establish a connection with this peripheral device. 

This application uses the following GPIO pins:
* RECOVERY\_GPIO - To activate the Recover Mode.
* SYSCLK\_GPIO - To output the system clock. 
* POWER\_MODE\_GPIO - To indicate Power Mode.
* WAKEUP\_ACTIVITY\_SENSOR - To indicate wakeup activity of FIFO wakeup
* WAKEUP\_ACTIVITY\_GPIO  - To indicate wakeup activity of GPIO1

Notes: If required, GPIO1 can be used as a wake-up source (codes to enable and
configure GPIO wake-up source are required in `lowpwr_manager.c`).

To verify that this application is functioning correctly, trace all signals
as follows:
- Run Mode:
    - SYSCLK\_GPIO: system clock is 3 or 8 MHz 
    - POWER\_MODE\_GPIO: low     
- Power Mode: 
    - SYSCLK\_GPIO: off for the whole sleep duration
    - POWER\_MODE\_GPIO: high
    
The system keeps cycling through Run and Power modes. The timing of Run-Power
cycles depends on the configured duration of the advertising interval and the 
connection interval. The advertising interval is defined by 
APP\_ADV\_INT\_MIN and APP\_ADV\_INT\_MAX  in `app.h` (by default is set to
40 ms). The connection interval can be configured using the central device
(typical value is 40 ms).   

The device power consumption can be greatly reduced by using the Debug_Light 
or Release_Light configuration.

There are other block that can be turned off to reduced power consumption
This can be find in `app.h`:
* SENSOR\_POWER\_DISABLE - Set this to 1 to turn off Sensor Interface
* CC312AO\_POWER\_DISABLE - Set this to 1 to turn off Crypto Cell
* DEBUG\_SLEEP\_GPIO -  Set this 0 to turn off debug GPIO capability
* POWER\_DOWN\_FPU - Set this to 1 to reduce run mode power consumption
* POWER\_DOWN\_DBG - Set this to 1 to reduce run mode power consumption

The BUCK\_EN is disabled by default and you can set this to have DC-DC enabled.
Use this when VBAT is higher than 1.8 V.

Low Power Clock Selection
-------------------------

The low power clock can be swtiched between XTAL32K or internal RC32K
This can be find in `app.h`
* LPCLK\_STANDBYCLK\_SRC - Set this to LPCLK\_SRC\_XTAL32 to enable XTAL32K for
low power clock or set this to LPCLK\_SRC\_RC32 to enable RC32K for low power clock

* LPCLK_DYNAMIC_UPDATE - If LPCLK\_STANDBYCLK\_SRC == LPCLK\_SRC\_RC32 setting this to 
1 will measure and update RC32K clock to ble stack every LOW\_POWER\_CLK\_MEASUREMENT\_INTERVAL_S
seconds.

* LPCLK_DYNAMIC_UPDATE - If LPCLK\_STANDBYCLK\_SRC == LPCLK\_SRC\_RC32 setting this to 
0 will measure and update RC32K clock to ble stack only once during cold boot reset.

Application files
------------------
`app.h / app.c`: application definitions and the `main()` function  
`app_init.h / app_init.c`: device, libraries and application initialization   
`app_msg_handler.h / app_msg_handler.c`: Bluetooth Low Energy event handlers  
`app_customss.h / app_customss.c`: application-defined Bluetooth Low Energy 
                                             custom service server
`lowpwr_manager.c`: contains necessary functions for sleep modes

Bluetooth Low Energy Abstraction
--------------------------------
This application takes advantage the Bluetooth Low Energy Abstraction layer, on top of 
the Bluetooth Low Energy stack. This provides an extra layer of simplicity and 
modularization to your firmware design, so you can focus on your specific application.

Notes
-----
Sometimes the firmware cannot be successfully re-flashed, due to the
application going into Low Power Mode or resetting continuously (either by design 
or due to programming error). To circumvent this scenario, a software recovery
mode using RECOVERY_GPIO can be implemented with the following steps:

1.  Connect RECOVERY_GPIO to ground.
2.  Press the RESET button (this restarts the application, which pauses at the
    start of its initialization routine).
3.  Re-flash the board. After successful re-flashing, disconnect RECOVERY_GPIO from
    ground, and press the RESET button so that the application can work
    properly.
    
***
Copyright (c) 2021 Semiconductor Components Industries, LLC
(d/b/a onsemi).
