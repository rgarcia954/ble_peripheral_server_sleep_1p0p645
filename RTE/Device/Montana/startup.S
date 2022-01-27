/**
 * @file startup.s
 * @brief GCC compatible CMSIS Cortex-M33 Core Device start-up file for the  
 *        ARM Cortex-M33 processor
 *
 * Assumes the use of CodeSourcery/Mentor Sourcery G++ with its CodeSourcery
 * Common Startup Code Sequence (CS3) naming and conventions.  Note, however,
 * that the default reset handler, Reset_Handler, does not follow CS3
 * conventions because it calls a routine (SystemInit) that is typically
 * written in C.  The reset handler should (in general) not assume that the
 * stack pointer is set up already for C functions, even though it is set up
 * at reset by the Cortex-M33 hardware.  The CS3 _start() function that
 * Reset_Handler calls will set up the stack pointer again, before calling
 * __cs3_start_c to initialize the C program's data areas and call main().
 * 
 * @copyright @parblock
 * Copyright (c) 2019-2020 Semiconductor Components Industries, LLC (d/b/a
 * ON Semiconductor), All Rights Reserved
 *
 * This code is the property of ON Semiconductor and may not be redistributed
 * in any form without prior written permission from ON Semiconductor.
 * The terms of use and warranty for this code are covered by contractual
 * agreements between ON Semiconductor and the licensee.
 *
 * @endparblock
 */

/* ----------------------------------------------------------------------------
 * Stack Configuration
 * ------------------------------------------------------------------------- */
/*
    .section ".stack", "w"
    .align  3
    .globl  __stack_mem
    .globl  __stack_size
__stack_mem:
    .if     Stack_Size
    .space  Stack_Size
    .endif
    .size   __stack_mem,  . - __stack_mem
    .set    __stack_size, . - __stack_mem
*/


/* ----------------------------------------------------------------------------
 * Heap Configuration
 * ------------------------------------------------------------------------- */
    .equ    Heap_Size,  0  /* Bytes - heap is empty by default */

    .section ".heap", "w"
    .align  3
    .globl  __heap_start
    .globl  __heap_end
__heap_start:
    .if     Heap_Size
    .space  Heap_Size
    .endif
__heap_end:


/* ----------------------------------------------------------------------------
 * Vector Table
 * - Mapped to address 0 at Reset
 * ------------------------------------------------------------------------- */

    .section ".interrupt_vector"
    .globl  ISR_Vector_Table
    .type   ISR_Vector_Table, %object
    .globl  __stack


ISR_Vector_Table:
    .long   __stack                         /*  0 Initial Stack Pointer */
    .long   Reset_Handler                   /*  1 Reset Handler */
    .long   NMI_Handler                     /*  2 Non-Maskable Interrupt Handler */
    .long   HardFault_Handler               /*  3 Hard Fault Handler */
    .long   MemManage_Handler               /*  4 Mem Manage Fault Handler */
    .long   BusFault_Handler                /*  5 Bus Fault Handler */
    .long   UsageFault_Handler              /*  6 Usage Fault Handler */
    .long   SecureFault_Handler             /*  7 Secure Fault Handler */
    .long   0                               /*  8 Reserved */
    .long   0                               /*  9 Reserved */
    .long   0                               /* 10 Reserved */
    .long   SVC_Handler                     /* 11 Supervisor Call Handler */
    .long   DebugMon_Handler                /* 12 Debug Monitor Handler */
    .long   0                               /* 13 Reserved */
    .long   PendSV_Handler                  /* 14 PendSV Handler */
    .long   SysTick_Handler                 /* 15 SysTick Handler */

    /* External Interrupts */
    .long   WAKEUP_IRQHandler                     /* 16 Wake-up interrupt */
    .long   RTC_ALARM_IRQHandler                  /* 17 RTC alarm interrupt */
    .long   RTC_CLOCK_IRQHandler                  /* 18 RTC clock interrupt */
    .long   LSAD_MONITOR_IRQHandler               /* 19 LSAD Battery monitor interrupt */
    .long   TIMER0_IRQHandler                     /* 20 Timer0 interrupt */
    .long   TIMER1_IRQHandler                     /* 21 Timer1 interrupt */
    .long   TIMER2_IRQHandler                     /* 22 Timer2 interrupt */
    .long   TIMER3_IRQHandler                     /* 23 Timer3 interrupt */
    .long   NFC_IRQHandler                        /* 24 NFC interrupt */
    .long   FIFO_IRQHandler                       /* 25 FIFO interrupt */
    .long   GPIO0_IRQHandler                      /* 26 GPIO0 interrupt */
    .long   GPIO1_IRQHandler                      /* 27 GPIO1 interrupt */
    .long   GPIO2_IRQHandler                      /* 28 GPIO2 interrupt */
    .long   GPIO3_IRQHandler                      /* 29 GPIO3 interrupt */
    .long   WATCHDOG_IRQHandler                   /* 30 Watchdog interrupt */
    .long   SPI0_RX_IRQHandler                    /* 31 SPI receive interrupt */
    .long   SPI0_TX_IRQHandler                    /* 32 SPI transmit interrupt */
    .long   SPI0_COM_IRQHandler                   /* 33 SPI error interrupt */
    .long   I2C0_IRQHandler                       /* 34 I2C interrupt */
    .long   UART0_RX_IRQHandler                   /* 35 UART receive interrupt */
    .long   UART0_TX_IRQHandler                   /* 36 UART transmit interrupt */
    .long   UART0_ERROR_IRQHandler                /* 37 UART error interrupt */
    .long   BLE_SW_IRQHandler                     /* 38 SW triggered interrupt, generated on SW request */
    .long   BLE_FINETGT_IRQHandler                /* 39 BLE fine timer target interrupt */
    .long   BLE_TIMESTAMP_TGT1_IRQHandler         /* 40 BLE timestamp target 1 interrupt generated on a defined instant */
    .long   BLE_TIMESTAMP_TGT2_IRQHandler         /* 41 BLE timestamp target 2 interrupt generated on a defined instant */
    .long   BLE_CRYPT_IRQHandler                  /* 42 BLE encryption engine interrupt */
    .long   BLE_SLP_IRQHandler                    /* 43 BLE sleep mode interrupt generated at end of sleep period */
    .long   BLE_HSLOT_IRQHandler                  /* 44 BLE half slot interrupt, generated each 312.5µs */
    .long   BLE_FIFO_IRQHandler                   /* 45 Activity FIFO interrupt */
    .long   BLE_ERROR_IRQHandler                  /* 46 BLE error interrupt */
    .long   BLE_COEX_IN_PROCESS_IRQHandler        /* 47 WLAN coexistence BLE in process interrupt */
    .long   BLE_COEX_RX_TX_IRQHandler             /* 48 WLAN coexistence Rx/Tx interrupt */
    .long   TOF_IRQHandler                        /* 49 Time of Flight counter interrupt */
    .long   RF_TX_IRQHandler                      /* 50 RF end of packet transmission interrupt */
    .long   RF_RXSTOP_IRQHandler                  /* 51 RF RX FSM stop interrupt */
    .long   RF_IRQ_RECEIVED_IRQHandler            /* 52 RF packet receive interrupt */
    .long   RF_SYNC_IRQHandler                    /* 53 RF sync word detection interrupt */
    .long   RF_TXFIFO_IRQHandler                  /* 54 TX FIFO near underflow detect interrupt */
    .long   RF_RXFIFO_IRQHandler                  /* 55 RX FIFO near overflow detect interrupt */
    .long   FLASH0_COPY_IRQHandler                /* 56 Flash0 copy interrupt */
    .long   FLASH0_ECC_IRQHandler                 /* 57 Flash0 ECC event interrupt */
    .long   FLASH1_COPY_IRQHandler                /* 58 Flash1 copy interrupt */
    .long   FLASH1_ECC_IRQHandler                 /* 59 Flash1 ECC event interrupt */
    .long   ACCESS_ERROR_IRQHandler               /* 60 Memory access error interrupt */
    .long   FPU_IRQHandler                        /* 61 Floating Point Unit exception interrupt */
    .long   DMA0_IRQHandler                       /* 62 DMA Channel 0 interrupt */
    .long   DMA1_IRQHandler                       /* 63 DMA Channel 1 interrupt */
    .long   DMA2_IRQHandler                       /* 64 DMA Channel 2 interrupt */
    .long   DMA3_IRQHandler                       /* 65 DMA Channel 2 interrupt */
    .long   CC312_IRQHandler                      /* 66 Crypto Cell (CC312) interrupt */
    .long   ASCC_PHASE_IRQHandler                 /* 67 ASCC Phase Interrupt */
    .long   ASCC_PERIOD_IRQHandler                /* 68 ASCC Period Interrupt */

    .size   ISR_Vector_Table, . - ISR_Vector_Table

    .thumb

/* ----------------------------------------------------------------------------
 * Reset Handler
 * ------------------------------------------------------------------------- */

    .section .reset,"x",%progbits
    .thumb_func
    .globl  Reset_Handler
    .type   Reset_Handler, %function
Reset_Handler:
    .fnstart
    LDR     R0, =SystemInit
    BLX     R0
    LDR     R0, =_start
    BX      R0
    .pool
    .cantunwind
    .fnend
    .size   Reset_Handler, . - Reset_Handler

    .section ".text"

/* ----------------------------------------------------------------------------
 * Place-holder Exception Handlers
 * - Weak definitions
 * - If not modified or replaced, these handlers implement infinite loops
 * ------------------------------------------------------------------------- */
    .weak   NMI_Handler
    .type   NMI_Handler, %function
NMI_Handler:
    B       .
    .size   NMI_Handler, . - NMI_Handler

    .weak   HardFault_Handler
    .type   HardFault_Handler, %function
HardFault_Handler:
    B       .
    .size   HardFault_Handler, . - HardFault_Handler

    .weak   MemManage_Handler
    .type   MemManage_Handler, %function
MemManage_Handler:
    B       .
    .size   MemManage_Handler, . - MemManage_Handler

    .weak   BusFault_Handler
    .type   BusFault_Handler, %function
BusFault_Handler:
    B       .
    .size   BusFault_Handler, . - BusFault_Handler

    .weak   UsageFault_Handler
    .type   UsageFault_Handler, %function
UsageFault_Handler:
    B       .
    .size   UsageFault_Handler, . - UsageFault_Handler

    .weak   SecureFault_Handler
    .type   SecureFault_Handler, %function
SecureFault_Handler:
    B       .
    .size   SecureFault_Handler, . - SecureFault_Handler

    .weak   SVC_Handler
    .type   SVC_Handler, %function
SVC_Handler:
    B       .
    .size   SVC_Handler, . - SVC_Handler

    .weak   DebugMon_Handler
    .type   DebugMon_Handler, %function
DebugMon_Handler:
    B       .
    .size   DebugMon_Handler, . - DebugMon_Handler

    .weak   PendSV_Handler
    .type   PendSV_Handler, %function
PendSV_Handler:
    B       .
    .size   PendSV_Handler, . - PendSV_Handler

    .weak   SysTick_Handler
    .type   SysTick_Handler, %function
SysTick_Handler:
    B       .
    .size   SysTick_Handler, . - SysTick_Handler

/* ----------------------------------------------------------------------------
 * Default Interrupt Request Handlers for External Interrupts
 * ------------------------------------------------------------------------- */

    .globl  Default_Handler
    .type   Default_Handler, %function
Default_Handler:
    B       .
    .size   Default_Handler, . - Default_Handler

    .macro  IRQ handler
    .weak   \handler
    .set    \handler, Default_Handler
    .endm

    IRQ   WAKEUP_IRQHandler
    IRQ   RTC_ALARM_IRQHandler
    IRQ   RTC_CLOCK_IRQHandler
    IRQ   LSAD_MONITOR_IRQHandler
    IRQ   TIMER0_IRQHandler
    IRQ   TIMER1_IRQHandler
    IRQ   TIMER2_IRQHandler
    IRQ   TIMER3_IRQHandler
    IRQ   NFC_IRQHandler
    IRQ   FIFO_IRQHandler
    IRQ   GPIO0_IRQHandler
    IRQ   GPIO1_IRQHandler
    IRQ   GPIO2_IRQHandler
    IRQ   GPIO3_IRQHandler
    IRQ   WATCHDOG_IRQHandler
    IRQ   SPI0_RX_IRQHandler
    IRQ   SPI0_TX_IRQHandler
    IRQ   SPI0_COM_IRQHandler
    IRQ   I2C0_IRQHandler
    IRQ   UART0_RX_IRQHandler
    IRQ   UART0_TX_IRQHandler
    IRQ   UART0_ERROR_IRQHandler
    IRQ   BLE_SW_IRQHandler
    IRQ   BLE_FINETGT_IRQHandler
    IRQ   BLE_TIMESTAMP_TGT1_IRQHandler
    IRQ   BLE_TIMESTAMP_TGT2_IRQHandler
    IRQ   BLE_CRYPT_IRQHandler
    IRQ   BLE_SLP_IRQHandler
    IRQ   BLE_HSLOT_IRQHandler
    IRQ   BLE_FIFO_IRQHandler
    IRQ   BLE_ERROR_IRQHandler
    IRQ   BLE_COEX_IN_PROCESS_IRQHandler
    IRQ   BLE_COEX_RX_TX_IRQHandler
    IRQ   TOF_IRQHandler
    IRQ   RF_TX_IRQHandler
    IRQ   RF_RXSTOP_IRQHandler
    IRQ   RF_IRQ_RECEIVED_IRQHandler
    IRQ   RF_SYNC_IRQHandler
    IRQ   RF_TXFIFO_IRQHandler
    IRQ   RF_RXFIFO_IRQHandler
    IRQ   FLASH0_COPY_IRQHandler
    IRQ   FLASH0_ECC_IRQHandler
    IRQ   FLASH1_COPY_IRQHandler
    IRQ   FLASH1_ECC_IRQHandler
    IRQ   ACCESS_ERROR_IRQHandler
    IRQ   FPU_IRQHandler
    IRQ   DMA0_IRQHandler
    IRQ   DMA1_IRQHandler
    IRQ   DMA2_IRQHandler
    IRQ   DMA3_IRQHandler
    IRQ   CC312_IRQHandler
    IRQ   ASCC_PHASE_IRQHandler
    IRQ   ASCC_PERIOD_IRQHandler

    .end

