// btlmt_session.t.cpp                                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <btlmt_session.h>
#include <bslma_testallocator.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// We are testing a pure protocol class.  We need to verify that a concrete
// derived class compiles and links.  We create a sample derived class that
// provides a dummy implementation of the base class virtual methods.  We then
// verify that when a method is called through a base class instance pointer
// the appropriate method in the derived class instance is invoked.
//-----------------------------------------------------------------------------
// [ 1] ~btlmt::Session();
// [ 1] int start();
// [ 1] int stop();
// [ 1] AsyncChannel *channel() const;
//-----------------------------------------------------------------------------
// [ 1] PROTOCOL TEST - Make sure derived class compiles and links.

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
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

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define NL() cout << endl;                    // End of line
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define T_()  cout << '\t' << flush;          // Print tab w/o newline
#define L_ __LINE__                           // current Line number

//=============================================================================
//                  SUPPORT CLASSES AND FUNCTIONS USED FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//          USAGE example from header(with assert replaced with ASSERT)
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Implementing a concrete channel type
///- - - - - - - - - - - - - - - - - - - - - - - -

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

class TestSession : public btlmt::Session
    // This 'class' provides a test implementation of the 'btlmt::Session'
    // protocol.
{
   // DATA
   int *d_funcCode_p;                  // code of the function, held

   // NOT IMPLEMENTED
   TestSession(const btlmt::Session&);
   TestSession& operator=(const btlmt::Session&);

  public:
   // CREATORS
   TestSession(int *funcCode)
   : d_funcCode_p (funcCode) { ASSERT(d_funcCode_p); *d_funcCode_p = 0; };

   ~TestSession() { *d_funcCode_p = 1; };

   // MANIPULATORS
   int start() { *d_funcCode_p = 2; return 0;}

   int stop() { *d_funcCode_p = 3; return 0;}

   btlmt::AsyncChannel* channel() const
       { *d_funcCode_p = 4; return (btlmt::AsyncChannel *) 0;}
};

void* MyCallback(int, btlmt::Session *)
{
   return (void *) 0;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;
    bslma::TestAllocator testAllocator(veryVeryVerbose);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //
        // Plan:
        //   Implement a dummy protocol and verify that we can
        //   invoke appropriate functions through the base class.
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "BREATHING TEST." << bsl::endl
                               << "===============" << bsl::endl;

        if (veryVerbose) bsl::cout
         << "\tTesting 'btlmt::Session': protocol test." << bsl::endl;
        {
           int opCode = -1;

           TestSession  mX(&opCode);           ASSERT(0 == opCode);

           btlmt::Session& session = mX;

           session.start();                    ASSERT(2 == opCode);
           session.stop();                     ASSERT(3 == opCode);
           session.channel();                  ASSERT(4 == opCode);
        }

        {
           int opCode = -1;

           if (veryVerbose) bsl::cout
            << "\tTesting 'btlmt::Session': destructor test." << bsl::endl;

           btlmt::Session *session = new(testAllocator) TestSession(&opCode);
           ASSERT(0 == opCode);

           testAllocator.deleteObjectRaw(session);
           ASSERT(1 == opCode);
        }

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
