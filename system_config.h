/**
 *
 * ChanMux Test Configuration
 *
 * Copyright (C) 2019, Hensoldt Cyber GmbH
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
// ChanMux channels
//-----------------------------------------------------------------------------

#define CHANMUX_CHANNEL_TEST_1          10
#define CHANMUX_CHANNEL_TEST_2          11


//-----------------------------------------------------------------------------
// ChanMux clients
//-----------------------------------------------------------------------------

#define CHANMUX_ID_TESTER_1        101
#define CHANMUX_ID_TESTER_2        102


//-----------------------------------------------------------------------------
// ChanMux Test FIFO
//-----------------------------------------------------------------------------

// Make each FIFO small enough that the Proxy can overflow it easily with a
// single packet
#define CHANMUX_TEST_FIFO_SIZE   (PAGE_SIZE / 2)
