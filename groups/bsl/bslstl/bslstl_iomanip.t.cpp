// bslstl_iomanip.t.cpp                                               -*-C++-*-
#include <bslstl_iomanip.h>

#include <bslstl_stringstream.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_nameof.h>

#include <cstdlib>   // 'atoi'
#include <stddef.h>  // 'size_t'
#include <stdio.h>   // 'stderr'
#include <string.h>  // 'strlen'

using namespace BloombergLP;
using bsls::NameOf;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// Normally, a BDE component provides many functions, that are individually
// fairly simple, individual test cases in test drivers test one, or a few,
// functions per test case, and test those functions thoroughly.  This
// component, however, provides only one function, and that function is a
// template function with a potentially wide range of inputs.  That function
// returns objects of special private types ('IoManip_QuotedStringFormatter'
// and 'IoManip_QuotedStringViewFormatter').  So test cases here check methods
// of these classes and that one function in different ways.
// ----------------------------------------------------------------------------
// [ 2] IoManip_QuotedStringViewFormatter(basic_string *s,t_TYPE,t_TYPE);
// [ 2] const t_TYPE *IoManip_QuotedStringViewFormatter::data();
// [ 2] t_TYPE IoManip_QuotedStringViewFormatter::delim();
// [ 2] t_TYPE IoManip_QuotedStringViewFormatter::escape();
// [ 2] IoManip_QuotedStringFormatter(basic_string *str, t_TYPE, t_TYPE);
// [ 2] basic_string *IoManip_QuotedStringFormatter::str();
// [ 2] t_TYPE IoManip_QuotedStringFormatter::delim();
// [ 2] t_TYPE IoManip_QuotedStringFormatter::escape();
// [ 3] operator>>(istream&, const IoManip_QuotedStringFormatter&);
// [ 3] operator<<(ostream&, const IoManip_QuotedStringFormatter&);
// [ 3] operator<<(ostream&, const IoManip_QuotedStringViewFormatter&)
// [ 4] decltype(auto) quoted(basic_string& value, t_TYPE d, t_TYPE e);
// [ 4] QuotedStringFormatter quoted(const basic_string&,t_TYPE,t_TYPE);
// [ 4] QuotedStringViewFormatter quoted(basic_string_view&,t_TYPE, TYPE);
// ----------------------------------------------------------------------------
// [ 5] USAGE EXAMPLE
// [ 1] BREATHING TEST

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

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                       GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

bool             verbose;
bool         veryVerbose;
bool     veryVeryVerbose;
bool veryVeryVeryVerbose;

struct DataRow {
    int         d_line;          // source line number

    const char *d_spec_p;        // specification of the original string

    const char *d_expDefault_p;  // specification string of expected stream
                                 // content for object with default delimiters
                                 // ('"' and '/')

    const char *d_expCustom_p;   // specification string of expected stream
                                 // content for object with custom delimiters
                                 //('$' and '%')
};

static const DataRow DATA[] = {
    //LINE  SPEC          EXP DEFAULT              EXP CUSTOM
    //----  ----------    ------------------       --------------
    { L_,   "",           "\"\"",                  "$$"           },
    { L_,   "A",          "\"A\"",                 "$A$"          },
    { L_,   "B",          "\"B\"",                 "$B$"          },
    { L_,   "\"",         "\"\\\"\"",              "$\"$"         },
    { L_,   "\\",         "\"\\\\\"",              "$\\$"         },
    { L_,   "$",          "\"$\"",                 "$%$$"         },
    { L_,   "%",          "\"%\"",                 "$%%$"         },
    { L_,   "AB",         "\"AB\"",                "$AB$"         },
    { L_,   "A\"",        "\"A\\\"\"",             "$A\"$"        },
    { L_,   "A\\",        "\"A\\\\\"",             "$A\\$"        },
    { L_,   "A$",         "\"A$\"",                "$A%$$"        },
    { L_,   "A%",         "\"A%\"",                "$A%%$"        },
    { L_,   "\"A",        "\"\\\"A\"",             "$\"A$"        },
    { L_,   "\\A",        "\"\\\\A\"",             "$\\A$"        },
    { L_,   "$A",         "\"$A\"",                "$%$A$"        },
    { L_,   "%A",         "\"%A\"",                "$%%A$"        },
    { L_,   "\"\"",       "\"\\\"\\\"\"",          "$\"\"$"       },
    { L_,   "\\\\",       "\"\\\\\\\\\"",          "$\\\\$"       },
    { L_,   "$$",         "\"$$\"",                "$%$%$$"       },
    { L_,   "%%",         "\"%%\"",                "$%%%%$"       },
    { L_,   "\"A\"",      "\"\\\"A\\\"\"",         "$\"A\"$"      },
    { L_,   "\\A\\",      "\"\\\\A\\\\\"",         "$\\A\\$"      },
    { L_,   "$A$",        "\"$A$\"",               "$%$A%$$"      },
    { L_,   "%A%",        "\"%A%\"",               "$%%A%%$"      },
    { L_,   "A\"B\"C",    "\"A\\\"B\\\"C\"",       "$A\"B\"C$"    },
    { L_,   "A\\B\\C",    "\"A\\\\B\\\\C\"",       "$A\\B\\C$"    },
    { L_,   "A$B$C",      "\"A$B$C\"",             "$A%$B%$C$"    },
    { L_,   "A%B%C",      "\"A%B%C\"",             "$A%%B%%C$"    },
    { L_,   "A\"\\$%B",   "\"A\\\"\\\\$%B\"",      "$A\"\\%$%%B$" },
};

const size_t NUM_DATA = sizeof DATA / sizeof *DATA;


//=============================================================================
//                      GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
template <class t_CHAR_TYPE, class t_CHAR_TRAITS>
bool operator==(
           const bslstl::IoManip_QuotedStringViewFormatter<t_CHAR_TYPE,
                                                           t_CHAR_TRAITS>& lhs,
           const bslstl::IoManip_QuotedStringViewFormatter<t_CHAR_TYPE,
                                                           t_CHAR_TRAITS>& rhs)
    // Return 'true' if the specified 'lhs' object has the same value as the
    // specified 'rhs' object, and 'false' otherwise.
{
    return (
      lhs.delim() == rhs.delim() &&
      lhs.escape() == rhs.escape() &&
      t_CHAR_TRAITS::length(lhs.data()) == t_CHAR_TRAITS::length(rhs.data()) &&
      0 == t_CHAR_TRAITS::compare(lhs.data(),
                                rhs.data(),
                                t_CHAR_TRAITS::length(lhs.data())));
}

template <class t_CHAR_TYPE, class t_CHAR_TRAITS, class t_ALLOC>
bool operator==(const bslstl::IoManip_QuotedStringFormatter<t_CHAR_TYPE,
                                                            t_CHAR_TRAITS,
                                                            t_ALLOC>& lhs,
                const bslstl::IoManip_QuotedStringFormatter<t_CHAR_TYPE,
                                                            t_CHAR_TRAITS,
                                                            t_ALLOC>& rhs)
    // Return 'true' if the specified 'lhs' object has the same value as the
    // specified 'rhs' object, and 'false' otherwise.
{
    return (lhs.delim() == rhs.delim() &&
            lhs.escape() == rhs.escape() &&
            *lhs.str() == *rhs.str());
}
#endif

//=============================================================================
//                       TEST DRIVER TEMPLATE
//-----------------------------------------------------------------------------

template <class t_TYPE,
          class t_TRAITS = bsl::char_traits<t_TYPE>,
          class t_ALLOC  = bsl::allocator<t_TYPE> >
struct TestDriver {
    // This templatized struct provide a namespace for testing the functions
    // declared in the 'bslstl_iomanip' header.  The parameterized 't_TYPE',
    // 't_TRAITS', and 't_ALLOC' specify the character type, the type traits
    // and allocator type respectively.  Every test case should be invoked with
    // various parameterized type to fully test the functions.

    // TEST CASES
    static void testCase2_string();
        // Test 'IoManip_QuotedStringFormatter'.

    static void testCase2_stringView();
        // Test 'IoManip_QuotedStringViewFormatter'.

    static void testCase3_string();
        // Test 'IoManip_QuotedStringFormatter' input and output operators.

    static void testCase3_stringView();
        // Test 'IoManip_QuotedStringViewFormatter' output operator.

    static void testCase4();
        // Test 'bsl::quoted'.
};

                               // --------------
                               // TEST APPARATUS
                               // --------------

template <class t_TYPE, class t_TRAITS, class t_ALLOC>
void TestDriver<t_TYPE,t_TRAITS,t_ALLOC>::testCase2_string()
{
    // ------------------------------------------------------------------------
    // 'IoManip_QuotedStringFormatter' PRIMARY MANIPULATORS/BASIC ACCESSORS
    //
    // Concerns:
    //: 1 Any string (including empty) can be used to create an
    //:   'IoManip_QuotedStringFormatter' object.
    //:
    //: 2 No additional memory is allocated upon
    //:   'IoManip_QuotedStringFormatter' object construction.
    //:
    //: 3 Default parameters assume expected values.
    //
    // Plan:
    //: 1 Create several different objects and verify their values using
    //:   accessors.  (C-1..3)
    //
    // Testing:
    //   IoManip_QuotedStringFormatter(basic_string *str, t_TYPE d, t_TYPE e);
    //   basic_string *IoManip_QuotedStringFormatter::str();
    //   t_TYPE IoManip_QuotedStringFormatter::delim();
    //   t_TYPE IoManip_QuotedStringFormatter::escape();
    // ------------------------------------------------------------------------

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
    if (verbose)
        printf("Testing 'IoManip_QuotedStringFormatter<%s>'\n",
               NameOf<t_TYPE>().name());

    typedef bsl::basic_string<t_TYPE, t_TRAITS, t_ALLOC>                String;
    typedef bslstl::IoManip_QuotedStringFormatter<t_TYPE, t_TRAITS, t_ALLOC>
                                                                        Obj;

    bslma::TestAllocator         oa("object", veryVeryVeryVerbose);
    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    String empty(&oa);
    String nonEmpty(64, t_TYPE('a'), &oa);

    bslma::TestAllocatorMonitor  oam(&oa);
    bslma::TestAllocatorMonitor  dam(&da);

    Obj        mXEmpty(&empty);
    const Obj& XEmpty = mXEmpty;

    ASSERT(&empty    == XEmpty.str());
    ASSERT('"'       == XEmpty.delim());
    ASSERT('\\'      == XEmpty.escape());

    Obj        mXNonEmpty(&nonEmpty, 'a', 'b');
    const Obj& XNonEmpty = mXNonEmpty;

    ASSERT(&nonEmpty == XNonEmpty.str());
    ASSERT('a'       == XNonEmpty.delim());
    ASSERT('b'       == XNonEmpty.escape());

    ASSERT(oam.isTotalSame());
    ASSERT(dam.isTotalSame());
#else
    if (verbose)
        printf("'IoManip_QuotedStringFormatter<%s>' is not supported\n",
               NameOf<t_TYPE>().name());
#endif
}

template <class t_TYPE, class t_TRAITS, class t_ALLOC>
void TestDriver<t_TYPE,t_TRAITS,t_ALLOC>::testCase2_stringView()
{
    // ------------------------------------------------------------------------
    // 'IoManip_QuotedStringViewFormatter' PRIMARY MANIPULATORS/BASIC ACCESSORS
    //
    // Concerns:
    //: 1 Any 'bsl::string_view' object (including empty) can be used to create
    //:   an 'IoManip_QuotedStringViewFormatter' object.
    //:
    //: 2 The default allocator is used to supply memory for
    //:   'IoManip_QuotedStringViewFormatter' object construction.
    //:
    //: 3 Default parameters assume expected values.
    //
    // Plan:
    //: 1 Create several different objects and verify their values using
    //:   accessors.  Verify memory is allocated from the default allocator
    //:   (C-1..3)
    //
    // Testing:
    //   IoManip_QuotedStringViewFormatter(basic_string *s, t_TYPE d,t_TYPE e);
    //   const t_TYPE *IoManip_QuotedStringViewFormatter::data();
    //   t_TYPE IoManip_QuotedStringViewFormatter::delim();
    //   t_TYPE IoManip_QuotedStringViewFormatter::escape();
    // ------------------------------------------------------------------------

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
    if (verbose)
        printf("Testing 'IoManip_QuotedStringViewFormatter<%s>'\n",
               NameOf<t_TYPE>().name());

    typedef bslstl::IoManip_QuotedStringViewFormatter<t_TYPE, t_TRAITS>
                                                                    Obj;
    typedef bsl::basic_string<t_TYPE, t_TRAITS>                     String;
    typedef bsl::basic_string_view<t_TYPE, t_TRAITS>                StringView;

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    String     empty;
    String     nonEmpty(64, t_TYPE('a'));
    StringView emptyView(empty.c_str());
    StringView nonEmptyView(nonEmpty.c_str());

    bslma::TestAllocatorMonitor dam(&da);
    {
        Obj        mXEmpty(emptyView);
        const Obj& XEmpty = mXEmpty;

        // We don't need allocated memory for an empty string.
        ASSERT(dam.isInUseSame());

        ASSERT(empty == XEmpty.data());
        ASSERT('"'   == XEmpty.delim());
        ASSERT('\\'  == XEmpty.escape());
    }

    ASSERT(dam.isInUseSame());

    {
        Obj        mXNonEmpty(nonEmptyView, 'a', 'b');
        const Obj& XNonEmpty = mXNonEmpty;

        ASSERT(dam.isInUseUp());

        ASSERT(nonEmpty == XNonEmpty.data());
        ASSERT('a'      == XNonEmpty.delim());
        ASSERT('b'      == XNonEmpty.escape());
    }
    ASSERT(dam.isInUseSame());
#else
    if (verbose)
        printf("'IoManip_QuotedStringViewFormatter<%s>' is not supported\n",
               NameOf<t_TYPE>().name());
#endif
}

template <class t_TYPE, class t_TRAITS, class t_ALLOC>
void TestDriver<t_TYPE,t_TRAITS,t_ALLOC>::testCase3_string()
{
    // ------------------------------------------------------------------------
    // TESTING I/O OPERATORS FOR 'IoManip_QuotedStringFormatter'
    //
    // Concerns:
    //: 1 Delimeter and escape character passed on object construction are
    //:   taken into account during input/output operations.
    //:
    //: 2 The output operator signature and return type are standard.
    //:
    //: 3 The input operator signature is not standard according to design.
    //:
    //: 4 The I/O operators return references to the source/destination
    //:   streams.
    //
    // Plan:
    //: 1 Use the addresses of the 'operator>>' and 'operator<<' free functions
    //:   defined in this component to initialize, respectively free-function
    //:   pointers having the appropriate signatures and return types.
    //:   (C-2..3)
    //:
    //: 2 Create several different objects and write their values to some
    //:   stream.  Verify the stream content.  Read data from the stream to
    //:   some objects.  Verify their values.  (C-1, 4)
    //
    // Testing:
    //   istream& operator>>(istream&, const IoManip_QuotedStringFormatter&);
    //   ostream& operator<<(ostream&, const IoManip_QuotedStringFormatter&);
    // ------------------------------------------------------------------------

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
    if (verbose)
        printf("Testing IoManip_QuotedStringFormatter<%s>'\n",
                NameOf<t_TYPE>().name());

    typedef bslstl::IoManip_QuotedStringFormatter<t_TYPE, t_TRAITS, t_ALLOC>
                                                                       Obj;
    typedef bsl::basic_string<t_TYPE, t_TRAITS, t_ALLOC>               String;
    typedef bsl::basic_stringstream<t_TYPE, t_TRAITS>                  Stream;
    typedef std::basic_ostream<t_TYPE, t_TRAITS>                       OStream;
    typedef std::basic_istream<t_TYPE, t_TRAITS>                       IStream;

    const t_TYPE DELIM  = '$';
    const t_TYPE ESCAPE = '%';

    // Verify that the signatures and return types are standard.
    {
        typedef IStream& (*operatorInPtr )(IStream&, const Obj&);
        typedef OStream& (*operatorOutPtr)(OStream&, const Obj&);

        operatorInPtr  operatorIn  = bslstl::operator>>;
        operatorOutPtr operatorOut = bslstl::operator<<;

        (void)operatorIn;
        (void)operatorOut;
    }

    for (size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int     LINE             = DATA[ti].d_line;
        const char   *SPEC             = DATA[ti].d_spec_p;
        const char   *DEFAULT_SPEC     = DATA[ti].d_expDefault_p;
        const char   *CUSTOM_SPEC      = DATA[ti].d_expCustom_p;
        const size_t  SPEC_LEN         = strlen(SPEC);
        const size_t  DEFAULT_SPEC_LEN = strlen(DEFAULT_SPEC);
        const size_t  CUSTOM_SPEC_LEN  = strlen(CUSTOM_SPEC);

        String originString;
        for (size_t i = 0; i < SPEC_LEN; ++i) {
            originString.push_back(t_TYPE(SPEC[i]));
        }

        String expectedDefault;
        for (size_t i = 0; i < DEFAULT_SPEC_LEN; ++i) {
            expectedDefault.push_back(t_TYPE(DEFAULT_SPEC[i]));
        }

        String expectedCustom;
        for (size_t i = 0; i < CUSTOM_SPEC_LEN; ++i) {
            expectedCustom.push_back(t_TYPE(CUSTOM_SPEC[i]));
        }

        Obj        mXDefault(&originString);
        const Obj& XDefault = mXDefault;

        Obj        mXCustom(&originString, DELIM, ESCAPE);
        const Obj& XCustom = mXCustom;

        Stream defaultStream;
        Stream customStream;

        OStream* mRODefault = &(defaultStream << XDefault);
        OStream* mROCustom  = &(customStream  << XCustom);

        ASSERTV(NameOf<t_TYPE>(), LINE, &defaultStream == mRODefault);
        ASSERTV(NameOf<t_TYPE>(), LINE, &customStream  == mROCustom );

        ASSERTV(NameOf<t_TYPE>(), LINE,
                expectedDefault == defaultStream.str());
        ASSERTV(NameOf<t_TYPE>(), LINE,
                expectedCustom  == customStream.str() );

        String defaultOutputBuffer;
        String customOutputBuffer;
        Obj    mXDefaultOutput(&defaultOutputBuffer);
        Obj    mXCustomOutput(&customOutputBuffer, DELIM, ESCAPE);

        IStream* mRIDefault = &(defaultStream >> mXDefaultOutput);
        IStream* mRICustom  = &(customStream  >> mXCustomOutput );

        ASSERTV(NameOf<t_TYPE>(), LINE, &defaultStream == mRIDefault);
        ASSERTV(NameOf<t_TYPE>(), LINE, &customStream  == mRICustom );

        ASSERTV(NameOf<t_TYPE>(), LINE, originString == defaultOutputBuffer);
        ASSERTV(NameOf<t_TYPE>(), LINE, originString == customOutputBuffer );
    }
#else
    if (verbose)
        printf("'IoManip_QuotedStringFormatter<%s>' is not supported\n",
               NameOf<t_TYPE>().name());
#endif
}

template <class t_TYPE, class t_TRAITS, class t_ALLOC>
void TestDriver<t_TYPE,t_TRAITS,t_ALLOC>::testCase3_stringView()
{
    // ------------------------------------------------------------------------
    // TESTING OUTPUT OPERATOR FOR 'IoManip_QuotedStringViewFormatter'
    //
    // Concerns:
    //: 1 Delimeter and escape character passed on object construction are
    //:   taken into account during output operation.
    //:
    //: 2 The output operator signature and return type is standard.
    //:
    //: 3 The output operator returns reference to the destination stream.
    //
    // Plan:
    //: 1 Use the address of the 'operator<<' free function defined in this
    //:   component to initialize free-function pointer having the appropriate
    //:   signature and return type.  (C-2)
    //:
    //: 2 Create several different objects and write their values to some
    //:   stream.  Verify the stream content.  (C-1, 3)
    //
    // Testing:
    //   ostream& operator<<(ostream&,const IoManip_QuotedStringViewFormatter&)
    // ------------------------------------------------------------------------

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
    if (verbose)
        printf("Testing IoManip_QuotedStringViewFormatter<%s>'\n",
               NameOf<t_TYPE>().name());

    typedef bslstl::IoManip_QuotedStringViewFormatter<t_TYPE, t_TRAITS>
                                                                    Obj;
    typedef bsl::basic_string<t_TYPE, t_TRAITS>                     String;
    typedef bsl::basic_string_view<t_TYPE, t_TRAITS>                StringView;
    typedef bsl::basic_stringstream<t_TYPE, t_TRAITS>               Stream;
    typedef std::basic_ostream<t_TYPE, t_TRAITS>                    OStream;

    const t_TYPE DELIM  = '$';
    const t_TYPE ESCAPE = '%';

    // Verify that the signatures and return types are standard.
    {
        typedef OStream& (*operatorOutPtr)(OStream&, const Obj&);

        operatorOutPtr operatorOut = bslstl::operator<<;

        (void)operatorOut;
    }

    for (size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int     LINE             = DATA[ti].d_line;
        const char   *SPEC             = DATA[ti].d_spec_p;
        const char   *DEFAULT_SPEC     = DATA[ti].d_expDefault_p;
        const char   *CUSTOM_SPEC      = DATA[ti].d_expCustom_p;
        const size_t  SPEC_LEN         = strlen(SPEC);
        const size_t  DEFAULT_SPEC_LEN = strlen(DEFAULT_SPEC);
        const size_t  CUSTOM_SPEC_LEN  = strlen(CUSTOM_SPEC);

        String originString;
        for (size_t i = 0; i < SPEC_LEN; ++i) {
            originString.push_back(t_TYPE(SPEC[i]));
        }
        StringView stringView(originString.c_str());

        String expectedDefault;
        for (size_t i = 0; i < DEFAULT_SPEC_LEN; ++i) {
            expectedDefault.push_back(t_TYPE(DEFAULT_SPEC[i]));
        }

        String expectedCustom;
        for (size_t i = 0; i < CUSTOM_SPEC_LEN; ++i) {
            expectedCustom.push_back(t_TYPE(CUSTOM_SPEC[i]));
        }

        Obj        mXDefault(stringView);
        const Obj& XDefault = mXDefault;

        Obj        mXCustom(stringView, DELIM, ESCAPE);
        const Obj& XCustom = mXCustom;

        Stream defaultStream;
        Stream customStream;

        OStream* mRDefault = &(defaultStream << XDefault);
        OStream* mRCustom  = &(customStream  << XCustom );

        ASSERTV(NameOf<t_TYPE>(), LINE, &defaultStream == mRDefault);
        ASSERTV(NameOf<t_TYPE>(), LINE, &customStream  == mRCustom );

        ASSERTV(NameOf<t_TYPE>(), LINE,
                expectedDefault == defaultStream.str());
        ASSERTV(NameOf<t_TYPE>(), LINE,
                expectedCustom  == customStream.str() );
    }
#else
    if (verbose)
        printf("'IoManip_QuotedStringViewFormatter<%s>, is not supported\n",
               NameOf<t_TYPE>().name());
#endif
}

template <class t_TYPE, class t_TRAITS, class t_ALLOC>
void TestDriver<t_TYPE,t_TRAITS,t_ALLOC>::testCase4()
{
    // ------------------------------------------------------------------------
    // TESTING 'bsl::quoted'
    //   Since 'bsl::quoted' just calls 'IoManip_Quoted*Formatter' constructors
    //   or 'std::quoted' function, we want to make sure that incoming
    //   parameters are correctly passed to these functions.
    //
    // Concerns:
    //: 1 Incoming parameters are correctly passed to the corresponding
    //:   'IoManip_Quoted*Formatter' constructors for non-constant
    //:   'bsl::string' objects and for 'bsl::string_view' objects.
    //:
    //: 2 Incoming parameters are correctly passed to the 'std::quoted'
    //:   function for constant 'bsl::string' objects.
    //
    // Plan:
    //: 1 Create several 'bsl::string' ans 'bsl::string_view' objects.  Create
    //:   corresponding 'IoManip_Quoted*Formatter' model objects, passing
    //:   various delimeters and escape characters.  Call 'bsl::quoted'
    //:   function for the original 'string' and 'string_view' objects, passing
    //:   the same delimeters and escape characterS and verify the results by
    //:   comparing them with the model objects.  (C-1)
    //:
    //: 2 Create several 'bsl::string' constant objects and print the result of
    //:   'bsl::quoted' function call to some stream.  Verify the content of
    //:   the stream.  (C-2)
    //
    // Testing:
    //   decltype(auto) quoted(basic_string& value, t_TYPE del, t_TYPE esc);
    //   decltype(auto) quoted(const basic_string& val,t_TYPE del, t_TYPE esc);
    //   decltype(auto) quoted(basic_string_view& val, t_TYPE del ,t_TYPE esc);
    // ------------------------------------------------------------------------

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
    if (verbose) printf("Testing %s\n", NameOf<t_TYPE>().name());

    typedef bsl::basic_string<t_TYPE, t_TRAITS, t_ALLOC> String;
    typedef bsl::basic_string_view<t_TYPE, t_TRAITS>     StringView;
    typedef bsl::basic_stringstream<t_TYPE, t_TRAITS>    Stream;

    typedef bslstl::IoManip_QuotedStringFormatter<t_TYPE, t_TRAITS, t_ALLOC>
                                                                         ObjS;
    typedef bslstl::IoManip_QuotedStringViewFormatter<t_TYPE, t_TRAITS>  ObjSV;

    const char *NOT    = NameOf<t_TYPE>().name();
    const t_TYPE  DELIM  = '$';
    const t_TYPE  ESCAPE = '%';

    for (size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int     LINE     = DATA[ti].d_line;
        const char   *SPEC     = DATA[ti].d_spec_p;
        const size_t  SPEC_LEN = strlen(SPEC);

        String str;
        for (size_t i = 0; i < SPEC_LEN; ++i) {
            str.push_back(t_TYPE(SPEC[i]));
        }
        StringView strView(str.c_str());

        ObjS  modelSDefault(&str);
        ObjS  modelSCustom(&str, DELIM, ESCAPE);
        ObjSV modelSVDefault(str.c_str());
        ObjSV modelSVCustom(str.c_str(), DELIM, ESCAPE);

        ASSERTV(NOT, LINE, modelSDefault  == bsl::quoted(str));
        ASSERTV(NOT, LINE,
                modelSCustom == bsl::quoted(str, DELIM, ESCAPE));
#ifndef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
        ASSERTV(NOT, LINE, modelSVDefault == bsl::quoted(strView));
        ASSERTV(NOT, LINE,
                modelSVCustom == bsl::quoted(strView, DELIM, ESCAPE));
#endif
    }

    for (size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int     LINE             = DATA[ti].d_line;
        const char   *SPEC             = DATA[ti].d_spec_p;
        const char   *DEFAULT_SPEC     = DATA[ti].d_expDefault_p;
        const char   *CUSTOM_SPEC      = DATA[ti].d_expCustom_p;
        const size_t  SPEC_LEN         = strlen(SPEC);
        const size_t  DEFAULT_SPEC_LEN = strlen(DEFAULT_SPEC);
        const size_t  CUSTOM_SPEC_LEN  = strlen(CUSTOM_SPEC);

        String originString;
        for (size_t i = 0; i < SPEC_LEN; ++i) {
            originString.push_back(t_TYPE(SPEC[i]));
        }
        const String& constOriginString = originString;

        String expectedDefault;
        for (size_t i = 0; i < DEFAULT_SPEC_LEN; ++i) {
            expectedDefault.push_back(t_TYPE(DEFAULT_SPEC[i]));
        }

        String expectedCustom;
        for (size_t i = 0; i < CUSTOM_SPEC_LEN; ++i) {
            expectedCustom.push_back(t_TYPE(CUSTOM_SPEC[i]));
        }

        Stream defaultStream;
        Stream customStream;

        defaultStream << bsl::quoted(constOriginString);
        customStream  << bsl::quoted(constOriginString, DELIM, ESCAPE);

        ASSERTV(NameOf<t_TYPE>(), LINE,
                expectedDefault == defaultStream.str());
        ASSERTV(NameOf<t_TYPE>(), LINE,
                expectedCustom  == customStream.str() );
    }
#else
    if (verbose) printf("'bsl::quoted' is not supported\n");
#endif
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? std::atoi(argv[1]) : 0;
                verbose = argc > 2;    (void) verbose;
            veryVerbose = argc > 3;    (void) veryVerbose;
        veryVeryVerbose = argc > 4;    (void) veryVeryVerbose;
    veryVeryVeryVerbose = argc > 5;    (void) veryVeryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of 'bsl::quoted'
///- - - - - - - - - - - - - - - - - - -
// Suppose we want to serialize some data into JSON.
//
// First, we define a struct, 'Employee', to contain the data:
//..
    struct Employee {
        bsl::string d_firstName;
        bsl::string d_lastName;
        int         d_age;
    };
//..
// Then, we create an 'Employee' object and populate it with data:
//..
    Employee john;
    john.d_firstName = "John";
    john.d_lastName  = "Doe";
    john.d_age       = 20;
//..
//  Now, we create an output stream and manually construct the JSON string
//  using 'bsl::quoted':
//..
    bsl::stringstream ss;
    ss << '{' << '\n';
    ss << bsl::quoted("firstName");
    ss << ':';
    ss << bsl::quoted(john.d_firstName);
    ss << ',' << '\n';
    ss << bsl::quoted("lastName");
    ss << ':';
    ss << bsl::quoted(john.d_lastName);
    ss << ',' << '\n';
    ss << bsl::quoted("age");
    ss << ':';
    ss << john.d_age;
    ss << '\n' << '}';
//..
//  Finally, we check out the JSON string:
//..
    bsl::string expected = "{\n"
                           "\"firstName\":\"John\",\n"
                           "\"lastName\":\"Doe\",\n"
                           "\"age\":20\n"
                           "}";
    ASSERT(expected == ss.str());
//..
//  The output should look like:
//..
//  {
//  "firstName":"John",
//  "lastName":"Doe",
//  "age":20
//  }
//..
#else
        if (verbose) printf("Feature is not supported prior to CPP14");
#endif
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'bsl::quoted'
        //   Since 'bsl::quoted' just calls 'IoManip_Quoted*Formatter'
        //   constructors or 'std::quoted' function, we want to make sure that
        //   incoming parameters are correctly passed to these functions.
        //
        // Concerns:
        //: 1 Incoming parameters are correctly passed to the corresponding
        //:   'IoManip_Quoted*Formatter' constructors for non-constant
        //:   'bsl::string' objects and for 'bsl::string_view' objects.
        //:
        //: 2 Incoming parameters are correctly passed to the 'std::quoted'
        //:   function for constant 'bsl::string' objects.
        //
        // Plan:
        //: 1 Create several 'bsl::string' ans 'bsl::string_view' objects.
        //:   Create corresponding 'IoManip_Quoted*Formatter' model objects,
        //:   passing various delimeters and escape characters.  Call
        //:   'bsl::quoted' function for the original 'string' and
        //:   'string_view' objects, passing the same delimeters and escape
        //:   characters and verify the results by comparing them with the
        //    model objects.  (C-1)
        //:
        //: 2 Create several 'bsl::string' constant objects and print the
        //:   result of 'bsl::quoted' function call to some stream.  Verify the
        //:   content of the stream.  (C-2)
        //
        // Testing:
        //   decltype(auto) quoted(basic_string& value, t_TYPE d, t_TYPE e);
        //   QuotedStringFormatter quoted(const basic_string&,t_TYPE,t_TYPE);
        //   QuotedStringViewFormatter quoted(basic_string_view&,t_TYPE, TYPE);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'bsl::quoted'"
                            "\n=====================\n");

        TestDriver<char   >::testCase4();
        TestDriver<wchar_t>::testCase4();

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING I/O OPERATORS FOR 'IoManip_Quoted*Formatter'
        //
        // Concerns:
        //: 1 Delimeter and escape character passed on object construction are
        //:   taken into account during input/output operations.
        //:
        //: 2 The output operator signatures and return types are standard.
        //:
        //: 3 The input operator signature is not standard according to design.
        //:
        //: 4 The I/O operators return references to the source/destination
        //:   streams.
        //
        // Plan:
        //: 1 Use the addresses of the 'operator>>' and 'operator<<' free
        //:   functions defined in this component to initialize, respectively
        //:   free-function pointers having the appropriate signatures and
        //:   return types.  (C-2..3)
        //:
        //: 2 Create several different objects and write their values to some
        //:   stream.  Verify the stream content.  Read data from the stream to
        //:   some objects.  Verify their values.  (C-1, 4)
        //
        // Testing:
        //   operator>>(istream&, const IoManip_QuotedStringFormatter&);
        //   operator<<(ostream&, const IoManip_QuotedStringFormatter&);
        //   operator<<(ostream&, const IoManip_QuotedStringViewFormatter&)
        // --------------------------------------------------------------------

        if (verbose) printf(
                   "\nTESTING I/O OPERATORS FOR 'IoManip_Quoted*Formatter'"
                   "\n====================================================\n");

        TestDriver<char   >::testCase3_string();
        TestDriver<wchar_t>::testCase3_string();

        TestDriver<char   >::testCase3_stringView();
        TestDriver<wchar_t>::testCase3_stringView();

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // 'IoManip_Quoted*Formatter' PRIMARY MANIPULATORS/BASIC ACCESSORS
        //
        // Concerns:
        //: 1 Any string (including empty) can be used to create an
        //:   'IoManip_QuotedStringFormatter' object.
        //:
        //: 2 No additional memory is allocated upon
        //:   'IoManip_QuotedStringFormatter' object construction.
        //:
        //: 3 Any 'bsl::string_view' object (including empty) can be used to
        //:   create an 'IoManip_QuotedtringViewFormatter' object.
        //:
        //: 4 The default allocator is used to supply memory for
        //:   'IoManip_QuotedStringViewFormatter' object construction.
        //:
        //: 5 Default parameters assume expected values.
        //
        // Plan:
        //: 1 Create several different objects and verify their values using
        //:   accessors.  Verify memory is allocated from the default allocator
        //:   (C-1..5)
        //
        // Testing:
        //   IoManip_QuotedStringViewFormatter(basic_string *s,t_TYPE,t_TYPE);
        //   const t_TYPE *IoManip_QuotedStringViewFormatter::data();
        //   t_TYPE IoManip_QuotedStringViewFormatter::delim();
        //   t_TYPE IoManip_QuotedStringViewFormatter::escape();
        //   IoManip_QuotedStringFormatter(basic_string *str, t_TYPE, t_TYPE);
        //   basic_string *IoManip_QuotedStringFormatter::str();
        //   t_TYPE IoManip_QuotedStringFormatter::delim();
        //   t_TYPE IoManip_QuotedStringFormatter::escape();
        // --------------------------------------------------------------------

        if (verbose) printf(
          "'IoManip_Quoted*Formatter' PRIMARY MANIPULATORS/BASIC ACCESSORS\n"
          "===============================================================\n");

        TestDriver<char   >::testCase2_string();
        TestDriver<wchar_t>::testCase2_string();

        TestDriver<char   >::testCase2_stringView();
        TestDriver<wchar_t>::testCase2_stringView();

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
        //: 1 Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("BREATHING TEST\n"
                            "==============\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY

        // Testing with 'bsl::string' and default delimiters
        {
            bsl::string       strIn("test \"default\" delimiters");
            bsl::stringstream stream;

            stream << bsl::quoted(strIn);
            ASSERTV(stream.str().c_str(),
                    "\"test \\\"default\\\" delimiters\"" == stream.str());

            bsl::string strOut;
            stream >> bsl::quoted(strOut);
            ASSERTV(strOut.c_str(), strIn == strOut);
        }

        // Testing with 'bsl::string' and custom delimiters
        {
            const char        delimiter = '$';
            const char        escape    = '%';
            bsl::string       strIn("test $custom$ delimiters");
            bsl::stringstream stream;

            stream << bsl::quoted(strIn, delimiter, escape);
            ASSERTV(stream.str().c_str(),
                    "$test %$custom%$ delimiters$" == stream.str());

            bsl::string strOut;
            stream >> bsl::quoted(strOut, delimiter, escape);
            ASSERTV(strOut.c_str(), strIn == strOut);
        }

        // Testing with 'bsl::string_view' and default delimiters
        {
            const char        *str = "test \"default\" delimiters//garbage//";
            bsl::string_view   strView(str, 25);
            bsl::stringstream  stream;

            stream << bsl::quoted(strView);
            ASSERTV(stream.str().c_str(),
                    "\"test \\\"default\\\" delimiters\"" == stream.str());
        }

        // Testing with 'bsl::string' and custom delimiters
        {
            const char       *str = "test $custom$ delimiters//garbage//";
            const char        delimiter = '$';
            const char        escape    = '%';
            bsl::string       strView(str, 24);
            bsl::stringstream stream;

            stream << bsl::quoted(strView, delimiter, escape);
            ASSERTV(stream.str().c_str(),
                    "$test %$custom%$ delimiters$" == stream.str());
        }

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
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
