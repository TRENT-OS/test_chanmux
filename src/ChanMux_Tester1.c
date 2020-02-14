#include <stdio.h>
#include "LibDebug/Debug.h"
#include "camkes.h"
#include "ChanMux_config.h"
#include "ChanMux_Tester_Common.h"
#include "SeosError.h"

#include "camkes.h"

extern bool fullDuplexTestTxRunning;
extern bool fullDuplexTestRxRunning;

int run()
{
    seos_err_t retval = ChanMuxTest_init();

    if (retval != SEOS_SUCCESS)
    {
        Debug_LOG_ERROR("%s(): channel %u: Error initializing ChanMuxTest! Error code:%d",
                        __func__, CHANNEL_TEST_1, retval);
        return retval;
    }

    ChanMuxTest_testReturnCodes(1);
    ChanMuxTest_testOverflow(1);

    if (fullDuplexTestTxRunning)
    {
        // In this case our testFullDuplexTxStream() is running from the
        // interface thread already thefore we get to run the counter part
        // (testFullDuplex()) soon in order to consume the echoed bytes
        Debug_ASSERT(!fullDuplexTestRxRunning);
        ChanMuxTest_testFullDuplex(1);
        ChanMuxTestExt_testFullDuplexTxStream(2);
    }
    else
    {
        Debug_ASSERT(!fullDuplexTestRxRunning);
        ChanMuxTestExt_testFullDuplexTxStream(2);
        ChanMuxTest_testFullDuplex(1);
    }

    Debug_LOG_INFO("%s: Done", __func__);
    return 0;
}
