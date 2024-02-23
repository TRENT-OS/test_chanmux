/*
 * Channel MUX
 *
 * Copyright (C) 2019-2024, HENSOLDT Cyber GmbH
 * 
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * For commercial licensing, contact: info.cyber@hensoldt.net
 */

#include "system_config.h"
#include "ChanMux/ChanMux.h"
#include <camkes.h>


//------------------------------------------------------------------------------
static unsigned int
resolveChannel(
    unsigned int  sender_id,
    unsigned int  chanNum_local)
{
    switch (sender_id)
    {
    //----------------------------------
    case CHANMUX_ID_TESTER_1:
        // TODO: check that chanNum_local is 0, reject anything else
        return CHANMUX_CHANNEL_TEST_1;
    //----------------------------------
    case CHANMUX_ID_TESTER_2:
        // TODO: check that chanNum_local is 0, reject anything else
        return CHANMUX_CHANNEL_TEST_2;
    //----------------------------------
    default:
        break;
    }

    return INVALID_CHANNEL;
}


//------------------------------------------------------------------------------
static uint8_t testerFifo[2][CHANMUX_TEST_FIFO_SIZE];

static ChanMux_Channel_t test_channel[2];


//------------------------------------------------------------------------------
static const ChanMux_ChannelCtx_t channelCtx[] = {

    CHANMUX_CHANNEL_CTX(
        CHANMUX_CHANNEL_TEST_1,
        &test_channel[0],
        testerFifo[0], // must be the buffer and not a pointer
        CHANMUX_DATAPORT_ASSIGN(tester1_chan_portRead, tester1_chan_portWrite),
        tester1_chan_eventHasData_emit),

    CHANMUX_CHANNEL_CTX(
        CHANMUX_CHANNEL_TEST_2,
        &test_channel[1],
        testerFifo[1], // must be the buffer and not a pointer
        CHANMUX_DATAPORT_ASSIGN(tester2_chan_portRead, tester2_chan_portWrite),
        tester2_chan_eventHasData_emit),

};


//------------------------------------------------------------------------------
// this is used by the ChanMux component
const ChanMux_Config_t cfgChanMux =
{
    .resolveChannel = &resolveChannel,
    .numChannels    = ARRAY_SIZE(channelCtx),
    .channelCtx     = channelCtx,
};
