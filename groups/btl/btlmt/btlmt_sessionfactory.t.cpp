// btlmt_sessionfactory.t.cpp                                         -*-C++-*-

#include <btlmt_sessionfactory.h>
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
// [ 1] ~btlmt::SessionFactory();
// [ 1] void allocate(bsl::shared_ptr<AsyncChannel>& channel, callback);
// [ 1] void allocate(AsyncChannel *channel, callback);
// [ 1] void deallocate(Session *session) = 0;
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
//

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

class TestSessionFactory : public btlmt::SessionFactory
    // This 'class' provides a test implementation of the
    // 'btlmt::SessionFactory' protocol.  This 'class' *does not* override the
    // 'allocate' method that takes a 'btlmt::AsyncChannel' by a
    // 'bsl::shared_ptr'.
{
   // DATA
   int *d_funcCode_p;                  // code of the function, held

   // NOT IMPLEMENTED
   TestSessionFactory(const btlmt::SessionFactory&);
   TestSessionFactory& operator=(const btlmt::SessionFactory&);

  public:
    // CREATORS
    TestSessionFactory(int *funcCode)
    : d_funcCode_p (funcCode) { *d_funcCode_p = 0; }

    ~TestSessionFactory() { *d_funcCode_p = 1; }

    // MANIPULATORS
    void allocate(btlmt::AsyncChannel                    *,
                  const btlmt::SessionFactory::Callback&  )
    {
       *d_funcCode_p = 2;
    }

    void deallocate(btlmt::Session *)
    {
       *d_funcCode_p = 3;
    }
};

class TestSessionFactoryWithSharedAsyncChannel : public btlmt::SessionFactory
    // This 'class' provides a test implementation of the
    // 'btlmt::SessionFactory' protocol.  This 'class' overrides all the
    // methods of the underlying protocol including the 'allocate' method that
    // takes a 'btlmt::AsyncChannel' by a 'bsl::shared_ptr'.
{
   // DATA
   int *d_funcCode_p;  // code of the function, held

   // NOT IMPLEMENTED
   TestSessionFactoryWithSharedAsyncChannel(const btlmt::SessionFactory&);
   TestSessionFactoryWithSharedAsyncChannel& operator=(
                                                 const btlmt::SessionFactory&);

  public:
    // CREATORS
    TestSessionFactoryWithSharedAsyncChannel(int *funcCode)
    : d_funcCode_p (funcCode) { *d_funcCode_p = 0; }

    ~TestSessionFactoryWithSharedAsyncChannel() { *d_funcCode_p = 1; }

    // MANIPULATORS
    void allocate(btlmt::AsyncChannel                    *,
                  const btlmt::SessionFactory::Callback&  )
    {
       *d_funcCode_p = 2;
    }

    void allocate(const bsl::shared_ptr<btlmt::AsyncChannel>& ,
                  const btlmt::SessionFactory::Callback&      )
    {
       *d_funcCode_p = 4;
    }

    void deallocate(btlmt::Session *)
    {
       *d_funcCode_p = 3;
    }
};

void* MyCallback(int , btlmt::Session*)
{
   return (void*)0;
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

        {
           if (veryVerbose) bsl::cout
            << "\tTesting 'btlmt::SessionFactory': protocol test."<< bsl::endl;

           int opCode = -1;

           TestSessionFactory mX1(&opCode);    ASSERT(0 == opCode);

           btlmt::SessionFactory& factory = mX1;

           // must intialize these poiinters, as copying an uninitialized value
           // as a function argument is undefined behaviour, even if it is not
           // used within the function.  This is a real problem on Windows in
           // debug builds.
           btlmt::AsyncChannel *channel = 0;
           btlmt::Session *session = 0;

           factory.allocate(channel, &MyCallback);     ASSERT(2 == opCode);
           factory.deallocate(session);                ASSERT(3 == opCode);
        }

        {
            if (veryVerbose) bsl::cout << "\tTesting 'btlmt::SessionFactory': "
                                       << "Derived imp does not override "
                                       << "shared async channel method."
                                       << bsl::endl;

           int opCode = -1;

           TestSessionFactory mX1(&opCode);    ASSERT(0 == opCode);

           btlmt::SessionFactory& factory = mX1;

           // must intialize these poiinters, as copying an uninitialized value
           // as a function argument is undefined behaviour, even if it is not
           // used within the function.  This is a real problem on Windows in
           // debug builds.
           bsl::shared_ptr<btlmt::AsyncChannel> spChannel;
           btlmt::Session *session = 0;

           factory.allocate(spChannel, &MyCallback);   ASSERT(2 == opCode);
           factory.deallocate(session);                ASSERT(3 == opCode);
        }

        {
            if (veryVerbose) bsl::cout << "\tTesting 'btlmt::SessionFactory': "
                                       << "Derived imp overrides "
                                       << "shared async channel method."
                                       << bsl::endl;

           int opCode = -1;

           TestSessionFactoryWithSharedAsyncChannel mX1(&opCode);
           ASSERT(0 == opCode);

           btlmt::SessionFactory& factory = mX1;

           // must intialize these poiinters, as copying an uninitialized value
           // as a function argument is undefined behaviour, even if it is not
           // used within the function.  This is a real problem on Windows in
           // debug builds.
           bsl::shared_ptr<btlmt::AsyncChannel> spChannel;
           btlmt::Session *session = 0;

           factory.allocate(spChannel.get(), &MyCallback); ASSERT(2 == opCode);
           factory.allocate(spChannel, &MyCallback);       ASSERT(4 == opCode);
           factory.deallocate(session);                    ASSERT(3 == opCode);
        }

        {
           int opCode = -1;

           if (veryVerbose) bsl::cout
            <<"\tTesting 'btlmt::SessionFactory': destructor test."<<bsl::endl;

           btlmt::SessionFactory *factory =
            new(testAllocator) TestSessionFactory(&opCode);
           ASSERT(0 == opCode);

           testAllocator.deleteObjectRaw(factory);
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
