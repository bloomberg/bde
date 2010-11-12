/***************************************************************************
 *
 * cmdopt.h - declarations of helper functions for the processing
 *            of command line options
 *
 * $Id: cmdopt.h 550991 2007-06-26 23:58:07Z sebor $
 *
 ************************************************************************
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
 * Copyright 1994-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#ifndef RW_CMDOPT_H_INCLUDED
#define RW_CMDOPT_H_INCLUDED


#include <testdefs.h>   // for test config macros


/**
 * Appends a set of command line options and their handlers to the global
 * list of command line option handlers for the current process.
 *
 * @param optspec  A string of command line option specifiers describing
 *        the names and parameters of the command line options and their
 *        handlers. The string has the following syntax:
 *
 *        <optspec> ::= <opt> [ ':' | '=' | '#' ]
 *                            [ @N | @* | '!' ]
 *                            [ <opts> ]
 *        <opt>     ::= <sopt> [ '|' <lopt>]
 *                  ::= '|' <lopt>
 *        <sopt>    ::= char
 *        <lopt>    ::= '-' char char*
 *        <char>    ::= 'A' - 'Z', 'a'-'z', '0' - '9', '_'
 *
 *        Each command line option may have a short name, a long name,
 *        or both. When referenced (either on the command line or in
 *        the environment), the name of command line option is
 *        introduced by a hyphen ('-').
 *
 *        A short option name (<sopt>) consits of a single alphanumeric
 *        character or an underscore ('_').
 *
 *        A long name (<lopt>) starts with a hyphen ('-') followed by
 *        one or more aphanumeric characters or underscores.
 *
 *        The name of the command line option is followd by one or more
 *        special characters with the following meaning:
 *
 *        ':'   the option takes an optional argument
 *        '='   the option takes a required argument that must immediately
 *              follow the equals sign
 *        '#'   the handler for this option is not a function but rather
 *              a pointer to an signed integer that rw_runopts() sets to
 *              a non-zero value if the option appears on the command line
 *        @N    the option handler will be invoked for the first N
 *              occurrences of the option name on the command line
 *        @*    the option handler will be invoked for every occurrence
 *              of the option name on the command line
 *        !     the option handler will be invoked only if the option name
 *              does not appear on the command line
 *
 * @param ...  A list of zero or more command line option handlers whose
 *        type is either int (*)(int, char**) or int*, the latter
 *        corresponding to options denoted with the '#' special character.
 *
 * @return  On success, returns the number of command line options
 *          currently defined for the process, negative value on error.
 */
_TEST_EXPORT int
rw_setopts (const char *optspec, ...);


/**
 * Processes the set of command line options and arguments specified by
 * the function arguments (usually the same arguments as those passed to
 * main()).
 *
 * @param argc  The number of non-zero elements of the argv vector.
 * @param argv  An array of pointers to command line options and arguments
 *        whose the last element, argv [argc], has the value 0.
 *
 * @return  Returns the status of last evaluated command line option handler.
 */
_TEST_EXPORT int
rw_runopts (int argc, char *argv[]);


/**
 * Processes the set of command line options and arguments specified by
 * the function argument (usually the value of an environment variable).
 *
 * @param argvstr  A character string of command line options and arguments
 *        separated by one or more spaces.
 *
 * @return  Returns the status of last evaluated command line option handler.
 */
_TEST_EXPORT int
rw_runopts (const char *argvstr);


/**
 * Determines whether a feature is enabled.
 *
 * @param name  The name of a feature.
 *
 * @return  Returns a non-zero value if the named feature is enabled,
 *          otherwise 0.
 */
_TEST_EXPORT int
rw_enabled (const char *name);


/**
 * Determines whether a case (or line) is enabled.
 *
 * @param name  The case (or line) number
 *
 * @return  Returns a non-zero value if the case (line) is enabled,
 *          otherwise 0.
 */
_TEST_EXPORT int
rw_enabled (int line);


#endif   // RW_CMDOPT_H_INCLUDED
