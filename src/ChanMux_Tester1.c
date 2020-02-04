#include <stdio.h>
#include "LibDebug/Debug.h"
#include "camkes.h"
#include "ChanMux_config.h"
#include "ChanMux_Tester_Common.h"
#include "camkes.h"

static char proxyBuffer[PAGE_SIZE];

extern bool fullDuplexTestTxRunning;
extern bool fullDuplexTestRxRunning;

int run()
{
    int ret_value = ChanMuxTest_init(CHANNEL_TEST_1, proxyBuffer);

    if (ret_value < 0)
    {
        Debug_LOG_ERROR("%s(): channel %u: Error initializing ChanMuxTest! Errno:%d",
                        __func__, CHANNEL_TEST_1, ret_value);
        return -1;
    }

    ChanMuxTest_testOverflow(1);

    if (fullDuplexTestTxRunning)
    {
        Debug_ASSERT(!fullDuplexTestRxRunning);
        ChanMuxTest_testFullDuplex(1);
        ChanMuxTest2_testFullDuplexTxStream(2);
    }
    else
    {
        Debug_ASSERT(!fullDuplexTestRxRunning);
        ChanMuxTest2_testFullDuplexTxStream(2);
        ChanMuxTest_testFullDuplex(1);
    }

    Debug_LOG_INFO("%s: Done", __func__);
    return 0;
}
