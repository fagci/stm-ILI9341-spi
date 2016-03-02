cmake_minimum_required(VERSION 3.4)
project(Test2)

set(PERIPH_LIB /home/fagci/libraries/STM32F10x_StdPeriph_Lib_V3.5.0/)
include_directories(
        ${PERIPH_LIB}/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/
        ${PERIPH_LIB}/Libraries/CMSIS/CM3/CoreSupport/
        ${PERIPH_LIB}/Libraries/STM32F10x_StdPeriph_Driver/inc/
        ${PERIPH_LIB}/Project/STM32F10x_StdPeriph_Template
)
set(PERIPH_LIB_SRC ${PERIPH_LIB}/Libraries/STM32F10x_StdPeriph_Driver/src/)

set(CMAKE_C_COMPILER "arm-none-eabi-gcc")
set(CMAKE_LINKER "arm-none-eabi-ld")
set(CMAKE_OBJCOPY "arm-none-eabi-objcopy")

set(LINK_SCRIPT /home/fagci/ClionProjects/Test2/stm32f10x.ld)

set(STARTUP system_stm32f10x.c startup_stm32f10x.c)
set(STARTUP ${STARTUP}
        ${PERIPH_LIB_SRC}/stm32f10x_rcc.c
        ${PERIPH_LIB_SRC}/stm32f10x_tim.c
        ${PERIPH_LIB_SRC}/stm32f10x_gpio.c
        ${PERIPH_LIB_SRC}/stm32f10x_exti.c
        ${PERIPH_LIB_SRC}/stm32f10x_spi.c
        ${PERIPH_LIB_SRC}/stm32f10x_dma.c
        ${PERIPH_LIB_SRC}/stm32f10x_usart.c
        ${PERIPH_LIB_SRC}/misc.c
                tiny_stdlib/itoa.c
        ILI9341_lib/delay.c
        ILI9341_lib/core.c
        ILI9341_lib/dma.c
        ILI9341_lib/graph.c
        ILI9341_lib/text.c
                USART_lib/usart.c
        #        test_suite/fps_counter.c
        #        test_suite/fill.c
                test_suite/primitives.c
        )

set(DEBUG_FLAGS "-O0 -g")

set(MCU_FLAGS "-march=armv7-m -mcpu=cortex-m3 -mthumb")
set(CMAKE_C_FLAGS "${MCU_FLAGS} -fno-common -fno-builtin -nostdlib -ffreestanding -Wall ${DEBUG_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
add_definitions(-DSTM32F103C8 -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER)
set(CMAKE_EXE_LINKER_FLAGS "${DEBUG_FLAGS} -T${LINK_SCRIPT}")
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")

add_executable(${PROJECT_NAME}.elf main.c ${STARTUP})

add_custom_command(DEPENDS ${PROJECT_NAME}.elf OUTPUT ${PROJECT_NAME}.hex COMMAND ${CMAKE_OBJCOPY} -Oihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex)
add_custom_command(DEPENDS ${PROJECT_NAME}.elf OUTPUT ${PROJECT_NAME}.bin COMMAND ${CMAKE_OBJCOPY} -Obinary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin)

add_custom_target(generate_bin ALL DEPENDS ${PROJECT_NAME}.bin)
add_custom_target(generate_hex ALL DEPENDS ${PROJECT_NAME}.hex)

add_custom_target(write stm32flash -w ${PROJECT_NAME}.bin -g 0x0 /dev/ttyUSB1 DEPENDS ${PROJECT_NAME}.bin)
