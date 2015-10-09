// btls_iovec.t.cpp                                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <btls_iovec.h>

#include <bslim_testutil.h>

#include <bsl_cstring.h>
#include <bsl_iostream.h>

#include <bsl_c_stdlib.h>     // 'atoi'

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

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// Verify the operations of 'btls::Iovec' and 'btls::Ovec' and that the
// structures map correctly onto the platform specific scatter/gather vectors.
//-----------------------------------------------------------------------------
// [ 3] btls::Iovec::Iovec(const Iovec&)
// [ 3] btls::Ovec::Ovec(const Ovec&)
// [ 3] btls::Ovec::Ovec(const Iovec&)
// [ 2] btls::Iovec::Iovec()
// [ 2] btls::Ovec::Ovec()
// [ 2] btls::Iovec::setBuffer()
// [ 2] btls::Ovec::setBuffer()
// [ 2] btls::Iovec::buffer()
// [ 2] btls::Ovec::buffer()
// [ 2] btls::Iovec::length()
// [ 2] btls::Ovec::length()
// [ 1] USAGE TEST
//=============================================================================

int globalVerbose = 0;

#ifdef BSLS_PLATFORM_CMP_MSVC
static int TestWSASend(SOCKET                             s,
                       LPWSABUF                           lpBuffers,
                       DWORD                              dwBufferCount,
                       LPDWORD                            lpNumberOfBytesSent,
                       DWORD                              dwFlags,
                       LPWSAOVERLAPPED                    lpOverlapped,
                       LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine )
{
    if (globalVerbose) P(dwBufferCount);
    DWORD i;
    for (i = 0; i < dwBufferCount; ++i) {
        btls::Ovec * ovec = (btls::Ovec *) lpBuffers;
        LOOP_ASSERT(i, ((void *) lpBuffers[i].buf) == ovec[i].buffer());
        LOOP_ASSERT(i, ((unsigned int) lpBuffers[i].len) == ovec[i].length());
        if (globalVerbose)
               cout << (void *) lpBuffers[i].buf << ' ' <<
                    lpBuffers[i].len << endl;
    }
    return 0;
}
#else
// UNIX like systems - writev scatter/gather write function
ssize_t writev(int fildes, const struct iovec *iov, int iovcnt)
{
    (void)fildes;
    if (globalVerbose) P(iovcnt);
    int i;
    for (i = 0; i < iovcnt; ++i) {
        const btls::Ovec *ovec = (const btls::Ovec *)iov;
        LOOP_ASSERT(i, ((void *) iov[i].iov_base) == ovec[i].buffer());
        LOOP_ASSERT(i, ((unsigned int) iov[i].iov_len) ==
                                              (unsigned int) ovec[i].length());
        if (globalVerbose)
               cout << (void *) iov[i].iov_base << ' ' <<
                    iov[i].iov_len << endl;
    }
    return 0;
}
#endif

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    globalVerbose = verbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        //   TEST COPY CONSTRUCTOR
        // Verifies results of the copy constructor and that Ovec and Iovec
        // have identical internal structure.
        //
        // Testing:
        //   btls::Iovec::Iovec(const Iovec&)
        //   btls::Ovec::Ovec(const Ovec&)
        //   btls::Ovec::Ovec(const Iovec&)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING COPY CONTRUCTOR" <<
                             endl << "-----------------------" << endl;

        if (veryVerbose) cout << endl << "TESTING Iovec(const Iovec&)" << endl;
        {
            char buf[1];
            char buf2[2];
            char buf3[4];
            btls::Iovec I;

            I.setBuffer(buf, 1);

            btls::Iovec C(I);

            ASSERT(I.buffer() == buf);
            ASSERT(I.length() == 1);

            ASSERT(C.buffer() == buf);
            ASSERT(C.length() == 1);

            I.setBuffer(buf2, 2);

            ASSERT(I.buffer() == buf2);
            ASSERT(I.length() == 2);

            ASSERT(C.buffer() == buf);
            ASSERT(C.length() == 1);

            C.setBuffer(buf3, 4);

            ASSERT(I.buffer() == buf2);
            ASSERT(I.length() == 2);

            ASSERT(C.buffer() == buf3);
            ASSERT(C.length() == 4);
        }

        if (veryVerbose) cout << endl << "TESTING Ovec(const Ovec&)" << endl;
        {
            char buf[1];
            char buf2[2];
            char buf3[4];
            btls::Ovec I;

            I.setBuffer(buf, 1);

            btls::Ovec C(I);

            ASSERT(I.buffer() == buf);
            ASSERT(I.length() == 1);

            ASSERT(C.buffer() == buf);
            ASSERT(C.length() == 1);

            I.setBuffer(buf2, 2);

            ASSERT(I.buffer() == buf2);
            ASSERT(I.length() == 2);

            ASSERT(C.buffer() == buf);
            ASSERT(C.length() == 1);

            C.setBuffer(buf3, 4);

            ASSERT(I.buffer() == buf2);
            ASSERT(I.length() == 2);

            ASSERT(C.buffer() == buf3);
            ASSERT(C.length() == 4);
        }

        if (veryVerbose) cout << endl << "TESTING Ovec(const Iovec&)" << endl;
        {
            char buf[1];
            char buf2[2];
            char buf3[4];
            btls::Iovec I;

            I.setBuffer(buf, 1);

            btls::Ovec C(I);

            ASSERT(sizeof(I) == sizeof(C));
            ASSERT(memcmp(&I, &C, sizeof(I)) == 0);

            ASSERT(I.buffer() == buf);
            ASSERT(I.length() == 1);

            ASSERT(C.buffer() == buf);
            ASSERT(C.length() == 1);

            I.setBuffer(buf2, 2);

            ASSERT(I.buffer() == buf2);
            ASSERT(I.length() == 2);

            ASSERT(C.buffer() == buf);
            ASSERT(C.length() == 1);

            C.setBuffer(buf3, 4);

            ASSERT(I.buffer() == buf2);
            ASSERT(I.length() == 2);

            ASSERT(C.buffer() == buf3);
            ASSERT(C.length() == 4);

       }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        //  TESTING DEFAULT CONSTRUCTOR
        //
        // Testing:
        //   btls::Iovec::Iovec()
        //   btls::Ovec::Ovec()
        //   btls::Iovec::setBuffer()
        //   btls::Ovec::setBuffer()
        //   btls::Iovec::buffer()
        //   btls::Ovec::buffer()
        //   btls::Iovec::length()
        //   btls::Ovec::length()
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING DEFAULT CONTRUCTOR" <<
                             endl << "--------------------------" << endl;

        char buf[1];
        btls::Iovec I;

        I.setBuffer(buf, 1);
        ASSERT(I.buffer() == buf);
        ASSERT(I.length() == 1);

        btls::Ovec O;

        O.setBuffer((const char *) buf, 1);
        ASSERT(O.buffer() == buf);
        ASSERT(O.length() == 1);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        //  USAGE
        // Plan
        //   The usage case calls two different scatter/gather socket
        //   operations.  These have been replaced in this test driver
        //   with test functions which validate the data passed.
        // Testing
        // USAGE
        // --------------------------------------------------------------------

///Usage
///-----
// The following snippets of code illustrate how to use 'btls::Iovec' with
// platform dependent scatter/gather operations.  Typically, an array of
// structures is created with each element containing a pointer to a buffer.  A
// pointer to the array of 'btls::Ovec' or 'btls::Iovec' is passed to the IO
// operation.  In this example an array of 3 buffer pointers is created.
//..
    char buf1[10];
    char buf2[4];
    char buf3[7];
    btls::Ovec vector[3];
    vector[0].setBuffer(buf1, 10);
    vector[1].setBuffer(buf2, 4);
    vector[2].setBuffer(buf3, 7);
//..
// On UNIX-like systems the internal structure of 'btls::Iovec' and
// 'btls::Ovec' use the 'iovec' 'struct'.  This structure is used for the
// 'writev' and 'readv' scatter/gather read/write operations or within the
// 'msgbuf' 'struct' used by 'sendmsg' and 'readmsg' socket operations.
//
// On Windows the internal structure of 'btls::Iovec' and 'btls::Ovec' use the
// 'WSABUF' 'struct'.  This structure is used for the 'WSARecv' and 'WSASend'
// scatter/gather send/receive socket operations or within the 'WSAMSG'
// 'struct' used by 'WSARecvMsg' and 'WSASendMsg' socket operations.
//..
    #ifndef BSLS_PLATFORM_CMP_MSVC
        // Verify values for UNIX like systems
        int socket = 0;
        ::writev(socket, (struct iovec *) vector, 3);
    #else
        // Verify values for Windows
        DWORD writeCount = 0;
        SOCKET socket = (SOCKET) 0;
        int ret =
               TestWSASend(socket, (WSABUF *) vector, 3, &writeCount, 0, 0, 0);
    #endif
//..
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
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
