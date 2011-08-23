// bdema_managedptr.t.cpp          -*-C++-*-

#include <bdema_managedptr.h>
#include <bslma_allocator.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslmf_assert.h>
#include <bsls_asserttest.h>

#include <bsl_algorithm.h>   // to test swap
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // memcpy()
#include <bsl_iostream.h>

//#define TEST_FOR_COMPILE_ERRORS

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//                             ---------
// The 'bdema_managedptr' component provides a small number of classes that
// combine to provide a common solution to the problem of managing and
// transferring ownership of a dynamically allocated object.  It further
// contains a number of private classes to supply important implementation
// details, while the test driver introduces a reasonable ammount of test
// machinery in order to carefully observe the correct handling of callbacks.
// We choose to test each class in turn, starting with the test machinery,
// according to their internal levelization in the component implementation.
//
// [ 2] Test machinery
// [ 3] imp. class bdema_ManagedPtr_Members
// [ 4] imp. class bdema_ManagedPtr_Ref
// [ 5] imp. class bdema_ManagedPtr_FactoryDeleter
//      class bdema_ManagedPtr
// [13] class bdema_ManagedPtrNilDeleter   [DEPRECATED]
// [14] class bdema_ManagedPtrNoOpDeleter
//
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
// We are testing a proctor class that makes sure that only one instance holds
// a copy of an allocated pointer, along with the necessary information to
// deallocate it properly (the deleter).  The primary goal of this test program
// is to ascertain that no resource ever gets leaked, i.e., that when the
// proctor is re-assigned or destroyed, the managed pointer gets deleted
// properly.  In addition, we must also make sure that all the conversion and
// aliasing machinery works as documented.  At last, we must also check that
// a 'bdema_ManagedPtr' acts exactly as a pointer wherever one is expected.
//-----------------------------------------------------------------------------
// [ 6] bdema_ManagedPtr();
// [ 6] bdema_ManagedPtr(bdema_ManagedPtr_Nullptr::Type);
// [ 6] template<class TARGET_TYPE> bdema_ManagedPtr(TARGET_TYPE *ptr);
// [ 9] bdema_ManagedPtr(bdema_ManagedPtr& original);
// [ 9] bdema_ManagedPtr(bdema_ManagedPtr_Ref<BDEMA_TYPE> ref);
// [10] bdema_ManagedPtr(bdema_ManagedPtr<OTHER> &alias, TYPE *ptr)
// [ 8] bdema_ManagedPtr(TYPE *ptr, FACTORY *factory)
// [ 8] bdema_ManagedPtr(TYPE *ptr, void *factory,void(*deleter)(TYPE*, void*))
// [ 6] ~bdema_ManagedPtr();
// [ 9] operator bdema_ManagedPtr_Ref<OTHER>();
// [ 7] void load(nullptr_t=0);
// [ 7] template<class TARGET_TYPE> void load(TARGET_TYPE *ptr);
// [ 7] void load(TYPE *ptr, FACTORY *factory)
// [ 7] void load(TYPE *ptr, void *factory, void (*deleter)(TYPE *, void*));
// [ 7] void load(TYPE *ptr, FACTORY *factory, void(*deleter)(TYPE *,FACTORY*))
// [10] void loadAlias(bdema_ManagedPtr<OTHER> &alias, TYPE *ptr)
// [12] void swap(bdema_ManagedPt& rhs);
// [12] bdema_ManagedPtr& operator=(bdema_ManagedPtr &rhs);
// [12] bdema_ManagedPtr& operator=(bdema_ManagedPtr_Ref<BDEMA_TYPE> ref);
// [13] void clear();
// [13] bsl::pair<TYPE*,bdema_ManagedPtrDeleter> release();
// [11] operator BoolType() const;
// [11] TYPE& operator*() const;
// [11] TYPE *operator->() const;
// [11] TYPE *ptr() const;
// [11] const bdema_ManagedPtrDeleter& deleter() const;
//
// [14] class bdema_ManagedPtrNilDeleter
// [15] class bdema_ManagedPtrNoOpDeleter
//
// [ 3] imp. class bdema_ManagedPtr_Members
// [ 4] imp. class bdema_ManagedPtr_Ref             (this one needs negative testing)
// [ 5] imp. class bdema_ManagedPtr_FactoryDeleter  (this one needs negative testing)
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] TESTING TEST MACHINERY
// [15] CASTING EXAMPLE
// [16] USAGE EXAMPLE
// [-1] VERIFYING FAILURES TO COMPILE

namespace {

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
int testStatus = 0;

void aSsErT(int c, const char *s, int i) {
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

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
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
// AJM: MIGRATE THIS to the 'bsls_asserttest' component after BDE 2.9 branches.
class bsls_AssertTestHandlerGuard {
    // Purpose

    // DATA
    bsls_AssertFailureHandlerGuard d_guard;

  public:
    bsls_AssertTestHandlerGuard();

    //! ~bsls_AssertTestHandlerGuard() = default;
};

inline
bsls_AssertTestHandlerGuard::bsls_AssertTestHandlerGuard()
: d_guard(&bsls_AssertTest::failTestDriver)
{
}

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

bool verbose;

class MyTestObject;
class MyDerivedObject;
class MySecondDerivedObject;

typedef MyTestObject TObj;
typedef bdema_ManagedPtr<MyTestObject> Obj;
typedef bdema_ManagedPtr<const MyTestObject> CObj;
typedef MyDerivedObject TDObj;
typedef bdema_ManagedPtr<MyDerivedObject> DObj;
typedef bdema_ManagedPtr<const MyDerivedObject> CDObj;
typedef MySecondDerivedObject TSObj;
typedef bdema_ManagedPtr<MySecondDerivedObject> SObj;
typedef bdema_ManagedPtr<const MySecondDerivedObject> CSObj;
typedef bdema_ManagedPtr<void> VObj;

//=============================================================================
//                         HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

class MyTestObject {
    // This test-class serves three purposes.  It provides a base class for the
    // test classes in this test driver, so that derived -> base conversions
    // can be tested.  It also signals when its destructor is run by
    // incrementing an externally managed counter, supplied when each object
    // is created.  Finally, it exposes an internal data structure that can be
    // use to demonstate the 'bdema_ManagedPtr' aliasing facility.

    volatile int *d_deleteCounter_p;
    mutable int   d_value[2];

  public:
    MyTestObject(int *counter);

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
    MyDerivedObject(int *counter);
    // Use compiler-generated copy and destruction
};

inline
MyDerivedObject::MyDerivedObject(int *counter)
: MyTestObject(counter)
{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class MySecondDerivedObject : public MyTestObject
{
    // This test-class has the same destructor-counting behavior as 
    // 'MyTestObject', but offers a second, distinct, derived class in order to
    // test correct behavior when handling derived->base conversions.

  public:
    MySecondDerivedObject(int *counter);
    // Use compiler-generated copy and destruction
};

inline
MySecondDerivedObject::MySecondDerivedObject(int *counter)
: MyTestObject(counter)
{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class CountedStackDeleter
{
    volatile int *d_deleteCounter_p;

    CountedStackDeleter(const CountedStackDeleter& orig); //=delete;
    CountedStackDeleter& operator=(const CountedStackDeleter& orig); //=delete;

  public:
    CountedStackDeleter(int *counter) : d_deleteCounter_p(counter) {}

    //! ~CountedStackDeleter();
        // Destroy this object.

    // ACCESSORS
    volatile int *deleteCounter() const { return d_deleteCounter_p; }

    void deleteObject(void *) const {
        ++*d_deleteCounter_p;
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct IncrementIntFactory
{
    void deleteObject(int *object)
    {
        ASSERT(object);
        ++*object;
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int g_deleteCount = 0;

static void countedNilDelete(void *, void*) {
    static int& deleteCount = g_deleteCount;
    ++g_deleteCount;
}

template<class TARGET_TYPE>
static void templateNilDelete(TARGET_TYPE *, void*) {
    static int& deleteCount = g_deleteCount;
    ++g_deleteCount;
}

//=============================================================================
//                              CREATORS TEST
//=============================================================================

namespace CREATORS_TEST_NAMESPACE {

struct SS {
    char  d_buf[100];
    int  *d_numDeletes_p;

    SS(int *numDeletes) {
        d_numDeletes_p = numDeletes;
    }
    ~SS() {
        ++*d_numDeletes_p;
    }
};

typedef bdema_ManagedPtr<SS> SSObj;
typedef bdema_ManagedPtr<char> ChObj;

}  // close namespace CREATORS_TEST_NAMESPACE

//=============================================================================
//                    FILE-STATIC FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static void myTestDeleter(TObj *object, bslma_TestAllocator *allocator)
{
    allocator->deleteObject(object);
    if (verbose) {
        bsl::cout << "myTestDeleter called" << endl;
    }
}

static bdema_ManagedPtr<MyTestObject>
returnManagedPtr(int *numDels, bslma_TestAllocator *allocator)
{
    MyTestObject *p = new (*allocator) MyTestObject(numDels);
    bdema_ManagedPtr<MyTestObject> ret(p, allocator);
    return ret;
}

static bdema_ManagedPtr<MyDerivedObject>
returnDerivedPtr(int *numDels, bslma_TestAllocator *allocator)
{
    MyDerivedObject *p = new (*allocator) MyDerivedObject(numDels);
    bdema_ManagedPtr<MyDerivedObject> ret(p, allocator);
    return ret;
}

static bdema_ManagedPtr<MySecondDerivedObject>
returnSecondDerivedPtr(int *numDels, bslma_TestAllocator *allocator)
{
    MySecondDerivedObject *p = new (*allocator) MySecondDerivedObject(numDels);
    bdema_ManagedPtr<MySecondDerivedObject> ret(p, allocator);
    return ret;
}

static void doNothingDeleter(void *object, void *)
{
    ASSERT(object);
}

}  // close unnamed namespace

//=============================================================================
//                                CASTING EXAMPLE
//-----------------------------------------------------------------------------
namespace TYPE_CASTING_TEST_NAMESPACE {

    typedef MyTestObject A;
    typedef MyDerivedObject B;

///Type Casting
///------------
// 'bdema_ManagedPtr' objects can be implicitly and explicitly cast to
// different types in the same way as native pointers can.
//
///Implicit Casting
/// - - - - - - - -
// As with native pointers, a pointer of the type 'B' that is derived from the
// type 'A', can be directly assigned a 'bcema_SharedPtr' of 'A'.
// In other words, consider the following code snippets:
//..
    void implicitCastingExample() {
//..
// If the statements:
//..
        bslma_TestAllocator localDefaultTa;
        bslma_TestAllocator localTa;

        bslma_DefaultAllocatorGuard guard(&localDefaultTa);

        int numdels = 0;

        {
            B *b_p = 0;
            A *a_p = b_p;
    //..
    // are legal expressions, then the statements
    //..
            bdema_ManagedPtr<A> a_mp1;
            bdema_ManagedPtr<B> b_mp1;

            ASSERT(!a_mp1 && !b_mp1);

            a_mp1 = b_mp1;      // conversion assignment of nil ptr to nil
            ASSERT(!a_mp1 && !b_mp1);

            B *b_p2 = new (localDefaultTa) B(&numdels);
            bdema_ManagedPtr<B> b_mp2(b_p2);    // default allocator
            ASSERT(!a_mp1 && b_mp2);

            a_mp1 = b_mp2;      // conversion assignment of nonnil ptr to nil
            ASSERT(a_mp1 && !b_mp2);

            B *b_p3 = new (localTa) B(&numdels);
            bdema_ManagedPtr<B> b_mp3(b_p3, &localTa);
            ASSERT(a_mp1 && b_mp3);

            a_mp1 = b_mp3;      // conversion assignment of nonnil to nonnil
            ASSERT(a_mp1 && !b_mp3);

            a_mp1 = b_mp3;  // conversion assignment of nil to nonnil
            ASSERT(!a_mp1 && !b_mp3);

            // c'tor conversion init with nil
            bdema_ManagedPtr<A> a_mp4(b_mp3, b_mp3.ptr());
            ASSERT(!a_mp4 && !b_mp3);

            // c'tor conversion init with nonnil
            B *p_b5 = new (localTa) B(&numdels);
            bdema_ManagedPtr<B> b_mp5(p_b5, &localTa);
            bdema_ManagedPtr<A> a_mp5(b_mp5, b_mp5.ptr());
            ASSERT(a_mp5 && !b_mp5);
            ASSERT(a_mp5.ptr() == p_b5);

            // c'tor conversion init with nonnil
            B *p_b6 = new (localTa) B(&numdels);
            bdema_ManagedPtr<B> b_mp6(p_b6, &localTa);
            bdema_ManagedPtr<A> a_mp6(b_mp6);
            ASSERT(a_mp6 && !b_mp6);
            ASSERT(a_mp6.ptr() == p_b6);

            struct S {
                int d_i[10];
            };

#if 0
            S *pS = new (localTa) S;
            for (int i = 0; 10 > i; ++i) {
                pS->d_i[i] = i;
            }

            bdema_ManagedPtr<S> s_mp1(pS);
            bdema_ManagedPtr<int> i_mp1(pS, static_cast<int*>(pS.ptr()) + 4);
            ASSERT(4 == *i_mp1);
#endif

            ASSERT(2 == numdels);
        }

        ASSERT(4 == numdels);
    } // implicitCastingExample()
//..
//
///Explicit Casting
/// - - - - - - - -
// Through "aliasing", a managed pointer of any type can be explicitly cast
// to a managed pointer of any other type using any legal cast expression.
// For example, to static-cast a managed pointer of type A to a shared pointer
// of type B, one can simply do the following:
//..
    void explicitCastingExample() {

        bdema_ManagedPtr<A> a_mp;
        bdema_ManagedPtr<B> b_mp1(a_mp, static_cast<B*>(a_mp.ptr()));
        //..
        // or even use the less safe "C"-style casts:
        //..
        // bdema_ManagedPtr<A> a_mp;
        bdema_ManagedPtr<B> b_mp2(a_mp, (B*)(a_mp.ptr()));

    } // explicitCastingExample()
//..
// Note that when using dynamic cast, if the cast fails, the target managed
// pointer will be reset to an unset state, and the source will not be
// modified.  Consider for example the following snippet of code:
//..
    void processPolymorphicObject(bdema_ManagedPtr<A> aPtr,
                                  bool *castSucceeded)
    {
        bdema_ManagedPtr<B> bPtr(aPtr, dynamic_cast<B*>(aPtr.ptr()));
        if (bPtr) {
            ASSERT(!aPtr);
            *castSucceeded = true;
        }
        else {
            ASSERT(aPtr);
            *castSucceeded = false;
        }
    }
//..
// If the value of 'aPtr' can be dynamically cast to 'B*' then ownership is
// transferred to 'bPtr', otherwise 'aPtr' is to be modified.  As previously
// stated, the managed instance will be destroyed correctly regardless of how
// it is cast.

} // namespace TYPE_CASTING_TEST_NAMESPACE

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
namespace USAGE_EXAMPLE {

// What follows is a concrete example illustrating the alias concept.
// Let's say our array stores data acquired from a ticker
// plant accessible by a global 'getQuote()' function:
//..
    double getQuote() // From ticker plant. Simulated here
    {
        static const double QUOTES[] = {
            7.25, 12.25, 11.40, 12.00, 15.50, 16.25, 18.75, 20.25, 19.25, 21.00
        };
        static const int NUM_QUOTES = sizeof(QUOTES) / sizeof(QUOTES[0]);
        static int index = 0;

        double ret = QUOTES[index];
        index = (index + 1) % NUM_QUOTES;
        return ret;
    }
//..
// We now want to find the first quote larger than a specified threshold, but
// would also like to keep the earlier and later quotes for possible
// examination.  Our 'getFirstQuoteLargerThan' function must allocate memory
// for an array of quotes (the threshold and its neighbors).  It thus returns
// a managed pointer to the desired value:
//..
    const double END_QUOTE = -1;

    bdema_ManagedPtr<double>
    getFirstQuoteLargerThan(double threshold, bslma_Allocator *allocator)
    {
        ASSERT( END_QUOTE < 0 && 0 <= threshold );
//..
// We allocate our array with extra room to mark the beginning and end with a
// special 'END_QUOTE' value:
//..
        const int MAX_QUOTES = 100;
        int numBytes = (MAX_QUOTES + 2) * sizeof(double);
        double *quotes = (double*) allocator->allocate(numBytes);
        quotes[0] = quotes[MAX_QUOTES + 1] = END_QUOTE;
//..
// Then we read quotes until the array is full, keeping track of the first
// quote that exceeds the threshold.
//..
        double *finger = 0;

        for (int i = 1; i <= MAX_QUOTES; ++i) {
            double quote = getQuote();
            quotes[i] = quote;
            if (! finger && quote > threshold) {
                finger = &quotes[i];
            }
        }
//..
// Before we return, we create a managed pointer to the entire array:
//..
        bdema_ManagedPtr<double> managedQuotes(quotes, allocator);
//..
// Then we use the alias constructor to create a managed pointer that points
// to the desired value (the finger) but manages the entire array:
//..
        return bdema_ManagedPtr<double>(managedQuotes, finger);
    }
//..
// Our main program calls 'getFirstQuoteLargerThan' like this:
//..
    int usageExample1()
    {
        bslma_TestAllocator ta;
        bdema_ManagedPtr<double> result = getFirstQuoteLargerThan(16.00, &ta);
        ASSERT(*result > 16.00);
        ASSERT(1 == ta.numBlocksInUse());
        if (verbose) bsl::cout << "Found quote: " << *result << bsl::endl;
//..
// We also print the preceding 5 quotes in last-to-first order:
//..
        int i;
        if (verbose) bsl::cout << "Preceded by:";
        for (i = -1; i >= -5; --i) {
            double quote = result.ptr()[i];
            if (END_QUOTE == quote) {
                break;
            }
            ASSERT(quote < *result);
            if (verbose) bsl::cout << ' ' << quote;
        }
        if (verbose) bsl::cout << bsl::endl;
// To move the finger, e.g. to the last position printed, one must be careful
// to retain the ownership of the entire array.  Using the statement
// 'result.load(result.ptr()-i)' would be an error, because it would first
// compute the pointer value 'result.ptr()-i' of the argument, then release the
// entire array before starting to manage what has now become an invalid
// pointer.  Instead, 'result' must retain its ownership to the entire array,
// which can be attained by:
//..
        result.loadAlias(result, result.ptr()-i);
//..
// If we reset the result pointer, the entire array is deallocated:
//..
        result.clear();
        ASSERT(0 == ta.numBlocksInUse());
        ASSERT(0 == ta.numBytesInUse());

        return 0;
    }
//..

} // namespace USAGE_EXAMPLE

//=============================================================================
//                  TEST PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
                     verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma_TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma_Default::setGlobalAllocator(&globalAllocator);

    bslma_TestAllocator da("default", veryVeryVeryVerbose);
    bslma_Default::setDefaultAllocator(&da);

    switch (test) { case 0:
      case 17: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        using namespace USAGE_EXAMPLE;

        if (verbose) cout << "\nUSAGE EXAMPLE"
                          << "\n-------------" << endl;

        usageExample1();

      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING CONVERSION EXAMPLES
        //
        // Concerns
        //   Test casting of managed pointers, both when the pointer is null
        //   and when it is not.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        using namespace TYPE_CASTING_TEST_NAMESPACE;

        if (verbose) cout << "\nTYPE CASTING EXAMPLE"
                          << "\n--------------------" << endl;

        int numdels = 0;

        {
            implicitCastingExample();
            explicitCastingExample();

            bool castSucceeded;

            bslma_TestAllocator ta("object", veryVeryVeryVerbose);
            bslma_TestAllocatorMonitor tam(ta);

            processPolymorphicObject(returnManagedPtr(&numdels, &ta),
                                                               &castSucceeded);
            ASSERT(!castSucceeded);
            processPolymorphicObject(
                    bdema_ManagedPtr<A>(returnDerivedPtr(&numdels, &ta)),
                                                               &castSucceeded);
            ASSERT(castSucceeded);
            processPolymorphicObject(
                    bdema_ManagedPtr<A>(returnSecondDerivedPtr(&numdels, &ta)),
                                                               &castSucceeded);
            ASSERT(!castSucceeded);

            returnManagedPtr(&numdels, &ta);
            returnDerivedPtr(&numdels, &ta);
            returnSecondDerivedPtr(&numdels, &ta);
        }

        LOOP_ASSERT(numdels, 6 == numdels);
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING bdema_ManagedPtrNoOpDeleter
        //
        // Concerns:
        //: 1 The 'deleter' method can be used as a deleter policy by
        //:   'bdema_ManagedPtr'.
        //:
        //: 2 When invoked, 'bdema_ManagedPtrNoOpDeleter::deleter' has no
        //:   effect.
        //:
        //: 3 No memory is allocated from the global or default allocators.
        //
        // Plan:
        //: 1 blah ...
        //
        // Testing:
        //    bdema_ManagedPtrNoOpDeleter::deleter
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING bdema_ManagedPtrNoOpDeleter"
                          << "\n-----------------------------------" << endl;

        if (verbose) cout << "\tConfirm the deleter does not destroy the "
                             "passsed object\n";

        int deleteCount = 0;
        MyTestObject t(&deleteCount);
        bdema_ManagedPtrNoOpDeleter::deleter(&t, 0);
        LOOP_ASSERT(deleteCount, 0 == deleteCount);

        if (verbose) cout << "\tConfirm the deleter can be registered with "
                             "a managed pointer\n";

        bslma_TestAllocatorMonitor gam(globalAllocator);
        bslma_TestAllocatorMonitor dam(da);

        int x;
        int y;
        bdema_ManagedPtr<int> p(&x, 0, 
                                &bdema_ManagedPtrNoOpDeleter::deleter);

        p.load(&y, 0, &bdema_ManagedPtrNoOpDeleter::deleter);

        ASSERT(dam.isInUseSame());
        ASSERT(gam.isInUseSame());
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING bdema_ManagedPtrNilDeleter
        //
        // Concerns:
        //: 1 The 'deleter' method can be used as a deleter policy by
        //:   'bdema_ManagedPtr'.
        //:
        //: 2 When invoked, 'bdema_ManagedPtrNilDeleter<T>::deleter' has no
        //:   effect.
        //:
        //: 3 No memory is allocated from the global or default allocators.
        //
        // Plan:
        //: 1 blah ...
        //
        // Testing:
        //   bdema_ManagedPtrNilDeleter<T>::deleter
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING bdema_ManagedPtrNilDeleter"
                          << "\n----------------------------------" << endl;

        if (verbose) cout << "\tConfirm the deleter does not destroy the "
                             "passsed object\n";

        int deleteCount = 0;
        MyTestObject t(&deleteCount);
        bdema_ManagedPtrNilDeleter<MyTestObject>::deleter(&t, 0);
        LOOP_ASSERT(deleteCount, 0 == deleteCount);

        if (verbose) cout << "\tConfirm the deleter can be registered with "
                             "a managed pointer\n";

        bslma_TestAllocatorMonitor gam(globalAllocator);
        bslma_TestAllocatorMonitor dam(da);

        int x;
        int y;
        bdema_ManagedPtr<int> p(&x, 0, 
                                &bdema_ManagedPtrNilDeleter<int>::deleter);

        p.load(&y, 0, &bdema_ManagedPtrNilDeleter<int>::deleter);

        ASSERT(dam.isInUseSame());
        ASSERT(gam.isInUseSame());
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // CLEAR and RELEASE
        //
        // Concerns:
        //: 1 'clear' destroys the managed object (if any) and re-ininitializes
        //:   the managed pointer to an unset state.
        //:
        //: 2 'clear' destroys any managed object using the stored 'deleter'.
        //:
        //   That release works properly.
        //   Release gives up ownership of resources without running deleters
        //
        //   Test each function behaves correctly given one of the following
        //     kinds of managed pointer objects:
        //     empty
        //     simple
        //     simple with factory
        //     simple with factory and deleter
        //     aliased
        //     aliased (original created with factory)
        //     aliased (original created with factory and deleter)
        //
        // Plan:
        //   TBD...
        //
        // Tested:
        //   void clear();
        //   bsl::pair<TYPE*, bdema_ManagedPtrDeleter> release();
        //
        // ADD NEGATIVE TESTING FOR operator*()
        // --------------------------------------------------------------------

        using namespace CREATORS_TEST_NAMESPACE;

        int numDeletes = 0;
        {
            TObj *p = new (da) MyTestObject(&numDeletes);
            Obj o(p);

            ASSERT(0 == numDeletes);
            o.clear();
            ASSERT(1 == numDeletes);

            ASSERT(!o && !o.ptr());
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            TObj *p;
            {
                p = new (da) MyTestObject(&numDeletes);
                Obj o(p);

                ASSERT(p == o.release().first);
                ASSERT(0 == numDeletes);

                ASSERT(!o && !o.ptr());
            }

            ASSERT(0 == numDeletes);
            da.deleteObject(p);
        }
        ASSERT(1 == numDeletes);

        // testing 'release().second'
        numDeletes = 0;
        {
            TObj *p;
            {
                p =  new (da) MyTestObject(&numDeletes);
                Obj o(p);

                bdema_ManagedPtrDeleter d(o.deleter());
                bdema_ManagedPtrDeleter d2(o.release().second);
                ASSERT(0 == numDeletes);

                ASSERT(d.object()  == d2.object());
                ASSERT(d.factory() == d2.factory());
                ASSERT(d.deleter() == d2.deleter());
            }

            ASSERT(0 == numDeletes);
            da.deleteObject(p);
        }
        ASSERT(1 == numDeletes);
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // SWAP AND ASSIGN TEST
        //
        // Concerns:
        //   Test all varieties of load, swap function and all assignments.
        //
        //   (AJM concerns, not yet confirmed to be tested)
        //
        //   assign clears the pointer being assigned from
        //   self-assignment safe
        //   assign destroys held pointer, does not merely swap
        //   assign-with-null
        //   assign with aliased pointer
        //   assign from pointer with factory/deleter
        //   assign to pointer with factory/deleter/aliased-pointer
        //   assign from a compatible managed pointer type
        //      (e.g. ptr-to-derived, to ptr-to-base, ptr to ptr-to-const)
        //   any managed pointer can be assigned to 'bdema_ManagedPtr<void>'
        //   assign to/from an empty managed pointer, each of the cases above
        //   assigning incompatible pointers should fail to compile (hand test)
        //
        //   swap with self changes nothing
        //   swap two simple pointer exchanged pointer values
        //   swap two aliased pointer exchanges aliases as well as pointers
        //   swap a simple managed pointer with an empty managed pointer
        //   swap a simple managed pointer with an aliased managed pointer
        //   swap an aliased managed pointer with an empty managed pointer
        //
        //   REFORMULATION
        //   want to be sure assignment works correctly for all combinations of
        //   assigning from and to a managed pointer with each of the following
        //   states.  Similarly, want to swap with each possible combination of
        //   each of the following states:
        //     empty
        //     simple
        //     simple with factory
        //     simple with factory and deleter
        //     aliased
        //     aliased (original created with factory)
        //     aliased (original created with factory and deleter)
        //
        //  In addition, assignment supports the following that 'swap' does not
        //  assignment from temporary/rvalue must be supported
        //  assignment from 'compatible' managed pointer must be supported
        //    i.e. where raw pointers would be convertible under assignment
        //
        //: X No 'bdema_ManagedPtr' method should allocate any memory.
        // Plan:
        //   TBD...
        //
        //   Test the functions in the order in which they are declared in
        //   the ManagedPtr class.
        //
        // Tested:
        //   [Just because a function is tested, we do not (yet) confirm that
        //    the testing is adequate.]
        //   void swap(bdema_ManagedPtr<BDEMA_TYPE>& rhs);
        //   bdema_ManagedPtr& operator=(bdema_ManagedPtr &rhs);
        //   bdema_ManagedPtr& operator=(bdema_ManagedPtr_Ref<BDEMA_TYPE> ref);
        // --------------------------------------------------------------------

        using namespace CREATORS_TEST_NAMESPACE;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tTest operator bdema_ManagedPtr_Ref<OTHER>()\n";

        int numDeletes = 0;
        {
            TObj *p =  new (da) MyTestObject(&numDeletes);
            TObj *p2 = new (da) MyTestObject(&numDeletes);

            Obj o(p);
            Obj o2(p2);

            o.swap(o2);

            ASSERT(o.ptr() == p2);
            ASSERT(o2.ptr() == p);
        }
        LOOP_ASSERT(numDeletes, 2 == numDeletes);

        if (verbose) cout << "\t\tswap with null pointer\n";

        numDeletes = 0;
        {
            TObj *p =  new (da) MyTestObject(&numDeletes);

            Obj o(p);
            Obj o2;

            o.swap(o2);

            ASSERT(!o.ptr());
            ASSERT(o2.ptr() == p);
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            o.swap(o2);

            ASSERT(o.ptr() == p);
            ASSERT(!o2.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        if (verbose) cout << "\t\tswap deleters\n";

        numDeletes = 0;
        {
            bslma_TestAllocator ta1("object1", veryVeryVeryVerbose);
            bslma_TestAllocator ta2("object2", veryVeryVeryVerbose);

            TObj *p =  new (ta1) MyTestObject(&numDeletes);
            TObj *p2 = new (ta2) MyTestObject(&numDeletes);

            Obj o(p, &ta1);
            Obj o2(p2, &ta2);

            o.swap(o2);

            ASSERT(o.ptr() == p2);
            ASSERT(o2.ptr() == p);

            ASSERT(&ta2 == o.deleter().factory());
            ASSERT(&ta1 == o2.deleter().factory());
        }
        LOOP_ASSERT(numDeletes, 2 == numDeletes);

        if (verbose) cout << "\t\tswap aliases\n";

        numDeletes = 0;
        {
            bslma_TestAllocator ta1("object1", veryVeryVeryVerbose);
            bslma_TestAllocator ta2("object2", veryVeryVeryVerbose);

            int * p3 = new (ta2) int;
            *p3 = 42;

            TObj *p =  new (ta1) MyTestObject(&numDeletes);
            MyDerivedObject d2(&numDeletes);

            bdema_ManagedPtr<int> o3(p3, &ta2);
            Obj o(p, &ta1);
            Obj o2(o3, &d2);

            o.swap(o2);

            ASSERT( o.ptr() == &d2);
            ASSERT(o2.ptr() ==   p);

            ASSERT(p3 ==  o.deleter().object());
            ASSERT( p == o2.deleter().object());
            ASSERT(&ta2 ==  o.deleter().factory());
            ASSERT(&ta1 == o2.deleter().factory());
        }
        LOOP_ASSERT(numDeletes, 2 == numDeletes);

//#define BDEMA_MANAGEDPTR_COMPILE_FAIL_SWAP_FOR_DIFFERENT_TYPES
#if defined(BDEMA_MANAGEDPTR_COMPILE_FAIL_SWAP_FOR_DIFFERENT_TYPES)
            {
                // confirm that the various implicit conversions in this
                // component do not accidentally introduce a dangerous 'swap'.
                bdema_ManagedPtr<int> x;
                bdema_ManagedPtr<double> y;
                x.swap(y);  // should not compile
                y.swap(x);  // should not compile

                bdema_ManagedPtr<MyTestObject> b;
                bdema_ManagedPtr<MyDerivedObject> d;
                b.swap(d);  // should not compile
                d.swap(b);  // should not compile

                using std::swap;
                swap(x, y);  // should not compile
                swap(b, d);  // should not compile
            }
#endif
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tTest operator=(bdema_ManagedPtr &rhs)\n";

        numDeletes = 0;
        {
            TObj *p =  new (da) MyTestObject(&numDeletes);
            TObj *p2 = new (da) MyTestObject(&numDeletes);

            Obj o(p);
            Obj o2(p2);

            o = o2;

            ASSERT(!o2);
            ASSERT(1 == numDeletes);

            ASSERT(o.ptr() == p2);
        }
        ASSERT(2 == numDeletes);

        numDeletes = 0;
        {
            TObj *p =   new (da) MyTestObject(&numDeletes);
            TDObj *p2 = new (da) MyDerivedObject(&numDeletes);

            Obj o(p);
            DObj o2(p2);

            o = o2;

            ASSERT(!o2);
            ASSERT(1 == numDeletes);

            ASSERT(o.ptr() == p2);
        }
        ASSERT(2 == numDeletes);

        numDeletes = 0;
        {
            // this test tests creation of a ref from the same type of
            // managedPtr, then assignment to a managedptr.

            Obj o2;
            {
                TObj *p = new (da) MyTestObject(&numDeletes);
                Obj o(p);

                bdema_ManagedPtr_Ref<TObj> r = o;
                o2 = r;

                ASSERT(o2.ptr() == p);
            }
            ASSERT(0 == numDeletes);
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            TObj *p = new (da) MyTestObject(&numDeletes);
            Obj o(p);
            Obj o2;

            bdema_ManagedPtr_Ref<TObj> r = o;
            o2 = r;
            ASSERT(o2);
            ASSERT(!o);
            ASSERT(0 == numDeletes);

            bdema_ManagedPtr_Ref<TObj> r2 = o;
            o2 = r2;
            ASSERT(!o2);
            ASSERT(!o);

            ASSERT(1 == numDeletes);
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            TDObj *p = new (da) MyDerivedObject(&numDeletes);
            DObj o(p);
            Obj o2;

            bdema_ManagedPtr_Ref<TObj> r = o;
            o2 = r;
            ASSERT(o2);
            ASSERT(!o);
            ASSERT(0 == numDeletes);
        }
        ASSERT(1 == numDeletes);

//#define BDEMA_MANAGEDPTR_COMPILE_FAIL_ASSIGN_FROM_INCOMPATIBLE_TYPE
#if defined(BDEMA_MANAGEDPTR_COMPILE_FAIL_ASSIGN_FROM_INCOMPATIBLE_TYPE)
            {
                bdema_ManagedPtr<int> x;
                bdema_ManagedPtr<double> y;
                y = x;  // This should fail to compile.
            }
#endif
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING ACCESSORS
        //
        // Concerns:
        //   That all accessors work properly.  The 'ptr()' accessor has
        //   already been substantially tested in previous tests.
        //   The unspecified bool conversion evaluates as expected in all
        //     circumstances: if/while/for, (implied) operator!
        //   All accessors work on 'const'- qualified objects
        //   All accessors can be called for 'bdema_ManagedPtr<void>'
        //
        //: X No 'bdema_ManagedPtr' method should allocate any memory.
        //
        // Plan:
        //   TBD...
        //
        // Tested:
        //   operator BoolType() const;
        //   TYPE& operator*() const;
        //   TYPE *operator->() const;
        //   TYPE *ptr() const;
        //   const bdema_ManagedPtrDeleter& deleter() const;
        //
        // ADD NEGATIVE TESTING FOR operator*()
        // --------------------------------------------------------------------

        using namespace CREATORS_TEST_NAMESPACE;

        // testing 'deleter()' accessor and 'release().second'
        int numDeletes = 0;
        {
            TObj *p;
            {
                p =  new (da) MyTestObject(&numDeletes);
                Obj o(p);

                bdema_ManagedPtrDeleter d(o.deleter());
                bdema_ManagedPtrDeleter d2(o.release().second);
                ASSERT(0 == numDeletes);

                ASSERT(d.object()  == d2.object());
                ASSERT(d.factory() == d2.factory());
                ASSERT(d.deleter() == d2.deleter());
            }

            ASSERT(0 == numDeletes);
            da.deleteObject(p);
        }
        ASSERT(1 == numDeletes);

        {
            bsls_Types::Int64 numDeallocation = da.numDeallocation();
            numDeletes = 0;
            {
                SS *p = new (da) SS(&numDeletes);
                std::strcpy(p->d_buf, "Woof meow");

                SSObj s(p);

                // testing * and -> references
                ASSERT(!strcmp(&(*s).d_buf[5], "meow"));
                ASSERT(!strcmp(&s->d_buf[5],   "meow"));
            }
            ASSERT(da.numDeallocation() == numDeallocation + 1);
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // ALIAS SUPPORT TEST
        //
        // Concerns:
        //   managed pointer can hold an alias
        //   'ptr()' returns the alias pointer, and not the managed pointer
        //   correct deleter is run when an aliased pointer is destroyed
        //   appropriate object is cleared/deleters run when assigning to/from an aliased managed pointer
        //   a managed pointer can alias itself
        //   alias type need not be the same as the managed type (often isn't)
        //   aliasing a null pointer clears the managed pointer, releasing any previously held object
        //
        //: X No 'bdema_ManagedPtr' method should allocate any memory.
        //
        // Plan:
        //   TBD...
        //
        // Tested:
        //   bdema_ManagedPtr(bdema_ManagedPtr<OTHER> &alias, TYPE *ptr)
        //   void loadAlias(bdema_ManagedPtr<OTHER> &alias, TYPE *ptr)
        // --------------------------------------------------------------------

        using namespace CREATORS_TEST_NAMESPACE;

        int numDeletes = 0;
        {
            SS *p = new (da) SS(&numDeletes);
            std::strcpy(p->d_buf, "Woof meow");

            SSObj s(p);
            ChObj c(s, &p->d_buf[5]);

            ASSERT(!s); // should not be testing operator! until test 13

            ASSERT(!std::strcmp(c.ptr(), "meow"));

            ASSERT(0 == numDeletes);
        }
        ASSERT(1 == numDeletes);


        bsls_Types::Int64 numDeallocation = da.numDeallocation();
        numDeletes = 0;
        {
            SS *p = new (da) SS(&numDeletes);
            std::strcpy(p->d_buf, "Woof meow");
            char *pc = (char *) da.allocate(5);
            std::strcpy(pc, "Werf");

            SSObj s(p);
            ChObj c(pc);

            ASSERT(da.numDeallocation() == numDeallocation);
            c.loadAlias(s, &p->d_buf[5]);
            ASSERT(da.numDeallocation() == numDeallocation + 1);

            ASSERT(!s); // should not be testing operator! until test 13

            ASSERT(!std::strcmp(c.ptr(), "meow"));
        }
        ASSERT(da.numDeallocation() == numDeallocation + 2);
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // MOVE-SEMANTICS
        //
        // Concerns:
        //: 1 No constructor nor conversion operator allocates any memory from
        //:   the default or global allocators.
        //:
        //: 2 Each constructor takes ownership of the passed managed object.
        //:
        //: 3 Move construction for lvalues directly invokes the copy/move-
        //:   constructor (or constructor template) to take ownership away from
        //:   the source managed pointer object.
        //:
        //: 4 Move semantics for temporary objects (rvalues) are supported
        //:   through an implicit conversion to 'bdema_ManagedPtr_Ref', and
        //:   the single argument (implicit) constructor taking ownership from
        //:   such a managed reference.
        //:
        //: 5 const-qualified objects cannot be moved from (compile-fail test).
        //:
        //: 6 Both lvalue and rvalue objects of 'bdema_ManagedPtr' types can
        //:   implicitly convert to a 'bdema_ManagedPtr_Ref' of any compatible
        //:   type, i.e. where a pointer to the specified '_Ref' type may be
        //:   converted from a pointer to the specified 'Managed' type.
        //
        // Plan:
        //   TBD...
        //
        // Tested:
        //   operator bdema_ManagedPtr_Ref<BDEMA_OTHER_TYPE>();
        //   bdema_ManagedPtr(bdema_ManagedPtr_Ref<BDEMA_TYPE> ref);
        //   bdema_ManagedPtr(bdema_ManagedPtr &original);
        // --------------------------------------------------------------------

        using namespace CREATORS_TEST_NAMESPACE;

        bslma_TestAllocator ta("object", veryVeryVeryVerbose);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tTest operator bdema_ManagedPtr_Ref<OTHER>()\n";

        LOOP_ASSERT(g_deleteCount, 0 == g_deleteCount);
        {
            bslma_TestAllocatorMonitor gam(globalAllocator);
            bslma_TestAllocatorMonitor dam(da);

            int numDeletes = 0;

            {
                TObj x(&numDeletes);
                Obj  o(&x, 0, countedNilDelete);

                bdema_ManagedPtr_Ref<TObj> r = o;
                LOOP_ASSERT(g_deleteCount, 0 == g_deleteCount);
                ASSERT(0 == numDeletes);

                ASSERT(&x == r.base()->pointer());
                ASSERT(&x == r.base()->deleter().object());
                ASSERT(0 == r.base()->deleter().factory());
                ASSERT(&countedNilDelete == r.base()->deleter().deleter());
            }
            LOOP_ASSERT(g_deleteCount, 1 == g_deleteCount);
            ASSERT(1 == numDeletes);
            ASSERT(dam.isInUseSame());
            ASSERT(dam.isMaxSame());
            ASSERT(gam.isInUseSame());
            ASSERT(gam.isMaxSame());

            g_deleteCount = 0;
            numDeletes = 0;
            {
                // To test conversion from an rvalue, we must bind the
                // the temporary to a function argument in order to prolong the
                // lifetime of the temporary until after testing is complete.
                // We must bind the temporary to a 'bdema_ManagedPtr_Ref' and
                // not a whole 'bdema_ManagedPtr' because we are testing an
                // implementation detail of that move-constructor that would be
                // invoked.
                struct local {
                    static void test(void * px,
                                     bdema_ManagedPtr_Ref<TObj> r) {
                        LOOP_ASSERT(g_deleteCount, 0 == g_deleteCount);

                        ASSERT(px == r.base()->pointer());
                        ASSERT(px == r.base()->deleter().object());
                        ASSERT(0 == r.base()->deleter().factory());
                        ASSERT(&countedNilDelete == r.base()->deleter().deleter());
                    }
                };

                TObj x(&numDeletes);
                local::test( &x, (Obj(&x, 0, countedNilDelete)));
            }
            LOOP_ASSERT(g_deleteCount, 1 == g_deleteCount);
            ASSERT(1 == numDeletes);
            ASSERT(dam.isInUseSame());
            ASSERT(dam.isMaxSame());
            ASSERT(gam.isInUseSame());
            ASSERT(gam.isMaxSame());

//#define BDEMA_MANAGEDPTR_COMPILE_FAIL_CONVERT_TO_REF_FROM_CONST
#if defined(BDEMA_MANAGEDPTR_COMPILE_FAIL_CONVERT_TO_REF_FROM_CONST)
            {
                TObj x(&numDeletes);
                const Obj o(&x, 0, countedNilDelete);

                bdema_ManagedPtr_Ref<TObj> r = o;   // should not compile
                LOOP_ASSERT(g_deleteCount, 0 == g_deleteCount);
                ASSERT(0 == numDeletes);
            }
#endif
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tbdema_ManagedPtr(bdema_ManagedPtr &donor)\n";

        {
            bslma_TestAllocatorMonitor gam(globalAllocator);
            bslma_TestAllocatorMonitor dam(da);

            g_deleteCount = 0;
            int numDeletes = 0;
            {
                TObj x(&numDeletes);
                Obj  o(&x, 0, countedNilDelete);
                ASSERT(&x == o.ptr());

                Obj o2(o);
                ASSERT( 0 ==  o.ptr());
                ASSERT(&x == o2.ptr());
                ASSERT(&x == o2.deleter().object());
                ASSERT( 0 == o2.deleter().factory());
                ASSERT(&countedNilDelete == o2.deleter().deleter());
            }

            LOOP_ASSERT(g_deleteCount, 1 == g_deleteCount);
            ASSERT(1 == numDeletes);
            ASSERT(dam.isInUseSame());
            ASSERT(dam.isMaxSame());
            ASSERT(gam.isInUseSame());
            ASSERT(gam.isMaxSame());

            g_deleteCount = 0;
            numDeletes = 0;
            {
                TObj x(&numDeletes);
                Obj  o = Obj(&x, 0, countedNilDelete);
                ASSERT(&x == o.ptr());

                Obj o2(o);
                ASSERT( 0 ==  o.ptr());
                ASSERT(&x == o2.ptr());
                ASSERT(&x == o2.deleter().object());
                ASSERT( 0 == o2.deleter().factory());
                ASSERT(&countedNilDelete == o2.deleter().deleter());
            }

            LOOP_ASSERT(g_deleteCount, 1 == g_deleteCount);
            ASSERT(1 == numDeletes);
            ASSERT(dam.isInUseSame());
            ASSERT(dam.isMaxSame());
            ASSERT(gam.isInUseSame());
            ASSERT(gam.isMaxSame());

//#define BDEMA_MANAGEDPTR_COMPILE_FAIL_MOVE_CONSTRUCT_FROM_CONST
#if defined(BDEMA_MANAGEDPTR_COMPILE_FAIL_MOVE_CONSTRUCT_FROM_CONST)
            {
                TObj x(&numDeletes);
                const Obj  o(&x, 0, countedNilDelete);
                ASSERT(&x == o.ptr());

                Obj o2(o);  // should not compile
                ASSERT(!"The preceding line should not have compiled");
            }
#endif
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout <<
             "\tTest bdema_ManagedPtr(bdema_ManagedPtr_Ref<BDEMA_TYPE> ref)\n";

        int numDeletes = 0;
        {
            // this cast tests both a cast while creating the ref,
            // and the constructor from a ref.

            TDObj *p = new (da) MyDerivedObject(&numDeletes);
            DObj o(p);

            ASSERT(o);
            ASSERT(o.ptr() == p);

            bdema_ManagedPtr_Ref<TObj> r = o;
            ASSERT(o);
            Obj o2(r);

            ASSERT(!o && !o.ptr()); // should not be testing operator! until test 13
            ASSERT(0 == numDeletes);

            ASSERT(o2.ptr() == p);
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            TDObj *p = new (da) MyDerivedObject(&numDeletes);
            DObj d(p);
            ASSERT(d.ptr() == p);

            Obj o(d);
            ASSERT(o.ptr() == p);
            ASSERT(0 == d.ptr());
        }
        ASSERT(1 == numDeletes);

// examples to demonstrate:
        // Moving from lvalues:
        //   derived->base
        //   no-cv -> const
        //   anything -> void
        //
        // Moving from rvalues:
        //   as above, plus...
        //   rvalue of same type

//#define BDEMA_TEST_COMPILE_TIME_FAIL_INCOMPATIBLE_POINTERS
#if defined BDEMA_TEST_COMPILE_TIME_FAIL_INCOMPATIBLE_POINTERS
        {
            int x;
            bdema_ManagedPtr<int> i_ptr(&x, 0, &countedNilDelete);
            bdema_ManagedPtr<double> d_ptr(i_ptr);

            struct local_factory {
                static bdema_ManagedPtr<double> exec() {
                    return bdema_ManagedPtr<double>();
                }
            };

            bdema_ManagedPtr<long> l_ptr(local_factory::exec());
        }

        // Additional failures to demonstrate
        //   base -> derived type (a likely user error)
        //   const -> non-const
        //   void -> anything but void
#endif
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // CREATORS WITH FACTORY OR DELETER
        //
        // Concerns:
        //: 1 No constructor allocates any memory from the default or global
        //:   allocators.
        //:
        //: 2 Each constructor takes ownership of the passed managed object.
        //:
        //: 3 Each constructor establishes the supplied 'deleter', unless the
        //:   specified managed object has a null pointer value.
        //:
        //: 4 Each constructor ASSERTs in appropriate build modes when passed
        //:   a null pointer for the deleter, but a non-null pointer to the
        //:   managed object.
        //:
        //: 5 It must be possible to pass a null-pointer constant for the
        //:   'factory' argument when the specified 'deleter' will use only the
        //:   managed pointer value.
        //
        //   Exercise each declared constructors of ManagedPtr (other than
        //   those already tested in an earlier test case; those constructors
        //   that implement move semantics; and the constructor that enables
        //   aliasing).  Note that the primary accessor, 'ptr', cannot be
        //   considered to be validated until after testing the alias support,
        //   see test case 11.
        //
        // Plan:
        //   TBD...
        //
        //   Go through the constructors in the order in which they are
        //   declared in the ManagedPtr class and exercise all of them,
        //   exercising the ManagedPtrRef class when need to exercise the
        //   ManagedPtr class.
        //   Remember to pass '0' as a null-pointer literal to all arguments
        //   that accept pointers (with negative testing if that is out of
        //   contract).
        //
        // Tested:
        //   bdema_ManagedPtr(BDEMA_TYPE *ptr, FACTORY *factory)
        //   bdema_ManagedPtr(BDEMA_TYPE *, void *, DeleterFunc);
        //   bdema_ManagedPtr(BDEMA_TYPE *,
        //                    void *, 
        //                    void(*)(BDEMA_TYPE *, FACTORY *))
        //   bdema_ManagedPtr(BDEMA_TYPE *ptr,
        //                    bdema_ManagedPtr_Nullptr::Type,
        //                    void(*)(BDEMA_TYPE *, void*));
        // --------------------------------------------------------------------

        using namespace CREATORS_TEST_NAMESPACE;

        bslma_TestAllocator ta("object", veryVeryVeryVerbose);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tTest "
                             "bdema_ManagedPtr(TYPE *ptr, FACTORY *factory)\n";

        int numDeletes = 0;
        {
            bslma_TestAllocatorMonitor gam(globalAllocator);
            bslma_TestAllocatorMonitor dam(da);

            {
                bslma_TestAllocatorMonitor tam(ta);

                TObj *p = new (ta) MyTestObject(&numDeletes);
                Obj o(p, &ta);

                TObj *q = o.ptr();
                LOOP2_ASSERT(p, q, p == q);
            }
            ASSERT(dam.isInUseSame());
            ASSERT(dam.isMaxSame());
            ASSERT(gam.isInUseSame());
            ASSERT(gam.isMaxSame());
        }
        ASSERT(1 == numDeletes);

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) cout << "\tNegative testing\n";

        {
            bsls_AssertTestHandlerGuard guard;

            bslma_Allocator * pNullAlloc = 0;
            TObj *p = new (ta) MyTestObject(&numDeletes);
            ASSERT_SAFE_FAIL_RAW(Obj x(p, pNullAlloc));
            ASSERT_SAFE_PASS_RAW(Obj y(p, &ta));
            ASSERT_SAFE_PASS_RAW(Obj z(0, pNullAlloc));

        }
#else
        if (verbose) cout << "\tNegative testing disabled due to lack of "
                             "exception support\n";
#endif

//#define BDEMA_MANAGEDPTR_TEST_NULL_FACTORY_COMPILE_FAIL
#if defined(BDEMA_MANAGEDPTR_TEST_NULL_FACTORY_COMPILE_FAIL)
        {
            int i = 0;
            bdema_ManagedPtr<int> x(&i, 0);
            bdema_ManagedPtr<int> x(0, 0);
        }
#endif
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout <<
                "\tTest bdema_ManagedPtr(BDEMA_TYPE *, void *, DeleterFunc)\n";

        numDeletes = 0;
        {
            bslma_TestAllocatorMonitor gam(globalAllocator);
            bslma_TestAllocatorMonitor dam(da);

            {
                Obj::DeleterFunc deleter = &bdema_ManagedPtr_FactoryDeleter<
                                   MyTestObject, bslma_TestAllocator>::deleter;

                bslma_TestAllocatorMonitor tam(ta);

                TObj *p = new (ta) MyTestObject(&numDeletes);
                Obj o(p, &ta, deleter);

                ASSERT(o.ptr() == p);
                ASSERT(o.deleter().object() == p);
                ASSERT(o.deleter().factory() == &ta);
                ASSERT(o.deleter().deleter() == deleter);
            }
            LOOP_ASSERT(numDeletes, 1 == numDeletes);

#ifdef BDE_BUILD_TARGET_EXC
            if (verbose) cout << "\t\tNegative testing\n";

            {
                bslma_TestAllocatorMonitor tam(ta);

                bsls_AssertTestHandlerGuard guard;

                Obj::DeleterFunc deleter = 0;
                TObj *p = new (ta) MyTestObject(&numDeletes);

                ASSERT_SAFE_FAIL_RAW(Obj o(p, &ta, deleter));
                ASSERT_SAFE_FAIL_RAW(Obj o(p, &ta, 0));

                // clean-up
                ta.deleteObject(p);
            }
#else
            if (verbose) cout << "\tNegative testing disabled due to lack of "
                                 "exception support\n";
#endif
            ASSERT(dam.isInUseSame());
            ASSERT(dam.isMaxSame());
            ASSERT(gam.isInUseSame());
            ASSERT(gam.isMaxSame());
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tTest bdema_ManagedPtr(BDEMA_TYPE *,"
                             " void *, "
                             " void(*)(BDEMA_TYPE *, FACTORY *))\n";

        numDeletes = 0;
        {
            bslma_TestAllocatorMonitor gam(globalAllocator);
            bslma_TestAllocatorMonitor dam(da);

            bslma_TestAllocatorMonitor tam(ta);
            {
                TObj *p = new (ta) MyTestObject(&numDeletes);
                Obj o(p, &ta, &myTestDeleter);
            }
            ASSERT(tam.isInUseSame());
            ASSERT(dam.isInUseSame());
            ASSERT(dam.isMaxSame());
            ASSERT(gam.isInUseSame());
            ASSERT(gam.isMaxSame());
        }
        ASSERT(1 == numDeletes);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tTest bdema_ManagedPtr(BDEMA_TYPE *ptr,"
                             " bdema_ManagedPtr_Nullptr::Type,"
                             " void(*)(BDEMA_TYPE *, void*));\n";

        numDeletes = 0;
        LOOP_ASSERT(g_deleteCount, 0 == g_deleteCount);
        {
            bslma_TestAllocatorMonitor tam(ta);

            MyTestObject obj(&numDeletes);
            Obj o(&obj, 0, &templateNilDelete<MyTestObject>);
        }
        ASSERT(1 == numDeletes);
        LOOP_ASSERT(g_deleteCount, 1 == g_deleteCount);

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // Testing 'load' overloads
        //
        // Concerns:
        //: 1 Calling 'load' on an empty managed pointer assigns ownership of 
        //:   the pointer passed as the argument.
        //:
        //: 2 Calling 'load' on a 'bdema_ManagedPtr' that owns a non-null
        //:   pointer destroys the referenced object, and takes ownership of
        //:   the new one.
        //:
        //: 3 Calling load with a null pointer, or no argument, causes a
        //:   'bdema_ManagedPtr' object to destroy any managed object, without
        //:   owning a new one.
        //:
        //: 4 'bdema_ManagedPtr<void>' can load a pointer to any other type,
        //:   owning the pointer and deducing a deleter that will correctly
        //:   destroy the pointed-to object.
        //:
        //: 5 'bdema_ManagedPtr<void>' can load a true 'void *' pointer only if
        //:   an appropriate factory or deleter function is also passed.  The
        //:   single argument 'load(void *) should fail to compile.
        //:
        //: 6 'bdema_ManagedPtr<const T>' can be loaded with a 'T *' pointer
        //:   (cv-qualification conversion).
        //:
        //: 7 'bdema_ManagedPtr<base>' can be loaded with a 'derived *' pointer
        //:   and the deleter will destroy the 'derived' type, even if the
        //:   'base' destructor is not virtual.
        //:
        //: 8 When 'bdema_ManagedPtr' is passed a single 'FACTORY *' argument,
        //:   the implicit deleter-function will destroy the pointed-to object
        //:   using the FACTORY::deleteObject (non-static) method.
        //:
        //: 9 'bslma_Allocator' serves as a valid FACTORY type.
        //:
        //:10 A custom type offering just the 'deleteObject' (non-virtual)
        //:   member function serves as a valid FACTORY type.
        //:
        //:11 A 'bdema_ManagedPtr' points to the same object as the pointer
        //:   passed to 'load'.  Note that this includes null pointers.
        //:
        //:12 Destroying a 'bdema_ManagedPtr' destroys any owned object using
        //:   the deleter mechanism supplied by 'load'.
        //:
        //:13 Destroying a bdema_ManagedPtr that does not own a pointer has
        //:   no observable effect.
        //:
        //:14 No 'bdema_ManagedPtr' method should allocate any memory.
        //:
        //:15 Defensive checks assert in safe build modes when passing null
        //:   pointers as arguments for factory or deleters, unless target
        //:   pointer is also null.
        //
        // Plan:
        //   take an empty pointer, and call each overload of load.
        //      confirm pointer is initially null
        //      confirm new pointer value is stored by 'ptr()'
        //      confirm destructor destroys target object
        //      be sure to pass both '0' and valid pointer values to each potential overload
        //   Write a pair of nested loops
        //     For each iteration, first create a default-constructed bdema_ManagedPtr
        //     Then call a load function (testing each overload by the first loop)
        //     Then, in inner loop, load a second pointer and verify first target is destroyed
        //     Then verify the new target is destroyed when test object goes out of scope.
        //
        // Tested:
        //   void load(bdema_ManagedPtr_Nullptr::Type =0);
        //   void load(BDEMA_TARGET_TYPE *ptr);
        //   void load(BDEMA_TYPE *ptr, FACTORY *factory);
        //   void load(BDEMA_TYPE *ptr, void *factory, DeleterFunc deleter);
        //   void load(BDEMA_TYPE *ptr,
        //             bdema_ManagedPtr_Nullptr::Type,
        //             void      (*deleter)(BDEMA_TYPE *, void*));
        //   void load(BDEMA_TYPE *ptr,
        //             FACTORY *factory,
        //             void(*deleter)(BDEMA_TYPE *,FACTORY*))
        //   ~bdema_ManagedPtr();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'load' overloads"
                          << "\n------------------------" << endl;

        using namespace CREATORS_TEST_NAMESPACE;

        if (verbose) cout << "\tTest bdema_ManagedPtr::load()\n";

        int numDeletes = 0;
        {
            if (veryVerbose) cout << "\tMyTestObject\n";

            Obj o;

            ASSERT(!o.ptr());
            ASSERT(0 == numDeletes);

            o.load();
            ASSERT(!o.ptr()); // should not be testing operator! until test 13

            ASSERT(0 == numDeletes);
        }
        ASSERT(0 == numDeletes);

        numDeletes = 0;
        {
            if (veryVerbose) cout << "\t\tload() a simple managed pointer\n";

            TObj *p = new (da) MyTestObject(&numDeletes);
            Obj o(p);

            ASSERT(o.ptr() == p);
            ASSERT(0 == numDeletes);

            o.load();

            ASSERT(!o); // should not be testing operator! until test 13
            ASSERT(1 == numDeletes);
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            if (veryVerbose) cout << "\tdema_ManagedPtr<void>\n";

            VObj o;

            ASSERT(!o.ptr());
            ASSERT(0 == numDeletes);

            o.load();
            ASSERT(!o.ptr()); // should not be testing operator! until test 13

            ASSERT(0 == numDeletes);
        }
        ASSERT(0 == numDeletes);

        numDeletes = 0;
        {
            if (veryVerbose) cout << "\t\tload() a simple managed pointer\n";

            TObj *p = new (da) MyTestObject(&numDeletes);
            VObj o(p);

            ASSERT(o.ptr() == p);
            ASSERT(0 == numDeletes);

            o.load();

            ASSERT(!o.ptr()); // should not be testing operator! until test 13
            ASSERT(1 == numDeletes);
        }
        ASSERT(1 == numDeletes);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tTest bdema_ManagedPtr::load(nullptr)\n";

        numDeletes = 0;
        {
            if (veryVerbose) cout << "\tMyTestObject\n";

            Obj o;

            ASSERT(!o.ptr());
            ASSERT(0 == numDeletes);

            o.load(0);
            ASSERT(!o.ptr()); // should not be testing operator! until test 13

            ASSERT(0 == numDeletes);
        }
        ASSERT(0 == numDeletes);

        numDeletes = 0;
        {
            if (veryVerbose) cout << "\t\tload(0) a simple managed pointer\n";

            TObj *p = new (da) MyTestObject(&numDeletes);
            Obj o(p);

            ASSERT(o.ptr() == p);
            ASSERT(0 == numDeletes);

            o.load(0);

            ASSERT(!o); // should not be testing operator! until test 13
            ASSERT(1 == numDeletes);
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            if (veryVerbose) cout << "\tdema_ManagedPtr<void>\n";

            VObj o;

            ASSERT(!o.ptr());
            ASSERT(0 == numDeletes);

            o.load(0);
            ASSERT(!o.ptr()); // should not be testing operator! until test 13

            ASSERT(0 == numDeletes);
        }
        ASSERT(0 == numDeletes);

        numDeletes = 0;
        {
            if (veryVerbose) cout << "\t\tload(0) a simple managed pointer\n";

            TObj *p = new (da) MyTestObject(&numDeletes);
            VObj o(p);

            ASSERT(o.ptr() == p);
            ASSERT(0 == numDeletes);

            o.load(0);

            ASSERT(!o.ptr()); // should not be testing operator! until test 13
            ASSERT(1 == numDeletes);
        }
        ASSERT(1 == numDeletes);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tTest bdema_ManagedPtr::load(T*)\n";

        numDeletes = 0;
        {
            if (veryVerbose) cout <<
                                    "\t\tload into an empty managed pointer\n";

            TObj *p = new (da) MyTestObject(&numDeletes);
            Obj o;

            ASSERT(!o.ptr());
            ASSERT(0 == numDeletes);

            o.load(p);

            ASSERT(o.ptr() == p);
            ASSERT(0 == numDeletes);
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            if (veryVerbose) cout <<
                                    "\t\tload into a simple managed pointer\n";

            TObj *p =  new (da) MyTestObject(&numDeletes);
            TObj *p2 = new (da) MyTestObject(&numDeletes);
            Obj o(p);

            ASSERT(o.ptr() == p);
            ASSERT(0 == numDeletes);

            o.load(p2);

            ASSERT(o.ptr() == p2);
            ASSERT(1 == numDeletes);
        }
        ASSERT(2 == numDeletes);

        numDeletes = 0;
        {
            if (veryVerbose) cout <<
                       "\t\tload derived type into a simple managed pointer\n";

            TObj *p =  new (da) MyTestObject(&numDeletes);
            TDObj *p2 = new (da) MyDerivedObject(&numDeletes);
            Obj o(p);

            ASSERT(o.ptr() == p);
            ASSERT(0 == numDeletes);

            o.load(p2, &da);

            ASSERT(o.ptr() == p2);
            ASSERT(1 == numDeletes);
        }
        ASSERT(2 == numDeletes);

        numDeletes = 0;
        {
            if (veryVerbose) cout <<
                                    "\t\tload into an empty void pointer\n";

            TObj *p = new (da) MyTestObject(&numDeletes);
            VObj o;

            ASSERT(!o.ptr());
            ASSERT(0 == numDeletes);

            o.load(p);

            ASSERT(o.ptr() == p);
            ASSERT(0 == numDeletes);
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            if (veryVerbose) cout <<
                                    "\t\tload into a non-empty void pointer\n";

            TObj *p =  new (da) MyTestObject(&numDeletes);
            TObj *p2 = new (da) MyTestObject(&numDeletes);
            VObj o(p);

            ASSERT(o.ptr() == p);
            ASSERT(0 == numDeletes);

            o.load(p2);

            ASSERT(o.ptr() == p2);
            ASSERT(1 == numDeletes);
        }
        ASSERT(2 == numDeletes);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tTest bdema_ManagedPtr::load(T*, factory)\n";

        numDeletes = 0;
        {
            typedef bdema_ManagedPtr<int> MyObj;

            int a = 0;
            IncrementIntFactory incrementer;

            {
                MyObj o;
                ASSERT(!o.ptr());
                ASSERT(0 == numDeletes);
                ASSERT(0 == a);

                o.load(&a, &incrementer);

                ASSERT(o.ptr() == &a);
                ASSERT(0 == numDeletes);
                ASSERT(0 == a);
            }

            ASSERT(1 == a);
        }
        ASSERT(0 == numDeletes);

        numDeletes = 0;
        {
            typedef bdema_ManagedPtr<int> MyObj;

            int a = 0;
            IncrementIntFactory incrementer;

            {
                MyObj o;
                ASSERT(!o.ptr());
                ASSERT(0 == numDeletes);
                ASSERT(0 == a);

                o.load(&a, &incrementer);

                ASSERT(o.ptr() == &a);
                ASSERT(0 == numDeletes);
                ASSERT(0 == a);

                // should reloading the same pointer assert?
                o.load(&a, &incrementer);

                ASSERT(o.ptr() == &a);
                ASSERT(0 == numDeletes);
                ASSERT(1 == a);
            }

            ASSERT(2 == a);
        }
        ASSERT(0 == numDeletes);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout <<
                   "\tTest bdema_ManagedPtr::load(T*, factory, DeleterFunc)\n";

        g_deleteCount = 0;
        numDeletes    = 0;
        {
            TObj *p =  new (da) MyTestObject(&numDeletes);
            MyTestObject b(&numDeletes);
            Obj o(p);

            ASSERT(o.ptr() == p);
            LOOP_ASSERT(g_deleteCount, 0 == g_deleteCount);
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            o.load(&b, 0, &countedNilDelete);

            ASSERT(o.ptr() == &b);
            LOOP_ASSERT(g_deleteCount, 0 == g_deleteCount);
            LOOP_ASSERT(numDeletes, 1 == numDeletes);
        }
        LOOP_ASSERT(g_deleteCount, 1 == g_deleteCount);
        LOOP_ASSERT(numDeletes, 2 == numDeletes);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tTest bdema_ManagedPtr::load(T*, factory,"
                             " deleter(T *, factory))\n";

        numDeletes = 0;
        {
            TObj *p =  new (da) MyTestObject(&numDeletes);
            TObj *p2 = new (da) MyTestObject(&numDeletes);
            Obj o(p);

            o.load(p2, &da, &myTestDeleter);

            ASSERT(1 == numDeletes);
        }
        ASSERT(2 == numDeletes);

        numDeletes = 0;
        {
            if (veryVerbose) cout <<
                       "\t\tload derived type into a simple managed pointer\n";

            typedef void (*DeleterFunc)(MyTestObject *, void *);
            DeleterFunc deleterFunc = (DeleterFunc) &myTestDeleter;

            TObj *p =  new (da) MyTestObject(&numDeletes);
            TObj *p2 = new (da) MyTestObject(&numDeletes);
            Obj o(p);

            o.load(p2, (void *) &da, deleterFunc);

            ASSERT(1 == numDeletes);
        }
        ASSERT(2 == numDeletes);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tTest bdema_ManagedPtr::load(T*, nullptr,"
                             " deleter(T *, void *)\n";

        numDeletes = 0;
        {
            typedef bdema_ManagedPtr<int> MyObj;

            int a = 0;

            MyObj o;
            o.load(&a, 0, &doNothingDeleter);
        }
        ASSERT(0 == numDeletes);

//#define BDEMA_MANAGEDPTR_TEST_COMPILE_FAIL_LOAD_INCOMPATIBLE_TYPE
#if defined(BDEMA_MANAGEDPTR_TEST_COMPILE_FAIL_LOAD_INCOMPATIBLE_TYPE)
        {
            int i = 0;
            bdema_ManagedPtr<double> x;
            x.load(&i);

            const double d = 0.0;
            x.load(&d);
        }
#endif
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // PRIMARY CREATORS TEST
        //   Note that we will not deem the destructor to be completely tested
        //   until the next test case, which tests the range of management
        //   strategies a bdema_ManagedPtr may hold.
        //
        // Concerns:
        //: 1 A default constructed 'bdema_ManagedPtr' does not own a pointer.
        //: 2 A default constructed 'bdema_ManagedPtr' does not allocate any
        //:   memory.
        //: 3 A 'bdema_ManagedPtr' takes ownership of a pointer passed as a
        //:   single argument to its constructor, and destroys the pointed-to
        //:   object in its destructor using the default allocator.  It does
        //:   not allocate any memory.
        //: 4 A 'bdema_ManagedPtr<base>' object created by passing a 'derived*'
        //:   pointer calls the 'derived' destructor when destroying the
        //:   managed object, regardless of whether the 'base' destructor is
        //:   declared as 'virtual'.  No memory is allocated by
        //:   'bdema_ManagedPtr'.
        //: 5 A 'bdema_ManagedPtr<void>' object created by passing a 'derived*'
        //:   pointer calls the 'derived' destructor when destroying the
        //:   managed object.  No memory is allocated by 'bdema_ManagedPtr'.
        //: 6 A 'bdema_ManagedPtr' taking ownership of a null pointer passed as
        //:   a single argument is equivalent to default construction; it does
        //:   not allocate any memory.
        //
        // Plan:
        //    TBD
        //
        // Tested:
        //   bdema_ManagedPtr();
        //   bdema_ManagedPtr(nullptr_t);
        //   template<TARGET_TYPE> bdema_ManagedPtr(TARGET_TYPE *ptr);
        //   template<TARGET_TYPE> bdema_ManagedPtr<void>(TARGET_TYPE *ptr);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING PRIMARY CREATORS"
                          << "\n------------------------" << endl;

        using namespace CREATORS_TEST_NAMESPACE;

        if (verbose) cout << "\tTest default constructor\n";

        int numDeletes = 0;
        {
            if (veryVerbose) cout << "\t\tBasic test object\n";

            bslma_TestAllocatorMonitor dam(da);
            Obj o;

            ASSERT(0 == o.ptr());
            ASSERT(dam.isTotalSame());
        }
        ASSERT(0 == numDeletes);

        numDeletes = 0;
        {
            if (veryVerbose) cout << "\t\tvoid type\n";

            bslma_TestAllocatorMonitor dam(da);
            bdema_ManagedPtr<void> o;

            ASSERT(0 == o.ptr());
            ASSERT(dam.isTotalSame());
        }
        ASSERT(0 == numDeletes);

        numDeletes = 0;
        {
            if (veryVerbose) cout << "\t\tconst-qualified int\n";

            bslma_TestAllocatorMonitor dam(da);
            bdema_ManagedPtr<const int> o;

            ASSERT(0 == o.ptr());
            ASSERT(dam.isTotalSame());
        }
        ASSERT(0 == numDeletes);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tTest constructing with a null pointer\n";

        numDeletes = 0;
        {
            if (veryVerbose) cout << "\t\tBasic test object\n";

            bslma_TestAllocatorMonitor dam(da);
            Obj o(0);

            ASSERT(0 == o.ptr());
            ASSERT(dam.isTotalSame());
        }
        ASSERT(0 == numDeletes);

        numDeletes = 0;
        {
            if (veryVerbose) cout << "\t\tvoid type\n";

            bslma_TestAllocatorMonitor dam(da);
            VObj o(0);

            ASSERT(0 == o.ptr());
            ASSERT(dam.isTotalSame());
        }
        ASSERT(0 == numDeletes);

        numDeletes = 0;
        {
            if (veryVerbose) cout << "\t\tconst-qualified int\n";

            bslma_TestAllocatorMonitor dam(da);
            bdema_ManagedPtr<const int> o(0);

            ASSERT(0 == o.ptr());
            ASSERT(dam.isTotalSame());
        }
        ASSERT(0 == numDeletes);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tTest the single owned-pointer constructor\n";

        numDeletes = 0;
        {
            if (veryVerbose) cout << "\t\tBasic test object\n";

            bslma_TestAllocatorMonitor dam(da);
            {
                TObj *p = new (da) MyTestObject(&numDeletes);

                bslma_TestAllocatorMonitor dam2(da);
                Obj o(p);

                ASSERT(o.ptr() == p);
                ASSERT(dam2.isInUseSame());
            }
            ASSERT(dam.isTotalUp());
            ASSERT(dam.isInUseSame());
        }
        ASSERT(1 == numDeletes);


        if (verbose) cout << "\tTest derived-to-base pointer in constructor\n";

        numDeletes = 0;
        {
            bslma_TestAllocatorMonitor dam(da);
            {
                TObj *p = new (da) MyDerivedObject(&numDeletes);

                bslma_TestAllocatorMonitor dam2(da);
                Obj o(p);

                ASSERT(o.ptr() == p);
                ASSERT(dynamic_cast<MyDerivedObject *>(o.ptr()) == p);
                ASSERT(dam2.isInUseSame());
            }
            ASSERT(dam.isTotalUp());
            ASSERT(dam.isInUseSame());
        }
        ASSERT(1 == numDeletes);


        if (verbose) cout << "\tTest valid pointer passed to void*\n";

        numDeletes = 0;
        {
            bslma_TestAllocatorMonitor dam(da);
            {
                TObj *p = new (da) MyDerivedObject(&numDeletes);

                bslma_TestAllocatorMonitor dam2(da);
                VObj o(p);

                ASSERT(o.ptr() == p);
                ASSERT(dam2.isInUseSame());
            }
            ASSERT(dam.isTotalUp());
            ASSERT(dam.isInUseSame());
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            if (veryVerbose) cout << "\t\tconst-qualified int\n";

            bslma_TestAllocatorMonitor dam(da);
            {
                const int *p = new (da) const int(0);

                bslma_TestAllocatorMonitor dam2(da);
                bdema_ManagedPtr<const int> o(p);

                ASSERT(o.ptr() == p);
                ASSERT(dam2.isInUseSame());
            }
            ASSERT(dam.isTotalUp());
            ASSERT(dam.isInUseSame());
        }
        ASSERT(0 == numDeletes);

        numDeletes = 0;
        {
            if (veryVerbose) cout << "\t\tint -> const int conversion\n";

            bslma_TestAllocatorMonitor dam(da);
            {
                int *p = new (da) int;

                bslma_TestAllocatorMonitor dam2(da);
                bdema_ManagedPtr<const int> o(p);

                ASSERT(o.ptr() == p);
                ASSERT(dam2.isInUseSame());
            }
            ASSERT(dam.isTotalUp());
            ASSERT(dam.isInUseSame());
        }
        ASSERT(0 == numDeletes);

//#define TEST_FOR_COMPILE_ERRORS
#if defined TEST_FOR_COMPILE_ERRORS
        // This segment of the test case examines the quality of compiler
        // diagnostics when trying to create a 'bdema_ManagedPtr' object with a
        // pointer that it not convertible to a pointer of the type that the
        // smart pointer is managing.
        if (verbose) cout << "\tTesting compiler diagnostics*\n";

        numDeletes = 0;
        {
            double *p = new (da) double;
            Obj o(p);

//            ASSERT(o.ptr() == p);
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            const MyTestObject *p = new (da) MyTestObject(&numDeletes);
            Obj o(p);

//            ASSERT(o.ptr() == p);
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            const int *p = new (da) const int;

            bslma_TestAllocatorMonitor dam2(da);
            bdema_ManagedPtr<int> o(p);

//            ASSERT(o.ptr() == p);
        }
        ASSERT(0 == numDeletes);

#endif
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING bdema_ManagedPtr_Ref
        //
        // 'bdema_ManagedPtr_Ref' is similar to an in-core value semantic type
        // having a single pointer as its only attribute; it does not offer the
        // traditional range of value-semantic operations such as equality
        // comparison and printing.  Its test concerns and plan are closely
        // modelled after such a value-semantic type.
        //
        // Concerns:
        //: 1 TBD Enumerate concerns
        //
        // Plan:
        //: 1 blah ...
        //
        // Testing:
        //    explicit bdema_ManagedPtr_Ref(bdema_ManagedPtr_Members *base);
        //    bdema_ManagedPtr_Ref(const bdema_ManagedPtr_Ref& original);
        //    ~bdema_ManagedPtr_Ref();
        //    bdema_ManagedPtr_Ref& opetator=(const bdema_ManagedPtr_Ref&);
        //    bdema_ManagedPtr_Members *base() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING bdema_ManagedPtr_Ref"
                          << "\n----------------------------" << endl;

        bslma_TestAllocatorMonitor gam(globalAllocator);
        bslma_TestAllocatorMonitor dam(da);

        {
            int deleteCount = 0;
            MyTestObject x(&deleteCount);

            {
                bdema_ManagedPtr_Members empty;
                bdema_ManagedPtr_Members simple(&x, 0, doNothingDeleter);

                if (verbose) cout << "\tTest value constructor\n";

                const bdema_ManagedPtr_Ref<MyTestObject> ref(&empty);
                bdema_ManagedPtr_Members * target = ref.base();
                LOOP2_ASSERT(&empty, target, &empty == target);

                if (verbose) cout << "\tTest copy constructor\n";

                bdema_ManagedPtr_Ref<MyTestObject> other = ref;
                target = ref.base();
                LOOP2_ASSERT(&empty, target, &empty == target);
                target = other.base();
                LOOP2_ASSERT(&empty, target, &empty == target);

                if (verbose) cout << "\tTest assignment\n";

                const bdema_ManagedPtr_Ref<MyTestObject> second(&simple);
                target = second.base();
                LOOP2_ASSERT(&simple, target, &simple == target);


                other = second;

                target = ref.base();
                LOOP2_ASSERT(&empty, target, &empty == target);
                target = other.base();
                LOOP2_ASSERT(&simple, target, &simple == target);
                target = second.base();
                LOOP2_ASSERT(&simple, target, &simple == target);

                if (verbose) cout << "\tTest destructor\n";
            }

            LOOP_ASSERT(deleteCount, 0 == deleteCount);
        }

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) cout << "\tNegative testing\n";

        {
            bsls_AssertTestHandlerGuard guard;
            ASSERT_SAFE_FAIL_RAW(bdema_ManagedPtr_Ref<MyTestObject> null(0));
        }
#else
        if (verbose) cout << "\tNegative testing disabled due to lack of "
                             "exception support\n";
#endif

        ASSERT(dam.isInUseSame());
        ASSERT(gam.isInUseSame());
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING bdema_ManagedPtr_Members
        //
        // Concerns:
        //: 1 TBD Enumerate concerns
        //
        // Plan:
        //: 1 blah ...
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

        bslma_TestAllocatorMonitor gam(globalAllocator);
        bslma_TestAllocatorMonitor dam(da);

        if (verbose) cout << "\tTest default constructor\n";

        {
            const bdema_ManagedPtr_Members empty;
            ASSERT(0 == empty.pointer());
        }

        if (verbose) cout << "\tTest value constructor\n";

        {
            const bdema_ManagedPtr_Members empty(0, 0, 0);
            ASSERT(0 == empty.pointer());
#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
            ASSERT(0 == empty.deleter().object());
            ASSERT(0 == empty.deleter().factory());
            ASSERT(0 == empty.deleter().deleter());
#endif

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
#if defined BSLS_ASSERT_LEVEL_ASSERT_SAFE
            ASSERT(0 == members.deleter().object());
            ASSERT(0 == members.deleter().factory());
            ASSERT(0 == members.deleter().deleter());
#endif

            int x;
            double y;
            members.set(&x, &y, &countedNilDelete);
            ASSERT(&x == members.pointer());
            ASSERT(&x == members.deleter().object());
            ASSERT(&y == members.deleter().factory());
            ASSERT(&countedNilDelete == members.deleter().deleter());

            members.set(0, 0, 0);
            ASSERT(0 == members.pointer());
#if defined BSLS_ASSERT_LEVEL_ASSERT_SAFE
            ASSERT(0 == members.deleter().object());
            ASSERT(0 == members.deleter().factory());
            ASSERT(0 == members.deleter().deleter());
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
#if defined BSLS_ASSERT_SAFE_IS_ACTIVE
                    ASSERT(0 == simple.deleter().object());
                    ASSERT(0 == simple.deleter().factory());
                    ASSERT(0 == simple.deleter().deleter());
#endif
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
                static void deleter(void *a, void *b) {
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
            ASSERT(0 == empty.deleter().object());
            ASSERT(0 == empty.deleter().factory());
            ASSERT(0 == empty.deleter().deleter());

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
//            ASSERT(0 == simple.deleter().object());
            ASSERT(&x == empty.pointer());
            ASSERT(&x == empty.deleter().object());
            ASSERT(&y == empty.deleter().factory());
            ASSERT(&countedNilDelete == empty.deleter().deleter());

            empty.swap(simple);
            ASSERT(0 == empty.pointer());
//            ASSERT(0 == empty.deleter().object());
            ASSERT(&x == simple.pointer());
            ASSERT(&x == simple.deleter().object());
            ASSERT(&y == simple.deleter().factory());
            ASSERT(&countedNilDelete == simple.deleter().deleter());

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
        // TESTING bdema_ManagedPtr_FactoryDeleter
        //
        // Concerns:
        //: 1 'bdema_ManagedPtr_FactoryDeleter<T,U>::deleter(obj, factory)'
        //:   calls the 'deleteObject' method through the passed pointer to a
        //:   'factory' of type 'U', with the argument 'obj' which is cast to a
        //:   pointer to type 'T'.
        //:
        //: 2 The 'deleter' method can be used as a deleter policy by
        //:   'bdema_ManagedPtr'.
        //:
        //: 3 The 'deleter' method asserts in safe builds if passed a null
        //:   pointer for either argument.
        //:
        //
        // Plan:
        //: 1 blah ...
        //
        // Testing:
        //    bdema_ManagedPtr_FactoryDeleter<T,U>::deleter(obj, factory)
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING bdema_ManagedPtr_FactoryDeleter"
                          << "\n---------------------------------------"
                          << endl;


        typedef bdema_ManagedPtr_FactoryDeleter<MyTestObject,
                                             CountedStackDeleter > TestFactory;

        if (verbose) cout << "\tConfirm the deleter does not destroy the "
                             "passsed object\n";

        {
            int deleteCount = 0;
            MyTestObject t(&deleteCount);

            int parallelCount = 0;
            CountedStackDeleter factory(&parallelCount);

            TestFactory::deleter(&t, &factory);
            LOOP_ASSERT(deleteCount, 0 == deleteCount);
            LOOP_ASSERT(parallelCount, 1 == parallelCount);
        }

        if (verbose) cout << "\tConfirm the deleter can be registered with "
                             "a managed pointer\n";

        {
            bslma_TestAllocatorMonitor gam(globalAllocator);
            bslma_TestAllocatorMonitor dam(da);

            int deleteCount = 0;
            CountedStackDeleter factory(&deleteCount);

            int dummy = 0;
            MyTestObject x(&dummy);
            MyTestObject y(&dummy);

            bdema_ManagedPtr<MyTestObject> p(&x, &factory, 
                                             &doNothingDeleter);

            p.load(&y, &factory, &TestFactory::deleter);

            ASSERT(dam.isInUseSame());
            ASSERT(gam.isInUseSame());
        }

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) cout << "\tNegative testing\n";

        {
            bsls_AssertTestHandlerGuard guard;

            int deleteCount = 0;
            MyTestObject t(&deleteCount);

            int parallelCount = 0;
            CountedStackDeleter factory(&parallelCount);

            ASSERT_SAFE_FAIL(TestFactory::deleter(0, &factory));
            ASSERT_SAFE_FAIL(TestFactory::deleter(&t, 0));
            ASSERT_SAFE_FAIL(TestFactory::deleter(0, 0));
            ASSERT_SAFE_PASS(TestFactory::deleter(&t, &factory));
        }
#else
        if (verbose) cout << "\tNegative testing disabled due to lack of "
                             "exception support\n";
#endif
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
        //:     has incremeneted by exactly 1.
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

        bslma_TestAllocatorMonitor gam(globalAllocator);
        bslma_TestAllocatorMonitor dam(da);

        int destructorCount = 0;
        {
            MyTestObject mt(&destructorCount);
            ASSERT(&destructorCount == mt.deleteCounter());
            LOOP_ASSERT(destructorCount, 0 == destructorCount);
        }
        ASSERT(1 == destructorCount);

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
        ASSERT(1 == destructorCount);

        destructorCount = 0;
        {
            MyDerivedObject dt1(&destructorCount);
            {
                MyDerivedObject dt2 = dt1;
                ASSERT(&destructorCount == dt1.deleteCounter());
                ASSERT(&destructorCount == dt2.deleteCounter());
                LOOP_ASSERT(destructorCount, 0 == destructorCount);
            }
            LOOP_ASSERT(destructorCount, 1 == destructorCount);
        }
        ASSERT(2 == destructorCount);

        if (verbose) cout << "\tTest class MySecondDerivedObject\n";

        destructorCount = 0;
        {
            MySecondDerivedObject st(&destructorCount);
            ASSERT(&destructorCount == st.deleteCounter());
            LOOP_ASSERT(destructorCount, 0 == destructorCount);
        }
        ASSERT(1 == destructorCount);

        destructorCount = 0;
        {
            MySecondDerivedObject st1(&destructorCount);
            {
                MySecondDerivedObject st2 = st1;
                ASSERT(&destructorCount == st1.deleteCounter());
                ASSERT(&destructorCount == st2.deleteCounter());
                LOOP_ASSERT(destructorCount, 0 == destructorCount);
            }
            LOOP_ASSERT(destructorCount, 1 == destructorCount);
       }
       ASSERT(2 == destructorCount);

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

        if (verbose) cout << "\tTest copy construction.\n";

        bslma_TestAllocator ta("object", veryVeryVeryVerbose);

        int numDeletes = 0;
        {
            TObj *p = new(da) MyTestObject(&numDeletes);
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            Obj o(p);
            Obj o2(o);

            ASSERT(p == o2.ptr());
            ASSERT(0 == o.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        if (verbose) cout << "\tTest assignment.\n";

        numDeletes = 0;
        {
            TObj *p = new(da) MyTestObject(&numDeletes);
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            Obj o(p);
            Obj o2;

            ASSERT(p == o.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            o2  = o;

            ASSERT(p == o2.ptr());
            ASSERT(0 == o.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        if (verbose) cout << "\tTest construction from an rvalue.\n";

        numDeletes = 0;
        {
            bslma_TestAllocatorMonitor tam(ta);

            Obj x(returnManagedPtr(&numDeletes, &ta)); Obj const &X = x;

            ASSERT(X.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        if (verbose) cout << "\tTest assignment from an rvalue.\n";

        numDeletes = 0;
        {
            Obj x; Obj const &X = x;
            x = returnManagedPtr(&numDeletes, &ta);

            ASSERT(X.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        if (verbose) cout << "\tTest conversion construction.\n";

        numDeletes = 0;
        {
            TDObj *p = new(da) MyDerivedObject(&numDeletes);
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            DObj o(p);

            ASSERT(p == o.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            Obj o2(o); // conversion construction

            ASSERT(p == o2.ptr());
            ASSERT(0 == o.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            CObj o3(o2); // const-conversion construction

            ASSERT(p == o3.ptr());
            ASSERT(0 == o2.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        if (verbose) cout << "\tTest conversion assignment.\n";

        numDeletes = 0;
        {
            TDObj *p = new(da) MyDerivedObject(&numDeletes);
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            DObj o(p);

            ASSERT(p == o.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            Obj o2;
            o2  = o; // conversion assignment

            ASSERT(p == o2.ptr());
            ASSERT(0 == o.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            CObj o3;
            o3 = o2; // const-conversion assignment

            ASSERT(p == o3.ptr());
            ASSERT(0 == o2.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        if (verbose)
            cout << "\tTest conversion construction from an rvalue.\n";

        numDeletes = 0;
        {
            Obj x(returnDerivedPtr(&numDeletes, &ta)); Obj const &X = x;

            ASSERT(X.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        if (verbose)
            cout << "\tTest conversion assignment from an rvalue.\n";

        numDeletes = 0;
        {
            Obj x; Obj const &X = x;
            x = returnDerivedPtr(&numDeletes, &ta); // conversion-assignment
                                                    // from an rvalue

            ASSERT(X.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        if (verbose) cout << "\tTest alias construction.\n";

        numDeletes = 0;
        {
            TObj *p = new(da) MyTestObject(&numDeletes);
            ASSERT(0 != p);
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            Obj o(p);
            bdema_ManagedPtr<int> o2(o, o->valuePtr()); // alias construction

            ASSERT(p->valuePtr() == o2.ptr());
            ASSERT(0 == o.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        if (verbose) cout << "\tTest alias construction with conversion.\n";

        numDeletes = 0;
        {
            TDObj *p = new(da) MyDerivedObject(&numDeletes);
            ASSERT(0 != p);
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            Obj o(p);
            bdema_ManagedPtr<int> o2(o, o->valuePtr()); // alias construction

            ASSERT(p->valuePtr() == o2.ptr());
            ASSERT(0 == o.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        if (verbose) cout << "\tTest 'load' method.\n";

        numDeletes = 0;
        {
            int numDeletes2 = 0;
            TObj *p = new(da) MyTestObject(&numDeletes2);
            ASSERT(0 != p);
            ASSERT(0 == numDeletes2);

            Obj o(p);

            TObj *p2 = new(da) MyTestObject(&numDeletes);
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            o.load(p2);
            ASSERT(p2 == o.ptr());
            ASSERT(1 == numDeletes2);
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        if (verbose) cout << "\tTest 'load' method with allocator.\n";

        numDeletes = 0;
        {
            int numDeletes2 = 0;
            TObj *p = new(da) MyTestObject(&numDeletes2);
            ASSERT(0 == numDeletes2);

            Obj o(p);

            TObj *p2 = new(ta) MyTestObject(&numDeletes);
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            o.load(p2,&ta);
            ASSERT(p2 == o.ptr());
            LOOP_ASSERT(numDeletes2, 1 == numDeletes2);
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        if (verbose) cout << "\tTest 'loadAlias'.\n";

        numDeletes = 0;
        {
            TObj *p = new(da) MyTestObject(&numDeletes);
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            Obj o(p);
            bdema_ManagedPtr<int> o2;

            ASSERT(p == o.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            o2.loadAlias(o, o->valuePtr());

            ASSERT(p->valuePtr() == o2.ptr());
            ASSERT(0 == o.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            // Check load alias to self
            o2.loadAlias(o2, p->valuePtr(1));
            ASSERT(p->valuePtr(1) == o2.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        if (verbose) cout << "\tTest 'swap'.\n";

        numDeletes = 0;
        {
            TObj *p = new(da) MyTestObject(&numDeletes);
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            Obj o(p);
            Obj o2;

            ASSERT(p == o.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            o2.swap(o);
            ASSERT(p == o2.ptr());
            ASSERT(0 == o.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        if (verbose) cout << "\tTest 'swap' with custom deleter.\n";

        numDeletes = 0;
        {
            TObj *p = new(ta) MyTestObject(&numDeletes);
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            Obj o(p, &ta, &myTestDeleter);
            Obj o2;

            ASSERT(p == o.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            o2.swap(o);
            ASSERT(p == o2.ptr());
            ASSERT(0 == o.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        if (verbose) cout << "\tTest boolean.\n";

        numDeletes = 0;
        {
            TObj *p = new(ta) MyTestObject(&numDeletes);
            LOOP_ASSERT(numDeletes, 0 == numDeletes);

            Obj o(p, &ta, &myTestDeleter);
            Obj o2;

            ASSERT(o);
            ASSERT(!o2);

            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        if (verbose) cout << "\tTest no-op deleter.\n";

        numDeletes = 0;
        {
            TObj x(&numDeletes);
            {
                Obj p(&x, 0, &bdema_ManagedPtrNoOpDeleter::deleter);
            }
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        if (verbose) cout << "\tTest (deprecated) nil deleter.\n";

        numDeletes = 0;
        {
            TObj x(&numDeletes);
            {
                Obj p(&x,
                      0,
                      &bdema_ManagedPtrNilDeleter<MyTestObject>::deleter);
            }
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

     } break;
      case -1: {
        // --------------------------------------------------------------------
        // TESTING ADDITIONAL CONCERNS
        //
        // Concerns:
        //: 1 Two 'bdema_ManagedPtr<T>' objects should not be comparable with
		//:   the equality operator.
		//
        //: 2 Two objects of different instantiations of the 'bdema_ManagedPtr'
		//:   class template should not be comparable with the equality
		//:   operator
        //
        // Plan:
        //   The absence of a specific operator will be tested by failing to
		//   compile test code using that operator.  These tests will be
		//   configured to compile only when specific macros are defined as
		//   part of the build configuration, and not routinely tested.
        //
        // Testing:
        //   This test is checking for the *absence* of the following operators
        //: o 'operator=='.
        //: o 'operator!='.
        //: o 'operator<'.
        //: o 'operator<='.
        //: o 'operator>='.
        //: o 'operator>'.
        // --------------------------------------------------------------------
//#define BDEMA_MANAGEDPTR_TEST_NO_HOMOGENEOUS_COMPARISON
//#define BDEMA_MANAGEDPTR_TEST_NO_HOMOGENEOUS_ORDERING
//#define BDEMA_MANAGEDPTR_TEST_NO_HETEROGENEOUS_COMPARISON
//#define BDEMA_MANAGEDPTR_TEST_NO_HETEROGENEOUS_ORDERING

#if defined BDEMA_MANAGEDPTR_TEST_NO_HOMOGENEOUS_COMPARISON
        {
		    bdema_ManagedPtr<int> x;
            bool b;

		    // The following six lines should fail to compile
		    b = (x == x);
		    b = (x != x);
        }
#endif

#if defined BDEMA_MANAGEDPTR_TEST_NO_HOMOGENEOUS_ORDERING
        {
		    bdema_ManagedPtr<int> x;
            bool b;

		    // The following six lines should fail to compile
		    b = (x <  x);
		    b = (x <= x);
		    b = (x >= x);
		    b = (x >  x);
        }
#endif

#if defined BDEMA_MANAGEDPTR_TEST_NO_HETEROGENEOUS_COMPARISON
        {
		    bdema_ManagedPtr<int>    x;
		    bdema_ManagedPtr<double> y;

            bool b;

		    // The following twelve lines should fail to compile
		    b = (x == y);
		    b = (x != y);

            b = (y == x);
		    b = (y != x);
        }
#endif

#if defined BDEMA_MANAGEDPTR_TEST_NO_HETEROGENEOUS_ORDERING
        {
		    bdema_ManagedPtr<int>    x;
		    bdema_ManagedPtr<double> y;

            bool b;

		    // The following twelve lines should fail to compile
		    b = (x <  y);
		    b = (x <= y);
		    b = (x >= y);
		    b = (x >  y);

		    b = (y <  x);
		    b = (y <= x);
		    b = (y >= x);
		    b = (y >  x);
        }
#endif
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
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
