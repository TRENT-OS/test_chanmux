/*
 *  WAN/LAN Channel MUX
 *
 *  Copyright (C) 2019, Hensoldt Cyber GmbH
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define CHANMUX_FIFO_SIZE   (PAGE_SIZE / 2)
enum
{
    CHANNEL_LAN_DATA,        // 0
    CHANNEL_WAN_DATA,        // 1
    CHANNEL_LAN_CTRL,        // 2
    CHANNEL_WAN_CTRL,        // 3
    CHANNEL_NW_STACK_CTRL,   // 4
    CHANNEL_NW_STACK_DATA,   // 5
    CHANNEL_NVM_USER1_DATA,  // 6
    CHANNEL_NW_STACK_CTRL2,  // 7
    CHANNEL_NW_STACK_DATA2,  // 8
    CHANNEL_NVM_USER2_DATA,  // 9
    CHANNEL_TEST_1,          // 10
    CHANNEL_TEST_2,          // 11
    CHANMUX_NUM_CHANNELS,    // 12
};

#ifdef __cplusplus
}
#endif
