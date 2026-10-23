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
// The component under test is a class template, `bdlf::Overloaded`, whose
// objects form an overload set from a sequence of callables.  We verify that
// overload sets can be built from various kinds of callables, and that
// invoking one calls the callable selected by overload resolution.
//-----------------------------------------------------------------------------
// CREATORS
// [1] Overloaded::Overloaded(...)
// [2] Overloaded::Overloaded(...)
// [3] Overloaded::Overloaded(...)
// [5] Overloaded::Overloaded(...)
//
// ACCESSORS
// [1] Overloaded::operator()(...)
// [2] Overloaded::operator()(...)
// [3] Overloaded::operator()(...)
// [4] Overloaded::operator()(...)
// [5] Overloaded::operator()(...)
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [6] USAGE EXAMPLE

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
    e_VOID,

    e_SHORT,
    e_UNSIGNED_SHORT,
    e_INT,
    e_UNSIGNED_INT,

    e_FLOAT,
    e_DOUBLE,
    e_LONG_DOUBLE,

    e_STRING_VIEW,
    e_END_OF_LIST
};

//  -- operator ()
struct FloatingPoint {
    int operator()(float)       const {return e_FLOAT;}
    int operator()(double)      const {return e_DOUBLE;}
    int operator()(long double) const {return e_LONG_DOUBLE;}
};

//  ----- free functions

/// A collection of functions that take a single parameter, and return an
/// enum value corresponding to the type that they were called with.
struct FreeFN {
    static int HandleShort                 (short) { return e_SHORT; }
    static int HandleUnsignedShort(unsigned short) { return e_UNSIGNED_SHORT; }
    static int HandleInt                     (int) { return e_INT; }
    static int HandleUnsignedInt    (unsigned int) { return e_UNSIGNED_INT; }

    // The same handlers, taking their argument by reference.
    static int HandleShortRef        (const short&)  { return e_SHORT; }
    static int HandleUnsignedShortRef(const unsigned short&)
    {
        return e_UNSIGNED_SHORT;
    }
    static int HandleDoubleRef       (const double&) { return e_DOUBLE; }

    static int TemplateFN() { return e_VOID; }

    // A templated function that returns an enum value corresponding to the
    // type that they were instantiated  with.
    template <class TYPE>
    static int TemplateFN(TYPE) {
        if (std::is_same_v<TYPE, short>)            return e_SHORT;
        if (std::is_same_v<TYPE, unsigned short>)   return e_UNSIGNED_SHORT;
        if (std::is_same_v<TYPE, int>)              return e_INT;
        if (std::is_same_v<TYPE, unsigned int>)     return e_UNSIGNED_INT;
        if (std::is_same_v<TYPE, float>)            return e_FLOAT;
        if (std::is_same_v<TYPE, double>)           return e_DOUBLE;
        if (std::is_same_v<TYPE, long double>)      return e_LONG_DOUBLE;
        if (std::is_same_v<TYPE, string_view>)      return e_STRING_VIEW;
        return -1;  // some other type
    }
};

//  ----- member functions

/// A structure containing a set of static member functions that take the
/// types that we are testing, and return the enum corresponding to the
/// type.
struct StaticMF {
    static int HandleIntMF (int) { return e_INT;}
};

/// A structure containing a set of member functions that take the types
/// that we are testing, and return the enum corresponding to the type.
struct NonStaticMF {

    int HandleShortMF          (short)                { return e_SHORT;}
    int HandleUnsignedShortMF  (unsigned short)       {
        return e_UNSIGNED_SHORT;
    }
    int CHandleShortMF         (short)          const { return e_SHORT;}
    int CHandleUnsignedShortMF (unsigned short) const {
        return e_UNSIGNED_SHORT;
    }

    int CHandleIntMF           (int)            const { return e_INT;}
    int NHandleIntMF           (int)                  noexcept { return e_INT;}
    int CNHandleUnsignedIntMF  (unsigned int)   const noexcept {
        return e_UNSIGNED_INT;
    }

    int CHandleStringViewMF    (string_view)    const { return e_STRING_VIEW;}
};

//  ----- templated member functions
struct staticTemplateMF {

# if defined(BSLS_COMPILERFEATURES_SUPPORT_STATIC_CALL_OPERATOR)
    // Declaring overloaded `operator()` as `static` is a C++23 feature
    // backported as an extension by Clang.
#   ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#       pragma GCC diagnostic push
#       pragma GCC diagnostic ignored "-Wc++23-extensions"
#   endif
    static int operator()() { return e_VOID; }

    template <class TYPE>
    static int operator()(TYPE) noexcept {
        if (bsl::is_same_v<TYPE, short>)            return e_SHORT;
        if (bsl::is_same_v<TYPE, unsigned short>)   return e_UNSIGNED_SHORT;
        if (bsl::is_same_v<TYPE, int>)              return e_INT;
        if (bsl::is_same_v<TYPE, unsigned int>)     return e_UNSIGNED_INT;
        if (bsl::is_same_v<TYPE, float>)            return e_FLOAT;
        if (bsl::is_same_v<TYPE, double>)           return e_DOUBLE;
        if (bsl::is_same_v<TYPE, long double>)      return e_LONG_DOUBLE;
        if (bsl::is_same_v<TYPE, string_view>)      return e_STRING_VIEW;
        return -1;  // some other type
    }
#   ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#       pragma GCC diagnostic pop
#   endif
# endif  // BSLS_COMPILERFEATURES_SUPPORT_STATIC_CALL_OPERATOR

    static int MemberFN() { return e_VOID; }

    template <class TYPE>
    static int MemberFN(TYPE) noexcept {
        if (bsl::is_same_v<TYPE, short>)            return e_SHORT;
        if (bsl::is_same_v<TYPE, unsigned short>)   return e_UNSIGNED_SHORT;
        if (bsl::is_same_v<TYPE, int>)              return e_INT;
        if (bsl::is_same_v<TYPE, unsigned int>)     return e_UNSIGNED_INT;
        if (bsl::is_same_v<TYPE, float>)            return e_FLOAT;
        if (bsl::is_same_v<TYPE, double>)           return e_DOUBLE;
        if (bsl::is_same_v<TYPE, long double>)      return e_LONG_DOUBLE;
        if (bsl::is_same_v<TYPE, string_view>)      return e_STRING_VIEW;
        return -1;  // some other type
    }
};

/// A structure containing a templated `operator()` member function that
/// returns the  enum corresponding to the type that the function was
/// instantiated with.  It also contains a member function `MemberFN` that
/// does the same thing.
struct TemplateMF {

    int operator()() const { return e_VOID; }

    template <class TYPE>
    int operator()(TYPE) const noexcept {
        if (bsl::is_same_v<TYPE, short>)            return e_SHORT;
        if (bsl::is_same_v<TYPE, unsigned short>)   return e_UNSIGNED_SHORT;
        if (bsl::is_same_v<TYPE, int>)              return e_INT;
        if (bsl::is_same_v<TYPE, unsigned int>)     return e_UNSIGNED_INT;
        if (bsl::is_same_v<TYPE, float>)            return e_FLOAT;
        if (bsl::is_same_v<TYPE, double>)           return e_DOUBLE;
        if (bsl::is_same_v<TYPE, long double>)      return e_LONG_DOUBLE;
        if (bsl::is_same_v<TYPE, string_view>)      return e_STRING_VIEW;
        return -1;  // some other type
    }


    int voidMemberFN() const { return e_VOID; }

    template <class TYPE>
    int MemberFN(TYPE) const noexcept {
        if (bsl::is_same_v<TYPE, short>)            return e_SHORT;
        if (bsl::is_same_v<TYPE, unsigned short>)   return e_UNSIGNED_SHORT;
        if (bsl::is_same_v<TYPE, int>)              return e_INT;
        if (bsl::is_same_v<TYPE, unsigned int>)     return e_UNSIGNED_INT;
        if (bsl::is_same_v<TYPE, float>)            return e_FLOAT;
        if (bsl::is_same_v<TYPE, double>)           return e_DOUBLE;
        if (bsl::is_same_v<TYPE, long double>)      return e_LONG_DOUBLE;
        if (bsl::is_same_v<TYPE, string_view>)      return e_STRING_VIEW;
        return -1;  // some other type
    }
};


/// Call the `operator()` of the specified `call` with various types, and
/// ensure that the return value corresponds to the types that were passed.
template <class CALLABLE>
void checkEm(const CALLABLE& call)
{
    ASSERT(e_VOID           == call());

    ASSERT(e_SHORT          == call(         (short)2));
    ASSERT(e_UNSIGNED_SHORT == call((unsigned short)2));
    ASSERT(e_INT            == call(                2));
    ASSERT(e_UNSIGNED_INT   == call(      (unsigned)2));

    ASSERT(e_FLOAT          == call(2.0f));
    ASSERT(e_DOUBLE         == call(2.0));
    ASSERT(e_LONG_DOUBLE    == call(2.0L));

    ASSERT(e_STRING_VIEW    == call(string_view("2.0")));

    short          lvShort       = 2;
    unsigned short lvUShort      = 2;
    int            lvInt         = 2;
    unsigned int   lvUInt        = 2;

    float          lvFloat       = 2.0f;
    double         lvDouble      = 2.0;
    long double    lvLongDouble  = 2.0L;

    string_view    lvStringView("2.0");

    ASSERT(e_SHORT          == call(lvShort));
    ASSERT(e_UNSIGNED_SHORT == call(lvUShort));
    ASSERT(e_INT            == call(lvInt));
    ASSERT(e_UNSIGNED_INT   == call(lvUInt));

    ASSERT(e_FLOAT          == call(lvFloat));
    ASSERT(e_DOUBLE         == call(lvDouble));
    ASSERT(e_LONG_DOUBLE    == call(lvLongDouble));

    ASSERT(e_STRING_VIEW    == call(lvStringView));

    const short&          clvShort      = lvShort;
    const unsigned short& clvUShort     = lvUShort;
    const int&            clvInt        = lvInt;
    const unsigned int&   clvUInt       = lvUInt;

    const float&          clvFloat      = lvFloat;
    const double&         clvDouble     = lvDouble;
    const long double&    clvLongDouble = lvLongDouble;

    const string_view&    clvStringView = lvStringView;

    ASSERT(e_SHORT          == call(clvShort));
    ASSERT(e_UNSIGNED_SHORT == call(clvUShort));
    ASSERT(e_INT            == call(clvInt));
    ASSERT(e_UNSIGNED_INT   == call(clvUInt));

    ASSERT(e_FLOAT          == call(clvFloat));
    ASSERT(e_DOUBLE         == call(clvDouble));
    ASSERT(e_LONG_DOUBLE    == call(clvLongDouble));

    ASSERT(e_STRING_VIEW    == call(clvStringView));
}

/// Call the `operator()` of the specified `call` with the specified
/// `objectPtr` and various types, and ensure that the return value
/// corresponds to the types that were passed.
template <class CALLABLE, class PARAM>
void checkEm(const CALLABLE& call, PARAM objectPtr)
{
    ASSERT(e_VOID           == call(objectPtr));

    ASSERT(e_SHORT          == call(objectPtr,          (short)2));
    ASSERT(e_UNSIGNED_SHORT == call(objectPtr, (unsigned short)2));
    ASSERT(e_INT            == call(objectPtr,                 2));
    ASSERT(e_UNSIGNED_INT   == call(objectPtr,       (unsigned)2));

    ASSERT(e_FLOAT          == call(objectPtr, 2.0f));
    ASSERT(e_DOUBLE         == call(objectPtr, 2.0));
    ASSERT(e_LONG_DOUBLE    == call(objectPtr, 2.0L));

    ASSERT(e_STRING_VIEW    == call(objectPtr, string_view("2.0")));

    short          lvShort       = 2;
    unsigned short lvUShort      = 2;
    int            lvInt         = 2;
    unsigned int   lvUInt        = 2;

    float          lvFloat       = 2.0f;
    double         lvDouble      = 2.0;
    long double    lvLongDouble  = 2.0L;

    string_view    lvStringView("2.0");

    ASSERT(e_SHORT          == call(objectPtr, lvShort));
    ASSERT(e_UNSIGNED_SHORT == call(objectPtr, lvUShort));
    ASSERT(e_INT            == call(objectPtr, lvInt));
    ASSERT(e_UNSIGNED_INT   == call(objectPtr, lvUInt));

    ASSERT(e_FLOAT          == call(objectPtr, lvFloat));
    ASSERT(e_DOUBLE         == call(objectPtr, lvDouble));
    ASSERT(e_LONG_DOUBLE    == call(objectPtr, lvLongDouble));

    ASSERT(e_STRING_VIEW    == call(objectPtr, lvStringView));

    const short&          clvShort      = lvShort;
    const unsigned short& clvUShort     = lvUShort;
    const int&            clvInt        = lvInt;
    const unsigned int&   clvUInt       = lvUInt;

    const float&          clvFloat      = lvFloat;
    const double&         clvDouble     = lvDouble;
    const long double&    clvLongDouble = lvLongDouble;

    const string_view&    clvStringView = lvStringView;

    ASSERT(e_SHORT          == call(objectPtr, clvShort));
    ASSERT(e_UNSIGNED_SHORT == call(objectPtr, clvUShort));
    ASSERT(e_INT            == call(objectPtr, clvInt));
    ASSERT(e_UNSIGNED_INT   == call(objectPtr, clvUInt));

    ASSERT(e_FLOAT          == call(objectPtr, clvFloat));
    ASSERT(e_DOUBLE         == call(objectPtr, clvDouble));
    ASSERT(e_LONG_DOUBLE    == call(objectPtr, clvLongDouble));

    ASSERT(e_STRING_VIEW    == call(objectPtr, clvStringView));
}

//=============================================================================
//                             USAGE EXAMPLES
//-----------------------------------------------------------------------------

namespace usage_example_1 {

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// First, create a bsl::variant object that can contain several different
// types.
// ```
void run()
{
    bsl::variant<unsigned, double, bsl::string> v;
// ```
// Next, Create an Overload object containing several options:
// ```
    bdlf::Overloaded over{
          [] (unsigned)           {return 1;}
        , [] (double)             {return 2;}
        , [] (const bsl::string&) {return 3;}
        };
// ```
// Set the value of variant, and then call std::visit, passing the overload set
// and the variant.  Check the return value to see that the right lambda was
// called.
// ```
    v = 2U;
    ASSERT(1 == bsl::visit(over, v));
    v = 2.0;
    ASSERT(2 == bsl::visit(over, v));
    v = bsl::string("2.0");
    ASSERT(3 == bsl::visit(over, v));
}
// ```

}  // close namespace usage_example_1
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

    (void)            verbose;  // unused variable warning
    (void)        veryVerbose;  // unused variable warning
    (void)    veryVeryVerbose;  // unused variable warning
    (void)veryVeryVeryVerbose;  // unused variable warning

    cout << "TEST " << __FILE__ << " CASE " << test << endl;
    ASSERT(true);  // silence `unused function` warning on C++11

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
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

        usage_example_1::run();
#endif
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // FUNCTION POINTERS WITH REFERENCE PARAMETERS
        //
        // Concerns:
        // 1. An overload set calls the function that would be selected by
        //    overload resolution for the supplied argument when every
        //    function in the set takes its argument by reference.
        //
        // 2. An overload set calls the function that would be selected by
        //    overload resolution for the supplied argument when the set mixes
        //    functions taking their argument by value with functions taking
        //    it by reference.
        //
        // 3. An overload set calls the function that would be selected by
        //    overload resolution for the supplied argument when every
        //    function takes its argument by value.
        //
        // Plan:
        // 1. Create an overload set from function pointers whose parameters
        //    are all references, call it directly with an rvalue, a
        //    modifiable lvalue, and a non-modifiable lvalue of each parameter
        //    type, and verify that the return value identifies the function
        //    that was called.  (C-1)
        //
        // 2. Repeat for an overload set built from one function taking its
        //    argument by value and one taking its argument by reference.
        //    (C-2)
        //
        // 3. Repeat for an overload set whose functions all take their
        //    argument by value.  (C-3)
        //
        // Testing:
        //   Overloaded::Overloaded(...)
        //   Overloaded::operator()(...)
        // --------------------------------------------------------------------
#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD

        if (verbose) cout << endl
                 << "FUNCTION POINTERS WITH REFERENCE PARAMETERS" << endl
                 << "===========================================" << endl;

        {
            bdlf::Overloaded over{&FreeFN::HandleShortRef,
                                  &FreeFN::HandleUnsignedShortRef};

            short          lvShort  = 2;
            unsigned short lvUShort = 2;

            const short&          clvShort  = lvShort;
            const unsigned short& clvUShort = lvUShort;

            ASSERT(e_SHORT          == over(         (short)2));
            ASSERT(e_UNSIGNED_SHORT == over((unsigned short)2));

            ASSERT(e_SHORT          == over(lvShort));
            ASSERT(e_UNSIGNED_SHORT == over(lvUShort));

            ASSERT(e_SHORT          == over(clvShort));
            ASSERT(e_UNSIGNED_SHORT == over(clvUShort));
        }

        {
            bdlf::Overloaded over{&FreeFN::HandleInt,
                                  &FreeFN::HandleDoubleRef};

            int    lvInt    = 2;
            double lvDouble = 2.0;

            const int&    clvInt    = lvInt;
            const double& clvDouble = lvDouble;

            ASSERT(e_INT    == over(2));
            ASSERT(e_DOUBLE == over(2.0));

            ASSERT(e_INT    == over(lvInt));
            ASSERT(e_DOUBLE == over(lvDouble));

            ASSERT(e_INT    == over(clvInt));
            ASSERT(e_DOUBLE == over(clvDouble));
        }

        {
            bdlf::Overloaded over{&FreeFN::HandleInt,
                                  &FreeFN::HandleShort};

            int   lvInt   = 2;
            short lvShort = 2;

            const int&   clvInt   = lvInt;
            const short& clvShort = lvShort;

            ASSERT(e_INT   == over(2));
            ASSERT(e_SHORT == over((short)2));

            ASSERT(e_INT   == over(lvInt));
            ASSERT(e_SHORT == over(lvShort));

            ASSERT(e_INT   == over(clvInt));
            ASSERT(e_SHORT == over(clvShort));
        }

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
        //   Overloaded::operator()(...)
        // --------------------------------------------------------------------
#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD

        if (verbose) cout << endl
                    << "WORKING WITH `VARIANT`" << endl
                    << "======================" << endl;


        {   // using bsl::variant
            typedef bsl::variant<short, unsigned short,
                                 int,   unsigned int,
                                 float, double, long double,
                                 string_view>   BslVariant;

            BslVariant       bslVariant;
            bdlf::Overloaded over{
                  [](short)          {return e_SHORT;}
                , [](unsigned short) {return e_UNSIGNED_SHORT;}
                , [](int)            {return e_INT;}
                , [](unsigned int)   {return e_UNSIGNED_INT;}

                , [](float)          {return e_FLOAT;}
                , [](double)         {return e_DOUBLE;}
                , [](long double)    {return e_LONG_DOUBLE;}

                , [](string_view)  {return e_STRING_VIEW;}
            };

            bslVariant = (short) 2;
            ASSERT(e_SHORT == bsl::visit(over, bslVariant));

            bslVariant = (unsigned short) 2;
            ASSERT(e_UNSIGNED_SHORT == bsl::visit(over, bslVariant));

            bslVariant = 2;
            ASSERT(e_INT == bsl::visit(over, bslVariant));

            bslVariant = (unsigned) 2;
            ASSERT(e_UNSIGNED_INT == bsl::visit(over, bslVariant));

            bslVariant = 2.0f;
            ASSERT(e_FLOAT == bsl::visit(over, bslVariant));

            bslVariant = 2.0;
            ASSERT(e_DOUBLE == bsl::visit(over, bslVariant));

            bslVariant = 2.0L;
            ASSERT(e_LONG_DOUBLE == bsl::visit(over, bslVariant));

            bslVariant = string_view("2.0");
            ASSERT(e_STRING_VIEW == bsl::visit(over, bslVariant));
        }

        {   // using std::variant
            typedef std::variant<short, unsigned short,
                                 int,   unsigned int,
                                 float, double, long double,
                                 string_view>   StdVariant;

            StdVariant       stdVariant;
            bdlf::Overloaded over{
                  [](short)          {return e_SHORT;}
                , [](unsigned short) {return e_UNSIGNED_SHORT;}
                , [](int)            {return e_INT;}
                , [](unsigned int)   {return e_UNSIGNED_INT;}

                , [](float)          {return e_FLOAT;}
                , [](double)         {return e_DOUBLE;}
                , [](long double)    {return e_LONG_DOUBLE;}

                , [](string_view)  {return e_STRING_VIEW;}
            };

            stdVariant = (short) 2;
            ASSERT(e_SHORT == std::visit(over, stdVariant));

            stdVariant = (unsigned short) 2;
            ASSERT(e_UNSIGNED_SHORT == std::visit(over, stdVariant));

            stdVariant = 2;
            ASSERT(e_INT == std::visit(over, stdVariant));

            stdVariant = (unsigned) 2;
            ASSERT(e_UNSIGNED_INT == std::visit(over, stdVariant));

            stdVariant = 2.0f;
            ASSERT(e_FLOAT == std::visit(over, stdVariant));

            stdVariant = 2.0;
            ASSERT(e_DOUBLE == std::visit(over, stdVariant));

            stdVariant = 2.0L;
            ASSERT(e_LONG_DOUBLE == std::visit(over, stdVariant));

            stdVariant = string_view("2.0");
            ASSERT(e_STRING_VIEW == std::visit(over, stdVariant));
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
        //   Overloaded::Overloaded(...)
        //   Overloaded::operator()(...)
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
              [](void)           {return e_VOID;}
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
              [](void)           {return e_VOID;}
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
        //   Overloaded::Overloaded(...)
        //   Overloaded::operator()(...)
        // --------------------------------------------------------------------
#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD

        if (verbose) cout << endl
                 << "FREE FUNCTIONS AND MEMBER FUNCTIONS" << endl
                 << "===================================" << endl;

        int someValue = 3;

        checkEm(bdlf::Overloaded{
              [&someValue](void)  {  (void)someValue;  return e_VOID;  }
            , FloatingPoint{}
            , FreeFN::HandleShort
            , FreeFN::HandleUnsignedShort
            , StaticMF::HandleIntMF
            , FreeFN::HandleUnsignedInt

            , [](string_view)  {return e_STRING_VIEW;}
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
              [](NonStaticMF *)                 {return e_VOID;}
            , &NonStaticMF::HandleShortMF
            , &NonStaticMF::HandleUnsignedShortMF
            , &NonStaticMF::NHandleIntMF    // noexcept
    //      , &NonStaticMF::CHandleIntMF    // does not work; see comment above
            , [](NonStaticMF *, unsigned int)   {return e_UNSIGNED_INT;}

            , [](NonStaticMF *, float)          {return e_FLOAT;}
            , [](NonStaticMF *, double)         {return e_DOUBLE;}
            , [](NonStaticMF *, long double)    {return e_LONG_DOUBLE;}

            , &NonStaticMF::CHandleStringViewMF // see above (2)
            };

        auto over2 = bdlf::Overloaded{
              [](const NonStaticMF *)                 {return e_VOID;}
            , &NonStaticMF::CHandleShortMF
            , &NonStaticMF::CHandleUnsignedShortMF
            , [](const NonStaticMF *, int)            {return e_INT;}
            , &NonStaticMF::CNHandleUnsignedIntMF

            , [](const NonStaticMF *, float)          {return e_FLOAT;}
            , [](const NonStaticMF *, double)         {return e_DOUBLE;}
            , [](const NonStaticMF *, long double)    {return e_LONG_DOUBLE;}

            , &NonStaticMF::CHandleStringViewMF
            };

            NonStaticMF        object;
            NonStaticMF       *objectPtr      = &object;
            const NonStaticMF *constObjectPtr = &object;

            checkEm(over1, objectPtr);
            checkEm(over2, constObjectPtr);

        //  Check to see that noexcept propagates
            ASSERT(!noexcept(over1 (objectPtr, 2.0))); // a lambda
            ASSERT( noexcept(over1 (objectPtr, 2)));   // NHandleIntMF

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
              [](void)           {return e_VOID;}

            , [](short)          {return e_SHORT;}
            , [](unsigned short) {return e_UNSIGNED_SHORT;}
            , [](int)            {return e_INT;}
            , [](unsigned int)   {return e_UNSIGNED_INT;}

            , [](float)          {return e_FLOAT;}
            , [](double)         {return e_DOUBLE;}
            , [](long double)    {return e_LONG_DOUBLE;}

            , [](string_view)  {return e_STRING_VIEW;}
            });

        // Build an overload set from a series of lambdas and a struct that
        // has an `operator()`
        checkEm(bdlf::Overloaded{
              [](void)           {return e_VOID;}
            , FloatingPoint{}
            , [](short)          {return e_SHORT;}
            , [](unsigned short) {return e_UNSIGNED_SHORT;}
            , [](int)            {return e_INT;}
            , [](unsigned int)   {return e_UNSIGNED_INT;}

            , [](string_view)  {return e_STRING_VIEW;}
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
