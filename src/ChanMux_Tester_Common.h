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
*@brief Performs test on overflow condition of ChanMux. A command is sent to the
 * proxy in order to trigger the big shot and then the overflow condition gets
 * tested.
*/
int ChanMuxTest_testOverflow();

