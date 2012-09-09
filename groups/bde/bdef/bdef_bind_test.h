// bdef_bind_test.h                                                   -*-C++-*-
#ifndef INCLUDED_BDEF_BIND_TEST
#define INCLUDED_BDEF_BIND_TEST

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a test apparatus for bdef_bind
//
//@CLASSES:
//            bdef_Bind_TestSlots: utility class for static test methods
//       bdef_Bind_TestArgNoAlloc: argument type parameterized by index
//      bdef_Bind_TestTypeNoAlloc: invocable supporting up to 14 arguments
// bdef_Bind_TestFunctionsNoAlloc: global versions of test type methods
//         bdef_Bind_TestArgAlloc: argument type parameterized by index
//        bdef_Bind_TestTypeAlloc: invokable supporting up to 14 arguments
//   bdef_Bind_TestFunctionsAlloc: global versions of test type methods
//
//@AUTHOR: Herve Bronnimann (hbronnimann)
//
//@SEE_ALSO: bdef_bind bdef_bind_test[0--14]
//
//@DESCRIPTION: This component provides a test apparatus for testing the
// 'bdef_bind' component.  This apparatus would usually be part of the test
// driver of that component, but is factored out in this component since the
// 'bdef_bind' test driver is so large and induces such long compilation times
// that the testing has been delegated to many components
// 'bdef_bind_test[0-14]' whose sole purpose is to share the testing
// 'bdef_bind'.  Thus this component provides all the common apparatus for
// these delegate components.
//
// This apparatus comprises test classes for tracking argument values and
// allocators, using a slot mechanism.  In the case of a test class that
// does *not* take allocators, the slot at any index contains an integer which
// tracks the value of the argument with same index that was passed to the test
// type invocation.  In the case of a test class that *does* take allocators,
// the slot at any index contains the address of the allocator used to allocate
// the value of the argument with same index that was passed to the test type
// invocation.
//
// The two test types provided, 'bdef_Bind_TestTypeNoAlloc' and
// 'bdef_Bind_TestTypeAlloc' support this slot mechanism.  They are invocable
// with up to 14 parameters, either as function objects (using 'operator()'),
// via member functions (using the 'testFunc[0--14]' methods), or via global
// functions that take a pointer to the object as first argument (provided as
// static methods of the utility classes 'bdef_Bind_TestFunctionsNoAlloc' and
// 'bdef_Bind_TestFunctionsAlloc'), thus supporting all protocols for binding
// an invocable in the 'bdef_bind' component.
//
///Usage
///-----
// The following shows how to use the test apparatus to test the 'bdef_Bind'
// constructors, i.e., to explicitly construct 'bdef_Bind' objects (as opposed
// to using the factory method 'bdef_BindUtil::bind') and exercise them to
// ascertain that they work as expected.  For more examples of use, see the
// test drivers 'bdef_bind_test[0--14]' themselves.
//
///Testing 'bdef_Bind' *without* allocators
///- - - - - - - - - - - - - - - - - - - -
// In this test code, we bind a 'bdef_Bind_TestTypeNoAlloc' invocable by a
// 'bdef_Bind' object constructed with and without placeholders.
//..
//  void usageExampleNoAlloc(int veryVeryVerbose)
//  {
//      bslma_TestAllocator          allocator0(veryVeryVerbose);
//      bslma_TestAllocator         *Z0 = &allocator0;
//      bslma_DefaultAllocatorGuard  allocGuard(Z0);
//
//      const int                         N1 = -1;
//      const bdef_Bind_TestArgNoAlloc<1> I1 = 1;
//
//      // 1 argument to function object, without placeholders.
//      {
//          const int NUM_ALLOCS = Z0->numAllocation();
//
//                bdef_Bind_TestTypeNoAlloc  mX;
//          const bdef_Bind_TestTypeNoAlloc& X = mX;
//
//          const bdef_Bind_TestTypeNoAlloc EXPECTED_X(I1);
//          const bdef_Bind_TestTypeNoAlloc DEFAULT_X(N1);
//          ASSERT(EXPECTED_X != X);
//          ASSERT(DEFAULT_X  == X);
//
//          // For passing to the constructor of 'bdef_Bind'.
//
//          typedef bdef_Bind_TestTypeNoAlloc *FUNC;
//          typedef bdef_Bind_Tuple1<bdef_Bind_TestArgNoAlloc<1> > ListType;
//          typedef bdef_Bind<bslmf_Nil, FUNC, ListType> Bind;
//
//                ListType  mL(I1);     // list of arguments
//          const ListType& L = mL;     // non-modifiable list of arguments
//
//                ListType  mM(N1);     // list of arguments
//          const ListType& M = mM;     // non-modifiable list of arguments
//
//          // Testing 'bdef_Bind' objects constructed explicitly.
//
//                Bind  mB1(&mX, L);
//          const Bind& B1 = mB1;
//          ASSERT(EXPECTED_X != X);
//          ASSERT(1 == mB1());
//          ASSERT(EXPECTED_X == X);
//
//                Bind  mB2(&mX, M, Z0);
//          const Bind& B2 = mB2;
//          ASSERT(DEFAULT_X != X);
//          ASSERT(1 == mB2());
//          ASSERT(DEFAULT_X == X);
//
//          Bind mB3(B1);
//          ASSERT(EXPECTED_X != X);
//          ASSERT(1 == mB3());
//          ASSERT(EXPECTED_X == X);
//
//          Bind mB4(B2, Z0);
//          ASSERT(DEFAULT_X != X);
//          ASSERT(1 == mB4());
//          ASSERT(DEFAULT_X == X);
//
//          ASSERT(NUM_ALLOCS == Z0->numAllocation());
//      }
//
//      // 1 argument to function object, *with* placeholders.
//      {
//          using namespace bdef_PlaceHolders;
//
//          const int NUM_ALLOCS = Z0->numAllocation();
//
//                bdef_Bind_TestTypeNoAlloc  mX;
//          const bdef_Bind_TestTypeNoAlloc& X = mX;
//
//          const bdef_Bind_TestTypeNoAlloc EXPECTED_X(I1);
//          const bdef_Bind_TestTypeNoAlloc DEFAULT_X(N1);
//          ASSERT(EXPECTED_X != X);
//          ASSERT(DEFAULT_X  == X);
//
//          // For passing to the constructor of 'bdef_Bind'.
//
//          typedef bdef_Bind_TestTypeNoAlloc *FUNC;
//          typedef bdef_Bind_Tuple1<PH1> ListType;
//          typedef bdef_Bind<bslmf_Nil, FUNC, ListType> Bind;
//
//                ListType  mL(_1);     // list of arguments
//          const ListType& L = mL;     // non-modifiable list of arguments
//
//                Bind  mB1(&mX, L);
//          const Bind& B1 = mB1;
//          ASSERT(EXPECTED_X != X);
//          ASSERT(1 == mB1(I1));
//          ASSERT(EXPECTED_X == X);
//
//                Bind  mB2(&mX, L, Z0);
//          const Bind& B2 = mB2;
//          ASSERT(DEFAULT_X != X);
//          ASSERT(1 == mB2(N1));
//          ASSERT(DEFAULT_X == X);
//
//          Bind mB3(B1);
//          ASSERT(EXPECTED_X != X);
//          ASSERT(1 == mB3(I1));
//          ASSERT(EXPECTED_X == X);
//
//          Bind mB4(B2, Z0);
//          ASSERT(DEFAULT_X != X);
//          ASSERT(1 == mB4(N1));
//          ASSERT(DEFAULT_X == X);
//
//          ASSERT(NUM_ALLOCS == Z0->numAllocation());
//      }
//  }
//..
//
///Testing 'bdef_bind' *with* allocators
///- - - - - - - - - - - - - - - - - - - -
// In this test code, we bind a 'bdef_Bind_TestTypeAlloc' invocable using a
// 'bdef_Bind' object constructed with and without placeholders.
//..
//  void usageExampleAlloc(int veryVeryVerbose)
//  {
//      bslma_TestAllocator  allocator0(veryVeryVerbose);
//      bslma_TestAllocator  allocator1(veryVeryVerbose);
//      bslma_TestAllocator  allocator2(veryVeryVerbose);
//
//      bslma_TestAllocator *Z0 = &allocator0;
//      bslma_TestAllocator *Z1 = &allocator1;
//      bslma_TestAllocator *Z2 = &allocator2;
//
//      bslma_DefaultAllocatorGuard allocGuard(Z0);
//
//      const bdef_Bind_TestArgAlloc<1> NV1 = -1;
//      const bdef_Bind_TestArgAlloc<1>  V1 =  1;
//
//      // 1 argument to function object, *without* placeholders.
//      {
//                bdef_Bind_TestTypeAlloc  mX(Z1);
//          const bdef_Bind_TestTypeAlloc& X = mX;
//
//          const bdef_Bind_TestTypeAlloc EXPECTED_X(Z1, V1);
//          const bdef_Bind_TestTypeAlloc DEFAULT_X(Z1, NV1);
//          ASSERT(EXPECTED_X != X);
//          ASSERT(DEFAULT_X  == X);
//
//          typedef bdef_Bind_TestTypeAlloc *FUNC;
//          typedef bdef_Bind_Tuple1<bdef_Bind_TestArgAlloc<1> > ListType;
//          typedef bdef_Bind<bslmf_Nil, FUNC, ListType> Bind;
//
//          // For passing to the constructor of 'bdef_Bind'.
//
//                ListType  mL(V1);  // list of arguments
//          const ListType& L = mL;  // non-modifiable list of arguments
//
//                ListType  mM(NV1); // list of arguments
//          const ListType& M = mM;  // non-modifiable list of arguments
//
//                Bind  mB1(&mX, L);
//          const Bind& B1 = mB1;
//          ASSERT(EXPECTED_X != X);
//          ASSERT(1 == mB1());
//          ASSERT(EXPECTED_X == X);
//
//                Bind  mB2(&mX, M, Z2);
//          const Bind& B2 = mB2;
//          ASSERT(DEFAULT_X != X);
//          ASSERT(1 == mB2());
//          ASSERT(DEFAULT_X == X);
//
//          Bind mB3(B1);
//          ASSERT(EXPECTED_X != X);
//          ASSERT(1 == mB3());
//          ASSERT(EXPECTED_X == X);
//
//          Bind mB4(B2, Z2);
//          ASSERT(DEFAULT_X != X);
//          ASSERT(1 == mB4());
//          ASSERT(DEFAULT_X == X);
//      }
//
//      // 1 argument to function object, *with* placeholders.
//      {
//          using namespace bdef_PlaceHolders;
//
//                bdef_Bind_TestTypeAlloc  mX(Z1);
//          const bdef_Bind_TestTypeAlloc& X = mX;
//
//          const bdef_Bind_TestTypeAlloc EXPECTED_X(Z1, V1);
//          const bdef_Bind_TestTypeAlloc DEFAULT_X(Z1, NV1);
//          ASSERT(EXPECTED_X != X);
//          ASSERT(DEFAULT_X  == X);
//
//          typedef bdef_Bind_TestTypeAlloc *FUNC;
//          typedef bdef_Bind_Tuple1<PH1> ListType;
//          typedef bdef_Bind<bslmf_Nil, FUNC, ListType> Bind;
//
//          // For passing to the constructor of 'bdef_Bind'.
//
//                ListType  mL(_1);  // list of arguments
//          const ListType& L = mL;  // non-modifiable list of arguments
//
//                Bind  mB1(&mX, L);
//          const Bind& B1 = mB1;
//          ASSERT(EXPECTED_X != X);
//          ASSERT(1 == mB1(V1));
//          ASSERT(EXPECTED_X == X);
//
//                Bind  mB2(&mX, L, Z2);
//          const Bind& B2 = mB2;
//          ASSERT(DEFAULT_X != X);
//          ASSERT(1 == mB2(NV1));
//          ASSERT(DEFAULT_X == X);
//
//          Bind mB3(B1);
//          ASSERT(EXPECTED_X != X);
//          ASSERT(1 == mB3(V1));
//          ASSERT(EXPECTED_X == X);
//
//          Bind mB4(B2, Z2);
//          ASSERT(DEFAULT_X != X);
//          ASSERT(1 == mB4(NV1));
//          ASSERT(DEFAULT_X == X);
//      }
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSL_CSTDIO
#include <bsl_cstdio.h>
#endif


namespace BloombergLP {

                      // =========================
                      // class bdef_Bind_TestSlots
                      // =========================

template <class VALUE>
struct bdef_Bind_TestSlotsBase {
    // This 'struct' defines an array of 'VALUE' to keep track (in conjunction
    // with the 'bdef_Bind_TestTypeNoAlloc' or 'bdef_Bind_TestTypeAlloc'
    // classes) of which value or allocator is being passed to which argument
    // of the test function by a 'bdef_bind' object.

    // ENUMERATIONS
    enum {
        NUM_SLOTS = 15
    };

  private:
    // PRIVATE CLASS DATA
    static VALUE d_slots[NUM_SLOTS];

  public:
    // CLASS METHODS
    static VALUE getSlot(int index);
        // Get the value of the slot at the specified 'index'.

    static void resetSlots(VALUE value = 0);
        // Reset all the slots in this instance to the specified 'value'.

    static void setSlot(VALUE value, int index);
        // Set the slot at the specified 'index' to the specified 'value'.

    static bool verifySlots(const VALUE *EXPECTED, bool verboseFlag = true);
        // Verify that all the slots in this instance compare equal to those in
        // the specified 'EXPECTED' value.  If the specified 'verboseFlag' is
        // not set, output diagnostic only in case the slots do not compare
        // equal.
};

                      // ===============================
                      // type bdef_Bind_TestSlotsNoAlloc
                      // ===============================

typedef bdef_Bind_TestSlotsBase<int> bdef_Bind_TestSlotsNoAlloc;
    // When used within the methods of 'bdef_Bind_TestTypeNoAlloc', the 'VALUE'
    // type will be 'int' and will keep track of which arguments have been
    // assigned a value (in case 'bdef_bind' accesses fields that it
    // should not).

                      // ==============================
                      // class bdef_Bind_TestArgNoAlloc
                      // ==============================

template <int ID>
class bdef_Bind_TestArgNoAlloc {
    // This very simple 'struct' is used purely to disambiguate types in
    // passing parameters due to the fact that 'bdef_Bind_TestArgNoAlloc<ID1>'
    // is a different type than 'bdef_Bind_TestArgNoAlloc<ID2>' is ID1 != ID2.
    // This class does not take an optional allocator.

    // INSTANCE DATA
    int d_value; // value held by this object

  public:
    // CREATORS
    bdef_Bind_TestArgNoAlloc(int value);                            // IMPLICIT
        // Create an object having the specified 'value'.

    // MANIPULATORS
    bdef_Bind_TestArgNoAlloc& operator=(const bdef_Bind_TestArgNoAlloc &rhs);
        // Assign to this object the value of the specified 'rhs' object.

    // ACCESSORS
    int value() const;
};

// FREE OPERATORS
template <int ID>
inline
bool operator==(bdef_Bind_TestArgNoAlloc<ID> const& lhs,
                bdef_Bind_TestArgNoAlloc<ID> const& rhs);

template <int ID>
inline
bool operator!=(bdef_Bind_TestArgNoAlloc<ID> const& lhs,
                bdef_Bind_TestArgNoAlloc<ID> const& rhs);

                      // ===============================
                      // class bdef_Bind_TestTypeNoAlloc
                      // ===============================

class bdef_Bind_TestTypeNoAlloc {
    // This 'struct' provides a test class capable of holding up to 14 bound
    // parameters of types 'TestArgNoAlloc[1--14]', with full (non-streamable)
    // value semantics defined by the 'operator=='.  By default, a
    // 'bdef_Bind_TestTypeNoAlloc' is constructed with nil ('N1') values, but
    // instances can be constructed with actual values (e.g., for creating
    // expected values).  A 'bdef_Bind_TestTypeNoAlloc' can be invoked with up
    // to 14 parameters, via member functions 'testFunc[1--14]'.  These
    // functions are also called by the overloaded member 'operator()' of the
    // same signatures, and similar global functions 'testFunc[1--14]'.  All
    // invocations support the above 'bdef_Bind_TestSlotsNoAlloc' mechanism.
    //
    // This 'struct' intentionally does *not* take an allocator.

    // PRIVATE TYPES
    typedef bdef_Bind_TestArgNoAlloc<1>  Arg1;
    typedef bdef_Bind_TestArgNoAlloc<2>  Arg2;
    typedef bdef_Bind_TestArgNoAlloc<3>  Arg3;
    typedef bdef_Bind_TestArgNoAlloc<4>  Arg4;
    typedef bdef_Bind_TestArgNoAlloc<5>  Arg5;
    typedef bdef_Bind_TestArgNoAlloc<6>  Arg6;
    typedef bdef_Bind_TestArgNoAlloc<7>  Arg7;
    typedef bdef_Bind_TestArgNoAlloc<8>  Arg8;
    typedef bdef_Bind_TestArgNoAlloc<9>  Arg9;
    typedef bdef_Bind_TestArgNoAlloc<10> Arg10;
    typedef bdef_Bind_TestArgNoAlloc<11> Arg11;
    typedef bdef_Bind_TestArgNoAlloc<12> Arg12;
    typedef bdef_Bind_TestArgNoAlloc<13> Arg13;
    typedef bdef_Bind_TestArgNoAlloc<14> Arg14;
        // Argument types for shortcut.

    // PRIVATE DATA
    mutable Arg1  d_a1;
    mutable Arg2  d_a2;
    mutable Arg3  d_a3;
    mutable Arg4  d_a4;
    mutable Arg5  d_a5;
    mutable Arg6  d_a6;
    mutable Arg7  d_a7;
    mutable Arg8  d_a8;
    mutable Arg9  d_a9;
    mutable Arg10 d_a10;
    mutable Arg11 d_a11;
    mutable Arg12 d_a12;
    mutable Arg13 d_a13;
    mutable Arg14 d_a14;

    // FRIEND
    friend bool operator==(const bdef_Bind_TestTypeNoAlloc& lhs,
                           const bdef_Bind_TestTypeNoAlloc& rhs);

  public:
    // TYPES
    typedef int ResultType;
        // Type returned by the function operator and test methods.

    enum {
        N1 = -1   // default value for all private data
    };

    // CREATORS
    explicit bdef_Bind_TestTypeNoAlloc(Arg1  a1  = N1, Arg2  a2  = N1, Arg3  a3  = N1,
              Arg4  a4  = N1, Arg5  a5  = N1, Arg6  a6  = N1, Arg7  a7  = N1,
              Arg8  a8  = N1, Arg9  a9  = N1, Arg10 a10 = N1, Arg11 a11 = N1,
              Arg12 a12 = N1, Arg13 a13 = N1, Arg14 a14 = N1);
        // Create a test object having the same value as the specified
        // 'original'.

    bdef_Bind_TestTypeNoAlloc(const bdef_Bind_TestTypeNoAlloc& original);
        // Create a test object having the same value as the specified
        // 'original'.

    // MANIPULATORS
    bdef_Bind_TestTypeNoAlloc& operator=(const bdef_Bind_TestTypeNoAlloc &rhs);
        // Assign to this object the value of the specified 'rhs' object.

    int operator()() const;
    int operator()(Arg1  const& a1) const;
    int operator()(Arg1  const& a1,  Arg2  const& a2) const;
    int operator()(Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3) const;
    int operator()(Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4) const;
    int operator()(Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5) const;
    int operator()(Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6) const;
    int operator()(Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
                   Arg7  const& a7) const;
    int operator()(Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
                   Arg7  const& a7,  Arg8  const& a8) const;
    int operator()(Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
                   Arg7  const& a7,  Arg8  const& a8,  Arg9  const& a9) const;
    int operator()(Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
                   Arg7  const& a7,  Arg8  const& a8,  Arg9  const& a9,
                   Arg10 const& a10) const;
    int operator()(Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
                   Arg7  const& a7,  Arg8  const& a8,  Arg9  const& a9,
                   Arg10 const& a10, Arg11 const& a11) const;
    int operator()(Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
                   Arg7  const& a7,  Arg8  const& a8,  Arg9  const& a9,
                   Arg10 const& a10, Arg11 const& a11, Arg12 const& a12) const;
    int operator()(Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
                   Arg7  const& a7,  Arg8  const& a8,  Arg9  const& a9,
                   Arg10 const& a10, Arg11 const& a11, Arg12 const& a12,
                   Arg13 const& a13) const;
    int operator()(Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
                   Arg7  const& a7,  Arg8  const& a8,  Arg9  const& a9,
                   Arg10 const& a10, Arg11 const& a11, Arg12 const& a12,
                   Arg13 const& a13, Arg14 const& a14) const;
        // Test operators invoking this test type with 0 up to 14 arguments.

    int testFunc0 () const;
    int testFunc1 (Arg1  const& a1) const;
    int testFunc2 (Arg1  const& a1,  Arg2  const& a2) const;
    int testFunc3 (Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3) const;
    int testFunc4 (Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4) const;
    int testFunc5 (Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5) const;
    int testFunc6 (Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6) const;
    int testFunc7 (Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
                   Arg7  const& a7) const;
    int testFunc8 (Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
                   Arg7  const& a7,  Arg8  const& a8) const;
    int testFunc9 (Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
                   Arg7  const& a7,  Arg8  const& a8,  Arg9  const& a9) const;
    int testFunc10(Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
                   Arg7  const& a7,  Arg8  const& a8,  Arg9  const& a9,
                   Arg10 const& a10) const;
    int testFunc11(Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
                   Arg7  const& a7,  Arg8  const& a8,  Arg9  const& a9,
                   Arg10 const& a10, Arg11 const& a11) const;
    int testFunc12(Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
                   Arg7  const& a7,  Arg8  const& a8,  Arg9  const& a9,
                   Arg10 const& a10, Arg11 const& a11, Arg12 const& a12) const;
    int testFunc13(Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
                   Arg7  const& a7,  Arg8  const& a8,  Arg9  const& a9,
                   Arg10 const& a10, Arg11 const& a11, Arg12 const& a12,
                   Arg13 const& a13) const;
    int testFunc14(Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
                   Arg7  const& a7,  Arg8  const& a8,  Arg9  const& a9,
                   Arg10 const& a10, Arg11 const& a11, Arg12 const& a12,
                   Arg13 const& a13, Arg14 const& a14) const;
        // Test methods invoking this test type with 0 up to 14 arguments.


    // ACCESSORS
    void print() const;
        // Output the value of this instance to the standard output.
};

// FREE OPERATORS
inline
bool operator==(const bdef_Bind_TestTypeNoAlloc& lhs,
                const bdef_Bind_TestTypeNoAlloc& rhs);

inline
bool operator!=(const bdef_Bind_TestTypeNoAlloc& lhs,
                const bdef_Bind_TestTypeNoAlloc& rhs);

                        // ====================================
                        // class bdef_Bind_TestFunctionsNoAlloc
                        // ====================================

struct bdef_Bind_TestFunctionsNoAlloc {
    // Global versions of 'bdef_Bind_TestTypeNoAlloc' member functions.

    // TYPES
    typedef bdef_Bind_TestArgNoAlloc<1>  Arg1;
    typedef bdef_Bind_TestArgNoAlloc<2>  Arg2;
    typedef bdef_Bind_TestArgNoAlloc<3>  Arg3;
    typedef bdef_Bind_TestArgNoAlloc<4>  Arg4;
    typedef bdef_Bind_TestArgNoAlloc<5>  Arg5;
    typedef bdef_Bind_TestArgNoAlloc<6>  Arg6;
    typedef bdef_Bind_TestArgNoAlloc<7>  Arg7;
    typedef bdef_Bind_TestArgNoAlloc<8>  Arg8;
    typedef bdef_Bind_TestArgNoAlloc<9>  Arg9;
    typedef bdef_Bind_TestArgNoAlloc<10> Arg10;
    typedef bdef_Bind_TestArgNoAlloc<11> Arg11;
    typedef bdef_Bind_TestArgNoAlloc<12> Arg12;
    typedef bdef_Bind_TestArgNoAlloc<13> Arg13;
    typedef bdef_Bind_TestArgNoAlloc<14> Arg14;
        // Types for shortcut.

    // CLASS METHODS
    static
    int func0(bdef_Bind_TestTypeNoAlloc *object);
    static
    int func1(bdef_Bind_TestTypeNoAlloc *object,
        Arg1 const& a1);
    static
    int func2(bdef_Bind_TestTypeNoAlloc *object,
        Arg1 const& a1, Arg2 const& a2);
    static
    int func3(bdef_Bind_TestTypeNoAlloc *object,
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3);
    static
    int func4(bdef_Bind_TestTypeNoAlloc *object,
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4);
    static
    int func5(bdef_Bind_TestTypeNoAlloc *object,
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4,
        Arg5 const& a5);
    static
    int func6(bdef_Bind_TestTypeNoAlloc *object,
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4,
        Arg5 const& a5, Arg6 const& a6);
    static
    int func7(bdef_Bind_TestTypeNoAlloc *object,
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4,
        Arg5 const& a5, Arg6 const& a6, Arg7 const& a7);
    static
    int func8(bdef_Bind_TestTypeNoAlloc *object,
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4,
        Arg5 const& a5, Arg6 const& a6, Arg7 const& a7, Arg8 const& a8);
    static
    int func9(bdef_Bind_TestTypeNoAlloc *object,
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4,
        Arg5 const& a5, Arg6 const& a6, Arg7 const& a7, Arg8 const& a8,
        Arg9 const& a9);
    static
    int func10(bdef_Bind_TestTypeNoAlloc *object,
        Arg1 const& a1, Arg2  const& a2,  Arg3  const& a3,  Arg4  const& a4,
        Arg5 const& a5, Arg6  const& a6,  Arg7  const& a7,  Arg8  const& a8,
        Arg9 const& a9, Arg10 const& a10);
    static
    int func11(bdef_Bind_TestTypeNoAlloc *object,
        Arg1 const& a1, Arg2  const& a2,  Arg3  const& a3,  Arg4  const& a4,
        Arg5 const& a5, Arg6  const& a6,  Arg7  const& a7,  Arg8  const& a8,
        Arg9 const& a9, Arg10 const& a10, Arg11 const& a11);
    static
    int func12(bdef_Bind_TestTypeNoAlloc *object,
        Arg1 const& a1, Arg2  const& a2,  Arg3  const& a3,  Arg4  const& a4,
        Arg5 const& a5, Arg6  const& a6,  Arg7  const& a7,  Arg8  const& a8,
        Arg9 const& a9, Arg10 const& a10, Arg11 const& a11, Arg12 const& a12);
    static
    int func13(bdef_Bind_TestTypeNoAlloc *object,
        Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,  Arg4  const& a4,
        Arg5  const& a5,  Arg6  const& a6,  Arg7  const& a7,  Arg8  const& a8,
        Arg9  const& a9,  Arg10 const& a10, Arg11 const& a11, Arg12 const& a12,
        Arg13 const& a13);
    static
    int func14(bdef_Bind_TestTypeNoAlloc *object,
        Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,  Arg4  const& a4,
        Arg5  const& a5,  Arg6  const& a6,  Arg7  const& a7,  Arg8  const& a8,
        Arg9  const& a9,  Arg10 const& a10, Arg11 const& a11, Arg12 const& a12,
        Arg13 const& a13, Arg14 const& a14);
        // Invoke the corresponding method 'testFunc[0-14]' on the specified
        // 'object' with the specified arguments 'a[0-14]'.
};

                      // ==================================
                      // class bdef_Bind_TestSlotsAllocBase
                      // ==================================

template <class AllocPtr>
struct bdef_Bind_TestSlotsAllocBase
{
    // The point of this base class is that we would like it to be a class
    // template, so that the class data does not pollute the library object
    // files.  Since there is no reason to make 'bdef_Bind_TestSlotsAlloc' a
    // template, we separate the static pointers into this base class.  These
    // pointers are needed for the pretty printing in the 'verifySlots' method.

    // PRIVATE CLASS DATA
    static AllocPtr d_Z0;
    static AllocPtr d_Z1;
    static AllocPtr d_Z2;

  public:
    // CLASS METHODS
    static void setZ0(AllocPtr Z0);
    static void setZ1(AllocPtr Z1);
    static void setZ2(AllocPtr Z2);
    static AllocPtr getZ0();
    static AllocPtr getZ1();
    static AllocPtr getZ2();
};

                      // ==============================
                      // class bdef_Bind_TestSlotsAlloc
                      // ==============================

class bdef_Bind_TestSlotsAlloc
: public bdef_Bind_TestSlotsBase<const bslma_Allocator*>
, public bdef_Bind_TestSlotsAllocBase<const bslma_Allocator*>
{
    // When used within the methods of 'bdef_Bind_TestTypeAlloc', the 'VALUE'
    // type will be 'const bslma_Allocator*' and will keep track of which
    // allocator is used with which argument of the 'bdef_Bind' object.  The
    // sole purpose of deriving a new class is to use pretty printing of the
    // allocators in 'verifySlots'.

  public:
    static bool verifySlots(const bslma_Allocator * const *EXPECTED,
                            bool                           verboseFlag = true);
        // Verify that all the slots in this instance compare equal to those in
        // the specified 'EXPECTED' value.  If the specified 'verboseFlag' is
        // not set, output diagnostic only in case the slots do not compare
        // equal.
};

                      // ==============================
                      // class bdef_Bind_TestArgAlloc
                      // ==============================

template <int ID>
class bdef_Bind_TestArgAlloc {
    // This class is used to disambiguate types in passing parameters due to
    // the fact that 'bdef_Bind_TestArgNoAlloc<ID1>' is a different type than
    // 'bdef_Bind_TestArgNoAlloc<ID2>' is ID1 != ID2.  This class is used for
    // testing memory allocator issues.

    // PRIVATE DATA
    bslma_Allocator *d_allocator_p;  // memory allocator (held, not owned)
    int             *d_value;        // value allocated from 'd_allocator_p'
        // NOTE: *Must* be declared and initialized in that order.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdef_Bind_TestArgAlloc,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS

#if defined(BSLS_PLATFORM_CMP_IBM) && BSLS_PLATFORM_CMP_VER_MAJOR >= 0x0900
    // TBD: This is probably an IBM xlC compiler bug.  Reproduce in a small
    // test case later.
    bdef_Bind_TestArgAlloc(int value);                               // IMPLICT
    bdef_Bind_TestArgAlloc(int value, bslma_Allocator *allocator);
#else
    bdef_Bind_TestArgAlloc(int value,
                           bslma_Allocator *allocator = 0);         // IMPLICIT
#endif
        // Create an object having the specified 'value'.  Use the specified
        // 'allocator' to supply memory.  If 'allocator' is 0, use the
        // currently installed default allocator.

#if defined(BSLS_PLATFORM_CMP_IBM) && BSLS_PLATFORM_CMP_VER_MAJOR >= 0x0900
    bdef_Bind_TestArgAlloc(const bdef_Bind_TestArgAlloc&  original);
    bdef_Bind_TestArgAlloc(const bdef_Bind_TestArgAlloc&  original,
                           bslma_Allocator               *allocator);
#else
    bdef_Bind_TestArgAlloc(const bdef_Bind_TestArgAlloc&  original,
                           bslma_Allocator               *allocator = 0);
#endif
        // Create a copy of the specified non-modifiable 'original'.    Use the
        // specified 'allocator' to supply memory.  If 'allocator' is 0, use
        // the currently installed default allocator.

    ~bdef_Bind_TestArgAlloc();
        // Destroy this object.

    // MANIPULATORS
    bdef_Bind_TestArgAlloc& operator=(const bdef_Bind_TestArgAlloc &rhs);
        // Assign to this object the value of the specified 'rhs' object.

    // ACCESSORS
    const bslma_Allocator *allocator() const;
        // Return the allocator specified at construction of this object.

    int value() const;
        // Return the value of this object.
};

// FREE OPERATORS
template <int ID>
inline
bool operator==(const bdef_Bind_TestArgAlloc<ID>& lhs,
                const bdef_Bind_TestArgAlloc<ID>& rhs);

template <int ID>
inline
bool operator!=(const bdef_Bind_TestArgAlloc<ID>& lhs,
                const bdef_Bind_TestArgAlloc<ID>& rhs);

                      // =============================
                      // class bdef_Bind_TestTypeAlloc
                      // =============================

class bdef_Bind_TestTypeAlloc {
    // This class provides a test class capable of holding up to 14 bound
    // parameters of types 'bdef_Bind_TestArgAlloc[1--14]', with full
    // (non-streamable) value semantics defined by the 'operator=='.  By
    // default, a 'bdef_Bind_TestTypeAlloc' is constructed with nil ('N1')
    // values, but objects can be constructed with actual values (e.g., for
    // creating expected values).  A 'bdef_Bind_TestTypeAlloc' can be invoked
    // with up to 14 parameters, via member functions 'testFunc[1--14]'.  These
    // functions are also called by the overloaded member 'operator()' of the
    // same signatures, and similar global functions 'testFunc1--14'.  All
    // invocations support the above 'bdef_Bind_TestSlotsAlloc' mechanism.
    //
    // This class intentionally *does* take an allocator.

    // PRIVATE TYPES
    typedef bdef_Bind_TestArgAlloc<1>  Arg1;
    typedef bdef_Bind_TestArgAlloc<2>  Arg2;
    typedef bdef_Bind_TestArgAlloc<3>  Arg3;
    typedef bdef_Bind_TestArgAlloc<4>  Arg4;
    typedef bdef_Bind_TestArgAlloc<5>  Arg5;
    typedef bdef_Bind_TestArgAlloc<6>  Arg6;
    typedef bdef_Bind_TestArgAlloc<7>  Arg7;
    typedef bdef_Bind_TestArgAlloc<8>  Arg8;
    typedef bdef_Bind_TestArgAlloc<9>  Arg9;
    typedef bdef_Bind_TestArgAlloc<10> Arg10;
    typedef bdef_Bind_TestArgAlloc<11> Arg11;
    typedef bdef_Bind_TestArgAlloc<12> Arg12;
    typedef bdef_Bind_TestArgAlloc<13> Arg13;
    typedef bdef_Bind_TestArgAlloc<14> Arg14;
        // Argument types for shortcut.

    enum {
        N1 = -1   // default value for all private data
    };

    // PRIVATE DATA
    mutable Arg1  d_a1;
    mutable Arg2  d_a2;
    mutable Arg3  d_a3;
    mutable Arg4  d_a4;
    mutable Arg5  d_a5;
    mutable Arg6  d_a6;
    mutable Arg7  d_a7;
    mutable Arg8  d_a8;
    mutable Arg9  d_a9;
    mutable Arg10 d_a10;
    mutable Arg11 d_a11;
    mutable Arg12 d_a12;
    mutable Arg13 d_a13;
    mutable Arg14 d_a14;

    // FRIEND
    friend bool operator==(const bdef_Bind_TestTypeAlloc& lhs,
                           const bdef_Bind_TestTypeAlloc& rhs);

  public:
    // PUBLIC TYPES
    typedef int ResultType;
        // Type returned by the function operator and test methods.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdef_Bind_TestTypeAlloc,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit bdef_Bind_TestTypeAlloc(bslma_Allocator *allocator = 0,
            Arg1  a1  = N1, Arg2  a2  = N1, Arg3  a3  = N1,
            Arg4  a4  = N1, Arg5  a5  = N1, Arg6  a6  = N1,
            Arg7  a7  = N1, Arg8  a8  = N1, Arg9  a9  = N1,
            Arg10 a10 = N1, Arg11 a11 = N1, Arg12 a12 = N1,
            Arg13 a13 = N1, Arg14 a14 = N1);
        // This constructor does *not* participate in the
        // 'UsesBdemaAllocatorTraits' contract, it is here simply to allow to
        // construct expected values with a 'specified' allocator as the first
        // argument (otherwise there would need to be fourteen different
        // constructors with 'allocator' as the last argument).

    bdef_Bind_TestTypeAlloc(Arg1  a1, Arg2  a2, Arg3  a3, Arg4  a4, Arg5  a5,
            Arg6  a6, Arg7  a7, Arg8  a8, Arg9  a9, Arg10 a10, Arg11 a11,
            Arg12 a12, Arg13 a13, Arg14 a14, bslma_Allocator *allocator = 0);

    bdef_Bind_TestTypeAlloc(const bdef_Bind_TestTypeAlloc&  original,
                            bslma_Allocator                *allocator = 0);

    // MANIPULATORS
    bdef_Bind_TestTypeAlloc& operator=(const bdef_Bind_TestTypeAlloc &rhs);
        // Assign to this object the value of the specified 'rhs' object.

    int operator()() const;
    int operator()(Arg1  const& a1)  const;
    int operator()(Arg1  const& a1,  Arg2  const& a2)  const;
    int operator()(Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3) const;
    int operator()(Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4) const;
    int operator()(Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5)  const;
    int operator()(Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6) const;
    int operator()(Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
                   Arg7  const& a7) const;
    int operator()(Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
                   Arg7  const& a7,  Arg8  const& a8) const;
    int operator()(Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
                   Arg7  const& a7,  Arg8  const& a8,  Arg9  const& a9) const;
    int operator()(Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
                   Arg7  const& a7,  Arg8  const& a8,  Arg9  const& a9,
                   Arg10 const& a10) const;
    int operator()(Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
                   Arg7  const& a7,  Arg8  const& a8,  Arg9  const& a9,
                   Arg10 const& a10, Arg11 const& a11) const;
    int operator()(Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
                   Arg7  const& a7,  Arg8  const& a8,  Arg9  const& a9,
                   Arg10 const& a10, Arg11 const& a11, Arg12 const& a12) const;
    int operator()(Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
                   Arg7  const& a7,  Arg8  const& a8,  Arg9  const& a9,
                   Arg10 const& a10, Arg11 const& a11, Arg12 const& a12,
                   Arg13 const& a13) const;
    int operator()(Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
                   Arg7  const& a7,  Arg8  const& a8,  Arg9  const& a9,
                   Arg10 const& a10, Arg11 const& a11, Arg12 const& a12,
                   Arg13 const& a13, Arg14 const& a14) const;
        // Test operators invoking this test type with 0 up to 14 arguments.

    int testFunc0 () const;
    int testFunc1 (Arg1  const& a1) const;
    int testFunc2 (Arg1  const& a1,  Arg2  const& a2) const;
    int testFunc3 (Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3) const;
    int testFunc4 (Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4) const;
    int testFunc5 (Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5) const;
    int testFunc6 (Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6) const;
    int testFunc7 (Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
                   Arg7  const& a7) const;
    int testFunc8 (Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
                   Arg7  const& a7,  Arg8  const& a8) const;
    int testFunc9 (Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
                   Arg7  const& a7,  Arg8  const& a8,  Arg9  const& a9) const;
    int testFunc10(Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
                   Arg7  const& a7,  Arg8  const& a8,  Arg9  const& a9,
                   Arg10 const& a10) const;
    int testFunc11(Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
                   Arg7  const& a7,  Arg8  const& a8,  Arg9  const& a9,
                   Arg10 const& a10, Arg11 const& a11) const;
    int testFunc12(Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
                   Arg7  const& a7,  Arg8  const& a8,  Arg9  const& a9,
                   Arg10 const& a10, Arg11 const& a11, Arg12 const& a12) const;
    int testFunc13(Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
                   Arg7  const& a7,  Arg8  const& a8,  Arg9  const& a9,
                   Arg10 const& a10, Arg11 const& a11, Arg12 const& a12,
                   Arg13 const& a13) const;
    int testFunc14(Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
                   Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
                   Arg7  const& a7,  Arg8  const& a8,  Arg9  const& a9,
                   Arg10 const& a10, Arg11 const& a11, Arg12 const& a12,
                   Arg13 const& a13, Arg14 const& a14) const;
        // Test methods invoking this test type with 0 up to 14 arguments.

    void setSlots();
        // Set slots with allocator values of internal data members (as opposed
        // to with allocator value of invocation arguments as in the operators
        // and 'testFunc*' functions above).

    // ACCESSORS
    void print() const;
        // Output the value of this object to the standard output.
};

// FREE OPERATORS
inline
bool operator==(bdef_Bind_TestTypeAlloc const& lhs,
                bdef_Bind_TestTypeAlloc const& rhs);

inline
bool operator!=(bdef_Bind_TestTypeAlloc const& lhs,
                bdef_Bind_TestTypeAlloc const& rhs);

                        // ==================================
                        // class bdef_Bind_TestFunctionsAlloc
                        // ==================================

struct bdef_Bind_TestFunctionsAlloc {
    // Global versions of 'bdef_Bind_TestTypeAlloc' member functions.

    // TYPES
    typedef bdef_Bind_TestArgAlloc<1>  Arg1;
    typedef bdef_Bind_TestArgAlloc<2>  Arg2;
    typedef bdef_Bind_TestArgAlloc<3>  Arg3;
    typedef bdef_Bind_TestArgAlloc<4>  Arg4;
    typedef bdef_Bind_TestArgAlloc<5>  Arg5;
    typedef bdef_Bind_TestArgAlloc<6>  Arg6;
    typedef bdef_Bind_TestArgAlloc<7>  Arg7;
    typedef bdef_Bind_TestArgAlloc<8>  Arg8;
    typedef bdef_Bind_TestArgAlloc<9>  Arg9;
    typedef bdef_Bind_TestArgAlloc<10> Arg10;
    typedef bdef_Bind_TestArgAlloc<11> Arg11;
    typedef bdef_Bind_TestArgAlloc<12> Arg12;
    typedef bdef_Bind_TestArgAlloc<13> Arg13;
    typedef bdef_Bind_TestArgAlloc<14> Arg14;

    // CLASS METHODS
    static
    int func0(bdef_Bind_TestTypeAlloc *o);
    static
    int func1(bdef_Bind_TestTypeAlloc *o,
        Arg1 const& a1);
    static
    int func2(bdef_Bind_TestTypeAlloc *o,
        Arg1 const& a1, Arg2 const& a2);
    static
    int func3(bdef_Bind_TestTypeAlloc *o,
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3);
    static
    int func4(bdef_Bind_TestTypeAlloc *o,
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4);
    static
    int func5(bdef_Bind_TestTypeAlloc *o,
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4,
        Arg5 const& a5);
    static
    int func6(bdef_Bind_TestTypeAlloc *o,
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4,
        Arg5 const& a5, Arg6 const& a6);
    static
    int func7(bdef_Bind_TestTypeAlloc *o,
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4,
        Arg5 const& a5, Arg6 const& a6, Arg7 const& a7);
    static
    int func8(bdef_Bind_TestTypeAlloc *o,
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4,
        Arg5 const& a5, Arg6 const& a6, Arg7 const& a7, Arg8 const& a8);
    static
    int func9(bdef_Bind_TestTypeAlloc *o,
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4,
        Arg5 const& a5, Arg6 const& a6, Arg7 const& a7, Arg8 const& a8,
        Arg9 const& a9);
    static
    int func10(bdef_Bind_TestTypeAlloc *o,
        Arg1 const& a1, Arg2  const& a2,  Arg3  const& a3,  Arg4  const& a4,
        Arg5 const& a5, Arg6  const& a6,  Arg7  const& a7,  Arg8  const& a8,
        Arg9 const& a9, Arg10 const& a10);
    static
    int func11(bdef_Bind_TestTypeAlloc *o,
        Arg1 const& a1, Arg2  const& a2,  Arg3  const& a3,  Arg4  const& a4,
        Arg5 const& a5, Arg6  const& a6,  Arg7  const& a7,  Arg8  const& a8,
        Arg9 const& a9, Arg10 const& a10, Arg11 const& a11);
    static
    int func12(bdef_Bind_TestTypeAlloc *o,
        Arg1 const& a1, Arg2  const& a2,  Arg3  const& a3,  Arg4  const& a4,
        Arg5 const& a5, Arg6  const& a6,  Arg7  const& a7,  Arg8  const& a8,
        Arg9 const& a9, Arg10 const& a10, Arg11 const& a11, Arg12 const& a12);
    static
    int func13(bdef_Bind_TestTypeAlloc *o,
        Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,  Arg4  const& a4,
        Arg5  const& a5,  Arg6  const& a6,  Arg7  const& a7,  Arg8  const& a8,
        Arg9  const& a9,  Arg10 const& a10, Arg11 const& a11, Arg12 const& a12,
        Arg13 const& a13);
    static
    int func14(bdef_Bind_TestTypeAlloc *o,
        Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,  Arg4  const& a4,
        Arg5  const& a5,  Arg6  const& a6,  Arg7  const& a7,  Arg8  const& a8,
        Arg9  const& a9,  Arg10 const& a10, Arg11 const& a11, Arg12 const& a12,
        Arg13 const& a13, Arg14 const& a14);
        // Invoke the corresponding method 'testFunc[0-14]' on the specified
        // 'object' with the specified arguments 'a[0-14]'.
};

// ============================================================================
//                    INLINE FUNCTION DEFINITIONS
// ============================================================================

                      // -----------------------------
                      // class bdef_Bind_TestSlotsBase
                      // -----------------------------

// PRIVATE CLASS DATA
template <typename VALUE>
VALUE
bdef_Bind_TestSlotsBase<VALUE>::d_slots[bdef_Bind_TestSlotsBase<VALUE>::
                                                                    NUM_SLOTS];

// CLASS METHODS
template <typename VALUE>
inline
VALUE bdef_Bind_TestSlotsBase<VALUE>::getSlot(int index)
{
    return d_slots[index];
}

template <typename VALUE>
void bdef_Bind_TestSlotsBase<VALUE>::resetSlots(VALUE value)
{
    for (int i = 0; i < NUM_SLOTS; ++i) {
        d_slots[i] = value;
    }
}

template <typename VALUE>
inline
void bdef_Bind_TestSlotsBase<VALUE>::setSlot(VALUE value, int index)
{
    d_slots[index] = value;
}

#ifdef BSLS_PLATFORM_CMP_MSVC
#pragma warning( push )
#pragma warning( disable : 4100 ) // Verify we still need separate test paths
                                  // for MSVC
#endif
template <typename VALUE>
bool bdef_Bind_TestSlotsBase<VALUE>::verifySlots(const VALUE *EXPECTED,
                                                 bool         verboseFlag)
{
    bool equalFlag = true;
#if !defined(BSLS_PLATFORM_CMP_MSVC)
    // Note: the calls to 'verifyAllocSlots' are disabled on Windows.
    // Their success depends on the "Return Value Optimization" (RVO)
    // which Windows does not seem to be applying.

    for (int i = 0; i < NUM_SLOTS; ++i) {
        if (EXPECTED[i] != getSlot(i)) {
            equalFlag = false;
            break;
        }
    }
#endif // !defined(BSLS_PLATFORM_CMP_MSVC)

    if (verboseFlag || !equalFlag) {
        bsl::printf("\tSlots:");
        for (int i = 0; i < NUM_SLOTS; ++i) {
            bsl::printf(" %d", getSlot(i));
        }
        bsl::printf("\n");
    }

    return equalFlag;
}
#ifdef BSLS_PLATFORM_CMP_MSVC
#pragma warning( pop )
#endif
                      // ------------------------------
                      // class bdef_Bind_TestArgNoAlloc
                      // ------------------------------

// CREATORS
template <int ID>
inline
bdef_Bind_TestArgNoAlloc<ID>::bdef_Bind_TestArgNoAlloc(int value)
: d_value(value)
{
}

// MANIPULATORS
template <int ID>
inline
bdef_Bind_TestArgNoAlloc<ID>&
bdef_Bind_TestArgNoAlloc<ID>::operator=(const bdef_Bind_TestArgNoAlloc &rhs)
{
    d_value = rhs.d_value;
    return *this;
}

// ACCESSORS
template <int ID>
inline
int bdef_Bind_TestArgNoAlloc<ID>::value() const
{
    return d_value;
}

// FREE OPERATORS
template <int ID>
bool operator==(bdef_Bind_TestArgNoAlloc<ID> const& lhs,
                bdef_Bind_TestArgNoAlloc<ID> const& rhs)
{
    return lhs.value() == rhs.value();
}

template <int ID>
bool operator!=(bdef_Bind_TestArgNoAlloc<ID> const& lhs,
                bdef_Bind_TestArgNoAlloc<ID> const& rhs)
{
    return !(lhs == rhs);
}

                      // -------------------------------
                      // class bdef_Bind_TestTypeNoAlloc
                      // -------------------------------

// CREATORS
inline
bdef_Bind_TestTypeNoAlloc::bdef_Bind_TestTypeNoAlloc(
        Arg1  a1,  Arg2  a2,  Arg3  a3,  Arg4  a4,  Arg5  a5,  Arg6  a6,
        Arg7  a7,  Arg8  a8,  Arg9  a9,  Arg10 a10, Arg11 a11, Arg12 a12,
        Arg13 a13, Arg14 a14)
: d_a1 (a1)
, d_a2 (a2)
, d_a3 (a3)
, d_a4 (a4)
, d_a5 (a5)
, d_a6 (a6)
, d_a7 (a7)
, d_a8 (a8)
, d_a9 (a9)
, d_a10(a10)
, d_a11(a11)
, d_a12(a12)
, d_a13(a13)
, d_a14(a14)
{
}

inline
bdef_Bind_TestTypeNoAlloc::bdef_Bind_TestTypeNoAlloc(
        const bdef_Bind_TestTypeNoAlloc& original)
: d_a1 (original.d_a1)
, d_a2 (original.d_a2)
, d_a3 (original.d_a3)
, d_a4 (original.d_a4)
, d_a5 (original.d_a5)
, d_a6 (original.d_a6)
, d_a7 (original.d_a7)
, d_a8 (original.d_a8)
, d_a9 (original.d_a9)
, d_a10(original.d_a10)
, d_a11(original.d_a11)
, d_a12(original.d_a12)
, d_a13(original.d_a13)
, d_a14(original.d_a14)
{
}

// MANIPULATORS
inline
bdef_Bind_TestTypeNoAlloc&
bdef_Bind_TestTypeNoAlloc::operator=(const bdef_Bind_TestTypeNoAlloc& rhs)
{
    d_a1  = rhs.d_a1;
    d_a2  = rhs.d_a2;
    d_a3  = rhs.d_a3;
    d_a4  = rhs.d_a4;
    d_a5  = rhs.d_a5;
    d_a6  = rhs.d_a6;
    d_a7  = rhs.d_a7;
    d_a8  = rhs.d_a8;
    d_a9  = rhs.d_a9;
    d_a10 = rhs.d_a10;
    d_a11 = rhs.d_a11;
    d_a12 = rhs.d_a12;
    d_a13 = rhs.d_a13;
    d_a14 = rhs.d_a14;
    return *this;
}

// ACCESSORS
inline
int bdef_Bind_TestTypeNoAlloc::operator()() const
{
    return testFunc0();
}

inline
int bdef_Bind_TestTypeNoAlloc::operator()(Arg1 const& a1) const
{
    return testFunc1(a1);
}

inline
int bdef_Bind_TestTypeNoAlloc::operator()(Arg1 const& a1, Arg2 const& a2) const
{
    return testFunc2(a1, a2);
}

inline
int bdef_Bind_TestTypeNoAlloc::operator()(Arg1 const& a1, Arg2 const& a2,
        Arg3 const& a3) const
{
    return testFunc3(a1, a2, a3);
}

inline
int bdef_Bind_TestTypeNoAlloc::operator()(Arg1 const& a1, Arg2 const& a2,
        Arg3 const& a3, Arg4 const& a4) const
{
    return testFunc4(a1, a2, a3, a4);
}

inline
int bdef_Bind_TestTypeNoAlloc::operator()(Arg1 const& a1, Arg2 const& a2,
        Arg3 const& a3, Arg4 const& a4, Arg5 const& a5) const
{
    return testFunc5(a1, a2, a3, a4, a5);
}

inline
int bdef_Bind_TestTypeNoAlloc::operator()(Arg1 const& a1, Arg2 const& a2,
        Arg3 const& a3, Arg4 const& a4, Arg5 const& a5, Arg6 const& a6) const
{
    return testFunc6(a1, a2, a3, a4, a5, a6);
}

inline
int bdef_Bind_TestTypeNoAlloc::operator()(Arg1 const& a1, Arg2 const& a2,
        Arg3 const& a3, Arg4 const& a4, Arg5 const& a5, Arg6 const& a6,
        Arg7 const& a7) const
{
    return testFunc7(a1, a2, a3, a4, a5, a6, a7);
}

inline
int bdef_Bind_TestTypeNoAlloc::operator()(Arg1 const& a1, Arg2 const& a2,
        Arg3 const& a3, Arg4 const& a4, Arg5 const& a5, Arg6 const& a6,
        Arg7 const& a7, Arg8 const& a8) const
{
    return testFunc8(a1, a2, a3, a4, a5, a6, a7, a8);
}

inline
int bdef_Bind_TestTypeNoAlloc::operator()(Arg1 const& a1, Arg2 const& a2,
        Arg3 const& a3, Arg4 const& a4, Arg5 const& a5, Arg6 const& a6,
        Arg7 const& a7, Arg8 const& a8, Arg9 const& a9) const
{
    return testFunc9(a1, a2, a3, a4, a5, a6, a7, a8, a9);
}

inline
int bdef_Bind_TestTypeNoAlloc::operator()(Arg1 const& a1, Arg2  const& a2,
        Arg3 const& a3, Arg4 const& a4, Arg5 const& a5, Arg6  const& a6,
        Arg7 const& a7, Arg8 const& a8, Arg9 const& a9, Arg10 const& a10) const
{
    return testFunc10(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
}

inline
int bdef_Bind_TestTypeNoAlloc::operator()(
        Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
        Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
        Arg7  const& a7,  Arg8  const& a8,  Arg9  const& a9,
        Arg10 const& a10, Arg11 const& a11) const
{
    return testFunc11(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11);
}

inline
int bdef_Bind_TestTypeNoAlloc::operator()(
        Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
        Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
        Arg7  const& a7,  Arg8  const& a8,  Arg9  const& a9,
        Arg10 const& a10, Arg11 const& a11, Arg12 const& a12) const
{
    return testFunc12(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12);
}

inline
int bdef_Bind_TestTypeNoAlloc::operator()(
        Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
        Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
        Arg7  const& a7,  Arg8  const& a8,  Arg9  const& a9,
        Arg10 const& a10, Arg11 const& a11, Arg12 const& a12,
        Arg13 const& a13) const
{
    return testFunc13(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12,
                      a13);
}

inline
int bdef_Bind_TestTypeNoAlloc::operator()(
        Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
        Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
        Arg7  const& a7,  Arg8  const& a8,  Arg9  const& a9,
        Arg10 const& a10, Arg11 const& a11, Arg12 const& a12,
        Arg13 const& a13, Arg14 const& a14) const
{
    return testFunc14(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12,
                      a13, a14);
}

inline
int bdef_Bind_TestTypeNoAlloc::testFunc0() const
{
    return 0;
}

inline
int bdef_Bind_TestTypeNoAlloc::testFunc1(Arg1 const& a1) const
{
    d_a1 = a1; bdef_Bind_TestSlotsNoAlloc::setSlot(a1.value(), 1);

    return 1;
}

inline
int bdef_Bind_TestTypeNoAlloc::testFunc2(Arg1 const& a1, Arg2 const& a2) const
{
    d_a1 = a1; bdef_Bind_TestSlotsNoAlloc::setSlot(a1.value(), 1);
    d_a2 = a2; bdef_Bind_TestSlotsNoAlloc::setSlot(a2.value(), 2);

    return 2;
}

inline
int bdef_Bind_TestTypeNoAlloc::testFunc3(
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3) const
{
    d_a1 = a1; bdef_Bind_TestSlotsNoAlloc::setSlot(a1.value(), 1);
    d_a2 = a2; bdef_Bind_TestSlotsNoAlloc::setSlot(a2.value(), 2);
    d_a3 = a3; bdef_Bind_TestSlotsNoAlloc::setSlot(a3.value(), 3);

    return 3;
}

inline
int bdef_Bind_TestTypeNoAlloc::testFunc4(
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4) const
{
    d_a1 = a1; bdef_Bind_TestSlotsNoAlloc::setSlot(a1.value(), 1);
    d_a2 = a2; bdef_Bind_TestSlotsNoAlloc::setSlot(a2.value(), 2);
    d_a3 = a3; bdef_Bind_TestSlotsNoAlloc::setSlot(a3.value(), 3);
    d_a4 = a4; bdef_Bind_TestSlotsNoAlloc::setSlot(a4.value(), 4);

    return 4;
}
inline
int bdef_Bind_TestTypeNoAlloc::testFunc5(
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4,
        Arg5 const& a5) const
{
    d_a1 = a1; bdef_Bind_TestSlotsNoAlloc::setSlot(a1.value(), 1);
    d_a2 = a2; bdef_Bind_TestSlotsNoAlloc::setSlot(a2.value(), 2);
    d_a3 = a3; bdef_Bind_TestSlotsNoAlloc::setSlot(a3.value(), 3);
    d_a4 = a4; bdef_Bind_TestSlotsNoAlloc::setSlot(a4.value(), 4);
    d_a5 = a5; bdef_Bind_TestSlotsNoAlloc::setSlot(a5.value(), 5);

    return 5;
}

inline
int bdef_Bind_TestTypeNoAlloc::testFunc6(
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4,
        Arg5 const& a5, Arg6 const& a6) const
{
    d_a1 = a1; bdef_Bind_TestSlotsNoAlloc::setSlot(a1.value(), 1);
    d_a2 = a2; bdef_Bind_TestSlotsNoAlloc::setSlot(a2.value(), 2);
    d_a3 = a3; bdef_Bind_TestSlotsNoAlloc::setSlot(a3.value(), 3);
    d_a4 = a4; bdef_Bind_TestSlotsNoAlloc::setSlot(a4.value(), 4);
    d_a5 = a5; bdef_Bind_TestSlotsNoAlloc::setSlot(a5.value(), 5);
    d_a6 = a6; bdef_Bind_TestSlotsNoAlloc::setSlot(a6.value(), 6);

    return 6;
}

inline
int bdef_Bind_TestTypeNoAlloc::testFunc7(
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4,
        Arg5 const& a5, Arg6 const& a6, Arg7 const& a7) const
{
    d_a1 = a1; bdef_Bind_TestSlotsNoAlloc::setSlot(a1.value(), 1);
    d_a2 = a2; bdef_Bind_TestSlotsNoAlloc::setSlot(a2.value(), 2);
    d_a3 = a3; bdef_Bind_TestSlotsNoAlloc::setSlot(a3.value(), 3);
    d_a4 = a4; bdef_Bind_TestSlotsNoAlloc::setSlot(a4.value(), 4);
    d_a5 = a5; bdef_Bind_TestSlotsNoAlloc::setSlot(a5.value(), 5);
    d_a6 = a6; bdef_Bind_TestSlotsNoAlloc::setSlot(a6.value(), 6);
    d_a7 = a7; bdef_Bind_TestSlotsNoAlloc::setSlot(a7.value(), 7);

    return 7;
}

inline
int bdef_Bind_TestTypeNoAlloc::testFunc8(
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4,
        Arg5 const& a5, Arg6 const& a6, Arg7 const& a7, Arg8 const& a8) const
{
    d_a1 = a1; bdef_Bind_TestSlotsNoAlloc::setSlot(a1.value(), 1);
    d_a2 = a2; bdef_Bind_TestSlotsNoAlloc::setSlot(a2.value(), 2);
    d_a3 = a3; bdef_Bind_TestSlotsNoAlloc::setSlot(a3.value(), 3);
    d_a4 = a4; bdef_Bind_TestSlotsNoAlloc::setSlot(a4.value(), 4);
    d_a5 = a5; bdef_Bind_TestSlotsNoAlloc::setSlot(a5.value(), 5);
    d_a6 = a6; bdef_Bind_TestSlotsNoAlloc::setSlot(a6.value(), 6);
    d_a7 = a7; bdef_Bind_TestSlotsNoAlloc::setSlot(a7.value(), 7);
    d_a8 = a8; bdef_Bind_TestSlotsNoAlloc::setSlot(a8.value(), 8);

    return 8;
}

inline
int bdef_Bind_TestTypeNoAlloc::testFunc9(
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4,
        Arg5 const& a5, Arg6 const& a6, Arg7 const& a7, Arg8 const& a8,
        Arg9 const& a9) const
{
    d_a1 = a1; bdef_Bind_TestSlotsNoAlloc::setSlot(a1.value(), 1);
    d_a2 = a2; bdef_Bind_TestSlotsNoAlloc::setSlot(a2.value(), 2);
    d_a3 = a3; bdef_Bind_TestSlotsNoAlloc::setSlot(a3.value(), 3);
    d_a4 = a4; bdef_Bind_TestSlotsNoAlloc::setSlot(a4.value(), 4);
    d_a5 = a5; bdef_Bind_TestSlotsNoAlloc::setSlot(a5.value(), 5);
    d_a6 = a6; bdef_Bind_TestSlotsNoAlloc::setSlot(a6.value(), 6);
    d_a7 = a7; bdef_Bind_TestSlotsNoAlloc::setSlot(a7.value(), 7);
    d_a8 = a8; bdef_Bind_TestSlotsNoAlloc::setSlot(a8.value(), 8);
    d_a9 = a9; bdef_Bind_TestSlotsNoAlloc::setSlot(a9.value(), 9);

    return 9;
}

inline
int bdef_Bind_TestTypeNoAlloc::testFunc10(
        Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,  Arg4  const& a4,
        Arg5  const& a5,  Arg6  const& a6,  Arg7  const& a7,  Arg8  const& a8,
        Arg9  const& a9,  Arg10 const& a10) const
{
    d_a1  = a1;  bdef_Bind_TestSlotsNoAlloc::setSlot(a1.value(),  1);
    d_a2  = a2;  bdef_Bind_TestSlotsNoAlloc::setSlot(a2.value(),  2);
    d_a3  = a3;  bdef_Bind_TestSlotsNoAlloc::setSlot(a3.value(),  3);
    d_a4  = a4;  bdef_Bind_TestSlotsNoAlloc::setSlot(a4.value(),  4);
    d_a5  = a5;  bdef_Bind_TestSlotsNoAlloc::setSlot(a5.value(),  5);
    d_a6  = a6;  bdef_Bind_TestSlotsNoAlloc::setSlot(a6.value(),  6);
    d_a7  = a7;  bdef_Bind_TestSlotsNoAlloc::setSlot(a7.value(),  7);
    d_a8  = a8;  bdef_Bind_TestSlotsNoAlloc::setSlot(a8.value(),  8);
    d_a9  = a9;  bdef_Bind_TestSlotsNoAlloc::setSlot(a9.value(),  9);
    d_a10 = a10; bdef_Bind_TestSlotsNoAlloc::setSlot(a10.value(), 10);

    return 10;
}

inline
int bdef_Bind_TestTypeNoAlloc::testFunc11(
        Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,  Arg4  const& a4,
        Arg5  const& a5,  Arg6  const& a6,  Arg7  const& a7,  Arg8  const& a8,
        Arg9  const& a9,  Arg10 const& a10, Arg11 const& a11) const
{
    d_a1  = a1;  bdef_Bind_TestSlotsNoAlloc::setSlot(a1.value(),  1);
    d_a2  = a2;  bdef_Bind_TestSlotsNoAlloc::setSlot(a2.value(),  2);
    d_a3  = a3;  bdef_Bind_TestSlotsNoAlloc::setSlot(a3.value(),  3);
    d_a4  = a4;  bdef_Bind_TestSlotsNoAlloc::setSlot(a4.value(),  4);
    d_a5  = a5;  bdef_Bind_TestSlotsNoAlloc::setSlot(a5.value(),  5);
    d_a6  = a6;  bdef_Bind_TestSlotsNoAlloc::setSlot(a6.value(),  6);
    d_a7  = a7;  bdef_Bind_TestSlotsNoAlloc::setSlot(a7.value(),  7);
    d_a8  = a8;  bdef_Bind_TestSlotsNoAlloc::setSlot(a8.value(),  8);
    d_a9  = a9;  bdef_Bind_TestSlotsNoAlloc::setSlot(a9.value(),  9);
    d_a10 = a10; bdef_Bind_TestSlotsNoAlloc::setSlot(a10.value(), 10);
    d_a11 = a11; bdef_Bind_TestSlotsNoAlloc::setSlot(a11.value(), 11);

    return 11;
}

inline
int bdef_Bind_TestTypeNoAlloc::testFunc12(
        Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,  Arg4  const& a4,
        Arg5  const& a5,  Arg6  const& a6,  Arg7  const& a7,  Arg8  const& a8,
        Arg9  const& a9,  Arg10 const& a10, Arg11 const& a11,
        Arg12 const& a12) const
{
    d_a1  = a1;  bdef_Bind_TestSlotsNoAlloc::setSlot(a1.value(),  1);
    d_a2  = a2;  bdef_Bind_TestSlotsNoAlloc::setSlot(a2.value(),  2);
    d_a3  = a3;  bdef_Bind_TestSlotsNoAlloc::setSlot(a3.value(),  3);
    d_a4  = a4;  bdef_Bind_TestSlotsNoAlloc::setSlot(a4.value(),  4);
    d_a5  = a5;  bdef_Bind_TestSlotsNoAlloc::setSlot(a5.value(),  5);
    d_a6  = a6;  bdef_Bind_TestSlotsNoAlloc::setSlot(a6.value(),  6);
    d_a7  = a7;  bdef_Bind_TestSlotsNoAlloc::setSlot(a7.value(),  7);
    d_a8  = a8;  bdef_Bind_TestSlotsNoAlloc::setSlot(a8.value(),  8);
    d_a9  = a9;  bdef_Bind_TestSlotsNoAlloc::setSlot(a9.value(),  9);
    d_a10 = a10; bdef_Bind_TestSlotsNoAlloc::setSlot(a10.value(), 10);
    d_a11 = a11; bdef_Bind_TestSlotsNoAlloc::setSlot(a11.value(), 11);
    d_a12 = a12; bdef_Bind_TestSlotsNoAlloc::setSlot(a12.value(), 12);

    return 12;
}

inline
int bdef_Bind_TestTypeNoAlloc::testFunc13(
        Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,  Arg4  const& a4,
        Arg5  const& a5,  Arg6  const& a6,  Arg7  const& a7,  Arg8  const& a8,
        Arg9  const& a9,  Arg10 const& a10, Arg11 const& a11, Arg12 const& a12,
        Arg13 const& a13) const
{
    d_a1  = a1;  bdef_Bind_TestSlotsNoAlloc::setSlot(a1.value(),  1);
    d_a2  = a2;  bdef_Bind_TestSlotsNoAlloc::setSlot(a2.value(),  2);
    d_a3  = a3;  bdef_Bind_TestSlotsNoAlloc::setSlot(a3.value(),  3);
    d_a4  = a4;  bdef_Bind_TestSlotsNoAlloc::setSlot(a4.value(),  4);
    d_a5  = a5;  bdef_Bind_TestSlotsNoAlloc::setSlot(a5.value(),  5);
    d_a6  = a6;  bdef_Bind_TestSlotsNoAlloc::setSlot(a6.value(),  6);
    d_a7  = a7;  bdef_Bind_TestSlotsNoAlloc::setSlot(a7.value(),  7);
    d_a8  = a8;  bdef_Bind_TestSlotsNoAlloc::setSlot(a8.value(),  8);
    d_a9  = a9;  bdef_Bind_TestSlotsNoAlloc::setSlot(a9.value(),  9);
    d_a10 = a10; bdef_Bind_TestSlotsNoAlloc::setSlot(a10.value(), 10);
    d_a11 = a11; bdef_Bind_TestSlotsNoAlloc::setSlot(a11.value(), 11);
    d_a12 = a12; bdef_Bind_TestSlotsNoAlloc::setSlot(a12.value(), 12);
    d_a13 = a13; bdef_Bind_TestSlotsNoAlloc::setSlot(a13.value(), 13);

    return 13;
}

inline
int bdef_Bind_TestTypeNoAlloc::testFunc14(
        Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,  Arg4  const& a4,
        Arg5  const& a5,  Arg6  const& a6,  Arg7  const& a7,  Arg8  const& a8,
        Arg9  const& a9,  Arg10 const& a10, Arg11 const& a11, Arg12 const& a12,
        Arg13 const& a13, Arg14 const& a14) const
{
    d_a1  = a1;  bdef_Bind_TestSlotsNoAlloc::setSlot(a1.value(),  1);
    d_a2  = a2;  bdef_Bind_TestSlotsNoAlloc::setSlot(a2.value(),  2);
    d_a3  = a3;  bdef_Bind_TestSlotsNoAlloc::setSlot(a3.value(),  3);
    d_a4  = a4;  bdef_Bind_TestSlotsNoAlloc::setSlot(a4.value(),  4);
    d_a5  = a5;  bdef_Bind_TestSlotsNoAlloc::setSlot(a5.value(),  5);
    d_a6  = a6;  bdef_Bind_TestSlotsNoAlloc::setSlot(a6.value(),  6);
    d_a7  = a7;  bdef_Bind_TestSlotsNoAlloc::setSlot(a7.value(),  7);
    d_a8  = a8;  bdef_Bind_TestSlotsNoAlloc::setSlot(a8.value(),  8);
    d_a9  = a9;  bdef_Bind_TestSlotsNoAlloc::setSlot(a9.value(),  9);
    d_a10 = a10; bdef_Bind_TestSlotsNoAlloc::setSlot(a10.value(), 10);
    d_a11 = a11; bdef_Bind_TestSlotsNoAlloc::setSlot(a11.value(), 11);
    d_a12 = a12; bdef_Bind_TestSlotsNoAlloc::setSlot(a12.value(), 12);
    d_a13 = a13; bdef_Bind_TestSlotsNoAlloc::setSlot(a13.value(), 13);
    d_a14 = a14; bdef_Bind_TestSlotsNoAlloc::setSlot(a14.value(), 14);

    return 14;
}

inline
void bdef_Bind_TestTypeNoAlloc::print() const
{
    bsl::printf("{ %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d }\n",
                d_a1.value(),  d_a2.value(),  d_a3.value(),  d_a4.value(),
                d_a5.value(),  d_a6.value(),  d_a7.value(),  d_a8.value(),
                d_a9.value(),  d_a10.value(), d_a11.value(), d_a12.value(),
                d_a13.value(), d_a14.value());
}

// FREE OPERATORS
inline
bool operator==(const bdef_Bind_TestTypeNoAlloc& lhs,
                const bdef_Bind_TestTypeNoAlloc& rhs)
{
    return lhs.d_a1.value()  == rhs.d_a1.value()
        && lhs.d_a1.value()  == rhs.d_a1.value()
        && lhs.d_a2.value()  == rhs.d_a2.value()
        && lhs.d_a3.value()  == rhs.d_a3.value()
        && lhs.d_a4.value()  == rhs.d_a4.value()
        && lhs.d_a5.value()  == rhs.d_a5.value()
        && lhs.d_a6.value()  == rhs.d_a6.value()
        && lhs.d_a7.value()  == rhs.d_a7.value()
        && lhs.d_a8.value()  == rhs.d_a8.value()
        && lhs.d_a9.value()  == rhs.d_a9.value()
        && lhs.d_a10.value() == rhs.d_a10.value()
        && lhs.d_a11.value() == rhs.d_a11.value()
        && lhs.d_a12.value() == rhs.d_a12.value()
        && lhs.d_a13.value() == rhs.d_a13.value()
        && lhs.d_a14.value() == rhs.d_a14.value();
}

inline
bool operator!=(const bdef_Bind_TestTypeNoAlloc& lhs,
                const bdef_Bind_TestTypeNoAlloc& rhs)
{
    return !(lhs == rhs);
}

                         // ------------------------------------
                         // class bdef_Bind_TestFunctionsNoAlloc
                         // ------------------------------------

// CLASS METHODS
inline
int bdef_Bind_TestFunctionsNoAlloc::func0(bdef_Bind_TestTypeNoAlloc *object)
{
    return object->testFunc0();
}

inline
int bdef_Bind_TestFunctionsNoAlloc::func1(bdef_Bind_TestTypeNoAlloc *object,
        Arg1 const& a1)
{
    return object->testFunc1(a1);
}

inline
int bdef_Bind_TestFunctionsNoAlloc::func2(bdef_Bind_TestTypeNoAlloc *object,
        Arg1 const& a1, Arg2 const& a2)
{
    return object->testFunc2(a1, a2);
}

inline
int bdef_Bind_TestFunctionsNoAlloc::func3(bdef_Bind_TestTypeNoAlloc *object,
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3)
{
    return object->testFunc3(a1, a2, a3);
}

inline
int bdef_Bind_TestFunctionsNoAlloc::func4(bdef_Bind_TestTypeNoAlloc *object,
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4)
{
    return object->testFunc4(a1, a2, a3, a4);
}

inline
int bdef_Bind_TestFunctionsNoAlloc::func5(bdef_Bind_TestTypeNoAlloc *object,
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4,
        Arg5 const& a5)
{
    return object->testFunc5(a1, a2, a3, a4, a5);
}

inline
int bdef_Bind_TestFunctionsNoAlloc::func6(bdef_Bind_TestTypeNoAlloc *object,
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4,
        Arg5 const& a5, Arg6 const& a6)
{
    return object->testFunc6(a1, a2, a3, a4, a5, a6);
}

inline
int bdef_Bind_TestFunctionsNoAlloc::func7(bdef_Bind_TestTypeNoAlloc *object,
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4,
        Arg5 const& a5, Arg6 const& a6, Arg7 const& a7)
{
    return object->testFunc7(a1, a2, a3, a4, a5, a6, a7);
}

inline
int bdef_Bind_TestFunctionsNoAlloc::func8(bdef_Bind_TestTypeNoAlloc *object,
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4,
        Arg5 const& a5, Arg6 const& a6, Arg7 const& a7, Arg8 const& a8)
{
    return object->testFunc8(a1, a2, a3, a4, a5, a6, a7, a8);
}

inline
int bdef_Bind_TestFunctionsNoAlloc::func9(bdef_Bind_TestTypeNoAlloc *object,
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4,
        Arg5 const& a5, Arg6 const& a6, Arg7 const& a7, Arg8 const& a8,
        Arg9 const& a9)
{
    return object->testFunc9(a1, a2, a3, a4, a5, a6, a7, a8, a9);
}

inline
int bdef_Bind_TestFunctionsNoAlloc::func10(bdef_Bind_TestTypeNoAlloc *object,
        Arg1  const& a1,  Arg2  const& a2, Arg3  const& a3,  Arg4  const& a4,
        Arg5  const& a5,  Arg6  const& a6, Arg7  const& a7,  Arg8  const& a8,
        Arg9  const& a9,  Arg10 const& a10)
{
    return object->testFunc10(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
}

inline
int bdef_Bind_TestFunctionsNoAlloc::func11(bdef_Bind_TestTypeNoAlloc *object,
        Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,  Arg4  const& a4,
        Arg5  const& a5,  Arg6  const& a6,  Arg7  const& a7,  Arg8  const& a8,
        Arg9  const& a9,  Arg10 const& a10, Arg11 const& a11)
{
    return object->testFunc11(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11);
}

inline
int bdef_Bind_TestFunctionsNoAlloc::func12(bdef_Bind_TestTypeNoAlloc *object,
        Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,  Arg4  const& a4,
        Arg5  const& a5,  Arg6  const& a6,  Arg7  const& a7,  Arg8  const& a8,
        Arg9  const& a9,  Arg10 const& a10, Arg11 const& a11, Arg12 const& a12)
{
    return object->testFunc12(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11,
                              a12);
}

inline
int bdef_Bind_TestFunctionsNoAlloc::func13(bdef_Bind_TestTypeNoAlloc *object,
        Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,  Arg4  const& a4,
        Arg5  const& a5,  Arg6  const& a6,  Arg7  const& a7,  Arg8  const& a8,
        Arg9  const& a9,  Arg10 const& a10, Arg11 const& a11, Arg12 const& a12,
        Arg13 const& a13)
{
    return object->testFunc13(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11,
                              a12, a13);
}

inline
int bdef_Bind_TestFunctionsNoAlloc::func14(bdef_Bind_TestTypeNoAlloc *object,
        Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,  Arg4  const& a4,
        Arg5  const& a5,  Arg6  const& a6,  Arg7  const& a7,  Arg8  const& a8,
        Arg9  const& a9,  Arg10 const& a10, Arg11 const& a11, Arg12 const& a12,
        Arg13 const& a13, Arg14 const& a14)
{
    return object->testFunc14(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11,
                              a12, a13, a14);
}

                      // ----------------------------------
                      // class bdef_Bind_TestSlotsAllocBase
                      // ----------------------------------

// CLASS DATA
template <typename AllocPtr>
AllocPtr bdef_Bind_TestSlotsAllocBase<AllocPtr>::d_Z0 = 0;

template <typename AllocPtr>
AllocPtr bdef_Bind_TestSlotsAllocBase<AllocPtr>::d_Z1 = 0;

template <typename AllocPtr>
AllocPtr bdef_Bind_TestSlotsAllocBase<AllocPtr>::d_Z2 = 0;

// CLASS METHODS
template <typename AllocPtr>
inline
void bdef_Bind_TestSlotsAllocBase<AllocPtr>::setZ0(AllocPtr Z0)
{
    d_Z0 = Z0;
}

template <typename AllocPtr>
inline
void bdef_Bind_TestSlotsAllocBase<AllocPtr>::setZ1(AllocPtr Z1)
{
    d_Z1 = Z1;
}

template <typename AllocPtr>
inline
void bdef_Bind_TestSlotsAllocBase<AllocPtr>::setZ2(AllocPtr Z2)
{
    d_Z2 = Z2;
}

template <typename AllocPtr>
inline
AllocPtr bdef_Bind_TestSlotsAllocBase<AllocPtr>::getZ0()
{
    return d_Z0;
}

template <typename AllocPtr>
inline
AllocPtr bdef_Bind_TestSlotsAllocBase<AllocPtr>::getZ1()
{
    return d_Z1;
}

template <typename AllocPtr>
inline
AllocPtr bdef_Bind_TestSlotsAllocBase<AllocPtr>::getZ2()
{
    return d_Z2;
}

                      // ----------------------------
                      // class bdef_Bind_TestArgAlloc
                      // ----------------------------

// CREATORS
#if defined(BSLS_PLATFORM_CMP_IBM) && BSLS_PLATFORM_CMP_VER_MAJOR >= 0x0900
template <int ID>
bdef_Bind_TestArgAlloc<ID>::bdef_Bind_TestArgAlloc(
        int              value)
: d_allocator_p(bslma_Default::allocator(0))
, d_value(new (*d_allocator_p) int(value))
{
}
#endif

template <int ID>
bdef_Bind_TestArgAlloc<ID>::bdef_Bind_TestArgAlloc(
        int              value,
        bslma_Allocator *allocator)
: d_allocator_p(bslma_Default::allocator(allocator))
, d_value(new (*d_allocator_p) int(value))
{
}

#if defined(BSLS_PLATFORM_CMP_IBM) && BSLS_PLATFORM_CMP_VER_MAJOR >= 0x0900
template <int ID>
bdef_Bind_TestArgAlloc<ID>::bdef_Bind_TestArgAlloc(
        const bdef_Bind_TestArgAlloc&  original)
: d_allocator_p(bslma_Default::allocator(0))
, d_value(new (*d_allocator_p) int(original.value()))
{
}
#endif

template <int ID>
bdef_Bind_TestArgAlloc<ID>::bdef_Bind_TestArgAlloc(
        const bdef_Bind_TestArgAlloc&  original,
        bslma_Allocator               *allocator)
: d_allocator_p(bslma_Default::allocator(allocator))
, d_value(new (*d_allocator_p) int(original.value()))
{
}

template <int ID>
bdef_Bind_TestArgAlloc<ID>::~bdef_Bind_TestArgAlloc()
{
    d_allocator_p->deleteObjectRaw(d_value);
}

// MANIPULATORS
template <int ID>
bdef_Bind_TestArgAlloc<ID>&
bdef_Bind_TestArgAlloc<ID>::operator=(const bdef_Bind_TestArgAlloc& rhs)
{
    if (this != &rhs) {
        d_allocator_p->deleteObjectRaw(d_value);
        d_value = new (*d_allocator_p) int(rhs.value());
    }

    return *this;
}

// ACCESSORS
template <int ID>
const bslma_Allocator *bdef_Bind_TestArgAlloc<ID>::allocator() const
{
    return d_allocator_p;
}

template <int ID>
int bdef_Bind_TestArgAlloc<ID>::value() const
{
    return *d_value;
}

// FREE OPERATORS
template <int ID>
inline
bool operator==(const bdef_Bind_TestArgAlloc<ID>& lhs,
                const bdef_Bind_TestArgAlloc<ID>& rhs)
{
    return lhs.value() == rhs.value();
}

template <int ID>
inline
bool operator!=(const bdef_Bind_TestArgAlloc<ID>& lhs,
                const bdef_Bind_TestArgAlloc<ID>& rhs)
{
    return !(lhs == rhs);
}

                      // -----------------------------
                      // class bdef_Bind_TestTypeAlloc
                      // -----------------------------

// CREATORS
inline
bdef_Bind_TestTypeAlloc::bdef_Bind_TestTypeAlloc(bslma_Allocator *allocator,
        Arg1  a1,  Arg2  a2,  Arg3  a3,  Arg4  a4,  Arg5  a5,  Arg6  a6,
        Arg7  a7,  Arg8  a8,  Arg9  a9,  Arg10 a10, Arg11 a11, Arg12 a12,
        Arg13 a13, Arg14 a14)
: d_a1 (a1,  allocator)
, d_a2 (a2,  allocator)
, d_a3 (a3,  allocator)
, d_a4 (a4,  allocator)
, d_a5 (a5,  allocator)
, d_a6 (a6,  allocator)
, d_a7 (a7,  allocator)
, d_a8 (a8,  allocator)
, d_a9 (a9,  allocator)
, d_a10(a10, allocator)
, d_a11(a11, allocator)
, d_a12(a12, allocator)
, d_a13(a13, allocator)
, d_a14(a14, allocator)
{
}

inline
bdef_Bind_TestTypeAlloc::bdef_Bind_TestTypeAlloc(
        Arg1  a1,  Arg2  a2,  Arg3  a3,  Arg4  a4,  Arg5  a5,  Arg6  a6,
        Arg7  a7,  Arg8  a8,  Arg9  a9,  Arg10 a10, Arg11 a11, Arg12 a12,
        Arg13 a13, Arg14 a14, bslma_Allocator *allocator)
: d_a1 (a1,  allocator)
, d_a2 (a2,  allocator)
, d_a3 (a3,  allocator)
, d_a4 (a4,  allocator)
, d_a5 (a5,  allocator)
, d_a6 (a6,  allocator)
, d_a7 (a7,  allocator)
, d_a8 (a8,  allocator)
, d_a9 (a9,  allocator)
, d_a10(a10, allocator)
, d_a11(a11, allocator)
, d_a12(a12, allocator)
, d_a13(a13, allocator)
, d_a14(a14, allocator)
{
}

inline
bdef_Bind_TestTypeAlloc::bdef_Bind_TestTypeAlloc(
        const bdef_Bind_TestTypeAlloc&  original,
        bslma_Allocator                *allocator)
: d_a1 (original.d_a1,  allocator)
, d_a2 (original.d_a2,  allocator)
, d_a3 (original.d_a3,  allocator)
, d_a4 (original.d_a4,  allocator)
, d_a5 (original.d_a5,  allocator)
, d_a6 (original.d_a6,  allocator)
, d_a7 (original.d_a7,  allocator)
, d_a8 (original.d_a8,  allocator)
, d_a9 (original.d_a9,  allocator)
, d_a10(original.d_a10, allocator)
, d_a11(original.d_a11, allocator)
, d_a12(original.d_a12, allocator)
, d_a13(original.d_a13, allocator)
, d_a14(original.d_a14, allocator)
{
}

// MANIPULATORS
inline
bdef_Bind_TestTypeAlloc&
bdef_Bind_TestTypeAlloc::operator=(const bdef_Bind_TestTypeAlloc& rhs)
{
    d_a1  = rhs.d_a1;
    d_a2  = rhs.d_a2;
    d_a3  = rhs.d_a3;
    d_a4  = rhs.d_a4;
    d_a5  = rhs.d_a5;
    d_a6  = rhs.d_a6;
    d_a7  = rhs.d_a7;
    d_a8  = rhs.d_a8;
    d_a9  = rhs.d_a9;
    d_a10 = rhs.d_a10;
    d_a11 = rhs.d_a11;
    d_a12 = rhs.d_a12;
    d_a13 = rhs.d_a13;
    d_a14 = rhs.d_a14;
    return *this;
}

// ACCESSORS
inline
int bdef_Bind_TestTypeAlloc::operator()() const
{
    return testFunc0();
}

inline
int bdef_Bind_TestTypeAlloc::operator()(Arg1 const& a1) const
{
    return testFunc1(a1);
}

inline
int bdef_Bind_TestTypeAlloc::operator()(Arg1 const& a1, Arg2 const& a2) const
{
    return testFunc2(a1, a2);
}

inline
int bdef_Bind_TestTypeAlloc::operator()(Arg1 const& a1, Arg2 const& a2,
        Arg3 const& a3) const
{
    return testFunc3(a1, a2, a3);
}

inline
int bdef_Bind_TestTypeAlloc::operator()(Arg1 const& a1, Arg2 const& a2,
        Arg3 const& a3, Arg4 const& a4) const
{
    return testFunc4(a1, a2, a3, a4);
}

inline
int bdef_Bind_TestTypeAlloc::operator()(Arg1 const& a1, Arg2 const& a2,
        Arg3 const& a3, Arg4 const& a4, Arg5 const& a5) const
{
    return testFunc5(a1, a2, a3, a4, a5);
}

inline
int bdef_Bind_TestTypeAlloc::operator()(Arg1 const& a1, Arg2 const& a2,
        Arg3 const& a3, Arg4 const& a4, Arg5 const& a5, Arg6 const& a6) const
{
    return testFunc6(a1, a2, a3, a4, a5, a6);
}

inline
int bdef_Bind_TestTypeAlloc::operator()(Arg1 const& a1, Arg2 const& a2,
        Arg3 const& a3, Arg4 const& a4, Arg5 const& a5, Arg6 const& a6,
        Arg7 const& a7) const
{
    return testFunc7(a1, a2, a3, a4, a5, a6, a7);
}

inline
int bdef_Bind_TestTypeAlloc::operator()(Arg1 const& a1, Arg2 const& a2,
        Arg3 const& a3, Arg4 const& a4, Arg5 const& a5, Arg6 const& a6,
        Arg7 const& a7, Arg8 const& a8) const
{
    return testFunc8(a1, a2, a3, a4, a5, a6, a7, a8);
}

inline
int bdef_Bind_TestTypeAlloc::operator()(Arg1 const& a1, Arg2 const& a2,
        Arg3 const& a3, Arg4 const& a4, Arg5 const& a5, Arg6 const& a6,
        Arg7 const& a7, Arg8 const& a8, Arg9 const& a9) const
{
    return testFunc9(a1, a2, a3, a4, a5, a6, a7, a8, a9);
}

inline
int bdef_Bind_TestTypeAlloc::operator()(Arg1 const& a1, Arg2  const& a2,
        Arg3 const& a3, Arg4 const& a4, Arg5 const& a5, Arg6  const& a6,
        Arg7 const& a7, Arg8 const& a8, Arg9 const& a9, Arg10 const& a10) const
{
    return testFunc10(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
}

inline
int bdef_Bind_TestTypeAlloc::operator()(
        Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
        Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
        Arg7  const& a7,  Arg8  const& a8,  Arg9  const& a9,
        Arg10 const& a10, Arg11 const& a11) const
{
    return testFunc11(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11);
}

inline
int bdef_Bind_TestTypeAlloc::operator()(
        Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
        Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
        Arg7  const& a7,  Arg8  const& a8,  Arg9  const& a9,
        Arg10 const& a10, Arg11 const& a11, Arg12 const& a12) const
{
    return testFunc12(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12);
}

inline
int bdef_Bind_TestTypeAlloc::operator()(
        Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
        Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
        Arg7  const& a7,  Arg8  const& a8,  Arg9  const& a9,
        Arg10 const& a10, Arg11 const& a11, Arg12 const& a12,
        Arg13 const& a13) const
{
    return testFunc13(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12,
                      a13);
}

inline
int bdef_Bind_TestTypeAlloc::operator()(
        Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,
        Arg4  const& a4,  Arg5  const& a5,  Arg6  const& a6,
        Arg7  const& a7,  Arg8  const& a8,  Arg9  const& a9,
        Arg10 const& a10, Arg11 const& a11, Arg12 const& a12,
        Arg13 const& a13, Arg14 const& a14) const
{
    return testFunc14(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12,
                      a13, a14);
}

inline
int bdef_Bind_TestTypeAlloc::testFunc0() const
{
    return 0;
}

inline
int bdef_Bind_TestTypeAlloc::testFunc1(Arg1 const& a1) const
{
    d_a1 = a1; bdef_Bind_TestSlotsAlloc::setSlot(a1.allocator(), 1);

    return 1;
}

inline
int bdef_Bind_TestTypeAlloc::testFunc2(Arg1 const& a1, Arg2 const& a2) const
{
    d_a1 = a1; bdef_Bind_TestSlotsAlloc::setSlot(a1.allocator(), 1);
    d_a2 = a2; bdef_Bind_TestSlotsAlloc::setSlot(a2.allocator(), 2);

    return 2;
}

inline
int bdef_Bind_TestTypeAlloc::testFunc3(
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3) const
{
    d_a1 = a1; bdef_Bind_TestSlotsAlloc::setSlot(a1.allocator(), 1);
    d_a2 = a2; bdef_Bind_TestSlotsAlloc::setSlot(a2.allocator(), 2);
    d_a3 = a3; bdef_Bind_TestSlotsAlloc::setSlot(a3.allocator(), 3);

    return 3;
}

inline
int bdef_Bind_TestTypeAlloc::testFunc4(
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4) const
{
    d_a1 = a1; bdef_Bind_TestSlotsAlloc::setSlot(a1.allocator(), 1);
    d_a2 = a2; bdef_Bind_TestSlotsAlloc::setSlot(a2.allocator(), 2);
    d_a3 = a3; bdef_Bind_TestSlotsAlloc::setSlot(a3.allocator(), 3);
    d_a4 = a4; bdef_Bind_TestSlotsAlloc::setSlot(a4.allocator(), 4);

    return 4;
}
inline
int bdef_Bind_TestTypeAlloc::testFunc5(
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4,
        Arg5 const& a5) const
{
    d_a1 = a1; bdef_Bind_TestSlotsAlloc::setSlot(a1.allocator(), 1);
    d_a2 = a2; bdef_Bind_TestSlotsAlloc::setSlot(a2.allocator(), 2);
    d_a3 = a3; bdef_Bind_TestSlotsAlloc::setSlot(a3.allocator(), 3);
    d_a4 = a4; bdef_Bind_TestSlotsAlloc::setSlot(a4.allocator(), 4);
    d_a5 = a5; bdef_Bind_TestSlotsAlloc::setSlot(a5.allocator(), 5);

    return 5;
}

inline
int bdef_Bind_TestTypeAlloc::testFunc6(
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4,
        Arg5 const& a5, Arg6 const& a6) const
{
    d_a1 = a1; bdef_Bind_TestSlotsAlloc::setSlot(a1.allocator(), 1);
    d_a2 = a2; bdef_Bind_TestSlotsAlloc::setSlot(a2.allocator(), 2);
    d_a3 = a3; bdef_Bind_TestSlotsAlloc::setSlot(a3.allocator(), 3);
    d_a4 = a4; bdef_Bind_TestSlotsAlloc::setSlot(a4.allocator(), 4);
    d_a5 = a5; bdef_Bind_TestSlotsAlloc::setSlot(a5.allocator(), 5);
    d_a6 = a6; bdef_Bind_TestSlotsAlloc::setSlot(a6.allocator(), 6);

    return 6;
}

inline
int bdef_Bind_TestTypeAlloc::testFunc7(
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4,
        Arg5 const& a5, Arg6 const& a6, Arg7 const& a7) const
{
    d_a1 = a1; bdef_Bind_TestSlotsAlloc::setSlot(a1.allocator(), 1);
    d_a2 = a2; bdef_Bind_TestSlotsAlloc::setSlot(a2.allocator(), 2);
    d_a3 = a3; bdef_Bind_TestSlotsAlloc::setSlot(a3.allocator(), 3);
    d_a4 = a4; bdef_Bind_TestSlotsAlloc::setSlot(a4.allocator(), 4);
    d_a5 = a5; bdef_Bind_TestSlotsAlloc::setSlot(a5.allocator(), 5);
    d_a6 = a6; bdef_Bind_TestSlotsAlloc::setSlot(a6.allocator(), 6);
    d_a7 = a7; bdef_Bind_TestSlotsAlloc::setSlot(a7.allocator(), 7);

    return 7;
}

inline
int bdef_Bind_TestTypeAlloc::testFunc8(
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4,
        Arg5 const& a5, Arg6 const& a6, Arg7 const& a7, Arg8 const& a8) const
{
    d_a1 = a1; bdef_Bind_TestSlotsAlloc::setSlot(a1.allocator(), 1);
    d_a2 = a2; bdef_Bind_TestSlotsAlloc::setSlot(a2.allocator(), 2);
    d_a3 = a3; bdef_Bind_TestSlotsAlloc::setSlot(a3.allocator(), 3);
    d_a4 = a4; bdef_Bind_TestSlotsAlloc::setSlot(a4.allocator(), 4);
    d_a5 = a5; bdef_Bind_TestSlotsAlloc::setSlot(a5.allocator(), 5);
    d_a6 = a6; bdef_Bind_TestSlotsAlloc::setSlot(a6.allocator(), 6);
    d_a7 = a7; bdef_Bind_TestSlotsAlloc::setSlot(a7.allocator(), 7);
    d_a8 = a8; bdef_Bind_TestSlotsAlloc::setSlot(a8.allocator(), 8);

    return 8;
}

inline
int bdef_Bind_TestTypeAlloc::testFunc9(
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4,
        Arg5 const& a5, Arg6 const& a6, Arg7 const& a7, Arg8 const& a8,
        Arg9 const& a9) const
{
    d_a1 = a1; bdef_Bind_TestSlotsAlloc::setSlot(a1.allocator(), 1);
    d_a2 = a2; bdef_Bind_TestSlotsAlloc::setSlot(a2.allocator(), 2);
    d_a3 = a3; bdef_Bind_TestSlotsAlloc::setSlot(a3.allocator(), 3);
    d_a4 = a4; bdef_Bind_TestSlotsAlloc::setSlot(a4.allocator(), 4);
    d_a5 = a5; bdef_Bind_TestSlotsAlloc::setSlot(a5.allocator(), 5);
    d_a6 = a6; bdef_Bind_TestSlotsAlloc::setSlot(a6.allocator(), 6);
    d_a7 = a7; bdef_Bind_TestSlotsAlloc::setSlot(a7.allocator(), 7);
    d_a8 = a8; bdef_Bind_TestSlotsAlloc::setSlot(a8.allocator(), 8);
    d_a9 = a9; bdef_Bind_TestSlotsAlloc::setSlot(a9.allocator(), 9);

    return 9;
}

inline
int bdef_Bind_TestTypeAlloc::testFunc10(
        Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,  Arg4  const& a4,
        Arg5  const& a5,  Arg6  const& a6,  Arg7  const& a7,  Arg8  const& a8,
        Arg9  const& a9,  Arg10 const& a10) const
{
    d_a1  = a1;  bdef_Bind_TestSlotsAlloc::setSlot(a1.allocator(),  1);
    d_a2  = a2;  bdef_Bind_TestSlotsAlloc::setSlot(a2.allocator(),  2);
    d_a3  = a3;  bdef_Bind_TestSlotsAlloc::setSlot(a3.allocator(),  3);
    d_a4  = a4;  bdef_Bind_TestSlotsAlloc::setSlot(a4.allocator(),  4);
    d_a5  = a5;  bdef_Bind_TestSlotsAlloc::setSlot(a5.allocator(),  5);
    d_a6  = a6;  bdef_Bind_TestSlotsAlloc::setSlot(a6.allocator(),  6);
    d_a7  = a7;  bdef_Bind_TestSlotsAlloc::setSlot(a7.allocator(),  7);
    d_a8  = a8;  bdef_Bind_TestSlotsAlloc::setSlot(a8.allocator(),  8);
    d_a9  = a9;  bdef_Bind_TestSlotsAlloc::setSlot(a9.allocator(),  9);
    d_a10 = a10; bdef_Bind_TestSlotsAlloc::setSlot(a10.allocator(), 10);

    return 10;
}

inline
int bdef_Bind_TestTypeAlloc::testFunc11(
        Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,  Arg4  const& a4,
        Arg5  const& a5,  Arg6  const& a6,  Arg7  const& a7,  Arg8  const& a8,
        Arg9  const& a9,  Arg10 const& a10, Arg11 const& a11) const
{
    d_a1  = a1;  bdef_Bind_TestSlotsAlloc::setSlot(a1.allocator(),  1);
    d_a2  = a2;  bdef_Bind_TestSlotsAlloc::setSlot(a2.allocator(),  2);
    d_a3  = a3;  bdef_Bind_TestSlotsAlloc::setSlot(a3.allocator(),  3);
    d_a4  = a4;  bdef_Bind_TestSlotsAlloc::setSlot(a4.allocator(),  4);
    d_a5  = a5;  bdef_Bind_TestSlotsAlloc::setSlot(a5.allocator(),  5);
    d_a6  = a6;  bdef_Bind_TestSlotsAlloc::setSlot(a6.allocator(),  6);
    d_a7  = a7;  bdef_Bind_TestSlotsAlloc::setSlot(a7.allocator(),  7);
    d_a8  = a8;  bdef_Bind_TestSlotsAlloc::setSlot(a8.allocator(),  8);
    d_a9  = a9;  bdef_Bind_TestSlotsAlloc::setSlot(a9.allocator(),  9);
    d_a10 = a10; bdef_Bind_TestSlotsAlloc::setSlot(a10.allocator(), 10);
    d_a11 = a11; bdef_Bind_TestSlotsAlloc::setSlot(a11.allocator(), 11);

    return 11;
}

inline
int bdef_Bind_TestTypeAlloc::testFunc12(
        Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,  Arg4  const& a4,
        Arg5  const& a5,  Arg6  const& a6,  Arg7  const& a7,  Arg8  const& a8,
        Arg9  const& a9,  Arg10 const& a10, Arg11 const& a11,
        Arg12 const& a12) const
{
    d_a1  = a1;  bdef_Bind_TestSlotsAlloc::setSlot(a1.allocator(),  1);
    d_a2  = a2;  bdef_Bind_TestSlotsAlloc::setSlot(a2.allocator(),  2);
    d_a3  = a3;  bdef_Bind_TestSlotsAlloc::setSlot(a3.allocator(),  3);
    d_a4  = a4;  bdef_Bind_TestSlotsAlloc::setSlot(a4.allocator(),  4);
    d_a5  = a5;  bdef_Bind_TestSlotsAlloc::setSlot(a5.allocator(),  5);
    d_a6  = a6;  bdef_Bind_TestSlotsAlloc::setSlot(a6.allocator(),  6);
    d_a7  = a7;  bdef_Bind_TestSlotsAlloc::setSlot(a7.allocator(),  7);
    d_a8  = a8;  bdef_Bind_TestSlotsAlloc::setSlot(a8.allocator(),  8);
    d_a9  = a9;  bdef_Bind_TestSlotsAlloc::setSlot(a9.allocator(),  9);
    d_a10 = a10; bdef_Bind_TestSlotsAlloc::setSlot(a10.allocator(), 10);
    d_a11 = a11; bdef_Bind_TestSlotsAlloc::setSlot(a11.allocator(), 11);
    d_a12 = a12; bdef_Bind_TestSlotsAlloc::setSlot(a12.allocator(), 12);

    return 12;
}

inline
int bdef_Bind_TestTypeAlloc::testFunc13(
        Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,  Arg4  const& a4,
        Arg5  const& a5,  Arg6  const& a6,  Arg7  const& a7,  Arg8  const& a8,
        Arg9  const& a9,  Arg10 const& a10, Arg11 const& a11, Arg12 const& a12,
        Arg13 const& a13) const
{
    d_a1  = a1;  bdef_Bind_TestSlotsAlloc::setSlot(a1.allocator(),  1);
    d_a2  = a2;  bdef_Bind_TestSlotsAlloc::setSlot(a2.allocator(),  2);
    d_a3  = a3;  bdef_Bind_TestSlotsAlloc::setSlot(a3.allocator(),  3);
    d_a4  = a4;  bdef_Bind_TestSlotsAlloc::setSlot(a4.allocator(),  4);
    d_a5  = a5;  bdef_Bind_TestSlotsAlloc::setSlot(a5.allocator(),  5);
    d_a6  = a6;  bdef_Bind_TestSlotsAlloc::setSlot(a6.allocator(),  6);
    d_a7  = a7;  bdef_Bind_TestSlotsAlloc::setSlot(a7.allocator(),  7);
    d_a8  = a8;  bdef_Bind_TestSlotsAlloc::setSlot(a8.allocator(),  8);
    d_a9  = a9;  bdef_Bind_TestSlotsAlloc::setSlot(a9.allocator(),  9);
    d_a10 = a10; bdef_Bind_TestSlotsAlloc::setSlot(a10.allocator(), 10);
    d_a11 = a11; bdef_Bind_TestSlotsAlloc::setSlot(a11.allocator(), 11);
    d_a12 = a12; bdef_Bind_TestSlotsAlloc::setSlot(a12.allocator(), 12);
    d_a13 = a13; bdef_Bind_TestSlotsAlloc::setSlot(a13.allocator(), 13);

    return 13;
}

inline
int bdef_Bind_TestTypeAlloc::testFunc14(
        Arg1  const& a1,  Arg2  const& a2,  Arg3  const& a3,  Arg4  const& a4,
        Arg5  const& a5,  Arg6  const& a6,  Arg7  const& a7,  Arg8  const& a8,
        Arg9  const& a9,  Arg10 const& a10, Arg11 const& a11, Arg12 const& a12,
        Arg13 const& a13, Arg14 const& a14) const
{
    d_a1  = a1;  bdef_Bind_TestSlotsAlloc::setSlot(a1.allocator(),  1);
    d_a2  = a2;  bdef_Bind_TestSlotsAlloc::setSlot(a2.allocator(),  2);
    d_a3  = a3;  bdef_Bind_TestSlotsAlloc::setSlot(a3.allocator(),  3);
    d_a4  = a4;  bdef_Bind_TestSlotsAlloc::setSlot(a4.allocator(),  4);
    d_a5  = a5;  bdef_Bind_TestSlotsAlloc::setSlot(a5.allocator(),  5);
    d_a6  = a6;  bdef_Bind_TestSlotsAlloc::setSlot(a6.allocator(),  6);
    d_a7  = a7;  bdef_Bind_TestSlotsAlloc::setSlot(a7.allocator(),  7);
    d_a8  = a8;  bdef_Bind_TestSlotsAlloc::setSlot(a8.allocator(),  8);
    d_a9  = a9;  bdef_Bind_TestSlotsAlloc::setSlot(a9.allocator(),  9);
    d_a10 = a10; bdef_Bind_TestSlotsAlloc::setSlot(a10.allocator(), 10);
    d_a11 = a11; bdef_Bind_TestSlotsAlloc::setSlot(a11.allocator(), 11);
    d_a12 = a12; bdef_Bind_TestSlotsAlloc::setSlot(a12.allocator(), 12);
    d_a13 = a13; bdef_Bind_TestSlotsAlloc::setSlot(a13.allocator(), 13);
    d_a14 = a14; bdef_Bind_TestSlotsAlloc::setSlot(a14.allocator(), 14);

    return 14;
}

inline
void bdef_Bind_TestTypeAlloc::setSlots()
{
    bdef_Bind_TestSlotsAlloc::setSlot(d_a1.allocator(),  1);
    bdef_Bind_TestSlotsAlloc::setSlot(d_a2.allocator(),  2);
    bdef_Bind_TestSlotsAlloc::setSlot(d_a3.allocator(),  3);
    bdef_Bind_TestSlotsAlloc::setSlot(d_a4.allocator(),  4);
    bdef_Bind_TestSlotsAlloc::setSlot(d_a5.allocator(),  5);
    bdef_Bind_TestSlotsAlloc::setSlot(d_a6.allocator(),  6);
    bdef_Bind_TestSlotsAlloc::setSlot(d_a7.allocator(),  7);
    bdef_Bind_TestSlotsAlloc::setSlot(d_a8.allocator(),  8);
    bdef_Bind_TestSlotsAlloc::setSlot(d_a9.allocator(),  9);
    bdef_Bind_TestSlotsAlloc::setSlot(d_a10.allocator(), 10);
    bdef_Bind_TestSlotsAlloc::setSlot(d_a11.allocator(), 11);
    bdef_Bind_TestSlotsAlloc::setSlot(d_a12.allocator(), 12);
    bdef_Bind_TestSlotsAlloc::setSlot(d_a13.allocator(), 13);
    bdef_Bind_TestSlotsAlloc::setSlot(d_a14.allocator(), 14);
}

inline
void bdef_Bind_TestTypeAlloc::print() const
{
    bsl::printf("{ %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d }\n",
                d_a1.value(),  d_a2.value(),  d_a3.value(),  d_a4.value(),
                d_a5.value(),  d_a6.value(),  d_a7.value(),  d_a8.value(),
                d_a9.value(),  d_a10.value(), d_a11.value(), d_a12.value(),
                d_a13.value(), d_a14.value());
}

// FREE OPERATORS
inline
bool operator==(const bdef_Bind_TestTypeAlloc& lhs,
                const bdef_Bind_TestTypeAlloc& rhs)
{
    return lhs.d_a1.value()  == rhs.d_a1.value()
        && lhs.d_a1.value()  == rhs.d_a1.value()
        && lhs.d_a2.value()  == rhs.d_a2.value()
        && lhs.d_a3.value()  == rhs.d_a3.value()
        && lhs.d_a4.value()  == rhs.d_a4.value()
        && lhs.d_a5.value()  == rhs.d_a5.value()
        && lhs.d_a6.value()  == rhs.d_a6.value()
        && lhs.d_a7.value()  == rhs.d_a7.value()
        && lhs.d_a8.value()  == rhs.d_a8.value()
        && lhs.d_a9.value()  == rhs.d_a9.value()
        && lhs.d_a10.value() == rhs.d_a10.value()
        && lhs.d_a11.value() == rhs.d_a11.value()
        && lhs.d_a12.value() == rhs.d_a12.value()
        && lhs.d_a13.value() == rhs.d_a13.value()
        && lhs.d_a14.value() == rhs.d_a14.value();
}

inline
bool operator!=(const bdef_Bind_TestTypeAlloc& lhs,
                const bdef_Bind_TestTypeAlloc& rhs)
{
    return !(lhs == rhs);
}

                        // ----------------------------------
                        // class bdef_Bind_TestFunctionsAlloc
                        // ----------------------------------

inline
int bdef_Bind_TestFunctionsAlloc::func0(bdef_Bind_TestTypeAlloc *o)
{
    return o->testFunc0();
}

inline
int bdef_Bind_TestFunctionsAlloc::func1(bdef_Bind_TestTypeAlloc *o,
        Arg1 const& a1)
{
    return o->testFunc1(a1);
}

inline
int bdef_Bind_TestFunctionsAlloc::func2(bdef_Bind_TestTypeAlloc *o,
        Arg1 const& a1, Arg2 const& a2)
{
    return o->testFunc2(a1, a2);
}

inline
int bdef_Bind_TestFunctionsAlloc::func3(bdef_Bind_TestTypeAlloc *o,
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3)
{
    return o->testFunc3(a1, a2, a3);
}

inline
int bdef_Bind_TestFunctionsAlloc::func4(bdef_Bind_TestTypeAlloc *o,
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4)
{
    return o->testFunc4(a1, a2, a3, a4);
}

inline
int bdef_Bind_TestFunctionsAlloc::func5(bdef_Bind_TestTypeAlloc *o,
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4,
        Arg5 const& a5)
{
    return o->testFunc5(a1, a2, a3, a4, a5);
}

inline
int bdef_Bind_TestFunctionsAlloc::func6(bdef_Bind_TestTypeAlloc *o,
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4,
        Arg5 const& a5, Arg6 const& a6)
{
    return o->testFunc6(a1, a2, a3, a4, a5, a6);
}

inline
int bdef_Bind_TestFunctionsAlloc::func7(bdef_Bind_TestTypeAlloc *o,
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4,
        Arg5 const& a5, Arg6 const& a6, Arg7 const& a7)
{
    return o->testFunc7(a1, a2, a3, a4, a5, a6, a7);
}

inline
int bdef_Bind_TestFunctionsAlloc::func8(bdef_Bind_TestTypeAlloc *o,
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4,
        Arg5 const& a5, Arg6 const& a6, Arg7 const& a7, Arg8 const& a8)
{
    return o->testFunc8(a1, a2, a3, a4, a5, a6, a7, a8);
}

inline
int bdef_Bind_TestFunctionsAlloc::func9(bdef_Bind_TestTypeAlloc *o,
        Arg1 const& a1, Arg2 const& a2, Arg3 const& a3, Arg4 const& a4,
        Arg5 const& a5, Arg6 const& a6, Arg7 const& a7, Arg8 const& a8,
        Arg9 const& a9)
{
    return o->testFunc9(a1, a2, a3, a4, a5, a6, a7, a8, a9);
}

inline
int bdef_Bind_TestFunctionsAlloc::func10(bdef_Bind_TestTypeAlloc *o,
        Arg1 const& a1, Arg2  const& a2, Arg3  const& a3,  Arg4  const& a4,
        Arg5 const& a5, Arg6  const& a6, Arg7  const& a7,  Arg8  const& a8,
        Arg9 const& a9, Arg10 const& a10)
{
    return o->testFunc10(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
}

inline
int bdef_Bind_TestFunctionsAlloc::func11(bdef_Bind_TestTypeAlloc *o,
        Arg1 const& a1, Arg2  const& a2, Arg3  const& a3,  Arg4  const& a4,
        Arg5 const& a5, Arg6  const& a6, Arg7  const& a7,  Arg8  const& a8,
        Arg9 const& a9, Arg10 const& a10, Arg11 const& a11)
{
    return o->testFunc11(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11);
}

inline
int bdef_Bind_TestFunctionsAlloc::func12(bdef_Bind_TestTypeAlloc *o,
        Arg1 const& a1, Arg2  const& a2, Arg3  const& a3,  Arg4  const& a4,
        Arg5 const& a5, Arg6  const& a6, Arg7  const& a7,  Arg8  const& a8,
        Arg9 const& a9, Arg10 const& a10, Arg11 const& a11, Arg12 const& a12)
{
    return o->testFunc12(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12);
}

inline
int bdef_Bind_TestFunctionsAlloc::func13(bdef_Bind_TestTypeAlloc *o,
        Arg1  const& a1,  Arg2  const& a2, Arg3  const& a3,  Arg4  const& a4,
        Arg5  const& a5,  Arg6  const& a6, Arg7  const& a7,  Arg8  const& a8,
        Arg9  const& a9,  Arg10 const& a10, Arg11 const& a11, Arg12 const& a12,
        Arg13 const& a13)
{
    return o->testFunc13(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12,
                         a13);
}

inline
int bdef_Bind_TestFunctionsAlloc::func14(bdef_Bind_TestTypeAlloc *o,
        Arg1  const& a1,  Arg2  const& a2, Arg3  const& a3,  Arg4  const& a4,
        Arg5  const& a5,  Arg6  const& a6, Arg7  const& a7,  Arg8  const& a8,
        Arg9  const& a9,  Arg10 const& a10, Arg11 const& a11, Arg12 const& a12,
        Arg13 const& a13, Arg14 const& a14)
{
    return o->testFunc14(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12,
                         a13, a14);
}

} // close namespace BloombergLP



#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
