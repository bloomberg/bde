// bdls_memoryutil.t.cpp                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bdls_memoryutil.h>

#include <bslim_testutil.h>

// TBD: this needs to test setting memory to executable

#include <bsls_platform.h>

#include <bsl_iostream.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
#endif
#ifdef BSLS_PLATFORM_OS_UNIX
#include <sys/resource.h>
#endif

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
//  bool         veryVerbose = argc > 3;
//  bool     veryVeryVerbose = argc > 4;
//  bool veryVeryVeryVerbose = argc > 5;

#ifdef BSLS_PLATFORM_OS_WINDOWS
    {
        // disable popup on crash

         SetErrorMode(SEM_NOGPFAULTERRORBOX);
    }
#endif
#ifdef BSLS_PLATFORM_OS_UNIX
    // disable core dumps

    {
        struct rlimit rl;
        rl.rlim_cur = rl.rlim_max = 0;
        setrlimit(RLIMIT_CORE, &rl);
    }
#endif

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // ---------------------------------------------------------------
        // Concern: functionality of protect()
        //
        // Test plan: enumerate all possible access mode combinations and try
        // reading, writing and executing the memory with these protection
        // modes.  Verify the actual protection modes match the specified ones.
        // ---------------------------------------------------------------

        // this test expects k_ACCESS_READ==1, k_ACCESS_WRITE==2,
        // k_ACCESS_EXECUTE==4

        ASSERT(bdls::MemoryUtil::k_ACCESS_READ == 1);
        ASSERT(bdls::MemoryUtil::k_ACCESS_WRITE == 2);
        ASSERT(bdls::MemoryUtil::k_ACCESS_EXECUTE == 4);

        static const char*const operations[] = { "read", "write" };
        static const char* const modes[] = {
            "k_ACCESS_NONE",          "k_ACCESS_READ",
            "k_ACCESS_WRITE",         "k_ACCESS_READ_WRITE",
            "k_ACCESS_EXECUTE",       "k_ACCESS_READ_EXECUTE",
            "k_ACCESS_WRITE_EXECUTE", "k_ACCESS_READ_WRITE_EXECUTE",
        };

        // test all 8 modes

        for (int mode=0; mode<sizeof(modes)/sizeof(*modes); ++mode) {
            // do not try to set executable bit when on HP-UX
#ifdef BSLS_PLATFORM_OS_HPUX
            if (mode & bdls::MemoryUtil::k_ACCESS_EXECUTE) {
                continue;
            }
#endif
            // test read & write

            for (int op=0;
                op<sizeof(operations)/sizeof(*operations);
                ++op)
            {
                if (op == 0 && mode != bdls::MemoryUtil::k_ACCESS_NONE
                    && !(mode & bdls::MemoryUtil::k_ACCESS_READ))
                {
                    // do not test disabled read with write/execute allowed:
                    // most platforms do not have fine-grained read access
                    // control

                    if (verbose) cout << "Skipping op:" << operations[op]
                                      << ", mode:" << modes[mode] << bsl::endl;
                    continue;
                }
                int expected_rc = !(mode & (1<<op));
                if (verbose) {
                    cout << "Testing op:" << operations[op]
                         << ", mode:" << modes[mode]
                         << ", expected to "
                         << ( expected_rc ? "fail" : "succeed" )
                         << bsl::endl;
                }
                enum {
                    ARBITRARY_BUT_SUFFICIENT_BUFFER_SIZE = 1000
                };
                char buffer[ARBITRARY_BUT_SUFFICIENT_BUFFER_SIZE];
#ifdef BSLS_PLATFORM_OS_WINDOWS
                const char* redirectToNull = " >NUL 2>&1";
#else
                const char* redirectToNull = " >/dev/null 2>&1";
#endif
                bsl::sprintf(buffer, "%s -1 %s %d %d%s",
                                             argv[0], argv[0], -10-op, mode,
                                             verbose ? "" : redirectToNull);
                int rc = system(buffer);
                LOOP4_ASSERT(modes[mode], operations[op], rc, expected_rc,
                             !rc == !expected_rc);
            }
        }
      } break;
      case 1: {
        ///USAGE EXAMPLE
        ///-------------
        //
        // First, allocate one page of memory.

        int pageSize = bdls::MemoryUtil::pageSize();
        char* data = (char*)bdls::MemoryUtil::allocate(pageSize);

        // Write into the allocated buffer.

        data[0] = 1;

        // Make the memory write protected

        bdls::MemoryUtil::protect(data, pageSize,
                                  bdls::MemoryUtil::k_ACCESS_READ);

        // Once again, try writing into the buffer.  This should crash our
        // process.

        // data[0] = 2;

        // Restore read/write access and free the allocated memory.  Actually,
        // this will never be executed, as the process has already crashed.

        bdls::MemoryUtil::protect(data, pageSize,
                                  bdls::MemoryUtil::k_ACCESS_READ_WRITE);
        bdls::MemoryUtil::deallocate(data);
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // Helper test case for case 2
        //
        // This case implements the system command, it allows case 2 to call
        // system within system, and thereby make abort messages redirectable.
        // --------------------------------------------------------------------

        char buffer[1000];
        buffer[0] = 0;

        for (int i = 2; i < argc; ++i) {
            strcat(buffer, " ");
            strcat(buffer, argv[i]);
        }

        cout << "Within system: " << buffer << endl;

        return !!system(buffer);                                      // RETURN
      } break;
      case -10: {
        // --------------------------------------------------------------------
        // Helper test case for case 2
        //
        // Plan: allocate some memory with protection mode specified in argv[2]
        // and verify it is readable.  Note that it is normal for this test
        // case to fail for some values of argv[2].
        // --------------------------------------------------------------------

        int size = bdls::MemoryUtil::pageSize();
        char* ptr = (char*) bdls::MemoryUtil::allocate(size);
        memset(ptr, 0x55, size);
        int rc = bdls::MemoryUtil::protect(ptr, size, atoi(argv[2]));
        ASSERT(0 == rc);
        for(int i=0; i<size; ++i) {
            ASSERT(((volatile char*)ptr)[i] == 0x55);
        }
        rc = bdls::MemoryUtil::protect(ptr, size,
                                    bdls::MemoryUtil::k_ACCESS_READ_WRITE);
        ASSERT(0 == rc);
        rc = bdls::MemoryUtil::deallocate(ptr);
        ASSERT(0 == rc);
      } break;
      case -11: {
        // --------------------------------------------------------------------
        // Helper test case for case 2
        //
        // Plan: allocate some memory with protection mode specified in argv[2]
        // and verify it is writable.  Note that it is normal for this test
        // case to fail for some values of argv[2].
        // --------------------------------------------------------------------

        int size = bdls::MemoryUtil::pageSize();
        char* ptr = (char*) bdls::MemoryUtil::allocate(size);
        int rc = bdls::MemoryUtil::protect(ptr, size, atoi(argv[2]));
        ASSERT(0 == rc);
        for(int i=0; i<size; ++i) {
            ((volatile char*)ptr)[i] = 0x55;
        }
        rc = bdls::MemoryUtil::protect(ptr, size,
                                    bdls::MemoryUtil::k_ACCESS_READ_WRITE);
        ASSERT(0 == rc);
        rc = bdls::MemoryUtil::deallocate(ptr);
        ASSERT(0 == rc);
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "."
             << endl;
    }
    return testStatus;
}

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
