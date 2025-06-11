// bslfmt_formatterspecificationnumericvalue.t.cpp                    -*-C++-*-
#include <bslfmt_formatterspecificationnumericvalue.h>

#include <bslfmt_format_arg.h>          // Testing only
#include <bslfmt_format_args.h>         // Testing only
#include <bslfmt_formatparsecontext.h>  // Testing only

#include <bsls_bsltestutil.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
    #include <format>
#endif

#include <limits.h>
#include <stdio.h>
#include <string.h>

using namespace BloombergLP;
using namespace bslfmt;

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------
namespace {

    int testStatus = 0;

    void aSsErT(bool condition, const char *message, int line)
    {
        if (condition) {
            printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);
            fflush(stdout);

            if (0 <= testStatus && testStatus <= 100) {
                ++testStatus;
            }
        }
    }

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q BSLS_BSLTESTUTIL_Q    // Quote identifier literally.
#define P BSLS_BSLTESTUTIL_P    // Print identifier and value.
#define P_ BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_ BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_ BSLS_BSLTESTUTIL_L_  // current Line number

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                     ASSISTANCE TYPES AND FUNCTIONS
// ----------------------------------------------------------------------------

typedef bslfmt::FormatterSpecificationNumericValue Object;

                            // ==============
                            // struct ArgType
                            // ==============

struct ArgType {
    enum Enum {
        e_UNKNOWN,
        e_MONOSTATE,
        e_BOOL,
        e_CHAR_TYPE,
        e_INT,
        e_UNSIGNED,
        e_LONG_LONG,
        e_UNSIGNED_LONG_LONG,
        e_FLOAT,
        e_DOUBLE,
        e_LONG_DOUBLE,
        e_CHAR_TYPE_PTR,
        e_STRING_VIEW,
        e_VOID_PTR,
        e_HANDLE
    };
};

                           // =================
                           // struct TypeToEnum
                           // =================

template <class t_CHAR, class t_VALUE_TYPE>
struct TypeToEnum {};

template <class t_CHAR>
struct TypeToEnum<t_CHAR, bool> {
    static const ArgType::Enum value = ArgType::e_BOOL;
};

template <class t_CHAR>
struct TypeToEnum<t_CHAR, t_CHAR> {
    static const ArgType::Enum value = ArgType::e_CHAR_TYPE;
};

template <class t_CHAR>
struct TypeToEnum<t_CHAR, int> {
    static const ArgType::Enum value = ArgType::e_INT;
};

template <class t_CHAR>
struct TypeToEnum<t_CHAR, unsigned> {
    static const ArgType::Enum value = ArgType::e_UNSIGNED;
};

template <class t_CHAR>
struct TypeToEnum<t_CHAR, long long> {
    static const ArgType::Enum value = ArgType::e_LONG_LONG;
};

template <class t_CHAR>
struct TypeToEnum<t_CHAR, unsigned long long> {
    static const ArgType::Enum value = ArgType::e_UNSIGNED_LONG_LONG;
};

template <class t_CHAR>
struct TypeToEnum<t_CHAR, float> {
    static const ArgType::Enum value = ArgType::e_FLOAT;
};

template <class t_CHAR>
struct TypeToEnum<t_CHAR, double> {
    static const ArgType::Enum value = ArgType::e_DOUBLE;
};

template <class t_CHAR>
struct TypeToEnum<t_CHAR, long double> {
    static const ArgType::Enum value = ArgType::e_LONG_DOUBLE;
};

template <class t_CHAR>
struct TypeToEnum<t_CHAR, const t_CHAR*> {
    static const ArgType::Enum value = ArgType::e_CHAR_TYPE_PTR;
};

#ifndef BSLSTL_STRING_VIEW_IS_ALIASED
template <class t_CHAR>
struct TypeToEnum<t_CHAR, bsl::basic_string_view<t_CHAR> > {
    static const ArgType::Enum value = ArgType::e_STRING_VIEW;
};
#endif

#if defined(BSLSTL_STRING_VIEW_AND_STD_STRING_VIEW_COEXIST) ||                \
    defined(BSLSTL_STRING_VIEW_IS_ALIASED)
template <class t_CHAR>
struct TypeToEnum<t_CHAR, std::basic_string_view<t_CHAR> > {
    static const ArgType::Enum value = ArgType::e_STRING_VIEW;
};
#endif
template <class t_CHAR>
struct TypeToEnum<t_CHAR, const void*> {
    static const ArgType::Enum value = ArgType::e_VOID_PTR;
};

// We cannot create an `std::handle` because it is an unspecified type, so
// for now we do not test `handle` (even though we could for `bslfmt`).

                    // =======================
                    // struct ArgTypingVisitor
                    // =======================

template <class t_CONTEXT>
struct ArgTypingVisitor : ArgType {
    // TYPES
    typedef typename t_CONTEXT::char_type char_type;

    // DATA
    ArgType::Enum d_argType;

    // CREATORS
    ArgTypingVisitor() : d_argType(e_UNKNOWN) {}

    // MANIPULATORS
    void operator()(bsl::monostate) {
        d_argType = e_MONOSTATE;
    }
    void operator()(bool) {
        d_argType = e_BOOL;
    }
    void operator()(char_type) {
        d_argType = e_CHAR_TYPE;
    }
    void operator()(int) {
        d_argType = e_INT;
    }
    void operator()(unsigned) {
        d_argType = e_UNSIGNED;
    }
    void operator()(long long) {
        d_argType = e_LONG_LONG;
    }
    void operator()(unsigned long long) {
        d_argType = e_UNSIGNED_LONG_LONG;
    }

#if defined(BSLS_LIBRARYFEATURES_STDCPP_GNU) && defined(__GLIBCXX__) && \
    __GLIBCXX__ >= 20230426 && defined(__SIZEOF_INT128__)
    // the 128-bit int overloads are required prior to version 15, due to
    // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=108053
  #define u_NEED_INT128
#endif
#if defined(BSLS_LIBRARYFEATURES_STDCPP_LLVM) &&                              \
    defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
    // libc++ requires these overloads, bug is reported as
    // https://github.com/llvm/llvm-project/issues/139582
  #define u_NEED_INT128
#endif
#ifdef u_NEED_INT128
    void operator()(__int128) {
        // We will not use `__int128` so we do not need to set anything here.
        // But for paranoia we do `ASSERT` if we get here somehow.
        ASSERT(0 == "`operator()(__int128)` must not be called!");
    }
    void operator()(unsigned __int128) {
        // We will not use `unsigned __int128` so we do not need to set
        // anything here.  But for paranoia we do `ASSERT` if we get here
        // somehow.
        ASSERT(0 == "`operator()(unsigned __int128)` must not be called!");
    }
#endif  // On GNU C++ lib and it uses __int128
    void operator()(float) {
        d_argType = e_FLOAT;
    }
    void operator()(double) {
        d_argType = e_DOUBLE;
    }
    void operator()(long double) {
        d_argType = e_LONG_DOUBLE;
    }
    void operator()(const char_type*) {
        d_argType = e_CHAR_TYPE_PTR;
    }
#if defined(BSLSTL_STRING_VIEW_AND_STD_STRING_VIEW_COEXIST) ||                \
    defined(BSLSTL_STRING_VIEW_IS_ALIASED)
    void operator()(const std::basic_string_view<char_type>&) {
        d_argType = e_STRING_VIEW;
    }
#endif
#ifndef BSLSTL_STRING_VIEW_IS_ALIASED
    void operator()(const bsl::basic_string_view<char_type>&) {
        d_argType = e_STRING_VIEW;
    }
#endif
    void operator()(const void*) {
        d_argType = e_VOID_PTR;
    }
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
    void operator()(typename std::basic_format_arg<t_CONTEXT>::handle) {
#else
    void operator()(typename bslfmt::basic_format_arg<t_CONTEXT>::handle) {
#endif
        d_argType = e_HANDLE;
    }

    // ACCESSORS
    ArgType::Enum argType() const {
        return d_argType;
    }
};

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
template <class t_CONTEXT>
ArgType::Enum getArgType(const std::basic_format_arg<t_CONTEXT>& arg)
#else
template <class t_CONTEXT>
ArgType::Enum getArgType(const bslfmt::basic_format_arg<t_CONTEXT>& arg)
#endif
{
    ArgTypingVisitor<t_CONTEXT> visitor;
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
    // Depending on the type of t_FORMAT_CONTEXT we may need to use
    // `visit_format_arg` from `bslfmt` or from `std`.
    using namespace std;
#else
    using namespace bslfmt;
#endif
    visit_format_arg(visitor, arg);

    return visitor.argType();
}

                        // ======================
                        // struct MockArgsContext
                        // ======================

template <class t_CHAR, class t_VALUE_TYPE, class t_BAD_TYPE>
struct MockArgsContext;

#if !defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
// there is no usable `std::format`
namespace BloombergLP {
namespace bslfmt {
/// This specialization of `basic_arg_format` for a `char`-based
/// `MockArgsContext` is used when the standard library does not provide a
/// sufficiently functional format library so that we have to implement our
/// own.  This specialization has to inherit from `basic_format_arg` of
/// `format_context` or `wformat_context` because `visit_format_arg` only
/// exists for those.  The conversion constructor from the base class exists so
/// that we can use (the only way of creating format-arguments the)
/// `make_format_args` function that does not have a specialization for our
/// context type, only for `format_context` and `wformat_context`.  See
/// `MockArgsContext`.
template <class t_VALUE_TYPE, class t_BAD_TYPE>
class basic_format_arg<MockArgsContext<char, t_VALUE_TYPE, t_BAD_TYPE> >
: public basic_format_arg<format_context>
{
  public:

    // CREATORS

    /// Create an empty argument that visits as `std::monostate`.
    basic_format_arg() {}

    /// Create a `basic_format_arg` (of a `MockArgsContext` type) from an
    /// object of its base class.  This constructor is necessary to make the
    /// conversion in the return statement of `createIntegralValue` and
    /// `createBadValue` work.
    basic_format_arg(const basic_format_arg<format_context>& other)
    : basic_format_arg<format_context>(other)
    {
    }

    // CLASS METHODS

    /// Create a `basic_format_arg` (of a `MockArgsContext` type) with the
    /// specified `value` and having `t_VALUE_TYPE` as its type.
    static basic_format_arg createIntegralValue(long long value)
    {
        static t_VALUE_TYPE typedValue;
        typedValue = static_cast<t_VALUE_TYPE>(value);
        return bslfmt::format_args(bslfmt::make_format_args(typedValue))
            .get(0);
    }

    /// Create a `basic_format_arg` (of a `MockArgsContext` type) with an
    /// unspecified value and having `t_BAD_TYPE` as its type.  This method is
    /// used to create type that cannot be used as numeric values (width or
    /// precision).
    static basic_format_arg createBadValue()
    {
        static t_BAD_TYPE x;
        return bslfmt::format_args(bslfmt::make_format_args(x)).get(0);
    }
};

/// This specialization of `basic_arg_format` for a `wchar_t`-based
/// `MockArgsContext` is used when the standard library does not provide a
/// sufficiently functional format library so that we have to implement our
/// own.  This specialization has to inherit from `basic_format_arg` of
/// `format_context` or `wformat_context` because `visit_format_arg` only
/// exists for those.  The conversion constructor from the base class exists so
/// that we can use (the only way of creating format-arguments the)
/// `make_format_args` function that does not have a specialization for our
/// context type, only for `format_context` and `wformat_context`.
template <class t_VALUE_TYPE, class t_BAD_TYPE>
class basic_format_arg<MockArgsContext<wchar_t, t_VALUE_TYPE, t_BAD_TYPE> >
: public basic_format_arg<wformat_context>
{
  public:

    // CREATORS

    /// Create an empty argument that visits as `std::monostate`.
    basic_format_arg() {}

    /// Create a `basic_format_arg` (of a `MockArgsContext` type) from an
    /// object of its base class.  This constructor is necessary to make the
    /// conversion in the return statement of `createIntegralValue` and
    /// `createBadValue` work.
    basic_format_arg(const basic_format_arg<wformat_context>& other)
    : basic_format_arg<wformat_context>(other)
    {
    }

    // CLASS METHODS

    /// Create a `basic_format_arg` (of a `MockArgsContext` type) with the
    /// specified `value` and having `t_VALUE_TYPE` as its type.
    static basic_format_arg createIntegralValue(long long value)
    {
        static t_VALUE_TYPE typedValue;
        typedValue = static_cast<t_VALUE_TYPE>(value);
        return bslfmt::wformat_args(bslfmt::make_wformat_args(typedValue))
            .get(0);
    }

    /// Create a `basic_format_arg` (of a `MockArgsContext` type) with an
    /// unspecified value and having `t_BAD_TYPE` as its type.  This method is
    /// used to create type that cannot be used as numeric values (width or
    /// precision).
    static basic_format_arg createBadValue()
    {
        static t_BAD_TYPE x;
        return bslfmt::wformat_args(bslfmt::make_wformat_args(x)).get(0);
    }
};

}  // close package namespace
}  // close enterprise namespace
#endif  // There is no usable `std::format`

/// The states of `MockArgsContext` argument-access detection.
struct MockArgsContextStatus{
    // TYPES
    enum Enum {
        e_NOT_ACCESSED,
        e_EMPTY_ARG_ACCESSED,
        e_INTEGRAL_ARG_ACCESSED,
        e_BAD_TYPE_ARG_ACCESSED,
        e_MORE_THAN_ONE_ARG_ACCESSED
    };
};


/// Provide an implementation for `MockArgsCreator`.
template <class t_CONTEXT, class t_CHAR>
struct MockArgsCreator_Impl;

/// The `char` `char_type` implementation.
template <class t_CONTEXT>
struct MockArgsCreator_Impl<t_CONTEXT, char> {
    typedef t_CONTEXT                               MockArgsContext;
    typedef typename MockArgsContext::FormatArgType FormatArgType;
    typedef typename MockArgsContext::IntType       IntType;
    typedef typename MockArgsContext::BadType       BadType;

    static FormatArgType createIntegralValue(long long value)
    {
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
        static IntType typedValue;
        typedValue = static_cast<IntType>(value);
        return std::basic_format_args(
                            std::make_format_args<MockArgsContext>(typedValue))
            .get(0);
#else
        static IntType typedValue;
        typedValue = static_cast<IntType>(value);
        return bslfmt::format_args(bslfmt::make_format_args(typedValue))
            .get(0);
#endif
    }
    static FormatArgType createBadValue()
    {
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
        static BadType typedValue;
        return std::basic_format_args(
                            std::make_format_args<MockArgsContext>(typedValue))
            .get(0);
#else
        static BadType typedValue;
        return bslfmt::format_args(bslfmt::make_format_args(typedValue))
            .get(0);
#endif
    }
};

/// The `whar_t` `char_type` implementation.
template <class t_CONTEXT>
struct MockArgsCreator_Impl<t_CONTEXT, wchar_t> {
    typedef t_CONTEXT                               MockArgsContext;
    typedef typename MockArgsContext::FormatArgType FormatArgType;
    typedef typename MockArgsContext::IntType       IntType;
    typedef typename MockArgsContext::BadType       BadType;

    static FormatArgType createIntegralValue(long long value)
    {
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
        static IntType typedValue;
        typedValue = static_cast<IntType>(value);
        return std::basic_format_args(
                            std::make_format_args<MockArgsContext>(typedValue))
            .get(0);
#else
        static IntType typedValue;
        typedValue = static_cast<IntType>(value);
        return bslfmt::wformat_args(bslfmt::make_wformat_args(typedValue))
            .get(0);
#endif
    }
    static FormatArgType createBadValue()
    {
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
        static BadType typedValue;
        return std::basic_format_args(
                            std::make_format_args<MockArgsContext>(typedValue))
            .get(0);
#else
        static BadType typedValue;
        return bslfmt::wformat_args(bslfmt::make_wformat_args(typedValue))
            .get(0);
#endif
    }
};

/// Creating the two `basic_format_arg` values for `MockArgsContext<...>`.  For
/// t_CHAR` that is either `char` or `wchar_t`, provide member functions
/// `createIntegralValue` and `createBadValue` that return a
/// `basic_format_arg<t_CONTEXT>` object referring to a single argument with
/// the requested type, and in case of integral value, value.
template <class t_CONTEXT>
struct MockArgsCreator
: MockArgsCreator_Impl<t_CONTEXT, typename t_CONTEXT::char_type> {
};

/// A format context that implements the bare minimum of functionality required
/// by the `postprocess` method (the `arg` method).  It also provides some of
/// the members provided by `basic_format_context` that are not supposed to be
/// used but must be declared to satisfy the requirements `basic_format_arg`
/// places on its template argument.  The context class serves one argument
/// for an argument identifier and value stored as attributes, and the
/// specified `t_VALUE_TYPE` type.  For `argId + 1` it serves an argument of
/// the specified `t_BAD_TYPE` with an unspecified value.  All other argument
/// identifiers will return an "empty argument" (visits as `monostate`).  The
/// object also maintains an argument-access status that starts out as "not
/// accessed", and may be set to "empty argument access", "integral argument
/// accessed", bad-type argument accessed", or "more than one argument
/// accessed" during a call to the `arg` method.
template <class t_CHAR, class t_INT_TYPE, class t_BAD_TYPE>
struct MockArgsContext {

    #ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
        #define u_NAMESPACE std
        #define u_NAMESPACE_FMT std
    #else
        #define u_NAMESPACE bsl
        #define u_NAMESPACE_FMT bslfmt
    #endif

    // TYPES

    typedef
        typename u_NAMESPACE_FMT::basic_format_parse_context<t_CHAR>::iterator
            iterator;
            // Necessary for fulfilling `basic_format_arg` requirements.

    /// Necessary for fulfilling `basic_format_arg` requirements.
    template <class t_TYPE>
    struct formatter_type {
        iterator format(t_TYPE, MockArgsContext) const
        {
            ASSERT(false);
            return iterator();
        }

        template <class t_PARSE_CONTEXT>
        typename t_PARSE_CONTEXT::iterator parse(t_PARSE_CONTEXT&)
        {
            ASSERT(false);
            return typename t_PARSE_CONTEXT::iterator();
        }
    };

    typedef MockArgsContextStatus Status;
    typedef Status::Enum          StatusEnum;

    typedef t_CHAR char_type;

    typedef u_NAMESPACE_FMT::basic_format_arg<MockArgsContext> FormatArgType;

    typedef t_INT_TYPE IntType;
    typedef t_BAD_TYPE BadType;

    // DATA
    mutable StatusEnum d_status;  // Argument-access status

    size_t             d_argId;
    long long          d_value;

    // CREATORS

    /// Create a `MockArgsContext` that provides the specified `value` with
    /// the type `t_VALUE_TYPE` for the specified `argId`, an argument with of
    /// `e_BAD_TYPE` with an unspecified value for `argId + 1`, while providing
    /// empty (`monostate`) arguments for any other argument id.
    MockArgsContext(size_t argId, long long value)
    : d_status(Status::e_NOT_ACCESSED)
    , d_argId(argId)
    , d_value(value)
    {
    }

    // MANIPULATORS

    void advance_to(iterator)
    {
        ASSERT(false);
    }

    iterator out()
    {
        ASSERT(false);
        return iterator();
    }

    std::locale locale()
    {
        ASSERT(false);
        return std::locale();
    }

    // Reset this object's `status` to `e_NOT_ACCESSED`.
    void reset() {
        d_status = Status::e_NOT_ACCESSED;
    }

    /// Change the `value` attribute of this object (the value it provides for
    /// the argument of `argId`).
    void setValue(long long value) {
        d_value = value;
    }

    // ACCESSORS

    /// Return the format argument for the specified argument `id`.  When `id`
    /// is the same as the `argId` attribute return a `format_arg` with the
    /// type `t_VALUE_TYPE` and the value of the `value` attribute.  When `id`
    /// is `argId + 1 ` return an argument of `e_BAD_TYPE` with an unspecified
    /// value.  Return an empty (`monostate`) `format_arg` for every other ID.
    /// Also set the argument access `status` of the object to the appropriate
    /// value.  Beware that due to the nature of `make_format_args` (that it
    /// works on references) the implementation of this method uses mutable
    /// `static` variables to keep the value around, therefore not suitable for
    /// use in multi-threaded code.
    FormatArgType arg(size_t id) const
    {
        if (d_status != Status::e_NOT_ACCESSED) {
            d_status = Status::e_MORE_THAN_ONE_ARG_ACCESSED;
        }

        if (id == d_argId) {
            if (d_status != Status::e_MORE_THAN_ONE_ARG_ACCESSED) {
                d_status = Status::e_INTEGRAL_ARG_ACCESSED;
            }
            return MockArgsCreator<MockArgsContext>::createIntegralValue(
                                                                      d_value);
        }

        if (id == d_argId + 1) {
            if (d_status != Status::e_MORE_THAN_ONE_ARG_ACCESSED) {
                d_status = Status::e_BAD_TYPE_ARG_ACCESSED;
            }
            return MockArgsCreator<MockArgsContext>::createBadValue();
        }

        if (d_status != Status::e_MORE_THAN_ONE_ARG_ACCESSED) {
            d_status = Status::e_EMPTY_ARG_ACCESSED;
        }
        return FormatArgType();
    }

    /// Return the argument-access `status` of this object.  See also `reset`.
    StatusEnum status() const
    {
        return d_status;
    }

    #undef u_NAMESPACE
    #undef u_NAMESPACE_FMT
};

//=============================================================================
//                             TEMPLATED TESTS
//-----------------------------------------------------------------------------

template <class t_CHAR>
void verifyMockArgsContext()
{
    typedef MockArgsContext<t_CHAR, char, bool> MockContext;

    MockContext ctx(12, 123);
    ASSERT(ctx.status() == MockArgsContextStatus::e_NOT_ACCESSED);

    typename MockContext::FormatArgType arg = ctx.arg(0);
    ASSERT(getArgType(arg) == ArgType::e_MONOSTATE);

    arg = ctx.arg(12);
    ASSERT(getArgType(arg) == ArgType::e_CHAR_TYPE);

    arg = ctx.arg(13);
    ASSERT(getArgType(arg) == ArgType::e_BOOL);
}

template <class t_CHAR>
void verifyNoPostprocessing()
{
    typedef MockArgsContext<t_CHAR, int, bool> MockContext;

    MockContext ctx(42, 42);

    Object obj;
    obj.postprocess(ctx);
    ASSERT(ctx.status() == MockArgsContextStatus::e_NOT_ACCESSED);

    ctx.reset();
    bool formatErrorCaught = false;
    obj = Object(Object::e_NEXT_ARG, 0);
    try {
        obj.postprocess(ctx);
    }
    catch (const bsl::format_error&) {
        formatErrorCaught = true;
    }
    ASSERT(formatErrorCaught);
    ASSERT(ctx.status() == MockArgsContextStatus::e_NOT_ACCESSED);
}

template <class t_CHAR, class t_VALUE_TYPE>
void verifyHasPostprocessingOnType()
{
    typedef MockArgsContext<t_CHAR, t_VALUE_TYPE, bool> MockContext;

    MockContext ctx(3, 127);

    Object obj(Object::e_ARG_ID, 3);
    obj.postprocess(ctx);
    ASSERT(ctx.status() == MockArgsContextStatus::e_INTEGRAL_ARG_ACCESSED);
    ASSERT(obj.category() == Object::e_VALUE);
    ASSERT(obj.value() == 127);

    ctx.reset();
    obj = Object(Object::e_ARG_ID, 2);
    bool caughtFormatError = false;
    try {
        obj.postprocess(ctx);
    }
    catch (const bsl::format_error&) {
        caughtFormatError = true;
    }
    ASSERT(caughtFormatError);
    ASSERT(ctx.status() == MockArgsContextStatus::e_EMPTY_ARG_ACCESSED);
}

template <class t_CHAR, class t_VALUE_TYPE>
void verifyTooLargeValueOnType()
{
    typedef MockArgsContext<t_CHAR, t_VALUE_TYPE, bool> MockContext;

    long long tooLarge = static_cast<long long>(INT_MAX) + 1;
    MockContext ctx(12, tooLarge);

    Object obj(Object::e_ARG_ID, 12);
    bool caughtFormatError = false;
    try {
        obj.postprocess(ctx);
    }
    catch (const bsl::format_error&) {
        caughtFormatError = true;
    }
    ASSERT(caughtFormatError);
    ASSERT(ctx.status() == MockArgsContextStatus::e_INTEGRAL_ARG_ACCESSED);

    ctx.reset();
    ctx.setValue(INT_MAX);
    obj.postprocess(ctx);
    ASSERT(ctx.status() == MockArgsContextStatus::e_INTEGRAL_ARG_ACCESSED);
    ASSERT(obj.category() == Object::e_VALUE);
    ASSERT(obj.value() == INT_MAX);
}

template <class t_CHAR>
void verifyHasPostprocessing()
{
    verifyHasPostprocessingOnType<t_CHAR, t_CHAR            >();
    verifyHasPostprocessingOnType<t_CHAR, int               >();
    verifyHasPostprocessingOnType<t_CHAR, unsigned          >();
    verifyHasPostprocessingOnType<t_CHAR, long long         >();
    verifyHasPostprocessingOnType<t_CHAR, unsigned long long>();

    verifyTooLargeValueOnType<t_CHAR, unsigned          >();
    verifyTooLargeValueOnType<t_CHAR, long long         >();
    verifyTooLargeValueOnType<t_CHAR, unsigned long long>();
}

template <class t_CHAR, class t_BAD_TYPE>
void verifyBadArgumentTypeReferencedWithType()
{
    typedef MockArgsContext<t_CHAR, int, t_BAD_TYPE> MockContext;

    MockContext ctx(5, 42);

    Object obj(Object::e_ARG_ID, 6);
    bool caughtFormatError = false;
    try {
        obj.postprocess(ctx);
    }
    catch (const bsl::format_error&) {
        caughtFormatError = true;
    }
    ASSERT(caughtFormatError);
    ASSERT(ctx.status() == MockArgsContextStatus::e_BAD_TYPE_ARG_ACCESSED);

    ctx.reset();
    obj = Object(Object::e_ARG_ID, 7);
    caughtFormatError = false;
    try {
        obj.postprocess(ctx);
    }
    catch (const bsl::format_error&) {
        caughtFormatError = true;
    }
    ASSERT(caughtFormatError);
    ASSERT(ctx.status() == MockArgsContextStatus::e_EMPTY_ARG_ACCESSED);
}

template <class t_CHAR>
void verifyBadArgumentTypeReferenced()
{
    verifyBadArgumentTypeReferencedWithType<t_CHAR, bool       >();
    verifyBadArgumentTypeReferencedWithType<t_CHAR, float      >();
    verifyBadArgumentTypeReferencedWithType<t_CHAR, double     >();
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
    verifyBadArgumentTypeReferencedWithType<t_CHAR, long double>();
#endif
    verifyBadArgumentTypeReferencedWithType<t_CHAR, const t_CHAR *>();
#if defined(BSLSTL_STRING_VIEW_AND_STD_STRING_VIEW_COEXIST) ||                \
    defined(BSLSTL_STRING_VIEW_IS_ALIASED)
    verifyBadArgumentTypeReferencedWithType<t_CHAR,
                                            std::basic_string_view<t_CHAR> >();
#endif
#ifndef BSLSTL_STRING_VIEW_IS_ALIASED
    verifyBadArgumentTypeReferencedWithType<t_CHAR,
                                            bsl::basic_string_view<t_CHAR> >();
#endif
    verifyBadArgumentTypeReferencedWithType<t_CHAR, const void *>();

    // We cannot create an `std::handle` because it is an unspecified type, so
    // for now we do not test `handle` (even though we could for `bslfmt`).
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char** argv)
{
    const int  test = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose = argc > 2;
    const bool veryVerbose = argc > 3;
    const bool veryVeryVerbose = argc > 4;

    printf("TEST %s CASE %d \n", __FILE__, test);

    switch (test) { case 0:
    case 3: {
        // --------------------------------------------------------------------
        // POSTPROCESS METHOD
        //
        // Concerns:
        // 1. `postprocess` does not change the numeric value if it has
        //    `e_DEFAULT` or `e_VALUE` category, and in such cases it will not
        //    try to access any argument value (via the context)..
        //
        // 2. `postprocess` throws a `format_error` exception if the numeric
        //    value has `e_NEXT_ARG` category, and in such cases it will not
        //    try to access any argument value (via the context)..
        //
        // 3. `postprocess` for `e_ARG_ID` category:
        //    1. accesses the argument with the given id and no other argument
        //
        //    2. throws `format_error` for wrong types (including `bool`)
        //
        //    3. throws `format_error` if the value is larger than `INT_MAX`
        //
        //    4. in case of valid type and value the numeric value is set to
        //       that value with `e_VALUE` category.
        //
        // Plan:
        // 1. Verify everything with both `char` and `wchar_t` char types.
        //
        // 2. Use the mock context to verify the concerns.
        //
        // Testing:
        //   POSTPROCESS METHOD
        // --------------------------------------------------------------------

        if (verbose) puts("\nPOSTPROCESS METHOD"
                          "\n==================");

        if (veryVerbose) puts("\tNo arguments (empty) context");
        {
            verifyNoPostprocessing<char>();
            verifyNoPostprocessing<wchar_t>();
        }

        if (veryVerbose) puts("\tOne argument with value context");
        {
            verifyHasPostprocessing<char>();
            verifyHasPostprocessing<wchar_t>();
        }

        if (veryVerbose) puts("\tBad argument type context");
        {
            verifyBadArgumentTypeReferenced<char>();
            verifyBadArgumentTypeReferenced<wchar_t>();
        }

    } break;
    case 2: {
        // --------------------------------------------------------------------
        // MOCK CONTEXT
        //
        // Concerns:
        // 1. The mock contexts for testing the `postprocess`
        //    1. returns the expected kind of argument for each argument id.
        //    2. maintains the proper argument-access status
        //
        // Plan:
        // 1. Verify everything with both `char` and `wchar_t` char types.
        //
        // Testing:
        //   MOCK CONTEXT
        // --------------------------------------------------------------------

        if (verbose) puts("\nMOCK CONTEXT"
                          "\n============");

        verifyMockArgsContext<char>();
        verifyMockArgsContext<wchar_t>();
    } break;
    case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Create objects with different categories, demonstrate that the
        //    attributes are set properly.
        //
        // 2. Exercise the parse function, assert the result.
        //
        // 3. Exercise the postprocess function, assert the result.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) puts("\nBREATHING TEST"
                          "\n==============");

        if (veryVerbose) puts("\tDefault values");
        {
            Object obj;

            ASSERTV(obj.category(), Object::e_DEFAULT == obj.category());
        }

        if (veryVerbose) puts("\tSpecified values");
        {
            Object obj(Object::e_VALUE, 42);

            ASSERTV(obj.category(), Object::e_VALUE == obj.category());
            ASSERTV(obj.value(), 42 == obj.value());
        }

        if (veryVerbose) puts("\tParse an empty width-like value");
        {
            const char  partialSpec[] = "}";
            const char *start = partialSpec;

            Object obj;
            obj.parse(&start, start + sizeof partialSpec - 1, false);

            ASSERT(partialSpec == start);

            ASSERTV(obj.category(), Object::e_DEFAULT == obj.category());
        }

        if (veryVerbose) puts("\tParse an empty precision-like value");
        {
            const char  partialSpec[] = "}";
            const char *start = partialSpec;

            Object obj;
            obj.parse(&start, start + sizeof partialSpec - 1, true);

            ASSERT(partialSpec == start);

            ASSERTV(obj.category(), Object::e_DEFAULT == obj.category());
        }

        if (veryVerbose) puts("\tParse a direct width-like value");
        {
            const char  partialSpec[] = "123}";
            const char *start = partialSpec;
            const char *const EXPECTED_START = start + sizeof partialSpec - 2;

            Object obj;
            obj.parse(&start, start + sizeof partialSpec - 1, false);

            ASSERT(EXPECTED_START == start);

            ASSERTV(obj.category(), Object::e_VALUE == obj.category());
            ASSERTV(obj.value(), 123 == obj.value());
        }

        if (veryVerbose) puts("\tParse a direct precision-like value");
        {
            const char  partialSpec[] = ".14}";
            const char *start = partialSpec;
            const char *const EXPECTED_START = start + sizeof partialSpec - 2;

            Object obj;
            obj.parse(&start, start + sizeof partialSpec - 1, true);

            ASSERT(EXPECTED_START == start);

            ASSERTV(obj.category(), Object::e_VALUE == obj.category());
            ASSERTV(obj.value(), 14 == obj.value());
        }

        if (veryVerbose) puts("\tParse a missing precision-like value");
        {
            const char  partialSpec[] = ".}";
            const char *start = partialSpec;

            Object obj;
            bool   formatErrorCaught = false;
            try {
                obj.parse(&start, start + sizeof partialSpec - 1, true);
            }
            catch (const bsl::format_error& e) {
                if (veryVeryVerbose) { T_ T_ P(e.what()); }
                formatErrorCaught = true;
            }
            ASSERT(formatErrorCaught);

            ASSERT(partialSpec == start);

            ASSERTV(obj.category(), Object::e_DEFAULT == obj.category());
        }

        if (veryVerbose) puts("\tParse a non-integer precision-like value");
        {
            const char  partialSpec[] = ".a}";
            const char *start = partialSpec;

            Object obj;
            bool   formatErrorCaught = false;
            try {
                obj.parse(&start, start + sizeof partialSpec - 1, true);
            }
            catch (const bsl::format_error& e) {
                if (veryVeryVerbose) { T_ T_ P(e.what()); }
                formatErrorCaught = true;
            }
            ASSERT(formatErrorCaught);
        }

        if (veryVerbose) puts("\tParse a too-large value");
        {
            const char  partialSpec[] = ".999999999999}";
            const char *pstart = partialSpec;
            const char *wstart = partialSpec + 1;

            Object obj;
            bool   formatErrorCaught = false;
            try {
                obj.parse(&wstart,
                          partialSpec + sizeof partialSpec - 1,
                          false);
            }
            catch (const bsl::format_error& e) {
                if (veryVeryVerbose) { T_ T_ P(e.what()); }
                formatErrorCaught = true;
            }
            ASSERT(formatErrorCaught);

            formatErrorCaught = false;
            try {
                obj.parse(&pstart, partialSpec + sizeof partialSpec - 1, true);
            }
            catch (const bsl::format_error& e) {
                if (veryVeryVerbose) { T_ T_ P(e.what()); }
                formatErrorCaught = true;
            }
            ASSERT(formatErrorCaught);
        }

        if (veryVerbose) puts("\tParse next-argument specification");
        {
            const char  partialSpec[] = ".{}}";
            const char *pstart = partialSpec;
            const char *wstart = partialSpec + 1;
            const char *const EXPECTED_START =
                pstart + sizeof partialSpec - 2;

            Object obj;
            obj.parse(&wstart, partialSpec + sizeof partialSpec - 1, false);
            ASSERT(EXPECTED_START == wstart);
            ASSERTV(obj.category(), Object::e_NEXT_ARG == obj.category());

            obj = Object();
            obj.parse(&pstart, partialSpec + sizeof partialSpec - 1, true);
            ASSERT(EXPECTED_START == pstart);
            ASSERTV(obj.category(), Object::e_NEXT_ARG == obj.category());
        }

        if (veryVerbose) puts("\tParse argument ID specification");
        {
            const char  partialSpec[] = ".{123}}";
            const char *pstart = partialSpec;
            const char *wstart = partialSpec + 1;
            const char *const EXPECTED_START =
                pstart + sizeof partialSpec - 2;

            Object obj;
            obj.parse(&wstart, partialSpec + sizeof partialSpec - 1, false);
            ASSERT(EXPECTED_START == wstart);
            ASSERTV(obj.category(), Object::e_ARG_ID == obj.category());
            ASSERTV(obj.value(), 123 == obj.value());

            obj = Object();
            obj.parse(&pstart, partialSpec + sizeof partialSpec - 1, true);
            ASSERT(EXPECTED_START == pstart);
            ASSERTV(obj.category(), Object::e_ARG_ID == obj.category());
            ASSERTV(obj.value(), 123 == obj.value());
        }

        if (veryVerbose) puts("\tParse bad argument ID");
        {
            const char  partialSpec[] = ".{gibberish}}";
            const char *pstart = partialSpec;
            const char *wstart = partialSpec + 1;

            Object obj;
            bool   formatErrorCaught = false;
            try {
                obj.parse(&wstart,
                          partialSpec + sizeof partialSpec - 1,
                          false);
            }
            catch (const bsl::format_error& e) {
                if (veryVeryVerbose) { T_ T_ P(e.what()); }
                formatErrorCaught = true;
            }
            ASSERT(formatErrorCaught);

            formatErrorCaught = false;
            try {
                obj.parse(&pstart, partialSpec + sizeof partialSpec - 1, true);
            }
            catch (const bsl::format_error& e) {
                if (veryVeryVerbose) { T_ T_ P(e.what()); }
                formatErrorCaught = true;
            }
            ASSERT(formatErrorCaught);
        }
    } break;
    default: {
        printf("WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
    }
    }

    if (testStatus > 0) {
        printf("Error, non-zero test status = %d .\n", testStatus);
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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
