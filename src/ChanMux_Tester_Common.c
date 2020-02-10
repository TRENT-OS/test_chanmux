/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "ChanMux_config.h"
#include "ChanMux/ChanMuxClient.h"
#include "camkes.h"
#include "LibDebug/Debug.h"

#define CMD_TEST_OVERFLOW       0
#define CMD_TEST_FULL_DUPLEX    1

static char dataBuf[PAGE_SIZE];
bool fullDuplexTestTxRunning = false;
bool fullDuplexTestRxRunning = false;

#define ITERATIONS                  20
#define FULL_DUPLEX_BLOCK_SIZE      (CHANMUX_FIFO_SIZE / 2)

/* Instance variables ---------------------------------------------------------*/

static ChanMuxClient testChanMuxClient;

static void
cpyIntToBuf(uint32_t integer, char* buf)
{
    buf[0] = (integer >> 24) & 0xFF;
    buf[1] = (integer >> 16) & 0xFF;
    buf[2] = (integer >> 8) & 0xFF;
    buf[3] = integer & 0xFF;
}

seos_err_t
ChanMuxTest_init(unsigned int chan)
{

    bool isSuccess = ChanMuxClient_ctor(&testChanMuxClient,
                                        chan,
                                        chanMuxDataPort);
    if (!isSuccess)
    {
        Debug_LOG_ERROR("Failed to construct testChanMuxClient!");
        return -1;
    }
    return 0;
}

seos_err_t
ChanMuxTest_testReturnCodes(int tester)
{
    seos_err_t retval = SEOS_ERROR_GENERIC;
    size_t len = sizeof(dataBuf);
    // TEST ChanMuxClient_write()
    if (ChanMuxClient_write(&testChanMuxClient, NULL, len, &len)
            != SEOS_ERROR_INVALID_PARAMETER)
    {
        Debug_LOG_ERROR("%s: FAIL (tester %d), failed @%s:%d",
                        __func__, tester, __FILE__, __LINE__);
    }
    else if (ChanMuxClient_write(&testChanMuxClient, dataBuf, len, NULL)
            != SEOS_ERROR_INVALID_PARAMETER)
    {
        Debug_LOG_ERROR("%s: FAIL (tester %d), failed @%s:%d",
                        __func__, tester, __FILE__, __LINE__);
    }
    else if (ChanMuxClient_write(&testChanMuxClient,
                                 dataBuf,
                                 PAGE_SIZE + 1,
                                 &len)
            != SEOS_ERROR_INVALID_PARAMETER)
    {
        Debug_LOG_ERROR("%s: FAIL (tester %d), failed @%s:%d",
                        __func__, tester, __FILE__, __LINE__);
    }
    else if (ChanMuxClient_readAsync(&testChanMuxClient, NULL, len, &len)
            != SEOS_ERROR_INVALID_PARAMETER)
    {
        Debug_LOG_ERROR("%s: FAIL (tester %d), failed @%s:%d",
                        __func__, tester, __FILE__, __LINE__);
    }
    else if (ChanMuxClient_readAsync(&testChanMuxClient, dataBuf, len, NULL)
            != SEOS_ERROR_INVALID_PARAMETER)
    {
        Debug_LOG_ERROR("%s: FAIL (tester %d), failed @%s:%d",
                        __func__, tester, __FILE__, __LINE__);
    }
    else if (ChanMuxClient_readAsync(&testChanMuxClient,
                                     dataBuf, PAGE_SIZE + 1,
                                     &len)
            != SEOS_ERROR_INVALID_PARAMETER)
    {
        Debug_LOG_ERROR("%s: FAIL (tester %d), failed @%s:%d",
                        __func__, tester, __FILE__, __LINE__);
    }
    else if (ChanMuxClient_read(&testChanMuxClient, NULL, len, &len)
            != SEOS_ERROR_INVALID_PARAMETER)
    {
        Debug_LOG_ERROR("%s: FAIL (tester %d), failed @%s:%d",
                        __func__, tester, __FILE__, __LINE__);
    }
    else if (ChanMuxClient_read(&testChanMuxClient, dataBuf, len, NULL)
            != SEOS_ERROR_INVALID_PARAMETER)
    {
        Debug_LOG_ERROR("%s: FAIL (tester %d), failed @%s:%d",
                        __func__, tester, __FILE__, __LINE__);
    }
    else if (ChanMuxClient_read(&testChanMuxClient, dataBuf, PAGE_SIZE + 1, &len)
            != SEOS_ERROR_INVALID_PARAMETER)
    {
        Debug_LOG_ERROR("%s: FAIL (tester %d), failed @%s:%d",
                        __func__, tester, __FILE__, __LINE__);
    }
    // test buffer overlap
    else if (ChanMuxClient_read(&testChanMuxClient,
                                testChanMuxClient.dataport,
                                PAGE_SIZE + 1,
                                &len)
            != SEOS_ERROR_INVALID_PARAMETER)
    {
        Debug_LOG_ERROR("%s: FAIL (tester %d), failed @%s:%d",
                        __func__, tester, __FILE__, __LINE__);
    }
    else
    {
        Debug_LOG_INFO("%s: SUCCESS (tester %d)", __func__, tester);
        retval = SEOS_SUCCESS;
    }
    return retval;
}

seos_err_t
ChanMuxTest_testOverflow(int tester)
{
    seos_err_t retval = SEOS_ERROR_GENERIC;
    char testCmd[] = { CMD_TEST_OVERFLOW };
    size_t len = sizeof(testCmd);

    Debug_LOG_DEBUG("%s: (tester %d) sending command to trigger overflow condition..",
                    __func__, tester);
    seos_err_t err = ChanMuxClient_write(&testChanMuxClient,
                                         testCmd,
                                         len,
                                         &len);

    Debug_LOG_DEBUG("%s: (tester %d) ..command sent, retrieving data and overflow return code",
                    __func__, tester);
    len = CHANMUX_FIFO_SIZE + 1; // we will try to read more then possible
    err = ChanMuxClient_read(&testChanMuxClient,
                             dataBuf,
                             len,
                             &len);
    if (SEOS_ERROR_OVERFLOW_DETECTED == err && CHANMUX_FIFO_SIZE == len)
    {
        Debug_LOG_INFO("%s: SUCCESS (tester %d)", __func__, tester);
        retval = SEOS_SUCCESS;
    }
    else
    {
        Debug_LOG_ERROR("%s: FAIL (tester %d), err was %d with %zu bytes read",
                        __func__,
                        tester,
                        err,
                        len);
    }
    return retval;
}

// This routine will be shared to the other test thread via an inteface so that
// we will have a thread (the one running the interface) making Tx streaming
// while the other is doing Rx streaming
seos_err_t
ChanMuxTest_testFullDuplexTxStream(int tester)
{
    Debug_LOG_DEBUG("%s: (tester %d) waiting signal to run...", __func__, tester);
    fullDuplexTestTxRunning = true;
    // stay in the busy loop until the other thread begins with running the
    // tx stream
    while (!fullDuplexTestRxRunning) seL4_Yield();
    Debug_LOG_DEBUG("%s: (tester %d) signal received", __func__, tester);

    seos_err_t retval       = SEOS_ERROR_GENERIC;
    char testCmd[5]         = { CMD_TEST_FULL_DUPLEX };
    size_t len              = 0;
    seos_err_t err          = SEOS_ERROR_GENERIC;

    // this variable must be local to the funtion as this function is exposed as
    // camkes interface function;
    static char dataBuf[PAGE_SIZE];

    // compose the command with the payload inside
    cpyIntToBuf(FULL_DUPLEX_BLOCK_SIZE, &testCmd[1]);
    memcpy(dataBuf, testCmd, sizeof(testCmd));
    for (int j = 0; j < FULL_DUPLEX_BLOCK_SIZE; j++)
    {
        dataBuf[j + sizeof(testCmd)] = j % 256;
    }

    for (int i = 0; i < ITERATIONS; i++)
    {
        Debug_LOG_DEBUG("%s: (tester %d) sending command to trigger full duplex streaming..",
                        __func__, tester);
        len = sizeof(testCmd) + FULL_DUPLEX_BLOCK_SIZE;
        err = ChanMuxClient_write(&testChanMuxClient, dataBuf, len, &len);
        if (SEOS_SUCCESS != err)
        {
            Debug_LOG_ERROR("%s: (tester %d) got error %d when trying to send a request to proxy",
                            __func__, tester, err);
            goto exit;
        }
        retval = 0;
    }
exit:
    if (SEOS_SUCCESS == retval)
    {
        Debug_LOG_INFO("%s: SUCCESS (tester %d)", __func__, tester);
    }
    return retval;
}

seos_err_t
ChanMuxTest_testFullDuplex(int tester)
{
    Debug_LOG_DEBUG("%s: (tester %d) waiting signal to run...", __func__, tester);
    fullDuplexTestRxRunning = true;
    // stay in the busy loop until the other thread begins with running the
    // tx stream
    while (!fullDuplexTestTxRunning) seL4_Yield();
    Debug_LOG_DEBUG("%s: (tester %d) signal received", __func__, tester);

    seos_err_t retval   = SEOS_ERROR_GENERIC;
    seos_err_t err      = SEOS_ERROR_GENERIC;
    size_t len          = 0;
    size_t amount       = 0;

    while (amount < ITERATIONS * FULL_DUPLEX_BLOCK_SIZE)
    {
        Debug_LOG_DEBUG("%s: (tester %d) attempt to read %zu bytes from ChanMux..",
                        __func__, tester, len);
        len = FULL_DUPLEX_BLOCK_SIZE;
        err = ChanMuxClient_read(&testChanMuxClient,
                                 dataBuf,
                                 len,
                                 &len);
        if (SEOS_SUCCESS == err)
        {
            Debug_LOG_DEBUG("%s: (tester %d) .. got %zu bytes from ChanMux",
                            __func__, tester, len);
            if (len > 0)
            {
                int i = 0;
                int expected = 0;

                while (i < len)
                {
                    expected = (i + amount) % 256;
                    if (dataBuf[i] == expected)
                    {
                        i++;
                    }
                    else
                    {
                        break;
                    }
                }
                if (i == len)
                {
                    retval = SEOS_SUCCESS;
                }
                else
                {
                    Debug_LOG_ERROR("%s: FAIL (tester %d), data received mismatches the expected pattern @ byte #%d, expected 0x%02x but received 0x%02x",
                                    __func__,
                                    tester,
                                    amount + i,
                                    expected,
                                    dataBuf[i]);
                    goto exit;
                }
                amount += len;
            }
        }
        else
        {
            Debug_LOG_ERROR("%s: FAIL (tester %d), err was %d with %zu bytes read",
                            __func__,
                            tester,
                            err,
                            len);
            goto exit;
        }
    }
 exit:
    if (SEOS_SUCCESS == retval)
    {
        Debug_LOG_INFO("%s: SUCCESS (tester %d)", __func__, tester);
    }
    return retval;
}
