// bdlf_memfn.t.cpp                                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlf_memfn.h>

#include <bslim_testutil.h>

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
// The 'bdlf::MemFn' class holds a pointer to a member function, and the
// 'bdlf::MemFnInstance' holds the same plus an object which either has pointer
// semantics.  Our main objective is to make sure that the member function
// wrapper object can be initialized with different kinds of invocables,
// including smart-pointer like objects which have pointer semantics, that the
// wrapper forwards invocation arguments properly, and returns the correct
// value.  In addition, we want to verify that the 'bslmf_MemFnInstance'
// wrapper propagates its allocator to the instance it holds, if that instance
// takes a 'bslma' allocator.  Finally, we also have concerns that
// 'const'-correctness is respected, i.e., a non-'const' member function cannot
// be involved on a non-modifiable instance.
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] 'bdlf::MemFn' INVOCATION WITH POINTER TO MODIFIABLE OBJECT
// [ 3] 'bdlf::MemFn' INVOCATION WITH POINTER TO NON-MODIFIABLE OBJECT
// [ 4] 'bdlf::MemFn' INVOCATION WITH REFERENCE TO MODIFIABLE OBJECT
// [ 5] 'bdlf::MemFn' INVOCATION WITH REFERENCE TO NON-MODIFIABLE OBJECT
// [ 6] 'bdlf::MemFnInstance' INVOCATION WITH PTR TO MODIFIABLE OBJECT
// [ 7] 'bdlf::MemFnInstance' INVOCATION WITH NON-MODIFIABLE INSTANCE
// [ 8] INSTANCES WITH POINTER SEMANTICS
// [ 9] IMPLICIT CONVERSION FROM POINTER-TO-TYPE (DRQS 13973002)
// [10] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                MACROS FOR MULTIPLE ARGUMENT EXPANSION
// ============================================================================

// Numbered items separated by given separator

#define S1(F,S)             F(1)
#define S2(F,S)  S1(F,S)  S F(2)
#define S3(F,S)  S2(F,S)  S F(3)
#define S4(F,S)  S3(F,S)  S F(4)
#define S5(F,S)  S4(F,S)  S F(5)
#define S6(F,S)  S5(F,S)  S F(6)
#define S7(F,S)  S6(F,S)  S F(7)
#define S8(F,S)  S7(F,S)  S F(8)
#define S9(F,S)  S8(F,S)  S F(9)
#define S10(F,S) S9(F,S)  S F(10)
#define S11(F,S) S10(F,S) S F(11)
#define S12(F,S) S11(F,S) S F(12)
#define S13(F,S) S12(F,S) S F(13)
#define S14(F,S) S13(F,S) S F(14)

// Comma-separated numbered items

#define C1(F)          F(1)
#define C2(F)  C1(F),  F(2)
#define C3(F)  C2(F),  F(3)
#define C4(F)  C3(F),  F(4)
#define C5(F)  C4(F),  F(5)
#define C6(F)  C5(F),  F(6)
#define C7(F)  C6(F),  F(7)
#define C8(F)  C7(F),  F(8)
#define C9(F)  C8(F),  F(9)
#define C10(F) C9(F),  F(10)
#define C11(F) C10(F), F(11)
#define C12(F) C11(F), F(12)
#define C13(F) C12(F), F(13)
#define C14(F) C13(F), F(14)

// Space-separated numbered items

#define L1(F)         F(1)
#define L2(F)  L1(F)  F(2)
#define L3(F)  L2(F)  F(3)
#define L4(F)  L3(F)  F(4)
#define L5(F)  L4(F)  F(5)
#define L6(F)  L5(F)  F(6)
#define L7(F)  L6(F)  F(7)
#define L8(F)  L7(F)  F(8)
#define L9(F)  L8(F)  F(9)
#define L10(F) L9(F)  F(10)
#define L11(F) L10(F) F(11)
#define L12(F) L11(F) F(12)
#define L13(F) L12(F) F(13)
#define L14(F) L13(F) F(14)

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                     GLOBAL HELPER CLASSES FOR TESTING
// ----------------------------------------------------------------------------

struct TestObject {
    int test1 (int a1) { return a1; }
    int test1Const (int a1) const { return a1; }
    int test2 (int a1, const char* a2) { return a1; }
};

#undef M
#define M(n)                                                                  \
struct TestArg##n {                                                           \
    int d_value;                                                              \
    TestArg##n(int value = -1) : d_value(value) { }                           \
    bool operator==(const TestArg##n &rhs) { return d_value == rhs.d_value; } \
};
L14(M)

                            // ====================
                            // class InplaceTestObj
                            // ====================

class InplaceTestObj {
    // This class is an invocable with any number from 0 up to 14 arguments, of
    // distinct types 'TestArg1' up to 'TestArg14'.  Invoking the test function
    // with 'N' arguments on a modifiable (resp., non-modifiable) instance will
    // set the first 'N' instance (resp., class) data members.  The class data
    // members can be reset to -1 (their default value) using the 'reset' class
    // method.

    // DATA
#undef M
#define M(n) TestArg##n d_a##n;
L14(M)

  public:
    // CLASS DATA
#undef M
#define M(n) static TestArg##n s_a##n;
L14(M)

    // CLASS METHODS
    static void reset()
    {
#undef M
#define M(n) s_a##n = -1;
L14(M)
    }

    // CREATORS
#undef N
#define N(n) TestArg##n a##n = -1
#undef M
#define M(n) d_a##n(a##n)
    InplaceTestObj(C14(N))
    : C14(M)
    {
    }

    // CLASS METHODS (continued)
    static InplaceTestObj statics()
    {
#undef M
#define M(n) s_a##n
        return InplaceTestObj(C14(M));
    }

    // MANIPULATORS
    int testFunc0()
    {
        return 0;
    }

#undef H
#define H(n) d_a##n = a##n;
#undef N
#define N(n) TestArg##n a##n
#undef M
#define M(n)                                                                  \
    int testFunc##n(C##n(N))                                                  \
    {                                                                         \
        S##n(H,)                                                              \
        return n;                                                             \
    }
L14(M)

    // ACCESSORS
    bool operator==(InplaceTestObj const &rhs) const
    {
#undef M
#define M(n) d_a##n.d_value == rhs.d_a##n.d_value
        return S14(M,&&);
    }

    int testCFunc0() const
    {
        return 0;
    }

#undef H
#define H(n) s_a##n = a##n;
#undef N
#define N(n) TestArg##n a##n
#undef M
#define M(n)                                                                  \
    int testCFunc##n(C##n(N)) const                                           \
    {                                                                         \
        S##n(H,)                                                              \
        return n;                                                             \
    }
L14(M)

    void print(bsl::ostream& stream) const {
#undef M
#define M(n) d_a##n.d_value << ", "
        stream << "[ " << S14(M,<<) << " ]";
    }
};

// CLASS DATA
#undef M
#define M(n) TestArg##n InplaceTestObj::s_a##n = -1;
L14(M)

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

    TYPE             **d_objPtr_p;
    bslma::Allocator  *d_allocator_p;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(TestPtrWrapper, bslma::UsesBslmaAllocator);
    BSLMF_NESTED_TRAIT_DECLARATION(TestPtrWrapper, bslmf::HasPointerSemantics);

    // CREATORS
    TestPtrWrapper(TYPE *objPtr, bslma::Allocator *allocator = 0)
    : d_objPtr_p(0)
    , d_allocator_p(bslma::Default::allocator(allocator))
    {
        d_objPtr_p = (TYPE **)d_allocator_p->allocate(sizeof(TYPE *));
        *d_objPtr_p = objPtr;
    }

    TestPtrWrapper(const TestPtrWrapper&  original,
                   bslma::Allocator      *allocator = 0)
    : d_objPtr_p(0)
    , d_allocator_p(bslma::Default::allocator(allocator))
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

// ============================================================================
//                    USAGE EXAMPLE CLASSES AND FUNCTIONS
// ----------------------------------------------------------------------------

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
// member function.  In the 'bdlf::MemFn' case, the wrapper can be built once,
// so we make it a 'static' local variable:
//..
   void doSomethingWithMemFn(MyObject *objectPtr)
   {
       typedef bdlf::MemFn<void (MyObject::*)(int, const char*)> MemFnType;
       static MemFnType func(&MyObject::doSomething);

       if (globalVerbose) func(objectPtr, 100, "Hello");
   }
//..
// In the 'bdlf::MemFnInstance' case, the wrapper needs to contain the object
// as well, so it must be built dynamically:
//..
    void doSomethingWithMemFnInstance(MyObject *objectPtr)
    {
        typedef bdlf::MemFnInstance<void (MyObject::*)(int, const char*),
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
// The following example demonstrates the use of 'bdlf::MemFn' with the
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
// returned or the end of the list is reached.  A 'bdlf::MemFn' object bound to
// the 'MyConnection::isAvailable' member function is used as the test
// functor:
//..
    MyConnection *MyConnectionManager::nextAvailable() const
    {
        MyConnectionList::const_iterator it =
            bsl::find_if(d_list.begin(),
                         d_list.end(),
                         bdlf::MemFnUtil::memFn(&MyConnection::isAvailable));
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
                      bdlf::MemFnUtil::memFn(&MyConnection::disconnect));
    }
//..

// ============================================================================
//                                TEST CASES
// ----------------------------------------------------------------------------
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
        //   Instantiate a 'bdlf::MemFnInstance' with a member function of a
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

        if (verbose) cout << "Check class 'bdlf::MemFn':" << endl;
        {
            typedef BloombergLP::bdlf::MemFn<
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

        if (verbose) cout << "Check class 'bdlf::MemFnInstance':" << endl;
        {
            typedef BloombergLP::bdlf::MemFnInstance<
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
        //   1. that classes which have pointer-like semantics (as indicated by
        //      the 'bslmf::HasPointerSemantics') are treated properly and that
        //      'const'-correctness is respected.
        //   2. that if such classes take 'bblma' allocators, the copy stored
        //      inside the 'bdlf::MemFnInstance' object is propagated the
        //      allocator passed at construction of the 'bdlf::MemFnInstance'.
        //
        // Plan:
        //   Instantiate 'bdlf::MemFn' and 'bdlf::MemFnInstance' wrapper with a
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

        bslma::TestAllocator da(veryVeryVerbose);
        bslma::TestAllocator ta(veryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&da);

        static const TestArg1 V1(1);

        if (verbose) cout << "Testing 'bdlf::MemFn':" << endl;

        if (verbose) cout << "\tnon-'const' member functions..." << endl;
        {
            typedef bdlf::MemFn<int (InplaceTestObj::*)(TestArg1)> MemFnType;

            InplaceTestObj mX; const InplaceTestObj& X = mX;
            const InplaceTestObj EXP(V1);

            TestPtrWrapper<InplaceTestObj> smartPtr(&mX);

            MemFnType mF(bdlf::MemFnUtil::memFn(&InplaceTestObj::testFunc1));
            const MemFnType& F = mF;

            ASSERT(1 == F(smartPtr, V1));

            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose) cout << "\t'const' member functions..." << endl;
        {
            typedef bdlf::MemFn<int (InplaceTestObj::*)(TestArg1) const>
                                                                     MemFnType;

            InplaceTestObj::reset();
            InplaceTestObj mX; const InplaceTestObj& X = mX;
            const InplaceTestObj EXP(V1);

            TestPtrWrapper<const InplaceTestObj> smartPtr(&X);

            MemFnType mF(bdlf::MemFnUtil::memFn(&InplaceTestObj::testCFunc1));
            const MemFnType& F = mF;

            ASSERT(1 == F(smartPtr, V1));

            LOOP_ASSERT(X, InplaceTestObj() == X);
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

        if (verbose) cout << "Testing 'bdlf::MemFnInstance':" << endl;

        if (verbose) cout << "\tnon-'const' member functions..." << endl;
        {
            typedef bdlf::MemFnInstance<int (InplaceTestObj::*)(TestArg1),
                                       TestPtrWrapper<InplaceTestObj>
                                      >  MemFnType;

            InplaceTestObj mX; const InplaceTestObj& X = mX;
            const InplaceTestObj EXP(V1);

            TestPtrWrapper<InplaceTestObj> smartPtr(&mX);

            MemFnType mF(bdlf::MemFnUtil::memFn(&InplaceTestObj::testFunc1,
                                               smartPtr));

            const int NUM_DEFAULT_ALLOCS = da.numAllocations();
            const int NUM_ALLOCS         = ta.numAllocations();

            MemFnType mG(mF, (bslma::Allocator*)&ta);  const MemFnType& G = mG;
            ASSERT(1 == G(V1));

            ASSERT(NUM_DEFAULT_ALLOCS == da.numAllocations());
            ASSERT(NUM_ALLOCS         <  ta.numAllocations());

            LOOP_ASSERT(X, EXP == X);
        }

        if (verbose)
            cout << "\t'const' member functions and instances..." << endl;
        {
            typedef bdlf::MemFnInstance<
                              int (InplaceTestObj::*)(TestArg1) const,
                              TestPtrWrapper<const InplaceTestObj> > MemFnType;

            InplaceTestObj::reset();
            InplaceTestObj mX; const InplaceTestObj& X = mX;
            const InplaceTestObj EXP(V1);

            TestPtrWrapper<const InplaceTestObj> smartPtr(&X);

            MemFnType mF(bdlf::MemFnUtil::memFn(&InplaceTestObj::testCFunc1,
                                               smartPtr));

            const int NUM_DEFAULT_ALLOCS = da.numAllocations();
            const int NUM_ALLOCS         = ta.numAllocations();

            MemFnType mG(mF, (bslma::Allocator*)&ta);  const MemFnType& G = mG;
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
            typedef bdlf::MemFnInstance<
                                    int (InplaceTestObj::*)(TestArg1) const,
                                    TestPtrWrapper<InplaceTestObj> > MemFnType;

            InplaceTestObj::reset();
            InplaceTestObj mX; const InplaceTestObj& X = mX;
            const InplaceTestObj EXP(V1);

            TestPtrWrapper<InplaceTestObj> smartPtr(&mX);

            MemFnType mF(bdlf::MemFnUtil::memFn(&InplaceTestObj::testCFunc1,
                                               smartPtr));

            const int NUM_DEFAULT_ALLOCS = da.numAllocations();
            const int NUM_ALLOCS         = ta.numAllocations();

            MemFnType mG(mF, (bslma::Allocator*)&ta);  const MemFnType& G = mG;
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
        // TESTING 'bdlf::MemFnInstance' WITH NON-MODIFIABLE INSTANCE
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
            cout << "\nTESTING 'bdlf::MemFnInstance' WITH NON-MODIFIABLE "
                    "INSTANCE"
                    "\n=================================================="
                    "========\n";

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

            ASSERT(0 ==
                   bdlf::MemFnUtil::memFn(&InplaceTestObj::testCFunc0, EXP)());

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

#undef N
#define N(n) V##n
#undef M
#define M(n)                                                                  \
        if (verbose) cout << "\t" << n << " argument(s)...\n";                \
        {                                                                     \
            InplaceTestObj::reset();                                          \
            static const InplaceTestObj EXP(C##n(N));                         \
                                                                              \
            ASSERT(n ==                                                       \
                   bdlf::MemFnUtil::memFn(&InplaceTestObj::testCFunc##n, EXP) \
                                                                  (C##n(N))); \
                                                                              \
            if (veryVerbose) { P(InplaceTestObj::statics()) }                 \
            LOOP_ASSERT(InplaceTestObj::statics(),                            \
                        EXP == InplaceTestObj::statics());                    \
        }
        L14(M)
      }

DEFINE_TEST_CASE(6) {
        // --------------------------------------------------------------------
        // TESTING 'bdlf::MemFnInstance' WITH PTR TO MODIFIABLE OBJECT
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
            cout << "\nTESTING 'bdlf::MemFnInstance' WITH PTR TO MODIFIABLE "
                    "OBJECT"
                    "\n====================================================="
                    "======\n";

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

            ASSERT(0 ==
                   bdlf::MemFnUtil::memFn(&InplaceTestObj::testFunc0, &x)());

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

#undef N
#define N(n) V##n
#undef M
#define M(n)                                                                  \
        if (verbose) cout << "\t" << n << " argument(s)...\n";                \
        {                                                                     \
            InplaceTestObj x; InplaceTestObj const &X=x;                      \
            static const InplaceTestObj EXP(C##n(N));                         \
                                                                              \
            ASSERT(n ==                                                       \
                   bdlf::MemFnUtil::memFn(&InplaceTestObj::testFunc##n, &x)   \
                                                                  (C##n(N))); \
                                                                              \
            if (veryVerbose) { P(X) }                                         \
            LOOP_ASSERT(X, EXP == X);                                         \
        }
        L14(M)
      }

DEFINE_TEST_CASE(5) {
        // --------------------------------------------------------------------
        // TESTING 'bdlf::MemFn' WITH REFERENCE TO NON-MODIFIABLE OBJECT
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
            cout << "\nTESTING 'bdlf::MemFn' WITH REFERENCE TO "
                    "NON-MODIFIABLE OBJECT"
                    "\n========================================"
                    "=====================\n";

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

            ASSERT(0 ==
                   bdlf::MemFnUtil::memFn(&InplaceTestObj::testCFunc0)(EXP));

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

#undef N
#define N(n) V##n
#undef M
#define M(n)                                                                  \
        if (verbose) cout << "\t" << n << " argument(s)...\n";                \
        {                                                                     \
            InplaceTestObj::reset();                                          \
            static const InplaceTestObj EXP(C##n(N));                         \
                                                                              \
            ASSERT(n ==                                                       \
                   bdlf::MemFnUtil::memFn(&InplaceTestObj::testCFunc##n)      \
                                                             (EXP, C##n(N))); \
                                                                              \
            if (veryVerbose) { P(InplaceTestObj::statics()) }                 \
            LOOP_ASSERT(InplaceTestObj::statics(),                            \
                        EXP == InplaceTestObj::statics());                    \
        }
        L14(M)
      }

DEFINE_TEST_CASE(4) {
        // --------------------------------------------------------------------
        // TESTING 'bdlf::MemFn' WITH REFERENCE TO MODIFIABLE OBJECT
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
            cout << "\nTESTING 'bdlf::MemFn' WITH REFERENCE TO MODIFIABLE "
                    "OBJECT"
                    "\n==================================================="
                    "======\n";

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

            ASSERT(0 == bdlf::MemFnUtil::memFn(&InplaceTestObj::testFunc0)(x));

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

#undef N
#define N(n) V##n
#undef M
#define M(n)                                                                  \
        if (verbose) cout << "\t" << n << " argument(s)...\n";                \
        {                                                                     \
            InplaceTestObj x; InplaceTestObj const &X=x;                      \
            static const InplaceTestObj EXP(C##n(N));                         \
                                                                              \
            ASSERT(n == bdlf::MemFnUtil::memFn(&InplaceTestObj::testFunc##n)  \
                                                               (x, C##n(N))); \
                                                                              \
            if (veryVerbose) { P(X) }                                         \
            LOOP_ASSERT(X, EXP == X);                                         \
        }
        L14(M)
      }

DEFINE_TEST_CASE(3) {
        // --------------------------------------------------------------------
        // TESTING 'bdlf::MemFn' WITH POINTER TO NON-MODIFIABLE OBJECT
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
            cout << "\nTESTING 'bdlf::MemFn' WITH POINTER TO NON-MODIFIABLE "
                    "OBJECT"
                    "\n====================================================="
                    "======\n";

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

            ASSERT(0 == (bdlf::MemFnUtil::memFn(&InplaceTestObj::testCFunc0)
                           (&EXP)));

            if (veryVerbose) { P(InplaceTestObj::statics()); };
            LOOP_ASSERT(InplaceTestObj::statics(),
                        EXP == InplaceTestObj::statics());
        }

#undef N
#define N(n) V##n
#undef M
#define M(n)                                                                  \
        if (verbose) cout << "\t" << n << " argument(s)...\n";                \
        {                                                                     \
            InplaceTestObj::reset();                                          \
            static const InplaceTestObj EXP(C##n(N));                         \
                                                                              \
            ASSERT(n ==                                                       \
                   bdlf::MemFnUtil::memFn(&InplaceTestObj::testCFunc##n)      \
                                                            (&EXP, C##n(N))); \
                                                                              \
            if (veryVerbose) { P(InplaceTestObj::statics()) }                 \
            LOOP_ASSERT(InplaceTestObj::statics(),                            \
                        EXP == InplaceTestObj::statics());                    \
        }
        L14(M)
      }

DEFINE_TEST_CASE(2) {
        // --------------------------------------------------------------------
        // TESTING 'bdlf::MemFn' INVOCATION WITH POINTER TO MODIFIABLE OBJECT
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
            cout << "\nTESTING 'bdlf::MemFn::operator()(&x, ...)'."
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

            ASSERT(0 ==
                   bdlf::MemFnUtil::memFn(&InplaceTestObj::testFunc0)(&x));

            if (veryVerbose) { P(X); };
            LOOP_ASSERT(X, EXP == X);
        }

#undef N
#define N(n) V##n
#undef M
#define M(n)                                                                  \
        if (verbose) cout << "\t" << n << " argument(s)...\n";                \
        {                                                                     \
            InplaceTestObj x; InplaceTestObj const &X=x;                      \
            static const InplaceTestObj EXP(C##n(N));                         \
                                                                              \
            ASSERT(n ==                                                       \
                   bdlf::MemFnUtil::memFn(&InplaceTestObj::testFunc##n)       \
                                                              (&x, C##n(N))); \
                                                                              \
            if (veryVerbose) { P(X) }                                         \
            LOOP_ASSERT(X, EXP == X);                                         \
        }
        L14(M)
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

        bdlf::MemFn<int (TestObject::*)(int)> f1(&TestObject::test1);
        f1(x,10);

        typedef bslmf::MemberFunctionPointerTraits<int (TestObject::*)(int)>
                                                                        Traits;

        TestObject *t = (Traits::ClassType*)0;
        ASSERT(0 == t); // remove a warning on gcc

        bdlf::MemFn<int (TestObject::*)(int) const> f2(
                                                      &TestObject::test1Const);

        f2(&X,10);

        f2(&x,10);

        f2(&x,10);

        f2(&X,10);

        TestPtrWrapper<const TestObject> w(&x);
        TestPtrWrapper<const TestObject> const &W=w;

        f2(w,10);
        f2(W,10);
      }

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    globalVerbose = verbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
#define CASE(NUMBER)                                                          \
      case NUMBER: {                                                          \
        testCase##NUMBER(verbose, veryVerbose, veryVeryVerbose);              \
      } break
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

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
