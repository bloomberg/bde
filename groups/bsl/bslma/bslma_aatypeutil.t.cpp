// bslma_aatypeutil.t.cpp                                             -*-C++-*-

#include <bslma_aatypeutil.h>

#include <bslma_testallocator.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_objectbuffer.h>

#include <cstdio>   // 'printf'
#include <cstdlib>  // 'atoi'

#ifdef BDE_VERIFY
// Suppress some pedantic bde_verify checks in this test driver
#pragma bde_verify -AC02   // Implicit copy constructor is not allocator-aware
#pragma bde_verify -AL01   // Class needs allocator() method
#pragma bde_verify -AP02   // Class needs d_allocator_p member
#pragma bde_verify -AT02   // Class does not have an allocator trait
#pragma bde_verify -FABC01 // Function not in alphabetical order
#pragma bde_verify -FD01   // Function declaration requires contract
#pragma bde_verify -IND01  // Possibly mis-indented line
#pragma bde_verify -MN01   // Class data members must be private
#endif

using std::printf;
using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// This component provides a utility struct, 'bslma::AATypeUtil' containing a
// number of static member functions to extract and return an allocator from an
// object.  The different functions differ in their return types and in the set
// of allocator-awareness (AA) models that they support.  Each test case,
// therefore, involves creating objects having AA models and invoking the
// function under test for each object, verifying that the return is of the
// right type and has the correct value.
// ----------------------------------------------------------------------------
// [ 4] bsl::allocator<char> getBslAllocator(const TYPE&)
// [ 5] ALLOCATOR getAllocatorFromSubobject(const TYPE&)
// [ 2] bslma::Allocator *getAdaptedAllocator(const TYPE&)
// [ 2] TYPE::allocator_type getAdaptedAllocator(const TYPE&)
// [ 3] bslma::Allocator *getNativeAllocator(const TYPE&)
// [ 3] TYPE::allocator_type getNativeAllocator(const TYPE&)
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [-1] NEGATIVE COMPILATION TESTS
// [ 6] USAGE EXAMPLES
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

class NonAAClass { };

class LegacyAAClass {
    // Class meeting minimal requirement for detection as *legacy-AA*.

    // DATA
    bslma::Allocator *d_allocator_p;

  public:
    // TYPE TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(LegacyAAClass, bslma::UsesBslmaAllocator);

    // CREATORS
    explicit LegacyAAClass(bslma::Allocator *allocator)
        : d_allocator_p(allocator) { }

    // ACCESSORS
    bslma::Allocator *allocator() const { return d_allocator_p; }
};

struct BslAAClass {
    // Class meeting minimal requirement for detection as *bsl-AA*.

    // DATA
    bsl::allocator<int> d_allocator_p;

  public:
    // TYPES
    typedef bsl::allocator<int> allocator_type;

    // CREATORS
    explicit BslAAClass(const allocator_type& allocator)
        : d_allocator_p(allocator) { }

    // ACCESSORS
    allocator_type get_allocator() const { return d_allocator_p; }
};

struct BslLegacyAAClass {
    // Class meeting the requirements for detection as *bsl-AA*, but also the
    // traits of a *legacy-AA* class.  This type is used to test for
    // ambiguities when using functions that work for both *legacy-AA* and
    // *bsl-AA* types.  Also, by using separate allocators for the *bsl* and
    // *legacy* functionality, it allows detection of which interface was used
    // by the utility functions.

    // DATA
    bsl::allocator<int>  d_bslAllocator;
    bslma::Allocator    *d_legacyAllocator_p;

  public:
    // TYPE TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(BslLegacyAAClass,
                                   bslma::UsesBslmaAllocator);

    // TYPES
    typedef bsl::allocator<int> allocator_type;

    // CREATORS
    explicit BslLegacyAAClass(const allocator_type&  bslAlloc,
                              bslma::Allocator      *legacyAlloc)
        : d_bslAllocator(bslAlloc), d_legacyAllocator_p(legacyAlloc) { }

    // ACCESSORS
    allocator_type    get_allocator() const { return d_bslAllocator; }
    bslma::Allocator *allocator()     const { return d_legacyAllocator_p; }
};

// FUTURE:
// class PmrAAClass {
//     // Class meeting minimal requirement for detection as *pmr-AA*.
//
//     // DATA
//     bsl::polymorphic_allocator<int> d_allocator;
//
//   public:
//     // TYPES
//     typedef bsl::polymorphic_allocator<int> allocator_type;
//
//     // CREATORS
//     explicit PmrAAClass(const allocator_type& allocator)
//         : d_allocator(allocator) { }
//
//     // ACCESSORS
//     allocator_type get_allocator() const { return d_allocator; }
// };

template <class TYPE>
class StlAllocator
{
    // This class meets the minimum requirements for an allocator according to
    // the standard.

    // DATA
    int d_id;  // Arbitrary state to distinguish instances from one another

  public:
    // TYPES
    typedef TYPE value_type;

    // CREATORS
    explicit StlAllocator(int id) : d_id(id) { }
    template <class T2> StlAllocator(const StlAllocator<T2>& original)
        : d_id(original.id()) { }

    // MANIPULATORS
    TYPE *allocate(std::size_t);
        // No definition needed for this test

    void deallocate(TYPE *, std::size_t);
        // No definition needed for this test

    // ACCESSORS
    int id() const { return d_id; }

    // HIDDEN FRIENDS
    template <class T2>
    friend bool operator==(const StlAllocator&     lhs,
                           const StlAllocator<T2>& rhs)
        { return lhs.id() == rhs.id(); }

    template <class T2>
    friend bool operator!=(const StlAllocator&     lhs,
                           const StlAllocator<T2>& rhs)
        { return lhs.id() != rhs.id(); }
};

class StlAAClass {
    // Class meeting minimal requirement for detection as *stl-AA*.
    StlAllocator<char> d_allocator;

  public:
    // TYPES
    typedef StlAllocator<char> allocator_type;

    // CREATORS
    explicit StlAAClass(const allocator_type& allocator)
        : d_allocator(allocator) { }

    // ACCESSORS
    allocator_type get_allocator() const { return d_allocator; }
};

template <class TYPE>
class BslCompatibleAllocator {
    // STL-like allocator that is convertible from 'bsl::allocator'

    // DATA
    bsl::allocator<char> d_imp;

  public:
    // TYPES
    typedef TYPE value_type;

    // CREATORS
    BslCompatibleAllocator(const bsl::allocator<char>& imp)         // IMPLICIT
        // Construct 'BslCompatibleAllocator' holding the specified 'imp'
        // allocator.
        : d_imp(imp) { }

    // MANIPULATORS
    TYPE *allocate(std::size_t);
        // No definition needed for this test

    void deallocate(TYPE *, std::size_t);
        // No definition needed for this test

    // ACCESSORS
    bslma::Allocator *mechanism() const { return d_imp.mechanism(); }

    // HIDDEN FRIENDS
    template <class T2>
    friend bool operator==(const BslCompatibleAllocator&     lhs,
                           const BslCompatibleAllocator<T2>& rhs)
        { return lhs.mechanism() == rhs.mechanism(); }

    template <class T2>
    friend bool operator!=(const BslCompatibleAllocator&     lhs,
                           const BslCompatibleAllocator<T2>& rhs)
        { return lhs.mechanism() != rhs.mechanism(); }
};

int allocatorModel(bslma::Allocator *)
    // Overloads of this function return the integer model tag for which the
    // argument is the expected vocabulary type, i.e., 'AAModelLegacy::value',
    // for an argument of type 'bslma::Allocator *', etc.
{
    return bslma::AAModelLegacy::value;
}

template <class TYPE>
int allocatorModel(const bsl::allocator<TYPE>&)
{
    return bslma::AAModelBsl::value;
}

// FUTURE:
// template <class TYPE>
// int allocatorModel(const bsl::polymorphic_allocator<TYPE>&)
// {
//     return bslma::AAModelPmr::value;
// }

template <class TYPE>
int allocatorModel(const StlAllocator<TYPE>&)
{
    return bslma::AAModelStl::value;
}

}  // close unnamed namespace

// ============================================================================
//                             USAGE EXAMPLES
// ----------------------------------------------------------------------------

namespace {

///Example 1: Constructing a New Member Using an Existing Member's Allocator
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example illustrates how 'bslma::AATypeUtil::getAdaptedAllocator'
// can be used to extract the allocator from an Allocator-Aware (AA) object and
// use it to construct a different AA object without regard to whether either
// object is *legacy-AA* (using 'bslma::Allocator *') or *bsl-AA* (using
// 'bsl::allocator').  We begin by defining two *legacy-AA* classes, 'Larry',
// and 'Curly':
//..
    class Larry {
        // A *legacy-AA* class.

        // DATA
        bslma::Allocator *d_allocator_p;
        int               d_value;

    public:
        // TYPE TRAITS
        BSLMF_NESTED_TRAIT_DECLARATION(Larry, bslma::UsesBslmaAllocator);

        // CREATORS
        explicit Larry(int v, bslma::Allocator *basicAllocator = 0)
            : d_allocator_p(bslma::Default::allocator(basicAllocator))
            , d_value(v) { }

        // ACCESSORS
        bslma::Allocator *allocator() const { return d_allocator_p; }
        int               value()     const { return d_value; }
    };

    class Curly {
        // Another *legacy-AA* class.

        // DATA
        bslma::Allocator *d_allocator_p;
        int               d_value;

    public:
        // TYPE TRAITS
        BSLMF_NESTED_TRAIT_DECLARATION(Curly, bslma::UsesBslmaAllocator);

        // CREATORS
        explicit Curly(int v, bslma::Allocator *basicAllocator = 0)
            : d_allocator_p(bslma::Default::allocator(basicAllocator))
            , d_value(v) { }

        // ACCESSORS
        bslma::Allocator *allocator() const { return d_allocator_p; }
        int               value()     const { return d_value; }
    };
//..
// Next, consider a class, 'LarryMaybeCurly', that holds a 'Larry' object and
// optionally, holds a 'Curly' object.  The data members for 'LarryMaybeCurly'
// include a 'Larry' object, a flag indicating the existence of a 'Curly'
// object, and an aligned buffer to hold the optional 'Curly' object, if it
// exists.  Because the 'Larry' member holds an allocator, there is no need for
// a separate allocator data member:
//..
    class LarryMaybeCurly {
        // Holds a 'Larry' object and possibly a 'Curly' object.

        // DATA
        bool                      d_hasCurly; // True if 'd_curly' is populated
        Larry                     d_larry;
        bsls::ObjectBuffer<Curly> d_curly;    // Maybe holds a 'Curly' object
//..
// Next we complete the public interface, which includes a constructor that
// sets the value of the 'Larry' object, a manipulator for setting the value of
// the 'Curly' object, and accessors for retrieving the 'Larry' and 'Curly'
// objects.  Because 'LarryMaybeCurly' is allocator-aware (AA), we must have an
// 'allocator_type' member, and a 'get_allocator' accessor; every constructor
// should also take an optional allocator argument.
//..
      public:
        // TYPES
        typedef bsl::allocator<char> allocator_type;

        // CREATORS
        explicit LarryMaybeCurly(int                   v,
                                 const allocator_type& a = allocator_type());
            // Create an object having a 'Larry' member with the specified 'v'
            // value and having no 'Curly' member.  Optionally specify an
            // allocator 'a' to supply memory.

        // ...

        // MANIPULATORS
        void setCurly(int v);
            // initialize the 'Curly' member to the specified 'v' value.

        // ACCESSORS
        bool         hasCurly() const { return d_hasCurly; }
        const Larry& larry()    const { return d_larry; }
        const Curly& curly()    const { return d_curly.object(); }

        allocator_type get_allocator() const;
    };
//..
// Now we implement the constructor that initializes value of the 'Larry'
// member and leaves the 'Curly' member unset.  Notice that we use
// 'bslma::AllocatorUtil::adapt' to smooth out the mismatch between the
// 'bsl::allocator' used by 'LarryMaybeCurly' and the 'bslma::Allocator *'
// expected by 'Larry'.
//..
    LarryMaybeCurly::LarryMaybeCurly(int v, const allocator_type& a)
        : d_hasCurly(false), d_larry(v, bslma::AllocatorUtil::adapt(a)) { }
//..
// Next, we implement the manipulator for setting the 'Curly' object.  This
// manipulator must use the allocator stored in 'd_larry'.  The function,
// 'getAdaptedAllocator' yields this allocator in a form that can be
// consumed by the 'Curly' constructor:
//..
        // MANIPULATORS
    void LarryMaybeCurly::setCurly(int v)
    {
        new (d_curly.address())
            Curly(v, bslma::AATypeUtil::getAdaptedAllocator(d_larry));
        d_hasCurly = true;
    }
//..
// Finally, we can use a test allocator to verify that, when a
// 'LarryMaybeCurly' object is constructed with an allocator, that same
// allocator is used to construct both the 'Larry' and 'Curly' objects within
// it:
//..
    int usageExample1()
    {
        bslma::TestAllocator ta;
        bsl::allocator<char> bslAlloc(&ta);

        LarryMaybeCurly obj1(5, bslAlloc);
        ASSERT(5   == obj1.larry().value());
        ASSERT(&ta == obj1.larry().allocator());
        ASSERT(! obj1.hasCurly());

        obj1.setCurly(10);
        ASSERT(5   == obj1.larry().value());
        ASSERT(&ta == obj1.larry().allocator());
        ASSERT(obj1.hasCurly());
        ASSERT(10  == obj1.curly().value());
        ASSERT(&ta == obj1.curly().allocator());
//..
// It may not be immediately obvious that 'getAdaptedAllocator' provides
// much benefit; indeed, the example would work just fine if we called
// 'Larry::allocator()' and passed the result directly to the constructor of
// 'Curly':
//..
        Larry larryObj(5, &ta);
        Curly curlyObj(10, larryObj.allocator());
        ASSERT(&ta == curlyObj.allocator());

        return 0;
    }
//..
// The code above is brittle, however, as updating 'Larry' to be *bsl-AA* would
// require calling 'larryObj.get_allocator().mechanism()' instead of
// 'larryObj.allocator().  By using 'getAdaptedAllocator', the 'setCurly'
// implementation above is robust in the face of such future evolution.  This
// benefit is even more important in generic code, especially when *pmr-AA*
// types are added into the mix in the future.
//
///Example 2: Retrieving a Specific Allocator Type from a Subobject
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example illustrates how 'bslma::AATypeUtil::getAllocatorFromSubobject'
// can be used to retrieve an allocator of a specific type from a subobject
// even if that subobject uses an allocator with a smaller interface.
//
// First, continuing from the previous example, we implement the
// 'get_allocator' accessor.  As we know, the allocator for a 'LarryMaybeCurly'
// object is stored in the 'd_larry' subobject, obviating a separate
// 'd_allocator_p' member.  However, the allocator within 'd_larry' is a
// 'bslma::Allocator *' whereas the 'allocator_type' for 'LarryMaybeCurly' is
// 'bsl::allocator<char>'.  When the 'LarryMaybeCurly' object was constructed,
// some type information was lost in the conversion to 'bslma::Allocator'.
// That information is recovered through the use of
// 'getAllocatorFromSubobject':
//..
    bsl::allocator<char> LarryMaybeCurly::get_allocator() const
    {
        typedef bslma::AATypeUtil Util;
        return Util::getAllocatorFromSubobject<allocator_type>(d_larry);
    }
//..
// Now we can construct a 'LarryMaybeCurly' object with a specific allocator
// and recover that allocator using the 'get_allocator' accessor:
//..
    int usageExample2()
    {
        bslma::TestAllocator ta;
        bsl::allocator<char> bslAlloc(&ta);

        LarryMaybeCurly obj1(5, bslAlloc);
        ASSERT(bslAlloc == obj1.get_allocator());
//..
// As in the previous example, it is possible to get the same effect without
// using the utilities in this component because 'bslma::Allocator *' is
// implicitly convertible to 'bsl::allocator<char>':
//..
        Larry                larryObj(5, &ta);
        bsl::allocator<char> objAlloc = larryObj.allocator();
        ASSERT(objAlloc == bslAlloc);
        return 0;
    }
//..
// However, the preceding 'get_allocator' implementation, like the 'setCurly'
// implementation in the previous example, is more robust because it need not
// be changed if 'Larry' is changed from *legacy-AA* to *bsl-AA* or if it is
// replaced by a template parameter that might use either AA model.  When
// *pmr-AA* is added to the Bloomberg codebase, using the
// 'getAllocatorFromSubobject' idiom will be vital to recovering 'bsl'
// allocators stored within *pmr-AA* objects.

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
      case 6: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLES
        //
        // Concerns:
        //: 1 That the usage examples shown in the component-level
        //:   documentation compile and run as described.
        //
        // Plan:
        //: 1 Copy the usage examples from the component header, changing
        //    'assert' to 'ASSERT' and execute them.
        //
        // Testing:
        //     USAGE EXAMPLES
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLES"
                            "\n==============\n");

        usageExample1();
        usageExample2();

      } break;

      case 5: {
        // --------------------------------------------------------------------
        // 'getAllocatorFromSubobject'
        //
        // Concerns:
        //: 1 For a *legacy-AA* object, 'obj', and 'ALLOCATOR' type
        //:   'bslma::Allocator *', 'getAllocatorFromSubobject<ALLOCATOR>(obj)'
        //:   returns 'obj.allocator()'.
        //: 2 For a *legacy-AA* object, 'obj', and 'ALLOCATOR' type explicitly
        //:   convertible from 'bsl::allocator<char>'
        //:   'getAllocatorFromSubobject<ALLOCATOR>(obj)' returns
        //:   'static_cast<ALLOCATOR>(bsl::allocator<char>(obj.allocator()))'.
        //: 3 For a *bsl-AA* object, 'obj', and 'ALLOCATOR' type
        //:   'bslma::Allocator *', 'getAllocatorFromSubobject<ALLOCATOR>(obj)'
        //:   returns 'obj.get_allocator().mechanism()'.
        //: 4 (FUTURE) For a *pmr-AA* object, 'obj',
        //:   'getAllocatorFromSubobject<bsl::memory_resource *>(obj)' returns
        //:   'obj.get_allocator().resource())' and
        //:   'getAllocatorFromSubobject<bslma::Allocator *>(obj)' returns
        //:   'static_cast<bslma::Allocator*>(obj.get_allocator().resource())'
        //: 5 For a *bsl-AA*, *pmr-AA* (FUTURE), or *stl-AA* object 'obj' of
        //:   type 'TYPE', and 'ALLOCATOR' type explicitly convertible from
        //:   'TYPE::allocator_type',
        //:   'getAllocatorFromSubobject<ALLOCATOR>(obj)' returns
        //:   'static_cast<ALLOCATOR>(obj.get_allocator())'.
        //: 6 For an object, 'obj', whose type has the interface of both
        //:   *legacy-AA* ('UsesBslmaAllocator' type trait and 'allocator'
        //:   accessor) and *bsl-AA* ('allocator_type' typedef and
        //:   'get_allocator' accessor),
        //:   'getAllocatorFromSubobject<ALLOCATOR>(obj)' is callable without
        //:   ambiguity and returns
        //:   'static_cast<ALLOCATOR>(obj.get_allocator())', (i.e., prefer the
        //:   *bsl-AA* model to the *legacy-AA* model).
        //
        // Plan
        //: 1 Construct *legacy-AA* object, 'obj', using the address of a
        //:   'bslma::TestAllocator' allocator.  For 'ALLOCATOR' types
        //:   'bslma::Allocator *', 'bsl::allocator<char>',
        //:   'bsl::allocator<int>', 'bsl::polymorphic_allocator<char>'
        //:   (FUTURE), and a synthetic type explicitly convertible from
        //:   'bsl::allocator<char>', invoke
        //:   'getAllocatorFromSubobject<ALLOCATOR>(obj)' and verify that the
        //:   resulting allocator was constructed from the test allocator.
        //:   (C-1, C-2)
        //: 2 Construct a *bsl-AA* object, 'obj', using the address of a
        //:   'bslma::TestAllocator' allocator.  Invoke
        //:   'getAllocatorFromSubobject<bslma::Allocator *>(obj)' and verify
        //:   that the resulting allocator was constructed from the test
        //:   allocator.  (C-3)
        //: 3 (FUTURE) Construct a *pmr-AA* object, 'obj', using the address of
        //:   a 'bslma::TestAllocator' allocator.  Invoke
        //:   'getAllocatorFromSubobject<bsl::memory_resource *>(obj)' and
        //:   'getAllocatorFromSubobject<bslma::Allocator *>(obj)' and verify
        //:   that, in both cases, the resulting allocator was constructed from
        //:   the test allocator.  (C-4)
        //: 4 Repeat step 1 with *bsl-AA* and *pmr-AA* (FUTURE) objects.  (C-5)
        //: 5 Construct an *stl-AA* object, 'obj', with an STL-style allocator.
        //:   For 'ALLOCATOR' types convertible from the STL-allocator type,
        //:   verify that 'getAllocatorFromSubobject<ALLOCATOR>(obj)' returns
        //:   an allocator representing a copy of that STL-allocator type.
        //:   (C-5)
        //: 6 Define a class having all of the attributes of both a *legacy-AA*
        //:   type and a *bsl-AA* type, using different allocators for each
        //:   model.  Construct an object, 'obj', of that class and repeat step
        //:   1 with that object.  Verify that, in each case,
        //:   'getAllocatorFromSubobject' returns an allocator derived from the
        //:   *bsl-AA* interface, not the *legacy-AA* interface.  (C-6)
        //
        // Testing:
        //      ALLOCATOR getAllocatorFromSubobject(const TYPE&)
        // --------------------------------------------------------------------

        if (verbose) printf("\n'getAllocatorFromSubobject'"
                            "\n===========================\n");

        typedef bslma::AATypeUtil Util;

        bslma::TestAllocator        ta1, ta2;
        bslma::Allocator     *const bslmaAlloc = &ta1;
        bsl::allocator<int>         bslAlloc(&ta2);
        StlAllocator<int>           stlAlloc(99);
        ASSERT(bslmaAlloc != bslAlloc);

        LegacyAAClass    laac(bslmaAlloc); const LegacyAAClass& LAAC = laac;
        BslAAClass       baac(bslAlloc);   const BslAAClass&    BAAC = baac;
        StlAAClass       saac(stlAlloc);   const StlAAClass&    SAAC = saac;

        BslLegacyAAClass        blaac(bslAlloc, bslmaAlloc);
        const BslLegacyAAClass& BLAAC = blaac;

        // Step 1: *legacy-AA*
        ASSERT(bslmaAlloc ==
               Util::getAllocatorFromSubobject<bslma::Allocator *>(LAAC));
        ASSERT(bslmaAlloc ==
               Util::getAllocatorFromSubobject<bsl::allocator<char> >(LAAC));
        ASSERT(bslmaAlloc ==
               Util::getAllocatorFromSubobject<bsl::allocator<int> >(LAAC));
        // FUTURE
        // ASSERT(bslmaAlloc ==
        //        Util::getAllocatorFromSubobject<
        //            bsl::polymorphic_allocator<int> >(LAAC));
        ASSERT(bslmaAlloc ==
               Util::getAllocatorFromSubobject<
                   BslCompatibleAllocator<char> >(LAAC).mechanism());

        // Step 2: get a 'bslma::Allocator *' from a *bsl-AA* object
        ASSERT(&ta2 ==
               Util::getAllocatorFromSubobject<bslma::Allocator *>(BAAC));

        // Step 3: get a 'bslma::Allocator *' from a *pmr-AA* object
        // (FUTURE)

        // Step 4: get other allocator types from *bsl-AA* and *bsl-AA*
        // (FUTURE) objects.
        ASSERT(bslAlloc ==
               Util::getAllocatorFromSubobject<bsl::allocator<char> >(BAAC));
        ASSERT(bslAlloc ==
               Util::getAllocatorFromSubobject<bsl::allocator<int> >(BAAC));
        // FUTURE
        // ASSERT(bslAlloc ==
        //        Util::getAllocatorFromSubobject<
        //            bsl::polymorphic_allocator<int> >(BAAC));
        ASSERT(bslAlloc ==
               Util::getAllocatorFromSubobject<
                   BslCompatibleAllocator<char> >(BAAC).mechanism());

        // Step 5: *stl-AA* types
        ASSERT(stlAlloc ==
               Util::getAllocatorFromSubobject<StlAllocator<int> >(SAAC));
        ASSERT(stlAlloc ==
               Util::getAllocatorFromSubobject<StlAllocator<char> >(SAAC));

        // Step 6: Type that has both *legacy-AA* and *bsl-AA* interfaces.
        ASSERT(&ta2 ==
               Util::getAllocatorFromSubobject<bslma::Allocator *>(BLAAC));
        ASSERT(bslAlloc ==
               Util::getAllocatorFromSubobject<bsl::allocator<char> >(BLAAC));
        ASSERT(bslAlloc ==
               Util::getAllocatorFromSubobject<bsl::allocator<int> >(BLAAC));
        // FUTURE
        // ASSERT(bslAlloc ==
        //        Util::getAllocatorFromSubobject<
        //            bsl::polymorphic_allocator<int> >(BLAAC));
        ASSERT(bslAlloc ==
               Util::getAllocatorFromSubobject<
                   BslCompatibleAllocator<char> >(BLAAC).mechanism());

      } break;

      case 4: {
        // --------------------------------------------------------------------
        // 'getBslAllocator'
        //
        // Concerns:
        //: 1 For a *legacy-AA* object, 'obj', 'getBslAllocator(obj)' returns
        //:   'bsl::allocator<char>(obj.allocator())'.
        //: 2 For a *bsl-AA* object 'obj' of 'getBslAllocator(obj)' returns
        //:   'obj.get_allocator()'
        //: 3 (FUTURE) For a *pmr-AA* object, 'obj', 'getBslAllocator(obj)'
        //:   returns 'bsl::allocator<char>( dynamic_cast<bslma::Allocator*>(
        //:   obj.get_allocator().resource() ))'
        //: 4 For an object, 'obj', whose type has the interface of both
        //:   *legacy-AA* ('UsesBslmaAllocator' type trait and 'allocator'
        //:   accessor) and *bsl-AA* ('allocator_type' typedef and
        //:   'get_allocator' accessor), 'getBslAllocator(obj)' is callable
        //:   without ambiguity and returns 'obj.get_allocator()', (i.e.,
        //:   prefer the *bsl-AA* model to the *legacy-AA* model).
        //
        // Plan
        //: 1 Construct *legacy-AA* object, 'obj', using the address of a
        //:   'bslma::TestAllocator' allocator.  Invoke 'getBslAllocator(obj)'
        //:   and verify that the resulting allocator was constructed from the
        //:   test allocator.  (C-1)
        //: 2 Repeat step 1 with a *bsl-AA* objects.  (C-2)
        //: 3 (FUTURE) Construct a *pmr-AA* object, 'obj', using the address of
        //:   a 'bslma::TestAllocator' allocator.  Invoke
        //:   'getBslAllocator(obj)' and verify that the that the resulting
        //:   allocator was constructed from the test allocator.  (C-3)
        //: 4 Define a class having all of the attributes of both a *legacy-AA*
        //:   type and a *bsl-AA* type, using different allocators for each
        //:   model.  Construct an object, 'obj', of that class and repeat step
        //:   1 with that object.  Verify that 'getBslAllocator' returns an
        //:   allocator constructed from the *bsl-AA* allocator, not the
        //:   *legacy-AA* allocator.  (C-4)
        //
        // Testing:
        //      bsl::allocator<char> getBslAllocator(const TYPE&)
        // --------------------------------------------------------------------

        if (verbose) printf("\n'getBslAllocator'"
                            "\n=================\n");

        typedef bslma::AATypeUtil Util;

        bslma::TestAllocator        ta1, ta2;
        bslma::Allocator     *const bslmaAlloc = &ta1;
        bsl::allocator<int>         bslAlloc(&ta2);
        StlAllocator<int>           stlAlloc(99);
        ASSERT(bslmaAlloc != bslAlloc);

        LegacyAAClass    laac(bslmaAlloc); const LegacyAAClass& LAAC = laac;
        BslAAClass       baac(bslAlloc);   const BslAAClass&    BAAC = baac;

        BslLegacyAAClass        blaac(bslAlloc, bslmaAlloc);
        const BslLegacyAAClass& BLAAC = blaac;

        // Step 1: *legacy-AA* object
        ASSERT(bslmaAlloc == Util::getBslAllocator(LAAC));

        // Step 2: *bsl-AA* object
        ASSERT(bslAlloc == Util::getBslAllocator(BAAC));

        // Step 3 (FUTURE): *pmr-AA*
        // ASSERT(bslAlloc == Util::getBslAllocator(PAAC));

        // Step 4: Object that has both *legacy-AA* and *bsl-AA* interfaces.
        ASSERT(bslAlloc == Util::getBslAllocator(BLAAC));

      } break;

      case 3: {
        // --------------------------------------------------------------------
        // 'getNativeAllocator'
        //
        // Concerns:
        //: 1 For a *legacy-AA* object, 'obj', the return type of
        //:   'getNativeAllocator(obj)' is 'bslma::Allocator *'.
        //: 2 For a *legacy-AA* object, 'obj', 'getNativeAllocator(obj)'
        //:   returns 'obj.allocator()'
        //: 3 For a *bsl-AA* object, 'obj', the return type of
        //:   'getNativeAllocator(obj)' is a specialization of
        //:   'bslma::allocator'.
        //: 4 For a *bsl-AA* object, 'obj', 'getNativeAllocator(obj)' returns
        //:   'obj.get_allocator())'.
        //: 5 (FUTURE) For a *pmr-AA* object, 'obj', the return type of
        //:   'getNativeAllocator(obj)' is a specialization of
        //:   'bslma::polymorphic_allocator'.
        //: 6 (FUTURE) For a *pmr-AA* object, 'obj', 'getNativeAllocator(obj)'
        //:   returns 'obj.get_allocator())'.
        //: 7 For an *stl-AA* object, 'obj', the return type of
        //:   'getNativeAllocator(obj)' is 'allocator_type' (possibly rebound).
        //: 8 For a *stl-AA* object, 'obj', 'getNativeAllocator(obj)' returns
        //:   'obj.get_allocator())'.
        //: 9 For an object, 'obj', whose type has the interface of both
        //:   *legacy-AA* ('UsesBslmaAllocator' type trait and 'allocator'
        //:   accessor) and *bsl-AA* ('allocator_type' typedef and
        //:   'get_allocator' accessor) 'getNativeAllocator(obj)' is callable
        //:   without ambiguity and returns a 'bsl::allocator' object with
        //:   value 'obj.get_allocator()' (not a 'bsl::Allocator *' pointer
        //:   with value 'obj.allocator()' (i.e., prefer the *bsl-AA* model to
        //:   the *legacy-AA* model).
        //
        // Plan:
        //: 1 Create a set of function overloads, 'allocatorModel', that return
        //:   a different integer value when given an argument of the allocator
        //:   vocabulary type for each of the AA models.
        //: 2 Construct a *legacy-AA* object and verify that
        //:   'getNativeAllocator' returns a pointer of type
        //:   'bslma::Allocator *' (as determined by the 'allocatorModel'
        //:   function from step 1) having the expected value.  (C-1, C-2)
        //: 3 Construct a *bsl-AA* object and verify that 'getNativeAllocator'
        //:   returns a value of type 'bsl::allocator<T>' (as determined by the
        //:   'allocatorModel' function from step 1) having the expected value.
        //:   (C-3, C-4)
        //: 4 (FUTURE) Construct a *pmr-AA* object and verify that
        //:   'getNativeAllocator' returns a value of type
        //:   'bsl::polymorphic_allocator<T>' (as determined by the
        //:   'allocatorModel' function from step 1) having the expected value.
        //:   (C-5, C-6)
        //: 5 Construct an *stl-AA* object and verify that 'getNativeAllocator'
        //:   returns a value of type 'allocator_type' (as determined by the
        //:   'allocatorModel' function from step 1) having the expected value.
        //:   (C-7, C-8)
        //: 6 Define a class with all of the attributes of both a *legacy-AA*
        //:   type and a *bsl-AA* type, using different allocators for each
        //:   model.  Construct an object, 'obj', of that class and verify that
        //:   'getNativeAlloator(obj)' returns a value of type
        //:   'bsl::allocator<T>' (as determined by the 'allocatorModel'
        //:   function from step 1) having the value returned by
        //:   'obj.get_allocator()'.  (C-9)
        //
        // Testing:
        //      bslma::Allocator *getNativeAllocator(const TYPE&)
        //      TYPE::allocator_type getNativeAllocator(const TYPE&)
        // --------------------------------------------------------------------

        if (verbose) printf("\n'getNativeAllocator'"
                            "\n====================\n");

        typedef bslma::AATypeUtil Util;

        bslma::TestAllocator        ta1, ta2;
        bslma::Allocator     *const bslmaAlloc = &ta1;
        bsl::allocator<int>         bslAlloc(&ta2);
        StlAllocator<int>           stlAlloc(99);
        ASSERT(bslmaAlloc != bslAlloc);

        LegacyAAClass    laac(bslmaAlloc); const LegacyAAClass& LAAC = laac;
        BslAAClass       baac(bslAlloc);   const BslAAClass&    BAAC = baac;
        StlAAClass       saac(stlAlloc);   const StlAAClass&    SAAC = saac;

        BslLegacyAAClass        blaac(bslAlloc, bslmaAlloc);
        const BslLegacyAAClass& BLAAC = blaac;

        // Step 2: Test *legacy-AA* type
        ASSERT(bslma::AAModelLegacy::value ==
               allocatorModel(Util::getNativeAllocator(LAAC)));
        ASSERT(Util::getNativeAllocator(LAAC)  == bslmaAlloc);

        // Step 3: Test *bsl-AA* type
        ASSERT(bslma::AAModelBsl::value ==
               allocatorModel(Util::getNativeAllocator(BAAC)));
        ASSERT(Util::getNativeAllocator(BAAC)  == bslAlloc);

        // Step 6: Test *pmr-AA* type
        // (FUTURE)

        // Step 5: Test *stl-AA* type
        ASSERT(bslma::AAModelStl::value ==
               allocatorModel(Util::getNativeAllocator(SAAC)));
        ASSERT(Util::getNativeAllocator(SAAC)  == stlAlloc);

        // Step 6, Prefer *bsl-AA* to *legacy-AA* when both are available
        ASSERT(bslma::AAModelBsl::value ==
               allocatorModel(Util::getNativeAllocator(BLAAC)));
        ASSERT(bslma::AAModelLegacy::value !=
               allocatorModel(Util::getNativeAllocator(BLAAC)));
        ASSERT(Util::getNativeAllocator(BLAAC) == bslAlloc);
        ASSERT(Util::getNativeAllocator(BLAAC) != bslmaAlloc);

      } break;

      case 2: {
        // --------------------------------------------------------------------
        // 'getAdaptedAllocator'
        //
        // Concerns:
        //: 1 For a *legacy-AA* object, 'obj', 'getAdaptedAllocator(obj)'
        //:   returns 'bslma::AllocatorUtil::adapt(obj.allocator())'.
        //: 2 For a *bsl-AA* object, 'obj', 'getAdaptedAllocator(obj)'
        //:   returns 'bslma::AllocatorUtil::adapt(obj.get_allocator())'.
        //: 3 For an *stl-AA* type or (future) *pmr-AA* object, 'obj',
        //:   'getAdapted(obj)' returns 'obj.get_allocator()'.
        //: 4 For an object, 'obj', whose type has the interface of both
        //:   *legacy-AA* ('UsesBslmaAllocator' type trait and 'allocator'
        //:   accessor) and *bsl-AA* ('allocator_type' typedef and
        //:   'get_allocator' accessor) 'getAdaptedAllocator(obj)' is
        //:   callable without ambiguity and returns
        //:   'bslma::AllocatorUtil::adapt(obj.get_allocator())' (i.e., prefer
        //:   the *bsl-AA* model to the *legacy-AA* model).
        //
        // Plan:
        //: 1 Construct a *legacy-AA* object and verify that
        //:   'getAdaptedAllocator' returns the expected allocator value.
        //:   (C-1)
        //: 2 Construct a *bsl-AA* object and verify that
        //:   'getAdaptedAllocator' returns the expected allocator value.
        //:   (C-2)
        //: 3 Construct an *stl-AA* object and (future) *pmr-AA* object and
        //:   verify that 'getAdaptedAllocator' returns the expected allocator
        //:   value for each.  (C-3)
        //: 4 Define a class with all of the attributes of both a *legacy-AA*
        //:   type and a *bsl-AA* type, using different allocators for each
        //:   model.  Construct an object, 'obj', of that class and verify that
        //:   'getConvertibleAlloator(obj)' returns the value returned by
        //:   'obj.get_allocator()', not 'obj.allocator()'.  (C-4)
        //
        // Testing:
        //      bslma::Allocator *getAdaptedAllocator(const TYPE&)
        //      TYPE::allocator_type getAdaptedAllocator(const TYPE&)
        // --------------------------------------------------------------------

        if (verbose) printf("\n'getAdaptedAllocator'"
                            "\n=====================\n");

        typedef bslma::AATypeUtil Util;

        bslma::TestAllocator        ta1, ta2;
        bslma::Allocator     *const bslmaAlloc = &ta1;
        bsl::allocator<int>         bslAlloc(&ta2);
        StlAllocator<char>          stlAlloc(3);
        ASSERT(bslmaAlloc != bslAlloc);

        LegacyAAClass   laac(bslmaAlloc); const LegacyAAClass& LAAC = laac;
        BslAAClass      baac(bslAlloc);   const BslAAClass&    BAAC = baac;
        StlAAClass      saac(stlAlloc);   const StlAAClass&    SAAC = saac;

        BslLegacyAAClass        blaac(bslAlloc, bslmaAlloc);
        const BslLegacyAAClass& BLAAC = blaac;

        ASSERT(Util::getAdaptedAllocator(LAAC)  == bslmaAlloc);
        ASSERT(Util::getAdaptedAllocator(BAAC)  == bslAlloc);
        ASSERT(Util::getAdaptedAllocator(BLAAC) == bslAlloc);
        ASSERT(Util::getAdaptedAllocator(BLAAC) != bslmaAlloc);
        ASSERT(Util::getAdaptedAllocator(SAAC)  == stlAlloc);

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
        //: 1 Execute each method to verify functionality for simple cases.
        //:   (C-1)
        //
        // Testing:
        //      BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        typedef bslma::AATypeUtil Util;

        bslma::TestAllocator        ta1, ta2;
        bslma::Allocator     *const bslmaAlloc = &ta1;
        bsl::allocator<int>         bslAlloc(&ta2);
        StlAllocator<int>           stlAlloc(99);

        LegacyAAClass laac(bslmaAlloc); const LegacyAAClass& LAAC = laac;
        BslAAClass    baac(bslAlloc);   const BslAAClass&    BAAC = baac;
        StlAAClass    saac(stlAlloc);   const StlAAClass&    SAAC = saac;

        // Exercise 'getAdaptedAllocator'
        ASSERT(bslmaAlloc == Util::getAdaptedAllocator(LAAC));
        ASSERT(bsl::allocator<char>(bslmaAlloc) ==
               Util::getAdaptedAllocator(LAAC));
        ASSERT(bslAlloc             == Util::getAdaptedAllocator(BAAC));
        ASSERT(bslAlloc.mechanism() == Util::getAdaptedAllocator(BAAC));

        // Exercise 'getNativeAllocator'
        ASSERT(bslmaAlloc == Util::getNativeAllocator(LAAC));
        ASSERT(bslAlloc   == Util::getNativeAllocator(BAAC));
        ASSERT(stlAlloc   == Util::getNativeAllocator(SAAC));

      } break;

      case -1: {
        // --------------------------------------------------------------------
        // NEGATIVE COMPILATION TESTS
        //   These tests check that certain constructs will not compile.
        //   Typically, these tests are commented out with '#if 0' but can be
        //   uncommented selectively to verify non-compilation.
        //
        // Concerns:
        //: 1 'AATypeUtil::getAdaptedAllocator' will not be found by
        //:   overload resolution when called on an object that is not AA.
        //: 2 'AATypeUtil::getNativeAllocator' will not be found by
        //:   overload resolution when called on an object that is not AA.
        //
        // Plan:
        //: 1 Call 'AATypeUtil::getAdaptedAllocator' on an 'int' and on an
        //:   object of non-AA class type.  Verify that the compiler complains
        //:   about each having no matching function.  (C-1)
        //: 2 Call 'AATypeUtil::getNativeAllocator' on an 'int' and on an
        //:   object of non-AA class type.  Verify that the compiler complains
        //:   about each having no matching function.  (C-2)
        //
        // Testing
        //      NEGATIVE COMPILATION TESTS
        // --------------------------------------------------------------------

        if (verbose) printf("\nNEGATIVE COMPILATION TESTS"
                            "\n==========================\n");

        typedef bslma::AATypeUtil Util;  (void) Util();

        int                i = 0;     (void) i;
        NonAAClass         nonAA;     (void) nonAA;

#if 0
        // Step 1: 'getAdaptedAllocator'
        Util::getAdaptedAllocator(i);
        Util::getAdaptedAllocator(nonAA);
#endif

#if 0
        // Step 2: 'getNativeAllocator'
        Util::getNativeAllocator(i);
        Util::getNativeAllocator(nonAA);
#endif

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
