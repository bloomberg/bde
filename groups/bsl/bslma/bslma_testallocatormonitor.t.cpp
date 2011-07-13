// bslma_testallocatormonitor.t.cpp
#include <bslma_testallocatormonitor.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_testallocator.h>
#include <bslmf_assert.h>

#include <cstring>     // memset()
#include <cstdlib>     // atoi()
#include <iostream>

using namespace BloombergLP;
using namespace std;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements a ...
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR & MANIPULATOR pointer/reference parameters are declared 'const'.
//: o No memory is ever allocated from the global allocator.
//: o Precondition violations are detected in appropriate build modes.
//
// Global Assumptions:
//: o All explicit memory allocations are presumed to use the global, default,
//:   or object allocator.
//: o ACCESSOR methods are 'const' thread-safe.
// ----------------------------------------------------------------------------
// CREATORS
// [  ] bslma_TestAllocatorMonitor(const bslma_TestAllocator& tA);
//
// ACCESSORS
// [  ] bool isInUseDown() const;
// [  ] bool isInUseSame() const;
// [  ] bool isInUseUp() const;
// [  ] bool isMaxSame() const;
// [  ] bool isMaxUp() const;
// [  ] bool isTotalSame() const;
// [  ] bool isTotalUp() const;
// ----------------------------------------------------------------------------
// [  ] BREATHING TEST
// [  ] USAGE EXAMPLE
// [ *] CONCERN: This test driver is reusable w/other, similar components.
// [ *] CONCERN: In no case does memory come from the global allocator.
// [  ] CONCERN: All creator/manipulator ptr./ref. parameters are 'const'.
// [  ] CONCERN: All accessor methods are declared 'const'.
// [  ] CONCERN: There is no temporary allocation from any allocator.
// [  ] CONCERN: Precondition violations are detected when enabled.

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

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) {                                                 \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": "                 \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                                               \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"     \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                                             \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\n";                    \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                                           \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" <<                  \
       #M << ": " << M << "\n";                                               \
       aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

class MyMemoryUser
{
    // This class, written in support of the "Usage Example", takes an
    // allocator and uses it to acquire, hold, and return memory.

    // DATA
    int              d_size;
    void            *d_held_p;
    bslma_Allocator *d_allocator_p;

  public:
    // CREATORS
    MyMemoryUser(bslma_Allocator *basicAllocator = 0);
        // Create a 'MyMemoryUser' object holding 0 bytes of allocated memory.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless 'size >= 0'.

    explicit MyMemoryUser(int size, bslma_Allocator *basicAllocator = 0);
        // Create a 'MyMemoryUser' object holding the specified 'size' bytes of
        // allocated memory.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The behavior is undefined unless
        // 'size >= 0'.

    ~MyMemoryUser();
        // Destroy this object.

    // MANIPULATORS
    void setMemorySize(int size);
        // Set the size of the held allocated memory to the specified 'size'.
        // Previously held memory is returned to the allocator.  The behavior
        // is undefined unless 'size >= 0'.

    // ACCESSORS
    int memorySize() const;
        // Return the size of the currently held memory.
};

// CREATORS
inline
MyMemoryUser::MyMemoryUser(bslma_Allocator *basicAllocator)
: d_size(0)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

inline
MyMemoryUser::MyMemoryUser(int size, bslma_Allocator *basicAllocator)
: d_size(size)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    d_held_p = d_allocator_p->allocate(d_size);
}

inline
MyMemoryUser::~MyMemoryUser()
{
    d_allocator_p->deallocate(d_held_p);
}

// MANIPULATORS
inline
void MyMemoryUser::setMemorySize(int size)
{
    d_allocator_p->deallocate(d_held_p);
    d_size   = size;
    d_held_p = d_allocator_p->allocate(d_size);
}

// ACCESSORS
inline
int MyMemoryUser::memorySize() const
{
    return d_size;
}

typedef MyMemoryUser Obj;

// ============================================================================
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// A 'bslma_TestAllocatorMonitor' object can be used in a simple check for
// memory leakage.
//
// First, we must specify a class of interest (to be the subject of the test).
// We here define 'MyClass', a simple, unconstrained attribute class.  An
// abbreviated implementation attribute suffices for the purposes of this
// example (i.e., the demonstration of 'bslma_TestAllocatorMonitor').  A proper
// attribute class would have value and copy constructors, 'operator==', etc.
//..
    class MyClass {
        // This unconstrained (value-semantic) attribute class has a single
        // attribute, 'description', an ascii string.

        // DATA
        int              d_capacity;
        char            *d_description_p;
        bslma_Allocator *d_allocator_p;   // held, not owned

      public:
        // CREATORS
        MyClass(bslma_Allocator *basicAllocator = 0);
            // Create a 'MyClass' object having the (default) attribute values:
            //..
            // description() == ""
            //..
            // Optionally specify a 'basicAllocator' used to supply memory.  If
            // 'basicAllocator' is 0, the currently installed default allocator
            // is used.

        ~MyClass();
           // Destroy this object.

        // MANIPULATORS
        void setDescription(const char* value);
            // Set the 'description' attribute of this object to data of the
            // specified 'length' at the specified 'value' address.

        // ACCESSORS
        const char *description() const;
            // Return a reference providing non-modifiable access to the
            // 'description' attribute of this object.
    };

    // ========================================================================
    //                      INLINE FUNCTION DEFINITIONS
    // ========================================================================

                            // -------------
                            // class MyClass
                            // -------------

    // CREATORS
    inline
    MyClass::MyClass(bslma_Allocator *basicAllocator)
    : d_capacity(0)
    , d_description_p(0)
    , d_allocator_p(bslma_Default::allocator(basicAllocator))
    {
    }

    inline
    MyClass::~MyClass()
    {
        BSLS_ASSERT_SAFE(0 <= d_capacity);

        d_allocator_p->deallocate(d_description_p);
    }

    // MANIPULATORS
    inline
    void MyClass::setDescription(const char *value)
    {
        BSLS_ASSERT_SAFE(value);

        int length = std::strlen(value);
        int size   = length + 1;
        if (size > d_capacity) {
            d_allocator_p->deallocate(d_description_p);
            d_description_p = (char *) d_allocator_p->allocate(size);
            d_capacity      = size;
        }
        std::memcpy(d_description_p, value, length);
        d_description_p[length] = '\0';
    }

    // MANIPULATORS
    inline
    const char *MyClass::description() const
    {
        return d_description_p ? d_description_p : "";
    }
//..

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

//
// Then, we create, 'oa', an 'bslma_TestAllocator' object, to serve as object
// allocator and 'oam', an 'bslma_TestAllocatorMonitor' object, to track that
// allocator.
//..
    {
        bslma_TestAllocator        oa("object",  veryVeryVeryVerbose);
        bslma_TestAllocatorMonitor oam(oa);
//..
// Next, we create 'obj', an object of the class of interest, and exercise it
// so that memory is allocated.  (Typically, an object must allocate memory to
// hold data that exceeds its own size.)
//..
        {
            MyClass obj(&oa);

            const char BRUTUS[]="The noblest Roman of them all.";
            BSLMF_ASSERT(sizeof(obj) < sizeof(BRUTUS));

            obj.setDescription(BRUTUS);
        }
//..
// Finally, we examine 'oam' check the usage of the object's allocator.

        ASSERT(true == oam.isTotalUp());   // Object allocator was used.
        ASSERT(true == oam.isInUseSame()); // All allocations were deallocated.
    }
//..
// Notice that 'isTotalUp' method indicates that some allocators and
// deallocations were made.  The 'isInUseSame' method indicates that the
// number of *outstanding* allocations from 'oa' has not changed since
// the creation of 'oam'.  The 'oam' object was created when that value was 0
// (before any use of 'oa').
//
///Example 2: Standard Usage
///- - - - - - - - - - - - -
// Classes taking 'bslma_allocator' objects have many testing concerns that
// other classes do not.  For example, objects of the class of interest must:
//: 1 Release all allocated memory on destruction.
//: 2 Allocate memory from the appropriate allocator.
//: 3 Allocate no memory when default constructed.
//: 4 Cache allocated memory for reuse.
//: 5 Create no temporary objects (i.e., no direct use of the default
//:   allocator).
//
// C-1 is (no memory leaks) is a universal concern of software engineering.
// C-2 is a requirement for classes following the BDE allocator paradigm.
// (There is a global assumption that all memory allocation is done from *some*
// allocator).  The remaining concerns are Quality-of-Implementation (QoI)
// issues for certain members of the BDE taxonomy.  Notice that none of these
// concerns are explicitly documented in the contract for the class of
// interest.
//
// We hear illustrate how 'bslma_TestAllocatorMonitor' objects can be used to
// address such concerns.  As a test subject, we reuse 'MyClass', the class
// defined in Example 1.
//
// First, following the typical testing pattern, we create a trio of
// 'bslma_TestAllocator' objects, and install two as the global and default
// allocators.  The remaining allocator will be supplied to the object.
//..
    {
        bslma_TestAllocator ga("global",  veryVeryVeryVerbose);
        bslma_TestAllocator da("default", veryVeryVeryVerbose);
        bslma_TestAllocator oa("object",  veryVeryVeryVerbose);

        bslma_Default::setGlobalAllocator(&ga);
        bslma_Default::setDefaultAllocatorRaw(&da);
//..
// Then, one creates a trio of 'bslma_TestAllocatorMonitor' objects to capture
// the current state (unused) of each of the 'bslma_TestAllocator' objects.
//..
        bslma_TestAllocatorMonitor gam(ga), dam(da), oam(oa);
//..
// Next, one creates an object of the class under test, supplying the remaining
// allocator, and subjects the object to the relevant operations.  In this
// first test, the object is just default constructed, and then destroyed.
//..
        {
            MyClass obj(&oa);
        }
//..
// Then, after the object is destroyed, the several monitor objects show that
// their respective allocators were not used.
//..
        ASSERT(true == gam.isTotalSame()); // no (de)allocations
        ASSERT(true == dam.isTotalSame()); // no (de)allocations
        ASSERT(true == oam.isTotalSame()); // no (de)allocations
//..
// Notice that we have addressed C-3.

// Next, we force the object into allocating memory, and confirm that memory
// was allocated.
//..
        {
            MyClass obj(&oa);

            const char DESCRIPTION1[]="abcdefghijklmnopqrstuvwyz"
                                      "abcdefghijklmnopqrstuvwyz";
            BSLMF_ASSERT(sizeof(obj) < sizeof(DESCRIPTION1));

            obj.setDescription(DESCRIPTION1);

            ASSERT(true == oam.isTotalUp());  // object allocator was used
            ASSERT(true == oam.isInUseUp());  // some outstanding allocation(s)
//..
// Then, we reset the attribute to a short value and test that the previously
// allocated memory is reused.  To measure changes in the object allocator
// relative to its current state, we introduce a second monitor object.
//..

            bslma_TestAllocatorMonitor oam2(oa);

            obj.setDescription("a");

            ASSERT(true == oam2.isTotalSame());  // no (de)allocations
//..
// Notice that this test addresses C-4.  Next, we assign a value that forces
// the object to allocate additional memory.
//..
            const char DESCRIPTION2[]="abcdefghijklmnopqrstuvwyz"
                                      "abcdefghijklmnopqrstuvwyz"
                                      "abcdefghijklmnopqrstuvwyz"
                                      "abcdefghijklmnopqrstuvwyz";
            obj.setDescription(DESCRIPTION2);

            ASSERT(true == oam2.isTotalUp());    // Object allocator was used.
            ASSERT(true == oam2.isInUseSame());  // Outstanding allocation
                                                 // count (but not byte count)
                                                 // is unchanged.
        }
//..
// Now that the object has been destroyed.
//..
        ASSERT(true == oam.isTotalUp());   // Object allocator was used.
        ASSERT(true == oam.isInUseSame()); // All allocations were deallocated.
//..
// Notice that these tests confirm C-2.
//
// Finally, we examine the two monitors of the two non-object allocators to
// confirm that they were not used by any of these operations.
//..
        ASSERT(true == gam.isTotalSame()); // Global  allocator was never used.
        ASSERT(true == dam.isTotalSame()); // Default allocator was never used.
    }
//..
// Notice that the last tests, along with our observations of the use of the
// object allocator, address C-2 and C-5.

      } break;
      case 1: {

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
