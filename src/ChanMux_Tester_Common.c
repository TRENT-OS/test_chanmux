/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "ChanMux_config.h"
#include "ChanMux/ChanMuxClient.h"
#include "camkes.h"
#include "LibDebug/Debug.h"

#define CMD_TEST_OVERFLOW       0
#define CMD_TEST_FULL_DUPLEX    1

#define ITERATIONS                  20
#define FULL_DUPLEX_BLOCK_SIZE      (CHANMUX_TEST_FIFO_SIZE / 2)

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
    // channel=0 parameter is nowadays obsolete and to be removed
    bool isSuccess = ChanMuxClient_ctor(&testChanMuxClient,
                                        chan,
                                        chanMuxRDataPort,
                                        chanMuxWDataPort);
    if (!isSuccess)
    {
        Debug_LOG_ERROR("Failed to construct testChanMuxClient!");
        return -1;
    }
    return 0;
}

seos_err_t
ChanMuxTest_testReturnCodes(unsigned int tester)
{
    static char dataBuf[PAGE_SIZE];
    seos_err_t retval = SEOS_ERROR_GENERIC;
    size_t len = sizeof(dataBuf);

    /* the following code structure may look strange because od the repetition
     of Debug_LOG_* calls. The reason why we want this is because the macros,
     based on where they are places will print out __FILE__ and __LINE__ too */
    // TEST ChanMuxClient_write()
    if (ChanMuxClient_write(&testChanMuxClient, NULL, len, &len)
            != SEOS_ERROR_INVALID_PARAMETER)
    {
        Debug_LOG_ERROR("%s: FAIL (tester %d)",
                        __func__, tester);
    }
    else if (ChanMuxClient_write(&testChanMuxClient, dataBuf, len, NULL)
            != SEOS_ERROR_INVALID_PARAMETER)
    {
        Debug_LOG_ERROR("%s: FAIL (tester %d)",
                        __func__, tester);
    }
    else if (ChanMuxClient_write(&testChanMuxClient,
                                 dataBuf,
                                 PAGE_SIZE + 1,
                                 &len)
            != SEOS_ERROR_INVALID_PARAMETER)
    {
        Debug_LOG_ERROR("%s: FAIL (tester %d)",
                        __func__, tester);
    }
    else if (ChanMuxClient_readAsync(&testChanMuxClient, NULL, len, &len)
            != SEOS_ERROR_INVALID_PARAMETER)
    {
        Debug_LOG_ERROR("%s: FAIL (tester %d)",
                        __func__, tester);
    }
    else if (ChanMuxClient_readAsync(&testChanMuxClient, dataBuf, len, NULL)
            != SEOS_ERROR_INVALID_PARAMETER)
    {
        Debug_LOG_ERROR("%s: FAIL (tester %d)",
                        __func__, tester);
    }
    else if (ChanMuxClient_readAsync(&testChanMuxClient,
                                     dataBuf, PAGE_SIZE + 1,
                                     &len)
            != SEOS_ERROR_INVALID_PARAMETER)
    {
        Debug_LOG_ERROR("%s: FAIL (tester %d)",
                        __func__, tester);
    }
    else if (ChanMuxClient_read(&testChanMuxClient, NULL, len, &len)
            != SEOS_ERROR_INVALID_PARAMETER)
    {
        Debug_LOG_ERROR("%s: FAIL (tester %d)",
                        __func__, tester);
    }
    else if (ChanMuxClient_read(&testChanMuxClient, dataBuf, len, NULL)
            != SEOS_ERROR_INVALID_PARAMETER)
    {
        Debug_LOG_ERROR("%s: FAIL (tester %d)",
                        __func__, tester);
    }
    else if (ChanMuxClient_read(&testChanMuxClient, dataBuf, PAGE_SIZE + 1, &len)
            != SEOS_ERROR_INVALID_PARAMETER)
    {
        Debug_LOG_ERROR("%s: FAIL (tester %d)",
                        __func__, tester);
    }
    // test buffer overlap
    else if (ChanMuxClient_read(&testChanMuxClient,
                                testChanMuxClient.readDataport,
                                PAGE_SIZE + 1,
                                &len)
            != SEOS_ERROR_INVALID_PARAMETER)
    {
        Debug_LOG_ERROR("%s: FAIL (tester %d)",
                        __func__, tester);
    }
    else
    {
        Debug_LOG_INFO("%s: SUCCESS (tester %d)", __func__, tester);
        retval = SEOS_SUCCESS;
    }
    return retval;
}

seos_err_t
ChanMuxTest_testOverflow(unsigned int tester)
{
    static char dataBuf[PAGE_SIZE];
    seos_err_t retval = SEOS_ERROR_GENERIC;
    char testCmd[] = { CMD_TEST_OVERFLOW };
    size_t len = sizeof(testCmd);

    Debug_LOG_DEBUG("%s: (tester %d) sending command to trigger overflow condition...",
                    __func__, tester);
    seos_err_t err = ChanMuxClient_write(&testChanMuxClient,
                                         testCmd,
                                         len,
                                         &len);
    if (SEOS_SUCCESS != err)
    {
        Debug_LOG_ERROR("%s: (tester %d) failed trying to send a command, err was %d with %zu bytes written",
                        __func__,
                        tester,
                        err,
                        len);
        goto exit;
    }
    Debug_LOG_DEBUG("%s: (tester %d) command sent, retrieving data and overflow return code",
                    __func__, tester);
    len = CHANMUX_TEST_FIFO_SIZE + 1; // we will try to read more then possible
    err = ChanMuxClient_read(&testChanMuxClient,
                             dataBuf,
                             len,
                             &len);
    if ((SEOS_ERROR_OVERFLOW_DETECTED == err) && (CHANMUX_TEST_FIFO_SIZE == len))
    {
        len = 0;
        err = ChanMuxClient_readAsync(&testChanMuxClient,
                                      dataBuf,
                                      len,
                                      &len);
        if ((SEOS_SUCCESS == err))
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
    }
    else
    {
        Debug_LOG_ERROR("%s: FAIL (tester %d), err was %d with %zu bytes read",
                        __func__,
                        tester,
                        err,
                        len);
    }
exit:
    return retval;
}

// This routine will be shared to the other test thread via an inteface so that
// we will have a thread (the one running the interface) making Tx streaming
// while the other is doing Rx streaming
seos_err_t
ChanMuxTest_testFullDuplexTxStream(unsigned int tester)
{
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
    for (unsigned int j = 0; j < FULL_DUPLEX_BLOCK_SIZE; j++)
    {
        dataBuf[j + sizeof(testCmd)] = j % 256;
    }

    for (unsigned int i = 0; i < ITERATIONS; i++)
    {
        Debug_LOG_DEBUG("%s: (tester %d) sending command to trigger full duplex streaming...",
                        __func__, tester);
        len = sizeof(testCmd) + FULL_DUPLEX_BLOCK_SIZE;
        err = ChanMuxClient_write(&testChanMuxClient, dataBuf, len, &len);
        if (SEOS_SUCCESS != err)
        {
            Debug_LOG_ERROR("%s: (tester %d) got error %d when trying to send a request to proxy",
                            __func__, tester, err);
            goto exit;
        }
        retval = SEOS_SUCCESS;
    }
exit:
    if (SEOS_SUCCESS == retval)
    {
        Debug_LOG_INFO("%s: SUCCESS (tester %d)", __func__, tester);
    }
    return retval;
}

seos_err_t
ChanMuxTest_testFullDuplex(unsigned int tester)
{
    seos_err_t retval   = SEOS_ERROR_GENERIC;
    seos_err_t err      = SEOS_ERROR_GENERIC;
    size_t len          = 0;
    size_t amount       = 0;

    static char dataBuf[PAGE_SIZE];

    while (amount < (ITERATIONS * FULL_DUPLEX_BLOCK_SIZE))
    {
        len = FULL_DUPLEX_BLOCK_SIZE;
        Debug_LOG_DEBUG("%s: (tester %d) attempting to read %zu bytes from ChanMux...",
                        __func__, tester, len);
        err = ChanMuxClient_read(&testChanMuxClient,
                                 dataBuf,
                                 len,
                                 &len);
        if (SEOS_SUCCESS == err)
        {
            Debug_LOG_DEBUG("%s: (tester %d) got %zu bytes from ChanMux",
                            __func__, tester, len);
            if (len > 0)
            {
                size_t i        = 0;
                size_t expected = 0;

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
