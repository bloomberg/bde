// bslstl_sharedptr.t.cpp                                             -*-C++-*-
#include <bslstl_sharedptr.h>

//#include <bdef_bind_test.h>
//#include <bdef_function.h>
#include <bslma_managedptr.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_usesbslmaallocator.h>
#include <bslmf_isconvertible.h>
#include <bsls_alignmenttotype.h>
#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_platform.h>
#include <bsls_stopwatch.h>
#include <bsls_types.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

//#include <bsl_algorithm.h>
//#include <bsl_iostream.h>
//#include <bsl_map.h>
//#include <bsl_memory.h>
//#include <bsl_new.h>          // placement syntax
//#include <bsl_utility.h>
//#include <bsl_vector.h>

#include <bslstl_vector.h>

#include <stdlib.h>      // atoi
#include <string.h>      // strcmp, strcpy

//using bsl::endl;
//using bsl::cerr;
//using bsl::cout;

#ifdef BSLS_PLATFORM_CMP_MSVC   // Microsoft Compiler
#ifdef _MSC_EXTENSIONS          // Microsoft Extensions Enabled
#include <new>                  // if so, need to include new as well
#endif
#endif

using namespace BloombergLP;
//using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This test driver tests the functionality of a complex mechanism with pointer
// semantics.  It is somewhat thorough but still incomplete: among the untested
// concerns:
// - The test plans are still to be written (so marked TBD).
// - There is currently no testing of exception-safety behavior, even though
//   this is a documented concern (e.g., in the createInplace function-level
//   documentation) and there is code written specifically for avoiding memory
//   leaks in the constructors (untested).
// - The usage example is also untested.
// - Many test cases assume that the default allocator will be the NewDelete
//   allocator, and fail if a TestAllocator is installed as the default in
//   'main'.  This should be addressed as part of resolving DRQS
//-----------------------------------------------------------------------------
// bslma::SharedPtrOutofplaceRep
//-----------------------------
// [17] void *originalPtr() const;
// [16] bslma::SharedPtrOutofplaceRep(...);
//
// bsl::shared_ptr
//----------------
// [ 2] bsl::shared_ptr();
//*[ 3] bsl::shared_ptr(OTHER *ptr)
//*[ 3] bsl::shared_ptr(OTHER *ptr, bslma::Allocator *basicAllocator)
// [ 3] bsl::shared_ptrTYPE *ptr, bslma::SharedPtrRep *rep);
// [ 3] bsl::shared_ptr(OTHER *ptr, DELETER *const& deleter);
// [ 3] bsl::shared_ptr(OTHER *ptr, const DELETER&, bslma::Allocator * = 0);
// [ 3] bsl::shared_ptr(nullptr_t, const DELETER&, bslma::Allocator * = 0);
// [  ] bsl::shared_ptr(bslma::ManagedPtr<OTHER>, bslma::Allocator * = 0);
// [ 3] bsl::shared_ptr(std::auto_ptr<OTHER>, bslma::Allocator * = 0);
// [  ] bsl::shared_ptr(const bsl::shared_ptr<OTHER>& alias, ELEMENT_TYPE *ptr)
// [  ] bsl::shared_ptr(const bsl::shared_ptr<OTHER>& other);
//*[ 3] bsl::shared_ptr(const bsl::shared_ptr& original);
// [ 3] bsl::shared_ptr(bslma::SharedPtrRep *rep);
// [ 2] ~bsl::shared_ptr();
// [ 4] bsl::shared_ptr& operator=(const bsl::shared_ptr& rhs);
// [ 4] bsl::shared_ptr& operator=(const bsl::shared_ptr<OTHER>& rhs);
// [ 4] bsl::shared_ptr& operator=(std::auto_ptr<OTHER> rhs);
// [ 2] void clear();
// [ 6] void load(OTHER *ptr, bslma::Allocator *allocator=0)
// [ 6] void load(OTHER *ptr, const DELETER&, bslma::Allocator *)
// [ 8] void loadAlias(const bsl::shared_ptr<OTHER>& target, TYPE *object)
// [ 5] void createInplace(bslma::Allocator *allocator=0);
// [ 5] void createInplace(bslma::Allocator *allocator, const A1& a1)
// [ 5] void createInplace(bslma::Allocator *allocator, const A1& a1, ..&a2);
// [ 5] void createInplace(bslma::Allocator *allocator, const A1& a1,...a3);
// [ 5] void createInplace(bslma::Allocator *allocator, const A1& a1,...a4);
// [ 5] void createInplace(bslma::Allocator *allocator, const A1& a1,...a5);
// [ 5] void createInplace(bslma::Allocator *allocator, const A1& a1,...a6);
// [ 5] void createInplace(bslma::Allocator *allocator, const A1& a1,...a7);
// [ 5] void createInplace(bslma::Allocator *allocator, const A1& a1,...a8);
// [ 5] void createInplace(bslma::Allocator *allocator, const A1& a1,...a9);
// [ 5] void createInplace(bslma::Allocator *allocator, const A1& a1,...a10);
// [ 5] void createInplace(bslma::Allocator *allocator, const A1& a1,...a11);
// [ 5] void createInplace(bslma::Allocator *allocator, const A1& a1,...a12);
// [ 5] void createInplace(bslma::Allocator *allocator, const A1& a1,...a13);
// [ 5] void createInplace(bslma::Allocator *allocator, const A1& a1,...a14);
// [  ] bsl::pair<TYPE *, bslma::SharedPtrRep *> release();
// [12] void swap(bsl::shared_ptr<OTHER> &src)
// [ 2] operator bsl::shared_ptr_UnspecifiedBool() const;
// [ 2] typename bsl::shared_ptr<TYPE>::Reference operator[](ptrdiff_t) const;
// [ 2] typename bsl::shared_ptr<TYPE>::Reference operator*() const;
// [ 2] TYPE *operator->() const;
// [ 2] TYPE *ptr() const;
// [ 2] bslma::SharedPtrRep *rep() const;
// [ 2] int numReferences() const;
// [13] bslma::ManagedPtr<TYPE> managedPtr() const;
// [  ] void reset();
// [  ] void reset(OTHER *ptr);
// [  ] void reset(OTHER *ptr, const DELETER& deleter);
// [  ] void reset(const bsl::shared_ptr<OTHER>& source, TYPE *ptr);
// [ 2] TYPE *get() const;
// [ 2] bool unique() const;
// [ 2] int use_count() const;
// [  ] bool owner_before(const bsl::shared_ptr<OTHER>& other) const;
// [  ] bool operator==(const bsl::shared_ptr<A>&, const bsl::shared_ptr<B>&);
// [  ] bool operator==(const bsl::shared_ptr<A>&, bsl::nullptr_t);
// [  ] bool operator==(bsl::nullptr_t,            const bsl::shared_ptr<B>&);
// [  ] bool operator!=(const bsl::shared_ptr<A>&, const bsl::shared_ptr<B>&);
// [  ] bool operator!=(const bsl::shared_ptr<A>&, bsl::nullptr_t);
// [  ] bool operator!=(bsl::nullptr_t,            const bsl::shared_ptr<B>&);
// [  ] bool operator< (const bsl::shared_ptr<A>&, const bsl::shared_ptr<B>&);
// [  ] bool operator< (const bsl::shared_ptr<A>&, bsl::nullptr_t);
// [  ] bool operator< (bsl::nullptr_t,            const bsl::shared_ptr<B>&);
// [  ] bool operator<=(const bsl::shared_ptr<A>&, const bsl::shared_ptr<B>&);
// [  ] bool operator<=(const bsl::shared_ptr<A>&, bsl::nullptr_t);
// [  ] bool operator<=(bsl::nullptr_t,            const bsl::shared_ptr<B>&);
// [  ] bool operator>=(const bsl::shared_ptr<A>&, const bsl::shared_ptr<B>&);
// [  ] bool operator>=(const bsl::shared_ptr<A>&, bsl::nullptr_t);
// [  ] bool operator>=(bsl::nullptr_t,            const bsl::shared_ptr<B>&);
// [  ] bool operator> (const bsl::shared_ptr<A>&, const bsl::shared_ptr<B>&);
// [  ] bool operator> (const bsl::shared_ptr<A>&, bsl::nullptr_t);
// [  ] bool operator> (bsl::nullptr_t,            const bsl::shared_ptr<B>&);
// [  ] bsl::ostream& operator<<(bsl::ostream&, const bsl::shared_ptr<TYPE>&);
// [  ] void swap(bsl::shared_ptr<TYPE>& a, bsl::shared_ptr<TYPE>& b);
//
// bslstl::SharedPtrUtil
//--------------------
// [ 9] bsl::shared_ptr<TARGET> dynamicCast(const bsl::shared_ptr<SOURCE>&);
// [ 9] bsl::shared_ptr<TARGET> staticCast(const bsl::shared_ptr<SOURCE>&);
// [ 9] bsl::shared_ptr<TARGET> constCast(const bsl::shared_ptr<SOURCE>&);
// [  ] void dynamicCast(bsl::shared_ptr<TRGT> *, const bsl::shared_ptr<SRC>&);
// [  ] void staticCast(bsl::shared_ptr<TRGT> *, const bsl::shared_ptr<SRC>&);
// [  ] void constCast(bsl::shared_ptr<TRGT> *, const bsl::shared_ptr<SRC>&);
// [10] bsl::shared_ptr<char> createInplaceUninitializedBuffer(...)
//
// bslstl::SharedPtrNilDeleter
//--------------------------
// [  ] void operator()(TYPE *);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] bsl::shared_ptr(TYPE *ptr);
// [ 2] bsl::shared_ptr(TYPE *ptr, bslma::Allocator *basicAllocator);
// [14] CONCERN: SHARED POINTER IN-PLACE INSIDE 'bdef_Function' OBJECT
// [15] CONCERN: C++ 'bsl::shared_ptr' COMPLIANCE
// [18] USAGE EXAMPLE // TBD
//-----------------------------------------------------------------------------

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
int testStatus = 0;

namespace {

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }

}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

}  // close unnamed namespace

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

// 'MyTestObject' CLASS HIERARCHY (defined below)
class MyTestObject;
class MyInplaceTestObject;
class MyTestBaseObject;
class MyTestDerivedObject;

// OTHER TEST OBJECTS (defined below)
class MyPDTestObject;
class MyInplaceTestObject;

// TEST DELETERS SECTION (defined below)
class MyTestObjectFactory;
class MyTestDeleter;
class MyAllocTestDeleter;
class MyTestFunctor;

void myTestFunctor(MyTestObject *);

// TYPEDEFS
typedef bsl::shared_ptr<MyTestObject> Obj;
typedef bsl::shared_ptr<const MyTestObject> ConstObj;
typedef MyTestObject TObj;

//=============================================================================
//               GLOBAL HELPER CLASSES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace NAMESPACE_TEST_CASE_16 {

int x = 1, y = 2;
double z = 3.0;

const bsl::shared_ptr<int>    ptrNil((int *)0);
const bsl::shared_ptr<int>    ptr1(&x, bslstl::SharedPtrNilDeleter(), 0);
const bsl::shared_ptr<int>    ptr2(&y, bslstl::SharedPtrNilDeleter(), 0);
const bsl::shared_ptr<double> ptr3(&z, bslstl::SharedPtrNilDeleter(), 0);

bsl::shared_ptr<int> ptrNilFun()
{
    return ptrNil;
}

bsl::shared_ptr<int> ptr1Fun()
{
    return ptr1;
}

}  // close namespace NAMESPACE_TEST_CASE_16

                   // *** 'MyTestObject' CLASS HIERARCHY ***

                           // ======================
                           // class MyTestBaseObject
                           // ======================

class MyTestBaseObject {
    // This class provides a test object used to verify that shared pointers
    // can statically and dynamically cast without slicing.

    // DATA
    char d_padding[32];

  public:
    // CREATORS
    MyTestBaseObject() {}
    virtual ~MyTestBaseObject() {}
};

                             // ==================
                             // class MyTestObject
                             // ==================

class MyTestObject : public MyTestBaseObject {
    // This class provides a test object that keeps track of how many objects
    // have been deleted.  Optionally, also keeps track of how many objects
    // have been copied.

    // DATA
    volatile bsls::Types::Int64 *d_deleteCounter_p;
    volatile bsls::Types::Int64 *d_copyCounter_p;

  public:
    // CREATORS
    MyTestObject(const MyTestObject& original);
    explicit MyTestObject(bsls::Types::Int64 *deleteCounter,
                          bsls::Types::Int64 *copyCounter = 0);
    virtual ~MyTestObject();

    // ACCESSORS
    volatile bsls::Types::Int64 *deleteCounter() const;
    volatile bsls::Types::Int64 *copyCounter() const;
};

                         // =========================
                         // class MyTestDerivedObject
                         // =========================

class MyTestObject2 : public MyTestBaseObject {
    // This supporting class for 'MyTestDerivedObject' is simply to make sure
    // that test objects with multiple inheritance work fine.

  public:
    // CREATORS
    MyTestObject2() {}
};

class MyTestDerivedObject : public MyTestObject2, public MyTestObject {
    // This class provides a test derived object, in order to make sure that
    // that test objects with multiple inheritance work fine.

  public:
    // CREATORS
    explicit MyTestDerivedObject(const MyTestObject& orig);
    explicit MyTestDerivedObject(bsls::Types::Int64 *counter,
                                 bsls::Types::Int64 *copyCounter = 0);
};

                             // ------------------
                             // class MyTestObject
                             // ------------------

// CREATORS
MyTestObject::MyTestObject(const MyTestObject& original)
: d_deleteCounter_p(original.d_deleteCounter_p)
, d_copyCounter_p(original.d_copyCounter_p)
{
    if (d_copyCounter_p) ++(*d_copyCounter_p);
}

MyTestObject::MyTestObject(bsls::Types::Int64 *deleteCounter,
                           bsls::Types::Int64 *copyCounter)
: d_deleteCounter_p(deleteCounter)
, d_copyCounter_p(copyCounter)
{
}

MyTestObject::~MyTestObject()
{
    ++(*d_deleteCounter_p);
}

// ACCESSORS
volatile bsls::Types::Int64* MyTestObject::deleteCounter() const
{
    return d_deleteCounter_p;
}

volatile bsls::Types::Int64* MyTestObject::copyCounter() const
{
    return d_copyCounter_p;
}

                         // -------------------------
                         // class MyTestDerivedObject
                         // -------------------------

// CREATORS
MyTestDerivedObject::MyTestDerivedObject(const MyTestObject& orig)
: MyTestObject(orig)
{
}

MyTestDerivedObject::MyTestDerivedObject(bsls::Types::Int64 *counter,
                                         bsls::Types::Int64 *copyCounter)
: MyTestObject(counter, copyCounter)
{
}

                         // *** OTHER TEST OBJECTS ***

                            // ====================
                            // class MyPDTestObject
                            // ====================

class MyPDTestObject {
    // This class defines a private destructor, in order to test the conversion
    // to managed pointers.

  private:
    ~MyPDTestObject() {}
};

                         // =========================
                         // class MyInplaceTestObject
                         // =========================

#define DECLARE_TEST_ARG(NAME)                                            \
class NAME {                                                              \
    int d_value;                                                          \
  public:                                                                 \
    NAME(int value = -1) : d_value(value) {}                              \
    operator int&()      { return d_value; }                              \
    operator int() const { return d_value; }                              \
};
    // This macro declares a separate type with the specified 'NAME' that wraps
    // an integer value and provides implicit conversion to and from 'int'.
    // Its main purpose is that having separate types allows to distinguish
    // them in function interface, thereby avoiding ambiguities or accidental
    // switching of arguments in the implementation of in-place constructors.

DECLARE_TEST_ARG(MyTestArg1)
DECLARE_TEST_ARG(MyTestArg2)
DECLARE_TEST_ARG(MyTestArg3)
DECLARE_TEST_ARG(MyTestArg4)
DECLARE_TEST_ARG(MyTestArg5)
DECLARE_TEST_ARG(MyTestArg6)
DECLARE_TEST_ARG(MyTestArg7)
DECLARE_TEST_ARG(MyTestArg8)
DECLARE_TEST_ARG(MyTestArg9)
DECLARE_TEST_ARG(MyTestArg10)
DECLARE_TEST_ARG(MyTestArg11)
DECLARE_TEST_ARG(MyTestArg12)
DECLARE_TEST_ARG(MyTestArg13)
DECLARE_TEST_ARG(MyTestArg14)
    // Define fourteen test argument types 'MyTestArg1..14' to be used with the
    // in-place constructors of 'MyInplaceTestObject'.

class MyInplaceTestObject {
    // This class provides a test object used to check that the arguments
    // passed for creating a shared pointer with an in-place representation are
    // of the correct types and values.

    // DATA
    MyTestArg1 d_a1;
    MyTestArg2 d_a2;
    MyTestArg3 d_a3;
    MyTestArg4 d_a4;
    MyTestArg5 d_a5;
    MyTestArg6 d_a6;
    MyTestArg7 d_a7;
    MyTestArg8 d_a8;
    MyTestArg9 d_a9;
    MyTestArg10 d_a10;
    MyTestArg11 d_a11;
    MyTestArg12 d_a12;
    MyTestArg13 d_a13;
    MyTestArg14 d_a14;

  public:
    // CREATORS
    MyInplaceTestObject() {}

    explicit MyInplaceTestObject(MyTestArg1 a1) : d_a1(a1) {}

    MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2)
        : d_a1(a1), d_a2(a2) {}

    MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2,
                        MyTestArg3 a3)
        : d_a1(a1), d_a2(a2), d_a3(a3) {}

    MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2,
                        MyTestArg3 a3, MyTestArg4 a4)
        : d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4) {}

    MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2,
                        MyTestArg3 a3, MyTestArg4 a4,
                        MyTestArg5 a5)
        : d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4), d_a5(a5) {}

    MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2,
                        MyTestArg3 a3, MyTestArg4 a4,
                        MyTestArg5 a5, MyTestArg6 a6)
        : d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4), d_a5(a5)
        , d_a6(a6) {}

    MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2,
                        MyTestArg3 a3, MyTestArg4 a4,
                        MyTestArg5 a5, MyTestArg6 a6,
                        MyTestArg7 a7)
        : d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4), d_a5(a5)
        , d_a6(a6), d_a7(a7) {}

    MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2,
                        MyTestArg3 a3, MyTestArg4 a4,
                        MyTestArg5 a5, MyTestArg6 a6,
                        MyTestArg7 a7, MyTestArg8 a8)
        : d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4), d_a5(a5)
        , d_a6(a6), d_a7(a7), d_a8(a8) {}

    MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2,
                        MyTestArg3 a3, MyTestArg4 a4,
                        MyTestArg5 a5, MyTestArg6 a6,
                        MyTestArg7 a7, MyTestArg8 a8,
                        MyTestArg9 a9)
        : d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4), d_a5(a5)
        , d_a6(a6), d_a7(a7), d_a8(a8), d_a9(a9) {}

    MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2,
                        MyTestArg3 a3, MyTestArg4 a4,
                        MyTestArg5 a5, MyTestArg6 a6,
                        MyTestArg7 a7, MyTestArg8 a8,
                        MyTestArg9 a9, MyTestArg10 a10)
        : d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4), d_a5(a5)
        , d_a6(a6), d_a7(a7), d_a8(a8), d_a9(a9), d_a10(a10) {}

    MyInplaceTestObject(MyTestArg1  a1, MyTestArg2  a2,
                        MyTestArg3  a3, MyTestArg4  a4,
                        MyTestArg5  a5, MyTestArg6  a6,
                        MyTestArg7  a7, MyTestArg8  a8,
                        MyTestArg9  a9, MyTestArg10 a10,
                        MyTestArg11 a11)
        : d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4), d_a5(a5)
        , d_a6(a6), d_a7(a7), d_a8(a8), d_a9(a9), d_a10(a10)
        , d_a11(a11) {}

    MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,
                        MyTestArg3  a3,  MyTestArg4  a4,
                        MyTestArg5  a5,  MyTestArg6  a6,
                        MyTestArg7  a7,  MyTestArg8  a8,
                        MyTestArg9  a9,  MyTestArg10 a10,
                        MyTestArg11 a11, MyTestArg12 a12)
        : d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4), d_a5(a5)
        , d_a6(a6), d_a7(a7), d_a8(a8), d_a9(a9), d_a10(a10)
        , d_a11(a11), d_a12(a12) {}

    MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,
                        MyTestArg3  a3,  MyTestArg4  a4,
                        MyTestArg5  a5,  MyTestArg6  a6,
                        MyTestArg7  a7,  MyTestArg8  a8,
                        MyTestArg9  a9,  MyTestArg10 a10,
                        MyTestArg11 a11, MyTestArg12 a12,
                        MyTestArg13 a13)
        : d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4), d_a5(a5)
        , d_a6(a6), d_a7(a7), d_a8(a8), d_a9(a9), d_a10(a10)
        , d_a11(a11), d_a12(a12), d_a13(a13) {}

    MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,
                        MyTestArg3  a3,  MyTestArg4  a4,
                        MyTestArg5  a5,  MyTestArg6  a6,
                        MyTestArg7  a7,  MyTestArg8  a8,
                        MyTestArg9  a9,  MyTestArg10 a10,
                        MyTestArg11 a11, MyTestArg12 a12,
                        MyTestArg13 a13, MyTestArg14 a14)
        : d_a1(a1), d_a2(a2), d_a3(a3), d_a4(a4), d_a5(a5)
        , d_a6(a6), d_a7(a7), d_a8(a8), d_a9(a9), d_a10(a10)
        , d_a11(a11), d_a12(a12), d_a13(a13), d_a14(a14) {}

    // ACCESSORS
    bool operator == (const MyInplaceTestObject& rhs) const
    {
        return d_a1  == rhs.d_a1  &&
               d_a1  == rhs.d_a1  &&
               d_a2  == rhs.d_a2  &&
               d_a3  == rhs.d_a3  &&
               d_a4  == rhs.d_a4  &&
               d_a5  == rhs.d_a5  &&
               d_a6  == rhs.d_a6  &&
               d_a7  == rhs.d_a7  &&
               d_a8  == rhs.d_a8  &&
               d_a9  == rhs.d_a9  &&
               d_a10 == rhs.d_a10 &&
               d_a11 == rhs.d_a11 &&
               d_a12 == rhs.d_a12 &&
               d_a13 == rhs.d_a13 &&
               d_a14 == rhs.d_a14;
    }
};

                       // *** TEST DELETERS SECTION ***

                         // =========================
                         // class MyTestObjectFactory
                         // =========================

class MyTestObjectFactory {
    // This class implements a prototypical factory deleter that simply wraps a
    // 'bslma::Allocator' without implementing this protocol.

    // DATA
    bslma::Allocator *d_allocator_p;

  public:
    // CREATORS
    explicit MyTestObjectFactory(bslma::Allocator *basicAllocator = 0);

    // ACCESSORS
    void deleteObject(MyTestObject *obj) const;
};

                            // ===================
                            // class MyTestDeleter
                            // ===================

class MyTestDeleter {
    // This class provides a prototypical function-like deleter.

    // DATA
    bslma::Allocator *d_allocator_p;

  public:
    // CREATORS
    explicit MyTestDeleter(bslma::Allocator *basicAllocator = 0);
    MyTestDeleter(const MyTestDeleter& original);

    // ACCESSORS
    template <class OBJECT_TYPE>
    void operator() (OBJECT_TYPE *ptr) const;

    bool operator==(const MyTestDeleter& rhs) const;
};

                         // ========================
                         // class MyAllocTestDeleter
                         // ========================

class MyAllocTestDeleter {
    // This class provides a prototypical function-like deleter that takes a
    // 'bslma::Allocator' at construction.  It is used to check that the
    // allocator used to construct the representation is passed correctly to
    // the deleter.

    // DATA
    bslma::Allocator *d_allocator_p;
    bslma::Allocator *d_deleter_p;
    void             *d_someMemory;

  public:
    // CREATORS
    explicit MyAllocTestDeleter(bslma::Allocator *deleter,
                                bslma::Allocator *basicAllocator = 0);

    MyAllocTestDeleter(const MyAllocTestDeleter&  original,
                       bslma::Allocator          *basicAllocator = 0);

    ~MyAllocTestDeleter();

    // MANIPULATORS
    MyAllocTestDeleter& operator=(const MyAllocTestDeleter& rhs);

    // ACCESSORS
    template <class OBJECT_TYPE>
    void operator()(OBJECT_TYPE *ptr) const;
};

namespace BloombergLP {
namespace bslma {
template <>
struct UsesBslmaAllocator<MyAllocTestDeleter>
     : bsl::true_type {};
}  // close namespace bslma
}  // close namespace BloombergLP

                             // ==================
                             // class MyTestFunctor
                             // ==================

class MyTestFunctor {
    // This class implements a 'bcef_Function' which does nothing.

  public:
    void operator()() {}
};

                           // ======================
                           // function myTestFunctor
                           // ======================

void myTestFunctor(MyTestObject *ptr)
    // This function can be used as a function-like deleter (by address) for a
    // 'bdema_SharedPtr' representation.
{
    delete ptr;
}

                         // -------------------------
                         // class MyTestObjectFactory
                         // -------------------------

// CREATORS
MyTestObjectFactory::MyTestObjectFactory(bslma::Allocator *basicAllocator)
: d_allocator_p(basicAllocator)
{
}

// ACCESSORS
void MyTestObjectFactory::deleteObject(MyTestObject *obj) const
{
    bslma::Allocator *ba = bslma::Default::allocator(d_allocator_p);
    ba->deleteObject(obj);
}

                            // -------------------
                            // class MyTestDeleter
                            // -------------------

// CREATORS
MyTestDeleter::MyTestDeleter(bslma::Allocator *basicAllocator)
: d_allocator_p(basicAllocator)
{
}

MyTestDeleter::MyTestDeleter(const MyTestDeleter& original)
: d_allocator_p(original.d_allocator_p)
{
}

template <class OBJECT_TYPE>
void MyTestDeleter::operator() (OBJECT_TYPE *ptr) const
{
    bslma::Allocator *ba = bslma::Default::allocator(d_allocator_p);
    ba->deleteObject(ptr);
}

bool MyTestDeleter::operator==(const MyTestDeleter& rhs) const
{
    return d_allocator_p == rhs.d_allocator_p;
}

                          // ------------------------
                          // class MyAllocTestDeleter
                          // ------------------------

// CREATORS
MyAllocTestDeleter::MyAllocTestDeleter(bslma::Allocator *deleter,
                                       bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_deleter_p(deleter)
{
    d_someMemory = d_allocator_p->allocate(13);
}

MyAllocTestDeleter::MyAllocTestDeleter(
                                     const MyAllocTestDeleter&  original,
                                     bslma::Allocator          *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_deleter_p(original.d_deleter_p)
{
    d_someMemory = d_allocator_p->allocate(13);
}

MyAllocTestDeleter::~MyAllocTestDeleter()
{
    d_allocator_p->deallocate(d_someMemory);
}

// MANIPULATORS
MyAllocTestDeleter& MyAllocTestDeleter::operator=(
                                                 const MyAllocTestDeleter& rhs)
{
    ASSERT(!"I think we do not use operator =");
    d_deleter_p = rhs.d_deleter_p;
    return *this;
}

// ACCESSORS
template <class OBJECT_TYPE>
void MyAllocTestDeleter::operator()(OBJECT_TYPE *ptr) const
{
    d_deleter_p->deleteObject(ptr);
}



// ============================================================================
//                      INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

template <class POINTER>
struct PerformanceTester
{
   static void test(bool verbose, bool allocVerbose) {
        bslma::TestAllocator ta(allocVerbose);

        enum {
            NUM_ITER        = 1000,
            VECTOR_SIZE     = 1000,
            BIG_VECTOR_SIZE = NUM_ITER * VECTOR_SIZE
        };

        bsls::Types::Int64 deleteCounter, copyCounter, numAlloc, numBytes;

        bsl::vector<TObj *> mZ(&ta);
        const bsl::vector<TObj *>& Z = mZ;

        bsls::Stopwatch timer;

        mZ.resize(BIG_VECTOR_SIZE);
        deleteCounter = copyCounter = 0;
        numAlloc = ta.numAllocations();
        numBytes = ta.numBytesInUse();
        timer.start();
        for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
            mZ[i] = new(ta) TObj(&deleteCounter, &copyCounter);
        }
        timer.stop();
#if 0 // TBD fix printing
        printf("Creating " << BIG_VECTOR_SIZE << " owned objects in "
             << timer.elapsedTime() << "s ("
             << timer.elapsedTime() / BIG_VECTOR_SIZE << "s each)\n");;
        if (verbose) {
            printf("\t" << ta.numAllocations() - numAlloc << " allocations, "
                 << ta.numBytesInUse() - numBytes << " bytes\n"
                 << "\t" << copyCounter << " copies of test objects\n"
                 << "\t" << deleteCounter << " deletions of test objects"
                 << endl;
        }
#endif // 0 TBD fix printing

        for (int i = 1; i < BIG_VECTOR_SIZE; ++i) {
            mZ[i]->~TObj();
        }

        deleteCounter = copyCounter = 0;
        numAlloc = ta.numAllocations();
        numBytes = ta.numBytesInUse();
        timer.start();
        for (int i = 1; i < BIG_VECTOR_SIZE; ++i) {
            new(mZ[i]) TObj(*Z[0]);
        }
        timer.stop();
#if 0 // TBD fix printing
        printf("Copy-constructing " << BIG_VECTOR_SIZE - 1
             << " owned objects in " << timer.elapsedTime() << "s ("
             << timer.elapsedTime() / (BIG_VECTOR_SIZE - 1) << "s each)"
             << endl;
        if (verbose) {
            printf("\t" << ta.numAllocations() - numAlloc << " allocations, "
                 << ta.numBytesInUse() - numBytes << " bytes\n"
                 << "\t" << copyCounter << " copies of test objects\n"
                 << "\t" << deleteCounter << " deletions of test objects"
                 << endl;
        }
#endif // 0 TBD fix printing

        deleteCounter = copyCounter = 0;
        numAlloc = ta.numAllocations();
        numBytes = ta.numBytesInUse();
        timer.start();
        for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
            ((bslma::Allocator *)&ta)->deleteObject(mZ[i]);
        }
        timer.stop();
#if 0 // TBD fix printing
        printf("Destroying " << BIG_VECTOR_SIZE << " owned objects in "
             << timer.elapsedTime() << "s ("
             << timer.elapsedTime() / BIG_VECTOR_SIZE << "s each)\n");;
        if (verbose) {
            printf("\t" << ta.numAllocations() - numAlloc << " allocations, "
                 << ta.numBytesInUse() - numBytes << " bytes\n"
                 << "\t" << copyCounter << " copies of test objects\n"
                 << "\t" << deleteCounter << " deletions of test objects"
                 << endl;
        }
#endif // 0 TBD fix printing

        deleteCounter = copyCounter = 0;
        numAlloc = ta.numAllocations();
        numBytes = ta.numBytesInUse();
        for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
            mZ[i] = new(ta) TObj(&deleteCounter, &copyCounter);
        }
#if 0 // TBD fix printing
        printf("Rehydrated " << BIG_VECTOR_SIZE
             << " owned objects\n");;
        if (verbose) {
            printf("\t" << ta.numAllocations() - numAlloc << " allocations, "
                 << ta.numBytesInUse() - numBytes << " bytes\n"
                 << "\t" << copyCounter << " copies of test objects\n"
                 << "\t" << deleteCounter << " deletions of test objects"
                 << endl;
        }
#endif // 0 TBD fix printing

        bsl::vector<POINTER> mX(&ta);
        const bsl::vector<POINTER>& X = mX;

        // -------------------------------------------------------------------
        printf("\nCreating out-of-place representations."
               "\n--------------------------------------\n");

        mX.resize(BIG_VECTOR_SIZE);
        for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
            // We first destroy the contents of mX in order to be able to
            // recreate them in place.  Using push_back instead would involve
            // an additional creation (for a temporary) and copy construction
            // into the vector, which is not what we intend to measure.

            (&mX[i])->~POINTER();
        }
        timer.reset();
        deleteCounter = copyCounter = 0;
        numAlloc = ta.numAllocations();
        numBytes = ta.numBytesInUse();
        timer.start();
        for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
            // If this code throws an exception, then the remaining elements
            // will be destroyed twice, once above and another time with the
            // destruction of mX.  But that is OK since they are empty.

            new(&mX[i]) POINTER(Z[i], &ta);
        }
        timer.stop();
#if 0 // TBD fix printing
        printf("Creating " << BIG_VECTOR_SIZE
             << " distinct shared pointers in "
             << timer.elapsedTime() << "s ("
             << timer.elapsedTime() / BIG_VECTOR_SIZE << "s each)\n");;
        if (verbose) {
            printf("\t" << ta.numAllocations() - numAlloc << " allocations, "
                 << ta.numBytesInUse() - numBytes << " bytes\n"
                 << "\t" << copyCounter << " copies of test objects\n"
                 << "\t" << deleteCounter << " deletions of test objects"
                 << endl;
        }
#endif // 0 TBD fix printing

        timer.reset();
        deleteCounter = copyCounter = 0;
        numAlloc = ta.numAllocations();
        numBytes = ta.numBytesInUse();
        timer.start();
        for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
            (&mX[i])->~POINTER();
        }
        timer.stop();
#if 0 // TBD fix printing
        printf("Destroying " << BIG_VECTOR_SIZE
             << " distinct shared pointers in "
             << timer.elapsedTime() << "s ("
             << timer.elapsedTime() / BIG_VECTOR_SIZE << "s each)\n");;
        if (verbose) {
            printf("\t" << ta.numAllocations() - numAlloc << " allocations, "
                 << ta.numBytesInUse() - numBytes << " bytes\n"
                 << "\t" << copyCounter << " copies of test objects\n"
                 << "\t" << deleteCounter << " deletions of test objects"
                 << endl;
        }
#endif // 0 TBD fix printing

        // Note:  Z now contains dangling pointers.  Rehydrate!
        deleteCounter = copyCounter = 0;
        numAlloc = ta.numAllocations();
        numBytes = ta.numBytesInUse();
        for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
            mZ[i] = new(ta) TObj(&deleteCounter, &copyCounter);
        }
#if 0 // TBD fix printing
        printf("Rehydrated " << BIG_VECTOR_SIZE
             << " owned objects\n");;
        if (verbose) {
            printf("\t" << ta.numAllocations() - numAlloc << " allocations, "
                 << ta.numBytesInUse() - numBytes << " bytes\n"
                 << "\t" << copyCounter << " copies of test objects\n"
                 << "\t" << deleteCounter << " deletions of test objects"
                 << endl;
        }
#endif // 0 TBD fix printing

        {
            POINTER Y(Z[0], &ta);
            timer.reset();
            deleteCounter = copyCounter = 0;
            numAlloc = ta.numAllocations();
            numBytes = ta.numBytesInUse();
            timer.start();
            for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
                new(&mX[i]) POINTER(Y);
            }
            timer.stop();
#if 0 // TBD fix printing
            printf("Creating " << BIG_VECTOR_SIZE
                 << " copies of the same shared pointer in "
                 << timer.elapsedTime() << "s ("
                 << timer.elapsedTime() / BIG_VECTOR_SIZE << "s each)\n");;
            if (verbose) {
                printf("\t" << ta.numAllocations() - numAlloc
                     << " allocations, "
                     << ta.numBytesInUse() - numBytes << " bytes\n"
                     << "\t" << copyCounter << " copies of test objects\n"
                     << "\t" << deleteCounter << " deletions of test objects"
                     << endl;
            }
#endif // 0 TBD fix printing
        }

        timer.reset();
        deleteCounter = copyCounter = 0;
        numAlloc = ta.numAllocations();
        numBytes = ta.numBytesInUse();
        timer.start();
        for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
            (&mX[i])->~POINTER();
        }
        timer.stop();
#if 0 // TBD fix printing
        printf("Destroying " << BIG_VECTOR_SIZE
             << " times the same shared pointer in "
             << timer.elapsedTime() << "s ("
             << timer.elapsedTime() / BIG_VECTOR_SIZE << "s each)\n");;
        if (verbose) {
            printf("\t" << ta.numAllocations() - numAlloc << " allocations, "
                 << ta.numBytesInUse() - numBytes << " bytes\n"
                 << "\t" << copyCounter << " copies of test objects\n"
                 << "\t" << deleteCounter << " deletions of test objects"
                 << endl;
        }
#endif // TBD fix printing

        // Note:  Z[0] is now dangling, and X contains only empty shared
        // pointers.  Rehydrate, but with empty shared pointers!
        deleteCounter = copyCounter = 0;
        numAlloc = ta.numAllocations();
        numBytes = ta.numBytesInUse();
        mZ[0] = new(ta) TObj(&deleteCounter, &copyCounter);
        for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
            new(&mX[i]) POINTER();
        }
#if 0 // TBD fix printing
        printf("Rehydrated 1 owned object and " << BIG_VECTOR_SIZE
             << " empty shared pointers\n");;
        if (verbose) {
            printf("\t" << ta.numAllocations() - numAlloc << " allocations, "
                 << ta.numBytesInUse() - numBytes << " bytes\n"
                 << "\t" << copyCounter << " copies of test objects\n"
                 << "\t" << deleteCounter << " deletions of test objects"
                 << endl;
        }
#endif // TBD fix printing

        printf("\nCreating in-place representations."
               "\n----------------------------------\n");

        timer.reset();
        deleteCounter = copyCounter = 0;
        numAlloc = ta.numAllocations();
        numBytes = ta.numBytesInUse();
        timer.start();
        for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
            mX[i].createInplace(&ta, *Z[i]);
        }
        timer.stop();
#if 0 // TBD fix printing
        printf("Creating " << BIG_VECTOR_SIZE
             << " distinct in-place shared pointers in "
             << timer.elapsedTime() << "s ("
             << timer.elapsedTime() / BIG_VECTOR_SIZE << "s each)\n");;
        if (verbose) {
            printf("\t" << ta.numAllocations() - numAlloc << " allocations, "
                 << ta.numBytesInUse() - numBytes << " bytes\n"
                 << "\t" << copyCounter << " copies of test objects\n"
                 << "\t" << deleteCounter << " deletions of test objects"
                 << endl;
        }
#endif // TBD fix printing

        timer.reset();
        deleteCounter = copyCounter = 0;
        numAlloc = ta.numAllocations();
        numBytes = ta.numBytesInUse();
        timer.start();
        for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
            (&mX[i])->~POINTER();
        }
        timer.stop();
#if 0 // TBD fix printing
        printf("Destroying " << BIG_VECTOR_SIZE
             << " distinct in-place shared pointers in "
             << timer.elapsedTime() << "s ("
             << timer.elapsedTime() / BIG_VECTOR_SIZE << "s each)\n");;
        if (verbose) {
            printf("\t" << ta.numAllocations() - numAlloc << " allocations, "
                 << ta.numBytesInUse() - numBytes << " bytes\n"
                 << "\t" << copyCounter << " copies of test objects\n"
                 << "\t" << deleteCounter << " deletions of test objects"
                 << endl;
        }
#endif // TBD fix printing

        printf("\nCreating aliased shared pointers."
               "\n---------------------------------\n");

        {
            POINTER Y(Z[0], &ta);
            timer.reset();
            deleteCounter = copyCounter = 0;
            numAlloc = ta.numAllocations();
            numBytes = ta.numBytesInUse();
            timer.start();
            for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
                new(&mX[i]) POINTER(Y, Z[i]);
            }
            timer.stop();
#if 0 // TBD fix printing
            printf("Creating " << BIG_VECTOR_SIZE
                 << " aliases of the same shared pointer in "
                 << timer.elapsedTime() << "s ("
                 << timer.elapsedTime() / BIG_VECTOR_SIZE << "s each)\n");;
            if (verbose) {
                printf("\t" << ta.numAllocations() - numAlloc
                     << " allocations, "
                     << ta.numBytesInUse() - numBytes << " bytes\n"
                     << "\t" << copyCounter << " copies of test objects\n"
                     << "\t" << deleteCounter << " deletions of test objects"
                     << endl;
            }
#endif // TBD fix printing
        }

        timer.reset();
        deleteCounter = copyCounter = 0;
        numAlloc = ta.numAllocations();
        numBytes = ta.numBytesInUse();
        timer.start();
        for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
            (&mX[i])->~POINTER();
        }
        timer.stop();
#if 0 // TBD fix printing
        printf("Destroying " << BIG_VECTOR_SIZE
             << " aliases of the same shared pointer in "
             << timer.elapsedTime() << "s ("
             << timer.elapsedTime() / BIG_VECTOR_SIZE << "s each)\n");;
        if (verbose) {
            printf("\t" << ta.numAllocations() - numAlloc << " allocations, "
                 << ta.numBytesInUse() - numBytes << " bytes\n"
                 << "\t" << copyCounter << " copies of test objects\n"
                 << "\t" << deleteCounter << " deletions of test objects"
                 << endl;
        }
#endif  // TBD fix printing

        // Note:  Z[0] is now dangling, and X contains only empty shared
        // pointers.  Rehydrate!
        deleteCounter = copyCounter = 0;
        numAlloc = ta.numAllocations();
        numBytes = ta.numBytesInUse();
        mZ[0] = new(ta) TObj(&deleteCounter, &copyCounter);
        for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
            new(&mX[i]) POINTER(Z[i], &ta);
        }
#if 0 // TBD fix printing
        printf("Rehydrated 1 owned object and " << BIG_VECTOR_SIZE
             << " shared pointers\n");;
        if (verbose) {
            printf("\t" << ta.numAllocations() - numAlloc << " allocations, "
                 << ta.numBytesInUse() - numBytes << " bytes\n"
                 << "\t" << copyCounter << " copies of test objects\n"
                 << "\t" << deleteCounter << " deletions of test objects"
                 << endl;
        }
#endif // TBD fix printing

        // -------------------------------------------------------------------
        printf("\nAssignment."
               "\n-----------\n");

        timer.reset();
        deleteCounter = copyCounter = 0;
        numAlloc = ta.numAllocations();
        numBytes = ta.numBytesInUse();
        timer.start();
        {
            POINTER Y = X[0];
            for (int j = 1; j < BIG_VECTOR_SIZE; ++j) {
                mX[j - 1] = X[j];
            }
            mX.back() = Y;
        }
        timer.stop();
#if 0 // TBD fix printing
        printf("Assigning " << BIG_VECTOR_SIZE + 1
             << " distinct shared pointers in "
             << timer.elapsedTime() << "s ("
             << timer.elapsedTime() / (BIG_VECTOR_SIZE + 1) << "s each)"
             << endl;
        if (verbose) {
            printf("\t" << ta.numAllocations() - numAlloc << " allocations, "
                 << ta.numBytesInUse() - numBytes << " bytes\n"
                 << "\t" << copyCounter << " copies of test objects\n"
                 << "\t" << deleteCounter << " deletions of test objects"
                 << endl;
        }
#endif // TBD fix printing

        timer.reset();
        deleteCounter = copyCounter = 0;
        numAlloc = ta.numAllocations();
        numBytes = ta.numBytesInUse();
        timer.start();
        {
            POINTER Y = X[0];
            for (int j = 1; j < BIG_VECTOR_SIZE; ++j) {
                mX[j] = Y;
            }
        }
        timer.stop();
#if 0 // TBD fix printing
        printf("Assigning " << BIG_VECTOR_SIZE
             << " times the same shared pointer in "
             << timer.elapsedTime() << "s ("
             << timer.elapsedTime() / BIG_VECTOR_SIZE << "s each)\n");;
        if (verbose) {
            printf("\t" << ta.numAllocations() - numAlloc << " allocations, "
                 << ta.numBytesInUse() - numBytes << " bytes\n"
                 << "\t" << copyCounter << " copies of test objects\n"
                 << "\t" << deleteCounter << " deletions of test objects"
                 << endl;
        }
#endif // TBD fix printing

        // Note:  Z now contains dangling pointers, except Z[0].  Rehydrate!
        // Note:  Z now contains dangling pointers.  Rehydrate!
        deleteCounter = copyCounter = 0;
        numAlloc = ta.numAllocations();
        numBytes = ta.numBytesInUse();
        for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
            mZ[i] = new(ta) TObj(&deleteCounter, &copyCounter);
        }
#if 0 // TBD fix printing
        printf("Rehydrated " << BIG_VECTOR_SIZE
             << " owned objects\n");;
        if (verbose) {
            printf("\t" << ta.numAllocations() - numAlloc << " allocations, "
                 << ta.numBytesInUse() - numBytes << " bytes\n"
                 << "\t" << copyCounter << " copies of test objects\n"
                 << "\t" << deleteCounter << " deletions of test objects"
                 << endl;
        }
#endif // TBD fix printing

        // -------------------------------------------------------------------
        printf("\nPooling out-of-place representations."
               "\n-------------------------------------\n");

        // TBD

        // -------------------------------------------------------------------
        for (int i = 0; i < BIG_VECTOR_SIZE; ++i) {
            ta.deleteObject(mZ[i]);
        }
   }
};

template <typename T>
class ManagedPtrTestDeleter {

    T* d_providedObj;

  public:
    ManagedPtrTestDeleter() : d_providedObj(0) {}

    void deleteObject(T* obj) {
        ASSERT((int)(0 == d_providedObj));
        d_providedObj = obj;
    }

    T* providedObj() {
        return d_providedObj;
    }

    void reset() {
        d_providedObj = 0;
    }
};

class SelfReference
{
    // DATA
    bsl::shared_ptr<SelfReference> d_dataPtr;

  public:
    // MANIPULATORS
    void setData(bsl::shared_ptr<SelfReference>& value) { d_dataPtr = value; }
    void release() { d_dataPtr.reset(); }
};


std::auto_ptr<TObj> makeAuto()
{
    return std::auto_ptr<TObj>((TObj*)0);
}

std::auto_ptr<TObj> makeAuto(bsls::Types::Int64 *counter)
{
    BSLS_ASSERT_OPT(counter);

    return std::auto_ptr<TObj>(new TObj(counter));
}

namespace TestDriver {
template <class TYPE>
void doNotDelete(TYPE *) {} // Do nothing
}  // close namespace TestDriver

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    bsls::Types::Int64 numDeallocations;
    bsls::Types::Int64 numAllocations;
    bsls::Types::Int64 numDeletes = 0;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
    case 23: {
        // --------------------------------------------------------------------
        // TESTING 'bsl::shared_ptr<cv-void> (DRQS 33549823)
        //
        // Concerns:
        //: 1 Can compare two constant shared pointer objects using any
        //:   comparison operator.
        //:
        //: 2 Can compare two shared pointer objects pointing to different
        //:   target types.
        //:
        //: 3 Can correctly compare a shared pointer with a null pointer in
        //:   either order.
        //
        // Plan:
        //: 1 TBD
        //
        // Testing:
        //   bool owner_before(const bsl::shared_ptr<OTHER>& other) const;
        //   bool operator==(const bsl::shared_ptr&, const bsl::shared_ptr&);
        //   bool operator==(const bsl::shared_ptr<A>&, bsl::nullptr_t);
        //   bool operator==(bsl::nullptr_t, const bsl::shared_ptr<B>&);
        //   bool operator!=(const bsl::shared_ptr&, const bsl::shared_ptr&);
        //   bool operator!=(const bsl::shared_ptr<A>&, bsl::nullptr_t);
        //   bool operator!=(bsl::nullptr_t, const bsl::shared_ptr<B>&);
        //   bool operator< (const bsl::shared_ptr&, const bsl::shared_ptr&);
        //   bool operator< (const bsl::shared_ptr<A>&, bsl::nullptr_t);
        //   bool operator< (bsl::nullptr_t, const bsl::shared_ptr<B>&);
        //   bool operator<=(const bsl::shared_ptr&, const bsl::shared_ptr&);
        //   bool operator<=(const bsl::shared_ptr<A>&, bsl::nullptr_t);
        //   bool operator<=(bsl::nullptr_t, const bsl::shared_ptr<B>&);
        //   bool operator>=(const bsl::shared_ptr&, const bsl::shared_ptr&);
        //   bool operator>=(const bsl::shared_ptr<A>&, bsl::nullptr_t);
        //   bool operator>=(bsl::nullptr_t, const bsl::shared_ptr<B>&);
        //   bool operator> (const bsl::shared_ptr&, const bsl::shared_ptr&);
        //   bool operator> (const bsl::shared_ptr<A>&, bsl::nullptr_t);
        //   bool operator> (bsl::nullptr_t, const bsl::shared_ptr<B>&);
        // --------------------------------------------------------------------

        typedef bsl::shared_ptr<const int> IntPtr;
        typedef bsl::shared_ptr<void>      VoidPtr;

        bslstl::SharedPtrNilDeleter doNothing = {};

        int sampleArray[] = { 42, 13 };
        int *const pA = &sampleArray[0];
        int *const pB = &sampleArray[1];

        const IntPtr  X(sampleArray, doNothing);
        const VoidPtr Y(X, pB);

        ASSERT(  X == X  );
        ASSERT(!(X != X) );
        ASSERT(!(X <  X) );
        ASSERT(  X <= X  );
        ASSERT(  X >= X  );
        ASSERT(!(X >  X) );

        ASSERT(!(X == Y) );
        ASSERT(  X != Y  );
        ASSERT(  X <  Y  );
        ASSERT(  X <= Y  );
        ASSERT(!(X >= Y) );
        ASSERT(!(X >  Y) );

        ASSERT(!(Y == X) );
        ASSERT(  Y != X  );
        ASSERT(!(Y <  X) );
        ASSERT(!(Y <= X) );
        ASSERT(  Y >= X  );
        ASSERT(  Y >  X  );

        ASSERT(  Y == Y  );
        ASSERT(!(Y != Y) );
        ASSERT(!(Y <  Y) );
        ASSERT(  Y <= Y  );
        ASSERT(  Y >= Y  );
        ASSERT(!(Y >  Y) );

        const IntPtr Z;
        ASSERT(  Z == 0  );
        ASSERT(!(Z != 0) );
        ASSERT(!(Z <  0) );
        ASSERT(  Z <= 0  );
        ASSERT(  Z >= 0  );
        ASSERT(!(Z >  0) );

        ASSERT(  0 == Z  );
        ASSERT(!(0 != Z) );
        ASSERT(!(0 <  Z) );
        ASSERT(  0 <= Z  );
        ASSERT(  0 >= Z  );
        ASSERT(!(0 >  Z) );

        ASSERT(!(X == 0) );
        ASSERT(  X != 0  );
        ASSERT(!(X <  0) );
        ASSERT(!(X <= 0) );
        ASSERT(  X >= 0  );
        ASSERT(  X >  0  );

        ASSERT(!(0 == X) );
        ASSERT(  0 != X  );
        ASSERT(  0 <  X  );
        ASSERT(  0 <= X  );
        ASSERT(!(0 >= X) );
        ASSERT(!(0 >  X) );

        ASSERT(!(X == Z) );
        ASSERT(  X != Z  );
        ASSERT(!(X <  Z) );
        ASSERT(!(X <= Z) );
        ASSERT(  X >= Z  );
        ASSERT(  X >  Z  );

        ASSERT(!(Z == X) );
        ASSERT(  Z != X  );
        ASSERT(  Z <  X  );
        ASSERT(  Z <= X  );
        ASSERT(!(Z >= X) );
        ASSERT(!(Z >  X) );

        const IntPtr  A(pA, doNothing);
        const VoidPtr B(pB, doNothing);

        ASSERT(!A.owner_before(A));
        ASSERT(!X.owner_before(X));
        ASSERT(!X.owner_before(Y));
        ASSERT(!Z.owner_before(Z));
        ASSERT(A.owner_before(B) != B.owner_before(A));
        ASSERT(A.owner_before(Z) == B.owner_before(Z));
    } break;
    case 22: {
        // --------------------------------------------------------------------
        // TESTING 'bsl::shared_ptr<cv-void> (DRQS 33549823)
        //
        // Concerns:
        //: 1 Can construct a shared pointer to a cv-qualified 'void' type.
        //:
        //: 2 Can perform basic operations that do not dereference such a
        //:   pointer.
        //
        // Plan:
        //: 1 Create a 'shared_ptr<cv-void>' object for each cv variant.
        //:
        //: 2 Run through a quick set of reasonable operations for a non-
        //:   dereferenceable smart pointer.
        //:
        //: 3 Verify that an assertion failure does not happen (in any mode).
        //
        // Testing:
        //   void reset();
        // --------------------------------------------------------------------

        if (verbose) printf("Confirming bsl::shared_ptr<void> support.\n");
        {
            typedef bsl::shared_ptr<void> TestObj;
            int iX = 42;
            TestObj pX(&iX, bslstl::SharedPtrNilDeleter(), 0);
            TestObj pY = pX;  const TestObj& Y = pY;

            ASSERT(   Y == pX );
            ASSERT(!(pX != Y) );

            ASSERT(&iX == static_cast<int *>(Y.get()));

            double dY = 3.14159;
            pY.reset(&dY, bslstl::SharedPtrNilDeleter());

            ASSERT(   Y != pX );
            ASSERT(!(pX == Y) );

            ASSERT(&dY == static_cast<double *>(Y.get()));
        }

        if (verbose) printf("Confirming bsl::shared_ptr<const void> support.\n");
        {
            typedef bsl::shared_ptr<const void> TestObj;
            const int iX = 42;
            TestObj pX(&iX, bslstl::SharedPtrNilDeleter(), 0);
            TestObj pY = pX;  const TestObj& Y = pY;

            ASSERT(   Y == pX );
            ASSERT(!(pX != Y) );

            ASSERT(&iX == static_cast<const int *>(Y.get()));

            double dY = 3.14159;
            pY.reset(&dY, bslstl::SharedPtrNilDeleter());

            ASSERT(   Y != pX );
            ASSERT(!(pX == Y) );

            ASSERT(&dY == static_cast<const double *>(Y.get()));
        }

#if 0   // volatile types are not yet supported in general.  The test case is
        // retained as a demonstration of what might need fixing if support for
        // volatile types becomes desirable, such as for standard conformance.
        // Note that the current failures occur in the out-of-place rep type.

        if (verbose) printf(
                        "Confirming bsl::shared_ptr<volatile void> support.\n");
        {
            typedef bsl::shared_ptr<volatile void> TestObj;
            int iX = 42;
            TestObj pX(&iX, bslstl::SharedPtrNilDeleter(), 0);
            TestObj pY = pX;  const TestObj& Y = pY;

            ASSERT(   Y == pX );
            ASSERT(!(pX != Y) );

            ASSERT(&iX == static_cast<volatile int *>(Y.get()));

            volatile double dY = 3.14159;
            pY.reset(&dY, bslstl::SharedPtrNilDeleter());

            ASSERT(   Y != pX );
            ASSERT(!(pX == Y) );

            ASSERT(&dY == static_cast<volatile double *>(Y.get()));
        }

        if (verbose) printf(
                  "Confirming bsl::shared_ptr<const volatile void> support.\n");
        {
            typedef bsl::shared_ptr<const volatile void> TestObj;
            volatile int iX = 42;
            TestObj pX(&iX, bslstl::SharedPtrNilDeleter(), 0);
            TestObj pY = pX;  const TestObj& Y = pY;

            ASSERT(   Y == pX );
            ASSERT(!(pX != Y) );

            ASSERT(&iX == static_cast<const volatile int *>(Y.get()));

            const double dY = 3.14159;
            pY.reset(&dY, bslstl::SharedPtrNilDeleter());

            ASSERT(   Y != pX );
            ASSERT(!(pX == Y) );

            ASSERT(&dY == static_cast<const volatile double *>(Y.get()));
        }
#endif
    } break;
    case 21: {
        // --------------------------------------------------------------------
        // TESTING 'reset' using a self-referenced shared ptr (DRQS 26465543)
        //
        // Concerns:
        //: 1 Resetting the last reference to a self-referenced shared pointer
        //:   calls 'releaseRef' only once.
        //
        // Plan:
        //: 1 Create a self-referring shared pointer.
        //:
        //: 2 Call 'reset' on the referenced object.
        //:
        //: 3 Verify that an assertion failure does not happen (in any mode).
        //
        // Testing:
        //   void reset();
        // --------------------------------------------------------------------

        SelfReference *ptr;
        {
            bsl::shared_ptr<SelfReference> mX;
            mX.createInplace();
            mX->setData(mX);
            ptr = mX.ptr();
        }

        ptr->release();
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING constructing from ManagedPtr
        //
        // Concerns:
        //   1) When constructing from a managed-ptr, the original deleter
        //      specified in the managed-ptr is used to destroy the object
        //
        //   2) When constructing from an aliased managed-ptr, the original
        //      deleter is supplied the correct address
        //
        // Testing:
        //   bsl::shared_ptr(bslma::ManagedPtr<OT> & original);
        // --------------------------------------------------------------------

        ManagedPtrTestDeleter<int> deleter;

        int obj1, obj2;

        bsl::shared_ptr<int> outerSp;
        {
            bslma::ManagedPtr<int> mp1 (&obj1, &deleter);

            bsl::shared_ptr<int> sp1 (mp1);
            sp1.clear();

            // check non-aliased managed-ptr assignment
            ASSERT(&obj1 == deleter.providedObj());

            deleter.reset();
            mp1.load(&obj2, &deleter);

            bslma::ManagedPtr<int> mp2 (mp1, &obj1);
            bsl::shared_ptr<int> sp2 (mp2);
            outerSp = sp2;
        }
        outerSp.clear();
        // check aliased managed-ptr assignment
        ASSERT(&obj2 == deleter.providedObj());
      } break;

      case 19: {
        // --------------------------------------------------------------------
        // TESTING bslma::SharedPtrOutofplaceRep
        //
        // Concerns:
        //   1) 'incrementRefs' and 'decrementRefs' correctly adjust the number
        //      of references by the specified amount.
        //   2) 'incrementRefs' and 'decrementRefs' defaults the number of
        //      increments and decrements to 1.
        //   3) 'decrementRefs' correctly return the updated number of
        //      references.
        //   4) 'numReferences' return the current count of the number of
        //      of references.
        //
        // Plan:
        //   First increment and decrement the number of references by the
        //   default amount, then verify using 'numReferences' that the number
        //   of references are adjusted accordingly.  Then increment and
        //   decrement the reference count by 1 first (non-default), then 2,
        //   then 3, and verify the number of references are adjusted
        //   accordingly between each increment / decrement.
        //
        // Testing:
        //   void incrementRefs(int incrementAmount = 1);
        //   int decrementRefs(int decrementAmount = 1);
        //   int numReferences() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\n");

        {
            bslma::Allocator     *da = bslma::Default::allocator();
            bslma::TestAllocator *t  = 0;
            int *x                  = new int(0);
            int count               = 0;

            bslma::SharedPtrOutofplaceRep<int, bslma::TestAllocator*> *implPtr =
                     bslma::SharedPtrOutofplaceRep<int, bslma::TestAllocator*>::
                                                         makeOutofplaceRep(x,
                                                                           t,
                                                                           t);
            bslma::SharedPtrOutofplaceRep<int, bslma::TestAllocator*>&impl
                                                              = *implPtr;
            LOOP_ASSERT(impl.numReferences(), 1 == impl.numReferences());

            impl.acquireRef();
            LOOP_ASSERT(impl.numReferences(), 2 == impl.numReferences());

            impl.acquireRef();
            LOOP_ASSERT(impl.numReferences(), 3 == impl.numReferences());

            impl.releaseRef();
            LOOP_ASSERT(impl.numReferences(), 2 == impl.numReferences());

            impl.releaseRef();
            LOOP_ASSERT(impl.numReferences(), 1 == impl.numReferences());

            impl.incrementRefs(1);
            LOOP_ASSERT(impl.numReferences(), 2 == impl.numReferences());

            impl.incrementRefs(1);
            LOOP_ASSERT(impl.numReferences(), 3 == impl.numReferences());

            impl.releaseRef();
            LOOP_ASSERT(impl.numReferences(), 2 == impl.numReferences());

            impl.releaseRef();
            LOOP_ASSERT(impl.numReferences(), 1 == impl.numReferences());

            impl.incrementRefs(2);
            LOOP_ASSERT(impl.numReferences(), 3 == impl.numReferences());

            impl.incrementRefs(2);
            LOOP_ASSERT(impl.numReferences(), 5 == impl.numReferences());

            impl.releaseRef();
            impl.releaseRef();
            LOOP_ASSERT(impl.numReferences(), 3 == impl.numReferences());

            impl.releaseRef();
            impl.releaseRef();
            LOOP_ASSERT(impl.numReferences(), 1 == impl.numReferences());

            impl.incrementRefs(3);
            LOOP_ASSERT(impl.numReferences(), 4 == impl.numReferences());

            impl.incrementRefs(3);
            LOOP_ASSERT(impl.numReferences(), 7 == impl.numReferences());

            impl.releaseRef();
            impl.releaseRef();
            impl.releaseRef();
            LOOP_ASSERT(impl.numReferences(), 4 == impl.numReferences());

            impl.releaseRef();
            impl.releaseRef();
            impl.releaseRef();
            LOOP_ASSERT(impl.numReferences(), 1 == impl.numReferences());

            impl.releaseRef();
            //da->deallocate(implPtr);
        }
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING bslma::SharedPtrOutofplaceRep CTORS
        //
        // Plan: TBD
        //
        // Testing:
        //   CONCERN: bslma::SharedPtrOutofplaceRep passes allocator
        //            to the deleter's constructor
        // --------------------------------------------------------------------

        if (verbose) printf("\nConcern: bslma::SharedPtrOutofplaceRep passes"
                            "\nallocator to the deleter's constructor."
                            "\n=======================================\n");

        bslma::TestAllocator ta1(veryVeryVerbose);
        bslma::TestAllocator ta2(veryVeryVerbose);
        bsls::Types::Int64 numDeletes1 = 0;

        {
            bslma::DefaultAllocatorGuard allocGuard(&ta1);

            MyTestObject *p1 = new (ta2) MyTestObject(&numDeletes1);
            MyAllocTestDeleter d(&ta2, &ta2);
            Obj x1(p1,d,&ta2);
            ASSERT(0 == ta1.numBytesInUse());
        }

        ASSERT(1 == numDeletes1);
        ASSERT(0 == ta1.numBytesInUse());
        ASSERT(0 == ta2.numBytesInUse());

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING bslma::SharedPtrOutofplaceRep::originalPtr
        //
        // Plan:  Create shared pointers with various representations, release
        // them (getting back a pointer to the representation object) and
        // assert that the 'originalPtr' of that representation is identical to
        // the address of the managed object.
        //
        // Testing:
        //   CONCERN: bslma::SharedPtrRep::originalPtr returns correct value
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nConcern: 'bslma::SharedPtrRep::originalPtr' returns"
                   "\ncorrect value for 'bslma::SharedPtrOutofplaceRep'"
                   "\n=================================================\n");

        {
            bslma::TestAllocator ta(veryVeryVerbose);
            bsls::Types::Int64 numDeletes1 = 0;
            MyTestObject *p1 = new (ta) MyTestObject(&numDeletes1);
            Obj x1(p1, &ta);

            bsl::pair<MyTestObject*,bslma::SharedPtrRep*> r = x1.release();

            ASSERT(0 == x1.get());

            ASSERT(r.first == r.second->originalPtr());
            ASSERT(p1 == r.first);

            Obj x2(r.first, r.second);
            x2.clear();

            ASSERT(1 == numDeletes1);
            ASSERT(2 == ta.numAllocations());
            ASSERT(0 == ta.numBytesInUse());
        }

        if (verbose)
            printf("\nConcern: 'bslma::SharedPtrRep::originalPtr' returns"
                   "\ncorrect value for 'bslma::SharedPtrInplaceRep'"
                   "\n====================================================\n");

        {
            bslma::TestAllocator ta(veryVeryVerbose);
            bsls::Types::Int64 numDeletes1 = 0;
            Obj x1;
            x1.createInplace(&ta, &numDeletes1);
            bsl::pair<MyTestObject*,bslma::SharedPtrRep*> r = x1.release();

            ASSERT(0 == x1.get());

            ASSERT(r.first == r.second->originalPtr());

            Obj x2(r.first, r.second);
            x2.clear();

            ASSERT(1 == numDeletes1);
            ASSERT(1 == ta.numAllocations());
            ASSERT(0 == ta.numBytesInUse());
        }

        if (verbose)
            printf("\nConcern: 'bslma::SharedPtrRep::originalPtr' returns"
                   "\ncorrect value when aliased"
                   "\n===================================================\n");

        {
            typedef bsl::vector<MyTestObject2> V;
            bslma::TestAllocator ta(veryVeryVerbose);
            bsls::Types::Int64 numDeletes1 = 0;
            V *v1 = new (ta) V(&ta);
            bsl::shared_ptr<V> x1(v1,&ta);
            v1->resize(2);

            bsl::shared_ptr<MyTestObject2> a1(x1,&v1->at(1));
            bsl::pair<MyTestObject2*,bslma::SharedPtrRep*> r = a1.release();
            ASSERT(0==a1.get());

            ASSERT(r.first != r.second->originalPtr());
            ASSERT(v1 == r.second->originalPtr());

            r.second->releaseRef();
            x1.clear();

            ASSERT(0 == ta.numBytesInUse());
        }

      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: CONVERSION TO BOOL
        //
        // Concerns: Implicit conversion to 'bool' are troublesome, because a
        //   bool is further convertible to integral types, and thus such
        //   conversions should be banned.  Specifically, we are concerned that
        //     o SharedPtr can be used in "boolean" contexts such as 'if (p)',
        //       'if (!p)', 'if
        //     o SharedPtr cannot be converted to an 'int'.
        //     o SharedPtr returned by a function (as a temporary) does not
        //       lead to erroneous bool value (DRQS 12252806).
        //
        // Plan: We test the conversion in a variety of "boolean" contexts and
        //   assert that the result is as expected.  In order to test for the
        //   *absence* of conversion, we can use 'bslmf_isconvertible'.  In
        //   order to test for the absence of 'operator<', we use our own
        //   definition of 'operator<', which will be picked up and which will
        //   create an ambiguity if one is already defined.  We verify that our
        //   'operator<' has been picked up by using a helper function, which
        //   has two matchings, one to the return type of our definition of
        //   'operator<', and the other to '...'.
        //
        // Testing:
        //   operator bsl::shared_ptr_UnspecifiedBool()
        // --------------------------------------------------------------------

        if (verbose) printf("\nConcern: 'bsl::shared_ptr' compliance\n"
                            "\n=====================================\n");

        using namespace NAMESPACE_TEST_CASE_16;

        if (verbose) printf("Not convertible to ints.\n");

        ASSERT((0 == bslmf::IsConvertible<bsl::shared_ptr<int>, int>::VALUE));

        if (verbose) printf("Simple boolean expressions.\n");

        ASSERT(!ptrNil);
        ASSERT(ptr1);

        if (verbose) printf("Comparisons.\n");

            // COMPARISON SHR PTR TO SHR PTR
        ASSERT(!(ptrNil == ptr1));
        ASSERT(ptrNil != ptr1);

            // COMPARISON SHR PTR TO BOOL
        ASSERT(ptrNil == false);
        ASSERT(ptr1 != false);

            // COMPARISON SHR PTR TO INT
        ASSERT(ptrNil == 0);
        ASSERT(ptr1 != 0);

            // COMPARISON BOOL TO SHR PTR
        ASSERT(false == ptrNil);
        ASSERT(false != ptr1);
        ASSERT(true  && ptr1);

        if (verbose) printf("Boolean operators.\n");

        ASSERT(!ptrNil && true);
        ASSERT(!ptrNil || false);
        ASSERT(ptr1 && true);
        ASSERT(ptr1 || false);

        ASSERT(true && !ptrNil);
        ASSERT(false || !ptrNil);
        ASSERT(true && ptr1);
        ASSERT(false || ptr1);

        ASSERT(!ptrNil && 1);
        ASSERT(!ptrNil || 0);
        ASSERT(ptr1 && 1);
        ASSERT(ptr1 || 0);

        ASSERT(1 && !ptrNil);
        ASSERT(0 || !ptrNil);
        ASSERT(1 && ptr1);
        ASSERT(0 || ptr1);

        if (verbose) printf("With function return values.\n");
        // DRQS 12252806

        ASSERT(ptr1Fun() && true);
        ASSERT(!ptrNilFun() && true);

        ASSERT(!ptrNilFun() && ptrNilFun() == ptrNil);
        ASSERT(!ptrNilFun() || ptrNilFun() != ptr1);
        ASSERT(ptr1Fun() && ptr1Fun() == ptr1);
        ASSERT(ptr1Fun() || ptr1Fun() != ptrNil);

        ASSERT(!(ptrNilFun() && *ptrNilFun() != *ptr1));

#if 0
        if (verbose) printf("Should not compile.\n\n");;

        // COMPARISON SHR PTR TO INT
        ASSERT(ptrNil != 1);
        ASSERT(ptr1 == 1);
        ASSERT(ptrNil != 2);
        ASSERT(ptr1 == 2);

        // COMPARISON BOOL TO SHR PTR
        ASSERT(ptrNil != true);   // compiles on CC :(
        ASSERT(ptr1 == true);     // compiles on CC :(
        ASSERT(true  != ptrNil);  // compiles on CC :(
        ASSERT(true  == ptr1);    // compiles on CC :(

        // COMPARISON INT TO SHR PTR
        ASSERT(0 == ptrNil);
        ASSERT(0 != ptr1);
        ASSERT(1 != ptrNil);
        ASSERT(1 == ptr1);
        ASSERT(2 != ptrNil);
        ASSERT(2 == ptr1);  printf("r on CC, error on x.\n\n");;

        // LESS THAN COMPARISONS
        ASSERT(ptrNil < ptr1);
        ASSERT(ptrNil < ptr3);
        ASSERT(ptr1 < ptr2);
        ASSERT(ptr1 < ptr3);

        bsl::map<SharedPtr<int>, int> sharedPtrMap;
        // no instantiation of comparison and conversion to int, no error!
        sharedPtrMap[p] = 1;  // needs comparisons: error on CC, error on xlC_r
        sharedPtrMap[q] = 2;  // needs comparisons: error on CC, error on xlC_r

        if using bool instead of unspecifiedBool, everything OK until here:
            printf(sharedPtrMap[p] << "\n";  // prints 1
        printf(sharedPtrMap[q] << "\n";  // prints 2
        printf(sharedPtrMap[r] << "\n";  // oops: prints 1, should print 0
#endif

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: C++ 'bsl::shared_ptr' COMPLIANCE
        //   Note that these tests assume that the default allocator is the
        //   NewDelete allocator; installing a test allocator for the default
        //   will break this test.
        //
        // Plan:  Check that the 'reset' function works, by simply exercising
        //   them on a non-empty shared pointer, and checking that the result
        //   and reference counts are as expected.  Also check that the 'swap'
        //   global function works, using the same test plan as in case 11.
        //
        //   No longer tested because no longer part of this component, but
        //   part of the C++ compliance test, we would check that the
        //   '..._pointer_cast' global functions work, using the same test plan
        //   as in case 9.  For 'get_deleter' we would need to test that
        //   'get_deleter' of objects created empty, or with the default or
        //   test allocator, a factory, or function-like deleters does return a
        //   pointer to the deleter if the correct type is passed as template
        //   argument of 'get_deleter' and 0 otherwise.  Next, we would test
        //   that the comparison operators and functors do return the correct
        //   comparison results.  Finally, we would test the 'make_shared' and
        //   'allocate_shared' functions using the same test plan as in case 5.
        //
        // Testing:
        //   CONCERN: C++ 'bsl::shared_ptr' COMPLIANCE
        // --------------------------------------------------------------------

        if (verbose) printf("\nConcern: 'bsl::shared_ptr' compliance"
                            "\n=====================================\n");

        if (verbose) printf("\nTesting 'reset'."
                            "\n----------------\n");

        bslma::TestAllocator ta(veryVeryVerbose);

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new(ta) TObj(&numDeletes);

            Obj x(p, &ta); const Obj& X=x;

            numAllocations = ta.numAllocations();

            if (veryVerbose) {
                T_ P_(numDeletes) P(X.numReferences())
            }

            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());

            x.reset();

            numDeallocations += 2;
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(1 == numDeletes);
            ASSERT(0 == X.ptr());
            ASSERT(0 == X.numReferences());
        }
        ASSERT(1 == numDeletes);
        ASSERT(numDeallocations == ta.numDeallocations());
        if (veryVerbose) {
            T_ P_(numDeletes)
            P_(numDeallocations)
            P(ta.numDeallocations())
        }

        if (verbose) printf("\nTesting reset(ptr)"
                            "\n------------------\n");

        {
            numDeletes = 0;
            TObj *p = new TObj(&numDeletes);

            Obj x; const Obj& X=x;

            x.reset(p);

            if (veryVerbose) {
                T_ P_(numDeletes) P(X.numReferences())
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());
        }
        ASSERT(1 == numDeletes);
        if (veryVerbose) {
            T_ P(numDeletes);
        }

        if (verbose) printf("\nTesting reset(ptr,deleter)."
                            "\n---------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new(ta) TObj(&numDeletes);
            numAllocations = ta.numAllocations();

            MyTestDeleter deleter(&ta);
            Obj x; const Obj& X=x;

            x.reset(p, deleter);

            ASSERT(numAllocations == ta.numAllocations());

            if (veryVerbose) {
                T_ P_(numDeletes) P(X.numReferences())
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());
        }
        ASSERT(1 == numDeletes);
        ASSERT(++numDeallocations == ta.numDeallocations());
        if (veryVerbose) {
            T_ P_(numDeletes) P_(numDeallocations) P(ta.numDeallocations())
        }

        if (verbose) printf("\nTesting 'reset(source, ptr)'."
                            "\n-----------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new(ta) TObj(&numDeletes);
            Obj x(p,&ta, 0); const Obj& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());

            bsl::shared_ptr<double> y; const bsl::shared_ptr<double>& Y=y;
            double dummy;

            y.reset(X, &dummy);
            ASSERT(&dummy == Y.ptr());
            ASSERT(2 == X.numReferences());
            ASSERT(2 == Y.numReferences());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);

            int dummy2;
            bsl::shared_ptr<int> z;
            const bsl::shared_ptr<int>& Z=z;
            z.loadAlias(Y, &dummy2);
            ASSERT(&dummy2 == Z.ptr());
            ASSERT(3 == Z.numReferences());
            ASSERT(3 == Y.numReferences());
            ASSERT(&dummy == Y.ptr());
            ASSERT(3 == X.numReferences());
            ASSERT(p == X.ptr());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);

        }
        ASSERT(++numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);
        if (veryVerbose) {
            T_ P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }

        if (verbose) printf("\nTesting global 'swap' function."
                            "\n-------------------------------\n");

        bsls::Types::Int64 numDeletes1 = 0;
        numDeletes = 0;
        {
            MyTestObject *p1 = new MyTestObject(&numDeletes);
            MyTestObject *p2 = new MyTestObject(&numDeletes1);
            Obj x(p1); const Obj &X = x;

            ASSERT(p1 == X.ptr());
            ASSERT(1 == X.numReferences());

            ASSERT(0 == numDeletes);
            ASSERT(0 == numDeletes1);
            {
                Obj y(p2); const Obj &Y = y;
                ASSERT(p2 == Y.ptr());
                ASSERT(1 == Y.numReferences());
                ASSERT(0 == numDeletes);
                ASSERT(0 == numDeletes1);

                swap(x, y);

                ASSERT(p2 == X.ptr());
                ASSERT(p1 == Y.ptr());
                ASSERT(0 == numDeletes);
                ASSERT(0 == numDeletes1);
                ASSERT(1 == X.numReferences());
                ASSERT(1 == Y.numReferences());
            }
            ASSERT(p2 == X.ptr());
            ASSERT(1 == X.numReferences());
            ASSERT(1 == numDeletes);
            ASSERT(0 == numDeletes1);
        }
        ASSERT(1 == numDeletes);
        ASSERT(1 == numDeletes1);

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: SHARED PTR IN-PLACE INSIDE 'bdef_Function' OBJECT
        //
        // Plan:  Simply instantiate a 'bsl::shared_ptr' to a function object
        //   inside a 'bdef_Function' object and make sure that the
        //   function object is created in-place.  This can even be achieved
        //   without creating a shared pointer, from the type only, using the
        //   'bdef_FunctionUtil::IsInplace' utility.
        //
        // Testing:
        //   CONCERN: Shared ptr in-place inside 'bdef_Function' object
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nConcern: Shared ptr in-place in 'bdef_Function'"
                   "\n===============================================\n");

#if 0  // TBD waiting on bdef 0> bsl function conversion
        typedef bsl::shared_ptr<MyTestFunctor>  SharedFunctor;

        ASSERT(1 == bdef_FunctionUtil::IsInplace<SharedFunctor>::VALUE);
#endif
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING conversion to bslma::ManagedPtr
        //
        // Plan: TBD
        //
        // Testing:
        //   bslma::ManagedPtr<TYPE> managedPtr() const;
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTesting conversion to bslma::ManagedPtr"
                   "\n======================================\n");

        bslma::TestAllocator ta(veryVeryVerbose);

        bslma::ManagedPtr<MyPDTestObject> mp;
        bsl::shared_ptr<MyPDTestObject>  sp(mp);

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new(ta) TObj(&numDeletes);

            Obj x(p, &ta, 0); const Obj& X=x;
            numAllocations = ta.numAllocations();

            if (veryVerbose) {
                T_ T_ P_(numDeletes); P(X.numReferences());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());

            {
                bslma::ManagedPtr<TObj> y(X.managedPtr());
                const bslma::ManagedPtr<TObj>& Y=y;

                ASSERT(0 == numDeletes);
                ASSERT(p == X.ptr());
                ASSERT(2 == X.numReferences());
                ASSERT(p == Y.ptr());
                ASSERT(numAllocations == ta.numAllocations());
                ASSERT(numDeallocations == ta.numDeallocations());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT(++numDeallocations == ta.numDeallocations());

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new(ta) TObj(&numDeletes);

            Obj x(p, &ta, 0); const Obj& X=x;
            numAllocations = ta.numAllocations();

            if (veryVerbose) {
                T_ T_ P_(numDeletes); P(X.numReferences());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());

            bslma::ManagedPtr<TObj> y(X.managedPtr());
            const bslma::ManagedPtr<TObj>& Y=y;

            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(2 == X.numReferences());
            ASSERT(p == Y.ptr());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());

            x.load((TObj*)0);

            ASSERT(0 == numDeletes);
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT(numAllocations == ta.numAllocations());
        ASSERT(++numDeallocations == ta.numDeallocations());

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'swap'
        //
        // Plan: TBD
        //
        // Testing:
        //   void swap(bsl::shared_ptr<OTHER> &src)
        // --------------------------------------------------------------------
        if (verbose) printf("\nTesting 'swap'"
                            "\n==============\n");

        if (verbose) printf("\tWith default allocator.\n");

        bslma::TestAllocator ta(veryVeryVerbose);

        bsls::Types::Int64 numDeletes1 = 0;
        numDeletes = 0;
        {
            MyTestObject *p1 = new MyTestObject(&numDeletes);
            MyTestObject *p2 = new MyTestObject(&numDeletes1);
            Obj x(p1); const Obj &X = x;

            ASSERT(p1 == X.ptr());
            ASSERT(1 == X.numReferences());

            ASSERT(0 == numDeletes);
            ASSERT(0 == numDeletes1);
            {
                Obj y(p2); const Obj &Y = y;
                ASSERT(p2 == Y.ptr());
                ASSERT(1 == Y.numReferences());
                ASSERT(0 == numDeletes);
                ASSERT(0 == numDeletes1);

                x.swap(y);

                ASSERT(p2 == X.ptr());
                ASSERT(p1 == Y.ptr());
                ASSERT(0 == numDeletes);
                ASSERT(0 == numDeletes1);
                ASSERT(1 == X.numReferences());
                ASSERT(1 == Y.numReferences());
            }
            ASSERT(p2 == X.ptr());
            ASSERT(1 == X.numReferences());
            ASSERT(1 == numDeletes);
            ASSERT(0 == numDeletes1);
        }
        ASSERT(1 == numDeletes);
        ASSERT(1 == numDeletes1);

        if (verbose) printf("\tWith mix of deleters.\n");;

        bslma::TestAllocator da(veryVeryVerbose);
        bslma::DefaultAllocatorGuard defaultGuard(&da);

        // WARNING:  Installing a test allocator as the default means that
        //    bsl::shared_ptr<T> x(new T());
        // will break, as it tries to delete the object using the installed
        // default allocator (i.e., the test allocator) and not the new/delete
        // allocator.  Therefore, from then on in this test case, whenever
        // using the expression 'new T()', should be replaced by 'new(da) T()'.

        numDeletes1 = 0;
        numDeletes = 0;
        {
            MyTestObject *p1 = new(da) MyTestObject(&numDeletes);
            MyTestObject *p2 = new(ta) MyTestObject(&numDeletes1);
            Obj x(p1); const Obj &X = x;

            ASSERT(p1 == X.ptr());
            ASSERT(1 == X.numReferences());

            ASSERT(0 == numDeletes);
            ASSERT(0 == numDeletes1);
            {
                Obj y(p2, &ta); const Obj &Y = y;
                ASSERT(p2 == Y.ptr());
                ASSERT(1 == Y.numReferences());
                ASSERT(0 == numDeletes);
                ASSERT(0 == numDeletes1);

                x.swap(y);

                ASSERT(p2 == X.ptr());
                ASSERT(p1 == Y.ptr());
                ASSERT(0 == numDeletes);
                ASSERT(0 == numDeletes1);
                ASSERT(1 == X.numReferences());
                ASSERT(1 == Y.numReferences());
            }
            ASSERT(p2 == X.ptr());
            ASSERT(1 == X.numReferences());
            ASSERT(1 == numDeletes);
            ASSERT(0 == numDeletes1);
        }
        ASSERT(1 == numDeletes);
        ASSERT(1 == numDeletes1);

        numDeletes1 = 0;
        numDeletes = 0;
        {
            MyTestObject *p1 = new(da) MyTestObject(&numDeletes);
            MyTestObject *p2 = new(ta) MyTestObject(&numDeletes1);
            MyTestDeleter deleter(&ta);
            Obj x(p1); const Obj &X = x;

            ASSERT(p1 == X.ptr());
            ASSERT(1 == X.numReferences());

            ASSERT(0 == numDeletes);
            ASSERT(0 == numDeletes1);
            {
                Obj y(p2, deleter, &da); const Obj &Y = y;
                ASSERT(p2 == Y.ptr());
                ASSERT(1 == Y.numReferences());
                ASSERT(0 == numDeletes);
                ASSERT(0 == numDeletes1);

                x.swap(y);

                ASSERT(p2 == X.ptr());
                ASSERT(p1 == Y.ptr());
                ASSERT(0 == numDeletes);
                ASSERT(0 == numDeletes1);
                ASSERT(1 == X.numReferences());
                ASSERT(1 == Y.numReferences());
            }
            ASSERT(p2 == X.ptr());
            ASSERT(1 == X.numReferences());
            ASSERT(1 == numDeletes);
            ASSERT(0 == numDeletes1);
        }
        ASSERT(1 == numDeletes);
        ASSERT(1 == numDeletes1);

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'PtrLess<TYPE>'
        //   Test that 'bslstl::SharedPtrUtil::PtrLess' is a functor that
        //   compares shared pointers according to their 'ptr' values.
        //
        // Plan: Create a functor 'LT' and check that it does compare suitably
        //   created and ordered shared pointers to either 'MyTestObject' or a
        //   derived type.
        //
        // Testing:
        //   struct bslstl::SharedPtrUtil::PtrLess<TYPE>;
        // --------------------------------------------------------------------

        if (verbose) printf("\tTesting 'bslstl::SharedPtrUtil::PtrLess'\n"
                            "\t--------------------------------------\n");

#if 0
        bslma::TestAllocator ta(veryVeryVerbose);

        bslstl::SharedPtrUtil::PtrLess<MyTestObject> LT;
        {
            MyTestDerivedObject *pd = new(ta) MyTestDerivedObject(&numDeletes);
            MyTestDerivedObject *qd = new(ta) MyTestDerivedObject(&numDeletes);
            if (qd < pd) { bsl::swap(pd, qd); }
            ASSERT(pd < qd);

            TObj *p = pd;
            TObj *q = qd;
            if (q < p) { bsl::swap(p, q); }
            ASSERT(p  < q);
            ASSERT(pd < q);
            ASSERT(p  < qd);

            bsl::shared_ptr<MyTestDerivedObject> xd(pd, &ta);
            const Obj& XD = xd;

            bsl::shared_ptr<MyTestDerivedObject> yd(qd, &ta);
            const Obj& YD = yd;

            Obj x(XD);  const Obj& X = x;
            Obj y(YD);  const Obj& Y = y;

            if (veryVerbose)
                bsl::printf("\tFunctor comparisons with self." << bsl::endl;

#if 0  // Uncomment to generate compilation errors.
            ASSERT(!(X  <  X));
            ASSERT(!(X  < XD));
            ASSERT(!(XD < X));
            ASSERT(!(XD < XD));
#endif

#if 0
            bsl::map<Obj, int> sharedPtrMap;
            sharedPtrMap[X] = 1;
#endif

            ASSERT(!LT(X,  X));
            ASSERT(!LT(X,  XD));
            ASSERT(!LT(XD, X));
            ASSERT(!LT(XD, XD));

#if 0
            {
                typedef bsl::map<Obj, int, bslstl::SharedPtrUtil::PtrLess<Obj> >
                   Map;
                Map map1;
                map1.insert(bsl::make_pair(X, 1));
                map1.insert(bsl::make_pair(Y, 2));
                Map::iterator it1 = map1.begin();
                ASSERT(it1->first == X && it1->second == 1);
                ++it1;
                ASSERT(it1->first == Y && it1->second == 2);
            }
#endif

            {
                typedef bsl::map<Obj, int, bsl::shared_ptrLess> Map;
                Map map1;
                map1.insert(bsl::make_pair(X, 1));
                map1.insert(bsl::make_pair(Y, 2));
                Map::iterator it1 = map1.begin();
                ASSERT(it1->first == X && it1->second == 1);
                ++it1;
                ASSERT(it1->first == Y && it1->second == 2);
            }

            if (veryVerbose)
                  printf("\tFunctor comparisons with other.\n");

#if 0  // Uncomment to generate compilation errors.
            ASSERT(X  < Y));
            ASSERT(XD < Y));
            ASSERT(X  < YD));
            ASSERT(XD < YD));
#endif

            ASSERT(LT(X,  Y));
            ASSERT(LT(XD, Y));
            ASSERT(LT(X,  YD));
            ASSERT(LT(XD, YD));
        }
#endif

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING
        //   Test that 'createInplaceUninitializedBuffer' creates a buffer of
        //   the specified size, properly aligned, and that the buffer is
        //   deallocated properly.
        //
        // Plan:  For every size between 1 and 5 times the maximal alignment,
        //   create a shared pointer to a buffer of this size using
        //   'createInplaceUninitializedBuffer', and verify that the returned
        //   pointer is at least naturally aligned, that we can write into that
        //   buffer, and that the allocated buffer has at least the requested
        //   size.
        //
        // Testing:
        //   bsl::shared_ptr<char> createInplaceUninitializedBuffer(...)
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTesting 'createInplaceUninitializedBuffer'"
                   "\n==========================================\n");

        static const char EXP[] = "createInplaceUninitializedBuffer";

        bslma::TestAllocator ta(veryVeryVerbose);

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();

        if (verbose) {
            printf("\nSizes of InplaceRepImpl<TYPE>:"
                   "\n------------------------------\n");

            enum {
                ONE_ALIGN = 1,
                TWO_ALIGN = (bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT > 2)
                                     ? 2
                                     : bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT,
                FOUR_ALIGN = (bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT > 4)
                                     ? 4
                                     : bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT,
                EIGHT_ALIGN = (bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT > 8)
                                     ? 8
                                     : bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT
            };

            P(sizeof(bslma::SharedPtrInplaceRep<
                                bsls::AlignmentToType<ONE_ALIGN>::Type[1] >));
            P(sizeof(bslma::SharedPtrInplaceRep<
                                bsls::AlignmentToType<TWO_ALIGN>::Type[1] >));
            P(sizeof(bslma::SharedPtrInplaceRep<
                                bsls::AlignmentToType<ONE_ALIGN>::Type[3] >));
            P(sizeof(bslma::SharedPtrInplaceRep<
                                bsls::AlignmentToType<FOUR_ALIGN>::Type[1] >));
            P(sizeof(bslma::SharedPtrInplaceRep<
                                bsls::AlignmentToType<ONE_ALIGN>::Type[5] >));
            P(sizeof(bslma::SharedPtrInplaceRep<
                                bsls::AlignmentToType<TWO_ALIGN>::Type[3] >));
            P(sizeof(bslma::SharedPtrInplaceRep<
                                bsls::AlignmentToType<ONE_ALIGN>::Type[7] >));
            P(sizeof(bslma::SharedPtrInplaceRep<
                               bsls::AlignmentToType<EIGHT_ALIGN>::Type[1] >));
            P(sizeof(bslma::SharedPtrInplaceRep<
                                bsls::AlignmentToType<ONE_ALIGN>::Type[9] >));
            P(sizeof(bslma::SharedPtrInplaceRep<
                                bsls::AlignmentToType<TWO_ALIGN>::Type[5] >));
            P(sizeof(bslma::SharedPtrInplaceRep<
                                bsls::AlignmentToType<ONE_ALIGN>::Type[11] >));
            P(sizeof(bslma::SharedPtrInplaceRep<
                                bsls::AlignmentToType<FOUR_ALIGN>::Type[3] >));
            P(sizeof(bslma::SharedPtrInplaceRep<
                                bsls::AlignmentToType<ONE_ALIGN>::Type[13] >));
            P(sizeof(bslma::SharedPtrInplaceRep<
                                bsls::AlignmentToType<TWO_ALIGN>::Type[7] >));
            P(sizeof(bslma::SharedPtrInplaceRep<
                                bsls::AlignmentToType<ONE_ALIGN>::Type[15] >));
        }

        if (verbose)
            printf("\nTesting 'createInplaceUninitializedBuffer'."
                   "\n-------------------------------------------\n");

        for (int size = 1;
             size < 5 * bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;
             ++size) {
            {
                bsl::shared_ptr<char> x;
                const bsl::shared_ptr<char>& X = x;

                x = bslstl::SharedPtrUtil::createInplaceUninitializedBuffer(
                                                                    size, &ta);
                strncpy(X.ptr(), EXP, size);

                static const int ALLOC_SIZE =
                    sizeof(bslma::SharedPtrInplaceRep<char>) + size - 1;
                LOOP_ASSERT(size, ++numAllocations == ta.numAllocations());
                LOOP_ASSERT(size, ALLOC_SIZE <= ta.lastAllocatedNumBytes());

                LOOP_ASSERT(size, X);
                LOOP_ASSERT(size, 0 == strncmp(EXP, X.ptr(), size));

                int alignment =
                         bsls::AlignmentUtil::calculateAlignmentFromSize(size);
                int alignmentOffset =
                                 bsls::AlignmentUtil::calculateAlignmentOffset(
                                                           X.ptr(), alignment);
                LOOP3_ASSERT(size, alignment, alignmentOffset,
                              0 == alignmentOffset);

                void* repAddr = ta.lastAllocatedAddress();
                int repAllocSize = ta.lastAllocatedNumBytes();
                LOOP4_ASSERT(repAddr, repAllocSize, (void*) X.ptr(), size,
                             (char*) repAddr + repAllocSize >= X.ptr() + size);

                if (veryVerbose) {
                    P_(size);
                    P_(ta.numAllocations());
                    P(ta.lastAllocatedNumBytes());
                    P_(alignment);
                    P((void *)X.ptr());
                    P(bsls::AlignmentUtil::calculateAlignmentOffset(X.ptr(),
                                                               alignment));
                }
            }
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING explicit cast 'load' operations
        //   Test that explicit cast operations properly loads the object
        //
        // Plan: TBD
        //
        // Testing:
        //  bsl::shared_ptr<TARGET> dynamicCast(bsl::shared_ptr<SOURCE> ..
        //  bsl::shared_ptr<TARGET> staticCast(const bsl::shared_ptr<SOURCE>& )
        //  bsl::shared_ptr<TARGET> constCast(const bsl::shared_ptr<SOURCE>& )
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting explicit cast operations"
                            "\n================================\n");

        if (verbose) printf("\nTesting 'dynamicCast'"
                            "\n---------------------\n");

        bslma::TestAllocator ta(veryVeryVerbose);

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            MyTestDerivedObject *p = new(ta) MyTestDerivedObject(&numDeletes);
            Obj x(p,&ta, 0); const Obj& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(static_cast<MyTestObject*>(p) == X.ptr());
            ASSERT(1 == X.numReferences());

            bsl::shared_ptr<MyTestDerivedObject> y;
            const bsl::shared_ptr<MyTestDerivedObject>& Y=y;

            {
                // This inner block necessary against Sun CC bug, the lifetime
                // of the temporary copied into y would otherwise pollute the
                // Y.numReferences below.
                y = bslstl::SharedPtrUtil::dynamicCast<MyTestDerivedObject>(X);
            }
            if (veryVerbose) {
                P_(Y.ptr());
                P_(X.numReferences());
                P(Y.numReferences());
            }
            ASSERT(p == Y.ptr());
            ASSERT(2 == X.numReferences());
            ASSERT(2 == Y.numReferences());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);

        if (verbose) printf("\nTesting 'staticCast'"
                            "\n--------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            MyTestDerivedObject *p = new(ta) MyTestDerivedObject(&numDeletes);
            bsl::shared_ptr<MyTestDerivedObject> x(p,&ta, 0);
            const bsl::shared_ptr<MyTestDerivedObject>& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());

            Obj y(bslstl::SharedPtrUtil::staticCast<TObj>(X)); const Obj& Y=y;

            ASSERT(static_cast<MyTestObject*>(p) == Y.ptr());
            ASSERT(2 == X.numReferences());
            ASSERT(2 == Y.numReferences());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);

        if (verbose) printf("\nTesting 'constCast'"
                            "\n-------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            // Construct ConstObj with a nil deleter.
            // This exposes a former const-safety bug.
            bsls::Types::Int64 counter = 0;
            const MyTestObject V(&counter);
            ConstObj x(&V, bslstl::SharedPtrNilDeleter(), 0);
        }
        {
            numDeletes = 0;
            TObj *p = new(ta) TObj(&numDeletes);
            ConstObj x(p,&ta, 0); const ConstObj& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());

            Obj y(bslstl::SharedPtrUtil::constCast<TObj>(X)); const Obj& Y=y;

            ASSERT(const_cast<TObj*>(p) == Y.ptr());
            ASSERT(2 == X.numReferences());
            ASSERT(2 == Y.numReferences());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING explicit cast 'load' operations
        //
        //
        // Plan: TBD
        //
        // Testing:
        //   void loadAlias(const bsl::shared_ptr<OTHER>& target, TYPE *)
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting alias operations"
                            "\n========================\n");

        if (verbose) printf("\nTesting 'loadAlias' (unset target)"
                            "\n----------------------------------\n");

        bslma::TestAllocator ta(veryVeryVerbose);

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new(ta) TObj(&numDeletes);
            Obj x(p, &ta, 0); const Obj& X=x;

            numAllocations = ta.numAllocations();
            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());

            bsl::shared_ptr<double> y; const bsl::shared_ptr<double>& Y=y;

            double dummy;
            y.loadAlias(X, &dummy);
            ASSERT(&dummy == Y.ptr());
            ASSERT(2 == Y.numReferences());
            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(2 == X.numReferences());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'loadAlias' (unset target and src)"
                            "\n------------------------------------------\n");
        {
            bsl::shared_ptr<MyTestObject2> x;
            const bsl::shared_ptr<MyTestObject2>& X=x;

            ASSERT(0 == X.ptr());
            ASSERT(0 == X.numReferences());

            bsl::shared_ptr<double> y; const bsl::shared_ptr<double>& Y=y;

            double dummy;
            y.loadAlias(X, &dummy);

            ASSERT(0 == Y.ptr());
            ASSERT(0 == Y.numReferences());
        }

        if (verbose) printf("\nTesting 'loadAlias' (partially unset)"
                            "\n-------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new(ta) TObj(&numDeletes);
            Obj x(p, &ta, 0); const Obj& X=x;

            numAllocations = ta.numAllocations();
            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());

            double dummy;
            bsl::shared_ptr<double> y(X, &dummy);
            const bsl::shared_ptr<double>& Y=y;
            ASSERT(2 == X.numReferences());
            ASSERT(2 == Y.numReferences());

            y.loadAlias(X, (double*)0);
            ASSERT(0 == Y.ptr());
            ASSERT(0 == Y.numReferences());
            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'loadAlias'(set)"
                            "\n------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new(ta) TObj(&numDeletes);
            Obj x(p,&ta, 0); const Obj& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());

            bsl::shared_ptr<double> y; const bsl::shared_ptr<double>& Y=y;
            double dummy;

            y.loadAlias(X, &dummy);
            ASSERT(&dummy == Y.ptr());
            ASSERT(2 == X.numReferences());
            ASSERT(2 == Y.numReferences());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);

            int dummy2;
            bsl::shared_ptr<int> z;
            const bsl::shared_ptr<int>& Z=z;
            z.loadAlias(Y, &dummy2);
            ASSERT(&dummy2 == Z.ptr());
            ASSERT(3 == Z.numReferences());
            ASSERT(3 == Y.numReferences());
            ASSERT(&dummy == Y.ptr());
            ASSERT(3 == X.numReferences());
            ASSERT(p == X.ptr());
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);

        }
        ASSERT(++numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING explicit cast constructors
        //   Test that the aliasing constructor work as expected
        //
        // Plan: TBD
        //
        // Testing:
        //   bsl::shared_ptr(const bsl::shared_ptr<TYPE>& alias, TYPE *object);
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTesting explicit cast constructors"
                   "\n==================================\n");

        if (verbose) printf("\nTesting \"alias\" constructor"
                            "\n-----------------------------\n");

        bslma::TestAllocator ta(veryVeryVerbose);

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            MyTestDerivedObject *p = new(ta) MyTestDerivedObject(&numDeletes);
            Obj x(p,&ta, 0); const Obj& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());

            double dummy;
            bsl::shared_ptr<double> y(X, &dummy);
            const bsl::shared_ptr<double>& Y=y;

            ASSERT(&dummy == Y.ptr());
            ASSERT(2 == X.numReferences());
            ASSERT(2 == Y.numReferences());
            ASSERT(numAllocations   == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);

        if (verbose) printf("\nTesting \"alias\" constructor (nil object)"
                            "\n------------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            MyTestDerivedObject *p = new(ta) MyTestDerivedObject(&numDeletes);
            Obj x(p,&ta, 0); const Obj& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());

            bsl::shared_ptr<double> y(X, 0);
            const bsl::shared_ptr<double>& Y=y;

            ASSERT(0 == Y.ptr());
            ASSERT(1 == X.numReferences());
            ASSERT(0 == Y.numReferences());
            ASSERT(numAllocations   == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());
        ASSERT(1 == numDeletes);

        if (verbose) printf("\nTesting \"alias\" constructor (unset target)"
                            "\n--------------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            Obj x; const Obj& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(0 == X.ptr());
            ASSERT(0 == X.numReferences());

            double dummy;
            bsl::shared_ptr<double> y(X, &dummy);
            const bsl::shared_ptr<double>& Y=y;

            ASSERT(0 == Y.ptr());
            ASSERT(0 == X.numReferences());
            ASSERT(0 == Y.numReferences());
            ASSERT(numAllocations   == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(numDeallocations == ta.numDeallocations());

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            Obj x(new (ta) TObj(&numDeletes), &ta, 0); const Obj& X = x;

            numAllocations = ta.numAllocations();
            ASSERT(0 != X.ptr());
            ASSERT(1 == X.numReferences());

            bsl::shared_ptr<double> y(X, (double *)0);
            const bsl::shared_ptr<double>& Y=y;

            ASSERT(0 == Y.ptr());
            ASSERT(1 == X.numReferences());
            ASSERT(0 == Y.numReferences());
            ASSERT(numAllocations   == ta.numAllocations());
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(0 == numDeletes);
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'load'
        //   Note that these tests assume that the default allocator is the
        //   NewDelete allocator; installing a test allocator for the default
        //   will break this test.  (This actually affects only one 'load' call
        //   in practice.)
        //
        // Plan: TBD
        //
        // Testing:
        //   void load(OTHER *ptr, bslma::Allocator *allocator=0)
        //   void load(OTHER *ptr, const DELETER&, bslma::Allocator *)
        // --------------------------------------------------------------------
        if (verbose)
            printf("\nTesting load of null ptr(on empty object)"
                   "\n-----------------------------------------\n");

        bslma::TestAllocator ta(veryVeryVerbose);

        numDeallocations = ta.numDeallocations();
        {
            Obj x; const Obj& X=x;
            x.load((TObj*)0);
            ASSERT(0 == X.ptr());
            ASSERT(0 == X.numReferences());

            numAllocations = ta.numAllocations();

            Obj y; const Obj& Y=y;
            y.load((TObj*)0, &ta);
            ASSERT(0 == Y.ptr());
            ASSERT(0 == Y.numReferences());
            ASSERT(numAllocations == ta.numAllocations());

            Obj z; const Obj& Z=z;
            z.load((TObj*)0, &ta, &ta);
            ASSERT(0 == Z.ptr());
            ASSERT(0 == Z.numReferences());
            ASSERT(numAllocations == ta.numAllocations());
        }
        ASSERT(numDeallocations == ta.numDeallocations());

        if (verbose)
            printf("\nTesting load of null ptr(on non-empty object)"
                   "\n---------------------------------------------\n");
        {
            numDeletes = 0;
            Obj x(new (ta) TObj(&numDeletes), &ta, 0); const Obj& X=x;
            numAllocations = ta.numAllocations();
            numDeallocations = ta.numDeallocations();
            ASSERT(0 == numDeletes);
            x.load((TObj*)0);
            ASSERT(1 == numDeletes);
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(++numDeallocations == ta.numDeallocations());
            ASSERT(0 == X.ptr());
            ASSERT(0 == X.numReferences());
        }
        ASSERT(numAllocations == ta.numAllocations());
        ASSERT(numDeallocations == ta.numDeallocations());

        {
            numDeletes = 0;
            Obj y(new (ta) TObj(&numDeletes), &ta, 0); const Obj& Y=y;
            numAllocations = ta.numAllocations();
            numDeallocations = ta.numDeallocations();
            ASSERT(0 == numDeletes);
            y.load((TObj*)0, &ta);
            ASSERT(1 == numDeletes);
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(++numDeallocations == ta.numDeallocations());
            ASSERT(0 == Y.ptr());
            ASSERT(0 == Y.numReferences());
            ASSERT(numAllocations == ta.numAllocations());
        }
        ASSERT(numAllocations == ta.numAllocations());
        ASSERT(numDeallocations == ta.numDeallocations());

        {
            numDeletes = 0;
            Obj z(new (ta) TObj(&numDeletes), &ta, 0); const Obj& Z=z;
            numAllocations = ta.numAllocations();
            numDeallocations = ta.numDeallocations();
            ASSERT(0 == numDeletes);
            z.load((TObj*)0, &ta, &ta);
            ASSERT(1 == numDeletes);
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(++numDeallocations == ta.numDeallocations());
            ASSERT(0 == Z.ptr());
            ASSERT(0 == Z.numReferences());
            ASSERT(numAllocations == ta.numAllocations());
        }
        ASSERT(numAllocations == ta.numAllocations());
        ASSERT(numDeallocations == ta.numDeallocations());

        if (verbose)
            printf("\nTesting load of non-null ptr (on non-empty object)"
                   "\n--------------------------------------------------\n");

        {
            numDeletes = 0;
            Obj x(new (ta) TObj(&numDeletes), &ta, 0); const Obj& X=x;
            numAllocations = ta.numAllocations();
            numDeallocations = ta.numDeallocations();
            ASSERT(0 == numDeletes);
            TObj *p = new TObj(&numDeletes);
            x.load(p);
            ASSERT(1 == numDeletes);
            ASSERT(numAllocations == ta.numAllocations());
            ASSERT(++numDeallocations == ta.numDeallocations());
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());
        }
        ASSERT(2 == numDeletes);
        ASSERT(numAllocations == ta.numAllocations());
        ASSERT(numDeallocations == ta.numDeallocations());

        {
            numDeletes = 0;
            Obj y(new (ta) TObj(&numDeletes), &ta, 0); const Obj& Y=y;
            numAllocations = ta.numAllocations();
            numDeallocations = ta.numDeallocations();
            ASSERT(0 == numDeletes);
            TObj *p = new(ta) TObj(&numDeletes);
            numAllocations = ta.numAllocations();
            y.load(p, &ta);
            ASSERT(1 == numDeletes);
            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(++numDeallocations == ta.numDeallocations());
            ASSERT(p == Y.ptr());
            ASSERT(1 == Y.numReferences());
        }
        ASSERT(2 == numDeletes);
        ASSERT(numAllocations == ta.numAllocations());
        ASSERT((numDeallocations+2) == ta.numDeallocations());

        {
            numDeletes = 0;
            Obj z(new (ta) TObj(&numDeletes), &ta, 0); const Obj& Z=z;
            numAllocations = ta.numAllocations();
            numDeallocations = ta.numDeallocations();
            ASSERT(0 == numDeletes);
            TObj *p = new(ta) TObj(&numDeletes);
            numAllocations = ta.numAllocations();
            z.load(p, &ta, &ta);
            ASSERT(1 == numDeletes);
            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(++numDeallocations == ta.numDeallocations());
            ASSERT(p == Z.ptr());
            ASSERT(1 == Z.numReferences());
            ASSERT(numAllocations == ta.numAllocations());
        }
        ASSERT(2 == numDeletes);
        ASSERT(numAllocations == ta.numAllocations());
        ASSERT((numDeallocations+2) == ta.numDeallocations());

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'createInplace'
        //
        // Concerns:
        //
        // Plan: TBD
        //
        // Testing:
        //   void createInplace(bslma::Allocator *allocator=0);
        //   void createInplace(bslma::Allocator *, const A1& a1)
        //   void createInplace(bslma::Allocator *, const A1& a1, ..&a2);
        //   void createInplace(bslma::Allocator *, const A1& a1,...a3);
        //   void createInplace(bslma::Allocator *, const A1& a1,...a4);
        //   void createInplace(bslma::Allocator *, const A1& a1,...a5);
        //   void createInplace(bslma::Allocator *, const A1& a1,...a6);
        //   void createInplace(bslma::Allocator *, const A1& a1,...a7);
        //   void createInplace(bslma::Allocator *, const A1& a1,...a8);
        //   void createInplace(bslma::Allocator *, const A1& a1,...a9);
        //   void createInplace(bslma::Allocator *, const A1& a1,...a10);
        //   void createInplace(bslma::Allocator *, const A1& a1,...a11);
        //   void createInplace(bslma::Allocator *, const A1& a1,...a12);
        //   void createInplace(bslma::Allocator *, const A1& a1,...a13);
        //   void createInplace(bslma::Allocator *, const A1& a1,...a14);
        // --------------------------------------------------------------------
        if (verbose) printf("\nTesting 'createInplace'\n"
                            "\n=======================\n");

        static const MyTestArg1 V1(1);
        static const MyTestArg2 V2(20);
        static const MyTestArg3 V3(23);
        static const MyTestArg4 V4(44);
        static const MyTestArg5 V5(66);
        static const MyTestArg6 V6(176);
        static const MyTestArg7 V7(878);
        static const MyTestArg8 V8(8);
        static const MyTestArg9 V9(912);
        static const MyTestArg10 V10(102);
        static const MyTestArg11 V11(111);
        static const MyTestArg12 V12(333);
        static const MyTestArg13 V13(712);
        static const MyTestArg14 V14(1414);

        if (verbose) printf("\nTesting 'createInplace' with 1 argument"
                            "\n---------------------------------------\n");

        bslma::TestAllocator ta(veryVeryVerbose);

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bsl::shared_ptr<MyInplaceTestObject> x;
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;
            static const MyInplaceTestObject EXP(V1);

            x.createInplace(&ta, V1);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.ptr()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'createInplace' with 2 arguments"
                            "\n----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bsl::shared_ptr<MyInplaceTestObject> x;
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;
            static const MyInplaceTestObject EXP(V1, V2);

            x.createInplace(&ta, V1, V2);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.ptr()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'createInplace' with 3 arguments"
                            "\n----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bsl::shared_ptr<MyInplaceTestObject> x;
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3);

            x.createInplace(&ta, V1, V2, V3);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.ptr()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'createInplace' with 4 arguments"
                            "\n----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bsl::shared_ptr<MyInplaceTestObject> x;
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3, V4);

            x.createInplace(&ta, V1, V2, V3, V4);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.ptr()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'createInplace' with 5 arguments"
                            "\n----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bsl::shared_ptr<MyInplaceTestObject> x;
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5);

            x.createInplace(&ta, V1, V2, V3, V4, V5);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.ptr()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'createInplace' with 6 arguments"
                            "\n----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bsl::shared_ptr<MyInplaceTestObject> x;
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6);

            x.createInplace(&ta, V1, V2, V3, V4, V5, V6);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.ptr()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'createInplace' with 7 arguments"
                            "\n----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bsl::shared_ptr<MyInplaceTestObject> x;
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7);

            x.createInplace(&ta, V1, V2, V3, V4, V5, V6,V7);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.ptr()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'createInplace' with 9 arguments"
                            "\n----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bsl::shared_ptr<MyInplaceTestObject> x;
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                    V8);

            x.createInplace(&ta, V1, V2, V3, V4, V5, V6,V7, V8);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.ptr()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'createInplace' with 10 arguments"
                            "\n-----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bsl::shared_ptr<MyInplaceTestObject> x;
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                    V8, V9);

            x.createInplace(&ta, V1, V2, V3, V4, V5, V6,V7, V8, V9);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.ptr()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'createInplace' with 2 arguments"
                            "\n----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bsl::shared_ptr<MyInplaceTestObject> x;
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                    V8, V9, V10);

            x.createInplace(&ta, V1, V2, V3, V4, V5, V6,V7, V8, V9, V10);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.ptr()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'createInplace' with 11 arguments"
                            "\n----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bsl::shared_ptr<MyInplaceTestObject> x;
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                    V8, V9, V10, V11);

            x.createInplace(&ta, V1, V2, V3, V4, V5, V6,V7, V8, V9, V10, V11);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.ptr()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'createInplace' with 12 arguments"
                            "\n-----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bsl::shared_ptr<MyInplaceTestObject> x;
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                    V8, V9, V10, V11, V12);

            x.createInplace(&ta, V1, V2, V3, V4, V5, V6,V7, V8, V9, V10, V11,
                    V12);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.ptr()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'createInplace' with 13 arguments"
                            "\n-----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bsl::shared_ptr<MyInplaceTestObject> x;
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                    V8, V9, V10, V11, V12, V13);

            x.createInplace(&ta, V1, V2, V3, V4, V5, V6,V7, V8, V9, V10,
                    V11, V12, V13);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.ptr()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

        if (verbose) printf("\nTesting 'createInplace' with 14 arguments"
                            "\n-----------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            bsl::shared_ptr<MyInplaceTestObject> x;
            const bsl::shared_ptr<MyInplaceTestObject>& X=x;
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                    V8, V9, V10, V11, V12, V13, V14);

            x.createInplace(&ta, V1, V2, V3, V4, V5, V6,V7, V8, V9, V10, V11,
                    V12, V13, V14);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(X);
            ASSERT(EXP == *(X.ptr()));
        }
        ASSERT(++numDeallocations == ta.numDeallocations());

#if 0 // TBD Waiting on a useable 'bind' facility to port to a good level
        if (verbose)
            printf("\nTesting 'createInplace' passing allocator to args"
                   "\n-------------------------------------------------\n");

        bslma::TestAllocator ta0;
        bslma::TestAllocator ta1;
        bslma::TestAllocator ta2;

        bslma::TestAllocator *Z0 = &ta0;
        bslma::TestAllocator *Z1 = &ta1;
        bslma::TestAllocator *Z2 = &ta2;

        bdef_Bind_TestSlotsAlloc::setZ0(Z0);
        bdef_Bind_TestSlotsAlloc::setZ1(Z1);
        bdef_Bind_TestSlotsAlloc::setZ2(Z2);

        bslma::DefaultAllocatorGuard allocGuard(Z0);

        bdef_Bind_TestArgAlloc<1>  VA1(1, Z2);
        bdef_Bind_TestArgAlloc<2>  VA2(2, Z2);
        bdef_Bind_TestArgAlloc<3>  VA3(3, Z2);
        bdef_Bind_TestArgAlloc<4>  VA4(4, Z2);
        bdef_Bind_TestArgAlloc<5>  VA5(5, Z2);
        bdef_Bind_TestArgAlloc<6>  VA6(6, Z2);
        bdef_Bind_TestArgAlloc<7>  VA7(7, Z2);
        bdef_Bind_TestArgAlloc<8>  VA8(8, Z2);
        bdef_Bind_TestArgAlloc<9>  VA9(9, Z2);
        bdef_Bind_TestArgAlloc<10> VA10(10, Z2);
        bdef_Bind_TestArgAlloc<11> VA11(11, Z2);
        bdef_Bind_TestArgAlloc<12> VA12(12, Z2);
        bdef_Bind_TestArgAlloc<13> VA13(13, Z2);
        bdef_Bind_TestArgAlloc<14> VA14(14, Z2);

        bslma::Allocator *ALLOC_SLOTS[] = {
            //   1,  2   3   4   5   6   7   8   9  10  11  12  13  14
            Z0, Z1, Z1, Z1, Z1, Z1, Z1, Z1, Z1, Z1, Z1, Z1, Z1, Z1, Z1
        };

        {
            bsl::shared_ptr<bdef_Bind_TestTypeAlloc> x;
            const bsl::shared_ptr<bdef_Bind_TestTypeAlloc>& X=x;

            const bdef_Bind_TestTypeAlloc EXP(Z0, VA1, VA2, VA3, VA4,
                    VA5, VA6, VA7, VA8, VA9, VA10, VA11, VA12, VA13);

            x.createInplace(Z1, Z1, VA1, VA2, VA3, VA4, VA5, VA6,VA7, VA8, VA9,
                    VA10, VA11, VA12, VA13);

            bdef_Bind_TestSlotsAlloc::resetSlots(Z0);
            x->setSlots(); // should set slots to Z1, as this should be the
                           // allocator used by data members of *(X.ptr())

            ASSERT(X);
            ASSERT(EXP == *(X.ptr()));
            ASSERT(bdef_Bind_TestSlotsAlloc::verifySlots(ALLOC_SLOTS,
                                                         verbose));
        }
#endif
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATORS
        //   Note that these tests assume that the default allocator is the
        //   NewDelete allocator; installing a test allocator for the default
        //   will break this test.
        //
        // Plan: TBD
        //
        // Testing:
        //   bsl::shared_ptr<TYPE>& operator=(const bsl::shared_ptr<TYPE>& r);
        //   bsl::shared_ptr<TYPE>& operator=(const bsl::shared_ptr<OTHER>& r);
        //   bsl::shared_ptr<TYPE>& operator=(std::auto_ptr<OTHER> rhs);
        // --------------------------------------------------------------------
        if (verbose) printf("\nTesting ASSIGNMENT OPERATORS"
                            "\n============================\n");

        if (verbose) printf("\nTesting ASSIGNMENT to empty object"
                            "\n----------------------------------\n");
        {
            Obj x1;
            const Obj& X1 = x1;
            ASSERT(0 == x1.ptr());
            ASSERT(0 == x1.numReferences());

            numDeletes = 0;
            TObj *p = new TObj(&numDeletes);

            Obj x2(p); const Obj& X2=x2;

            if (veryVerbose) {
                P_(numDeletes); P_(X1.numReferences());
                P(X2.numReferences());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X2.ptr());
            ASSERT(1 == X2.numReferences());

            x1 = X2;

            if (veryVerbose) {
                P_(numDeletes); P_(X1.numReferences());
                P(X2.numReferences());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X2.ptr());
            ASSERT(p == X1.ptr());

            ASSERT(2 == X2.numReferences());
            ASSERT(2 == X1.numReferences());
        }
        ASSERT(1 == numDeletes);

        if (verbose) printf("\nTesting ASSIGNMENT of auto_ptr"
                            "\n------------------------------\n");
        {
            Obj x;
            const Obj& X = x;
            ASSERT(0 == x.ptr());
            ASSERT(0 == x.numReferences());

            numDeletes = 0;
            TObj *p = new TObj(&numDeletes);
            std::auto_ptr<TObj> ap(p);

            x = ap;

            if (veryVerbose) {
                P_(numDeletes);        P_(X.numReferences());
                P(ap.get());
            }

            ASSERT(0 == ap.get());
            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());
        }
        ASSERT(1 == numDeletes);

        if (verbose) printf("\nTesting ASSIGNMENT of auto_ptr rvalue"
                            "\n-------------------------------------\n");
        {
            Obj x;
            const Obj& X = x;
            ASSERT(0 == x.ptr());
            ASSERT(0 == x.numReferences());

            numDeletes = 0;

            x = makeAuto(&numDeletes);

            if (veryVerbose) {
                P_(numDeletes);        P_(X.numReferences());
                P(X.get());
            }

            ASSERT(0 == numDeletes);
            ASSERT(0 != X.ptr());
            ASSERT(1 == X.numReferences());
        }
        ASSERT(1 == numDeletes);

        if (verbose) printf("\nTesting ASSIGNMENT of empty object"
                            "\n----------------------------------\n");
        {
            numDeletes = 0;
            TObj *p = new TObj(&numDeletes);

            Obj x1(p); const Obj& X1=x1;

            if (veryVerbose) {
                P_(numDeletes); P_(X1.numReferences());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X1.ptr());
            ASSERT(1 == X1.numReferences());

            Obj x2;
            const Obj& X2 = x2;
            ASSERT(0 == x2.ptr());
            ASSERT(0 == x2.numReferences());

            x1 = X2;

            if (veryVerbose) {
                P_(numDeletes); P_(X1.numReferences());
                P(X2.numReferences());
            }
            ASSERT(1 == numDeletes);
            ASSERT(0 == X2.ptr());
            ASSERT(0 == X1.ptr());

            ASSERT(0 == X2.numReferences());
            ASSERT(0 == X1.numReferences());
        }
        ASSERT(1 == numDeletes);

        if (verbose) printf("\nTesting ASSIGNMENT of loaded object"
                            "\n----------------------------------\n");
        {
            bsls::Types::Int64 numDeletes1 = 0;
            numDeletes = 0;
            TObj *p1 = new TObj(&numDeletes1);

            Obj x1(p1); const Obj& X1=x1;

            if (veryVerbose) {
                P_(numDeletes1); P_(X1.numReferences());
            }
            ASSERT(0 == numDeletes1);
            ASSERT(p1 == X1.ptr());
            ASSERT(1 == X1.numReferences());

            numDeletes = 0;
            TObj *p2 = new TObj(&numDeletes);
            Obj x2(p2);
            const Obj& X2 = x2;

            ASSERT(0 == numDeletes);
            ASSERT(p2 == x2.ptr());
            ASSERT(1 == x2.numReferences());

            x1 = X2;

            if (veryVerbose) {
                P_(numDeletes1); P_(numDeletes); P_(X1.numReferences());
                P(X2.numReferences());
            }

            ASSERT(1 == numDeletes1);
            ASSERT(0 == numDeletes);
            ASSERT(p2 == X2.ptr());
            ASSERT(p2 == X1.ptr());

            ASSERT(2 == X2.numReferences());
            ASSERT(2 == X1.numReferences());
        }
        ASSERT(1 == numDeletes);

        if (verbose) printf("\nTesting ASSIGNMENT to self"
                            "\n--------------------------\n");
        {
            numDeletes = 0;
            TObj *p = new TObj(&numDeletes);

            Obj x1(p); const Obj& X1=x1;

            x1 = X1;

            if (veryVerbose) {
                P_(numDeletes); P_(X1.numReferences());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X1.ptr());
            ASSERT(1 == X1.numReferences());
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTORS
        //   Verify that upon construction the object is properly initialized,
        //   and the reference count is updated.  Check that memory is
        //   deallocated upon object destruction.  Check that no memory is
        //   leaked in the presence of exceptions.
        //   Note that these tests assume that the default allocator is the
        //   NewDelete allocator; installing a test allocator for the default
        //   will break this test.
        //   Note that we do not appear to test construction from a pointer to
        //   some 'OTHER' type, nor the copy constructor, despite being listed
        //   here in the component test plan.
        //
        // Plan: TBD
        //
        // Testing:
        // * bsl::shared_ptr(OTHER *ptr)
        // * bsl::shared_ptr(OTHER *ptr, bslma::Allocator *allocator)
        //   bsl::shared_ptrTYPE *ptr, bslma::SharedPtrRep *rep)
        //   bsl::shared_ptr(OTHER *ptr, DELETER *const& deleter)
        //   bsl::shared_ptr(OTHER *ptr, const DELETER& deleter, ...
        //   bsl::shared_ptr(nullptr_t, const DELETER&, bslma::Allocator * =0);
        //   bsl::shared_ptr(std::auto_ptr<OTHER> autoPtr, bslma::Allocator*=0)
        // * bsl::shared_ptr(const bsl::shared_ptr& original);
        //   bsl::shared_ptr(bslma::SharedPtrRep *);
        // --------------------------------------------------------------------
        if (verbose) printf("\nTesting Constructors and Destructor"
                            "\n===================================\n");

        if (verbose) printf("\nTesting null ptr constructor"
                            "\n----------------------------\n");

        bslma::TestAllocator ta(veryVeryVerbose);

        numAllocations   = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
#if defined(AJM_HAS_RECONCILED_CONFLICTING_BRANCHES)
            Obj w(0); const Obj& W = w;
            ASSERT(0 == W.ptr());
            ASSERT(0 == W.numReferences());
            ASSERT(numAllocations == ta.numAllocations());
#endif // AJM_HAS_RECONCILED_CONFLICTING_BRANCHES

            Obj x(0, &ta); const Obj& X = x;
            ASSERT(0 == X.ptr());
            ASSERT(0 == X.numReferences());
            ASSERT(numAllocations == ta.numAllocations());

            std::auto_ptr<TObj> apY(0);
            Obj y(apY, &ta); const Obj& Y = y;
            ASSERT(0 == Y.ptr());
            ASSERT(0 == Y.numReferences());
            ASSERT(numAllocations == ta.numAllocations());

            Obj y2(makeAuto(), &ta); const Obj& Y2 = y2;
            ASSERT(0 == Y.ptr());
            ASSERT(0 == Y.numReferences());
            ASSERT(numAllocations == ta.numAllocations());

            Obj z(0, &ta, &ta); const Obj& Z = z;
            ASSERT(0 == Z.ptr());
            ASSERT(0 == Z.numReferences());
            ASSERT(numAllocations == ta.numAllocations());
        }
        ASSERT(numDeallocations == ta.numDeallocations());

        if (verbose)
            printf("\nTesting (cast) null ptr constructor"
                   "\n-----------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            Obj w((TObj*)0); const Obj& W = w;
            ASSERT(0 == W.ptr());
            ASSERT(0 == W.numReferences());
            ASSERT(numAllocations == ta.numAllocations());

            Obj x((TObj*)0, &ta); const Obj& X = x;
            ASSERT(0 == X.ptr());
            ASSERT(0 == X.numReferences());
            ASSERT(numAllocations == ta.numAllocations());

            Obj z((TObj*)0, &ta, &ta); const Obj& Z = z;
            ASSERT(0 == Z.ptr());
            ASSERT(0 == Z.numReferences());
            ASSERT(numAllocations == ta.numAllocations());
        }
        ASSERT(numDeallocations == ta.numDeallocations());

        if (verbose)
            printf("\nTesting constructor (with factory)"
                   "\n----------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new (ta) TObj(&numDeletes);
            numAllocations = ta.numAllocations();

            Obj x(p, &ta); const Obj& X = x;
            ASSERT(++numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.numReferences());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT((numDeallocations+2) == ta.numDeallocations());

        if (verbose)
            printf("\nTesting auto_ptr constructor (with allocator)"
                   "\n---------------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj  *p = new TObj(&numDeletes);
            std::auto_ptr<TObj> ap(p);

            numAllocations = ta.numAllocations();

            Obj x(ap, &ta); const Obj& X = x;
            ASSERT(0 == ap.get());
            ASSERT(++numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.numReferences());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT((++numDeallocations) == ta.numDeallocations());

        if (verbose)
            printf("\nTesting auto_ptr rvalue constructor (with allocator)"
                   "\n----------------------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            numAllocations = ta.numAllocations();

            Obj x(makeAuto(&numDeletes), &ta); const Obj& X = x;
            ASSERT(++numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.numReferences());
            }
            ASSERT(0 == numDeletes);
            ASSERT(0 != X.ptr());
            ASSERT(1 == X.numReferences());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT((++numDeallocations) == ta.numDeallocations());

        if (verbose)
            printf("\nTesting constructor (with factory and allocator)"
                   "\n-----------------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new (ta) TObj(&numDeletes);
            numAllocations = ta.numAllocations();

            // Note - not a great test when factor and allocator are the same.

            Obj x(p, &ta, &ta); const Obj& X = x;
            ASSERT(++numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.numReferences());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT(numDeallocations + 2 == ta.numDeallocations());

        if (verbose)
            printf("\nTesting constructor (with deleter object)"
                   "\n-----------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new (ta) TObj(&numDeletes);
            numAllocations = ta.numAllocations();

            MyTestDeleter deleter(&ta);
            Obj x(p, deleter);  const Obj& X = x;
            ASSERT(numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.numReferences());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT(numDeallocations + 1 == ta.numDeallocations());

        if (verbose)
            printf("\nTesting constructor (with deleter function pointer)"
                   "\n---------------------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj testObject(&numDeletes);
            numAllocations = ta.numAllocations();

            Obj x(&testObject, &TestDriver::doNotDelete<TObj>);
            const Obj& X = x;
            ASSERT(numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.numReferences());
            }
            ASSERT(0 == numDeletes);
            ASSERT(&testObject == X.ptr());
            ASSERT(1 == X.numReferences());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT(numDeallocations == ta.numDeallocations());

#if !defined(BSLS_PLATFORM_CMP_IBM)
        if (verbose)
            printf("\nTesting constructor (with deleter function type)"
                   "\n------------------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj testObject(&numDeletes);
            numAllocations = ta.numAllocations();

            Obj x(&testObject, TestDriver::doNotDelete<TObj>);
            const Obj& X = x;
            ASSERT(numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.numReferences());
            }
            ASSERT(0 == numDeletes);
            ASSERT(&testObject == X.ptr());
            ASSERT(1 == X.numReferences());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT(numDeallocations == ta.numDeallocations());
#endif  // BSLS_PLATFORM_CMP_IBM

        if (verbose)
            printf("\nTesting constructor (with deleter and allocator)"
                   "\n------------------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new (ta) TObj(&numDeletes);
            numAllocations = ta.numAllocations();

            MyTestDeleter deleter(&ta);
            Obj x(p, deleter, &ta); const Obj& X = x;
            ASSERT(++numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.numReferences());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT((numDeallocations+2) == ta.numDeallocations());


        if (verbose)
            printf("\nTesting ctor (with function pointer and allocator)"
                   "\n--------------------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj testObject(&numDeletes);
            numAllocations = ta.numAllocations();

            Obj x(&testObject, &TestDriver::doNotDelete<TObj>, &ta);
            const Obj& X = x;
            ASSERT(++numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.numReferences());
            }
            ASSERT(0 == numDeletes);
            ASSERT(&testObject == X.ptr());
            ASSERT(1 == X.numReferences());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT(numDeallocations+1 == ta.numDeallocations());

#if !defined(BSLS_PLATFORM_CMP_IBM)
        if (verbose)
            printf("\nTesting ctor (with function type and allocator)"
                   "\n-----------------------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj testObject(&numDeletes);
            numAllocations = ta.numAllocations();

            Obj x(&testObject, TestDriver::doNotDelete<TObj>, &ta);
            const Obj& X = x;
            ASSERT(++numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P_(numDeletes); P(X.numReferences());
            }
            ASSERT(0 == numDeletes);
            ASSERT(&testObject == X.ptr());
            ASSERT(1 == X.numReferences());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT(1 == numDeletes);
        ASSERT(numDeallocations+1 == ta.numDeallocations());
#endif  // BSLS_PLATFORM_CMP_IBM

        if (verbose)
            printf("\nTesting constructor (with rep)"
                   "\n------------------------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new (ta) TObj(&numDeletes);
            numAllocations = ta.numAllocations();

            Obj x(p, &ta); const Obj& X = x;
            ASSERT(++numAllocations == ta.numAllocations());

            if (veryVerbose) {
                P(X.numReferences());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(1 == X.numReferences());

            bslma::SharedPtrRep *rep = x.rep();
            x.release();

            Obj xx(p, rep); const Obj& XX = xx;
            ASSERT(p == XX.ptr());
            ASSERT(rep ==  XX.rep());
            ASSERT(1 == XX.numReferences());
        }
        if (veryVerbose) {
            P_(numDeletes); P_(numDeallocations); P(ta.numDeallocations());
        }
        ASSERT((numDeallocations+2) == ta.numDeallocations());

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING BASIC CONSTRUCTORS AND ACCESSORS
        //   Verify that upon construction the object is properly initialized,
        //   Note that these tests assume that the default allocator is the
        //   NewDelete allocator; installing a test allocator for the default
        //   will break this test.
        //
        // Testing:
        //   bsl::shared_ptr();
        //   [bsl::shared_ptr(TYPE *ptr);]
        //   [bsl::shared_ptr(TYPE *ptr, bslma::Allocator *allocator);]
        //   operator bsl::shared_ptr_UnspecifiedBool() const;
        //   typename bsl::shared_ptr<TYPE>::Reference operator[]() const;
        //   typename bsl::shared_ptr<TYPE>::Reference operator*() const;
        //   TYPE *operator->() const;
        //   TYPE *ptr() const;
        //   bslma::SharedPtrRep *rep() const;
        //   int numReferences() const;
        //   TYPE *get() const;
        //   bool unique() const;
        //   int use_count() const;
        //   void clear();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Constructors and Destructor"
                            "\n===================================\n");

        bslma::TestAllocator ta(veryVeryVerbose);

        if (verbose) printf("\nTesting default constructor"
                            "\n---------------------------\n");
        {
            Obj x; const Obj& X=x;

            ASSERT(0 == X.ptr());
            ASSERT(0 == X.get());
            ASSERT(0 == X.rep());
            ASSERT(0 == X.numReferences());
            ASSERT(0 == X.use_count());
            ASSERT(false == X.unique());
            ASSERT(false == X);
        }

        if (verbose) printf("\nTesting basic constructor"
                            "\n-------------------------\n");
        {
            numDeletes = 0;
            TObj *p = new TObj(&numDeletes);

            Obj x(p); const Obj& X=x;

            if (veryVerbose) {
                P_(numDeletes); P(X.numReferences());
            }
            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(0 != X.rep());
            ASSERT(1 == X.rep()->numReferences());
            ASSERT((void *) p == X.rep()->originalPtr());
            ASSERT(p == X.get());
            ASSERT(1 == X.numReferences());
            ASSERT(1 == X.use_count());
            ASSERT(true == X.unique());
            ASSERT(false != X);
            ASSERT(p == X.operator->());
            ASSERT(p == &X.operator*());
            ASSERT(p == &X.operator[](0));
        }

        if (verbose) printf("\nTesting 'clear'"
                            "\n---------------\n");

        numDeallocations = ta.numDeallocations();
        {
            numDeletes = 0;
            TObj *p = new(ta) TObj(&numDeletes);

            Obj x(p, &ta); const Obj& X=x;
            numAllocations = ta.numAllocations();

            if (veryVerbose) {
                P_(numDeletes); P(X.numReferences());
            }

            ASSERT(0 == numDeletes);
            ASSERT(p == X.ptr());
            ASSERT(0 != X.rep());
            ASSERT(1 == X.rep()->numReferences());
            ASSERT((void *) p == X.rep()->originalPtr());
            ASSERT(1 == X.numReferences());

            x.clear();

            numDeallocations += 2;
            ASSERT(numDeallocations == ta.numDeallocations());
            ASSERT(1 == numDeletes);
            ASSERT(0 == X.ptr());
            ASSERT(0 == X.rep());
            ASSERT(0 == X.numReferences());
        }

        ASSERT(1 == numDeletes);
        ASSERT(numDeallocations == ta.numDeallocations());

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   Note that these tests assume that the default allocator is the
        //   NewDelete allocator; installing a test allocator for the default
        //   will break this test.
        //
        // Testing:
        //   This test exercises basic functionality but tests nothing.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST\n"
                            "\n==============\n");

        bsls::Types::Int64 numDeletes = 0;
        {
            MyTestObject *obj = new MyTestObject(&numDeletes);
            ASSERT(0 == numDeletes);

            Obj x(obj); const Obj& X=x;
            ASSERT(0 == numDeletes);
            ASSERT(obj == X.ptr());
            ASSERT(obj == X.get());
            ASSERT(X);
        }
        ASSERT(1 == numDeletes);
        if (veryVerbose) { P(numDeletes); }

        numDeletes = 0;
        {
            MyTestObject *obj = new MyTestObject(&numDeletes);
            MyTestObjectFactory deleter;
            ASSERT(0 == numDeletes);

            Obj x(obj,&deleter,0);
            const Obj& X=x;

            ASSERT(0 == numDeletes);
            ASSERT(obj == X.ptr());
            ASSERT(obj == X.get());
            ASSERT(X);
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            MyTestObject *obj = new MyTestObject(&numDeletes);
            MyTestDeleter deleter;
            ASSERT(0 == numDeletes);

            Obj x(obj, deleter, 0); const Obj& X=x;
            ASSERT(0 == numDeletes);
            ASSERT(obj == X.ptr());
            ASSERT(obj == X.get());
            ASSERT(X);

        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            MyTestObject *obj = new MyTestObject(&numDeletes);

            Obj x1(obj); const Obj& X1=x1;
            Obj x2; const Obj& X2=x2;
            ASSERT(0 == numDeletes);
            ASSERT(obj == X1.ptr());
            ASSERT(obj == X1.get());
            ASSERT(X1);
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            ConstObj x1(new MyTestObject(&numDeletes));
            const ConstObj& X1=x1;

            Obj x2(bslstl::SharedPtrUtil::constCast<TObj>(x1)); const Obj& X2=x2;
            if (veryVeryVerbose) {
                P(numDeletes);
            }
            ASSERT(0 == numDeletes);
            ASSERT(X1.ptr() == X2.ptr());
            if (veryVeryVerbose) {
                P(numDeletes);
            }

        }
        if (veryVeryVerbose) {
            P(numDeletes);
        }
        ASSERT(1 == numDeletes);

        numDeletes = 0;
        {
            TObj obj(&numDeletes);

            Obj x1; const Obj &X1 = x1;

            ASSERT(0 == X1.ptr());
            x1.createInplace(0, obj);

            ASSERT(0 != X1.ptr());
            ASSERT(0 == numDeletes);
            ASSERT(0 == X1->copyCounter());
            ASSERT(&numDeletes == X1->deleteCounter());
        }
        ASSERT(2 == numDeletes);

        numDeletes = 0;
        {
            TObj *p = new TObj(&numDeletes);

            Obj x1(p, &myTestFunctor, (bslma::Allocator *)0);
            const Obj &X1 = x1;

        }
        ASSERT(1 == numDeletes);
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        //
        // Concerns:  We should have a benchmark to time performance and keep
        //   track of performance improvements or losses as the code evolves.
        //   Performance here means both runtime and memory usage.
        //
        // Test plan: First measure the basics:  1. time to construct and
        //   destroy a shared pointer (executed in a loop), for various kinds
        //   of representations, and for various kinds of creation (allocating
        //   a new representation or simply incrementing a reference count); 2.
        //   time to assign copy shared pointers (measured by shuffling a
        //   vector of shared pointers).  Note that the type that is pointed to
        //   matters in (1), for in-place construction, but what matters is the
        //   base line, not the absolute timings; the type that is pointed to
        //   does *not* matter in (2), since this only measures the time to
        //   increment/decrement the shared pointers.  We measure the baselines
        //   (creation, copying and destruction of the test objects) so that
        //   they can be deducted from the shared pointers runtimes (e.g., when
        //   constructing inplace).
        //       Then we measure the times in various more specialized
        //   scenarios such as releasing in-place representations into a pool.
        //
        // Testing:
        //   PERFORMANCE
        // --------------------------------------------------------------------

        printf("\nPERFORMANCE TEST"
               "\n================\n");

        PerformanceTester<Obj>::test(verbose, veryVeryVerbose);

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
