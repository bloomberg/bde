// bdema_newdeleteallocator.t.cpp -*-C++-*-

#include <bdema_newdeleteallocator.h>

#include <bslma_allocator.h>    // for testing only

#include <bsls_platform.h>

#include <bsl_cstdio.h>      // printf()
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // memcpy()
#include <bsl_iostream.h>

#ifdef BSLS_PLATFORM__CMP_MSVC       // Microsoft Compiler
#ifdef _MSC_EXTENSIONS               // Microsoft Extensions Enabled
#include <bsl_new.h>                     // if so, need to include new as well
#endif
#endif

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//==========================================================================
//                    STANDARD BDE ASSERT TEST MACRO
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
//--------------------------------------------------------------------------

//==========================================================================
//                              TEST PLAN
//--------------------------------------------------------------------------
//                              Overview
//                              --------
// We are testing a concrete implementation of a protocol.  The component also
// provides access to a singleton instance.  This singleton must be available
// from the moment it is first needed until after its last use, yet we want to
// suppress any reports of potential memory leaks resulting from analysis tools
// such as purify.  We also need to verify that instances of this concrete
// class all global operators 'new' and 'delete'.  We can do that by redefining
// these global operators and instrumenting them to be sure that these
// operators are in fact called.
//--------------------------------------------------------------------------
// [ 2] static bdema_NewDeleteAllocator& singleton();
// [ 2] static bslma_Allocator *allocator(bslma_Allocator *basicAllocator);
// [ 1] bdema_NewDeleteAllocator();
// [ 1] ~bdema_NewDeleteAllocator();
// [ 1] void *allocate(int size);
// [ 1] void deallocate(void *address);
//--------------------------------------------------------------------------
// [ 3] USAGE TEST - Make sure main usage example compiles and works properly.
// [ 1] Make sure that global operators new and delete are called.
// [ 2] Make sure that the lifetime of the singleton is sufficient.
// [ 2] Make sure that memory is not leaked.
//==========================================================================

//==========================================================================
//                      USAGE EXAMPLE
//--------------------------------------------------------------------------
class my_ShortArray {
    short *d_array_p; // dynamically-allocated array of short integers
    int d_size;       // physical size of the 'd_array_p' array (elements)
    int d_length;     // logical length of the 'd_array_p' array (elements)
    bslma_Allocator *d_allocator_p; // holds (but doesn't own) allocator

  private:
    void increaseSize(); // Increase the capacity by at least one element.

  public:
    // CREATORS
    my_ShortArray(bslma_Allocator *basicAllocator = 0);
        // Create a empty array.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, global
        // operators 'new' and 'delete' are used.
     // ...

    ~my_ShortArray();
    void append(int value);
    const short& operator[](int index) const { return d_array_p[index]; }
    int length() const { return d_length; }
};

enum { INITIAL_SIZE = 1, GROW_FACTOR = 2 };

// ...

my_ShortArray::my_ShortArray(bslma_Allocator *basicAllocator)
: d_size(INITIAL_SIZE)
, d_length(0)
, d_allocator_p(bdema_NewDeleteAllocator::allocator(basicAllocator))
    // The above initialization expression is equivalent to 'basicAllocator
    // ? basicAllocator : &bdema_NewDeleteAllocator::singleton()'.
{
    ASSERT(d_allocator_p);
    d_array_p = (short *)
                d_allocator_p->allocate(d_size * sizeof *d_array_p);
}

my_ShortArray::~my_ShortArray()
{
    // CLASS INVARIANTS
    ASSERT(d_allocator_p);
    ASSERT(d_array_p);
    ASSERT(0 <= d_size);
    ASSERT(0 <= d_length); ASSERT(d_length <= d_size);

    d_allocator_p->deallocate(d_array_p);
}

inline void my_ShortArray::append(int value)
{
    if (d_length >= d_size) {
        increaseSize();
    }
    d_array_p[d_length++] = value;
}

inline static
void reallocate(short **array, int newSize, int length,
                bslma_Allocator *basicAllocator)
    // Reallocate memory in the specified 'array' to the specified
    // 'newSize' using the specified 'basicAllocator'.  The specified
    // 'length' number of leading elements are preserved.  Since the
    //  class invariant requires that the physical capacity of the
    // container may grow but never shrink; the behavior is undefined
    // unless length <= newSize.
{
    ASSERT(array);
    ASSERT(1 <= newSize);
    ASSERT(0 <= length);
    ASSERT(basicAllocator);
    ASSERT(length <= newSize);        // enforce class invariant

    short *tmp = *array;             // support exception neutrality
    *array = (short *) basicAllocator->allocate(newSize * sizeof **array);

    // COMMIT POINT

    memcpy(*array, tmp, length * sizeof **array);
    basicAllocator->deallocate(tmp);
}

void my_ShortArray::increaseSize()
{
     int proposedNewSize = d_size * GROW_FACTOR;      // reallocate can throw
     ASSERT(proposedNewSize > d_length);
     reallocate(&d_array_p, proposedNewSize, d_length, d_allocator_p);
     d_size = proposedNewSize;                        // we're committed
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//           Additional Functionality Need to Complete Usage Test Case


ostream& operator<<(ostream& stream, const my_ShortArray& array)
{
    stream << '[';
    const int len = array.length();
    for (int i = 0; i < len; ++i) {
        stream << ' ' << array[i];
    }
    return stream << " ]" << flush;
}

//--------------------------------------------------------------------------
//                      REDEFINED GLOBAL OPERATOR NEW
//--------------------------------------------------------------------------

static int globalVeryVerbose = 0;

static int globalNewCalledCount = 0;
static int globalNewCalledCountIsEnabled = 0;
static int globalNewCalledLastArg = 0;

static  int  globalDeleteCalledCount = 0;
static  int  globalDeleteCalledCountIsEnabled = 0;
static void *globalDeleteCalledLastArg = 0;

#ifdef BDE_BUILD_TARGET_EXC
void *operator new(size_t size) throw(std::bad_alloc)
#else
void *operator new(size_t size)
#endif
    // Trace use of global operator new.  Note that we must use printf
    // to avoid recursion.
{
    void *addr = malloc(size);

    if (globalNewCalledCountIsEnabled) {
        ++globalNewCalledCount;
        globalNewCalledLastArg = size;
    }


    if (globalVeryVerbose) {
        printf("global new called -");
        if (!globalNewCalledCountIsEnabled) {
            printf(" not");
        }
        printf(" enabled - count = %d: %p\n", globalNewCalledCount, addr);
    }

    return addr;
}

#ifdef BDE_BUILD_TARGET_EXC
void operator delete(void *address) throw()
#else
void operator delete(void *address)
#endif
    // Trace use of global operator delete.
{
    if (globalDeleteCalledCountIsEnabled) {
        ++globalDeleteCalledCount;
        globalDeleteCalledLastArg = address;
    }

    if (globalVeryVerbose) {
        printf("global delete called -");
        if (!globalDeleteCalledCountIsEnabled) {
            printf(" not");
        }
        printf(" enabled - count = %d: freeing %p\n",
                globalDeleteCalledCount, address);
    }

    free(address);
}

//==========================================================================
//                      MAIN PROGRAM
//--------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    static int test = argc > 1 ? atoi(argv[1]) : 0;
    static int verbose = argc > 2;
    static int veryVerbose = globalVeryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 3: {
        // -----------------------------------------------------------------
        // USAGE TEST:
        //
        // Testing:
        //  USAGE TEST - Make sure main usage example compiles and works.
        // -----------------------------------------------------------------

        if (verbose) cout << endl << "USAGE TEST" << endl
                                  << "==========" << endl;

        const void *addrFirstElement = 0;

        {
            ASSERT(0 == globalNewCalledCount);
            ASSERT(0 == globalDeleteCalledCount);
            ASSERT(0 == globalNewCalledLastArg);
            ASSERT(0 == globalDeleteCalledLastArg);

            globalNewCalledCountIsEnabled = 1;
            globalDeleteCalledCountIsEnabled = 1;
            my_ShortArray a;
            globalNewCalledCountIsEnabled = 0;
            globalDeleteCalledCountIsEnabled = 0;
            ASSERT(1 == globalNewCalledCount);
            ASSERT(0 == globalDeleteCalledCount);
            ASSERT(sizeof(short) == globalNewCalledLastArg);
            ASSERT(0 == globalDeleteCalledLastArg);

            addrFirstElement = &a[0];
            ASSERT(0 == a.length());

            globalNewCalledCountIsEnabled = 1;
            globalDeleteCalledCountIsEnabled = 1;
            a.append(1);
            globalNewCalledCountIsEnabled = 0;
            globalDeleteCalledCountIsEnabled = 0;
            ASSERT(1 == globalNewCalledCount);
            ASSERT(0 == globalDeleteCalledCount);
            ASSERT(sizeof(short) == globalNewCalledLastArg);
            ASSERT(0 == globalDeleteCalledLastArg);

            ASSERT(1 == a.length());    ASSERT(1 == a[0]);

            globalNewCalledCountIsEnabled = 1;
            globalDeleteCalledCountIsEnabled = 1;
            a.append(2);
            globalNewCalledCountIsEnabled = 0;
            globalDeleteCalledCountIsEnabled = 0;
            ASSERT(2 == globalNewCalledCount);
            ASSERT(1 == globalDeleteCalledCount);
            ASSERT(2 * sizeof(short) == globalNewCalledLastArg);
            ASSERT(addrFirstElement == globalDeleteCalledLastArg);

            addrFirstElement = &a[0];
            ASSERT(2 == a.length());    ASSERT(1 == a[0]);
                                        ASSERT(2 == a[1]);

            globalNewCalledCountIsEnabled = 1;
            globalDeleteCalledCountIsEnabled = 1;
            a.append(3);
            globalNewCalledCountIsEnabled = 0;
            globalDeleteCalledCountIsEnabled = 0;
            ASSERT(3 == globalNewCalledCount);
            ASSERT(2 == globalDeleteCalledCount);
            ASSERT(4 * sizeof(short) == globalNewCalledLastArg);
            ASSERT(addrFirstElement == globalDeleteCalledLastArg);

            addrFirstElement = &a[0];
            ASSERT(3 == a.length());    ASSERT(1 == a[0]);
                                        ASSERT(2 == a[1]);
                                        ASSERT(3 == a[2]);

            globalNewCalledCountIsEnabled = 1;
            globalDeleteCalledCountIsEnabled = 1;
        }
        globalNewCalledCountIsEnabled = 0;
        globalDeleteCalledCountIsEnabled = 0;
        ASSERT(3 == globalNewCalledCount);
        ASSERT(3 == globalDeleteCalledCount);
        ASSERT(4 * sizeof(short) == globalNewCalledLastArg);
        ASSERT(addrFirstElement == globalDeleteCalledLastArg);

      } break;
      case 2: {
        // -----------------------------------------------------------------
        // SINGLETON TEST:
        //   We need to make sure that we get a valid singleton and that
        //   it remains valid as long as we need it.  The way we'll try do
        //   this is to allocate memory in the destructor of a static object
        //   that is defined prior the first use of the allocator.
        //
        // Testing:
        //   static bdema_NewDeleteAllocator& singleton();
        //   static bslma_Allocator *allocator(bslma_Allocator *basicAlloc);
        //
        //   Make sure that the lifetime of the singleton is sufficient.
        //   Make sure that memory is not leaked.
        // -----------------------------------------------------------------

        if (verbose) cout << endl << "SINGLETON TEST" << endl
                                  << "==============" << endl;

        if (veryVerbose) cout << "\nTesting 'singleton'." << endl;
        {
            ASSERT(0 == globalNewCalledCount);
            ASSERT(0 == globalDeleteCalledCount);

            globalNewCalledCountIsEnabled = 1;
            globalDeleteCalledCountIsEnabled = 1;

            bdema_NewDeleteAllocator *p =
                                &bdema_NewDeleteAllocator::singleton();
            bdema_NewDeleteAllocator *q =
                                &bdema_NewDeleteAllocator::singleton();

            globalNewCalledCountIsEnabled = 0;
            globalDeleteCalledCountIsEnabled = 0;

            ASSERT(0 == globalNewCalledCount);
            ASSERT(0 == globalDeleteCalledCount);

            ASSERT(p == q);

            globalNewCalledCountIsEnabled = 1;
            void *addr1 = p->allocate(15);
            globalNewCalledCountIsEnabled = 0;
            ASSERT(1 == globalNewCalledCount);
            ASSERT(0 == globalDeleteCalledCount);
            ASSERT(15 == globalNewCalledLastArg);

            ASSERT(0 == globalDeleteCalledCount);
            ASSERT(0 == globalDeleteCalledLastArg);

            globalDeleteCalledCountIsEnabled = 1;
            q->deallocate(addr1);
            globalDeleteCalledCountIsEnabled = 0;
            ASSERT(1 == globalNewCalledCount);
            ASSERT(1 == globalDeleteCalledCount);
            ASSERT(addr1 == globalDeleteCalledLastArg);
        }

        if (veryVerbose) cout << "\nTesting 'allocator'." << endl;
        {
            bslma_Allocator *p = (bslma_Allocator *) 0xDeadBeef;
            bslma_Allocator *q = &bdema_NewDeleteAllocator::singleton();

            bslma_Allocator *r = bdema_NewDeleteAllocator::allocator(p);
            ASSERT(r == p);

            bslma_Allocator *s = bdema_NewDeleteAllocator::allocator(0);
            ASSERT(q == s);

            bslma_Allocator *t = bdema_NewDeleteAllocator::allocator(q);
            ASSERT(q == t);
        }

      } break;
      case 1: {
        // -----------------------------------------------------------------
        // BASIC TEST:
        //   Create a new-delete allocator on the program stack and verify that
        //   'new' and 'delete' are each called exactly once per method
        //   invocation, and with the appropriate arguments.
        //
        // Testing:
        //    bdema_NewDeleteAllocator();
        //    ~bdema_NewDeleteAllocator();
        //    void *allocate(int size);
        //    void deallocate(void *address);
        //
        //    Make sure that global operators new and delete are called.
        // -----------------------------------------------------------------

        if (verbose) cout << endl << "BASIC TEST" << endl
                                  << "==========" << endl;

        if (veryVerbose) cout << "\nCreate an allocator" << endl;

        bdema_NewDeleteAllocator a;

        ASSERT(0 == globalNewCalledCount);
        ASSERT(0 == globalNewCalledLastArg);

        globalNewCalledCountIsEnabled = 1;
        void *addr1 = a.allocate(5);
        globalNewCalledCountIsEnabled = 0;
        ASSERT(1 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(5 == globalNewCalledLastArg);

        globalNewCalledCountIsEnabled = 1;
        void *addr2 = a.allocate(10);
        globalNewCalledCountIsEnabled = 0;
        ASSERT(2 == globalNewCalledCount);
        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(10 == globalNewCalledLastArg);

        ASSERT(0 == globalDeleteCalledCount);
        ASSERT(0 == globalDeleteCalledLastArg);

        globalDeleteCalledCountIsEnabled = 1;
        a.deallocate(addr1);
        globalDeleteCalledCountIsEnabled = 0;
        ASSERT(2 == globalNewCalledCount);
        ASSERT(1 == globalDeleteCalledCount);
        ASSERT(addr1 == globalDeleteCalledLastArg);

        globalDeleteCalledCountIsEnabled = 1;
        a.deallocate(addr2);
        globalDeleteCalledCountIsEnabled = 0;
        ASSERT(2 == globalNewCalledCount);
        ASSERT(2 == globalDeleteCalledCount);
        ASSERT(addr2 == globalDeleteCalledLastArg);

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
