// bslstl_format_context.t.cpp                                        -*-C++-*-
#include <bslfmt_format_context.h>

#include <bsls_bsltestutil.h>

#include <bslstl_string.h>

#include <bslfmt_format_args.h> // Testing only

#include <stdio.h>
#include <string.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// 'basic_format_context<t_OUT, t_CHAR> >' where t_OUT is
// an output iterator is a standard-compliant implementation of
// 'std::basic_format_context'. It is hard to test standalone as it is designed
// to be constructed only indirectly by the 'bslfmt::format' suite of
// functions, and such testing requires the creation of "mock" visitors and
// argument containers.
//
// It should meet the requirements specified in [format.string.std].
//
//-----------------------------------------------------------------------------
// CLASS 'bsl::basic_format_args'
//
// CREATORS
// [ 3] basic_format_context();
// [ 3] ~basic_format_context();
// [ 7] basic_format_context(const basic_format_context &);
// [12] TBD
//
// MANIPULATORS
// [ 6] operator==(const basic_format_context &);
// [ 9] operator=(const basic_format_context &);
//
// ACCESSORS
// [ 4] get(size_t);
//
// FREE FUNCTIONS
// [ 8] swap(basic_format_context &, basic_format_context &);
// [11] TBD
// [11] TBD
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

    bslfmt::Format_ArgUtil::makeFormatArgArray(&arr, val);

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


// ============================================================================
//                    GLOBAL TYPES AND FUNCTIONS FOR USAGE EXAMPLE
// ----------------------------------------------------------------------------

struct MyCharFormatter {
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
        char              v,
        t_FORMAT_CONTEXT& fc) const
    {
        typename t_FORMAT_CONTEXT::iterator out = fc.out();

        *out++ = v;

        return out;
    }
};

struct MyCharFormatterTestVisitor {
    mutable char value;

    void operator()(char v) const { value = v; }

    template <class t_TYPE>
    void operator()(const t_TYPE &) const
    {
        ASSERT(false);  // Wrong Type
    }
};

void testFormatter(bslfmt::format_args args)
{
    MyCharFormatter                        f;
    bsl::string                            result;
    bsl::back_insert_iterator<bsl::string> backiter(result);
    bslfmt::Format_ContextOutputIteratorImpl<
        char, bsl::back_insert_iterator<bsl::string> >
                                           iter(backiter);
    bslfmt::format_context fc = 
        bslfmt::Format_ContextFactory::construct(
                      bslfmt::Format_ContextOutputIteratorRef<char>(&iter),
                      args);
    MyCharFormatterTestVisitor visitor;
    bslfmt::visit_format_arg(visitor, fc.arg(0));
    f.format(visitor.value, fc);
    ASSERT(result.size() == 1);
    ASSERT(result.front() == 'x');
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
        //:
        //: 2 Construct an instance with a single value and verify contents.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

///Example: 1 Testing a user defined formatter's `format` method.
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// We do not expect most users of `bsl::format` to interact with this type
// directly and instead use `bsl::format` or `bsl::vformat`. In addition, there
// are only a very limited number of public methods so this example is
// necessarily unrealistic.
//
// Suppose we have a user-defined formatter and want to test its `format`
// method:
//
//..
//  struct MyCharFormatter {
//      template <class t_FORMAT_CONTEXT>
//      typename t_FORMAT_CONTEXT::iterator format(
//          char              v,
//          t_FORMAT_CONTEXT& fc) const
//      {
//          typename t_FORMAT_CONTEXT::iterator out = fc.out();
//
//          *out++ = v;
//
//          return out;
//      }
//  };
//..
// 
// We can then write a test function. Note that it is not possible for users
// to construct a context directly, so we are forced to abuse the
// internal-use-only types `Format_ContextOutputIteratorRef` and
// `Format_ContextFactory` in order to write this usage example.
//
//..
//  struct MyCharFormatterTestVisitor {
//      char value;
//
//      void operator()(char v) { value = v; }
//
//      template <class t_TYPE>
//      void operator()(const t_TYPE &x) const
//      {
//          assert(false);  // Wrong Type
//      }
//  };
//
//  void testFormatter(bslfmt::format_args args)
//  {
//      MyCharFormatter                        f;
//      bsl::string                            result;
//      bsl::back_insert_iterator<bsl::string> backiter(result);
//      bslfmt::Format_ContextOutputIteratorImpl<
//          char, bsl::back_insert_iterator<bsl::string> >
//                                             iter(backiter);
//      bslfmt::format_context fc = 
//          bslfmt::Format_ContextFactory::construct(
//                       bslfmt::Format_ContextOutputIteratorRef<char>(&iter),
//                       args);
//      MyCharFormatterTestVisitor visitor;
//      bslfmt::visit_format_arg(visitor, fc.arg(0));
//      f.format(visitor.value, fc);
//      assert(result.size() == 1);
//      assert(result.front() == 'x');
//  }
//..
//
// Then we perform the test itself:
//
//..
        char value = 'x';
        testFormatter(bslfmt::make_format_args(value));
//..
      } break;
      case 12: {
        // --------------------------------------------
        // TESTING CONSTRUCTION FROM ARG STORE - WORK IN PROGRESS
        //
        // Testing:
        //   basic_format_args(Format_ArgsStore);
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
        //
        // Plan:
        //
        // Testing:
        //   make_format_args(ARGS&...)
        //   make_Wformat_args(ARGS&...)
        // --------------------------------------------------------------------
        if (verbose)
            printf("\nFREE FUNCTIONS"
                   "\n==============\n");

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
        //
        // Plan:
        //
        // Testing:
        //   operator=(const basic_format_args &);
        //   operator=(basic_format_args &&);
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING ASSIGNMENT OPERATOR"
                   "\n===========================\n");

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING SWAP
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   swap(formatter &, formatter &)
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING SWAP"
                   "\n============\n");

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY/MOVE CONSTRUCTOR
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   formatter(const formatter &);
        //   formatter(formatter &&);
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING COPY CONSTRUCTOR"
                   "\n========================\n");

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

        typedef bslfmt::Format_ArgsStore<bslfmt::format_context, int>
            FASI;
        typedef bslfmt::Format_ArgsStore<bslfmt::wformat_context, int>
            WFASI;

        // This type has very limited public-facing functionality, so there
        // is very little we can do in a breathing test.

        {
            int value = 5;

            bsl::string                            result;
            bsl::back_insert_iterator<bsl::string> backiter(result);
            bslfmt::Format_ContextOutputIteratorImpl<
                char,
                bsl::back_insert_iterator<bsl::string> >
                iter(backiter);

            FASI                argstore = bslfmt::make_format_args(value);
            bslfmt::format_args args(argstore);

            bslfmt::format_context fc =
                      bslfmt::Format_ContextFactory::construct(
                          bslfmt::Format_ContextOutputIteratorRef<char>(&iter),
                          args);

            ASSERT(fc.arg(0));
            ASSERT(!fc.arg(1));
        }

        {
            int value = 5;

            bsl::wstring                            result;
            bsl::back_insert_iterator<bsl::wstring> backiter(result);
            bslfmt::Format_ContextOutputIteratorImpl<
                wchar_t,
                bsl::back_insert_iterator<bsl::wstring> >
                iter(backiter);

            WFASI                argstore = bslfmt::make_wformat_args(value);
            bslfmt::wformat_args args(argstore);

            bslfmt::wformat_context fc =
                   bslfmt::Format_ContextFactory::construct(
                       bslfmt::Format_ContextOutputIteratorRef<wchar_t>(&iter),
                       args);

            ASSERT(fc.arg(0));
            ASSERT(!fc.arg(1));
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
