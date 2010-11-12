// btes_iovec.t.cpp    -*-C++-*-

#include <btes_iovec.h>

#include <bsl_cstring.h>
#include <bsl_iostream.h>

#include <bsl_c_stdlib.h>     // atoi()

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//==========================================================================
//                             TEST PLAN
//--------------------------------------------------------------------------
// Verify the operations of 'btes_Iovec' and 'btes_Ovec'
// and that the structures map correctly onto the platform specific
// scatter/gather vectors.
//--------------------------------------------------------------------------
// [ 3] btes_Iovec::Iovec(const Iovec&)
// [ 3] btes_Ovec::Ovec(const Ovec&)
// [ 3] btes_Ovec::Ovec(const Iovec&)
// [ 2] btes_Iovec::Iovec()
// [ 2] btes_Ovec::Ovec()
// [ 2] btes_Iovec::setBuffer()
// [ 2] btes_Ovec::setBuffer()
// [ 2] btes_Iovec::buffer()
// [ 2] btes_Ovec::buffer()
// [ 2] btes_Iovec::length()
// [ 2] btes_Ovec::length()
// [ 1] USAGE TEST
//==========================================================================

int globalVerbose = 0;

#ifdef BSLS_PLATFORM__CMP_MSVC
static int TestWSASend(
  SOCKET s,
  LPWSABUF lpBuffers,
  DWORD dwBufferCount,
  LPDWORD lpNumberOfBytesSent,
  DWORD dwFlags,
  LPWSAOVERLAPPED lpOverlapped,
  LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
)
{
    if (globalVerbose) P(dwBufferCount);
    DWORD i;
    for (i = 0; i < dwBufferCount; ++i) {
        btes_Ovec * ovec = (btes_Ovec *) lpBuffers;
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
ssize_t writev(int fildes,  const  struct  iovec  *iov,  int iovcnt)
{
    if (globalVerbose) P(iovcnt);
    int i;
    for (i = 0; i < iovcnt; ++i) {
        btes_Ovec * ovec = (btes_Ovec *) iov;
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

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    globalVerbose = verbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------
        //   TEST COPY CONSTRUCTOR
        // Verifies results of the copy constructor and that Ovec and
        // Iovec have identical internal structure.
        //
        // Testing:
        // btes_Iovec::Iovec(const Iovec&)
        // btes_Ovec::Ovec(const Ovec&)
        // btes_Ovec::Ovec(const Iovec&)
        // --------------------------------------------------------

        if (verbose) cout << endl << "TESTING COPY CONTRUCTOR" <<
                             endl << "-----------------------" << endl;

        if (veryVerbose) cout << endl << "TESTING Iovec(const Iovec&)" << endl;
        {
            char buf[1];
            char buf2[2];
            char buf3[4];
            btes_Iovec I;

            I.setBuffer(buf, 1);

            btes_Iovec C(I);

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
            btes_Ovec I;

            I.setBuffer(buf, 1);

            btes_Ovec C(I);

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
            btes_Iovec I;

            I.setBuffer(buf, 1);

            btes_Ovec C(I);

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
        // --------------------------------------------------------
        //  TESTING DEFAULT CONSTRUCTOR
        //
        // Testing:
        // btes_Iovec::Iovec()
        // btes_Ovec::Ovec()
        // btes_Iovec::setBuffer()
        // btes_Ovec::setBuffer()
        // btes_Iovec::buffer()
        // btes_Ovec::buffer()
        // btes_Iovec::length()
        // btes_Ovec::length()
        // --------------------------------------------------------

        if (verbose) cout << endl << "TESTING DEFAULT CONTRUCTOR" <<
                             endl << "--------------------------" << endl;

        char buf[1];
        btes_Iovec I;

        I.setBuffer(buf, 1);
        ASSERT(I.buffer() == buf);
        ASSERT(I.length() == 1);

        btes_Ovec O;

        O.setBuffer((const char *) buf, 1);
        ASSERT(O.buffer() == buf);
        ASSERT(O.length() == 1);
      } break;
      case 1: {
        // --------------------------------------------------------
        //  USAGE
        // Plan
        //   The usage case calls two different scatter/gather socket
        //   operations.  These have been replaced in this test driver
        //   with test functions which validate the data passed.
        // Testing
        // USAGE
        // --------------------------------------------------------
        char buf1[10];
        char buf2[4];
        char buf3[7];
        btes_Ovec vector[3];
        vector[0].setBuffer(buf1, 10);
        vector[1].setBuffer(buf2, 4);
        vector[2].setBuffer(buf3, 7);

#ifndef BSLS_PLATFORM__CMP_MSVC
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
