/*
 * Channel Tester 1
 *
 * Copyright (C) 2019-2024, HENSOLDT Cyber GmbH
 * 
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * For commercial licensing, contact: info.cyber@hensoldt.net
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
        Debug_LOG_ERROR("%s(): (tester 1) error initializing ChanMuxTest, code: %d",
                        __func__, retval);
        return retval;
    }

    ChanMuxTest_testReturnCodes();
    ChanMuxTest_testOverflow();
    ChanMuxTest_testMaxSize();

    ready_emit();
    Debug_LOG_DEBUG("%s: (tester 1) waiting signal to run...", __func__);
    tester2Ready_wait();
    Debug_LOG_DEBUG("%s: (tester 1) signal received!", __func__);

    ChanMuxTestExt_testFullDuplexTxStream(2);
    ChanMuxTest_testFullDuplex(1);

    Debug_LOG_INFO("%s: Done", __func__);
    return 0;
}
