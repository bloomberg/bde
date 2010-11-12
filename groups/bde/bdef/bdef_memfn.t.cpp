// bdef_memfn.t.cpp                -*-C++-*-

#include <bdef_memfn.h>

#include <bslalg_typetraithaspointersemantics.h>
#include <bslalg_typetraits.h>

#include <bslma_default.h>                      // for testing only
#include <bslma_defaultallocatorguard.h>        // for testing only
#include <bslma_testallocator.h>                // for testing only

#include <bsl_algorithm.h>    // bsl::find_if, bsl::for_each
#include <bsl_iostream.h>     // bsl::cout
#include <bsl_list.h>         // bsl::list

#include <bsl_c_stdlib.h>     // atoi()

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The 'bdef_MemFn' class holds a pointer to a member function, and the
// 'bdef_MemFnInstance' holds the same plus an object which either has pointer
// semantics.  Our main objective is to make sure that the member function
// wrapper object can be initialized with different kinds of invocables,
// including smart-pointer like objects which have pointer semantics, that the
// wrapper forwards invocation arguments properly, and returns the correct
// value.  In addition, we want to verify that the 'bslmf_MemFnInstance'
// wrapper propagates its allocator to the instance it holds, if that instance
// takes a 'bdema' allocator.  Finally, we also have concerns that
// 'const'-correctness is respected, i.e., a non-'const' member function cannot
// be involved on a non-modifiable instance.
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] 'bdef_MemFn' INVOCATION WITH POINTER TO MODIFIABLE OBJECT
// [ 3] 'bdef_MemFn' INVOCATION WITH POINTER TO NON-MODIFIABLE OBJECT
// [ 4] 'bdef_MemFn' INVOCATION WITH REFERENCE TO MODIFIABLE OBJECT
// [ 5] 'bdef_MemFn' INVOCATION WITH REFERENCE TO NON-MODIFIABLE OBJECT
// [ 6] 'bdef_MemFnInstance' INVOCATION WITH PTR TO MODIFIABLE OBJECT
// [ 7] 'bdef_MemFnInstance' INVOCATION WITH NON-MODIFIABLE INSTANCE
// [ 8] INSTANCES WITH POINTER SEMANTICS
// [ 9] IMPLICIT CONVERSION FROM POINTER-TO-TYPE (DRQS 13973002)
// [10] USAGE EXAMPLE

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
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

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
        if (!(X)) { cout << #I << " = " << I << endl; \
                    aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
        if (!(X)) { cout << #I << " = " << I << ", " \
                         << #J << " = " << J << endl; \
                            aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

struct TestObject {
    int test1 (int a1) { return a1; }
    int test1Const (int a1) const { return a1; }
    int test2 (int a1, const char* a2) { return a1; }
};

#define DECLARE_TEST_ARG(name)                                                \
struct name {                                                                 \
    int d_value;                                                              \
    name() : d_value(-1) {}                                                   \
    name(int value) : d_value(value) {}                                       \
    name& operator= (const name &rhs) { return d_value = rhs.d_value, *this;} \
    bool operator== (const name &rhs) { return d_value == rhs.d_value; }      \
};

DECLARE_TEST_ARG(TestArg1)
DECLARE_TEST_ARG(TestArg2)
DECLARE_TEST_ARG(TestArg3)
DECLARE_TEST_ARG(TestArg4)
DECLARE_TEST_ARG(TestArg5)
DECLARE_TEST_ARG(TestArg6)
DECLARE_TEST_ARG(TestArg7)
DECLARE_TEST_ARG(TestArg8)
DECLARE_TEST_ARG(TestArg9)
DECLARE_TEST_ARG(TestArg10)
DECLARE_TEST_ARG(TestArg11)
DECLARE_TEST_ARG(TestArg12)
DECLARE_TEST_ARG(TestArg13)
DECLARE_TEST_ARG(TestArg14)

                            // ====================
                            // class InplaceTestObj
                            // ====================

class InplaceTestObj {
    // This class is an invocable with any number from 0 up to 14 arguments, of
    // distinct types 'TestArg1' up to 'TestArg14'.  Invoking the test function
    // with 'N' arguments on a modifiable (resp. non-modifiable) instance will
    // set the first 'N' instance (resp. class) data members.  The class data
    // members can be reset to -1 (their default value) using the 'reset' class
    // method.

    // DATA
    TestArg1  d_a1;
    TestArg2  d_a2;
    TestArg3  d_a3;
    TestArg4  d_a4;
    TestArg5  d_a5;
    TestArg6  d_a6;
    TestArg7  d_a7;
    TestArg8  d_a8;
    TestArg9  d_a9;
    TestArg10 d_a10;
    TestArg11 d_a11;
    TestArg12 d_a12;
    TestArg13 d_a13;
    TestArg14 d_a14;

  public:
    // CLASS DATA
    static TestArg1  s_a1;
    static TestArg2  s_a2;
    static TestArg3  s_a3;
    static TestArg4  s_a4;
    static TestArg5  s_a5;
    static TestArg6  s_a6;
    static TestArg7  s_a7;
    static TestArg8  s_a8;
    static TestArg9  s_a9;
    static TestArg10 s_a10;
    static TestArg11 s_a11;
    static TestArg12 s_a12;
    static TestArg13 s_a13;
    static TestArg14 s_a14;

    // CLASS METHODS
    static void reset()
    {
        s_a1  = -1;
        s_a2  = -1;
        s_a3  = -1;
        s_a4  = -1;
        s_a5  = -1;
        s_a6  = -1;
        s_a7  = -1;
        s_a8  = -1;
        s_a9  = -1;
        s_a10 = -1;
        s_a11 = -1;
        s_a12 = -1;
        s_a13 = -1;
        s_a14 = -1;
    }

    // CREATORS
    InplaceTestObj(TestArg1  a1  = -1, TestArg2  a2  = -1, TestArg3  a3  = -1,
                   TestArg4  a4  = -1, TestArg5  a5  = -1, TestArg6  a6  = -1,
                   TestArg7  a7  = -1, TestArg8  a8  = -1, TestArg9  a9  = -1,
                   TestArg10 a10 = -1, TestArg11 a11 = -1, TestArg12 a12 = -1,
                   TestArg13 a13 = -1, TestArg14 a14 = -1)
    : d_a1 (a1 )
    , d_a2 (a2 )
    , d_a3 (a3 )
    , d_a4 (a4 )
    , d_a5 (a5 )
    , d_a6 (a6 )
    , d_a7 (a7 )
    , d_a8 (a8 )
    , d_a9 (a9 )
    , d_a10(a10)
    , d_a11(a11)
    , d_a12(a12)
    , d_a13(a13)
    , d_a14(a14)
    {
    }

    // CLASS METHODS (continued)
    static InplaceTestObj statics()
    {
        return InplaceTestObj(s_a1, s_a2, s_a3,  s_a4,  s_a5,  s_a6,  s_a7,
                              s_a8, s_a9, s_a10, s_a11, s_a12, s_a13, s_a14);
    }

    // MANIPULATORS
    int testFunc0()
    {
        return 0;
    }

    int testFunc1(TestArg1 a1)
    {
        d_a1 = a1;
        return 1;
    }

    int  testFunc2(TestArg1 const &a1, TestArg2 const &a2)
    {
        d_a1 = a1;
        d_a2 = a2;
        return 2;
    }

    int testFunc3(TestArg1 a1, TestArg2 a2, TestArg3 a3)
    {
        d_a1 = a1;
        d_a2 = a2;
        d_a3 = a3;
        return 3;
    }

    int testFunc4(TestArg1 a1, TestArg2 a2, TestArg3 a3, TestArg4 a4)
    {
        d_a1 = a1;
        d_a2 = a2;
        d_a3 = a3;
        d_a4 = a4;
        return 4;
    }

    int testFunc5(TestArg1 a1, TestArg2 a2, TestArg3 a3, TestArg4 a4,
                  TestArg5 a5)
    {
        d_a1 = a1;
        d_a2 = a2;
        d_a3 = a3;
        d_a4 = a4;
        d_a5 = a5;
        return 5;
    }

    int testFunc6(TestArg1 a1, TestArg2 a2, TestArg3 a3, TestArg4 a4,
                  TestArg5 a5, TestArg6 a6)
    {
        d_a1 = a1;
        d_a2 = a2;
        d_a3 = a3;
        d_a4 = a4;
        d_a5 = a5;
        d_a6 = a6;
        return 6;
    }

    int testFunc7(TestArg1 a1, TestArg2 a2, TestArg3 a3, TestArg4 a4,
                  TestArg5 a5, TestArg6 a6, TestArg7 a7)
    {
        d_a1 = a1;
        d_a2 = a2;
        d_a3 = a3;
        d_a4 = a4;
        d_a5 = a5;
        d_a6 = a6;
        d_a7 = a7;
        return 7;
    }

    int testFunc8(TestArg1 a1, TestArg2 a2, TestArg3 a3, TestArg4 a4,
                  TestArg5 a5, TestArg6 a6, TestArg7 a7, TestArg8 a8)
    {
        d_a1 = a1;
        d_a2 = a2;
        d_a3 = a3;
        d_a4 = a4;
        d_a5 = a5;
        d_a6 = a6;
        d_a7 = a7;
        d_a8 = a8;
        return 8;
    }

    int testFunc9(TestArg1 a1, TestArg2 a2, TestArg3 a3, TestArg4 a4,
                  TestArg5 a5, TestArg6 a6, TestArg7 a7, TestArg8 a8,
                  TestArg9 a9)
    {
        d_a1 = a1;
        d_a2 = a2;
        d_a3 = a3;
        d_a4 = a4;
        d_a5 = a5;
        d_a6 = a6;
        d_a7 = a7;
        d_a8 = a8;
        d_a9 = a9;
        return 9;
    }

    int testFunc10(TestArg1 a1, TestArg2  a2, TestArg3 a3, TestArg4 a4,
                   TestArg5 a5, TestArg6  a6, TestArg7 a7, TestArg8 a8,
                   TestArg9 a9, TestArg10 a10)
    {
        d_a1  = a1;
        d_a2  = a2;
        d_a3  = a3;
        d_a4  = a4;
        d_a5  = a5;
        d_a6  = a6;
        d_a7  = a7;
        d_a8  = a8;
        d_a9  = a9;
        d_a10 = a10;
        return 10;
    }

    int testFunc11(TestArg1 a1, TestArg2  a2,  TestArg3  a3, TestArg4 a4,
                   TestArg5 a5, TestArg6  a6,  TestArg7  a7, TestArg8 a8,
                   TestArg9 a9, TestArg10 a10, TestArg11 a11)
    {
        d_a1  = a1;
        d_a2  = a2;
        d_a3  = a3;
        d_a4  = a4;
        d_a5  = a5;
        d_a6  = a6;
        d_a7  = a7;
        d_a8  = a8;
        d_a9  = a9;
        d_a10 = a10;
        d_a11 = a11;
        return 11;
    }

    int testFunc12(TestArg1 a1, TestArg2  a2,  TestArg3  a3,  TestArg4  a4,
                   TestArg5 a5, TestArg6  a6,  TestArg7  a7,  TestArg8  a8,
                   TestArg9 a9, TestArg10 a10, TestArg11 a11, TestArg12 a12)
    {
        d_a1  = a1;
        d_a2  = a2;
        d_a3  = a3;
        d_a4  = a4;
        d_a5  = a5;
        d_a6  = a6;
        d_a7  = a7;
        d_a8  = a8;
        d_a9  = a9;
        d_a10 = a10;
        d_a11 = a11;
        d_a12 = a12;
        return 12;
    }

    int testFunc13(TestArg1 a1, TestArg2  a2,  TestArg3  a3,  TestArg4  a4,
                   TestArg5 a5, TestArg6  a6,  TestArg7  a7,  TestArg8  a8,
                   TestArg9 a9, TestArg10 a10, TestArg11 a11, TestArg12 a12,
                   TestArg13 a13)
    {
        d_a1  = a1;
        d_a2  = a2;
        d_a3  = a3;
        d_a4  = a4;
        d_a5  = a5;
        d_a6  = a6;
        d_a7  = a7;
        d_a8  = a8;
        d_a9  = a9;
        d_a10 = a10;
        d_a11 = a11;
        d_a12 = a12;
        d_a13 = a13;
        return 13;
    }

    int testFunc14(TestArg1  a1,  TestArg2  a2,  TestArg3  a3,  TestArg4  a4,
                   TestArg5  a5,  TestArg6  a6,  TestArg7  a7,  TestArg8  a8,
                   TestArg9  a9,  TestArg10 a10, TestArg11 a11, TestArg12 a12,
                   TestArg13 a13, TestArg14 a14)
    {
        d_a1  = a1;
        d_a2  = a2;
        d_a3  = a3;
        d_a4  = a4;
        d_a5  = a5;
        d_a6  = a6;
        d_a7  = a7;
        d_a8  = a8;
        d_a9  = a9;
        d_a10 = a10;
        d_a11 = a11;
        d_a12 = a12;
        d_a13 = a13;
        d_a14 = a14;
        return 14;
    }

    // ACCESSORS
    bool operator==(InplaceTestObj const &rhs) const
    {
        return d_a1.d_value  == rhs.d_a1.d_value &&
               d_a1.d_value  == rhs.d_a1.d_value &&
               d_a2.d_value  == rhs.d_a2.d_value &&
               d_a3.d_value  == rhs.d_a3.d_value &&
               d_a4.d_value  == rhs.d_a4.d_value &&
               d_a5.d_value  == rhs.d_a5.d_value &&
               d_a6.d_value  == rhs.d_a6.d_value &&
               d_a7.d_value  == rhs.d_a7.d_value &&
               d_a8.d_value  == rhs.d_a8.d_value &&
               d_a9.d_value == rhs.d_a9.d_value &&
               d_a10.d_value == rhs.d_a10.d_value &&
               d_a11.d_value == rhs.d_a11.d_value &&
               d_a12.d_value == rhs.d_a12.d_value &&
               d_a13.d_value == rhs.d_a13.d_value &&
               d_a14.d_value == rhs.d_a14.d_value;
    }

    int testCFunc0() const
    {
        return 0;
    }

    int testCFunc1(TestArg1 a1) const
    {
        s_a1 = a1;
        return 1;
    }

    int  testCFunc2(TestArg1 const &a1, TestArg2 const &a2) const
    {
        s_a1 = a1;
        s_a2 = a2;
        return 2;
    }

    int testCFunc3(TestArg1 a1, TestArg2 a2, TestArg3 a3) const
    {
        s_a1 = a1;
        s_a2 = a2;
        s_a3 = a3;
        return 3;
    }

    int testCFunc4(TestArg1 a1, TestArg2 a2, TestArg3 a3, TestArg4 a4) const
    {
        s_a1 = a1;
        s_a2 = a2;
        s_a3 = a3;
        s_a4 = a4;
        return 4;
    }

    int testCFunc5(TestArg1 a1, TestArg2 a2, TestArg3 a3, TestArg4 a4,
                   TestArg5 a5) const
    {
        s_a1 = a1;
        s_a2 = a2;
        s_a3 = a3;
        s_a4 = a4;
        s_a5 = a5;
        return 5;
    }

    int testCFunc6(TestArg1 a1, TestArg2 a2, TestArg3 a3, TestArg4 a4,
                   TestArg5 a5, TestArg6 a6) const
    {
        s_a1 = a1;
        s_a2 = a2;
        s_a3 = a3;
        s_a4 = a4;
        s_a5 = a5;
        s_a6 = a6;
        return 6;
    }

    int testCFunc7(TestArg1 a1, TestArg2 a2, TestArg3 a3, TestArg4 a4,
                   TestArg5 a5, TestArg6 a6, TestArg7 a7) const
    {
        s_a1 = a1;
        s_a2 = a2;
        s_a3 = a3;
        s_a4 = a4;
        s_a5 = a5;
        s_a6 = a6;
        s_a7 = a7;
        return 7;
    }

    int testCFunc8(TestArg1 a1, TestArg2 a2, TestArg3 a3, TestArg4 a4,
                   TestArg5 a5, TestArg6 a6, TestArg7 a7, TestArg8 a8) const
    {
        s_a1 = a1;
        s_a2 = a2;
        s_a3 = a3;
        s_a4 = a4;
        s_a5 = a5;
        s_a6 = a6;
        s_a7 = a7;
        s_a8 = a8;
        return 8;
    }

    int testCFunc9(TestArg1 a1, TestArg2 a2, TestArg3 a3, TestArg4 a4,
                   TestArg5 a5, TestArg6 a6, TestArg7 a7, TestArg8 a8,
                   TestArg9 a9) const
    {
        s_a1 = a1;
        s_a2 = a2;
        s_a3 = a3;
        s_a4 = a4;
        s_a5 = a5;
        s_a6 = a6;
        s_a7 = a7;
        s_a8 = a8;
        s_a9 = a9;
        return 9;
    }

    int testCFunc10(TestArg1 a1, TestArg2  a2, TestArg3 a3, TestArg4 a4,
                    TestArg5 a5, TestArg6  a6, TestArg7 a7, TestArg8 a8,
                    TestArg9 a9, TestArg10 a10) const
    {
        s_a1  = a1;
        s_a2  = a2;
        s_a3  = a3;
        s_a4  = a4;
        s_a5  = a5;
        s_a6  = a6;
        s_a7  = a7;
        s_a8  = a8;
        s_a9  = a9;
        s_a10 = a10;
        return 10;
    }

    int testCFunc11(TestArg1 a1, TestArg2  a2,  TestArg3  a3, TestArg4 a4,
                    TestArg5 a5, TestArg6  a6,  TestArg7  a7, TestArg8 a8,
                    TestArg9 a9, TestArg10 a10, TestArg11 a11) const
    {
        s_a1  = a1;
        s_a2  = a2;
        s_a3  = a3;
        s_a4  = a4;
        s_a5  = a5;
        s_a6  = a6;
        s_a7  = a7;
        s_a8  = a8;
        s_a9  = a9;
        s_a10 = a10;
        s_a11 = a11;
        return 11;
    }

    int testCFunc12(TestArg1 a1, TestArg2  a2,  TestArg3  a3,  TestArg4  a4,
                    TestArg5 a5, TestArg6  a6,  TestArg7  a7,  TestArg8  a8,
                    TestArg9 a9, TestArg10 a10, TestArg11 a11, TestArg12 a12)
                                                                          const
    {
        s_a1  = a1;
        s_a2  = a2;
        s_a3  = a3;
        s_a4  = a4;
        s_a5  = a5;
        s_a6  = a6;
        s_a7  = a7;
        s_a8  = a8;
        s_a9  = a9;
        s_a10 = a10;
        s_a11 = a11;
        s_a12 = a12;
        return 12;
    }

    int testCFunc13(TestArg1 a1, TestArg2  a2,  TestArg3  a3,  TestArg4  a4,
                    TestArg5 a5, TestArg6  a6,  TestArg7  a7,  TestArg8  a8,
                    TestArg9 a9, TestArg10 a10, TestArg11 a11, TestArg12 a12,
                    TestArg13 a13) const
    {
        s_a1  = a1;
        s_a2  = a2;
        s_a3  = a3;
        s_a4  = a4;
        s_a5  = a5;
        s_a6  = a6;
        s_a7  = a7;
        s_a8  = a8;
        s_a9  = a9;
        s_a10 = a10;
        s_a11 = a11;
        s_a12 = a12;
        s_a13 = a13;
        return 13;
    }

    int testCFunc14(TestArg1  a1,  TestArg2  a2,  TestArg3  a3,  TestArg4  a4,
                    TestArg5  a5,  TestArg6  a6,  TestArg7  a7,  TestArg8  a8,
                    TestArg9  a9,  TestArg10 a10, TestArg11 a11, TestArg12 a12,
                    TestArg13 a13, TestArg14 a14) const
    {
        s_a1  = a1;
        s_a2  = a2;
        s_a3  = a3;
        s_a4  = a4;
        s_a5  = a5;
        s_a6  = a6;
        s_a7  = a7;
        s_a8  = a8;
        s_a9  = a9;
        s_a10 = a10;
        s_a11 = a11;
        s_a12 = a12;
        s_a13 = a13;
        s_a14 = a14;
        return 14;
    }

    void print(bsl::ostream& stream) const {
        stream << "[ "
               << d_a1.d_value << ", "
               << d_a2.d_value << ", "
               << d_a3.d_value << ", "
               << d_a4.d_value << ", "
               << d_a5.d_value << ", "
               << d_a6.d_value << ", "
               << d_a7.d_value << ", "
               << d_a8.d_value << ", "
               << d_a9.d_value << ", "
               << d_a10.d_value << ", "
               << d_a11.d_value << ", "
               << d_a12.d_value << ", "
               << d_a13.d_value << ", "
               << d_a14.d_value << " ]";
    }
};

// CLASS DATA
TestArg1  InplaceTestObj::s_a1  = -1;
TestArg2  InplaceTestObj::s_a2  = -1;
TestArg3  InplaceTestObj::s_a3  = -1;
TestArg4  InplaceTestObj::s_a4  = -1;
TestArg5  InplaceTestObj::s_a5  = -1;
TestArg6  InplaceTestObj::s_a6  = -1;
TestArg7  InplaceTestObj::s_a7  = -1;
TestArg8  InplaceTestObj::s_a8  = -1;
TestArg9  InplaceTestObj::s_a9  = -1;
TestArg10 InplaceTestObj::s_a10 = -1;
TestArg11 InplaceTestObj::s_a11 = -1;
TestArg12 InplaceTestObj::s_a12 = -1;
TestArg13 InplaceTestObj::s_a13 = -1;
TestArg14 InplaceTestObj::s_a14 = -1;

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, const InplaceTestObj& rhs)
{
    rhs.print(stream);
    return stream;
}

                            // ====================
                            // class TestPtrWrapper
                            // ====================

template <class TYPE>
class TestPtrWrapper {

    TYPE            **d_objPtr_p;
    bslma_Allocator  *d_allocator_p;

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(TestPtrWrapper,
                                  bslalg_TypeTraitHasPointerSemantics,
                                  bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    TestPtrWrapper(TYPE *objPtr, bslma_Allocator *allocator = 0)
    : d_objPtr_p(0)
    , d_allocator_p(bslma_Default::allocator(allocator))
    {
        d_objPtr_p = (TYPE **)d_allocator_p->allocate(sizeof(TYPE *));
        *d_objPtr_p = objPtr;
    }

    TestPtrWrapper(const TestPtrWrapper&  original,
                   bslma_Allocator       *allocator = 0)
    : d_objPtr_p(0)
    , d_allocator_p(bslma_Default::allocator(allocator))
    {
        d_objPtr_p = (TYPE **)d_allocator_p->allocate(sizeof(TYPE *));
        *d_objPtr_p = *original.d_objPtr_p;
    }

    ~TestPtrWrapper()
    {
        d_allocator_p->deallocate(d_objPtr_p);
    }

    // ACCESSORS
    TYPE& operator *() const
    {
        return **d_objPtr_p;
    }

    TYPE *operator->() const
    {
        return *d_objPtr_p;
    }

    TYPE *ptr() const
    {
        return *d_objPtr_p;
    }
};

                    // ====================================
                    // class ConstructibleFromPointerToSelf
                    // ====================================

struct ConstructibleFromPointerToSelf {
    ConstructibleFromPointerToSelf *d_instance_p;

    ConstructibleFromPointerToSelf(ConstructibleFromPointerToSelf *instancePtr)
    : d_instance_p(instancePtr)
    {
    }

    bool memberFunction()
    {
        return d_instance_p == this;
    }
};

//=============================================================================
//                 USAGE EXAMPLE CLASSES AND FUNCTIONS
//-----------------------------------------------------------------------------

int globalVerbose = 0;

///Usage
///-----
///Basic Usage
///- - - - - -
// To illustrate basic usage more concretely, let us introduce a generic type:
//..
    class MyObject {
      public:
        void doSomething(int x, const char* s) {
            if (globalVerbose) printf("%d: %s\n", x, s);
        }
    };
//..
// The following function invokes the member function 'doSomething' on the
// specified 'objectPtr', with the two arguments 100 and "Hello", in two
// different ways.  In both cases, 'object' is passed as parameter to a
// function, and a wrapper is built containing a pointer to the 'doSomething'
// member function.  In the 'bdef_MemFn' case, the wrapper can be built once,
// so we make it a 'static' local variable:
//..
   void doSomethingWithMemFn(MyObject *objectPtr)
   {
       typedef bdef_MemFn<void (MyObject::*)(int, const char*)> MemFnType;
       static MemFnType func(&MyObject::doSomething);

       if (globalVerbose) func(objectPtr, 100, "Hello");
   }
//..
// In the 'bdef_MemFnInstance' case, the wrapper needs to contain the object as
// well, so it must be built dynamically:
//..
    void doSomethingWithMemFnInstance(MyObject *objectPtr)
    {
        typedef bdef_MemFnInstance<void (MyObject::*)(int, const char*),
                                   MyObject*> MemFnInstanceType;
        MemFnInstanceType func(&MyObject::doSomething, objectPtr);
        if (globalVerbose) func(100, "Hello");
    }
//..
// This latter example is for exposition only.  It would be much easier to
// invoke the member function directly.  Note that both function calls
// ultimately result in the member function call:
//..
//  objectPtr->doSomething(100, "Hello");
//..
//
///Usage With Standard Algorithms
/// - - - - - - - - - - - - - - -
// The following example demonstrates the use of 'bdef_MemFn' with the
// standard algorithms 'find_if' and 'for_each'.  First we declare the
// 'MyConnection' and 'MyConnectionManager' classes used in the example,
// keeping the class definitions short to highlight the member functions for
// which we will later build wrappers:
//..
    class MyConnection {

        // DATA
        bool d_available;

      public:
        // CREATORS
        explicit
        MyConnection(bool available = false)
        : d_available(available)
        {
        }

        // ACCESSORS
        bool isAvailable() const
        {
            return d_available;
        }

        // MANIPULATORS
        void disconnect()
        {
            if (globalVerbose) printf("Disconnect\n");
        }
    };

    class MyConnectionManager {

        // PRIVATE TYPES
        typedef bsl::list<MyConnection*>  MyConnectionList;

        // DATA
        MyConnectionList                  d_list;

      public:
        // CREATORS
        MyConnectionManager()
        {
            d_list.push_back(new MyConnection(false));
            d_list.push_back(new MyConnection(false));
            d_list.push_back(new MyConnection(true));
        }

        ~MyConnectionManager()
        {
            delete d_list.back(); d_list.pop_back();
            delete d_list.back(); d_list.pop_back();
            delete d_list.back(); d_list.pop_back();
        }

        // MANIPULATORS
        void disconnectAll();

        // ACCESSORS
        MyConnection *nextAvailable() const;
    };
//..
// The 'nextAvailable' function returns the next 'MyConnection' object that
// is available.  The 'find_if' algorithm is used to search the list for the
// first 'MyConnection' object that is available.  'find_if' invokes the
// provided function object for each item in the list until a true result is
// returned or the end of the list is reached.  A 'bdef_MemFn' object bound to
// the 'MyConnection::isAvailable' member function is used as the test
// functor:
//..
    MyConnection *MyConnectionManager::nextAvailable() const
    {
        MyConnectionList::const_iterator it =
               bsl::find_if(d_list.begin(),
                            d_list.end(),
                            bdef_MemFnUtil::memFn(&MyConnection::isAvailable));
        return it == d_list.end() ? 0 : *it;
    }
//..
// The 'disconnectAll' function calls 'disconnect' on each 'MyConnection'
// object in the list.  The 'for_each' algorithm is used to iterate through
// each 'MyConnection' object in the list and invoke the 'disconnect' method:
//..
    void MyConnectionManager::disconnectAll()
    {
        bsl::for_each(d_list.begin(),
                      d_list.end(),
                      bdef_MemFnUtil::memFn(&MyConnection::disconnect));
    }
//..

//=============================================================================
//                              TEST CASES
//-----------------------------------------------------------------------------
#define DEFINE_TEST_CASE(NUMBER)                                              \
void testCase##NUMBER(bool verbose, bool veryVerbose, bool veryVeryVerbose)   

DEFINE_TEST_CASE(10) {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
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

        if (verbose) cout << endl
                          << "Testing Usage Example" << endl
                          << "=====================" << endl;
        {
            MyObject mX;
            doSomethingWithMemFn(&mX);
            doSomethingWithMemFnInstance(&mX);
        }

        {
            MyConnectionManager manager;

            ASSERT(manager.nextAvailable());

            manager.disconnectAll();
        }
      }

DEFINE_TEST_CASE(9) {
        // --------------------------------------------------------------------
        // TESTING IMPLICIT CONVERSION FROM POINTER-TO-TYPE (DRQS 13973002)
        //
        // Concern: that invocable is not mistaken for a pointer type if the
        //   class has an implicit conversion from pointer-to-type.
        //
        // Plan:
        //   Instantiate a 'bdef_MemFnInstance' with a member function of a
        //   class that has an implicit conversion from pointer-to-type.  For
        //   added bonus, construct such a wrapper with an instance of the
        //   type and check that the function returns as expected.
        //
        // Testing:
        //   DRQS 13973002
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING IMPLICIT CONVERSION FROM POINTER-TO_TYPE."
                 << "\n================================================="
                 << endl;

        if (verbose) cout << "Check class 'bdef_MemFn':" << endl;
        {
            typedef BloombergLP::bdef_MemFn<
                        bool (ConstructibleFromPointerToSelf::*)()> MemFnType;

            // Windows compiler balks at the following:
            // ConstructibleFromPointerToSelf mX(&mX);

            ConstructibleFromPointerToSelf mX(0); mX.d_instance_p = &mX;
            ConstructibleFromPointerToSelf mY(&mX);
            MemFnType func(&ConstructibleFromPointerToSelf::memberFunction);

            ASSERT( func(mX));
            ASSERT(!func(mY));

            ASSERT( func(&mX));
            ASSERT(!func(&mY));
        }

        if (verbose) cout << "Check class 'bdef_MemFnInstance':" << endl;
        {
            typedef BloombergLP::bdef_MemFnInstance<
                        bool (ConstructibleFromPointerToSelf::*)(),
                                  ConstructibleFromPointerToSelf *>  MemFnType;

            ConstructibleFromPointerToSelf mX(0); mX.d_instance_p = &mX;
            MemFnType func(&ConstructibleFromPointerToSelf::memberFunction,
                           &mX);

            ASSERT(func());
        }
      }

DEFINE_TEST_CASE(8) {
        // --------------------------------------------------------------------
        // TESTING INSTANCES WITH POINTER SEMANTICS
        //
        // Concern:
        //   1. that classes which have pointer-like semantics (as
        //      indicated by the 'bslalg_TypeTraitHasPointerSemantics') are
        //      treated properly and that 'const'-correctness is respected.
        //   2. that if such classes take 'bdema' allocators, the copy stored
        //      inside the 'bdef_MemFnInstance' object is propagated the
        //      allocator passed at construction of the 'bdef_MemFnInstance'.
        //
        // Plan:
        //   Instantiate 'bdef_MemFn' and 'bdef_MemFnInstance' wrapper with a
        //   'TestPtrWrapper' that has pointer-like semantics and holds a
        //   pointer to a test object.  Check that invocation succeeds and
        //   returns as expected.  Note that it suffices to make sure that it
        //   works with a single invocation argument.
        //
        //   For concern 2, use a default allocator guard and make sure that
        //   memory for constructing the member function pointer wrappers use
        //   the specified allocator and not the default allocator.
        //
        // Testing:
        //   INSTANCES WITH POINTER SEMANTICS
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING INSTANCES WITH POINTER SEMANTICS."
                 << "\n=========================================" << endl;

        bslma_TestAllocator da(veryVeryVerbose);
        bslma_TestAllocator ta(veryVeryVerbose);
        bslma_DefaultAllocatorGuard guard(&da);

        static const TestArg1 V1(1);

        if (verbose) cout << "Testing 'bdef_MemFn':" << endl;

        if (verbose) cout << "\tnon-'const' member functions..." << endl;
        {
            typedef bdef_MemFn<int (InplaceTestObj::*)(TestArg1)> MemFnType;

            InplaceTestObj mX; const InplaceTestObj& X = mX;
            const InplaceTestObj EXP(V1);

            TestPtrWrapper<InplaceTestObj> smartPtr(&mX);

            MemFnType mF(bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc1));
            const MemFnType& F = mF;

            ASSERT(1 == F(smartPtr, V1));

            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\t'const' member functions..." << endl;
        {
            typedef bdef_MemFn<int (InplaceTestObj::*)(TestArg1) const>
                                                                     MemFnType;

            InplaceTestObj::reset();
            InplaceTestObj mX; const InplaceTestObj& X = mX;
            const InplaceTestObj EXP(V1);

            TestPtrWrapper<const InplaceTestObj> smartPtr(&X);

            MemFnType mF(bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc1));
            const MemFnType& F = mF;

            ASSERT(1 == F(smartPtr, V1));

            LOOP_ASSERT(X, InplaceTestObj() == X);
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "Testing 'bdef_MemFnInstance':" << endl;

        if (verbose) cout << "\tnon-'const' member functions..." << endl;
        {
            typedef bdef_MemFnInstance<int (InplaceTestObj::*)(TestArg1),
                                       TestPtrWrapper<InplaceTestObj>
                                      >  MemFnType;

            InplaceTestObj mX; const InplaceTestObj& X = mX;
            const InplaceTestObj EXP(V1);

            TestPtrWrapper<InplaceTestObj> smartPtr(&mX);

            MemFnType mF(bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc1,
                                               smartPtr));

            const int NUM_DEFAULT_ALLOCS = da.numAllocations();
            const int NUM_ALLOCS         = ta.numAllocations();

            MemFnType mG(mF, (bslma_Allocator*)&ta);  const MemFnType& G = mG;
            ASSERT(1 == G(V1));

            ASSERT(NUM_DEFAULT_ALLOCS == da.numAllocations());
            ASSERT(NUM_ALLOCS         <  ta.numAllocations());

            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose)
            cout << "\t'const' member functions and instances..." << endl;
        {
            typedef bdef_MemFnInstance<int (InplaceTestObj::*)(TestArg1) const,
                                       TestPtrWrapper<const InplaceTestObj>
                                      >  MemFnType;

            InplaceTestObj::reset();
            InplaceTestObj mX; const InplaceTestObj& X = mX;
            const InplaceTestObj EXP(V1);

            TestPtrWrapper<const InplaceTestObj> smartPtr(&X);

            MemFnType mF(bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc1,
                                               smartPtr));

            const int NUM_DEFAULT_ALLOCS = da.numAllocations();
            const int NUM_ALLOCS         = ta.numAllocations();

            MemFnType mG(mF, (bslma_Allocator*)&ta);  const MemFnType& G = mG;
            ASSERT(1 == G(V1));

            ASSERT(NUM_DEFAULT_ALLOCS == da.numAllocations());
            ASSERT(NUM_ALLOCS         <  ta.numAllocations());

            LOOP_ASSERT(X, InplaceTestObj() == X);
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose)
            cout << "\t'const' member functions and non-'const' instances..."
                 << endl;
        {
            typedef bdef_MemFnInstance<int (InplaceTestObj::*)(TestArg1) const,
                                       TestPtrWrapper<InplaceTestObj>
                                      >  MemFnType;

            InplaceTestObj::reset();
            InplaceTestObj mX; const InplaceTestObj& X = mX;
            const InplaceTestObj EXP(V1);

            TestPtrWrapper<InplaceTestObj> smartPtr(&mX);

            MemFnType mF(bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc1,
                                               smartPtr));

            const int NUM_DEFAULT_ALLOCS = da.numAllocations();
            const int NUM_ALLOCS         = ta.numAllocations();

            MemFnType mG(mF, (bslma_Allocator*)&ta);  const MemFnType& G = mG;
            ASSERT(1 == G(V1));

            ASSERT(NUM_DEFAULT_ALLOCS == da.numAllocations());
            ASSERT(NUM_ALLOCS         <  ta.numAllocations());

            LOOP_ASSERT(X, InplaceTestObj() == X);
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }
      }

DEFINE_TEST_CASE(7) {
        // --------------------------------------------------------------------
        // TESTING 'bdef_MemFnInstance' INVOCATION WITH NON-MODIF. INSTANCE
        //
        // Concern:
        //   That one may create a member function wrapper with a
        //   non-modifiable instance of the test type (owned), and that such
        //   wrapper may be invoked in same way and return same value as a
        //   direct member function call.
        //
        // Plan:
        //   Create member function pointer wrappers around member functions of
        //   our invocable test object with various signatures and
        //   a non-modifiable test object, and invoke them with
        //   singular-valued arguments (to differentiate from the default
        //   values in the global variables) and verify that the invocation
        //   returns the correct value and that the global variables are
        //   modified as expected.
        //
        // Testing:
        //   ResultType operator()(INSTANCE x);
        //   ResultType operator()(INSTANCE x, A1 const &a1)
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...&a2);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a3);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a4);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a5);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a6);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a7);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a8);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a9);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a10);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a11);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a12);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a13);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a14);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING 'bdef_MemFnInstance' WITH NON-MODIFIABLE TYPE."
                 << "\n======================================================"
                 << endl;

        static const TestArg1 V1(1);
        static const TestArg2 V2(20);
        static const TestArg3 V3(23);
        static const TestArg4 V4(44);
        static const TestArg5 V5(66);
        static const TestArg6 V6(176);
        static const TestArg7 V7(878);
        static const TestArg8 V8(8);
        static const TestArg9 V9(912);
        static const TestArg10 V10(102);
        static const TestArg11 V11(111);
        static const TestArg12 V12(333);
        static const TestArg13 V13(712);
        static const TestArg14 V14(1414);

        if (verbose) cout << "\tno argument..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP;

            ASSERT(0 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc0,
                                               EXP)()));

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\tone argument..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1);

            ASSERT(1 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc1,
                                               EXP)
                (V1)));

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\ttwo arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2);

            ASSERT(2 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc2,
                                               EXP)
                (V1, V2))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\tthree arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3);

            ASSERT(3 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc3,
                                               EXP)
                (V1, V2, V3))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\tfour arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3, V4);

            ASSERT(4 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc4,
                                               EXP)
                (V1, V2, V3, V4))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\tfive arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5);

            ASSERT(5 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc5,
                                               EXP)
                (V1, V2, V3, V4, V5))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\tsix arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6);

            ASSERT(6 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc6,
                                               EXP)
                (V1, V2, V3, V4, V5, V6))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\tseven arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7);

            ASSERT(7 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc7,
                                               EXP)
                (V1, V2, V3, V4, V5, V6, V7))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\teight arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8);

            ASSERT(8 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc8,
                                               EXP)
                (V1, V2, V3, V4, V5, V6, V7, V8))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\tnine arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9);

            ASSERT(9 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc9,
                                               EXP)
                (V1, V2, V3, V4, V5, V6, V7, V8, V9))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\tten arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9, V10);

            ASSERT(10 ==(bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc10,
                                               EXP)
                (V1, V2, V3, V4, V5, V6, V7, V8, V9, V10))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\televen arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9, V10, V11);

            ASSERT(11 ==(bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc11,
                                               EXP)
                (V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\ttwelve arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9, V10, V11, V12);

            ASSERT(12 ==(bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc12,
                                               EXP)
                (V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\tthirteen arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9, V10, V11, V12, V13);

            ASSERT(13 ==(bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc13,
                                               EXP)
                (V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12,
                 V13))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\tfourteen arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9, V10, V11, V12, V13, V14);

            ASSERT(14 ==(bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc14,
                                               EXP)
                (V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12,
                 V13, V14))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }
      }

DEFINE_TEST_CASE(6) {
        // --------------------------------------------------------------------
        // TESTING 'bdef_MemFnInstance' INVOCATION WITH PTR TO MODIF. OBJECT
        //
        // Concern:
        //   That one may create a member function wrapper with the address of
        //   a modifiable instance of the test type (held), and that such
        //   wrapper may be invoked and affect instance state in same way, and
        //   return same value as a direct member function call.
        //
        // Plan:
        //   Create member function pointer and instance wrappers around member
        //   functions of our invocable test object with various signatures and
        //   references to modifiable default-constructed test objects, and
        //   invoke them with singular-valued arguments (to differentiate from
        //   the values in the default-constructed object) and verify that the
        //   invocation returns the correct value and that the object is
        //   modified as expected.
        //
        // Testing:
        //   ResultType operator()();
        //   ResultType operator()(A1 const &a1)
        //   ResultType operator()(A1 const &a1, ...&a2);
        //   ResultType operator()(A1 const &a1, ...a3);
        //   ResultType operator()(A1 const &a1, ...a4);
        //   ResultType operator()(A1 const &a1, ...a5);
        //   ResultType operator()(A1 const &a1, ...a6);
        //   ResultType operator()(A1 const &a1, ...a7);
        //   ResultType operator()(A1 const &a1, ...a8);
        //   ResultType operator()(A1 const &a1, ...a9);
        //   ResultType operator()(A1 const &a1, ...a10);
        //   ResultType operator()(A1 const &a1, ...a11);
        //   ResultType operator()(A1 const &a1, ...a12);
        //   ResultType operator()(A1 const &a1, ...a13);
        //   ResultType operator()(A1 const &a1, ...a14);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING 'bdef_MemFnInstance' WITH PTR TO MODIF. TYPE."
                 << "\n====================================================="
                 << endl;

        static const TestArg1 V1(1);
        static const TestArg2 V2(20);
        static const TestArg3 V3(23);
        static const TestArg4 V4(44);
        static const TestArg5 V5(66);
        static const TestArg6 V6(176);
        static const TestArg7 V7(878);
        static const TestArg8 V8(8);
        static const TestArg9 V9(912);
        static const TestArg10 V10(102);
        static const TestArg11 V11(111);
        static const TestArg12 V12(333);
        static const TestArg13 V13(712);
        static const TestArg14 V14(1414);

        if (verbose) cout << "\tno argument..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1);

            ASSERT(1 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc1, &x)
                 (V1)));

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\tone argument..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1);

            ASSERT(1 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc1, &x)
                 (V1)));

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\ttwo arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2);

            ASSERT(2 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc2, &x)
                (V1, V2))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\tthree arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3);

            ASSERT(3 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc3, &x)
                (V1, V2, V3))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\tfour arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3, V4);

            ASSERT(4 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc4, &x)
                (V1, V2, V3, V4))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\tfive arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5);

            ASSERT(5 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc5, &x)
                (V1, V2, V3, V4, V5))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\tsix arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6);

            ASSERT(6 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc6, &x)
                (V1, V2, V3, V4, V5, V6))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\tseven arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7);

            ASSERT(7 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc7, &x)
                (V1, V2, V3, V4, V5, V6, V7))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\teight arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8);

            ASSERT(8 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc8, &x)
                (V1, V2, V3, V4, V5, V6, V7, V8))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\tnine arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9);

            ASSERT(9 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc9, &x)
                (V1, V2, V3, V4, V5, V6, V7, V8, V9))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\tten arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9, V10);

            ASSERT(10 ==(bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc10, &x)
                (V1, V2, V3, V4, V5, V6, V7, V8, V9, V10))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\televen arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9, V10, V11);

            ASSERT(11 ==(bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc11, &x)
                (V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\ttwelve arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9, V10, V11, V12);

            ASSERT(12 ==(bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc12, &x)
                (V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\tthirteen arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9, V10, V11, V12, V13);

            ASSERT(13 ==(bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc13, &x)
                (V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12,
                 V13))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\tfourteen arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9, V10, V11, V12, V13, V14);

            ASSERT(14 ==(bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc14, &x)
                (V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12,
                 V13, V14))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }
      }

DEFINE_TEST_CASE(5) {
        // --------------------------------------------------------------------
        // TESTING 'bdef_MemFn' INVOCATION WITH *REF.* TO NON-MODIF. OBJECT
        //
        // Concern:
        //   That one may create a member function wrapper, and that such
        //   wrapper may be invoked with a reference to a non-modifiable
        //   instance of the test type, that such call does not affect instance
        //   state, but return same value as a direct member function call.
        //
        // Plan:
        //   Create member function pointer wrappers around member functions of
        //   our invocable test object with various signatures, and invoke them
        //   with a reference to a non-modifiable test object and with
        //   singular-valued arguments (to differentiate from the default
        //   values in the global variables) and verify that the invocation
        //   returns the correct value and that the global variables are
        //   modified as expected.
        //
        // Testing:
        //   ResultType operator()(INSTANCE x);
        //   ResultType operator()(INSTANCE x, A1 const &a1)
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...&a2);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a3);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a4);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a5);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a6);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a7);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a8);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a9);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a10);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a11);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a12);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a13);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a14);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING 'bdef_MemFn::operator()(X, ...)'."
                 << "\n=========================================" << endl;

        static const TestArg1 V1(1);
        static const TestArg2 V2(20);
        static const TestArg3 V3(23);
        static const TestArg4 V4(44);
        static const TestArg5 V5(66);
        static const TestArg6 V6(176);
        static const TestArg7 V7(878);
        static const TestArg8 V8(8);
        static const TestArg9 V9(912);
        static const TestArg10 V10(102);
        static const TestArg11 V11(111);
        static const TestArg12 V12(333);
        static const TestArg13 V13(712);
        static const TestArg14 V14(1414);

        if (verbose) cout << "\tno argument..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP;

            ASSERT(0 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc0)
                           (EXP)));

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\tone argument..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1);

            ASSERT(1 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc1)
                           (EXP,V1)));

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\ttwo arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2);

            ASSERT(2 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc2)
                (EXP, V1, V2))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\tthree arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3);

            ASSERT(3 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc3)
                (EXP, V1, V2, V3))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\tfour arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3, V4);

            ASSERT(4 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc4)
                (EXP, V1, V2, V3, V4))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\tfive arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5);

            ASSERT(5 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc5)
                (EXP, V1, V2, V3, V4, V5))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\tsix arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6);

            ASSERT(6 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc6)
                (EXP, V1, V2, V3, V4, V5, V6))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\tseven arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7);

            ASSERT(7 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc7)
                (EXP, V1, V2, V3, V4, V5, V6, V7))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\teight arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8);

            ASSERT(8 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc8)
                (EXP, V1, V2, V3, V4, V5, V6, V7, V8))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\tnine arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9);

            ASSERT(9 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc9)
                (EXP, V1, V2, V3, V4, V5, V6, V7, V8, V9))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\tten arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9, V10);

            ASSERT(10 ==(bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc10)
                (EXP, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\televen arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9, V10, V11);

            ASSERT(11 ==(bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc11)
                (EXP, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\ttwelve arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9, V10, V11, V12);

            ASSERT(12 ==(bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc12)
                      (EXP,V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11,
                 V12))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\tthirteen arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9, V10, V11, V12, V13);

            ASSERT(13 ==(bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc13)
                (EXP, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12,
                 V13))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\tfourteen arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9, V10, V11, V12, V13, V14);

            ASSERT(14 ==(bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc14)
                (EXP, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12,
                 V13, V14))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }
      }

DEFINE_TEST_CASE(4) {
        // --------------------------------------------------------------------
        // TESTING 'bdef_MemFn' INVOCATION WITH *REF.* TO MODIFIABLE OBJECT
        //
        // Concern:
        //   That one may create a member function wrapper, that such wrapper
        //   may be invoked with a reference to a modifiable instance of the
        //   test type, and that such call affect instance state in same way
        //   and return same value as a direct member function call.
        //
        // Plan:
        //   Create member function pointer wrappers around member functions of
        //   our invocable test object with various signatures, and invoke them
        //   with a reference to a modifiable default-constructed test object
        //   and singular-valued arguments (to differentiate from the values
        //   in the default-constructed object) and verify that the invocation
        //   returns the correct value and that the object is modified as
        //   expected.
        //
        // Testing:
        //   ResultType operator()(INSTANCE x);
        //   ResultType operator()(INSTANCE x, A1 const &a1)
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...&a2);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a3);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a4);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a5);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a6);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a7);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a8);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a9);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a10);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a11);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a12);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a13);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a14);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING 'bdef_MemFn::operator()(x, ...)'."
                 << "\n=========================================" << endl;

        static const TestArg1 V1(1);
        static const TestArg2 V2(20);
        static const TestArg3 V3(23);
        static const TestArg4 V4(44);
        static const TestArg5 V5(66);
        static const TestArg6 V6(176);
        static const TestArg7 V7(878);
        static const TestArg8 V8(8);
        static const TestArg9 V9(912);
        static const TestArg10 V10(102);
        static const TestArg11 V11(111);
        static const TestArg12 V12(333);
        static const TestArg13 V13(712);
        static const TestArg14 V14(1414);

        if (verbose) cout << "\tno argument..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1);

            ASSERT(1 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc1)
                           (x,V1)));

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\tone argument..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1);

            ASSERT(1 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc1)
                           (x,V1)));

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\ttwo arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2);

            ASSERT(2 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc2)
                (x, V1, V2))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\tthree arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3);

            ASSERT(3 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc3)
                (x, V1, V2, V3))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\tfour arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3, V4);

            ASSERT(4 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc4)
                (x, V1, V2, V3, V4))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\tfive arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5);

            ASSERT(5 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc5)
                (x, V1, V2, V3, V4, V5))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\tsix arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6);

            ASSERT(6 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc6)
                (x, V1, V2, V3, V4, V5, V6))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\tseven arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7);

            ASSERT(7 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc7)
                (x, V1, V2, V3, V4, V5, V6, V7))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\teight arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8);

            ASSERT(8 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc8)
                (x, V1, V2, V3, V4, V5, V6, V7, V8))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\tnine arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9);

            ASSERT(9 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc9)
                (x, V1, V2, V3, V4, V5, V6, V7, V8, V9))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\tten arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9, V10);

            ASSERT(10 ==(bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc10)
                (x, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\televen arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9, V10, V11);

            ASSERT(11 ==(bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc11)
                (x, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\ttwelve arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9, V10, V11, V12);

            ASSERT(12 ==(bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc12)
                      (x,V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11,
                 V12))) ;

            if (veryVerbose) { P(X); };
            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\tthirteen arguments..." << endl;

        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9, V10, V11, V12, V13);

            ASSERT(13 ==(bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc13)
                (x, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12,
                 V13))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\tfourteen arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9, V10, V11, V12, V13, V14);

            ASSERT(14 ==(bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc14)
                (x, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12,
                 V13, V14))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }
      }

DEFINE_TEST_CASE(3) {
        // --------------------------------------------------------------------
        // TESTING 'bdef_MemFn' INVOCATION WITH POINTER TO NON-MODIF. OBJECT
        //
        // Concern:
        //   That one may create a member function wrapper, that such
        //   wrapper may be invoked with the address of a non-modifiable
        //   instance of the test type, and that such call may not affect
        //   instance state but return same value as a direct member function
        //   call.
        //
        // Plan:
        //   Create member function pointer wrappers around member functions of
        //   our invocable test object with various signatures, and invoke them
        //   with the address of a non-modifiable test object and with
        //   singular-valued arguments (to differentiate from the default
        //   values in the global variables) and verify that the invocation
        //   returns the correct value and that the global variables are
        //   modified as expected.
        //
        // Testing:
        //   ResultType operator()(INSTANCE x);
        //   ResultType operator()(INSTANCE x, A1 const &a1)
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...&a2);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a3);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a4);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a5);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a6);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a7);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a8);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a9);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a10);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a11);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a12);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a13);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a14);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING 'bdef_MemFn::operator()(&X, ...)'."
                 << "\n=========================================" << endl;

        static const TestArg1 V1(1);
        static const TestArg2 V2(20);
        static const TestArg3 V3(23);
        static const TestArg4 V4(44);
        static const TestArg5 V5(66);
        static const TestArg6 V6(176);
        static const TestArg7 V7(878);
        static const TestArg8 V8(8);
        static const TestArg9 V9(912);
        static const TestArg10 V10(102);
        static const TestArg11 V11(111);
        static const TestArg12 V12(333);
        static const TestArg13 V13(712);
        static const TestArg14 V14(1414);

        if (verbose) cout << "\tno argument..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP;

            ASSERT(0 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc0)
                           (&EXP)));

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\tone argument..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1);

            ASSERT(1 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc1)
                           (&EXP,V1)));

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\ttwo arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2);

            ASSERT(2 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc2)
                (&EXP, V1, V2))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\tthree arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3);

            ASSERT(3 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc3)
                (&EXP, V1, V2, V3))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\tfour arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3, V4);

            ASSERT(4 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc4)
                (&EXP, V1, V2, V3, V4))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\tfive arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5);

            ASSERT(5 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc5)
                (&EXP, V1, V2, V3, V4, V5))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\tsix arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6);

            ASSERT(6 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc6)
                (&EXP, V1, V2, V3, V4, V5, V6))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\tseven arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7);

            ASSERT(7 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc7)
                (&EXP, V1, V2, V3, V4, V5, V6, V7))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\teight arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8);

            ASSERT(8 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc8)
                (&EXP, V1, V2, V3, V4, V5, V6, V7, V8))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\tnine arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9);

            ASSERT(9 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc9)
                (&EXP, V1, V2, V3, V4, V5, V6, V7, V8, V9))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\tten arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9, V10);

            ASSERT(10 ==(bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc10)
                (&EXP, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\televen arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9, V10, V11);

            ASSERT(11 ==(bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc11)
                (&EXP, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\ttwelve arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9, V10, V11, V12);

            ASSERT(12 ==(bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc12)
                      (&EXP,V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11,
                 V12))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\tthirteen arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9, V10, V11, V12, V13);

            ASSERT(13 ==(bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc13)
                (&EXP, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12,
                 V13))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "\tfourteen arguments..." << endl;
        {
            InplaceTestObj::reset();
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9, V10, V11, V12, V13, V14);

            ASSERT(14 ==(bdef_MemFnUtil::memFn(&InplaceTestObj::testCFunc14)
                (&EXP, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12,
                 V13, V14))) ;

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }
      }

DEFINE_TEST_CASE(2) {
        // --------------------------------------------------------------------
        // TESTING 'bdef_MemFn' INVOCATION WITH POINTER TO MODIFIABLE OBJECT
        //
        // Concern:
        //   That one may create a member function wrapper, that such
        //   wrapper may be invoked with the address of a modifiable
        //   instance of the test type, and that such call affect instance
        //   state in same way and return same value as a direct member
        //   function call.
        //
        // Plan:
        //   Create member function pointer wrappers around member functions of
        //   our invocable test object with various signatures, and invoke them
        //   with the address of a modifiable default-constructed test object
        //   with singular-valued arguments (to differentiate from the values
        //   in the default-constructed object) and verify that the invocation
        //   returns the correct value and that the object is modified as
        //   expected.
        //
        // Testing:
        //   ResultType operator()(INSTANCE x);
        //   ResultType operator()(INSTANCE x, A1 const &a1)
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...&a2);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a3);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a4);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a5);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a6);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a7);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a8);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a9);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a10);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a11);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a12);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a13);
        //   ResultType operator()(INSTANCE x, A1 const &a1, ...a14);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING 'bdef_MemFn::operator()(&x, ...)'."
                 << "\n=========================================" << endl;

        static const TestArg1 V1(1);
        static const TestArg2 V2(20);
        static const TestArg3 V3(23);
        static const TestArg4 V4(44);
        static const TestArg5 V5(66);
        static const TestArg6 V6(176);
        static const TestArg7 V7(878);
        static const TestArg8 V8(8);
        static const TestArg9 V9(912);
        static const TestArg10 V10(102);
        static const TestArg11 V11(111);
        static const TestArg12 V12(333);
        static const TestArg13 V13(712);
        static const TestArg14 V14(1414);

        if (verbose) cout << "\tno argument..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP;

            ASSERT(0 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc0)
                           (&x)));

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\tone argument..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1);

            ASSERT(1 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc1)
                           (&x,V1)));

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\ttwo arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2);

            ASSERT(2 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc2)
                (&x, V1, V2))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\tthree arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3);

            ASSERT(3 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc3)
                (&x, V1, V2, V3))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\tfour arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3, V4);

            ASSERT(4 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc4)
                (&x, V1, V2, V3, V4))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\tfive arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5);

            ASSERT(5 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc5)
                (&x, V1, V2, V3, V4, V5))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\tsix arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6);

            ASSERT(6 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc6)
                (&x, V1, V2, V3, V4, V5, V6))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\tseven arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7);

            ASSERT(7 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc7)
                (&x, V1, V2, V3, V4, V5, V6, V7))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\teight arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8);

            ASSERT(8 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc8)
                (&x, V1, V2, V3, V4, V5, V6, V7, V8))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\tnine arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9);

            ASSERT(9 == (bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc9)
                (&x, V1, V2, V3, V4, V5, V6, V7, V8, V9))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\tten arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9, V10);

            ASSERT(10 ==(bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc10)
                (&x, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\televen arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9, V10, V11);

            ASSERT(11 ==(bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc11)
                (&x, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\ttwelve arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9, V10, V11, V12);

            ASSERT(12 ==(bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc12)
                      (&x,V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11,
                 V12))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\tthirteen arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9, V10, V11, V12, V13);

            ASSERT(13 ==(bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc13)
                (&x, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12,
                 V13))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\tfourteen arguments..." << endl;
        {
            InplaceTestObj x; InplaceTestObj const &X=x;
            static const InplaceTestObj EXP(V1, V2, V3, V4, V5, V6, V7,
                            V8, V9, V10, V11, V12, V13, V14);

            ASSERT(14 ==(bdef_MemFnUtil::memFn(&InplaceTestObj::testFunc14)
                (&x, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12,
                 V13, V14))) ;

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }
      }

DEFINE_TEST_CASE(1) {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   Exercise elementary usage of the component before beginning
        //   testing in earnest.
        //
        // Testing:
        //   This test case *exercises* the code but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST."
                          << "\n===============" << endl;

        TestObject x;
        TestObject const &X=x;

        bdef_MemFn<int (TestObject::*)(int)> f1(&TestObject::test1);
        f1(x,10);

        typedef bslmf_MemberFunctionPointerTraits<int (TestObject::*)(int)>
                                                                        Traits;

        TestObject *t = (Traits::ClassType*)0;
        ASSERT(0 == t); // remove a warning on gcc

        bdef_MemFn<int (TestObject::*)(int) const> f2(&TestObject::test1Const);

        f2(&X,10);

        f2(&x,10);

        f2(&x,10);

        f2(&X,10);

        TestPtrWrapper<const TestObject> w(&x);
        TestPtrWrapper<const TestObject> const &W=w;

        f2(w,10);
        f2(W,10);
      }

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    globalVerbose = verbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma_TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
#define CASE(NUMBER)                                                     \
  case NUMBER: testCase##NUMBER(verbose, veryVerbose, veryVeryVerbose); break
        CASE(10);
        CASE(9);
        CASE(8);
        CASE(7);
        CASE(6);
        CASE(5);
        CASE(4);
        CASE(3);
        CASE(2);
        CASE(1);
#undef CASE
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
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
