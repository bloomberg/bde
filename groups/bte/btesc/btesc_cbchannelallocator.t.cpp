// btesc_cbchannelallocator.t.cpp        -*-C++-*-

#include <btesc_cbchannelallocator.h>

#include <btesc_flag.h>
#include <bdet_timeinterval.h>

#include <bdef_function.h>

#include <bsl_iostream.h>
#include <bsl_c_stdlib.h>          // atoi
using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
// We are testing a pure protocol class.  We need to verify that a concrete
// derived class compiles and links.  We create a sample derived class that
// provides a dummy implementation of the base class methods.  We then
// verify that when a method is called through a base class instance pointer
// the appropriate method in the derived class instance is invoked.
//-----------------------------------------------------------------------------
// [ 1] ~btesc_CbChannelAllocator();
// [ 1] void *allocate(const Callback& callback, int flags = 0);
// [ 1] int timedAllocate(const Callback&          callback,
//                        const bdet_TimeInterval& timeout,
//                        int                      flags = 0);
// [ 1] int allocateTimed(const TimedCallback& timedCallback,
//                        int                  flags = 0);
// [ 1] timedAllocateTimed(const TimedCallback&     timedCallback,
//                         const bdet_TimeInterval& timeout,
//                         int                      flags = 0);
// [ 1] void deallocate(btes_CbChannel *channel);
// [ 1] void invalidate();
// [ 1] int isInvalid() const;
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

//=============================================================================
//                      CONCRETE DERIVED TYPE
//-----------------------------------------------------------------------------
class MyAllocator : public btesc_CbChannelAllocator {
  // Test class used to verify protocol.

    int d_fun;  // holds code describing function:
                //   + 1 allocate
                //   + 2 allocateTimed
                //   + 3 deallocate
                //   + 4 invalidate
                //   + 5 cancelAll
                //   + 6 isInvalid()

    int d_flags; // last value of flags passed

    int d_valid; // Is this object instance valid?

  public:
    MyAllocator() : d_fun(0), d_flags(0), d_valid(1) { }
    ~MyAllocator() { }

    int allocate(const Callback& callback, int flags = 0)
        { d_fun = 1; d_flags = flags; return 0; }

    int allocateTimed(const TimedCallback& timedCallback,
                      int                  flags = 0)
        { d_fun = 2; d_flags = flags; return 0; }

    void deallocate(btesc_CbChannel *channel)
        { d_fun = 3; }
    void invalidate()
        { d_fun = 4; d_valid = 0; }
    void cancelAll()
        { d_fun = 5; d_valid = 0; }

    //int isInvalid() const { return !d_valid; }

    // = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

    virtual int isInvalid() const
    {
        MyAllocator *const tmp = const_cast<MyAllocator* > (this);
        tmp-> d_fun = 6;
        return !d_valid;
    }

    // non-virtual functions for testing
    int fun() const { return d_fun; }
    int flags() const { return d_flags; }
    int valid() const { return d_valid; }
};

// Free functions passed in as callbacks.
void myCbFn(btesc_CbChannel *, int) { }
void myTimedCbFn(btesc_TimedCbChannel *, int) { }

//=============================================================================
//                      MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // USAGE TEST:
        //   This test is really just to make sure the syntax is correct.
        // Testing:
        //   USAGE TEST - Make sure main usage example compiles and works.
        //                TBD.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE TEST" << endl
                                  << "==========" << endl;

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // OPERATOR TEST:
        //   No operators are defined.
        // Testing:
        //   N/a.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "OPERATOR TEST" << endl
                                  << "=============" << endl;
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // STATIC MEMBER TEMPLATE METHOD TEST:
        //
        // Plan:
        //   No static member is defined.  N/A.
        //
        // Testing:
        //   N/A.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "STATIC MEMBER TEMPLATE METHOD" << endl
                                  << "=============" << endl;
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // PROTOCOL TEST:
        //   All we need to do is make sure that a concrete subclass of the
        //   'btesc_TimedCbChannelAllocator' class compiles and links when all
        //   functions are defined.
        // Testing:
        //   ~btesc_TimedCbChannelAllocator() { }
        //   int allocate(const Callback& callback, int flags = 0);
        //   int allocateTimed(const TimedCallback& timedCallback,
        //                     int                  flags = 0);
        //   void deallocate(btes_CbChannel *channel);
        //   void invalidate();
        //   void cancelAll();
        //   int isInvalid() const;
        //
        //   PROTOCOL TEST - Make sure derived class compiles and links.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "PROTOCOL TEST" << endl
                                  << "=============" << endl;

        // Ensure that an instance of the derived class can be created.
        MyAllocator myA;
        btesc_CbChannelAllocator& a = myA;
        ASSERT(1 == myA.valid()); ASSERT(0 == myA.isInvalid());

        if (verbose) cout << "\nTesting protocol interface" << endl;
        {
            // Invoke different methods via the base class reference and check
            // if the derived class method is invoked.

            // Create a dummy functor object.
            btesc_CbChannelAllocator::Callback myCb(&myCbFn);
            btesc_CbChannelAllocator::TimedCallback myTimedCb(&myTimedCbFn);

            // Create a dummy bdet_TimeInterval object.
            bdet_TimeInterval myTimeout;

            // Flags to indicate that async interrupts are allowed.
            int myFlags = btesc_Flag::BTESC_ASYNC_INTERRUPT;

            a.allocate(myCb);
            ASSERT(1 == myA.fun()); ASSERT(0 == myA.flags());
            a.allocate(myCb, myFlags);
            ASSERT(1 == myA.fun()); ASSERT(myFlags == myA.flags());

            a.allocateTimed(myTimedCb);
            ASSERT(2 == myA.fun()); ASSERT(0 == myA.flags());
            a.allocateTimed(myTimedCb, myFlags);
            ASSERT(2 == myA.fun()); ASSERT(myFlags == myA.flags());

            a.deallocate(0);
            ASSERT(3 == myA.fun());

            a.cancelAll();
            ASSERT(5 == myA.fun());

            a.invalidate();
            ASSERT(4 == myA.fun());         ASSERT(0 == myA.valid());
            ASSERT(1 == myA.isInvalid());   ASSERT(6 == myA.fun());
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
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
