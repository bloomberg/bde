// bslma_managedptr_members.t.cpp                                     -*-C++-*-
#include <bslma_managedptr_members.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_managedptr_factorydeleter.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                                  TEST PLAN
//                                  ---------
//-----------------------------------------------------------------------------
// [ 3] ManagedPtr_Members();
// [ 4] ManagedPtr_Members(ManagedPtr_Members& other);
// [ 4] ManagedPtr_Members(void *obj, void *factory, DeleterFunc del);
// [  ] ManagedPtr_Members(void *o, void *f, DeleterFunc d, void *alias);
// [ 3] ~ManagedPtr_Members();
// [  ] void clear();
// [ 4] void move(ManagedPtr_Members *other);
// [  ] void moveAssign(ManagedPtr_Members *other);
// [ 4] void set(void *object, void *factory, DeleterFunc deleter);
// [ 4] void setAliasPtr(void *ptr);
// [ 4] void swap(ManagedPtr_Members& other);
// [ 4] void runDeleter() const;
// [ 4] void *pointer() const;
// [ 4] const ManagedPtrDeleter& deleter() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] class MyTestObject
// [ 2] class MyDerivedObject
// [ 2] class MySecondDerivedObject

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                              TEST APPARATUS
// ----------------------------------------------------------------------------

namespace {

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

class MyTestObject;
class MyDerivedObject;

//=============================================================================
//                      HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

class MyTestObject {
    // This test-class serves three purposes.  It provides a base class for the
    // test classes in this test driver, so that derived -> base conversions
    // can be tested.  It also signals when its destructor is run by
    // incrementing an externally managed counter, supplied when each object is
    // created.  Finally, it exposes an internal data structure that can be use
    // to demonstrate the 'ManagedPtr' aliasing facility.

    // DATA
    volatile int *d_deleteCounter_p;
    mutable int   d_value[2];

  public:
    // CREATORS
    explicit MyTestObject(int *counter);
        // Create a 'MyTestObject' using the specified 'counter' to record when
        // this object's destructor is run.

    // Use compiler-generated copy constructor and assignment operator
    //  MyTestObject(const MyTestObject& other) = default;
    //  MyTestObject operator=(const MyTestObject& other) = default;

    virtual ~MyTestObject();
        // Destroy this object.

    // ACCESSORS
    volatile int *deleteCounter() const;
        // Return the address of the counter used to track when this object's
        // destructor is run.

    int *valuePtr(int index = 0) const;
        // Return the address of the value associated with the optionally
        // specified 'index', and the address of the first such object if no
        // 'index' is specified.
};

// CREATORS
MyTestObject::MyTestObject(int *counter)
: d_deleteCounter_p(counter)
, d_value()
{
}

MyTestObject::~MyTestObject()
{
    ++*d_deleteCounter_p;
}

// ACCESSORS
volatile int* MyTestObject::deleteCounter() const
{
    return d_deleteCounter_p;
}

inline
int *MyTestObject::valuePtr(int index) const
{
    BSLS_ASSERT_SAFE(2 > index);

    return d_value + index;
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
        // Create a 'MyDerivedObject' using the specified 'counter' to record
        // when this object's destructor is run.

    // Use compiler-generated copy constructor and assignment operator
    //  MyDerivedObject(const MyDerivedObject& other) = default;
    //  MyDerivedObject operator=(const MyDerivedObject& other) = default;

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
        // Create a 'MySecondDerivedObject' using the specified 'counter' to
        // record when this object's destructor is run.

    // Use compiler-generated copy constructor and assignment operator
    // MySecondDerivedObject(const MySecondDerivedObject& orig);
    // MySecondDerivedObject operator=(const MySecondDerivedObject& orig);

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
        // Create a 'CountedStackDeleter' using the specified 'counter' to
        // record when this object is invoked as a deleter.

    //! ~CountedStackDeleter();
        // Destroy this object.

    // ACCESSORS
    volatile int *deleteCounter() const { return d_deleteCounter_p; }
        // Return the address of the counter used to track when this object is
        // invoked as a deleter.

    void deleteObject(void *) const
        // Increment the stored reference to a counter to indicate that this
        // method has been called.
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
    // Increment the global delete counterer 'g_deleteCount'.
{
//    static int& deleteCount = g_deleteCount;
    ++g_deleteCount;
}

//=============================================================================
//                    FILE-STATIC FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static void doNothingDeleter(void *object, void *)
    // The behavior is undefined unless the specified 'object' pointer is not
    // null.  Otherwise, this function has no effect.
{
    ASSERT(object);
}

}  // close unnamed namespace

//=============================================================================
//                              TEST PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVerbose;           // suppress unused variable warning
    (void)veryVeryVerbose;       // suppress unused variable warning

    printf("TEST " __FILE__ " CASE %d\n", test);

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // Confirm no static intialization locekd the global allocator
    ASSERT(&globalAllocator == bslma::Default::globalAllocator());

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&da);

    // Confirm no static intialization locked the default allocator
    ASSERT(&da == bslma::Default::defaultAllocator());

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'ManagedPtr_Members'
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
        //    ManagedPtr_Members(void *obj, void *factory, DeleterFunc del);
        //    ManagedPtr_Members(ManagedPtr_Members& other);
        //    void move(ManagedPtr_Members *other);
        //    void set(void *object, void *factory, DeleterFunc deleter);
        //    void setAliasPtr(void *ptr);
        //    void swap(ManagedPtr_Members& other);
        //    void runDeleter() const;
        //    void *pointer() const;
        //    const ManagedPtrDeleter& deleter() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'ManagedPtr_Members'"
                            "\n============================\n");


        typedef bslma::ManagedPtr_FactoryDeleter<MyTestObject,
                                             CountedStackDeleter > TestFactory;

        bslma::TestAllocatorMonitor gam(&globalAllocator);
        bslma::TestAllocatorMonitor dam(&da);

        if (verbose) printf("\tTest default constructor\n");

        {
            const bslma::ManagedPtr_Members empty;
            ASSERT(0 == empty.pointer());
        }

        if (verbose) printf("\tTest value constructor\n");

        {
            const bslma::ManagedPtr_Members empty(0, 0, 0);
            ASSERT(0 == empty.pointer());

            int deleteCount = 0;
            {
                CountedStackDeleter del(&deleteCount);

                int x;
                const bslma::ManagedPtr_Members simple(&x,
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
                if (verbose) printf("\t\tNegative testing\n");

                {
                    bsls::AssertTestHandlerGuard guard;

                    const bslma::ManagedPtr_Members empty(0, 0, 0);
                    ASSERT_SAFE_FAIL(empty.deleter());

                    int x;
                    ASSERT_SAFE_FAIL(bslma::ManagedPtr_Members b(&x, &del, 0));
                    ASSERT_SAFE_PASS(bslma::ManagedPtr_Members g( 0, &del, 0));
                }
#else
                if (verbose) printf("\tNegative testing disabled due to lack"
                                    " of exception support\n");
#endif
            }
        }

        if (verbose) printf("\tTest set\n");

        {
            bslma::ManagedPtr_Members members(0, 0, 0);
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
            if (verbose) printf("\t\tNegative testing\n");

            {
                bsls::AssertTestHandlerGuard guard;
                ASSERT_SAFE_FAIL(members.deleter());
            }
#else
            if (verbose) printf("\tNegative testing disabled due to lack of"
                                 " exception support\n");
#endif

            {
                int deleteCount = 0;
                CountedStackDeleter del(&deleteCount);

#ifdef BDE_BUILD_TARGET_EXC
                if (verbose) printf("\t\tNegative testing\n");

                {
                    bsls::AssertTestHandlerGuard guard;
                    int x;
                    ASSERT_SAFE_FAIL(members.set(&x, &del, 0));
                    ASSERT_SAFE_PASS(members.set( 0, &del, 0));
                }
#else
                if (verbose) printf("\tNegative testing disabled due to lack"
                                     " of exception support\n");
#endif
            }

        }

        if (verbose) printf("\tTest setAliasPtr\n");

        {
            int deleteCount = 0;
            {
                CountedStackDeleter del(&deleteCount);

                int x;
                bslma::ManagedPtr_Members simple(&x, &del, &countedNilDelete);
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
                if (verbose) printf("\t\tNegative testing\n");

                {
                    bsls::AssertTestHandlerGuard guard;
                    ASSERT_SAFE_FAIL(simple.setAliasPtr(0));

                    simple.set(0, 0, 0);
                    ASSERT(0 == simple.pointer());
                    ASSERT_SAFE_FAIL(simple.deleter());

                    ASSERT_SAFE_FAIL(simple.setAliasPtr(&y));
                    ASSERT_SAFE_PASS(simple.setAliasPtr(0));
                }
#else
                if (verbose) printf("\tNegative testing disabled due to lack"
                                     " of exception support\n");
#endif
            }
            LOOP_ASSERT(deleteCount, 0 == deleteCount);
            LOOP_ASSERT(g_deleteCount, 0 == g_deleteCount);
        }

        if (verbose) printf("\tTest move constructor\n");

        {
            int deleteCount = 0;
            {
                CountedStackDeleter del(&deleteCount);

                int x;
                bslma::ManagedPtr_Members donor(&x, &del, &countedNilDelete);
                ASSERT(&x == donor.pointer());
                ASSERT(&x == donor.deleter().object());
                ASSERT(&del == donor.deleter().factory());
                ASSERT(&countedNilDelete == donor.deleter().deleter());

                bslma::ManagedPtr_Members sink(donor);
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

                bslma::ManagedPtr_Members sink2(sink);
                ASSERT(&y == sink2.pointer());
                ASSERT(&x == sink2.deleter().object());
                ASSERT(&del == sink2.deleter().factory());
                ASSERT(&countedNilDelete == sink2.deleter().deleter());
                ASSERT(0 == sink.pointer());
            }
            LOOP_ASSERT(deleteCount, 0 == deleteCount);
            LOOP_ASSERT(g_deleteCount, 0 == g_deleteCount);
        }

        if (verbose) printf("\tTest move\n");

        {
            int deleteCount = 0;
            {
                CountedStackDeleter del1(&deleteCount);
                CountedStackDeleter del2(&deleteCount);

                int x;
                double y;
                bslma::ManagedPtr_Members a(&x, &del1, &countedNilDelete);
                bslma::ManagedPtr_Members b(&y, &del2, &doNothingDeleter);

                ASSERT(&x == a.pointer());
                ASSERT(&x == a.deleter().object());
                ASSERT(&del1 == a.deleter().factory());
                ASSERT(&countedNilDelete == a.deleter().deleter());

                ASSERT(&y == b.pointer());
                ASSERT(&y == b.deleter().object());
                ASSERT(&del2 == b.deleter().factory());
                ASSERT(&doNothingDeleter == b.deleter().deleter());

                a.move(&b);
                ASSERT(&y == a.pointer());
                ASSERT(&y == a.deleter().object());
                ASSERT(&del2 == a.deleter().factory());
                ASSERT(&doNothingDeleter == a.deleter().deleter());

                ASSERT(0 == b.pointer());

                b.set(0, 0, 0);
                a.setAliasPtr(&x);

                b.move(&a);
                ASSERT(&x == b.pointer());
                ASSERT(&y == b.deleter().object());
                ASSERT(&del2 == b.deleter().factory());
                ASSERT(&doNothingDeleter == b.deleter().deleter());

                ASSERT(0 == a.pointer());

#ifdef BDE_BUILD_TARGET_EXC
                if (verbose) printf("\t\tNegative testing\n");

                {
                    bsls::AssertTestHandlerGuard guard;
                    ASSERT_SAFE_FAIL(b.move(&b));
                }
#else
                if (verbose) printf("\tNegative testing disabled due to lack"
                                     " of exception support\n");
#endif
            }
            LOOP_ASSERT(deleteCount, 0 == deleteCount);
            LOOP_ASSERT(g_deleteCount, 0 == g_deleteCount);
        }

        if (verbose) printf("\tTest runDeleter\n");

        ASSERT(0 == g_deleteCount);
        {
            bslma::ManagedPtr_Members members(0, 0, &countedNilDelete);
            ASSERT(0 == members.pointer());

            members.runDeleter();
            ASSERT(0 == g_deleteCount);

            int deleteCount = 0;
            MyTestObject obj(&deleteCount);
            members.set(&obj, 0, &countedNilDelete);

            members.runDeleter();
            ASSERT(0 == deleteCount);
            ASSERT(1 == g_deleteCount);

            struct Local {
                int d_x;
                static void deleter(void *a, void *b)
                    // 'ASSERT' that the 'd_x' data member of the 'Local'
                    // object pointed to by the specified 'a' has the specieid
                    // 'b' address, and currently has the value '13', then
                    // assign to that 'd_x' the value 42,  Note that this
                    // function provides a verifiable test condition that the
                    // 'deleter' function is executed when expected, without
                    // actually destroying any objects nor reclaiming any
                    // memory.
                {
                    Local * pThis = static_cast<Local *>(a);
                    ASSERT(&pThis->d_x == b);
                    ASSERT(13 == pThis->d_x);
                    pThis->d_x = 42;
                }
            };

            Local test = { 13 };
            members.set(&test, &test.d_x, &Local::deleter);
            members.runDeleter();
            LOOP_ASSERT(test.d_x, 42 == test.d_x);

            Local alias = { 99 };
            members.setAliasPtr(&alias);
            test.d_x = 13;
            members.runDeleter();
            LOOP_ASSERT(test.d_x, 42 == test.d_x)
        }
        g_deleteCount = 0;

        if (verbose) printf("\tTest swap\n");
        // remember to set an alias pointer before swapping
        // investigate if current failure is significant
        {
            bslma::ManagedPtr_Members empty(0, 0, 0);
            ASSERT(0 == empty.pointer());
#ifdef BDE_BUILD_TARGET_EXC
            {
                bsls::AssertTestHandlerGuard guard;
                ASSERT_SAFE_FAIL(empty.deleter());
            }
#endif

            int x;
            double y;
            bslma::ManagedPtr_Members simple(&x, &y, &countedNilDelete);
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
                bsls::AssertTestHandlerGuard guard;
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
                bsls::AssertTestHandlerGuard guard;
                ASSERT_SAFE_FAIL(empty.deleter());
            }
#endif

            short s;
            float f;
            bslma::ManagedPtr_Members other(&f, &s, &countedNilDelete);
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
        // TESTING DEFAULT CONSTRUCTOR AND PRIMARY MANIPULATORS
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   ManagedPtr_Members();
        //   ~ManagedPtr_Members();
        // --------------------------------------------------------------------

        if (verbose) {
            printf("\nTESTING DEFAULT CONSTRUCTOR AND PRIMARY MANIPULATORS"
                   "\n====================================================\n");
        }

        if (verbose) printf("\tTest class MyTestObject\n");

        bslma::TestAllocatorMonitor gam(&globalAllocator);
        bslma::TestAllocatorMonitor dam(&da);

        if (verbose) printf("\tTest default constructor\n");

        {
            const bslma::ManagedPtr_Members empty;
            ASSERT(0 == empty.pointer());

#ifdef BDE_BUILD_TARGET_EXC
            if (verbose) printf("\t\tNegative testing\n");

            {
                bsls::AssertTestHandlerGuard guard;

                ASSERT_SAFE_FAIL(empty.deleter());
            }
#else
            if (verbose) printf("\tNegative testing disabled due to lack of"
                                 " exception support\n");
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

        if (verbose) printf("\nTESTING TEST MACHINERY"
                            "\n======================\n");

        if (verbose) printf("\tTest class MyTestObject\n");

        bslma::TestAllocatorMonitor gam(&globalAllocator);
        bslma::TestAllocatorMonitor dam(&da);

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

        if (verbose) printf("\tTest class MyDerivedObject\n");

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

        if (verbose) printf("\tTest class MySecondDerivedObject\n");

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

        if (verbose) printf("\tTest pointer conversions\n");

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
        //   This test exercises basic functionality but *tests* *nothing*.
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
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        if (verbose) printf("Nothing tested yet.\n");
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}


// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
