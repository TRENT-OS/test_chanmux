/*
 *  Channel MUX
 *
 *  Copyright (C) 2019, Hensoldt Cyber GmbH
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

static ChanMux_channel_t test_channel[2];


//------------------------------------------------------------------------------
static const ChanMux_channel_ctx_t channelCtx[] = {

    CHANMUX_CHANNEL_CTX(
        CHANMUX_CHANNEL_TEST_1,
        &test_channel[0],
        testerFifo[0], // must be the buffer and not a pointer
        CHANMUX_DATAPORT_DUPLEX_ASSIGN(tester1_port_read, tester1_port_write),
        tester1_event_hasData_emit),

    CHANMUX_CHANNEL_CTX(
        CHANMUX_CHANNEL_TEST_2,
        &test_channel[1],
        testerFifo[1], // must be the buffer and not a pointer
        CHANMUX_DATAPORT_DUPLEX_ASSIGN(tester2_port_read, tester2_port_write),
        tester2_event_hasData_emit),

};


//------------------------------------------------------------------------------
// this is used by the ChanMux component
const ChanMux_config_t cfgChanMux =
{
    .resolveChannel = &resolveChannel,
    .numChannels    = ARRAY_SIZE(channelCtx),
    .channelCtx     = channelCtx,
};
