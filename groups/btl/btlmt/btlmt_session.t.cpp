// btlmt_session.t.cpp          -*-C++-*-

#include <btlmt_session.h>
#include <bslma_testallocator.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
// Create concrete implementations of the 'btlmt::Session', and
// btlmt::SessionFactory protocols and instantiate them.
//
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This test driver simply verifies that a concrete instance of 'btlmt::Session'
// and btlmt::SessionFactory can be implement and instantiated.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// [1 ] BREATHING TEST
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
    void allocate(btlmt::AsyncChannel                    *channel,
                  const btlmt::SessionFactory::Callback&  callback)
    {
       *d_funcCode_p = 2;
    }

    void deallocate(btlmt::Session *session)
    {
       *d_funcCode_p = 3;
    }
};

class TestSessionFactoryWithSharedAsyncChannel : public btlmt::SessionFactory
    // This 'class' provides a test implementation of the
    // 'btlmt::SessionFactory' protocol.  This 'class' overrides all the methods
    // of the underlying protocol including the 'allocate' method that takes a
    // 'btlmt::AsyncChannel' by a 'bsl::shared_ptr'.
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
    void allocate(btlmt::AsyncChannel                    *channel,
                  const btlmt::SessionFactory::Callback&  callback)
    {
       *d_funcCode_p = 2;
    }

    void allocate(const bsl::shared_ptr<btlmt::AsyncChannel>& channel,
                  const btlmt::SessionFactory::Callback&      callback)
    {
       *d_funcCode_p = 4;
    }

    void deallocate(btlmt::Session *session)
    {
       *d_funcCode_p = 3;
    }
};

void* MyCallback(int session_id, btlmt::Session* session)
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
