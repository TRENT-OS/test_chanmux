/*
 *  Channel Tester 2
 *
 *  Copyright (C) 2019-2020, Hensoldt Cyber GmbH
 */

#include "LibDebug/Debug.h"
#include "OS_Error.h"
#include "ChanMux_Tester_Common.h"
#include "camkes.h"
#include <stdio.h>


int run()
{
    OS_Error_t retval = ChanMuxTest_init();

    if (retval != OS_SUCCESS)
    {
        Debug_LOG_ERROR("%s(): (tester 2) error initializing ChanMuxTest, code: %d",
                        __func__, retval);
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
