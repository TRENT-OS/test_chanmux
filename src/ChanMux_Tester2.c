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
                        __func__, CHANNEL_TEST_2, retval);
        return retval;
    }

    ChanMuxTest_testReturnCodes(2);
    ChanMuxTest_testOverflow(2);

    if (fullDuplexTestTxRunning)
    {
        // In this case our testFullDuplexTxStream() is running from the
        // interface thread already thefore we get to run the counter part
        // (testFullDuplex()) soon in order to consume the echoed bytes
        Debug_ASSERT(!fullDuplexTestRxRunning);
        ChanMuxTest_testFullDuplex(2);
        ChanMuxTestExt_testFullDuplexTxStream(1);
    }
    else
    {
        Debug_ASSERT(!fullDuplexTestRxRunning);
        ChanMuxTestExt_testFullDuplexTxStream(1);
        ChanMuxTest_testFullDuplex(2);
    }

    Debug_LOG_INFO("%s: Done", __func__);
    return 0;
}
