// btlsc_channel.t.cpp                                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlsc_channel.h>

#include <btlsc_flag.h>          // for testing only

#include <bdls_testutil.h>

#include <bsls_protocoltest.h>

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
// [ 1] virtual ~Channel();
//
// MANIPULATORS
// [ 1] virtual int read(char *, int, int = 0) = 0;
// [ 1] virtual int read(int *, char *, int, int = 0) = 0;
// [ 1] virtual int readv(const btls::Iovec *, int, int = 0) = 0;
// [ 1] virtual int readv(int *, const btls::Iovec *, int, int = 0) = 0;
// [ 1] virtual int readRaw(char *, int, int = 0) = 0;
// [ 1] virtual int readRaw(int *, char *, int, int = 0) = 0;
// [ 1] virtual int readvRaw(const btls::Iovec *, int, int = 0) = 0;
// [ 1] virtual int readvRaw(int *, const btls::Iovec *, int, int=0) = 0;
// [ 1] virtual int bufferedRead(const char **, int, int = 0) = 0;
// [ 1] virtual int bufferedRead(int *, const char **, int, int = 0) = 0;
// [ 1] virtual int bufferedReadRaw(const char **, int, int = 0) = 0;
// [ 1] virtual int bufferedReadRaw(int *, const char **, int, int=0) = 0;
// [ 1] virtual int write(const char *, int, int= 0) = 0;
// [ 1] virtual int write(int *, const char *, int, int = 0) = 0;
// [ 1] virtual int writeRaw(const char *, int, int = 0) = 0;
// [ 1] virtual int writeRaw(int *, const char *, int, int = 0) = 0;
// [ 1] virtual int writev(const btls::Ovec *, int, int = 0) = 0;
// [ 1] virtual int writev(const btls::Iovec *, int, int = 0) = 0;
// [ 1] virtual int writev(int *, const btls::Ovec *, int, int = 0) = 0;
// [ 1] virtual int writev(int *, const btls::Iovec *, int, int = 0) = 0;
// [ 1] virtual int writevRaw(const btls::Ovec *, int, int = 0) = 0;
// [ 1] virtual int writevRaw(const btls::Iovec *, int, int = 0) = 0;
// [ 1] virtual int writevRaw(int *, const btls::Ovec *, int, int=0) = 0;
// [ 1] virtual int writevRaw(int *, const btls::Iovec *, int, int=0) = 0;
// [ 1] virtual void invalidate() = 0;
//
// ACCESSORS
// [ 1] virtual int isInvalid() const = 0;
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

typedef btlsc::Channel ProtocolClass;

struct ProtocolClassTestImp : bsls::ProtocolTestImp<ProtocolClass> {
    int read(char *, int, int = 0)                       { return markDone(); }

    int read(int *, char *, int, int = 0)                { return markDone(); }

    int readv(const btls::Iovec *, int, int = 0)         { return markDone(); }

    int readv(int *, const btls::Iovec *, int, int = 0)  { return markDone(); }

    int readRaw(char *, int, int = 0)                    { return markDone(); }

    int readRaw(int *, char *, int, int = 0)             { return markDone(); }

    int readvRaw(const btls::Iovec *, int, int = 0)      { return markDone(); }

    int readvRaw(int *, const btls::Iovec *, int, int = 0)
                                                         { return markDone(); }

    int bufferedRead(const char **, int, int = 0)        { return markDone(); }

    int bufferedRead(int *, const char **, int, int = 0) { return markDone(); }

    int bufferedReadRaw(const char **, int, int = 0)     { return markDone(); }

    int bufferedReadRaw(int *, const char **, int, int = 0)
                                                         { return markDone(); }

    int write(const char *, int, int= 0)                 { return markDone(); }

    int write(int *, const char *, int, int = 0)         { return markDone(); }

    int writeRaw(const char *, int, int = 0)             { return markDone(); }

    int writeRaw(int *, const char *, int, int = 0)      { return markDone(); }

    int writev(const btls::Ovec *, int, int = 0)         { return markDone(); }

    int writev(const btls::Iovec *, int, int = 0)        { return markDone(); }

    int writev(int *, const btls::Ovec *, int, int = 0)  { return markDone(); }

    int writev(int *, const btls::Iovec *, int, int = 0) { return markDone(); }

    int writevRaw(const btls::Ovec *, int, int = 0)      { return markDone(); }

    int writevRaw(const btls::Iovec *, int, int = 0)     { return markDone(); }

    int writevRaw(int *, const btls::Ovec *, int, int = 0)
                                                         { return markDone(); }

    int writevRaw(int *, const btls::Iovec *, int, int = 0)
                                                         { return markDone(); }

    void invalidate()                                    {        markDone(); }

    int isInvalid() const                                { return markDone(); }
};

}  // close unnamed namespace

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// The 'btlsc' style of channel interface is used to transmit sequences of
// specified size across some concrete channel implementation.  In this example
// we demonstrate how to implement a remote procedure call (RPC) to a factorial
// function taking an 'int' and returning a 'double'.  For simplicity, we will
// assume that both the 'int' and 'double' formats are binary compatible across
// client and server platforms:
//..
    double factorial(int number)
        // Return the factorial of the specified integral 'number' as a value
        // of type 'double'.  The behavior is undefined unless '0 <= number'.
        // Note that this helper function is provided for the server to
        // calculate the factorial value.
    {
        if (0 == number) {
            return 1;                                                 // RETURN
        }
        else {
            return number * factorial(number - 1);                    // RETURN
        }
    }

    int factorialClient(double *result, int input, btlsc::Channel *channel)
        // Load into the specified 'result' the factorial of the specified
        // 'input' using the specified 'channel' (which is assumed to be
        // connected to an appropriate factorial service).  Return 0 on
        // success, and -1, with no effect on 'result', on error.  The
        // behavior is undefined unless '0 <= input'.
    {
        ASSERT(0 <= input);

        enum {
            ERROR_STATUS   = -1,
            SUCCESS_STATUS = 0
        };
        int numBytes = sizeof input;
        int writeStatus = channel->write((const char *)&input, numBytes);
        ASSERT(0 != writeStatus);

        if (writeStatus != numBytes) {
            return ERROR_STATUS;                                      // RETURN
        }

        int readStatus = channel->read((char *)result, sizeof *result);
        if (readStatus != sizeof *result) {
            return ERROR_STATUS;                                      // RETURN
        }

        return SUCCESS_STATUS;                                        // RETURN
    }

    int factorialServer(btlsc::Channel *channel)
        // Repeatedly read integer sequences from the specified 'channel'.
        // When a read succeeds, interpret the byte sequence as an integer
        // value in host-byte order.  Return -1 if that value is negative.
        // Otherwise, calculate the factorial of the (non-negative) integer and
        // write back the result to 'channel' as a sequence of bytes
        // representing a 'double' in the host's native format.  Return a
        // negative value if any write operation doesn't succeed (refer to the
        // following 'enum' values for specific errors).  Note that this
        // implementation is just to show how a channel could be used; there is
        // much room to improve.
    {
        enum {
            SUCCESS            =  0,
            INVALID_INPUT      = -1,
            ERROR_READ         = -2,
            ERROR_WRITE        = -3,
            ERROR_INTERRUPTED  = -4,
            ERROR_UNCLASSIFIED = -5
        };

        while (1) {
            int input, augStatus;
            int readStatus = channel->read(&augStatus,
                                           (char *)&input,
                                           sizeof input);
            if (readStatus < 0) {
                return ERROR_READ;                                    // RETURN
            }
            else if (readStatus != sizeof input) {
                if (augStatus > 0) {
                    return ERROR_INTERRUPTED;                         // RETURN
                }
                return ERROR_UNCLASSIFIED;                            // RETURN
            }

            if (input < 0) {
                return INVALID_INPUT;                                 // RETURN
            }

            double result = factorial(input);
            augStatus = 0;
            int writeStatus = channel->write(&augStatus,
                                             (const char *)&result,
                                             sizeof result);

            if (writeStatus < 0) {
                return ERROR_WRITE;                                   // RETURN
            }
            else if (writeStatus != sizeof input){
                if (augStatus > 0) {
                    return ERROR_INTERRUPTED;                         // RETURN
                }
                return ERROR_UNCLASSIFIED;                            // RETURN
            }
        }
    }
//..

class MyChannel : public ProtocolClass {

  public:
    int read(char *, int, int = 0)                          { return -1; }

    int read(int *, char *, int, int = 0)                   { return -1; }

    int readv(const btls::Iovec *, int, int = 0)            { return -1; }

    int readv(int *, const btls::Iovec *, int, int = 0)     { return -1; }

    int readRaw(char *, int, int = 0)                       { return -1; }

    int readRaw(int *, char *, int, int = 0)                { return -1; }

    int readvRaw(const btls::Iovec *, int, int = 0)         { return -1; }

    int readvRaw(int *, const btls::Iovec *, int, int = 0)  { return -1; }

    int bufferedRead(const char **, int, int = 0)           { return -1; }

    int bufferedRead(int *, const char **, int, int = 0)    { return -1; }

    int bufferedReadRaw(const char **, int, int = 0)        { return -1; }

    int bufferedReadRaw(int *, const char **, int, int = 0) { return -1; }

    int write(const char *, int, int= 0)                    { return -1; }

    int write(int *, const char *, int, int = 0)            { return -1; }

    int writeRaw(const char *, int, int = 0)                { return -1; }

    int writeRaw(int *, const char *, int, int = 0)         { return -1; }

    int writev(const btls::Ovec *, int, int = 0)            { return -1; }

    int writev(const btls::Iovec *, int, int = 0)           { return -1; }

    int writev(int *, const btls::Ovec *, int, int = 0)     { return -1; }

    int writev(int *, const btls::Iovec *, int, int = 0)    { return -1; }

    int writevRaw(const btls::Ovec *, int, int = 0)         { return -1; }

    int writevRaw(const btls::Iovec *, int, int = 0)        { return -1; }

    int writevRaw(int *, const btls::Ovec *, int, int = 0)  { return -1; }

    int writevRaw(int *, const btls::Iovec *, int, int = 0) { return -1; }

    void invalidate()                                       {            }

    int isInvalid() const                                   { return -1; }
};

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

        {
            if (veryVerbose) {
                cout << "Usage example as the 'factorialClient'." << endl
                     << "=======================================" << endl;
            }

            MyChannel myChannel;
            int       input  = 5;
            double    result = 0;

            int ret = factorialClient(&result, input, &myChannel);
            ASSERT(0 > ret);
        }

        {
            if (veryVerbose) {
                cout << "Usage example as the 'factorialServer'." << endl
                     << "=======================================" << endl;
            }

            MyChannel myChannel;

            int ret = factorialServer(&myChannel);
            ASSERT(0 > ret);
        }
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
        //   virtual ~Channel();
        //   virtual int read(char *, int, int = 0) = 0;
        //   virtual int read(int *, char *, int, int = 0) = 0;
        //   virtual int readv(const btls::Iovec *, int, int = 0) = 0;
        //   virtual int readv(int *, const btls::Iovec *, int, int = 0) = 0;
        //   virtual int readRaw(char *, int, int = 0) = 0;
        //   virtual int readRaw(int *, char *, int, int = 0) = 0;
        //   virtual int readvRaw(const btls::Iovec *, int, int = 0) = 0;
        //   virtual int readvRaw(int *, const btls::Iovec *, int, int=0) = 0;
        //   virtual int bufferedRead(const char **, int, int = 0) = 0;
        //   virtual int bufferedRead(int *, const char **, int, int = 0) = 0;
        //   virtual int bufferedReadRaw(const char **, int, int = 0) = 0;
        //   virtual int bufferedReadRaw(int *, const char **, int, int=0) = 0;
        //   virtual int write(const char *, int, int= 0) = 0;
        //   virtual int write(int *, const char *, int, int = 0) = 0;
        //   virtual int writeRaw(const char *, int, int = 0) = 0;
        //   virtual int writeRaw(int *, const char *, int, int = 0) = 0;
        //   virtual int writev(const btls::Ovec *, int, int = 0) = 0;
        //   virtual int writev(const btls::Iovec *, int, int = 0) = 0;
        //   virtual int writev(int *, const btls::Ovec *, int, int = 0) = 0;
        //   virtual int writev(int *, const btls::Iovec *, int, int = 0) = 0;
        //   virtual int writevRaw(const btls::Ovec *, int, int = 0) = 0;
        //   virtual int writevRaw(const btls::Iovec *, int, int = 0) = 0;
        //   virtual int writevRaw(int *, const btls::Ovec *, int, int=0) = 0;
        //   virtual int writevRaw(int *, const btls::Iovec *, int, int=0) = 0;
        //   virtual void invalidate() = 0;
        //   virtual int isInvalid() const = 0;
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

        BSLS_PROTOCOLTEST_ASSERT(testObj, read(0, 0, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, read(0, 0, 0, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, readv(pIovec, 0, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, readv(0, pIovec, 0, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, readRaw(0, 0, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, readRaw(0, 0, 0, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, readvRaw(pIovec, 0, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, readvRaw(0, pIovec, 0, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, bufferedRead(0, 0, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, bufferedRead(0, 0, 0, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, bufferedReadRaw(0, 0, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, bufferedReadRaw(0, 0, 0, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, write(0, 0, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, write(0, 0, 0, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, writeRaw(0, 0, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, writeRaw(0, 0, 0, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, writev(pOvec, 0, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, writev(pIovec, 0, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, writev(0, pOvec, 0, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, writev(0, pIovec, 0, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, writevRaw(pOvec, 0, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, writevRaw(pIovec, 0, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, writevRaw(0, pOvec, 0, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, writevRaw(0, pIovec, 0, 0));

        BSLS_PROTOCOLTEST_ASSERT(testObj, invalidate());

        BSLS_PROTOCOLTEST_ASSERT(testObj, isInvalid());

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
