/* Includes ------------------------------------------------------------------*/
#include "LibDebug/Debug.h"
#include "ChanMuxNvmDriver.h"
#include "LibUtil/BitConverter.h"
#include "camkes.h"
#include <string.h>
#include <stdio.h>

#define CMD_TEST_OVERFLOW       0
#define CMD_TEST_FULL_DUPLEX    1
#define CMD_TEST_MAX_SIZE       2

#define ITERATIONS                  20
#define FULL_DUPLEX_BLOCK_SIZE      (CHANMUX_TEST_FIFO_SIZE / 2)

/* Instance variables ---------------------------------------------------------*/

static const ChanMuxClientConfig_t chanMuxClientConfig = {
    .port   = CHANMUX_DATAPORT_DUPLEX_ASSIGN(
                    chanMux_port_read,
                    chanMux_port_write ),
    .wait   = chanMux_event_hasData_wait,
    .write  = chanMux_rpc_write,
    .read   = chanMux_rpc_read
};

static ChanMuxClient testChanMuxClient;

static OS_Error_t
testMaxSize(unsigned int tester, size_t len)
{
    OS_Error_t retval   = OS_ERROR_GENERIC;
    OS_Error_t err      = OS_ERROR_GENERIC;

    // PAGE_SIZE is the size of the dataport to the ChanMux
    static char dataBuf[PAGE_SIZE];
    char testCmd[5]             = { CMD_TEST_MAX_SIZE };
    const size_t maxPatterLen   = ChanMuxClient_MTU - sizeof(testCmd);
    size_t patternLen           = len - sizeof(testCmd);

    // compose the command with the payload inside
    BitConverter_putUint32BE((uint32_t) patternLen, &testCmd[1]);

    memcpy(dataBuf, testCmd, sizeof(testCmd));
    for (unsigned int j = 0; j < patternLen; j++)
    {
        dataBuf[j + sizeof(testCmd)] = j % 256;
    }

    size_t lenWritten = 0;
    Debug_LOG_DEBUG("%s: (tester %u) sending command sized (%zu), ChanMux MTU is %d ...",
                    __func__, tester, len, ChanMuxClient_MTU);
    err = ChanMuxClient_write(&testChanMuxClient, dataBuf, len, &lenWritten);
    if (OS_SUCCESS != err)
    {
        Debug_LOG_ERROR("%s: FAIL (tester %u) got error %d when trying to send a request to proxy",
                        __func__, tester, err);
        goto exit;
    }
    Debug_LOG_DEBUG("%s: (tester %u) %zu sent", __func__, tester, lenWritten);

    // expecting to read an uint32, its value is the offset of the
    // first byte that mismatches the pattern
    size_t lenRead = 0;
    Debug_LOG_DEBUG("%s: (tester %u) attempting to read %zu bytes from ChanMux...",
                    __func__, tester, sizeof(uint32_t));
    err = ChanMuxClient_read(&testChanMuxClient,
                             dataBuf,
                             sizeof(uint32_t),
                             &lenRead);
    if (OS_SUCCESS != err)
    {
        Debug_LOG_ERROR("%s: FAIL (tester %u), err was %d with %zu bytes read",
                        __func__,
                        tester,
                        err,
                        lenRead);
        goto exit;
    }

    size_t numMatches       = BitConverter_getUint32BE(dataBuf);
    size_t expextedMatches  = len > ChanMuxClient_MTU ?
        maxPatterLen : patternLen;
    if (numMatches != expextedMatches)
    {
        Debug_LOG_ERROR("%s: FAIL (tester %u), peer complains of %zu out of %zu matches",
                        __func__,
                        tester,
                        numMatches,
                        expextedMatches);
        goto exit;
    }
    else
    {
        Debug_LOG_DEBUG("%s: (tester %u) %zu out of %zu match",
                        __func__,
                        tester,
                        numMatches,
                        expextedMatches);
    }
    retval = OS_SUCCESS;
exit:
    return retval;
}


OS_Error_t
ChanMuxTest_init(void)
{
    bool isSuccess = ChanMuxClient_ctor(
                        &testChanMuxClient,
                        &chanMuxClientConfig);
    if (!isSuccess)
    {
        Debug_LOG_ERROR("Failed to construct testChanMuxClient!");
        return -1;
    }
    return 0;
}

OS_Error_t
ChanMuxTest_testReturnCodes(unsigned int tester)
{
    static char dataBuf[PAGE_SIZE];
    OS_Error_t retval = OS_ERROR_GENERIC;
    size_t len = sizeof(dataBuf);

    /* the following code structure may look strange because od the repetition
     of Debug_LOG_* calls. The reason why we want this is because the macros,
     based on where they are places will print out __FILE__ and __LINE__ too */
    // TEST ChanMuxClient_write()
    if (ChanMuxClient_write(&testChanMuxClient, NULL, len, &len)
            != OS_ERROR_INVALID_PARAMETER)
    {
        Debug_LOG_ERROR("%s: FAIL (tester %u)",
                        __func__, tester);
    }
    else if (ChanMuxClient_write(&testChanMuxClient, dataBuf, len, NULL)
            != OS_ERROR_INVALID_PARAMETER)
    {
        Debug_LOG_ERROR("%s: FAIL (tester %u)",
                        __func__, tester);
    }
    else if (ChanMuxClient_write(&testChanMuxClient,
                                 dataBuf,
                                 PAGE_SIZE + 1,
                                 &len)
            != OS_ERROR_INVALID_PARAMETER)
    {
        Debug_LOG_ERROR("%s: FAIL (tester %u)",
                        __func__, tester);
    }
    else if (ChanMuxClient_readAsync(&testChanMuxClient, NULL, len, &len)
            != OS_ERROR_INVALID_PARAMETER)
    {
        Debug_LOG_ERROR("%s: FAIL (tester %u)",
                        __func__, tester);
    }
    else if (ChanMuxClient_readAsync(&testChanMuxClient, dataBuf, len, NULL)
            != OS_ERROR_INVALID_PARAMETER)
    {
        Debug_LOG_ERROR("%s: FAIL (tester %u)",
                        __func__, tester);
    }
    else if (ChanMuxClient_readAsync(&testChanMuxClient,
                                     dataBuf, PAGE_SIZE + 1,
                                     &len)
            != OS_ERROR_INVALID_PARAMETER)
    {
        Debug_LOG_ERROR("%s: FAIL (tester %u)",
                        __func__, tester);
    }
    else if (ChanMuxClient_read(&testChanMuxClient, NULL, len, &len)
            != OS_ERROR_INVALID_PARAMETER)
    {
        Debug_LOG_ERROR("%s: FAIL (tester %u)",
                        __func__, tester);
    }
    else if (ChanMuxClient_read(&testChanMuxClient, dataBuf, len, NULL)
            != OS_ERROR_INVALID_PARAMETER)
    {
        Debug_LOG_ERROR("%s: FAIL (tester %u)",
                        __func__, tester);
    }
    else if (ChanMuxClient_read(&testChanMuxClient, dataBuf, PAGE_SIZE + 1, &len)
            != OS_ERROR_INVALID_PARAMETER)
    {
        Debug_LOG_ERROR("%s: FAIL (tester %u)",
                        __func__, tester);
    }
    // test buffer overlap
    else if (ChanMuxClient_read(&testChanMuxClient,
                                *testChanMuxClient.config->port.read.io,
                                testChanMuxClient.config->port.read.len + 1,
                                &len)
            != OS_ERROR_INVALID_PARAMETER)
    {
        Debug_LOG_ERROR("%s: FAIL (tester %u)",
                        __func__, tester);
    }
    else
    {
        Debug_LOG_INFO("%s: SUCCESS (tester %u)", __func__, tester);
        retval = OS_SUCCESS;
    }
    return retval;
}

OS_Error_t
ChanMuxTest_testOverflow(unsigned int tester)
{
    static char dataBuf[PAGE_SIZE];
    OS_Error_t retval = OS_ERROR_GENERIC;
    char testCmd[] = { CMD_TEST_OVERFLOW };
    size_t len = sizeof(testCmd);

    Debug_LOG_DEBUG("%s: (tester %u) sending command to trigger overflow condition...",
                    __func__, tester);
    OS_Error_t err = ChanMuxClient_write(&testChanMuxClient,
                                         testCmd,
                                         len,
                                         &len);
    if (OS_SUCCESS != err)
    {
        Debug_LOG_ERROR("%s: (tester %u) failed trying to send a command, err was %d with %zu bytes written",
                        __func__,
                        tester,
                        err,
                        len);
        goto exit;
    }
    Debug_LOG_DEBUG("%s: (tester %u) command sent, retrieving data and overflow return code",
                    __func__, tester);
    len = CHANMUX_TEST_FIFO_SIZE + 1; // we will try to read more then possible
    err = ChanMuxClient_read(&testChanMuxClient,
                             dataBuf,
                             len,
                             &len);
    if ((OS_ERROR_OVERFLOW_DETECTED == err) && (CHANMUX_TEST_FIFO_SIZE == len))
    {
        len = 0;
        err = ChanMuxClient_readAsync(&testChanMuxClient,
                                      dataBuf,
                                      len,
                                      &len);
        if ((OS_SUCCESS == err))
        {
            Debug_LOG_INFO("%s: SUCCESS (tester %u)", __func__, tester);
            retval = OS_SUCCESS;
        }
        else
        {
            Debug_LOG_ERROR("%s: FAIL (tester %u), err was %d with %zu bytes read",
                            __func__,
                            tester,
                            err,
                            len);
        }
    }
    else
    {
        Debug_LOG_ERROR("%s: FAIL (tester %u), err was %d with %zu bytes read",
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
OS_Error_t
ChanMuxTest_testFullDuplexTxStream(unsigned int tester)
{
    OS_Error_t retval       = OS_ERROR_GENERIC;
    char testCmd[5]         = { CMD_TEST_FULL_DUPLEX };
    size_t len              = 0;
    OS_Error_t err          = OS_ERROR_GENERIC;

    // this variable must be local to the funtion as this function is exposed as
    // camkes interface function;
    static char dataBuf[PAGE_SIZE];

    // compose the command with the payload inside
    BitConverter_putUint32BE(FULL_DUPLEX_BLOCK_SIZE, &testCmd[1]);
    memcpy(dataBuf, testCmd, sizeof(testCmd));
    for (unsigned int j = 0; j < FULL_DUPLEX_BLOCK_SIZE; j++)
    {
        dataBuf[j + sizeof(testCmd)] = j % 256;
    }

    for (unsigned int i = 0; i < ITERATIONS; i++)
    {
        Debug_LOG_DEBUG("%s: (tester %u) sending command to trigger full duplex streaming...",
                        __func__, tester);
        len = sizeof(testCmd) + FULL_DUPLEX_BLOCK_SIZE;
        err = ChanMuxClient_write(&testChanMuxClient, dataBuf, len, &len);
        if (OS_SUCCESS != err)
        {
            Debug_LOG_ERROR("%s: (tester %u) got error %d when trying to send a request to proxy",
                            __func__, tester, err);
            goto exit;
        }
        retval = OS_SUCCESS;
    }
exit:
    if (OS_SUCCESS == retval)
    {
        Debug_LOG_INFO("%s: SUCCESS (tester %u)", __func__, tester);
    }
    return retval;
}

OS_Error_t
ChanMuxTest_testFullDuplex(unsigned int tester)
{
    OS_Error_t retval   = OS_ERROR_GENERIC;
    OS_Error_t err      = OS_ERROR_GENERIC;
    size_t len          = 0;
    size_t amount       = 0;

    static char dataBuf[PAGE_SIZE];

    while (amount < (ITERATIONS * FULL_DUPLEX_BLOCK_SIZE))
    {
        len = FULL_DUPLEX_BLOCK_SIZE;
        Debug_LOG_DEBUG("%s: (tester %u) attempting to read %zu bytes from ChanMux...",
                        __func__, tester, len);
        err = ChanMuxClient_read(&testChanMuxClient,
                                 dataBuf,
                                 len,
                                 &len);
        if (OS_SUCCESS == err)
        {
            Debug_LOG_DEBUG("%s: (tester %u) got %zu bytes from ChanMux",
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
                    retval = OS_SUCCESS;
                }
                else
                {
                    Debug_LOG_ERROR("%s: FAIL (tester %u), data received mismatches the expected pattern @ byte #%zu, expected 0x%02zx but received 0x%02x",
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
            Debug_LOG_ERROR("%s: FAIL (tester %u), err was %d with %zu bytes read",
                            __func__,
                            tester,
                            err,
                            len);
            goto exit;
        }
    }
 exit:
    if (OS_SUCCESS == retval)
    {
        Debug_LOG_INFO("%s: SUCCESS (tester %u)", __func__, tester);
    }
    return retval;
}

OS_Error_t
ChanMuxTest_testMaxSize(unsigned int tester)
{
    OS_Error_t retval = testMaxSize(tester, ChanMuxClient_MTU - 1);

    if (OS_SUCCESS == retval)
    {
        retval = testMaxSize(tester, ChanMuxClient_MTU);
    }

    if (OS_SUCCESS == retval)
    {
        retval = testMaxSize(tester, ChanMuxClient_MTU + 1);
    }

    if (OS_SUCCESS == retval)
    {
        Debug_LOG_INFO("%s: SUCCESS (tester %u)", __func__, tester);
    }
    return retval;
}
