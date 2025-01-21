// bslfmt_formatterspecificationnumericvalue.t.cpp                    -*-C++-*-
#include <bslfmt_formatterspecificationnumericvalue.h>

#include <bslfmt_format_arg.h>          // Testing only
#include <bslfmt_format_args.h>         // Testing only
#include <bslfmt_format_string.h>       // Testing only
#include <bslfmt_formatparsecontext.h>  // Testing only
#include <bslfmt_formattertestutil.h>   // Testing only

#include <bsls_bsltestutil.h>

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

// Unfortunately I have no idea how to create a `handle`, so we skip it

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

                    // ========================
                    // struct MockNoArgsContext
                    // ========================

template <class t_CHAR>
struct MockNoArgsContext;

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
namespace std {
/// This specialization of `basic_arg_format` for `MockNoArgsContext` is used
/// when the standard library supports a sufficiently functional format library
/// so that we do not use our own, but rather extend the existing one.  This
/// specialization has to inherit from `basic_format_arg` of `format_context`
/// or `wformat_context` because `std::visit_format_arg` only exists for those.
/// For `MockNoArgsContext` the class can be empty because we work only with
/// `monostate` state that represents a non-existent argument and our code
/// should not use it in any way, so this is sufficient to make things compile.
template <class t_CHAR>
class basic_format_arg<MockNoArgsContext<t_CHAR> >
: public basic_format_arg<typename bsl::conditional<
                                             bsl::is_same<t_CHAR, char>::value,
                                             format_context,
                                             wformat_context>::type>
{
};
}  // close namespace std
#else
namespace BloombergLP {
namespace bslfmt {
template <class t_CHAR>
/// This specialization of `basic_arg_format` for `MockNoArgsContext` is used
/// when the standard library does not provide a sufficiently functional format
/// library so we have to implement our own.  This specialization has to
/// inherit from `basic_format_arg` of `format_context` or `wformat_context`
/// because `visit_format_arg` only exists for those.  For `MockNoArgsContext`
/// the class can be empty because we work only with `monostate` state that
/// represents a non-existent argument and our code should not use it in any
/// way, so this is sufficient to make things compile.
class basic_format_arg<MockNoArgsContext<t_CHAR> >
: public basic_format_arg<typename bsl::conditional<
                                             bsl::is_same<t_CHAR, char>::value,
                                             format_context,
                                             wformat_context>::type>
{
};
}  // close package namespace
}  // close enterprise namespace
#endif

/// A format context that implements the bare minimum of functionality required
/// by the `postprocess` method (the `arg` method) and returns an "no such
/// argument" for each argument identifier (`monostate`) as well as sets a flag
/// that indicates that there was an attempt to access an argument.  This
/// context exists so that we can verify those cases where `postprocess` must
/// not access arguments either because there is nothing to process, or because
/// state is `d_NEXT_ARG` which should lead to an exception.
template <class t_CHAR>
struct MockNoArgsContext {
    // TYPES
    typedef t_CHAR char_type;
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
    typedef std::basic_format_arg<MockNoArgsContext>    FormatArgType;
#else
    typedef bslfmt::basic_format_arg<MockNoArgsContext> FormatArgType;
#endif

    // DATA
    mutable bool d_argAccessed;

    // CREATORS

    /// Create a `MockNoArgsContext` object.
    MockNoArgsContext() : d_argAccessed(false) {}

    // MANIPULATORS

    /// Reset the argument-access flag (see `argAccessed`) to `false`.
    void reset() {
        d_argAccessed = false;
    }

    // ACCESSORS

    /// Return an empty (visits as `monostate`) `format_arg` and set the
    /// `argAccessed` flag (to `true`).
    FormatArgType arg(int) const {
        d_argAccessed = true;
        return FormatArgType();
    }

    /// Return `true` if any argument has been requested since construction or
    /// the last call to `reset`, and `false` otherwise.
    bool argAccessed() const {
        return d_argAccessed;
    }
};

                        // ========================
                        // struct MockOneArgContext
                        // ========================

template <class t_CHAR, class t_VALUE_TYPE>
struct MockOneArgContext;

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
namespace std {
/// This specialization of `basic_arg_format` for `MockOneArgContext` is used
/// when the standard library supports a sufficiently functional format library
/// so that we do not use our own, but rather extend the existing one.  This
/// specialization has to inherit from `basic_format_arg` of `format_context`
/// or `wformat_context` because `std::visit_format_arg` only exists for those.
/// The conversion constructor from the base class exists so that we can use
/// (the only way of creating format-arguments the) `make_format_args` function
/// that does not have a specialization for our context type, only for
/// `std::format_context` and `std::wformat_context`.  See `MockOneArgContext`.
template <class t_CHAR, class t_VALUE_TYPE>
class basic_format_arg<MockOneArgContext<t_CHAR, t_VALUE_TYPE> >
: public basic_format_arg<typename bsl::conditional<
                                             bsl::is_same<t_CHAR, char>::value,
                                             format_context,
                                             wformat_context>::type>
{
  public:

    // TYPES

    // The context type of the base class.
    typedef typename bsl::conditional<bsl::is_same<t_CHAR, char>::value,
                                      format_context,
                                      wformat_context>::type       BaseContext;

    // CREATORS

    /// Create an empty argument that visits as `std::monostate`.
    basic_format_arg() {}

    /// Create a `basic_format_arg` (of a `MockArgContext` type) from an object
    /// of its base class.  This constructor is necessary to make the
    /// conversion in the return statement of `createValue` work.
    basic_format_arg(const basic_format_arg<BaseContext>& other)    // IMPLICIT
    : basic_format_arg<BaseContext>(other)
    {
    }

    // CLASS METHODS

    /// Create a `basic_format_arg` (of a `MockOneArgContext` type) with the
    /// specified `value` and having `t_VALUE_TYPE` as its type.
    static basic_format_arg createValue(long long value)
    {
        static t_VALUE_TYPE typedValue;
        typedValue = static_cast<t_VALUE_TYPE>(value);
        return basic_format_args(
            make_format_args<BaseContext, t_VALUE_TYPE>(typedValue)).get(0);
    }
};
}  // close namespace std
#else
namespace BloombergLP {
namespace bslfmt {
/// This specialization of `basic_arg_format` for a `char`-based
/// `MockOneArgContext` is used when the standard library does not provide a
/// sufficiently functional format library so that we have to implement our
/// own.  This specialization has to inherit from `basic_format_arg` of
/// `format_context` or `wformat_context` because `visit_format_arg` only
/// exists for those.  The conversion constructor from the base class exists so
/// that we can use (the only way of creating format-arguments the)
/// `make_format_args` function that does not have a specialization for our
/// context type, only for `format_context` and `wformat_context`.  See
/// `MockOneArgContext`.
template <class t_VALUE_TYPE>
class basic_format_arg<MockOneArgContext<char, t_VALUE_TYPE> >
: public basic_format_arg<format_context>
{
  public:

    // CREATORS

    /// Create an empty argument that visits as `std::monostate`.
    basic_format_arg() {}

    /// Create a `basic_format_arg` (of a `MockArgContext` type) from an object
    /// of its base class.  This constructor is necessary to make the
    /// conversion in the return statement of `createValue` work.
    basic_format_arg(const basic_format_arg<format_context>& other)
        : basic_format_arg<format_context>(other)
    {
    }

    // CLASS METHODS

    /// Create a `basic_format_arg` (of a `MockOneArgContext` type) with the
    /// specified `value` and having `t_VALUE_TYPE` as its type.
    static basic_format_arg createValue(long long value)
    {
        using namespace bslfmt;

        static t_VALUE_TYPE typedValue;
        typedValue = static_cast<t_VALUE_TYPE>(value);
        return format_args(make_format_args(typedValue)).get(0);
    }
};

/// This specialization of `basic_arg_format` for a `wchar_t`-based
/// `MockOneArgContext` is used when the standard library does not provide a
/// sufficiently functional format library so that we have to implement our
/// own.  This specialization has to inherit from `basic_format_arg` of
/// `format_context` or `wformat_context` because `visit_format_arg` only
/// exists for those.  The conversion constructor from the base class exists so
/// that we can use (the only way of creating format-arguments the)
/// `make_format_args` function that does not have a specialization for our
/// context type, only for `format_context` and `wformat_context`.
template <class t_VALUE_TYPE>
class basic_format_arg<MockOneArgContext<wchar_t, t_VALUE_TYPE> >
: public basic_format_arg<wformat_context>
{
  public:

    // CREATORS

    /// Create an empty argument that visits as `std::monostate`.
    basic_format_arg() {}

    /// Create a `basic_format_arg` (of a `MockArgContext` type) from an object
    /// of its base class.  This constructor is necessary to make the
    /// conversion in the return statement of `createValue` work.
    basic_format_arg(const basic_format_arg<wformat_context>& other)
        : basic_format_arg<wformat_context>(other)
    {
    }

    // CLASS METHODS

    /// Create a `basic_format_arg` (of a `MockOneArgContext` type) with the
    /// specified `value` and having `t_VALUE_TYPE` as its type.
    static basic_format_arg createValue(long long value)
    {
        using namespace bslfmt;

        static t_VALUE_TYPE typedValue;
        typedValue = static_cast<t_VALUE_TYPE>(value);
        return wformat_args(make_wformat_args(typedValue)).get(0);
    }
};

}  // close package namespace
}  // close enterprise namespace
#endif

/// A format context that implements the bare minimum of functionality required
/// by the `postprocess` method (the `arg` method).  It serves one argument
/// for an argument identifier and value stored as attributes, and the
/// `t_VALUE_TYPE` type.  All other argument identifiers will return an "empty
/// argument" (`monostate`) as well as sets a flag that indicates that there
/// was an attempt to access a wrong argument.  This context exists so that we
/// can verify the `e_ARG_ID` case where `postprocess` must access an argument
/// to retrieve the numeric value, and must not access any other argument.  For
/// this to work `t_VALUE_TYPE` must be one of the acceptable integral types of
/// `char_type`, `int`, `unsigned`, `long long`, or `unsigned long long`.
template <class t_CHAR, class t_VALUE_TYPE>
struct MockOneArgContext {
    // TYPES
    typedef t_CHAR       char_type;  // Required by `ArgTypingVisitor`

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
    typedef std::basic_format_arg<MockOneArgContext>    FormatArgType;
#else
    typedef bslfmt::basic_format_arg<MockOneArgContext> FormatArgType;
#endif

    // DATA
    int          d_argId;
    long long    d_value;

    mutable bool d_wrongArgAccessed;

    // CREATORS

    /// Create a `MockOneArgContext` that provides the specified `value` with
    /// the type `t_VALUE_TYPE` for the specified `argId` while providing empty
    /// (`monostate`) arguments for any other argument id, and has its
    /// `wrongArgAccessed` attribute reset.
    MockOneArgContext(int argId, long long value)
    : d_argId(argId)
    , d_value(value)
    , d_wrongArgAccessed(false)
    {
    }

    // MANIPULATORS

    // Reset (to `false`) this object's `wrongArgAccessed` attribute.
    void reset() {
        d_wrongArgAccessed = false;
    }

    /// Change the `value` attribute of this object (the value it provides for
    /// the one argument of `argId`).
    void setValue(long long value) {
        d_value = value;
    }

    // ACCESSORS

    /// Return the format argument for the specified argument `id`.  When `id`
    /// is the same as the `argId` attribute return a `format_arg` with the
    /// type `t_VALUE_TYPE` and the value of the `value` attribute.  Return an
    /// empty (`monostate`) `format_arg` for every other ID and also set the
    /// `wrongArgAccessed` attribute (to `true`).
    FormatArgType arg(int id) const {
        if (id != d_argId) {
            d_wrongArgAccessed = true;
            return FormatArgType();                                   // RETURN
        }

        return FormatArgType::createValue(d_value);
    }

    /// Return `true` if an argument with different ID than expected (the ID
    /// specified during construction) has been requested since construction or
    /// the last call to `reset`, and `false` otherwise.
    bool wrongArgAccessed() const {
        return d_wrongArgAccessed;
    }
};

                        // ========================
                        // struct MockBadArgContext
                        // ========================

template <class t_CHAR, class t_VALUE_TYPE>
struct MockBadArgContext;

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
namespace std {
/// This specialization of `basic_arg_format` for `MockBadArgContext` is used
/// when the standard library supports a sufficiently functional format library
/// so that we do not use our own, but rather extend the existing one.  This
/// specialization has to inherit from `basic_format_arg` of `format_context`
/// or `wformat_context` because `std::visit_format_arg` only exists for those.
/// The conversion constructor from the base class exists so that we can use
/// (the only way of creating format-arguments the) `make_format_args` function
/// that does not have a specialization for our context type, only for
/// `std::format_context` and `std::wformat_context`.  See `MockBadArgContext`.
template <class t_CHAR, class t_VALUE_TYPE>
class basic_format_arg<MockBadArgContext<t_CHAR, t_VALUE_TYPE> >
: public basic_format_arg<typename bsl::conditional<
                                             bsl::is_same<t_CHAR, char>::value,
                                             format_context,
                                             wformat_context>::type>
{
  public:

    // TYPES

    /// The context type of the base class.
    typedef typename bsl::conditional<bsl::is_same<t_CHAR, char>::value,
                                      format_context,
                                      wformat_context>::type       BaseContext;

    // CREATORS

    /// Create an empty argument that visits as `std::monostate`.
    basic_format_arg() {}

    /// Create a `basic_format_arg` (of a `MockBadContext` type) from an object
    /// of its base class.  This constructor is necessary to make the
    /// conversion in the return statement of `createValue` work.
    basic_format_arg(const basic_format_arg<BaseContext>& other)    // IMPLICIT
    : basic_format_arg<BaseContext>(other)
    {
    }

    // CLASS METHODS

    /// Create a `basic_format_arg` (of a `MockBadArgContext` type) with an
    /// unspecified value and having `t_VALUE_TYPE` as its type.  This method
    /// is used to create type that cannot be used as numeric values (width or
    /// precision).
    static basic_format_arg createBadType()
    {
        static t_VALUE_TYPE x;
        basic_format_args fas = make_format_args<BaseContext, t_VALUE_TYPE>(x);
        return fas.get(0);
    }
};
}  // close namespace std
#else
namespace BloombergLP {
namespace bslfmt {
/// This specialization of `basic_arg_format` for a `char`-based
/// `MockBadArgContext` is used when the standard library does not provide a
/// sufficiently functional format library so that we have to implement our
/// own.  This specialization has to inherit from `basic_format_arg` of
/// `format_context` or `wformat_context` because `visit_format_arg` only
/// exists for those.  The conversion constructor from the base class exists so
/// that we can use (the only way of creating format-arguments the)
/// `make_format_args` function that does not have a specialization for our
/// context type, only for `format_context` and `wformat_context`.  See
/// `MockBadArgContext`.
template <class t_VALUE_TYPE>
class basic_format_arg<MockBadArgContext<char, t_VALUE_TYPE> >
: public basic_format_arg<format_context>
{
  public:

    // CREATORS

    /// Create an empty argument that visits as `std::monostate`.
    basic_format_arg() {}

    /// Create a `basic_format_arg` (of a `MockBadContext` type) from an object
    /// of its base class.  This constructor is necessary to make the
    /// conversion in the return statement of `createValue` work.
    basic_format_arg(const basic_format_arg<format_context>& other)
    : basic_format_arg<format_context>(other) {}

    // CLASS METHODS

    /// Create a `basic_format_arg` (of a `MockBadArgContext` type) with an
    /// unspecified value and having `t_VALUE_TYPE` as its type.  This method
    /// is used to create type that cannot be used as numeric values (width or
    /// precision).
    static basic_format_arg createBadType()
    {
        using namespace bslfmt;

        static t_VALUE_TYPE x;
        return format_args(make_format_args(x)).get(0);
    }
};

/// This specialization of `basic_arg_format` for a `wchar_t`-based
/// `MockBadArgContext` is used when the standard library does not provide a
/// sufficiently functional format library so that we have to implement our
/// own.  This specialization has to inherit from `basic_format_arg` of
/// `format_context` or `wformat_context` because `visit_format_arg` only
/// exists for those.  The conversion constructor from the base class exists so
/// that we can use (the only way of creating format-arguments the)
/// `make_format_args` function that does not have a specialization for our
/// context type, only for `format_context` and `wformat_context`.  See
/// `MockBadArgContext`.
template <class t_VALUE_TYPE>
class basic_format_arg<MockBadArgContext<wchar_t, t_VALUE_TYPE> >
: public basic_format_arg<wformat_context>
{
  public:

    // CREATORS

    /// Create an empty argument that visits as `std::monostate`.
    basic_format_arg() {}

    /// Create a `basic_format_arg` (of a `MockBadContext` type) from an object
    /// of its base class.  This constructor is necessary to make the
    /// conversion in the return statement of `createValue` work.
    basic_format_arg(const basic_format_arg<wformat_context>& other)
    : basic_format_arg<wformat_context>(other) {}

    // CLASS METHODS

    /// Create a `basic_format_arg` (of a `MockBadArgContext` type) with an
    /// unspecified value and having `t_VALUE_TYPE` as its type.  This method
    /// is used to create type that cannot be used as numeric values (width or
    /// precision).
    static basic_format_arg createBadType()
    {
        using namespace bslfmt;

        static t_VALUE_TYPE x;
        return wformat_args(make_wformat_args(x)).get(0);;
    }
};
}  // close package namespace
}  // close enterprise namespace
#endif

/// A format context that implements the bare minimum of functionality required
/// by the `postprocess` method (the `arg` method).  It serves one argument
/// for an argument identifier stored as attribute with an unspecified value,
/// and the `t_VALUE_TYPE` type.  All other argument identifiers will return an
/// "empty argument" (`monostate`) as well as sets a flag that indicates that
/// there was an attempt to access a wrong argument.  This context exists so
/// that we can verify the `e_ARG_ID` case where `postprocess` must access an
/// argument to retrieve the numeric value (and must not access any other
/// argument), but that argument is of the wrong type.  For this to work
/// `t_VALUE_TYPE` must be one of the not-acceptable types of
/// `bool`, `float`, `double`, `long double`, `const cnhar_type *`, `handle`,
/// `const void *`, or `basic_string_view<char_type>`.
template <class t_CHAR, class t_VALUE_TYPE>
struct MockBadArgContext {
    // TYPES
    typedef t_CHAR       char_type;

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
    typedef std::basic_format_arg<MockBadArgContext>    FormatArgType;
#else
    typedef bslfmt::basic_format_arg<MockBadArgContext> FormatArgType;
#endif

    // DATA
    int          d_argId;

    mutable bool d_wrongArgAccessed;

    // CREATORS

    /// Create a `MockBadArgContext` that provides an unspecified value with
    /// the type `t_VALUE_TYPE` for the specified `argId` while providing empty
    /// (`monostate`) arguments for any other argument id, and has its
    /// `wrongArgAccessed` attribute reset.
    MockBadArgContext(int argId)
    : d_argId(argId)
    , d_wrongArgAccessed(false)
    {
    }

    // MANIPULATORS

    // Reset (to `false`) this object's `wrongArgAccessed` attribute.
    void reset() {
        d_wrongArgAccessed = false;
    }

    // ACCESSORS

    /// Return a `format_arg` of `t_VALUE_TYPE` with an unspecified value if
    /// the specified argument `id` is the same as the `argId` attribute.
    /// Return an empty (`monostate`) `format_arg` for every other ID and also
    /// set the `wrongArgAccessed` attribute (to `true`).
    FormatArgType arg(int id) const {
        if (id != d_argId) {
            d_wrongArgAccessed = true;
            return FormatArgType();                                   // RETURN
        }

        return FormatArgType::createBadType();
    }

    /// Return `true` if an argument with different ID than expected (the ID
    /// specified during construction) has been requested since construction or
    /// the last call to `reset`, and `false` otherwise.
    bool wrongArgAccessed() const {
        return d_wrongArgAccessed;
    }
};

//=============================================================================
//                             TEMPLATED TESTS
//-----------------------------------------------------------------------------

template <class t_CHAR>
void verifyNoPostprocessing()
{
    MockNoArgsContext<char> ctx;
    ASSERT(ctx.argAccessed() == false);
    MockNoArgsContext<char>::FormatArgType arg = ctx.arg(0);
    ASSERT(getArgType(arg) == ArgType::e_MONOSTATE);

    ASSERT(ctx.argAccessed() == true);
    ctx.reset();

    arg = ctx.arg(1234);
    ASSERT(getArgType(arg) == ArgType::e_MONOSTATE);
    ASSERT(ctx.argAccessed() == true);
    ctx.reset();

    Object obj;
    obj.postprocess(ctx);
    ASSERT(ctx.argAccessed() == false);

    bool formatErrorCaught = false;
    obj = Object(Object::e_NEXT_ARG, 0);
    try {
        obj.postprocess(ctx);
    }
    catch (const bsl::format_error&) {
        formatErrorCaught = true;
    }
    ASSERT(formatErrorCaught);
    ASSERT(ctx.argAccessed() == false);
}

template <class t_CHAR, class t_VALUE_TYPE>
void verifyHasPostprocessingOnType()
{
    typedef MockOneArgContext<t_CHAR, t_VALUE_TYPE> MockContext;

    MockContext ctx(3, 42);

    typename MockContext::FormatArgType arg = ctx.arg(3);
    ASSERT((getArgType(arg) == TypeToEnum<t_CHAR, t_VALUE_TYPE>::value));
    ASSERT(ctx.wrongArgAccessed() == false);

    arg = ctx.arg(1);
    ASSERT(getArgType(arg) == ArgType::e_MONOSTATE);
    ASSERT(ctx.wrongArgAccessed() == true);
    ctx.reset();

    Object obj(Object::e_ARG_ID, 3);
    obj.postprocess(ctx);
    ASSERT(ctx.wrongArgAccessed() == false);
    ASSERT(obj.category() == Object::e_VALUE);
    ASSERT(obj.value() == 42);

    obj = Object(Object::e_ARG_ID, 2);
    bool caughtFormatError = false;
    try {
        obj.postprocess(ctx);
    }
    catch (const bsl::format_error&) {
        caughtFormatError = true;
    }
    ASSERT(caughtFormatError);
    ASSERT(ctx.wrongArgAccessed() == true);
}

template <class t_CHAR, class t_VALUE_TYPE>
void verifyTooLargeValueOnType()
{
    typedef MockOneArgContext<t_CHAR, t_VALUE_TYPE> MockContext;

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
    ASSERT(ctx.wrongArgAccessed() == false);

    ctx.setValue(INT_MAX);
    obj.postprocess(ctx);
    ASSERT(ctx.wrongArgAccessed() == false);
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

template <class t_CHAR, class t_VALUE_TYPE>
void verifyBadArgumentTypeReferencedWithType()
{
    typedef MockBadArgContext<t_CHAR, t_VALUE_TYPE> MockContext;

    MockContext ctx(5);

    typename MockContext::FormatArgType arg = ctx.arg(5);
    ASSERT((getArgType(arg) == TypeToEnum<t_CHAR, t_VALUE_TYPE>::value));
    ASSERT(ctx.wrongArgAccessed() == false);

    arg = ctx.arg(1);
    ASSERT(getArgType(arg) == ArgType::e_MONOSTATE);
    ASSERT(ctx.wrongArgAccessed() == true);
    ctx.reset();

    Object obj(Object::e_ARG_ID, 5);
    bool caughtFormatError = false;
    try {
        obj.postprocess(ctx);
    }
    catch (const bsl::format_error&) {
        caughtFormatError = true;
    }
    ASSERT(caughtFormatError);
    ASSERT(ctx.wrongArgAccessed() == false);

    obj = Object(Object::e_ARG_ID, 3);
    caughtFormatError = false;
    try {
        obj.postprocess(ctx);
    }
    catch (const bsl::format_error&) {
        caughtFormatError = true;
    }
    ASSERT(caughtFormatError);
    ASSERT(ctx.wrongArgAccessed() == true);
}

template <class t_CHAR>
void verifyBadArgumentTypeReferenced()
{
    verifyBadArgumentTypeReferencedWithType<t_CHAR, bool                           >();
    verifyBadArgumentTypeReferencedWithType<t_CHAR, float                          >();
    verifyBadArgumentTypeReferencedWithType<t_CHAR, double                         >();
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
    verifyBadArgumentTypeReferencedWithType<t_CHAR, long double                    >();
#endif
    verifyBadArgumentTypeReferencedWithType<t_CHAR, const t_CHAR*                  >();
#if defined(BSLSTL_STRING_VIEW_AND_STD_STRING_VIEW_COEXIST) ||                \
    defined(BSLSTL_STRING_VIEW_IS_ALIASED)
    verifyBadArgumentTypeReferencedWithType<t_CHAR,
                                            std::basic_string_view<t_CHAR> >();
#endif
#ifndef BSLSTL_STRING_VIEW_IS_ALIASED
    verifyBadArgumentTypeReferencedWithType<t_CHAR,
                                            bsl::basic_string_view<t_CHAR> >();
#endif
    verifyBadArgumentTypeReferencedWithType<t_CHAR, const void*                    >();
    // We cannot create `handle` so we won't test it.
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
    case 2: {
        // --------------------------------------------------------------------
        // MOCK CONTEXTS AND POSTPROCESS
        //
        // Concerns:
        // 1. The contexts for testing the `postprocess` method are functional.
        //
        // Plan:
        // 1. Verify everything with both `char` and `wchar_t` char types.
        //
        // 2. Verify that the "empty" mock context gives empty (`monostate`)
        //    arguments for any argument ID.
        //
        // 3. Use the empty mock context to verify that numeric values of
        //    states `e_DEFAULT` and `e_VALUE` are not changed during
        //    postprocessing.  The empty mock context serves to verify that the
        //    postprocessing does not attempt to access any arguments.
        //
        // 4. Use the empty mock context to verify that numeric values of
        //    state `e_NEXT_ARG` result in a `bsl::format_error` thrown during
        //    postprocessing.  The empty mock context serves to verify that the
        //    postprocessing does not attempt to access any arguments.
        //
        // 5. Verify that the "one argument" mock context gives empty
        //    (`monostate`) for arguments for any argument ID except the one
        //    with value, and that the one with value gives a value with the
        //    requested type.
        //
        // 6. Use the one-argument mock context to verify that numeric values
        //    state `e_ARG_ID` postprocess into an `e_VALUE` with the given
        //    value that the mock context provides.  Also verify that a
        //    non-matching (to the value) argument ID results in
        //    `bsl::format_error` exception thrown.  Do that for all accepted
        //    value types (`char_type`, `int`, `unsigned`, `long long`,
        //    `unsigned long long`).
        //
        // 7. With a one-argument mock context that provides a too large value
        //    (one that does not fit an integer) verify that postprocessing
        //    results in `bsl::format_error` exception thrown.  Also verify
        //    that the just-acceptable maximum (`INT_MAX`) is accepted.  Do
        //    that for all value types that may store such large number
        //    (`unsigned`, `long long`, `unsigned long long`).
        //
        // 8. Verify that the bad-argument mock context gives the requested
        //    argument type for the given argument ID, and `monostate` for all
        //    others.
        //
        // 9. Using the bad-argument mock context verify that prostprocessing
        //    a `e_ARG_ID` that refers to a bad argument type results in a
        //    `bsl::format_error` exception thrown.  Do this for all types that
        //    are not supported (except for `monostate` as we have tested that
        //    everywhere else).
        //
        // Testing:
        //   MOCK CONTEXTS AND POSTPROCESS
        // --------------------------------------------------------------------

        if (verbose) puts("\nMOCK CONTEXTS AND POSTPROCESS"
                          "\n=============================");

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
