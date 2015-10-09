// bdlf_bind_test.h                                                   -*-C++-*-
#ifndef INCLUDED_BDLF_BIND_TEST
#define INCLUDED_BDLF_BIND_TEST

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a test apparatus for bdlf_bind.
//
//@CLASSES:
//             bdlf::Bind_TestUtil: utility class for static test methods
//        bdlf::Bind_TestSlotsBase: base class for tracking arg & alloc objects
//       bdlf::Bind_TestArgNoAlloc: argument type parameterized by index
//      bdlf::Bind_TestTypeNoAlloc: invocable supporting up to 14 arguments
// bdlf::Bind_TestFunctionsNoAlloc: global versions of test type methods
//         bdlf::Bind_TestArgAlloc: argument type parameterized by index
//        bdlf::Bind_TestTypeAlloc: invocable supporting up to 14 arguments
//   bdlf::Bind_TestFunctionsAlloc: global versions of test type methods
//
//@SEE_ALSO: bdlf_bind bdlf_bind_test[0--14]
//
//@DESCRIPTION: This component provides a test apparatus for testing the
// 'bdlf_bind' component.  This apparatus would usually be part of the test
// driver of that component, but is factored out in this component since the
// 'bdlf_bind' test driver is so large and induces such long compilation times
// that the testing has been delegated to many components
// 'bdlf_bind_test[0-14]' whose sole purpose is to share the testing
// 'bdlf_bind'.  Thus this component provides all the common apparatus for
// these delegate components.
//
// This apparatus comprises test classes for tracking argument values and
// allocators, using a slot mechanism.  In the case of a test class that does
// *not* take allocators, the slot at any index contains an integer which
// tracks the value of the argument with same index that was passed to the test
// type invocation.  In the case of a test class that *does* take allocators,
// the slot at any index contains the address of the allocator used to allocate
// the value of the argument with same index that was passed to the test type
// invocation.
//
// The two test types provided, 'bdlf::Bind_TestTypeNoAlloc' and
// 'bdlf::Bind_TestTypeAlloc' support this slot mechanism.  They are invocable
// with up to 14 parameters, either as function objects (using 'operator()'),
// via member functions (using the 'testFunc[0--14]' methods), or via global
// functions that take a pointer to the object as first argument (provided as
// static methods of the utility classes 'bdlf::Bind_TestFunctionsNoAlloc' and
// 'bdlf::Bind_TestFunctionsAlloc'), thus supporting all protocols for binding
// an invocable in the 'bdlf_bind' component.  Additional classes
// 'bdlf::Bind_TestSlots', 'bdlf::Bind_TestArgNoAlloc', and
// 'bdlf::Bind_TestArgAlloc' augment the test apparatus.
//
///Usage
///-----
// The following shows how to use the test apparatus to test the 'bdlf::Bind'
// constructors, i.e., to explicitly construct 'bdlf::Bind' objects (as opposed
// to using the factory method 'bdlf::BindUtil::bind') and exercise them to
// ascertain that they work as expected.  For more examples of use, see the
// test drivers 'bdlf_bind_test[0--14]' themselves.
//
///Testing 'bdlf::Bind' *Without* Allocators
///- - - - - - - - - - - - - - - - - - - - -
// In this test code, we bind a 'bdlf::Bind_TestTypeNoAlloc' invocable by a
// 'bdlf::Bind' object constructed with and without placeholders.
//..
//  void usageExampleNoAlloc(int veryVeryVerbose)
//  {
//      bslma::TestAllocator          allocator0(veryVeryVerbose);
//      bslma::TestAllocator         *Z0 = &allocator0;
//      bslma::DefaultAllocatorGuard  allocGuard(Z0);
//
//      const int                         N1 = -1;
//      const bdlf::Bind_TestArgNoAlloc<1> I1 = 1;
//
//      // 1 argument to function object, without placeholders.
//      {
//          const int NUM_ALLOCS = Z0->numAllocations();
//
//                bdlf::Bind_TestTypeNoAlloc  mX;
//          const bdlf::Bind_TestTypeNoAlloc& X = mX;
//
//          const bdlf::Bind_TestTypeNoAlloc EXPECTED_X(I1);
//          const bdlf::Bind_TestTypeNoAlloc DEFAULT_X(N1);
//          assert(EXPECTED_X != X);
//          assert(DEFAULT_X  == X);
//
//          // For passing to the constructor of 'bdlf::Bind'.
//
//          typedef bdlf::Bind_TestTypeNoAlloc *FUNC;
//          typedef bdlf::Bind_Tuple1<bdlf::Bind_TestArgNoAlloc<1> > ListType;
//          typedef bdlf::Bind<bslmf::Nil, FUNC, ListType> Bind;
//
//                ListType  mL(I1);     // list of arguments
//          const ListType& L = mL;     // non-modifiable list of arguments
//
//                ListType  mM(N1);     // list of arguments
//          const ListType& M = mM;     // non-modifiable list of arguments
//
//          // Testing 'bdlf::Bind' objects constructed explicitly.
//
//                Bind  mB1(&mX, L);
//          const Bind& B1 = mB1;
//          assert(EXPECTED_X != X);
//          assert(1 == mB1());
//          assert(EXPECTED_X == X);
//
//                Bind  mB2(&mX, M, Z0);
//          const Bind& B2 = mB2;
//          assert(DEFAULT_X != X);
//          assert(1 == mB2());
//          assert(DEFAULT_X == X);
//
//          Bind mB3(B1);
//          assert(EXPECTED_X != X);
//          assert(1 == mB3());
//          assert(EXPECTED_X == X);
//
//          Bind mB4(B2, Z0);
//          assert(DEFAULT_X != X);
//          assert(1 == mB4());
//          assert(DEFAULT_X == X);
//
//          assert(NUM_ALLOCS == Z0->numAllocations());
//      }
//
//      // 1 argument to function object, *with* placeholders.
//      {
//          using namespace bdlf::PlaceHolders;
//
//          const int NUM_ALLOCS = Z0->numAllocations();
//
//                bdlf::Bind_TestTypeNoAlloc  mX;
//          const bdlf::Bind_TestTypeNoAlloc& X = mX;
//
//          const bdlf::Bind_TestTypeNoAlloc EXPECTED_X(I1);
//          const bdlf::Bind_TestTypeNoAlloc DEFAULT_X(N1);
//          assert(EXPECTED_X != X);
//          assert(DEFAULT_X  == X);
//
//          // For passing to the constructor of 'bdlf::Bind'.
//
//          typedef bdlf::Bind_TestTypeNoAlloc *FUNC;
//          typedef bdlf::Bind_Tuple1<PH1> ListType;
//          typedef bdlf::Bind<bslmf::Nil, FUNC, ListType> Bind;
//
//                ListType  mL(_1);     // list of arguments
//          const ListType& L = mL;     // non-modifiable list of arguments
//
//                Bind  mB1(&mX, L);
//          const Bind& B1 = mB1;
//          assert(EXPECTED_X != X);
//          assert(1 == mB1(I1));
//          assert(EXPECTED_X == X);
//
//                Bind  mB2(&mX, L, Z0);
//          const Bind& B2 = mB2;
//          assert(DEFAULT_X != X);
//          assert(1 == mB2(N1));
//          assert(DEFAULT_X == X);
//
//          Bind mB3(B1);
//          assert(EXPECTED_X != X);
//          assert(1 == mB3(I1));
//          assert(EXPECTED_X == X);
//
//          Bind mB4(B2, Z0);
//          assert(DEFAULT_X != X);
//          assert(1 == mB4(N1));
//          assert(DEFAULT_X == X);
//
//          assert(NUM_ALLOCS == Z0->numAllocations());
//      }
//  }
//..
//
///Testing 'bdlf_bind' *With* Allocators
///- - - - - - - - - - - - - - - - - - -
// In this test code, we bind a 'bdlf::Bind_TestTypeAlloc' invocable using a
// 'bdlf::Bind' object constructed with and without placeholders.
//..
//  void usageExampleAlloc(int veryVeryVerbose)
//  {
//      bslma::TestAllocator  allocator0(veryVeryVerbose);
//      bslma::TestAllocator  allocator1(veryVeryVerbose);
//      bslma::TestAllocator  allocator2(veryVeryVerbose);
//
//      bslma::TestAllocator *Z0 = &allocator0;
//      bslma::TestAllocator *Z1 = &allocator1;
//      bslma::TestAllocator *Z2 = &allocator2;
//
//      bslma::DefaultAllocatorGuard allocGuard(Z0);
//
//      const bdlf::Bind_TestArgAlloc<1> NV1 = -1;
//      const bdlf::Bind_TestArgAlloc<1>  V1 =  1;
//
//      // 1 argument to function object, *without* placeholders.
//      {
//                bdlf::Bind_TestTypeAlloc  mX(Z1);
//          const bdlf::Bind_TestTypeAlloc& X = mX;
//
//          const bdlf::Bind_TestTypeAlloc EXPECTED_X(Z1, V1);
//          const bdlf::Bind_TestTypeAlloc DEFAULT_X(Z1, NV1);
//          assert(EXPECTED_X != X);
//          assert(DEFAULT_X  == X);
//
//          typedef bdlf::Bind_TestTypeAlloc *FUNC;
//          typedef bdlf::Bind_Tuple1<bdlf::Bind_TestArgAlloc<1> > ListType;
//          typedef bdlf::Bind<bslmf::Nil, FUNC, ListType> Bind;
//
//          // For passing to the constructor of 'bdlf::Bind'.
//
//                ListType  mL(V1);  // list of arguments
//          const ListType& L = mL;  // non-modifiable list of arguments
//
//                ListType  mM(NV1); // list of arguments
//          const ListType& M = mM;  // non-modifiable list of arguments
//
//                Bind  mB1(&mX, L);
//          const Bind& B1 = mB1;
//          assert(EXPECTED_X != X);
//          assert(1 == mB1());
//          assert(EXPECTED_X == X);
//
//                Bind  mB2(&mX, M, Z2);
//          const Bind& B2 = mB2;
//          assert(DEFAULT_X != X);
//          assert(1 == mB2());
//          assert(DEFAULT_X == X);
//
//          Bind mB3(B1);
//          assert(EXPECTED_X != X);
//          assert(1 == mB3());
//          assert(EXPECTED_X == X);
//
//          Bind mB4(B2, Z2);
//          assert(DEFAULT_X != X);
//          assert(1 == mB4());
//          assert(DEFAULT_X == X);
//      }
//
//      // 1 argument to function object, *with* placeholders.
//      {
//          using namespace bdlf::PlaceHolders;
//
//                bdlf::Bind_TestTypeAlloc  mX(Z1);
//          const bdlf::Bind_TestTypeAlloc& X = mX;
//
//          const bdlf::Bind_TestTypeAlloc EXPECTED_X(Z1, V1);
//          const bdlf::Bind_TestTypeAlloc DEFAULT_X(Z1, NV1);
//          assert(EXPECTED_X != X);
//          assert(DEFAULT_X  == X);
//
//          typedef bdlf::Bind_TestTypeAlloc *FUNC;
//          typedef bdlf::Bind_Tuple1<PH1> ListType;
//          typedef bdlf::Bind<bslmf::Nil, FUNC, ListType> Bind;
//
//          // For passing to the constructor of 'bdlf::Bind'.
//
//                ListType  mL(_1);  // list of arguments
//          const ListType& L = mL;  // non-modifiable list of arguments
//
//                Bind  mB1(&mX, L);
//          const Bind& B1 = mB1;
//          assert(EXPECTED_X != X);
//          assert(1 == mB1(V1));
//          assert(EXPECTED_X == X);
//
//                Bind  mB2(&mX, L, Z2);
//          const Bind& B2 = mB2;
//          assert(DEFAULT_X != X);
//          assert(1 == mB2(NV1));
//          assert(DEFAULT_X == X);
//
//          Bind mB3(B1);
//          assert(EXPECTED_X != X);
//          assert(1 == mB3(V1));
//          assert(EXPECTED_X == X);
//
//          Bind mB4(B2, Z2);
//          assert(DEFAULT_X != X);
//          assert(1 == mB4(NV1));
//          assert(DEFAULT_X == X);
//      }
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSL_CSTDIO
#include <bsl_cstdio.h>
#endif

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

#define P(n) Arg##n const& a##n
#define V(n) Arg##n a##n
#define A(n) a##n

namespace BloombergLP {

namespace bdlf {
                      // ===================
                      // class Bind_TestUtil
                      // ===================

struct Bind_TestUtil {
    // Utility class for static functions useful in 'bdlf_bind' testing.

    template <class T>
    static bool isBitwiseMoveableType(const T&);
        // Return true if the specified paramter type 'T' has the
        // 'bslmf::IsBitwiseMovable' trait and false otherwise.
};


                      // ====================
                      // class Bind_TestSlots
                      // ====================

template <class VALUE>
struct Bind_TestSlotsBase {
    // This 'struct' defines an array of 'VALUE' to keep track (in conjunction
    // with the 'Bind_TestTypeNoAlloc' or 'Bind_TestTypeAlloc' classes) of
    // which value or allocator is being passed to which argument of the test
    // function by a 'bdlf_bind' object.

    // ENUMERATIONS
    enum {
        k_NUM_SLOTS = 15
    };

  private:
    // PRIVATE CLASS DATA
    static VALUE s_slots[k_NUM_SLOTS];

  public:
    // CLASS METHODS
    static VALUE getSlot(int index);
        // Get the value of the slot at the specified 'index'.

    static void resetSlots(VALUE value = 0);
        // Reset all the slots in this instance to the optionally specified
        // 'value'.

    static void setSlot(VALUE value, int index);
        // Set the slot at the specified 'index' to the specified 'value'.

    static bool verifySlots(const VALUE *EXPECTED, bool verboseFlag = true);
        // Verify that all the slots in this instance compare equal to those in
        // the specified 'EXPECTED' value.  If the optionally specified
        // 'verboseFlag' is not set, output diagnostic only in case the slots
        // do not compare equal.
};

                      // ==========================
                      // type Bind_TestSlotsNoAlloc
                      // ==========================

typedef Bind_TestSlotsBase<int> Bind_TestSlotsNoAlloc;
    // When used within the methods of 'Bind_TestTypeNoAlloc', the 'VALUE' type
    // will be 'int' and will keep track of which arguments have been assigned
    // a value (in case 'bdlf_bind' accesses fields that it should not).

                      // =========================
                      // class Bind_TestArgNoAlloc
                      // =========================

template <int ID>
class Bind_TestArgNoAlloc {
    // This very simple 'struct' is used purely to disambiguate types in
    // passing parameters due to the fact that 'Bind_TestArgNoAlloc<ID1>' is a
    // different type than 'Bind_TestArgNoAlloc<ID2>' is ID1 != ID2.  This
    // class does not take an optional allocator.

    // INSTANCE DATA
    int d_value; // value held by this object

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Bind_TestArgNoAlloc,
                                   bslmf::IsBitwiseMoveable);

    // CREATORS
    Bind_TestArgNoAlloc(int value);                            // IMPLICIT
        // Create an object having the specified 'value'.

    // MANIPULATORS
    Bind_TestArgNoAlloc& operator=(const Bind_TestArgNoAlloc &rhs);
        // Assign to this object the value of the specified 'rhs' object.

    // ACCESSORS
    int value() const;
};

// FREE OPERATORS
template <int ID>
inline
bool operator==(Bind_TestArgNoAlloc<ID> const& lhs,
                Bind_TestArgNoAlloc<ID> const& rhs);

template <int ID>
inline
bool operator!=(Bind_TestArgNoAlloc<ID> const& lhs,
                Bind_TestArgNoAlloc<ID> const& rhs);

                      // ==========================
                      // class Bind_TestTypeNoAlloc
                      // ==========================

class Bind_TestTypeNoAlloc {
    // This 'struct' provides a test class capable of holding up to 14 bound
    // parameters of types 'TestArgNoAlloc[1--14]', with full (non-streamable)
    // value semantics defined by the 'operator=='.  By default, a
    // 'Bind_TestTypeNoAlloc' is constructed with nil ('N1') values, but
    // instances can be constructed with actual values (e.g., for creating
    // expected values).  A 'Bind_TestTypeNoAlloc' can be invoked with up to 14
    // parameters, via member functions 'testFunc[1--14]'.  These functions are
    // also called by the overloaded member 'operator()' of the same
    // signatures, and similar global functions 'testFunc[1--14]'.  All
    // invocations support the above 'Bind_TestSlotsNoAlloc' mechanism.
    //
    // This 'struct' intentionally does *not* take an allocator.

    // PRIVATE TYPES
#undef  F
#define F(n) typedef Bind_TestArgNoAlloc<n> Arg##n;
    L14(F)
        // Argument types for shortcut.

    // PRIVATE DATA
#undef  F
#define F(n) mutable Arg##n d_a##n;
    L14(F)

    // FRIEND
    friend bool operator==(const Bind_TestTypeNoAlloc& lhs,
                           const Bind_TestTypeNoAlloc& rhs);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Bind_TestTypeNoAlloc,
                                   bslmf::IsBitwiseMoveable);

    // TYPES
    typedef int ResultType;
        // Type returned by the function operator and test methods.

    enum {
        k_N1 = -1   // default value for all private data
    };

    // CREATORS
#undef  F
#define F(n) Arg##n a##n = k_N1
    explicit Bind_TestTypeNoAlloc(C14(F));
        // Create a test object having the same value as the specified
        // 'original'.

    Bind_TestTypeNoAlloc(const Bind_TestTypeNoAlloc& original);
        // Create a test object having the same value as the specified
        // 'original'.

    // MANIPULATORS
    Bind_TestTypeNoAlloc& operator=(const Bind_TestTypeNoAlloc &rhs);
        // Assign to this object the value of the specified 'rhs' object.

    int operator()() const;
        // Test operators invoking this test type with 0 up to 14 arguments.
#undef  F
#define F(n) int operator()(C##n(P)) const;
    L14(F)

    int testFunc0() const;
        // Test methods invoking this test type with 0 up to 14 arguments.
#undef  F
#define F(n) int testFunc##n(C##n(P)) const;
    L14(F)

    // ACCESSORS
    void print() const;
        // Output the value of this instance to the standard output.
};

// FREE OPERATORS
inline
bool operator==(const Bind_TestTypeNoAlloc& lhs,
                const Bind_TestTypeNoAlloc& rhs);

inline
bool operator!=(const Bind_TestTypeNoAlloc& lhs,
                const Bind_TestTypeNoAlloc& rhs);

                        // ===============================
                        // class Bind_TestFunctionsNoAlloc
                        // ===============================

struct Bind_TestFunctionsNoAlloc {
    // Global versions of 'Bind_TestTypeNoAlloc' member functions.

    // TYPES
#undef  F
#define F(n) typedef Bind_TestArgNoAlloc<n> Arg##n;
    L14(F)
        // Types for shortcut.

    // CLASS METHODS
    static int func0(Bind_TestTypeNoAlloc *object);
        // Invoke the corresponding method 'testFunc[0-14]' on the specified
        // 'object' with the specified arguments 'a[0-14]'.
#undef  F
#define F(n) static int func##n(Bind_TestTypeNoAlloc *object, C##n(P));
    L14(F)
};

                      // =============================
                      // class Bind_TestSlotsAllocBase
                      // =============================

template <class AllocPtr>
struct Bind_TestSlotsAllocBase
{
    // The point of this base class is that we would like it to be a class
    // template, so that the class data does not pollute the library object
    // files.  Since there is no reason to make 'Bind_TestSlotsAlloc' a
    // template, we separate the static pointers into this base class.  These
    // pointers are needed for the pretty printing in the 'verifySlots' method.

    // PRIVATE CLASS DATA
    static AllocPtr s_Z0;
    static AllocPtr s_Z1;
    static AllocPtr s_Z2;

  public:
    // CLASS METHODS
    static void setZ0(AllocPtr Z0);
    static void setZ1(AllocPtr Z1);
    static void setZ2(AllocPtr Z2);
    static AllocPtr getZ0();
    static AllocPtr getZ1();
    static AllocPtr getZ2();
};

                      // =========================
                      // class Bind_TestSlotsAlloc
                      // =========================

class Bind_TestSlotsAlloc
: public Bind_TestSlotsBase<const bslma::Allocator*>
, public Bind_TestSlotsAllocBase<const bslma::Allocator*>
{
    // When used within the methods of 'Bind_TestTypeAlloc', the 'VALUE' type
    // will be 'const bslma::Allocator*' and will keep track of which allocator
    // is used with which argument of the 'Bind' object.  The sole purpose of
    // deriving a new class is to use pretty printing of the allocators in
    // 'verifySlots'.

  public:
    static bool verifySlots(const bslma::Allocator *const *EXPECTED,
                            bool                           verboseFlag = true);
        // Verify that all the slots in this instance compare equal to those in
        // the specified 'EXPECTED' value.  If the optionally specified
        // 'verboseFlag' is not set, output diagnostic only in case the slots
        // do not compare equal.
};

                      // =======================
                      // class Bind_TestArgAlloc
                      // =======================

template <int ID>
class Bind_TestArgAlloc {
    // This class is used to disambiguate types in passing parameters due to
    // the fact that 'Bind_TestArgAlloc<ID1>' is a different type than
    // 'Bind_TestArgAlloc<ID2>' is ID1 != ID2.  This class is used for
    // testing memory allocator issues.

    // PRIVATE DATA
    bslma::Allocator *d_allocator_p;  // memory allocator (held, not owned)
    int              *d_value;        // value allocated from 'd_allocator_p'
        // NOTE: *Must* be declared and initialized in that order.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Bind_TestArgAlloc,
                                   bslma::UsesBslmaAllocator);

    // CREATORS

#if defined(BSLS_PLATFORM_CMP_IBM) && BSLS_PLATFORM_CMP_VER_MAJOR >= 0x0900
    // TBD: This is probably an IBM xlC compiler bug.  Reproduce in a small
    // test case later.
    Bind_TestArgAlloc(int value);                                   // IMPLICIT
    Bind_TestArgAlloc(int value, bslma::Allocator *allocator);
#else
    Bind_TestArgAlloc(int value, bslma::Allocator *allocator = 0);  // IMPLICIT
#endif
        // Create an object having the specified 'value'.  Use the specified
        // 'allocator' to supply memory.  If 'allocator' is 0, use the
        // currently installed default allocator.

#if defined(BSLS_PLATFORM_CMP_IBM) && BSLS_PLATFORM_CMP_VER_MAJOR >= 0x0900
    Bind_TestArgAlloc(const Bind_TestArgAlloc&  original);
    Bind_TestArgAlloc(const Bind_TestArgAlloc&  original,
                      bslma::Allocator         *allocator);
#else
    Bind_TestArgAlloc(const Bind_TestArgAlloc&  original,
                      bslma::Allocator         *allocator = 0);
#endif
        // Create a copy of the specified non-modifiable 'original'.    Use the
        // specified 'allocator' to supply memory.  If 'allocator' is 0, use
        // the currently installed default allocator.

    ~Bind_TestArgAlloc();
        // Destroy this object.

    // MANIPULATORS
    Bind_TestArgAlloc& operator=(const Bind_TestArgAlloc &rhs);
        // Assign to this object the value of the specified 'rhs' object.

    // ACCESSORS
    const bslma::Allocator *allocator() const;
        // Return the allocator specified at construction of this object.

    int value() const;
        // Return the value of this object.
};

// FREE OPERATORS
template <int ID>
inline
bool operator==(const Bind_TestArgAlloc<ID>& lhs,
                const Bind_TestArgAlloc<ID>& rhs);

template <int ID>
inline
bool operator!=(const Bind_TestArgAlloc<ID>& lhs,
                const Bind_TestArgAlloc<ID>& rhs);

                      // ========================
                      // class Bind_TestTypeAlloc
                      // ========================

class Bind_TestTypeAlloc {
    // This class provides a test class capable of holding up to 14 bound
    // parameters of types 'Bind_TestArgAlloc[1--14]', with full
    // (non-streamable) value semantics defined by the 'operator=='.  By
    // default, a 'Bind_TestTypeAlloc' is constructed with nil ('k_N1') values,
    // but objects can be constructed with actual values (e.g., for creating
    // expected values).  A 'Bind_TestTypeAlloc' can be invoked with up to 14
    // parameters, via member functions 'testFunc[1--14]'.  These functions are
    // also called by the overloaded member 'operator()' of the same
    // signatures, and similar global functions 'testFunc1--14'.  All
    // invocations support the above 'Bind_TestSlotsAlloc' mechanism.
    //
    // This class intentionally *does* take an allocator.

    // PRIVATE TYPES
#undef  F
#define F(n) typedef Bind_TestArgAlloc<n> Arg##n;
    L14(F)
        // Argument types for shortcut.

    enum {
        k_N1 = -1   // default value for all private data
    };

    // PRIVATE DATA
#undef  F
#define F(n) mutable Arg##n d_a##n;
    L14(F)

    // FRIEND
    friend bool operator==(const Bind_TestTypeAlloc& lhs,
                           const Bind_TestTypeAlloc& rhs);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Bind_TestTypeAlloc,
                                   bslma::UsesBslmaAllocator);

    // PUBLIC TYPES
    typedef int ResultType;
        // Type returned by the function operator and test methods.

    // CREATORS
#undef  F
#define F(n) Arg##n a##n = k_N1
    explicit Bind_TestTypeAlloc(bslma::Allocator *allocator = 0, C14(F));
        // This constructor does *not* participate in the
        // 'UsesBdemaAllocatorTraits' contract, it is here simply to allow to
        // construct expected values with a specified 'allocator' as the first
        // argument (otherwise there would need to be fourteen different
        // constructors with 'allocator' as the last argument).

#undef  F
#define F(n) Arg##n a##n
    Bind_TestTypeAlloc(C14(F), bslma::Allocator *allocator = 0);

    Bind_TestTypeAlloc(const Bind_TestTypeAlloc&  original,
                       bslma::Allocator          *allocator = 0);

    // MANIPULATORS
    Bind_TestTypeAlloc& operator=(const Bind_TestTypeAlloc &rhs);
        // Assign to this object the value of the specified 'rhs' object.

    int operator()() const;
        // Test operators invoking this test type with 0 up to 14 arguments.
#undef  F
#define F(n) int operator()(C##n(P)) const;
    L14(F)

    int testFunc0() const;
        // Test methods invoking this test type with 0 up to 14 arguments.
#undef  F
#define F(n) int testFunc##n(C##n(P)) const;
    L14(F)

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
bool operator==(Bind_TestTypeAlloc const& lhs, Bind_TestTypeAlloc const& rhs);

inline
bool operator!=(Bind_TestTypeAlloc const& lhs, Bind_TestTypeAlloc const& rhs);

                        // =============================
                        // class Bind_TestFunctionsAlloc
                        // =============================

struct Bind_TestFunctionsAlloc {
    // Global versions of 'Bind_TestTypeAlloc' member functions.

    // TYPES
#undef  F
#define F(n) typedef Bind_TestArgAlloc<n> Arg##n;
    L14(F)

    // CLASS METHODS
    static int func0(Bind_TestTypeAlloc *o);
        // Invoke the corresponding method 'testFunc[0-14]' on the specified
        // 'object' with the specified arguments 'a[0-14]'.
#undef  F
#define F(n) static int func##n(Bind_TestTypeAlloc *o, C##n(P));
    L14(F)
};
}  // close package namespace

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                      // -------------------------
                      // class bdlf::Bind_TestUtil
                      // -------------------------

template <class T>
inline bool bdlf::Bind_TestUtil::isBitwiseMoveableType(const T&) {
    return bslmf::IsBitwiseMoveable<T>::VALUE;
}

                      // ------------------------------
                      // class bdlf::Bind_TestSlotsBase
                      // ------------------------------

// PRIVATE CLASS DATA
template <class VALUE>
VALUE bdlf::Bind_TestSlotsBase<VALUE>::s_slots[k_NUM_SLOTS];

namespace bdlf {

// CLASS METHODS
template <class VALUE>
inline
VALUE Bind_TestSlotsBase<VALUE>::getSlot(int index)
{
    return s_slots[index];
}

template <class VALUE>
void Bind_TestSlotsBase<VALUE>::resetSlots(VALUE value)
{
    for (int i = 0; i < k_NUM_SLOTS; ++i) {
        s_slots[i] = value;
    }
}

template <class VALUE>
inline
void Bind_TestSlotsBase<VALUE>::setSlot(VALUE value, int index)
{
    s_slots[index] = value;
}

#ifdef BSLS_PLATFORM_CMP_MSVC
#pragma warning( push )
#pragma warning( disable : 4100 ) // Verify we still need separate test paths
                                  // for MSVC
#endif

template <class VALUE>
bool Bind_TestSlotsBase<VALUE>::verifySlots(const VALUE *EXPECTED,
                                            bool         verboseFlag)
{
    bool equalFlag = true;
#if !defined(BSLS_PLATFORM_CMP_MSVC)
    // Note: the calls to 'verifyAllocSlots' are disabled on Windows.  Their
    // success depends on the "Return Value Optimization" (RVO) which Windows
    // does not seem to be applying.

    for (int i = 0; i < k_NUM_SLOTS; ++i) {
        if (EXPECTED[i] != getSlot(i)) {
            equalFlag = false;
            break;
        }
    }
#endif // !defined(BSLS_PLATFORM_CMP_MSVC)

    if (verboseFlag || !equalFlag) {
        bsl::printf("\tSlots:");
        for (int i = 0; i < k_NUM_SLOTS; ++i) {
            bsl::printf(" %d", getSlot(i));
        }
        bsl::printf("\n");
    }

    return equalFlag;
}

#ifdef BSLS_PLATFORM_CMP_MSVC
#pragma warning( pop )
#endif

                      // -------------------------
                      // class Bind_TestArgNoAlloc
                      // -------------------------

// CREATORS
template <int ID>
inline
Bind_TestArgNoAlloc<ID>::Bind_TestArgNoAlloc(int value)
: d_value(value)
{
}

// MANIPULATORS
template <int ID>
inline
Bind_TestArgNoAlloc<ID>&
Bind_TestArgNoAlloc<ID>::operator=(const Bind_TestArgNoAlloc &rhs)
{
    d_value = rhs.d_value;
    return *this;
}

// ACCESSORS
template <int ID>
inline
int Bind_TestArgNoAlloc<ID>::value() const
{
    return d_value;
}
}  // close package namespace

// FREE OPERATORS
template <int ID>
bool bdlf::operator==(Bind_TestArgNoAlloc<ID> const& lhs,
                      Bind_TestArgNoAlloc<ID> const& rhs)
{
    return lhs.value() == rhs.value();
}

template <int ID>
bool bdlf::operator!=(Bind_TestArgNoAlloc<ID> const& lhs,
                      Bind_TestArgNoAlloc<ID> const& rhs)
{
    return !(lhs == rhs);
}

namespace bdlf {
                      // --------------------------
                      // class Bind_TestTypeNoAlloc
                      // --------------------------

// CREATORS
#undef  F
#define F(n) d_a##n(a##n)
inline
Bind_TestTypeNoAlloc::Bind_TestTypeNoAlloc(C14(V))
: C14(F)
{
}

#undef  F
#define F(n) d_a##n(original.d_a##n)
inline
Bind_TestTypeNoAlloc::Bind_TestTypeNoAlloc(
                                          const Bind_TestTypeNoAlloc& original)
: C14(F)
{
}

// MANIPULATORS
inline
Bind_TestTypeNoAlloc&
Bind_TestTypeNoAlloc::operator=(const Bind_TestTypeNoAlloc& rhs)
{
#undef  F
#define F(n) d_a##n = rhs.d_a##n;
    L14(F)
    return *this;
}

// ACCESSORS
inline
int Bind_TestTypeNoAlloc::operator()() const
{
    return testFunc0();
}

#undef  F
#define F(n)                                                                  \
inline                                                                        \
int Bind_TestTypeNoAlloc::operator()(C##n(P)) const                           \
{                                                                             \
    return testFunc##n(C##n(A));                                              \
}
L14(F)

inline
int Bind_TestTypeNoAlloc::testFunc0() const
{
    return 0;
}

#undef  G
#define G(n) d_a##n = a##n; Bind_TestSlotsNoAlloc::setSlot(a##n.value(), n);
#undef  F
#define F(n)                                                                  \
inline                                                                        \
int Bind_TestTypeNoAlloc::testFunc##n(C##n(P)) const                          \
{                                                                             \
    S##n(G,)                                                                  \
    return n;                                                                 \
}
L14(F)

#undef  F
#define F(n) d_a##n.value()
inline
void Bind_TestTypeNoAlloc::print() const
{
    bsl::printf("{ %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d }\n",
                C14(F));
}
}  // close package namespace

// FREE OPERATORS
inline
bool bdlf::operator==(const Bind_TestTypeNoAlloc& lhs,
                      const Bind_TestTypeNoAlloc& rhs)
{
#undef  F
#define F(n) lhs.d_a##n.value() == rhs.d_a##n.value()
    return S14(F,&&);
}

inline
bool bdlf::operator!=(const Bind_TestTypeNoAlloc& lhs,
                      const Bind_TestTypeNoAlloc& rhs)
{
    return !(lhs == rhs);
}

namespace bdlf {
                         // -------------------------------
                         // class Bind_TestFunctionsNoAlloc
                         // -------------------------------

// CLASS METHODS
inline
int Bind_TestFunctionsNoAlloc::func0(Bind_TestTypeNoAlloc *object)
{
    return object->testFunc0();
}

#undef  F
#define F(n)                                                                  \
inline                                                                        \
int Bind_TestFunctionsNoAlloc::func##n(Bind_TestTypeNoAlloc *object, C##n(P)) \
{                                                                             \
    return object->testFunc##n(C##n(A));                                      \
}
L14(F)

}  // close package namespace

                      // -----------------------------------
                      // class bdlf::Bind_TestSlotsAllocBase
                      // -----------------------------------

// CLASS DATA
template <class AllocPtr>
AllocPtr bdlf::Bind_TestSlotsAllocBase<AllocPtr>::s_Z0 = 0;

template <class AllocPtr>
AllocPtr bdlf::Bind_TestSlotsAllocBase<AllocPtr>::s_Z1 = 0;

template <class AllocPtr>
AllocPtr bdlf::Bind_TestSlotsAllocBase<AllocPtr>::s_Z2 = 0;

namespace bdlf {

// CLASS METHODS
template <class AllocPtr>
inline
void Bind_TestSlotsAllocBase<AllocPtr>::setZ0(AllocPtr Z0)
{
    s_Z0 = Z0;
}

template <class AllocPtr>
inline
void Bind_TestSlotsAllocBase<AllocPtr>::setZ1(AllocPtr Z1)
{
    s_Z1 = Z1;
}

template <class AllocPtr>
inline
void Bind_TestSlotsAllocBase<AllocPtr>::setZ2(AllocPtr Z2)
{
    s_Z2 = Z2;
}

template <class AllocPtr>
inline
AllocPtr Bind_TestSlotsAllocBase<AllocPtr>::getZ0()
{
    return s_Z0;
}

template <class AllocPtr>
inline
AllocPtr Bind_TestSlotsAllocBase<AllocPtr>::getZ1()
{
    return s_Z1;
}

template <class AllocPtr>
inline
AllocPtr Bind_TestSlotsAllocBase<AllocPtr>::getZ2()
{
    return s_Z2;
}
}  // close package namespace

                      // -----------------------------
                      // class bdlf::Bind_TestArgAlloc
                      // -----------------------------

// CREATORS

namespace bdlf {

#if defined(BSLS_PLATFORM_CMP_IBM) && BSLS_PLATFORM_CMP_VER_MAJOR >= 0x0900
template <int ID>
Bind_TestArgAlloc<ID>::Bind_TestArgAlloc(int value)
: d_allocator_p(bslma::Default::allocator(0))
, d_value(new (*d_allocator_p) int(value))
{
}
#endif

template <int ID>
Bind_TestArgAlloc<ID>::Bind_TestArgAlloc(int               value,
                                         bslma::Allocator *allocator)
: d_allocator_p(bslma::Default::allocator(allocator))
, d_value(new (*d_allocator_p) int(value))
{
}

#if defined(BSLS_PLATFORM_CMP_IBM) && BSLS_PLATFORM_CMP_VER_MAJOR >= 0x0900
template <int ID>
Bind_TestArgAlloc<ID>::Bind_TestArgAlloc(const Bind_TestArgAlloc& original)
: d_allocator_p(bslma::Default::allocator(0))
, d_value(new (*d_allocator_p) int(original.value()))
{
}
#endif

template <int ID>
Bind_TestArgAlloc<ID>::Bind_TestArgAlloc(const Bind_TestArgAlloc&  original,
                                         bslma::Allocator         *allocator)
: d_allocator_p(bslma::Default::allocator(allocator))
, d_value(new (*d_allocator_p) int(original.value()))
{
}

template <int ID>
Bind_TestArgAlloc<ID>::~Bind_TestArgAlloc()
{
    d_allocator_p->deleteObjectRaw(d_value);
}

// MANIPULATORS
template <int ID>
Bind_TestArgAlloc<ID>&
Bind_TestArgAlloc<ID>::operator=(const Bind_TestArgAlloc& rhs)
{
    if (this != &rhs) {
        d_allocator_p->deleteObjectRaw(d_value);
        d_value = new (*d_allocator_p) int(rhs.value());
    }

    return *this;
}

// ACCESSORS
template <int ID>
const bslma::Allocator *Bind_TestArgAlloc<ID>::allocator() const
{
    return d_allocator_p;
}

template <int ID>
int Bind_TestArgAlloc<ID>::value() const
{
    return *d_value;
}

}  // close package namespace

// FREE OPERATORS
template <int ID>
inline
bool bdlf::operator==(const Bind_TestArgAlloc<ID>& lhs,
                      const Bind_TestArgAlloc<ID>& rhs)
{
    return lhs.value() == rhs.value();
}

template <int ID>
inline
bool bdlf::operator!=(const Bind_TestArgAlloc<ID>& lhs,
                      const Bind_TestArgAlloc<ID>& rhs)
{
    return !(lhs == rhs);
}

namespace bdlf {
                      // ------------------------
                      // class Bind_TestTypeAlloc
                      // ------------------------

// CREATORS

#undef  F
#define F(n) d_a##n(a##n, allocator)
inline
Bind_TestTypeAlloc::Bind_TestTypeAlloc(bslma::Allocator *allocator, C14(V))
: C14(F)
{
}

#undef  F
#define F(n) d_a##n(a##n, allocator)
inline
Bind_TestTypeAlloc::Bind_TestTypeAlloc(C14(V), bslma::Allocator *allocator)
: C14(F)
{
}

#undef  F
#define F(n) d_a##n(original.d_a##n, allocator)
inline
Bind_TestTypeAlloc::Bind_TestTypeAlloc(const Bind_TestTypeAlloc&  original,
                                       bslma::Allocator          *allocator)
: C14(F)
{
}

// MANIPULATORS

#undef  F
#define F(n) d_a##n = rhs.d_a##n;
inline
Bind_TestTypeAlloc&
Bind_TestTypeAlloc::operator=(const Bind_TestTypeAlloc& rhs)
{
    L14(F)
    return *this;
}

// ACCESSORS
inline
int Bind_TestTypeAlloc::operator()() const
{
    return testFunc0();
}

#undef  F
#define F(n)                                                                  \
inline                                                                        \
int Bind_TestTypeAlloc::operator()(C##n(P)) const                             \
{                                                                             \
    return testFunc##n(C##n(A));                                              \
}
L14(F)

inline
int Bind_TestTypeAlloc::testFunc0() const
{
    return 0;
}

#undef  G
#define G(n) d_a##n = a##n; Bind_TestSlotsAlloc::setSlot(a##n.allocator(), n);
#undef  F
#define F(n)                                                                  \
inline                                                                        \
int Bind_TestTypeAlloc::testFunc##n(C##n(P)) const                            \
{                                                                             \
    S##n(G,)                                                                  \
    return n;                                                                 \
}
L14(F)

inline
void Bind_TestTypeAlloc::setSlots()
{
#undef  F
#define F(n) Bind_TestSlotsAlloc::setSlot(d_a##n.allocator(), n);
    L14(F)
}

inline
void Bind_TestTypeAlloc::print() const
{
#undef  F
#define F(n) d_a##n.value()
    bsl::printf("{ %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d }\n",
                C14(F));
}
}  // close package namespace

// FREE OPERATORS
inline
bool bdlf::operator==(const Bind_TestTypeAlloc& lhs,
                      const Bind_TestTypeAlloc& rhs)
{
#undef  F
#define F(n) lhs.d_a##n.value()  == rhs.d_a##n.value()
    return S14(F,&&);
}

inline
bool bdlf::operator!=(const Bind_TestTypeAlloc& lhs,
                      const Bind_TestTypeAlloc& rhs)
{
    return !(lhs == rhs);
}

namespace bdlf {
                        // -----------------------------
                        // class Bind_TestFunctionsAlloc
                        // -----------------------------

inline
int Bind_TestFunctionsAlloc::func0(Bind_TestTypeAlloc *o)
{
    return o->testFunc0();
}

#undef  F
#define F(n)                                                                  \
inline                                                                        \
int Bind_TestFunctionsAlloc::func##n(Bind_TestTypeAlloc *o, C##n(P))          \
{                                                                             \
    return o->testFunc##n(C##n(A));                                           \
}
L14(F)

}  // close package namespace
}  // close enterprise namespace

#undef  S1
#undef  S2
#undef  S3
#undef  S4
#undef  S5
#undef  S6
#undef  S7
#undef  S8
#undef  S9
#undef  S10
#undef  S11
#undef  S12
#undef  S13
#undef  S14

#undef  C1
#undef  C2
#undef  C3
#undef  C4
#undef  C5
#undef  C6
#undef  C7
#undef  C8
#undef  C9
#undef  C10
#undef  C11
#undef  C12
#undef  C13
#undef  C14

#undef  L1
#undef  L2
#undef  L3
#undef  L4
#undef  L5
#undef  L6
#undef  L7
#undef  L8
#undef  L9
#undef  L10
#undef  L11
#undef  L12
#undef  L13
#undef  L14

#undef  A
#undef  F
#undef  G
#undef  P
#undef  V

#endif

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
