




MEMORY
{

  RAM (rw) : ORIGIN = 0x0, LENGTH = 0x80000
}


__SRAM_segment_end__ = _end_main_stack;







__stack = _end_main_stack;
_estack = __stack;







__Main_Stack_Size = 2048 ;

PROVIDE ( _Main_Stack_Size = __Main_Stack_Size ) ;

__Main_Stack_Limit = __stack - __Main_Stack_Size ;


PROVIDE ( _Main_Stack_Limit = __Main_Stack_Limit ) ;





_Minimum_Stack_Size = 1024 ;
_Minimum_Heap_Size = 1024 ;
ENTRY(_start)



SECTIONS
{





 .isr_vector :
 {
  . = ALIGN(4);
  _isr_vector = .;
  KEEP(*(.isr_vector))
  . = ALIGN(4);
 } >RAM


    .text :
    {
        . = ALIGN(4);






        KEEP(*(.after_vectors .after_vectors.*))

        . = ALIGN(4);







        KEEP(*(.init))
        KEEP(*(.fini))

        . = ALIGN(4);





        PROVIDE_HIDDEN (__preinit_array_start = .);




        KEEP(*(.preinit_array_sysinit .preinit_array_sysinit.*))




        KEEP(*(.preinit_array_platform .preinit_array_platform.*))





        KEEP(*(.preinit_array .preinit_array.*))

        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);




        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(4);




        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);
        . = ALIGN(4);

        . = ALIGN(4);

        *(EXCLUDE_FILE(*libarm_cortexM4lf_math.a:*.o *libVP.a:*.o *libVQ.a:*.o) .text*)



        *(vtable)

    } >RAM

    .rodata :
    {

        *(EXCLUDE_FILE(*libarm_cortexM4lf_math.a:*.o *libVP.a:*.o *libVQ.a:*.o) .rodata*)



    } >RAM

    .glue :
    {
        KEEP(*(.eh_frame*))
        *(.glue_7)
        *(.glue_7t)
    } >RAM


    .ARM.extab :
    {
       *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > RAM

    __exidx_start = .;
    .ARM.exidx :
    {
       *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > RAM
    __exidx_end = .;

    . = ALIGN(4);
    _etext = .;
    __etext = .;





    _sidata = _etext;
    .data :
    {
        . = ALIGN(4);


        _sdata = . ;
        __data_start__ = . ;
        *(.data_begin .data_begin.*)

        *(EXCLUDE_FILE(*libarm_cortexM4lf_math.a:*.o *libVP.a:*.o *libVQ.a:*.o) .data*)



        *(.data_end .data_end.*)
        . = ALIGN(4);


        _edata = . ;
        __data_end__ = . ;

    } >RAM





    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        __bss_start__ = .;
        _sbss = .;
        *(.bss_begin .bss_begin.*)

        *(EXCLUDE_FILE(*libarm_cortexM4lf_math.a:*.o *libVP.a:*.o *libVQ.a:*.o) .bss*)
        *(EXCLUDE_FILE(*libarm_cortexM4lf_math.a:*.o *libVP.a:*.o *libVQ.a:*.o) COMMON)




        *(.bss_end .bss_end.*)
        . = ALIGN(4);
        __bss_end__ = .;
        _ebss = . ;
    } >RAM

    .dma_buffer :
    {
        *(.dma_buffer_64)
    } > RAM

    .bench :
    {
        *(.bench)
    } > RAM

    .msp :
    {
      . = ALIGN(4);

      . = . + _Minimum_Stack_Size ;
      . = ALIGN(4);
      _end_main_stack = .;
    } > RAM

    .dsp_bss :
    {
        . = ALIGN(4);
        *libarm_cortexM4lf_math.a:*.o (.bss .bss.* COMMON)
        . = ALIGN(4);
    } >RAM

    .dsp_text_data :
    {
        . = ALIGN(4);
        *libarm_cortexM4lf_math.a:*.o (.text* .rodata* .data*)
        . = ALIGN(4);
    } >RAM

    __vp_start__ = .;
    .vp_bss :
    {
        . = ALIGN(4);
        *libVP.a:*.o (.bss .bss.* COMMON)
        . = ALIGN(4);
    } >RAM

    .vp_text_data :
    {
        . = ALIGN(4);
        *libVP.a:*.o (.text* .rodata* .data*)
        . = ALIGN(4);
    } >RAM
    __vp_end__ = .;

    __vq_start__ = .;
    .vq_bss :
    {
        . = ALIGN(4);
        *libVQ.a:*.o (.bss .bss.* COMMON)
        . = ALIGN(4);
    } >RAM

    .vq_text_data :
    {
        . = ALIGN(4);
        *libVQ.a:*.o (.text* .rodata* .data*)
        . = ALIGN(4);
    } >RAM
    __vq_end__ = .;

    .noinit (NOLOAD) :
    {
        . = ALIGN(4);
        _noinit = .;
        *(.noinit .noinit.*)
         . = ALIGN(4) ;
        _end_noinit = .;
    } > RAM


    PROVIDE ( end = _end_noinit );
    PROVIDE ( _end = _end_noinit );
    PROVIDE ( __end = _end_noinit );
    PROVIDE ( __end__ = _end_noinit );
    PROVIDE ( ROM_DATA_START = __data_start__ );

    .heap :
    {
      . = ALIGN(4);
      _heap_start = .;
      . = . + _Minimum_Heap_Size ;
      _heap_end = .;
      . = ALIGN(4);
    } > RAM

    .bin_end :
    {
        *(.plate_number)
    } > RAM







    ._check_stack :
    {
        . = ALIGN(4);
        . = . + _Minimum_Stack_Size ;
        . = ALIGN(4);
    } >RAM

    ._check_rom_log :
    {
        . = ALIGN(4);



        . = ALIGN(4);
    } >RAM
    .stab 0 : { *(.stab) }
    .stabstr 0 : { *(.stabstr) }
    .stab.excl 0 : { *(.stab.excl) }
    .stab.exclstr 0 : { *(.stab.exclstr) }
    .stab.index 0 : { *(.stab.index) }
    .stab.indexstr 0 : { *(.stab.indexstr) }
    .comment 0 : { *(.comment) }






    .debug 0 : { *(.debug) }
    .line 0 : { *(.line) }

    .debug_srcinfo 0 : { *(.debug_srcinfo) }
    .debug_sfnames 0 : { *(.debug_sfnames) }

    .debug_aranges 0 : { *(.debug_aranges) }
    .debug_pubnames 0 : { *(.debug_pubnames) }

    .debug_info 0 : { *(.debug_info .gnu.linkonce.wi.*) }
    .debug_abbrev 0 : { *(.debug_abbrev) }
    .debug_line 0 : { *(.debug_line) }
    .debug_frame 0 : { *(.debug_frame) }
    .debug_str 0 : { *(.debug_str) }
    .debug_loc 0 : { *(.debug_loc) }
    .debug_macinfo 0 : { *(.debug_macinfo) }

    .debug_weaknames 0 : { *(.debug_weaknames) }
    .debug_funcnames 0 : { *(.debug_funcnames) }
    .debug_typenames 0 : { *(.debug_typenames) }
    .debug_varnames 0 : { *(.debug_varnames) }
}
