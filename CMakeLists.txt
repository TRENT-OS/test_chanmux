#
# ChanMux Test System
#
# Copyright (C) 2019-2020, Hensoldt Cyber GmbH
#

cmake_minimum_required(VERSION 3.7.2)

os_set_config_file(system_config.h)

os_use_libs(
    OS_LIBS
    OS_CHANMUX
)


project(system_config C)
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
        seos_core_api
        seos_libs
        chanmux_client
)

DeclareCAmkESComponent_UART(
    UART
)

DeclareCAmkESComponent_ChanMux(
    ChanMux
    components/ChanMux/ChanMux_config.c
    system_config
)

#ROOT SERVER
DeclareAndCreateCamkESSystem(main.camkes)
GenerateSimulateScript()
