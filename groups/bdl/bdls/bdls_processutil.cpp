// bdls_processutil.cpp                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdls_processutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdls_processutil_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_algorithm.h>
#include <bsl_iostream.h>

#if defined BSLS_PLATFORM_OS_WINDOWS
# include <bdlde_charconvertutf16.h>
# include <bdlma_localsequentialallocator.h>
# include <windows.h>
#else
# include <unistd.h>    // getpid

# if defined BSLS_PLATFORM_OS_DARWIN
#   include <libproc.h>
# elif defined BSLS_PLATFORM_OS_HPUX
#   include <sys/pstat.h>
# elif defined BSLS_PLATFORM_OS_SOLARIS
#   include <stdlib.h>
# elif defined BSLS_PLATFORM_OS_CYGWIN
#   ifndef   _REENTRANT
#     define _REENTRANT
#   endif

#   include <bdlsb_memoutstreambuf.h>
#   include <bsl_fstream.h>
#   include <fcntl.h>
#   include <procfs.h>
# elif defined BSLS_PLATFORM_OS_LINUX
#   include <errno.h>
# elif defined BSLS_PLATFORM_OS_AIX
#   include <bslma_allocator.h>
#   include <bslma_deallocatorproctor.h>
#   include <bslma_default.h>
#   include <bsl_algorithm.h>
#   include <sys/procfs.h>
#   include <fcntl.h>
#   include <procinfo.h>

// DRQS 30045663 - AIX is missing this prototype
extern int getargs(void *processBuffer,
                   int   bufferLen,
                   char *argsBuffer,
                   int   argsLen);
# else
#   error Unrecognized Platform
# endif
#endif

namespace BloombergLP {

namespace bdls {
                             // ------------------
                             // struct ProcessUtil
                             // ------------------

// CLASS METHODS
int ProcessUtil::getProcessId()
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    return static_cast<int>(GetCurrentProcessId());
#else
    return static_cast<int>(::getpid());
#endif
}

int ProcessUtil::getProcessName(bsl::string *result)
{
    BSLS_ASSERT(result);

    result->clear();

#if defined BSLS_PLATFORM_OS_WINDOWS
    static const size_t k_INITIAL_SIZE = MAX_PATH + 1;
    bdlma::LocalSequentialAllocator<sizeof(wchar_t) * k_INITIAL_SIZE> la;
    bsl::wstring wResult(MAX_PATH, L'\0', &la);
    while (wResult.length() <= 4 * k_INITIAL_SIZE) {
        DWORD length = GetModuleFileNameW(0, &wResult[0], wResult.length());
        if (length == 0) {  // Error
            return 1;                                                 // RETURN
        }
        else if (length < wResult.length()) {  // Success
            wResult.resize(length);
            return bdlde::CharConvertUtf16::utf16ToUtf8(result, wResult);
                                                                      // RETURN
        }
        else { // Not enough space for the process name in 'wResult'
            wResult.resize(wResult.length() * 2); // Make more space
        }
    }

    return -1; // The path name is too long (more than 4 * k_INITIAL_SIZE)
#elif defined BSLS_PLATFORM_OS_DARWIN
    char pathbuf[PROC_PIDPATHINFO_MAXSIZE];
    if (proc_pidpath (getpid(), pathbuf, sizeof(pathbuf)) <= 0) {
        return -1;
    }
    result->assign(pathbuf);
#elif defined BSLS_PLATFORM_OS_HPUX
    result->resize(256);
    int rc = pstat_getcommandline(&(*result->begin()),
                                  result->size(), 1,
                                  getpid());
    if (rc < 0)
    {
        return -1;
    }

    bsl::string::size_type pos = result->find_first_of(' ');
    if (bsl::string::npos != pos) {
        result->resize(pos);
    }
#elif defined BSLS_PLATFORM_OS_SOLARIS
    *result = ::getexecname();
#elif defined BSLS_PLATFORM_OS_CYGWIN
    enum { NUM_ELEMENTS = 14 + 16 };  // "/proc/<pid>/cmdline"

    bdlsb::MemOutStreamBuf osb(NUM_ELEMENTS);
    bsl::ostream           os(&osb);
    os << "/proc/" << getpid() << "/cmdline" << bsl::ends;
    const char *procfs = osb.data();

    bsl::ifstream ifs;
    ifs.open(procfs, bsl::ios_base::in | bsl::ios_base::binary);

    if (ifs.fail()) {
        return -1;                                                    // RETURN
    }

    ifs >> *result;

    bsl::string::size_type pos = result->find_first_of('\0');
    if (bsl::string::npos != pos) {
        result->resize(pos);
    }
#elif defined BSLS_PLATFORM_OS_LINUX
    *result = ::program_invocation_name;
#elif defined BSLS_PLATFORM_OS_AIX
    struct procentry64  procbuffer;
    static const int    argslen = 65535;
    bslma::Allocator   *allocator = bslma::Default::allocator(0);

    char *argsbuffer = static_cast<char *>(allocator->allocate(argslen));
    bslma::DeallocatorProctor<bslma::Allocator> proctor(argsbuffer, allocator);

    procbuffer.pi_pid = getpid();

    if (getargs(&procbuffer, sizeof(procbuffer), argsbuffer, argslen) < 0 ) {
        return -1;
    }

    if (bsl::find(argsbuffer, argsbuffer + argslen, '\0')
                                                     == argsbuffer + argslen) {
        // Process name is longer than 65K, and it was truncated.
        return -2;                                                    // RETURN
    }

    result->assign(argsbuffer);
#else
# error    Unrecognized Platform
#endif

    return result->empty();    // 'false' == 0 if success
}
}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
