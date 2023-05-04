// bslma_aamodel.t.cpp                                                -*-C++-*-

#include <bslma_aamodel.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bslmf_enableif.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bslmf_isconvertible.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <cstdio>   // 'printf'
#include <cstdlib>  // 'atoi'

#ifdef BDE_VERIFY
// Suppress some pedantic bde_verify checks in this test driver
#pragma bde_verify -AC02   // Implicit copy constructor is not allocator-aware
#pragma bde_verify -FD01   // Function declaration requires contract
#pragma bde_verify -MN01   // Class data members must be private
#pragma bde_verify -FABC01 // Function not in alphabetical order
#endif

using std::printf;
using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// This component defines two template metafunctions.  The tests instantiate
// the metafunctions on a variety of types, verifying that they produce the
// expected result type.
// ----------------------------------------------------------------------------
// [ 3] AAModel<TYPE>
// [ 2] AAModelIsSupported<TYPE, MODEL>
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] USAGE EXAMPLES
// ----------------------------------------------------------------------------

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
//                  SEMI-STANDARD NEGATIVE-TESTING MACROS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                                VERBOSITY
// ----------------------------------------------------------------------------

int verbose = 0;
int veryVerbose = 0;
int veryVeryVerbose = 0;
int veryVeryVeryVerbose = 0; // For test allocators

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

struct NonAAClass { };

struct LegacyAAClass {
    // Class meeting minimal requirement for detection as *legacy-AA*.
    BSLMF_NESTED_TRAIT_DECLARATION(LegacyAAClass, bslma::UsesBslmaAllocator);
};

struct BslAAClass {
    // Class meeting minimal requirement for detection as *bsl-AA*.

#ifndef BSLMA_USESBSLMAALLOCATOR_AUTODETECT_ALLOCATOR_TYPE
    BSLMF_NESTED_TRAIT_DECLARATION(BslAAClass, bslma::UsesBslmaAllocator);
#endif

    typedef bsl::allocator<int> allocator_type;
};

// FUTURE:
// struct PmrAAClass {
//     // Class meeting minimal requirement for detection as *pmr-AA*.
//     typedef bsl::polymorphic_allocator<int> allocator_type;
// };

struct StlAAClass {
    // Class meeting minimal requirement for detection as *stl-AA*.
    struct allocator_type { };
};

// These 'structs' would not meet the requirements for an AA type except that
// the corresponding trait is explicitly specialized below.
struct LegacyAAByTrait { };
struct BslAAByTrait { };
struct PmrAAByTrait { };
struct StlAAByTrait { };

}  // close unnamed namespace

namespace BloombergLP {
namespace bslma {

template <>
struct AAModelIsSupported<LegacyAAByTrait, AAModelLegacy> : bsl::true_type {};

template <>
struct AAModelIsSupported<BslAAByTrait   , AAModelBsl   > : bsl::true_type {};

template <>
struct AAModelIsSupported<PmrAAByTrait   , AAModelPmr   > : bsl::true_type {};

template <>
struct AAModelIsSupported<StlAAByTrait   , AAModelStl   > : bsl::true_type {};

}  // close package namespace
}  // close enterprise namespace

namespace {

template <class PRED>
inline
typename bsl::enable_if<
    bsl::is_convertible<PRED *,
                        bsl::integral_constant<bool, PRED::value> *>::value,
        bool>::type
predVal(PRED)
    // Return 'true' if 'PRED' is 'true_type' and 'false' if 'PRED' is
    // 'false_type'.  This function does not participate in overload resolution
    // unless its argument is (or is derived from) a Boolean specialization of
    // 'integral_constant'.
{
    return PRED::value;
}

template <class DERIVED, class BASE>
inline bool isDerivedFrom()
    // Return 'true' if 'DERIVED' is the same or derived from 'BASE'.
{
    return bsl::is_convertible<DERIVED *, BASE *>::value;
}

}  // close unnamed namespace

// ============================================================================
//                             USAGE EXAMPLES
// ----------------------------------------------------------------------------

namespace {

///Example 1: Conditionally Passing an Allocator to a Constructor
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates the use of 'AAModelIsSupported' to choose an
// appropriate overload for AA constructors.  Consider a *bsl-AA* class,
// 'Wrapper', that wraps an object of template-parameter type, 'TYPE'.  First,
// we define the data members:
//..
    template <class TYPE>
    class Wrapper {
        // Wrap an object of type 'TYPE'.

        // DATA
        bsl::allocator<char> d_allocator;
        TYPE                 d_object;

      public:
        // TYPES
        typedef bsl::allocator<char> allocator_type;
//..
// Next, we define the constructors.  The constructors for 'Wrapper' would all
// take an optional 'allocator_type' argument, but the 'd_object' member might
// or might not be constructed with an allocator argument.  To handle the
// allocator correctly, therefore, we choose to have two versions of each
// constructor: one that is invoked if 'TYPE' is AA and one that is invoked if
// it is not.  Since both constructors have the same argument list, we must
// make them templates and distinguish them using SFINAE so that only one
// instantiation is valid, i.e., by using 'enable_if' along with the
// 'AAModelIsSupported':
//..
        // CREATORS
        template <class ALLOC>
        explicit
        Wrapper(const ALLOC& a,
                typename bsl::enable_if<
                   bsl::is_convertible<ALLOC,allocator_type>::value
                && bslma::AAModelIsSupported<TYPE,bslma::AAModelLegacy>::value,
                    int
                >::type = 0)
            // Construct a 'Wrapper' using the specified 'a' allocator, passing
            // the allocator to the wrapped object.  This constructor will not
            // participate in overload resolution unless 'TYPE' supports the
            // legacy allocator-awareness model (*legacy-AA*).
            : d_allocator(a), d_object(d_allocator.mechanism()) { }

        template <class ALLOC>
        explicit
        Wrapper(const ALLOC& a,
                typename bsl::enable_if<
                   bsl::is_convertible<ALLOC,allocator_type>::value
               && !bslma::AAModelIsSupported<TYPE,bslma::AAModelLegacy>::value,
                   int
                >::type = 0)
            // Construct a 'Wrapper' using the specified 'a' allocator,
            // constructing the wrapped object without an explicit allocator.
            // This constructor will not participate in overload resolution if
            // 'TYPE' supports the legacy allocator-awareness model
            // (*legacy-AA*).
            : d_allocator(a), d_object() { }
//..
// Support for *bsl-AA* implies support for *legacy-AA*, so the example above
// needs to test for only the latter model; the first constructor overload is
// selected if 'TYPE' implements either AA model.  Similarly
// 'd_allocator.mechanism()' yields a common denominator type,
// 'bslma::Allocator *' that can be passed to the constructor for 'd_object',
// regardless of its preferred AA model.  The second overload is selected for
// types that do *not* support the *legacy-AA* (or *bsl-AA*) model.  Note that
// this example, though functional, does not handle all cases; e.g., it does
// not handle types whose allocator constructor parameter is preceded by
// 'bsl::allocator_arg_t'.  See higher-level components such as
// 'bslma_contructionutil' for a more comprehensive treatment of AA constructor
// variations.
//
// Next, we finish up our class by creating accessors to get the allocator and
// wrapped object:
//..
        // ACCESSORS
        const allocator_type get_allocator() const { return d_allocator; }
            // Return the allocator used to construct this object.

        const TYPE& value() const { return d_object; }
    };
//..
// Now, to see the effect of these constructors, we'll use a simple AA class,
// 'SampleAAType' that does nothing more than hold the allocator:
//..
    class SampleAAType {
        // Sample AA class that adheres to the bsl-AA interface.

        // DATA
        bsl::allocator<char> d_allocator;

      public:
#ifndef BSLMA_USESBSLMAALLOCATOR_AUTODETECT_ALLOCATOR_TYPE
        // TRAITS
        BSLMF_NESTED_TRAIT_DECLARATION(SampleAAType,
                                       bslma::UsesBslmaAllocator);
#endif

        // TYPES
        typedef bsl::allocator<char> allocator_type;

        // CREATORS
        explicit SampleAAType(const allocator_type& alloc = allocator_type())
            : d_allocator(alloc) { }
        SampleAAType(const SampleAAType&) { }

        // MANIPULATORS
        SampleAAType& operator=(const SampleAAType&) { return *this; }

        // ACCESSORS
        allocator_type get_allocator() const { return d_allocator; }
    };
//..
// Finally, in our main program, create an allocator and pass it to a couple of
// 'Wrapper' objects, one instantiated with 'int' and the other instantiated
// with our 'SampleAAType'.  We verify that both were constructed
// appropriately, with the allocator being used by the 'SampleAAType' object,
// as desired:
//..
    void usageExample1()
    {
        bslma::TestAllocator alloc;

        Wrapper<int> w1(&alloc);
        ASSERT(&alloc == w1.get_allocator());
        ASSERT(0 == w1.value());

        Wrapper<SampleAAType> w2(&alloc);
        ASSERT(&alloc == w2.get_allocator());
        ASSERT(&alloc == w2.value().get_allocator());
    }
//..
// Note that, even though 'SampleAAType' conforms to the *bsl-AA* interface, it
// is also supports the *legacy-AA* model because 'bslma::Allocator *' is
// convertible to 'bsl::allocator'.
//
///Example 2: Choose an Implementation Based on Allocator-Aware (AA) model
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates the use of 'AAModel' to dispatch among several
// implementations based on the AA model preferred by a parameter type.  We
// would like a uniform way to get the allocator used by an object.  We'll
// define a utility class, 'Util', containing a static member function
// template, 'getAllocator(const TYPE& obj)' returning a 'bsl::allocator<char>'
// as follows:
//
//: o If 'TYPE' is *bsl-AA*, return 'obj.get_allocator()'.
//: o If 'TYPE' is *legacy-AA*, return 'bsl::allocator<char>(obj.allocator())'.
//: o If 'TYPE' is not AA, return 'bsl::allocator<char>()'.
//: o If 'TYPE' is AA but not one of the above, compilation will fail.
//
// We'll use 'AAModel<TYPE>' to dispatch to one of three implementations of
// 'getAllocator'.
//
// First, we declare the 'Util' class and three private overloaded
// implemention functions, each taking an argument of a different AA model
// tag:
//..
    class Util {
        // Namespace for functions that operate on AA types.

        template <class TYPE>
        static bsl::allocator<char> getAllocatorImp(const TYPE& obj,
                                                    bslma::AAModelBsl)
            { return obj.get_allocator(); }
        template <class TYPE>
        static bsl::allocator<char> getAllocatorImp(const TYPE& obj,
                                                    bslma::AAModelLegacy)
            { return obj.allocator(); }
        template <class TYPE>
        static bsl::allocator<char> getAllocatorImp(const TYPE&,
                                                    bslma::AAModelNone)
            { return bsl::allocator<char>(); }
//..
// Next, we dispatch to one of the implementation functions using
// 'AAModel<TYPE>' to yield a tag that indicates the AA model used by 'TYPE'.
//..

      public:
        // CLASS METHODS
        template <class TYPE>
        static bsl::allocator<char> getAllocator(const TYPE& obj)
            { return getAllocatorImp(obj, bslma::AAModel<TYPE>()); }
    };
//..
// Now, to check all of the possibilities, we create a minimal AA type sporting
// the *legacy-AA* interface:
//..
    class SampleLegacyAAType {
        // Sample AA class that adheres to the bsl-AA interface.

        // DATA
        bslma::Allocator *d_allocator_p;

    public:
        // TRAITS
        BSLMF_NESTED_TRAIT_DECLARATION(SampleLegacyAAType,
                                       bslma::UsesBslmaAllocator);

        // CREATORS
        explicit SampleLegacyAAType(bslma::Allocator *a =0)
            : d_allocator_p(a) { }

        // ACCESSORS
        bslma::Allocator *allocator() const { return d_allocator_p; }
    };
//..
// Finally, we create objects of 'SampleAAType' and 'SampleLegacyAAType' using
// different allocators as well as an object of type 'float' (which, of course
// is not AA), and verify that 'Util::getAllocator' returns the correct
// allocator for each.
//..
    void usageExample2()
    {
        bslma::TestAllocator ta1, ta2;

        SampleAAType       obj1(&ta1);
        SampleLegacyAAType obj2(&ta2);
        float              obj3 = 0.0;

        ASSERT(Util::getAllocator(obj1) == &ta1);
        ASSERT(Util::getAllocator(obj2) == &ta2);
        ASSERT(Util::getAllocator(obj3) == bslma::Default::defaultAllocator());
    }

}  // close unnamed namespace

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? std::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLES
        //
        // Concerns:
        //: 1 That the usage examples shown in the component-level
        //:   documentation compile and run as described.
        //
        // Plan:
        //: 1 Copy the usage examples from the component header, changing
        //:   'assert' to 'ASSERT' and execute them.
        //
        // Testing:
        //     USAGE EXAMPLES
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLES"
                            "\n==============\n");

        usageExample1();
        usageExample2();

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'AAModel'
        //
        // Concerns:
        //: 1 For each 'TYPE', 'AAModel<TYPE>' is derived from exactly one of
        //:   the five AA *model tags**.  If a 'TYPE' supports more than one AA
        //:   model, the returned tag will be the first supported of
        //:   'AAModelPmr', 'AAModelBsl', 'AAModelLegacy', 'AAModelStl', and
        //:   'AAModelNone'.
        //: 2 If 'AAModelIsSupported' is explicitly specialized to 'true' for a
        //:   specific 'TYPE' and 'MODEL', then 'AAModel' behaves as though
        //:   'TYPE' supports that 'MODEL', even such support would not
        //:   otherwise be deduceable.
        //
        // Plan:
        //: 1 Define a set of classes with interfaces supporting each of the
        //:   models (including non-AA interfaces).  Verify that
        //:   'AAModel' is derived the expected *model tag* struct.  (C-1)
        //: 2 Create types that do not meet the interface requirements for
        //:   being AA but which are declared as being AA through
        //:   specialization of 'AAModelIsSupported' for one of the AA *model
        //:   tags*.  Add these types to the list of types tested in step 1 and
        //:   ensure that the deduced results are as expected.  (C-2)
        //
        // Testing:
        //     AAModel<TYPE>
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'AAModel'"
                            "\n=================\n");

        using namespace bslma;

#define TEST(TYPE, EXP_MODEL) \
        ASSERT((isDerivedFrom<AAModel<TYPE>, EXP_MODEL>()))

        //   TYPE             Expected Model
        //   ===============  ==============
        TEST(int            , AAModelNone  );
        TEST(NonAAClass     , AAModelNone  );
        TEST(LegacyAAClass  , AAModelLegacy);
        TEST(BslAAClass     , AAModelBsl   );
//      TEST(PmrAAClass     , AAModelPmr   );
        TEST(StlAAClass     , AAModelStl   );
        TEST(LegacyAAByTrait, AAModelLegacy);
        TEST(BslAAByTrait   , AAModelBsl   );
        TEST(PmrAAByTrait   , AAModelPmr   );
        TEST(StlAAByTrait   , AAModelStl   );

#undef TEST

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'AAModelIsSupported'
        //
        // Concerns:
        //: 1 For each model, 'AAModelIsSupported<TYPE, MODEL>' is derived from
        //:   'bsl::true_type' if 'TYPE' supports the specified 'MODEL';
        //:   otherwise it is derived from 'bsl::false_type'.
        //: 2 If, for a given 'TYPE', 'UsesBslmaAllocator<TYPE>::value' is
        //:   'true', then 'AAModelIsSupported<TYPE, AAModelLegacy>' is derived
        //:   from 'bsl::true_type' (but not necessarily vice-versa).
        //: 3 If, for a given 'TYPE', 'TYPE::allocator_type' exists, then
        //:   'AAModelIsSupported<TYPE, MODEL>' is 'true_type' for one or more
        //:   of 'AAModelBsl', 'AAModelPmr', 'AAModelStl', depending on which
        //:   allocator types are convertible to 'allocator_type'.
        //: 4 If 'AAModelIsSupported<TYPE, AAModelPmr>::value' is 'true', then
        //:   'AAModelIsSupported<TYPE, AAModelBsl>' is also 'true'.  If
        //:   'AAModelIsSupported<TYPE, AAModelBsl>::value' is 'true', then
        //:   'AAModelIsSupported<TYPE, AAModelLegacy>' is also 'true'.  If
        //:   either 'AAModelIsSupported<TYPE, AAModelPmr>::value' or
        //:   'AAModelIsSupported<TYPE, AAModelBsl>' is 'true', then
        //:   'AAModelIsSupported<TYPE, AAModelStl>' is also true.
        //: 5 Concern 4 applies even for 'TYPE's for which 'AAModelIsSupported'
        //:   is explicitly specialized to 'true' for a specific 'MODEL' (but
        //:   not specialized to 'false' for one of the otherwise-deduced cases
        //:   described in concern 4), even if 'TYPE' would otherwise not be
        //:   deduced as supporting that 'MODEL'.
        //: 6 'AAModelIsSupported<TYPE, AAModelNone>' is always true (unless
        //:   explicitly specialized to false, which should never be done).
        //
        // Plan:
        //: 1 Define a set of classes with interfaces supporting each of the
        //:   models (including non-AA interfaces).  Verify that
        //:   'AAModelIsSupported' is derived from either 'bsl::true_type' or
        //:   'bsl::false_type' and that it's 'value' member is correct when
        //:   specialized with each type and each of the five *model
        //:   tags*.  (C-1)
        //: 2 Among the types tested in step 1, include types that support the
        //:   *legacy-AA* interface by virtual of having the
        //:   'bslma::UsesBslmaAllocator' trait.  (C-2)
        //: 3 Among the types tested in step 1, include types that have member
        //:   type 'allocator_type' convertible from 'bsl::allocator'
        //:   (*bsl-AA*), convertible from 'bsl::polymorphic_allocator'
        //:   (*pmr-AA*), or not convertible from either (*stl-AA*).  (C-3)
        //: 4 In step 1, ensure that the expected results are consistant with
        //:   the deduction rules (i.e., *bsl-AA* is a superset of
        //:   *legacy-AA*).  (C-4)
        //: 5 Create types that do not meet the interface requirements for
        //:   being AA but which are declared as being AA through
        //:   specialization of 'AAModelIsSupported' for one of the AA *model
        //:   tags*.  Add these types to the list of types tested in steps 1
        //:   and 4 and ensure that the deduced results are as expected.  (C-5)
        //: 6 Verify that 'AAModelIsSupported<TYPE, AAModelNone>' is true for
        //:   all types used in this test case.  (C-6)
        //
        // Testing:
        //     AAModelIsSupported<TYPE, MODEL>
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'AAModelIsSupported'"
                            "\n===========================\n");

        using namespace bslma;

#define TEST(T, NONE, LEGACY, BSL, PMR, STL) do {                          \
        ASSERT(NONE   == predVal(AAModelIsSupported<T, AAModelNone  >())); \
        ASSERT(LEGACY == predVal(AAModelIsSupported<T, AAModelLegacy>())); \
        ASSERT(BSL    == predVal(AAModelIsSupported<T, AAModelBsl   >())); \
        ASSERT(PMR    == predVal(AAModelIsSupported<T, AAModelPmr   >())); \
        ASSERT(STL    == predVal(AAModelIsSupported<T, AAModelStl   >())); \
    } while (false)

        //   TYPE             None, Legacy  Bsl    Pmr    Stl
        //   ===============  ====  ====== =====  =====  =====
        TEST(int            , true, false, false, false, false);
        TEST(NonAAClass     , true, false, false, false, false);
        TEST(LegacyAAClass  , true, true , false, false, false);
        TEST(BslAAClass     , true, true , true , false, true );
//      TEST(PmrAAClass     , true, true , true , true , true );
        TEST(StlAAClass     , true, false, false, false, true );
        TEST(LegacyAAByTrait, true, true , false, false, false);
        TEST(BslAAByTrait   , true, true , true , false, true );
        TEST(PmrAAByTrait   , true, true , true , true , true );
        TEST(StlAAByTrait   , true, false, false, false, true );

#undef TEST

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Execute each methods to verify functionality for simple case.
        //
        // Testing:
        //      BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        using namespace bslma;

        ASSERT(AAModelNone::value   == AAModel<int>::value);
        ASSERT(AAModelNone::value   == AAModel<NonAAClass>::value);
        ASSERT(AAModelLegacy::value == AAModel<LegacyAAClass>::value);
        ASSERT(AAModelBsl::value    == AAModel<BslAAClass>::value);
//        ASSERT(AAModelPmr::value    == AAModel<PmrAAClass>::value);
        ASSERT(AAModelStl::value    == AAModel<StlAAClass>::value);

        ASSERT((  AAModelIsSupported<int          , AAModelNone>::value));
        ASSERT((! AAModelIsSupported<int          , AAModelLegacy>::value));
        ASSERT((  AAModelIsSupported<NonAAClass   , AAModelNone>::value));
        ASSERT((! AAModelIsSupported<NonAAClass   , AAModelLegacy>::value));

        ASSERT((  AAModelIsSupported<LegacyAAClass, AAModelNone>::value));
        ASSERT((  AAModelIsSupported<LegacyAAClass, AAModelLegacy>::value));
        ASSERT((! AAModelIsSupported<LegacyAAClass, AAModelBsl>::value));
        ASSERT((! AAModelIsSupported<LegacyAAClass, AAModelStl>::value));

        ASSERT((  AAModelIsSupported<BslAAClass   , AAModelLegacy>::value));
        ASSERT((  AAModelIsSupported<BslAAClass   , AAModelBsl>::value));
        ASSERT((  AAModelIsSupported<BslAAClass   , AAModelStl>::value));

        ASSERT((  AAModelIsSupported<StlAAClass   , AAModelNone>::value));
        ASSERT((! AAModelIsSupported<StlAAClass   , AAModelBsl>::value));
        ASSERT((  AAModelIsSupported<StlAAClass   , AAModelStl>::value));

      } break;

      default: {
        std::fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        std::fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
