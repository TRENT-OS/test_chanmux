cmake_minimum_required(VERSION 3.7.2)

seos_set_config_file(config.h)

seos_use_libs(SEOS_LIBS)

project(tests_chanmux C)

DeclareCAmkESComponent(ChanMux_Tester1
    INCLUDES
		components/ChanMux/src/instance
    SOURCES
        src/ChanMux_Tester1.c
        src/ChanMux_Tester_Common.c
    C_FLAGS
        -Wall -Werror
    LIBS
        seos_core_api
        seos_libs
)

DeclareCAmkESComponent(ChanMux_Tester2
    INCLUDES
        components/ChanMux/src/instance
    SOURCES
        src/ChanMux_Tester2.c
        src/ChanMux_Tester_Common.c
    C_FLAGS
        -Wall -Werror
    LIBS
        seos_core_api
        seos_libs
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
        components/ChanMux/src/instance/ChanMux_Instance.c
    C_FLAGS
        -Wall -Werror
    LIBS
        seos_core_api
        seos_libs
)

#ROOT SERVER
DeclareCAmkESRootserver(main.camkes)
GenerateCAmkESRootServer()
GenerateSimulateScript()
