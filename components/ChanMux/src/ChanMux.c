/*
 *  Channel MUX
 *
 *  Copyright (C) 2019, Hensoldt Cyber GmbH
 */

#include "ChanMux/ChanMux.h"
#include "system_config.h"
#include "SeosError.h"
#include "assert.h"
#include <camkes.h>

static uint8_t chanmux_Tester1FifoBuf[CHANMUX_TEST_FIFO_SIZE];
static uint8_t chanmux_Tester2FifoBuf[CHANMUX_TEST_FIFO_SIZE];

static const ChanMuxConfig_t cfgChanMux =
{
    .numChannels = CHANMUX_NUM_CHANNELS,
    .outputDataport = {
        .io  = (void**) &outputDataPort,
        .len = PAGE_SIZE
    },
    .channelsFifos = {
        {
            // Channel 0
            .buffer = NULL,
            .len = 0
        },
        {
            // Channel 1
            .buffer = NULL,
            .len = 0
        },
        {
            // Channel 2
            .buffer = NULL,
            .len = 0
        },
        {
            // Channel 3
            .buffer = NULL,
            .len = 0
        },
        {
            // Channel 4
            .buffer = NULL,
            .len = 0
        },
        {
            // Channel 5
            .buffer = NULL,
            .len = 0
        },
        {
            // Channel 6
            .buffer = NULL,
            .len = 0
        },
        {
            // Channel 7
            .buffer = NULL,
            .len = 0
        },
        {
            // Channel 8
            .buffer = NULL,
            .len = 0
        },
        {
            // Channel 9
            .buffer = NULL,
            .len = 0
        },
        {
            // Channel 10
            .buffer = chanmux_Tester1FifoBuf,
            .len = sizeof(chanmux_Tester1FifoBuf)
        },
        {
            // Channel 11
            .buffer = chanmux_Tester2FifoBuf,
            .len = sizeof(chanmux_Tester2FifoBuf)
        }
    }
};

static const ChannelDataport_t wDataports[CHANMUX_NUM_CHANNELS] =
{
    { 0 }, // 0
    { 0 }, // 1
    { 0 }, // 2
    { 0 }, // 3
    { 0 }, // 4
    { 0 }, // 5
    { 0 }, // 6
    { 0 }, // 7
    { 0 }, // 8
    { 0 }, // 9
    { // 10
        .io  = (void**) &tester1_dataPort_write,
        .len = PAGE_SIZE
    },
    { // 11
        .io  = (void**) &tester2_dataPort_write,
        .len = PAGE_SIZE
    }
};

static const ChannelDataport_t rDataports[CHANMUX_NUM_CHANNELS] =
{
    { 0 }, // 0
    { 0 }, // 1
    { 0 }, // 2
    { 0 }, // 3
    { 0 }, // 4
    { 0 }, // 5
    { 0 }, // 6
    { 0 }, // 7
    { 0 }, // 8
    { 0 }, // 9
    { // 10
        .io  = (void**) &tester1_dataPort_read,
        .len = PAGE_SIZE
    },
    { // 11
        .io  = (void**) &tester2_dataPort_read,
        .len = PAGE_SIZE
    }
};

//------------------------------------------------------------------------------
const ChanMuxConfig_t*
ChanMux_config_getConfig(void)
{
    return &cfgChanMux;
}

//------------------------------------------------------------------------------
void
ChanMux_dataAvailable_emit(unsigned int chanNum)
{
    Debug_LOG_TRACE("%s: chan %u",
                    __func__, chanNum);
    switch (chanNum)
    {
    case CHANNEL_TEST_1:
    case CHANNEL_TEST_2:
        tester_dataAvailable_emit();
        break;
    default:
        Debug_LOG_ERROR("%s(): invalid channel %u", __func__, chanNum);
        break;
    }
}

//------------------------------------------------------------------------------
static ChanMux*
ChanMux_getInstance(void)
{
    // singleton
    static ChanMux  theOne;
    static ChanMux* self = NULL;
    static Channel_t channels[CHANMUX_NUM_CHANNELS];

    if ((NULL == self) && ChanMux_ctor(&theOne,
                                       channels,
                                       ChanMux_config_getConfig(),
                                       ChanMux_dataAvailable_emit,
                                       Output_write))
    {
        self = &theOne;
    }

    return self;
}

void
ChanMuxOut_takeByte(char byte)
{
    ChanMux_takeByte(ChanMux_getInstance(), byte);
}

//==============================================================================
// CAmkES Interface
//==============================================================================
extern unsigned int ChanMuxIn_get_sender_id();
//------------------------------------------------------------------------------
seos_err_t
ChanMuxRpc_write(
    unsigned int  chanNum, // legacy, to be removed
    size_t        len,
    size_t*       lenWritten)
{
    Debug_LOG_TRACE("%s(): channel %u, len %u", __func__, chanNum, len);

    // set defaults to error
    *lenWritten = 0;

    const ChannelDataport_t* dp = NULL;
    switch (chanNum)
    {
    //---------------------------------
    case CHANNEL_TEST_1:
    case CHANNEL_TEST_2:
        dp = &wDataports[chanNum];
        break;
    //---------------------------------
    default:
        Debug_LOG_ERROR("%s(): invalid channel %u", __func__, chanNum);
        return SEOS_ERROR_ACCESS_DENIED;
    }

    Debug_ASSERT( NULL != dp );

    seos_err_t ret = ChanMux_write(ChanMux_getInstance(), chanNum, dp, &len);

    *lenWritten = len;

    Debug_LOG_TRACE("%s(): channel %u, lenWritten %u", __func__, chanNum, len);

    return ret;
}


//------------------------------------------------------------------------------
seos_err_t
ChanMuxRpc_read(
    unsigned int  chanNum, // legacy, to be removed
    size_t        len,
    size_t*       lenRead)
{
    Debug_LOG_TRACE("%s(): channel %u, len %u", __func__, chanNum, len);

    // set defaults to error
    *lenRead = 0;

    const ChannelDataport_t* dp = NULL;
    switch (chanNum)
    {
    //---------------------------------
    case CHANNEL_TEST_1:
    case CHANNEL_TEST_2:
        dp = &rDataports[chanNum];
        break;
    //---------------------------------
    default:
        Debug_LOG_ERROR("%s(): invalid channel %u", __func__, chanNum);
        return SEOS_ERROR_ACCESS_DENIED;
    }

    Debug_ASSERT( NULL != dp );
    seos_err_t ret = ChanMux_read(ChanMux_getInstance(), chanNum, dp, &len);
    *lenRead = len;

    Debug_LOG_TRACE("%s(): channel %u, lenRead %u", __func__, chanNum, len);

    return ret;
}
