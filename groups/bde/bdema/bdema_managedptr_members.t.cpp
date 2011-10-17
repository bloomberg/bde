// bdema_managedptr_members.t.cpp                                     -*-C++-*-
#include <bdema_managedptr_members.h>

#include <bdema_managedptr_factorydeleter.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // memcpy()
#include <bsl_iostream.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::endl;

//=============================================================================
//                             TEST PLAN
//                             ---------
// [ 4] imp. class bdema_ManagedPtr_Members
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] Test machinery

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
int testStatus = 0;

namespace {

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
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
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define PA(X, L) cout << #X " = "; printArray(X, L); cout << endl;
                                              // Print array 'X' of length 'L'
#define PA_(X, L) cout << #X " = "; printArray(X, L); cout << ", " << flush;
                                              // PA(X, L) without '\n'
#define L_ __LINE__                           // current Line number

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)

// ============================================================================
//                               TEST APPARATUS
// ----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

class MyTestObject;
class MyDerivedObject;

//=============================================================================
//                         HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

class MyTestObject {
    // This test-class serves three purposes.  It provides a base class for the
    // test classes in this test driver, so that derived -> base conversions
    // can be tested.  It also signals when its destructor is run by
    // incrementing an externally managed counter, supplied when each object
    // is created.  Finally, it exposes an internal data structure that can be
    // use to demonstrate the 'bdema_ManagedPtr' aliasing facility.

    // DATA
    volatile int *d_deleteCounter_p;
    mutable int   d_value[2];

  public:
    // CREATORS
    explicit MyTestObject(int *counter);

    // Use compiler-generated copy constructor and assignment operator
    // MyTestObject(MyTestObject const& orig);
    // MyTestObject operator=(MyTestObject const& orig);

    virtual ~MyTestObject();
        // Destroy this object.

    // ACCESSORS
    int *valuePtr(int index = 0) const;

    volatile int *deleteCounter() const;
};

MyTestObject::MyTestObject(int *counter)
: d_deleteCounter_p(counter)
, d_value()
{
}

MyTestObject::~MyTestObject()
{
    ++(*d_deleteCounter_p);
}

inline
int *MyTestObject::valuePtr(int index) const
{
    BSLS_ASSERT_SAFE(2 > index);

    return d_value + index;
}

volatile int* MyTestObject::deleteCounter() const
{
    return d_deleteCounter_p;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class MyDerivedObject : public MyTestObject
{
    // This test-class has the same destructor-counting behavior as
    // 'MyTestObject', but offers a derived class in order to test correct
    // behavior when handling derived->base conversions.

  public:
    // CREATORS
    explicit MyDerivedObject(int *counter);
    // Use compiler-generated copy

    ~MyDerivedObject();
        // Increment the stored reference to a counter by 100, then destroy
        // this object.
};

inline
MyDerivedObject::MyDerivedObject(int *counter)
: MyTestObject(counter)
{
}

inline
MyDerivedObject::~MyDerivedObject()
{
    (*deleteCounter()) += 99; // +1 from base -> 100
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class MySecondDerivedObject : public MyTestObject
{
    // This test-class has the same destructor-counting behavior as
    // 'MyTestObject', but offers a second, distinct, derived class in order to
    // test correct behavior when handling derived->base conversions.

  public:
    // CREATORS
    explicit MySecondDerivedObject(int *counter);
    // Use compiler-generated copy

    ~MySecondDerivedObject();
        // Increment the stored reference to a counter by 10000, then destroy
        // this object.
};

inline
MySecondDerivedObject::MySecondDerivedObject(int *counter)
: MyTestObject(counter)
{
}

inline
MySecondDerivedObject::~MySecondDerivedObject()
{
    (*deleteCounter()) += 9999;  // +1 from base -> 10000
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class CountedStackDeleter
{
    // DATA
    volatile int *d_deleteCounter_p;

  private:
    // NOT IMPLEMENTED
    CountedStackDeleter(const CountedStackDeleter&); //=delete;
    CountedStackDeleter& operator=(const CountedStackDeleter&); //=delete;

  public:
    // CREATORS
    explicit CountedStackDeleter(int *counter) : d_deleteCounter_p(counter) {}

    //! ~CountedStackDeleter();
        // Destroy this object.

    // ACCESSORS
    volatile int *deleteCounter() const { return d_deleteCounter_p; }

    void deleteObject(void *) const
    {
        ++*d_deleteCounter_p;
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The two deleters defined below do not use the factory (or even object)
// argument to perform their bookkeeping.  They are typically used to test
// overloads taking 'NULL' factories.
int g_deleteCount = 0;

static void countedNilDelete(void *, void*)
{
    static int& deleteCount = g_deleteCount;
    ++g_deleteCount;
}

//=============================================================================
//                    FILE-STATIC FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static void doNothingDeleter(void *object, void *)
{
    ASSERT(object);
}

//=============================================================================
//                  TEST PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma_TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma_Default::setGlobalAllocator(&globalAllocator);

    bslma_TestAllocator da("default", veryVeryVeryVerbose);
    bslma_Default::setDefaultAllocator(&da);

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // TESTING bdema_ManagedPtr_Members
        //  This class looks far too big to test in a single test case.
        //  Really ought to break out into the following test cases:
        //    basic ctor/dtor
        //    set function
        //    basic accessors  (pointer/deleter)
        //    value ctor
        //    setAlias
        //    move operations
        //    runDeleter
        //
        //  Note that this poor organization arose when this class was an imp.
        //  detail of a larger component, and there is no excuse not to split
        //  test tests out now
        //
        // Concerns:
        //: 1 TBD Enumerate concerns
        //
        // Plan:
        //: 1 TBD Describe the test plan
        //
        // Testing:
        //    bdema_ManagedPtr_Members();
        //    bdema_ManagedPtr_Members(void *, void *, DeleterFunc);
        //    bdema_ManagedPtr_Members(bdema_ManagedPtr_Members&);
        //    ~bdema_ManagedPtr_Members();
        //    void move(bdema_ManagedPtr_Members& other);
        //    void set(void *object, void *factory, DeleterFunc deleter);
        //    void setAliasPtr(void *ptr);
        //    void swap(bdema_ManagedPtr_Members& other);
        //    void runDeleter() const;
        //    void *pointer() const;
        //    const bdema_ManagedPtrDeleter& deleter() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING bdema_ManagedPtr_Members"
                          << "\n--------------------------------" << endl;


        typedef bdema_ManagedPtr_FactoryDeleter<MyTestObject,
                                             CountedStackDeleter > TestFactory;

        bslma_TestAllocatorMonitor gam(&globalAllocator);
        bslma_TestAllocatorMonitor dam(&da);

        if (verbose) cout << "\tTest default constructor\n";

        {
            const bdema_ManagedPtr_Members empty;
            ASSERT(0 == empty.pointer());
        }

        if (verbose) cout << "\tTest value constructor\n";

        {
            const bdema_ManagedPtr_Members empty(0, 0, 0);
            ASSERT(0 == empty.pointer());

            int deleteCount = 0;
            {
                CountedStackDeleter del(&deleteCount);

                int x;
                const bdema_ManagedPtr_Members simple(&x,
                                                      &del,
                                                      &countedNilDelete);
                ASSERT(&x == simple.pointer());
                ASSERT(&x == simple.deleter().object());
                ASSERT(&del == simple.deleter().factory());
                ASSERT(&countedNilDelete == simple.deleter().deleter());
            }
            LOOP_ASSERT(deleteCount, 0 == deleteCount);
            LOOP_ASSERT(g_deleteCount, 0 == g_deleteCount);

            deleteCount = 0;
            {
                CountedStackDeleter del(&deleteCount);

#ifdef BDE_BUILD_TARGET_EXC
                if (verbose) cout << "\t\tNegative testing\n";

                {
                    bsls_AssertTestHandlerGuard guard;

                    const bdema_ManagedPtr_Members empty(0, 0, 0);
                    ASSERT_SAFE_FAIL(empty.deleter());

                    int x;
                    ASSERT_SAFE_FAIL(bdema_ManagedPtr_Members bd(&x, &del, 0));
                    ASSERT_SAFE_PASS(bdema_ManagedPtr_Members gd( 0, &del, 0));
                }
#else
                if (verbose) cout << "\tNegative testing disabled due to lack"
                                     " of exception support\n";
#endif
            }
        }

        if (verbose) cout << "\tTest set\n";

        {
            bdema_ManagedPtr_Members members(0, 0, 0);
            ASSERT(0 == members.pointer());

            int x;
            double y;
            members.set(&x, &y, &countedNilDelete);
            ASSERT(&x == members.pointer());
            ASSERT(&x == members.deleter().object());
            ASSERT(&y == members.deleter().factory());
            ASSERT(&countedNilDelete == members.deleter().deleter());

            members.set(0, 0, 0);
            ASSERT(0 == members.pointer());

#ifdef BDE_BUILD_TARGET_EXC
                if (verbose) cout << "\t\tNegative testing\n";

                {
                    bsls_AssertTestHandlerGuard guard;
                    ASSERT_SAFE_FAIL(members.deleter());
                }
#else
                if (verbose) cout << "\tNegative testing disabled due to lack"
                                     " of exception support\n";
#endif

            {
                int deleteCount = 0;
                CountedStackDeleter del(&deleteCount);

#ifdef BDE_BUILD_TARGET_EXC
                if (verbose) cout << "\t\tNegative testing\n";

                {
                    bsls_AssertTestHandlerGuard guard;
                    int x;
                    ASSERT_SAFE_FAIL(members.set(&x, &del, 0));
                    ASSERT_SAFE_PASS(members.set( 0, &del, 0));
                }
#else
                if (verbose) cout << "\tNegative testing disabled due to lack"
                                     " of exception support\n";
#endif
            }

        }

        if (verbose) cout << "\tTest setAliasPtr\n";

        {
            int deleteCount = 0;
            {
                CountedStackDeleter del(&deleteCount);

                int x;
                bdema_ManagedPtr_Members simple(&x, &del, &countedNilDelete);
                ASSERT(&x == simple.pointer());
                ASSERT(&x == simple.deleter().object());
                ASSERT(&del == simple.deleter().factory());
                ASSERT(&countedNilDelete == simple.deleter().deleter());

                double y;
                simple.setAliasPtr(&y);
                ASSERT(&y == simple.pointer());
                ASSERT(&x == simple.deleter().object());
                ASSERT(&del == simple.deleter().factory());
                ASSERT(&countedNilDelete == simple.deleter().deleter());

#ifdef BDE_BUILD_TARGET_EXC
                if (verbose) cout << "\t\tNegative testing\n";

                {
                    bsls_AssertTestHandlerGuard guard;
                    ASSERT_SAFE_FAIL(simple.setAliasPtr(0));

                    simple.set(0, 0, 0);
                    ASSERT(0 == simple.pointer());
                    ASSERT_SAFE_FAIL(simple.deleter());

                    ASSERT_SAFE_FAIL(simple.setAliasPtr(&y));
                    ASSERT_SAFE_PASS(simple.setAliasPtr(0));
                }
#else
                if (verbose) cout << "\tNegative testing disabled due to lack"
                                     " of exception support\n";
#endif
            }
            LOOP_ASSERT(deleteCount, 0 == deleteCount);
            LOOP_ASSERT(g_deleteCount, 0 == g_deleteCount);
        }

        if (verbose) cout << "\tTest move constructor\n";

        {
            int deleteCount = 0;
            {
                CountedStackDeleter del(&deleteCount);

                int x;
                bdema_ManagedPtr_Members donor(&x, &del, &countedNilDelete);
                ASSERT(&x == donor.pointer());
                ASSERT(&x == donor.deleter().object());
                ASSERT(&del == donor.deleter().factory());
                ASSERT(&countedNilDelete == donor.deleter().deleter());

                bdema_ManagedPtr_Members sink(donor);
                ASSERT(&x == sink.pointer());
                ASSERT(&x == sink.deleter().object());
                ASSERT(&del == sink.deleter().factory());
                ASSERT(&countedNilDelete == sink.deleter().deleter());
                ASSERT(0 == donor.pointer());

                double y;
                sink.setAliasPtr(&y);
                ASSERT(&y == sink.pointer());
                ASSERT(&x == sink.deleter().object());
                ASSERT(&del == sink.deleter().factory());
                ASSERT(&countedNilDelete == sink.deleter().deleter());

                bdema_ManagedPtr_Members sink2(sink);
                ASSERT(&y == sink2.pointer());
                ASSERT(&x == sink2.deleter().object());
                ASSERT(&del == sink2.deleter().factory());
                ASSERT(&countedNilDelete == sink2.deleter().deleter());
                ASSERT(0 == sink.pointer());
            }
            LOOP_ASSERT(deleteCount, 0 == deleteCount);
            LOOP_ASSERT(g_deleteCount, 0 == g_deleteCount);
        }

        if (verbose) cout << "\tTest move\n";

        {
            int deleteCount = 0;
            {
                CountedStackDeleter del1(&deleteCount);
                CountedStackDeleter del2(&deleteCount);

                int x;
                double y;
                bdema_ManagedPtr_Members a(&x, &del1, &countedNilDelete);
                bdema_ManagedPtr_Members b(&y, &del2, &doNothingDeleter);

                ASSERT(&x == a.pointer());
                ASSERT(&x == a.deleter().object());
                ASSERT(&del1 == a.deleter().factory());
                ASSERT(&countedNilDelete == a.deleter().deleter());

                ASSERT(&y == b.pointer());
                ASSERT(&y == b.deleter().object());
                ASSERT(&del2 == b.deleter().factory());
                ASSERT(&doNothingDeleter == b.deleter().deleter());

                a.move(b);
                ASSERT(&y == a.pointer());
                ASSERT(&y == a.deleter().object());
                ASSERT(&del2 == a.deleter().factory());
                ASSERT(&doNothingDeleter == a.deleter().deleter());

                ASSERT(0 == b.pointer());

                b.set(0, 0, 0);
                a.setAliasPtr(&x);

                b.move(a);
                ASSERT(&x == b.pointer());
                ASSERT(&y == b.deleter().object());
                ASSERT(&del2 == b.deleter().factory());
                ASSERT(&doNothingDeleter == b.deleter().deleter());

                ASSERT(0 == a.pointer());

#ifdef BDE_BUILD_TARGET_EXC
                if (verbose) cout << "\t\tNegative testing\n";

                {
                    bsls_AssertTestHandlerGuard guard;
                    ASSERT_SAFE_FAIL(b.move(b));
                }
#else
                if (verbose) cout << "\tNegative testing disabled due to lack"
                                     " of exception support\n";
#endif
            }
            LOOP_ASSERT(deleteCount, 0 == deleteCount);
            LOOP_ASSERT(g_deleteCount, 0 == g_deleteCount);
        }

        if (verbose) cout << "\tTest runDeleter\n";

        ASSERT(0 == g_deleteCount);
        {
            bdema_ManagedPtr_Members members(0, 0, &countedNilDelete);
            ASSERT(0 == members.pointer());

            members.runDeleter();
            ASSERT(0 == g_deleteCount);

            int deleteCount = 0;
            MyTestObject obj(&deleteCount);
            members.set(&obj, 0, &countedNilDelete);

            members.runDeleter();
            ASSERT(0 == deleteCount);
            ASSERT(1 == g_deleteCount);

            struct local {
                int d_x;
                static void deleter(void *a, void *b)
                {
                    local * pThis = reinterpret_cast<local *>(a);
                    ASSERT(&pThis->d_x == b);
                    ASSERT(13 == pThis->d_x);
                    pThis->d_x = 42;
                }
            };

            local test = { 13 };
            members.set(&test, &test.d_x, &local::deleter);
            members.runDeleter();
            LOOP_ASSERT(test.d_x, 42 == test.d_x);

            local alias = { 99 };
            members.setAliasPtr(&alias);
            test.d_x = 13;
            members.runDeleter();
            LOOP_ASSERT(test.d_x, 42 == test.d_x)
        }
        g_deleteCount = 0;

        if (verbose) cout << "\tTest swap\n";
        // remember to set an alias pointer before swapping
        // investigate if current failure is significant
        {
            bdema_ManagedPtr_Members empty(0, 0, 0);
            ASSERT(0 == empty.pointer());
#ifdef BDE_BUILD_TARGET_EXC
            {
                bsls_AssertTestHandlerGuard guard;
                ASSERT_SAFE_FAIL(empty.deleter());
            }
#endif

            int x;
            double y;
            bdema_ManagedPtr_Members simple(&x, &y, &countedNilDelete);
            ASSERT(&x == simple.pointer());
            ASSERT(&x == simple.deleter().object());
            ASSERT(&y == simple.deleter().factory());
            ASSERT(&countedNilDelete == simple.deleter().deleter());

            simple.swap(simple);
            ASSERT(&x == simple.pointer());
            ASSERT(&x == simple.deleter().object());
            ASSERT(&y == simple.deleter().factory());
            ASSERT(&countedNilDelete == simple.deleter().deleter());

            empty.swap(simple);
            ASSERT(0 == simple.pointer());
            ASSERT(&x == empty.pointer());
            ASSERT(&x == empty.deleter().object());
            ASSERT(&y == empty.deleter().factory());
            ASSERT(&countedNilDelete == empty.deleter().deleter());
#ifdef BDE_BUILD_TARGET_EXC
            {
                bsls_AssertTestHandlerGuard guard;
                ASSERT_SAFE_FAIL(simple.deleter());
            }
#endif

            empty.swap(simple);
            ASSERT(0 == empty.pointer());
            ASSERT(&x == simple.pointer());
            ASSERT(&x == simple.deleter().object());
            ASSERT(&y == simple.deleter().factory());
            ASSERT(&countedNilDelete == simple.deleter().deleter());
#ifdef BDE_BUILD_TARGET_EXC
            {
                bsls_AssertTestHandlerGuard guard;
                ASSERT_SAFE_FAIL(empty.deleter());
            }
#endif

            short s;
            float f;
            bdema_ManagedPtr_Members other(&f, &s, &countedNilDelete);
            ASSERT(&f == other.pointer());
            ASSERT(&f == other.deleter().object());
            ASSERT(&s == other.deleter().factory());
            ASSERT(&countedNilDelete == other.deleter().deleter());

            simple.swap(other);
            ASSERT(&x == other.pointer());
            ASSERT(&x == other.deleter().object());
            ASSERT(&y == other.deleter().factory());
            ASSERT(&countedNilDelete == other.deleter().deleter());
            ASSERT(&f == simple.pointer());
            ASSERT(&f == simple.deleter().object());
            ASSERT(&s == simple.deleter().factory());
            ASSERT(&countedNilDelete == simple.deleter().deleter());
        }

        ASSERT(dam.isInUseSame());
        ASSERT(gam.isInUseSame());
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // Testing default constructor and primary manipulators
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   bdema_ManagedPtr_Members()
        //   ~bdema_ManagedPtr_Members()
        // --------------------------------------------------------------------

        if (verbose) {
               cout << "\nTesting default constructor and primary manipulators"
                    << "\n----------------------------------------------------"
                    << endl;
        }

        if (verbose) cout << "\tTest class MyTestObject\n";

        bslma_TestAllocatorMonitor gam(&globalAllocator);
        bslma_TestAllocatorMonitor dam(&da);

        if (verbose) cout << "\tTest default constructor\n";

        {
            const bdema_ManagedPtr_Members empty;
            ASSERT(0 == empty.pointer());

#ifdef BDE_BUILD_TARGET_EXC
            if (verbose) cout << "\t\tNegative testing\n";

            {
                bsls_AssertTestHandlerGuard guard;

                ASSERT_SAFE_FAIL(empty.deleter());
            }
#else
            if (verbose) cout << "\tNegative testing disabled due to lack of"
                                 " exception support\n";
#endif
        }

        ASSERT(dam.isInUseSame());
        ASSERT(gam.isInUseSame());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING TEST MACHINERY
        //
        // Concerns:
        //: 1 'MyTestObject', 'MyDerivedObject' and 'MySecondDerivedObject'
        //:   objects do not allocate any memory from the default allocator nor
        //:   from the global allocator for any of their operations.
        //:
        //: 2 'MyTestObject', 'MyDerivedObject' and 'MySecondDerivedObject'
        //:   objects, created with a pointer to an integer, increment the
        //:   referenced integer exactly once when they are destroyed.
        //:
        //: 3 'MyTestObject', 'MyDerivedObject' and 'MySecondDerivedObject'
        //:   objects, created by copying another object of the same type,
        //:   increment the integer referenced by the original object, exactly
        //:   once, when they are destroyed.
        //:
        //: 4 'MyDerivedObject' is derived from 'MyTestObject'.
        //:
        //: 5 'MySecondDerivedObject' is derived from 'MyTestObject'.
        //:
        //: 6 'MyDerivedObject' is *not* derived from 'MySecondDerivedObject',
        //:   nor is 'MySecondDerivedObject' derived from 'MyDerivedObject'.
        //
        // Plan:
        //: 1 Install test allocator monitors to verify that neither the global
        //:   nor default allocators allocate any memory executing this test
        //:   case.
        //:
        //: 2 For each test-class type:
        //:   1 Initialize an 'int' counter to zero
        //:   2 Create a object of tested type, having the address of the 'int'
        //:     counter.
        //:   3 Confirm the test object 'deleterCounter' points to the 'int'
        //:     counter.
        //:   4 Confirm the 'int' counter value has not changed.
        //:   5 Destroy the test object and confirm the 'int' counter value
        //:     has incremented by exactly 1.
        //:   6 Create a second object of tested type, having the address of
        //:     the 'int' counter.
        //:   7 Create a copy of the second test object, and confirm both test
        //:     object's 'deleterCount' point to the same 'int' counter.
        //:   8 Confirm the 'int' counter value has not changed.
        //:   9 Destroy one test object, and confirm test 'int' counter is
        //:     incremented exactly once.
        //:  10 Destroy the other test object, and confirm test 'int' counter
        //:     is incremented exactly once.
        //:
        //: 3 For each test-class type:
        //:   1 Create a function overload set, where one function takes a
        //:     pointer to the test-class type and returns 'true', while the
        //:     other overload matches anything and returns 'false'.
        //:   2 Call each of the overloaded function sets with a pointer to
        //:     'int', and confirm each returns 'false'.
        //:   3 Call each of the overloaded function sets with a pointer to
        //:     an object of each of the test-class types, and confirm each
        //:     call returns 'true' only when the pointer type matches the
        //:     test-class type for that function, or points to a type publicly
        //:     derived from that test-class type.
        //:
        //: 4 Verify that no unexpected memory was allocated by inspecting the
        //:   allocator guards.
        //
        // Testing:
        //    class MyTestObject
        //    class MyDerivedObject
        //    class MySecondDerivedObject
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING TEST MACHINERY"
                          << "\n----------------------" << endl;

        if (verbose) cout << "\tTest class MyTestObject\n";

        bslma_TestAllocatorMonitor gam(&globalAllocator);
        bslma_TestAllocatorMonitor dam(&da);

        int destructorCount = 0;
        {
            MyTestObject mt(&destructorCount);
            ASSERT(&destructorCount == mt.deleteCounter());
            LOOP_ASSERT(destructorCount, 0 == destructorCount);
        }
        LOOP_ASSERT(destructorCount, 1 == destructorCount);

        destructorCount = 0;
        {
            MyTestObject mt1(&destructorCount);
            {
                MyTestObject mt2 = mt1;
                ASSERT(&destructorCount == mt1.deleteCounter());
                ASSERT(&destructorCount == mt2.deleteCounter());
                LOOP_ASSERT(destructorCount, 0 == destructorCount);
            }
            LOOP_ASSERT(destructorCount, 1 == destructorCount);
        }
        ASSERT(2 == destructorCount);

        if (verbose) cout << "\tTest class MyDerivedObject\n";

        destructorCount = 0;
        {
            MyDerivedObject dt(&destructorCount);
            ASSERT(&destructorCount == dt.deleteCounter());
            LOOP_ASSERT(destructorCount, 0 == destructorCount);
        }
        ASSERT(100 == destructorCount);

        destructorCount = 0;
        {
            MyDerivedObject dt1(&destructorCount);
            {
                MyDerivedObject dt2 = dt1;
                ASSERT(&destructorCount == dt1.deleteCounter());
                ASSERT(&destructorCount == dt2.deleteCounter());
                LOOP_ASSERT(destructorCount, 0 == destructorCount);
            }
            LOOP_ASSERT(destructorCount, 100 == destructorCount);
        }
        ASSERT(200 == destructorCount);

        if (verbose) cout << "\tTest class MySecondDerivedObject\n";

        destructorCount = 0;
        {
            MySecondDerivedObject st(&destructorCount);
            ASSERT(&destructorCount == st.deleteCounter());
            LOOP_ASSERT(destructorCount, 0 == destructorCount);
        }
        LOOP_ASSERT(destructorCount, 10000 == destructorCount);

        destructorCount = 0;
        {
            MySecondDerivedObject st1(&destructorCount);
            {
                MySecondDerivedObject st2 = st1;
                ASSERT(&destructorCount == st1.deleteCounter());
                ASSERT(&destructorCount == st2.deleteCounter());
                LOOP_ASSERT(destructorCount, 0 == destructorCount);
            }
            LOOP_ASSERT(destructorCount, 10000 == destructorCount);
       }
       ASSERT(20000 == destructorCount);

       if (verbose) cout << "\tTest pointer conversions\n";

       struct Local {
            static bool matchBase(MyTestObject *) { return true; }
            static bool matchBase(...) { return false; }

            static bool matchDerived(MyDerivedObject *) { return true; }
            static bool matchDerived(...) { return false; }

            static bool matchSecond(MySecondDerivedObject *) { return true; }
            static bool matchSecond(...) { return false; }
        };

        {
            int badValue;
            ASSERT(!Local::matchBase(&badValue));
            ASSERT(!Local::matchDerived(&badValue));
            ASSERT(!Local::matchSecond(&badValue));
        }

        {
            MyTestObject mt(&destructorCount);
            ASSERT(Local::matchBase(&mt));
            ASSERT(!Local::matchDerived(&mt));
            ASSERT(!Local::matchSecond(&mt));
        }

        {
            MyDerivedObject dt(&destructorCount);
            ASSERT(Local::matchBase(&dt));
            ASSERT(Local::matchDerived(&dt));
            ASSERT(!Local::matchSecond(&dt));
        }

        {
            MySecondDerivedObject st(&destructorCount);
            ASSERT(Local::matchBase(&st));
            ASSERT(!Local::matchDerived(&st));
            ASSERT(Local::matchSecond(&st));
        }

        ASSERT(dam.isInUseSame());
        ASSERT(gam.isInUseSame());

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   1. That the functions exist with the documented signatures.
        //   2. That the basic functionality works as documented.
        //
        // Plan:
        //   Exercise each function in turn and devise an elementary test
        //   sequence to ensure that the basic functionality is as documented.
        //
        // Testing:
        //   This test exercises basic functionality but *tests* *nothing*.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n--------------" << endl;

        cout << "Nothing tested yet." << endl;
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                                                                    << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
