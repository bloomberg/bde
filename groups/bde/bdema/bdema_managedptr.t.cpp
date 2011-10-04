// bdema_managedptr.t.cpp                                             -*-C++-*-
#include <bdema_managedptr.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslmf_assert.h>
#include <bsls_asserttest.h>

#include <bsl_algorithm.h>   // to test swap
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // memcpy()
#include <bsl_iostream.h>
#include <bsl_typeinfo.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::endl;

//=============================================================================
//                             TEST PLAN
//                             ---------
// The 'bdema_managedptr' component provides a small number of classes that
// combine to provide a common solution to the problem of managing and
// transferring ownership of a dynamically allocated object.  It further
// contains a number of private classes to supply important implementation
// details, while the test driver introduces a reasonable amount of test
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
// Further, there are a number of behaviors that explicitly should not compile
// by accident that we will provide tests for.  These tests should fail to
// compile if the appropriate macro is defined.  Each such test will use a
// unique macro for its feature test, and provide a commented-out definition
// of that macro immediately above the test, to easily enable compiling that
// test while in development.  Below is the list of all macros that control
// the availability of these tests:
//  #define BDEMA_MANAGEDPTR_COMPILE_FAIL_SWAP_FOR_DIFFERENT_TYPES
//  #define BDEMA_MANAGEDPTR_COMPILE_FAIL_ASSIGN_FROM_INCOMPATIBLE_TYPE
//  #define BDEMA_MANAGEDPTR_COMPILE_FAIL_DEREFERENCE_VOID_PTR
//  #define BDEMA_MANAGEDPTR_COMPILE_FAIL_CONVERT_TO_REF_FROM_CONST
//  #define BDEMA_MANAGEDPTR_COMPILE_FAIL_MOVE_CONSTRUCT_FROM_CONST
//  #define BDEMA_MANAGEDPTR_COMPILE_FAIL_INCOMPATIBLE_POINTERS
//  #define BDEMA_MANAGEDPTR_COMPILE_FAIL_TEST_NULL_FACTORY
//  #define BDEMA_MANAGEDPTR_COMPILE_FAIL_TEST_NULL_DELETER
//  #define BDEMA_MANAGEDPTR_COMPILE_FAIL_LOAD_INCOMPATIBLE_TYPE
//  #define BDEMA_MANAGEDPTR_COMPILE_FAIL_TEST_LOAD_NULL_FACTORY
//  #define BDEMA_MANAGEDPTR_COMPILE_FAIL_TEST_LOAD_NULL_DELETER
//  #define BDEMA_MANAGEDPTR_COMPILE_FAIL_CONSTRUCT_FROM_INCOMPATIBLE_POINTER
//  #define BDEMA_MANAGEDPTR_COMPILE_FAIL_HOMOGENEOUS_COMPARISON
//  #define BDEMA_MANAGEDPTR_COMPILE_FAIL_HOMOGENEOUS_ORDERING
//  #define BDEMA_MANAGEDPTR_COMPILE_FAIL_HETEROGENEOUS_COMPARISON
//  #define BDEMA_MANAGEDPTR_COMPILE_FAIL_HETEROGENEOUS_ORDERING
//-----------------------------------------------------------------------------
//                         bdema_ManagedPtr
//                         ----------------
// We test 'bdema_ManagedPtr' incrementally, increasing the functionality that
// can be relied upon in later tests.  Starting with the default constructor,
// we will demonstrate that any valid state can be attained with the methods
// 'load' and 'loadAlias'.  In turn, this will be used to prove the correct
// behavior of the basic accessors, which are assumed correct but minimally
// relied up for the intial tests.  This allows us to test the remaining
// constructors, the move-semantic operations, and finally any other operations
// of the class.  Negative testing will ensure that all expected assertions are
// present and correct in the implementation, and a number of compile-fail
// tests will ensure that attempts to construct invalid managed pointers are
// caught early by the compiler, ideally with a helpful error diagnostic.
//-----------------------------------------------------------------------------
// [ 6] bdema_ManagedPtr();
// [ 6] bdema_ManagedPtr(bdema_ManagedPtr_Nullptr::Type);
// [ 6] template<class TARGET_TYPE> bdema_ManagedPtr(TARGET_TYPE *ptr);
// [11] bdema_ManagedPtr(bdema_ManagedPtr& original);
// [11] bdema_ManagedPtr(bdema_ManagedPtr_Ref<BDEMA_TYPE> ref);
// [12] bdema_ManagedPtr(bdema_ManagedPtr<OTHER> &alias, TYPE *ptr)
// [10] bdema_ManagedPtr(TYPE *ptr, FACTORY *factory)
// [10] bdema_ManagedPtr(TYPE *ptr, void *factory,void(*deleter)(TYPE*, void*))
// [ 6] ~bdema_ManagedPtr();
// [11] operator bdema_ManagedPtr_Ref<OTHER>();
// [ 7] void load(nullptr_t=0,nullptr_t=0,nullptr_t=0);
// [ 7] template<class TARGET_TYPE> void load(TARGET_TYPE *ptr);
// [ 7] void load(TYPE *ptr, FACTORY *factory)
// [ 7] void load(TYPE *ptr, nullptr_t, void (*deleter)(TYPE *, void*));
// [ 7] void load(TYPE *ptr, void *factory, void (*deleter)(void *, void*));
// [ 7] void load(TYPE *ptr, void *factory, void (*deleter)(TYPE *, void*));
// [ 7] void load(TYPE *ptr, FACTORY *factory, void(*deleter)(TYPE *,FACTORY*))
// [ 8] void loadAlias(bdema_ManagedPtr<OTHER> &alias, TYPE *ptr)
// [13] void swap(bdema_ManagedPt& rhs);
// [13] bdema_ManagedPtr& operator=(bdema_ManagedPtr &rhs);
// [13] bdema_ManagedPtr& operator=(bdema_ManagedPtr_Ref<BDEMA_TYPE> ref);
// [14] void clear();
// [14] bsl::pair<TYPE*,bdema_ManagedPtrDeleter> release();
// [ 9] operator BoolType() const;
// [ 9] TYPE& operator*() const;
// [ 9] TYPE *operator->() const;
// [ 9] TYPE *ptr() const;
// [ 9] const bdema_ManagedPtrDeleter& deleter() const;
//
// [15] class bdema_ManagedPtrNilDeleter
// [16] class bdema_ManagedPtrNoOpDeleter
//
// [ 3] imp. class bdema_ManagedPtr_Ref
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] TESTING TEST MACHINERY
// [17] CASTING EXAMPLE
// [18] USAGE EXAMPLE
// [-1] VERIFYING FAILURES TO COMPILE

namespace {

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
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

bool g_verbose;
bool g_veryVerbose;
bool g_veryVeryVeryVerbose;

class MyTestObject;
class MyDerivedObject;

typedef MyTestObject TObj;
typedef bdema_ManagedPtr<MyTestObject> Obj;
typedef bdema_ManagedPtr<const MyTestObject> CObj;
typedef MyDerivedObject TDObj;
typedef bdema_ManagedPtr<MyDerivedObject> DObj;
typedef bdema_ManagedPtr<void> VObj;

//=============================================================================
//                         HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

struct Base {
    explicit Base(int *deleteCount)
    : d_count_p(deleteCount)
    {
    }

    ~Base() { ++*d_count_p; }

    int *d_count_p;
};

struct Base1 : virtual Base {
    explicit Base1(int *deleteCount = 0)
    : Base(deleteCount)
    , d_padding()
    {
    }

    ~Base1() { *d_count_p += 10; }

    char d_padding;
};

struct Base2 : virtual Base {
    explicit Base2(int *deleteCount = 0)
    : Base(deleteCount)
    , d_padding()
    {
    }

    ~Base2() { *d_count_p += 100; }

    char d_padding;
};

struct Composite : Base1, Base2 {
    explicit Composite(int *deleteCount)
    : Base(deleteCount)
    , d_padding()
    {
    }

    ~Composite() { *d_count_p += 1000; }

    char d_padding;
};

void testCompsite() {
    int deleteCount;
    Composite x(&deleteCount);
    Base1 *p1 = &x;
    Base2 *p2 = &x;
    void *v1 = p1;
    void *v2 = p2;
    ASSERT(v1 != v2);
    Base *b1 = p1;
    Base *b2 = p2;
    ASSERT(b1 == b2);
}

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

struct IncrementIntFactory
{
    void destroy(int *object)
    {
        ASSERT(object);
        ++*object;
    }
};

void incrementIntDeleter(int *ptr, IncrementIntFactory *factory)
{
    ASSERT(ptr);
    ASSERT(factory);

    factory->destroy(ptr);
}
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

template<class TARGET_TYPE>
static void templateNilDelete(TARGET_TYPE *, void*)
{
    static int& deleteCount = g_deleteCount;
    ++g_deleteCount;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template<class BDEMA_TYPE>
class AllocatorDeleter
{
  public:
      static void deleter(BDEMA_TYPE *ptr, bslma_Allocator *alloc) {
          BSLS_ASSERT_SAFE(0 != ptr);
          BSLS_ASSERT_SAFE(0 != alloc);

          alloc->deleteObject(ptr);
      }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

struct OverloadTest {
    // This struct provides a small overload set taking managed pointer values
    // with similar looking (potentially related) types, to be sure there are
    // no unexpected ambiguities or conversions.
    static int invoke(bdema_ManagedPtr<void>)     { return 0; }
    static int invoke(bdema_ManagedPtr<int>)      { return 1; }
    static int invoke(bdema_ManagedPtr<const int>){ return 2; }
        // Return an integer code reporting which specific overload was called.
};

//=============================================================================
//                              CREATORS TEST
//=============================================================================

namespace CREATORS_TEST_NAMESPACE {

struct SS {
    // DATA
    char  d_buf[100];
    int  *d_numDeletes_p;

    // CREATORS
    explicit SS(int *numDeletes)
    {
        d_numDeletes_p = numDeletes;
    }

    ~SS()
    {
        ++*d_numDeletes_p;
    }
};

typedef bdema_ManagedPtr<SS> SSObj;
typedef bdema_ManagedPtr<char> ChObj;

}  // close namespace CREATORS_TEST_NAMESPACE

}  // close unnamed namespace

//=============================================================================
//                    FILE-STATIC FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static void myTestDeleter(TObj *object, bslma_TestAllocator *allocator)
{
    allocator->deleteObject(object);
    if (g_verbose) {
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

template<typename T>
void validateManagedState(const bdema_ManagedPtr<T>&     obj,
                          const void                    *ptr,
                          const bdema_ManagedPtrDeleter& del)
{
    // Testing the following properties of the specified 'obj'
    //   operator BoolType() const;
    //   TYPE& operator*() const;
    //   TYPE *operator->() const;
    //   TYPE *ptr() const;
    //   const bdema_ManagedPtrDeleter& deleter() const;

    bslma_TestAllocatorMonitor gam(dynamic_cast<bslma_TestAllocator*>
                                           (bslma_Default::globalAllocator()));
    bslma_TestAllocatorMonitor dam(dynamic_cast<bslma_TestAllocator*>
                                          (bslma_Default::defaultAllocator()));

    if (!ptr) {
        // Different negative testing constraints when 'ptr' is null.
        ASSERT(false == obj);
        ASSERT(!obj);
        ASSERT(0 == obj.operator->());
        ASSERT(0 == obj.ptr());

#ifdef BDE_BUILD_TARGET_EXC
        if (g_veryVerbose) cout << "\tNegative testing\n";

        {
            bsls_AssertTestHandlerGuard guard;

            ASSERT_SAFE_FAIL(*obj);
            ASSERT_SAFE_FAIL(obj.deleter());
        }
#endif
    }
    else {
        // Different negative testing constraints when 'ptr' is null.
        ASSERT(true  == (bool)obj);
        ASSERT(false == !obj);

        T *arrow = obj.operator->();
        LOOP2_ASSERT(ptr, arrow, ptr == arrow);

        T * objPtr = obj.ptr();
        LOOP2_ASSERT(ptr, objPtr, ptr == objPtr);

        T &target = *obj;
        LOOP2_ASSERT(&target, ptr, &target == ptr);

        const bdema_ManagedPtrDeleter& objDel = obj.deleter();
        LOOP2_ASSERT(del, objDel, del == objDel);
    }

    ASSERT(gam.isInUseSame());
    ASSERT(gam.isMaxSame());

    ASSERT(dam.isInUseSame());
    ASSERT(dam.isMaxSame());
}

void validateManagedState(const bdema_ManagedPtr<void>&  obj,
                          void                          *ptr,
                          const bdema_ManagedPtrDeleter& del)
{
    // Testing the following properties of the specified 'obj'
    //   operator BoolType() const;
    //   TYPE& operator*() const;
    //   TYPE *operator->() const;
    //   TYPE *ptr() const;
    //   const bdema_ManagedPtrDeleter& deleter() const;

    bslma_TestAllocatorMonitor gam(dynamic_cast<bslma_TestAllocator*>
                                           (bslma_Default::globalAllocator()));
    bslma_TestAllocatorMonitor dam(dynamic_cast<bslma_TestAllocator*>
                                          (bslma_Default::defaultAllocator()));

    if (!ptr) {
        // Different negative testing constraints when 'ptr' is null.
        ASSERT(false == obj);
        ASSERT(!obj);
        ASSERT(0 == obj.operator->());
        ASSERT(0 == obj.ptr());
#ifdef BDE_BUILD_TARGET_EXC
        if (g_veryVerbose) cout << "\tNegative testing\n";

        {
            bsls_AssertTestHandlerGuard guard;

            ASSERT_SAFE_FAIL(obj.deleter());
        }
#endif
    }
    else {
        // Different negative testing constraints when 'ptr' is null.
        ASSERT(true  == (bool)obj);
        ASSERT(false == !obj);

        void *arrow = obj.operator->();
        LOOP2_ASSERT(ptr, arrow, ptr == arrow);

        void * objPtr = obj.ptr();
        LOOP2_ASSERT(ptr, objPtr, ptr == objPtr);

        const bdema_ManagedPtrDeleter& objDel = obj.deleter();
        LOOP2_ASSERT(del, objDel, del == objDel);

#if defined(BDEMA_MANAGEDPTR_COMPILE_FAIL_DEREFERENCE_VOID_PTR)
        *obj;
#endif
    }

    ASSERT(gam.isInUseSame());
    ASSERT(gam.isMaxSame());

    ASSERT(dam.isInUseSame());
    ASSERT(dam.isMaxSame());
}

void validateManagedState(const bdema_ManagedPtr<const void>& obj,
                          const void                         *ptr,
                          const bdema_ManagedPtrDeleter&      del)
{
    // Testing the following properties of the specified 'obj'
    //   operator BoolType() const;
    //   TYPE& operator*() const;
    //   TYPE *operator->() const;
    //   TYPE *ptr() const;
    //   const bdema_ManagedPtrDeleter& deleter() const;
    // Testing the following properties of the specified 'obj'
    //   operator BoolType() const;
    //   TYPE& operator*() const;
    //   TYPE *operator->() const;
    //   TYPE *ptr() const;
    //   const bdema_ManagedPtrDeleter& deleter() const;

    bslma_TestAllocatorMonitor gam(dynamic_cast<bslma_TestAllocator*>
                                           (bslma_Default::globalAllocator()));
    bslma_TestAllocatorMonitor dam(dynamic_cast<bslma_TestAllocator*>
                                          (bslma_Default::defaultAllocator()));

    if (!ptr) {
        // Different negative testing constraints when 'ptr' is null.
        ASSERT(false == obj);
        ASSERT(!obj);
        ASSERT(0 == obj.operator->());
        ASSERT(0 == obj.ptr());
#ifdef BDE_BUILD_TARGET_EXC
        if (g_veryVerbose) cout << "\tNegative testing\n";

        {
            bsls_AssertTestHandlerGuard guard;

            ASSERT_SAFE_FAIL(obj.deleter());
        }
#endif
    }
    else {
        // Different negative testing constraints when 'ptr' is null.
        ASSERT(true  == (bool)obj);
        ASSERT(false == !obj);

        const void *arrow = obj.operator->();
        LOOP2_ASSERT(ptr, arrow, ptr == arrow);

        const void * objPtr = obj.ptr();
        LOOP2_ASSERT(ptr, objPtr, ptr == objPtr);

        const bdema_ManagedPtrDeleter& objDel = obj.deleter();
        LOOP2_ASSERT(del, objDel, del == objDel);

#if defined(BDEMA_MANAGEDPTR_COMPILE_FAIL_DEREFERENCE_VOID_PTR)
        *obj;
#endif
    }

    ASSERT(gam.isInUseSame());
    ASSERT(gam.isMaxSame());

    ASSERT(dam.isInUseSame());
    ASSERT(dam.isMaxSame());
}
//=============================================================================
//                             'LOAD' TESTING SUPPORT
//-----------------------------------------------------------------------------
// The following functions load a 'bdema_ManagedPtr' into a defined final state
// assuming that it is passed in with an initial state known to the calling
// function.  None of the following functions have their own test case, as they
// vital implementation details of testing the 'load' function, which in turn
// is later used to test the basic accessors.  However, these functions are
// very thoroughly exercised in the basic 'load' test case, in particular by
// taking an empty 'bdema_ManagedPtr' and taking it to the known state expected
// of each of these functions.  Similarly, we will test each transition from
// every possible initial state (in the simplified state space of owning a
// "kind" of pointer, factory, and deleter, rather than each possible value of
// pointer, factory, and deleter) through each of these functions to validate
// all 'load' state transitions.  Essentially, these are implementation details
// of the 'load' test case that may be deemed validated by that test case, and
// so safely relied on for all later test cases.
//
// Each function below has the same signature so that they can be used to
// populate a test table supporting table-driven testing techniques.  This will
// enable exhaustive testing of the state space and transitions of holding
// various kinds of 'bdema_ManagedPtr' objects.  The "move" and "alias" related
// functions are tested subsequently as those facilities are tested in later
// test cases.
//
// Each function performs the same set of operations in turn:
//: 1 Copy the initial values stored in passed pointers to compute expected
//:   side effects of calling 'load'.
//:
//: 2 'load' the specified 'Obj' pointer 'o' into the new defined state.
//:
//: 3 Set the new value for 'deleteDelta' when this new state of 'o' is
//    destroyed.
//:
//: 3 confirm the act of 'load'ing ran the expected deleters by comparing
//:   new state of 'deleteCount' with the computed value in (1).
//:
//: 4 confirm that each (defined) attribute of 'o' is in the expected state.
//
// The state combinations that need testing are invoking load with 0, 1, 2 or 3
// arguments.  Each combination should be tested for 'bdema_ManagedPtr'
// parameterized with
//: o 'MyTestObject'
//: o 'const MyTestObject'
//: o 'MyDerivedObject'
//: o 'void'
//: o 'const void'
//
// The first pointer argument should be tested with a pointer of each of the
// following types:
//: o '0' literal
//: o 'MyTestObject *'
//: o 'const MyTestObject *'
//: o 'MyDerivedObject *'
//: o 'void *'
//: o 'const void *'
//
// When no 'factory' argument is passed, each function should behave as if the
// default allocator were passing in that place.
//
// The second factory argument should be tested with:
//: o '0' literal
//: o 'bslma_Allocator *'
//: o 'ta' to test the specific 'bslma_TestAllocator' derived factory type
//: 0 SOME OTHER FACTORY TYPE NOT DERIVED FROM BSLMA_TESTALLOCATOR
//
// The 'deleter' argument will be tested with each of:
//: o '0' literal
//
// The 'line' and 'index' parameters describe the source line number at the
// call site, and any index into a function table to identify the specific
// invocation of this test function.  The 'useDefault' argument must be set to
// 'true' if the function allocates memory from the default allocator.  This is
// then used by the calling harness to know if it can check the default
// allocator's memory usage.
//
// The following chart describes the complete set of test scenarios, labeled
// with their corresponding function:
//:        Object  Code     Value
//:        ------  ----     -----
//:             -  (none)   use default (if any)
//:             0  Onull    null pointer literal
//:          base  Obase    pointer to allocated MyTestObject
//:    const base  OCbase   pointer to allocated 'const MyTestObject'
//:       derived  Oderiv   pointer to allocated 'MyDerivedObject'
//: const derived  OCderiv  pointer to allocated 'const MyDerivedObject'
//:  cast derived  Octob    'MyDerivedObject *' cast to 'MyTestObject *'
//:
//:       Factory  Code     Value
//:       -------  ----     -----
//:             -  (none)   use default (if any)
//:             0  Fnull    null pointer literal
//:         bslma  Fbsl    'bslma_TestAllocator' factory cast to 'bslma_Allocator *'
//:          Test  Ftst    'bslma_TestAllocator' factory
//:     void Test  FVtest   'bslma_TestAllocator' factory cast as 'void *'
//:       default  Fdflt    default allocator, passed as 'bslma_Allocator *'
//:  void default  FVdflt   default allocator, passed as 'void *'
//: [No const factory support by default, but can 'deleter' support this?]
//: [Probably only through the deprecated interface, and no code can do this yet?]
//:
//: Scenarios to consider:
//: "V(V* V*)" can hide many casting opportunities inside the function body.
//: This implies we may have many scenarios to test inside this one case, or
//: we may want to pick the most representative single case.  In fact two cases
//: dominate our analysis, "V(bslma_Allocator, Base)", and "V(actual, actual)".
//: The former can be explicitly coded up as a non-template function.  The
//: latter is already implemented as 'bdema_ManagedPtr_FactoryDeleter'.  Note
//: that there is a third category of deleter, where the deleter function acts
//: only on the 'object' parameter and ignores the 'factory'.  This is an
//: important case, as we must support '0' literals and null pointers for
//: factories based on existing code.  We can test this case with a deleter
//: that assumes the object was allocated using the default allocator.  This
//: gives us our 3 test scenarios for "V(V*,V*)".
//: The "V(V*,B*)" and "V(V*,D*)" cases could be tricky if the first "V*"
//: parameter is thought to be a type-erased factory that is cast back
//: internally.  We believe there are such cases in existing code, so must be
//: supported - we cannot assume the initial "V*" factory argument is ignored
//: by the deleter.  Here we will test just two forms, 'D' ignoring the factory
//: argument and using the default allocator to destroy the 'object', and 'B'
//: which destroys the 'object' by casting the 'factory' to 'bslma_Allocator*'.
//:
//:      Deleter  Code   Value
//:      -------  ----   -----
//:            -  (none) use default (if any)
//:            0  Dnull  [ALL SUCH OVERLOADS ARE COMPILE-FAIL TEST CASES]
//:    V(V*, V*)  Dzero  a pointer variable with value '0'.
//:    V(V*, V*)  DvvF
//:    V(V*, V*)  DvvT
//:    V(V*, V*)  DvvD
//:    V(V*, B*)  DvbD
//:    V(V*, B*)  DvbB
//:    V(V*, D*)  DvdD
//:    V(V*, D*)  DvdB
//:    V(B*, V*)  Dbv
//:    V(B*, B*)  Dbb
//:    V(B*, D*)  Dbd
//:    V(T*, V*)  Dtv
//:    V(T*, B*)  Dtb
//:    V(T*, D*)  Dtd
//:
//: Deleter codes used above:
//:     V(X* Y*) is a function type, returning 'void' taking arguments of type
//:              'X*' and 'Y*'.
//:
//: Possible values of X:
//: o V void
//: o B blsma_Allocator
//: o T bslma_TestAllocator
//:
//: Possible values of Y:
//: o V void
//: o B MyTestClass
//: o D MyDerivedClass

//X doLoad

//X doLoadOnull
//X doLoadObase
//X doLoadOCbase
//X doLoadOderiv
//X doLoadOCderiv

//- doLoadOnullFbsl    [COMPILE-FAIL], but might permit
//- doLoadOnullFtst    [COMPILE-FAIL], but might permit
//X doLoadObaseFbsl
//X doLoadObaseFtst
//X doLoadOCbaseFbsl
//X doLoadOCbaseFtst
//X doLoadOderivFbsl
//X doLoadOderivFtst
//X doLoadOCderivFbsl
//X doLoadOCderivFtst

//X doLoadObaseFbslDzero
//X doLoadObaseFtstDzero
//X doLoadOderivFbslDzero
//X doLoadOderivFtstDzero

// TBD Can we store a 'void *' object with a knowledgeable factory?
//     (I think we require a knowledgeable deleter to handle such type erasure)

// WOULD THESE SERVE ANY PURPOSE, OR PURELY DELETER TESTS?
// Patterns that would form compile-fails
//   O*Fnull
//   O*Fdflt
//   O*FVtest
//   O*FVdflt
//   O*F*Dnull

// These stems match the above pattern, but are interesting when testing
// deleters

//: doLoadOnullFdflt
//: doLoadObaseFdflt
//: doLoadOCbaseFdflt
//: doLoadOderivFdflt
//: doLoadOCderivFdflt

//: doLoadOnullFnull
//: doLoadObaseFnull
//: doLoadOCbaseFnull
//: doLoadOderivFnull
//: doLoadOCderivFnull

//: doLoadOnullFVtest
//: doLoadObaseFVtest
//: doLoadOCbaseFVtest
//: doLoadOderivFVtest
//: doLoadOCderivFVtest

//: doLoadOnullFVdflt
//: doLoadObaseFVdflt
//: doLoadOCbaseFVdflt
//: doLoadOderivFVdflt
//: doLoadOCderivFVdflt

namespace {

template<typename POINTER_TYPE>
struct TestLoadArgs {
    // This struct holds the set of arguments that will be passed into a
    // policy based test function.  It collects all information for the range
    // of tests and expectations to be set up on entry, and reported on exit.
    int  d_deleteCount; // An integer to be passed to 'MyTestObject's
    int  d_deleteDelta; //
    bool d_useDefault;  // Set to true if the test uses the default allocator
    bslma_TestAllocator *d_ta; // pointer to a test allocator whose lifetime will outlast the function call
    unsigned int d_config; // Valid values are 0-3.  The low-bit represents whether to pass a null for 'object', the second bit whether to pass a null for 'factory'
    bdema_ManagedPtr<POINTER_TYPE> *d_p; // pointer to the long-lived managed pointer on which to execute tests
};

template<typename POINTER_TYPE>
void validateTestLoadArgs(int callLine,
                          int testLine,
                          const TestLoadArgs<POINTER_TYPE> *args)
{
    // Assert pre-conditions that are appropriate for every call using 'args'.
    LOOP3_ASSERT(callLine, testLine, args->d_deleteCount,
                                                     0 == args->d_deleteCount);
    LOOP3_ASSERT(callLine, testLine, args->d_p,      0 != args->d_p);
    LOOP3_ASSERT(callLine, testLine, args->d_ta,     0 != args->d_ta);
    LOOP3_ASSERT(callLine, testLine, args->d_config, 4  > args->d_config);
}

//=============================================================================
//                          Target Object policies
// A Target Object policy consist of two members:
//: 1 A typedef, 'ObjectType', that reports the type of object to create
//: 2 An enum value 'DELETE_DELTA' reporting the expected change in
//:   'deleteCount' when the created object is destroyed.
//
// Note that the dynamic type of the object used in the test might be quite
// different to the static type of the created object described by this policy,
// notably for tests of 'bdema_ManagedPtr<void>'.
//
// List of available policies:
struct Obase;
struct OCbase;
struct Oderiv;
struct OCderiv;

// Policy implementations
struct Obase {
    typedef MyTestObject ObjectType;

    enum { DELETE_DELTA = 1 };
};

struct OCbase {
    typedef const MyTestObject ObjectType;

    enum { DELETE_DELTA = 1 };
};

struct Oderiv {
    typedef MyDerivedObject ObjectType;

    enum { DELETE_DELTA = 100 };
};

struct OCderiv {
    typedef const MyDerivedObject ObjectType;

    enum { DELETE_DELTA = 100 };
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//                             Factory Policies
// List of available policies:
struct Fbsl;
struct Ftst;
struct Fdflt;

// Policy implementations
struct Fbsl {
    typedef bslma_Allocator FactoryType;

    static FactoryType *factory(bslma_TestAllocator *f) {
        return f;
    }

    enum { USE_DEFAULT = false };
    enum { DELETER_USES_FACTORY = true};
};

struct Ftst {
    typedef bslma_TestAllocator FactoryType;

    static FactoryType *factory(bslma_TestAllocator *f) {
        return f;
    }

    enum { USE_DEFAULT = false };
    enum { DELETER_USES_FACTORY = true};
};

struct Fdflt {
    typedef bslma_Allocator FactoryType;

    static FactoryType *factory(bslma_TestAllocator *) {
        return bslma_Default::defaultAllocator();
    }

    enum { USE_DEFAULT = true };
    enum { DELETER_USES_FACTORY = false};
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//                             Deleter Policies
// List of available policies:
template<class ObjectPolicy, class FactoryPolicy> struct DObjFac;
template<class ObjectPolicy, class FactoryPolicy> struct DObjVoid;
template<class ObjectPolicy, class FactoryPolicy> struct DVoidFac;
template<class ObjectPolicy, class FactoryPolicy> struct DVoidVoid;

// Policy implementations
template<class ObjectPolicy, class FactoryPolicy>
struct DObjFac {
    typedef typename  ObjectPolicy::ObjectType  ObjectType;
    typedef typename FactoryPolicy::FactoryType FactoryType;

    typedef void DeleterType(ObjectType*, FactoryType *);

    enum {DELETER_USES_FACTORY = FactoryPolicy::DELETER_USES_FACTORY};

    static void doDelete(ObjectType * object, FactoryType * factory) {
        if (DELETER_USES_FACTORY) {
            factory->deleteObject(object);
        }
        else {
            // Use default allocator as the deleter,
            // ignore the passed factory pointer
            bslma_Allocator *pDa = bslma_Default::defaultAllocator();
            pDa->deleteObject(object);
        }
    }

    static DeleterType *deleter() {
        return &doDelete;
    }
};

template<class ObjectPolicy, class FactoryPolicy>
struct DObjVoid {
    typedef typename  ObjectPolicy::ObjectType  ObjectType;
    typedef typename FactoryPolicy::FactoryType FactoryType;

    typedef void DeleterType(ObjectType*, void *);

    enum {DELETER_USES_FACTORY = FactoryPolicy::DELETER_USES_FACTORY};

    static void doDelete(ObjectType * object, void * factory) {
        if (DELETER_USES_FACTORY) {
            FactoryType *fac = reinterpret_cast<FactoryType *>(factory);
            fac->deleteObject(object);
        }
        else {
            // Use default allocator as the deleter,
            // ignore the passed factory pointer
            bslma_Allocator *pDa = bslma_Default::defaultAllocator();
            pDa->deleteObject(object);
        }
    }

    static DeleterType *deleter() {
        return &doDelete;
    }
};

// The 'ToVoid' metafunction supports tests that need to use a 'void' pointer
// representing a pointer to the test object, while also retaining the correct
// cv-qualification.
template <class TYPE>
struct ToVoid {
    typedef void type;
};

template <class TYPE>
struct ToVoid<const TYPE> {
    typedef const void type;
};

template <class TYPE>
struct ToVoid<volatile TYPE> {
    typedef volatile void type;
};

template <class TYPE>
struct ToVoid<const volatile TYPE> {
    typedef const volatile void type;
};

template<class ObjectPolicy, class FactoryPolicy>
struct DVoidFac {
    typedef typename  ObjectPolicy::ObjectType  ObjectType;
    typedef typename FactoryPolicy::FactoryType FactoryType;

    typedef typename ToVoid<ObjectType>::type VoidType;

    typedef void DeleterType(VoidType *, FactoryType *);

    enum {DELETER_USES_FACTORY = FactoryPolicy::DELETER_USES_FACTORY};

    static void doDelete(VoidType * object, FactoryType * factory) {
        ObjectType *obj = reinterpret_cast<ObjectType *>(object);
        if (DELETER_USES_FACTORY) {
            factory->deleteObject(obj);
        }
        else {
            // Use default allocator as the deleter,
            // ignore the passed factory pointer
            bslma_Allocator *pDa = bslma_Default::defaultAllocator();
            pDa->deleteObject(obj);
        }
    }

    static DeleterType *deleter() {
        return &doDelete;
    }
};

template<class ObjectPolicy, class FactoryPolicy>
struct DVoidVoid {
    typedef typename  ObjectPolicy::ObjectType  ObjectType;
    typedef typename FactoryPolicy::FactoryType FactoryType;

    typedef void DeleterType(void*, void *);

    enum {DELETER_USES_FACTORY = FactoryPolicy::DELETER_USES_FACTORY};

    static void doDelete(void * object, void * factory) {
        ObjectType *obj = reinterpret_cast<ObjectType *>(object);
        if (DELETER_USES_FACTORY) {
            FactoryType *fac = reinterpret_cast<FactoryType *>(factory);
            fac->deleteObject(obj);
        }
        else {
            // Use default allocator as the deleter,
            // ignore the passed factory pointer
            bslma_Allocator *pDa = bslma_Default::defaultAllocator();
            pDa->deleteObject(obj);
        }
    }

    static DeleterType *deleter() {
        return &doDelete;
    }
};

//=============================================================================
//                       POLICY BASED TEST FUNCTIONS
//=============================================================================
// The following set of functions use the policies defined in the previous
// section to construct a set of tests that will exhaustively cover the
// potential type-space of valid combinations of type for each set of arguments
// to 'bdema_ManagedPtr::load'.  Each is a function template, taking at least
// a single type parameter describing the type 'bdema_ManagedPtr' should be
// instantiated for.  Most function templates will take additional type
// arguments describing different policies that are used to define the
// functionality of that test.
// This decomposition into 11 test policies and 10 test functions allows us to
// generate over 200 distint test functions, that in turn may be specified for
// each of the 5 types we instantiate 'bdema_ManagedPtr' with for testings.
// Note that not all 200 tests are valid for each of the 5 types, and indeed
// many will not compile if instantiated.
// In order to sequentially test each state and permutation of state changes we
// generate large test tables for each of our 5 test types taking the address
// of each valid test function that can be instantiated.  For completeness and
// ease of auditing, we list all combinations of function and policy for each
// of the 5 test types, and comment out only those we believe must be disabled.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// No policies needed for 'load' of empty managed pointers

template<typename POINTER_TYPE>
void doLoad(int callLine, int testLine, int index,
            TestLoadArgs<POINTER_TYPE> *args)
{
    validateTestLoadArgs(callLine, testLine, args); // Assert pre-conditions

    const int expectedCount = args->d_deleteDelta;

    args->d_p->load();
    args->d_deleteDelta = 0;

    LOOP5_ASSERT(callLine, testLine, index, expectedCount, args->d_deleteCount,
                 expectedCount == args->d_deleteCount);

    POINTER_TYPE *ptr = args->d_p->ptr();
    LOOP4_ASSERT(callLine, testLine, index, ptr, 0 == ptr);

    // As 'd_p' is empty, none of its other properties have a defined state.
}

template<typename POINTER_TYPE>
void doLoadOnull(int callLine, int testLine, int index,
                 TestLoadArgs<POINTER_TYPE> *args)
{
    validateTestLoadArgs(callLine, testLine, args); // Assert pre-conditions

    const int expectedCount = args->d_deleteDelta;

    args->d_p->load(0);
    args->d_deleteDelta = 0;

    LOOP5_ASSERT(callLine, testLine, index, expectedCount, args->d_deleteCount,
                 expectedCount == args->d_deleteCount);

    POINTER_TYPE *ptr = args->d_p->ptr();
    LOOP4_ASSERT(callLine, testLine, index, ptr, 0 == ptr);

    // As 'd_p' is empty, none of its other properties have a defined state.
}

template<typename POINTER_TYPE>
void doLoadOnullFnull(int callLine, int testLine, int index,
                      TestLoadArgs<POINTER_TYPE> *args)
{
    validateTestLoadArgs(callLine, testLine, args); // Assert pre-conditions

    const int expectedCount = args->d_deleteDelta;

    args->d_p->load(0, 0);
    args->d_deleteDelta = 0;

    LOOP5_ASSERT(callLine, testLine, index, expectedCount, args->d_deleteCount,
                 expectedCount == args->d_deleteCount);

    POINTER_TYPE *ptr = args->d_p->ptr();
    LOOP4_ASSERT(callLine, testLine, index, ptr, 0 == ptr);

    // As 'd_p' is empty, none of its other properties have a defined state.
}

template<typename POINTER_TYPE>
void doLoadOnullFnullDnull(int callLine, int testLine, int index,
                           TestLoadArgs<POINTER_TYPE> *args)
{
    validateTestLoadArgs(callLine, testLine, args); // Assert pre-conditions

    const int expectedCount = args->d_deleteDelta;

// A workaround for early GCC compilers
#if defined(BSLS_PLATFORM__CMP_GNU) && BSLS_PLATFORM__CMP_VER_MAJOR < 40000
    args->d_p->load(0, 0);
#else
    args->d_p->load(0, 0, 0);
#endif
    args->d_deleteDelta = 0;

    LOOP5_ASSERT(callLine, testLine, index, expectedCount, args->d_deleteCount,
                 expectedCount == args->d_deleteCount);

    POINTER_TYPE *ptr = args->d_p->ptr();
    LOOP4_ASSERT(callLine, testLine, index, ptr, 0 == ptr);

    // As 'd_p' is empty, none of its other properties have a defined state.
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// A simple object policy governs loading a single argument

template<class POINTER_TYPE, class ObjectPolicy>
void doLoadObject(int callLine, int testLine, int index,
                  TestLoadArgs<POINTER_TYPE> *args)
{
    validateTestLoadArgs(callLine, testLine, args); // Assert pre-conditions

    const bool nullObject  = args->d_config & 1;

    const int expectedCount = args->d_deleteDelta;

    typedef typename ObjectPolicy::ObjectType ObjectType;

    ObjectType *pO = 0;
    if(nullObject) {
        args->d_p->load(pO);
        args->d_deleteDelta = 0;
    }
    else {
        bslma_Allocator& da = *bslma_Default::defaultAllocator();
        pO = new(da)ObjectType(&args->d_deleteCount);
        args->d_useDefault = true;

        args->d_p->load(pO);
        args->d_deleteDelta = ObjectPolicy::DELETE_DELTA;
    }

    LOOP5_ASSERT(callLine, testLine, index, expectedCount, args->d_deleteCount,
                 expectedCount == args->d_deleteCount);

    POINTER_TYPE *ptr = args->d_p->ptr();
    LOOP5_ASSERT(callLine, testLine, index, pO, ptr, pO == ptr);

    // If we are feeling brave, verify that 'd_p.deleter' has the expected
    // 'object', 'factory' and 'deleter'
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following functions load a 'bdema_ManagedPtr' object using a factory.
// We now require separate policies for Object and Factory types

template<class POINTER_TYPE, class ObjectPolicy, class FactoryPolicy>
void doLoadObjectFactory(int callLine, int testLine, int index,
                         TestLoadArgs<POINTER_TYPE> *args)
{
    BSLMF_ASSERT( FactoryPolicy::DELETER_USES_FACTORY );

    validateTestLoadArgs(callLine, testLine, args); // Assert pre-conditions

    const bool nullObject  = args->d_config & 1;
    const bool nullFactory = args->d_config & 2;

    const int expectedCount = args->d_deleteDelta;

    // given a two-argument call to 'load', there is a problem only if
    // 'factory' is null while 'object' has a non-null value, as there is no
    // way to destroy the target object.  Pass a null deleter if that is the
    // goal.
    bool negativeTesting = !nullObject && nullFactory;

    // If we are negative-testing, we will create and destroy any target
    // object entirely within this function, so must track with a local counter
    // instead of the 'args' counter.
    int localDeleteCount = 0;

    int * counter = negativeTesting
                  ? &localDeleteCount
                  : &args->d_deleteCount;

    typedef typename  ObjectPolicy::ObjectType  ObjectType;
    typedef typename FactoryPolicy::FactoryType FactoryType;

    // We need two factory pointers, 'pAlloc' is used for all necessary
    // allocations and destructions within this function, while 'pF' is the
    // factory pointer passed to load, which is either the same as 'pAlloc' or
    // null.
    FactoryType *pAlloc = FactoryPolicy::factory(args->d_ta);
    FactoryType *pF = nullFactory
                    ? 0
                    : pAlloc;

    ObjectType  *pO = nullObject
                    ? 0
                    : new(*pAlloc)ObjectType(counter);
    if (FactoryPolicy::USE_DEFAULT) {
        args->d_useDefault = true;
    }

    // Load the 'bdema_ManagedPtr' and check that the previous state is
    // correctly cleared.
    if(!negativeTesting) {
        args->d_p->load(pO, pF);
        args->d_deleteDelta = nullObject ? 0 : ObjectPolicy::DELETE_DELTA;

        LOOP5_ASSERT(callLine, testLine, index,
                     expectedCount,   args->d_deleteCount,
                     expectedCount == args->d_deleteCount);

        POINTER_TYPE *ptr = args->d_p->ptr();
        LOOP5_ASSERT(callLine, testLine, index, pO, ptr, pO == ptr);
    }
    else {
#ifdef BDE_BUILD_TARGET_EXC
        if (g_verbose) cout << "\tNegative testing null factory pointer\n";

        {
            bsls_AssertTestHandlerGuard guard;

            ASSERT_SAFE_FAIL(args->d_p->load(pO, pF));

            pAlloc->deleteObject(pO);

            LOOP_ASSERT(localDeleteCount,
                        ObjectPolicy::DELETE_DELTA == localDeleteCount);
        }
#else
        if (verbose) cout << "\tNegative testing disabled due to lack of "
                             "exception support\n";
#endif
    }

    // If we are feeling brave, verify that 'p.deleter' has the expected
    // 'object', 'factory' and 'deleter'
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following functions load a 'bdema_ManagedPtr' object using both a
// factory and a deleter.
// First we perform negative testing when the 'deleter' argument is equal to
// a null pointer.  Note that passing a null pointer literal will produce a
// compile time error in this case, so we store the null in a variable of the
// desired function-pointer type.

template<class POINTER_TYPE, class ObjectPolicy, class FactoryPolicy>
void doLoadObjectFactoryDzero(int callLine, int testLine, int index,
                              TestLoadArgs<POINTER_TYPE> *args)
{
    validateTestLoadArgs(callLine, testLine, args); // Assert pre-conditions

    bool nullObject  = args->d_config & 1;
    bool nullFactory = args->d_config & 2;

    void (*nullFn)(void *, void*) = 0;

    const int expectedCount = args->d_deleteDelta;

    // given a two-argument call to 'load', there is a problem only if
    // 'factory' is null while 'object' has a non-null value, as there is no
    // way to destroy the target object.  Pass a null deleter if that is the
    // goal.
    bool negativeTesting = !nullObject;

    // If we are negative-testing, we will create and destroy any target
    // object entirely within this function, so must track with a local counter
    // instead of the 'args' counter.
    int localDeleteCount = 0;

    typedef typename  ObjectPolicy::ObjectType  ObjectType;
    typedef typename FactoryPolicy::FactoryType FactoryType;

    // We need two factory pointers, 'pAlloc' is used for all necessary
    // allocations and destructions within this function, while 'pF' is the
    // factory pointer passed to load, which is either the same as 'pAlloc' or
    // null.
    FactoryType *pAlloc = FactoryPolicy::factory(args->d_ta);
    FactoryType *pF = nullFactory
                    ? 0
                    : pAlloc;

    ObjectType *pO = 0;
    if(!nullObject) {
        pO = new(*pAlloc)ObjectType(&localDeleteCount);
        if (FactoryPolicy::USE_DEFAULT) {
            args->d_useDefault = true;
        }
    }

    if (!negativeTesting) {
        args->d_p->load(pO, pF, nullFn);
        args->d_deleteDelta = 0;

        LOOP5_ASSERT(callLine, testLine, index,
                     expectedCount,   args->d_deleteCount,
                     expectedCount == args->d_deleteCount);

        POINTER_TYPE *ptr = args->d_p->ptr();
        LOOP5_ASSERT(callLine, testLine, index, pO, ptr, pO == ptr);
    }
    else {
#ifdef BDE_BUILD_TARGET_EXC
        if (g_verbose) cout << "\tNegative testing null factory pointer\n";

        {
            bsls_AssertTestHandlerGuard guard;

            ASSERT_SAFE_FAIL(args->d_p->load(pO, pF, nullFn));
            ASSERT_SAFE_FAIL(args->d_p->load(pO,  0, nullFn));

            pAlloc->deleteObject(pO);
            LOOP_ASSERT(localDeleteCount,
                        ObjectPolicy::DELETE_DELTA == localDeleteCount);
        }
#else
        if (verbose) cout << "\tNegative testing disabled due to lack of "
                             "exception support\n";
#endif
    }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Next we supply the actual deleter argument, which now requires three
// separate policies.  Note that the 'deleter' policy is in turn parameterized
// on the types it expects to see, which may be different to (but compatible
// with) the actual 'object' and 'factory' policies used in a given test.

template<class POINTER_TYPE,
         class ObjectPolicy, class FactoryPolicy, class DeleterPolicy>
void doLoadObjectFactoryDeleter(int callLine, int testLine, int index,
                                TestLoadArgs<POINTER_TYPE> *args)
{
    validateTestLoadArgs(callLine, testLine, args); // Assert pre-conditions

    bool nullObject  = args->d_config & 1;
    bool nullFactory = args->d_config & 2;

    if(nullFactory && FactoryPolicy::DELETER_USES_FACTORY) {
        // It is perfectly well defined to pass a null pointer as the factory
        // if it is not going to be used by the deleter.  We cannot assert
        // this condition in the 'bdema_ManagedPtr' component, so simply exit
        // from this test case, rather than try negative testing strategies.
        // Note that some factory/deleter policies do not actually use the
        // factory argument when running the deleter.  These must be allowed
        // to continue through the rest of this test.
        return;
    }

    const int expectedCount = args->d_deleteDelta;

    typedef typename  ObjectPolicy::ObjectType  ObjectType;
    typedef typename FactoryPolicy::FactoryType FactoryType;
    typedef typename DeleterPolicy::DeleterType DeleterType;

    // We need two factory pointers, 'pAlloc' is used for all necessary
    // allocations and destructions within this function, while 'pF' is the
    // factory pointer passed to load, which is either the same as 'pAlloc' or
    // null.
    FactoryType *pAlloc = FactoryPolicy::factory(args->d_ta);
    FactoryType *pF = nullFactory
                    ? 0
                    : pAlloc;

    ObjectType *pO = 0;
    if (!nullObject) {
        pO = new(*pAlloc)ObjectType(&args->d_deleteCount);
        if (FactoryPolicy::USE_DEFAULT) {
            args->d_useDefault = true;
        }
        args->d_deleteDelta = ObjectPolicy::DELETE_DELTA;
    }

    DeleterType *deleter = DeleterPolicy::deleter();
    args->d_p->load(pO, pF, deleter);

    LOOP5_ASSERT(callLine, testLine, index, expectedCount, args->d_deleteCount,
                 expectedCount == args->d_deleteCount);

    POINTER_TYPE *ptr = args->d_p->ptr();
    LOOP5_ASSERT(callLine, testLine, index, pO, ptr, pO == ptr);
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Finally we test the small set of policies that combine to allow passing
// a null pointer literal as the factory.  This requires a deleter that will
// not use the factory pointer.
template<class POINTER_TYPE, class ObjectPolicy, class DeleterPolicy>
void doLoadObjectFnullDeleter(int callLine, int testLine, int index,
                              TestLoadArgs<POINTER_TYPE> *args)
{
    BSLMF_ASSERT(!DeleterPolicy::DELETER_USES_FACTORY);

    validateTestLoadArgs(callLine, testLine, args); // Assert pre-conditions

    bool nullObject  = args->d_config & 1;

    const int expectedCount = args->d_deleteDelta;

    typedef typename  ObjectPolicy::ObjectType  ObjectType;
    typedef typename DeleterPolicy::DeleterType DeleterType;

    ObjectType *pO = 0;
    if (!nullObject) {
        bslma_Allocator *pA = bslma_Default::defaultAllocator();
        pO = new(*pA)ObjectType(&args->d_deleteCount);
        args->d_useDefault  = true;
        args->d_deleteDelta = ObjectPolicy::DELETE_DELTA;
    }

    DeleterType *deleter = DeleterPolicy::deleter();
    args->d_p->load(pO, 0, deleter);

    LOOP5_ASSERT(callLine, testLine, index, expectedCount, args->d_deleteCount,
                 expectedCount == args->d_deleteCount);

    POINTER_TYPE *ptr = args->d_p->ptr();
    LOOP5_ASSERT(callLine, testLine, index, pO, ptr, pO == ptr);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// One all the testing policies are composed into arrays of test functions, we
// need some driver functions to iterate over each valid combination (stored
// in separate test tables) and check that the behavior transitions correctly
// in each case.

template<class TEST_TARGET,
         class TEST_FUNCTION_TYPE,
         std::size_t TEST_ARRAY_SIZE>
void testLoadOps(int callLine,
                 const TEST_FUNCTION_TYPE (&TEST_ARRAY)[TEST_ARRAY_SIZE])
{
    // This function iterates all viable variations of test functions composed
    // of the policies above, to verify that all 'bcema_ManagedPtr::load'
    // behave according to contract.  First, we call 'load' on an empty managed
    // pointer using a test function from the passed array, confirming that
    // the managed pointer takes up the correct state.  Then we allow that
    // pointer to go out of scope, and confirm that any managed object is
    // destroyed using the correct deleter.  Next we repeat the test, setting
    // up the same, now well-known, state of the managed pointer, and replace
    // it with a second call to load (by a second iterator over the array of
    // test functions).  We confirm that the original state and managed object
    // (if any) are destroyed correctly, and that the expected new state has
    // been established correctly.  Finally, we allow this pointer to leave
    // scope and confirm that all managed objects are destroyed correctly and
    // all allocated memory has been reclaimed.  At each stage, we perform
    // negative testing where appropriate, and check that no memory is being
    // allocated other than by the object allocator, or the default allocator
    // only for those test functions that return a state indicating that they
    // used the default allocator.
    typedef bdema_ManagedPtr<TEST_TARGET> TestPointer;

    bslma_TestAllocator& ga = dynamic_cast<bslma_TestAllocator&>
                                           (*bslma_Default::globalAllocator());

    bslma_TestAllocator& da = dynamic_cast<bslma_TestAllocator&>
                                          (*bslma_Default::defaultAllocator());

    TestLoadArgs<TEST_TARGET> args = {};

    for(unsigned configI = 0; configI != 4; ++configI) {
        for(int i = 0; i != TEST_ARRAY_SIZE; ++i) {
            bslma_TestAllocatorMonitor gam(&ga);
            bslma_TestAllocatorMonitor dam(&da);

            args.d_useDefault = false;
            args.d_config = configI;

            {
                bslma_TestAllocator ta("TestLoad 1", g_veryVeryVeryVerbose);
                TestPointer p;
                ASSERT(0 == p.ptr());

                args.d_p  = &p;
                args.d_ta = &ta;

                args.d_deleteCount = 0;
                args.d_deleteDelta = 0;
                TEST_ARRAY[i](callLine, L_, i, &args);
            }
            LOOP2_ASSERT(args.d_deleteCount,   args.d_deleteDelta,
                         args.d_deleteCount == args.d_deleteDelta);

            LOOP_ASSERT(i, gam.isInUseSame());
            LOOP_ASSERT(i, gam.isMaxSame());

            LOOP_ASSERT(i, dam.isInUseSame());
            if(!args.d_useDefault) {
                LOOP_ASSERT(i, dam.isMaxSame());
            }

            for(unsigned configJ = 0; configJ != 4; ++configJ) {
                for(int j = 0; j != TEST_ARRAY_SIZE; ++j) {
                    bslma_TestAllocatorMonitor dam2(&da);

                    bslma_TestAllocator ta("TestLoad 2",
                                           g_veryVeryVeryVerbose);

                    TestPointer p;
                    ASSERT(0 == p.ptr());

                    args.d_p  = &p;
                    args.d_ta = &ta;
                    args.d_config = configI;

                    args.d_deleteCount = 0;
                    args.d_deleteDelta = 0;
                    args.d_useDefault  = false;
                    TEST_ARRAY[i](callLine, L_, i, &args);

                    args.d_config = configJ;
                    args.d_deleteCount = 0;
                    TEST_ARRAY[j](callLine, L_, j, &args);

                    // Clear 'deleteCount' before 'p' is destroyed.
                    args.d_deleteCount = 0;

                    LOOP_ASSERT(i, gam.isInUseSame());
                    LOOP_ASSERT(i, gam.isMaxSame());

                    if(!args.d_useDefault) {
                        LOOP_ASSERT(i, dam2.isInUseSame());
                        LOOP_ASSERT(i, dam2.isMaxSame());
                    }
                }
            }

            // Validate the final deleter run when 'p' is destroyed.
            LOOP2_ASSERT(args.d_deleteCount,   args.d_deleteDelta,
                         args.d_deleteCount == args.d_deleteDelta);

            LOOP_ASSERT(i, dam.isInUseSame());
            LOOP_ASSERT(i, gam.isInUseSame());
            LOOP_ASSERT(i, gam.isMaxSame());
        }
    }
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
template <class T>
struct AliasTestType1 {
    typedef MySecondDerivedObject type;
};

template <class T>
struct AliasTestType2 {
    typedef MySecondDerivedObject type;
};

//template <>
//struct AliasTestType2<MyTestObject> {
//    typedef MySecondDerivedObject<MyDerivedObject> type;
//};

template <class T>
struct AliasTestType2<const T> : AliasTestType2<T> {};

template<class TEST_TARGET,
         class TEST_FUNCTION_TYPE,
         std::size_t TEST_ARRAY_SIZE>
void testLoadAliasOps1(int callLine,
                       const TEST_FUNCTION_TYPE (&TEST_ARRAY)[TEST_ARRAY_SIZE])
{
    // This function validates the simple scenario of calling 'loadAlias' to
    // create a simple aliased managed pointer, and confirming that pointer
    // destroyed its managed object with the correct deleter and reclaims all
    // memory when destroyed by leaving scope.
    typedef bdema_ManagedPtr<TEST_TARGET> TestPointer;

    bslma_TestAllocator& ga = dynamic_cast<bslma_TestAllocator&>
                                           (*bslma_Default::globalAllocator());

    bslma_TestAllocator& da = dynamic_cast<bslma_TestAllocator&>
                                          (*bslma_Default::defaultAllocator());

    int aliasDeleterCount = 0;
    typename AliasTestType1<TEST_TARGET>::type aliasTarget(&aliasDeleterCount);

    for(unsigned configI = 0; configI != 4; ++configI) {
        for(int i = 0; i != TEST_ARRAY_SIZE; ++i) {
            bslma_TestAllocatorMonitor gam(&ga);
            bslma_TestAllocatorMonitor dam(&da);

            TestLoadArgs<TEST_TARGET> args = {};
            args.d_useDefault = false;
            args.d_config = configI;

            {
                bslma_TestAllocator ta("TestLoad 1", g_veryVeryVeryVerbose);
                TestPointer p;
                ASSERT(0 == p.ptr());

                args.d_p  = &p;
                args.d_ta = &ta;

                args.d_deleteCount = 0;
                args.d_deleteDelta = 0;
                TEST_ARRAY[i](callLine, L_, i, &args);

                // Check that no more memory is allocated or freed.
                // All operations from here are effectively 'mode' operations.
                bslma_TestAllocatorMonitor gam2(&ga);
                bslma_TestAllocatorMonitor dam2(&da);
                bslma_TestAllocatorMonitor tam2(&ta);

#ifdef BDE_BUILD_TARGET_EXC
                if (g_verbose) cout << "\tNegative testing null pointers\n";

                TestPointer pAlias;
                if (0 == p.ptr()) {
                    bsls_AssertTestHandlerGuard guard;

                    ASSERT_SAFE_FAIL( pAlias.loadAlias(p, &aliasTarget) );
                    ASSERT_SAFE_PASS( pAlias.loadAlias(p, 0) );

                    LOOP_ASSERT(p.ptr(),      0 == p.ptr());
                    LOOP_ASSERT(pAlias.ptr(), 0 == pAlias.ptr());
                }
                else {
                    bsls_AssertTestHandlerGuard guard;

                    ASSERT_SAFE_FAIL( pAlias.loadAlias(p, 0) );
                    ASSERT_SAFE_PASS( pAlias.loadAlias(p, &aliasTarget) );

                    LOOP_ASSERT(p.ptr(),      0 == p.ptr());
                    LOOP_ASSERT(pAlias.ptr(), &aliasTarget == pAlias.ptr());
                }
#else
                TestPointer pAlias;
                TEST_TARGET pTarget = 0 == p.ptr()
                                    ? 0
                                    : &aliasTarget;

                pAlias.loadAlias(p, pTarget);

                LOOP_ASSERT(p.ptr(),  0 == p.ptr());
                LOOP2_ASSERT(pTarget, pAlias.ptr(), pTarget == pAlias.ptr());
#endif

                // Assert that no memory was allocated or freed
                LOOP_ASSERT(i, tam2.isInUseSame());
                LOOP_ASSERT(i, tam2.isMaxSame());
                LOOP_ASSERT(i, dam2.isInUseSame());
                LOOP_ASSERT(i, dam2.isMaxSame());
                LOOP_ASSERT(i, gam2.isInUseSame());
                LOOP_ASSERT(i, gam2.isMaxSame());
            }

            // Validate the final deleter run when 'p' is destroyed.
            LOOP2_ASSERT(args.d_deleteCount,   args.d_deleteDelta,
                         args.d_deleteCount == args.d_deleteDelta);

            LOOP_ASSERT(i, gam.isInUseSame());
            LOOP_ASSERT(i, gam.isMaxSame());

            LOOP_ASSERT(i, dam.isInUseSame());
            if(!args.d_useDefault) {
                LOOP_ASSERT(i, dam.isMaxSame());
            }
        }
    }
}

template<class TEST_TARGET,
         class TEST_FUNCTION_TYPE,
         std::size_t TEST_ARRAY_SIZE>
void testLoadAliasOps2(int callLine,
                       const TEST_FUNCTION_TYPE (&TEST_ARRAY)[TEST_ARRAY_SIZE])
{
    // This scenario tests the correct state change for following a 'loadAlias'
    // call with another 'loadAlias' call.  It will also test derived* -> base*
    // conversions for the aliased pointer, and non-const* -> const*.
    // The test process is to take an empty 'bdema_ManagedPtr' object and
    // 'load' a known state into it using a well-known test function.  Then we
    // "alias" this pointer by calling 'loadAlias' on another (empty) managed
    // pointer object, and check that the new aliased state has been created
    // correctly, without allocating any memory, and that the original managed
    // pointer object is now empty.  Next we establish another well-known
    // managed pointer value, and call 'loadAlias' again on the pointer in the
    // existing aliased state.  We again confirm that the aliased state is
    // transferred without allocating any memory, but also that the object
    // managed by the original 'bcema_ManagedPtr' object has now been destroyed
    // as expected.  Finally we let this final managed pointer object leave
    // scope and confirm that all managed objects have been destroyed, as
    // expected, and that all memory has been reclaimed.  At each step, we
    // further implement negative testing if a null pointer may be passed, and
    // that passing a null pointer would yield (negatively testable) undefined
    // behavior.
    typedef bdema_ManagedPtr<TEST_TARGET> TestPointer;

    bslma_TestAllocator& ga = dynamic_cast<bslma_TestAllocator&>
                                           (*bslma_Default::globalAllocator());

    bslma_TestAllocator& da = dynamic_cast<bslma_TestAllocator&>
                                          (*bslma_Default::defaultAllocator());

    TestLoadArgs<TEST_TARGET> args = {};

    int aliasDeleterCount1 = 0;
    int aliasDeleterCount2 = 0;
    typename AliasTestType1<TEST_TARGET>::type alias1(&aliasDeleterCount1);
    typename AliasTestType2<TEST_TARGET>::type alias2(&aliasDeleterCount2);

    for(unsigned configI = 0; configI != 4; ++configI) {
        for(int i = 0; i != TEST_ARRAY_SIZE; ++i) {
            bslma_TestAllocatorMonitor gam(&ga);
            bslma_TestAllocatorMonitor dam(&da);

            args.d_useDefault = false;
            args.d_config = configI;

            {
                bslma_TestAllocator ta("TestLoad 1", g_veryVeryVeryVerbose);
                TestPointer p;
                ASSERT(0 == p.ptr());

                args.d_p  = &p;
                args.d_ta = &ta;

                args.d_deleteCount = 0;
                args.d_deleteDelta = 0;
                TEST_ARRAY[i](callLine, L_, i, &args);

                // Check that no more memory is allocated or freed.
                // All operations from here are effectively 'mode' operations.
                bslma_TestAllocatorMonitor gam2(&ga);
                bslma_TestAllocatorMonitor dam2(&da);
                bslma_TestAllocatorMonitor tam2(&ta);

#ifdef BDE_BUILD_TARGET_EXC
                if (g_verbose) cout << "\tNegative testing null pointers\n";

                // Declare variables so that the lifetime extends to the end
                // of the loop.  Otherwise, the 'ta' monitor tests will flag
                // the 'pAlias2' destructor for freeing the original object.
                TestPointer pAlias1;
                TestPointer pAlias2;

                if (0 == p.ptr()) {
                    bsls_AssertTestHandlerGuard guard;

                    ASSERT_SAFE_FAIL( pAlias1.loadAlias(p, &alias1) );
                    ASSERT_SAFE_PASS( pAlias1.loadAlias(p, 0) );

                    LOOP_ASSERT(p.ptr(),       0 == p.ptr());
                    LOOP_ASSERT(pAlias1.ptr(), 0 == pAlias1.ptr());
                }
                else {
                    bsls_AssertTestHandlerGuard guard;

                    ASSERT_SAFE_FAIL( pAlias1.loadAlias(p, 0) );
                    ASSERT_SAFE_PASS( pAlias1.loadAlias(p, &alias1) );

                    LOOP_ASSERT(p.ptr(), 0 == p.ptr());
                    LOOP2_ASSERT(&alias1,   pAlias1.ptr(),
                                 &alias1 == pAlias1.ptr());

                    ASSERT_SAFE_FAIL( pAlias2.loadAlias(pAlias1, 0) );
                    ASSERT_SAFE_PASS( pAlias2.loadAlias(pAlias1, &alias2) );

                    LOOP_ASSERT(pAlias1.ptr(), 0 == pAlias1.ptr());
                    LOOP2_ASSERT(&alias2,   pAlias2.ptr(),
                                 &alias2 == pAlias2.ptr());
                }
#else
                TestPointer pAlias1;
                TEST_TARGET pTarget = 0 == p.ptr()
                                    ? 0
                                    : &alias1;

                pAlias.loadAlias(p, pTarget);

                LOOP_ASSERT(p.ptr(),  0 == p.ptr());
                LOOP2_ASSERT(pTarget, pAlias1.ptr(), pTarget == pAlias1.ptr());
#endif

                // Assert that no memory was allocated or freed
                LOOP_ASSERT(i, tam2.isInUseSame());
                LOOP_ASSERT(i, tam2.isMaxSame());
                LOOP_ASSERT(i, dam2.isInUseSame());
                LOOP_ASSERT(i, dam2.isMaxSame());
                LOOP_ASSERT(i, gam2.isInUseSame());
                LOOP_ASSERT(i, gam2.isMaxSame());
            }

            // Validate the final deleter run when 'p' is destroyed.
            LOOP2_ASSERT(args.d_deleteCount,   args.d_deleteDelta,
                         args.d_deleteCount == args.d_deleteDelta);

            LOOP_ASSERT(i, gam.isInUseSame());
            LOOP_ASSERT(i, gam.isMaxSame());

            LOOP_ASSERT(i, dam.isInUseSame());
            if(!args.d_useDefault) {
                LOOP_ASSERT(i, dam.isMaxSame());
            }
        }
    }
}


template<class TEST_TARGET,
         class TEST_FUNCTION_TYPE,
         std::size_t TEST_ARRAY_SIZE>
void testLoadAliasOps3(int callLine,
                       const TEST_FUNCTION_TYPE (&TEST_ARRAY)[TEST_ARRAY_SIZE])
{
    // This function tests the correct interaction of 'load' and 'loadAlias'.
    // Initially, an empty 'bdema_ManagedPtr' object is loaded into a well
    // defined non-empty state using a well-known test loader.  This state is
    // then transferred to a second empty pointer through a 'loadAlias' call,
    // and we validate that no memory is allocated for this operation, and the
    // state is correctly transferred.  Next we replace this aliased state with
    // another well-known state using 'load' again.  We test that the initial
    // state is correctly destroyed, and the new state is in place without any
    // aliasing.  Then we allow this final state to be destroyed, and confirm
    // that all managed objects have been correctly disposed of.
    typedef bdema_ManagedPtr<TEST_TARGET> TestPointer;

    bslma_TestAllocator& ga = dynamic_cast<bslma_TestAllocator&>
                                           (*bslma_Default::globalAllocator());

    bslma_TestAllocator& da = dynamic_cast<bslma_TestAllocator&>
                                          (*bslma_Default::defaultAllocator());

    int aliasDeleterCount = 0;
    typename AliasTestType1<TEST_TARGET>::type aliasTarget(&aliasDeleterCount);

    for(int i = 0; i != TEST_ARRAY_SIZE; ++i) {
        for(int j = 0; j != TEST_ARRAY_SIZE; ++j) {
            bslma_TestAllocatorMonitor gam(&ga);
            bslma_TestAllocatorMonitor dam(&da);

            TestLoadArgs<TEST_TARGET> args = {};
            args.d_useDefault = false;
            args.d_config = 0;  // We need only test a fully defined pointer,
                                // there are no concerns about null arguments.
            {
                bslma_TestAllocator ta("TestLoad 1", g_veryVeryVeryVerbose);
                TestPointer p;
                ASSERT(0 == p.ptr());

                args.d_p  = &p;
                args.d_ta = &ta;

                args.d_deleteCount = 0;
                args.d_deleteDelta = 0;
                TEST_ARRAY[i](callLine, L_, i, &args);
                if (0 == p.ptr()) {
                    // We have no interest in tests that create a null pointer,
                    // this scenario is negative tested in testLoadAliasOps1.
                    continue;
                }

                // Check that no more memory is allocated or freed.
                // All operations from here are effectively 'move' operations.
                bslma_TestAllocatorMonitor gam2(&ga);
                bslma_TestAllocatorMonitor dam2(&da);
                bslma_TestAllocatorMonitor tam2(&ta);

                TestPointer pAlias;
                pAlias.loadAlias(p, &aliasTarget);

                LOOP_ASSERT(p.ptr(),      0 == p.ptr());
                LOOP_ASSERT(pAlias.ptr(), &aliasTarget == pAlias.ptr());

                // Assert that no memory was allocated or freed
                LOOP_ASSERT(i, tam2.isInUseSame());
                LOOP_ASSERT(i, tam2.isMaxSame());
                LOOP_ASSERT(i, dam2.isInUseSame());
                LOOP_ASSERT(i, dam2.isMaxSame());
                LOOP_ASSERT(i, gam2.isInUseSame());
                LOOP_ASSERT(i, gam2.isMaxSame());

                // Next we load a fresh state into the pointer to verify the
                // final concern for 'load'; that it correctly destroys an
                // aliased state while acquire the new value.
                args.d_p  = &pAlias;

                // The test function itself asserts correct destructor count
                // for this transition, and that the 'pAlias' has the correct
                // final state.
                TEST_ARRAY[j](callLine, L_, j, &args);

                LOOP_ASSERT(i, gam.isInUseSame());
                LOOP_ASSERT(i, gam.isMaxSame());

                if(!args.d_useDefault) {
                    LOOP_ASSERT(i, dam.isInUseSame());
                    LOOP_ASSERT(i, dam.isMaxSame());
                }

                // Nothing further to assert, but reset 'deleteCount' to
                // verify destroying final objects outside the loop.
                args.d_deleteCount = 0;
            }

            // Validate the final deleter run when 'p' is destroyed.
            LOOP2_ASSERT(args.d_deleteCount,   args.d_deleteDelta,
                         args.d_deleteCount == args.d_deleteDelta);

            LOOP_ASSERT(i, gam.isInUseSame());
            LOOP_ASSERT(i, gam.isMaxSame());

            LOOP_ASSERT(i, dam.isInUseSame());
            if(!args.d_useDefault) {
                LOOP_ASSERT(i, dam.isMaxSame());
            }
        }
    }
}


//=============================================================================

typedef void (*TestBaseFn)(int, int, int, TestLoadArgs<MyTestObject> *);

static const TestBaseFn TEST_BASE_ARRAY[] = {
    // default test
    &doLoad<MyTestObject>,

    // single object-pointer tests
    &doLoadOnull <MyTestObject>,

    &doLoadObject<MyTestObject, Obase>,
    &doLoadObject<MyTestObject, Oderiv>,
    //&doLoadObject<MyTestObject, OCbase>,
    //&doLoadObject<MyTestObject, OCderiv>,

    // factory tests
    &doLoadOnullFnull <MyTestObject>,

    &doLoadObjectFactory<MyTestObject, Obase,   Ftst>,
    &doLoadObjectFactory<MyTestObject, Obase,   Fbsl>,
    &doLoadObjectFactory<MyTestObject, Oderiv,  Ftst>,
    &doLoadObjectFactory<MyTestObject, Oderiv,  Fbsl>,
    //&doLoadObjectFactory<MyTestObject, OCbase,  Ftst>,
    //&doLoadObjectFactory<MyTestObject, OCbase,  Fbsl>,
    //&doLoadObjectFactory<MyTestObject, OCderiv, Ftst>,
    //&doLoadObjectFactory<MyTestObject, OCderiv, Fbsl>,

    // deleter tests
    &doLoadOnullFnullDnull <MyTestObject>,

    // First test the non-deprecated interface, using the policy
    // 'DVoidVoid'.

    // MyTestObject
    &doLoadObjectFactoryDeleter<MyTestObject, Obase,   Ftst,
                                   DVoidVoid< Obase,   Ftst > >,
    &doLoadObjectFactoryDeleter<MyTestObject, Obase,   Fbsl,
                                   DVoidVoid< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<MyTestObject, Obase,   Ftst,
                                   DVoidVoid< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<MyTestObject, Obase,   Ftst,
                                   DVoidVoid< OCbase,  Ftst > >,
    &doLoadObjectFactoryDeleter<MyTestObject, Obase,   Fbsl,
                                   DVoidVoid< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<MyTestObject, Obase,   Ftst,
                                   DVoidVoid< OCbase,  Fbsl > >,

    // const MyTestObject
    //&doLoadObjectFactoryDeleter<MyTestObject, OCbase,  Ftst,
    //                               DVoidVoid< OCbase,  Ftst > >,
    //&doLoadObjectFactoryDeleter<MyTestObject, OCbase,  Fbsl,
    //                               DVoidVoid< OCbase,  Fbsl > >,

    //&doLoadObjectFactoryDeleter<MyTestObject, OCbase,  Ftst,
    //                               DVoidVoid< OCbase,  Fbsl > >,

    // MyDerivedObject
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Ftst,
                                   DVoidVoid< Oderiv,  Ftst > >,
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Ftst,
                                   DVoidVoid< Obase,   Ftst > >,

    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Fbsl,
                                   DVoidVoid< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Fbsl,
                                   DVoidVoid< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Ftst,
                                   DVoidVoid< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Ftst,
                                   DVoidVoid< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Ftst,
                                   DVoidVoid< OCderiv, Ftst > >,
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Ftst,
                                   DVoidVoid< OCbase,  Ftst > >,

    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Fbsl,
                                   DVoidVoid< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Fbsl,
                                   DVoidVoid< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Ftst,
                                   DVoidVoid< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Ftst,
                                   DVoidVoid< OCbase,  Fbsl > >,

    // const MyDerivedObject
    //&doLoadObjectFactoryDeleter<MyTestObject, OCderiv, Ftst,
    //                               DVoidVoid< OCderiv, Ftst > >,
    //&doLoadObjectFactoryDeleter<MyTestObject, OCderiv, Ftst,
    //                               DVoidVoid< OCbase,  Ftst > >,

    //&doLoadObjectFactoryDeleter<MyTestObject, OCderiv, Fbsl,
    //                               DVoidVoid< OCderiv, Fbsl > >,
    //&doLoadObjectFactoryDeleter<MyTestObject, OCderiv, Fbsl,
    //                               DVoidVoid< OCbase,  Fbsl > >,

    //&doLoadObjectFactoryDeleter<MyTestObject, OCderiv, Ftst,
    //                               DVoidVoid< OCderiv, Fbsl > >,
    //&doLoadObjectFactoryDeleter<MyTestObject, OCderiv, Ftst,
    //                               DVoidVoid< OCbase,  Fbsl > >,


    // Also test a deleter that does not use the 'factory'
    // argument.  These tests must also validate passing a null
    // pointer lvalue as the 'factory' argument.
    &doLoadObjectFactoryDeleter<MyTestObject, Obase,   Fdflt,
                                    DVoidVoid<Obase,   Fdflt> >,
    &doLoadObjectFactoryDeleter<MyTestObject, Obase,   Fdflt,
                                    DVoidVoid<OCbase,  Fdflt> >,

    //&doLoadObjectFactoryDeleter<MyTestObject, OCbase,  Fdflt,
    //                                DVoidVoid<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Fdflt,
                                    DVoidVoid<Oderiv,  Fdflt> >,
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Fdflt,
                                    DVoidVoid<Obase,   Fdflt> >,

    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Fdflt,
                                    DVoidVoid<OCderiv, Fdflt> >,
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Fdflt,
                                    DVoidVoid<OCbase,  Fdflt> >,

    //&doLoadObjectFactoryDeleter<MyTestObject, OCderiv, Fdflt,
    //                                DVoidVoid<OCderiv, Fdflt> >,
    //&doLoadObjectFactoryDeleter<MyTestObject, OCderiv, Fdflt,
    //                                DVoidVoid<OCbase,  Fdflt> >,

    // Also, verify null pointer literal can be used for the
    // factory argument in each case.
    &doLoadObjectFnullDeleter<MyTestObject, Obase,
                                  DVoidVoid<Obase,   Fdflt> >,
    &doLoadObjectFnullDeleter<MyTestObject, Obase,
                                  DVoidVoid<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<MyTestObject, OCbase,
    //                              DVoidVoid<OCbase,  Fdflt> >,

    &doLoadObjectFnullDeleter<MyTestObject, Oderiv,
                                  DVoidVoid<Oderiv,  Fdflt> >,
    &doLoadObjectFnullDeleter<MyTestObject, Oderiv,
                                  DVoidVoid<Obase,   Fdflt> >,

    &doLoadObjectFnullDeleter<MyTestObject, Oderiv,
                                  DVoidVoid<OCderiv, Fdflt> >,
    &doLoadObjectFnullDeleter<MyTestObject, Oderiv,
                                  DVoidVoid<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<MyTestObject, OCderiv,
    //                              DVoidVoid<OCderiv, Fdflt> >,
    //&doLoadObjectFnullDeleter<MyTestObject, OCderiv,
    //                              DVoidVoid<OCbase,  Fdflt> >,


    // Next we test the deprecated support for deleters other than
    // 'void (*)(void *, void *)', starting with deleters that
    // type-erase the 'object' type, but have a strongly typed
    // 'factory' argument.  Such deleters are generated by the
    // 'DVoidFac' policy..

    // MyTestObject
    &doLoadObjectFactoryDeleter<MyTestObject, Obase,   Ftst,
                                    DVoidFac< Obase,   Ftst > >,
    &doLoadObjectFactoryDeleter<MyTestObject, Obase,   Fbsl,
                                    DVoidFac< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<MyTestObject, Obase,   Ftst,
                                    DVoidFac< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<MyTestObject, Obase,   Ftst,
                                    DVoidFac< OCbase,  Ftst > >,
    &doLoadObjectFactoryDeleter<MyTestObject, Obase,   Fbsl,
                                    DVoidFac< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<MyTestObject, Obase,   Ftst,
                                    DVoidFac< OCbase,  Fbsl > >,

    // const MyTestObject
    //&doLoadObjectFactoryDeleter<MyTestObject, OCbase,  Ftst,
    //                                DVoidFac< OCbase,  Ftst > >,
    //&doLoadObjectFactoryDeleter<MyTestObject, OCbase,  Fbsl,
    //                                DVoidFac< OCbase,  Fbsl > >,

    //&doLoadObjectFactoryDeleter<MyTestObject, OCbase,  Ftst,
    //                                DVoidFac< OCbase,  Fbsl > >,

    // MyDerivedObject
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Ftst,
                                    DVoidFac< Oderiv,  Ftst > >,
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Ftst,
                                    DVoidFac< Obase,   Ftst > >,

    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Fbsl,
                                    DVoidFac< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Fbsl,
                                    DVoidFac< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Ftst,
                                    DVoidFac< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Ftst,
                                    DVoidFac< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Ftst,
                                    DVoidFac< OCderiv, Ftst > >,
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Ftst,
                                    DVoidFac< OCbase,  Ftst > >,

    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Fbsl,
                                    DVoidFac< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Fbsl,
                                    DVoidFac< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Ftst,
                                    DVoidFac< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Ftst,
                                    DVoidFac< OCbase,  Fbsl > >,

    // const MyDerivedObject
    //&doLoadObjectFactoryDeleter<MyTestObject, OCderiv, Ftst,
    //                                DVoidFac< OCderiv, Ftst > >,
    //&doLoadObjectFactoryDeleter<MyTestObject, OCderiv, Ftst,
    //                                DVoidFac< OCbase,  Ftst > >,

    //&doLoadObjectFactoryDeleter<MyTestObject, OCderiv, Fbsl,
    //                                DVoidFac< OCderiv, Fbsl > >,
    //&doLoadObjectFactoryDeleter<MyTestObject, OCderiv, Fbsl,
    //                                DVoidFac< OCbase,  Fbsl > >,

    //&doLoadObjectFactoryDeleter<MyTestObject, OCderiv, Ftst,
    //                                DVoidFac< OCderiv, Fbsl > >,
    //&doLoadObjectFactoryDeleter<MyTestObject, OCderiv, Ftst,
    //                                DVoidFac< OCbase,  Fbsl > >,


    // Also test a deleter that does not use the 'factory'
    // argument.  These tests must also validate passing a null
    // pointer lvalue as the 'factory' argument.
    &doLoadObjectFactoryDeleter<MyTestObject, Obase,   Fdflt,
                                     DVoidFac<Obase,   Fdflt> >,
    &doLoadObjectFactoryDeleter<MyTestObject, Obase,   Fdflt,
                                     DVoidFac<OCbase,  Fdflt> >,

    //&doLoadObjectFactoryDeleter<MyTestObject, OCbase,  Fdflt,
    //                                 DVoidFac<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Fdflt,
                                     DVoidFac<Oderiv,  Fdflt> >,
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Fdflt,
                                     DVoidFac<Obase,   Fdflt> >,

    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Fdflt,
                                     DVoidFac<OCderiv, Fdflt> >,
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Fdflt,
                                     DVoidFac<OCbase,  Fdflt> >,

    //&doLoadObjectFactoryDeleter<MyTestObject, OCderiv, Fdflt,
    //                               DVoidFac<OCderiv, Fdflt> >,
    //&doLoadObjectFactoryDeleter<MyTestObject, OCderiv, Fdflt,
    //                               DVoidFac<OCbase,  Fdflt> >,

    // Also, verify null pointer literal can be used for the
    // factory argument in each case.
    // DESIGN NOTE - NULL POINTER LITERALS CAN BE USED ONLY WITH
    //               DELETERS THAT TYPE-ERASE THE FACTORY.
    //&doLoadObjectFnullDeleter<MyTestObject, Obase,
    //                               DVoidFac<Obase,   Fdflt> >,
    //&doLoadObjectFnullDeleter<MyTestObject, Obase,
    //                               DVoidFac<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<MyTestObject, Oderiv,
    //                               DVoidFac<Oderiv,  Fdflt> >,
    //&doLoadObjectFnullDeleter<MyTestObject, Oderiv,
    //                               DVoidFac<Obase,   Fdflt> >,

    //&doLoadObjectFnullDeleter<MyTestObject, Oderiv,
    //                               DVoidFac<OCderiv, Fdflt> >,
    //&doLoadObjectFnullDeleter<MyTestObject, Oderiv,
    //                               DVoidFac<OCbase,  Fdflt> >,

    // HERE WE ARE DOUBLY-BROKEN AS CV-QUALIFIED TYPES ARE NOT
    // SUPPORTED FOR TYPE-ERASURE THROUGH DELETER
    //&doLoadObjectFnullDeleter<MyTestObject, OCbase,
    //                               DVoidFac<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<MyTestObject, OCderiv,
    //                               DVoidFac<OCderiv, Fdflt> >,
    //&doLoadObjectFnullDeleter<MyTestObject, OCderiv,
    //                               DVoidFac<OCbase,  Fdflt> >,


    // Now we test deleters that are strongly typed for the
    // 'object' parameter, but type-erase the 'factory'.

    // MyTestObject
    &doLoadObjectFactoryDeleter<MyTestObject, Obase,   Ftst,
                                    DObjVoid< Obase,   Ftst > >,
    &doLoadObjectFactoryDeleter<MyTestObject, Obase,   Fbsl,
                                    DObjVoid< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<MyTestObject, Obase,   Ftst,
                                    DObjVoid< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<MyTestObject, Obase,   Ftst,
                                    DObjVoid< OCbase,  Ftst > >,
    &doLoadObjectFactoryDeleter<MyTestObject, Obase,   Fbsl,
                                    DObjVoid< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<MyTestObject, Obase,   Ftst,
                                    DObjVoid< OCbase,  Fbsl > >,

    // const MyTestObject
    //&doLoadObjectFactoryDeleter<MyTestObject, OCbase,  Ftst,
    //                                DObjVoid< OCbase,  Ftst > >,
    //&doLoadObjectFactoryDeleter<MyTestObject, OCbase,  Fbsl,
    //                                DObjVoid< OCbase,  Fbsl > >,

    //&doLoadObjectFactoryDeleter<MyTestObject, OCbase,  Ftst,
    //                                DObjVoid< OCbase,  Fbsl > >,

    // MyDerivedObject
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Ftst,
                                    DObjVoid< Oderiv,  Ftst > >,
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Ftst,
                                    DObjVoid< Obase,   Ftst > >,

    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Fbsl,
                                    DObjVoid< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Fbsl,
                                    DObjVoid< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Ftst,
                                    DObjVoid< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Ftst,
                                    DObjVoid< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Ftst,
                                    DObjVoid< OCderiv, Ftst > >,
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Ftst,
                                    DObjVoid< OCbase,  Ftst > >,

    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Fbsl,
                                    DObjVoid< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Fbsl,
                                    DObjVoid< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Ftst,
                                    DObjVoid< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Ftst,
                                    DObjVoid< OCbase,  Fbsl > >,

    // const MyDerivedObject
    //&doLoadObjectFactoryDeleter<MyTestObject, OCderiv, Ftst,
    //                                DObjVoid< OCderiv, Ftst > >,
    //&doLoadObjectFactoryDeleter<MyTestObject, OCderiv, Ftst,
    //                                DObjVoid< OCbase,  Ftst > >,

    //&doLoadObjectFactoryDeleter<MyTestObject, OCderiv, Fbsl,
    //                                DObjVoid< OCderiv, Fbsl > >,
    //&doLoadObjectFactoryDeleter<MyTestObject, OCderiv, Fbsl,
    //                                DObjVoid< OCbase,  Fbsl > >,

    //&doLoadObjectFactoryDeleter<MyTestObject, OCderiv, Ftst,
    //                                DObjVoid< OCderiv, Fbsl > >,
    //&doLoadObjectFactoryDeleter<MyTestObject, OCderiv, Ftst,
    //                                DObjVoid< OCbase,  Fbsl > >,


    // Also test a deleter that does not use the 'factory'
    // argument.  These tests must also validate passing a null
    // pointer lvalue as the 'factory' argument.
    &doLoadObjectFactoryDeleter<MyTestObject, Obase,   Fdflt,
                                     DObjVoid<Obase,   Fdflt> >,
    &doLoadObjectFactoryDeleter<MyTestObject, Obase,   Fdflt,
                                     DObjVoid<OCbase,  Fdflt> >,

    //&doLoadObjectFactoryDeleter<MyTestObject, OCbase,  Fdflt,
    //                                 DObjVoid<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Fdflt,
                                     DObjVoid<Oderiv,  Fdflt> >,
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Fdflt,
                                     DObjVoid<Obase,   Fdflt> >,

    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Fdflt,
                                     DObjVoid<OCderiv, Fdflt> >,
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Fdflt,
                                     DObjVoid<OCbase,  Fdflt> >,

    //&doLoadObjectFactoryDeleter<MyTestObject, OCderiv, Fdflt,
    //                                 DObjVoid<OCderiv, Fdflt> >,
    //&doLoadObjectFactoryDeleter<MyTestObject, OCderiv, Fdflt,
    //                                 DObjVoid<OCbase,  Fdflt> >,

    // Also, verify null pointer literal can be used for the
    // factory argument in each case.
    &doLoadObjectFnullDeleter<MyTestObject, Obase,
                                   DObjVoid<Obase,   Fdflt> >,
    &doLoadObjectFnullDeleter<MyTestObject, Obase,
                                   DObjVoid<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<MyTestObject, OCbase,
    //                             DObjVoid<OCbase,  Fdflt> >,

    &doLoadObjectFnullDeleter<MyTestObject, Oderiv,
                                   DObjVoid<Oderiv,  Fdflt> >,
    &doLoadObjectFnullDeleter<MyTestObject, Oderiv,
                                   DObjVoid<Obase,   Fdflt> >,

    &doLoadObjectFnullDeleter<MyTestObject, Oderiv,
                                   DObjVoid<OCderiv, Fdflt> >,
    &doLoadObjectFnullDeleter<MyTestObject, Oderiv,
                                   DObjVoid<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<MyTestObject, OCderiv,
    //                               DObjVoid<OCderiv, Fdflt> >,
    //&doLoadObjectFnullDeleter<MyTestObject, OCderiv,
    //                               DObjVoid<OCbase,  Fdflt> >,


    // Finally we test the most generic combination of generic
    // object type, a factory, and a deleter taking two arguments
    // compatible with pointers to the invoking 'object' and
    // 'factory' types.

    // MyTestObject
    &doLoadObjectFactoryDeleter<MyTestObject, Obase,   Ftst,
                                     DObjFac< Obase,   Ftst > >,
    &doLoadObjectFactoryDeleter<MyTestObject, Obase,   Fbsl,
                                     DObjFac< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<MyTestObject, Obase,   Ftst,
                                     DObjFac< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<MyTestObject, Obase,   Ftst,
                                     DObjFac< OCbase,  Ftst > >,
    &doLoadObjectFactoryDeleter<MyTestObject, Obase,   Fbsl,
                                     DObjFac< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<MyTestObject, Obase,   Ftst,
                                     DObjFac< OCbase,  Fbsl > >,

    // const MyTestObject
    //&doLoadObjectFactoryDeleter<MyTestObject, OCbase,  Ftst,
    //                                 DObjFac< OCbase,  Ftst > >,
    //&doLoadObjectFactoryDeleter<MyTestObject, OCbase,  Fbsl,
    //                                 DObjFac< OCbase,  Fbsl > >,

    //&doLoadObjectFactoryDeleter<MyTestObject, OCbase,  Ftst,
    //                                 DObjFac< OCbase,  Fbsl > >,

    // MyDerivedObject
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Ftst,
                                     DObjFac< Oderiv,  Ftst > >,
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Ftst,
                                     DObjFac< Obase,   Ftst > >,

    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Fbsl,
                                     DObjFac< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Fbsl,
                                     DObjFac< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Ftst,
                                     DObjFac< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Ftst,
                                     DObjFac< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Ftst,
                                     DObjFac< OCderiv, Ftst > >,
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Ftst,
                                     DObjFac< OCbase,  Ftst > >,

    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Fbsl,
                                     DObjFac< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Fbsl,
                                     DObjFac< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Ftst,
                                     DObjFac< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Ftst,
                                     DObjFac< OCbase,  Fbsl > >,

    // const MyDerivedObject
    //&doLoadObjectFactoryDeleter<MyTestObject, OCderiv, Ftst,
    //                                 DObjFac< OCderiv, Ftst > >,
    //&doLoadObjectFactoryDeleter<MyTestObject, OCderiv, Ftst,
    //                                 DObjFac< OCbase,  Ftst > >,

    //&doLoadObjectFactoryDeleter<MyTestObject, OCderiv, Fbsl,
    //                                 DObjFac< OCderiv, Fbsl > >,
    //&doLoadObjectFactoryDeleter<MyTestObject, OCderiv, Fbsl,
    //                                 DObjFac< OCbase,  Fbsl > >,

    //&doLoadObjectFactoryDeleter<MyTestObject, OCderiv, Ftst,
    //                                 DObjFac< OCderiv, Fbsl > >,
    //&doLoadObjectFactoryDeleter<MyTestObject, OCderiv, Ftst,
    //                                 DObjFac< OCbase,  Fbsl > >,


    // Also test a deleter that does not use the 'factory'
    // argument.  These tests must also validate passing a null
    // pointer lvalue as the 'factory' argument.
    &doLoadObjectFactoryDeleter<MyTestObject, Obase,   Fdflt,
                                      DObjFac<Obase,   Fdflt> >,
    &doLoadObjectFactoryDeleter<MyTestObject, Obase,   Fdflt,
                                      DObjFac<OCbase,  Fdflt> >,

    //&doLoadObjectFactoryDeleter<MyTestObject, OCbase,  Fdflt,
    //                                  DObjFac<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Fdflt,
                                      DObjFac<Oderiv,  Fdflt> >,
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Fdflt,
                                      DObjFac<Obase,   Fdflt> >,

    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Fdflt,
                                      DObjFac<OCderiv, Fdflt> >,
    &doLoadObjectFactoryDeleter<MyTestObject, Oderiv,  Fdflt,
                                      DObjFac<OCbase,  Fdflt> >,

    //&doLoadObjectFactoryDeleter<MyTestObject, OCderiv, Fdflt,
    //                                  DObjFac<OCderiv, Fdflt> >,
    //&doLoadObjectFactoryDeleter<MyTestObject, OCderiv, Fdflt,
    //                                  DObjFac<OCbase,  Fdflt> >,

    // Also, verify null pointer literal can be used for the
    // factory argument in each case.
    // DESIGN NOTE - NULL POINTER LITERALS CAN BE USED ONLY WITH
    //               DELETERS THAT TYPE-ERASE THE FACTORY.
    //&doLoadObjectFnullDeleter<MyTestObject, Obase,
    //                                DObjFac<Obase,   Fdflt> >,
    //&doLoadObjectFnullDeleter<MyTestObject, Obase,
    //                                DObjFac<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<MyTestObject, OCbase,
    //                                DObjFac<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<MyTestObject, Oderiv,
    //                                DObjFac<Oderiv,  Fdflt> >,
    //&doLoadObjectFnullDeleter<MyTestObject, Oderiv,
    //                                DObjFac<Obase,   Fdflt> >,

    //&doLoadObjectFnullDeleter<MyTestObject, Oderiv,
    //                                DObjFac<OCderiv, Fdflt> >,
    //&doLoadObjectFnullDeleter<MyTestObject, Oderiv,
    //                                DObjFac<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<MyTestObject, OCderiv,
    //                                DObjFac<OCderiv, Fdflt> >,
    //&doLoadObjectFnullDeleter<MyTestObject, OCderiv,
    //                                DObjFac<OCbase,  Fdflt> >,


    // negative tests for deleters look for a null pointer lvalue.
    // Note that null pointer literal would be a compile-fail test
    &doLoadObjectFactoryDzero<MyTestObject, Obase,   Ftst>,
    &doLoadObjectFactoryDzero<MyTestObject, Obase,   Fbsl>,
    &doLoadObjectFactoryDzero<MyTestObject, Oderiv,  Ftst>,
    &doLoadObjectFactoryDzero<MyTestObject, Oderiv,  Fbsl>,
    //&doLoadObjectFactoryDzero<MyTestObject, OCbase,  Ftst>,
    //&doLoadObjectFactoryDzero<MyTestObject, OCbase,  Fbsl>,
    //&doLoadObjectFactoryDzero<MyTestObject, OCderiv, Ftst>,
    //&doLoadObjectFactoryDzero<MyTestObject, OCderiv, Fbsl>
};
static const int TEST_BASE_ARRAY_SIZE =
                            sizeof(TEST_BASE_ARRAY)/sizeof(TEST_BASE_ARRAY[0]);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef void (*TestConstBaseFn)(int, int, int,
                                TestLoadArgs<const MyTestObject> *);

static const TestConstBaseFn TEST_CONST_BASE_ARRAY[] = {
    // default test
    &doLoad<const MyTestObject>,

    // single object-pointer tests
    &doLoadOnull <const MyTestObject>,

    &doLoadObject<const MyTestObject, Obase>,
    &doLoadObject<const MyTestObject, Oderiv>,
    &doLoadObject<const MyTestObject, OCbase>,
    &doLoadObject<const MyTestObject, OCderiv>,

    // factory tests
    &doLoadOnullFnull <const MyTestObject>,

    &doLoadObjectFactory<const MyTestObject, Obase,   Ftst>,
    &doLoadObjectFactory<const MyTestObject, Obase,   Fbsl>,
    &doLoadObjectFactory<const MyTestObject, Oderiv,  Ftst>,
    &doLoadObjectFactory<const MyTestObject, Oderiv,  Fbsl>,
    &doLoadObjectFactory<const MyTestObject, OCbase,  Ftst>,
    &doLoadObjectFactory<const MyTestObject, OCbase,  Fbsl>,
    &doLoadObjectFactory<const MyTestObject, OCderiv, Ftst>,
    &doLoadObjectFactory<const MyTestObject, OCderiv, Fbsl>,

    // deleter tests
    &doLoadOnullFnullDnull <const MyTestObject>,

    // First test the non-deprecated interface, using the policy
    // 'DVoidVoid'.

    // MyTestObject
    &doLoadObjectFactoryDeleter<const MyTestObject, Obase,   Ftst,
                                         DVoidVoid< Obase,   Ftst > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Obase,   Fbsl,
                                         DVoidVoid< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, Obase,   Ftst,
                                         DVoidVoid< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<const MyTestObject, Obase,   Ftst,
                                         DVoidVoid< OCbase,  Ftst > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Obase,   Fbsl,
                                         DVoidVoid< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, Obase,   Ftst,
                                         DVoidVoid< OCbase,  Fbsl > >,

    // const MyTestObject
    &doLoadObjectFactoryDeleter<const MyTestObject, OCbase,  Ftst,
                                         DVoidVoid< OCbase,  Ftst > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, OCbase,  Fbsl,
                                         DVoidVoid< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, OCbase,  Ftst,
                                         DVoidVoid< OCbase,  Fbsl > >,

    // MyDerivedObject
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Ftst,
                                         DVoidVoid< Oderiv,  Ftst > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Ftst,
                                         DVoidVoid< Obase,   Ftst > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Fbsl,
                                         DVoidVoid< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Fbsl,
                                         DVoidVoid< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Ftst,
                                         DVoidVoid< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Ftst,
                                         DVoidVoid< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Ftst,
                                         DVoidVoid< OCderiv, Ftst > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Ftst,
                                         DVoidVoid< OCbase,  Ftst > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Fbsl,
                                         DVoidVoid< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Fbsl,
                                         DVoidVoid< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Ftst,
                                         DVoidVoid< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Ftst,
                                         DVoidVoid< OCbase,  Fbsl > >,

    // const MyDerivedObject
    &doLoadObjectFactoryDeleter<const MyTestObject, OCderiv, Ftst,
                                         DVoidVoid< OCderiv, Ftst > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, OCderiv, Ftst,
                                         DVoidVoid< OCbase,  Ftst > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, OCderiv, Fbsl,
                                         DVoidVoid< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, OCderiv, Fbsl,
                                         DVoidVoid< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, OCderiv, Ftst,
                                         DVoidVoid< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, OCderiv, Ftst,
                                         DVoidVoid< OCbase,  Fbsl > >,


    // Also test a deleter that does not use the 'factory'
    // argument.  These tests must also validate passing a null
    // pointer lvalue as the 'factory' argument.
    &doLoadObjectFactoryDeleter<const MyTestObject, Obase,   Fdflt,
                                          DVoidVoid<Obase,   Fdflt> >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Obase,   Fdflt,
                                          DVoidVoid<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<const MyTestObject, OCbase,  Fdflt,
                                          DVoidVoid<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Fdflt,
                                          DVoidVoid<Oderiv,  Fdflt> >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Fdflt,
                                          DVoidVoid<Obase,   Fdflt> >,

    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Fdflt,
                                          DVoidVoid<OCderiv, Fdflt> >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Fdflt,
                                          DVoidVoid<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<const MyTestObject, OCderiv, Fdflt,
                                          DVoidVoid<OCderiv, Fdflt> >,
    &doLoadObjectFactoryDeleter<const MyTestObject, OCderiv, Fdflt,
                                          DVoidVoid<OCbase,  Fdflt> >,

    // Also, verify null pointer literal can be used for the
    // factory argument in each case.
    &doLoadObjectFnullDeleter<const MyTestObject, Obase,
                                        DVoidVoid<Obase,   Fdflt> >,
    &doLoadObjectFnullDeleter<const MyTestObject, Obase,
                                        DVoidVoid<OCbase,  Fdflt> >,

    &doLoadObjectFnullDeleter<const MyTestObject, OCbase,
                                        DVoidVoid<OCbase,  Fdflt> >,

    &doLoadObjectFnullDeleter<const MyTestObject, Oderiv,
                                        DVoidVoid<Oderiv,  Fdflt> >,
    &doLoadObjectFnullDeleter<const MyTestObject, Oderiv,
                                        DVoidVoid<Obase,   Fdflt> >,

    &doLoadObjectFnullDeleter<const MyTestObject, Oderiv,
                                        DVoidVoid<OCderiv, Fdflt> >,
    &doLoadObjectFnullDeleter<const MyTestObject, Oderiv,
                                        DVoidVoid<OCbase,  Fdflt> >,

    &doLoadObjectFnullDeleter<const MyTestObject, OCderiv,
                                        DVoidVoid<OCderiv, Fdflt> >,
    &doLoadObjectFnullDeleter<const MyTestObject, OCderiv,
                                        DVoidVoid<OCbase,  Fdflt> >,


    // Next we test the deprecated support for deleters other than
    // 'void (*)(void *, void *)', starting with deleters that
    // type-erase the 'object' type, but have a strongly typed
    // 'factory' argument.  Such deleters are generated by the
    // 'DVoidFac' policy..

    // MyTestObject
    &doLoadObjectFactoryDeleter<const MyTestObject, Obase,   Ftst,
                                          DVoidFac< Obase,   Ftst > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Obase,   Fbsl,
                                          DVoidFac< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, Obase,   Ftst,
                                          DVoidFac< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<const MyTestObject, Obase,   Ftst,
                                          DVoidFac< OCbase,  Ftst > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Obase,   Fbsl,
                                          DVoidFac< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, Obase,   Ftst,
                                          DVoidFac< OCbase,  Fbsl > >,

    // const MyTestObject
    &doLoadObjectFactoryDeleter<const MyTestObject, OCbase,  Ftst,
                                          DVoidFac< OCbase,  Ftst > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, OCbase,  Fbsl,
                                          DVoidFac< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, OCbase,  Ftst,
                                          DVoidFac< OCbase,  Fbsl > >,

    // MyDerivedObject
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Ftst,
                                          DVoidFac< Oderiv,  Ftst > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Ftst,
                                          DVoidFac< Obase,   Ftst > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Fbsl,
                                          DVoidFac< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Fbsl,
                                          DVoidFac< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Ftst,
                                          DVoidFac< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Ftst,
                                          DVoidFac< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Ftst,
                                          DVoidFac< OCderiv, Ftst > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Ftst,
                                          DVoidFac< OCbase,  Ftst > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Fbsl,
                                          DVoidFac< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Fbsl,
                                          DVoidFac< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Ftst,
                                          DVoidFac< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Ftst,
                                          DVoidFac< OCbase,  Fbsl > >,

    // const MyDerivedObject
    &doLoadObjectFactoryDeleter<const MyTestObject, OCderiv, Ftst,
                                          DVoidFac< OCderiv, Ftst > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, OCderiv, Ftst,
                                          DVoidFac< OCbase,  Ftst > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, OCderiv, Fbsl,
                                          DVoidFac< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, OCderiv, Fbsl,
                                          DVoidFac< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, OCderiv, Ftst,
                                          DVoidFac< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, OCderiv, Ftst,
                                          DVoidFac< OCbase,  Fbsl > >,


    // Also test a deleter that does not use the 'factory'
    // argument.  These tests must also validate passing a null
    // pointer lvalue as the 'factory' argument.
    &doLoadObjectFactoryDeleter<const MyTestObject, Obase,   Fdflt,
                                           DVoidFac<Obase,   Fdflt> >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Obase,   Fdflt,
                                           DVoidFac<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<const MyTestObject, OCbase,  Fdflt,
                                           DVoidFac<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Fdflt,
                                           DVoidFac<Oderiv,  Fdflt> >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Fdflt,
                                           DVoidFac<Obase,   Fdflt> >,

    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Fdflt,
                                           DVoidFac<OCderiv, Fdflt> >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Fdflt,
                                           DVoidFac<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<const MyTestObject, OCderiv, Fdflt,
                                           DVoidFac<OCderiv, Fdflt> >,
    &doLoadObjectFactoryDeleter<const MyTestObject, OCderiv, Fdflt,
                                           DVoidFac<OCbase,  Fdflt> >,

    // Also, verify null pointer literal can be used for the
    // factory argument in each case.
    // DESIGN NOTE - NULL POINTER LITERALS CAN BE USED ONLY WITH
    //               DELETERS THAT TYPE-ERASE THE FACTORY.
    //&doLoadObjectFnullDeleter<const MyTestObject, Obase,
    //                                     DVoidFac<Obase,   Fdflt> >,
    //&doLoadObjectFnullDeleter<const MyTestObject, Obase,
    //                                     DVoidFac<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<const MyTestObject, Oderiv,
    //                                     DVoidFac<Oderiv,  Fdflt> >,
    //&doLoadObjectFnullDeleter<const MyTestObject, Oderiv,
    //                                     DVoidFac<Obase,   Fdflt> >,

    //&doLoadObjectFnullDeleter<const MyTestObject, Oderiv,
    //                                     DVoidFac<OCderiv, Fdflt> >,
    //&doLoadObjectFnullDeleter<const MyTestObject, Oderiv,
    //                                     DVoidFac<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<const MyTestObject, OCbase,
    //                                     DVoidFac<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<const MyTestObject, OCderiv,
    //                                     DVoidFac<OCderiv, Fdflt> >,
    //&doLoadObjectFnullDeleter<const MyTestObject, OCderiv,
    //                                     DVoidFac<OCbase,  Fdflt> >,


    // Now we test deleters that are strongly typed for the
    // 'object' parameter, but type-erase the 'factory'.

    // MyTestObject
    &doLoadObjectFactoryDeleter<const MyTestObject, Obase,   Ftst,
                                          DObjVoid< Obase,   Ftst > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Obase,   Fbsl,
                                          DObjVoid< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, Obase,   Ftst,
                                          DObjVoid< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<const MyTestObject, Obase,   Ftst,
                                          DObjVoid< OCbase,  Ftst > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Obase,   Fbsl,
                                          DObjVoid< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, Obase,   Ftst,
                                          DObjVoid< OCbase,  Fbsl > >,

    // const MyTestObject
    &doLoadObjectFactoryDeleter<const MyTestObject, OCbase,  Ftst,
                                          DObjVoid< OCbase,  Ftst > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, OCbase,  Fbsl,
                                          DObjVoid< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, OCbase,  Ftst,
                                          DObjVoid< OCbase,  Fbsl > >,

    // MyDerivedObject
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Ftst,
                                          DObjVoid< Oderiv,  Ftst > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Ftst,
                                          DObjVoid< Obase,   Ftst > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Fbsl,
                                          DObjVoid< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Fbsl,
                                          DObjVoid< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Ftst,
                                          DObjVoid< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Ftst,
                                          DObjVoid< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Ftst,
                                          DObjVoid< OCderiv, Ftst > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Ftst,
                                          DObjVoid< OCbase,  Ftst > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Fbsl,
                                          DObjVoid< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Fbsl,
                                          DObjVoid< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Ftst,
                                          DObjVoid< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Ftst,
                                          DObjVoid< OCbase,  Fbsl > >,

    // const MyDerivedObject
    &doLoadObjectFactoryDeleter<const MyTestObject, OCderiv, Ftst,
                                          DObjVoid< OCderiv, Ftst > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, OCderiv, Ftst,
                                          DObjVoid< OCbase,  Ftst > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, OCderiv, Fbsl,
                                          DObjVoid< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, OCderiv, Fbsl,
                                          DObjVoid< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, OCderiv, Ftst,
                                          DObjVoid< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, OCderiv, Ftst,
                                          DObjVoid< OCbase,  Fbsl > >,


    // Also test a deleter that does not use the 'factory'
    // argument.  These tests must also validate passing a null
    // pointer lvalue as the 'factory' argument.
    &doLoadObjectFactoryDeleter<const MyTestObject, Obase,   Fdflt,
                                           DObjVoid<Obase,   Fdflt> >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Obase,   Fdflt,
                                           DObjVoid<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<const MyTestObject, OCbase,  Fdflt,
                                           DObjVoid<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Fdflt,
                                           DObjVoid<Oderiv,  Fdflt> >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Fdflt,
                                           DObjVoid<Obase,   Fdflt> >,

    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Fdflt,
                                           DObjVoid<OCderiv, Fdflt> >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Fdflt,
                                           DObjVoid<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<const MyTestObject, OCderiv, Fdflt,
                                           DObjVoid<OCderiv, Fdflt> >,
    &doLoadObjectFactoryDeleter<const MyTestObject, OCderiv, Fdflt,
                                           DObjVoid<OCbase,  Fdflt> >,

    // Also, verify null pointer literal can be used for the
    // factory argument in each case.
    &doLoadObjectFnullDeleter<const MyTestObject, Obase,
                                         DObjVoid<Obase,   Fdflt> >,
    &doLoadObjectFnullDeleter<const MyTestObject, Obase,
                                         DObjVoid<OCbase,  Fdflt> >,

    &doLoadObjectFnullDeleter<const MyTestObject, OCbase,
                                         DObjVoid<OCbase,  Fdflt> >,

    &doLoadObjectFnullDeleter<const MyTestObject, Oderiv,
                                         DObjVoid<Oderiv,  Fdflt> >,
    &doLoadObjectFnullDeleter<const MyTestObject, Oderiv,
                                         DObjVoid<Obase,   Fdflt> >,

    &doLoadObjectFnullDeleter<const MyTestObject, Oderiv,
                                         DObjVoid<OCderiv, Fdflt> >,
    &doLoadObjectFnullDeleter<const MyTestObject, Oderiv,
                                         DObjVoid<OCbase,  Fdflt> >,

    &doLoadObjectFnullDeleter<const MyTestObject, OCderiv,
                                         DObjVoid<OCderiv, Fdflt> >,
    &doLoadObjectFnullDeleter<const MyTestObject, OCderiv,
                                         DObjVoid<OCbase,  Fdflt> >,


    // Finally we test the most generic combination of generic
    // object type, a factory, and a deleter taking two arguments
    // compatible with pointers to the invoking 'object' and
    // 'factory' types.

    // MyTestObject
    &doLoadObjectFactoryDeleter<const MyTestObject, Obase,   Ftst,
                                           DObjFac< Obase,   Ftst > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Obase,   Fbsl,
                                           DObjFac< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, Obase,   Ftst,
                                           DObjFac< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<const MyTestObject, Obase,   Ftst,
                                           DObjFac< OCbase,  Ftst > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Obase,   Fbsl,
                                           DObjFac< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, Obase,   Ftst,
                                           DObjFac< OCbase,  Fbsl > >,

    // const MyTestObject
    &doLoadObjectFactoryDeleter<const MyTestObject, OCbase,  Ftst,
                                           DObjFac< OCbase,  Ftst > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, OCbase,  Fbsl,
                                           DObjFac< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, OCbase,  Ftst,
                                           DObjFac< OCbase,  Fbsl > >,

    // MyDerivedObject
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Ftst,
                                           DObjFac< Oderiv,  Ftst > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Ftst,
                                           DObjFac< Obase,   Ftst > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Fbsl,
                                           DObjFac< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Fbsl,
                                           DObjFac< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Ftst,
                                           DObjFac< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Ftst,
                                           DObjFac< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Ftst,
                                           DObjFac< OCderiv, Ftst > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Ftst,
                                           DObjFac< OCbase,  Ftst > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Fbsl,
                                           DObjFac< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Fbsl,
                                           DObjFac< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Ftst,
                                           DObjFac< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Ftst,
                                           DObjFac< OCbase,  Fbsl > >,

    // const MyDerivedObject
    &doLoadObjectFactoryDeleter<const MyTestObject, OCderiv, Ftst,
                                           DObjFac< OCderiv, Ftst > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, OCderiv, Ftst,
                                           DObjFac< OCbase,  Ftst > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, OCderiv, Fbsl,
                                           DObjFac< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, OCderiv, Fbsl,
                                           DObjFac< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<const MyTestObject, OCderiv, Ftst,
                                           DObjFac< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<const MyTestObject, OCderiv, Ftst,
                                           DObjFac< OCbase,  Fbsl > >,


    // Also test a deleter that does not use the 'factory'
    // argument.  These tests must also validate passing a null
    // pointer lvalue as the 'factory' argument.
    &doLoadObjectFactoryDeleter<const MyTestObject, Obase,   Fdflt,
                                            DObjFac<Obase,   Fdflt> >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Obase,   Fdflt,
                                            DObjFac<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<const MyTestObject, OCbase,  Fdflt,
                                            DObjFac<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Fdflt,
                                            DObjFac<Oderiv,  Fdflt> >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Fdflt,
                                            DObjFac<Obase,   Fdflt> >,

    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Fdflt,
                                            DObjFac<OCderiv, Fdflt> >,
    &doLoadObjectFactoryDeleter<const MyTestObject, Oderiv,  Fdflt,
                                            DObjFac<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<const MyTestObject, OCderiv, Fdflt,
                                            DObjFac<OCderiv, Fdflt> >,
    &doLoadObjectFactoryDeleter<const MyTestObject, OCderiv, Fdflt,
                                            DObjFac<OCbase,  Fdflt> >,

    // Also, verify null pointer literal can be used for the
    // factory argument in each case.
    // DESIGN NOTE - NULL POINTER LITERALS CAN BE USED ONLY WITH
    //               DELETERS THAT TYPE-ERASE THE FACTORY.
    //&doLoadObjectFnullDeleter<const MyTestObject, Obase,
    //                                      DObjFac<Obase,   Fdflt> >,
    //&doLoadObjectFnullDeleter<const MyTestObject, Obase,
    //                                      DObjFac<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<const MyTestObject, OCbase,
    //                                      DObjFac<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<const MyTestObject, Oderiv,
    //                                      DObjFac<Oderiv,  Fdflt> >,
    //&doLoadObjectFnullDeleter<const MyTestObject, Oderiv,
    //                                      DObjFac<Obase,   Fdflt> >,

    //&doLoadObjectFnullDeleter<const MyTestObject, Oderiv,
    //                                      DObjFac<OCderiv, Fdflt> >,
    //&doLoadObjectFnullDeleter<const MyTestObject, Oderiv,
    //                                      DObjFac<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<const MyTestObject, OCderiv,
    //                                      DObjFac<OCderiv, Fdflt> >,
    //&doLoadObjectFnullDeleter<const MyTestObject, OCderiv,
    //                                      DObjFac<OCbase,  Fdflt> >,


    // negative tests for deleters look for a null pointer lvalue.
    // Note that null pointer literal would be a compile-fail test
    &doLoadObjectFactoryDzero<const MyTestObject, Obase,   Ftst>,
    &doLoadObjectFactoryDzero<const MyTestObject, Obase,   Fbsl>,
    &doLoadObjectFactoryDzero<const MyTestObject, Oderiv,  Ftst>,
    &doLoadObjectFactoryDzero<const MyTestObject, Oderiv,  Fbsl>,
    &doLoadObjectFactoryDzero<const MyTestObject, OCbase,  Ftst>,
    &doLoadObjectFactoryDzero<const MyTestObject, OCbase,  Fbsl>,
    &doLoadObjectFactoryDzero<const MyTestObject, OCderiv, Ftst>,
    &doLoadObjectFactoryDzero<const MyTestObject, OCderiv, Fbsl>
};
static const int TEST_CONST_BASE_ARRAY_SIZE =
                sizeof(TEST_CONST_BASE_ARRAY)/sizeof(TEST_CONST_BASE_ARRAY[0]);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef void (*TestDerivedFn)(int, int, int, TestLoadArgs<MyDerivedObject> *);

static const TestDerivedFn TEST_DERIVED_ARRAY[] = {
    // default test
    &doLoad<MyDerivedObject>,

    // single object-pointer tests
    &doLoadOnull <MyDerivedObject>,

    //&doLoadObject<MyDerivedObject, Obase>,
    &doLoadObject<MyDerivedObject, Oderiv>,
    //&doLoadObject<MyDerivedObject, OCbase>,
    //&doLoadObject<MyDerivedObject, OCderiv>,

    // factory tests
    &doLoadOnullFnull <MyDerivedObject>,

    //&doLoadObjectFactory<MyDerivedObject, Obase,   Ftst>,
    //&doLoadObjectFactory<MyDerivedObject, Obase,   Fbsl>,
    &doLoadObjectFactory<MyDerivedObject, Oderiv,  Ftst>,
    &doLoadObjectFactory<MyDerivedObject, Oderiv,  Fbsl>,
    //&doLoadObjectFactory<MyDerivedObject, OCbase,  Ftst>,
    //&doLoadObjectFactory<MyDerivedObject, OCbase,  Fbsl>,
    //&doLoadObjectFactory<MyDerivedObject, OCderiv, Ftst>,
    //&doLoadObjectFactory<MyDerivedObject, OCderiv, Fbsl>,

    // deleter tests
    &doLoadOnullFnullDnull <MyDerivedObject>,

    // First test the non-deprecated interface, using the policy
    // 'DVoidVoid'.

    // MyTestObject
    //&doLoadObjectFactoryDeleter<MyDerivedObject, Obase,   Ftst,
    //                                  DVoidVoid< Obase,   Ftst > >,
    //&doLoadObjectFactoryDeleter<MyDerivedObject, Obase,   Fbsl,
    //                                  DVoidVoid< Obase,   Fbsl > >,

    //&doLoadObjectFactoryDeleter<MyDerivedObject, Obase,   Ftst,
    //                                  DVoidVoid< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    //&doLoadObjectFactoryDeleter<MyDerivedObject, Obase,   Ftst,
    //                                  DVoidVoid< OCbase,  Ftst > >,
    //&doLoadObjectFactoryDeleter<MyDerivedObject, Obase,   Fbsl,
    //                                  DVoidVoid< OCbase,  Fbsl > >,

    //&doLoadObjectFactoryDeleter<MyDerivedObject, Obase,   Ftst,
    //                                  DVoidVoid< OCbase,  Fbsl > >,

    // const MyTestObject
    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCbase,  Ftst,
    //                                  DVoidVoid< OCbase,  Ftst > >,
    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCbase,  Fbsl,
    //                                  DVoidVoid< OCbase,  Fbsl > >,

    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCbase,  Ftst,
    //                                  DVoidVoid< OCbase,  Fbsl > >,

    // MyDerivedObject
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Ftst,
                                      DVoidVoid< Oderiv,  Ftst > >,
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Ftst,
                                      DVoidVoid< Obase,   Ftst > >,

    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Fbsl,
                                      DVoidVoid< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Fbsl,
                                      DVoidVoid< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Ftst,
                                      DVoidVoid< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Ftst,
                                      DVoidVoid< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Ftst,
                                      DVoidVoid< OCderiv, Ftst > >,
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Ftst,
                                      DVoidVoid< OCbase,  Ftst > >,

    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Fbsl,
                                      DVoidVoid< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Fbsl,
                                      DVoidVoid< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Ftst,
                                      DVoidVoid< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Ftst,
                                      DVoidVoid< OCbase,  Fbsl > >,

    // const MyDerivedObject
    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCderiv, Ftst,
    //                                  DVoidVoid< OCderiv, Ftst > >,
    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCderiv, Ftst,
    //                                  DVoidVoid< OCbase,  Ftst > >,

    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCderiv, Fbsl,
    //                                  DVoidVoid< OCderiv, Fbsl > >,
    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCderiv, Fbsl,
    //                                  DVoidVoid< OCbase,  Fbsl > >,

    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCderiv, Ftst,
    //                                  DVoidVoid< OCderiv, Fbsl > >,
    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCderiv, Ftst,
    //                                  DVoidVoid< OCbase,  Fbsl > >,


    // Also test a deleter that does not use the 'factory'
    // argument.  These tests must also validate passing a null
    // pointer lvalue as the 'factory' argument.
    //&doLoadObjectFactoryDeleter<MyDerivedObject, Obase,   Fdflt,
    //                                   DVoidVoid<Obase,   Fdflt> >,
    //&doLoadObjectFactoryDeleter<MyDerivedObject, Obase,   Fdflt,
    //                                   DVoidVoid<OCbase,  Fdflt> >,

    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCbase,  Fdflt,
    //                                   DVoidVoid<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Fdflt,
                                       DVoidVoid<Oderiv,  Fdflt> >,
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Fdflt,
                                       DVoidVoid<Obase,   Fdflt> >,

    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Fdflt,
                                       DVoidVoid<OCderiv, Fdflt> >,
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Fdflt,
                                       DVoidVoid<OCbase,  Fdflt> >,

    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCderiv, Fdflt,
    //                                   DVoidVoid<OCderiv, Fdflt> >,
    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCderiv, Fdflt,
    //                                   DVoidVoid<OCbase,  Fdflt> >,

    // Also, verify null pointer literal can be used for the
    // factory argument in each case.
    //&doLoadObjectFnullDeleter<MyDerivedObject, Obase,
    //                                 DVoidVoid<Obase,   Fdflt> >,
    //&doLoadObjectFnullDeleter<MyDerivedObject, Obase,
    //                                 DVoidVoid<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<MyDerivedObject, OCbase,
    //                                 DVoidVoid<OCbase,  Fdflt> >,

    &doLoadObjectFnullDeleter<MyDerivedObject, Oderiv,
                                     DVoidVoid<Oderiv,  Fdflt> >,
    &doLoadObjectFnullDeleter<MyDerivedObject, Oderiv,
                                     DVoidVoid<Obase,   Fdflt> >,

    &doLoadObjectFnullDeleter<MyDerivedObject, Oderiv,
                                     DVoidVoid<OCderiv, Fdflt> >,
    &doLoadObjectFnullDeleter<MyDerivedObject, Oderiv,
                                     DVoidVoid<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<MyDerivedObject, OCderiv,
    //                                 DVoidVoid<OCderiv, Fdflt> >,
    //&doLoadObjectFnullDeleter<MyDerivedObject, OCderiv,
    //                                 DVoidVoid<OCbase,  Fdflt> >,


    // Next we test the deprecated support for deleters other than
    // 'void (*)(void *, void *)', starting with deleters that
    // type-erase the 'object' type, but have a strongly typed
    // 'factory' argument.  Such deleters are generated by the
    // 'DVoidFac' policy..

    // MyTestObject
    //&doLoadObjectFactoryDeleter<MyDerivedObject, Obase,   Ftst,
    //                                   DVoidFac< Obase,   Ftst > >,
    //&doLoadObjectFactoryDeleter<MyDerivedObject, Obase,   Fbsl,
    //                                   DVoidFac< Obase,   Fbsl > >,

    //&doLoadObjectFactoryDeleter<MyDerivedObject, Obase,   Ftst,
    //                                   DVoidFac< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    //&doLoadObjectFactoryDeleter<MyDerivedObject, Obase,   Ftst,
    //                                   DVoidFac< OCbase,  Ftst > >,
    //&doLoadObjectFactoryDeleter<MyDerivedObject, Obase,   Fbsl,
    //                                   DVoidFac< OCbase,  Fbsl > >,

    //&doLoadObjectFactoryDeleter<MyDerivedObject, Obase,   Ftst,
    //                                   DVoidFac< OCbase,  Fbsl > >,

    // const MyTestObject
    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCbase,  Ftst,
    //                                   DVoidFac< OCbase,  Ftst > >,
    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCbase,  Fbsl,
    //                                   DVoidFac< OCbase,  Fbsl > >,

    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCbase,  Ftst,
    //                                   DVoidFac< OCbase,  Fbsl > >,

    // MyDerivedObject
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Ftst,
                                       DVoidFac< Oderiv,  Ftst > >,
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Ftst,
                                       DVoidFac< Obase,   Ftst > >,

    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Fbsl,
                                       DVoidFac< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Fbsl,
                                       DVoidFac< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Ftst,
                                       DVoidFac< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Ftst,
                                       DVoidFac< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Ftst,
                                       DVoidFac< OCderiv, Ftst > >,
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Ftst,
                                       DVoidFac< OCbase,  Ftst > >,

    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Fbsl,
                                       DVoidFac< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Fbsl,
                                       DVoidFac< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Ftst,
                                       DVoidFac< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Ftst,
                                       DVoidFac< OCbase,  Fbsl > >,

    // const MyDerivedObject
    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCderiv, Ftst,
    //                                   DVoidFac< OCderiv, Ftst > >,
    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCderiv, Ftst,
    //                                   DVoidFac< OCbase,  Ftst > >,

    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCderiv, Fbsl,
    //                                   DVoidFac< OCderiv, Fbsl > >,
    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCderiv, Fbsl,
    //                                   DVoidFac< OCbase,  Fbsl > >,

    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCderiv, Ftst,
    //                                   DVoidFac< OCderiv, Fbsl > >,
    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCderiv, Ftst,
    //                                   DVoidFac< OCbase,  Fbsl > >,


    // Also test a deleter that does not use the 'factory'
    // argument.  These tests must also validate passing a null
    // pointer lvalue as the 'factory' argument.
    //&doLoadObjectFactoryDeleter<MyDerivedObject, Obase,   Fdflt,
    //                                    DVoidFac<Obase,   Fdflt> >,
    //&doLoadObjectFactoryDeleter<MyDerivedObject, Obase,   Fdflt,
    //                                    DVoidFac<OCbase,  Fdflt> >,

    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCbase,  Fdflt,
    //                                    DVoidFac<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Fdflt,
                                        DVoidFac<Oderiv,  Fdflt> >,
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Fdflt,
                                        DVoidFac<Obase,   Fdflt> >,

    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Fdflt,
                                        DVoidFac<OCderiv, Fdflt> >,
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Fdflt,
                                        DVoidFac<OCbase,  Fdflt> >,

    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCderiv, Fdflt,
    //                                    DVoidFac<OCderiv, Fdflt> >,
    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCderiv, Fdflt,
    //                                    DVoidFac<OCbase,  Fdflt> >,

    // Also, verify null pointer literal can be used for the
    // factory argument in each case.
    // DESIGN NOTE - NULL POINTER LITERALS CAN BE USED ONLY WITH
    //               DELETERS THAT TYPE-ERASE THE FACTORY.
    //&doLoadObjectFnullDeleter<MyDerivedObject, Obase,
    //                                  DVoidFac<Obase,   Fdflt> >,
    //&doLoadObjectFnullDeleter<MyDerivedObject, Obase,
    //                                  DVoidFac<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<MyDerivedObject, Oderiv,
    //                                  DVoidFac<Oderiv,  Fdflt> >,
    //&doLoadObjectFnullDeleter<MyDerivedObject, Oderiv,
    //                                  DVoidFac<Obase,   Fdflt> >,

    //&doLoadObjectFnullDeleter<MyDerivedObject, Oderiv,
    //                                  DVoidFac<OCderiv, Fdflt> >,
    //&doLoadObjectFnullDeleter<MyDerivedObject, Oderiv,
    //                                  DVoidFac<OCbase,  Fdflt> >,

    // HERE WE ARE DOUBLY-BROKEN AS CV-QUALIFIED TYPES ARE NOT
    // SUPPORTED FOR TYPE-ERASURE THROUGH DELETER
    //&doLoadObjectFnullDeleter<MyDerivedObject, OCbase,
    //                                  DVoidFac<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<MyDerivedObject, OCderiv,
    //                                  DVoidFac<OCderiv, Fdflt> >,
    //&doLoadObjectFnullDeleter<MyDerivedObject, OCderiv,
    //                                  DVoidFac<OCbase,  Fdflt> >,


    // Now we test deleters that are strongly typed for the
    // 'object' parameter, but type-erase the 'factory'.

    // MyTestObject
    //&doLoadObjectFactoryDeleter<MyDerivedObject, Obase,   Ftst,
    //                                   DObjVoid< Obase,   Ftst > >,
    //&doLoadObjectFactoryDeleter<MyDerivedObject, Obase,   Fbsl,
    //                                   DObjVoid< Obase,   Fbsl > >,

    //&doLoadObjectFactoryDeleter<MyDerivedObject, Obase,   Ftst,
    //                                   DObjVoid< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    //&doLoadObjectFactoryDeleter<MyDerivedObject, Obase,   Ftst,
    //                                   DObjVoid< OCbase,  Ftst > >,
    //&doLoadObjectFactoryDeleter<MyDerivedObject, Obase,   Fbsl,
    //                                   DObjVoid< OCbase,  Fbsl > >,

    //&doLoadObjectFactoryDeleter<MyDerivedObject, Obase,   Ftst,
    //                                   DObjVoid< OCbase,  Fbsl > >,

    // const MyTestObject
    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCbase,  Ftst,
    //                                   DObjVoid< OCbase,  Ftst > >,
    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCbase,  Fbsl,
    //                                   DObjVoid< OCbase,  Fbsl > >,

    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCbase,  Ftst,
    //                                   DObjVoid< OCbase,  Fbsl > >,

    // MyDerivedObject
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Ftst,
                                       DObjVoid< Oderiv,  Ftst > >,
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Ftst,
                                       DObjVoid< Obase,   Ftst > >,

    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Fbsl,
                                       DObjVoid< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Fbsl,
                                       DObjVoid< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Ftst,
                                       DObjVoid< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Ftst,
                                       DObjVoid< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Ftst,
                                       DObjVoid< OCderiv, Ftst > >,
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Ftst,
                                       DObjVoid< OCbase,  Ftst > >,

    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Fbsl,
                                       DObjVoid< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Fbsl,
                                       DObjVoid< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Ftst,
                                       DObjVoid< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Ftst,
                                       DObjVoid< OCbase,  Fbsl > >,

    // const MyDerivedObject
    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCderiv, Ftst,
    //                                   DObjVoid< OCderiv, Ftst > >,
    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCderiv, Ftst,
    //                                   DObjVoid< OCbase,  Ftst > >,

    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCderiv, Fbsl,
    //                                   DObjVoid< OCderiv, Fbsl > >,
    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCderiv, Fbsl,
    //                                   DObjVoid< OCbase,  Fbsl > >,

    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCderiv, Ftst,
    //                                   DObjVoid< OCderiv, Fbsl > >,
    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCderiv, Ftst,
    //                                   DObjVoid< OCbase,  Fbsl > >,


    // Also test a deleter that does not use the 'factory'
    // argument.  These tests must also validate passing a null
    // pointer lvalue as the 'factory' argument.
    //&doLoadObjectFactoryDeleter<MyDerivedObject, Obase,   Fdflt,
    //                                    DObjVoid<Obase,   Fdflt> >,
    //&doLoadObjectFactoryDeleter<MyDerivedObject, Obase,   Fdflt,
    //                                    DObjVoid<OCbase,  Fdflt> >,

    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCbase,  Fdflt,
    //                                    DObjVoid<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Fdflt,
                                        DObjVoid<Oderiv,  Fdflt> >,
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Fdflt,
                                        DObjVoid<Obase,   Fdflt> >,

    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Fdflt,
                                        DObjVoid<OCderiv, Fdflt> >,
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Fdflt,
                                        DObjVoid<OCbase,  Fdflt> >,

    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCderiv, Fdflt,
    //                                    DObjVoid<OCderiv, Fdflt> >,
    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCderiv, Fdflt,
    //                                    DObjVoid<OCbase,  Fdflt> >,

    // Also, verify null pointer literal can be used for the
    // factory argument in each case.
    //&doLoadObjectFnullDeleter<MyDerivedObject, Obase,
    //                                  DObjVoid<Obase,   Fdflt> >,
    //&doLoadObjectFnullDeleter<MyDerivedObject, Obase,
    //                                  DObjVoid<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<MyDerivedObject, OCbase,
    //                                  DObjVoid<OCbase,  Fdflt> >,

    &doLoadObjectFnullDeleter<MyDerivedObject, Oderiv,
                                      DObjVoid<Oderiv,  Fdflt> >,
    &doLoadObjectFnullDeleter<MyDerivedObject, Oderiv,
                                      DObjVoid<Obase,   Fdflt> >,

    &doLoadObjectFnullDeleter<MyDerivedObject, Oderiv,
                                      DObjVoid<OCderiv, Fdflt> >,
    &doLoadObjectFnullDeleter<MyDerivedObject, Oderiv,
                                      DObjVoid<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<MyDerivedObject, OCderiv,
    //                                  DObjVoid<OCderiv, Fdflt> >,
    //&doLoadObjectFnullDeleter<MyDerivedObject, OCderiv,
    //                                  DObjVoid<OCbase,  Fdflt> >,


    // Finally we test the most generic combination of generic
    // object type, a factory, and a deleter taking two arguments
    // compatible with pointers to the invoking 'object' and
    // 'factory' types.

    // MyTestObject
    //&doLoadObjectFactoryDeleter<MyDerivedObject, Obase,   Ftst,
    //                                    DObjFac< Obase,   Ftst > >,
    //&doLoadObjectFactoryDeleter<MyDerivedObject, Obase,   Fbsl,
    //                                    DObjFac< Obase,   Fbsl > >,

    //&doLoadObjectFactoryDeleter<MyDerivedObject, Obase,   Ftst,
    //                                    DObjFac< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    //&doLoadObjectFactoryDeleter<MyDerivedObject, Obase,   Ftst,
    //                                    DObjFac< OCbase,  Ftst > >,
    //&doLoadObjectFactoryDeleter<MyDerivedObject, Obase,   Fbsl,
    //                                    DObjFac< OCbase,  Fbsl > >,

    //&doLoadObjectFactoryDeleter<MyDerivedObject, Obase,   Ftst,
    //                                    DObjFac< OCbase,  Fbsl > >,

    // const MyTestObject
    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCbase,  Ftst,
    //                                    DObjFac< OCbase,  Ftst > >,
    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCbase,  Fbsl,
    //                                    DObjFac< OCbase,  Fbsl > >,

    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCbase,  Ftst,
    //                                    DObjFac< OCbase,  Fbsl > >,

    // MyDerivedObject
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Ftst,
                                        DObjFac< Oderiv,  Ftst > >,
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Ftst,
                                        DObjFac< Obase,   Ftst > >,

    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Fbsl,
                                        DObjFac< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Fbsl,
                                        DObjFac< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Ftst,
                                        DObjFac< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Ftst,
                                        DObjFac< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Ftst,
                                        DObjFac< OCderiv, Ftst > >,
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Ftst,
                                        DObjFac< OCbase,  Ftst > >,

    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Fbsl,
                                        DObjFac< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Fbsl,
                                        DObjFac< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Ftst,
                                        DObjFac< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Ftst,
                                        DObjFac< OCbase,  Fbsl > >,

    // const MyDerivedObject
    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCderiv, Ftst,
    //                                    DObjFac< OCderiv, Ftst > >,
    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCderiv, Ftst,
    //                                    DObjFac< OCbase,  Ftst > >,

    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCderiv, Fbsl,
    //                                    DObjFac< OCderiv, Fbsl > >,
    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCderiv, Fbsl,
    //                                    DObjFac< OCbase,  Fbsl > >,

    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCderiv, Ftst,
    //                                    DObjFac< OCderiv, Fbsl > >,
    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCderiv, Ftst,
    //                                    DObjFac< OCbase,  Fbsl > >,


    // Also test a deleter that does not use the 'factory'
    // argument.  These tests must also validate passing a null
    // pointer lvalue as the 'factory' argument.
    //&doLoadObjectFactoryDeleter<MyDerivedObject, Obase,   Fdflt,
    //                                     DObjFac<Obase,   Fdflt> >,
    //&doLoadObjectFactoryDeleter<MyDerivedObject, Obase,   Fdflt,
    //                                     DObjFac<OCbase,  Fdflt> >,

    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCbase,  Fdflt,
    //                                     DObjFac<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Fdflt,
                                         DObjFac<Oderiv,  Fdflt> >,
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Fdflt,
                                         DObjFac<Obase,   Fdflt> >,

    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Fdflt,
                                         DObjFac<OCderiv, Fdflt> >,
    &doLoadObjectFactoryDeleter<MyDerivedObject, Oderiv,  Fdflt,
                                         DObjFac<OCbase,  Fdflt> >,

    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCderiv, Fdflt,
    //                                     DObjFac<OCderiv, Fdflt> >,
    //&doLoadObjectFactoryDeleter<MyDerivedObject, OCderiv, Fdflt,
    //                                     DObjFac<OCbase,  Fdflt> >,

    // Also, verify null pointer literal can be used for the
    // factory argument in each case.
    // DESIGN NOTE - NULL POINTER LITERALS CAN BE USED ONLY WITH
    //               DELETERS THAT TYPE-ERASE THE FACTORY.
    //&doLoadObjectFnullDeleter<MyDerivedObject, Obase,
    //                                   DObjFac<Obase,   Fdflt> >,
    //&doLoadObjectFnullDeleter<MyDerivedObject, Obase,
    //                                   DObjFac<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<MyDerivedObject, OCbase,
    //                                   DObjFac<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<MyDerivedObject, Oderiv,
    //                                   DObjFac<Oderiv,  Fdflt> >,
    //&doLoadObjectFnullDeleter<MyDerivedObject, Oderiv,
    //                                   DObjFac<Obase,   Fdflt> >,

    //&doLoadObjectFnullDeleter<MyDerivedObject, Oderiv,
    //                                   DObjFac<OCderiv, Fdflt> >,
    //&doLoadObjectFnullDeleter<MyDerivedObject, Oderiv,
    //                                   DObjFac<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<MyDerivedObject, OCderiv,
    //                                   DObjFac<OCderiv, Fdflt> >,
    //&doLoadObjectFnullDeleter<MyDerivedObject, OCderiv,
    //                                   DObjFac<OCbase,  Fdflt> >,


    // negative tests for deleters look for a null pointer lvalue.
    // Note that null pointer literal would be a compile-fail test
    //&doLoadObjectFactoryDzero<MyDerivedObject, Obase,   Ftst>,
    //&doLoadObjectFactoryDzero<MyDerivedObject, Obase,   Fbsl>,
    &doLoadObjectFactoryDzero<MyDerivedObject, Oderiv,  Ftst>,
    &doLoadObjectFactoryDzero<MyDerivedObject, Oderiv,  Fbsl>,
    //&doLoadObjectFactoryDzero<MyDerivedObject, OCbase,  Ftst>,
    //&doLoadObjectFactoryDzero<MyDerivedObject, OCbase,  Fbsl>,
    //&doLoadObjectFactoryDzero<MyDerivedObject, OCderiv, Ftst>,
    //&doLoadObjectFactoryDzero<MyDerivedObject, OCderiv, Fbsl>
};
static const int TEST_DERIVED_ARRAY_SIZE =
                      sizeof(TEST_DERIVED_ARRAY)/sizeof(TEST_DERIVED_ARRAY[0]);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef void (*TestVoidFn)(int, int, int, TestLoadArgs<void> *);

static const TestVoidFn TEST_VOID_ARRAY[] = {
    // default test
    &doLoad<void>,

    // single object-pointer tests
    &doLoadOnull <void>,

    &doLoadObject<void, Obase>,
    &doLoadObject<void, Oderiv>,
    //&doLoadObject<void, OCbase>,
    //&doLoadObject<void, OCderiv>,

    // factory tests
    &doLoadOnullFnull <void>,

    &doLoadObjectFactory<void, Obase,   Ftst>,
    &doLoadObjectFactory<void, Obase,   Fbsl>,
    &doLoadObjectFactory<void, Oderiv,  Ftst>,
    &doLoadObjectFactory<void, Oderiv,  Fbsl>,
    //&doLoadObjectFactory<void, OCbase,  Ftst>,
    //&doLoadObjectFactory<void, OCbase,  Fbsl>,
    //&doLoadObjectFactory<void, OCderiv, Ftst>,
    //&doLoadObjectFactory<void, OCderiv, Fbsl>,

    // deleter tests
    &doLoadOnullFnullDnull <void>,

    // First test the non-deprecated interface, using the policy
    // 'DVoidVoid'.

    // MyTestObject
    &doLoadObjectFactoryDeleter<void, Obase,   Ftst,
                           DVoidVoid< Obase,   Ftst > >,
    &doLoadObjectFactoryDeleter<void, Obase,   Fbsl,
                           DVoidVoid< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<void, Obase,   Ftst,
                           DVoidVoid< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<void, Obase,   Ftst,
                           DVoidVoid< OCbase,  Ftst > >,
    &doLoadObjectFactoryDeleter<void, Obase,   Fbsl,
                           DVoidVoid< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<void, Obase,   Ftst,
                           DVoidVoid< OCbase,  Fbsl > >,

    // const MyTestObject
    //&doLoadObjectFactoryDeleter<void, OCbase,  Ftst,
    //                       DVoidVoid< OCbase,  Ftst > >,
    //&doLoadObjectFactoryDeleter<void, OCbase,  Fbsl,
    //                       DVoidVoid< OCbase,  Fbsl > >,

    //&doLoadObjectFactoryDeleter<void, OCbase,  Ftst,
    //                       DVoidVoid< OCbase,  Fbsl > >,

    // MyDerivedObject
    &doLoadObjectFactoryDeleter<void, Oderiv,  Ftst,
                           DVoidVoid< Oderiv,  Ftst > >,
    &doLoadObjectFactoryDeleter<void, Oderiv,  Ftst,
                           DVoidVoid< Obase,   Ftst > >,

    &doLoadObjectFactoryDeleter<void, Oderiv,  Fbsl,
                           DVoidVoid< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<void, Oderiv,  Fbsl,
                           DVoidVoid< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<void, Oderiv,  Ftst,
                           DVoidVoid< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<void, Oderiv,  Ftst,
                           DVoidVoid< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<void, Oderiv,  Ftst,
                           DVoidVoid< OCderiv, Ftst > >,
    &doLoadObjectFactoryDeleter<void, Oderiv,  Ftst,
                           DVoidVoid< OCbase,  Ftst > >,

    &doLoadObjectFactoryDeleter<void, Oderiv,  Fbsl,
                           DVoidVoid< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<void, Oderiv,  Fbsl,
                           DVoidVoid< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<void, Oderiv,  Ftst,
                           DVoidVoid< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<void, Oderiv,  Ftst,
                           DVoidVoid< OCbase,  Fbsl > >,

    // const MyDerivedObject
    //&doLoadObjectFactoryDeleter<void, OCderiv, Ftst,
    //                       DVoidVoid< OCderiv, Ftst > >,
    //&doLoadObjectFactoryDeleter<void, OCderiv, Ftst,
    //                       DVoidVoid< OCbase,  Ftst > >,

    //&doLoadObjectFactoryDeleter<void, OCderiv, Fbsl,
    //                       DVoidVoid< OCderiv, Fbsl > >,
    //&doLoadObjectFactoryDeleter<void, OCderiv, Fbsl,
    //                       DVoidVoid< OCbase,  Fbsl > >,

    //&doLoadObjectFactoryDeleter<void, OCderiv, Ftst,
    //                       DVoidVoid< OCderiv, Fbsl > >,
    //&doLoadObjectFactoryDeleter<void, OCderiv, Ftst,
    //                       DVoidVoid< OCbase,  Fbsl > >,


    // Also test a deleter that does not use the 'factory'
    // argument.  These tests must also validate passing a null
    // pointer lvalue as the 'factory' argument.
    &doLoadObjectFactoryDeleter<void, Obase,   Fdflt,
                            DVoidVoid<Obase,   Fdflt> >,
    &doLoadObjectFactoryDeleter<void, Obase,   Fdflt,
                            DVoidVoid<OCbase,  Fdflt> >,

    //&doLoadObjectFactoryDeleter<void, OCbase,  Fdflt,
    //                        DVoidVoid<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<void, Oderiv,  Fdflt,
                            DVoidVoid<Oderiv,  Fdflt> >,
    &doLoadObjectFactoryDeleter<void, Oderiv,  Fdflt,
                            DVoidVoid<Obase,   Fdflt> >,

    &doLoadObjectFactoryDeleter<void, Oderiv,  Fdflt,
                            DVoidVoid<OCderiv, Fdflt> >,
    &doLoadObjectFactoryDeleter<void, Oderiv,  Fdflt,
                            DVoidVoid<OCbase,  Fdflt> >,

    //&doLoadObjectFactoryDeleter<void, OCderiv, Fdflt,
    //                        DVoidVoid<OCderiv, Fdflt> >,
    //&doLoadObjectFactoryDeleter<void, OCderiv, Fdflt,
    //                        DVoidVoid<OCbase,  Fdflt> >,

    // Also, verify null pointer literal can be used for the
    // factory argument in each case.
    &doLoadObjectFnullDeleter<void, Obase,
                          DVoidVoid<Obase,   Fdflt> >,
    &doLoadObjectFnullDeleter<void, Obase,
                          DVoidVoid<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<void, OCbase,
    //                      DVoidVoid<OCbase,  Fdflt> >,

    &doLoadObjectFnullDeleter<void, Oderiv,
                          DVoidVoid<Oderiv,  Fdflt> >,
    &doLoadObjectFnullDeleter<void, Oderiv,
                          DVoidVoid<Obase,   Fdflt> >,

    &doLoadObjectFnullDeleter<void, Oderiv,
                          DVoidVoid<OCderiv, Fdflt> >,
    &doLoadObjectFnullDeleter<void, Oderiv,
                          DVoidVoid<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<void, OCderiv,
    //                      DVoidVoid<OCderiv, Fdflt> >,
    //&doLoadObjectFnullDeleter<void, OCderiv,
    //                      DVoidVoid<OCbase,  Fdflt> >,


    // Next we test the deprecated support for deleters other than
    // 'void (*)(void *, void *)', starting with deleters that
    // type-erase the 'object' type, but have a strongly typed
    // 'factory' argument.  Such deleters are generated by the
    // 'DVoidFac' policy..

    // MyTestObject
    &doLoadObjectFactoryDeleter<void, Obase,   Ftst,
                            DVoidFac< Obase,   Ftst > >,
    &doLoadObjectFactoryDeleter<void, Obase,   Fbsl,
                            DVoidFac< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<void, Obase,   Ftst,
                            DVoidFac< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<void, Obase,   Ftst,
                            DVoidFac< OCbase,  Ftst > >,
    &doLoadObjectFactoryDeleter<void, Obase,   Fbsl,
                            DVoidFac< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<void, Obase,   Ftst,
                            DVoidFac< OCbase,  Fbsl > >,

    // const MyTestObject
    //&doLoadObjectFactoryDeleter<void, OCbase,  Ftst,
    //                        DVoidFac< OCbase,  Ftst > >,
    //&doLoadObjectFactoryDeleter<void, OCbase,  Fbsl,
    //                        DVoidFac< OCbase,  Fbsl > >,

    //&doLoadObjectFactoryDeleter<void, OCbase,  Ftst,
    //                        DVoidFac< OCbase,  Fbsl > >,

    // MyDerivedObject
    &doLoadObjectFactoryDeleter<void, Oderiv,  Ftst,
                            DVoidFac< Oderiv,  Ftst > >,
    &doLoadObjectFactoryDeleter<void, Oderiv,  Ftst,
                            DVoidFac< Obase,   Ftst > >,

    &doLoadObjectFactoryDeleter<void, Oderiv,  Fbsl,
                            DVoidFac< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<void, Oderiv,  Fbsl,
                            DVoidFac< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<void, Oderiv,  Ftst,
                            DVoidFac< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<void, Oderiv,  Ftst,
                            DVoidFac< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<void, Oderiv,  Ftst,
                            DVoidFac< OCderiv, Ftst > >,
    &doLoadObjectFactoryDeleter<void, Oderiv,  Ftst,
                            DVoidFac< OCbase,  Ftst > >,

    &doLoadObjectFactoryDeleter<void, Oderiv,  Fbsl,
                            DVoidFac< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<void, Oderiv,  Fbsl,
                            DVoidFac< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<void, Oderiv,  Ftst,
                            DVoidFac< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<void, Oderiv,  Ftst,
                            DVoidFac< OCbase,  Fbsl > >,

    // const MyDerivedObject
    //&doLoadObjectFactoryDeleter<void, OCderiv, Ftst,
    //                        DVoidFac< OCderiv, Ftst > >,
    //&doLoadObjectFactoryDeleter<void, OCderiv, Ftst,
    //                        DVoidFac< OCbase,  Ftst > >,

    //&doLoadObjectFactoryDeleter<void, OCderiv, Fbsl,
    //                        DVoidFac< OCderiv, Fbsl > >,
    //&doLoadObjectFactoryDeleter<void, OCderiv, Fbsl,
    //                        DVoidFac< OCbase,  Fbsl > >,

    //&doLoadObjectFactoryDeleter<void, OCderiv, Ftst,
    //                        DVoidFac< OCderiv, Fbsl > >,
    //&doLoadObjectFactoryDeleter<void, OCderiv, Ftst,
    //                        DVoidFac< OCbase,  Fbsl > >,


    // Also test a deleter that does not use the 'factory'
    // argument.  These tests must also validate passing a null
    // pointer lvalue as the 'factory' argument.
    &doLoadObjectFactoryDeleter<void, Obase,   Fdflt,
                             DVoidFac<Obase,   Fdflt> >,
    &doLoadObjectFactoryDeleter<void, Obase,   Fdflt,
                             DVoidFac<OCbase,  Fdflt> >,

    //&doLoadObjectFactoryDeleter<void, OCbase,  Fdflt,
    //                         DVoidFac<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<void, Oderiv,  Fdflt,
                             DVoidFac<Oderiv,  Fdflt> >,
    &doLoadObjectFactoryDeleter<void, Oderiv,  Fdflt,
                             DVoidFac<Obase,   Fdflt> >,

    &doLoadObjectFactoryDeleter<void, Oderiv,  Fdflt,
                             DVoidFac<OCderiv, Fdflt> >,
    &doLoadObjectFactoryDeleter<void, Oderiv,  Fdflt,
                             DVoidFac<OCbase,  Fdflt> >,

    //&doLoadObjectFactoryDeleter<void, OCderiv, Fdflt,
    //                         DVoidFac<OCderiv, Fdflt> >,
    //&doLoadObjectFactoryDeleter<void, OCderiv, Fdflt,
    //                         DVoidFac<OCbase,  Fdflt> >,

    // Also, verify null pointer literal can be used for the
    // factory argument in each case.
    // DESIGN NOTE - NULL POINTER LITERALS CAN BE USED ONLY WITH
    //               DELETERS THAT TYPE-ERASE THE FACTORY.
    //&doLoadObjectFnullDeleter<void, Obase,
    //                       DVoidFac<Obase,   Fdflt> >,
    //&doLoadObjectFnullDeleter<void, Obase,
    //                       DVoidFac<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<void, Oderiv,
    //                       DVoidFac<Oderiv,  Fdflt> >,
    //&doLoadObjectFnullDeleter<void, Oderiv,
    //                       DVoidFac<Obase,   Fdflt> >,

    //&doLoadObjectFnullDeleter<void, Oderiv,
    //                       DVoidFac<OCderiv, Fdflt> >,
    //&doLoadObjectFnullDeleter<void, Oderiv,
    //                       DVoidFac<OCbase,  Fdflt> >,

    // HERE WE ARE DOUBLY-BROKEN AS CV-QUALIFIED TYPES ARE NOT
    // SUPPORTED FOR TYPE-ERASURE THROUGH DELETER
    //&doLoadObjectFnullDeleter<void, OCbase,
    //                       DVoidFac<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<void, OCderiv,
    //                       DVoidFac<OCderiv, Fdflt> >,
    //&doLoadObjectFnullDeleter<void, OCderiv,
    //                       DVoidFac<OCbase,  Fdflt> >,


    // Now we test deleters that are strongly typed for the
    // 'object' parameter, but type-erase the 'factory'.

    // MyTestObject
    &doLoadObjectFactoryDeleter<void, Obase,   Ftst,
                            DObjVoid< Obase,   Ftst > >,
    &doLoadObjectFactoryDeleter<void, Obase,   Fbsl,
                            DObjVoid< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<void, Obase,   Ftst,
                            DObjVoid< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<void, Obase,   Ftst,
                            DObjVoid< OCbase,  Ftst > >,
    &doLoadObjectFactoryDeleter<void, Obase,   Fbsl,
                            DObjVoid< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<void, Obase,   Ftst,
                            DObjVoid< OCbase,  Fbsl > >,

    // const MyTestObject
    //&doLoadObjectFactoryDeleter<void, OCbase,  Ftst,
    //                        DObjVoid< OCbase,  Ftst > >,
    //&doLoadObjectFactoryDeleter<void, OCbase,  Fbsl,
    //                        DObjVoid< OCbase,  Fbsl > >,

    //&doLoadObjectFactoryDeleter<void, OCbase,  Ftst,
    //                        DObjVoid< OCbase,  Fbsl > >,

    // MyDerivedObject
    &doLoadObjectFactoryDeleter<void, Oderiv,  Ftst,
                            DObjVoid< Oderiv,  Ftst > >,
    &doLoadObjectFactoryDeleter<void, Oderiv,  Ftst,
                            DObjVoid< Obase,   Ftst > >,

    &doLoadObjectFactoryDeleter<void, Oderiv,  Fbsl,
                            DObjVoid< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<void, Oderiv,  Fbsl,
                            DObjVoid< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<void, Oderiv,  Ftst,
                            DObjVoid< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<void, Oderiv,  Ftst,
                            DObjVoid< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<void, Oderiv,  Ftst,
                            DObjVoid< OCderiv, Ftst > >,
    &doLoadObjectFactoryDeleter<void, Oderiv,  Ftst,
                            DObjVoid< OCbase,  Ftst > >,

    &doLoadObjectFactoryDeleter<void, Oderiv,  Fbsl,
                            DObjVoid< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<void, Oderiv,  Fbsl,
                            DObjVoid< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<void, Oderiv,  Ftst,
                            DObjVoid< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<void, Oderiv,  Ftst,
                            DObjVoid< OCbase,  Fbsl > >,

    // const MyDerivedObject
    //&doLoadObjectFactoryDeleter<void, OCderiv, Ftst,
    //                        DObjVoid< OCderiv, Ftst > >,
    //&doLoadObjectFactoryDeleter<void, OCderiv, Ftst,
    //                        DObjVoid< OCbase,  Ftst > >,

    //&doLoadObjectFactoryDeleter<void, OCderiv, Fbsl,
    //                        DObjVoid< OCderiv, Fbsl > >,
    //&doLoadObjectFactoryDeleter<void, OCderiv, Fbsl,
    //                        DObjVoid< OCbase,  Fbsl > >,

    //&doLoadObjectFactoryDeleter<void, OCderiv, Ftst,
    //                        DObjVoid< OCderiv, Fbsl > >,
    //&doLoadObjectFactoryDeleter<void, OCderiv, Ftst,
    //                        DObjVoid< OCbase,  Fbsl > >,


    // Also test a deleter that does not use the 'factory'
    // argument.  These tests must also validate passing a null
    // pointer lvalue as the 'factory' argument.
    &doLoadObjectFactoryDeleter<void, Obase,   Fdflt,
                             DObjVoid<Obase,   Fdflt> >,
    &doLoadObjectFactoryDeleter<void, Obase,   Fdflt,
                             DObjVoid<OCbase,  Fdflt> >,

    //&doLoadObjectFactoryDeleter<void, OCbase,  Fdflt,
    //                         DObjVoid<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<void, Oderiv,  Fdflt,
                             DObjVoid<Oderiv,  Fdflt> >,
    &doLoadObjectFactoryDeleter<void, Oderiv,  Fdflt,
                             DObjVoid<Obase,   Fdflt> >,

    &doLoadObjectFactoryDeleter<void, Oderiv,  Fdflt,
                             DObjVoid<OCderiv, Fdflt> >,
    &doLoadObjectFactoryDeleter<void, Oderiv,  Fdflt,
                             DObjVoid<OCbase,  Fdflt> >,

    //&doLoadObjectFactoryDeleter<void, OCderiv, Fdflt,
    //                         DObjVoid<OCderiv, Fdflt> >,
    //&doLoadObjectFactoryDeleter<void, OCderiv, Fdflt,
    //                         DObjVoid<OCbase,  Fdflt> >,

    // Also, verify null pointer literal can be used for the
    // factory argument in each case.
    &doLoadObjectFnullDeleter<void, Obase,
                           DObjVoid<Obase,   Fdflt> >,
    &doLoadObjectFnullDeleter<void, Obase,
                           DObjVoid<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<void, OCbase,
    //                       DObjVoid<OCbase,  Fdflt> >,

    &doLoadObjectFnullDeleter<void, Oderiv,
                           DObjVoid<Oderiv,  Fdflt> >,
    &doLoadObjectFnullDeleter<void, Oderiv,
                           DObjVoid<Obase,   Fdflt> >,

    &doLoadObjectFnullDeleter<void, Oderiv,
                           DObjVoid<OCderiv, Fdflt> >,
    &doLoadObjectFnullDeleter<void, Oderiv,
                           DObjVoid<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<void, OCderiv,
    //                       DObjVoid<OCderiv, Fdflt> >,
    //&doLoadObjectFnullDeleter<void, OCderiv,
    //                       DObjVoid<OCbase,  Fdflt> >,


    // Finally we test the most generic combination of generic
    // object type, a factory, and a deleter taking two arguments
    // compatible with pointers to the invoking 'object' and
    // 'factory' types.

    // MyTestObject
    &doLoadObjectFactoryDeleter<void, Obase,   Ftst,
                             DObjFac< Obase,   Ftst > >,
    &doLoadObjectFactoryDeleter<void, Obase,   Fbsl,
                             DObjFac< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<void, Obase,   Ftst,
                             DObjFac< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<void, Obase,   Ftst,
                             DObjFac< OCbase,  Ftst > >,
    &doLoadObjectFactoryDeleter<void, Obase,   Fbsl,
                             DObjFac< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<void, Obase,   Ftst,
                             DObjFac< OCbase,  Fbsl > >,

    // const MyTestObject
    //&doLoadObjectFactoryDeleter<void, OCbase,  Ftst,
    //                         DObjFac< OCbase,  Ftst > >,
    //&doLoadObjectFactoryDeleter<void, OCbase,  Fbsl,
    //                         DObjFac< OCbase,  Fbsl > >,

    //&doLoadObjectFactoryDeleter<void, OCbase,  Ftst,
    //                         DObjFac< OCbase,  Fbsl > >,

    // MyDerivedObject
    &doLoadObjectFactoryDeleter<void, Oderiv,  Ftst,
                             DObjFac< Oderiv,  Ftst > >,
    &doLoadObjectFactoryDeleter<void, Oderiv,  Ftst,
                             DObjFac< Obase,   Ftst > >,

    &doLoadObjectFactoryDeleter<void, Oderiv,  Fbsl,
                             DObjFac< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<void, Oderiv,  Fbsl,
                             DObjFac< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<void, Oderiv,  Ftst,
                             DObjFac< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<void, Oderiv,  Ftst,
                             DObjFac< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<void, Oderiv,  Ftst,
                             DObjFac< OCderiv, Ftst > >,
    &doLoadObjectFactoryDeleter<void, Oderiv,  Ftst,
                             DObjFac< OCbase,  Ftst > >,

    &doLoadObjectFactoryDeleter<void, Oderiv,  Fbsl,
                             DObjFac< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<void, Oderiv,  Fbsl,
                             DObjFac< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<void, Oderiv,  Ftst,
                             DObjFac< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<void, Oderiv,  Ftst,
                             DObjFac< OCbase,  Fbsl > >,

    // const MyDerivedObject
    //&doLoadObjectFactoryDeleter<void, OCderiv, Ftst,
    //                         DObjFac< OCderiv, Ftst > >,
    //&doLoadObjectFactoryDeleter<void, OCderiv, Ftst,
    //                         DObjFac< OCbase,  Ftst > >,

    //&doLoadObjectFactoryDeleter<void, OCderiv, Fbsl,
    //                         DObjFac< OCderiv, Fbsl > >,
    //&doLoadObjectFactoryDeleter<void, OCderiv, Fbsl,
    //                         DObjFac< OCbase,  Fbsl > >,

    //&doLoadObjectFactoryDeleter<void, OCderiv, Ftst,
    //                         DObjFac< OCderiv, Fbsl > >,
    //&doLoadObjectFactoryDeleter<void, OCderiv, Ftst,
    //                         DObjFac< OCbase,  Fbsl > >,


    // Also test a deleter that does not use the 'factory'
    // argument.  These tests must also validate passing a null
    // pointer lvalue as the 'factory' argument.
    &doLoadObjectFactoryDeleter<void, Obase,   Fdflt,
                              DObjFac<Obase,   Fdflt> >,
    &doLoadObjectFactoryDeleter<void, Obase,   Fdflt,
                              DObjFac<OCbase,  Fdflt> >,

    //&doLoadObjectFactoryDeleter<void, OCbase,  Fdflt,
    //                          DObjFac<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<void, Oderiv,  Fdflt,
                              DObjFac<Oderiv,  Fdflt> >,
    &doLoadObjectFactoryDeleter<void, Oderiv,  Fdflt,
                              DObjFac<Obase,   Fdflt> >,

    &doLoadObjectFactoryDeleter<void, Oderiv,  Fdflt,
                              DObjFac<OCderiv, Fdflt> >,
    &doLoadObjectFactoryDeleter<void, Oderiv,  Fdflt,
                              DObjFac<OCbase,  Fdflt> >,

    //&doLoadObjectFactoryDeleter<void, OCderiv, Fdflt,
    //                          DObjFac<OCderiv, Fdflt> >,
    //&doLoadObjectFactoryDeleter<void, OCderiv, Fdflt,
    //                          DObjFac<OCbase,  Fdflt> >,

    // Also, verify null pointer literal can be used for the
    // factory argument in each case.
    // DESIGN NOTE - NULL POINTER LITERALS CAN BE USED ONLY WITH
    //               DELETERS THAT TYPE-ERASE THE FACTORY.
    //&doLoadObjectFnullDeleter<void, Obase,
    //                        DObjFac<Obase,   Fdflt> >,
    //&doLoadObjectFnullDeleter<void, Obase,
    //                        DObjFac<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<void, OCbase,
    //                        DObjFac<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<void, Oderiv,
    //                        DObjFac<Oderiv,  Fdflt> >,
    //&doLoadObjectFnullDeleter<void, Oderiv,
    //                        DObjFac<Obase,   Fdflt> >,

    //&doLoadObjectFnullDeleter<void, Oderiv,
    //                        DObjFac<OCderiv, Fdflt> >,
    //&doLoadObjectFnullDeleter<void, Oderiv,
    //                        DObjFac<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<void, OCderiv,
    //                        DObjFac<OCderiv, Fdflt> >,
    //&doLoadObjectFnullDeleter<void, OCderiv,
    //                        DObjFac<OCbase,  Fdflt> >,


    // negative tests for deleters look for a null pointer lvalue.
    // Note that null pointer literal would be a compile-fail test
    &doLoadObjectFactoryDzero<void, Obase,   Ftst>,
    &doLoadObjectFactoryDzero<void, Obase,   Fbsl>,
    &doLoadObjectFactoryDzero<void, Oderiv,  Ftst>,
    &doLoadObjectFactoryDzero<void, Oderiv,  Fbsl>,
    //&doLoadObjectFactoryDzero<void, OCbase,  Ftst>,
    //&doLoadObjectFactoryDzero<void, OCbase,  Fbsl>,
    //&doLoadObjectFactoryDzero<void, OCderiv, Ftst>,
    //&doLoadObjectFactoryDzero<void, OCderiv, Fbsl>
};
static const int TEST_VOID_ARRAY_SIZE =
                            sizeof(TEST_VOID_ARRAY)/sizeof(TEST_VOID_ARRAY[0]);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef void (*TestConstVoidFn)(int, int, int, TestLoadArgs<const void> *);

static const TestConstVoidFn TEST_CONST_VOID_ARRAY[] = {
    // default test
    &doLoad<const void>,

    // single object-pointer tests
    &doLoadOnull <const void>,

    &doLoadObject<const void, Obase>,
    &doLoadObject<const void, Oderiv>,
    &doLoadObject<const void, OCbase>,
    &doLoadObject<const void, OCderiv>,

    // factory tests
    &doLoadOnullFnull <const void>,

    &doLoadObjectFactory<const void, Obase,   Ftst>,
    &doLoadObjectFactory<const void, Obase,   Fbsl>,
    &doLoadObjectFactory<const void, Oderiv,  Ftst>,
    &doLoadObjectFactory<const void, Oderiv,  Fbsl>,
    &doLoadObjectFactory<const void, OCbase,  Ftst>,
    &doLoadObjectFactory<const void, OCbase,  Fbsl>,
    &doLoadObjectFactory<const void, OCderiv, Ftst>,
    &doLoadObjectFactory<const void, OCderiv, Fbsl>,

    // deleter tests
    &doLoadOnullFnullDnull <const void>,

    // First test the non-deprecated interface, using the policy
    // 'DVoidVoid'.

    // MyTestObject
    &doLoadObjectFactoryDeleter<const void, Obase,   Ftst,
                                 DVoidVoid< Obase,   Ftst > >,
    &doLoadObjectFactoryDeleter<const void, Obase,   Fbsl,
                                 DVoidVoid< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<const void, Obase,   Ftst,
                                 DVoidVoid< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<const void, Obase,   Ftst,
                                 DVoidVoid< OCbase,  Ftst > >,
    &doLoadObjectFactoryDeleter<const void, Obase,   Fbsl,
                                 DVoidVoid< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<const void, Obase,   Ftst,
                                 DVoidVoid< OCbase,  Fbsl > >,

    // const MyTestObject
    &doLoadObjectFactoryDeleter<const void, OCbase,  Ftst,
                                 DVoidVoid< OCbase,  Ftst > >,
    &doLoadObjectFactoryDeleter<const void, OCbase,  Fbsl,
                                 DVoidVoid< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<const void, OCbase,  Ftst,
                                 DVoidVoid< OCbase,  Fbsl > >,

    // MyDerivedObject
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Ftst,
                                 DVoidVoid< Oderiv,  Ftst > >,
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Ftst,
                                 DVoidVoid< Obase,   Ftst > >,

    &doLoadObjectFactoryDeleter<const void, Oderiv,  Fbsl,
                                 DVoidVoid< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Fbsl,
                                 DVoidVoid< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<const void, Oderiv,  Ftst,
                                 DVoidVoid< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Ftst,
                                 DVoidVoid< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Ftst,
                                 DVoidVoid< OCderiv, Ftst > >,
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Ftst,
                                 DVoidVoid< OCbase,  Ftst > >,

    &doLoadObjectFactoryDeleter<const void, Oderiv,  Fbsl,
                                 DVoidVoid< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Fbsl,
                                 DVoidVoid< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<const void, Oderiv,  Ftst,
                                 DVoidVoid< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Ftst,
                                 DVoidVoid< OCbase,  Fbsl > >,

    // const MyDerivedObject
    &doLoadObjectFactoryDeleter<const void, OCderiv, Ftst,
                                 DVoidVoid< OCderiv, Ftst > >,
    &doLoadObjectFactoryDeleter<const void, OCderiv, Ftst,
                                 DVoidVoid< OCbase,  Ftst > >,

    &doLoadObjectFactoryDeleter<const void, OCderiv, Fbsl,
                                 DVoidVoid< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<const void, OCderiv, Fbsl,
                                 DVoidVoid< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<const void, OCderiv, Ftst,
                                 DVoidVoid< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<const void, OCderiv, Ftst,
                                 DVoidVoid< OCbase,  Fbsl > >,


    // Also test a deleter that does not use the 'factory'
    // argument.  These tests must also validate passing a null
    // pointer lvalue as the 'factory' argument.
    &doLoadObjectFactoryDeleter<const void, Obase,   Fdflt,
                                  DVoidVoid<Obase,   Fdflt> >,
    &doLoadObjectFactoryDeleter<const void, Obase,   Fdflt,
                                  DVoidVoid<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<const void, OCbase,  Fdflt,
                                  DVoidVoid<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<const void, Oderiv,  Fdflt,
                                  DVoidVoid<Oderiv,  Fdflt> >,
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Fdflt,
                                  DVoidVoid<Obase,   Fdflt> >,

    &doLoadObjectFactoryDeleter<const void, Oderiv,  Fdflt,
                                  DVoidVoid<OCderiv, Fdflt> >,
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Fdflt,
                                  DVoidVoid<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<const void, OCderiv, Fdflt,
                                  DVoidVoid<OCderiv, Fdflt> >,
    &doLoadObjectFactoryDeleter<const void, OCderiv, Fdflt,
                                  DVoidVoid<OCbase,  Fdflt> >,

    // Also, verify null pointer literal can be used for the
    // factory argument in each case.
    &doLoadObjectFnullDeleter<const void, Obase,
                                DVoidVoid<Obase,   Fdflt> >,
    &doLoadObjectFnullDeleter<const void, Obase,
                                DVoidVoid<OCbase,  Fdflt> >,

    &doLoadObjectFnullDeleter<const void, OCbase,
                                DVoidVoid<OCbase,  Fdflt> >,

    &doLoadObjectFnullDeleter<const void, Oderiv,
                                DVoidVoid<Oderiv,  Fdflt> >,
    &doLoadObjectFnullDeleter<const void, Oderiv,
                                DVoidVoid<Obase,   Fdflt> >,

    &doLoadObjectFnullDeleter<const void, Oderiv,
                                DVoidVoid<OCderiv, Fdflt> >,
    &doLoadObjectFnullDeleter<const void, Oderiv,
                                DVoidVoid<OCbase,  Fdflt> >,

    &doLoadObjectFnullDeleter<const void, OCderiv,
                                DVoidVoid<OCderiv, Fdflt> >,
    &doLoadObjectFnullDeleter<const void, OCderiv,
                                DVoidVoid<OCbase,  Fdflt> >,


    // Next we test the deprecated support for deleters other than
    // 'void (*)(void *, void *)', starting with deleters that
    // type-erase the 'object' type, but have a strongly typed
    // 'factory' argument.  Such deleters are generated by the
    // 'DVoidFac' policy..

    // MyTestObject
    &doLoadObjectFactoryDeleter<const void, Obase,   Ftst,
                                  DVoidFac< Obase,   Ftst > >,
    &doLoadObjectFactoryDeleter<const void, Obase,   Fbsl,
                                  DVoidFac< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<const void, Obase,   Ftst,
                                  DVoidFac< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<const void, Obase,   Ftst,
                                  DVoidFac< OCbase,  Ftst > >,
    &doLoadObjectFactoryDeleter<const void, Obase,   Fbsl,
                                  DVoidFac< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<const void, Obase,   Ftst,
                                  DVoidFac< OCbase,  Fbsl > >,

    // const MyTestObject
    &doLoadObjectFactoryDeleter<const void, OCbase,  Ftst,
                                  DVoidFac< OCbase,  Ftst > >,
    &doLoadObjectFactoryDeleter<const void, OCbase,  Fbsl,
                                  DVoidFac< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<const void, OCbase,  Ftst,
                                  DVoidFac< OCbase,  Fbsl > >,

    // MyDerivedObject
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Ftst,
                                  DVoidFac< Oderiv,  Ftst > >,
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Ftst,
                                  DVoidFac< Obase,   Ftst > >,

    &doLoadObjectFactoryDeleter<const void, Oderiv,  Fbsl,
                                  DVoidFac< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Fbsl,
                                  DVoidFac< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<const void, Oderiv,  Ftst,
                                  DVoidFac< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Ftst,
                                  DVoidFac< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Ftst,
                                  DVoidFac< OCderiv, Ftst > >,
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Ftst,
                                  DVoidFac< OCbase,  Ftst > >,

    &doLoadObjectFactoryDeleter<const void, Oderiv,  Fbsl,
                                  DVoidFac< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Fbsl,
                                  DVoidFac< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<const void, Oderiv,  Ftst,
                                  DVoidFac< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Ftst,
                                  DVoidFac< OCbase,  Fbsl > >,

    // const MyDerivedObject
    &doLoadObjectFactoryDeleter<const void, OCderiv, Ftst,
                                  DVoidFac< OCderiv, Ftst > >,
    &doLoadObjectFactoryDeleter<const void, OCderiv, Ftst,
                                  DVoidFac< OCbase,  Ftst > >,

    &doLoadObjectFactoryDeleter<const void, OCderiv, Fbsl,
                                  DVoidFac< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<const void, OCderiv, Fbsl,
                                  DVoidFac< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<const void, OCderiv, Ftst,
                                  DVoidFac< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<const void, OCderiv, Ftst,
                                  DVoidFac< OCbase,  Fbsl > >,


    // Also test a deleter that does not use the 'factory'
    // argument.  These tests must also validate passing a null
    // pointer lvalue as the 'factory' argument.
    &doLoadObjectFactoryDeleter<const void, Obase,   Fdflt,
                                   DVoidFac<Obase,   Fdflt> >,
    &doLoadObjectFactoryDeleter<const void, Obase,   Fdflt,
                                   DVoidFac<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<const void, OCbase,  Fdflt,
                                   DVoidFac<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<const void, Oderiv,  Fdflt,
                                   DVoidFac<Oderiv,  Fdflt> >,
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Fdflt,
                                   DVoidFac<Obase,   Fdflt> >,

    &doLoadObjectFactoryDeleter<const void, Oderiv,  Fdflt,
                                   DVoidFac<OCderiv, Fdflt> >,
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Fdflt,
                                   DVoidFac<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<const void, OCderiv, Fdflt,
                                   DVoidFac<OCderiv, Fdflt> >,
    &doLoadObjectFactoryDeleter<const void, OCderiv, Fdflt,
                                   DVoidFac<OCbase,  Fdflt> >,

    // Also, verify null pointer literal can be used for the
    // factory argument in each case.
    // DESIGN NOTE - NULL POINTER LITERALS CAN BE USED ONLY WITH
    //               DELETERS THAT TYPE-ERASE THE FACTORY.
    //&doLoadObjectFnullDeleter<const void, Obase,
    //                             DVoidFac<Obase,   Fdflt> >,
    //&doLoadObjectFnullDeleter<const void, Obase,
    //                             DVoidFac<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<const void, Oderiv,
    //                             DVoidFac<Oderiv,  Fdflt> >,
    //&doLoadObjectFnullDeleter<const void, Oderiv,
    //                             DVoidFac<Obase,   Fdflt> >,

    //&doLoadObjectFnullDeleter<const void, Oderiv,
    //                             DVoidFac<OCderiv, Fdflt> >,
    //&doLoadObjectFnullDeleter<const void, Oderiv,
    //                             DVoidFac<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<const void, OCbase,
    //                            DVoidFac<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<const void, OCderiv,
    //                            DVoidFac<OCderiv, Fdflt> >,
    //&doLoadObjectFnullDeleter<const void, OCderiv,
    //                            DVoidFac<OCbase,  Fdflt> >,


    // Now we test deleters that are strongly typed for the
    // 'object' parameter, but type-erase the 'factory'.

    // MyTestObject
    &doLoadObjectFactoryDeleter<const void, Obase,   Ftst,
                                  DObjVoid< Obase,   Ftst > >,
    &doLoadObjectFactoryDeleter<const void, Obase,   Fbsl,
                                  DObjVoid< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<const void, Obase,   Ftst,
                                  DObjVoid< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<const void, Obase,   Ftst,
                                  DObjVoid< OCbase,  Ftst > >,
    &doLoadObjectFactoryDeleter<const void, Obase,   Fbsl,
                                  DObjVoid< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<const void, Obase,   Ftst,
                                  DObjVoid< OCbase,  Fbsl > >,

    // const MyTestObject
    &doLoadObjectFactoryDeleter<const void, OCbase,  Ftst,
                                  DObjVoid< OCbase,  Ftst > >,
    &doLoadObjectFactoryDeleter<const void, OCbase,  Fbsl,
                                  DObjVoid< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<const void, OCbase,  Ftst,
                                  DObjVoid< OCbase,  Fbsl > >,

    // MyDerivedObject
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Ftst,
                                  DObjVoid< Oderiv,  Ftst > >,
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Ftst,
                                  DObjVoid< Obase,   Ftst > >,

    &doLoadObjectFactoryDeleter<const void, Oderiv,  Fbsl,
                                  DObjVoid< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Fbsl,
                                  DObjVoid< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<const void, Oderiv,  Ftst,
                                  DObjVoid< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Ftst,
                                  DObjVoid< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Ftst,
                                  DObjVoid< OCderiv, Ftst > >,
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Ftst,
                                  DObjVoid< OCbase,  Ftst > >,

    &doLoadObjectFactoryDeleter<const void, Oderiv,  Fbsl,
                                  DObjVoid< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Fbsl,
                                  DObjVoid< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<const void, Oderiv,  Ftst,
                                  DObjVoid< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Ftst,
                                  DObjVoid< OCbase,  Fbsl > >,

    // const MyDerivedObject
    &doLoadObjectFactoryDeleter<const void, OCderiv, Ftst,
                                  DObjVoid< OCderiv, Ftst > >,
    &doLoadObjectFactoryDeleter<const void, OCderiv, Ftst,
                                  DObjVoid< OCbase,  Ftst > >,

    &doLoadObjectFactoryDeleter<const void, OCderiv, Fbsl,
                                  DObjVoid< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<const void, OCderiv, Fbsl,
                                  DObjVoid< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<const void, OCderiv, Ftst,
                                  DObjVoid< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<const void, OCderiv, Ftst,
                                  DObjVoid< OCbase,  Fbsl > >,


    // Also test a deleter that does not use the 'factory'
    // argument.  These tests must also validate passing a null
    // pointer lvalue as the 'factory' argument.
    &doLoadObjectFactoryDeleter<const void, Obase,   Fdflt,
                                   DObjVoid<Obase,   Fdflt> >,
    &doLoadObjectFactoryDeleter<const void, Obase,   Fdflt,
                                   DObjVoid<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<const void, OCbase,  Fdflt,
                                   DObjVoid<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<const void, Oderiv,  Fdflt,
                                   DObjVoid<Oderiv,  Fdflt> >,
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Fdflt,
                                   DObjVoid<Obase,   Fdflt> >,

    &doLoadObjectFactoryDeleter<const void, Oderiv,  Fdflt,
                                   DObjVoid<OCderiv, Fdflt> >,
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Fdflt,
                                   DObjVoid<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<const void, OCderiv, Fdflt,
                                   DObjVoid<OCderiv, Fdflt> >,
    &doLoadObjectFactoryDeleter<const void, OCderiv, Fdflt,
                                   DObjVoid<OCbase,  Fdflt> >,

    // Also, verify null pointer literal can be used for the
    // factory argument in each case.
    &doLoadObjectFnullDeleter<const void, Obase,
                                 DObjVoid<Obase,   Fdflt> >,
    &doLoadObjectFnullDeleter<const void, Obase,
                                 DObjVoid<OCbase,  Fdflt> >,

    &doLoadObjectFnullDeleter<const void, OCbase,
                                 DObjVoid<OCbase,  Fdflt> >,

    &doLoadObjectFnullDeleter<const void, Oderiv,
                                 DObjVoid<Oderiv,  Fdflt> >,
    &doLoadObjectFnullDeleter<const void, Oderiv,
                                 DObjVoid<Obase,   Fdflt> >,

    &doLoadObjectFnullDeleter<const void, Oderiv,
                                 DObjVoid<OCderiv, Fdflt> >,
    &doLoadObjectFnullDeleter<const void, Oderiv,
                                 DObjVoid<OCbase,  Fdflt> >,

    &doLoadObjectFnullDeleter<const void, OCderiv,
                                 DObjVoid<OCderiv, Fdflt> >,
    &doLoadObjectFnullDeleter<const void, OCderiv,
                                 DObjVoid<OCbase,  Fdflt> >,


    // Finally we test the most generic combination of generic
    // object type, a factory, and a deleter taking two arguments
    // compatible with pointers to the invoking 'object' and
    // 'factory' types.

    // MyTestObject
    &doLoadObjectFactoryDeleter<const void, Obase,   Ftst,
                                   DObjFac< Obase,   Ftst > >,
    &doLoadObjectFactoryDeleter<const void, Obase,   Fbsl,
                                   DObjFac< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<const void, Obase,   Ftst,
                                   DObjFac< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<const void, Obase,   Ftst,
                                   DObjFac< OCbase,  Ftst > >,
    &doLoadObjectFactoryDeleter<const void, Obase,   Fbsl,
                                   DObjFac< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<const void, Obase,   Ftst,
                                   DObjFac< OCbase,  Fbsl > >,

    // const MyTestObject
    &doLoadObjectFactoryDeleter<const void, OCbase,  Ftst,
                                   DObjFac< OCbase,  Ftst > >,
    &doLoadObjectFactoryDeleter<const void, OCbase,  Fbsl,
                                   DObjFac< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<const void, OCbase,  Ftst,
                                   DObjFac< OCbase,  Fbsl > >,

    // MyDerivedObject
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Ftst,
                                   DObjFac< Oderiv,  Ftst > >,
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Ftst,
                                   DObjFac< Obase,   Ftst > >,

    &doLoadObjectFactoryDeleter<const void, Oderiv,  Fbsl,
                                   DObjFac< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Fbsl,
                                   DObjFac< Obase,   Fbsl > >,

    &doLoadObjectFactoryDeleter<const void, Oderiv,  Ftst,
                                   DObjFac< Oderiv,  Fbsl > >,
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Ftst,
                                   DObjFac< Obase,   Fbsl > >,

    // ... plus safe const-conversions
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Ftst,
                                   DObjFac< OCderiv, Ftst > >,
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Ftst,
                                   DObjFac< OCbase,  Ftst > >,

    &doLoadObjectFactoryDeleter<const void, Oderiv,  Fbsl,
                                   DObjFac< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Fbsl,
                                   DObjFac< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<const void, Oderiv,  Ftst,
                                   DObjFac< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Ftst,
                                   DObjFac< OCbase,  Fbsl > >,

    // const MyDerivedObject
    &doLoadObjectFactoryDeleter<const void, OCderiv, Ftst,
                                   DObjFac< OCderiv, Ftst > >,
    &doLoadObjectFactoryDeleter<const void, OCderiv, Ftst,
                                   DObjFac< OCbase,  Ftst > >,

    &doLoadObjectFactoryDeleter<const void, OCderiv, Fbsl,
                                   DObjFac< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<const void, OCderiv, Fbsl,
                                   DObjFac< OCbase,  Fbsl > >,

    &doLoadObjectFactoryDeleter<const void, OCderiv, Ftst,
                                   DObjFac< OCderiv, Fbsl > >,
    &doLoadObjectFactoryDeleter<const void, OCderiv, Ftst,
                                   DObjFac< OCbase,  Fbsl > >,


    // Also test a deleter that does not use the 'factory'
    // argument.  These tests must also validate passing a null
    // pointer lvalue as the 'factory' argument.
    &doLoadObjectFactoryDeleter<const void, Obase,   Fdflt,
                                    DObjFac<Obase,   Fdflt> >,
    &doLoadObjectFactoryDeleter<const void, Obase,   Fdflt,
                                    DObjFac<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<const void, OCbase,  Fdflt,
                                    DObjFac<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<const void, Oderiv,  Fdflt,
                                    DObjFac<Oderiv,  Fdflt> >,
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Fdflt,
                                    DObjFac<Obase,   Fdflt> >,

    &doLoadObjectFactoryDeleter<const void, Oderiv,  Fdflt,
                                    DObjFac<OCderiv, Fdflt> >,
    &doLoadObjectFactoryDeleter<const void, Oderiv,  Fdflt,
                                    DObjFac<OCbase,  Fdflt> >,

    &doLoadObjectFactoryDeleter<const void, OCderiv, Fdflt,
                                    DObjFac<OCderiv, Fdflt> >,
    &doLoadObjectFactoryDeleter<const void, OCderiv, Fdflt,
                                    DObjFac<OCbase,  Fdflt> >,

    // Also, verify null pointer literal can be used for the
    // factory argument in each case.
    // DESIGN NOTE - NULL POINTER LITERALS CAN BE USED ONLY WITH
    //               DELETERS THAT TYPE-ERASE THE FACTORY.
    //&doLoadObjectFnullDeleter<const void, Obase,
    //                              DObjFac<Obase,   Fdflt> >,
    //&doLoadObjectFnullDeleter<const void, Obase,
    //                              DObjFac<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<const void, OCbase,
    //                              DObjFac<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<const void, Oderiv,
    //                              DObjFac<Oderiv,  Fdflt> >,
    //&doLoadObjectFnullDeleter<const void, Oderiv,
    //                              DObjFac<Obase,   Fdflt> >,

    //&doLoadObjectFnullDeleter<const void, Oderiv,
    //                              DObjFac<OCderiv, Fdflt> >,
    //&doLoadObjectFnullDeleter<const void, Oderiv,
    //                              DObjFac<OCbase,  Fdflt> >,

    //&doLoadObjectFnullDeleter<const void, OCderiv,
    //                              DObjFac<OCderiv, Fdflt> >,
    //&doLoadObjectFnullDeleter<const void, OCderiv,
    //                              DObjFac<OCbase,  Fdflt> >,


    // negative tests for deleters look for a null pointer lvalue.
    // Note that null pointer literal would be a compile-fail test
    &doLoadObjectFactoryDzero<const void, Obase,   Ftst>,
    &doLoadObjectFactoryDzero<const void, Obase,   Fbsl>,
    &doLoadObjectFactoryDzero<const void, Oderiv,  Ftst>,
    &doLoadObjectFactoryDzero<const void, Oderiv,  Fbsl>,
    &doLoadObjectFactoryDzero<const void, OCbase,  Ftst>,
    &doLoadObjectFactoryDzero<const void, OCbase,  Fbsl>,
    &doLoadObjectFactoryDzero<const void, OCderiv, Ftst>,
    &doLoadObjectFactoryDzero<const void, OCderiv, Fbsl>
};
static const int TEST_CONST_VOID_ARRAY_SIZE =
                sizeof(TEST_CONST_VOID_ARRAY)/sizeof(TEST_CONST_VOID_ARRAY[0]);

} // close anonymous namespace
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
    void implicitCastingExample()
    {
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

            ASSERT(200 == numdels);
        }

        ASSERT(400 == numdels);
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
// stated, the managed object will be destroyed correctly regardless of how
// it is cast.

}  // close namespace TYPE_CASTING_TEST_NAMESPACE

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
namespace USAGE_EXAMPLE {

// What follows is a concrete example illustrating the alias concept.
// Let's say our array stores data acquired from a ticker
// plant accessible by a global 'getQuote' function:
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
        if (g_verbose) bsl::cout << "Found quote: " << *result << bsl::endl;
//..
// We also print the preceding 5 quotes in last-to-first order:
//..
        int i;
        if (g_verbose) bsl::cout << "Preceded by:";
        for (i = -1; i >= -5; --i) {
            double quote = result.ptr()[i];
            if (END_QUOTE == quote) {
                break;
            }
            ASSERT(quote < *result);
            if (g_verbose) bsl::cout << ' ' << quote;
        }
        if (g_verbose) bsl::cout << bsl::endl;
// To move the finger, e.g., to the last position printed, one must be careful
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

}  // close namespace USAGE_EXAMPLE

//=============================================================================
//                  TEST PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
testCompsite();

    int test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;
                   g_verbose = verbose;
               g_veryVerbose = veryVerbose;
       g_veryVeryVeryVerbose = veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma_TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma_Default::setGlobalAllocator(&globalAllocator);

    bslma_TestAllocator da("default", veryVeryVeryVerbose);
    bslma_Default::setDefaultAllocator(&da);

    switch (test) { case 0:
      case 18: {
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
      case 17: {
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
            bslma_TestAllocatorMonitor tam(&ta);

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

        LOOP_ASSERT(numdels, 20202 == numdels);
      } break;
      case 16: {
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

        bslma_TestAllocatorMonitor gam(&globalAllocator);
        bslma_TestAllocatorMonitor dam(&da);

        int x;
        int y;
        bdema_ManagedPtr<int> p(&x, 0,
                                &bdema_ManagedPtrNoOpDeleter::deleter);

        p.load(&y, 0, &bdema_ManagedPtrNoOpDeleter::deleter);

        ASSERT(dam.isInUseSame());
        ASSERT(gam.isInUseSame());
      } break;
      case 15: {
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

        bslma_TestAllocatorMonitor gam(&globalAllocator);
        bslma_TestAllocatorMonitor dam(&da);

        int x;
        int y;
        bdema_ManagedPtr<int> p(&x, 0,
                                &bdema_ManagedPtrNilDeleter<int>::deleter);

        p.load(&y, 0, &bdema_ManagedPtrNilDeleter<int>::deleter);

        ASSERT(dam.isInUseSame());
        ASSERT(gam.isInUseSame());
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // CLEAR and RELEASE
        //
        // Concerns:
        //: 1 'clear' destroys the managed object (if any) and re-initializes
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
            LOOP_ASSERT(numDeletes, 1 == numDeletes);

            ASSERT(!o && !o.ptr());
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

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
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

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
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

#if 0
        // testing 'deleter' accessor and 'release().second'
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
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

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
#endif

      } break;
      case 13: {
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
        //      (e.g., ptr-to-derived, to ptr-to-base, ptr to ptr-to-const)
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
        //     simple with null factory and deleter
        //     aliased
        //     aliased (original created with factory)
        //     aliased (original created with factory and deleter)
        //
        //  In addition, assignment supports the following that 'swap' does not
        //  assignment from temporary/rvalue must be supported
        //  assignment from 'compatible' managed pointer must be supported
        //    i.e., where raw pointers would be convertible under assignment
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
        LOOP_ASSERT(numDeletes, 101 == numDeletes);

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
            LOOP_ASSERT(numDeletes, 1 == numDeletes);

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
            LOOP_ASSERT(numDeletes, 1 == numDeletes);

            ASSERT(o.ptr() == p2);
        }
        ASSERT(101 == numDeletes);

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
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

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

            LOOP_ASSERT(numDeletes, 1 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

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
        LOOP_ASSERT(numDeletes, 100 == numDeletes);

//#define BDEMA_MANAGEDPTR_COMPILE_FAIL_ASSIGN_FROM_INCOMPATIBLE_TYPE
#if defined(BDEMA_MANAGEDPTR_COMPILE_FAIL_ASSIGN_FROM_INCOMPATIBLE_TYPE)
            {
                bdema_ManagedPtr<int> x;
                bdema_ManagedPtr<double> y;
                y = x;  // This should fail to compile.
            }
#endif
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // ALIAS SUPPORT TEST
        //
        // Concerns:
        //   managed pointer can hold an alias
        //   'ptr' returns the alias pointer, and not the managed pointer
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

        // TEST SCENARIOS for 'loadAlias'
        //   Alias an existing state:
        //     Run through the function table for test case 'load'
        //     Test 1:
        //       Load a known state into an empty managed pointer
        //       call 'loadAlias' on a second empty managed pointer
        //       Check aliased state, and original managed pointer
        //         negative test alias with a null pointer value
        //         negative test if aliased managed pointer is empty
        //       Check no memory allocated by aliasing
        //       Run destructor and validate
        //     Test 2:
        //       Load a known state into an empty managed pointer
        //       call 'loadAlias' on a second empty managed pointer
        //       Check aliased state, and original managed pointer
        //       call 'loadAlias' again on a third empty managed pointer
        //       Check new aliased state, and first aliased managed pointer
        //       Check no memory allocated by aliasing
        //       Run destructor and validate
        //     Test 3: (to be written)
        //       Create an alias
        //       Check aliased state, and original managed pointer
        //       run another 'load' function, and check alias destroys correctly
        //       destroy 'load'ed managed pointer, validating results
        // --------------------------------------------------------------------

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\nTesting 'loadAlias' overloads"
                          << "\n-----------------------------" << endl;

        {
            if (veryVerbose)
                 cout << "Testing bdema_ManagedPtr<MyTestObject>::loadAlias\n";

            testLoadAliasOps1<MyTestObject>(L_, TEST_BASE_ARRAY);
            testLoadAliasOps2<MyTestObject>(L_, TEST_BASE_ARRAY);
            testLoadAliasOps3<MyTestObject>(L_, TEST_BASE_ARRAY);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        {
            if (veryVerbose) cout <<
                   "Testing bdema_ManagedPtr<const MyTestObject>::loadAlias\n";

            testLoadAliasOps1<const MyTestObject>(L_, TEST_CONST_BASE_ARRAY);
            testLoadAliasOps2<const MyTestObject>(L_, TEST_CONST_BASE_ARRAY);
            testLoadAliasOps3<const MyTestObject>(L_, TEST_CONST_BASE_ARRAY);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        {
            //if (veryVerbose) cout <<
            //          "Testing bdema_ManagedPtr<MyDerivedObject>::loadAlias\n";

            //testLoadAliasOps1<MyDerivedObject>(L_, TEST_DERIVED_ARRAY);
            //testLoadAliasOps2<MyDerivedObject>(L_, TEST_DERIVED_ARRAY);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        {
            if (veryVerbose)
                         cout << "Testing bdema_ManagedPtr<void>::loadAlias\n";

            testLoadAliasOps1<void>(L_, TEST_VOID_ARRAY);
            testLoadAliasOps2<void>(L_, TEST_VOID_ARRAY);
            testLoadAliasOps3<void>(L_, TEST_VOID_ARRAY);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        {
            if (veryVerbose)
                   cout << "Testing bdema_ManagedPtr<const void>::loadAlias\n";

            testLoadAliasOps1<const void>(L_, TEST_CONST_VOID_ARRAY);
            testLoadAliasOps2<const void>(L_, TEST_CONST_VOID_ARRAY);
            testLoadAliasOps3<const void>(L_, TEST_CONST_VOID_ARRAY);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

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
        LOOP_ASSERT(numDeletes, 1 == numDeletes);


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
      case 11: {
        // --------------------------------------------------------------------
        // MOVE-CONSTRUCTION
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
        //:   type, i.e., where a pointer to the specified '_Ref' type may be
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
            bslma_TestAllocatorMonitor gam(&globalAllocator);
            bslma_TestAllocatorMonitor dam(&da);

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
            LOOP_ASSERT(numDeletes, 1 == numDeletes);
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
                                     bdema_ManagedPtr_Ref<TObj> r)
                    {
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
            LOOP_ASSERT(numDeletes, 1 == numDeletes);
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
            bslma_TestAllocatorMonitor gam(&globalAllocator);
            bslma_TestAllocatorMonitor dam(&da);

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
            LOOP_ASSERT(numDeletes, 1 == numDeletes);
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
            LOOP_ASSERT(numDeletes, 1 == numDeletes);
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
        LOOP_ASSERT(numDeletes, 100 == numDeletes);

        numDeletes = 0;
        {
            TDObj *p = new (da) MyDerivedObject(&numDeletes);
            DObj d(p);
            ASSERT(d.ptr() == p);

            Obj o(d);
            ASSERT(o.ptr() == p);
            ASSERT(0 == d.ptr());
        }
        LOOP_ASSERT(numDeletes, 100 == numDeletes);

// examples to demonstrate:
        // Moving from lvalues:
        //   derived->base
        //   no-cv -> const
        //   anything -> void
        //
        // Moving from rvalues:
        //   as above, plus...
        //   rvalue of same type

//#define BDEMA_MANAGEDPTR_COMPILE_FAIL_INCOMPATIBLE_POINTERS
#if defined BDEMA_MANAGEDPTR_COMPILE_FAIL_INCOMPATIBLE_POINTERS
        {
            int x;
            bdema_ManagedPtr<int> i_ptr(&x, 0, &countedNilDelete);
            bdema_ManagedPtr<double> d_ptr(i_ptr);

            struct local_factory {
                static bdema_ManagedPtr<double> exec()
                {
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
      case 10: {
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
        //   declared in the ManagedPtr class and exercise all of them.
        //
        //   Remember to pass '0' as a null-pointer literal to all arguments
        //   that accept pointers (with negative testing if that is out of
        //   contract).
        //
        // Tested:
        //   bdema_ManagedPtr(TARGET_TYPE *ptr)
        //   bdema_ManagedPtr(TARGET_TYPE *ptr, FACTORY *factory)
        //   bdema_ManagedPtr(TARGET_TYPE *, void *, DeleterFunc)
        //   bdema_ManagedPtr(TARGET_TYPE *,
        //                      nullptr_t *,
        //                           void(*)(TARGET_BASE *, void *))
        //   bdema_ManagedPtr(TARGET_TYPE *,
        //                        FACTORY *,
        //                           void(*)(TARGET_BASE *, FACTORY_BASE *))
        // --------------------------------------------------------------------

        using namespace CREATORS_TEST_NAMESPACE;

        bslma_TestAllocator ta("object", veryVeryVeryVerbose);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tTest the single owned-pointer constructor\n";

        int numDeletes = 0;
        {
            if (veryVerbose) cout << "\t\tBasic test object\n";

            bslma_TestAllocatorMonitor dam(&da);
            {
                TObj *p = new (da) MyTestObject(&numDeletes);

                bslma_TestAllocatorMonitor dam2(&da);
                Obj o(p);

                ASSERT(o.ptr() == p);
                ASSERT(dam2.isInUseSame());
            }
            ASSERT(dam.isTotalUp());
            ASSERT(dam.isInUseSame());
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);


        if (verbose) cout << "\tTest derived-to-base pointer in constructor\n";

        numDeletes = 0;
        {
            bslma_TestAllocatorMonitor dam(&da);
            {
                TObj *p = new (da) MyDerivedObject(&numDeletes);

                bslma_TestAllocatorMonitor dam2(&da);
                Obj o(p);

                ASSERT(o.ptr() == p);
                ASSERT(dynamic_cast<MyDerivedObject *>(o.ptr()) == p);
                ASSERT(dam2.isInUseSame());
            }
            LOOP_ASSERT(numDeletes, 100 == numDeletes);
            ASSERT(dam.isTotalUp());
            ASSERT(dam.isInUseSame());
        }


        if (verbose) cout << "\tTest valid pointer passed to void*\n";

        numDeletes = 0;
        {
            bslma_TestAllocatorMonitor dam(&da);
            {
                TObj *p = new (da) MyDerivedObject(&numDeletes);

                bslma_TestAllocatorMonitor dam2(&da);
                VObj o(p);

                ASSERT(o.ptr() == p);
                ASSERT(dam2.isInUseSame());
            }
            LOOP_ASSERT(numDeletes, 100 == numDeletes);
            ASSERT(dam.isTotalUp());
            ASSERT(dam.isInUseSame());
        }

        numDeletes = 0;
        {
            if (veryVerbose) cout << "\t\tconst-qualified int\n";

            bslma_TestAllocatorMonitor dam(&da);
            {
                const int *p = new (da) const int(0);

                bslma_TestAllocatorMonitor dam2(&da);
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

            bslma_TestAllocatorMonitor dam(&da);
            {
                int *p = new (da) int;

                bslma_TestAllocatorMonitor dam2(&da);
                bdema_ManagedPtr<const int> o(p);

                ASSERT(o.ptr() == p);
                ASSERT(dam2.isInUseSame());
            }
            ASSERT(dam.isTotalUp());
            ASSERT(dam.isInUseSame());
        }
        ASSERT(0 == numDeletes);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tTest "
                             "bdema_ManagedPtr(TYPE *ptr, FACTORY *factory)\n";

        numDeletes = 0;
        {
            if (veryVerbose) cout << "Store a basic pointer\n";

            bslma_TestAllocatorMonitor gam(&globalAllocator);
            bslma_TestAllocatorMonitor dam(&da);

            {
                bslma_TestAllocatorMonitor tam(&ta);

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
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        numDeletes = 0;
        {
            if (veryVerbose) cout << "Store a derived pointer\n";

            bslma_TestAllocatorMonitor gam(&globalAllocator);
            bslma_TestAllocatorMonitor dam(&da);
            bslma_TestAllocatorMonitor tam(&ta);

            {

                TDObj *p = new (ta) MyDerivedObject(&numDeletes);
                Obj o(p, &ta);

                TObj *q = o.ptr();
                LOOP2_ASSERT(p, q, p == q);
            }
            LOOP_ASSERT(numDeletes, 100 == numDeletes);
            ASSERT(tam.isInUseSame());
            ASSERT(dam.isInUseSame());
            ASSERT(dam.isMaxSame());
            ASSERT(gam.isInUseSame());
            ASSERT(gam.isMaxSame());
        }

        numDeletes = 0;
        {
            if (veryVerbose) cout << "Store in a bdema_ManagedPtr<void>\n";

            bslma_TestAllocatorMonitor gam(&globalAllocator);
            bslma_TestAllocatorMonitor dam(&da);
            bslma_TestAllocatorMonitor tam(&ta);

            {
                TObj *p = new (ta) MyDerivedObject(&numDeletes);
                VObj o(p, &ta);

                void *q = o.ptr();
                LOOP2_ASSERT(p, q, p == q);
            }
            LOOP_ASSERT(numDeletes, 100 == numDeletes);
            ASSERT(tam.isInUseSame());
            ASSERT(dam.isInUseSame());
            ASSERT(dam.isMaxSame());
            ASSERT(gam.isInUseSame());
            ASSERT(gam.isMaxSame());
        }

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) cout << "\tNegative testing\n";

        {
            bsls_AssertTestHandlerGuard guard;

            bslma_Allocator * pNullAlloc = 0;
            TObj *p = new (ta) MyTestObject(&numDeletes);
            ASSERT_SAFE_FAIL_RAW(Obj x(p, pNullAlloc));
            ASSERT_SAFE_PASS_RAW(Obj y(p, &ta));
        }
#else
        if (verbose) cout << "\tNegative testing disabled due to lack of "
                             "exception support\n";
#endif
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout <<
                "\tTest bdema_ManagedPtr(BDEMA_TYPE *, void *, DeleterFunc)\n";

        numDeletes = 0;
        {
            bslma_TestAllocatorMonitor gam(&globalAllocator);
            bslma_TestAllocatorMonitor dam(&da);

            {
                Obj::DeleterFunc deleter = &bdema_ManagedPtr_FactoryDeleter<
                                   MyTestObject, bslma_TestAllocator>::deleter;

                bslma_TestAllocatorMonitor tam(&ta);

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
                bslma_TestAllocatorMonitor tam(&ta);

                bsls_AssertTestHandlerGuard guard;

                Obj::DeleterFunc deleter = 0;
                TObj *p = new (ta) MyTestObject(&numDeletes);

                ASSERT_SAFE_FAIL_RAW(Obj o(p, &ta, deleter));
//              ASSERT_SAFE_FAIL_RAW(Obj o(p, &ta, 0));  // Now a compile fail

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
            bslma_TestAllocatorMonitor gam(&globalAllocator);
            bslma_TestAllocatorMonitor dam(&da);

            bslma_TestAllocatorMonitor tam(&ta);
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
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tTest bdema_ManagedPtr(BDEMA_TYPE *ptr,"
                             " bdema_ManagedPtr_Nullptr::Type,"
                             " void(*)(BDEMA_TYPE *, void*));\n";

        numDeletes = 0;
        LOOP_ASSERT(g_deleteCount, 0 == g_deleteCount);
        {
            bslma_TestAllocatorMonitor tam(&ta);

            MyTestObject obj(&numDeletes);
            Obj o(&obj, 0, &templateNilDelete<MyTestObject>);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);
        LOOP_ASSERT(g_deleteCount, 1 == g_deleteCount);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tTest bdema_ManagedPtr(TARGET_TYPE*, FACTORY*,"
                             " void (*)(TARGET_BASE*, FACTORY_BASE*)\n";

        numDeletes = 0;
        {
            bslma_TestAllocatorMonitor gam(&globalAllocator);
            bslma_TestAllocatorMonitor dam(&da);
            bslma_TestAllocatorMonitor tam(&ta);

            {

                TObj *p = new (ta) MyTestObject(&numDeletes);
                Obj o(p, &ta, &AllocatorDeleter<TObj>::deleter);

                TObj *q = o.ptr();
                LOOP2_ASSERT(p, q, p == q);
            }
            LOOP_ASSERT(numDeletes, 1 == numDeletes);
            ASSERT(tam.isInUseSame());
            ASSERT(dam.isInUseSame());
            ASSERT(dam.isMaxSame());
            ASSERT(gam.isInUseSame());
            ASSERT(gam.isMaxSame());
        }

        numDeletes = 0;
        {
            if (veryVerbose) cout <<
                       "\t\tload derived type into a simple managed pointer\n";

            typedef void (*DeleterFunc)(MyTestObject *, void *);
            DeleterFunc deleterFunc = (DeleterFunc) &myTestDeleter;

            bslma_TestAllocatorMonitor gam(&globalAllocator);
            bslma_TestAllocatorMonitor dam(&da);
            bslma_TestAllocatorMonitor tam(&ta);

            {

                TDObj *p = new (ta) MyDerivedObject(&numDeletes);
                Obj o;

                // Test must pass without this workaround, for compatibility
                // We might want to test the workaround separately, but believe
                // that is not necessary.
                o.load(p, &ta, deleterFunc);
            }
            LOOP_ASSERT(numDeletes, 100 == numDeletes);
            ASSERT(tam.isInUseSame());
            ASSERT(dam.isInUseSame());
            ASSERT(dam.isMaxSame());
            ASSERT(gam.isInUseSame());
            ASSERT(gam.isMaxSame());
        }

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) cout << "\tNegative testing\n";

        {
            bsls_AssertTestHandlerGuard guard;

            bslma_Allocator * pNullAlloc = 0;
            TObj *p = new (ta) MyTestObject(&numDeletes);
            ASSERT_SAFE_FAIL_RAW(Obj x(p, pNullAlloc));
            ASSERT_SAFE_PASS_RAW(Obj y(p, &ta));

        }
#else
        if (verbose) cout << "\tNegative testing disabled due to lack of "
                             "exception support\n";
#endif

//#define BDEMA_MANAGEDPTR_COMPILE_FAIL_CONSTRUCT_FROM_INCOMPATIBLE_POINTER
#if defined BDEMA_MANAGEDPTR_COMPILE_FAIL_CONSTRUCT_FROM_INCOMPATIBLE_POINTER
        // This segment of the test case examines the quality of compiler
        // diagnostics when trying to create a 'bdema_ManagedPtr' object with a
        // pointer that it not convertible to a pointer of the type that the
        // smart pointer is managing.
        if (verbose) cout << "\tTesting compiler diagnostics*\n";

        // distint, unrelated types
        numDeletes = 0;
        {
            double *p = new (da) double;
            Obj o(p);

//            ASSERT(o.ptr() == p);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        // const-conversion
        numDeletes = 0;
        {
            const MyTestObject *p = new (da) MyTestObject(&numDeletes);
            Obj o(p);

//            ASSERT(o.ptr() == p);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        numDeletes = 0;
        {
            const MyTestObject *p = new (da) MyTestObject(&numDeletes);
            VObj o(p);

            ASSERT(o.ptr() == p);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);
#endif

//#define BDEMA_MANAGEDPTR_COMPILE_FAIL_TEST_NULL_FACTORY
#if defined(BDEMA_MANAGEDPTR_COMPILE_FAIL_TEST_NULL_FACTORY)
        {
            int i = 0;
            bdema_ManagedPtr<int> x(&i, 0);
            bdema_ManagedPtr<int> y( 0, 0);

            bslma_Allocator * pNullAlloc = 0;
            bdema_ManagedPtr<int> z(0, pNullAlloc);
        }
#endif

//#define BDEMA_MANAGEDPTR_COMPILE_FAIL_TEST_NULL_FACTORY
#if defined(BDEMA_MANAGEDPTR_COMPILE_FAIL_TEST_NULL_FACTORY)
        {
            int *i = 0;
            bdema_ManagedPtr<const int> x(&i, 0);
            bdema_ManagedPtr<int> y( 0, 0);  // allow this?

            bslma_Allocator * pNullAlloc = 0;
            bdema_ManagedPtr<const int> z(0, pNullAlloc);  // allow this?
        }
#endif

//#define BDEMA_MANAGEDPTR_COMPILE_FAIL_TEST_NULL_DELETER
#if defined(BDEMA_MANAGEDPTR_COMPILE_FAIL_TEST_NULL_DELETER)
        {
            int *i = 0;
            bdema_ManagedPtr<const int> x(i, 0, 0);
            bdema_ManagedPtr<const int> y(0, 0, 0);  // allow this?

            // These are currently runtime (UB) failures, rather than
            // compile-time errors.
            bslma_Allocator * pNullAlloc = 0;
            bdema_ManagedPtr<const int> z( i, pNullAlloc, 0);
            bdema_ManagedPtr<const int> zz(0, pNullAlloc, 0);  // allow this?
        }
#endif
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ACCESSORS
        //
        // Concerns:
        //   That all accessors work properly.  The 'ptr' accessor has
        //   already been substantially tested in previous tests.
        //   The unspecified bool conversion evaluates as expected in all
        //     circumstances: if/while/for, (implied) operator!
        //   All accessors work on 'const'- qualified objects
        //   All accessors can be called for 'bdema_ManagedPtr<void>'
        //   All accessors return expected values when a 'bdema_ManagedPtr' has
        //     been aliased
        //   'operator*' should assert in SAFE builds for empty pointers
        //   'deleter' should assert in SAFE builds for empty pointers
        //   'operator*' should be well-formed, but not callable for
        //     'bdema_ManagedPtr<void>'.
        //
        //: X No 'bdema_ManagedPtr' method should allocate any memory.
        //
        // Plan:
        //   Test each accessor for the expected value on each of the following
        //   cases:
        //     empty
        //     simple
        //     simple with factory
        //     simple with factory and deleter
        //     simple with null factory and deleter
        //     aliased
        //     aliased (original created with factory)
        //     aliased (original created with factory and deleter)
        //
        //  For 'bdema_ManagedPtr<void>', test syntax of 'operator*' in an
        //    unevaluated context, such as a 'typeid' expression.
        //
        //  Test that illegal expressions cannot compile in compile-fail tests,
        //  guarded by #ifdefs, where necessary.
        //
        // Tested:
        //   operator BoolType() const;
        //   TYPE& operator*() const;
        //   TYPE *operator->() const;
        //   TYPE *ptr() const;
        //   const bdema_ManagedPtrDeleter& deleter() const;
        // --------------------------------------------------------------------

        using namespace CREATORS_TEST_NAMESPACE;

        typedef bdema_ManagedPtr_FactoryDeleter<MyTestObject, bslma_Allocator>
                                                                DefaultDeleter;

        const bdema_ManagedPtr<MyTestObject>::DeleterFunc defaultDelete =
                                                      &DefaultDeleter::deleter;

        bslma_TestAllocator ta("object", veryVeryVeryVerbose);

        if (verbose) cout << "\tTest accessors on empty object\n";

        int numDeletes = 0;
        {
            const Obj o;
            const bdema_ManagedPtrDeleter del;

            validateManagedState(o, 0, del);
        }

        LOOP_ASSERT(numDeletes, 0 == numDeletes);
        {
            const VObj o;
            const bdema_ManagedPtrDeleter del;

            validateManagedState(o, 0, del);
            typeid(*o); // should parse, even if it cannot be called
        }

        LOOP_ASSERT(numDeletes, 0 == numDeletes);
        {
            const bdema_ManagedPtr<const void> o(0);
            const bdema_ManagedPtrDeleter del;

            validateManagedState(o, 0, del);
            typeid(*o); // should parse, even if it cannot be called
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tTest accessors on simple object\n";

        LOOP_ASSERT(numDeletes, 0 == numDeletes);
        {
            Obj o;
            TObj *p = new (da) MyTestObject(&numDeletes);
            o.load(p);
            const bdema_ManagedPtrDeleter del(p, &da,
              &bdema_ManagedPtr_FactoryDeleter<TObj,bslma_Allocator>::deleter);

            validateManagedState(o, p, del);

            Obj oD;
            {
                MyDerivedObject d(&numDeletes);
                oD.loadAlias(o, &d);
                validateManagedState(oD, &d, del);
            }
            LOOP_ASSERT(numDeletes, 100 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 101 == numDeletes);

        numDeletes = 0;
        {
            VObj o;
            TObj *p = new (da) MyTestObject(&numDeletes);
            o.load(p);
            const bdema_ManagedPtrDeleter del(p, &da,
              &bdema_ManagedPtr_FactoryDeleter<TObj,bslma_Allocator>::deleter);

            validateManagedState(o, p, del);

            VObj oD;
            {
                MyDerivedObject d(&numDeletes);
                oD.loadAlias(o, &d);
                validateManagedState(oD, &d, del);
            }
            LOOP_ASSERT(numDeletes, 100 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 101 == numDeletes);

        numDeletes = 0;
        {
            bdema_ManagedPtr<const void> o;
            TObj *p = new (da) MyTestObject(&numDeletes);
            o.load(p);
            const bdema_ManagedPtrDeleter del(p, &da,
              &bdema_ManagedPtr_FactoryDeleter<TObj,bslma_Allocator>::deleter);

            validateManagedState(o, p, del);

            bdema_ManagedPtr<const void> oD;
            {
                MyDerivedObject d(&numDeletes);
                oD.loadAlias(o, &d);
                validateManagedState(oD, &d, del);
            }
            LOOP_ASSERT(numDeletes, 100 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 101 == numDeletes);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout <<
                         "\tTest accessors on simple object using a factory\n";

        bslma_TestAllocatorMonitor tam(&ta);
        numDeletes = 0;
        {
            Obj o;
            TObj *p = new (ta) MyTestObject(&numDeletes);
            o.load(p, &ta);
            const bdema_ManagedPtrDeleter del(p, &ta,
              &bdema_ManagedPtr_FactoryDeleter<TObj,bslma_Allocator>::deleter);

            bslma_TestAllocatorMonitor tam2(&ta);

            validateManagedState(o, p, del);
            ASSERT(tam2.isInUseSame());
            ASSERT(tam2.isMaxSame());

            Obj oD;
            {
                MyDerivedObject d(&numDeletes);
                oD.loadAlias(o, &d);
                validateManagedState(oD, &d, del);
            }
            LOOP_ASSERT(numDeletes, 100 == numDeletes);
            ASSERT(tam2.isInUseSame());
            ASSERT(tam2.isMaxSame());
        }
        LOOP_ASSERT(numDeletes, 101 == numDeletes);
        ASSERT(tam.isInUseSame());

        numDeletes = 0;
        {
            VObj o;
            TObj *p = new (ta) MyTestObject(&numDeletes);
            o.load(p, &ta);
            const bdema_ManagedPtrDeleter del(p, &ta,
              &bdema_ManagedPtr_FactoryDeleter<TObj,bslma_Allocator>::deleter);

            bslma_TestAllocatorMonitor tam2(&ta);

            validateManagedState(o, p, del);
            ASSERT(tam2.isInUseSame());
            ASSERT(tam2.isMaxSame());

            VObj oD;
            {
                MyDerivedObject d(&numDeletes);
                oD.loadAlias(o, &d);
                validateManagedState(oD, &d, del);
            }
            LOOP_ASSERT(numDeletes, 100 == numDeletes);
            ASSERT(tam2.isInUseSame());
            ASSERT(tam2.isMaxSame());
        }
        LOOP_ASSERT(numDeletes, 101 == numDeletes);
        ASSERT(tam.isInUseSame());

        numDeletes = 0;
        {
            bdema_ManagedPtr<const void> o;
            TObj *p = new (ta) MyTestObject(&numDeletes);
            o.load(p, &ta);
            const bdema_ManagedPtrDeleter del(p, &ta,
              &bdema_ManagedPtr_FactoryDeleter<TObj,bslma_Allocator>::deleter);

            bslma_TestAllocatorMonitor tam2(&ta);

            validateManagedState(o, p, del);
            ASSERT(tam2.isInUseSame());
            ASSERT(tam2.isMaxSame());

            bdema_ManagedPtr<const void> oD;
            {
                MyDerivedObject d(&numDeletes);
                oD.loadAlias(o, &d);
                validateManagedState(oD, &d, del);
            }
            LOOP_ASSERT(numDeletes, 100 == numDeletes);
            ASSERT(tam2.isInUseSame());
            ASSERT(tam2.isMaxSame());
        }
        LOOP_ASSERT(numDeletes, 101 == numDeletes);
        ASSERT(tam.isInUseSame());

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout <<
          "\tTest accessors on simple object using a deleter but no factory\n";

        g_deleteCount = 0;
        numDeletes = 0;
        {
            Obj o;
            TObj obj(&numDeletes);
            o.load(&obj, 0, &countedNilDelete);
            const bdema_ManagedPtrDeleter del(&obj, 0, &countedNilDelete);

            validateManagedState(o, &obj, del);

            Obj oD;
            {
                MyDerivedObject d(&numDeletes);
                oD.loadAlias(o, &d);
                validateManagedState(oD, &d, del);
            }
            LOOP_ASSERT(numDeletes, 100 == numDeletes);
            LOOP_ASSERT(g_deleteCount, 0 == g_deleteCount);
        }
        LOOP_ASSERT(numDeletes, 101 == numDeletes);
        LOOP_ASSERT(g_deleteCount, 1 == g_deleteCount);

        g_deleteCount = 0;
        numDeletes = 0;
        {
            VObj o;
            TObj obj(&numDeletes);
            o.load(&obj, 0, &countedNilDelete);
            const bdema_ManagedPtrDeleter del(&obj, 0, &countedNilDelete);

            validateManagedState(o, &obj, del);

            VObj oD;
            {
                MyDerivedObject d(&numDeletes);
                oD.loadAlias(o, &d);
                validateManagedState(oD, &d, del);
            }
            LOOP_ASSERT(numDeletes, 100 == numDeletes);
            LOOP_ASSERT(g_deleteCount, 0 == g_deleteCount);
        }
        LOOP_ASSERT(numDeletes, 101 == numDeletes);
        LOOP_ASSERT(g_deleteCount, 1 == g_deleteCount);

        g_deleteCount = 0;
        numDeletes = 0;
        {
            bdema_ManagedPtr<const void> o;
            TObj obj(&numDeletes);
            o.load(&obj, 0, &countedNilDelete);
            const bdema_ManagedPtrDeleter del(&obj, 0, &countedNilDelete);

            validateManagedState(o, &obj, del);

            bdema_ManagedPtr<const void> oD;
            {
                MyDerivedObject d(&numDeletes);
                oD.loadAlias(o, &d);
                validateManagedState(oD, &d, del);
            }
            LOOP_ASSERT(numDeletes, 100 == numDeletes);
            LOOP_ASSERT(g_deleteCount, 0 == g_deleteCount);
        }
        LOOP_ASSERT(numDeletes, 101 == numDeletes);
        LOOP_ASSERT(g_deleteCount, 1 == g_deleteCount);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout <<
        "\tTest accessors on simple object using both a factory and deleter\n";

        numDeletes = 0;
        {
            bdema_ManagedPtr<int> o;
            IncrementIntFactory factory;
            o.load(&numDeletes, &factory, &incrementIntDeleter);
            const bdema_ManagedPtrDeleter del(&numDeletes,
                                              &factory,
            (bdema_ManagedPtrDeleter::Deleter)&incrementIntDeleter);

            validateManagedState(o, &numDeletes, del);

            bdema_ManagedPtr<int> o2;
            int i2 = 0;
            {
                o2.loadAlias(o, &i2);
                validateManagedState(o2, &i2, del);
            }
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        numDeletes = 0;
        {
            VObj o;
            IncrementIntFactory factory;
            o.load(&numDeletes, &factory, &incrementIntDeleter);
            const bdema_ManagedPtrDeleter del(&numDeletes,
                                              &factory,
            (bdema_ManagedPtrDeleter::Deleter)&incrementIntDeleter);

            validateManagedState(o, &numDeletes, del);

            bdema_ManagedPtr<int> o2;
            int i2 = 0;
            {
                o2.loadAlias(o, &i2);
                validateManagedState(o2, &i2, del);
            }
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        numDeletes = 0;
        {
            bdema_ManagedPtr<const void> o;
            IncrementIntFactory factory;
            o.load(&numDeletes, &factory, &incrementIntDeleter);
            const bdema_ManagedPtrDeleter del(&numDeletes,
                                              &factory,
            (bdema_ManagedPtrDeleter::Deleter)&incrementIntDeleter);

            validateManagedState(o, &numDeletes, del);

            bdema_ManagedPtr<int> o2;
            int i2 = 0;
            {
                o2.loadAlias(o, &i2);
                validateManagedState(o2, &i2, del);
            }
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 1 == numDeletes);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//#define BDEMA_MANAGEDPTR_COMPILE_FAIL_DEREFERENCE_VOID_PTR
#if defined(BDEMA_MANAGEDPTR_COMPILE_FAIL_DEREFERENCE_VOID_PTR)
            {
                int x;
                VObj p(&x);
                *p;

                bdema_ManagedPtr<const void> p2(&x);
                *p2;
            }
#endif
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING loadAlias
        //
        // Concerns:
        //   managed pointer can hold an alias
        //   'ptr' returns the alias pointer, and not the managed pointer
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
        //   void loadAlias(bdema_ManagedPtr<OTHER> &alias, TYPE *ptr)

        // TEST SCENARIOS for 'loadAlias'
        //   Alias an existing state:
        //     Run through the function table for test case 'load'
        //     Test 1:
        //       Load a known state into an empty managed pointer
        //       call 'loadAlias' on a second empty managed pointer
        //       Check aliased state, and original managed pointer
        //         negative test alias with a null pointer value
        //         negative test if aliased managed pointer is empty
        //       Check no memory allocated by aliasing
        //       Run destructor and validate
        //     Test 2:
        //       Load a known state into an empty managed pointer
        //       call 'loadAlias' on a second empty managed pointer
        //       Check aliased state, and original managed pointer
        //       call 'loadAlias' again on a third empty managed pointer
        //       Check new aliased state, and first aliased managed pointer
        //       Check no memory allocated by aliasing
        //       Run destructor and validate
        //     Test 3: (to be written)
        //       Create an alias
        //       Check aliased state, and original managed pointer
        //       run another 'load' function, and check alias destroys correctly
        //       destroy 'load'ed managed pointer, validating results
        // --------------------------------------------------------------------

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\nTesting 'loadAlias' overloads"
                          << "\n-----------------------------" << endl;

        {
            if (veryVerbose)
                 cout << "Testing bdema_ManagedPtr<MyTestObject>::loadAlias\n";

            testLoadAliasOps1<MyTestObject>(L_, TEST_BASE_ARRAY);
            testLoadAliasOps2<MyTestObject>(L_, TEST_BASE_ARRAY);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        {
            if (veryVerbose) cout <<
                   "Testing bdema_ManagedPtr<const MyTestObject>::loadAlias\n";

            testLoadAliasOps1<const MyTestObject>(L_, TEST_CONST_BASE_ARRAY);
            testLoadAliasOps2<const MyTestObject>(L_, TEST_CONST_BASE_ARRAY);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        {
            //if (veryVerbose) cout <<
            //          "Testing bdema_ManagedPtr<MyDerivedObject>::loadAlias\n";

            //testLoadAliasOps1<MyDerivedObject>(L_, TEST_DERIVED_ARRAY);
            //testLoadAliasOps2<MyDerivedObject>(L_, TEST_DERIVED_ARRAY);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        {
            if (veryVerbose)
                         cout << "Testing bdema_ManagedPtr<void>::loadAlias\n";

            testLoadAliasOps1<void>(L_, TEST_VOID_ARRAY);
            testLoadAliasOps2<void>(L_, TEST_VOID_ARRAY);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        {
            if (veryVerbose)
                   cout << "Testing bdema_ManagedPtr<const void>::loadAlias\n";

            testLoadAliasOps1<const void>(L_, TEST_CONST_VOID_ARRAY);
            testLoadAliasOps2<const void>(L_, TEST_CONST_VOID_ARRAY);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        using namespace CREATORS_TEST_NAMESPACE;

        bsls_Types::Int64 numDeallocation = da.numDeallocation();
        int numDeletes = 0;
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
        //      confirm new pointer value is stored by 'ptr'
        //      confirm destructor destroys target object
        //      be sure to pass both '0' and valid pointer values to each potential overload
        //   Write a pair of nested loops
        //     For each iteration, first create a default-constructed bdema_ManagedPtr
        //     Then call a load function (testing each overload by the first loop)
        //     Then, in inner loop, load a second pointer and verify first target is destroyed
        //     Then verify the new target is destroyed when test object goes out of scope.
        //
        //   Test a number of scenarios in a consistent way.
        //   The 5 scenarios are:  (TestTarget)
        //      MyTestObject
        //      const MyTestObject
        //      MyDerivedObject
        //      void
        //      const void
        //
        //   For each, create a table of test-functions that use encoded names:
        //      Object  - function supplies an object to 'load'
        //      Factory - function supplies a factory to 'load'
        //      Fnull   - function passes a null pointer literal as factory
        //      Deleter - function supplies a deleter to 'load'
        //      Dzero   - function calls 'load' with a 0-value pointer variable
        //                and *not* a null-pointer literal.
        //
        //   Codes that may be passed as 'Object' policies are:
        //      Obase
        //      OCbase
        //      Oderiv
        //      OCDeriv
        //
        //   Codes for specific factories are:
        //      Fbsl   factory is cast to base 'bslma_Allocator'
        //      Ftst   factory is cast to specific 'bslma_TestAllocator' type
        //      Fdflt  factory argument is ignored and default allocator used
        //
        //   Each test-function taking an 'Object' parameter will call 'load'
        //   with both an allocate object, and a pointer variable holding a
        //   null-pointer value.
        //
        //   Each test-function taking a 'Factory' parameter will call 'load'
        //   with both an allocate object, and a pointer variable holding a
        //   null-pointer value.
        //
        //   Each of the four combinations of valid/null pointer for the
        //   factory/object arguments will be tested for test-functions taking
        //   both parameters.  The combination will be negatively tested using
        //   the 'bsls_asserttest' facility if that is appropriate.
        //
        //   These functions are assembled using further policy-functions that
        //   will create and supply objects, deleters and factories of types
        //   that are specified as template type parameters.  This allows us to
        //   compose test cases with the full set of conversion scenarios that
        //   may be needed.
        //
        //   The test function will take a default-constructed managed pointer
        //   object, call the 'load' test function, establish that the expected
        //   results of 'load' are evident, and then let the managed pointer
        //   fall out of scope, and check that the destructor destroys any
        //   managed object appropriately.
        //
        //   Then an inner-loop will again create an empty managed pointer
        //   object and 'load' it using the test function.  It will then call
        //   'load' again with the next function in the test table, and verify
        //   that the original managed object (if any) is destroyed correctly
        //   and that the new managed object is held as expected.  Then this
        //   managed pointer will fall out of scope, and we test again that any
        //   held managed object is destroyed correctly.
        //
        //   Currently well tested:
        //     const MyObjectType
        //     const void  [audit the disabled tests before moving on]
        //
        //   Note that base/derived are a conventional pair of polymorphic
        //   classes with virtual destructors.  This class should work equally
        //   well for base/derived classes that are not polymorphic, but we
        //   do not currently test that.
        //
        // Tested:
        //   void load(bdema_ManagedPtr_Nullptr::Type = 0,
        //             bdema_ManagedPtr_Nullptr::Type = 0,
        //             bdema_ManagedPtr_Nullptr::Type = 0)
        //   void load(BDEMA_TARGET_TYPE *ptr)
        //   void load(BDEMA_TARGET_TYPE *ptr, FACTORY *factory)
        //   void load(BDEMA_TYPE *ptr, void *factory, DeleterFunc deleter)
        //   void load(BDEMA_TARGET_TYPE *ptr,
        //             bdema_ManagedPtr_Nullptr::Type,
        //             void      (*deleter)(BDEMA_TARGET_BASE *, void*))
        //   void load(BDEMA_TARGET_TYPE *ptr,
        //             BDEMA_FACTORY *factory,
        //             void(*deleter)(BDEMA_TARGET_BASE*,BDEMA_BASE_FACTORY*))
        //   ~bdema_ManagedPtr()
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'load' overloads"
                          << "\n------------------------" << endl;

        {
            if (veryVerbose)
                      cout << "Testing bdema_ManagedPtr<MyTestObject>::load\n";

            testLoadOps<MyTestObject>(L_, TEST_BASE_ARRAY);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        {
            if (veryVerbose)
                cout << "Testing bdema_ManagedPtr<const MyTestObject>::load\n";

            testLoadOps<const MyTestObject>(L_, TEST_CONST_BASE_ARRAY);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        {
            if (veryVerbose)
                   cout << "Testing bdema_ManagedPtr<MyDerivedObject>::load\n";

            testLoadOps<MyDerivedObject>(L_, TEST_DERIVED_ARRAY);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        {
            if (veryVerbose) cout << "Testing bdema_ManagedPtr<void>::load\n";

            testLoadOps<void>(L_, TEST_VOID_ARRAY);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        {
            if (veryVerbose)
                        cout << "Testing bdema_ManagedPtr<const void>::load\n";

            testLoadOps<const void>(L_, TEST_CONST_VOID_ARRAY);
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Compile-fail tests

//#define BDEMA_MANAGEDPTR_COMPILE_FAIL_LOAD_INCOMPATIBLE_TYPE
#if defined(BDEMA_MANAGEDPTR_COMPILE_FAIL_LOAD_INCOMPATIBLE_TYPE)
        {
            int i = 0;
            bdema_ManagedPtr<double> x;
            x.load(&i);

            const double d = 0.0;
            x.load(&d);

            void *v = 0;
            x.load(v);
        }
#endif

//#define BDEMA_MANAGEDPTR_COMPILE_FAIL_TEST_LOAD_NULL_FACTORY
#if defined(BDEMA_MANAGEDPTR_COMPILE_FAIL_TEST_LOAD_NULL_FACTORY)
        {
            int i = 0;
            bdema_ManagedPtr<int> x;
            x.load(&i, 0);
            x.load( 0, 0); // We may consider allowing this

            void *v = 0;
            x.load(v, 0);
            x.load(0, v); // We may consider allowing this

            bslma_Allocator * pNullAlloc = 0;
            x.load(0, pNullAlloc); // We may consider allowing this

            MyDerivedObject * pd = 0;
            bdema_ManagedPtr<MyDerivedObject> md;
            md.load(pd, 0);
        }
#endif

//#define BDEMA_MANAGEDPTR_COMPILE_FAIL_TEST_LOAD_NULL_DELETER
#if defined(BDEMA_MANAGEDPTR_COMPILE_FAIL_TEST_LOAD_NULL_DELETER)
        {
            int *i = 0;
            bdema_ManagedPtr<int> x;
            x.load(i, 0, 0);
            x.load(0, 0, 0); // We may consider allowing this

            bslma_Allocator * pNullAlloc = 0;
            x.load(i, pNullAlloc, 0);
            x.load(0, pNullAlloc, 0);  // We may consider allowing this
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
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING PRIMARY CREATORS"
                          << "\n------------------------" << endl;

        using namespace CREATORS_TEST_NAMESPACE;

        if (verbose) cout << "\tTest default constructor\n";

        int numDeletes = 0;
        {
            if (veryVerbose) cout << "\t\tBasic test object\n";

            bslma_TestAllocatorMonitor dam(&da);
            Obj o;

            ASSERT(0 == o.ptr());
            ASSERT(dam.isTotalSame());
        }
        ASSERT(0 == numDeletes);

        numDeletes = 0;
        {
            if (veryVerbose) cout << "\t\tvoid type\n";

            bslma_TestAllocatorMonitor dam(&da);
            bdema_ManagedPtr<void> o;

            ASSERT(0 == o.ptr());
            ASSERT(dam.isTotalSame());
        }
        ASSERT(0 == numDeletes);

        numDeletes = 0;
        {
            if (veryVerbose) cout << "\t\tconst-qualified int\n";

            bslma_TestAllocatorMonitor dam(&da);
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

            bslma_TestAllocatorMonitor dam(&da);
            Obj o(0);

            ASSERT(0 == o.ptr());
            ASSERT(dam.isTotalSame());
        }
        ASSERT(0 == numDeletes);

        numDeletes = 0;
        {
            if (veryVerbose) cout << "\t\tvoid type\n";

            bslma_TestAllocatorMonitor dam(&da);
            VObj o(0);

            ASSERT(0 == o.ptr());
            ASSERT(dam.isTotalSame());
        }
        ASSERT(0 == numDeletes);

        numDeletes = 0;
        {
            if (veryVerbose) cout << "\t\tconst-qualified int\n";

            bslma_TestAllocatorMonitor dam(&da);
            bdema_ManagedPtr<const int> o(0);

            ASSERT(0 == o.ptr());
            ASSERT(dam.isTotalSame());
        }
        ASSERT(0 == numDeletes);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tTest constructing with two null pointers\n";

        numDeletes = 0;
        {
            if (veryVerbose) cout << "\t\tBasic test object\n";

            bslma_TestAllocatorMonitor dam(&da);
            Obj o(0, 0);

            ASSERT(0 == o.ptr());
            ASSERT(dam.isTotalSame());
        }
        ASSERT(0 == numDeletes);

        numDeletes = 0;
        {
            if (veryVerbose) cout << "\t\tvoid type\n";

            bslma_TestAllocatorMonitor dam(&da);
            VObj o(0, 0);

            ASSERT(0 == o.ptr());
            ASSERT(dam.isTotalSame());
        }
        ASSERT(0 == numDeletes);

        numDeletes = 0;
        {
            if (veryVerbose) cout << "\t\tconst-qualified int\n";

            bslma_TestAllocatorMonitor dam(&da);
            bdema_ManagedPtr<const int> o(0, 0);

            ASSERT(0 == o.ptr());
            ASSERT(dam.isTotalSame());
        }
        ASSERT(0 == numDeletes);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tTest constructing with three null pointers\n";

        numDeletes = 0;
        {
            if (veryVerbose) cout << "\t\tBasic test object\n";

            bslma_TestAllocatorMonitor dam(&da);
            Obj o(0, 0, 0);

            ASSERT(0 == o.ptr());
            ASSERT(dam.isTotalSame());
        }
        ASSERT(0 == numDeletes);

        numDeletes = 0;
        {
            if (veryVerbose) cout << "\t\tvoid type\n";

            bslma_TestAllocatorMonitor dam(&da);
            VObj o(0, 0, 0);

            ASSERT(0 == o.ptr());
            ASSERT(dam.isTotalSame());
        }
        ASSERT(0 == numDeletes);

        numDeletes = 0;
        {
            if (veryVerbose) cout << "\t\tconst-qualified int\n";

            bslma_TestAllocatorMonitor dam(&da);
            bdema_ManagedPtr<const int> o(0, 0, 0);

            ASSERT(0 == o.ptr());
            ASSERT(dam.isTotalSame());
        }
        ASSERT(0 == numDeletes);

      } break;
      case 5: {
      } break;
      case 4: {
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING bdema_ManagedPtr_Ref
        //
        // 'bdema_ManagedPtr_Ref' is similar to an in-core value semantic type
        // having a single pointer as its only attribute; it does not offer the
        // traditional range of value-semantic operations such as equality
        // comparison and printing.  Its test concerns and plan are closely
        // modeled after such a value-semantic type.
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
        //    bdema_ManagedPtr_Ref& operator=(const bdema_ManagedPtr_Ref&);
        //    bdema_ManagedPtr_Members *base() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING bdema_ManagedPtr_Ref"
                          << "\n----------------------------" << endl;

        bslma_TestAllocatorMonitor gam(&globalAllocator);
        bslma_TestAllocatorMonitor dam(&da);

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
            bslma_TestAllocatorMonitor tam(&ta);

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
        LOOP_ASSERT(numDeletes, 100 == numDeletes);

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
        LOOP_ASSERT(numDeletes, 100 == numDeletes);

        if (verbose)
            cout << "\tTest conversion construction from an rvalue.\n";

        numDeletes = 0;
        {
            Obj x(returnDerivedPtr(&numDeletes, &ta)); Obj const &X = x;

            ASSERT(X.ptr());
            LOOP_ASSERT(numDeletes, 0 == numDeletes);
        }
        LOOP_ASSERT(numDeletes, 100 == numDeletes);

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
        LOOP_ASSERT(numDeletes, 100 == numDeletes);

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
        LOOP_ASSERT(numDeletes, 100 == numDeletes);

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

        if (verbose) cout << "\tTest unambiguous overloads.\n";

        {
            bdema_ManagedPtr<void>      pV;
            bdema_ManagedPtr<int>       pI;
            bdema_ManagedPtr<const int> pCi;

            ASSERT(0 == OverloadTest::invoke(pV));
            ASSERT(1 == OverloadTest::invoke(pI));
            ASSERT(2 == OverloadTest::invoke(pCi));

#if 0  // compile fail test, think about giving a named macro to test
            bdema_ManagedPtr<double>    pD;
            ASSERT(0 == OverloadTest::invoke(pD));
#endif
        }
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
//#define BDEMA_MANAGEDPTR_COMPILE_FAIL_HOMOGENEOUS_COMPARISON
//#define BDEMA_MANAGEDPTR_COMPILE_FAIL_HOMOGENEOUS_ORDERING
//#define BDEMA_MANAGEDPTR_COMPILE_FAIL_HETEROGENEOUS_COMPARISON
//#define BDEMA_MANAGEDPTR_COMPILE_FAIL_HETEROGENEOUS_ORDERING

#if defined BDEMA_MANAGEDPTR_COMPILE_FAIL_HOMOGENEOUS_COMPARISON
        {
            bdema_ManagedPtr<int> x;
            bool b;

            // The following six lines should fail to compile
            b = (x == x);
            b = (x != x);
        }
#endif

#if defined BDEMA_MANAGEDPTR_COMPILE_FAIL_HOMOGENEOUS_ORDERING
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

#if defined BDEMA_MANAGEDPTR_COMPILE_FAIL_HETEROGENEOUS_COMPARISON
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

#if defined BDEMA_MANAGEDPTR_COMPILE_FAIL_HETEROGENEOUS_ORDERING
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
