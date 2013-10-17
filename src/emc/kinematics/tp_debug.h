/**
 *       @file  tp_debug.h
 *      @brief  A set of hacked-together debug macros for rtapi
 *
 * These simple Preprocessor macros are meant to be used for dumping text to
 * stdout in simulation mode. Eventually they should work in RTAPI proper, but
 * for now, there are no gaurantees.
 *
 *     @author  Robert W. Ellenberg, rwe24g@gmail.com
 *
 *   @internal
 *     Created  10/16/2013
 *    Revision  $Id: doxygen.templates,v 1.3 2010/07/06 09:20:12 mehner Exp $
 *    Compiler  gcc/g++
 *   Copyright  Copyright (c) 2013, Robert W. Ellenberg
 *
 * This source code is released for free distribution under the terms of the
 * GNU General Public License as published by the Free Software Foundation.
 * =====================================================================================
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "rtapi.h"		/* rtapi_print_msg */
#include "rtapi_string.h"       /* NULL */

#ifndef __TP_DEBUG_MACROS__
#define __TP_DEBUG_MACROS__

#ifdef TP_DEBUG_OUTPUT
#define debug(M, ...) rtapi_print("DEBUG %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define debug(M, ...) 
#endif

//Simple "hello-world" debug function to show that the program reached a given location.
//GDB would be easier...
#define debug_ping() debug("");
#endif
