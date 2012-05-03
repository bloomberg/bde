/************************************************************************
 *
 * file.h - common file I/O definitions
 *
 * $Id: file.h 423332 2006-07-19 01:40:24Z sebor $
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
 * Copyright 2004-2006 Rogue Wave Software.
 *
 **************************************************************************/

#ifndef RW_FILE_H_INCLUDED
#define RW_FILE_H_INCLUDED


#include <testdefs.h>   // for test config macros


#ifndef _MSC_VER
   // POSIX special files:
   // http://www.opengroup.org/onlinepubs/007904975/basedefs/xbd_chap10.html
#  define DEV_CONSOLE   "/dev/console"
#  define DEV_NULL      "/dev/null"
#  define DEV_TTY       "/dev/tty"
#else   // if defined (_MSC_VER)
#  define DEV_CONSOLE   "CON:"
#  define DEV_NULL      "NUL:"
#  define DEV_TTY       "CON:"
#endif   // _MSC_VER


#if _RWSTD_PATH_SEP == '/'
#  define SLASH          "/"
#  define SHELL_MV       "mv "
#  define SHELL_RM_F     "rm -f "
#  define SHELL_RM_RF    "rm -rf "
#else
#  define SLASH          "\\"
#  define SHELL_MV       "move /Y "
#  define SHELL_RM_F     "del /F "
#  define SHELL_RM_RF    "rmdir /Q /S "
#endif


// writes chars using symbolic names from the Portable Character Set (PCS)
// or using the <U00XX> notations for narrow characters outside that set
// if teh second argument is 0, writes out the CHARMAP section of the locale
// definition file for the Portable Character Set (in POSIX-compliant format)
_TEST_EXPORT void
pcs_write (void*, const char*);


// creates a unique temporary file name as if by calling tmpnam()
// but avoiding various platform-specific quirks (such as HP-UX
// failure when _REENTRANT is #defined or GNU glibc warnings)
_TEST_EXPORT
const char* rw_tmpnam (char*);

// tries to open file named by the first argument and, if successful,
// allocates a block of storage sufficiently large to hold the file's
// entire contents, as determined by the stat() function; it then reads
// the contents of the file into the block of storage, returning a pointer
// to the block; if the second argument is non-0, sets the pointed-to value
// to the number of bytes read
// as a special case, when the first argument is 0 and the second is not,
// the function takes the third argument as a pointer to the buffer that
// it will use to read the contents of files into in subsequent calls,
// provided the buffer is large enough
_TEST_EXPORT void*
rw_fread (const char*,
          _RWSTD_SIZE_T* = 0   /* size in bytes */,
          const char*    = "r" /* stdio open mode */);

// if the second argument is non-0, writes N bytes starting at that
// location into the file named by the first argument; N is taken
// from the value pointed to by the third argument, if non-0, or
// as the result of calling strlen() on the buffer pointed to by
// the second argument; if the second argument is 0, the function
// removes the named file; returns the number of bytes written
_TEST_EXPORT _RWSTD_SIZE_T
rw_fwrite (const char*,
           const void*,
           _RWSTD_SIZE_T = ~0  /* size in bytes */,
           const char*   = "w" /* stdio open mode */ );


_TEST_EXPORT int
rw_nextfd (int*);

#endif   // RW_FILE_H_INCLUDED
