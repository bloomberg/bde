// bslma_sharedptrinplacerep.t.cpp                                    -*-C++-*-
#include <bslma_sharedptrinplacerep.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'

#ifdef BSLS_PLATFORM_CMP_MSVC  // Microsoft Compiler
#ifdef _MSC_EXTENSIONS         // Microsoft Extensions Enabled
#include <new>                 // if so, need to include new as well
#endif
#endif

// These warning suppressions will be eliminated in due course.
// BDE_VERIFY pragma: -FD01  // Test-machinery lacks a contract
// BDE_VERIFY pragma: -TP18  // Test-case banners are ALL-CAPS

using namespace BloombergLP;
//using namespace bsl;  // automatically added by script

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// This test driver tests the functionality of the in-place shared pointer
// representation.
//-----------------------------------------------------------------------------
// bslma::SharedPtrInplaceRep
//---------------------------
// [ 2] SharedPtrInplaceRep(bslma::Allocator *basicAllocator);
// [ 3] SharedPtrInplaceRep(bslma::Allocator *allocator, const A1& a1);
// [ 3] SharedPtrInplaceRep(bslma::Allocator *allocator, const A1&...a2);
// [ 3] SharedPtrInplaceRep(bslma::Allocator *allocator, const A1&...a3);
// [ 3] SharedPtrInplaceRep(bslma::Allocator *allocator, const A1&...a4);
// [ 3] SharedPtrInplaceRep(bslma::Allocator *allocator, const A1&...a5);
// [ 3] SharedPtrInplaceRep(bslma::Allocator *allocator, const A1&...a6);
// [ 3] SharedPtrInplaceRep(bslma::Allocator *allocator, const A1&...a7);
// [ 3] SharedPtrInplaceRep(bslma::Allocator *allocator, const A1&...a8);
// [ 3] SharedPtrInplaceRep(bslma::Allocator *allocator, const A1&...a9);
// [ 3] SharedPtrInplaceRep(bslma::Allocator *allocator, const A1&...a10);
// [ 3] SharedPtrInplaceRep(bslma::Allocator *allocator, const A1&...a11);
// [ 3] SharedPtrInplaceRep(bslma::Allocator *allocator, const A1&...a12);
// [ 3] SharedPtrInplaceRep(bslma::Allocator *allocator, const A1&...a13);
// [ 3] SharedPtrInplaceRep(bslma::Allocator *allocator, const A1&...a14);
// [ 2] TYPE *ptr();
// [ 2] void disposeRep();
// [ 4] void disposeObject();
// [  ] void *getDeleter(const std::type_info& type);
// [ 2] void *originalPtr() const;
// [ 5] void releaseRef();
// [ 5] void releaseWeakRef();
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 6] USAGE EXAMPLE
//-----------------------------------------------------------------------------

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

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

//=============================================================================
//                 GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

// 'MyTestObject' CLASS HIERARCHY (defined below)
class MyTestObject;

// OTHER TEST OBJECTS (defined below)
class MyInplaceTestObject;

// TYPEDEFS
typedef bslma::SharedPtrInplaceRep<MyTestObject> Obj;

typedef bslma::SharedPtrInplaceRep<MyInplaceTestObject> TCObj;
                                                    // For testing constructors
typedef MyTestObject TObj;

//=============================================================================
//              GLOBAL HELPER CLASSES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

                             // ==================
                             // class MyTestObject
                             // ==================

class MyTestObject {
    // This class provides a test object that keeps track of how many instances
    // have been deleted.  Optionally, also keeps track of how many instances
    // have been copied.

    // DATA
    volatile int *d_deleteCounter_p;
    volatile int *d_copyCounter_p;

  public:
    // CREATORS
    MyTestObject(const MyTestObject& orig);
    explicit MyTestObject(int *deleteCounter, int *copyCounter = 0);
    virtual ~MyTestObject();

    // ACCESSORS
    volatile int *copyCounter() const;
    volatile int *deleteCounter() const;
};

                             // ------------------
                             // class MyTestObject
                             // ------------------

// CREATORS
MyTestObject::MyTestObject(const MyTestObject& orig)
: d_deleteCounter_p(orig.d_deleteCounter_p)
, d_copyCounter_p(orig.d_copyCounter_p)
{
    if (d_copyCounter_p) ++(*d_copyCounter_p);
}

MyTestObject::MyTestObject(int *deleteCounter, int *copyCounter)
: d_deleteCounter_p(deleteCounter)
, d_copyCounter_p(copyCounter)
{
}

MyTestObject::~MyTestObject()
{
    ++(*d_deleteCounter_p);
}

// ACCESSORS
volatile int* MyTestObject::copyCounter() const
{
    return d_copyCounter_p;
}

volatile int* MyTestObject::deleteCounter() const
{
    return d_deleteCounter_p;
}

                         // ====================
                         // class MyTestArg<int>
                         // ====================

template <int N>
class MyTestArg {
    // This class template declares a separate type for each template parameter
    // value 'N', that wraps an integer value and provides implicit conversion
    // to and from 'int'.  Its main purpose is that having separate types for
    // testing enables distinguishing them when calling through a function
    // template interface, thereby avoiding ambiguities or accidental switching
    // of arguments in the implementation of in-place constructors.

    // DATA
    int d_value;

  public:
    // CREATORS
    explicit MyTestArg(int value = -1) : d_value(value) {}

    // MANIPULATORS
    operator int&()      { return d_value; }

    // ACCESSORS
    operator int() const { return d_value; }
};

typedef MyTestArg< 1> MyTestArg1;
typedef MyTestArg< 2> MyTestArg2;
typedef MyTestArg< 3> MyTestArg3;
typedef MyTestArg< 4> MyTestArg4;
typedef MyTestArg< 5> MyTestArg5;
typedef MyTestArg< 6> MyTestArg6;
typedef MyTestArg< 7> MyTestArg7;
typedef MyTestArg< 8> MyTestArg8;
typedef MyTestArg< 9> MyTestArg9;
typedef MyTestArg<10> MyTestArg10;
typedef MyTestArg<11> MyTestArg11;
typedef MyTestArg<12> MyTestArg12;
typedef MyTestArg<13> MyTestArg13;
typedef MyTestArg<14> MyTestArg14;
    // Define fourteen test argument types 'MyTestArg1..14' to be used with the
    // in-place constructors of 'MyInplaceTestObject'.

                         // =========================
                         // class MyInplaceTestObject
                         // =========================

class MyInplaceTestObject {
    // This class provides a test object used to check that the arguments
    // passed for creating a shared pointer with an in-place representation are
    // of the correct types and values.

    // DATA
    MyTestArg1  d_a1;
    MyTestArg2  d_a2;
    MyTestArg3  d_a3;
    MyTestArg4  d_a4;
    MyTestArg5  d_a5;
    MyTestArg6  d_a6;
    MyTestArg7  d_a7;
    MyTestArg8  d_a8;
    MyTestArg9  d_a9;
    MyTestArg10 d_a10;
    MyTestArg11 d_a11;
    MyTestArg12 d_a12;
    MyTestArg13 d_a13;
    MyTestArg14 d_a14;
    static int  s_numDeletes;

  public:
    // CREATORS
    MyInplaceTestObject();
    explicit MyInplaceTestObject(MyTestArg1 a1);
    MyInplaceTestObject(MyTestArg1  a1,
                        MyTestArg2  a2);
    MyInplaceTestObject(MyTestArg1  a1,
                        MyTestArg2  a2,
                        MyTestArg3  a3);
    MyInplaceTestObject(MyTestArg1  a1,
                        MyTestArg2  a2,
                        MyTestArg3  a3,
                        MyTestArg4  a4);
    MyInplaceTestObject(MyTestArg1  a1,
                        MyTestArg2  a2,
                        MyTestArg3  a3,
                        MyTestArg4  a4,
                        MyTestArg5  a5);
    MyInplaceTestObject(MyTestArg1  a1,
                        MyTestArg2  a2,
                        MyTestArg3  a3,
                        MyTestArg4  a4,
                        MyTestArg5  a5,
                        MyTestArg6  a6);
    MyInplaceTestObject(MyTestArg1  a1,
                        MyTestArg2  a2,
                        MyTestArg3  a3,
                        MyTestArg4  a4,
                        MyTestArg5  a5,
                        MyTestArg6  a6,
                        MyTestArg7  a7);
    MyInplaceTestObject(MyTestArg1  a1,
                        MyTestArg2  a2,
                        MyTestArg3  a3,
                        MyTestArg4  a4,
                        MyTestArg5  a5,
                        MyTestArg6  a6,
                        MyTestArg7  a7,
                        MyTestArg8  a8);
    MyInplaceTestObject(MyTestArg1  a1,
                        MyTestArg2  a2,
                        MyTestArg3  a3,
                        MyTestArg4  a4,
                        MyTestArg5  a5,
                        MyTestArg6  a6,
                        MyTestArg7  a7,
                        MyTestArg8  a8,
                        MyTestArg9  a9);
    MyInplaceTestObject(MyTestArg1  a1,
                        MyTestArg2  a2,
                        MyTestArg3  a3,
                        MyTestArg4  a4,
                        MyTestArg5  a5,
                        MyTestArg6  a6,
                        MyTestArg7  a7,
                        MyTestArg8  a8,
                        MyTestArg9  a9,
                        MyTestArg10 a10);
    MyInplaceTestObject(MyTestArg1  a1,
                        MyTestArg2  a2,
                        MyTestArg3  a3,
                        MyTestArg4  a4,
                        MyTestArg5  a5,
                        MyTestArg6  a6,
                        MyTestArg7  a7,
                        MyTestArg8  a8,
                        MyTestArg9  a9,
                        MyTestArg10 a10,
                        MyTestArg11 a11);
    MyInplaceTestObject(MyTestArg1  a1,
                        MyTestArg2  a2,
                        MyTestArg3  a3,
                        MyTestArg4  a4,
                        MyTestArg5  a5,
                        MyTestArg6  a6,
                        MyTestArg7  a7,
                        MyTestArg8  a8,
                        MyTestArg9  a9,
                        MyTestArg10 a10,
                        MyTestArg11 a11,
                        MyTestArg12 a12);
    MyInplaceTestObject(MyTestArg1  a1,
                        MyTestArg2  a2,
                        MyTestArg3  a3,
                        MyTestArg4  a4,
                        MyTestArg5  a5,
                        MyTestArg6  a6,
                        MyTestArg7  a7,
                        MyTestArg8  a8,
                        MyTestArg9  a9,
                        MyTestArg10 a10,
                        MyTestArg11 a11,
                        MyTestArg12 a12,
                        MyTestArg13 a13);
    MyInplaceTestObject(MyTestArg1  a1,
                        MyTestArg2  a2,
                        MyTestArg3  a3,
                        MyTestArg4  a4,
                        MyTestArg5  a5,
                        MyTestArg6  a6,
                        MyTestArg7  a7,
                        MyTestArg8  a8,
                        MyTestArg9  a9,
                        MyTestArg10 a10,
                        MyTestArg11 a11,
                        MyTestArg12 a12,
                        MyTestArg13 a13,
                        MyTestArg14 a14);
        // Create a 'MyInplaceTestObject' by initializing the data members
        // 'd_a1'..'d_a14' with the specified 'a1'..'a14', and initializing any
        // remaining data members with their default value (-1).

    ~MyInplaceTestObject();
        // Increment the count of calls to this destructor, and destroy this
        // object.

    // ACCESSORS
    bool operator==(const MyInplaceTestObject& rhs) const;
        // Return 'true' if the specified 'rhs' has the same value as this
        // object, and 'false' otherwise.  Two 'MyInplaceTestObject' objects
        // have the same value if each of their corresponding data members
        // 'd1'..'d14' have the same value.

    static int getNumDeletes();
        // Return the number of times an object of this type has been
        // destroyed.
};

                         // -------------------------
                         // class MyInplaceTestObject
                         // -------------------------

int MyInplaceTestObject::s_numDeletes = 0;

// CREATORS
MyInplaceTestObject::MyInplaceTestObject()
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1 a1)
: d_a1(a1)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2)
: d_a1(a1)
, d_a2(a2)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1 a1,
                                         MyTestArg2 a2,
                                         MyTestArg3 a3)
: d_a1(a1)
, d_a2(a2)
, d_a3(a3)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1 a1,
                                         MyTestArg2 a2,
                                         MyTestArg3 a3,
                                         MyTestArg4 a4)
: d_a1(a1)
, d_a2(a2)
, d_a3(a3)
, d_a4(a4)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1 a1,
                                         MyTestArg2 a2,
                                         MyTestArg3 a3,
                                         MyTestArg4 a4,
                                         MyTestArg5 a5)
: d_a1(a1)
, d_a2(a2)
, d_a3(a3)
, d_a4(a4)
, d_a5(a5)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1 a1,
                                         MyTestArg2 a2,
                                         MyTestArg3 a3,
                                         MyTestArg4 a4,
                                         MyTestArg5 a5,
                                         MyTestArg6 a6)
: d_a1(a1)
, d_a2(a2)
, d_a3(a3)
, d_a4(a4)
, d_a5(a5)
, d_a6(a6)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1 a1,
                                         MyTestArg2 a2,
                                         MyTestArg3 a3,
                                         MyTestArg4 a4,
                                         MyTestArg5 a5,
                                         MyTestArg6 a6,
                                         MyTestArg7 a7)
: d_a1(a1)
, d_a2(a2)
, d_a3(a3)
, d_a4(a4)
, d_a5(a5)
, d_a6(a6)
, d_a7(a7)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1 a1,
                                         MyTestArg2 a2,
                                         MyTestArg3 a3,
                                         MyTestArg4 a4,
                                         MyTestArg5 a5,
                                         MyTestArg6 a6,
                                         MyTestArg7 a7,
                                         MyTestArg8 a8)
: d_a1(a1)
, d_a2(a2)
, d_a3(a3)
, d_a4(a4)
, d_a5(a5)
, d_a6(a6)
, d_a7(a7)
, d_a8(a8)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1 a1,
                                         MyTestArg2 a2,
                                         MyTestArg3 a3,
                                         MyTestArg4 a4,
                                         MyTestArg5 a5,
                                         MyTestArg6 a6,
                                         MyTestArg7 a7,
                                         MyTestArg8 a8,
                                         MyTestArg9 a9)
: d_a1(a1)
, d_a2(a2)
, d_a3(a3)
, d_a4(a4)
, d_a5(a5)
, d_a6(a6)
, d_a7(a7)
, d_a8(a8)
, d_a9(a9)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1  a1,
                                         MyTestArg2  a2,
                                         MyTestArg3  a3,
                                         MyTestArg4  a4,
                                         MyTestArg5  a5,
                                         MyTestArg6  a6,
                                         MyTestArg7  a7,
                                         MyTestArg8  a8,
                                         MyTestArg9  a9,
                                         MyTestArg10 a10)
: d_a1(a1)
, d_a2(a2)
, d_a3(a3)
, d_a4(a4)
, d_a5(a5)
, d_a6(a6)
, d_a7(a7)
, d_a8(a8)
, d_a9(a9)
, d_a10(a10)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1  a1,
                                         MyTestArg2  a2,
                                         MyTestArg3  a3,
                                         MyTestArg4  a4,
                                         MyTestArg5  a5,
                                         MyTestArg6  a6,
                                         MyTestArg7  a7,
                                         MyTestArg8  a8,
                                         MyTestArg9  a9,
                                         MyTestArg10 a10,
                                         MyTestArg11 a11)
: d_a1(a1)
, d_a2(a2)
, d_a3(a3)
, d_a4(a4)
, d_a5(a5)
, d_a6(a6)
, d_a7(a7)
, d_a8(a8)
, d_a9(a9)
, d_a10(a10)
, d_a11(a11)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1  a1,
                                         MyTestArg2  a2,
                                         MyTestArg3  a3,
                                         MyTestArg4  a4,
                                         MyTestArg5  a5,
                                         MyTestArg6  a6,
                                         MyTestArg7  a7,
                                         MyTestArg8  a8,
                                         MyTestArg9  a9,
                                         MyTestArg10 a10,
                                         MyTestArg11 a11,
                                         MyTestArg12 a12)
: d_a1(a1)
, d_a2(a2)
, d_a3(a3)
, d_a4(a4)
, d_a5(a5)
, d_a6(a6)
, d_a7(a7)
, d_a8(a8)
, d_a9(a9)
, d_a10(a10)
, d_a11(a11)
, d_a12(a12)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1  a1,
                                         MyTestArg2  a2,
                                         MyTestArg3  a3,
                                         MyTestArg4  a4,
                                         MyTestArg5  a5,
                                         MyTestArg6  a6,
                                         MyTestArg7  a7,
                                         MyTestArg8  a8,
                                         MyTestArg9  a9,
                                         MyTestArg10 a10,
                                         MyTestArg11 a11,
                                         MyTestArg12 a12,
                                         MyTestArg13 a13)
: d_a1(a1)
, d_a2(a2)
, d_a3(a3)
, d_a4(a4)
, d_a5(a5)
, d_a6(a6)
, d_a7(a7)
, d_a8(a8)
, d_a9(a9)
, d_a10(a10)
, d_a11(a11)
, d_a12(a12)
, d_a13(a13)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1  a1,
                                         MyTestArg2  a2,
                                         MyTestArg3  a3,
                                         MyTestArg4  a4,
                                         MyTestArg5  a5,
                                         MyTestArg6  a6,
                                         MyTestArg7  a7,
                                         MyTestArg8  a8,
                                         MyTestArg9  a9,
                                         MyTestArg10 a10,
                                         MyTestArg11 a11,
                                         MyTestArg12 a12,
                                         MyTestArg13 a13,
                                         MyTestArg14 a14)
: d_a1(a1)
, d_a2(a2)
, d_a3(a3)
, d_a4(a4)
, d_a5(a5)
, d_a6(a6)
, d_a7(a7)
, d_a8(a8)
, d_a9(a9)
, d_a10(a10)
, d_a11(a11)
, d_a12(a12)
, d_a13(a13)
, d_a14(a14)
{
}

inline
MyInplaceTestObject::~MyInplaceTestObject()
{
    ++s_numDeletes;
}

inline
bool MyInplaceTestObject::operator==(MyInplaceTestObject const& rhs) const
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

#if 0  // TBD Need an appropriately levelized usage example
                              // ================
                              // MySharedDatetime
                              // ================

class MySharedDatetime {
    // This class provide a reference counted smart pointer to support shared
    // ownership of a 'bdet_Datetime' object.

  private:
    bdet_Datetime      *d_ptr_p;  // pointer to the managed object
    bslma::SharedPtrRep *d_rep_p;  // pointer to the representation object

  private:
    // NOT IMPLEMENTED
    MySharedDatetime& operator=(const MySharedDatetime&);

  public:
    // CREATORS
    MySharedDatetime();
        // Create an empty shared datetime.

    MySharedDatetime(bdet_Datetime* ptr, bslma::SharedPtrRep* rep);
        // Create a shared datetime that adopts ownership of the specified
        // 'ptr' and the specified 'rep.

    MySharedDatetime(const MySharedDatetime& original);
        // Create a shared datetime that refers to the same object managed by
        // the specified 'original'

    ~MySharedDatetime();
        // Destroy this shared datetime and release the reference any object it
        // might be referring to.  If this is the last shared reference,
        // deleted the managed object.

    // MANIPULATORS
    void createInplace(bslma::Allocator *allocator,
                       int               year,
                       int               month,
                       int               day);
        // Create a new 'MySharedDatetimeRepImpl', using the specified
        // 'allocator' to supply memory, using the specified 'year', 'month'
        // and 'day' to initialize the 'bdet_Datetime' within the newly created
        // 'MySharedDatetimeRepImpl', and make this 'MySharedDatetime' refer to
        // the 'bdet_Datetime'.

    bdet_Datetime& operator*() const;
        // Return a modifiable reference to the shared 'bdet_Datetime' object.

    bdet_Datetime *operator->() const;
        // Return the address of the modifiable 'bdet_Datetime' to which this
        // object refers.

    bdet_Datetime *ptr() const;
        // Return the address of the modifiable 'bdet_Datetime' to which this
        // object refers.
};

                              // ----------------
                              // MySharedDatetime
                              // ----------------

MySharedDatetime::MySharedDatetime()
: d_ptr_p(0)
, d_rep_p(0)
{
}

MySharedDatetime::MySharedDatetime(bdet_Datetime      *ptr,
                                   bslma::SharedPtrRep *rep)
: d_ptr_p(ptr)
, d_rep_p(rep)
{
}

MySharedDatetime::MySharedDatetime(const MySharedDatetime& original)
: d_ptr_p(original.d_ptr_p)
, d_rep_p(original.d_rep_p)
{
    if (d_ptr_p) {
        d_rep_p->acquireRef();
    } else {
        d_rep_p = 0;
    }
}

MySharedDatetime::~MySharedDatetime()
{
    if (d_rep_p) {
        d_rep_p->releaseRef();
    }
}

void MySharedDatetime::createInplace(bslma::Allocator *allocator,
                                     int               year,
                                     int               month,
                                     int               day)
{
    allocator = bslma::Default::allocator(allocator);
    bslma::SharedPtrInplaceRep<bdet_Datetime> *rep = new (*allocator)
                        bslma::SharedPtrInplaceRep<bdet_Datetime>(allocator,
                                                                 year,
                                                                 month,
                                                                 day);
    MySharedDatetime temp(rep->ptr(), rep);
    bsl::swap(d_ptr_p, temp.d_ptr_p);
    bsl::swap(d_rep_p, temp.d_rep_p);
}

bdet_Datetime& MySharedDatetime::operator*() const {
    return *d_ptr_p;
}

bdet_Datetime *MySharedDatetime::operator->() const {
    return d_ptr_p;
}

bdet_Datetime *MySharedDatetime::ptr() const {
    return d_ptr_p;
}
#endif

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

    (void)veryVerbose;      // silence unused variable warnings
    (void)veryVeryVerbose;  // silence unused variable warnings

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // Confirm no static initialization locked the global allocator
    ASSERT(&globalAllocator == bslma::Default::globalAllocator());

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    // Confirm no static initialization locked the default allocator
    ASSERT(&defaultAllocator == bslma::Default::defaultAllocator());

    bslma::TestAllocator ta(veryVeryVeryVerbose);
    bsls::Types::Int64 numDeallocations;
    bsls::Types::Int64 numAllocations;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
#if 0  // TBD Need an appropriately levelized usage example
      case 6: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concern:
        //   Usage example described in header doc compiles and run.
        //
        // Plan:
        //   Copy the usage example and strip the comments.  Then create simple
        //   test case to use the implementation described in doc.
        //
        // Testing:
        //   Usage example
        // --------------------------------------------------------------------
        if (verbose) printf(endl
                        << "Testing Usage Example\n"
                        << "=====================\n");
        {
            MySharedDatetime dt1;
            ASSERT(0 == ta.numAllocations());

            dt1.createInplace(&ta, 2011, 1, 1);

            ASSERT(1 == ta.numAllocations());

            MySharedDatetime dt2(dt1);

            ASSERT(dt2.ptr() == dt1.ptr());
            ASSERT(*dt2 == *dt1);

            dt2->addDays(31);
            ASSERT(2 == dt2->month());
            ASSERT(*dt1 == *dt2);
            ASSERT(0 == ta.numDeallocations());
        }
        ASSERT(1 == ta.numDeallocations());
      } break;
#endif
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'releaseRef' and 'releaseWeakRef'
        //
        // Concerns:
        //   1) 'releaseRef' and 'releaseWeakRef' is decrementing the reference
        //      count correctly.
        //   2) disposeObject() is called when there is no shared reference.
        //   3) disposeRep() is called only when there is no shared reference
        //      and no weak reference.
        //
        // Plan:
        //   1) Call 'acquireRef' then 'releaseRef' and verify 'numReference'
        //      did not change.  Call 'acquireWeakRef' then 'releaseWeakRef'
        //      and verify 'numWeakReference' did not change.
        //   2) Call 'releaseRef' when there is only one reference remaining.
        //      Then verify that both 'disposeObject' and 'disposeRep' is
        //      called.
        //   3) Create another object and call 'acquireWeakRef' before calling
        //      'releaseRef'.  Verify that only 'disposeObject' is called.
        //      Then call 'releaseWeakRef' and verify that 'disposeRep' is
        //      called.
        //
        // Testing:
        //   void releaseRef();
        //   void releaseWeakRef();
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING 'releaseRef' and 'releaseWeakRef'"
                            "\n=========================================\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            int numDeletes = 0;
            Obj* xPtr = new(ta) Obj(&ta, &numDeletes);
            Obj& x = *xPtr;
            Obj const& X = *xPtr;

            ASSERT(++numAllocations == ta.numAllocations());

            x.acquireRef();
            x.releaseRef();

            ASSERT(1 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(true == X.hasUniqueOwner());

            x.acquireWeakRef();
            x.releaseWeakRef();

            ASSERT(1 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(true == X.hasUniqueOwner());

            if (verbose) printf(
                        "\nTesting 'releaseRef' with no weak reference'"
                        "\n--------------------------------------------\n");

            x.releaseRef();

            ASSERT(1 == numDeletes);
            ASSERT(++numDeallocations == ta.numDeallocations());
        }
        if (verbose) printf("\nTesting 'releaseRef' with weak reference'"
                            "\n-----------------------------------------\n");

        {
            int numDeletes = 0;
            Obj* xPtr = new(ta) Obj(&ta, &numDeletes);
            Obj& x = *xPtr;
            Obj const& X = *xPtr;

            ASSERT(++numAllocations == ta.numAllocations());

            x.acquireWeakRef();
            x.releaseRef();

            ASSERT(0 == X.numReferences());
            ASSERT(1 == X.numWeakReferences());
            ASSERT(false == X.hasUniqueOwner());
            ASSERT(1 == numDeletes);
            ASSERT(numDeallocations == ta.numDeallocations());

            x.releaseWeakRef();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'disposeObject'
        //
        // Concerns:
        //   The destructor of the object is called when 'disposeObject' is
        //   called.
        //
        // Plan:
        //   Call 'disposeObject' and verify that the destructor is called.
        //
        // Testing:
        //   void disposeObject();
        //
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING 'disposeObject'"
                            "\n=======================\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            int numDeletes = 0;
            Obj* xPtr = new(ta) Obj(&ta, &numDeletes);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(0 == numDeletes);

            xPtr->disposeObject();
            ASSERT(1 == numDeletes);

            xPtr->disposeRep();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTORS
        //
        // Concerns:
        //   All constructor is able to initialize the object correctly.
        //
        // Plan:
        //   Call all 14 different constructors and supply it with the
        //   appropriate arguments.  Then verify that the object created inside
        //   the representation is initialized using the arguments supplied.
        //
        // Testing:
        //   SharedPtrInplaceRep(bslma::Allocator *allocator, ...a1);
        //   SharedPtrInplaceRep(bslma::Allocator *allocator, ...a2);
        //   SharedPtrInplaceRep(bslma::Allocator *allocator, ...a3);
        //   SharedPtrInplaceRep(bslma::Allocator *allocator, ...a4);
        //   SharedPtrInplaceRep(bslma::Allocator *allocator, ...a5);
        //   SharedPtrInplaceRep(bslma::Allocator *allocator, ...a6);
        //   SharedPtrInplaceRep(bslma::Allocator *allocator, ...a7);
        //   SharedPtrInplaceRep(bslma::Allocator *allocator, ...a8);
        //   SharedPtrInplaceRep(bslma::Allocator *allocator, ...a9);
        //   SharedPtrInplaceRep(bslma::Allocator *allocator, ...a10);
        //   SharedPtrInplaceRep(bslma::Allocator *allocator, ...a11);
        //   SharedPtrInplaceRep(bslma::Allocator *allocator, ...a12);
        //   SharedPtrInplaceRep(bslma::Allocator *allocator, ...a13);
        //   SharedPtrInplaceRep(bslma::Allocator *allocator, ...a14);
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING CONSTRUCTORS"
                            "\n====================\n");

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

        if (verbose) printf("\nTesting constructor with 1 argument"
                            "\n-----------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1);
            TCObj* xPtr = new(ta) TCObj(&ta, V1);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *(xPtr->ptr()));
            xPtr->disposeRep();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) printf("\nTesting contructor with 2 arguments"
                            "\n-----------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2);
            TCObj* xPtr = new(ta) TCObj(&ta, V1, V2);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *(xPtr->ptr()));
            xPtr->disposeRep();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) printf("\nTesting constructor with 3 arguments"
                            "\n------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3);
            TCObj* xPtr = new(ta) TCObj(&ta, V1, V2, V3);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *(xPtr->ptr()));
            xPtr->disposeRep();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) printf("\nTesting constructor with 4 arguments"
                            "\n------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4);
            TCObj* xPtr = new(ta) TCObj(&ta, V1, V2, V3, V4);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *(xPtr->ptr()));
            xPtr->disposeRep();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) printf("\nTesting constructor with 5 arguments"
                            "\n------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5);
            TCObj* xPtr = new(ta) TCObj(&ta, V1, V2, V3, V4, V5);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *(xPtr->ptr()));
            xPtr->disposeRep();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) printf("\nTesting constructor with 6 arguments"
                            "\n------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6);
            TCObj* xPtr = new(ta) TCObj(&ta, V1, V2, V3, V4, V5, V6);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *(xPtr->ptr()));
            xPtr->disposeRep();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) printf("\nTesting constructor with 7 arguments"
                            "\n------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7);
            TCObj* xPtr = new(ta) TCObj(&ta, V1, V2, V3, V4, V5, V6, V7);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *(xPtr->ptr()));
            xPtr->disposeRep();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) printf("\nTesting constructor with 8 arguments"
                            "\n------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                                           V8);
            TCObj* xPtr = new(ta) TCObj(&ta, V1, V2, V3, V4, V5, V6, V7, V8);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *(xPtr->ptr()));
            xPtr->disposeRep();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) printf("\nTesting constructor with 9 arguments"
                            "\n------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                                       V8, V9);
            TCObj* xPtr = new(ta) TCObj(&ta, V1, V2, V3, V4, V5, V6, V7, V8,
                                                                           V9);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *(xPtr->ptr()));
            xPtr->disposeRep();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) printf("\nTesting constructor with 10 arguments"
                            "\n-------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                                  V8, V9, V10);
            TCObj* xPtr = new(ta) TCObj(&ta, V1, V2, V3, V4, V5, V6, V7, V8,
                                                                      V9, V10);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *(xPtr->ptr()));
            xPtr->disposeRep();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) printf("\nTesting constructor with 11 arguments"
                            "\n-------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                             V8, V9, V10, V11);
            TCObj* xPtr = new(ta) TCObj(&ta, V1, V2, V3, V4, V5, V6, V7, V8,
                                                                 V9, V10, V11);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *(xPtr->ptr()));
            xPtr->disposeRep();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) printf("\nTesting constructor with 12 arguments"
                            "\n-------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                        V8, V9, V10, V11, V12);
            TCObj* xPtr = new(ta) TCObj(&ta, V1, V2, V3, V4, V5, V6, V7, V8,
                                                            V9, V10, V11, V12);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *(xPtr->ptr()));
            xPtr->disposeRep();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) printf("\nTesting constructor with 13 arguments"
                            "\n-------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                                   V8, V9, V10, V11, V12, V13);
            TCObj* xPtr = new(ta) TCObj(&ta, V1, V2, V3, V4, V5, V6, V7, V8,
                                                       V9, V10, V11, V12, V13);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *(xPtr->ptr()));
            xPtr->disposeRep();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

        if (verbose) printf("\nTesting constructor with 14 arguments"
                            "\n-------------------------------------\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                              V8, V9, V10, V11, V12, V13, V14);
            TCObj* xPtr = new(ta) TCObj(&ta, V1, V2, V3, V4, V5, V6, V7, V8,
                                                  V9, V10, V11, V12, V13, V14);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *(xPtr->ptr()));
            xPtr->disposeRep();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING BASIC CONSTRUCTOR
        //
        // Concerns:
        //   Verify that upon construction the object is properly initialized,
        //
        // Plan:
        //   Construct object using basic constructor and verify that that
        //   accessors return the expected values.
        //
        // Testing:
        //   bslma::SharedPtrInplaceRep(bslma::Allocator *basicAllocator);
        //   TYPE *ptr();
        //   void disposeRep();
        //   void *originalPtr() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BASIC CONSTRUCTOR"
                            "\n=========================\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP;

            // Dynamically allocate object as the destructor is declared as
            // private.

            TCObj* xPtr = new(ta) TCObj(&ta);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *(xPtr->ptr()));
            ASSERT(xPtr->originalPtr() == static_cast<void*>(xPtr->ptr()));

            // Manually deallocate the representation using 'disposeRep'.

            xPtr->disposeRep();

            ASSERT(++numDeallocations == ta.numDeallocations());
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   This test exercises basic functionality but tests nothing.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

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
