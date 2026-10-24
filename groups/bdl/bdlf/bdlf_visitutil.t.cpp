// bdlf_visitutil.t.cpp                                               -*-C++-*-
#include <bdlf_visitutil.h>

#include <bslim_testutil.h>

#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>

#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_memory.h>
#include <bsl_string.h>
#include <bsl_string_view.h>
#include <bsl_type_traits.h>
#include <bsl_variant.h>

#include <utility>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
#include <variant>
#endif

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test provides two overloads of a function template that
// forward their arguments to `bsl::visit` or `std::visit`, wrapping the
// callables in a `bdlf::Overloaded` object.  One overload deduces its return
// type, and the other converts the result to an explicitly specified type.
// For both overloads we verify that the correct callable is selected, that the
// value category and cv-qualification of the variant are preserved, and that
// both `bsl::variant` and `std::variant`, as well as classes derived from
// them, are supported.  We also verify that the deducing overload preserves
// the return type of the selected callable, and that the other overload
// accepts callables with different return types and converts the result to
// the specified type, and that a call specifying the return type explicitly
// is not ambiguous even if the variant could bind to a parameter of that type.
//-----------------------------------------------------------------------------
// CLASS METHODS
// [ 2] decltype(auto) visitWith(t_VARIANT&&, t_CALLABLE&&, t_CALLABLES&&...);
// [ 2] t_RET visitWith<t_RET>(t_VARIANT&&, t_CALLABLE&&, t_CALLABLES&&...);
// [ 3] decltype(auto) visitWith(t_VARIANT&&, t_CALLABLE&&, t_CALLABLES&&...);
// [ 3] t_RET visitWith<t_RET>(t_VARIANT&&, t_CALLABLE&&, t_CALLABLES&&...);
// [ 4] decltype(auto) visitWith(t_VARIANT&&, t_CALLABLE&&, t_CALLABLES&&...);
// [ 4] t_RET visitWith<t_RET>(t_VARIANT&&, t_CALLABLE&&, t_CALLABLES&&...);
// [ 5] decltype(auto) visitWith(t_VARIANT&&, t_CALLABLE&&, t_CALLABLES&&...);
// [ 5] t_RET visitWith<t_RET>(t_VARIANT&&, t_CALLABLE&&, t_CALLABLES&&...);
// [ 6] decltype(auto) visitWith(t_VARIANT&&, t_CALLABLE&&, t_CALLABLES&&...);
// [ 6] t_RET visitWith<t_RET>(t_VARIANT&&, t_CALLABLE&&, t_CALLABLES&&...);
// [ 7] decltype(auto) visitWith(t_VARIANT&&, t_CALLABLE&&, t_CALLABLES&&...);
// [ 7] t_RET visitWith<t_RET>(t_VARIANT&&, t_CALLABLE&&, t_CALLABLES&&...);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 8] USAGE EXAMPLE

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

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
typedef bdlf::VisitUtil Util;

enum {
    e_INT,
    e_DOUBLE,
    e_STRING,
    e_FALLBACK,

    e_LVALUE,
    e_CONST_LVALUE,
    e_RVALUE
};

struct FreeFN {
    static int handleInt(int) { return e_INT; }

    static int handleDouble(double) { return e_DOUBLE; }
};

/// A callable type that is not copyable, used to verify that callables are
/// forwarded rather than copied.
struct MoveOnlyCallable {

    // DATA
    bsl::unique_ptr<int> d_value;

    // CREATORS
    explicit MoveOnlyCallable(int value)
    : d_value(new int(value))
    {
    }

    // ACCESSORS

    /// Return the value supplied at construction.
    int operator()(int) const { return *d_value; }
};

/// A callable type with a function call operator overloaded on the value
/// category and cv-qualification of its argument.
struct CategoryDetector {

    // ACCESSORS

    /// Return `e_LVALUE`.
    int operator()(bsl::string&) const { return e_LVALUE; }

    /// Return `e_CONST_LVALUE`.
    int operator()(const bsl::string&) const { return e_CONST_LVALUE; }

    /// Return `e_RVALUE`.
    int operator()(bsl::string&&) const { return e_RVALUE; }
};

/// A class derived from the (template parameter) `t_VARIANT`, used to verify
/// that classes derived from `bsl::variant` and `std::variant` are supported.
template <class t_VARIANT>
struct DerivedVariant : t_VARIANT {

    // CREATORS
    using t_VARIANT::t_VARIANT;
};

/// Verify that `Util::visitWith<long>` accepts callables returning `int` and
/// `long` when supplied with an object of the (template parameter)
/// `t_VARIANT` holding each alternative, and returns the result of the
/// selected callable converted to `long`.  Use the specified `line` to report
/// failures.  The behavior is undefined unless `t_VARIANT` has exactly the
/// alternatives `int` and `long`.
template <class t_VARIANT>
void testDifferentReturnTypes(int line)
{
    auto intFn  = [](int i)  { return i + 1; };
    auto longFn = [](long l) { return l * 2; };

    t_VARIANT intVariant(3);
    t_VARIANT longVariant(5L);

    typedef decltype(Util::visitWith<long>(intVariant, intFn, longFn)) Result;
    ASSERTV(line, (bsl::is_same<Result, long>::value));

    ASSERTV(line, 4  == Util::visitWith<long>(intVariant,  intFn, longFn));
    ASSERTV(line, 10 == Util::visitWith<long>(longVariant, intFn, longFn));
}

#ifdef BDE_BUILD_TARGET_EXC
/// A type whose value constructor always throws, used to put a variant into
/// the valueless-by-exception state.
struct ThrowingAlternative {

    // CREATORS

    /// Throw an exception of type `int`.
    explicit ThrowingAlternative(int)
    {
        throw 0;
    }

    /// Create a `ThrowingAlternative` object.  Note that this constructor is
    /// user-provided so that the type is not trivially copyable, which
    /// prevents `std::variant::emplace` from constructing a temporary before
    /// destroying the current alternative.
    ThrowingAlternative(const ThrowingAlternative&)
    {
    }
};

/// Put the specified `variant` into the valueless-by-exception state by
/// emplacing a `ThrowingAlternative`.  The behavior is undefined unless the
/// alternative at index 1 of `t_VARIANT` is `ThrowingAlternative`.
template <class t_VARIANT>
void makeValueless(t_VARIANT *variant)
{
    try {
        variant->template emplace<1>(0);
    }
    catch (int) {
    }
}

/// Verify that both overloads of `Util::visitWith` throw `bad_variant_access`
/// without invoking the callable when supplied with a valueless object of the
/// (template parameter) `t_VARIANT` as a modifiable lvalue, a non-modifiable
/// lvalue, and an rvalue.  Use the specified `line` to report failures.  The
/// behavior is undefined unless the alternative at index 0 of `t_VARIANT` is
/// default-constructible and the alternative at index 1 is
/// `ThrowingAlternative`.
template <class t_VARIANT>
void testValuelessVisitation(int line)
{
    t_VARIANT        valueless;
    const t_VARIANT& constValueless = valueless;

    makeValueless(&valueless);
    ASSERTV(line, valueless.valueless_by_exception());

    int  calls      = 0;
    auto countingFn = [&](const auto&) { return ++calls; };

    auto throwsBadVariantAccess = [&](auto&& object) {
        try {
            Util::visitWith(std::forward<decltype(object)>(object),
                            countingFn);
        }
        catch (const bsl::bad_variant_access&) {
            return true;
        }
        return false;
    };

    auto throwsBadVariantAccessWithReturnType = [&](auto&& object) {
        try {
            Util::visitWith<int>(std::forward<decltype(object)>(object),
                                 countingFn);
        }
        catch (const bsl::bad_variant_access&) {
            return true;
        }
        return false;
    };

    ASSERTV(line, throwsBadVariantAccess(valueless));
    ASSERTV(line, throwsBadVariantAccess(constValueless));
    ASSERTV(line, throwsBadVariantAccess(std::move(valueless)));

    makeValueless(&valueless);
    ASSERTV(line, valueless.valueless_by_exception());

    ASSERTV(line, throwsBadVariantAccessWithReturnType(valueless));
    ASSERTV(line, throwsBadVariantAccessWithReturnType(constValueless));
    ASSERTV(line, throwsBadVariantAccessWithReturnType(std::move(valueless)));

    ASSERTV(line, calls, 0 == calls);
}
#endif

//=============================================================================
//                              USAGE EXAMPLES
//-----------------------------------------------------------------------------

namespace usage_example_1 {

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// First, create a `bsl::variant` object that can contain several different
// types:
// ```
void run()
{
    bsl::variant<unsigned, double, bsl::string> value;
// ```
// Then, set the value of the variant, and call `visitWith`, passing the
// variant followed by a lambda for each alternative.  Check the return value
// to see that the right lambda was called:
// ```
    value = 2U;
    int result = bdlf::VisitUtil::visitWith(
                                         value,
                                         [](unsigned)           { return 1; },
                                         [](double)             { return 2; },
                                         [](const bsl::string&) { return 3; });
    ASSERT(1 == result);
// ```
// The callables do not have to cover each alternative individually; a
// generic lambda can act as a fallback for the alternatives that are not
// handled explicitly:
// ```
    value = bsl::string("2.0");
    result = bdlf::VisitUtil::visitWith(value,
                                        [](double)      { return 2; },
                                        [](const auto&) { return 0; });
    ASSERT(0 == result);
// ```
// Finally, the callables can modify the active alternative when the variant
// is passed as a modifiable lvalue:
// ```
    value = 2.0;
    bdlf::VisitUtil::visitWith(value,
                               [](double& d) { d *= 2; },
                               [](auto&)     {});
    ASSERT(4.0 == bsl::get<double>(value));
}
// ```

}  // close namespace usage_example_1

namespace usage_example_2 {

///Example 2: Callables Returning Different Types
/// - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to obtain a numeric size of the value held by a variant that
// can contain either an `int` or a `bsl::string`: the value itself for an
// `int`, and the length for a `bsl::string`.  First, create the variant:
// ```
void run()
{
    bsl::variant<int, bsl::string> value(5);
// ```
// The lambda handling `int` returns `int`, whereas the lambda handling
// `bsl::string` returns `bsl::size_t`.  Since these types differ, calling
// `visitWith` without a template argument would fail to compile.  Instead,
// specify the return type explicitly; the result of the selected lambda is
// implicitly converted to it:
// ```
    auto intSize    = [](int i)                { return i; };
    auto stringSize = [](const bsl::string& s) { return s.length(); };

    bsl::size_t size = bdlf::VisitUtil::visitWith<bsl::size_t>(value,
                                                               intSize,
                                                               stringSize);
    ASSERT(5 == size);
// ```
// Finally, change the active alternative and visit the variant again:
// ```
    value = bsl::string("abc");
    size  = bdlf::VisitUtil::visitWith<bsl::size_t>(value,
                                                    intSize,
                                                    stringSize);
    ASSERT(3 == size);
}
// ```

}  // close namespace usage_example_2
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
      case 8: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        // 1. The usage examples provided in the component header file
        //    compile, link, and run as shown.
        //
        // Plan:
        // 1. Test usage examples from header, replace leading comment
        //    characters with spaces, and replace `assert` with `ASSERT`.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

        usage_example_1::run();
        usage_example_2::run();
#endif
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // EXPLICIT RETURN TYPE
        //
        // Concerns:
        // 1. The callables supplied to `visitWith<t_RET>` may return different
        //    types, and the result of the selected callable is converted to
        //    `t_RET`, for `bsl::variant`, `std::variant`, and classes derived
        //    from either.
        //
        // 2. The result of the selected callable is implicitly converted to
        //    `t_RET` even if `t_RET` differs from the return types of all
        //    callables.
        //
        // 3. If `t_RET` is `void`, the callables may return values, which are
        //    discarded.
        //
        // 4. A call to `visitWith` without an explicit template argument
        //    selects the overload that deduces its return type.
        //
        // 5. A call to `visitWith<t_RET>` is not ambiguous when the variant
        //    argument can bind to a parameter of type `t_RET&&`.
        //
        // Plan:
        // 1. For each of `bsl::variant<int, long>`, `std::variant<int, long>`,
        //    and `DerivedVariant` objects derived from them, call
        //    `visitWith<long>` with callables returning `int` and `long`, and
        //    verify the type and value of the result.  (C-1)
        //
        // 2. Call `visitWith<bsl::string>` with a callable returning
        //    `const char *`, and verify the type and value of the result.
        //    (C-2)
        //
        // 3. Call `visitWith<void>` with a callable returning `int` that
        //    increments a counter, and verify the type of the call expression
        //    and its side effect.  (C-3)
        //
        // 4. Call `visitWith` without an explicit template argument and
        //    `visitWith<long>` with a callable returning `short`, and verify,
        //    using `bsl::is_same`, that the result types are `short` and
        //    `long`, respectively.  (C-4)
        //
        // 5. For each of `bsl::variant<int>` and `std::variant<int>`, call
        //    `visitWith<variant<int>>` with an rvalue variant, and
        //    `visitWith<variant<int>&>` with an lvalue variant, and verify the
        //    results.  (C-5)
        //
        // Testing:
        //   decltype(auto) visitWith(t_VARIANT&&, t_CALLABLE&&,
        //                            t_CALLABLES&&...);
        //   t_RET visitWith<t_RET>(t_VARIANT&&, t_CALLABLE&&,
        //                          t_CALLABLES&&...);
        // --------------------------------------------------------------------

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
        if (verbose) cout << endl
                          << "EXPLICIT RETURN TYPE" << endl
                          << "====================" << endl;

        typedef bsl::variant<int, long> BslIntOrLong;

        testDifferentReturnTypes<BslIntOrLong>(L_);
        testDifferentReturnTypes<DerivedVariant<BslIntOrLong>>(L_);

        auto literalFn  = [](const auto&) { return "abc"; };
        auto shortFn    = [](const auto&) -> short { return 1; };
        int  count      = 0;
        auto countingFn = [&](const auto&) { return ++count; };

        BslIntOrLong bslIntOrLong(2);

        typedef decltype(Util::visitWith<bsl::string>(bslIntOrLong,
                                                      literalFn))
                                                               BslStringResult;
        ASSERT((bsl::is_same<BslStringResult, bsl::string>::value));
        ASSERT("abc" == Util::visitWith<bsl::string>(bslIntOrLong,
                                                     literalFn));

        typedef decltype(Util::visitWith<void>(bslIntOrLong, countingFn))
                                                                 BslVoidResult;
        ASSERT((bsl::is_same<BslVoidResult, void>::value));
        Util::visitWith<void>(bslIntOrLong, countingFn);
        ASSERTV(count, 1 == count);

        typedef decltype(Util::visitWith(bslIntOrLong, shortFn))
                                                              BslDeducedResult;
        typedef decltype(Util::visitWith<long>(bslIntOrLong, shortFn))
                                                             BslExplicitResult;
        ASSERT((bsl::is_same<BslDeducedResult,  short>::value));
        ASSERT((bsl::is_same<BslExplicitResult, long>::value));

        auto addThreeFn = [](int i) { return i + 3; };

        typedef bsl::variant<int> BslInt;

        BslInt bslIntRvalue(4);
        BslInt bslIntResult = Util::visitWith<BslInt>(std::move(bslIntRvalue),
                                                      addThreeFn);
        ASSERTV(bsl::get<int>(bslIntResult), 7 == bsl::get<int>(bslIntResult));

        auto bslRefFn = [&](int) -> BslInt& { return bslIntResult; };

        BslInt  bslIntLvalue(4);
        BslInt& bslIntRef = Util::visitWith<BslInt&>(bslIntLvalue, bslRefFn);
        ASSERT(&bslIntResult == &bslIntRef);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        typedef std::variant<int, long> StdIntOrLong;

        testDifferentReturnTypes<StdIntOrLong>(L_);
        testDifferentReturnTypes<DerivedVariant<StdIntOrLong>>(L_);

        StdIntOrLong stdIntOrLong(2);

        typedef decltype(Util::visitWith<bsl::string>(stdIntOrLong,
                                                      literalFn))
                                                               StdStringResult;
        ASSERT((bsl::is_same<StdStringResult, bsl::string>::value));
        ASSERT("abc" == Util::visitWith<bsl::string>(stdIntOrLong,
                                                     literalFn));

        typedef decltype(Util::visitWith<void>(stdIntOrLong, countingFn))
                                                                 StdVoidResult;
        ASSERT((bsl::is_same<StdVoidResult, void>::value));
        Util::visitWith<void>(stdIntOrLong, countingFn);
        ASSERTV(count, 2 == count);

        typedef decltype(Util::visitWith(stdIntOrLong, shortFn))
                                                              StdDeducedResult;
        typedef decltype(Util::visitWith<long>(stdIntOrLong, shortFn))
                                                             StdExplicitResult;
        ASSERT((bsl::is_same<StdDeducedResult,  short>::value));
        ASSERT((bsl::is_same<StdExplicitResult, long>::value));

        typedef std::variant<int> StdInt;

        StdInt stdIntRvalue(4);
        StdInt stdIntResult = Util::visitWith<StdInt>(std::move(stdIntRvalue),
                                                      addThreeFn);
        ASSERTV(std::get<int>(stdIntResult), 7 == std::get<int>(stdIntResult));

        auto stdRefFn = [&](int) -> StdInt& { return stdIntResult; };

        StdInt  stdIntLvalue(4);
        StdInt& stdIntRef = Util::visitWith<StdInt&>(stdIntLvalue, stdRefFn);
        ASSERT(&stdIntResult == &stdIntRef);
#endif
#endif
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // VALUELESS VARIANTS
        //
        // Concerns:
        // 1. `visitWith` throws `bad_variant_access` if the variant is
        //    valueless by exception, for `bsl::variant`, `std::variant`, and
        //    classes derived from either.
        //
        // 2. No callable is invoked if the variant is valueless by exception.
        //
        // 3. The exception is thrown regardless of the value category and
        //    cv-qualification of the variant.
        //
        // Plan:
        // 1. For each of `bsl::variant`, `std::variant`, and `DerivedVariant`
        //    objects derived from them, create an object, put it into the
        //    valueless-by-exception state by emplacing a
        //    `ThrowingAlternative`, and call `visitWith` and `visitWith<int>`
        //    with a counting callable on a modifiable lvalue, a non-modifiable
        //    lvalue, and an rvalue.  Verify that `bad_variant_access` is
        //    thrown each time and that the callable is never invoked.
        //    (C-1..3)
        //
        // Testing:
        //   decltype(auto) visitWith(t_VARIANT&&, t_CALLABLE&&,
        //                            t_CALLABLES&&...);
        //   t_RET visitWith<t_RET>(t_VARIANT&&, t_CALLABLE&&,
        //                          t_CALLABLES&&...);
        // --------------------------------------------------------------------

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
        if (verbose) cout << endl
                          << "VALUELESS VARIANTS" << endl
                          << "==================" << endl;

#ifdef BDE_BUILD_TARGET_EXC
        typedef bsl::variant<int, ThrowingAlternative> BslVariant;

        testValuelessVisitation<BslVariant>(L_);
        testValuelessVisitation<DerivedVariant<BslVariant>>(L_);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        typedef std::variant<int, ThrowingAlternative> StdVariant;

        testValuelessVisitation<StdVariant>(L_);
        testValuelessVisitation<DerivedVariant<StdVariant>>(L_);
#endif
#endif
#endif
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // KINDS OF CALLABLES
        //
        // Concerns:
        // 1. Function pointers and function references can be supplied as
        //    callables.
        //
        // 2. Objects of class type with an `operator()` can be supplied as
        //    callables, both as lvalues and as rvalues.
        //
        // 3. A move-only callable can be supplied as an rvalue.
        //
        // 4. A single callable can be supplied.
        //
        // Plan:
        // 1. Call `visitWith` with function pointers and with function names,
        //    and verify the result.  (C-1)
        //
        // 2. Call `visitWith` with a named `CategoryDetector` object and with
        //    a temporary one, and verify the result.  (C-2)
        //
        // 3. Call `visitWith` with a temporary `MoveOnlyCallable`, and verify
        //    the result.  (C-3)
        //
        // 4. Call `visitWith` with a single generic lambda.  (C-4)
        //
        // 5. Repeat P-1..4 with `visitWith<int>`, for both `bsl::variant` and
        //    `std::variant`.  (C-1..4)
        //
        // Testing:
        //   decltype(auto) visitWith(t_VARIANT&&, t_CALLABLE&&,
        //                            t_CALLABLES&&...);
        //   t_RET visitWith<t_RET>(t_VARIANT&&, t_CALLABLE&&,
        //                          t_CALLABLES&&...);
        // --------------------------------------------------------------------

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
        if (verbose) cout << endl
                          << "KINDS OF CALLABLES" << endl
                          << "==================" << endl;

        typedef bsl::variant<int, double> IntOrDouble;

        IntOrDouble intOrDouble(2);

        ASSERT(e_INT == Util::visitWith(intOrDouble,
                                        &FreeFN::handleInt,
                                        &FreeFN::handleDouble));
        ASSERT(e_INT == Util::visitWith(intOrDouble,
                                        FreeFN::handleInt,
                                        FreeFN::handleDouble));
        intOrDouble = 2.0;
        ASSERT(e_DOUBLE == Util::visitWith(intOrDouble,
                                           &FreeFN::handleInt,
                                           &FreeFN::handleDouble));

        bsl::variant<bsl::string> stringVariant(bsl::in_place_index_t<0>(),
                                                "abc");

        const CategoryDetector detector = CategoryDetector();
        ASSERT(e_LVALUE == Util::visitWith(stringVariant, detector));
        ASSERT(e_LVALUE == Util::visitWith(stringVariant, CategoryDetector()));

        intOrDouble = 2;
        ASSERT(7 == Util::visitWith(intOrDouble,
                                    MoveOnlyCallable(7),
                                    [](double) { return 0; }));

        ASSERT(e_FALLBACK == Util::visitWith(
                                     intOrDouble,
                                     [](const auto&) { return e_FALLBACK; }));

        ASSERT(e_INT == Util::visitWith<int>(intOrDouble,
                                             &FreeFN::handleInt,
                                             &FreeFN::handleDouble));
        ASSERT(e_INT == Util::visitWith<int>(intOrDouble,
                                             FreeFN::handleInt,
                                             FreeFN::handleDouble));
        ASSERT(e_LVALUE == Util::visitWith<int>(stringVariant, detector));
        ASSERT(e_LVALUE == Util::visitWith<int>(stringVariant,
                                                CategoryDetector()));
        ASSERT(7 == Util::visitWith<int>(intOrDouble,
                                         MoveOnlyCallable(7),
                                         [](double) { return 0; }));
        ASSERT(e_FALLBACK == Util::visitWith<int>(
                                     intOrDouble,
                                     [](const auto&) { return e_FALLBACK; }));

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        std::variant<int, double> stdIntOrDouble(2);
        std::variant<bsl::string> stdStringVariant(std::in_place_index<0>,
                                                   "abc");

        ASSERT(e_INT == Util::visitWith<int>(stdIntOrDouble,
                                             &FreeFN::handleInt,
                                             &FreeFN::handleDouble));
        ASSERT(e_INT == Util::visitWith<int>(stdIntOrDouble,
                                             FreeFN::handleInt,
                                             FreeFN::handleDouble));
        ASSERT(e_LVALUE == Util::visitWith<int>(stdStringVariant, detector));
        ASSERT(e_LVALUE == Util::visitWith<int>(stdStringVariant,
                                                CategoryDetector()));
        ASSERT(7 == Util::visitWith<int>(stdIntOrDouble,
                                         MoveOnlyCallable(7),
                                         [](double) { return 0; }));
        ASSERT(e_FALLBACK == Util::visitWith<int>(
                                     stdIntOrDouble,
                                     [](const auto&) { return e_FALLBACK; }));
#endif
#endif
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // RETURN TYPES
        //
        // Concerns:
        // 1. The return type of the selected callable is returned exactly,
        //    including reference return types.
        //
        // 2. Callables returning `void` are supported.
        //
        // Plan:
        // 1. Call `visitWith` with callables returning `int&`, and verify,
        //    using `bsl::is_same`, that the result is `int&` and refers to the
        //    active alternative.  (C-1)
        //
        // 2. Call `visitWith` with callables returning `void`, and verify the
        //    type of the call expression and its side effect.  (C-2)
        //
        // 3. Repeat P-1 with `visitWith<int&>` and P-2 with `visitWith<void>`,
        //    for both `bsl::variant` and `std::variant`.  (C-1..2)
        //
        // Testing:
        //   decltype(auto) visitWith(t_VARIANT&&, t_CALLABLE&&,
        //                            t_CALLABLES&&...);
        //   t_RET visitWith<t_RET>(t_VARIANT&&, t_CALLABLE&&,
        //                          t_CALLABLES&&...);
        // --------------------------------------------------------------------

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
        if (verbose) cout << endl
                          << "RETURN TYPES" << endl
                          << "============" << endl;

        static int dummy = 0;

        bsl::variant<int, double> bslVariant(5);

        auto refFn   = [](int& i) -> int& { return i; };
        auto otherFn = [](double&) -> int& { return dummy; };

        typedef decltype(Util::visitWith(bslVariant, refFn, otherFn))
                                                                    BslResult;

        ASSERT((bsl::is_same<BslResult, int&>::value));

        ASSERT(&bsl::get<int>(bslVariant) ==
                               &Util::visitWith(bslVariant, refFn, otherFn));

        int count = 0;

        auto incrementFn = [&](int)    { ++count; };
        auto addTenFn    = [&](double) { count += 10; };

        typedef decltype(Util::visitWith(bslVariant, incrementFn, addTenFn))
                                                                   VoidResult;
        ASSERT((bsl::is_same<VoidResult, void>::value));

        Util::visitWith(bslVariant, incrementFn, addTenFn);
        ASSERTV(count, 1 == count);

        typedef decltype(Util::visitWith<int&>(bslVariant, refFn, otherFn))
                                                            BslExplicitResult;
        ASSERT((bsl::is_same<BslExplicitResult, int&>::value));

        ASSERT(&bsl::get<int>(bslVariant) ==
                         &Util::visitWith<int&>(bslVariant, refFn, otherFn));

        typedef decltype(Util::visitWith<void>(bslVariant,
                                               incrementFn,
                                               addTenFn)) ExplicitVoidResult;
        ASSERT((bsl::is_same<ExplicitVoidResult, void>::value));

        Util::visitWith<void>(bslVariant, incrementFn, addTenFn);
        ASSERTV(count, 2 == count);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        std::variant<int, double> stdVariant(5);

        typedef decltype(Util::visitWith(stdVariant, refFn, otherFn))
                                                                    StdResult;

        ASSERT((bsl::is_same<StdResult, int&>::value));

        ASSERT(&std::get<int>(stdVariant) ==
                               &Util::visitWith(stdVariant, refFn, otherFn));

        Util::visitWith(stdVariant, incrementFn, addTenFn);
        ASSERTV(count, 3 == count);

        typedef decltype(Util::visitWith<int&>(stdVariant, refFn, otherFn))
                                                            StdExplicitResult;
        ASSERT((bsl::is_same<StdExplicitResult, int&>::value));

        ASSERT(&std::get<int>(stdVariant) ==
                         &Util::visitWith<int&>(stdVariant, refFn, otherFn));

        Util::visitWith<void>(stdVariant, incrementFn, addTenFn);
        ASSERTV(count, 4 == count);
#endif
#endif
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // WORKING WITH `std::variant`
        //
        // Concerns:
        // 1. `visitWith` accepts a `std::variant` and invokes the callable
        //    matching its active alternative.
        //
        // 2. The value category and cv-qualification of the `std::variant`
        //    are propagated to the selected callable.
        //
        // 3. An object of a class derived from `std::variant` is accepted,
        //    and its value category and cv-qualification are propagated to
        //    the selected callable.
        //
        // 4. A move-only alternative can be moved out of an rvalue
        //    `std::variant`.
        //
        // Plan:
        // 1. Create a `std::variant`, set each alternative in turn, call
        //    `visitWith`, and verify the result.  (C-1)
        //
        // 2. Call `visitWith` with a `CategoryDetector` on a modifiable
        //    lvalue, a non-modifiable lvalue and an rvalue `std::variant`,
        //    and verify the result.  (C-2)
        //
        // 3. Repeat P-1 and P-2 with `DerivedVariant` objects derived from
        //    `std::variant`.  (C-3)
        //
        // 4. Call `visitWith` on an rvalue `std::variant` holding a
        //    `bsl::unique_ptr`, move the pointer out in the callable, and
        //    verify that the variant's pointer is now null.  (C-4)
        //
        // 5. Repeat P-1..4 with `visitWith<int>`, and with `visitWith<void>`
        //    where the callables return `void`.  (C-1..4)
        //
        // Testing:
        //   decltype(auto) visitWith(t_VARIANT&&, t_CALLABLE&&,
        //                            t_CALLABLES&&...);
        //   t_RET visitWith<t_RET>(t_VARIANT&&, t_CALLABLE&&,
        //                          t_CALLABLES&&...);
        // --------------------------------------------------------------------

#if defined(BSLS_COMPILERFEATURES_SUPPORT_CTAD) &&                           \
    defined(BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY)
        if (verbose) cout << endl
                          << "WORKING WITH `std::variant`" << endl
                          << "===========================" << endl;

        typedef std::variant<int, double, bsl::string> IntDoubleOrString;

        IntDoubleOrString intDoubleOrString;

        intDoubleOrString = 2;
        ASSERT(e_INT == Util::visitWith(
                                 intDoubleOrString,
                                 [](int)                { return e_INT; },
                                 [](double)             { return e_DOUBLE; },
                                 [](const bsl::string&) { return e_STRING; }));
        intDoubleOrString = 2.0;
        ASSERT(e_DOUBLE == Util::visitWith(
                                 intDoubleOrString,
                                 [](int)                { return e_INT; },
                                 [](double)             { return e_DOUBLE; },
                                 [](const bsl::string&) { return e_STRING; }));
        intDoubleOrString = bsl::string("2");
        ASSERT(e_STRING == Util::visitWith(
                                 intDoubleOrString,
                                 [](int)                { return e_INT; },
                                 [](double)             { return e_DOUBLE; },
                                 [](const bsl::string&) { return e_STRING; }));

        std::variant<bsl::string> stringVariant(std::in_place_index<0>, "abc");
        const std::variant<bsl::string>& constStringVariant = stringVariant;

        ASSERT(e_LVALUE       == Util::visitWith(stringVariant,
                                                 CategoryDetector()));
        ASSERT(e_CONST_LVALUE == Util::visitWith(constStringVariant,
                                                 CategoryDetector()));
        ASSERT(e_RVALUE       == Util::visitWith(std::move(stringVariant),
                                                 CategoryDetector()));

        DerivedVariant<std::variant<int, double>> derivedIntOrDouble(2.0);

        ASSERT(e_DOUBLE == Util::visitWith(derivedIntOrDouble,
                                           [](int)    { return e_INT; },
                                           [](double) { return e_DOUBLE; }));

        DerivedVariant<std::variant<bsl::string>> derivedStringVariant(
                                                       std::in_place_index<0>,
                                                       "abc");
        const DerivedVariant<std::variant<bsl::string>>&
                              constDerivedStringVariant = derivedStringVariant;

        ASSERT(e_LVALUE       == Util::visitWith(derivedStringVariant,
                                                 CategoryDetector()));
        ASSERT(e_CONST_LVALUE == Util::visitWith(constDerivedStringVariant,
                                                 CategoryDetector()));
        ASSERT(e_RVALUE       == Util::visitWith(
                                               std::move(derivedStringVariant),
                                               CategoryDetector()));

        std::variant<int, bsl::unique_ptr<int>> intOrPointer(
                                             std::in_place_index<1>,
                                             bsl::unique_ptr<int>(new int(4)));

        bsl::unique_ptr<int> taken;
        Util::visitWith(
                     std::move(intOrPointer),
                     [](int) {},
                     [&](bsl::unique_ptr<int>&& p) { taken = std::move(p); });
        ASSERT(taken && 4 == *taken);
        ASSERT(!std::get<1>(intOrPointer));

        intDoubleOrString = 2;
        ASSERT(e_INT == Util::visitWith<int>(
                                 intDoubleOrString,
                                 [](int)                { return e_INT; },
                                 [](double)             { return e_DOUBLE; },
                                 [](const bsl::string&) { return e_STRING; }));
        intDoubleOrString = 2.0;
        ASSERT(e_DOUBLE == Util::visitWith<int>(
                                 intDoubleOrString,
                                 [](int)                { return e_INT; },
                                 [](double)             { return e_DOUBLE; },
                                 [](const bsl::string&) { return e_STRING; }));
        intDoubleOrString = bsl::string("2");
        ASSERT(e_STRING == Util::visitWith<int>(
                                 intDoubleOrString,
                                 [](int)                { return e_INT; },
                                 [](double)             { return e_DOUBLE; },
                                 [](const bsl::string&) { return e_STRING; }));

        stringVariant.emplace<0>("abc");
        derivedStringVariant.emplace<0>("abc");

        ASSERT(e_LVALUE       == Util::visitWith<int>(stringVariant,
                                                      CategoryDetector()));
        ASSERT(e_CONST_LVALUE == Util::visitWith<int>(constStringVariant,
                                                      CategoryDetector()));
        ASSERT(e_RVALUE       == Util::visitWith<int>(std::move(stringVariant),
                                                      CategoryDetector()));

        ASSERT(e_DOUBLE == Util::visitWith<int>(
                                           derivedIntOrDouble,
                                           [](int)    { return e_INT; },
                                           [](double) { return e_DOUBLE; }));

        ASSERT(e_LVALUE       == Util::visitWith<int>(derivedStringVariant,
                                                      CategoryDetector()));
        ASSERT(e_CONST_LVALUE == Util::visitWith<int>(
                                                     constDerivedStringVariant,
                                                     CategoryDetector()));
        ASSERT(e_RVALUE       == Util::visitWith<int>(
                                               std::move(derivedStringVariant),
                                               CategoryDetector()));

        intOrPointer.emplace<1>(new int(5));
        Util::visitWith<void>(
                     std::move(intOrPointer),
                     [](int) {},
                     [&](bsl::unique_ptr<int>&& p) { taken = std::move(p); });
        ASSERT(taken && 5 == *taken);
        ASSERT(!std::get<1>(intOrPointer));
#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // `visitWith` WITH `bsl::variant`
        //
        // Concerns:
        // 1. The value category and cv-qualification of the `bsl::variant`
        //    are propagated to the selected callable.
        //
        // 2. The active alternative can be modified through a modifiable
        //    lvalue variant.
        //
        // 3. A move-only alternative can be moved out of an rvalue variant.
        //
        // 4. An object of a class derived from `bsl::variant` is accepted,
        //    and its value category and cv-qualification are propagated to
        //    the selected callable.
        //
        // Plan:
        // 1. Call `visitWith` with a `CategoryDetector` on a modifiable
        //    lvalue, a non-modifiable lvalue and an rvalue `bsl::variant`,
        //    and verify the result.  (C-1)
        //
        // 2. Modify the active alternative from a callable, and verify the
        //    new value.  (C-2)
        //
        // 3. Call `visitWith` on an rvalue variant holding a
        //    `bsl::unique_ptr`, move the pointer out in the callable, and
        //    verify that the variant's pointer is now null.  (C-3)
        //
        // 4. Repeat P-1 with a `DerivedVariant` object derived from
        //    `bsl::variant`.  (C-4)
        //
        // 5. Repeat P-1 and P-4 with `visitWith<int>`, and P-2 and P-3 with
        //    `visitWith<void>`.  (C-1..4)
        //
        // Testing:
        //   decltype(auto) visitWith(t_VARIANT&&, t_CALLABLE&&,
        //                            t_CALLABLES&&...);
        //   t_RET visitWith<t_RET>(t_VARIANT&&, t_CALLABLE&&,
        //                          t_CALLABLES&&...);
        // --------------------------------------------------------------------

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
        if (verbose) cout << endl
                          << "`visitWith` WITH `bsl::variant`" << endl
                          << "===============================" << endl;

        bsl::variant<bsl::string> stringVariant(bsl::in_place_index_t<0>(),
                                                "abc");
        const bsl::variant<bsl::string>& constStringVariant = stringVariant;

        ASSERT(e_LVALUE       == Util::visitWith(stringVariant,
                                                 CategoryDetector()));
        ASSERT(e_CONST_LVALUE == Util::visitWith(constStringVariant,
                                                 CategoryDetector()));
        ASSERT(e_RVALUE       == Util::visitWith(std::move(stringVariant),
                                                 CategoryDetector()));

        bsl::variant<int, bsl::string> intOrString(3);
        Util::visitWith(intOrString,
                        [](int& i)          { i *= 3; },
                        [](bsl::string& st) { st += "!"; });
        ASSERTV(bsl::get<int>(intOrString), 9 == bsl::get<int>(intOrString));

        intOrString = bsl::string("hi");
        Util::visitWith(intOrString,
                        [](int& i)          { i *= 3; },
                        [](bsl::string& st) { st += "!"; });
        ASSERTV(bsl::get<bsl::string>(intOrString),
                "hi!" == bsl::get<bsl::string>(intOrString));

        bsl::variant<int, bsl::unique_ptr<int>> intOrPointer(
                                            bsl::in_place_index_t<1>(),
                                            bsl::unique_ptr<int>(new int(4)));

        bsl::unique_ptr<int> taken;
        Util::visitWith(
                     std::move(intOrPointer),
                     [](int) {},
                     [&](bsl::unique_ptr<int>&& p) { taken = std::move(p); });
        ASSERT(taken && 4 == *taken);
        ASSERT(!bsl::get<1>(intOrPointer));

        DerivedVariant<bsl::variant<bsl::string>> derivedStringVariant(
                                                   bsl::in_place_index_t<0>(),
                                                   "abc");
        const DerivedVariant<bsl::variant<bsl::string>>&
                              constDerivedStringVariant = derivedStringVariant;

        ASSERT(e_LVALUE       == Util::visitWith(derivedStringVariant,
                                                 CategoryDetector()));
        ASSERT(e_CONST_LVALUE == Util::visitWith(constDerivedStringVariant,
                                                 CategoryDetector()));
        ASSERT(e_RVALUE       == Util::visitWith(
                                               std::move(derivedStringVariant),
                                               CategoryDetector()));

        stringVariant.emplace<0>("abc");
        derivedStringVariant.emplace<0>("abc");

        ASSERT(e_LVALUE       == Util::visitWith<int>(stringVariant,
                                                      CategoryDetector()));
        ASSERT(e_CONST_LVALUE == Util::visitWith<int>(constStringVariant,
                                                      CategoryDetector()));
        ASSERT(e_RVALUE       == Util::visitWith<int>(std::move(stringVariant),
                                                      CategoryDetector()));

        intOrString = 3;
        Util::visitWith<void>(intOrString,
                              [](int& i)          { i *= 3; },
                              [](bsl::string& st) { st += "!"; });
        ASSERTV(bsl::get<int>(intOrString), 9 == bsl::get<int>(intOrString));

        intOrString = bsl::string("hi");
        Util::visitWith<void>(intOrString,
                              [](int& i)          { i *= 3; },
                              [](bsl::string& st) { st += "!"; });
        ASSERTV(bsl::get<bsl::string>(intOrString),
                "hi!" == bsl::get<bsl::string>(intOrString));

        intOrPointer.emplace<1>(new int(5));
        Util::visitWith<void>(
                     std::move(intOrPointer),
                     [](int) {},
                     [&](bsl::unique_ptr<int>&& p) { taken = std::move(p); });
        ASSERT(taken && 5 == *taken);
        ASSERT(!bsl::get<1>(intOrPointer));

        ASSERT(e_LVALUE       == Util::visitWith<int>(derivedStringVariant,
                                                      CategoryDetector()));
        ASSERT(e_CONST_LVALUE == Util::visitWith<int>(
                                                     constDerivedStringVariant,
                                                     CategoryDetector()));
        ASSERT(e_RVALUE       == Util::visitWith<int>(
                                               std::move(derivedStringVariant),
                                               CategoryDetector()));
#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        // 1. `visitWith` invokes the callable matching the active alternative
        //    of a `bsl::variant`.
        //
        // 2. A generic lambda can serve as a fallback.
        //
        // 3. `visitWith<t_RET>` accepts callables with different return types.
        //
        // Plan:
        // 1. Create a `bsl::variant`, set each alternative in turn, call
        //    `visitWith` with one lambda per alternative, and verify the
        //    result.  (C-1)
        //
        // 2. Call `visitWith` with one specific lambda and a generic lambda,
        //    and verify the result for each alternative.  (C-2)
        //
        // 3. Call `visitWith<long>` with lambdas returning different types,
        //    and verify the result.  (C-3)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        typedef bsl::variant<int, double, bsl::string_view>
                                                         IntDoubleOrStringView;

        IntDoubleOrStringView intDoubleOrStringView;

        intDoubleOrStringView = 2;
        ASSERT(e_INT == Util::visitWith(
                                   intDoubleOrStringView,
                                   [](int)              { return e_INT; },
                                   [](double)           { return e_DOUBLE; },
                                   [](bsl::string_view) { return e_STRING; }));
        intDoubleOrStringView = 2.0;
        ASSERT(e_DOUBLE == Util::visitWith(
                                   intDoubleOrStringView,
                                   [](int)              { return e_INT; },
                                   [](double)           { return e_DOUBLE; },
                                   [](bsl::string_view) { return e_STRING; }));
        intDoubleOrStringView = bsl::string_view("2");
        ASSERT(e_STRING == Util::visitWith(
                                   intDoubleOrStringView,
                                   [](int)              { return e_INT; },
                                   [](double)           { return e_DOUBLE; },
                                   [](bsl::string_view) { return e_STRING; }));

        ASSERT(e_FALLBACK == Util::visitWith(
                                    intDoubleOrStringView,
                                    [](double)      { return e_DOUBLE; },
                                    [](const auto&) { return e_FALLBACK; }));
        intDoubleOrStringView = 2.0;
        ASSERT(e_DOUBLE == Util::visitWith(
                                    intDoubleOrStringView,
                                    [](double)      { return e_DOUBLE; },
                                    [](const auto&) { return e_FALLBACK; }));

        ASSERT(e_DOUBLE == Util::visitWith<long>(
                                    intDoubleOrStringView,
                                    [](double)      { return e_DOUBLE; },
                                    [](const auto&) { return 0L; }));
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
// Copyright 2026 Bloomberg Finance L.P.
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
