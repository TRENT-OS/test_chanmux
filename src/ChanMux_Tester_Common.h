/*
 *  Common functions used for initializing and using the ChanMuxTest
 *
 *  Copyright (C) 2019, Hensoldt Cyber GmbH
 */

#pragma once

#include <stddef.h>

/* Exported functions ------------------------------------------------------- */

/**
*@brief Initializes the ChanMuxClient and the ChanMuxTest and maps the passed channel to the defined dataport.
*/
int
ChanMuxTest_init(unsigned int chan, char* proxyBuffer);

/**
*@brief Performs tests on parameters and return codes
*/
int ChanMuxTest_testReturnCodes(int tester);

/**
*@brief Performs test on overflow condition of ChanMux. A command is sent to the
 * proxy in order to trigger the big shot and then the overflow condition gets
 * tested.
*/
int ChanMuxTest_testOverflow(int tester);

/**
*@brief This fuction will stream data to the Proxy that will, in turn, echo
 * those data. This funtion is meant to be called by the other one of the "twin"
 * tester threads running in this test. The function is exposed as camkes
 * interface so it will run in a separate thread: this is how we grant that
 * the stream is full duplex. The receiving will happen simoultaneously to the
 * transmission.
 *
*/
int ChanMuxTest_testFullDuplexTxStream(int tester);

/**
*@brief Performs test of full duplex streaming on ChanMux. This function is
 * is synchronized with ChanMuxTest_testFullDuplexTxStream(). It will receive
 * and check the data sent by ChanMuxTest_testFullDuplexTxStream() 'online'
 * while it is still sending the data.
 *
*/
int ChanMuxTest_testFullDuplex(int tester);
