// bdema_autodeallocator.t.cpp  -*-C++-*-

#include <bdema_autodeallocator.h>
#include <bslma_allocator.h>       // for testing only

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // memcpy()
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// We are testing a proctor object to ensure that it deallocates the correct
// memory address with the allocator it holds.  We achieve this goal by
// utilizing the 'TestAllocator' allocator, whose 'deallocate' method is
// instrumented to record the most recent memory address used to invoke the
// method.  We initialize the 'bdema_AutoDeallocator' proctor object with this
// allocator and verify that when the proctor object is destroyed the expected
// memory address is recorded in the allocator.  Since 'TestAllocator' is not
// derived from 'bslma_Allocator' and does not implement an 'allocate' method,
// we ensure that this proctor works with any 'ALLOCATOR' object that supports
// the required 'deallocate' method.  We also need to verify that when the
// 'release' method is invoked on the proctor object the proctor does not
// deallocate its managed memory.  We achieve this goal by instrumenting the
// allocator object with a flag that indicates whether its 'deallocate' method
// has ever been called, and ensure that this flag is in the proper state when
// the proctor's 'release' method is called.
//-----------------------------------------------------------------------------
// [2] bdema_AutoDeallocator<ALLOCATOR>(memory, originalAllocator);
// [2] ~bdema_AutoDeallocator<ALLOCATOR>();
// [2] void release();
// [2] void reset(memory);
//-----------------------------------------------------------------------------
// [3] USAGE EXAMPLE
// [1] Ensure TestAllocator works as expected.
//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                          HELPER CLASS FOR TESTING
//-----------------------------------------------------------------------------

class TestAllocator {
  private:
    int   d_isDeallocateCalled;    // set if 'deallocate' has bee called
    void *d_lastDeallocateAddress; // last memory address deallocated

  public:
    // CREATORS
    TestAllocator() : d_isDeallocateCalled(0), d_lastDeallocateAddress(0) {}
        // Create this object.

    ~TestAllocator() {}
        // Destroy this object.

    // MANIPULATORS
    void deallocate(void *address);
        // Record the specified 'address' and set an internal flag to indicate
        // this method has been called.

    // ACCESSORS
    void *lastDeallocateAddress() const { return d_lastDeallocateAddress; }
        // Return the last memory address that 'deallocate' was invoked with.
        // Return 'null' if 'deallocate' has never been called on this object.

    int isDeallocateCalled() const { return d_isDeallocateCalled; }
        // Return 1 if 'deallocate' has been called on this object, and 0
        // otherwise.
};

void TestAllocator::deallocate(void *address)
{
    d_lastDeallocateAddress = address;
    d_isDeallocateCalled = 1;
}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

// my_List.h

class my_List {
    char            *d_typeArray_p;
    void           **d_list_p;
    int              d_length;
    int              d_size;
    bslma_Allocator *d_allocator_p;

  private:
    void increaseSize();

  public :
    enum Type { CHAR, INT, DOUBLE };

    my_List(bslma_Allocator *basicAllocator);
    ~my_List();
    void append(char value);
    void append(int value);
    void append(double value);
    const char *theChar(int index) const { return (char *)   d_list_p[index]; }
    const int *theInt(int index) const   { return (int *)    d_list_p[index]; }
    const double *theDouble(int index) const
                                         { return (double *) d_list_p[index]; }
    Type type(int index) const           { return (Type) d_typeArray_p[index];}
    int length() const                   { return d_length;                   }
    // ...
};

// ...

// my_List.cpp

enum { INITIAL_SIZE = 1, GROW_FACTOR = 2 };

my_List::my_List(bslma_Allocator *basicAllocator)
: d_length(0)
, d_size(INITIAL_SIZE)
, d_allocator_p(basicAllocator)
{
    ASSERT(d_allocator_p);
    d_typeArray_p =
        (char *) d_allocator_p->allocate(d_size * sizeof *d_typeArray_p);
    bdema_AutoDeallocator<bslma_Allocator> autoDealloc(d_typeArray_p,
                                                       d_allocator_p);
    d_list_p =
        (void **) d_allocator_p->allocate(d_size * sizeof *d_list_p);
    autoDealloc.release();
}

my_List::~my_List()
{
    ASSERT(d_typeArray_p);
    ASSERT(d_list_p);
    ASSERT(0 <= d_size);
    ASSERT(0 <= d_length);  ASSERT(d_length <= d_size);
    ASSERT(d_allocator_p);
    d_allocator_p->deallocate(d_typeArray_p);
    for (int i =0; i < d_length; ++i) {
        d_allocator_p->deallocate(d_list_p[i]);
    }
    d_allocator_p->deallocate(d_list_p);
}

void my_List::append(char value)
{
    if (d_length >= d_size) {
        increaseSize();
    }
    char *item = (char *) d_allocator_p->allocate(sizeof *item);
    *item = value;
    d_typeArray_p[d_length] = (char) my_List::CHAR;
    d_list_p[d_length++] = item;
}

void my_List::append(int value)
{
    if (d_length >= d_size) {
        increaseSize();
    }
    int *item = (int *) d_allocator_p->allocate(sizeof *item);
    *item = value;
    d_typeArray_p[d_length] = (char) my_List::INT;
    d_list_p[d_length++] = item;
}

void my_List::append(double value)
{
    if (d_length >= d_size) {
        increaseSize();
    }
    double *item = (double *) d_allocator_p->allocate(sizeof *item);
    *item = value;
    d_typeArray_p[d_length] = (char) my_List::DOUBLE;
    d_list_p[d_length++] = item;
}

inline static
void reallocate(void ***list, char **typeArray, int *size,
                int newSize, int length, bslma_Allocator *basicAllocator)
    // Reallocate memory in the specified 'list' and 'typeArray' using the
    // specified 'basicAllocator' and update the specified size to the
    // specified 'newSize'.  The specified 'length' number of leading
    // elements are preserved in 'list' and 'typeArray'.  If 'allocate'
    // should throw an exception, this function has no effect.  The
    // behavior is undefined unless 1 <= newSize, 0 <= length, and
    // newSize <= length.
{
    ASSERT(list);
    ASSERT(*list);
    ASSERT(typeArray);
    ASSERT(*typeArray);
    ASSERT(size);
    ASSERT(1 <= newSize);
    ASSERT(0 <= length);
    ASSERT(length <= *size);    // sanity check
    ASSERT(length <= newSize);  // ensure class invariant
    ASSERT(basicAllocator);
    void **newList =
        (void **) basicAllocator->allocate(newSize * sizeof *newList);
    bdema_AutoDeallocator<bslma_Allocator> autoDealloc(newList,basicAllocator);
    char *newTypeArray =
        (char *) basicAllocator->allocate(newSize * sizeof *newTypeArray);
    autoDealloc.release();
    memcpy(newList, *list, length * sizeof **list);
    memcpy(newTypeArray, *typeArray, length * sizeof **typeArray);
    basicAllocator->deallocate(*list);
    basicAllocator->deallocate(*typeArray);
    *list = newList;
    *typeArray = newTypeArray;
    *size = newSize;
}

void my_List::increaseSize()
{
     int newSize = d_size * GROW_FACTOR;
     reallocate(&d_list_p, &d_typeArray_p, &d_size, newSize,
                d_length, d_allocator_p);
}

// ...

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//           Additional Functionality Needed to Complete Usage Test Case

// my_mallocfreeallocator.h

class my_MallocFreeAllocator : public bslma_Allocator {
  private: // not implemented.
    my_MallocFreeAllocator(const my_MallocFreeAllocator&);
    my_MallocFreeAllocator& operator=(const my_MallocFreeAllocator&);

  public:
    // CREATORS
    my_MallocFreeAllocator() {}
    ~my_MallocFreeAllocator() {}
    void *allocate(size_type size) { return (void *) malloc(size); }
    inline void deallocate(void *address) { free(address); }
    // ...
};

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // USAGE TEST
        //
        // Testing:
        //   USAGE TEST - Make sure main usage example compiles and works.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE TEST" << endl
                                  << "==========" << endl;


        my_MallocFreeAllocator mA;
        my_List mX(&mA);    const my_List &X = mX;
        ASSERT(0 == X.length());
        const char C = 'A';
        mX.append(C);
        ASSERT(1 ==  X.length());
        ASSERT(C == *X.theChar(0));     ASSERT(my_List::CHAR == X.type(0));

        const int I = 5;
        mX.append(I);
        ASSERT(2 ==  X.length());
        ASSERT(C == *X.theChar(0));     ASSERT(my_List::CHAR == X.type(0));
        ASSERT(I == *X.theInt(1));      ASSERT(my_List::INT  == X.type(1));

        const double D = 2.5;
        mX.append(D);
        ASSERT(3 ==  X.length());
        ASSERT(C == *X.theChar(0));     ASSERT(my_List::CHAR   == X.type(0));
        ASSERT(I == *X.theInt(1));      ASSERT(my_List::INT    == X.type(1));
        ASSERT(D == *X.theDouble(2));   ASSERT(my_List::DOUBLE == X.type(2));
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CTOR/RELEASE TEST
        //   Create 'bdema_AutoDeallocator' proctor objects holding
        //   'TestAllocator' objects and varying memory addresses.  Verify that
        //   when the proctor object goes out of scope the allocator object
        //   contains the expected memory address.  Perform the same procedure,
        //   but call the proctor's 'release' method before the proctor goes
        //   out of scope, and verify that the allocator's 'deallocate' method
        //   has never been called.  Also verify that the allocator's
        //   'deallocate' method is not called when the proctor is initialized
        //   with a 'null' memory address.
        //
        // Testing:
        //   bdema_AutoDeallocator<ALLOCATOR>(memory, originalAllocator);
        //   ~bdema_AutoDeallocator<ALLOCATOR>();
        //   void release();
        //   void reset(memory);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "CTOR/RELEASE TEST" << endl
                                  << "=================" << endl;

        const void *DATA[] = {(void *) 1, (void *) 2, (void *) 3 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "Testing constructor." << endl;
        int di;
        for (di = 0; di < NUM_DATA; ++di) {
            TestAllocator a;                   const TestAllocator &A = a;
            void *addr = (void *) DATA[di];    const void *ADDR = addr;
            {
                const bdema_AutoDeallocator<TestAllocator> X(addr, &a);
            }
            if (veryVerbose) { cout << '\t'; P(ADDR); }
            LOOP_ASSERT(di, ADDR == A.lastDeallocateAddress());
            LOOP_ASSERT(di, A.isDeallocateCalled());
        }

        if (verbose) cout << "Testing constructor w/ 'null' address." << endl;
        {
            TestAllocator a;                    const TestAllocator &A = a;
            {
                const bdema_AutoDeallocator<TestAllocator> X(0, &a);
            }
            ASSERT(0 == A.lastDeallocateAddress());
            ASSERT(!A.isDeallocateCalled());
        }

        if (verbose) cout << "Testing 'release'." << endl;
        for (di = 0; di < NUM_DATA; ++di) {
            TestAllocator a;                   const TestAllocator &A = a;
            void *addr = (void *) DATA[di];    const void *ADDR = addr;
            {
                bdema_AutoDeallocator<TestAllocator> x(addr, &a);
                x.release();
            }
            if (veryVerbose) { cout << '\t'; P(ADDR); }
            LOOP_ASSERT(di, 0 == A.lastDeallocateAddress());
            LOOP_ASSERT(di, !A.isDeallocateCalled());
        }

        if (verbose) cout << "Testing 'reset'." << endl;

        for (di = 0; di < NUM_DATA; ++di) {
            TestAllocator a;                   const TestAllocator &A = a;
            void *addr = (void *) DATA[di];    const void *ADDR = addr;
            {
                bdema_AutoDeallocator<TestAllocator> x(0, &a);
                x.release();
                x.reset(addr);
            }
            if (veryVerbose) { cout << '\t'; P(ADDR); }
            LOOP_ASSERT(di, ADDR == A.lastDeallocateAddress());
            LOOP_ASSERT(di, A.isDeallocateCalled());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // HELPER CLASS TEST
        //   Create a 'TestAllocator' object and call its 'deallocate' method
        //   with varying memory address.  Verify that 'lastDeallocateAddress'
        //   returns the expected memory addresses.  Also verify that
        //   'isDeallocateCalled' indicates the proper state of the
        //   'TestAllocator' object before and after 'deallocate' is called.
        //
        // Testing:
        //   TestAllocator();
        //   ~TestAllocator();
        //   void deallocate(address);
        //   int isDeallocateCalled();
        //   void *lastDeallocateAddress();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "HELPER CLASS TEST" << endl
                                  << "=================" << endl;

        if (verbose) cout << "Testing 'TestAllocator'." << endl;

        const void *DATA[] = {(void *) 0, (void *) 1, (void *) 2, (void *) 3 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        TestAllocator mX;    const TestAllocator &X = mX;
        ASSERT(0 == X.lastDeallocateAddress());
        ASSERT(!X.isDeallocateCalled());
        for (int di = 0; di < NUM_DATA; ++di) {
            void *addr = (void *) DATA[di];    const void *ADDR = addr;
            mX.deallocate(addr);
            if (veryVerbose) { cout << '\t'; P(ADDR); }
            LOOP_ASSERT(di, ADDR == X.lastDeallocateAddress());
            LOOP_ASSERT(di, X.isDeallocateCalled());
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
