// bslstl_formatargs.t.cpp                                            -*-C++-*-
#include <bslfmt_formatargs.h>

#include <bsls_bsltestutil.h>

#include <bslstl_string.h>

#include <bslfmt_formatarg.h>

#include <stdio.h>
#include <string.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// 'basic_format_args<basic_format_context<t_OUT, t_CHAR> >' where t_OUT is
// an output iterator is a standard-compliant implementation of
// 'std::basic_format_args'. It is hard to test standalone as it is designed
// to be constructed only indirectly by the 'bslfmt::format' suite of
// functions, and such testing requires the creation of "mock" contexts.
//
// It should meet the requirements specified in [format.string.std].
//
//-----------------------------------------------------------------------------
// CLASS 'bsl::basic_format_args'
//
// CREATORS
// [ 3] basic_format_args();
// [ 3] ~basic_format_args();
// [ 7] basic_format_args(const basic_format_args &);
// [12] basic_format_args(Format_FormatArgStore);
//
// MANIPULATORS
// [ 6] operator==(const basic_format_args &);
// [ 9] operator=(const basic_format_args &);
//
// ACCESSORS
// [ 4] get(size_t);
//
// FREE FUNCTIONS
// [ 8] swap(basic_format_args &, basic_format_args &);
// [11] make_format_args(ARGS&...)
// [11] make_wformat_args(ARGS&...)
//
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] TESTING PRIMARY MANIPULATORS: Not Applicable
// [ 5] TESTING OUTPUT:               Not Applicable
// [10] STREAMING FUNCTIONALITY:      Not Applicable
// [13] USAGE EXAMPLE
//-----------------------------------------------------------------------------

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
//                     GLOBAL HELPER CLASSES FOR TESTING
// ----------------------------------------------------------------------------

namespace BloombergLP {
namespace bslfmt {

/// A minimum implementation of a format context output iterator to enable arg
/// testing
template <class t_VALUE>
class Format_OutputIteratorRef {
  public:
    // TYPES
    typedef bsl::output_iterator_tag iterator_category;
    typedef void                     difference_type;
    typedef t_VALUE                  value_type;
    typedef void                     reference;
    typedef void                     pointer;
};

/// A minimum implementation of a format context to enable arg testing
template <class t_OUT, class t_CHAR>
class basic_format_context {
  private:
    // DATA
    t_OUT d_out;

  public:
    // TYPES
    typedef t_OUT  iterator;
    typedef t_CHAR char_type;

    // CREATORS
    basic_format_context(t_OUT out)
    : d_out(out)
    {
    }

    // MANIPULATORS
    iterator out() { return d_out; }

    void advance_to(iterator it) { d_out = it; }
};

/// A minimum implementation of a format parse context to enable arg testing
template <class t_CHAR>
class basic_format_parse_context {
  public:
    // TYPES
    typedef t_CHAR char_type;
    typedef
        typename bsl::basic_string_view<t_CHAR>::const_iterator const_iterator;
    typedef const_iterator                                      iterator;

  private:
    // DATA
    iterator d_begin;
    iterator d_end;

  public:
    // CREATORS
    BSLS_KEYWORD_CONSTEXPR_CPP20 explicit basic_format_parse_context(
                      bsl::basic_string_view<t_CHAR> fmt) BSLS_KEYWORD_NOEXCEPT
    : d_begin(fmt.begin())
    , d_end(fmt.end())
    {
    }

    BSLS_KEYWORD_CONSTEXPR_CPP20 explicit basic_format_parse_context(
                  bsl::basic_string_view<t_CHAR> fmt,
                  size_t                         numArgs) BSLS_KEYWORD_NOEXCEPT
    : d_begin(fmt.begin())
    , d_end(fmt.end())
    {
    }

    // MANIPULATORS
    BSLS_KEYWORD_CONSTEXPR_CPP20 void advance_to(const_iterator it)
    {
        d_begin = it;
    }

    // ACCESSORS
    BSLS_KEYWORD_CONSTEXPR_CPP20 const_iterator
    begin() const BSLS_KEYWORD_NOEXCEPT
    {
        return d_begin;
    }

    BSLS_KEYWORD_CONSTEXPR_CPP20 const_iterator
    end() const BSLS_KEYWORD_NOEXCEPT
    {
        return d_end;
    }

  private:
    // NOT IMPLEMENTED
    basic_format_parse_context(
                       const basic_format_parse_context&) BSLS_KEYWORD_DELETED;
    basic_format_parse_context& operator=(
                       const basic_format_parse_context&) BSLS_KEYWORD_DELETED;
};

}
}

namespace {

template <class t_OUT, class t_CHAR>
struct TestVisitor {
  private:
    // PRIVATE TYPES
    typedef typename bslfmt::basic_format_arg<
        bslfmt::basic_format_context<t_OUT, t_CHAR> >::handle handle;

  public:
    // PUBLIC DATA

    bool                            d_bool;
    t_CHAR                          d_char;
    int                             d_int;
    unsigned                        d_unsigned;
    long long                       d_longlong;
    unsigned long long              d_unsignedlonglong;
    float                           d_float;
    double                          d_double;
    long double                     d_longdouble;
    const t_CHAR                   *d_charstar;
    const void                     *d_voidstar;
    bsl::basic_string_view<t_CHAR>  d_stringview;
    bool                            d_ishandle;

    // CREATORS

    TestVisitor()
    : d_bool(false)
    , d_char(0)
    , d_int(0)
    , d_unsigned(0)
    , d_longlong(0)
    , d_unsignedlonglong(0)
    , d_float(0)
    , d_double(0)
    , d_longdouble(0)
    , d_charstar(0)
    , d_voidstar(0)
    , d_ishandle(false) {};

    // MANIPULATORS

    void operator()(bsl::monostate) const {}

    void operator()(bool x) { d_bool = x; }
    void operator()(t_CHAR x) { d_char = x; }
    void operator()(unsigned x) { d_unsigned = x; }
    void operator()(long long x) { d_longlong = x; }
    void operator()(unsigned long long x) { d_unsignedlonglong = x; }
    void operator()(float x) { d_float = x; }
    void operator()(double x) { d_double = x; }
    void operator()(long double x) { d_longdouble = x; }
    void operator()(const t_CHAR *x) { d_charstar = x; }
    void operator()(const void *x) { d_voidstar = x; }
    void operator()(int x) { d_int = x; }
    void operator()(bsl::basic_string_view<t_CHAR> sv)
    {
        d_stringview = sv;
    }
    void operator()(const handle& h)
    {
        t_OUT dummy = t_OUT();

        bslfmt::basic_format_context<t_OUT, t_CHAR> fc(dummy);
        bsl::basic_string_view<t_CHAR>              fmt;
        bslfmt::basic_format_parse_context<t_CHAR>  pc(fmt);

        h.format(pc, fc);
        d_ishandle = true;
    }
};

/// A minimal user defined type for testing.
struct FormattableType {
    // PUBLIC DATA
    int x;

    // CREATORS
    FormattableType(int v)
    : x(v)
    {}

    // PUBLIC CLASS DATA
    static int parseCalls;
    static int formatSum;
};

int FormattableType::parseCalls = 0;
int FormattableType::formatSum = 0;
}

namespace bsl {
// FORMATTER SPECIALIZATIONS

/// A basic test formatter whose sole purpose is to record any calls made to
/// its member functions.
template <class t_CHAR>
struct formatter<FormattableType, t_CHAR> {
  public:
    // TRAITS
    // BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20;

    // MANIPULATORS
    template <class t_PARSE_CONTEXT>
    BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator parse(
                                                           t_PARSE_CONTEXT& pc)
    {
        FormattableType::parseCalls++;
        return pc.begin();
    }

    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(const FormattableType& v,
                                               t_FORMAT_CONTEXT& fc) const
    {
        FormattableType::formatSum += v.x;
        return fc.out();
    }
};

}



// ============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

template <class t_CHAR, class t_TYPE>
bslfmt::basic_format_arg<bslfmt::basic_format_context<t_CHAR *, t_CHAR> >
makeTestArg(const t_TYPE& val)
{
    bsl::array<bslfmt::basic_format_arg<
                   bslfmt::basic_format_context<t_CHAR *, t_CHAR> >,
               1>
        arr;

    bslfmt::Format_FormatArg_ImpUtil::makeFormatArgArray(&arr, val);

    return arr[0];
}

template <class t_OUT, class t_CHAR>
bool checkValue(
 bslfmt::basic_format_arg<bslfmt::basic_format_context<t_OUT, t_CHAR> > arg,
 bool                                                                   value)
{
    TestVisitor<t_OUT, t_CHAR> visitor;

    arg.visit(visitor);

    TestVisitor<t_OUT, t_CHAR> visitor2;

    visit_format_arg(visitor2, arg);

    return visitor.d_bool == value && visitor2.d_bool == value;
}

template <class t_OUT, class t_CHAR>
bool checkValue(
 bslfmt::basic_format_arg<bslfmt::basic_format_context<t_OUT, t_CHAR> > arg,
 t_CHAR                                                                 value)
{
    TestVisitor<t_OUT, t_CHAR> visitor;

    arg.visit(visitor);

    TestVisitor<t_OUT, t_CHAR> visitor2;

    visit_format_arg(visitor2, arg);

    return visitor.d_char == value && visitor2.d_char == value;
}

template <class t_OUT, class t_CHAR>
bool checkValue(
 bslfmt::basic_format_arg<bslfmt::basic_format_context<t_OUT, t_CHAR> > arg,
 int                                                                    value)
{
    TestVisitor<t_OUT, t_CHAR> visitor;

    arg.visit(visitor);

    TestVisitor<t_OUT, t_CHAR> visitor2;

    visit_format_arg(visitor2, arg);

    return visitor.d_int == value && visitor2.d_int == value;
}

template <class t_OUT, class t_CHAR>
bool checkValue(
 bslfmt::basic_format_arg<bslfmt::basic_format_context<t_OUT, t_CHAR> > arg,
 long double                                                            value)
{
    TestVisitor<t_OUT, t_CHAR> visitor;

    arg.visit(visitor);

    TestVisitor<t_OUT, t_CHAR> visitor2;

    visit_format_arg(visitor2, arg);

    return visitor.d_longlong == value && visitor2.d_longlong == value;
}

template <class t_OUT, class t_CHAR>
bool checkFormattableTypeValue(
        bslfmt::basic_format_arg<bslfmt::basic_format_context<t_OUT, t_CHAR> >
            arg,
        int handleValue)
{
    FormattableType::parseCalls = 0;
    FormattableType::formatSum  = 0;

    TestVisitor<t_OUT, t_CHAR> visitor;

    arg.visit(visitor);

    TestVisitor<t_OUT, t_CHAR> visitor2;

    visit_format_arg(visitor2, arg);

    return (FormattableType::parseCalls == 2) &&
           (FormattableType::formatSum == 2 * handleValue);
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------


int main(int argc, char **argv)
{
    const int  test    = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose = argc > 2;
    // const bool veryVerbose = argc > 3;

    printf("TEST %s CASE %d \n", __FILE__, test);

    switch (test) {  case 0:
      case 13: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern:
        //: 1 Demonstrate the functioning of this component.
        //
        // Plan:
        //: 1 Construct an instance and verify it holds no value.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

///Example: Default construction and value verification
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// We do not expect most users of `bsl::format` to interact with this type
// directly and instead use `bsl::format` or `bsl::vformat`. In addition, there
// are only a very limited number of public methods so this example is
// necessarily unrealistic.
//
// Suppose we want to construct a default-constructed `basic_format_arg` and
// verify that it contains no value.
//
//..
        bslfmt::basic_format_arg<
            bslfmt::basic_format_context<char *, char> > arg;

        ASSERT(!arg);
//..
//
      } break;
      case 12: {
        // --------------------------------------------
        // TESTING CONSTRUCTION FROM ARG STORE - WORK IN PROGRESS
        //
        // Testing:
        //   basic_format_args(Format_FormatArgStore);
        // --------------------------------------------
        if (verbose)
            printf("\nCONSTRUCTION FROM ARG STORE"
                   "\n= =========================\n");

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING FREE FUNCTIONS - WORK IN PROGRESS
        //
        // Concerns:
        //: 1 make_format_args returns an object containing a `basic_format_arg` containing the passed value(s) or reference(s) thereto.
        //:
        //: 2 Where the contained type is a reference (stored in a `handle`) to
        //:   a user defined type, calling `handle::format` will construct an
        //:   appropriate `formatter` and call `format` and `parse` on that
        //:   formatter.
        //
        // Plan:
        //: 1 For each constructor not taking a user defined type, construct a
        //:   `basic_format_arg` containing a known value
        //:
        //: 2 For the type object constructed in step 1, use a `TestVisitor` to
        //:   verify that the visitor mechanism passes the contained value to
        //:   the passed `TestVisitor`. (C-1)
        //:
        //: 3 For the constructor taking a user defined type, construct a
        //:   `basic_format_arg` containing a `FormattableType` reference.
        //:
        //: 4 For the type object constructed in step 1, use a `TestVisitor` to
        //:   call `handle::format` and verify that this creates an instance of
        //:   the correct (testing) partial specialization of `formatter` and
        //:   calls both `parse` and `format` on that instance. (C-2)
        //
        // Testing:
        //   make_format_args(ARGS&...)
        //   make_Wformat_args(ARGS&...)
        // --------------------------------------------------------------------
        if (verbose)
            printf("\nFREE FUNCTIONS"
                   "\n==============\n");

    //bool                            d_bool;
    //    t_CHAR                          d_char;
    //    int                             d_int;
    //    unsigned                        d_unsigned;
    //    long long                       d_longlong;
    //    unsigned long long              d_unsignedlonglong;
    //    float                           d_float;
    //    double                          d_double;
    //    long double                     d_longdouble;
    //    const t_CHAR                   *d_charstar;
    //    const void                     *d_voidstar;
    //    bsl::basic_string_view<t_CHAR>  d_stringview;
    //    bool                            d_ishandle;

#if 0
        typedef bslfmt::basic_format_arg<bslfmt::format_context> FA;

        typedef bslfmt::basic_format_arg<bslfmt::wformat_context> WFA;

        typedef bslfmt::Format_FormatArgStore<bslfmt::format_context>
            FAS;
        typedef bslfmt::Format_FormatArgStore<bslfmt::wformat_context>
            WFAS;

        typedef bslfmt::Format_FormatArgStore<bslfmt::format_context, bool>
            FAS1;
        typedef bslfmt::Format_FormatArgStore<bslfmt::wformat_context, bool>
            WFAS1;

        typedef bslfmt::Format_FormatArgStore<bslfmt::format_context, char, char>
            FAS2;
        typedef bslfmt::Format_FormatArgStore<bslfmt::wformat_context, wchar_t, wchar_t>
            WFAS2;

        typedef bslfmt::Format_FormatArgStore<bslfmt::format_context, unsigned, long long, unsigned long long>
            FAS3I;
        typedef bslfmt::Format_FormatArgStore<bslfmt::wformat_context,
                                              unsigned,
                                              long long,
                                              unsigned long long>
            WFAS3I;

        typedef bslfmt::Format_FormatArgStore<bslfmt::format_context, float, double, long double>
            FAS3F;
        typedef bslfmt::Format_FormatArgStore<bslfmt::wformat_context, bool>
            WFAS3F;

        typedef bslfmt::Format_FormatArgStore<bslfmt::format_context, int>
            FASI;
        typedef bslfmt::Format_FormatArgStore<bslfmt::wformat_context, int>
            WFASI;

        typedef bslfmt::Format_FormatArgStore<bslfmt::format_context,
                                              FormattableType>
            FAST;
        typedef bslfmt::Format_FormatArgStore<bslfmt::wformat_context,
                                              FormattableType>
            WFAST;

        if (verbose)
            printf("\nValidating visit functionality\n");
        {
            int val = 5;

            FAS temp = bslfmt::make_format_args();

            bslfmt::format_args args(temp);

            ASSERT(args.get(0));
            ASSERT(!args.get(1));

            FA arg = args.get(0);

            ASSERT(checkValue(arg, val));
        }

        {
            WFA a;
            ASSERT(!a);
        }

        // Testing bool

        {
            bool val = true;

            FASB temp = bslfmt::make_format_args(val);

            bslfmt::format_args args(temp);

            ASSERT(args.get(0));
            ASSERT(!args.get(1));

            FA arg = args.get(0);

            ASSERT(checkValue(arg, val));
        }

        {
            bool value = true;
            WFA arg   = makeTestArg<wchar_t>(value);

            TestVisitor<wchar_t *, wchar_t> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_bool == value);

            TestVisitor<wchar_t *, wchar_t> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_bool == value);
        }

        // Testing int

        {
            int value = 21;
            FA  arg   = makeTestArg<char>(value);

            TestVisitor<char *, char> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_int == value);

            TestVisitor<char *, char> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_int == value);
        }

        {
            int value = 22;
            WFA arg   = makeTestArg<wchar_t>(value);

            TestVisitor<wchar_t *, wchar_t> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_int == value);

            TestVisitor<wchar_t *, wchar_t> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_int == value);
        }

        // Testing char

        {
            char value = 'm';
            FA   arg   = makeTestArg<char>(value);

            TestVisitor<char *, char> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_char == value);

            TestVisitor<char *, char> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_char == value);
        }

        {
            char value = 'm';
            WFA arg   = makeTestArg<wchar_t>(value);

            TestVisitor<wchar_t *, wchar_t> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_char == value);

            TestVisitor<wchar_t *, wchar_t> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_char == value);
        }

        // Testing wchar_t

        {
            wchar_t value = 'n';
            WFA  arg   = makeTestArg<wchar_t>(value);

            TestVisitor<wchar_t *, wchar_t> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_char == value);

            TestVisitor<wchar_t *, wchar_t> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_char == value);
        }

        // Testing unsigned

        {
            unsigned value = 33;
            FA   arg   = makeTestArg<char>(value);

            TestVisitor<char *, char> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_unsigned == value);

            TestVisitor<char *, char> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_unsigned == value);
        }

        {
            unsigned value = 34;
            WFA  arg   = makeTestArg<wchar_t>(value);

            TestVisitor<wchar_t *, wchar_t> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_unsigned == value);

            TestVisitor<wchar_t *, wchar_t> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_unsigned == value);
        }

        // Testing long long

        {
            long long value = 45;
            FA        arg   = makeTestArg<char>(value);

            TestVisitor<char *, char> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_longlong == value);

            TestVisitor<char *, char> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_longlong == value);
        }

        {
            long long value = 46;
            WFA       arg   = makeTestArg<wchar_t>(value);

            TestVisitor<wchar_t *, wchar_t> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_longlong == value);

            TestVisitor<wchar_t *, wchar_t> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_longlong == value);
        }

        // Testing unsigned long long

        {
            unsigned long long value = 57;
            FA                 arg   = makeTestArg<char>(value);

            TestVisitor<char *, char> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_unsignedlonglong == value);

            TestVisitor<char *, char> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_unsignedlonglong == value);
        }

        {
            unsigned long long value = 58;
            WFA                arg   = makeTestArg<wchar_t>(value);

            TestVisitor<wchar_t *, wchar_t> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_unsignedlonglong == value);

            TestVisitor<wchar_t *, wchar_t> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_unsignedlonglong == value);
        }

        // Testing float

        {
            float value = 3.1f;
            FA    arg   = makeTestArg<char>(value);

            TestVisitor<char *, char> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_float == value);

            TestVisitor<char *, char> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_float == value);
        }

        {
            float value = 4.2f;
            WFA   arg   = makeTestArg<wchar_t>(value);

            TestVisitor<wchar_t *, wchar_t> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_float == value);

            TestVisitor<wchar_t *, wchar_t> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_float == value);
        }

        // Testing double

        {
            double value = 5.1;
            FA     arg   = makeTestArg<char>(value);

            TestVisitor<char *, char> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_double == value);

            TestVisitor<char *, char> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_double == value);
        }

        {
            double value = 6.2;
            WFA    arg   = makeTestArg<wchar_t>(value);

            TestVisitor<wchar_t *, wchar_t> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_double == value);

            TestVisitor<wchar_t *, wchar_t> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_double == value);
        }

        // Testing long double

        {
            long double value = 7.1;
            FA          arg   = makeTestArg<char>(value);

            TestVisitor<char *, char> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_longdouble == value);

            TestVisitor<char *, char> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_longdouble == value);
        }

        {
            long double value = 8.2;
            WFA         arg   = makeTestArg<wchar_t>(value);

            TestVisitor<wchar_t *, wchar_t> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_longdouble == value);

            TestVisitor<wchar_t *, wchar_t> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_longdouble == value);
        }

        // Testing char*

        {
            const char *value = "testing testing";
            FA       arg   = makeTestArg<char>(value);

            TestVisitor<char *, char> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_charstar == value);

            TestVisitor<char *, char> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_charstar == value);
        }

        // Testing wchar_t*

        {
            const wchar_t *value = L"more testing testing";
            WFA      arg   = makeTestArg<wchar_t>(value);

            TestVisitor<wchar_t *, wchar_t> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_charstar == value);

            TestVisitor<wchar_t *, wchar_t> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_charstar == value);
        }

        // Testing void*

        {
            int         dummy = 0;
            const void *value = static_cast<const void *>(&dummy);
            FA          arg   = makeTestArg<char>(value);

            TestVisitor<char *, char> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_voidstar == value);

            TestVisitor<char *, char> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_voidstar == value);
        }

        {
            int         dummy = 0;
            const void *value = static_cast<const void *>(&dummy);
            WFA            arg   = makeTestArg<wchar_t>(value);

            TestVisitor<wchar_t *, wchar_t> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_voidstar == value);

            TestVisitor<wchar_t *, wchar_t> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_voidstar == value);
        }

        // Testing bsl::basic_string_view

        {
            bsl::basic_string<char>      dummy("Testing");
            bsl::basic_string_view<char> value(dummy);
            FA                           arg = makeTestArg<char>(value);

            TestVisitor<char *, char> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_stringview == value);

            TestVisitor<char *, char> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_stringview == value);
        }

        {
            bsl::basic_string<wchar_t>      dummy(L"Testing");
            bsl::basic_string_view<wchar_t> value(dummy);
            WFA                             arg = makeTestArg<wchar_t>(value);

            TestVisitor<wchar_t *, wchar_t> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_stringview == value);

            TestVisitor<wchar_t *, wchar_t> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_stringview == value);
        }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        // Testing std::basic_string_view

        {
            std::basic_string<char>  dummy("Testing");
            std::basic_string_view<char> value(dummy);
            FA          arg   = makeTestArg<char>(value);

            TestVisitor<char *, char> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_stringview == value);

            TestVisitor<char *, char> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_stringview == value);
        }

        {
            std::basic_string<wchar_t>   dummy(L"Testing");
            std::basic_string_view<wchar_t> value(dummy);
            WFA         arg   = makeTestArg<wchar_t>(value);

            TestVisitor<wchar_t *, wchar_t> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_stringview == value);

            TestVisitor<wchar_t *, wchar_t> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_stringview == value);
        }
#endif

        // Testing bsl::basic_string

        {
            bsl::basic_string<char> value("Testing");
            FA                      arg = makeTestArg<char>(value);

            TestVisitor<char *, char> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_stringview == value);

            TestVisitor<char *, char> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_stringview == value);
        }

        {
            bsl::basic_string<wchar_t> value(L"Testing");
            WFA                        arg = makeTestArg<wchar_t>(value);

            TestVisitor<wchar_t *, wchar_t> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_stringview == value);

            TestVisitor<wchar_t *, wchar_t> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_stringview == value);
        }

        // Testing std::basic_string

        {
            std::basic_string<char> value("Testing");
            FA                      arg = makeTestArg<char>(value);

            TestVisitor<char *, char> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_stringview == value);

            TestVisitor<char *, char> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_stringview == value);
        }

        {
            std::basic_string<wchar_t> value(L"Testing");
            WFA                        arg = makeTestArg<wchar_t>(value);

            TestVisitor<wchar_t *, wchar_t> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_stringview == value);

            TestVisitor<wchar_t *, wchar_t> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_stringview == value);
        }

        // Testing handle of FormattableType and handle::format()

        {
            FormattableType value(44);
            FA                      arg = makeTestArg<char>(value);

            FormattableType::parseCalls = 0;
            FormattableType::formatSum  = 0;

            TestVisitor<char *, char> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_ishandle);
            ASSERT(FormattableType::parseCalls == 1);
            ASSERT(FormattableType::formatSum == value.x);

            FormattableType::parseCalls = 0;
            FormattableType::formatSum  = 0;

            TestVisitor<char *, char> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_ishandle);
            ASSERT(FormattableType::parseCalls == 1);
            ASSERT(FormattableType::formatSum == value.x);
        }

        {
            FormattableType value(45);
            WFA             arg = makeTestArg<wchar_t>(value);

            FormattableType::parseCalls = 0;
            FormattableType::formatSum  = 0;

            TestVisitor<wchar_t *, wchar_t> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_ishandle);
            ASSERT(FormattableType::parseCalls == 1);
            ASSERT(FormattableType::formatSum == value.x);

            FormattableType::parseCalls = 0;
            FormattableType::formatSum  = 0;

            TestVisitor<wchar_t *, wchar_t> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_ishandle);
            ASSERT(FormattableType::parseCalls == 1);
            ASSERT(FormattableType::formatSum == value.x);
        }

#endif

      } break;
      case 10: {
        // -----------------------------------------------
        // TESTING STREAMING FUNCTIONALITY: Not Applicable
        //
        // Testing:
        //   STREAMING FUNCTIONALITY: Not Applicable
        // -----------------------------------------------
        if (verbose)
            printf("\nSTREAMING FUNCTIONALITY: Not Applicable"
                   "\n=======================================\n");
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR
        //
        // Concerns:
        //: 1 We can assign 'bslfmt::basic_format_args' types and they
        //:   retain the same values when assigned.
        //:
        //: 2 We can move assign 'bslfmt::basic_format_args' types and they
        //:   retain the same values when assigned.
        //
        // Plan:
        //: 1 Construct a valueless 'bslfmt::basic_format_args', assign it and
        //:   verify the assigned version is valueless. (C-1)
        //:
        //: 2 Construct a 'bslfmt::basic_format_args' holding an int, assign it
        //:   and verify the assigned version holds the same value. (C-1)
        //:
        //: 3 Construct a 'bslfmt::basic_format_args' holding a reference to a
        //:   type, assign it and verify the assigned version references the
        //:   same type. (C-1)
        //:
        //: 4 Repeat the above tests using rvalue construction. (C-2)
        //
        // Testing:
        //   operator=(const basic_format_args &);
        //   operator=(basic_format_args &&);
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING ASSIGNMENT OPERATOR"
                   "\n===========================\n");

        typedef bslfmt::basic_format_arg<bslfmt::format_context> FA;

        typedef bslfmt::basic_format_arg<bslfmt::wformat_context> WFA;

        typedef bslfmt::Format_FormatArgStore<bslfmt::format_context, int>
            FASI;
        typedef bslfmt::Format_FormatArgStore<bslfmt::wformat_context, int>
            WFASI;

        typedef bslfmt::Format_FormatArgStore<bslfmt::format_context,
                                              FormattableType>
            FAST;
        typedef bslfmt::Format_FormatArgStore<bslfmt::wformat_context,
                                              FormattableType>
            WFAST;

        typedef bslmf::MovableRefUtil MoveUtil;

        if (verbose)
            printf("\nValidating copy construction\n");

        {
            bslfmt::format_args a1;
            bslfmt::format_args a2;

            a2 = a1;

            ASSERT(!a1.get(0));
            ASSERT(!a2.get(0));
        }

        {
            bslfmt::wformat_args a1;
            bslfmt::wformat_args a2;

            a2 = a1;

            ASSERT(!a1.get(0));
            ASSERT(!a2.get(0));
        }

        {
            int  value = 99;
            FASI asi   = bslfmt::make_format_args(value);

            bslfmt::format_args a1(asi);
            bslfmt::format_args a2;

            a2 = a1;

            ASSERT(a1.get(0));
            ASSERT(a2.get(0));
            ASSERT(!a1.get(1));
            ASSERT(!a2.get(1));

            ASSERT(checkValue(a1.get(0), 99));
            ASSERT(checkValue(a2.get(0), 99));
        }

        {
            int   value = 99;
            WFASI asi   = bslfmt::make_wformat_args(value);

            bslfmt::wformat_args a1(asi);
            bslfmt::wformat_args a2;

            a2 = a1;

            ASSERT(a1.get(0));
            ASSERT(a2.get(0));
            ASSERT(!a1.get(1));
            ASSERT(!a2.get(1));

            ASSERT(checkValue(a1.get(0), 99));
            ASSERT(checkValue(a2.get(0), 99));
        }

        {
            FormattableType ft(42);
            FAST            asi = bslfmt::make_format_args(ft);

            bslfmt::format_args a1(asi);
            bslfmt::format_args a2;

            a2 = a1;

            ASSERT(a1.get(0));
            ASSERT(a2.get(0));
            ASSERT(!a1.get(1));
            ASSERT(!a2.get(1));

            ASSERT(checkFormattableTypeValue(a1.get(0), 42));
            ASSERT(checkFormattableTypeValue(a2.get(0), 42));
        }

        {
            FormattableType ft(42);
            WFAST           asi = bslfmt::make_wformat_args(ft);

            bslfmt::wformat_args a1(asi);
            bslfmt::wformat_args a2;

            a2 = a1;

            ASSERT(a1.get(0));
            ASSERT(a2.get(0));
            ASSERT(!a1.get(1));
            ASSERT(!a2.get(1));

            ASSERT(checkFormattableTypeValue(a1.get(0), 42));
            ASSERT(checkFormattableTypeValue(a2.get(0), 42));
        }

        {
            bslfmt::format_args a1;
            bslfmt::format_args a2;

            a2 = MoveUtil::move(a1);

            ASSERT(!a1.get(0));
            ASSERT(!a2.get(0));
        }

        {
            bslfmt::wformat_args a1;
            bslfmt::wformat_args a2;

            a2 = MoveUtil::move(a1);

            ASSERT(!a1.get(0));
            ASSERT(!a2.get(0));
        }

        {
            int  value = 99;
            FASI asi   = bslfmt::make_format_args(value);

            bslfmt::format_args a1(asi);
            bslfmt::format_args a2;

            a2 = MoveUtil::move(a1);

            ASSERT(a1.get(0));
            ASSERT(a2.get(0));
            ASSERT(!a1.get(1));
            ASSERT(!a2.get(1));

            ASSERT(checkValue(a1.get(0), 99));
            ASSERT(checkValue(a2.get(0), 99));
        }

        {
            int   value = 99;
            WFASI asi   = bslfmt::make_wformat_args(value);

            bslfmt::wformat_args a1(asi);
            bslfmt::wformat_args a2;

            a2 = MoveUtil::move(a1);

            ASSERT(a1.get(0));
            ASSERT(a2.get(0));
            ASSERT(!a1.get(1));
            ASSERT(!a2.get(1));

            ASSERT(checkValue(a1.get(0), 99));
            ASSERT(checkValue(a2.get(0), 99));
        }

        {
            FormattableType ft(42);
            FAST            asi = bslfmt::make_format_args(ft);

            bslfmt::format_args a1(asi);
            bslfmt::format_args a2;

            a2 = MoveUtil::move(a1);

            ASSERT(a1.get(0));
            ASSERT(a2.get(0));
            ASSERT(!a1.get(1));
            ASSERT(!a2.get(1));

            ASSERT(checkFormattableTypeValue(a1.get(0), 42));
            ASSERT(checkFormattableTypeValue(a2.get(0), 42));
        }

        {
            FormattableType ft(42);
            WFAST           asi = bslfmt::make_wformat_args(ft);

            bslfmt::wformat_args a1(asi);
            bslfmt::wformat_args a2;

            a2 = MoveUtil::move(a1);

            ASSERT(a1.get(0));
            ASSERT(a2.get(0));
            ASSERT(!a1.get(1));
            ASSERT(!a2.get(1));

            ASSERT(checkFormattableTypeValue(a1.get(0), 42));
            ASSERT(checkFormattableTypeValue(a2.get(0), 42));
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING SWAP
        //
        // Concerns:
        //: 1 We can swap construct 'bslfmt::basic_format_args' types and they
        //:   retain the same values when swapped.
        //
        // Plan:
        //: 1 Construct a 'bslfmt::basic_format_arg'.
        //:
        //: 2 Construct a 'bslfmt::basic_format_arg' holding an int.
        //:
        //: 3 Construct a 'bslfmt::basic_format_arg' holding a reference to a
        //:   type.
        //:
        //: 4 Use `bsl::swap` to rotate the three constructed objects and
        //:   verify that they contain the expected values.
        //
        // Testing:
        //   swap(formatter &, formatter &)
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING SWAP"
                   "\n============\n");

        typedef bslfmt::basic_format_arg<bslfmt::format_context> FA;

        typedef bslfmt::basic_format_arg<bslfmt::wformat_context> WFA;

        typedef bslfmt::Format_FormatArgStore<bslfmt::format_context, int>
            FASI;
        typedef bslfmt::Format_FormatArgStore<bslfmt::wformat_context, int>
            WFASI;

        typedef bslfmt::Format_FormatArgStore<bslfmt::format_context,
                                              FormattableType>
            FAST;
        typedef bslfmt::Format_FormatArgStore<bslfmt::wformat_context,
                                              FormattableType>
            WFAST;

        if (verbose)
            printf("\nValidating swap\n");

        {
            int  value = 99;
            FASI asi   = bslfmt::make_format_args(value);

            FormattableType ft(42);
            FAST            ast = bslfmt::make_format_args(ft);

            bslfmt::format_args a1;
            bslfmt::format_args a2(asi);
            bslfmt::format_args a3(ast);

            bsl::swap(a1, a2);
            bsl::swap(a2, a3);

            ASSERT(checkValue(a1.get(0), 99));
            ASSERT(checkFormattableTypeValue(a2.get(0), 42));
            ASSERT(!a3.get(0));
        }

        {
            int   value = 99;
            WFASI asi   = bslfmt::make_wformat_args(value);

            FormattableType ft(42);
            WFAST           ast = bslfmt::make_wformat_args(ft);

            bslfmt::wformat_args a1;
            bslfmt::wformat_args a2(asi);
            bslfmt::wformat_args a3(ast);

            bsl::swap(a1, a2);
            bsl::swap(a2, a3);

            ASSERT(checkValue(a1.get(0), 99));
            ASSERT(checkFormattableTypeValue(a2.get(0), 42));
            ASSERT(!a3.get(0));
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY/MOVE CONSTRUCTOR
        //
        // Concerns:
        //: 1 We can copy construct 'bslfmt::basic_format_args' types and they
        //:   retain the same values when copied.
        //:
        //: 2 We can move construct 'bslfmt::basic_format_args' types and they
        //:   retain the same values when copied.
        //
        // Plan:
        //: 1 Construct a valueless 'bslfmt::basic_format_args', copy it and
        //:   verify the copied version is valueless. (C-1)
        //:
        //: 2 Construct a 'bslfmt::basic_format_args' holding an int, copy it
        //:   and verify the copied version holds the same value. (C-1)
        //:
        //: 3 Construct a 'bslfmt::basic_format_args' holding a reference to a
        //:   type, copy it and verify the copied version references the same
        //:   type. (C-1)
        //:
        //: 4 Repeat the above tests using rvalue construction. (C-2)
        //
        // Testing:
        //   formatter(const formatter &);
        //   formatter(formatter &&);
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING COPY CONSTRUCTOR"
                   "\n========================\n");

        typedef bslfmt::basic_format_arg<bslfmt::format_context> FA;

        typedef bslfmt::basic_format_arg<bslfmt::wformat_context> WFA;

        typedef bslfmt::Format_FormatArgStore<bslfmt::format_context, int>
            FASI;
        typedef bslfmt::Format_FormatArgStore<bslfmt::wformat_context, int>
            WFASI;

        typedef bslfmt::Format_FormatArgStore<bslfmt::format_context,
                                              FormattableType>
            FAST;
        typedef bslfmt::Format_FormatArgStore<bslfmt::wformat_context,
                                              FormattableType>
            WFAST;

        typedef bslmf::MovableRefUtil  MoveUtil;

        if (verbose)
            printf("\nValidating copy construction\n");

        {
            bslfmt::format_args a1;
            bslfmt::format_args a2(a1);

            ASSERT(!a1.get(0));
            ASSERT(!a2.get(0));
        }

        {
            bslfmt::wformat_args a1;
            bslfmt::wformat_args a2(a1);

            ASSERT(!a1.get(0));
            ASSERT(!a2.get(0));
        }

        {
            int  value = 99;
            FASI asi   = bslfmt::make_format_args(value);

            bslfmt::format_args a1(asi);
            bslfmt::format_args a2(a1);

            ASSERT(a1.get(0));
            ASSERT(a2.get(0));
            ASSERT(!a1.get(1));
            ASSERT(!a2.get(1));

            ASSERT(checkValue(a1.get(0), 99));
            ASSERT(checkValue(a2.get(0), 99));
        }

        {
            int  value = 99;
            WFASI asi   = bslfmt::make_wformat_args(value);

            bslfmt::wformat_args a1(asi);
            bslfmt::wformat_args a2(a1);

            ASSERT(a1.get(0));
            ASSERT(a2.get(0));
            ASSERT(!a1.get(1));
            ASSERT(!a2.get(1));

            ASSERT(checkValue(a1.get(0), 99));
            ASSERT(checkValue(a2.get(0), 99));
        }

        {
            FormattableType ft(42);
            FAST                asi = bslfmt::make_format_args(ft);

            bslfmt::format_args a1(asi);
            bslfmt::format_args a2(a1);

            ASSERT(a1.get(0));
            ASSERT(a2.get(0));
            ASSERT(!a1.get(1));
            ASSERT(!a2.get(1));

            ASSERT(checkFormattableTypeValue(a1.get(0), 42));
            ASSERT(checkFormattableTypeValue(a2.get(0), 42));
        }

        {
            FormattableType      ft(42);
            WFAST                 asi = bslfmt::make_wformat_args(ft);

            bslfmt::wformat_args a1(asi);
            bslfmt::wformat_args a2(a1);

            ASSERT(a1.get(0));
            ASSERT(a2.get(0));
            ASSERT(!a1.get(1));
            ASSERT(!a2.get(1));

            ASSERT(checkFormattableTypeValue(a1.get(0), 42));
            ASSERT(checkFormattableTypeValue(a2.get(0), 42));
        }

        {
            bslfmt::format_args a1;
            bslfmt::format_args a2(MoveUtil::move(a1));

            ASSERT(!a1.get(0));
            ASSERT(!a2.get(0));
        }

        {
            bslfmt::wformat_args a1;
            bslfmt::wformat_args a2(MoveUtil::move(a1));

            ASSERT(!a1.get(0));
            ASSERT(!a2.get(0));
        }

        {
            int  value = 99;
            FASI asi   = bslfmt::make_format_args(value);

            bslfmt::format_args a1(asi);
            bslfmt::format_args a2(MoveUtil::move(a1));

            ASSERT(a1.get(0));
            ASSERT(a2.get(0));
            ASSERT(!a1.get(1));
            ASSERT(!a2.get(1));

            ASSERT(checkValue(a1.get(0), 99));
            ASSERT(checkValue(a2.get(0), 99));
        }

        {
            int   value = 99;
            WFASI asi   = bslfmt::make_wformat_args(value);

            bslfmt::wformat_args a1(asi);
            bslfmt::wformat_args a2(MoveUtil::move(a1));

            ASSERT(a1.get(0));
            ASSERT(a2.get(0));
            ASSERT(!a1.get(1));
            ASSERT(!a2.get(1));

            ASSERT(checkValue(a1.get(0), 99));
            ASSERT(checkValue(a2.get(0), 99));
        }

        {
            FormattableType ft(42);
            FAST            asi = bslfmt::make_format_args(ft);

            bslfmt::format_args a1(asi);
            bslfmt::format_args a2(MoveUtil::move(a1));

            ASSERT(a1.get(0));
            ASSERT(a2.get(0));
            ASSERT(!a1.get(1));
            ASSERT(!a2.get(1));

            ASSERT(checkFormattableTypeValue(a1.get(0), 42));
            ASSERT(checkFormattableTypeValue(a2.get(0), 42));
        }

        {
            FormattableType ft(42);
            WFAST           asi = bslfmt::make_wformat_args(ft);

            bslfmt::wformat_args a1(asi);
            bslfmt::wformat_args a2(MoveUtil::move(a1));

            ASSERT(a1.get(0));
            ASSERT(a2.get(0));
            ASSERT(!a1.get(1));
            ASSERT(!a2.get(1));

            ASSERT(checkFormattableTypeValue(a1.get(0), 42));
            ASSERT(checkFormattableTypeValue(a2.get(0), 42));
        }

      } break;
      case 6: {
        // --------------------------------------------
        // TESTING EQUALITY OPERATOR: Not Applicable
        //
        // Testing:
        //   EQUALITY OPERATOR: Not Applicable
        // --------------------------------------------
        if (verbose)
            printf("\nEQUALITY OPERATOR: Not Applicable"
                   "\n= ===============================\n");

      } break;
      case 5: {
        // --------------------------------------------
        // TESTING OUTPUT: Not Applicable
        //
        // Testing:
        //   OUTPUT: Not Applicable
        // --------------------------------------------
        if (verbose)
            printf("\nOUTPUT: Not Applicable"
                   "\n======================\n");
      } break;
      case 4: {
        // --------------------------------------------
        // TESTING BASIC ACCESSORS
        // 
        // Concerns:
        //: 1 `get()` outside the range of contained values
        //:    will return a default-constructed `basic_format_arg`.
        //:
        //: 1 `get()` inside the range of contained values
        //:    will return the contained `basic_format_arg`.
        //
        // Plan:
        //: 1 Construct a default 'bslfmt::basic_format_arg' and verify the
        //:   result of calling `get()`. (C-1)
        //:
        //: 2 Construct a non-default 'bslfmt::basic_format_arg' and verify
        //:   the result of calling `operator BoolType()`. (C-2)
        //
        // Testing:
        //   operator get();
        // --------------------------------------------
        if (verbose)
            printf("\nTESTING BASIC ACCESSORS"
                   "\n=======================\n");

        typedef bslfmt::basic_format_arg<bslfmt::format_context> FA;

        typedef bslfmt::basic_format_arg<bslfmt::wformat_context> WFA;

        typedef bslfmt::Format_FormatArgStore<bslfmt::format_context, int>
            FASI;
        typedef bslfmt::Format_FormatArgStore<bslfmt::wformat_context, int>
            WFASI;

        {  // Testing with no contained values
            try {
                bslfmt::basic_format_args<
                    bslfmt::basic_format_context<char *, char> >
                    dummy_c_1;

                ASSERT(!dummy_c_1.get(0));

                bslfmt::basic_format_args<bslfmt::format_context> dummy_c_2;

                ASSERT(!dummy_c_2.get(0));

                bslfmt::basic_format_args<
                    bslfmt::basic_format_context<wchar_t *, wchar_t> >
                    dummy_w_1;

                ASSERT(!dummy_w_1.get(0));

                bslfmt::basic_format_args<bslfmt::wformat_context> dummy_w_2;

                ASSERT(!dummy_w_2.get(0));
            }
            catch (...) {
                ASSERT(false);
            }
        }
        {  // Test with contained values
            try {
                int val = 5;

                FASI temp = bslfmt::make_format_args(val);

                bslfmt::format_args dummy_c(temp);

                ASSERT( dummy_c.get(0));
                ASSERT(!dummy_c.get(1));

                FA arg_c = dummy_c.get(0);

                ASSERT(checkValue(arg_c, val));
            }
            catch (...) {
                ASSERT(false);
            }

            try {
                int val = 5;

                WFASI temp = bslfmt::make_wformat_args(val);

                bslfmt::wformat_args dummy_w(temp);

                ASSERT( dummy_w.get(0));
                ASSERT(!dummy_w.get(1));

                WFA arg_w = dummy_w.get(0);

                ASSERT(checkValue(arg_w, val));
            }
            catch (...) {
                ASSERT(false);
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING (PRIMITIVE) GENERATORS
        //
        // Concerns:
        //: 1 We can default construct a `bslfmt::basic_format_arg` type.
        //:
        //: 2 We can construct a `bslfmt::basic_format_arg` type containing a
        //:   value.
        //
        // Plan:
        //: 1 Construct a default `bslfmt::basic_format_arg` type. (C-1)
        //:
        //: 2 Construct a non-default `bslfmt::basic_format_arg` type. This
        //:   will need to be performed indirectly as the constructors are
        //:   private (C-2)
        //
        // Testing:
        //   basic_format_args();
        //   ~basic_format_args()
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING (PRIMITIVE) GENERATORS"
                            "\n==============================\n");

        if (verbose)
            printf("\nValidating bslfmt construction\n");

        {
            // Test default construction
            try {
                bslfmt::format_args
                    dummy_c;

                bslfmt::wformat_args
                    dummy_w;
            }
            catch (...) {
                ASSERT(false);
            }
            ASSERT(true);
        }


      } break;
      case 2: {
        // --------------------------------------------
        // TESTING PRIMARY MANIPULATORS: Not Applicable
        //
        // Testing:
        //   PRIMARY MANIPULATORS: Not Applicable
        // --------------------------------------------
        if (verbose)
            printf("\nPRIMARY MANIPULATORS: Not Applicable"
                   "\n====================================\n");
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 That basic functionality appears to work as advertised before
        //:   before beginning testing in earnest:
        //:   - default and copy constructors
        //:   - assignment operator
        //:   - primary manipulators, basic accessors
        //:   - 'operator==', 'operator!='
        //
        // Plan:
        //: 1 Test all public methods mentioned in concerns.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        if (verbose)
            printf("\nBREATHING TEST"
                   "\n==============\n");

        typedef bslfmt::Format_FormatArgStore<bslfmt::format_context, int>
            FASI;
        typedef bslfmt::Format_FormatArgStore<bslfmt::wformat_context, int>
            WFASI;

        // This type has very limited public-facing functionality, so there is
        // very little we can do in a breathing test.
        {
            // Test default construction
            try {
                bslfmt::basic_format_args<
                    bslfmt::basic_format_context<char *, char> >
                    dummy_c_1;

                ASSERT(!dummy_c_1.get(0));

                bslfmt::basic_format_args<bslfmt::format_context> dummy_c_2;

                ASSERT(!dummy_c_2.get(0));

                {
                    int val = 5;

                    FASI temp = bslfmt::make_format_args(val);

                    bslfmt::format_args dummy_c_3(temp);

                    ASSERT(dummy_c_3.get(0));
                    ASSERT(!dummy_c_3.get(1));

                    checkValue(dummy_c_3.get(0), val);
                }


                bslfmt::basic_format_args<
                    bslfmt::basic_format_context<wchar_t *, wchar_t> >
                    dummy_w_1;

                ASSERT(!dummy_w_1.get(0));

                bslfmt::basic_format_args<bslfmt::wformat_context> dummy_w_2;

                ASSERT(!dummy_w_2.get(0));

                {
                    int val = 5;

                    WFASI temp = bslfmt::make_wformat_args(val);

                    bslfmt::wformat_args dummy_w_3(temp);

                    ASSERT( dummy_w_3.get(0));
                    ASSERT(!dummy_w_3.get(1));

                    checkValue(dummy_w_3.get(0), val);
                }
            }
            catch (...) {
                ASSERT(false);
            }
            ASSERT(true);
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
// Copyright 2023 Bloomberg Finance L.P.
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
