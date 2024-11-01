#
# Test ChanMux server
#
# Copyright (C) 2019-2024, HENSOLDT Cyber GmbH
# 
# SPDX-License-Identifier: GPL-2.0-or-later
#
# For commercial licensing, contact: info.cyber@hensoldt.net
#

cmake_minimum_required(VERSION 3.7.2)

#-------------------------------------------------------------------------------
# use the SDK
find_package("os-sdk" REQUIRED)
os_sdk_set_defaults()
os_sdk_setup(CONFIG_FILE "system_config.h" CONFIG_PROJECT "system_config")


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

if(("${KernelPlatform}" STREQUAL "tx1") OR ("${KernelPlatform}" STREQUAL "tx2") OR ("${KernelPlatform}" STREQUAL "xavier"))
    SystemController_DeclareCAmkESComponent(
        SystemController
    )
endif()

#ROOT SERVER
os_sdk_create_CAmkES_system("main.camkes")
