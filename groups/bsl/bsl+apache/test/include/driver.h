/************************************************************************
 *
 * driver.h - testsuite driver declarations
 *
 * $Id: driver.h 648752 2008-04-16 17:01:56Z faridz $
 *
 ***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  "License"); you may  not use  this file
 * except in  compliance with the License.   You may obtain  a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the  License is distributed on an  "AS IS" BASIS,
 * WITHOUT  WARRANTIES OR CONDITIONS  OF ANY  KIND, either  express or
 * implied.   See  the License  for  the  specific language  governing
 * permissions and limitations under the License.
 *
 * Copyright 1994-2005 Rogue Wave Software.
 * 
 **************************************************************************/

#ifndef RW_DRIVER_H_INCLUDED
#define RW_DRIVER_H_INCLUDED

#include <testdefs.h>   // for test config macros


/**
 * Initializes the test driver, passes program arguments to it, processes
 * command line arguments, any arguments specified in the environment
 * variable RWSTD_TESTOPTS, and if successful, invokes the program-supplied
 * callback function.
 *
 * @param argc  The number of non-null elements in the argv array.
 * @param argv  A null-terminated array of command line arguments.
 *        If (argc == 0) the argument may be null.
 * @param filename  The name of the source file name, typically
 *        the __FILE__ macro. The argument may be null.
 * @param clause  The clause exercised by the program, such as
 *        lib.basic.string. The argument may be null.
 * @param comment An optional comment describing in more detail
 *        the functionality of the program. The argument may be null.
 * @param testfun  A pointer to a callback function to call by the
 *        driver after successful initialization. The argument must
 *        not be null.
 * @param optspec An optional character string describing command
 *        line options accepted by the program. The argument may
 *        be null.
 * @param ...  Optional list of handlers of command line options
 *         corresponding to the optspec.
 *
 * @return If initialization is successful, returns the value returned
 *         by the callback function. Otherwise, returns the non-zero
 *         value returned by the last initialization function or
 *         command line option handler.
 *
 * After the driver has been initialzied the user-supplied callback function
 * may call any of the driver diagnostic functions to record and perhaps also
 * issue diagnostic messages of varying severity.
 *
 * There are 10 levels of severity with 0 being the lowest and 9 the highest.
 * Diagnostics of all levels of severity come in two states: active and
 * inactive. All diagnostics are recorded but normally only active diagnostics
 * of severity 4 or above are issued. It is possible to cause diagnostics of
 * lower severity levels to be issued via a command line option to the driver.
 * Choosing to issue severity 0 diagnostics has the effect of issuing inactive
 * diagnostics.
 *
 * After the callback function returns the driver displays a summary detailing
 * the number of recorded diagnostics in each of the two states (active and
 * inactive).
 * 
 */
_TEST_EXPORT int
rw_test (int           argc,
         char*         argv[],
         const char*   filename,
         const char*   clause,
         const char*   comment,
         int         (*testfun)(int, char**),
         const char*   optspec,
         ...);

/**
 * Records and optionally issues a diagnostic of the highest severity 9.
 *
 * @param expr  A zero value denoting an active diagnostic or any non-zero
 *        vaue denoting an inactive diagnostic.
 * @param filename  An optional name of the file invoking the function.
 *        The argument may be null.
 * @param line  When positive, denotes the line number of the location
 *        relevant to the diagnostic. Negative values are ignored.
 * @param fmtspec  A printf format specifier (with extensions) used
 *        to format the text of the diagnostic.
 * @param ... Optional list of values to format.
 *
 * @return  Returns the value of expr passed to it.
 *
 * Every diagnostic is recorded but only active diagnostics may be issued,
 * depending on the setting of the diagnosable severity. The value of the
 * first argument determines whether a diagnostc is active or inactive.
 * Unlike the remaining diagnostic functions, rw_fatal doesn't return to
 * the caller when expr is 0 (i.e., when the diagnostic is active).
 * Instead, it causes the driver the exit the process with the staus equal
 * to 9, the severity of the diagnostic.
 */
_TEST_EXPORT int
rw_fatal (int         expr,
          const char* filename,
          int         line,
          const char* fmtspec,
          ...);

/**
 * Records and optionally issues a diagnostic of severity 8.
 *
 * @see #rw_fatal
 */
_TEST_EXPORT int
rw_error (int, const char*, int, const char*, ...);

/**
 * Records and optionally issues a diagnostic of severity 7.
 *
 * @see #rw_fatal
 */
_TEST_EXPORT int
rw_assert (int, const char*, int, const char*, ...);

/**
 * Records and optionally issues a diagnostic of severity 5.
 *
 * @see #rw_fatal
 */
_TEST_EXPORT int
rw_warn (int, const char*, int, const char*, ...);

/**
 * Records and optionally issues a diagnostic of severity 2.
 *
 * @see #rw_fatal
 */
_TEST_EXPORT int
rw_note (int, const char*, int, const char*, ...);

/**
 * Records and optionally issues a diagnostic of severity 1.
 *
 * @see #rw_fatal
 */
_TEST_EXPORT int
rw_info (int, const char*, int, const char*, ...);

/**
 * Enable/disable the specified diagnostic.
 *
 * @param fun Diagnostic function to enable or disable. Must be one of
 *        rw_fatal, rw_error, rw_assert, rw_warn, rw_note or rw_info.
 * @param enable Flag to indicate that the diagnostic function should
 *        be enabled or disabled.
 * @return  Returns the previous state of the diagnostic. If the first
 *        parameter is not an acceptable input, will return false.
 *
 * Example:
 *   rw_enable (rw_error, false); // disable all rw_error diagnostics
 *   rw_enable (rw_error);        // enable all rw_error diagnostics
 */
_TEST_EXPORT bool
rw_enable (int (*fun) (int, const char*, int, const char*, ...),
           bool enable = true);

#endif   // RW_DRIVER_H_INCLUDED
