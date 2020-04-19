
#include "system_config.h"
#include "LibDebug/Debug.h"
#include "SeosError.h"
#include "ChanMux_Tester_Common.h"
#include "camkes.h"
#include <stdio.h>


int run()
{
    seos_err_t retval = ChanMuxTest_init(CHANNEL_TEST_2);

    if (retval != SEOS_SUCCESS)
    {
        Debug_LOG_ERROR("%s(): channel %d: Error initializing ChanMuxTest! Error code:%d",
                        __func__, CHANNEL_TEST_2, retval);
        return retval;
    }

    ChanMuxTest_testReturnCodes(2);
    ChanMuxTest_testOverflow(2);

    ready_emit();
    Debug_LOG_DEBUG("%s: (tester 2) waiting signal to run...", __func__);
    tester1Ready_wait();
    Debug_LOG_DEBUG("%s: (tester 2) signal received!", __func__);

    ChanMuxTest_testFullDuplex(2);
    ChanMuxTestExt_testFullDuplexTxStream(1);

    ChanMuxTest_testMaxSize(2);

    Debug_LOG_INFO("%s: Done", __func__);
    return 0;
}
