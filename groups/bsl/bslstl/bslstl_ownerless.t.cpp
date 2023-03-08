// bslstl_ownerless.t.cpp                                             -*-C++-*-
#include <bslstl_ownerless.h>

#include <bslstl_sharedptr.h>
#include <bslstl_map.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>
#include <bslmf_issame.h>
#include <bsls_assert.h>
#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>

#include <stdio.h>       // printf
#include <stdlib.h>      // atoi
#include <string.h>      // strcmp, strcpy

using namespace BloombergLP;

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
//   'main'.  This should be addressed as part of resolving DRQS 27411521.
//-----------------------------------------------------------------------------

// ============================================================================
//                        TEST PLAN (Additional functors)
//
// Most of these classes have trivial contracts that are almost too trivial to
// validate, such as a function-call operator to "do nothing".  The essence of
// validating these functors is that there are a valid, copy-constructible
// functor that than can be invoked with the expected arguments, and produce
// the expected observable result (if any).  In the trickier case of
// 'SharedPtrNilDeleter', it is not reasonable to check that the entire world
// has not changed, but it would be good to confirm that the object itself has
// not altered, nor the memory on the other end of the passed pointer.  The
// preferred way to do this would be to store the test object in a write-
// protected page of memory, and similarly invoke with a pointer to another
// write-protected page of memory.  Unfortunately, we do not have easy access
// to such utilities at this point in our levelized library hierarchy, so will
// settle for merely confirming that bit-patterns have not changed.
// ----------------------------------------------------------------------------
// bsl::owner_less<shared_ptr<TYPE> >
//-----------------------------------
// [ 1] bool operator()(const shared_ptr<TYPE>&, const shared_ptr<TYPE>&) const
// [ 1] bool operator()(const shared_ptr<TYPE>&, const weak_ptr<TYPE>&)   const
// [ 1] bool operator()(const weak_ptr<TYPE>&,   const shared_ptr<TYPE>&) const
//
// bsl::owner_less<weak_ptr<TYPE> >
//---------------------------------
// [ 1] bool operator()(const shared_ptr<TYPE>&, const shared_ptr<TYPE>&) const
// [ 1] bool operator()(const weak_ptr<TYPE>&,   const shared_ptr<TYPE>&) const
// [ 1] bool operator()(const weak_ptr<TYPE>&,   const weak_ptr<TYPE>&)   const
//
// bsl::owner_less<void>
//---------------------------------
// [ 1] bool operator()(const shared_ptr<T>&, const shared_ptr<U>&) const
// [ 1] bool operator()(const shared_ptr<T>&, const weak_ptr<U>&)   const
// [ 1] bool operator()(const weak_ptr<T>&,   const shared_ptr<U>&) const
// [ 1] bool operator()(const weak_ptr<T>&,   const weak_ptr<U>&)   const
// ----------------------------------------------------------------------------
// [ 4] USAGE EXAMPLE
// [ 2] TESTING TYPEDEF
// [ 3] QoI: Support for empty base optimization

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
//                      COMPILER FEATURE DETECTION MACROS
// ----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_IBM)
# define BSLSTL_OWNERLESS_WANT_RETURN_AFTER_ABORT 1
#endif

//=============================================================================
//              GLOBAL HELPER CLASSES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

                        // ===============
                        // class SimpleRep
                        // ===============

class SimpleRep : public bslma::SharedPtrRep {
    // Partially implemented shared pointer representation ("letter") protocol.
    // This class provides a reference counter and a concrete implementation of
    // the 'bdlma::Deleter' protocol that decrements the number references and
    // destroys itself if the number of references reaches zero.

    // DATA
    int *d_int_p;   // address of an externally managed 'int' that will outlive
                    // this 'SimpleRep' object.

  private:
    // NOT IMPLEMENTED
    SimpleRep(const SimpleRep&);             // = delete
    SimpleRep& operator=(const SimpleRep&);  // = delete

  public:
    // CREATORS

    SimpleRep(int *address);  // IMPLICIT
        // Construct a shared ptr rep object holding the specified 'address' of
        // an externally managed 'int' that will outlive this 'SimpleRep'
        // object.

    ~SimpleRep();
        // Destroy this test shared ptr rep object.

    // MANIPULATORS
    virtual void disposeObject();
        // This method has no effect.

    virtual void disposeRep();
        // This method has no effect.

    virtual void *getDeleter(const std::type_info&) { return 0; }
        // Return a null pointer.

    // ACCESSORS
    virtual void *originalPtr() const;
        // Return the address of the 'int' referred to by this object.

    int *ptr() const;
        // Return the address of the 'int' referred to by this object.
};

                        // ====================
                        // class SimpleRepArray
                        // ====================

class SimpleRepArray {
    // This 'class' holds three 'SimpleRep' objects that have an order in
    // memory guaranteed by the C++ standard.  This class is necessary as
    // 'SimpleRep' objects cannot be stored in an array due to their lack of a
    // copy constructor, which is a consquence of deriving from 'SharedPtrRep'.
    // To guarantee more thorough testing, we ensure that each 'SimpleRep'
    // object refers to a pointer that would sort in a different order than the
    // order of 'SimpleRep' members in memory, and the values held by those
    // 'int' objects forms a third sorted order.  This will allow confidence
    // that subsequent testing is validating the correct property when testing
    // for ordered behavior.

    // DATA
    int       d_a;
    int       d_b;
    int       d_c;

    SimpleRep d_0;
    SimpleRep d_1;
    SimpleRep d_2;

  private:
    // NOT IMPLEMENTED
    SimpleRepArray(const SimpleRepArray&);             // = delete
    SimpleRepArray& operator=(const SimpleRepArray&);  // = delete

  public:
    SimpleRepArray();
    ~SimpleRepArray();

    // MANIPULATORS
    SimpleRep& operator[](int);

    // ACCESSORS
    const SimpleRep& operator[](int) const;
};

// ============================================================================
//                  MEMBER- AND FUNCTION-TEMPLATE IMPLEMENTATIONS
// ============================================================================

                        // ---------------
                        // class SimpleRep
                        // ---------------

// CREATORS
inline
SimpleRep::SimpleRep(int *address)
: d_int_p(address)
{
}

inline
SimpleRep::~SimpleRep()
{
}

// MANIPULATORS
inline
void SimpleRep::disposeObject()
{
}

inline
void SimpleRep::disposeRep()
{
}

// ACCESSORS
inline
void *SimpleRep::originalPtr() const
{
    return static_cast<void *>(d_int_p);
}

inline
int *SimpleRep::ptr() const
{
    return d_int_p;
}

                        // --------------------
                        // class SimpleRepArray
                        // --------------------

// CREATORS
inline
SimpleRepArray::SimpleRepArray()
: d_a(3)
, d_b(1)
, d_c(2)
, d_0(&d_b)
, d_1(&d_a)
, d_2(&d_c)
{
}

inline
SimpleRepArray::~SimpleRepArray()
{
}

// MANIPULATORS
inline
SimpleRep & SimpleRepArray::operator[](int index)
{
    switch (index) {
      case 0: return d_0;
      case 1: return d_1;
      case 2: return d_2;
    };

    BSLS_ASSERT_OPT(!"Array index is out of bounds");
    // silence bad return path warning
#if !defined(BSLSTL_OWNERLESS_WANT_RETURN_AFTER_ABORT)
    abort();
#else
    struct CannotCatchMe {};
    throw CannotCatchMe();
#endif
}

// ACCESSORS
inline
const SimpleRep & SimpleRepArray::operator[](int index) const
{
    switch (index) {
      case 0: return d_0;
      case 1: return d_1;
      case 2: return d_2;
    };

    BSLS_ASSERT_OPT(!"Array index is out of bounds");
    // silence bad return path warning
#if !defined(BSLSTL_OWNERLESS_WANT_RETURN_AFTER_ABORT)
    abort();
#else
    struct CannotCatchMe {};
    throw CannotCatchMe();
#endif
}

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

    (void)veryVerbose;
    (void)veryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // Confirm no static initialization locked the global allocator
    ASSERT(&globalAllocator == bslma::Default::globalAllocator());

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    // Confirm no static intialization locked the default allocator
    ASSERT(&defaultAllocator == bslma::Default::defaultAllocator());

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of 'owner_less<void>'
/// - - - - - - - - - - - - - - - - - - - - -
// Suppose we need a map accepting shared pointers as keys.  We also expect
// that this container will be accessible from multiple threads and some of
// them will store weak versions of smart pointers to break reference cycles.
// To avoid excessive conversions we can use a transparent comparator to
// enable heterogeneous lookup with 'bsl::weak_ptr' objects as parameters for
// search functions.
//
// First, we create a container and populate it:
//..
        typedef bsl::map<bsl::shared_ptr<int>, int, bsl::owner_less<void> >
                                                                           Map;
        Map                  container;

        bsl::shared_ptr<int> sharedPtr1 = bsl::make_shared<int>(1);
        bsl::shared_ptr<int> sharedPtr2 = bsl::make_shared<int>(2);
        bsl::weak_ptr<int>   weakPtr1(sharedPtr1);

        container[sharedPtr1] = 1;
        container[sharedPtr2] = 2;
//..
// Now, we make sure, that shared pointers can be used to perform lookup:
//..
        Map::const_iterator iter = container.find(sharedPtr1);
        ASSERT(container.end() != iter        );
        ASSERT(1               == iter->second);

        iter = container.find(sharedPtr2);
        ASSERT(container.end() != iter);
        ASSERT(2               == iter->second);
//..
// Finally, we simulate the situation of accessing the container from another
// thread and perform lookup using weak pointers:
//..
        iter = container.find(weakPtr1);
        ASSERT(container.end() != iter        );
        ASSERT(1               == iter->second);

        bsl::weak_ptr<int> weakPtr3(bsl::make_shared<int>(3));
        iter = container.find(weakPtr3);
        ASSERT(container.end() == iter);
//..
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING QOI: 'owner_less' IS AN EMPTY TYPE
        //   As a quality of implementation issue, the class has no state and
        //   should support the use of the empty base class optimization on
        //   compilers that support it.
        //
        // Concerns:
        //: 1 Class 'bsl::owner_less' does not increase the size of an object
        //:   when used as a base class.
        //:
        //: 2 Object of 'bsl::owner_less' class increases size of an object
        //:   when used as a class member.
        //
        // Plan:
        //: 1 Define two identical non-empty classes with no padding, but
        //:   derive one of them from 'bsl::owner_less', then assert that both
        //:   classes have the same size. (C-1)
        //:
        //: 2 Create a non-empty class with an 'bsl::owner_less' additional
        //:   member, assert that class size is larger than sum of other data
        //:   member's sizes. (C-2)
        //
        // Testing:
        //   QoI: Support for empty base optimization
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING QOI: 'owner_less' IS AN EMPTY TYPE"
                            "\n==========================================\n");

        typedef bsl::owner_less<bsl::shared_ptr<int> > SharedObj;
        typedef bsl::owner_less<bsl::weak_ptr<int> >   WeakObj;
        typedef bsl::owner_less<void>                  VoidObj;

        struct TwoInts {
            int d_a;
            int d_b;
        };

        struct DerivedIntsShared : SharedObj {
            int d_a;
            int d_b;
        };

        struct DerivedIntsWeak : WeakObj {
            int d_a;
            int d_b;
        };

        struct DerivedIntsVoid : VoidObj {
            int d_a;
            int d_b;
        };

        struct IntWithMemberShared {
            SharedObj d_dummy;
            int       d_a;
        };

        struct IntWithMemberWeak {
            WeakObj d_dummy;
            int     d_a;
        };

        struct IntWithMemberVoid {
            VoidObj d_dummy;
            int     d_a;
        };

        ASSERT(sizeof(TwoInts) == sizeof(DerivedIntsShared));
        ASSERT(sizeof(TwoInts) == sizeof(DerivedIntsWeak  ));
        ASSERT(sizeof(TwoInts) == sizeof(DerivedIntsVoid  ));

        ASSERT(sizeof(int)     <  sizeof(IntWithMemberShared));
        ASSERT(sizeof(int)     <  sizeof(IntWithMemberWeak  ));
        ASSERT(sizeof(int)     <  sizeof(IntWithMemberVoid  ));

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING TYPEDEF
        //   Comparator's transparency is determined by the presence of the
        //   'is_transparent' type.  We need to verify that the class offers
        //   the required typedef.
        //
        // Concerns:
        //: 1 The type 'is_transparent' is defined in 'bsl::owner_less<void>',
        //:   publicly accessible and an alias for 'void'.
        //
        // Plan:
        //: 1 ASSERT the typedef aliases the correct type using
        //    'bsl::is_same'. (C-1)
        //
        // Testing:
        //  TESTING TYPEDEF
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TYPEDEF"
                            "\n===============\n");

        ASSERT((bsl::is_same<void,
                             bsl::owner_less<void>::is_transparent>::value));

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TEST 'owner_less' FUNCTOR
        //   'owner_less' is an empty POD type with implicitly defined special
        //   member functions and a 'const'-qualified function call operator.
        //
        // Concerns:
        //:  1 'owner_less' can be value-initialized.
        //:  2 'owner_less' can be copy-initialized.
        //:  3 'owner_less' can be copy-assigned.
        //:  4 'owner_less<>' is the same type as 'owner_less<void>'.
        //:  5 'owner_less<shared_ptr<T> >' has overloads for the function
        //:    call operator that can take two 'shared_ptr<T>' objects by
        //:    reference, or a 'shared_ptr<T>' and a 'weak_ptr<T>', both by
        //:    reference, and passed in either order.
        //:  6 'owner_less<weak_ptr<T> >' has overloads for the function
        //:    call operator that can take two 'weak_ptr<T>' objects by
        //:    reference, or a 'shared_ptr<T>' and a 'weak_ptr<T>', both by
        //:    reference, and passed in either order.
        //:  7 'owner_less<>' has overloads for the function call operator that
        //:    can take two any combination of 'shared_ptr' and 'weak_ptr'
        //:    objects by reference, and passed in either order.
        //:  8 The overloaded function call operator for all 'owner_less'
        //:    templates returns 'true' if the 'rep' held by the first argument
        //:    has a lower address thna the 'rep' held by the second argument,
        //:    and 'false' otherwise.
        //:  9 QoI: No operations on 'owner_less' objects allocate any memory.
        //
        // Plan:
        //   Create two shared pointer representation objects, with a known
        //   relationship between their addresses.  Then create shared and weak
        //   ptr objects from these representations, and confirm the correct
        //   runtime behavior when invoking the function call operator of the
        //   'owner_less' functor.
        //
        // Testing:
        //  bsl::owner_less<shared_ptr<T> >::
        //   bool operator()(const shared_ptr<T>&, const shared_ptr<T>&) const;
        //   bool operator()(const shared_ptr<T>&, const weak_ptr<T>&)   const;
        //   bool operator()(const weak_ptr<T>&,   const shared_ptr<T>&) const;
        //
        //  bsl::owner_less<weak_ptr<TYPE> >::
        //   bool operator()(const shared_ptr<T>&, const shared_ptr<T>&) const;
        //   bool operator()(const weak_ptr<T>&,   const shared_ptr<T>&) const;
        //   bool operator()(const weak_ptr<T>&,   const weak_ptr<T>&)   const;
        //
        //  bsl::owner_less<>::
        //   bool operator()(const shared_ptr<T>&, const shared_ptr<U>&) const;
        //   bool operator()(const shared_ptr<T>&, const weak_ptr<U>&)   const;
        //   bool operator()(const weak_ptr<T>&,   const shared_ptr<U>&) const;
        //   bool operator()(const weak_ptr<T>&,   const weak_ptr<U>&)   const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'owner_less' FUNCTOR"
                            "\n============================\n");

        {
            typedef bsl::owner_less<bsl::shared_ptr<int> > OwnerShared;
            typedef bsl::owner_less<bsl::weak_ptr<int> >   OwnerWeak;
            typedef bsl::owner_less<void>                  OwnerVoid;
            typedef bsl::owner_less<>                      OwnerDiamond;

            ASSERT((bsl::is_same<OwnerVoid, OwnerDiamond>::value));

            SimpleRepArray       reps;
            bslma::SharedPtrRep *repPtrs[3] = {
                &reps[0],
                &reps[1],
                &reps[2]
            };

            // Create tables of shared and weak pointers to 'int'
            const bsl::shared_ptr<int> SHARED_PTRS[4] = {
                bsl::shared_ptr<int>(),       // null pointers order before all
                bsl::shared_ptr<int>(reps[0].ptr(), repPtrs[0]),
                bsl::shared_ptr<int>(reps[1].ptr(), repPtrs[1]),
                bsl::shared_ptr<int>(reps[2].ptr(), repPtrs[2])
            };

            const bsl::weak_ptr<int> WEAK_PTRS[4] = {
                bsl::weak_ptr<int>(),         // null pointers order before all
                SHARED_PTRS[1],
                SHARED_PTRS[2],
                SHARED_PTRS[3]
            };

            // Create tables of shared and weak pointers to 'void' for testing
            // heterogeneous comparison.

            double a, b, c;  // dummy data for the 'void' pointers to point to.

            const bsl::shared_ptr<void> ALIAS_PTRS[4] = {
                bsl::shared_ptr<void>(SHARED_PTRS[0],  0),
                bsl::shared_ptr<void>(SHARED_PTRS[1], &c),
                bsl::shared_ptr<void>(SHARED_PTRS[2], &a),
                bsl::shared_ptr<void>(SHARED_PTRS[3], &b)
            };

            const bsl::weak_ptr<void> WEAK_ALIAS[4] = {
                ALIAS_PTRS[0],
                ALIAS_PTRS[1],
                ALIAS_PTRS[2],
                ALIAS_PTRS[3]
            };

            // Use aggregate initialization as 'owner_less' is a POD type, so
            // the default constructor is not called to initialize each object.

            OwnerShared  ownerShared  = {};
            OwnerWeak    ownerWeak    = {};
            OwnerDiamond ownerDiamond = {};

            // Make a 'const' copy to validate the copy constructor, and that
            // the function call operator tested below is 'const' qualified.

            const OwnerShared  OS = ownerShared;
            const OwnerWeak    OW = ownerWeak;
            const OwnerDiamond OD = ownerDiamond;

            // Confirm that we can assign the 'const' copy back to the original
            // object.  As this should be a null operation, rather is nothing
            // to test but the syntax.

            ownerShared  = OS;
            ownerWeak    = OW;
            ownerDiamond = OD;

            // Nested loop through all ordering of values in the data tables,
            // to confirm that 'owner_less' returns the correct ordering in
            // each case, which is designed to be the same as the ordering of
            // the loop indices.

            for (int i = 0; i != 4; ++i) {
                for (int j = 0; j != 4; ++j) {
                  ASSERTV(i, j, OS(SHARED_PTRS[i], SHARED_PTRS[j]) == (i < j));
                  ASSERTV(i, j, OD(SHARED_PTRS[i], SHARED_PTRS[j]) == (i < j));

                  ASSERTV(i, j, OS(SHARED_PTRS[i], WEAK_PTRS[j])   == (i < j));
                  ASSERTV(i, j, OW(SHARED_PTRS[i], WEAK_PTRS[j])   == (i < j));
                  ASSERTV(i, j, OD(SHARED_PTRS[i], WEAK_PTRS[j])   == (i < j));

                  ASSERTV(i, j, OS(WEAK_PTRS[i],   SHARED_PTRS[j]) == (i < j));
                  ASSERTV(i, j, OW(WEAK_PTRS[i],   SHARED_PTRS[j]) == (i < j));
                  ASSERTV(i, j, OD(WEAK_PTRS[i],   SHARED_PTRS[j]) == (i < j));

                  ASSERTV(i, j, OW(WEAK_PTRS[i],   WEAK_PTRS[j])   == (i < j));
                  ASSERTV(i, j, OD(WEAK_PTRS[i],   WEAK_PTRS[j])   == (i < j));

                  // Confirm aliases behave correctly

                  ASSERTV(i, j, OD(ALIAS_PTRS[i],  ALIAS_PTRS[j])  == (i < j));
                  ASSERTV(i, j, OD(ALIAS_PTRS[i],  WEAK_ALIAS[j])  == (i < j));
                  ASSERTV(i, j, OD(WEAK_ALIAS[i],  ALIAS_PTRS[j])  == (i < j));
                  ASSERTV(i, j, OD(WEAK_ALIAS[i],  WEAK_ALIAS[j])  == (i < j));

                  // Confirm heterogeneous support for 'owner_less<>'

                  ASSERTV(i, j, OD(SHARED_PTRS[i], ALIAS_PTRS[j])  == (i < j));
                  ASSERTV(i, j, OD(SHARED_PTRS[i], WEAK_ALIAS[j])  == (i < j));
                  ASSERTV(i, j, OD(WEAK_PTRS[i],   ALIAS_PTRS[j])  == (i < j));
                  ASSERTV(i, j, OD(WEAK_PTRS[i],   WEAK_ALIAS[j])  == (i < j));

                  ASSERTV(i, j, OD(ALIAS_PTRS[i],  SHARED_PTRS[j]) == (i < j));
                  ASSERTV(i, j, OD(ALIAS_PTRS[i],  WEAK_PTRS[j])   == (i < j));
                  ASSERTV(i, j, OD(WEAK_ALIAS[i],  SHARED_PTRS[j]) == (i < j));
                  ASSERTV(i, j, OD(WEAK_ALIAS[i],  WEAK_PTRS[j])   == (i < j));
                }
            }

            // CONCERN: No operations in this test case should allocate memory.

            ASSERTV(defaultAllocator.numBlocksTotal(),
                    0 == defaultAllocator.numBlocksTotal());
        }
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
