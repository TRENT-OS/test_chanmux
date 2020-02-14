/*
 *  WAN/LAN Channel MUX
 *
 *  Copyright (C) 2019, Hensoldt Cyber GmbH
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// this is defined like this in order to allow the
// other side to overlow the channel more easily
// (with a single packet)
#define CHANMUX_TEST_FIFO_SIZE   (PAGE_SIZE / 2)

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

#ifdef __cplusplus
}
#endif
