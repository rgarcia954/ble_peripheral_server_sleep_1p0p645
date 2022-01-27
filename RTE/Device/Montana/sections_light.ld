/**
 * @file sections_light.ld
 * @brief This is a special linker file configuration where only the 4 DRAM instances are alllocated.
 *        The purpose of this to demonstrate low power consumption of device with minimum
 *        amount of DRAM powered ON
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
 * @cond NO_DOXYGEN
 */

/* Total DRAM in the system is defined as 64K, this is split into three
 * sections:
 *  ROM:    Contains the definition of the SystemCoreClock initialised
 *          By the boot rom.
 *  Stack:  The stack area is defined here so that if it over-runs then it will
 *          cause a hard fault.
 *  Usable: The rest of the RAM is available for normal use with statics and
 *          heap.
 */ 

/** @brief DRAM base address */ 
_DRAM_Total_Base = 0x20000000;

/** @brief DRAM total size is defined as 64K */ 
_DRAM_Total_Size = 0x8000;

/** @brief Wakeup address is at the end of DRAM minus 32 */
_DRAM_Wakeup_Reserved_Size = 0x20;

/** @brief DRAM base address */ 
_DRAM_Total_Top = _DRAM_Total_Base + _DRAM_Total_Size;

/**
 * @brief Size of area reserved for ROM use
 * @details
 * By default we reserve 6 words to provide ROM status information.
 * - Word 0    - SystemCoreClock
 * - Words 1-4 - ROM Status values (Refer to ROMStatus.h)
 * - Word 5    - Reserved
 *
 * @note Only the first of these words needs to be maintained. If the
 * application is not interested in the ROM status words, then the size can be
 * reduced to 0x04, the system core clock value should be maintained.
 */
_DRAM_ROM_Reserved = 0x18;

/** @brief Available DRAM size is the total minus that reserved for the ROM */
_DRAM_Available_Size = _DRAM_Total_Size - _DRAM_ROM_Reserved - _DRAM_Wakeup_Reserved_Size;

/** @brief Define the stack size as a constant 2K here */
_DRAM_Stack_Size = 2048;

/** @brief Define the stack area to be at the top of RAM */
_DRAM_Stack_Base = _DRAM_Total_Top - _DRAM_Stack_Size - _DRAM_Wakeup_Reserved_Size;

/** @brief The application usable DRAM will start after the ROM Reserved Area */
_DRAM_Base = _DRAM_Total_Base + _DRAM_ROM_Reserved;

/**@brief The DRAM size will be defined as available minus stack */
_DRAM_Size = _DRAM_Available_Size - _DRAM_Stack_Size;

/*
 * Define the memory map
 *
 * Note that this uses the stack definitions from startup.S to control the
 * layout of the sections as described above.
 */
MEMORY
{
    /* The ROM is defined to be the bottom 24K of memory */
    ROM  (r)            : ORIGIN = 0x00000000, LENGTH = 24K   
  
    /* 720K of code flash is available, but the top 16K is reserved */
    FLASH (xrw)         : ORIGIN = 0x00100000, LENGTH = 704K
    
    /* The ROM requires the first 3K of data flash for DEU transfer space */
    FLASH_RSVD (xrw)    : ORIGIN = 0x001B0000, LENGTH = 3K

    /* Reserve 2k for Bluetooth bond information */
    FLASH_BOND_RSVD (xrw)    : ORIGIN = 0x001B0C00, LENGTH = 2K

    /* The rest of the data flash is available for application use */
    FLASH_DATA (xrw)    : ORIGIN = 0x001B1400, LENGTH = 315K
  
    /* Define the ROM reserved area of DRAM */
    DRAM_ROM (xrw)      : ORIGIN = _DRAM_Total_Base, LENGTH = _DRAM_ROM_Reserved
    
    /* Application available DRAM */
    DRAM (xrw)          : ORIGIN = _DRAM_Base, LENGTH = _DRAM_Size
    
    /* The stack */
    DRAM_STACK (xrw)    : ORIGIN = _DRAM_Stack_Base, LENGTH = _DRAM_Stack_Size
    
    /* Reserved area for the wakeup elements */
    DRAM_WAKEUP_RSVD (xrw)    : ORIGIN = (_DRAM_Total_Top - _DRAM_Wakeup_Reserved_Size), LENGTH = _DRAM_Wakeup_Reserved_Size    
    
    /* Baseband specific DRAM */
    DRAM_BB (xrw)       : ORIGIN = 0x20010000, LENGTH = 16K
    
    /* NFC specific DRAM */
    DRAM_NFC (xrw)      : ORIGIN = 0x20048000, LENGTH = 1K
}

/** @brief The cmsis library needs to know the relocated address of the stack */
PROVIDE(__stack_limit = ORIGIN(DRAM_STACK));
PROVIDE(__stack = ORIGIN(DRAM_STACK) + LENGTH(DRAM_STACK));
PROVIDE(__Wakeup_addr = ORIGIN(DRAM_WAKEUP_RSVD));

/* Define the heap to run from the end of the static data to the top of RAM
 */
PROVIDE (__Heap_Begin__ = __noinit_end__);
PROVIDE (__Heap_Limit__ = ORIGIN(DRAM) + LENGTH(DRAM));

/* The entry point is informative, for debuggers and simulators,
 * since the Cortex-M vector points to it anyway.
 */
ENTRY(Reset_Handler)

/* As is the VTOR register, we refer to it in startup documentation
 */
__VTOR = 0xE000ED08;

/* ----------------------------------------------------------------------------
 * Section definitions
 * ------------------------------------------------------------------------- */
SECTIONS
{
    /* For Cortex-M devices, the beginning of the startup code is stored in
     * the .interrupt_vector section, which goes to FLASH 
     */
    .text :
    {
        . = ALIGN(4);
        KEEP(*(.interrupt_vector)) 

        /* This section is here to store the startup code immediately after
         * the interrupt vectors, as required by the program ROM.
         */
        KEEP(*(.reset))

        /* Pre-initialization Code */
        . = ALIGN(4);
        PROVIDE_HIDDEN (__preinit_array_start__ = .);

        /* System initialization and the platform initialization (if present)
         * should be first */
        KEEP(*(.preinit_array_sysinit .preinit_array_sysinit.*))
        KEEP(*(.preinit_array_platform .preinit_array_platform.*))

        /* Pre-initialization functions (to be executed before C++ 
         * constructors are run) */
        KEEP(*(.preinit_array .preinit_array.*))

        PROVIDE_HIDDEN (__preinit_array_end__ = .);

        /* Initialization Code */
        . = ALIGN(4);
        PROVIDE_HIDDEN (__init_array_start__ = .);

        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))

        PROVIDE_HIDDEN (__init_array_end__ = .);

        /* The program code is stored in the .text section, 
         * which goes to FLASH.
         */
        . = ALIGN(4);
   
        *(.text .text.*)            /* all remaining code */
        *(.rodata .rodata.*)        /* read-only data (constants) */

        . = ALIGN(4);
        
    } >FLASH
    
        . = ALIGN(4);
    __data_init__ = .;

    /* Place the SystemClock variable needed for CMSIS in a place that is
     * compatible with the ROM's placement of this variable so that the 
     * variable can be used by CMSIS and the ROM's flash write libary
     */
    .systemclock (NOLOAD) :
    {
        . = ALIGN(4);
        KEEP(*(.systemclock))
    } > DRAM_ROM
   
    /* Provide a fixed location for the ROM status words if used
     */
    .romstatus (NOLOAD)  : 
    {
        . = ALIGN(4);
        KEEP(*(.romstatus))
    } >DRAM_ROM
      
    /* Provide a fixed location for wakeup address section
     */
     .wakeupsection :
     {
        . = ALIGN(4);
        app_ptr = .; /* Place wakeuparray app_ptr at current address, ie first address of DRAM_WAKEUP_RSVD */
        . = ALIGN(4);     
     } >DRAM_WAKEUP_RSVD
           
    /* This address is used by the startup code to initialize the .data
     * section.
     */

    /* The initialised data section.
     *
     * The program executes knowing that the data is in the RAM
     * but the loader puts the initial values in the FLASH (inidata).
     * It is one task of the startup to copy the initial values from 
     * FLASH to RAM.
     */
    .data  : AT ( __data_init__ )
    {
        . = ALIGN(4);

        /* This is used by the startup code to initialize the .data section */
        __data_start__ = . ;
        *(.data_begin .data_begin.*)
        *(.wakeup_section)
        *(.data .data.*)
        *(.data_end .data_end.*)
        . = ALIGN(4);

        /* This is used by the startup code to initialize the .data section */
        __data_end__ = . ;

    } >DRAM
      
    /*
     * The uninitialized data section. NOLOAD is used to avoid
     * the "section `.bss' type changed to PROGBITS" warning
     */
    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss_begin .bss_begin.*)

        *(.bss .bss.*)
        *(COMMON)
        
        *(.bss_end .bss_end.*)
        . = ALIGN(4);
        __bss_end__ = .;
    } >DRAM
    
    .noinit (NOLOAD) :
    {
        . = ALIGN(4);
        __noinit_start__ = .;
        
        *(.noinit .noinit.*) 
        
         . = ALIGN(4) ;
        __noinit_end__ = .;   
    } > DRAM
    
    /* Check if there is enough space to allocate the main stack */
    ._stack (NOLOAD) :
    {
        . = ALIGN(4);
        
        . = . + _DRAM_Stack_Size;
        
        . = ALIGN(4);
    } >DRAM_STACK

}
