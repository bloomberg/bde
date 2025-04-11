// bslstl_format_arg.t.cpp                                            -*-C++-*-
#include <bslfmt_format_arg.h>

#include <bsls_bsltestutil.h>

#include <bslstl_string.h>

#include <stdio.h>
#include <string.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// 'basic_format_arg<basic_format_context<t_OUT, t_CHAR> >' where t_OUT is
// an output iterator is a standard-compliant implementation of
// 'std::basic_format_arg'. It is hard to test standalone as it is designed
// to be constructed only indirectly by the 'bslfmt::format' suite of
// functions, and such testing requires the creation of "mock" contexts.
//
// It should meet the requirements specified in [format.string.std].
//
//-----------------------------------------------------------------------------
// CLASS 'bsl::basic_format_arg'
//
// CREATORS
// [ 3] basic_format_arg();
// [ 3] ~basic_format_arg();
// [ 7] basic_format_arg(const basic_format_arg &);
//
// MANIPULATORS
// [ 6] operator==(const basic_format_arg &);
// [ 9] operator=(const basic_format_arg &);
// [10] visit(VISITOR&& visitor)
//
// ACCESSORS
// [ 4] operator BoolType();
// [11] visit(ARG)
// [11] handle::format(TYPE, FORMAT_CONTEXT&);
//
// FREE FUNCTIONS
// [ 8] swap(basic_format_arg &, basic_format_arg&);
// [10] visit_format_arg(VISITOR&& visitor)
//
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] TESTING PRIMARY MANIPULATORS: Not Applicable
// [ 5] TESTING OUTPUT
// [10] STREAMING FUNCTIONALITY:      Not Applicable
// [12] USAGE EXAMPLE
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
//                     GLOBAL HELPER CLASSES FOR TESTING
// ----------------------------------------------------------------------------

namespace BloombergLP {
namespace bslfmt {

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
                  size_t) BSLS_KEYWORD_NOEXCEPT
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

}  // close package namespace
}  // close enterprise namespace

namespace {

template <class t_CHAR>
struct TestVisitor {
  private:
    // PRIVATE TYPES
    typedef typename bslfmt::basic_format_arg<
        bslfmt::basic_format_context<t_CHAR *, t_CHAR> >::handle handle;

    // FRIENDS
    friend bool getValueFn(const TestVisitor& visitor, bool * = 0)
    {
        return visitor.d_bool;
    }
    friend t_CHAR getValueFn(const TestVisitor& visitor, t_CHAR * = 0)
    {
        return visitor.d_char;
    }
    friend int getValueFn(const TestVisitor& visitor, int * = 0)
    {
        return visitor.d_int;
    }
    friend unsigned getValueFn(const TestVisitor& visitor, unsigned * = 0)
    {
        return visitor.d_unsigned;
    }
    friend long long getValueFn(const TestVisitor& visitor, long long * = 0)
    {
        return visitor.d_longLong;
    }
    friend unsigned long long getValueFn(const TestVisitor&  visitor,
                                         unsigned long long * = 0)
    {
        return visitor.d_unsignedLongLong;
    }
    friend float getValueFn(const TestVisitor& visitor, float * = 0)
    {
        return visitor.d_float;
    }
    friend double getValueFn(const TestVisitor& visitor, double * = 0)
    {
        return visitor.d_double;
    }
    friend long double getValueFn(const TestVisitor&  visitor,
                                  long double        * = 0)
    {
        return visitor.d_longDouble;
    }
    friend const t_CHAR *getValueFn(const TestVisitor&   visitor,
                                    const t_CHAR       ** = 0)
    {
        return visitor.d_constCharPtr_p;
    }
    friend const void *getValueFn(const TestVisitor&   visitor,
                                  const void         ** = 0)
    {
        return visitor.d_constVoidPtr_p;
    }
    friend const bsl::basic_string_view<t_CHAR> getValueFn(
                                 const TestVisitor&                    visitor,
                                 const bsl::basic_string_view<t_CHAR> * = 0)
    {
        return visitor.d_stringView;
    }

  public:
    // PUBLIC DATA
    bool                            d_bool;
    t_CHAR                          d_char;
    int                             d_int;
    unsigned                        d_unsigned;
    long long                       d_longLong;
    unsigned long long              d_unsignedLongLong;
    float                           d_float;
    double                          d_double;
    long double                     d_longDouble;
    const t_CHAR                   *d_constCharPtr_p;
    const void                     *d_constVoidPtr_p;
    bsl::basic_string_view<t_CHAR>  d_stringView;
    bool                            d_ishandle;

    // CREATORS
    TestVisitor()
    : d_bool(false)
    , d_char(0)
    , d_int(0)
    , d_unsigned(0)
    , d_longLong(0)
    , d_unsignedLongLong(0)
    , d_float(0)
    , d_double(0)
    , d_longDouble(0)
    , d_constCharPtr_p(0)
    , d_constVoidPtr_p(0)
    , d_ishandle(false) {};

    // MANIPULATORS
    void operator()(bsl::monostate) const {}

    void operator()(bool x) { d_bool = x; }
    void operator()(t_CHAR x) { d_char = x; }
    void operator()(unsigned x) { d_unsigned = x; }
    void operator()(long long x) { d_longLong = x; }
    void operator()(unsigned long long x) { d_unsignedLongLong = x; }
    void operator()(float x) { d_float = x; }
    void operator()(double x) { d_double = x; }
    void operator()(long double x) { d_longDouble = x; }
    void operator()(const t_CHAR *x) { d_constCharPtr_p = x; }
    void operator()(const void *x) { d_constVoidPtr_p = x; }
    void operator()(int x) { d_int = x; }
    void operator()(bsl::basic_string_view<t_CHAR> sv)
    {
        d_stringView = sv;
    }
    void operator()(const handle& h)
    {
        bslfmt::basic_format_context<t_CHAR *, t_CHAR> fc((t_CHAR *)0);
        bsl::basic_string_view<t_CHAR>                 fmt;
        bslfmt::basic_format_parse_context<t_CHAR>     pc(fmt);
        h.format(pc, fc);
        d_ishandle = true;
    }

    // ACCESSORS
    template <class t_TYPE>
    t_TYPE getValue() const
    {
        return getValueFn(*this, static_cast<t_TYPE *>(0));
    }
};


/// A minimal user defined type for testing.
class FormattableType {
  private:
    // CLASS DATA
    static int s_parseCalls;
    static int s_formatSum;

    // DATA
    int d_value;

  public:
    // CLASS METHODS
    static int& parseCalls() { return s_parseCalls; }

    static int& formatSum() { return s_formatSum; }

    static void resetCounters()
    {
        s_parseCalls = 0;
        s_formatSum  = 0;
    }

    // CREATORS
    FormattableType(int value)
    : d_value(value)
    {
    }

    // MANIPULATORS
    void setValue(int value) { d_value = value; }

    // ACCESSORS
    int value() const { return d_value; }
};

int FormattableType::s_parseCalls = 0;
int FormattableType::s_formatSum = 0;
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
        FormattableType::parseCalls()++;
        return pc.begin();
    }

    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(const FormattableType& value,
                                               t_FORMAT_CONTEXT&      fc) const
    {
        FormattableType::formatSum() += value.value();
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

    bslfmt::Format_ArgUtil::makeFormatArgArray(&arr, val);

    return arr[0];
}

template <class t_CHAR>
bool checkValue(bslfmt::basic_format_arg<
                    bslfmt::basic_format_context<t_CHAR *, t_CHAR> >& arg,
                bool                                                  value)
{
    TestVisitor<t_CHAR> visitor;

    arg.visit(visitor);

    TestVisitor<t_CHAR> visitor2;

    visit_format_arg(visitor2, arg);

    return visitor.d_bool == value && visitor2.d_bool == value;
}

template <class t_CHAR>
bool checkValue(bslfmt::basic_format_arg<
                    bslfmt::basic_format_context<t_CHAR *, t_CHAR> >& arg,
                t_CHAR                                                value)
{
    TestVisitor<t_CHAR> visitor;

    arg.visit(visitor);

    TestVisitor<t_CHAR> visitor2;

    visit_format_arg(visitor2, arg);

    return visitor.d_char == value && visitor2.d_char == value;
}

template <class t_CHAR>
bool checkValue(bslfmt::basic_format_arg<
                    bslfmt::basic_format_context<t_CHAR *, t_CHAR> >& arg,
                int                                                   value)
{
    TestVisitor<t_CHAR> visitor;

    arg.visit(visitor);

    TestVisitor<t_CHAR> visitor2;

    visit_format_arg(visitor2, arg);

    return visitor.d_int == value && visitor2.d_int == value;
}

template <class t_CHAR>
bool checkValue(bslfmt::basic_format_arg<
                    bslfmt::basic_format_context<t_CHAR *, t_CHAR> >& arg,
                long double                                           value)
{
    TestVisitor<t_CHAR> visitor;

    arg.visit(visitor);

    TestVisitor<t_CHAR> visitor2;

    visit_format_arg(visitor2, arg);

    return visitor.d_longLong == value && visitor2.d_longLong == value;
}

template <class t_CHAR>
bool checkFormattableTypeValue(
     bslfmt::basic_format_arg<bslfmt::basic_format_context<t_CHAR *, t_CHAR> >&
         arg,
     int handleValue)
{
    FormattableType::resetCounters();

    TestVisitor<t_CHAR> visitor;

    arg.visit(visitor);

    TestVisitor<t_CHAR> visitor2;

    visit_format_arg(visitor2, arg);

    return (FormattableType::parseCalls() == 2) &&
           (FormattableType::formatSum() == 2 * handleValue);
}

/// Test the `basic_format_arg`class for the ability to be visited by assigning
/// its object the specified `value` and obtaining it back using test visitors.
/// The specified `line` is used to identify the function call location.
template <class t_CHAR, class t_TYPE>
void visitTest(int line, t_TYPE value)
{
    typedef bslfmt::basic_format_arg<
        bslfmt::basic_format_context<t_CHAR *, t_CHAR> >
           FA;

    // To make sure that scalar and pointer types are stored by value we are
    // going to create a copy of the original value, modify the original
    // variable, passed to the `makeTestArg` and then compare the obtained
    // result with the copy.

    t_TYPE model = value;
    ASSERTV(line, value == model);

    FA cArg = makeTestArg<t_CHAR>(value);

    // Modify the original variable
    value  = t_TYPE();

    // Obtain value of the `basic_format_arg`
    TestVisitor<t_CHAR> visitor;
    cArg.visit(visitor);
    t_TYPE result = visitor.template getValue<t_TYPE>();

    // Compare obtained result with the copy
    ASSERTV(line, model != value);
    ASSERTV(line, model == result);

    // Repeat the test using `visit_format_arg` function
    TestVisitor<t_CHAR> visitor2;
    visit_format_arg(visitor2, cArg);
    result = visitor2.template getValue<t_TYPE>();
    ASSERTV(line, model == result);
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    const int  test    = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose = argc > 2;

    printf("TEST %s CASE %d \n", __FILE__, test);

    switch (test) {  case 0:
      case 12: {
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
      case 11: {
        // --------------------------------------------------------------------
        // TESTING VISIT AND HANDLE FORMAT FUNCTIONALITY
        //     As it is not possible to access a contained handle in order to
        //     test handle::format other than using the visitor pattern, the
        //     visitor test must be combined with the handle::format test
        //
        // Concerns:
        //: 1 For all constructed types of 'bslfmt::basic_format_arg' we can
        //:   use the visitor member and free functions to extract the
        //:   contained value.
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
        //   visit()
        //   visit_format_arg()
        //   handle::format()
        // --------------------------------------------------------------------
        if (verbose)
            printf("\nVISIT AND HANDLE FORMAT FUNCTIONALITY"
                   "\n=====================================\n");

        typedef bslfmt::basic_format_arg<
            bslfmt::basic_format_context<char *, char> >
            FA;

        typedef bslfmt::basic_format_arg<
            bslfmt::basic_format_context<wchar_t *, wchar_t> >
            WFA;

        if (verbose)
            printf("\nValidating visit functionality\n");
        {
            FA a;
            ASSERT(!a);
        }

        {
            WFA a;
            ASSERT(!a);
        }

        // Testing bool

        bool boolValue = true;
        visitTest<char   >(L_, boolValue);
        visitTest<wchar_t>(L_, boolValue);

        // Testing int

        int intValue1 = 21;
        visitTest<char   >(L_, intValue1);
        int intValue2 = 22;
        visitTest<wchar_t>(L_, intValue2);


        // Testing char

        char charValue = 'm';
        visitTest<char>(L_, charValue);

        // Testing wchar_t

        wchar_t wcharValue = 'n';
        visitTest<wchar_t>(L_, wcharValue);

        // Testing unsigned

        unsigned unsignedValue1 = 33;
        visitTest<char   >(L_, unsignedValue1);
        unsigned unsignedValue2 = 34;
        visitTest<wchar_t>(L_, unsignedValue2);

        // Testing long long

        long long longLongValue1 = 45;
        visitTest<char   >(L_, longLongValue1);
        long long longLongValue2 = 46;
        visitTest<wchar_t>(L_, longLongValue2);


        // Testing unsigned long long

        unsigned long long unsignedLongLongValue1 = 57;
        visitTest<char   >(L_, unsignedLongLongValue1);
        unsigned long long unsignedLongLongValue2 = 58;
        visitTest<wchar_t>(L_, unsignedLongLongValue2);

        // Testing float

        float floatValue1 = 6.9f;
        visitTest<char   >(L_, floatValue1);
        float floatValue2 = 6.0f;
        visitTest<wchar_t>(L_, floatValue2);

        // Testing double

        double doubleValue1 = 7.1;
        visitTest<char   >(L_, doubleValue1);
        double doubleValue2 = 7.2;
        visitTest<wchar_t>(L_, doubleValue2);

        // TODO: The following should be re-added if we update our
        // implementation to support long double.
        //
        // Testing long double

#if u_BSLFMT_LONG_DOUBLE_IS_SUPPORTED

        long double longDoubleValue1 = 8.3;
        visitTest<char   >(L_, longDoubleValue1);
        long double longDoubleValue2 = 8.4;
        visitTest<wchar_t>(L_, longDoubleValue2);
#endif

        // Testing char*

        const char *constCharPtrValue = "95";
        visitTest<char>(L_, constCharPtrValue);


        // Testing wchar_t*

        const wchar_t *constWcharPtrValue = L"96";
        visitTest<wchar_t>(L_, constWcharPtrValue);

        // Testing void*

        int         intDummy1          = 107;
        const void *constVoidPtrValue1 = static_cast<const void *>(&intDummy1);
        visitTest<char>(L_, constVoidPtrValue1);
        int         intDummy2          = 108;
        const void *constVoidPtrValue2 = static_cast<const void *>(&intDummy2);
        visitTest<wchar_t>(L_, constVoidPtrValue2);

        // Testing bsl::basic_string_view

        bsl::basic_string<char>      strDummy("119");
        bsl::basic_string_view<char> stringViewValue(strDummy);
        visitTest<char>(L_, stringViewValue);
        bsl::basic_string<wchar_t>      wStrDummy(L"110");
        bsl::basic_string_view<wchar_t> wStringViewValue(wStrDummy);
        visitTest<wchar_t>(L_, wStringViewValue);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

        // Testing std::basic_string_view
        {
            std::basic_string<char>  dummy("Testing");
            std::basic_string_view<char> value(dummy);
            FA          arg   = makeTestArg<char>(value);

            TestVisitor<char> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_stringView == value);

            TestVisitor<char> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_stringView == value);
        }

        {
            std::basic_string<wchar_t>   dummy(L"Testing");
            std::basic_string_view<wchar_t> value(dummy);
            WFA         arg   = makeTestArg<wchar_t>(value);

            TestVisitor<wchar_t> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_stringView == value);

            TestVisitor<wchar_t> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_stringView == value);
        }
#endif

        // Testing bsl::basic_string

        {
            bsl::basic_string<char> value("Testing");
            FA                      arg = makeTestArg<char>(value);

            TestVisitor<char> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_stringView == value);

            TestVisitor<char> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_stringView == value);
        }

        {
            bsl::basic_string<wchar_t> value(L"Testing");
            WFA                        arg = makeTestArg<wchar_t>(value);

            TestVisitor<wchar_t> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_stringView == value);

            TestVisitor<wchar_t> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_stringView == value);
        }

        // Testing std::basic_string

        {
            std::basic_string<char> value("Testing");
            FA                      arg = makeTestArg<char>(value);

            TestVisitor<char> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_stringView == value);

            TestVisitor<char> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_stringView == value);
        }

        {
            std::basic_string<wchar_t> value(L"Testing");
            WFA                        arg = makeTestArg<wchar_t>(value);

            TestVisitor<wchar_t> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_stringView == value);

            TestVisitor<wchar_t> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_stringView == value);
        }

        // Testing handle of FormattableType and handle::format()

        {
            FormattableType value(44);
            FA              arg = makeTestArg<char>(value);

            // Check that user-defined types are stored by reference

            value.setValue(45);
            ASSERTV(value.value(), 45 == value.value());

            FormattableType::resetCounters();

            TestVisitor<char> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_ishandle);
            ASSERT(FormattableType::parseCalls() == 1);
            ASSERT(FormattableType::formatSum() == value.value());

            FormattableType::resetCounters();

            TestVisitor<char> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_ishandle);
            ASSERT(FormattableType::parseCalls() == 1);
            ASSERT(FormattableType::formatSum() == value.value());
        }

        {
            FormattableType value(46);

            WFA arg = makeTestArg<wchar_t>(value);

            // Check that user-defined types are stored by reference

            value.setValue(47);
            ASSERTV(value.value(), 47 == value.value());

            FormattableType::resetCounters();

            TestVisitor<wchar_t> visitor;

            arg.visit(visitor);

            ASSERT(visitor.d_ishandle);
            ASSERT(FormattableType::parseCalls() == 1);
            ASSERT(FormattableType::formatSum() == value.value());

            FormattableType::resetCounters();

            TestVisitor<wchar_t> visitor2;

            visit_format_arg(visitor2, arg);

            ASSERT(visitor2.d_ishandle);
            ASSERT(FormattableType::parseCalls() == 1);
            ASSERT(FormattableType::formatSum() == value.value());
        }

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
        //: 1 We can copy construct 'bslfmt::basic_format_arg' types and they
        //:   retain the same values when copied.
        //
        // Plan:
        //: 1 Construct a valueless 'bslfmt::basic_format_arg', assign it and
        //:   verify the assigned version is valueless. (C-1)
        //:
        //: 2 Construct a 'bslfmt::basic_format_arg' holding an int, assign it
        //:   and verify the assigned version holds the same value. (C-1)
        //:
        //: 3 Construct a 'bslfmt::basic_format_arg' holding a reference to a
        //:   type, assign it and verify the assigned version references the
        //:   same type. (C-1)
        //
        // Testing:
        //   operator=(const formatter &);
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING ASSIGNMENT OPERATOR"
                   "\n===========================\n");

        typedef bslfmt::basic_format_arg<
            bslfmt::basic_format_context<char *, char> >
            FA;

        typedef bslfmt::basic_format_arg<
            bslfmt::basic_format_context<wchar_t *, wchar_t> >
            WFA;

        typedef bslmf::MovableRefUtil  MoveUtil;

        if (verbose)
            printf("\nValidating operator=\n");
        {
            FA a1;
            FA a2 = a1;

            ASSERT(!a1);
            ASSERT(!a2);
        }

        {
            WFA a1;
            WFA a2 = a1;

            ASSERT(!a1);
            ASSERT(!a2);
        }

        {
            FA a1 = makeTestArg<char>((int)99);
            FA a2 = a1;

            ASSERT(a1);
            ASSERT(a2);

            ASSERT(checkValue(a1, 99));
            ASSERT(checkValue(a2, 99));
        }

        {
            WFA a1 = makeTestArg<wchar_t>((int)99);
            WFA a2 = a1;

            ASSERT(a1);
            ASSERT(a2);

            ASSERT(checkValue(a1, 99));
            ASSERT(checkValue(a2, 99));
        }

        {
            FormattableType ft(42);
            FA a1 = makeTestArg<char>(ft);
            FA a2 = a1;

            ASSERT(a1);
            ASSERT(a2);

            ASSERT(checkFormattableTypeValue(a1, 42));
            ASSERT(checkFormattableTypeValue(a2, 42));
        }

        {
            FormattableType ft(42);
            WFA a1 = makeTestArg<wchar_t>(ft);
            WFA a2 = a1;

            ASSERT(a1);
            ASSERT(a2);

            ASSERT(checkFormattableTypeValue(a1, 42));
            ASSERT(checkFormattableTypeValue(a2, 42));
        }

        {
            FA a1;
            FA a2;
            a2 = MoveUtil::move(a1);

            ASSERT(!a1);
            ASSERT(!a2);
        }

        {
            WFA a1;
            WFA a2;
            a2 = MoveUtil::move(a1);

            ASSERT(!a1);
            ASSERT(!a2);
        }

        {
            FA a1 = makeTestArg<char>((int)99);
            FA a2;
            a2 = MoveUtil::move(a1);

            ASSERT(a1);
            ASSERT(a2);

            ASSERT(checkValue(a1, 99));
            ASSERT(checkValue(a2, 99));
        }

        {
            WFA a1 = makeTestArg<wchar_t>((int)99);
            WFA a2;
            a2 = MoveUtil::move(a1);

            ASSERT(a1);
            ASSERT(a2);

            ASSERT(checkValue(a1, 99));
            ASSERT(checkValue(a2, 99));
        }

        {
            FormattableType ft(42);
            FA a1 = makeTestArg<char>(ft);
            FA a2;
            a2 = MoveUtil::move(a1);

            ASSERT(a1);
            ASSERT(a2);

            ASSERT(checkFormattableTypeValue(a1, 42));
            ASSERT(checkFormattableTypeValue(a2, 42));
        }

        {
            FormattableType ft(42);
            WFA a1 = makeTestArg<wchar_t>(ft);
            WFA a2;
            a2 = MoveUtil::move(a1);

            ASSERT(a1);
            ASSERT(a2);

            ASSERT(checkFormattableTypeValue(a1, 42));
            ASSERT(checkFormattableTypeValue(a2, 42));
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING SWAP
        //
        // Concerns:
        //: 1 We can copy construct 'bslfmt::basic_format_arg' types and they
        //:   retain the same values when copied.
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

        typedef bslfmt::basic_format_arg<
            bslfmt::basic_format_context<char *, char> >
            FA;

        typedef bslfmt::basic_format_arg<
            bslfmt::basic_format_context<wchar_t *, wchar_t> >
            WFA;

        if (verbose)
            printf("\nValidating swap\n");

        {
            FormattableType formattable(42);
            FA              a1;
            FA              a2 = makeTestArg<char>((int)99);
            FA              a3 = makeTestArg<char>(formattable);

            bsl::swap(a1, a2);
            bsl::swap(a2, a3);

            ASSERT(checkValue(a1, 99));
            ASSERT(checkFormattableTypeValue(a2, 42));
            ASSERT(!a3);
        }

        {
            FormattableType formattable(42);
            WFA             a1;
            WFA             a2 = makeTestArg<wchar_t>((int)99);
            WFA             a3 = makeTestArg<wchar_t>(formattable);

            bsl::swap(a1, a2);
            bsl::swap(a2, a3);

            ASSERT(checkValue(a1, 99));
            ASSERT(checkFormattableTypeValue(a2, 42));
            ASSERT(!a3);
        }


      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY/MOVE CONSTRUCTOR
        //
        // Concerns:
        //: 1 We can copy construct 'bslfmt::basic_format_arg' types and they
        //:   retain the same values when copied.
        //
        // Plan:
        //: 1 Construct a valueless 'bslfmt::basic_format_arg', copy it and
        //:   verify the copied version is valueless. (C-1)
        //:
        //: 2 Construct a 'bslfmt::basic_format_arg' holding an int, copy it
        //:   and verify the copied version holds the same value. (C-1)
        //:
        //: 3 Construct a 'bslfmt::basic_format_arg' holding a reference to a
        //:   type, copy it and verify the copied version references the same
        //:   type. (C-1)
        //:
        //: 4 Repeat the above tests using rvalue construction.
        //
        // Testing:
        //   formatter(const formatter &);
        //   formatter(formatter &&);
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING COPY CONSTRUCTOR"
                   "\n========================\n");

        typedef bslfmt::basic_format_arg<
            bslfmt::basic_format_context<char *, char> >
            FA;

        typedef bslfmt::basic_format_arg<
            bslfmt::basic_format_context<wchar_t *, wchar_t> >
            WFA;

        typedef bslmf::MovableRefUtil  MoveUtil;

        if (verbose)
            printf("\nValidating copy construction\n");

        {
            FA a1;
            FA a2(a1);

            ASSERT(!a1);
            ASSERT(!a2);
        }

        {
            WFA a1;
            WFA a2(a1);

            ASSERT(!a1);
            ASSERT(!a2);
        }

        {
            FA a1 = makeTestArg<char>((int)99);
            FA a2(a1);

            ASSERT(a1);
            ASSERT(a2);

            ASSERT(checkValue(a1, 99));
            ASSERT(checkValue(a2, 99));
        }

        {
            WFA a1 = makeTestArg<wchar_t>((int)99);
            WFA a2(a1);

            ASSERT(a1);
            ASSERT(a2);

            ASSERT(checkValue(a1, 99));
            ASSERT(checkValue(a2, 99));
        }

        {
            FormattableType ft(42);
            FA              a1 = makeTestArg<char>(ft);
            FA              a2(a1);

            ASSERT(a1);
            ASSERT(a2);

            ASSERT(checkFormattableTypeValue(a1, 42));
            ASSERT(checkFormattableTypeValue(a2, 42));
        }

        {
            FormattableType ft(42);
            WFA             a1 = makeTestArg<wchar_t>(ft);
            WFA             a2(a1);

            ASSERT(a1);
            ASSERT(a2);

            ASSERT(checkFormattableTypeValue(a1, 42));
            ASSERT(checkFormattableTypeValue(a2, 42));
        }

        {
            FA a1;
            FA a2(MoveUtil::move(a1));

            ASSERT(!a1);
            ASSERT(!a2);
        }

        {
            WFA a1;
            WFA a2(MoveUtil::move(a1));

            ASSERT(!a1);
            ASSERT(!a2);
        }

        {
            FA a1 = makeTestArg<char>((int)99);
            FA a2(MoveUtil::move(a1));

            ASSERT(a1);
            ASSERT(a2);

            ASSERT(checkValue(a1, 99));
            ASSERT(checkValue(a2, 99));
        }

        {
            WFA a1 = makeTestArg<wchar_t>((int)99);
            WFA a2(MoveUtil::move(a1));

            ASSERT(a1);
            ASSERT(a2);

            ASSERT(checkValue(a1, 99));
            ASSERT(checkValue(a2, 99));
        }

        {
            FormattableType ft(42);
            FA              a1 = makeTestArg<char>(ft);
            FA              a2(MoveUtil::move(a1));

            ASSERT(a1);
            ASSERT(a2);

            ASSERT(checkFormattableTypeValue(a1, 42));
            ASSERT(checkFormattableTypeValue(a2, 42));
        }

        {
            FormattableType ft(42);
            WFA             a1 = makeTestArg<wchar_t>(ft);
            WFA             a2(MoveUtil::move(a1));

            ASSERT(a1);
            ASSERT(a2);

            ASSERT(checkFormattableTypeValue(a1, 42));
            ASSERT(checkFormattableTypeValue(a2, 42));
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
        //: 1 `operator BoolType()` for a default constructed
        //:   `bslfmt::basic_format_arg` will return false;
        //:
        //: 2 `operator BoolType()` for a non-default constructed
        //:   `bslfmt::basic_format_arg` will return true;
        //
        // Plan:
        //: 1 Construct a default 'bslfmt::basic_format_arg' and verify the
        //:   result of calling `operator BoolType()`. (C-1)
        //:
        //: 2 Construct a non-default 'bslfmt::basic_format_arg' and verify
        //:   the result of calling `operator BoolType()`. (C-2)
        //
        // Testing:
        //   operator BoolType();
        // --------------------------------------------
        if (verbose)
            printf("\nTESTING BASIC ACCESSORS"
                   "\n=======================\n");

        typedef bslfmt::basic_format_arg<
            bslfmt::basic_format_context<char *, char> >
            FA;

        typedef bslfmt::basic_format_arg<
            bslfmt::basic_format_context<wchar_t *, wchar_t> >
            WFA;

        {
            // Test with default construction

            FA dummy_c;
            ASSERT(!dummy_c);

            WFA dummy_w;
            ASSERT(!dummy_w);
        }

        {
            // Test with non-default construction. Note that we cannot
            // construct this directly so have to rely on the internal-use-only
            // utility struct `Format_ArgUtil`
            int value = 1;

            FA fa = makeTestArg<char>(value);

            ASSERT(fa);

            WFA wfa = makeTestArg<wchar_t>(value);

            ASSERT(wfa);
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
        //   basic_format_arg();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING (PRIMITIVE) GENERATORS"
                            "\n==============================\n");

        if (verbose)
            printf("\nValidating bslfmt construction\n");

        {
            // Test default construction
            try {
                bslfmt::basic_format_arg<
                    bslfmt::basic_format_context<char *, char> >
                    dummy_c;

                bslfmt::basic_format_arg<
                    bslfmt::basic_format_context<wchar_t *, wchar_t> >
                    dummy_w;
            }
            catch (...) {
                ASSERT(false);
            }
            ASSERT(true);
        }
        {
            // Test construction with a value. Note that we cannot test this
            // directly so have to rely on the internal-use-only utility struct
            // `Format_ArgUtil`
            try {
                int value = 1;

                bsl::array<bslfmt::basic_format_arg<
                               bslfmt::basic_format_context<char *, char> >,
                           1>
                    arr_c;

                bslfmt::Format_ArgUtil::makeFormatArgArray(&arr_c, value);

                bsl::array<bslfmt::basic_format_arg<
                               bslfmt::basic_format_context<char *, char> >,
                           1>
                    arr_w;

                bslfmt::Format_ArgUtil::makeFormatArgArray(&arr_w, value);
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

        // This type has very limited public-facing functionality, so there is
        // very little we can do in a breathing test.
        {
            // Test default construction
            try {
                bslfmt::basic_format_arg<
                    bslfmt::basic_format_context<char *, char> >
                    dummy_c;

                ASSERT(!dummy_c);

                bslfmt::basic_format_arg<
                    bslfmt::basic_format_context<wchar_t *, wchar_t> >
                    dummy_w;

                ASSERT(!dummy_w);
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
