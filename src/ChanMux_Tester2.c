#include <stdio.h>
#include "LibDebug/Debug.h"
#include "camkes.h"
#include "ChanMux_config.h"
#include "ChanMux_Tester_Common.h"
#include "SeosError.h"

#include "camkes.h"

int run()
{
    seos_err_t retval = ChanMuxTest_init(CHANNEL_TEST_2);

    if (retval != SEOS_SUCCESS)
    {
        Debug_LOG_ERROR("%s(): channel %u: Error initializing ChanMuxTest! Error code:%d",
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

    Debug_LOG_INFO("%s: Done", __func__);
    return 0;
}
