/* Includes ------------------------------------------------------------------*/
#include "LibDebug/Debug.h"
#include "ChanMuxNvmDriver.h"
#include "LibUtil/BitConverter.h"
#include "LibMacros/Test.h"

#include "camkes.h"

#include <string.h>
#include <stdio.h>

#define CMD_TEST_OVERFLOW       0
#define CMD_TEST_FULL_DUPLEX    1
#define CMD_TEST_MAX_SIZE       2

#define ITERATIONS                  100
#define FULL_DUPLEX_BLOCK_SIZE      (CHANMUX_TEST_FIFO_SIZE / ITERATIONS)

/* Instance variables ---------------------------------------------------------*/

static const ChanMuxClientConfig_t chanMuxClientConfig = {
    .port   = CHANMUX_DATAPORT_ASSIGN(chanMux_chan_portRead,
                                      chanMux_chan_portWrite),
    .wait   = chanMux_chan_eventHasData_wait,
    .write  = chanMux_Rpc_write,
    .read   = chanMux_Rpc_read
};

static ChanMuxClient testChanMuxClient;

static OS_Error_t
testMaxSize(size_t len)
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
    Debug_LOG_DEBUG("%s: sending command sized (%zu), ChanMux MTU is %lu ...",
                    __func__, len, ChanMuxClient_MTU);
    err = ChanMuxClient_write(&testChanMuxClient, dataBuf, len, &lenWritten);
    if (OS_SUCCESS != err)
    {
        Debug_LOG_ERROR("%s: ChanMuxClient_write() failed, got error %d when trying to send a request to proxy",
                        __func__, err);
        goto exit;
    }
    Debug_LOG_DEBUG("%s: %zu sent", __func__, lenWritten);

    // expecting to read an uint32, its value is the offset of the
    // first byte that mismatches the pattern
    size_t lenRead = 0;
    Debug_LOG_DEBUG("%s: attempting to read %zu bytes from ChanMux...",
                    __func__, sizeof(uint32_t));
    err = ChanMuxClient_read(&testChanMuxClient,
                             dataBuf,
                             sizeof(uint32_t),
                             &lenRead);
    if (OS_SUCCESS != err)
    {
        Debug_LOG_ERROR("%s: ChanMuxClient_read() failed, err was %d with %zu bytes read",
                        __func__,
                        err,
                        lenRead);
        goto exit;
    }

    size_t numMatches       = BitConverter_getUint32BE(dataBuf);
    size_t expextedMatches  = len > ChanMuxClient_MTU ?
        maxPatterLen : patternLen;
    if (numMatches != expextedMatches)
    {
        Debug_LOG_ERROR("%s: peer complains of %zu out of %zu matches",
                        __func__,
                        numMatches,
                        expextedMatches);
        goto exit;
    }
    else
    {
        Debug_LOG_DEBUG("%s: %zu out of %zu match",
                        __func__,
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

void
ChanMuxTest_testReturnCodes()
{
    static char dataBuf[PAGE_SIZE];
    size_t len = sizeof(dataBuf);

    TEST_START();

    TEST_INVAL_PARAM(
        ChanMuxClient_write(&testChanMuxClient, NULL, len, &len));
    TEST_INVAL_PARAM(
        ChanMuxClient_write(&testChanMuxClient, dataBuf, len, NULL));
    TEST_INVAL_PARAM(
        ChanMuxClient_write(&testChanMuxClient, dataBuf, PAGE_SIZE + 1, &len));
    TEST_INVAL_PARAM(
        ChanMuxClient_readAsync( &testChanMuxClient, NULL, len, &len));
    TEST_INVAL_PARAM(
        ChanMuxClient_readAsync(&testChanMuxClient, dataBuf, len, NULL));
    TEST_INVAL_PARAM(
        ChanMuxClient_readAsync(
            &testChanMuxClient, dataBuf, PAGE_SIZE + 1, &len));
    TEST_INVAL_PARAM(
        ChanMuxClient_read(&testChanMuxClient, NULL, len, &len));
    TEST_INVAL_PARAM(
        ChanMuxClient_read(&testChanMuxClient, dataBuf, len, NULL));
    TEST_INVAL_PARAM(
        ChanMuxClient_read(&testChanMuxClient, dataBuf, PAGE_SIZE + 1, &len));
    // test buffer overlap
    TEST_INVAL_PARAM(
        ChanMuxClient_read(
            &testChanMuxClient,
            OS_Dataport_getBuf(testChanMuxClient.config->port.read),
            OS_Dataport_getSize(testChanMuxClient.config->port.read) + 1,
            &len));

    TEST_FINISH();
}

void
ChanMuxTest_testOverflow()
{
    TEST_START();

    static char dataBuf[PAGE_SIZE];
    char testCmd[] = { CMD_TEST_OVERFLOW };
    size_t len = sizeof(testCmd);

    Debug_LOG_DEBUG("%s: sending command to trigger overflow condition", __func__);
    OS_Error_t err = ChanMuxClient_write(&testChanMuxClient,
                                         testCmd,
                                         len,
                                         &len);
    if (OS_SUCCESS != err)
    {
        Debug_LOG_ERROR("%s: ChanMuxClient_write() failed trying to send a command, err was %d with %zu bytes written",
                        __func__,
                        err,
                        len);
    }
    TEST_SUCCESS(err);

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
        if (OS_SUCCESS != err)
        {
            Debug_LOG_ERROR("%s: ChanMuxClient_readAsync() failed, err was %d with %zu bytes read",
                            __func__,
                            err,
                            len);
        }
        TEST_SUCCESS(err);
    }
    TEST_SUCCESS(err);

    TEST_FINISH();
}

void
ChanMuxTest_testMaxSize()
{
    TEST_START();

    TEST_SUCCESS(testMaxSize(ChanMuxClient_MTU - 1));
    TEST_SUCCESS(testMaxSize(ChanMuxClient_MTU));
    TEST_SUCCESS(testMaxSize(ChanMuxClient_MTU + 1));

    TEST_FINISH();
}

// This routine will be shared to the other test thread via an interface so that
// we will have a thread (the one running the interface) making Tx streaming
// while the other is doing Rx streaming
OS_Error_t
ChanMuxTest_testFullDuplexTxStream(unsigned int tester)
{
    OS_Error_t retval       = OS_ERROR_GENERIC;
    char testCmd[5]         = { CMD_TEST_FULL_DUPLEX };
    size_t len              = 0;
    OS_Error_t err          = OS_ERROR_GENERIC;

    // this variable must be local to the function as this function is exposed as
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
    size_t len          = 0;
    size_t amount       = 0;

    static char dataBuf[PAGE_SIZE];

    while (amount < (ITERATIONS * FULL_DUPLEX_BLOCK_SIZE))
    {
        len = FULL_DUPLEX_BLOCK_SIZE;
        Debug_LOG_DEBUG("%s: (tester %u) attempting to read %zu bytes from ChanMux...",
                        __func__, tester, len);
        OS_Error_t err = ChanMuxClient_read(&testChanMuxClient,
                                            dataBuf,
                                            len,
                                            &len);
        if (OS_SUCCESS != err)
        {
            Debug_LOG_ERROR("%s: FAIL (tester %u), err was %d with %zu bytes read",
                            __func__, tester, err, len);
            return err;
        }
        else if (0 == len)
        {
            Debug_LOG_ERROR("%s: FAIL (tester %u), ChanMuxClient_read() unexpectedly successful although returned len is 0",
                            __func__, tester);
            return OS_ERROR_ABORTED;
        }
        else
        {
            Debug_LOG_DEBUG("%s: (tester %u) got %zu bytes from ChanMux",
                        __func__, tester, len);
            size_t i        = 0;
            size_t expected = 0;

            while (i < len)
            {
                const size_t modulo =
                    FULL_DUPLEX_BLOCK_SIZE < 256 ? FULL_DUPLEX_BLOCK_SIZE : 256;
                expected = (i + amount) % modulo;
                if (dataBuf[i] == expected)
                {
                    i++;
                }
                else
                {
                    break;
                }
            }
            if (i != len)
            {
                Debug_LOG_ERROR("%s: FAIL (tester %u), data received mismatches the expected pattern @ byte #%zu, expected 0x%02zx but received 0x%02x",
                                __func__,
                                tester,
                                amount + i,
                                expected,
                                dataBuf[i]);
                return OS_ERROR_ABORTED;
            }
            amount += len;
        }
    }
    Debug_LOG_INFO("%s: SUCCESS (tester %u)", __func__, tester);
    return OS_SUCCESS;
}
