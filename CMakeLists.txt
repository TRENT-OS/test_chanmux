cmake_minimum_required(VERSION 3.7.2)

seos_set_config_file(seos_system_config.h)

seos_use_libs(
    SEOS_LIBS
    SEOS_CHANMUX
)


project(seos_system_config C)
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR})


project(tests_chanmux C)

DeclareCAmkESComponent(ChanMux_Tester1
    SOURCES
        src/ChanMux_Tester1.c
        src/ChanMux_Tester_Common.c
    C_FLAGS
        -Wall -Werror
    LIBS
        seos_system_config
        seos_core_api
        seos_libs
        chanmux_client
)

DeclareCAmkESComponent(ChanMux_Tester2
    SOURCES
        src/ChanMux_Tester2.c
        src/ChanMux_Tester_Common.c
    C_FLAGS
        -Wall -Werror
    LIBS
        seos_system_config
        seos_core_api
        seos_libs
        chanmux_client
)

DeclareCAmkESComponent(UartDrv
    SOURCES
        components/Uart/src/UartDrv.c
        components/Uart/src/qemu_uart.c
    C_FLAGS
        -Wall -Werror
)

DeclareCAmkESComponent(ChanMux
    SOURCES
        components/ChanMux/src/ChanMux.c
    C_FLAGS
        -Wall -Werror
    LIBS
        seos_system_config
        seos_core_api
        seos_libs
        chanmux_server
)

#ROOT SERVER
DeclareCAmkESRootserver(main.camkes)
GenerateCAmkESRootServer()
GenerateSimulateScript()
