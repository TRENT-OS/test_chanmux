/*
 *  Channel Tester 2
 *
 *  Copyright (C) 2019-2020, Hensoldt Cyber GmbH
 */

#include "lib_debug/Debug.h"
#include "OS_Error.h"
#include "ChanMux_Tester_Common.h"
#include "camkes.h"
#include <stdio.h>


int run()
{
    OS_Error_t retval = ChanMuxTest_init();

    if (retval != OS_SUCCESS)
    {
        Debug_LOG_ERROR("%s(): (tester 2) error initialising ChanMuxTest, code: %d",
                        __func__, retval);
        return retval;
    }

    // We wait the other side to have finished with his tests not to confuse
    // the printout text
    Debug_LOG_DEBUG("%s: (tester 2) waiting signal to run...", __func__);
    tester1Ready_wait();
    Debug_LOG_DEBUG("%s: (tester 2) signal received!", __func__);

    ready_emit();
    // now we are ready as the other tester thread. Therefore now we can play
    // the full duplex test in Rx mode for our tester (2) and right after the Tx
    // mode part for the other tester (1)
    ChanMuxTest_testFullDuplex(2);
    ChanMuxTestExt_testFullDuplexTxStream(1);

    Debug_LOG_INFO("%s: Done", __func__);
    return 0;
}
