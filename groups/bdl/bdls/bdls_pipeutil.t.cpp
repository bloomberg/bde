// bdls_pipeutil.t.cpp                                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdls_pipeutil.h>

#include <bslim_testutil.h>

#include <bslmt_threadutil.h>

#include <bsls_platform.h>

#include <bdlde_charconvertutf16.h>

#include <bdls_filedescriptorguard.h>
#include <bdls_pathutil.h>
#include <bdls_processutil.h>
#include <bdls_filesystemutil.h>

#include <bsl_algorithm.h>
#include <bsl_cstdlib.h>
#include <bsl_c_stdlib.h>
#include <bsl_iostream.h>
#include <bsl_string_view.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
#include <winerror.h>
#endif

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

// ----------------------------------------------------------------------------
// [ 1] int makeCanonicalName(string *pipeName, const string_view& baseName);
// [ 2] bool isOpenForReading(const string_view& pipeName);
// ----------------------------------------------------------------------------

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
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ============================================================================

namespace {

template <class STRING_TYPE>
void testCase1_makeCanonicalName(const STRING_TYPE& typeName,
                                 int                test,
                                 int                verbose)
{
    (void) verbose; (void) test;

    if (verbose) cout << "\n\t+++++++++++++++ Testing " << typeName << endl;

#ifdef BSLS_PLATFORM_OS_WINDOWS
    STRING_TYPE compare("\\\\.\\pipe\\foo.bar");
    STRING_TYPE name;
    ASSERT(0 == bdls::PipeUtil::makeCanonicalName(&name, "FOO.Bar"));
    LOOP2_ASSERT(name, compare, name == compare);
    if (verbose) { P(name) }
#else
    STRING_TYPE compare;
    const char  *tmpdirPtr;
    if (0 != (tmpdirPtr = bsl::getenv("SOCKDIR"))) {
        compare = tmpdirPtr;
    }
    else if (0 != (tmpdirPtr = bsl::getenv("TMPDIR"))) {
        compare = tmpdirPtr;
    }
    else {
        bdls::FilesystemUtil::getWorkingDirectory(&compare);
    }

    ASSERT(0 == bdls::PathUtil::appendIfValid(&compare, "foo.bar"));

    STRING_TYPE name;
    ASSERT(0 == bdls::PipeUtil::makeCanonicalName(&name, "FOO.Bar"));
    LOOP2_ASSERT(name, compare, name == compare);
    if (verbose) { P(name) }

    if (0 == bsl::getenv("SOCKDIR")) {
        // 'SOCKDIR' is not set.  Set it and retest.
        static char sockdir[] = "SOCKDIR=bozonono";
        ::putenv(sockdir);
        compare = bsl::getenv("SOCKDIR");
        ASSERT(0 == bdls::PathUtil::appendIfValid(&compare, "xy.ab"));
        ASSERT(0 == bdls::PipeUtil::makeCanonicalName(&name, "XY.Ab"));
        LOOP2_ASSERT(name, compare, name == compare);
        if (verbose) { P(name) }
    }
    else if (0 != bsl::getenv("TMPDIR")) {
        // Both 'SOCKDIR' and 'TMPDIR' are set.  If 'SOCKDIR' can be
        // removed, retest 'TMPDIR'.
        static char sockdir[] = "SOCKDIR";
        ::putenv(sockdir);
        if (0 == bsl::getenv("SOCKDIR")) {
            compare = bsl::getenv("TMPDIR");
            ASSERT(0 == bdls::PathUtil::appendIfValid(&compare, "xy.ab"));
            ASSERT(0 == bdls::PipeUtil::makeCanonicalName(&name, "XY.Ab"));
            LOOP2_ASSERT(name, compare, name == compare);
            if (verbose) { P(name) }
        }
    }
#endif
}

#ifdef BSLS_PLATFORM_OS_WINDOWS
bool namedPipeExists(const bsl::string_view& pipeName)
    // Returns 'true' if a named pipe exists at the path given by the specified
    // 'pipeName', and 'false' otherwise.  The result is unspecified if
    // 'pipeName' does not start with R"(\\.\pipe\)".
{
    wstring wPipeName;
    ASSERT(0 == bdlde::CharConvertUtf16::utf8ToUtf16(&wPipeName, pipeName));

    HANDLE handle = CreateFileW(wPipeName.data(),
                                0, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (INVALID_HANDLE_VALUE == handle) {
        return ERROR_FILE_NOT_FOUND != GetLastError();                // RETURN
    }

    CloseHandle(handle);
    return true;
}

struct ConnectAndRead {
    HANDLE d_handle;

    ConnectAndRead(HANDLE handle) : d_handle(handle) {}

    void operator()() const
        // Connect to the specified 'handle' and read until the client end has
        // disconnected, then disconnect.
    {
        // Note that 'ConnectNamedPipe' returns the value 0, normally
        // associated with failure, when a client has called 'CreateFile'
        // first.  To detect this situation, we have to check that
        // 'GetLastError' is 'ERROR_PIPE_CONNECTED'.  In this case, the
        // connection has been established.
        if (!ConnectNamedPipe(d_handle, NULL) &&
            ERROR_PIPE_CONNECTED != GetLastError()) {
            puts("'ConnectNamedPipe' failed");
            fflush(stdout);
            // This situation will cause 'isOpenForReading' to hang
            // indefinitely, so we might as well terminate the process.
            abort();
        }
        char buffer[1];
        DWORD bytesRead;
        while (ReadFile(d_handle, buffer, 1, &bytesRead, NULL)) { }
        DisconnectNamedPipe(d_handle);
    }
};
#endif

}  // close unnamed namespace

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;

    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
//  bool     veryVeryVerbose = argc > 4;
//  bool veryVeryVeryVerbose = argc > 5;

    switch(test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'isOpenForReading'
        //
        // Concerns:
        //: 1 'isOpenForReading' returns 'true' for a named pipe opened with
        //:   'PIPE_ACCESS_INBOUND' and 'PIPE_READMODE_BYTE' (which are the
        //:   mode flags used by 'balb_pipecontrolchannel') when there is a
        //:   pending call to 'ConnectNamedPipe'.
        //:
        //: 2 'isOpenForReading' returns 'false' immediately for a named pipe
        //:   opened with 'PIPE_ACCESS_OUTBOUND' and 'PIPE_READMODE_BYTE'
        //:   (meaning that the server end is only writing, not reading).
        //:
        //: 3 If a named pipe is busy, 'isOpenForReading' still returns
        //:   immediately and doesn't block.
        //:
        //: 4 'isOpenForReading' returns 'false' when given a filename that
        //:   refers to a nonexistent named pipe.
        //:
        //: 5 'isOpenForReadding' returns 'false' when given a filename that
        //:   refers to a file other than a pipe.
        //
        // Plan:
        //: 1 Create a named pipe using 'PIPE_ACCESS_INBOUND' and
        //:   'PIPE_READMODE_BYTE' and start a new thread that calls
        //:   'ConnectNamedPipe' on the server end of the pipe.  In the main
        //:   thread, call 'isOpenForReading' on the name of the pipe and
        //:   verify that it returns 'true'.  (C-1)
        //:
        //: 2 Create a named pipe using 'PIPE_ACCESS_OUTBOUND' and
        //:   'PIPE_READMODE_BYTE'.  Call 'isOpenForReading' on the name of the
        //:   pipe and verify that it returns 'false'.  (C-2)
        //:
        //: 3 Create a named pipe as in P-1, call 'CreateFile' to create a
        //:   handle to the client end, and then call 'ConnectNamedPipe' on
        //:   the server end to establish a connection.  Finally, call
        //:   'isOpenForReading'.  Then, create a named pipe as in P-2 and
        //:   repeat.  (C-3)
        //:
        //: 4 Use the 'CreateFile' function to ensure that a hardcoded pipe
        //:   name doesn't exist.  Then, call 'isOpenForReading' and verify
        //:   that it returns 'false'.  (This will fail if someone meddles
        //:   around and creates the pipe after we check for its existence.)
        //:   (C-4)
        //:
        //: 5 Call 'isOpenForReading' on the current executable name and verify
        //:   that it returns 'false'.  (C-5)
        //
        // Testing:
        //   bool isOpenForReading(const string_view& pipeName);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'isOpenForReading'" << endl
                          << "==========================" << endl;
#ifdef BSLS_PLATFORM_OS_WINDOWS
        {
            // P-1..2
            for (int i = 1; i <= 2; i++) {
                if (veryVerbose) { T_ P(i) }
                char buffer[200];
                sprintf(buffer,
                        "bdls_pipeutil.t#2.%d#%d",
                        i,
                        bdls::ProcessUtil::getProcessId());

                const DWORD serverOpenMode = (1 == i
                                              ? PIPE_ACCESS_INBOUND
                                              : PIPE_ACCESS_OUTBOUND);

                string pipeName;
                ASSERT(0 == bdls::PipeUtil::makeCanonicalName(&pipeName,
                                                              buffer));
                wstring wPipeName;
                ASSERT(0 == bdlde::CharConvertUtf16::utf8ToUtf16(&wPipeName,
                                                                 pipeName));

                // Create the pipe with nMaxInstances set to 1, so that this
                // will fail if the pipe already exists.
                HANDLE handle = CreateNamedPipeW(wPipeName.c_str(),
                                                 serverOpenMode,
                                                 PIPE_READMODE_BYTE,
                                                 1,
                                                 256,
                                                 256,
                                                 0,
                                                 NULL);
                ASSERT(INVALID_HANDLE_VALUE != handle);
                if (INVALID_HANDLE_VALUE == handle) {
                    continue;
                }

                bdls::FileDescriptorGuard guard(handle);

                bslmt::ThreadUtil::Handle serverThread;
                if (1 == i) {
                    if (0 != bslmt::ThreadUtil::create(&serverThread,
                                                       ConnectAndRead(handle)))
                    {
                        ASSERT(!"Failed to create server thread");
                        continue;
                    }
                }

                ASSERT((1 == i) == bdls::PipeUtil::isOpenForReading(pipeName));

                if (1 == i) {
                    bslmt::ThreadUtil::join(serverThread);
                }
            }
        }
        {
            // P-3
            for (int i = 1; i <= 2; i++) {
                if (veryVerbose) { T_ P(i) }
                char buffer[200];
                sprintf(buffer,
                        "bdls_pipeutil.t#2.3.%d#%d",
                        i,
                        bdls::ProcessUtil::getProcessId());

                const DWORD serverOpenMode = (i == 1
                                              ? PIPE_ACCESS_INBOUND
                                              : PIPE_ACCESS_OUTBOUND);

                string pipeName;
                ASSERT(0 == bdls::PipeUtil::makeCanonicalName(&pipeName,
                                                              buffer));
                wstring wPipeName;
                ASSERT(0 == bdlde::CharConvertUtf16::utf8ToUtf16(&wPipeName,
                                                                 pipeName));

                HANDLE serverHandle = CreateNamedPipeW(wPipeName.c_str(),
                                                       serverOpenMode,
                                                       PIPE_READMODE_BYTE,
                                                       1,
                                                       256,
                                                       256,
                                                       0,
                                                       NULL);
                ASSERT(INVALID_HANDLE_VALUE != serverHandle);
                if (INVALID_HANDLE_VALUE == serverHandle) {
                    continue;
                }

                bdls::FileDescriptorGuard serverGuard(serverHandle);

                const DWORD desiredAccess = (i == 1
                                             ? GENERIC_WRITE
                                             : GENERIC_READ);

                HANDLE clientHandle = CreateFileW(wPipeName.c_str(),
                                                  desiredAccess, 0, NULL,
                                                  OPEN_EXISTING, 0, NULL);
                ASSERT(INVALID_HANDLE_VALUE != clientHandle);
                if (INVALID_HANDLE_VALUE == clientHandle) {
                    continue;
                }

                bdls::FileDescriptorGuard clientGuard(clientHandle);

                // See 'ConnectAndRead::operator()' above for an explanation of
                // 'ERROR_PIPE_CONNECTED'.
                ASSERT(0 == ConnectNamedPipe(serverHandle, NULL) &&
                       ERROR_PIPE_CONNECTED == GetLastError());

                ASSERT(!bdls::PipeUtil::isOpenForReading(pipeName));
            }
        }
        {
            // P-4
            string pipeName;
            bdls::PipeUtil::makeCanonicalName(&pipeName,
                                              "bdls_pipeutil.t#2.4");
            ASSERT(!namedPipeExists(pipeName));
            ASSERT(!bdls::PipeUtil::isOpenForReading(pipeName));
        }
        {
            // P-5
            char buffer[200];
            const int retval =
                              GetModuleFileNameA(NULL, buffer, sizeof(buffer));
            ASSERT(0 < retval && retval < sizeof(buffer));
            ASSERT(!bdls::PipeUtil::isOpenForReading(buffer));
        }
#else
        if (verbose) puts("Skipping test case 2 on Unix");
        (void)veryVerbose;
#endif
      } break;
      case 1: {
        ///////////////////////////////////////////////////////////////////////
        // makeCanonicalName test
        //
        // makeCanonicalName() is, well, canonical.  Here we re-implement its
        // canonical behavior and ensure that the function behaves that way.
        ///////////////////////////////////////////////////////////////////////

        if (verbose) {
           cout << "makeCanonicalName test" << endl;
        }

        testCase1_makeCanonicalName<bsl::string>(
            "bsl::string", test, verbose);
        testCase1_makeCanonicalName<std::string>(
            "std::string", test, verbose);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
        testCase1_makeCanonicalName<std::pmr::string>(
            "std::pmr::string", test, verbose);
#endif
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
