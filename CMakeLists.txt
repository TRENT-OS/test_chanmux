#
# Test ChanMux server
#
# Copyright (C) 2019-2020, Hensoldt Cyber GmbH
#

cmake_minimum_required(VERSION 3.7.2)

os_set_config_file(
    system_config.h
    CREATE_CONFIG_PROJECT # name is "system_config"
)


#-------------------------------------------------------------------------------
project(tests_chanmux C)

DeclareCAmkESComponent(
    ChanMux_Tester1
    SOURCES
        src/ChanMux_Tester1.c
        src/ChanMux_Tester_Common.c
    C_FLAGS
        -Wall
        -Werror
    LIBS
        os_core_api
        lib_debug
        lib_macros
        lib_utils
        chanmux_client
)

DeclareCAmkESComponent(
    ChanMux_Tester2
    SOURCES
        src/ChanMux_Tester2.c
        src/ChanMux_Tester_Common.c
    C_FLAGS
        -Wall
        -Werror
    LIBS
        os_core_api
        lib_debug
        lib_macros
        lib_utils
        chanmux_client
)

ChanMux_UART_DeclareCAmkESComponents(
    ChanMux_UART
    components/ChanMux/ChanMux_config.c
    system_config
)

#ROOT SERVER
DeclareAndCreateCamkESSystem(main.camkes)
GenerateSimulateScript()
