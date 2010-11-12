// btesc_timedchannelallocator.t.cpp   -*-C++-*-

#include <btesc_timedchannelallocator.h>

#include <btesc_channel.h>
#include <btesc_timedchannel.h>
#include <btesc_flag.h>

#include <bdet_timeinterval.h>
#include <bdetu_systemtime.h>
#include <bdet_timeinterval.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
// We are testing a pure protocol class.  We need to verify that a concrete
// derived class (1) can be invoked and (2) the method calls are forwarded
// when invoked through the protocol.  We create a sample derived class that
// provides a dummy implementation of the base class virtual methods.  We then
// verify that when a method is called through a base class instance pointer
// the appropriate method in the derived class instance is invoked.
//-----------------------------------------------------------------------------
// virtual ~btesc_TimedChannelAllocator();
// virtual btesc_Channel *allocate();
// virtual btesc_Channel *timedAllocate();
// virtual btesc_TimedChannel *allocateTimed();
// virtual btesc_TimedChannel *timedAllocateTimed();
// virtual void deallocate();
// virtual void invalidate();
// virtual int isInvalid();
//-----------------------------------------------------------------------------
// [ 1] PROTOCOL TEST - Make sure derived class compiles and links.
//=============================================================================

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
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
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define PS(X) cout << #X " = \n" << (X) << endl; // Print identifier and value.
#define T_()  cout << "\t" << flush;          // Print a tab (w/o newline)

//============================================================================
//                      CONCRETE DERIVED TYPE
//----------------------------------------------------------------------------
class MyAllocator : public btesc_TimedChannelAllocator {
  // Test class used to verify protocol.

    int d_fun;  // holds code describing (non-const) function:
                //   1 allocate()
                //   2 timedAllocate()
                //   3 allocateTimed()
                //   4 timedAllocateTimed()
                //   5 deallocate()
                //   6 invalidate()
                //   7 isInvalid()

    int d_flags;     // last value of flags passed

    int d_valid;     // Is this object instance valid?

  public:
    MyAllocator() : d_fun(0),   d_flags(0), d_valid(1) { }
    ~MyAllocator() { }

    virtual btesc_Channel *allocate(int *status, int flags = 0)
    { d_fun = 1; d_flags = flags; return 0; }

    virtual btesc_Channel *timedAllocate(int                      *status,
                                         const bdet_TimeInterval&  timeout,
                                         int                       flags = 0)
    { d_fun = 2; d_flags = flags; return 0; }

    virtual btesc_TimedChannel *allocateTimed(int *status, int flags = 0)
    { d_fun = 3; d_flags = flags; return 0; }

    virtual btesc_TimedChannel *timedAllocateTimed(
                                      int                      *status,
                                      const bdet_TimeInterval&  timeout,
                                      int                       flags = 0)
    { d_fun = 4; d_flags = flags; return 0; }

    virtual void deallocate(btesc_Channel *channel)
    { d_fun = 5;}

    virtual void invalidate()
    { d_fun = 6; d_valid = 0; }

    // = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

    virtual int isInvalid() const
    {
        MyAllocator *const tmp = const_cast<MyAllocator* > (this);
        tmp-> d_fun = 7;
        return !d_valid;
    }

    // non-virtual functions for testing
    int fun()   const { return d_fun; }
    int flags() const { return d_flags; }
};

//=============================================================================
//                      MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // USAGE TEST:
        //   This test is really just to make sure the syntax is correct.
        // Testing:
        //   USAGE TEST - Make sure main usage example compiles and works.
        //                TBD
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE TEST" << endl
                                  << "==========" << endl;

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // PROTOCOL TEST:
        //   All we need to do is make sure that a concrete subclass of the
        //   'btesc_Channel' class compiles and links when all virtual
        //   functions are defined.
        // Testing:
        //   virtual ~btesc_TimedChannelAllocator(...);
        //   virtual btesc_Channel *allocate();
        //   virtual btesc_Channel *timedAllocate();
        //   virtual btesc_TimedChannel *allocateTimed();
        //   virtual btesc_TimedChannel *timedAllocateTimed();
        //   virtual void deallocate();
        //   virtual void invalidate();
        //   virtual int isInvalid(...)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "PROTOCOL TEST" << endl
                                  << "=============" << endl;

        // Ensure that an instance of the derived class can be created.
        MyAllocator myC;
        btesc_TimedChannelAllocator& c = myC;
        ASSERT(0 == myC.isInvalid());
        bdet_TimeInterval timeout(bdetu_SystemTime::now());

        if (verbose) cout << "\nTesting protocol interface"
                          << "\n==========================" << endl;
        {
            // Invoke different methods via the base class reference and check
            // if the derived class method is invoked.

            // Flags to indicate that async interrupts are allowed.
            int myFlags = btesc_Flag::BTESC_ASYNC_INTERRUPT;
            int status = 0;
            ASSERT(0 == myC.flags());

            c.allocate(&status);
            ASSERT(1 == myC.fun()); ASSERT(0 == myC.flags());
            c.allocate(&status, myFlags);
            ASSERT(1 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.timedAllocate(&status, timeout);
            ASSERT(2 == myC.fun()); ASSERT(0 == myC.flags());
            c.timedAllocate(&status, timeout, myFlags);
            ASSERT(2 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.allocateTimed(&status);
            ASSERT(3 == myC.fun()); ASSERT(0 == myC.flags());
            c.allocateTimed(&status, myFlags);
            ASSERT(3 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.timedAllocateTimed(&status, timeout);
            ASSERT(4 == myC.fun()); ASSERT(0 == myC.flags());
            c.timedAllocateTimed(&status, timeout, myFlags);
            ASSERT(4 == myC.fun()); ASSERT(myFlags == myC.flags());

            c.deallocate(0);        ASSERT(5 == myC.fun());

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

            ASSERT(0 == myC.isInvalid()); // non-virtual test method
            c.invalidate();               ASSERT(6 == myC.fun());
            ASSERT(1 == c.isInvalid());   ASSERT(7 == myC.fun());
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
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
