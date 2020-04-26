
#include "system_config.h"
#include "LibDebug/Debug.h"
#include "SeosError.h"
#include "ChanMux_Tester_Common.h"
#include "camkes.h"
#include <stdio.h>


int run()
{
    seos_err_t retval = ChanMuxTest_init();

    if (retval != SEOS_SUCCESS)
    {
        Debug_LOG_ERROR("%s(): (tester 1) error initializing ChanMuxTest, code: %d",
                        __func__, retval);
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

    ChanMuxTest_testMaxSize(1);

    Debug_LOG_INFO("%s: Done", __func__);
    return 0;
}
