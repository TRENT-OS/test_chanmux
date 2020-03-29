/**
 * Copyright (C) 2019, Hensoldt Cyber GmbH
 *
 * SEOS libraries configurations
 *
 */
#pragma once


//-----------------------------------------------------------------------------
// Debug
//-----------------------------------------------------------------------------

#if !defined(NDEBUG)
#   define Debug_Config_STANDARD_ASSERT
#   define Debug_Config_ASSERT_SELF_PTR
#else
#   define Debug_Config_DISABLE_ASSERT
#   define Debug_Config_NO_ASSERT_SELF_PTR
#endif

#define Debug_Config_LOG_LEVEL              Debug_LOG_LEVEL_DEBUG
#define Debug_Config_INCLUDE_LEVEL_IN_MSG
#define Debug_Config_LOG_WITH_FILE_LINE


//-----------------------------------------------------------------------------
// Memory
//-----------------------------------------------------------------------------

#define Memory_Config_USE_STDLIB_ALLOC


//-----------------------------------------------------------------------------
// ChanMUX/Proxy channels
//-----------------------------------------------------------------------------

enum
{
    CHANNEL_UNUSED_0,
    CHANNEL_UNUSED_1,
    CHANNEL_UNUSED_2,
    CHANNEL_UNUSED_3,
    CHANNEL_UNUSED_4,
    CHANNEL_UNUSED_5,
    CHANNEL_UNUSED_6,
    CHANNEL_UNUSED_7,
    CHANNEL_UNUSED_8,
    CHANNEL_UNUSED_9,
    CHANNEL_TEST_1,          // 10
    CHANNEL_TEST_2,          // 11
    CHANMUX_NUM_CHANNELS,    // 12
};


//-----------------------------------------------------------------------------
// ChanMUX cients
//-----------------------------------------------------------------------------
// Make each FIFO small enough that the Proxy can overflow it easily with a
// single packet
#define CHANMUX_TEST_FIFO_SIZE   (PAGE_SIZE / 2)
