#include <stdio.h>
#include "LibDebug/Debug.h"
#include "camkes.h"
#include "ChanMux_config.h"
#include "ChanMux_Tester_Common.h"

static char proxyBuffer[PAGE_SIZE];

int run()
{
    int ret_value = ChanMuxTest_init(CHANNEL_TEST_1, proxyBuffer);

    if (ret_value < 0)
    {
        Debug_LOG_ERROR("%s(): channel %u: Error initializing ChanMuxTest! Errno:%d",
                        __func__, CHANNEL_TEST_1, ret_value);
        return -1;
    }
    ChanMuxTest_testOverflow();

    Debug_LOG_INFO("%s: Done", __func__);
    return 0;
}
