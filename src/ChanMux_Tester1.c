#include <stdio.h>
#include "LibDebug/Debug.h"
#include "camkes.h"
#include "ChanMux_config.h"
#include "ChanMux_Tester_Common.h"
#include "SeosError.h"

#include "camkes.h"

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

    ready_emit();
    Debug_LOG_DEBUG("%s: (tester 1) waiting signal to run...", __func__);
    tester2Ready_wait();
    Debug_LOG_DEBUG("%s: (tester 1) signal received!", __func__);

    ChanMuxTestExt_testFullDuplexTxStream(2);
    ChanMuxTest_testFullDuplex(1);

    Debug_LOG_INFO("%s: Done", __func__);
    return 0;
}
