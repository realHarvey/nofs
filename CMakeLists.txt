#
# useful on windows platform
# not recommended on linux
#
cmake_minimum_required(VERSION 3.22)

# 目标为无系统平台
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)

set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_C_COMPILER   arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(nofs C ASM)
set(CMAKE_C_STANDARD 11)

# 编译
add_compile_options(
        -mcpu=cortex-m3
        -mthumb
        -mfloat-abi=soft
        -ffunction-sections
        -fdata-sections
        -fno-common
        -fmessage-length=0
        -fno-builtin
        $<$<COMPILE_LANGUAGE:ASM>:-x$<SEMICOLON>assembler-with-cpp> # 允许汇编器预处理
        -Og -g
)
include_directories(
        kernel/include
)
file(GLOB_RECURSE SOURCES
        "boot/*.*"
        "kernel/*.*"
        "init/*.*"
)

# 链接
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/linker.ld )
add_link_options(
        -mcpu=cortex-m3
        -mthumb
        -Wl,-gc-sections,--print-memory-usage,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map
        -T ${LINKER_SCRIPT}
        -Wl,--no-warn-rwx-segments #消除链接器rwx警告
)
add_executable(
        ${PROJECT_NAME}.elf
        ${SOURCES}
        ${LINKER_SCRIPT}
)

# 额外生成hex和bin
set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE}
Building ${BIN_FILE}")
