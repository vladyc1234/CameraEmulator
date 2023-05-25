add_executable(cam_emu_main
        main.c
        circular_buffer.c
	packages.c
	crc.c

)
target_link_libraries(cam_emu_main
        pico_stdlib              # for core functionality
       	#pico_cyw43_arch_none     # we need Wifi to access the GPIO, but we don't need anything
	FreeRTOS-Kernel-Heap4 
	hardware_uart
        )
target_include_directories(cam_emu_main PRIVATE
/../../lib/FreeRTOS-Kernel/include
     )
     
target_compile_definitions(cam_emu_main PRIVATE
    FREE_RTOS_KERNEL_SMP=1
    portSUPPORT_SMP=1
)
#target_link_libraries(cam_emu_main pico_stdlib )
#pico_enable_stdio_usb(blink 1)
#pico_enable_stdio_uart(blink 0)
pico_add_extra_outputs(cam_emu_main)
