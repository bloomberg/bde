// btlsc_timedcbchannel.t.cpp                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlsc_timedcbchannel.h>

#include <btlsc_flag.h>          // for testing only

#include <bdlf_bind.h>
#include <bdlf_placeholder.h>

#include <bdls_testutil.h>

#include <bsls_protocoltest.h>
#include <bsls_systemclocktype.h>
#include <bsls_systemtime.h>
#include <bsls_timeinterval.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test defines a protocol class the purpose of which is to
// provide an interface for loading calendars.
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o It is possible to create a concrete implementation the protocol.
//-----------------------------------------------------------------------------
// CREATORS
// [ 1] virtual ~TimedCbChannel();
//
// MANIPULATORS
// [ 1] virtual int read(char *, int, const RdCb&, int = 0) = 0;
// [ 1] virtual int timedRead(char *, int, TI, const RdCb&, int = 0) = 0;
// [ 1] virtual int readRaw(char *, int, const RdCb&, int = 0) = 0;
// [ 1] virtual int timedReadRaw(char *, int, TI, RdCb, int = 0) = 0;
// [ 1] virtual int readvRaw(const Iovec *, int, const RdCb&, int=0) = 0;
// [ 1] virtual int timedReadvRaw(CIovec *, int, TI, RdCb, int = 0) = 0;
// [ 1] virtual int bufferedRead(int, const BufRdCb&, int = 0) = 0;
// [ 1] virtual int timedBufferedRead(int, TI, BufRdCb, int = 0) = 0;
// [ 1] virtual int bufferedReadRaw(int, const BufRdCb&, int = 0) = 0;
// [ 1] virtual int timedBufferedReadRaw(int, TI, BufRdCb, int = 0) = 0;
// [ 1] virtual int write(const char *, int, const WrCb&, int = 0) = 0;
// [ 1] virtual int timedWrite(const char *, int, TI, WrCb, int = 0) = 0;
// [ 1] virtual int writeRaw(const char *, int, WrCb, int = 0) = 0;
// [ 1] virtual int timedWriteRaw(const char *, int, TI, WrCb, int=0) = 0;
// [ 1] virtual int writevRaw(const Iovec *, int, WrCb, int = 0) = 0;
// [ 1] virtual int writevRaw(const Ovec *, int, WrCb, int = 0) = 0;
// [ 1] virtual int timedWritevRaw(CIovec *, int, TI, WrCb, int = 0) = 0;
// [ 1] virtual int timedWritevRaw(COvec *, int, TI, WrCb, int = 0) = 0;
// [ 1] virtual int bufferedWrite(const char *, int, WrCb, int = 0) = 0;
// [ 1] virtual int timedBufferedWrite(Cchar *, int, TI, WrCb, int=0) = 0;
// [ 1] virtual int bufferedWritev(const Iovec *, int, WrCb, int = 0) = 0;
// [ 1] virtual int bufferedWritev(const Ovec *, int, WrCb, int = 0) = 0;
// [ 1] virtual int timedBufferedWritev(CIovec *, int, TI, WrCb, int=0)=0;
// [ 1] virtual int timedBufferedWritev(COvec *, int, TI, WrCb, int=0)=0;
// [ 1] virtual void cancelAll() = 0;
// [ 1] virtual void cancelRead() = 0;
// [ 1] virtual void cancelWrite() = 0;
// [ 1] virtual void invalidate() = 0;
// [ 1] virtual void invalidateRead() = 0;
// [ 1] virtual void invalidateWrite() = 0;
//
// ACCESSORS
// [ 1] virtual int isInvalidRead() const = 0;
// [ 1] virtual int isInvalidWrite() const = 0;
// [ 1] virtual int numPendingReadOperations() const = 0;
// [ 1] virtual int numPendingWriteOperations() const = 0;
//-----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE

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

#define ASSERT       BDLS_TESTUTIL_ASSERT
#define ASSERTV      BDLS_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BDLS_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BDLS_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BDLS_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BDLS_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BDLS_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BDLS_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BDLS_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BDLS_TESTUTIL_LOOP6_ASSERT

#define Q            BDLS_TESTUTIL_Q   // Quote identifier literally.
#define P            BDLS_TESTUTIL_P   // Print identifier and value.
#define P_           BDLS_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BDLS_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BDLS_TESTUTIL_L_  // current Line number

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

namespace {

typedef btlsc::TimedCbChannel ProtocolClass;
typedef ProtocolClass         PC;

struct ProtocolClassTestImp : bsls::ProtocolTestImp<ProtocolClass> {
    int read(char *, int, const PC::ReadCallback&, int = 0)
                                                         { return markDone(); }

    int timedRead(char *,
                  int,
                  const bsls::TimeInterval&,
                  const PC::ReadCallback&,
                  int = 0)                               { return markDone(); }

    int readRaw(char *, int, const PC::ReadCallback&, int = 0)
                                                         { return markDone(); }

    int timedReadRaw(char *,
                     int,
                     const bsls::TimeInterval&,
                     const PC::ReadCallback&,
                     int = 0)                            { return markDone(); }

    int readvRaw(const btls::Iovec *, int, const PC::ReadCallback&, int = 0)
                                                         { return markDone(); }

    int timedReadvRaw(const btls::Iovec *,
                      int,
                      const bsls::TimeInterval&,
                      const PC::ReadCallback&,
                      int = 0)                           { return markDone(); }

    int bufferedRead(int, const PC::BufferedReadCallback&, int = 0)
                                                         { return markDone(); }

    int timedBufferedRead(int,
                          const bsls::TimeInterval&,
                          const PC::BufferedReadCallback&,
                          int = 0)                       { return markDone(); }

    int bufferedReadRaw(int, const PC::BufferedReadCallback&, int = 0)
                                                         { return markDone(); }

    int timedBufferedReadRaw(int,
                             const bsls::TimeInterval&,
                             const PC::BufferedReadCallback&,
                             int = 0)                    { return markDone(); }

    int write(const char *, int, const PC::WriteCallback&, int = 0)
                                                         { return markDone(); }

    int timedWrite(const char *,
                   int,
                   const bsls::TimeInterval&,
                   const PC::WriteCallback&,
                   int = 0)                              { return markDone(); }

    int writeRaw(const char *, int, const PC::WriteCallback&, int = 0)
                                                         { return markDone(); }

    int timedWriteRaw(const char *,
                      int,
                      const bsls::TimeInterval&,
                      const PC::WriteCallback&,
                      int = 0)                           { return markDone(); }

    int writevRaw(const btls::Iovec *, int, const PC::WriteCallback&, int = 0)
                                                         { return markDone(); }

    int writevRaw(const btls::Ovec *, int, const PC::WriteCallback&, int = 0)
                                                         { return markDone(); }

    int timedWritevRaw(const btls::Iovec *,
                       int,
                       const bsls::TimeInterval&,
                       const PC::WriteCallback&,
                       int = 0)                          { return markDone(); }

    int timedWritevRaw(const btls::Ovec *,
                       int,
                       const bsls::TimeInterval&,
                       const PC::WriteCallback&,
                       int = 0)                          { return markDone(); }

    int bufferedWrite(const char *, int, const PC::WriteCallback&, int = 0)
                                                         { return markDone(); }

    int timedBufferedWrite(const char *,
                           int,
                           const bsls::TimeInterval&,
                           const PC::WriteCallback&,
                           int = 0)                      { return markDone(); }

    int bufferedWritev(const btls::Iovec *,
                       int,
                       const PC::WriteCallback&,
                       int = 0)                          { return markDone(); }

    int bufferedWritev(const btls::Ovec *,
                       int,
                       const PC::WriteCallback&,
                       int = 0)                          { return markDone(); }

    int timedBufferedWritev(const btls::Iovec *,
                            int,
                            const bsls::TimeInterval&,
                            const PC::WriteCallback&,
                            int = 0)                     { return markDone(); }

    int timedBufferedWritev(const btls::Ovec *,
                            int,
                            const bsls::TimeInterval&,
                            const PC::WriteCallback&,
                            int = 0)                     { return markDone(); }

    void cancelAll()                                     {        markDone(); }

    void cancelRead()                                    {        markDone(); }

    void cancelWrite()                                   {        markDone(); }

    void invalidate()                                    {        markDone(); }

    void invalidateRead()                                {        markDone(); }

    void invalidateWrite()                               {        markDone(); }

    int isInvalidRead() const                            { return markDone(); }

    int isInvalidWrite() const                           { return markDone(); }

    int numPendingReadOperations() const                 { return markDone(); }

    int numPendingWriteOperations() const                { return markDone(); }
};

}  // close unnamed namespace

// Free functions used as callbacks.

void myRdCbFn(int, int) { }
void myBufRdCbFn(const char *, int, int) { }
void myWrCbFn(int, int) { }

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int         test = argc > 1 ? atoi(argv[1]) : 0;
    const bool     verbose = argc > 2;
    const bool veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // PROTOCOL TEST:
        //   Ensure this class is a properly defined protocol.
        //
        // Concerns:
        //: 1 The protocol is abstract: no objects of it can be created.
        //:
        //: 2 The protocol has no data members.
        //:
        //: 3 The protocol has a virtual destructor.
        //:
        //: 4 All methods of the protocol are pure virtual.
        //:
        //: 5 All methods of the protocol are publicly accessible.
        //
        // Plan:
        //: 1 Define a concrete derived implementation, 'ProtocolClassTestImp',
        //:   of the protocol.
        //:
        //: 2 Create an object of the 'bsls::ProtocolTest' class template
        //:   parameterized by 'ProtocolClassTestImp', and use it to verify
        //:   that:
        //:
        //:   1 The protocol is abstract. (C-1)
        //:
        //:   2 The protocol has no data members. (C-2)
        //:
        //:   3 The protocol has a virtual destructor. (C-3)
        //:
        //: 3 Use the 'BSLS_PROTOCOLTEST_ASSERT' macro to verify that
        //:   non-creator methods of the protocol are:
        //:
        //:   1 virtual, (C-4)
        //:
        //:   2 publicly accessible. (C-5)
        //
        // Testing:
        //   virtual ~TimedCbChannel();
        //   virtual int read(char *, int, const RdCb&, int = 0) = 0;
        //   virtual int timedRead(char *, int, TI, const RdCb&, int = 0) = 0;
        //   virtual int readRaw(char *, int, const RdCb&, int = 0) = 0;
        //   virtual int timedReadRaw(char *, int, TI, RdCb, int = 0) = 0;
        //   virtual int readvRaw(const Iovec *, int, const RdCb&, int=0) = 0;
        //   virtual int timedReadvRaw(CIovec *, int, TI, RdCb, int = 0) = 0;
        //   virtual int bufferedRead(int, const BufRdCb&, int = 0) = 0;
        //   virtual int timedBufferedRead(int, TI, BufRdCb, int = 0) = 0;
        //   virtual int bufferedReadRaw(int, const BufRdCb&, int = 0) = 0;
        //   virtual int timedBufferedReadRaw(int, TI, BufRdCb, int = 0) = 0;
        //   virtual int write(const char *, int, const WrCb&, int = 0) = 0;
        //   virtual int timedWrite(const char *, int, TI, WrCb, int = 0) = 0;
        //   virtual int writeRaw(const char *, int, WrCb, int = 0) = 0;
        //   virtual int timedWriteRaw(const char *, int, TI, WrCb, int=0) = 0;
        //   virtual int writevRaw(const Iovec *, int, WrCb, int = 0) = 0;
        //   virtual int writevRaw(const Ovec *, int, WrCb, int = 0) = 0;
        //   virtual int timedWritevRaw(CIovec *, int, TI, WrCb, int = 0) = 0;
        //   virtual int timedWritevRaw(COvec *, int, TI, WrCb, int = 0) = 0;
        //   virtual int bufferedWrite(const char *, int, WrCb, int = 0) = 0;
        //   virtual int timedBufferedWrite(Cchar *, int, TI, WrCb, int=0) = 0;
        //   virtual int bufferedWritev(const Iovec *, int, WrCb, int = 0) = 0;
        //   virtual int bufferedWritev(const Ovec *, int, WrCb, int = 0) = 0;
        //   virtual int timedBufferedWritev(CIovec *, int, TI, WrCb, int=0)=0;
        //   virtual int timedBufferedWritev(COvec *, int, TI, WrCb, int=0)=0;
        //   virtual void cancelAll() = 0;
        //   virtual void cancelRead() = 0;
        //   virtual void cancelWrite() = 0;
        //   virtual void invalidate() = 0;
        //   virtual void invalidateRead() = 0;
        //   virtual void invalidateWrite() = 0;
        //   virtual int isInvalidRead() const = 0;
        //   virtual int isInvalidWrite() const = 0;
        //   virtual int numPendingReadOperations() const = 0;
        //   virtual int numPendingWriteOperations() const = 0;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "PROTOCOL TEST" << endl
                                  << "=============" << endl;

        if (verbose) cout << "\nCreate a test object.\n";

        bsls::ProtocolTest<ProtocolClassTestImp> testObj(veryVerbose);

        if (verbose) cout << "\nVerify that the protocol is abstract.\n";

        ASSERT(testObj.testAbstract());

        if (verbose) cout << "\nVerify that there are no data members.\n";

        ASSERT(testObj.testNoDataMembers());

        if (verbose) cout << "\nVerify that the destructor is virtual.\n";

        ASSERT(testObj.testVirtualDestructor());

        if (verbose) cout << "\nVerify that methods are public and virtual.\n";

        const btls::Iovec *pIovec = 0;
        const btls::Ovec  *pOvec  = 0;

        const bsls::TimeInterval TI;

        BSLS_PROTOCOLTEST_ASSERT(testObj, read(0, 0, &myRdCbFn, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, timedRead(0, 0, TI, &myRdCbFn, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, readRaw(0, 0, &myRdCbFn, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, timedReadRaw(0, 0, TI, &myRdCbFn,
                                                       0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, readvRaw(pIovec, 0, &myRdCbFn, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, timedReadvRaw(pIovec, 0, TI,
                                                        &myRdCbFn, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, bufferedRead(0, &myBufRdCbFn, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, timedBufferedRead(0, TI,
                                                            &myBufRdCbFn, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, bufferedReadRaw(0, &myBufRdCbFn, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, timedBufferedReadRaw(0, TI,
                                                               &myBufRdCbFn,
                                                               0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, write(0, 0, &myWrCbFn, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, timedWrite(0, 0, TI, &myWrCbFn, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, writeRaw(0, 0, &myWrCbFn, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, timedWriteRaw(0, 0, TI, &myWrCbFn,
                                                        0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, writevRaw(pIovec, 0, &myWrCbFn, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, writevRaw(pOvec, 0, &myWrCbFn, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, timedWritevRaw(pIovec, 0, TI,
                                                         &myWrCbFn, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, timedWritevRaw(pOvec, 0, TI,
                                                         &myWrCbFn, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, bufferedWrite(0, 0, &myWrCbFn, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, timedBufferedWrite(0, 0, TI,
                                                             &myWrCbFn, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, bufferedWritev(pIovec, 0, &myWrCbFn,
                                                         0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, bufferedWritev(pOvec, 0, &myWrCbFn,
                                                         0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, timedBufferedWritev(pIovec, 0, TI,
                                                              &myWrCbFn, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, timedBufferedWritev(pOvec, 0, TI,
                                                              &myWrCbFn, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, cancelAll());

        BSLS_PROTOCOLTEST_ASSERT(testObj, cancelRead());

        BSLS_PROTOCOLTEST_ASSERT(testObj, cancelWrite());

        BSLS_PROTOCOLTEST_ASSERT(testObj, invalidate());

        BSLS_PROTOCOLTEST_ASSERT(testObj, invalidateRead());

        BSLS_PROTOCOLTEST_ASSERT(testObj, invalidateWrite());

        BSLS_PROTOCOLTEST_ASSERT(testObj, isInvalidRead());

        BSLS_PROTOCOLTEST_ASSERT(testObj, isInvalidWrite());

        BSLS_PROTOCOLTEST_ASSERT(testObj, numPendingReadOperations());

        BSLS_PROTOCOLTEST_ASSERT(testObj, numPendingWriteOperations());

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
