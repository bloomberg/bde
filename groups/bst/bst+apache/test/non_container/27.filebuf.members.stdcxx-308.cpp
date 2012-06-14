/***********************************************************************
 *
 * 27.filebuf.members.stdcxx-308.cpp - regression test for STDCXX-308
 *   http://issues.apache.org/jira/browse/STDCXX-308
 *
 * $Id: 27.filebuf.members.stdcxx-308.cpp 648752 2008-04-16 17:01:56Z faridz $
 *
 ***********************************************************************
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
 * Copyright 2008 Rogue Wave Software, Inc.
 *
 ***********************************************************************
 *
 * Test description:
 *  Sets a maxinum file size limit, forces filebuf to exceed the limit,
 *  causing the class dtor to fail to flush the excess data. Verifies
 *  that the dtor closes the associated file descriptor regardless of
 *  the failure as required by the resolution of LWG issue 622:
 *  http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-active.html#622
 *
 ***********************************************************************/

#include <cassert>   // for assert()
#include <cstdio>    // for remove(), size_t
#include <fstream>   // for filebuf

#if !defined (_WIN32) || defined (__CYGWIN__)
#  include <sys/resource.h>   // for getrlimit(), rlim_t
#  include <unistd.h>         // for close(), write(), ssize_t
#endif   // !_WIN32 || __CYGWIN__


int write_bytes (const char *fname, std::size_t nbytes)
{
    std::filebuf fb;

    if (   0 == fb.pubsetbuf (0, nbytes + 1)
        || 0 == fb.open (fname, std::ios::out))
        return -1;

#if defined (_RWSTD_VER) && !defined (_RWSTD_NO_EXT_FILEBUF)

    // use the filebuf::fd() extension to get the filebuf's
    // associated file descriptor
    const int fd = fb.fd ();

#else   // if defined (RWSTD_NO_EXT_FILEBUF)

    // assume fd is the next available file descriptor after
    // STDIN_FILENO, _FILENO_STDOUT, and STDERR_FILENO
    const int fd = 3;

#endif   // RWSTD_NO_EXT_FILEBUF

    if (0 < fd) {
        // fill up the filebuf's character buffer without
        // overflowing
        for (std::size_t i = 0; i != nbytes; ++i)
            fb.sputc ('*');
    }

    // have filebuf dtor try to flush the object's character
    // buffer and expect it to fail (and flush out only the
    // number specified by RLIMIT_FSIZE)

    // return the file descriptor to the caller so that it can
    // verify that it has been closed
    return fd;
}


int main ()
{
#if !defined (_WIN32) || defined (__CYGWIN__)

    const char fname[] = "testfile.text";

    std::remove (fname);

    rlimit rl;
    int status;

    // retrieve the current file size limits
    status = getrlimit (RLIMIT_FSIZE, &rl);

    if (status) {
        std::perror ("getrlimit(RLIMIT_FSIZE, ...)");
        return 1;
    }

    // uncomment for debugging
    // std::printf ("file size limits = %ld, %ld\n",
    //              long (rl.rlim_cur), long (rl.rlim_max));

    const rlim_t rlim_cur_saved = rl.rlim_cur;
    rl.rlim_cur = 32;

    // set a new file size limit
    status = setrlimit (RLIMIT_FSIZE, &rl);
    if (status) {
        std::perror ("setrlimit(RLIMIT_FSIZE, ...)");
        return 1;
    }

    // uncomment for debugging
    // status = getrlimit (RLIMIT_FSIZE, &rl);

    // if (status) {
    //     std::perror ("getrlimit(RLIMIT_FSIZE, ...)");
    //     return 1;
    // }
    // std::printf ("file size limits = %ld, %ld\n",
    //              long (rl.rlim_cur), long (rl.rlim_max));

    // try to write more bytes than the current soft limit using
    // filebuf, expecting the class dtor to fail but close the
    // associated file descriptor
    const int fd = write_bytes (fname, std::size_t (rl.rlim_cur + 1));
    if (fd < 0) {
        std::perror ("filebuf::~filebuf()");
        std::remove (fname);
        return 1;
    }

    // restore the previous soft limit
    rl.rlim_cur = rlim_cur_saved;

    status = setrlimit (RLIMIT_FSIZE, &rl);
    if (status) {
        std::perror ("setrlimit(RLIMIT_FSIZE, ...)");

        close (fd);

        std::remove (fname);
        return 1;
    }

    // try to write to the filebuf's file descriptor, expecting
    // the write() to fail
    const ssize_t nwrote = write (fd, "<", 1);

    assert (-1 == nwrote);

    // close file descriptor before removing the file (it's okay
    // if the call fails because the fd has already been closed)
    close (fd);
    std::remove (fname);

#else   // Windows

    // See about implementing using SetFileValidData():
    // http://msdn.microsoft.com/en-us/library/aa365544(VS.85).aspx

#endif   // !_WIN32 || __CYGWIN__

    return 0;
}
