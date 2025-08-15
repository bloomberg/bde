// bdlf_overloaded.t.cpp                                              -*-C++-*-
#include <bdlf_overloaded.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_compilerfeatures.h>
#include <bsls_review.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>

#include <bsl_string_view.h>
#include <bsl_variant.h>

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
#include <variant>
#endif

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// This component consists of an empty tag type and a constant object of that
// type.  There is minimal functionality to test.
//-----------------------------------------------------------------------------
// [1] Overloaded::Overloaded(...)
// [1] Overloaded::operator()(...)
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [2] FREE FUNCTIONS AND MEMBER FUNCTIONS
// [3] TEMPLATED FREE FUNCTIONS AND MEMBER FUNCTIONS
// [4] WORKING WITH `VARIANT`
// [5] USAGE EXAMPLE

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
//                STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

//  The idea here is that all the test functions return a value that
//  corresponds to the type of their input.  That way we can be sure that the
//  correct choice in the overload set is called.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
enum {
    kVoid,

    kShort,
    kUnsignedShort,
    kInt,
    kUnsignedInt,

    kFloat,
    kDouble,
    kLongDouble,

    kStringView,
    kEndOfList
};

//  -- operator ()
struct FloatingPoint {
    int operator()(float)       const {return kFloat;}
    int operator()(double)      const {return kDouble;}
    int operator()(long double) const {return kLongDouble;}
};

//  ----- free functions
namespace FreeFN {
    // A collection of free functions that take a single parameter, and return
    // an enum value corresponding to the type that they were called with.
    int HandleShort                 (short) { return kShort; }
    int HandleUnsignedShort(unsigned short) { return kUnsignedShort; }
    int HandleInt                     (int) { return kInt; }
    int HandleUnsignedInt    (unsigned int) { return kUnsignedInt; }

    int TemplateFN() { return kVoid; }

    // A templated function that returns an enum value corresponding to the
    // type that they were instantiated  with.
    template <class TYPE>
    int TemplateFN(TYPE) {
        if (std::is_same_v<TYPE, short>)            return kShort;
        if (std::is_same_v<TYPE, unsigned short>)   return kUnsignedShort;
        if (std::is_same_v<TYPE, int>)              return kInt;
        if (std::is_same_v<TYPE, unsigned int>)     return kUnsignedInt;
        if (std::is_same_v<TYPE, float>)            return kFloat;
        if (std::is_same_v<TYPE, double>)           return kDouble;
        if (std::is_same_v<TYPE, long double>)      return kLongDouble;
        if (std::is_same_v<TYPE, string_view>)      return kStringView;
        return -1;  // some other type
    }

}  // close namespace FreeFN

//  ----- member functions

/// A structure containing a set of static member functions that take the
/// types that we are testing, and return the enum corresponding to the
/// type.
struct StaticMF {
    static int HandleIntMF (int) { return kInt;}
};

/// A structure containing a set of member functions that take the types
/// that we are testing, and return the enum corresponding to the type.
struct NonStaticMF {

    int HandleShortMF          (short)                { return kShort;}
    int HandleUnsignedShortMF  (unsigned short)       { return kUnsignedShort;}
    int CHandleShortMF         (short)          const { return kShort;}
    int CHandleUnsignedShortMF (unsigned short) const { return kUnsignedShort;}

    int CHandleIntMF           (int)            const { return kInt;}
    int NHandleIntMF           (int)                  noexcept { return kInt;}
    int CNHandleUnsignedIntMF  (unsigned int)   const noexcept {
        return kUnsignedInt;
    }

    int CHandleStringViewMF    (string_view)    const { return kStringView;}
};

//  ----- templated member functions
struct staticTemplateMF {

# if defined(BSLS_COMPILERFEATURES_SUPPORT_STATIC_CALL_OPERATOR)
    // Declaring overloaded `operator()` as `static` is a C++23 feature
    // backported as an extension by Clang.#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#   ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#       pragma GCC diagnostic push
#       pragma GCC diagnostic ignored "-Wc++23-extensions"
#   endif
    static int operator()() { return kVoid; }

    template <class TYPE>
    static int operator()(TYPE) noexcept {
        if (bsl::is_same_v<TYPE, short>)            return kShort;
        if (bsl::is_same_v<TYPE, unsigned short>)   return kUnsignedShort;
        if (bsl::is_same_v<TYPE, int>)              return kInt;
        if (bsl::is_same_v<TYPE, unsigned int>)     return kUnsignedInt;
        if (bsl::is_same_v<TYPE, float>)            return kFloat;
        if (bsl::is_same_v<TYPE, double>)           return kDouble;
        if (bsl::is_same_v<TYPE, long double>)      return kLongDouble;
        if (bsl::is_same_v<TYPE, string_view>)      return kStringView;
        return -1;  // some other type
    }
#   ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#       pragma GCC diagnostic pop
#   endif
# endif  // BSLS_COMPILERFEATURES_SUPPORT_STATIC_CALL_OPERATOR

    static int MemberFN() { return kVoid; }

    template <class TYPE>
    static int MemberFN(TYPE) noexcept {
        if (bsl::is_same_v<TYPE, short>)            return kShort;
        if (bsl::is_same_v<TYPE, unsigned short>)   return kUnsignedShort;
        if (bsl::is_same_v<TYPE, int>)              return kInt;
        if (bsl::is_same_v<TYPE, unsigned int>)     return kUnsignedInt;
        if (bsl::is_same_v<TYPE, float>)            return kFloat;
        if (bsl::is_same_v<TYPE, double>)           return kDouble;
        if (bsl::is_same_v<TYPE, long double>)      return kLongDouble;
        if (bsl::is_same_v<TYPE, string_view>)      return kStringView;
        return -1;  // some other type
    }
};

/// A structure containing a templated `operator()` member function that
/// returns the  enum corresponding to the type that the function was
/// instantiated with.  It also contains a member function `MemberFN` that
/// does the same thing.
struct TemplateMF {

    int operator()() const { return kVoid; }

    template <class TYPE>
    int operator()(TYPE) const noexcept {
        if (bsl::is_same_v<TYPE, short>)            return kShort;
        if (bsl::is_same_v<TYPE, unsigned short>)   return kUnsignedShort;
        if (bsl::is_same_v<TYPE, int>)              return kInt;
        if (bsl::is_same_v<TYPE, unsigned int>)     return kUnsignedInt;
        if (bsl::is_same_v<TYPE, float>)            return kFloat;
        if (bsl::is_same_v<TYPE, double>)           return kDouble;
        if (bsl::is_same_v<TYPE, long double>)      return kLongDouble;
        if (bsl::is_same_v<TYPE, string_view>)      return kStringView;
        return -1;  // some other type
    }


    int voidMemberFN() const { return kVoid; }

    template <class TYPE>
    int MemberFN(TYPE) const noexcept {
        if (bsl::is_same_v<TYPE, short>)            return kShort;
        if (bsl::is_same_v<TYPE, unsigned short>)   return kUnsignedShort;
        if (bsl::is_same_v<TYPE, int>)              return kInt;
        if (bsl::is_same_v<TYPE, unsigned int>)     return kUnsignedInt;
        if (bsl::is_same_v<TYPE, float>)            return kFloat;
        if (bsl::is_same_v<TYPE, double>)           return kDouble;
        if (bsl::is_same_v<TYPE, long double>)      return kLongDouble;
        if (bsl::is_same_v<TYPE, string_view>)      return kStringView;
        return -1;  // some other type
    }
};


/// Call the `operator()` of the specified `call` with various types, and
/// ensure that the return value corresponds to the types that were passed.
template <class CALLABLE>
void checkEm(const CALLABLE& call)
{
    ASSERT(kVoid          == call());

    ASSERT(kShort         == call(         (short)2));
    ASSERT(kUnsignedShort == call((unsigned short)2));
    ASSERT(kInt           == call(                2));
    ASSERT(kUnsignedInt   == call(      (unsigned)2));

    ASSERT(kFloat         == call(2.0f));
    ASSERT(kDouble        == call(2.0));
    ASSERT(kLongDouble    == call(2.0L));

    ASSERT(kStringView    == call(string_view("2.0")));
}

/// Call the `operator()` of the specified `call` the specified `p` and
/// various types, and ensure that the return value corresponds to the types
/// that were passed.
template <class CALLABLE, class PARAM>
void checkEm(const CALLABLE& call, PARAM p)
{
    ASSERT(kVoid          == call(p));

    ASSERT(kShort         == call(p,          (short)2));
    ASSERT(kUnsignedShort == call(p, (unsigned short)2));
    ASSERT(kInt           == call(p,                 2));
    ASSERT(kUnsignedInt   == call(p,       (unsigned)2));

    ASSERT(kFloat         == call(p, 2.0f));
    ASSERT(kDouble        == call(p, 2.0));
    ASSERT(kLongDouble    == call(p, 2.0L));

    ASSERT(kStringView    == call(p, string_view("2.0")));
}
#endif

//=============================================================================
//                             USAGE EXAMPLES
//-----------------------------------------------------------------------------

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

    (void)            verbose;  // unused variable warning
    (void)        veryVerbose;  // unused variable warning
    (void)    veryVeryVerbose;  // unused variable warning
    (void)veryVeryVeryVerbose;  // unused variable warning

    cout << "TEST " << __FILE__ << " CASE " << test << endl;
    ASSERT(true);  // silence `unused function` warning on C++11

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, replace
        //    leading comment characters with spaces, and replace `assert` with
        //    `ASSERT`.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

        // First, create a bsl::variant object that can contain several different
        // types.
        // ```
        bsl::variant<unsigned, double, bsl::string> v;
        // ```
        // Next, Create an Overload object containing several options:
        // ```
        bdlf::Overloaded over{
              [](unsigned)           {return 1;}
            , [](double)             {return 2;}
            , [](const bsl::string&) {return 3;}
            };
        // ```
        // Set the value of variant, and then call std::visit, passing the
        // overload set and the variant.  Check the return value to see that
        // the right lambda was called.
        // ```
        v = 2U;
        ASSERT(1 == bsl::visit(over, v));
        v = 2.0;
        ASSERT(2 == bsl::visit(over, v));
        v = bsl::string("2.0");
        ASSERT(3 == bsl::visit(over, v));
        // ```

#endif
      } break;


      case 4: {
        // --------------------------------------------------------------------
        // WORKING WITH `VARIANT`
        //
        // Concerns:
        // 1. `bdlf::Overloaded` works with `std::visit`
        //
        // Plan:
        // 1. Create a `std::variant` that can hold several types. Call
        //    std::visit on that variant, passing a `bdlf::Overloaded` object
        //    and verify the results (C-1)
        //
        // Testing:
        //   WORKING WITH `VARIANT`
        // --------------------------------------------------------------------
#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD

        if (verbose) cout << endl
                    << "WORKING WITH `VARIANT`" << endl
                    << "======================" << endl;


        {   // using bsl::variant
            typedef bsl::variant<short, unsigned short,
                                 int,   unsigned int,
                                 float, double, long double,
                                 string_view>   V;

            V                v;
            bdlf::Overloaded over{
                  [](short)          {return kShort;}
                , [](unsigned short) {return kUnsignedShort;}
                , [](int)            {return kInt;}
                , [](unsigned int)   {return kUnsignedInt;}

                , [](float)          {return kFloat;}
                , [](double)         {return kDouble;}
                , [](long double)    {return kLongDouble;}

                , [](string_view)  {return kStringView;}
            };

            v = (short) 2;
            ASSERT(kShort == bsl::visit(over, v));

            v = (unsigned short) 2;
            ASSERT(kUnsignedShort == bsl::visit(over, v));

            v = 2;
            ASSERT(kInt == bsl::visit(over, v));

            v = (unsigned) 2;
            ASSERT(kUnsignedInt == bsl::visit(over, v));

            v = 2.0f;
            ASSERT(kFloat == bsl::visit(over, v));

            v = 2.0;
            ASSERT(kDouble == bsl::visit(over, v));

            v = 2.0L;
            ASSERT(kLongDouble == bsl::visit(over, v));

            v = string_view("2.0");
            ASSERT(kStringView == bsl::visit(over, v));
        }

        {   // using std::variant
            typedef std::variant<short, unsigned short,
                                 int,   unsigned int,
                                 float, double, long double,
                                 string_view>   V;

            V                v;
            bdlf::Overloaded over{
                  [](short)          {return kShort;}
                , [](unsigned short) {return kUnsignedShort;}
                , [](int)            {return kInt;}
                , [](unsigned int)   {return kUnsignedInt;}

                , [](float)          {return kFloat;}
                , [](double)         {return kDouble;}
                , [](long double)    {return kLongDouble;}

                , [](string_view)  {return kStringView;}
            };

            v = (short) 2;
            ASSERT(kShort == std::visit(over, v));

            v = (unsigned short) 2;
            ASSERT(kUnsignedShort == std::visit(over, v));

            v = 2;
            ASSERT(kInt == std::visit(over, v));

            v = (unsigned) 2;
            ASSERT(kUnsignedInt == std::visit(over, v));

            v = 2.0f;
            ASSERT(kFloat == std::visit(over, v));

            v = 2.0;
            ASSERT(kDouble == std::visit(over, v));

            v = 2.0L;
            ASSERT(kLongDouble == std::visit(over, v));

            v = string_view("2.0");
            ASSERT(kStringView == std::visit(over, v));
        }

#endif
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TEMPLATED FREE FUNCTIONS AND MEMBER FUNCTIONS
        //
        // Concerns:
        // 1. We can create overload sets containing templated free functions,
        //   templated static member functions, and templated non-static member
        //   functions.
        //
        // Plan:
        // 1. Create overload sets from a various combinations of free
        //    functions, static member functions, and non-static member
        //    functions (C-1).
        //
        // 2. Call the overloads with a variety of types as the (single)
        //    parameter, and inspect the result of the call to verify that the
        //    correct option was invoked.
        //
        // Testing:
        //   TEMPLATED FREE FUNCTIONS AND MEMBER FUNCTIONS
        // --------------------------------------------------------------------
#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD

        if (verbose) cout << endl
            << "TEMPLATED FREE FUNCTIONS AND MEMBER FUNCTIONS" << endl
            << "=============================================" << endl;


        //  You can use classes that have a templated `operator()`
# if defined(BSLS_COMPILERFEATURES_SUPPORT_STATIC_CALL_OPERATOR)
#   ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#       pragma GCC diagnostic push
#       pragma GCC diagnostic ignored "-Wc++23-extensions"
#   endif
        checkEm(bdlf::Overloaded{staticTemplateMF()}); // works with operator()
#   ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#       pragma GCC diagnostic pop
#   endif
# endif
        checkEm(bdlf::Overloaded{TemplateMF()});       // works with operator()

        // You cannot take the address of a templated function (member or free)
        // - no such address exists.  You _can_ take a function of a specific
        // instantiation of such a function, though.

        //  auto over3 = Overloaded{&FreeFN::TemplateFN};   // fails
        auto over3 = bdlf::Overloaded{
              [](void)           {return kVoid;}
            , &FreeFN::TemplateFN<short>
            , &FreeFN::TemplateFN<unsigned short>
            , &FreeFN::TemplateFN<int>
            , &FreeFN::TemplateFN<unsigned int>
            , &FreeFN::TemplateFN<float>
            , &FreeFN::TemplateFN<double>
            , &FreeFN::TemplateFN<long double>
            , &FreeFN::TemplateFN<string_view>
            };
        checkEm(over3);

        //  auto over4 = Overloaded{&staticTemplateMF::MemberFN};   // fails
        auto over4 = bdlf::Overloaded{
              [](void)           {return kVoid;}
            , &staticTemplateMF::MemberFN<short>
            , &staticTemplateMF::MemberFN<unsigned short>
            , &staticTemplateMF::MemberFN<int>
            , &staticTemplateMF::MemberFN<unsigned int>
            , &staticTemplateMF::MemberFN<float>
            , &staticTemplateMF::MemberFN<double>
            , &staticTemplateMF::MemberFN<long double>
            , &staticTemplateMF::MemberFN<string_view>
            };
        checkEm(over4);

        //  auto over5 = Overloaded{&TemplateMF::MemberFN}; // fails
        auto over5 = bdlf::Overloaded{
              &TemplateMF::voidMemberFN
            , &TemplateMF::MemberFN<short>
            , &TemplateMF::MemberFN<unsigned short>
            , &TemplateMF::MemberFN<int>
            , &TemplateMF::MemberFN<unsigned int>
            , &TemplateMF::MemberFN<float>
            , &TemplateMF::MemberFN<double>
            , &TemplateMF::MemberFN<long double>
            , &TemplateMF::MemberFN<string_view>
            };

        const TemplateMF tempMF;
        checkEm(over5, &tempMF);

#endif  // BSLS_COMPILERFEATURES_SUPPORT_CTAD
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // FREE FUNCTIONS AND MEMBER FUNCTIONS
        //
        // Concerns:
        // 1. We can create overload sets containing free functions, static
        //    member functions, and non-static member functions.
        //
        // 2. The `no-except`-ness of a member function is reflected in the
        //    overload set that contains it.
        //
        // Plan:
        // 1. Create overload sets from a various combinations of free
        //    functions, static member functions, and non-static member
        //    functions (C-1).
        //
        // 2. Call the overloads with a variety of types as the (single)
        //    parameter, and inspect the result of the call to verify that the
        //    correct option was invoked.
        //
        // 3. Test that a call to an overload set that would call a noexcept
        //    function is actually noexcept, and the converse.
        //    useless). (C-2)
        //
        // Testing:
        //   FREE FUNCTIONS AND MEMBER FUNCTIONS
        // --------------------------------------------------------------------
#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD

        if (verbose) cout << endl
                 << "FREE FUNCTIONS AND MEMBER FUNCTIONS" << endl
                 << "===================================" << endl;

        int someValue = 3;

        checkEm(bdlf::Overloaded{
              [&someValue](void)  {  (void)someValue;  return kVoid;  }
            , FloatingPoint{}
            , FreeFN::HandleShort
            , FreeFN::HandleUnsignedShort
            , StaticMF::HandleIntMF
            , FreeFN::HandleUnsignedInt

            , [](string_view)  {return kStringView;}
            });

        // I would like the line below marked `does not work; see above` to
        // work, where we add a const member function to an overload set that
        // gets passed a non-const object pointer.  It *could* work, but not in
        // this case.  In the overload set `over2`, if we call with the
        // parameters `(NonStaticMF *, int)`, the following functions in the
        // overload set are equally good (and result in a `call is ambiguous`
        // error):
        // ```
        //      const NonStaticMF *, int
        //            NonStaticMF *, float
        //            NonStaticMF *, double
        //            NonStaticMF *, long double
        // ```
        // but if I pass something for the second parameter that doesn't have a
        // lot of conversions, like a string_view, then the conversion from
        // `NonStaticMF *` to a `const NonStaticMF *` doesn't open the door to
        // other members of the overload.  That example is marked as
        // `see above (2)`.
        auto over1 = bdlf::Overloaded{
              [](NonStaticMF *)                 {return kVoid;}
            , &NonStaticMF::HandleShortMF
            , &NonStaticMF::HandleUnsignedShortMF
            , &NonStaticMF::NHandleIntMF    // noexcept
    //      , &NonStaticMF::CHandleIntMF    // does not work; see comment above
            , [](NonStaticMF *, unsigned int)   {return kUnsignedInt;}

            , [](NonStaticMF *, float)          {return kFloat;}
            , [](NonStaticMF *, double)         {return kDouble;}
            , [](NonStaticMF *, long double)    {return kLongDouble;}

            , &NonStaticMF::CHandleStringViewMF // see above (2)
            };

        auto over2 = bdlf::Overloaded{
              [](const NonStaticMF *)                 {return kVoid;}
            , &NonStaticMF::CHandleShortMF
            , &NonStaticMF::CHandleUnsignedShortMF
            , [](const NonStaticMF *, int)            {return kInt;}
            , &NonStaticMF::CNHandleUnsignedIntMF

            , [](const NonStaticMF *, float)          {return kFloat;}
            , [](const NonStaticMF *, double)         {return kDouble;}
            , [](const NonStaticMF *, long double)    {return kLongDouble;}

            , &NonStaticMF::CHandleStringViewMF
            };

            NonStaticMF        obj;
            NonStaticMF       *p  = &obj;
            const NonStaticMF *cp = &obj;

            checkEm(over1, p);
            checkEm(over2, cp);

        //  Check to see that noexcept propagates
            ASSERT(!noexcept(over1 (p, 2.0))); // a lambda
            ASSERT( noexcept(over1 (p, 2)));   // NonStaticMF::NHandleIntMF

#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        // 1. Users can create a `bdlf::Overloaded` from a sequence of lambda
        //    expressions.
        //
        // 2. Users can also use structures containing one or more
        //    `operator()` members when constructing the `bdlf::Overloaded`.
        //
        // 3. When the Overloaded object is called, the correct lambda is
        //    invoked.
        //
        // Plan:
        // 1. Create the overloads from the sources mentioned above (C-1, 2)
        //
        // 2. Call the overloads with a variety of types as the (single)
        //    parameter, and inspect the result of the call to verify that the
        //    correct option was invoked. (C-3)
        //
        // Testing:
        //   BREATHING TEST
        //   Overloaded::Overloaded(...)
        //   Overloaded::operator()(...)
        // --------------------------------------------------------------------
#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        // Build an overload set from a series of lambdas.  This is expected to
        // be the most common usage.
        checkEm(bdlf::Overloaded{
              [](void)           {return kVoid;}

            , [](short)          {return kShort;}
            , [](unsigned short) {return kUnsignedShort;}
            , [](int)            {return kInt;}
            , [](unsigned int)   {return kUnsignedInt;}

            , [](float)          {return kFloat;}
            , [](double)         {return kDouble;}
            , [](long double)    {return kLongDouble;}

            , [](string_view)  {return kStringView;}
            });

        // Build an overload set from a series of lambdas and a struct that
        // has an `operator()`
        checkEm(bdlf::Overloaded{
              [](void)           {return kVoid;}
            , FloatingPoint{}
            , [](short)          {return kShort;}
            , [](unsigned short) {return kUnsignedShort;}
            , [](int)            {return kInt;}
            , [](unsigned int)   {return kUnsignedInt;}

            , [](string_view)  {return kStringView;}
            });

#endif
      } break;
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
// Copyright 2025 Bloomberg Finance L.P.
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
