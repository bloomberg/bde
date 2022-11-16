// bslstl_sharedptrallocateinplacerep.t.cpp                           -*-C++-*-
#include <bslstl_sharedptrallocateinplacerep.h>

#include <bslma_allocator.h>
#include <bslma_allocatortraits.h>
#include <bslma_default.h>
#include <bslma_stdallocator.h>
#include <bslma_testallocator.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_types.h>

#include <bsltf_stdstatefulallocator.h>

#include <stdio.h>
#include <stdlib.h>             // 'atoi'

#include <new>

using namespace BloombergLP;

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// This test driver tests the functionality of the in-place shared pointer
// representation.
//-----------------------------------------------------------------------------
// [ 2] SharedPtrAllocateInplaceRep(const ReboundAllocator& alloc);
// [ 2] SharedPtrAllocateInplaceRep makeRep(ReboundAllocator alloc);
// [ 2] TYPE *ptr();
// [ 2] void disposeRep();
// [ 4] void disposeObject();
// [ 6] void *getDeleter(const std::type_info& type);
// [ 2] void *originalPtr() const;
// [ 5] void releaseRef();
// [ 5] void releaseWeakRef();
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] USAGE EXAMPLE
//-----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

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

#if 0  // TBD Need an appropriately levelized usage example
                              // ================
                              // MySharedDatetime
                              // ================

class MySharedDatetime {
    // This class provide a reference counted smart pointer to support shared
    // ownership of a 'bdlt::Datetime' object.

  private:
    bdlt::Datetime      *d_ptr_p;  // pointer to the managed object
    bslma::SharedPtrRep *d_rep_p;  // pointer to the representation object

  private:
    // NOT IMPLEMENTED
    MySharedDatetime& operator=(const MySharedDatetime&);

  public:
    // CREATORS
    MySharedDatetime();
        // Create an empty shared datetime.

    MySharedDatetime(bdlt::Datetime* ptr, bslma::SharedPtrRep* rep);
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
        // and 'day' to initialize the 'bdlt::Datetime' within the newly
        // created 'MySharedDatetimeRepImpl', and make this 'MySharedDatetime'
        // refer to the 'bdlt::Datetime'.

    bdlt::Datetime& operator*() const;
        // Return a modifiable reference to the shared 'bdlt::Datetime' object.

    bdlt::Datetime *operator->() const;
        // Return the address of the modifiable 'bdlt::Datetime' to which this
        // object refers.

    bdlt::Datetime *ptr() const;
        // Return the address of the modifiable 'bdlt::Datetime' to which this
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

MySharedDatetime::MySharedDatetime(bdlt::Datetime      *ptr,
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
    bslstl::SharedPtrAllocateInplaceRep<bdlt::Datetime> *rep = new (*allocator)
                 bslstl::SharedPtrAllocateInplaceRep<bdlt::Datetime>(allocator,
                                                                     year,
                                                                     month,
                                                                     day);
    MySharedDatetime temp(rep->ptr(), rep);
    bsl::swap(d_ptr_p, temp.d_ptr_p);
    bsl::swap(d_rep_p, temp.d_rep_p);
}

bdlt::Datetime& MySharedDatetime::operator*() const {
    return *d_ptr_p;
}

bdlt::Datetime *MySharedDatetime::operator->() const {
    return d_ptr_p;
}

bdlt::Datetime *MySharedDatetime::ptr() const {
    return d_ptr_p;
}
#endif

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

// 'MyTestObject' CLASS HIERARCHY (defined below)
class MyTestObject;

// OTHER TEST OBJECTS (defined below)
class MyInplaceTestObject;

// TYPEDEFS
typedef bslstl::SharedPtrAllocateInplaceRep<MyTestObject,
                                            bsl::allocator<MyTestObject> > Obj;

typedef bslstl::SharedPtrAllocateInplaceRep<MyInplaceTestObject,
                                            bsl::allocator<MyInplaceTestObject>
                                           > TCObj; // For testing constructors

typedef MyTestObject TObj;

// ============================================================================
//              GLOBAL HELPER CLASSES AND FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

                             // ==================
                             // class MyTestObject
                             // ==================

class MyTestObject {
    // This class provides a test object that keeps track of how many objects
    // have been deleted.  Optionally, also keeps track of how many objects
    // have been copied.

    // DATA
    volatile bsls::Types::Int64 *d_deleteCounter_p;
    volatile bsls::Types::Int64 *d_copyCounter_p;

  public:
    // CREATORS
    MyTestObject(const MyTestObject& original);
        // Create a copy of the specified 'original' object.

    explicit MyTestObject(bsls::Types::Int64 *deleteCounter,
                          bsls::Types::Int64 *copyCounter = 0);

    virtual ~MyTestObject();
        // Destroy this object.

    // ACCESSORS
    volatile bsls::Types::Int64 *copyCounter() const;
        // Return a pointer to the counter (if any) used to track the number of
        // times an object of type 'MyTestObject' has been copied.

    volatile bsls::Types::Int64 *deleteCounter() const;
        // Return a pointer to the counter used to track the number of times an
        // object of type 'MyTestObject' has been copied.

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
volatile bsls::Types::Int64* MyTestObject::copyCounter() const
{
    return d_copyCounter_p;
}

volatile bsls::Types::Int64* MyTestObject::deleteCounter() const
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
        // Create a test argument object having the optionally specified
        // 'value', and having the value '-1' otherwise.

    // ACCESSORS
    operator int() const { return d_value; }
        // Return the value of this test argument object.
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
    MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2);
    MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,  MyTestArg3  a3);
    MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,  MyTestArg3  a3,
                        MyTestArg4  a4);
    MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,  MyTestArg3  a3,
                        MyTestArg4  a4,  MyTestArg5  a5);
    MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,  MyTestArg3  a3,
                        MyTestArg4  a4,  MyTestArg5  a5,  MyTestArg6  a6);
    MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,  MyTestArg3  a3,
                        MyTestArg4  a4,  MyTestArg5  a5,  MyTestArg6  a6,
                        MyTestArg7  a7);
    MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,  MyTestArg3  a3,
                        MyTestArg4  a4,  MyTestArg5  a5,  MyTestArg6  a6,
                        MyTestArg7  a7,  MyTestArg8  a8);
    MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,  MyTestArg3  a3,
                        MyTestArg4  a4,  MyTestArg5  a5,  MyTestArg6  a6,
                        MyTestArg7  a7,  MyTestArg8  a8,  MyTestArg9  a9);
    MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,  MyTestArg3  a3,
                        MyTestArg4  a4,  MyTestArg5  a5,  MyTestArg6  a6,
                        MyTestArg7  a7,  MyTestArg8  a8,  MyTestArg9  a9,
                        MyTestArg10 a10);
    MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,  MyTestArg3  a3,
                        MyTestArg4  a4,  MyTestArg5  a5,  MyTestArg6  a6,
                        MyTestArg7  a7,  MyTestArg8  a8,  MyTestArg9  a9,
                        MyTestArg10 a10, MyTestArg11 a11);
    MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,  MyTestArg3  a3,
                        MyTestArg4  a4,  MyTestArg5  a5,  MyTestArg6  a6,
                        MyTestArg7  a7,  MyTestArg8  a8,  MyTestArg9  a9,
                        MyTestArg10 a10, MyTestArg11 a11, MyTestArg12 a12);
    MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,  MyTestArg3  a3,
                        MyTestArg4  a4,  MyTestArg5  a5,  MyTestArg6  a6,
                        MyTestArg7  a7,  MyTestArg8  a8,  MyTestArg9  a9,
                        MyTestArg10 a10, MyTestArg11 a11, MyTestArg12 a12,
                        MyTestArg13 a13);
    MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,  MyTestArg3  a3,
                        MyTestArg4  a4,  MyTestArg5  a5,  MyTestArg6  a6,
                        MyTestArg7  a7,  MyTestArg8  a8,  MyTestArg9  a9,
                        MyTestArg10 a10, MyTestArg11 a11, MyTestArg12 a12,
                        MyTestArg13 a13, MyTestArg14 a14);
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

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2,
                                         MyTestArg3 a3)
: d_a1(a1)
, d_a2(a2)
, d_a3(a3)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2,
                                         MyTestArg3 a3, MyTestArg4 a4)
: d_a1(a1)
, d_a2(a2)
, d_a3(a3)
, d_a4(a4)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2,
                                         MyTestArg3 a3, MyTestArg4 a4,
                                         MyTestArg5 a5)
: d_a1(a1)
, d_a2(a2)
, d_a3(a3)
, d_a4(a4)
, d_a5(a5)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2,
                                         MyTestArg3 a3, MyTestArg4 a4,
                                         MyTestArg5 a5, MyTestArg6 a6)
: d_a1(a1)
, d_a2(a2)
, d_a3(a3)
, d_a4(a4)
, d_a5(a5)
, d_a6(a6)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2,
                                         MyTestArg3 a3, MyTestArg4 a4,
                                         MyTestArg5 a5, MyTestArg6 a6,
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

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2,
                                         MyTestArg3 a3, MyTestArg4 a4,
                                         MyTestArg5 a5, MyTestArg6 a6,
                                         MyTestArg7 a7, MyTestArg8 a8)
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

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2,
                                         MyTestArg3 a3, MyTestArg4 a4,
                                         MyTestArg5 a5, MyTestArg6 a6,
                                         MyTestArg7 a7, MyTestArg8 a8,
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

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1 a1, MyTestArg2 a2,
                                         MyTestArg3 a3, MyTestArg4 a4,
                                         MyTestArg5 a5, MyTestArg6 a6,
                                         MyTestArg7 a7, MyTestArg8 a8,
                                         MyTestArg9 a9, MyTestArg10 a10)
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

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1  a1, MyTestArg2  a2,
                                         MyTestArg3  a3, MyTestArg4  a4,
                                         MyTestArg5  a5, MyTestArg6  a6,
                                         MyTestArg7  a7, MyTestArg8  a8,
                                         MyTestArg9  a9, MyTestArg10 a10,
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

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,
                                         MyTestArg3  a3,  MyTestArg4  a4,
                                         MyTestArg5  a5,  MyTestArg6  a6,
                                         MyTestArg7  a7,  MyTestArg8  a8,
                                         MyTestArg9  a9,  MyTestArg10 a10,
                                         MyTestArg11 a11, MyTestArg12 a12)
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

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,
                                         MyTestArg3  a3,  MyTestArg4  a4,
                                         MyTestArg5  a5,  MyTestArg6  a6,
                                         MyTestArg7  a7,  MyTestArg8  a8,
                                         MyTestArg9  a9,  MyTestArg10 a10,
                                         MyTestArg11 a11, MyTestArg12 a12,
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

MyInplaceTestObject::MyInplaceTestObject(MyTestArg1  a1,  MyTestArg2  a2,
                                         MyTestArg3  a3,  MyTestArg4  a4,
                                         MyTestArg5  a5,  MyTestArg6  a6,
                                         MyTestArg7  a7,  MyTestArg8  a8,
                                         MyTestArg9  a9,  MyTestArg10 a10,
                                         MyTestArg11 a11, MyTestArg12 a12,
                                         MyTestArg13 a13, MyTestArg14 a14)
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

// ACCESSORS
bool MyInplaceTestObject::operator==(const MyInplaceTestObject& rhs) const
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

int MyInplaceTestObject::getNumDeletes()
{
    return s_numDeletes;
}

                         // ==================
                         // struct TestHarness
                         // ==================

template <class ALLOCATOR>
struct TestHarness {

    // PUBLIC TYPES
    typedef typename bsl::allocator_traits<ALLOCATOR>::
                          template rebind_traits<MyTestObject> Obj_AllocTraits;
    typedef typename Obj_AllocTraits::allocator_type Obj_Alloc;

    typedef bslstl::SharedPtrAllocateInplaceRep<MyTestObject, Obj_Alloc> Obj;

    typedef typename bsl::allocator_traits<ALLOCATOR>::
                 template rebind_traits<MyInplaceTestObject> TCObj_AllocTraits;
    typedef typename TCObj_AllocTraits::allocator_type TCObj_ElementAlloc;

    typedef bslstl::SharedPtrAllocateInplaceRep<MyInplaceTestObject,
                                                    TCObj_ElementAlloc> TCObj;

    typedef typename TCObj::ReboundAllocator TCObj_Alloc;

    // CLASS METHODS
    static void testCase2(bool verbose,
                          bool veryVerbose,
                          bool veryVeryVerbose,
                          bool veryVeryVeryVerbose);
        // Implement test case 2 using the specified 'verbose', 'veryVerbose',
        // 'veryVeryVerbose' and 'veryVeryVeryVerbose' flags to control the
        // level of user feedback.  See the test case function for documented
        // concerns and test plan.

    static void testCase3(bool verbose,
                          bool veryVerbose,
                          bool veryVeryVerbose,
                          bool veryVeryVeryVerbose);
        // Implement test case 3 using the specified 'verbose', 'veryVerbose',
        // 'veryVeryVerbose' and 'veryVeryVeryVerbose' flags to control the
        // level of user feedback.  See the test case function for documented
        // concerns and test plan.


    static void testCase4(bool verbose,
                          bool veryVerbose,
                          bool veryVeryVerbose,
                          bool veryVeryVeryVerbose);
        // Implement test case 4 using the specified 'verbose', 'veryVerbose',
        // 'veryVeryVerbose' and 'veryVeryVeryVerbose' flags to control the
        // level of user feedback.  See the test case function for documented
        // concerns and test plan.


    static void testCase5(bool verbose,
                          bool veryVerbose,
                          bool veryVeryVerbose,
                          bool veryVeryVeryVerbose);
        // Implement test case 5 using the specified 'verbose', 'veryVerbose',
        // 'veryVeryVerbose' and 'veryVeryVeryVerbose' flags to control the
        // level of user feedback.  See the test case function for documented
        // concerns and test plan.

    static void testCase6(bool verbose,
                          bool veryVerbose,
                          bool veryVeryVerbose,
                          bool veryVeryVeryVerbose);
        // Implement test case 6 using the specified 'verbose', 'veryVerbose',
        // 'veryVeryVerbose' and 'veryVeryVeryVerbose' flags to control the
        // level of user feedback.  See the test case function for documented
        // concerns and test plan.

};

template <class ALLOCATOR>
void TestHarness<ALLOCATOR>::testCase2(bool verbose,
                                       bool veryVerbose,
                                       bool veryVeryVerbose,
                                       bool veryVeryVeryVerbose)
{
    // --------------------------------------------------------------------
    // TESTING BASIC CONSTRUCTOR
    //
    // Concerns:
    //   Verify that upon construction the object is properly initialized.
    //
    // Plan:
    //   Construct object using basic constructor and verify that accessors
    //   return the expected values.
    //
    // Testing:
    //   bslstl::SharedPtrAllocateInplaceRep(bslma::Allocator *basicAllocator);
    //   TYPE *ptr();
    //   void disposeRep();
    //   void *originalPtr() const;
    // --------------------------------------------------------------------

    if (verbose) printf("\nTesting Constructor"
                        "\n-------------------\n");

    (void)veryVerbose;
    (void)veryVeryVerbose;

    bslma::TestAllocator ta("Tese case 2", veryVeryVeryVerbose);
    ALLOCATOR            alloc_base(&ta);

    Obj_Alloc   alloc1(alloc_base);
    TCObj_Alloc alloc2(&ta);

    bsls::Types::Int64 numAllocations   = ta.numAllocations();
    bsls::Types::Int64 numDeallocations = ta.numDeallocations();
    {
        static const MyInplaceTestObject EXP;

        // Dynamically allocate object as the destructor is declared as
        // private.

        TCObj *xPtr = TCObj::makeRep(alloc_base);

//        TCObj* xPtr = new(ta) TCObj(&ta);
        TCObj&       x = *xPtr;

        ASSERT(++numAllocations == ta.numAllocations());
//        ASSERT(EXP == *(x.ptr()));
        ASSERT(x.originalPtr() == static_cast<void*>(x.ptr()));

        // Manually deallocate the representation using 'disposeRep'.

        x.disposeRep();

        ASSERT(++numDeallocations == ta.numDeallocations());
    }

}

template <class ALLOCATOR>
void TestHarness<ALLOCATOR>::testCase3(bool verbose,
                                       bool veryVerbose,
                                       bool veryVeryVerbose,
                                       bool veryVeryVeryVerbose)
{
    // --------------------------------------------------------------------
    // TESTING CONSTRUCTOR
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
    //   bslstl::SharedPtrAllocateInplaceRep(const ALLOCATOR& a);
    //   bslstl::SharedPtrAllocateInplaceRep(const ALLOCATOR& a, ...a2);
    //   bslstl::SharedPtrAllocateInplaceRep(const ALLOCATOR& a, ...a3);
    //   bslstl::SharedPtrAllocateInplaceRep(const ALLOCATOR& a, ...a4);
    //   bslstl::SharedPtrAllocateInplaceRep(const ALLOCATOR& a, ...a5);
    //   bslstl::SharedPtrAllocateInplaceRep(const ALLOCATOR& a, ...a6);
    //   bslstl::SharedPtrAllocateInplaceRep(const ALLOCATOR& a, ...a7);
    //   bslstl::SharedPtrAllocateInplaceRep(const ALLOCATOR& a, ...a8);
    //   bslstl::SharedPtrAllocateInplaceRep(const ALLOCATOR& a, ...a9);
    //   bslstl::SharedPtrAllocateInplaceRep(const ALLOCATOR& a, ...a10);
    //   bslstl::SharedPtrAllocateInplaceRep(const ALLOCATOR& a, ...a11);
    //   bslstl::SharedPtrAllocateInplaceRep(const ALLOCATOR& a, ...a12);
    //   bslstl::SharedPtrAllocateInplaceRep(const ALLOCATOR& a, ...a13);
    //   bslstl::SharedPtrAllocateInplaceRep(const ALLOCATOR& a, ...a14);
    // --------------------------------------------------------------------
    if (verbose) printf("\nTesting constructor\n"
                        "\n===================\n");

    (void)veryVerbose;
    (void)veryVeryVerbose;
    (void)veryVeryVeryVerbose;

    // TBD rewrite this test case
    if (verbose) printf("\nTBD\n");

#if 0
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

    bslma::TestAllocator ta("Tese case 3", veryVeryVeryVerbose);
    ALLOCATOR alloc_base(&ta);

    Obj_Alloc alloc1(alloc_base);
    TCObj_Alloc alloc2(&ta);

    if (verbose) printf("\nTesting constructor with no arguments"
                        "\n-------------------------------------\n");

    bsls::Types::Int64 numAllocations = ta.numAllocations();
    bsls::Types::Int64 numDeallocations = ta.numDeallocations();
    {
        static const MyInplaceTestObject EXP = MyInplaceTestObject();
        TCObj* xPtr = alloc2.allocate(1);
        bsl::allocator_traits<TCObj_Alloc>::construct(alloc2, xPtr, alloc2);
        TCObj& x = *xPtr;

        ASSERT(++numAllocations == ta.numAllocations());
        ASSERT(EXP == *(x.ptr()));
        x.disposeRep();
        ASSERT(++numDeallocations == ta.numDeallocations());
    }


    if (verbose) printf("\nTesting constructor with 14 arguments"
                        "\n-------------------------------------\n");

    numAllocations = ta.numAllocations();
    numDeallocations = ta.numDeallocations();
    {
        static const MyInplaceTestObject EXP(V1, V2, V3, V4, V5, V6, V7,
                                              V8, V9, V10);
        TCObj* xPtr = new(ta) TCObj(alloc2, V1, V2, V3, V4, V5, V6, V7, V8,
                                                  V9, V104);
        TCObj& x = *xPtr;

        ASSERT(++numAllocations == ta.numAllocations());
        ASSERT(EXP == *(x.ptr()));
        x.disposeRep();
        ASSERT(++numDeallocations == ta.numDeallocations());
    }
#endif
}

template <class ALLOCATOR>
void TestHarness<ALLOCATOR>::testCase4(bool verbose,
                                       bool veryVerbose,
                                       bool veryVeryVerbose,
                                       bool veryVeryVeryVerbose)
{
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
    if (verbose) printf("\nTesting disposeObject"
                        "\n---------------------\n");

    (void)veryVerbose;
    (void)veryVeryVerbose;

    bslma::TestAllocator ta("Tese case 4", veryVeryVeryVerbose);
    ALLOCATOR            alloc_base(&ta);

    Obj_Alloc   alloc1(alloc_base);
    TCObj_Alloc alloc2(&ta);

    bsls::Types::Int64 numAllocations   = ta.numAllocations();
    bsls::Types::Int64 numDeallocations = ta.numDeallocations();
    {
        bsls::Types::Int64 numDeletes = 0;

        Obj        *xPtr = Obj::makeRep(alloc_base);
        Obj&        x    = *xPtr;

        ASSERT(++numAllocations == ta.numAllocations());
        ASSERT(0 == numDeletes);

        new(x.ptr()) MyTestObject(&numDeletes);

        x.disposeObject();
        ASSERT(1 == numDeletes);

        x.disposeRep();
        ASSERT(++numDeallocations == ta.numDeallocations());
    }
}

template <class ALLOCATOR>
void TestHarness<ALLOCATOR>::testCase5(bool verbose,
                                       bool veryVerbose,
                                       bool veryVeryVerbose,
                                       bool veryVeryVeryVerbose)
{
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

    if (verbose) printf("\nTesting 'releaseRef' and 'releaseWeakRef'"
                        "\n=========================================\n");

    (void)veryVerbose;
    (void)veryVeryVerbose;

    bslma::TestAllocator ta("Tese case 5", veryVeryVeryVerbose);
    ALLOCATOR            alloc_base(&ta);

    Obj_Alloc   alloc1(alloc_base);
    TCObj_Alloc alloc2(&ta);

    bsls::Types::Int64 numAllocations   = ta.numAllocations();
    bsls::Types::Int64 numDeallocations = ta.numDeallocations();
    {
        bsls::Types::Int64 numDeletes = 0;

        Obj        *xPtr = Obj::makeRep(alloc_base);
        Obj&        x    = *xPtr;
        const Obj&  X    = *xPtr;

        ASSERT(++numAllocations == ta.numAllocations());

        new(x.ptr()) MyTestObject(&numDeletes);

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
        bsls::Types::Int64 numDeletes = 0;

        Obj        *xPtr = Obj::makeRep(alloc_base);
        Obj&        x    = *xPtr;
        const Obj&  X    = *xPtr;

        ASSERT(++numAllocations == ta.numAllocations());

        new(x.ptr()) MyTestObject(&numDeletes);

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
}

template <class ALLOCATOR>
void TestHarness<ALLOCATOR>::testCase6(bool verbose,
                                       bool veryVerbose,
                                       bool veryVeryVerbose,
                                       bool veryVeryVeryVerbose)
{
    // --------------------------------------------------------------------
    // TESTING 'getDeleter'
    //
    // Concerns:
    //   1) 'getDeleter' overrides the pure virtual function in the base
    //      class.
    //   2) 'getDeleter' returns a null pointer when called, regardless of the
    //      supplied 'type_info'.
    //
    // Plan:
    //   1) ...
    //
    // Testing:
    //   void *getDeleter(const std::type_info& type);
    // --------------------------------------------------------------------

    if (verbose) printf("\nTESTING 'getDeleter'"
                        "\n====================\n");

    (void)veryVerbose;
    (void)veryVeryVerbose;

    bslma::TestAllocator ta("Tese case 5", veryVeryVeryVerbose);
    ALLOCATOR            alloc_base(&ta);

    Obj_Alloc   alloc1(alloc_base);
    TCObj_Alloc alloc2(&ta);

    bsls::Types::Int64 numDeallocations = ta.numDeallocations();
    {
        bsls::Types::Int64 numDeletes = 0;

        Obj  *xPtr = Obj::makeRep(alloc_base);
        Obj&  x    = *xPtr;

        ASSERT(0 == x.getDeleter(typeid(int)));

        new(x.ptr()) MyTestObject(&numDeletes);

        ASSERT(0 == x.getDeleter(typeid(int)));

        x.releaseRef();

        ASSERT(1 == numDeletes);
        ASSERT(++numDeallocations == ta.numDeallocations());
    }
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                test = argc > 1 ? atoi(argv[1]) : 0;
    int             verbose = argc > 2;
    int         veryVerbose = argc > 3;
    int     veryVeryVerbose = argc > 4;
    int veryVeryVeryVerbose = argc > 5;

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // Confirm no static initialization locked the global allocator
    ASSERT(&globalAllocator == bslma::Default::globalAllocator());

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    // Confirm no static initialization locked the default allocator
    ASSERT(&defaultAllocator == bslma::Default::defaultAllocator());

    bslma::TestAllocator ta(veryVeryVeryVerbose);
    bsls::Types::Int64   numDeallocations;
    bsls::Types::Int64   numAllocations;

    (void) numDeallocations;
    (void) numAllocations;
    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
#if 0  // TBD Need an appropriately levelized usage example
   case 7: {
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
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'getDeleter'
        //
        // Concerns:
        //   1) 'getDeleter' overrides the pure virtual function in the base
        //      class.
        //   2) 'getDeleter' returns a null pointer when called, regardless of
        //      the supplied 'type_info'.
        //
        // Plan:
        //   1) ...
        //
        // Testing:
        //   void *getDeleter(const std::type_info& type);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'getDeleter'"
                            "\n====================\n");

        using BloombergLP::bsltf::StdStatefulAllocator;

        typedef bsl::allocator<int> ALLOC_1;
        typedef StdStatefulAllocator<int, true, true, true, true> ALLOC_2;
        typedef StdStatefulAllocator<int, false, false, false, false> ALLOC_3;

        typedef TestHarness<ALLOC_1> T1;
        typedef TestHarness<ALLOC_2> T2;
        typedef TestHarness<ALLOC_3> T3;

        T1::testCase6(verbose,
                      veryVerbose,
                      veryVeryVerbose,
                      veryVeryVeryVerbose);
        T2::testCase6(verbose,
                      veryVerbose,
                      veryVeryVerbose,
                      veryVeryVeryVerbose);
        T3::testCase6(verbose,
                      veryVerbose,
                      veryVeryVerbose,
                      veryVeryVeryVerbose);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'releaseRef' AND 'releaseWeakRef'
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

        if (verbose) printf("\nTESTING 'releaseRef' AND 'releaseWeakRef'"
                            "\n=========================================\n");

#if 0
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
#else
        using BloombergLP::bsltf::StdStatefulAllocator;

        typedef bsl::allocator<int> ALLOC_1;
        typedef StdStatefulAllocator<int, true, true, true, true> ALLOC_2;
        typedef StdStatefulAllocator<int, false, false, false, false> ALLOC_3;

        typedef TestHarness<ALLOC_1> T1;
        typedef TestHarness<ALLOC_2> T2;
        typedef TestHarness<ALLOC_3> T3;

        T1::testCase5(verbose,
                      veryVerbose,
                      veryVeryVerbose,
                      veryVeryVeryVerbose);
        T2::testCase5(verbose,
                      veryVerbose,
                      veryVeryVerbose,
                      veryVeryVeryVerbose);
        T3::testCase5(verbose,
                      veryVerbose,
                      veryVeryVerbose,
                      veryVeryVeryVerbose);
#endif
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

#if 0
        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            int numDeletes = 0;
            Obj* xPtr = new(ta) Obj(&ta, &numDeletes);
            Obj& x = *xPtr;
            Obj const& X = *xPtr;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(0 == numDeletes);

            x.disposeObject();
            ASSERT(1 == numDeletes);

            x.disposeRep();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }
#else
        using BloombergLP::bsltf::StdStatefulAllocator;

        typedef bsl::allocator<int> ALLOC_1;
        typedef StdStatefulAllocator<int, true, true, true, true> ALLOC_2;
        typedef StdStatefulAllocator<int, false, false, false, false> ALLOC_3;

        typedef TestHarness<ALLOC_1> T1;
        typedef TestHarness<ALLOC_2> T2;
        typedef TestHarness<ALLOC_3> T3;

        T1::testCase4(verbose,
                      veryVerbose,
                      veryVeryVerbose,
                      veryVeryVeryVerbose);
        T2::testCase4(verbose,
                      veryVerbose,
                      veryVeryVerbose,
                      veryVeryVeryVerbose);
        T3::testCase4(verbose,
                      veryVerbose,
                      veryVeryVerbose,
                      veryVeryVeryVerbose);
#endif
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING ARGUMENT-FORWARDING CONSTRUCTORS
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
        //   bslstl::SharedPtrAllocateInplaceRep(const ALLOCATOR& a, ...a1);
        //   bslstl::SharedPtrAllocateInplaceRep(const ALLOCATOR& a, ...a2);
        //   bslstl::SharedPtrAllocateInplaceRep(const ALLOCATOR& a, ...a3);
        //   bslstl::SharedPtrAllocateInplaceRep(const ALLOCATOR& a, ...a4);
        //   bslstl::SharedPtrAllocateInplaceRep(const ALLOCATOR& a, ...a5);
        //   bslstl::SharedPtrAllocateInplaceRep(const ALLOCATOR& a, ...a6);
        //   bslstl::SharedPtrAllocateInplaceRep(const ALLOCATOR& a, ...a7);
        //   bslstl::SharedPtrAllocateInplaceRep(const ALLOCATOR& a, ...a8);
        //   bslstl::SharedPtrAllocateInplaceRep(const ALLOCATOR& a, ...a9);
        //   bslstl::SharedPtrAllocateInplaceRep(const ALLOCATOR& a, ...a10);
        //   bslstl::SharedPtrAllocateInplaceRep(const ALLOCATOR& a, ...a11);
        //   bslstl::SharedPtrAllocateInplaceRep(const ALLOCATOR& a, ...a12);
        //   bslstl::SharedPtrAllocateInplaceRep(const ALLOCATOR& a, ...a13);
        //   bslstl::SharedPtrAllocateInplaceRep(const ALLOCATOR& a, ...a14);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ARGUMENT-FORWARDING CONSTRUCTORS"
                            "\n========================================\n");

        using BloombergLP::bsltf::StdStatefulAllocator;

        typedef bsl::allocator<int> ALLOC_1;
        typedef StdStatefulAllocator<int, true, true, true, true> ALLOC_2;
        typedef StdStatefulAllocator<int, false, false, false, false> ALLOC_3;

        typedef TestHarness<ALLOC_1> T1;
        typedef TestHarness<ALLOC_2> T2;
        typedef TestHarness<ALLOC_3> T3;

        T1::testCase3(verbose,
                      veryVerbose,
                      veryVeryVerbose,
                      veryVeryVeryVerbose);
        T2::testCase3(verbose,
                      veryVerbose,
                      veryVeryVerbose,
                      veryVeryVeryVerbose);
        T3::testCase3(verbose,
                      veryVerbose,
                      veryVeryVerbose,
                      veryVeryVeryVerbose);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING BASIC CONSTRUCTOR
        //
        // Concerns:
        //   Verify that upon construction the object is properly initialized.
        //
        // Plan:
        //   Construct object using basic constructor and verify that accessors
        //   return the expected values.
        //
        // Testing:
        //   SharedPtrAllocateInplaceRep(const ReboundAllocator& alloc);
        //   SharedPtrAllocateInplaceRep makeRep(ReboundAllocator alloc);
        //   TYPE *ptr();
        //   void disposeRep();
        //   void *originalPtr() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BASIC CONSTRUCTOR"
                            "\n=========================\n");

#if 0
        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP;

            // Dynamically allocate object as the destructor is declared as
            // private.

            TCObj* xPtr = new(ta) TCObj(&ta);
            TCObj& x = *xPtr;
            TCObj const& X = *xPtr;

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *(x.ptr()));
            ASSERT(x.originalPtr() == static_cast<void*>(x.ptr()));

            // Manually deallocate the representation using 'disposeRep'.

            x.disposeRep();

            ASSERT(++numDeallocations == ta.numDeallocations());
        }
#else
        using BloombergLP::bsltf::StdStatefulAllocator;

        typedef bsl::allocator<int> ALLOC_1;
        typedef StdStatefulAllocator<int, true, true, true, true> ALLOC_2;
        typedef StdStatefulAllocator<int, false, false, false, false> ALLOC_3;

        typedef TestHarness<ALLOC_1> T1;
        typedef TestHarness<ALLOC_2> T2;
        typedef TestHarness<ALLOC_3> T3;

        T1::testCase2(verbose,
                      veryVerbose,
                      veryVeryVerbose,
                      veryVeryVeryVerbose);
        T2::testCase2(verbose,
                      veryVerbose,
                      veryVeryVerbose,
                      veryVeryVeryVerbose);
        T3::testCase2(verbose,
                      veryVerbose,
                      veryVeryVerbose,
                      veryVeryVeryVerbose);
#endif
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
// Copyright 2014 Bloomberg Finance L.P.
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
