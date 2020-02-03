/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "ChanMux_config.h"
#include "ChanMux/ChanMuxClient.h"
#include "camkes.h"
#include "LibDebug/Debug.h"

#define CMD_TEST_OVERFLOW   0

static char dataBuf[PAGE_SIZE];


/* Instance variables ---------------------------------------------------------*/

static ChanMuxClient testChanMuxClient;

int ChanMuxTest_init(unsigned int chan)
{

    bool isSuccess = ChanMuxClient_ctor(&testChanMuxClient,
                                        chan,
                                        chanMuxDataPort);
    if (!isSuccess)
    {
        Debug_LOG_ERROR("Failed to construct testChanMuxClient!\n");
        return -1;
    }
    return 0;
}

int ChanMuxTest_testOverflow()
{
    int retval = -1;
    char testCmd[] = { CMD_TEST_OVERFLOW };
    size_t len = sizeof(testCmd);

    Debug_LOG_DEBUG("%s: sending command to trigger overflow condition..",
                    __func__);
    seos_err_t err = ChanMuxClient_write(&testChanMuxClient,
                                         testCmd,
                                         len,
                                         &len);

    Debug_LOG_DEBUG("%s: ..command sent, retrieving data and overflow return code",
                    __func__);
    len = CHANMUX_FIFO_SIZE + 1; // we will try to read more then possible
    err = ChanMuxClient_read(&testChanMuxClient,
                             dataBuf,
                             len,
                             &len);
    if (SEOS_ERROR_OVERFLOW_DETECTED == err && CHANMUX_FIFO_SIZE == len)
    {
        Debug_LOG_INFO("%s: SUCCESS", __func__);
        retval = 0;
    }
    else
    {
        Debug_LOG_ERROR("%s: FAIL, err was %d with %zu bytes read",
                        __func__,
                        err,
                        len);
    }
    return retval;
}
