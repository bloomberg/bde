// balcl_commandline.t.cpp                                            -*-C++-*-
#include <balcl_commandline.h>

#include <balcl_constraint.h>
#include <balcl_option.h>
#include <balcl_optioninfo.h>
#include <balcl_optiontype.h>
#include <balcl_optionvalue.h>
#include <balcl_typeinfo.h>

#include <bdlb_tokenizer.h>

#include <bdls_filesystemutil.h>

#include <bdlma_localsequentialallocator.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_time.h>

#include <bdlb_printmethods.h>   // `bdlb::HasPrintMethod`

#include <bslim_testutil.h>

#include <bslmf_allocatorargt.h> // `bsl::allocator_arg`
#include <bslmf_assert.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bsls_asserttest.h>
#include <bsls_platform.h>
#include <bsls_review.h>
#include <bsls_types.h>     // `bsls::Types::Int64`

#include <bsl_algorithm.h>  // `bsl::fill`
#include <bsl_cstddef.h>    // `bsl::size_t`
#include <bsl_cstdlib.h>    // `::setenv`
#include <bsl_cstring.h>    // `bsl::strcmp`, `bsl::strspn`
#include <bsl_fstream.h>
#include <bsl_functional.h> // `bsl::function`
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_map.h>
#include <bsl_optional.h>
#include <bsl_ostream.h>    // `operator<<`
#include <bsl_sstream.h>
#include <bsl_stdexcept.h>
#include <bsl_streambuf.h>
#include <bsl_string.h>     // `bslstl::StringRef`
#include <bsl_utility.h>    // `bsl::pair`, `bsl::make_pair`
#include <bsl_vector.h>

#include <bsl_c_ctype.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
# include <windows.h>       // `SetEnvironmentVariable`
#endif

#ifdef BDE_BUILD_TARGET_ASAN
# ifdef BSLS_PLATFORM_CMP_GNU
#   pragma GCC diagnostic ignored "-Wlarger-than="
# endif
#endif

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// The class, `balcl::CommandLine`, has in-core value semantics and supports
// the `bslma::Allocator` memory model.  A slight complication is that there is
// an invalid state (when no value is defined) and a valid state (after `parse`
// returns successfully).  Nevertheless, we follow the usual value-semantic
// type test driver, simplified from the fact that there are no manipulators
// (once created and parsed, a `balcl::CommandLine` object does not change
// value).
//
// Primary Manipulators
//  - `CommandLine(const Oi *table, int len, *bA = 0);`
//  - `int parse(int argc, const char *const argv[], ostream& stream);`
//
// Basic Accessors
//  - `bool isParsed() const;`
//  - `bool isValid() const;`
//  - `bslma::Allocator *allocator() const;`
//
// The `parse` method is given a BOOTSTRAP test in TC 3.  Those tests, based on
// valid input, are sufficient to allow the method to be used in subsequent
// tests.  Thorough tests of the wide range of allowed input for `parse`
// appear in later test cases.  See TCs 12 to 17.
//
///Input Tables
///------------
// There are several input tables (defined at file scope) that are used
// throughout this test driver.  In particular, these files are the basis of
// generating option configurations by the `u::generateTestData` helper
// function.  The cross product of these tables covers the space of
// qualitatively equivalent inputs.  The tables are:
//
// 1. `OPTION_TYPEINFO`: This table represents the space of `TypeInfo`
//    attributes.  There are entries that cover the cross product of every
//    supported option type, linked variable (with and without), and constraint
//    (with and without, where allowed).
//
//    - A helper function, `u::createTypeInfo`, is defined to create test
//      arguments from table entries.
//
// 2. `OPTION_OCCURRENCES`: This table has an entry for each of the three
//    allowed values of `OccurrenceType`.
//
//    - A helper function, `u::createOccurrenceInfo`, is defined to create test
//      arguments from table entries.
//
// 3. `OPTION_VALUES`: This table provides an entry for each of the allowed
//    option types and the address of a value of that type.  These values are
//    available for general use in the test driver.  For example, they can be
//    used as default values for options when creating option-specification
//    tables.
//
//    - None of these option values correspond to the default value of their
//      respective types.
//
//    - The value chosen for `balcl::OptionType::e_STRING` is sufficiently long
//      to exceed the short-string optimization.
//
//    - A helper function, `u::getSomeOptionValue`, is provided to return the
//      address (a `void *`) of the value for a specified option type.
//
//    - A helper function, `u::setOptionValue`, is provided to convert the
//      "value" field of `OPTION_VALUES` into an argument for the
//      `setDefaultValue` method.
//
///Usage and Error Messages
///------------------------
// As a note, we made all the usage strings and error messages part of this
// test driver.  This may seem like an overkill and an annoyance (if the usage
// formatting changes, all the strings in the test driver will need to be
// changed too).  In fact, that is a *good* thing since it forces the
// implementer to manually verify that all these usage strings are properly
// aligned, human-readable, etc.: something that an automatic test driver
// cannot do.  Besides finding obvious errors in the documentation, of course.
//
// ----------------------------------------------------------------------------
// balcl::CommandLine
// ----------------------------------------------------------------------------
// CLASS METHODS
// [11] isValidOptionSpecificationTable(const Oi (&tbl)[L]);
// [11] isValidOptionSpecificationTable(      Oi (&tbl)[L]);
// [11] isValidOptionSpecificationTable(const Oi (&tbl)[L], ostream& s);
// [11] isValidOptionSpecificationTable(      Oi (&tbl)[L], ostream& s);
// [11] isValidOptionSpecificationTable(const Oi *tbl, int l);
// [11] isValidOptionSpecificationTable(const Oi *tbl, int l, ostream& s);
//
// CREATORS
// [ 8] CommandLine(const Oi (&table)[LEN], ostream& stream, *bA = 0);
// [ 8] CommandLine(      Oi (&table)[LEN], ostream& stream, *bA = 0);
// [ 8] CommandLine(const Oi (&table)[LEN], *bA = 0);
// [ 8] CommandLine(      Oi (&table)[LEN], *bA = 0);
// [ 3] CommandLine(const Oi *table, int len, *bA = 0);
// [ 8] CommandLine(const Oi *table, int len, ostream& stream, *bA = 0);
// [ 6] CommandLine(const CommandLine& original, *bA = 0);
// [ 3] ~CommandLine();
//
// MANIPULATORS
// [ 7] CommandLine& operator=(const CommandLine& rhs);
// [10] int parse(int argc, const char *const argv[]);
// [ 3] int parse(int argc, const char *const argv[], ostream& stream);
//
// ACCESSORS
// [ 9] bool hasOption(const bsl::string& name) const;
// [ 9] bool hasValue(const bsl::string& name) const;
// [ 3] bool isParsed() const;
// [ 9] bool isSpecified(const string& name) const;
// [ 9] bool isSpecified(const string& name, int *count) const;
// [ 3] bool isValid() const;
// [ 9] int numSpecified(const string& name) const;
// [ 9] CommandLineOptionsHandle options() const;
// [ 9] const vector<int>& positions(const string& name) const;
// [ 9] int position(const bsl::string& name) const;
// [ 9] CommandLineOptionsHandle specifiedOptions() const;
// [ 9] OptionType::Enum type(const bsl::string name) const;
//
// [ 9] bool theBool(const bsl::string& name) const;
// [ 9] char theChar(const bsl::string& name) const;
// [ 9] int theInt(const bsl::string& name) const;
// [ 9] Int64 theInt64(const bsl::string& name) const;
// [ 9] double theDouble(const bsl::string& name) const;
// [ 9] const string& theString(const string& name) const;
// [ 9] const Datetime& theDatetime(const string& name) const;
// [ 9] const Date& theDate(const string& name) const;
// [ 9] const Time& theTime(const string& name) const;
// [ 9] const vector<char>& theCharArray(const string& name) const;
// [ 9] const vector<int>& theIntArray(const string& name) const;
// [ 9] const vector<Int64>& theInt64Array(const string& name) const;
// [ 9] const vector<double>& theDoubleArray(const string& name) const;
// [ 9] const vector<string>& theStringArray(const string& name) const;
// [ 9] const vector<Datetime>& theDatetimeArray(const string& nom) const;
// [ 9] const vector<Date>& theDateArray(const string& name) const;
// [ 9] const vector<Time>& theTimeArray(const string& name) const;
//
// [18] void printUsage() const;
// [18] void printUsage(const string& pName);
// [18] void printUsage(bsl::ostream& stream) const;
// [18] void printUsage(bsl::ostream& stream, const string& pName) const;
//
// [ 3] bslma::Allocator *allocator() const;
// [ 4] ostream& print(ostream& stream, int level = 0, int spl = 4) const;
//
// FREE OPERATORS
// [ 5] bool operator==(const CommandLine& lhs, const CommandLine& rhs);
// [ 5] bool operator!=(const CommandLine& lhs, const CommandLine& rhs);
// [ 4] ostream& operator<<(ostream& stream, const CommandLine& rhs);
//
// ----------------------------------------------------------------------------
// balcl::CommandLineOptionsHandle
// ----------------------------------------------------------------------------
// CREATORS
// [ 9] ~CommandLineOptionsHandle();
//
// ACCESSORS
// [ 9] int index(const char *name) const;
// [ 9] const char *name(size_t index) const;
// [ 9] size_t numOptions() const;
// [ 9] OptionType::Enum type(size_t index) const;
// [ 9] OptionType::Enum type(const char *name) const;
// [ 9] const OptionValue& value(size_t index) const;
// [ 9] const OptionValue& value(const char *name) const;
//
// [ 9] template <class TYPE> const TYPE& the(size_t index) const;
// [ 9] template <class TYPE> const TYPE& the(const char *name) const;
// [ 9] bool theBool(const char *name) const;
// [ 9] char theChar(const char *name) const;
// [ 9] int theInt(const char *name) const;
// [ 9] Int64 theInt64(const char *name) const;
// [ 9] double theDouble(const char *name) const;
// [ 9] const string& theString(const char *name) const;
// [ 9] const Datetime& theDatetime(const char *name) const;
// [ 9] const Date& theDate(const char *name) const;
// [ 9] const Time& theTime(const char *name) const;
// [ 9] const vector<char>& theCharArray(const char *name) const;
// [ 9] const vector<int>& theIntArray(const char *name) const;
// [ 9] const vector<Int64>& theInt64Array(const char *name)
// [ 9] const vector<double>& theDoubleArray(const char *name) const;
// [ 9] const vector<string>& theStringArray(const char *name) const;
// [ 9] const vector<Datetime>& theDatetimeArray(const char *name)
// [ 9] const vector<Date>& theDateArray(const char *name) const;
// [ 9] const vector<Time>& theTimeArray(const char *name) const;
//
// FREE OPERATORS
// [ 5] bool operator==(const CommandLineOptionsHandle& lhs, rhs);
// [ 5] bool operator!=(const CommandLineOptionsHandle& lhs, rhs);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] TESTING `u::parseCommandLine` TESTING UTILITY
// [11] TESTING INVALID OPTION SPECS
// [12] TESTING FLAGS BUNDLING
// [13] TESTING MULTIPLY-SPECIFIED FLAGS
// [14] TESTING ORDER OF ARGUMENTS
// [15] TESTING PARSING OF STRINGS
// [16] TESTING NON-OPTION TOGGLE `--`
// [19] TESTING OPTIONAL LINKED VARIABLES
// [ *] CONCERN: The global allocator is not used.
// [21] CONCERN: DRQS 166843299
// [17] TESTING ABILITY TO INPUT VALUE FOR FLAG
// [22] CONCERN: READING ENVIRONMENT VARIABLES
// [23] PARSING ARRAY ENVIRONMENT VARIABLES
// [24] Environment Variables Overriden by Command Line, Always Checked
// [25] INITIALIZATION OF FIELDS WITH EMPTY CURLY BRACES
// [26] USAGE EXAMPLE 1
// [27] USAGE EXAMPLE 3
// [28] USAGE EXAMPLE 4
// [29] USAGE EXAMPLE 5

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
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
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

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)

#if defined(BDE_BUILD_TARGET_EXC) && defined(BSLS_REVIEW_OPT_IS_ACTIVE)

#define U_REVIEW_THROW_GUARD                                                  \
    bsls::ReviewFailureHandlerGuard u_review_guard(bsls::Review::failByThrow)

enum { k_REVIEW_FAIL_IS_ENABLED = true };
# define U_REVIEW_FAIL(expr)                                                  \
    do {                                                                      \
        bool u_review_caught = false;                                         \
        try {                                                                 \
            expr;                                                             \
        } catch (const bsls::AssertTestException&) {                          \
            u_review_caught = true;                                           \
        }                                                                     \
        ASSERT(u_review_caught);                                              \
    } while(false)
# define U_REVIEW_PASS(expr)                                                  \
    do {                                                                      \
        bool u_review_caught = false;                                         \
        try {                                                                 \
            expr;                                                             \
        } catch (const bsls::AssertTestException&) {                          \
            u_review_caught = true;                                           \
        }                                                                     \
        ASSERT(!u_review_caught);                                             \
    } while(false)
#else

#define U_REVIEW_THROW_GUARD                                                  \
    bsls::ReviewFailureHandlerGuard u_review_guard(bsls::Review::failByAbort)

enum { k_REVIEW_FAIL_IS_ENABLED = false };
# define U_REVIEW_FAIL(expr)
# define U_REVIEW_PASS(expr)   expr
#endif

// ============================================================================
//                      GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef balcl::CommandLine                Obj;

typedef balcl::Constraint                 Constraint;
typedef balcl::OccurrenceInfo             OccurrenceInfo;
typedef OccurrenceInfo::OccurrenceType    OccurrenceType;
typedef balcl::Option                     Option;
typedef balcl::OptionInfo                 OptionInfo;
typedef balcl::TypeInfo                   TypeInfo;
typedef balcl::OptionValue                OptionValue;
typedef balcl::CommandLineOptionsHandle   OptionsHandle;

typedef balcl::OptionType                 Ot;
typedef balcl::OptionType::Enum           ElemType;

typedef bsls::Types::Int64                Int64;

// ============================================================================
//                                TYPE TRAITS
// ----------------------------------------------------------------------------

BSLMF_ASSERT(bslma::UsesBslmaAllocator<Obj>::value);
BSLMF_ASSERT(bdlb::HasPrintMethod<Obj>::value);

// ============================================================================
//          GLOBAL CONSTANTS/VARIABLES/FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

static int             verbose = false;
static int         veryVerbose = false;
static int     veryVeryVerbose = false;
static int veryVeryVeryVerbose = false;

enum { k_DATETIME_FIELD_WIDTH = 25
     ,     k_DATE_FIELD_WIDTH =  9
     ,     k_TIME_FIELD_WIDTH = 15 };

// ATTRIBUTES FOR `balcl::Option`
static const struct {
    int         d_line;   // line number
    const char *d_tag_p;  // tag attribute
} OPTION_TAGS[] = {
    { L_, "" }
  , { L_, "a|aa" }
  , { L_, "b|bb" }
  , { L_, "cdefghijklmab" }  // note: only first four tags used in case 8
  , { L_, "a|aaabb" }
  , { L_, "b|bbbbb" }
  , { L_, "nopqrstuvwxyz" }
};
enum { NUM_OPTION_TAGS = sizeof OPTION_TAGS / sizeof *OPTION_TAGS };

bslma::TestAllocator ga("Global");

// ATTRIBUTES FOR `balcl::TypeInfo`
bool                          linkedBool;
char                          linkedChar;
int                           linkedInt;
Int64                         linkedInt64;
double                        linkedDouble;
bsl::string                   linkedString       (&ga);
bdlt::Datetime                linkedDatetime;
bdlt::Date                    linkedDate;
bdlt::Time                    linkedTime;
bsl::vector<char>             linkedCharArray    (&ga);
bsl::vector<int>              linkedIntArray     (&ga);
bsl::vector<Int64>            linkedInt64Array   (&ga);
bsl::vector<double>           linkedDoubleArray  (&ga);
bsl::vector<bsl::string>      linkedStringArray  (&ga);
bsl::vector<bdlt::Datetime>   linkedDatetimeArray(&ga);
bsl::vector<bdlt::Date>       linkedDateArray    (&ga);
bsl::vector<bdlt::Time>       linkedTimeArray    (&ga);

bsl::optional<char>           oLinkedChar;
bsl::optional<int>            oLinkedInt;
bsl::optional<Int64>          oLinkedInt64;
bsl::optional<double>         oLinkedDouble;
bsl::optional<bsl::string>    oLinkedString(bsl::allocator_arg, &ga);
bsl::optional<bdlt::Datetime> oLinkedDatetime;
bsl::optional<bdlt::Date>     oLinkedDate;
bsl::optional<bdlt::Time>     oLinkedTime;

namespace {
namespace u {

void setEnvironmentVariable(const char *envVarName, const char *envVarValue)
{
#ifdef BSLS_PLATFORM_OS_UNIX
    int rc = ::setenv(envVarName, envVarValue, 1);
    BSLS_ASSERT_OPT(0 == rc);
#else
    int rc = ::SetEnvironmentVariable(envVarName, 0);
    BSLS_ASSERT_OPT(0 != rc);
    rc =     ::SetEnvironmentVariable(envVarName, envVarValue);
    BSLS_ASSERT_OPT(0 != rc);
#endif
    (void) rc;
}

void unsetEnvironmentVariable(const char *envVarName)
{
#ifdef BSLS_PLATFORM_OS_UNIX
    int rc = ::unsetenv(envVarName);
    BSLS_ASSERT_OPT(0 == rc);
#else
    int rc = ::SetEnvironmentVariable(envVarName, 0);
    BSLS_ASSERT_OPT(0 != rc);
#endif
    (void) rc;
}

}  // close namespace u
}  // close unnamed namespace

                        // =====================
                        // struct TestConstraint
                        // =====================

// BDE_VERIFY pragma: -FABC01  // Function ... not in alphabetic order

/// This `struct` provides a namespace for functions, one for each
/// constraint type, used to initialize `Constraint` objects for testing.
struct TestConstraint {

  private:
    /// Return `s_constraintValue` and if `false == s_constraintValue`
    /// output an error message to the specified `stream`.
    static bool commonLogic(bsl::ostream& stream);

  public:
    // PUBLIC CLASS DATA

    /// Global return value (for easier control).
    static bool s_constraintValue;

    static bool     charFunc(const char           *, bsl::ostream& stream);
    static bool      intFunc(const int            *, bsl::ostream& stream);
    static bool    int64Func(const Int64          *, bsl::ostream& stream);
    static bool   doubleFunc(const double         *, bsl::ostream& stream);
    static bool   stringFunc(const bsl::string    *, bsl::ostream& stream);
    static bool datetimeFunc(const bdlt::Datetime *, bsl::ostream& stream);
    static bool     dateFunc(const bdlt::Date     *, bsl::ostream& stream);

    /// Return `s_constraintValue` and if `false == s_constraintValue`
    /// output an error message to the specified `stream`.  Note that the
    /// first argument is ignored.
    static bool     timeFunc(const bdlt::Time     *, bsl::ostream& stream);
};

                        // ---------------------
                        // struct TestConstraint
                        // ---------------------

bool TestConstraint::commonLogic(bsl::ostream& stream)
{
    if (!s_constraintValue) {
        stream << "Constraint Functor: error message.\n"  << flush;
    }
    return s_constraintValue;
}

// PUBLIC CLASS DATA
bool TestConstraint::s_constraintValue = true;

// CLASS METHODS
bool TestConstraint::charFunc(const char *, bsl::ostream& stream)
{
    return commonLogic(stream);
}

bool TestConstraint::intFunc(const int *, bsl::ostream& stream)
{
    return commonLogic(stream);
}

bool TestConstraint::int64Func(const Int64 *, bsl::ostream& stream)
{
    return commonLogic(stream);
}

bool TestConstraint::doubleFunc(const double *, bsl::ostream& stream)
{
    return commonLogic(stream);
}

bool TestConstraint::stringFunc(const bsl::string *, bsl::ostream& stream)
{
    return commonLogic(stream);
}

bool TestConstraint::datetimeFunc(const bdlt::Datetime *, bsl::ostream& stream)
{
    return commonLogic(stream);
}

bool TestConstraint::dateFunc(const bdlt::Date *, bsl::ostream& stream)
{
    return commonLogic(stream);
}

bool TestConstraint::timeFunc(const bdlt::Time *, bsl::ostream& stream)
{
    return commonLogic(stream);
}

// BDE_VERIFY pragma: +FABC01  // Function ... not in alphabetic order

#define TC TestConstraint

Constraint::    CharConstraint     testCharConstraint(&TC::    charFunc);
Constraint::     IntConstraint      testIntConstraint(&TC::     intFunc);
Constraint::   Int64Constraint    testInt64Constraint(&TC::   int64Func);
Constraint::  DoubleConstraint   testDoubleConstraint(&TC::  doubleFunc);
Constraint::  StringConstraint   testStringConstraint(&TC::  stringFunc);
Constraint::DatetimeConstraint testDatetimeConstraint(&TC::datetimeFunc);
Constraint::    DateConstraint     testDateConstraint(&TC::    dateFunc);
Constraint::    TimeConstraint     testTimeConstraint(&TC::    timeFunc);

#undef TC  // TestConstraint

const struct {
    int       d_line;              // line number
    ElemType  d_type;              // option type
    void     *d_linkedVariable_p;  // linked variable attribute(s)
    void     *d_constraint_p;      // linked variable attribute(s)
} OPTION_TYPEINFO[] = {
   { L_, Ot::e_BOOL,           0,                    0                       }
 , { L_, Ot::e_CHAR,           0,                    0                       }
 , { L_, Ot::e_INT,            0,                    0                       }
 , { L_, Ot::e_INT64,          0,                    0                       }
 , { L_, Ot::e_DOUBLE,         0,                    0                       }
 , { L_, Ot::e_STRING,         0,                    0                       }
 , { L_, Ot::e_DATETIME,       0,                    0                       }
 , { L_, Ot::e_DATE,           0,                    0                       }
 , { L_, Ot::e_TIME,           0,                    0                       }
 , { L_, Ot::e_CHAR_ARRAY,     0,                    0                       }
 , { L_, Ot::e_INT_ARRAY,      0,                    0                       }
 , { L_, Ot::e_INT64_ARRAY,    0,                    0                       }
 , { L_, Ot::e_DOUBLE_ARRAY,   0,                    0                       }
 , { L_, Ot::e_STRING_ARRAY,   0,                    0                       }
 , { L_, Ot::e_DATETIME_ARRAY, 0,                    0                       }
 , { L_, Ot::e_DATE_ARRAY,     0,                    0                       }
 , { L_, Ot::e_TIME_ARRAY,     0,                    0                       }
 , { L_, Ot::e_BOOL,           &linkedBool,          0                       }
 , { L_, Ot::e_CHAR,           &linkedChar,          0                       }
 , { L_, Ot::e_INT,            &linkedInt,           0                       }
 , { L_, Ot::e_INT64,          &linkedInt64,         0                       }
 , { L_, Ot::e_DOUBLE,         &linkedDouble,        0                       }
 , { L_, Ot::e_STRING,         &linkedString,        0                       }
 , { L_, Ot::e_DATETIME,       &linkedDatetime,      0                       }
 , { L_, Ot::e_DATE,           &linkedDate,          0                       }
 , { L_, Ot::e_TIME,           &linkedTime,          0                       }
 , { L_, Ot::e_CHAR_ARRAY,     &linkedCharArray,     0                       }
 , { L_, Ot::e_INT_ARRAY,      &linkedIntArray,      0                       }
 , { L_, Ot::e_INT64_ARRAY,    &linkedInt64Array,    0                       }
 , { L_, Ot::e_DOUBLE_ARRAY,   &linkedDoubleArray,   0                       }
 , { L_, Ot::e_STRING_ARRAY,   &linkedStringArray,   0                       }
 , { L_, Ot::e_DATETIME_ARRAY, &linkedDatetimeArray, 0                       }
 , { L_, Ot::e_DATE_ARRAY,     &linkedDateArray,     0                       }
 , { L_, Ot::e_TIME_ARRAY,     &linkedTimeArray,     0                       }
 , { L_, Ot::e_CHAR,           &oLinkedChar,         0                       }
 , { L_, Ot::e_INT,            &oLinkedInt,          0                       }
 , { L_, Ot::e_INT64,          &oLinkedInt64,        0                       }
 , { L_, Ot::e_DOUBLE,         &oLinkedDouble,       0                       }
 , { L_, Ot::e_STRING,         &oLinkedString,       0                       }
 , { L_, Ot::e_DATETIME,       &oLinkedDatetime,     0                       }
 , { L_, Ot::e_DATE,           &oLinkedDate,         0                       }
 , { L_, Ot::e_TIME,           &oLinkedTime,         0                       }
 , { L_, Ot::e_BOOL,           0,                    0                       }
 , { L_, Ot::e_CHAR,           0,                    &testCharConstraint     }
 , { L_, Ot::e_INT,            0,                    &testIntConstraint      }
 , { L_, Ot::e_INT64,          0,                    &testInt64Constraint    }
 , { L_, Ot::e_DOUBLE,         0,                    &testDoubleConstraint   }
 , { L_, Ot::e_STRING,         0,                    &testStringConstraint   }
 , { L_, Ot::e_DATETIME,       0,                    &testDatetimeConstraint }
 , { L_, Ot::e_DATE,           0,                    &testDateConstraint     }
 , { L_, Ot::e_TIME,           0,                    &testTimeConstraint     }
 , { L_, Ot::e_CHAR_ARRAY,     0,                    &testCharConstraint     }
 , { L_, Ot::e_INT_ARRAY,      0,                    &testIntConstraint      }
 , { L_, Ot::e_INT64_ARRAY,    0,                    &testInt64Constraint    }
 , { L_, Ot::e_DOUBLE_ARRAY,   0,                    &testDoubleConstraint   }
 , { L_, Ot::e_STRING_ARRAY,   0,                    &testStringConstraint   }
 , { L_, Ot::e_DATETIME_ARRAY, 0,                    &testDatetimeConstraint }
 , { L_, Ot::e_DATE_ARRAY,     0,                    &testDateConstraint     }
 , { L_, Ot::e_TIME_ARRAY,     0,                    &testTimeConstraint     }
 , { L_, Ot::e_BOOL,           &linkedBool,          0                       }
 , { L_, Ot::e_CHAR,           &linkedChar,          &testCharConstraint     }
 , { L_, Ot::e_INT,            &linkedInt,           &testIntConstraint      }
 , { L_, Ot::e_INT64,          &linkedInt64,         &testInt64Constraint    }
 , { L_, Ot::e_DOUBLE,         &linkedDouble,        &testDoubleConstraint   }
 , { L_, Ot::e_STRING,         &linkedString,        &testStringConstraint   }
 , { L_, Ot::e_DATETIME,       &linkedDatetime,      &testDatetimeConstraint }
 , { L_, Ot::e_DATE,           &linkedDate,          &testDateConstraint     }
 , { L_, Ot::e_TIME,           &linkedTime,          &testTimeConstraint     }
 , { L_, Ot::e_CHAR_ARRAY,     &linkedCharArray,     &testCharConstraint     }
 , { L_, Ot::e_INT_ARRAY,      &linkedIntArray,      &testIntConstraint      }
 , { L_, Ot::e_INT64_ARRAY,    &linkedInt64Array,    &testInt64Constraint    }
 , { L_, Ot::e_DOUBLE_ARRAY,   &linkedDoubleArray,   &testDoubleConstraint   }
 , { L_, Ot::e_STRING_ARRAY,   &linkedStringArray,   &testStringConstraint   }
 , { L_, Ot::e_DATETIME_ARRAY, &linkedDatetimeArray, &testDatetimeConstraint }
 , { L_, Ot::e_DATE_ARRAY,     &linkedDateArray,     &testDateConstraint     }
 , { L_, Ot::e_TIME_ARRAY,     &linkedTimeArray,     &testTimeConstraint     }
 , { L_, Ot::e_CHAR,           &oLinkedChar,         &testCharConstraint     }
 , { L_, Ot::e_INT,            &oLinkedInt,          &testIntConstraint      }
 , { L_, Ot::e_INT64,          &oLinkedInt64,        &testInt64Constraint    }
 , { L_, Ot::e_DOUBLE,         &oLinkedDouble,       &testDoubleConstraint   }
 , { L_, Ot::e_STRING,         &oLinkedString,       &testStringConstraint   }
 , { L_, Ot::e_DATETIME,       &oLinkedDatetime,     &testDatetimeConstraint }
 , { L_, Ot::e_DATE,           &oLinkedDate,         &testDateConstraint     }
 , { L_, Ot::e_TIME,           &oLinkedTime,         &testTimeConstraint     }
};
enum { NUM_OPTION_TYPEINFO = sizeof  OPTION_TYPEINFO
                           / sizeof *OPTION_TYPEINFO };

// ATTRIBUTES FOR `balcl::OccurrenceInfo`
static const struct {
    int             d_line;  // line number
    OccurrenceType  d_type;  // name attribute
} OPTION_OCCURRENCES[] = {
    { L_, OccurrenceInfo::e_REQUIRED }
  , { L_, OccurrenceInfo::e_OPTIONAL }
  , { L_, OccurrenceInfo::e_HIDDEN   }
};
enum { NUM_OPTION_OCCURRENCES = sizeof  OPTION_OCCURRENCES
                              / sizeof *OPTION_OCCURRENCES };

#ifdef BSLS_PLATFORM_CPU_32_BIT
#define SUFFICIENTLY_LONG_STRING "123456789012345678901234567890123"
#else  // 64_BIT
#define SUFFICIENTLY_LONG_STRING "12345678901234567890123456789012" \
                                 "123456789012345678901234567890123"
#endif
BSLMF_ASSERT(sizeof SUFFICIENTLY_LONG_STRING > sizeof(bsl::string));

bool                        valueBool          = true;
char                        valueChar          = 'D';
int                         valueInt           = 1234567;
Int64                       valueInt64         = 123456789LL;
double                      valueDouble        = 0.015625;  // 1/64
bsl::string                 valueString(SUFFICIENTLY_LONG_STRING, &ga);
bdlt::Datetime              valueDatetime(1234, 12, 3, 4, 5, 6);
bdlt::Date                  valueDate(1234, 4, 6);
bdlt::Time                  valueTime(7, 8, 9, 10);
bsl::vector<char>           valueCharArray    (1, valueChar,      &ga);
bsl::vector<int>            valueIntArray     (1, valueInt,       &ga);
bsl::vector<Int64>          valueInt64Array   (1, valueInt64,     &ga);
bsl::vector<double>         valueDoubleArray  (1, valueDouble,    &ga);
bsl::vector<bsl::string>    valueStringArray  (1, valueString,    &ga);
bsl::vector<bdlt::Datetime> valueDatetimeArray(1, valueDatetime,  &ga);
bsl::vector<bdlt::Date>     valueDateArray    (1, valueDate,      &ga);
bsl::vector<bdlt::Time>     valueTimeArray    (1, valueTime,      &ga);

static const struct {
    int             d_line;     // line number
    ElemType        d_type;     // option type
    const void     *d_value_p;  // value attribute(s)
} OPTION_VALUES[] = {
    { L_, Ot::e_BOOL,            &valueBool          }
  , { L_, Ot::e_CHAR,            &valueChar          }
  , { L_, Ot::e_INT,             &valueInt           }
  , { L_, Ot::e_INT64,           &valueInt64         }
  , { L_, Ot::e_DOUBLE,          &valueDouble        }
  , { L_, Ot::e_STRING,          &valueString        }
  , { L_, Ot::e_DATETIME,        &valueDatetime      }
  , { L_, Ot::e_DATE,            &valueDate          }
  , { L_, Ot::e_TIME,            &valueTime          }
  , { L_, Ot::e_CHAR_ARRAY,      &valueCharArray     }
  , { L_, Ot::e_INT_ARRAY,       &valueIntArray      }
  , { L_, Ot::e_INT64_ARRAY,     &valueInt64Array    }
  , { L_, Ot::e_DOUBLE_ARRAY,    &valueDoubleArray   }
  , { L_, Ot::e_STRING_ARRAY,    &valueStringArray   }
  , { L_, Ot::e_DATETIME_ARRAY,  &valueDatetimeArray }
  , { L_, Ot::e_DATE_ARRAY,      &valueDateArray     }
  , { L_, Ot::e_TIME_ARRAY,      &valueTimeArray     }
};
enum { NUM_OPTION_VALUES = sizeof  OPTION_VALUES / sizeof *OPTION_VALUES };

bool                     linkedBoolA;
bool                     linkedBoolB;
bool                     linkedBoolC;
bsl::string              linkedString1     (&ga);
bsl::vector<bsl::string> linkedStringArray1(&ga);

const int MAX_SPEC_SIZE = 12;
static const struct {
    int        d_line;
    int        d_numSpecTable;
    OptionInfo d_specTable[MAX_SPEC_SIZE];
} SPECS[] = {
    { L_, 11, {  // Spec #1 (index 0)
                 {
                     "a|aLong",
                     "Name_a",
                     "Description for a",
                     TypeInfo(&linkedBoolA),
                     OccurrenceInfo::e_OPTIONAL,
                     ""
                 },
                 {
                     "b|bLong",
                     "Name_b",
                     "Description for b",
                     TypeInfo(&linkedBoolB),
                     OccurrenceInfo::e_OPTIONAL,
                     ""
                 },
                 {
                     "c|cLong",
                     "Name_c",
                     "Description for c",
                     TypeInfo(&linkedBoolC),
                     OccurrenceInfo::e_OPTIONAL,
                     ""
                 },
                 {
                     "i|iLong",
                     "Name_i",
                     "Description for i",
                     TypeInfo(&linkedInt),
                     OccurrenceInfo::e_OPTIONAL,
                     ""
                 },
                 {
                     "d|dLong",
                     "Name_d",
                     "Description for d",
                     TypeInfo(&linkedDouble),
                     OccurrenceInfo::e_OPTIONAL,
                     ""
                 },
                 {
                     "s|sLong",
                     "Name_s",
                     "Description for s",
                     TypeInfo(&linkedString),
                     OccurrenceInfo::e_OPTIONAL,
                     ""
                 },
                 {
                     "I|ILong",
                     "Name_I",
                     "Description for I",
                     TypeInfo(&linkedIntArray),
                     OccurrenceInfo::e_OPTIONAL,
                     ""
                 },
                 {
                     "D|DLong",
                     "Name_D",
                     "Description for D",
                     TypeInfo(&linkedDoubleArray),
                     OccurrenceInfo::e_OPTIONAL,
                     ""
                 },
                 {
                     "S|SLong",
                     "Name_S",
                     "Description for S",
                     TypeInfo(&linkedStringArray),
                     OccurrenceInfo::e_OPTIONAL,
                     ""
                 },
                 {
                     "",
                     "Name1",
                     "Description1",
                     TypeInfo(&linkedString1),
                     OccurrenceInfo::e_OPTIONAL,
                     ""
                 },
                 {
                     "",
                     "Name2",
                     "Description2",
                     TypeInfo(&linkedStringArray1),
                     OccurrenceInfo::e_OPTIONAL,
                     ""
                 }
             }
    }
};
enum { NUM_SPECS = sizeof SPECS / sizeof *SPECS };

// ============================================================================
//                          HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

namespace {
namespace u {

/// Return the number of arguments (non-null pointers) found starting at
/// the specified `argv`.  The behavior is undefined unless `0 != argv[0]`.
int getArgc(const char * const *argv)
{
    ASSERT(argv);
    ASSERT(argv[0])

    int argc = 0;

    while (argv[++argc]) {
        ;
    }

    return argc;
}

                         // =========================
                         // function generateArgument
                         // =========================

/// Generate into the specified `argString` a command-line string suitable
/// to be parsed by a `balcl::CommandLine` object having the specified
/// `optionInfo`.  Optionally specify a `seed` for changing the return value
/// of `argString` in a pseudo-random fashion.  Return 0 upon success, and
/// non-zero otherwise.
int generateArgument(bsl::string       *argString,
                     const OptionInfo&  optionInfo,
                     int                seed = 0)
{
    const OccurrenceInfo& occurrenceInfo = optionInfo.d_defaultInfo;
    Option                option(optionInfo);

    if (OccurrenceInfo::e_REQUIRED == occurrenceInfo.occurrenceType()
     || 0 != seed % 3)
    {
        bsl::string optionFlag;
        if (OptionInfo::e_NON_OPTION != option.argType()) {
            if (1 == seed % 3 && option.shortTag()) {
                optionFlag = bsl::string("-") + option.shortTag();
            } else {
                const char         *longTag = option.longTag();
                bsl::ostringstream  oss;
                ASSERT(true == option.isLongTagValid(longTag, oss));

                optionFlag = bsl::string("--") + longTag;
            }
        }
        char separator = ' ';
        if (3 < seed % 7) {
            separator = '=';
        }
        switch (optionInfo.d_typeInfo.type()) {
          case Ot::e_BOOL: {
            *argString = optionFlag;
          } break;
          case Ot::e_CHAR: {
            *argString = optionFlag + separator + "a";
          } break;
          case Ot::e_INT: {
            *argString = optionFlag + separator + "123654";
          } break;
          case Ot::e_INT64: {
            *argString = optionFlag + separator + "987654321";
          } break;
          case Ot::e_DOUBLE: {
            *argString = optionFlag + separator + "0.376739501953125";
          } break;
          case Ot::e_STRING: {
            *argString = optionFlag + separator + "someString";
          } break;
          case Ot::e_DATETIME: {
            *argString = optionFlag + separator + "2008-07-22T04:06:08";
          } break;
          case Ot::e_DATE: {
            *argString = optionFlag + separator + "2007-08-22";
          } break;
          case Ot::e_TIME: {
            *argString = optionFlag + separator + "08:06:04";
          } break;
          case Ot::e_CHAR_ARRAY: {
            *argString  = optionFlag + separator + "a";
            *argString += ' ';
            *argString += optionFlag + separator + "z";
          } break;
          case Ot::e_INT_ARRAY: {
            *argString  = optionFlag + separator + "123654";
            *argString += ' ';
            *argString += optionFlag + separator + "654321";
          } break;
          case Ot::e_INT64_ARRAY: {
            *argString  = optionFlag + separator + "987654321";
            *argString += ' ';
            *argString += optionFlag + separator + "192837465";
          } break;
          case Ot::e_DOUBLE_ARRAY: {
            *argString  = optionFlag + separator + "0.376739501953125";
            *argString += ' ';
            *argString += optionFlag + separator + "0.623260498046875";
          } break;
          case Ot::e_STRING_ARRAY: {
            *argString  = optionFlag + separator + "someString";
            *argString += ' ';
            *argString += optionFlag + separator + "someOtherString";
          } break;
          case Ot::e_DATETIME_ARRAY: {
            *argString  = optionFlag + separator + "2008-07-22T04:06:08";
            *argString += ' ';
            *argString += optionFlag + separator + "2006-05-11T02:04:06";
          } break;
          case Ot::e_DATE_ARRAY: {
            *argString  = optionFlag + separator + "2007-08-22";
            *argString += ' ';
            *argString += optionFlag + separator + "2005-06-11";
          } break;
          case Ot::e_TIME_ARRAY: {
            *argString  = optionFlag + separator + "08:06:04";
            *argString += ' ';
            *argString += optionFlag + separator + "02:04:06";
          } break;
          default: {
            ASSERTV("Shouldn't be reached", 0);
          } break;
        }
    }

    // Assert that required arguments must be provided.

    ASSERT(OccurrenceInfo::e_REQUIRED != occurrenceInfo.occurrenceType()
                                                       || !argString->empty());
    return 0;
}

                         // =========================
                         // function parseCommandLine
                         // =========================

const int MAX_ARGS = 512;

/// Parse the specified modifiable `cmdLine` as would a Unix shell, by
/// replacing every space by a character '\0' and recording the beginning of
/// each field into an array that is loaded into the specified `argv`, and
/// load the number of fields into the specified modifiable `argc`.
/// Optionally specify `maxArgs`, the maximum allowed number of arguments.
/// Return 0 on success, non-zero if the command line exceeds `maxArgs`
/// number of arguments, or on failure.  Note that the `cmdLine` must be
/// null-terminated and may not contain a carriage return or newline.
/// Fields must be separated by either a space or a tab character.
int parseCommandLine(char       *cmdLine,
                     int&        argc,
                     const char *argv[],
                     int         maxArgs = MAX_ARGS)
{
    // If passing null string (not even empty), failure.

    if (!cmdLine) {
        return -1;                                                    // RETURN
    }

    // Initialize `argc`.

    argc = 0;

    // Parse command line.

    while (1) {
        // Skip spaces if any.

        while (' ' == *cmdLine || '\t' == *cmdLine) {           // SPACE or TAB
            ++cmdLine;
        }

        // Check for end-of-string.

        if (!*cmdLine) { // null-terminating character           // End-of-line
            return 0;    // we're done                                // RETURN
        }
        if (0xa == *cmdLine || 0xd == *cmdLine) {               // '\n' or '\r'
            return -1;    // FAILURE                                  // RETURN
        }

        // Check overflow.

        if (argc >= maxArgs) {
            return -1;                                                // RETURN
        }

        // Not a space, newline, CR, or null-terminating character.  Mark
        // beginning of field.

        argv[argc] = cmdLine;
        ++argc;

        // Find the end of this field.

        while (' ' != *cmdLine && '\t' != *cmdLine) {
            if (!*cmdLine) { // null-terminating character
                return 0;    // we're done                            // RETURN
            }
            if (0xa == *cmdLine || 0xd == *cmdLine) {
                return -1;    // FAILURE                              // RETURN
            }
            ++cmdLine;
        }

        // And mark it.

        *cmdLine = 0; // mark the end of this field
        ++cmdLine; // continue to next field
    }

    return 0; // never reached
}

// BDE_VERIFY pragma: +FABC01  // Function ... not in alphabetic order

                        // ======================
                        // function setConstraint
                        // ======================

/// Set the constraint of the specified `typeInfo` to the function at the
/// specified `address` of the signature corresponding to the specified
/// `type`.  The behavior is undefined unless `Ot::e_VOID != type` and
/// `Ot::e_BOOL != type`.
void setConstraint(TypeInfo *typeInfo, ElemType type, const void *address)
{
    BSLS_ASSERT(typeInfo);
    BSLS_ASSERT(Ot::e_VOID != type);
    BSLS_ASSERT(Ot::e_BOOL != type);
    BSLS_ASSERT(address);

#define CASE(ENUM, CONSTRAINT)                                                \
    case ENUM:                                                                \
    case ENUM##_ARRAY: {                                                      \
      typeInfo->setConstraint(                                                \
                      *static_cast<const Constraint::CONSTRAINT *>(address)); \
    } break;                                                                  \

    switch (type) {
      case Ot::e_VOID: {
        ASSERTV("Shouldn't be reached", 0);
      } break;
      case Ot::e_BOOL: {
        ASSERTV("Shouldn't be reached", 0);
      } break;

      CASE(Ot::e_CHAR,         CharConstraint)
      CASE(Ot::e_INT,           IntConstraint)
      CASE(Ot::e_INT64,       Int64Constraint)
      CASE(Ot::e_DOUBLE,     DoubleConstraint)
      CASE(Ot::e_STRING,     StringConstraint)
      CASE(Ot::e_DATETIME, DatetimeConstraint)
      CASE(Ot::e_DATE,         DateConstraint)
      CASE(Ot::e_TIME,         TimeConstraint)

      default: {
        BSLS_ASSERT_INVOKE_NORETURN("Shouldn't be reached");
      } break;
    }

#undef CASE

}

                         // ==========================
                         // function setLinkedVariable
                         // ==========================

/// Invoke the `setLinkedVariable` overload of `balcl::TypeInfo` that
/// accepts addresses of type `Ot::EnumToType<type>::type *` with the
/// specified `variable` (address).  If the specified
/// `isOptionalLinkedVariable` is `true`, then invoke the overload that
/// accepts addresses of type `bsl::optional<Ot::EnumToType<type>::type> *`.
/// The behavior is undefined unless `variable` can be legally cast to the
/// target type.  Note that `isOptionalLinkedVariable` is *disallowed* when
/// `type` is an "array" option type or `type` is `Ot::e_BOOL`.
void setLinkedVariable(TypeInfo *typeInfo,
                       ElemType  type,
                       void     *variable,
                       bool      isOptionalLinkedVariable)
{
    ASSERT(typeInfo);

#define CASE(ENUM)                                                            \
    case ENUM: {                                                              \
      typedef Ot::EnumToType<ENUM>::type      LinkType;                       \
      typeInfo->setLinkedVariable(static_cast<LinkType *>(variable));         \
    } break;                                                                  \

#define CASE_MAYBE_OPTIONAL_LINK(ENUM)                                        \
    case ENUM: {                                                              \
      typedef Ot::EnumToType<ENUM>::type LinkType;                            \
                                                                              \
      if (isOptionalLinkedVariable) {                                         \
        BSLS_ASSERT(Ot::e_BOOL != type);                                      \
        BSLS_ASSERT(false      == Ot::isArrayType(type));                     \
                                                                              \
        typedef bsl::optional<LinkType> OptLinkType;                          \
                                                                              \
        typeInfo->setLinkedVariable(static_cast<OptLinkType *>(variable));    \
      } else {                                                                \
        typeInfo->setLinkedVariable(static_cast<   LinkType *>(variable));    \
      }                                                                       \
    } break;                                                                  \

    switch (type) {
      case Ot::e_VOID: {
        BSLS_ASSERT_INVOKE_NORETURN("Reached: `e_VOID`");
      } break;

      CASE                    (Ot::e_BOOL)
      CASE_MAYBE_OPTIONAL_LINK(Ot::e_CHAR)
      CASE_MAYBE_OPTIONAL_LINK(Ot::e_INT)
      CASE_MAYBE_OPTIONAL_LINK(Ot::e_INT64)
      CASE_MAYBE_OPTIONAL_LINK(Ot::e_DOUBLE)
      CASE_MAYBE_OPTIONAL_LINK(Ot::e_STRING)
      CASE_MAYBE_OPTIONAL_LINK(Ot::e_DATETIME)
      CASE_MAYBE_OPTIONAL_LINK(Ot::e_DATE)
      CASE_MAYBE_OPTIONAL_LINK(Ot::e_TIME)
      CASE                    (Ot::e_CHAR_ARRAY)
      CASE                    (Ot::e_INT_ARRAY)
      CASE                    (Ot::e_INT64_ARRAY)
      CASE                    (Ot::e_DOUBLE_ARRAY)
      CASE                    (Ot::e_STRING_ARRAY)
      CASE                    (Ot::e_DATETIME_ARRAY)
      CASE                    (Ot::e_DATE_ARRAY)
      CASE                    (Ot::e_TIME_ARRAY)

      default: {
        BSLS_ASSERT_INVOKE_NORETURN("Reached: Unknown");
      } break;
    }

#undef CASE_MAYBE_OPTIONAL_LINK
#undef CASE

}

                         // ================
                         // function setType
                         // ================

/// Set the `balcl::OptionType` element of the specified `typeInfo` to the
/// specified `type`.  The behavior is undefined unless
/// `Ot::e_VOID != type`.  Note that this resets both the linked variable
/// and constraint of `typeInfo`.
void setType(TypeInfo *typeInfo, ElemType type)
{
    ASSERT(typeInfo);

#define CASE(TYPE)                                                            \
      case Ot::e_##TYPE: {                                                    \
        typeInfo->setLinkedVariable(Ot::k_##TYPE);                            \
      } break;                                                                \

    switch (type) {
      case Ot::e_VOID: {
        BSLS_ASSERT_INVOKE_NORETURN("Not reachable.");
      } break;

      CASE(BOOL)
      CASE(CHAR)
      CASE(INT)
      CASE(INT64)
      CASE(DOUBLE)
      CASE(STRING)
      CASE(DATETIME)
      CASE(DATE)
      CASE(TIME)
      CASE(CHAR_ARRAY)
      CASE(INT_ARRAY)
      CASE(INT64_ARRAY)
      CASE(DOUBLE_ARRAY)
      CASE(STRING_ARRAY)
      CASE(DATETIME_ARRAY)
      CASE(DATE_ARRAY)
      CASE(TIME_ARRAY)

      default: {
        BSLS_ASSERT_INVOKE_NORETURN("Shouldn't be reached");
      } break;
    }

#undef CASE

}

                          // =======================
                          // function createTypeInfo
                          // =======================

/// Set the specified `typeInfo` to have the specified `type`.  Optionally
/// specify `variable`, the address of a linked variable'.  Optionally
/// specify `isOptionalLinkedVariable` to `true` if `variable` is the
/// address of a `bsl::optional` object.  Optionally specify `constraint`,
/// the address of a constraint functor.  The behavior is undefined unless
/// `Ot::e_VOID != type`, `variable` is 0 or can be cast to either
/// `Ot::EnumToType<type>::type *` if `isOptionalLinkedVariable` is `false`
/// or `bsl::optional<Ot::EnumToType<type>::type> *` if
/// `isOptionalLinkedVariable` is `true`, `constraint` is 0 or can be cast
/// to the type defined by `Constraint` for `type`, and if
/// `Ot::e_BOOL == type` then `constraint` must be 0.
void createTypeInfo(TypeInfo *typeInfo,
                    ElemType  type,
                    void     *variable = 0,
                    bool      isOptionalLinkedVariable = false,
                    void     *constraint = 0)
{
    BSLS_ASSERT(typeInfo);

    if (balcl::OptionType::e_BOOL == type) {
        BSLS_ASSERT(0 == constraint);
    }

    setType(typeInfo, type);

    if (variable) {
        setLinkedVariable(typeInfo, type, variable, isOptionalLinkedVariable);
    }

    if (constraint) {
        setConstraint(typeInfo, type, constraint);
    }
}

// BDE_VERIFY pragma: -AR01  // Type using allocator is returned by value

/// Return (by value) a `TypeInfo` object having the specified `type`.
/// Optionally specify `variable`, an address to be linked to the option.
/// Optionally specify `isOptionalLinkedVariable` (if `true`, then
/// `variable` is the address of a `bsl::optional` object).  Optionally
/// specify a `constraint` on the value of the option.  The returned object
/// uses the currently installed default allocator.  The behavior is
/// undefined unless `Ot::e_VOID != type`, `variable` is 0 or can be cast to
/// `Ot::EnumToType<type>::type *` if `isOptionalLinkedVariable` is `false`
/// or `bsl::optional<Ot::EnumToType<type>::type> *` if
/// `isOptionalLinkedVariable` is `true`, and `constraint` is 0 or can be
/// cast to the type defined by `Constraint` for `type` .
TypeInfo createTypeInfo(ElemType  type,
                        void     *variable = 0,
                        bool      isOptionalLinkedVariable = false,
                        void     *constraint = 0)
{
    TypeInfo result;
    createTypeInfo(&result,
                   type,
                   variable,
                   isOptionalLinkedVariable,
                   constraint);
    return result;
}
// BDE_VERIFY pragma: +AR01  // Type using allocator is returned by value

/// Set the value at the specified `dst` to the value found at the specified
/// `src` that is of the specified `type`.  The behavior is undefined unless
/// `src` can be cast to a pointer of `Ot::EnumToType<type>::type`,
/// `Ot::e_VOID != type`, and `dst->type() == type`.
void setOptionValue(OptionValue *dst, const void *src, ElemType type)
{
    BSLS_ASSERT(dst);
    BSLS_ASSERT(src);
    BSLS_ASSERT(Ot::e_VOID  != type);
    BSLS_ASSERT(dst->type() == type);

#define CASE(ENUM)                                                            \
      case ENUM: {                                                            \
        dst->set(*(static_cast<const Ot::EnumToType<ENUM>::type *>(src)));    \
      } break;                                                                \

    switch (type) {
      case Ot::e_VOID: {
        BSLS_ASSERT_INVOKE_NORETURN("Not reachable.");
      } break;

      CASE(Ot::e_BOOL)
      CASE(Ot::e_CHAR)
      CASE(Ot::e_INT)
      CASE(Ot::e_INT64)
      CASE(Ot::e_DOUBLE)
      CASE(Ot::e_STRING)
      CASE(Ot::e_DATETIME)
      CASE(Ot::e_DATE)
      CASE(Ot::e_TIME)
      CASE(Ot::e_CHAR_ARRAY)
      CASE(Ot::e_INT_ARRAY)
      CASE(Ot::e_INT64_ARRAY)
      CASE(Ot::e_DOUBLE_ARRAY)
      CASE(Ot::e_STRING_ARRAY)
      CASE(Ot::e_DATETIME_ARRAY)
      CASE(Ot::e_DATE_ARRAY)
      CASE(Ot::e_TIME_ARRAY)

      default: {
        BSLS_ASSERT_INVOKE_NORETURN("Not reachable");
      } break;
    }

#undef CASE

}

                          // =============================
                          // function createOccurrenceInfo
                          // =============================

// BDE_VERIFY pragma: -AR01  // Type using allocator is returned by value

/// Return (by value) an `OccurrenceInfo` object having the specified
/// `occurrenceType` and having a default value of the specified `type` and
/// a value determined by the specified `variable` (if non-zero).  The
/// returned object uses the currently installed default allocator.  The
/// behavior is undefined unless `Ot::e_VOID != type` and `variable` can be
/// cast to a pointer of `Ot::EnumToType<type>::type`,
OccurrenceInfo createOccurrenceInfo(OccurrenceType  occurrenceType,
                                    ElemType        type,
                                    const void     *variable)
{
    BSLS_ASSERT(Ot::e_VOID != type);

    OccurrenceInfo result(occurrenceType);

    if (occurrenceType != OccurrenceInfo::e_REQUIRED
     && Ot::e_BOOL     != type
     && variable) {
        OptionValue defaultValue(type);
        setOptionValue(&defaultValue, variable, type);

        result.setDefaultValue(defaultValue);
    }

    return result;
}
// BDE_VERIFY pragma: +AR01  // Type using allocator is returned by value

#ifdef BDE_BUILD_TARGET_EXC

                         // =========================
                         // function throwInvalidSpec
                         // =========================

typedef bsl::logic_error InvalidSpec;

/// Throw an exception of type `InvalidSpec` containing the specified `text`
/// and referring to the specified `file` at the specified `line` number.
void throwInvalidSpec(const char *text, const char *file, int line)
{
    bsl::string errorMsg = "In file: ";
    errorMsg += file;
    errorMsg += ", at line: ";
    errorMsg += static_cast<char>(line);
    errorMsg += "\nAn invalid `balcl::OptionInfo` was encountered.";
    errorMsg += "\nThe following assertion failed: ";
    errorMsg += text;
    throw u::InvalidSpec(errorMsg.c_str());
}

#endif // BDE_BUILD_TARGET_EXC

                       // =============================
                       // function isCompatibleOrdering
                       // =============================

/// Return `true` if all the arguments specifying non-option argument values
/// and multi-valued options held in the specified `argv1` array of the
/// specified length `argc` occur in the same order as in the specified
/// `argv2` array of the same length, and `false` otherwise.  The behavior
/// is undefined unless the arguments are either "-S=..." for multi-valued
/// short options (single capitalized letter), "-s=..." for singly-valued
/// short options (non-capitalized), or non-option argument values.
bool isCompatibleOrdering(const char *const *argv1,
                          const char *const *argv2,
                          int                argc)
{
    bool nonOptionSeenFlag = false;
    bool multiOptionSeenFlag[26];
    bsl::fill(multiOptionSeenFlag, multiOptionSeenFlag + 26, false);

    bdlma::LocalSequentialAllocator<16 * 1024> ba;

    for (int i = 1; i < argc; ++i) {
        bool isOption = argv1[i][0] == '-';
        char shortOptionTag = isOption ? argv1[i][1] : 0;
        if ((isOption && 'A' <= shortOptionTag && shortOptionTag <= 'Z' &&
                                    !multiOptionSeenFlag[shortOptionTag - 'A'])
         || (!isOption && !nonOptionSeenFlag)) {
            if (isOption) {
                multiOptionSeenFlag[shortOptionTag - 'A'] = true;
            } else {
                nonOptionSeenFlag = true;
            }
            bsl::vector<bsl::string> filter1(&ba);
            bsl::vector<bsl::string> filter2(&ba);

            filter1.push_back(bsl::string(argv1[i], &ba));
            for (int j = i + 1; j < argc; ++j) {
                if ((argv1[i][0] == '-' && argv1[j][0] == '-' &&
                                                 argv1[j][1] == shortOptionTag)
                 || (argv1[i][0] != '-' && argv1[j][0] != '-')) {
                    filter1.push_back(bsl::string(argv1[j], &ba));
                }
            }
            for (int j = 1; j < argc; ++j) {
                if ((argv1[i][0] == '-' && argv2[j][0] == '-' &&
                                                 argv2[j][1] == shortOptionTag)
                 || (argv1[i][0] != '-' && argv2[j][0] != '-')) {
                    filter2.push_back(bsl::string(argv2[j], &ba));
                }
            }
            if (filter1 != filter2) {
                return false;                                         // RETURN
            }
        }
    }
    return true;
}

                         // ===========================
                         // function getSomeOptionValue
                         // ===========================

/// Return the address of the element in `OPTION_VALUES` having the
/// specified `type` and 0 if no such element is found.
const void *getSomeOptionValue(ElemType type)
{
    for (bsl::size_t i = 0; i < NUM_OPTION_VALUES; ++i) {
        if (OPTION_VALUES[i].d_type == type) {
            return OPTION_VALUES[i].d_value_p;                        // RETURN
        }
    }
    return 0;
}

                         // ========================================
                         // function isOptionalLinkedVariableInTable
                         // ========================================

/// Return `true` is the specified `variable`, possibly having a 0 value, is
/// the address of one of the `bsl::optional` objects that are used as
/// linked variables in some entries of the `OPTION_TYPEINFO` table, and
/// `false` otherwise.
bool isOptionalLinkedVariableInTable(void *variable)
{
    if (variable == &oLinkedChar
     || variable == &oLinkedInt
     || variable == &oLinkedInt64
     || variable == &oLinkedDouble
     || variable == &oLinkedString
     || variable == &oLinkedDatetime
     || variable == &oLinkedDate
     || variable == &oLinkedTime) {
        return true;                                                  // RETURN
    }
    return false;
}

                         // =========================
                         // function generateTestData
                         // =========================

/// Load into the specified `options` a sequence of `OptionInfo` objects
/// generated from the global tables of representative values
/// `OPTION_TYPEINFO`, `OPTION_OCCURRENCE`, and `OPTION_VALUES` (see {Input
/// Tables}).  Load into the specified `numBaseOptions` the number of
/// generated options.  The following sequences of options can be validly
/// used to construct a `balcl::CommandLine` object' where `i` is in the
/// range `[0 .. numBaseOptions)`:
/// * Single option
///   - `option[i]`
/// * Double option
///   - `option[i + ]`
///   - `option[i + numBaseOptions]`
/// * Triple options
///   - `option[i]`
///   - `option[i + 1 * numBaseOptions]`
///   - `option[i + 2 * numBaseOptions]`
/// Load into the specified `arguments` a string that can be used on the
/// command line to satisfy the corresponding option in `options`.  Return 0
/// on success and a non-zero value if the input data cannot be generated.
int generateTestData(bsl::vector<OptionInfo>  *options,
                     bsl::vector<bsl::string> *arguments,
                     int                      *numBaseOptions)
{
    ASSERT(options);
    ASSERT(arguments);
    ASSERT(numBaseOptions);

    char uniqueIdent[] = "_AAAA"; // 26^4 == 456976, enough for NUM_OPTIONS
    ASSERT(4 <= NUM_OPTION_TAGS);

// BDE_VERIFY pragma: -TP21    // Loops must contain very verbose action
// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
    for (int i = 0; i < 4;                          ++i) {
    for (int t = 0; t < NUM_OPTION_TYPEINFO;        ++t) {
    for (int m = 0; m < NUM_OPTION_OCCURRENCES - 1; ++m) {

        if (veryVerbose) { T_ P_(i) P_(t) P(m) }

        const char     *TAG        = OPTION_TAGS[i].d_tag_p;
        const char     *NAME       = "SOME UNIQUE NAME";
        const char     *DESC       = "SOME VERY LONG DESCRIPTION...";
        const ElemType  TYPE       = OPTION_TYPEINFO[t].d_type;
        void           *VARIABLE   = OPTION_TYPEINFO[t].d_linkedVariable_p;
        void           *CONSTRAINT = OPTION_TYPEINFO[t].d_constraint_p;

        const bool      IS_OPTIONAL_LINKED_VARIABLE =
                                     isOptionalLinkedVariableInTable(VARIABLE);

        const OccurrenceType OTYPE = OPTION_OCCURRENCES[
                                                t % NUM_OPTION_OCCURRENCES]
                                                                       .d_type;
        const void  *DEFAULT_VALUE = u::getSomeOptionValue(TYPE);

        const TypeInfo       TYPE_INFO       = createTypeInfo(
                                                   TYPE,
                                                   VARIABLE,
                                                   IS_OPTIONAL_LINKED_VARIABLE,
                                                   CONSTRAINT);
        const OccurrenceInfo OCCURRENCE_INFO = createOccurrenceInfo(
                                                                OTYPE,
                                                                TYPE,
                                                                DEFAULT_VALUE);

        const balcl::OptionInfo OPTION_INFO = {
            TAG,
            bsl::string(NAME) + uniqueIdent,
            DESC,
            TYPE_INFO,
            OCCURRENCE_INFO,
            ""
        };

        if (Ot::e_BOOL == TYPE_INFO.type() && OPTION_INFO.d_tag.empty()) {
            // Flags cannot have an empty tag, or in other words non-options
            // cannot be of type `bool`.  Skip this.

            continue;
        }

        if (OccurrenceInfo::e_HIDDEN == OCCURRENCE_INFO.occurrenceType()
         && OPTION_INFO.d_tag.empty()) {
            // A non-option argument cannot be hidden.  Skip this.

            continue;
        }

        if (TYPE_INFO.isOptionalLinkedVariable()
         && (TYPE_INFO.type() == Ot::e_BOOL  // Not impossible for `TypeInfo`.
          || OCCURRENCE_INFO.isRequired()
          || OCCURRENCE_INFO.hasDefaultValue())) {
            // Disallowed combinations when linked variable is a
            // `bsl::optional` object.  Skip this.

            continue;
        }

        // Generate the (table) option info.

        options->push_back(OPTION_INFO);

        // Generate a matching command line argument for parsing.

        bsl::string arg;  const bsl::string& ARG = arg;

       generateArgument(&arg, OPTION_INFO, i);
        arguments->push_back(ARG);

        // Guarantee unique names.

        for (int k = 1; ++(uniqueIdent[k]) > 'Z'; ++k) {
            uniqueIdent[k] = 'A';
        }

    }
    }
    }

// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line
// BDE_VERIFY pragma: +TP21    // Loops must contain very verbose action

    const int NUM_OPTIONS = static_cast<int>(options->size());
    *numBaseOptions = NUM_OPTIONS;

    if (verbose) cout << "\n\tBuilt " << NUM_OPTIONS << " options." << endl;

    int  limit            = 0;
    bool arrayNonOption   = false;
    bool defaultNonOption = false;

    // Add second layer (for 2-option specs).

    for (int i = 0; i < NUM_OPTIONS; ++i) {
        if (veryVerbose) { T_ P(i) }
        limit = 0;
          arrayNonOption = (*options)[i].d_tag.empty()
                         && Ot::isArrayType((*options)[i].d_typeInfo.type());
        defaultNonOption =  (*options)[i].d_tag.empty()
                         && (*options)[i].d_defaultInfo.hasDefaultValue();

        int j = ((i + 37) * (i + 101)) % NUM_OPTIONS;
        while (((*options)[i].d_tag[0] == (*options)[j].d_tag[0]
                                      && !(*options)[j].d_tag.empty())
            || (  arrayNonOption && (*options)[j].d_tag.empty())
            || (defaultNonOption && (*options)[j].d_tag.empty()
             && !(*options)[j].d_defaultInfo.hasDefaultValue())) {
            // Tags (long and short, if not empty) must be distinct; if there
            // is a previous non-option argument of array type then this cannot
            // be a non-option; and if a previous non-option argument had a
            // default value, then this non-option argument must have a default
            // value.

            if (veryVerbose) { T_ T_ P(j)  }

            j = (j + 1) % NUM_OPTIONS;
            if (++limit == NUM_OPTIONS) {
                break;
            }
        }
        if (limit == NUM_OPTIONS) {
            break;
        }
          options->push_back(  (*options)[j]);
        arguments->push_back((*arguments)[j]);
    }
    if (limit == NUM_OPTIONS) {
        ASSERTV("***CRITICAL ERROR***  Infinite loop in data.", 0);
        return -1;                                                    // RETURN
    }

    if (verbose) cout << "\tAdded another (shuffled) "
                      << NUM_OPTIONS << " options." << endl;

    ASSERT(static_cast<bsl::size_t>(2 * NUM_OPTIONS) ==   options->size());
    ASSERT(static_cast<bsl::size_t>(2 * NUM_OPTIONS) == arguments->size());

    // Add third layer (for 3-option specs).

    for (int i = 0, j = NUM_OPTIONS; i < NUM_OPTIONS; ++i, ++j) {
        if (veryVerbose) { T_ P(i) }
        limit = 0;
          arrayNonOption = ((*options)[i].d_tag.empty()
                         && Ot::isArrayType((*options)[i].d_typeInfo.type()))
                        || ((*options)[j].d_tag.empty()
                         && Ot::isArrayType((*options)[j].d_typeInfo.type()));

        defaultNonOption = ((*options)[i].d_tag.empty()
                         && (*options)[i].d_defaultInfo.hasDefaultValue())
                        || ((*options)[j].d_tag.empty()
                         && (*options)[j].d_defaultInfo.hasDefaultValue());

        int k = ((i + 107) * (i + 293)) % NUM_OPTIONS;

        while (  (*options)[i].d_name == (*options)[k].d_name
            ||   (*options)[j].d_name == (*options)[k].d_name
            || ( (*options)[i].d_tag[0] == (*options)[k].d_tag[0]
             && !(*options)[k].d_tag.empty())
            || ( (*options)[j].d_tag[0] == (*options)[k].d_tag[0]
             && !(*options)[k].d_tag.empty())
            || (   arrayNonOption && (*options)[k].d_tag.empty())
            || ( defaultNonOption && (*options)[k].d_tag.empty()
             && !(*options)[k].d_defaultInfo.hasDefaultValue())) {

            // Names and tags must be distinct, if there is a previous
            // non-option argument of array type then this cannot be a
            // non-option argument, and if a previous non-option argument had a
            // default value, then this non-option argument must have a default
            // value.

            if (veryVerbose) { T_ T_ P(k) }

            k = (k + 1) % NUM_OPTIONS;
            if (++limit == NUM_OPTIONS) {
                break;
            }
        }
        if (limit == NUM_OPTIONS) {
            break;
        }
          options->push_back(  (*options)[k]);
        arguments->push_back((*arguments)[k]);
    }
    if (limit == NUM_OPTIONS) {
        ASSERTV("***CRITICAL ERROR***  Infinite loop in data.", 0);
        return -2;                                                    // RETURN
    }

    if (verbose) cout << "\tAdded another (shuffled) "
                      << NUM_OPTIONS << " options." << endl;

    ASSERT(static_cast<bsl::size_t>(3 * NUM_OPTIONS) ==   options->size());
    ASSERT(static_cast<bsl::size_t>(3 * NUM_OPTIONS) == arguments->size());

    return 0;
}

                         // ===========================
                         // function generateParseInput
                         // ===========================

/// Load into the specified `argString` a Unix-style command line having
/// "programName' as the first (0th) argument followed by the specified `n`
/// entries from the specified `arguments`.  Each argument is separated by a
/// blank character.  The entry indices are determined by the specified `i`
/// and the specified `numOptions` by the formula: `i + x * numOptions`
/// where `x` ranges from 0 to `n - 1`.  Append the individual entries as
/// elements of the specified `argStrings`.  Load `argPtrs` with the address
/// of the start of each entry in `argStrings`.  Note that the values in
/// `argPtrs` remain valid as long as the elements of `argStrings` remain
/// stable.
void generateParseInput(bsl::string                     *argString,
                        bsl::vector<bsl::string>        *argStrings,
                        bsl::vector<const char *>       *argPtrs,
                        const bsl::vector<bsl::string>&  arguments,
                        int                              n,
                        int                              i,
                        int                              numOptions)
{
    ASSERT(argString)
    ASSERT(argStrings)
    ASSERT(argPtrs)

    argStrings->reserve(n + 5); // `argStrings` must be stable

    *argString = "\"programName";

    ASSERT(0 < argStrings->capacity());
    argStrings->push_back("programName");

    argPtrs->push_back(&(argStrings->back()[0]));

    for (int j = 0; j < n; ++j) {
        int index = i + j * numOptions;

        if (!arguments[index].empty()) {
            const int   MAX_ARGC = 16;
            const char *argv[MAX_ARGC];

            int argc = -1;
            argStrings->push_back(arguments[index]);
            ASSERT(0 == parseCommandLine(&(argStrings->back()[0]),
                                         argc,
                                         argv,
                                         MAX_ARGC));

            for (int k = 0; k < argc; ++k) {
                *argString += " ";
                *argString += argv[k];
                argPtrs->push_back(argv[k]);
            }
        }
    }
    *argString += "\"";
}

                        // =============================
                        // function normalizeIndentation
                        // =============================

/// Load into the specified `output` a "normalized" version of the specified
/// `input` where `input` is the output of the `TypeInfo` `print` method
/// called with the specified `level` and the specified `spacesPerLevel`.
/// The "normalized" version corresponds to calling `print` with `level` and
/// `spacesPerLevel` having the values 0 and 4, respectively.  The behavior
/// is undefined unless `level` and `spacesPerLevel` are both non-negative,
/// and unless `output->empty()`.
void normalizeIndentation(bsl::string        *output,
                          const bsl::string&  input,
                          int                 level,
                          int                 spacesPerLevel)
{
    BSLS_ASSERT(output);
    BSLS_ASSERT(output->empty());
    BSLS_ASSERT(0 <= level);
    BSLS_ASSERT(0 <= spacesPerLevel);

    bsl::vector<bslstl::StringRef> lines;

    for (bdlb::Tokenizer mech(input, "", "\n\0");
                         mech.isValid();
                       ++mech) {
        lines.push_back(mech.token());
    }

    for (bsl::size_t i = 0; i < lines.size(); ++i) {

        int numBlanks = 0;

        for (bsl::size_t j = 0; j < lines[i].size();  ++j, ++numBlanks) {
            if (' ' != lines[i].data()[j]) {
                break;
            }
        }

        ASSERT(0  == numBlanks % spacesPerLevel);

        int numLevelsIndented   = numBlanks / spacesPerLevel;
        int numLevelsNormalized = numLevelsIndented - level;

        ASSERT(0 <= numLevelsNormalized);
        int  numBlanksNormalized = numLevelsNormalized * 4;

        bsl::string normalizedIndentation(numBlanksNormalized, ' ');

        bsl::string line     = normalizedIndentation;
        bsl::string residual = bsl::string(lines[i].data() + numBlanks,
                                           lines[i].size() - numBlanks);
        line += residual;
        line += '\n';  // Terminate every line (even the last line) with '\n'.

        (*output) += line;
    }
}

}  // close namespace u
}  // close unnamed namespace

// ============================================================================
//                  USAGE EXAMPLE CLASSES AND FUNCTIONS
// ----------------------------------------------------------------------------

namespace BALCL_COMMANDLINE_USAGE_EXAMPLE_1 {

// BDE_VERIFY pragma: -FD01  // Avoid contract for `main` below.

//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Parsing Command Line Options Using Minimal Functionality
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to design a sorting utility named `mysort` that has the
// following syntax:
// ```
//  Usage: mysort [-r|reverse] [-f|field-separator <fieldSeparator>]
//                -o|outputfile <outputFile> [<fileList>]+
//                            // Sort the specified files (in `fileList`), and
//                            // write the output to the specified output file.
// ```
// The `<fileList>` argument is a `non-option`, meaning that its value or
// values appear on the command line unannounced by tags.  In this case, the
// '+' following the argument means that it is an array type of argument where
// at least one element is required, so its values are stored in a
// `bsl::vector`.
// ```
    int main(int argc, const char **argv)
    {
        using balcl::TypeInfo;
        using balcl::OccurrenceInfo;
// ```
// First, we define our variables to be initialized from the command line.  All
// values must be initialized to their default state:
// ```
        bool reverse = false;
        bsl::string outputFile;
        char fieldSeparator = '|';
        bsl::vector<bsl::string> files;
// ```
// Then, we define our `OptionInfo` table of attributes to be set.  The fields
// of the `OptionInfo` are:
//
//  - tag - the tag on the command line for supplying the option
//
//  - name - a one word description of the value to provide for the option
//
//  - description - a short body of text describing the purpose of the option
//    etc.
//
//  - TypeInfo - information about the type of the input expected (possibly the
//    variable in which to load the value)
//
//  - OccurenceInfo - (optional) whether the option is required, optional, or
//    hidden (where hidden means hidden from the help text)
//
//  - environment variable name - (optional) the name of an environment
//    variable to use for the option if the option is not provided on the
//    command line
// ```
        static const balcl::OptionInfo specTable[] = {
          {
            "r|reverse",                                   // tag
            "isReverse",                                   // name
            "sort in reverse order",                       // description
            TypeInfo(&reverse),                            // link
            OccurrenceInfo(),                              // occurrence
            ""                                             // env var name
          },
          {
            "f|field-separator",                           // tag
            "fieldSeparator",                              // name
            "field separator character",                   // description
            TypeInfo(&fieldSeparator),                     // link
            OccurrenceInfo(),                              // occurrence
            ""                                             // env var name
          },
          {
            "o|outputfile",                                // tag
            "outputFile",                                  // name
            "output file",                                 // description
            TypeInfo(&outputFile),                         // link
            OccurrenceInfo::e_REQUIRED,                    // occurrence info
                                                           // (not optional)
            ""                                             // env var name
          },
          {
            "",                                            // non-option
            "fileList",                                    // name
            "input files to be sorted",                    // description
            TypeInfo(&files),                              // link
            OccurrenceInfo::e_REQUIRED,                    // occurrence info
                                                           // (at least one
                                                           // file required)
            ""                                             // env var name
          }
        };
// ```
// Now, we create a balcl command-line object, supplying it with the spec table
// that we have just defined:
// ```
        balcl::CommandLine cmdLine(specTable);
// ```
// Parse the options and if an error occurred, print a usage message describing
// the options:
// ```
        if (cmdLine.parse(argc, argv)) {
            cmdLine.printUsage();
            return -1;                                                // RETURN
        }
// ```
// If there are no errors in the specification table and correct arguments are
// passed, `parse` will set any variables that were specified on the command
// line, return 0 and there will be no output.
//
// Finally, we show what will happen if `mysort` is called with invalid
// arguments.  We will call without specifying an input file to `fileList`,
// which will be an error.  `parse` streams a message describing the error and
// then returns non-zero, so our program will call `cmdLine.printUsage`, which
// prints a detailed usage message.
// ```
//  $ mysort -r -o sorted.txt
//  Error: No value supplied for the non-option argument "fileList".
//
//  Usage: mysort [-r|reverse] [-f|field-separator <fieldSeparator>]
//                -o|outputfile <outputFile> [<fileList>]+
//  Where:
//    -r | --reverse
//            sort in reverse order
//    -f | --field-separator  <fieldSeparator>
//            field separator character
//    -o | --outputfile       <outputFile>
//            output file
//                            <fileList>
//            input files to be sorted
// ```
//
///Example 2: Accessing Option Values Through `balcl::CommandLine` Accessors
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Imagine we defined the same `mysort` program with the same options.  After a
// successful `parse`, `balcl::Commandline` makes the state of every option
// available through accessors (in addition to setting external variables as
// shown in example 1).
//
// For every type that is supported, there is a `the<TYPE>` accessor which
// takes a single argument, the name of the argument.  In the above program, if
// parsing was successful, the following asserts will always pass:
// ```
        ASSERT(cmdLine.theBool("isReverse")       == reverse);
        ASSERT(cmdLine.theString("outputFile")    == outputFile);
        ASSERT(cmdLine.theStringArray("fileList") == files);
// ```
// The next accessors we'll discuss are `isSpecified` and `numSpecified`.
// Here, we use `isSpecified` to determine whether "fieldSeparator" was
// specified on the command line, and we use `numSpecified` to determine the
// number of times the "fieldSeparator" option appeared on the command line:
// ```
        if (cmdLine.isSpecified("fieldSeparator")) {
            const unsigned char uc = cmdLine.theChar("fieldSeparator");
            if (!::isprint(uc)) {
                bsl::cerr << "`fieldSeparator` must be printable.\n";

                return -1;                                            // RETURN
            }
        }

        if (1 < cmdLine.numSpecified("fieldSeparator")) {
            bsl::cerr <<
                     "`fieldSeparator` may not be specified more than once.\n";

            return -1;                                                // RETURN
        }
return 0;
}
// ```

}  // close namespace BALCL_COMMANDLINE_USAGE_EXAMPLE_1

namespace BALCL_COMMANDLINE_USAGE_EXAMPLE_3 {

int LINE;

/// This function checks that the values passed to the command line in usage
/// example 2 are as expected.
void checkExample3(const balcl::CommandLine& cmdLine)
{
    ASSERTV(LINE, cmdLine.theBool("isReverse"));
    const bsl::vector<bsl::string>& sa = cmdLine.theStringArray("fileList");
    ASSERTV(LINE, 3 == sa.size());
    ASSERTV(LINE, sa[0] == "inputFile1");
    ASSERTV(LINE, sa[1] == "inputFile2");
    ASSERTV(LINE, sa[2] == "inputFile3");
}

//
///Example 3: Default Values and Specifying Values Via The Environment
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are implementing `mysort` (from examples 1 & 2) again, but here
// we want to make use of default option values and the ability to supply
// options via the environment.
//
// In this example, we have decided not to link local variables, and instead
// access the option values via the `balcl::CommandLine` object.  Since we are
// not linking local variables, we specify `OptionType::k_<TYPE>` in the
// specification table below for each `TypeInfo` field.
//
// To specify default values, we pass the default value to the `OccurrenceInfo`
// field.  Boolean options always have a default value of `false`.
//
// We also choose to allow these options to be supplied through the
// environment.  To enable this, we specify an environment variable name as the
// 5th (optional) element of the `OptionInfo` specification for the option.  If
// no name is supplied for the environment variable, the option cannot be set
// via the environment.
//
// First, in `main`, we define our spec table:
// ```
    using balcl::TypeInfo;
    using balcl::OptionType;
    using balcl::OccurrenceInfo;

    // option specification table
    static const balcl::OptionInfo specTable[] = {
      {
        "r|reverse",                             // tag
        "isReverse",                             // name
        "sort in reverse order",                 // description
        TypeInfo(OptionType::k_BOOL),            // type
        OccurrenceInfo::e_OPTIONAL,              // optional
        "MYSORT_REVERSE"                         // env var name
      },
      {
        "",                                      // non-option
        "fileList",                              // name
        "input files to be sorted",              // description
        TypeInfo(OptionType::k_STRING_ARRAY),    // type
        OccurrenceInfo::e_REQUIRED,              // at least one file required
        "MYSORT_FILES"                           // env var name
      }
    };
//
    int main(int argc, const char **argv)
    {
// ```
// Then, we declare our `cmdLine` object.  This time, we pass it a stream, and
// messages will be written to that stream rather than `cerr` (the default).
// ```
        balcl::CommandLine cmdLine(specTable, bsl::cout);
// ```
// Next, we call `parse` (just like in Example 1):
// ```
        if (cmdLine.parse(argc, argv)) {
            cmdLine.printUsage();
            return -1;                                                // RETURN
        }
checkExample3(cmdLine);
return 0;
}
// ```
// `balcl::CommandLine` uses the following precedence to determine the value of
// a command line option:
//
// 1. Use the option value on the command-line (if one was supplied)
//
// 2. Use the option value supplied by an environment variable (if one was
//    supplied)
//
// 3. Use the default value (if one was supplied, or `false` for booleans)
//
// Finally, if an option value is not supplied by either the command line or
// environment, and there is no default value, any linked variable will be
// unmodified, `cmdLine.hasValue` for the option will return `false`, and the
// behavior is undefined if `cmdLine.the<TYPE>` for the option is called.
//
// Note that `cmdLine.isSpecified` will be `true` only if an option was
// supplied by the command line or the environment.
//
// If an array options is set by an environment variable, the different
// elements of the array are separated by spaces by default.
//
// All these calling sequences are equivalent:
// ```
//  $ mysort -r inputFile1 inputFile2 inputFile3
// ```
// or
// ```
//  $ mysort inputFile1 --reverse inputFile2 inputFile3
// ```
// or
// ```
//  $ mysort inputFile1 inputFile2 inputFile3 -r
// ```
// or the user can specify arguments through environment variables:
// ```
//  $ export MYSORT_REVERSE=true
//  $ export MYSORT_FILES="inputFile1 inputFile2 inputFile3"
//  $ mysort
// ```
// or as a combination of command line arguments and environment variables:
// ```
//  $ export MYSORT_FILES="inputFile1 inputFile2 inputFile3"
//  $ mysort -r
// ```
// The '\' character is used as an escape character for array values provided
// via an environment variable.  So, for example, if we needed to encode file
// names that contain a space (' '), which is the element separator (by
// default), we would use "\ ":
//..
//  $ export MYSORT_FILES='C:\\file\ name\ 1 C:\\file\ name\ 2'
//..
// Notice we used a single tick to avoid requiring a double escape when
// supplying the string to the shell (e.g., avoiding "C:\\\\file\\ name\\ 1").

}  // close namespace BALCL_COMMANDLINE_USAGE_EXAMPLE_3

namespace BALCL_COMMANDLINE_USAGE_EXAMPLE_4 {

int                                       LINE;

bdlma::LocalSequentialAllocator<1 << 13>  parseAlloc;
bsl::ostringstream                        parseOss(&parseAlloc);
const char                               *expMessage;

bool                                      expReverse;
char                                      expFieldSeparator;
const char                               *expFileName;

void checkExample5(const balcl::CommandLine& cmdLine, int rc)
{
    if (expMessage) {
        ASSERTV(LINE, bsl::string::npos != parseOss.view().find(expMessage));

        return;                                                       // RETURN
    }

    ASSERTV(LINE, 0 == rc);
    if (rc) {
        return;                                                       // RETURN
    }

    ASSERTV(LINE, parseOss.view().empty());

    ASSERTV(LINE, cmdLine.theBool("isReverse")      == expReverse);
    ASSERTV(LINE, cmdLine.theChar("fieldSeparator") == expFieldSeparator);

    const bsl::vector<bsl::string>& fileList =
                                            cmdLine.theStringArray("fileList");
    ASSERTV(LINE, fileList.size() == 1);
    ASSERTV(LINE, fileList[0] == expFileName);
}

//
///Example 4: Option Constraints
// - - - - - - - - - - - - - - -
// Suppose, we are again implementing `mysort`, and we want to introduce some
// constraints on the values supplied for the variables.  In this example, we
// will ensure that the supplied input files exist and are not directories, and
// that `fieldSeparator` is appropriate.
//
// First, we write a validation function for the file name.  A validation
// function supplied to `balcl::CommandLine` takes an argument of a const
// pointer to the input option type (with the user provided value) and a stream
// on which to write an error message, and the validation function returns a
// `bool` that is `true` if the option is valid, and `false` otherwise.
//
// Here, we implement a function to validate a file name, that returns `true`
// if the file exists and is a regular file, and `false` otherwise (writing an
// description of the error to the `stream`):
// ```
    bool isValidFileName(const bsl::string *fileName, bsl::ostream& stream)
    {
        if (!bdls::FilesystemUtil::isRegularFile(*fileName, true)) {
            stream << "Invalid file: " << *fileName << bsl::endl;

            return false;                                             // RETURN
        }

        return true;
    }
// ```
// Then, we also want to make sure that the specified `fieldSeparator` is
// a non-whitespace printable ascii character, so we write a function for that:
// ```
    bool isValidFieldSeparator(const char    *fieldSeparator,
                               bsl::ostream&  stream)
    {
        const unsigned char uc = *fieldSeparator;
        if (::isspace(uc) || !::isprint(uc)) {
            stream << "Invalid field separator specified." << bsl::endl;

            return false;                                             // RETURN
        }

        return true;
    }
// ```
// Next, we define `main` and declare the variables to be configured:
// ```
    int main(int argc, const char **argv)
    {
        using balcl::Constraint;
        using balcl::OptionType;
        using balcl::OccurrenceInfo;
        using balcl::TypeInfo;
//
        bool                     reverse = false;
        char                     fieldSeparator;
        bsl::vector<bsl::string> files;
// ```
// Notice that `fieldSeparator` are in automatic storage with no constructor or
// initial value.  We can safely use an uninitialized variable in the
// `specTable` below because the `specTable` provides a default value for it,
// which will be assigned to the variable if an option value is not provided on
// the command line or through environment variables.  `reverse` has to be
// initialized because no default for it is provided in `specTable`.
//
// Then, we declare our `specTable`, providing function pointers for our
// constraint functions to the second argument of the `TypeInfo` constructor.
// ```
        // option specification table

        static const balcl::OptionInfo specTable[] = {
          {
            "r|reverse",                             // tag
            "isReverse",                             // name
            "sort in reverse order",                 // description
            TypeInfo(&reverse),                      // linked variable
            OccurrenceInfo(),                        // default value
            ""                                       // env variable name
          },
          {
            "f|field-separator",                     // tag
            "fieldSeparator",                        // name
            "field separator character",             // description
            TypeInfo(&fieldSeparator,                // linked variable
                     &isValidFieldSeparator),        // constraint
            OccurrenceInfo('|'),
            ""                                       // env variable name
          },
          {
            "",                                      // non-option
            "fileList",                              // name
            "input files to be sorted",              // description
            TypeInfo(&files, &isValidFileName),      // linked variable and
                                                     // constraint
            OccurrenceInfo::e_REQUIRED,              // at least one file
                                                     // required
            ""                                       // env variable name
          }
        };
//
        balcl::CommandLine cmdLine(specTable);
//      if (cmdLine.parse(argc, argv)) {
//          cmdLine.printUsage();
//          return -1;                                                // RETURN
//      }
parseOss.str("");
int rc = cmdLine.parse(argc, argv, parseOss);
if (rc) cmdLine.printUsage(parseOss);
checkExample5(cmdLine, rc);
if (veryVerbose) cout << parseOss.view();
return rc;
}
// ```
// If the constraint functions return `false`, `cmdLine.parse` will return
// non-zero, and the output will contain the message from the constraint
// function followed by the usage message.

}  // close namespace BALCL_COMMANDLINE_USAGE_EXAMPLE_4

namespace BALCL_COMMANDLINE_USAGE_EXAMPLE_5 {

// BDE_VERIFY pragma: -FD01  // Avoid contract for `main` below.

void performTask(bsl::istream& input)
{
    bsl::string str;
    input >> str;    input >> str;
    if (verbose) cout << "We got: '" << str << "'\n";
}

//
///Example 5: Using `bsl::optional` for Optional Command Line Parameters
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// We can use a `bsl::optional` variables when providing optional command line
// parameters.  Suppose we want to write a command line that takes an optional
// input file, and if the file is not supplied, take input from a
// `bsl::stringstream`.
//
// To represent the optional file name parameter, we link a variable of type
// `bsl::optional<bsl::string>`.  In general, when linking a variable to an
// option, we can choose to use `bsl::optonal<TYPE>` in place of `TYPE`, for
// any option type other than `bool`.
// ```
    int main(int argc, const char **argv)
    {
        bsl::optional<bsl::string> optionalFileName;

        const balcl::OptionInfo specTable[] = {
          {
            "i|inputFile",                         // tag
            "filename",                            // name
            "name of input file",                  // description
            balcl::TypeInfo(&optionalFileName),    // linked optional variable
            balcl::OccurrenceInfo(),               // occurence info
            ""                                     // env variable name
          }
        };

        balcl::CommandLine cmdLine(specTable);
        if (cmdLine.parse(argc, argv)) {
            cmdLine.printUsage();
            return -1;                                                // RETURN
        }
// ```
// Finally, we test whether `optionalFileName` has been set, and if it has not
// been set, take input from a prepared `stringstream`.
// ```
        bsl::stringstream ss;
        bsl::istream *inStream = &ss;

        bsl::ifstream fileStream;
        if (optionalFileName.has_value()) {
            fileStream.open(optionalFileName->c_str());
            inStream = &fileStream;
        }
        else {
            // Prepare the `stringstream`.

            ss << "The quick brown fox jumps over the lazy dog\n";
        }

        performTask(*inStream);
// ```
return 0;
}

// BDE_VERIFY pragma: -FD01  // Avoid contract for `main` above.

}  // close namespace BALCL_COMMANDLINE_USAGE_EXAMPLE_5

namespace {
namespace u {

typedef int (*MainFunc)(int, const char **);

/// Return the valid of the `main` function (in this namespace) called with
/// the specified `argc` and `argv`.  Output to `bsl::cout` and `bsl::cerr`
/// is redirected to a temporary `bsl::ostringstream` before the call to
/// `main` and restored afterwards.  Thus, the console output demonstrated
/// by the example code is suppressed.
int redirectedMain(int line, const char *cmdLine, MainFunc mainFunc)
{
    char cmdBuf[1024];
    bsl::strcpy(cmdBuf, cmdLine);
    bsl::strcat(cmdBuf, " ");

    int         argc;
    const char *argv[40];
    u::parseCommandLine(cmdBuf, argc, argv);

    bsl::streambuf *outStreamBuf = cout.rdbuf();
    bsl::streambuf *errStreamBuf = cerr.rdbuf();

    bdlma::LocalSequentialAllocator<8 * 1024>  ossOutAlloc;
    bsl::ostringstream                         ossOut(&ossOutAlloc);

    cout.rdbuf(ossOut.rdbuf());  // Redirect `cout`.
    cerr.rdbuf(ossOut.rdbuf());  // Redirect `cerr`.

    int rc = (*mainFunc)(argc, argv);

    cout.rdbuf(outStreamBuf);    // Restore `cout`.
    cerr.rdbuf(errStreamBuf);    // Restore `cerr`.

    if (veryVerbose) {
        cout << "line: " << line << ", parse rc: " << rc << endl;
        cout << "$ " << cmdLine << endl;
        cout << ossOut.view();
        cout << "$\n";
    }

    return rc;
}

}  // close namespace u
}  // close unnamed namespace

// ============================================================================
//                              MACROS
// ----------------------------------------------------------------------------

#define PREAMBLE_OPTIONS_ARGUMENTS                                            \
              bsl::vector<OptionInfo>  options;                               \
        const bsl::vector<OptionInfo>& OPTIONS = options;                     \
                                                                              \
              bsl::vector<bsl::string>  arguments;                            \
        const bsl::vector<bsl::string>& ARGUMENTS = arguments;                \
                                                                              \
              int  numBaseOptions = 0;                                        \
        const int& NUM_OPTIONS    = numBaseOptions;                           \
                                                                              \
        if (0 != u::generateTestData(&options, &arguments, &numBaseOptions)) {\
            break;                                                            \
        }                                                                     \

#define GENERATE_PARSE_INPUT(N, I)                                            \
            bsl::string               argString;                              \
            bsl::vector<bsl::string>  argStrings;                             \
            bsl::vector<const char *> argPtrs;                                \
                                                                              \
            u::generateParseInput(&argString,                                 \
                                  &argStrings,                                \
                                  &argPtrs,                                   \
                                  ARGUMENTS,                                  \
                                  N,                                          \
                                  I,                                          \
                                  NUM_OPTIONS);                               \
                                                                              \
            const bsl::string&               ARG_STRING  = argString;         \
            const bsl::vector<const char *>& ARG_PTRS    = argPtrs;           \
                                                                              \
            (void) ARG_STRING;                                                \

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, const char *argv[])
{
    int            test = argc > 1 ? bsl::atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4; (void) veryVeryVerbose;
    veryVeryVeryVerbose = argc > 5; (void) veryVeryVeryVerbose;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    bslma::TestAllocator ta("test", veryVeryVeryVerbose);

    bslma::Default::setGlobalAllocator(&ga);

    switch (test) { case 0:  // Zero is always the leading case.
      case 29: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 5
        //
        // Concern:
        // 1. That Usage Example 5 compiles and works properly.
        //
        // Plan:
        // 1. Call the simulated `main` in the namespace of the usage example.
        //
        // 2. If `veryVerbose` (meaning `3 < argc`) and `argv[3]` is a file
        //    name, then pass that file name to the `-i` option on the command
        //    line.
        // --------------------------------------------------------------------

        namespace TC = BALCL_COMMANDLINE_USAGE_EXAMPLE_5;

        if (verbose) cout << "TESTING USAGE EXAMPLE 5\n"
                             "=======================\n";

        bsl::string cmdStr("myprog");
        if (3 < argc) {                                 // `veryVerbose` is set
            const char *maybeFileName = argv[3];
            if (bdls::FilesystemUtil::exists(maybeFileName)) {
                cmdStr += " -i ";
                cmdStr += maybeFileName;
            }
        }

        ASSERT(0 == u::redirectedMain(0, cmdStr.c_str(), &TC::main));
      } break;
      case 28: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 4
        //
        // Cencern:
        // 1. That Usage Example 4 compiles and works properly.
        //
        // Plan:
        // 1. Establish 2 strings: `VALID_FN`, the name of a valid plain file,
        //    and `VALID_DN`, the name of a valid directory.
        //
        // 2. Iterate through a table which provides command lines, expected
        //    values of options, and expected warnings (when a warning is
        //    expected, the expected option values are ignored).
        //
        // 3. Populate variables named `exp*` in the namespace of the usage
        //    example with the expected values from the table, so that code in
        //    that namespace can compare them to the configured values.
        //
        // 4. Pass the command line from the table and the function ptr of the
        //    simulated `main` to `u::redirectedMain` which will parse the
        //    command line into `argc`, `argv` variables, redirect `cout` and
        //    `cerr`, and call the simulated `main`.
        // --------------------------------------------------------------------

        namespace TC = BALCL_COMMANDLINE_USAGE_EXAMPLE_4;

        if (verbose) cout << "TESTING USAGE EXAMPLE 4\n"
                             "=======================\n";

        #define VALID_FN __FILE__

        // Valid directory name, invalid input file.

#ifdef BSLS_PLATFORM_OS_UNIX
        #define VALID_DN  "/usr"
#else
        #define VALID_DN  "c:/windows"
#endif

        ASSERTV(VALID_FN, bdls::FilesystemUtil::isRegularFile(VALID_FN));
        ASSERTV(VALID_DN, bdls::FilesystemUtil::isDirectory(VALID_DN));

        static const struct Data {
            int         d_line;
            bool        d_reverse;
            char        d_fieldSeparator;
            const char *d_fileName;
            const char *d_message;
            const char *d_cmd;
        } DATA[] = {
        //      V -- expected value of `reverse`
        //      |   V -- expected value of `fieldSeparator`
        //      |   |   V -- expected value of `fileName` (non-option)
        //      |   |   |         V -- pattern expected to appear in streamed
        //      |   |   |         |    warning messsage (or 0 if no message)
        //      |   |   |         |  V -- command line
        //      V   V   V         V  V
          { L_, 1, ',', VALID_FN, 0, "mysort -r -f=, " VALID_FN },
          { L_, 0, '|', VALID_FN, 0, "mysort " VALID_FN },
          { L_, 1, '|', VALID_FN, 0, "mysort -r " VALID_FN },
          { L_, 0,   0, 0,        "Invalid file: " VALID_DN,
                                     "mysort " VALID_DN },
          { L_, 0,   0, 0,        "Invalid field separator specified.",
                                     "mysort -f=\xa3" },
          { L_, 0,   0, 0,        "Invalid file: a.txt",
                                     "mysort a.txt" },
          { L_, 1, '|', VALID_FN, 0, "mysort -r " VALID_FN },
        };
        enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int ti = 0; ti < k_NUM_DATA; ++ti) {
            const Data& data      = DATA[ti];
            TC::LINE              = data.d_line;
            TC::expMessage        = data.d_message;
            TC::expReverse        = data.d_reverse;
            TC::expFieldSeparator = data.d_fieldSeparator;
            TC::expFileName       = data.d_fileName;
            const char *CMD       = data.d_cmd;

            ASSERTV(TC::LINE, (0 == TC::expMessage) ? "0" : TC::expMessage,
                    (0 == TC::expMessage) ==
                           (0 == u::redirectedMain(TC::LINE, CMD, &TC::main)));
        }
      } break;
      case 27: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 3
        //
        // Concerns:
        // 1. Demonstrate usage of component without linked variables, using
        //    `the*`, `isSpecified`, and `numSpecified` accessors.
        //
        // 2. Also demonstrate setting arguments with environment variables.
        //
        // Plan:
        // 1. Do an example based on exactly the same program as was done in
        //    Example 1, only using the other methods to set values and obtain
        //    configured argument state.
        //
        // 2. The expect values of the `reverse` boolean option and the input
        //    files is always the same, so no expected values are configured in
        //    the table.
        //
        // Testing:
        //   USAGE EXAMPLE 3
        // --------------------------------------------------------------------

        namespace TC = BALCL_COMMANDLINE_USAGE_EXAMPLE_3;

        if (verbose) cout << "TESTING USAGE EXAMPLE 3\n"
                             "=======================\n";

        static const struct Data {
            int         d_line;
            const char *d_reverse;
            const char *d_files;
            const char *d_cmd;
        } DATA[] = {
        //      V -- `MYSORT_REVERSE` environment variable value ("" for unset)
        //      |       V -- 'MYSORT_FILES" environment variable value ("" for
        //      |       |    unset)
        //      |       |   V -- command line
        //      V       V   V
          { L_, "",     "", "mysort -r inputFile1 inputFile2 inputFile3" },
          { L_, "",     "", "mysort inputFile1 -r inputFile2 inputFile3" },
          { L_, "",     "", "mysort inputFile1 inputFile2 inputFile3 -r" },
          { L_, "true", "inputFile1 inputFile2 inputFile3",
                            "mysort" },
          { L_, "",     "inputFile1 inputFile2 inputFile3",
                            "mysort --reverse" },
          { L_, "   1", "inputFile1 inputFile2 inputFile3",
                            "mysort" },
        };
        enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int ti = 0; ti < k_NUM_DATA; ++ti) {
            const Data&        data      = DATA[ti];
            const int          LINE      = data.d_line;
            const char        *REV_STR   = data.d_reverse;
            const char        *FILES_STR = data.d_files;
            const char        *CMD       = data.d_cmd;

            TC::LINE = LINE;

            if (*REV_STR) {
                u::setEnvironmentVariable("MYSORT_REVERSE", REV_STR);
                if (veryVerbose) {
                    cout << "$ export " << "MYSORT_REVERSE=" << REV_STR <<
                                                                          endl;
                }
            }
            else {
                u::unsetEnvironmentVariable("MYSORT_REVERSE");
            }

            if (*FILES_STR) {
                u::setEnvironmentVariable("MYSORT_FILES", FILES_STR);
                if (veryVerbose) {
                    cout << "$ export MYSORT_FILES=\"" << FILES_STR << "\"\n";
                }
            }
            else {
                u::unsetEnvironmentVariable("MYSORT_FILES");
            }

            ASSERT(0 == u::redirectedMain(LINE, CMD, &TC::main));
        }
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 1
        //
        // Concerns:
        // 1. Demonstrate the use of the component with the absolute minimal
        //    functionality.
        //
        // Plan:
        // 1. Do a test case involving all linked variables with no defaults in
        //    the `specTable`, all variables assigned to their default values
        //    in their definitions.
        //
        // 2. Check for correctness of values in code following the parse
        //    rather than specifying constraints in the `specTable`.
        //
        // Testing:
        //   USAGE EXAMPLE 1
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING USAGE EXAMPLE 1\n"
                             "=======================\n";

        namespace TC = BALCL_COMMANDLINE_USAGE_EXAMPLE_1;

        const struct Test {
            int         d_line;
            bool        d_succeed;
            const char *d_cmdLine;
        } TESTS[] = {
            { L_, false, 0 },
            { L_, false, "mysort" },
            { L_, false, "mysort -r -o sorted.txt" },
            { L_, true,  "mysort -r -o sorted.txt in" },
            { L_, true,  "mysort -o sorted.txt -f=, in1 in2 in3" },
            { L_, false, "mysort -o sorted.txt -f=, -f=, in1 in2 in3" } };
        enum { k_NUM_TESTS = sizeof TESTS / sizeof *TESTS };

        for (int ii = 1; ii < k_NUM_TESTS; ++ii) {
            const Test&  test    = TESTS[ii];
            const int    LINE    = test.d_line;
            const bool   EXP     = test.d_succeed;
            const char  *CMD     = test.d_cmdLine;

            if (verbose) cout << "Test[" << ii << "]:\n\n";

            ASSERTV(LINE, EXP ==
                               (0 == u::redirectedMain(LINE, CMD, &TC::main)));
        }
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // INITIALIZATION OF FIELDS WITH EMPTY CURLY BRACES
        //
        // Concern:
        // 1. Spectable is able to take empty curly braces initilizing
        //    `OccurenceInfo` or `TypeInfo` on C++11 and beyond.
        //
        // Plan:
        // 1. Create a spectable passing empty curly braces to the type info
        //    and occurrence info fields of `OptionInfo` and verify that it
        //    compiles and functions correctly.
        //
        // Testing:
        //   INITIALIZATION OF FIELDS WITH EMPTY CURLY BRACES
        // --------------------------------------------------------------------

        if (verbose) cout <<
                          "INITIALIZATION OF FIELDS WITH EMPTY CURLY BRACES\n"
                          "================================================\n";

// Passing `{}` to a non-trivial default constructor in an aggregate
// initialization is supported in C++11 and later, but is technically not
// supported in C++03.  However, the Sun compilers support this, and there is
// production code that relies on this behavior, so we test it here for
// backwards compatibility.

#if 201103L <= BSLS_COMPILERFEATURES_CPLUSPLUS                                \
                                              || defined(BSLS_PLATFORM_CMP_SUN)

        bsl::string stringOne;
        static const OptionInfo SPEC[] = {
            {
                "o|stringOne",
                "StringOne",
                "first String",
                balcl::TypeInfo(&stringOne),
                {}
            },
            {
                "t|stringTwo",
                "StringTwo",
                "second String",
                {}
            }
        };

        char        cmdLine[256] = { "myprog -o woof -t meow" };
        int         argc;
        const char *argv[40];
        u::parseCommandLine(cmdLine, argc, argv);;

        bsl::ostringstream oss;
        Obj mX(SPEC, oss);    const Obj& X = mX;

        const int rc = mX.parse(argc, argv, oss);
        ASSERTV(rc, 0 == rc);

        ASSERT("woof" == stringOne);
        ASSERT("meow" == X.theString("StringTwo"));
#endif
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // Environment Variables Overriden by Command Line, Always Checked
        //
        // Concerns:
        // 1. If an option is set by both the command line and the environment
        //    variable, the command line value takes precedence.
        //
        // 2. If both command line and environment values are provided, both
        //    are parsed for syntactic correctness, and if either fails, it
        //    results in `parse` returning non-zero.
        //
        // Plan:
        // 1. Define 3 variables of 3 types and a spec table for setting all
        //   3. of them, with environment variable names.
        //
        // 2. Define non-default-constructed `default values` for the 3
        //    variables, and a couple of other non-default-constructed values
        //    for the array variable.
        //
        // 3. Define a table with:
        //    - expected return value from `parse`
        //
        //    - expected values of the 3 variables
        //
        //    - the command line (not including the program name)
        //
        //    - an index of which of 3 environment variables are to be set
        //
        //    - the string that the selected environment variable, if any, is
        //      to be set
        //
        // 4. Define a table setting the above values and iterate through it.
        //
        // 5. Set only the selected environment variable, if any, to the
        //    value indicated in the table.
        //
        // 6. Create a `CommandLine` object with the spec table and call
        //    `parse`.  Observe that the return value of `parse` was as
        //    expected, and if and only if the return value was non-zero, there
        //    was output streamed.
        //
        // 7. If the `parse` call succeeded, confirm that the values of the
        //   3. variables was as expected.
        // --------------------------------------------------------------------

        typedef bsl::vector<int> Array;

        int myInt;
        double myDouble;
        Array myArray(&ta);

        static const int              DI = 3;                 // Default Int
        static const double           DD = 0;                 // Default Double

        static const int DA_S[] = { 1, 2, 3, 4 };
        static const bsl::vector<int> DA(DA_S+0, DA_S+4, &ga);// Default Array

        static const int A1_S[] = { 5, 7 };
        static const bsl::vector<int> A1(A1_S+0, A1_S+2, &ga);

        static const int A2_S[] = { 74, 33, -2 };
        static const bsl::vector<int> A2(A2_S+0, A2_S+3, &ga);

        static const OptionInfo SPEC[] = {
            {
                "i|int",                     // tag
                "intOption",                 // name
                "int desc",                  // description
                TypeInfo(&myInt),            // flag option
                OccurrenceInfo(DI),          // occurrence required
                "BALCL_COMMANDLINE_MYINT"    // environment var name
            },
            {
                "d|double",                  // tag
                "doubleOption",              // name
                "double desc",               // description
                TypeInfo(&myDouble),         // flag option
                OccurrenceInfo(DD),          // occurrence required
                "BALCL_COMMANDLINE_MYDOUBLE" // environment var name
            },
            {
                "a|array",                   // tag
                "arrayOption",               // name
                "array desc",                // description
                TypeInfo(&myArray),          // flag option
                OccurrenceInfo(DA),          // occurrence required
                "BALCL_COMMANDLINE_MYARRAY"  // environment var name
            }
        };

        enum EnvVarSet { k_NONE= -1, k_INT = 0, k_DOUBLE, k_ARRAY };

        static const struct Data {
            int           d_line;
            int           d_expRc;   // expected `parse` return code
            int           d_expI;    // expected int
            double        d_expD;    // expected double
            const Array&  d_expA;    // expected array
            const char   *d_cmdLine; // command line
            EnvVarSet     d_envIdx;  // env idx:
                                     //  - -1: none
                                     //  -  0: int
                                     //  -  1: double
                                     //  -  2: array
            const char   *d_envVal;  // environment variable value
        } DATA[] = {
        //    V -- line
        //    |   V -- exp return value
        //    |   |   V -- exp `myInt` value
        //    |   |   |   V -- exp `myDouble` value
        //    |   |   |   |    V -- exp `myArray` value
        //    |   |   |   |    |   V -- command line (after prog name)
        //    |   |   |   |    |   |           V -- which env var
        //    V   V   V   V    V   V           V         V -- environment value

                        // correctly set on cmd line, no env variables

            { L_,  0, 27, DD,  DA, "-i=27",    k_NONE,   0 },
            { L_,  0, DI, 4.6, DA, "-d=4.6",   k_NONE,   0 },
            { L_,  0, DI, DD,  A2, "-a 74 -a 33 -a=-2",
                                               k_NONE,   0 },

                        // errors on cmd line, no env variables

            { L_, -1, 27, DD,   DA, "-i=woof", k_NONE,   0 },
            { L_, -1, DI, 4.6,  DA, "-d=woof", k_NONE,   0 },
            { L_, -1, DI, DD,   A2, "-a woof -a 33 -a=-2",
                                               k_NONE,   0 },

                        // errors on cmd line, correct via env variables

            { L_, -1, 27, DD,   DA, "-i=woof", k_INT,    "100" },
            { L_, -1, DI, 4.6,  DA, "-d=woof", k_DOUBLE, "10.4" },
            { L_, -1, DI, DD,   A2, "-a woof -a 33 -a=-2",
                                               k_ARRAY,  "1 2 3" },

                        // one option correct on cmd line, another via env

            { L_,  0, 27, 4.5,  DA, "-i=27",   k_DOUBLE, "4.5" },
            { L_,  0, DI, 4.6,  A1, "-d=4.6",  k_ARRAY,  "5 7" },
            { L_,  0, 100, DD,  A2, "-a 74 -a 33 -a=-2",
                                               k_INT,    "100" },

                        // correctly set on cmd line, same option correctly
                        // set to another via env, cmd line takes precedence

            { L_,  0, 27, DD,   DA, "-i=27",   k_INT,    "100" },
            { L_,  0, DI, 4.6,  DA, "-d=4.6",  k_DOUBLE, "10.4" },
            { L_,  0, DI, DD,   A2, "-a 74 -a 33 -a=-2",
                                               k_ARRAY,  "0 1 2 3 4" },

                        // correctly set on cmd line, same option error via
                        // env variables, env variables ignored

            { L_,  0, 27, DD,   DA, "-i=27",          k_INT,    "woof" },
            { L_,  0, DI, 4.6,  DA, "-d=4.6",         k_DOUBLE, "woof" },
            { L_,  0, DI, DD,   A2, "-a 74 -a 33 -a=-2",
                                                      k_ARRAY,  "woof m o"},
        };
        enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int ti = 0; ti < k_NUM_DATA; ++ti) {
            const Data&      data     = DATA[ti];
            const int        LINE     = data.d_line;
            const int        EXP_RC   = data.d_expRc;
            const int        EXP_I    = data.d_expI;
            const double     EXP_D    = data.d_expD;
            const Array      EXP_A(     data.d_expA, &ta);
            const char      *CMD_LINE = data.d_cmdLine;
            const EnvVarSet  ENV_IDX  = data.d_envIdx;
            const char      *ENV_VAL  = data.d_envVal;

            // set variables to garbage values

            bsl::memset(&myInt,    0xa5, sizeof(myInt));
            bsl::memset(&myDouble, 0xa5, sizeof(myDouble));
            myArray.clear();    myArray.insert(myArray.begin(), 100, myInt);

            u::unsetEnvironmentVariable("BALCL_COMMANDLINE_MYINT");
            u::unsetEnvironmentVariable("BALCL_COMMANDLINE_MYDOUBLE");
            u::unsetEnvironmentVariable("BALCL_COMMANDLINE_MYARRAY");

            switch (ENV_IDX) {
              case k_NONE: {
                ; // no env variable set
              } break;
              case k_INT: {
                u::setEnvironmentVariable("BALCL_COMMANDLINE_MYINT", ENV_VAL);
              } break;
              case k_DOUBLE: {
                u::setEnvironmentVariable("BALCL_COMMANDLINE_MYDOUBLE",
                                                                      ENV_VAL);
              } break;
              case k_ARRAY: {
                u::setEnvironmentVariable("BALCL_COMMANDLINE_MYARRAY",ENV_VAL);
              } break;
            }

            char cmdLine[256] = { "myprog " };
            bsl::strcat(cmdLine, CMD_LINE);

            int         argc;
            const char *argv[40];
            u::parseCommandLine(cmdLine, argc, argv);;

            bsl::ostringstream oss;

            Obj mX(SPEC, oss);

            const int rc = mX.parse(argc, argv, oss);
            ASSERTV(LINE, EXP_RC == rc);
            ASSERTV(LINE, oss.view(), (0 == rc) == oss.view().empty());
            if (0 != rc) {
                continue;
            }

            ASSERTV(LINE, EXP_I, myInt,    EXP_I == myInt);
            ASSERTV(LINE, EXP_D, myDouble, EXP_D == myDouble);
            if (EXP_A != myArray) {
                P_(LINE)    P_(EXP_A.size())    P_(myArray.size());
                for (unsigned uu = 0; uu < EXP_A.size(); ++uu) {
                    P_(EXP_A[uu]);
                }
                for (unsigned uu = 0; uu < myArray.size(); ++uu) {
                    P_(myArray[uu]);
                }
                ASSERT(EXP_A == myArray);
            }
        }
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // PARSING ARRAY ENVIRONMENT VARIABLES
        //
        // Concern:
        // 1. That parsing arrays in environment variables handles the
        //    separator character properly.
        //
        // 2. That parsing arrays in environment variables handles the escape
        //    character properly.
        //
        // Plan:
        // 1. Create a spec table with a single optional option of type string
        //    array, that can be set by environment variable.
        //
        // 2. Have a table of string values of the environment variable, and
        //    arrays of `const char *`s to the string values that are expected
        //    to be in the string array, if parsed correctly, and an array
        //    separator character which, if '\0' (an illegal value), means do
        //    not set it an let it defaullt to space.
        //
        // 3. Create a stream `oss` that will record any output streamed by
        //    the command line parser, and use it to confirm that no output
        //    occurred while parsing.
        //
        // 4. Create a command line parser and call with an `argv` with a
        //    program name and no command line arguments.
        //
        // 5. Confirm that the array has the expected number of elements with
        //    the expected values.
        //
        // Testing:
        //   PARSING ARRAY ENVIRONMENT VARIABLES
        // --------------------------------------------------------------------

        if (verbose) cout << "Parsing Array Environment Variables\n"
                             "===================================\n";

        using bsl::size_t;

        enum { k_MAX_NUM_ELEMENTS = 4 };

        bsl::vector<bsl::string> array(&ta);

        static const balcl::OptionInfo specTable[] = {
            { "s|strings",
              "stringArray",
              "array of strings",
              balcl::TypeInfo(&array, &ga),
              balcl::OccurrenceInfo::e_OPTIONAL,
              "STRING_ARRAY"
            }
        };

        const bool T = true, F = false;

        static const struct Data {
            int         d_line;
            bool        d_success;
            const char *d_envVal;
            const char *d_expElements[k_MAX_NUM_ELEMENTS + 1];
        } DATA[] = {
        //        V -- success (`T`) or failure (`F`)
        //        |  V -- environment variable value
        //        V  V                    V -- null-terminated array val
            { L_, T, "woof meow arf",     { "woof", "meow", "arf", 0 } },
            { L_, T, "grrrr bow\\ wow",   { "grrrr", "bow wow", 0 } },
            { L_, F, "grrrr \\bow\\ wow", { "grrrr", "bow wow", 0 } },
            { L_, F, "grrrr bow\\ wow\\", { "grrrr", "bow wow", 0 } },
            { L_, F, "grrrr bow\\&wow",   { "grrrr", "bow&wow", 0 } },
            { L_, T, "grrrr\\  bow&wow",  { "grrrr ", "bow&wow", 0 } },
            { L_, T, "a\\\\b c d",        { "a\\b", "c", "d", 0 } },
        };
        enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int ti = 0; ti < k_NUM_DATA; ++ti) {
            const Data&         data         = DATA[ti];
            const int           LINE         = data.d_line;
            const bool          SUCCESS      = data.d_success;
            const char         *ENV_VAL      = data.d_envVal;
            const char * const *EXP_ELEMENTS = data.d_expElements;
            const size_t        NUM_ELEMENTS =
                               bsl::find(EXP_ELEMENTS,
                                         EXP_ELEMENTS + k_MAX_NUM_ELEMENTS + 1,
                                         (const char *) 0) - EXP_ELEMENTS;

            ASSERT(0 < NUM_ELEMENTS);
            ASSERT(NUM_ELEMENTS <= k_MAX_NUM_ELEMENTS);
            ASSERT(0 == EXP_ELEMENTS[NUM_ELEMENTS]);
            ASSERT(0 != EXP_ELEMENTS[NUM_ELEMENTS - 1]);

            array.clear();

            u::setEnvironmentVariable("STRING_ARRAY", ENV_VAL);

            bsl::ostringstream oss(&ta);
            balcl::CommandLine cmdLine(specTable, oss, &ta);

            const char *argv[] = { "myprog" };
            int rc = cmdLine.parse(1, argv, oss);
            ASSERT((0 == rc) == SUCCESS);
            if (rc) {
                continue;
            }
            ASSERT(oss.view().empty());

            ASSERT(NUM_ELEMENTS == array.size());
            for (unsigned uu = 0; uu < NUM_ELEMENTS; ++uu) {
                ASSERTV(LINE, uu, array[uu], EXP_ELEMENTS[uu],
                                                array[uu] == EXP_ELEMENTS[uu]);
            }
        }
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // CONCERN: READING ENVIRONMENT VARIABLES
        //
        // Concern:
        // 1. That `parse` reads environment variables, if set, and if the
        //    corresponding option is not set on the command line.
        //
        // 2. That environment variables, if set, override default values.
        //
        // 3. Environment variables can be used to set optional or required
        //    options.
        //
        // 4. That linked variables are correctly set by environment variables
        //    when appropriate.
        //
        // 5. That when an option is set by environment variables, its
        //    `position` is -2.
        //
        // Plan:
        // 1. Create an `OptionInfo` array with two `string` options, a
        //    required one and an optional one, both with environment variable
        //    names.
        //
        // 2. Verify that neither of the environment variables are set.
        //
        // 3. Declare an array of 4 `argv` arrays of arguments with all 4
        //    possible combination of command-line arguments?
        //
        // 4. With the environment variables still unset, declare 4
        //    `CommandLine` objects using the declared `OptionInfo` array.
        //
        // 5. Parse the 4 command line objects with the 4 `argv` arrays,
        //    observe that the objects parsed without required arguments fail
        //    to parse.
        //
        // 6. Verify that the state, post-parsing, is as expected.
        //
        // 7. Set the 2 environment variables.
        //
        // 8. In a new block, declare the 4 `CommandLine` objects again, and
        //    call parse on them with the 4 `argv` arrays.  This time, all 4
        //    parses should succeed.
        //
        // 9. Verify the state, post-parsing, is as expected.
        //
        // Testing:
        //   ENVIRONMENT VARIABLES
        // --------------------------------------------------------------------

        if (verbose) cout << "CONCERN: READING ENVIRONMENT VARIABLES\n"
                             "======================================\n";

        {
            bsl::string reqString, optString;
            int pos;

            const OptionInfo SPEC_TABLE[] = {
                {
                    "r|required",                // tag
                    "requiredOption",            // name
                    "Oxymoron",                  // description
                    TypeInfo(&reqString),        // flag option
                    OccurrenceInfo::e_REQUIRED,  // occurrence required
                    "BALCL_COMMANDLINE_REQUIRED" // environment var name
                }
              , {
                    "o|optional",                // tag
                    "optionalOption",            // name
                    "Redundant",                 // description
                    TypeInfo(&optString),        // string option
                    OccurrenceInfo(bsl::string("grrr")),    // default value
                    "BALCL_COMMANDLINE_OPTIONAL" // environment var name
                }
            };

            ASSERT(!bsl::getenv("BALCL_COMMANDLINE_REQUIRED"));
            ASSERT(!bsl::getenv("BALCL_COMMANDLINE_OPTIONAL"));

            ASSERT(Obj::isValidOptionSpecificationTable(SPEC_TABLE));

            const char *const noFlags[]   = { "programName"        };
            const char *const opt1Flags[] = { "programName", "-o", "meow" };
            const char *const opt2Flags[] = { "programName", "-r", "woof" };
            const char *const allFlags[]  = { "programName",
                                              "-r", "woof", "-o", "meow" };

            {
                Obj mA(SPEC_TABLE);
                Obj mB(SPEC_TABLE);
                Obj mC(SPEC_TABLE); const Obj& C = mC;
                Obj mD(SPEC_TABLE); const Obj& D = mD;

                ASSERT(0 != mA.parse(1,   noFlags));
                reqString.clear();    optString.clear();

                ASSERT(0 != mB.parse(3, opt1Flags));
                reqString.clear();    optString.clear();

                ASSERT(0 == mC.parse(3, opt2Flags));

                ASSERT( C.isSpecified("requiredOption"));
                ASSERT(!C.isSpecified("optionalOption"));
                ASSERT(reqString == "woof");
                ASSERT(optString == "grrr");
                ASSERT( C.theString("requiredOption") == "woof");
                ASSERT( C.theString("optionalOption") == "grrr");
                ASSERTV(pos, (pos = C.position("requiredOption")) == 1);
                ASSERTV(pos, (pos = C.position("optionalOption")) == -1);

                reqString.clear();    optString.clear();

                ASSERT(0 == mD.parse(5,  allFlags));

                ASSERT( D.isSpecified("requiredOption"));
                ASSERT( D.isSpecified("optionalOption"));
                ASSERT(reqString == "woof");
                ASSERT(optString == "meow");
                ASSERT( D.theString("requiredOption") == "woof");
                ASSERT( D.theString("optionalOption") == "meow");
                ASSERTV(pos, (pos = D.position("requiredOption")) == 1);
                ASSERTV(pos, (pos = D.position("optionalOption")) == 3);

                reqString.clear();    optString.clear();
            }

            u::setEnvironmentVariable("BALCL_COMMANDLINE_REQUIRED", "arf");
            u::setEnvironmentVariable("BALCL_COMMANDLINE_OPTIONAL",
                                                                    "bow wow");

            {
                Obj mA(SPEC_TABLE); const Obj& A = mA;
                Obj mB(SPEC_TABLE); const Obj& B = mB;
                Obj mC(SPEC_TABLE); const Obj& C = mC;
                Obj mD(SPEC_TABLE); const Obj& D = mD;

                ASSERT(0 == mA.parse(1,   noFlags));

                ASSERT( A.isSpecified("requiredOption"));
                ASSERT( A.isSpecified("optionalOption"));
                ASSERT(reqString == "arf");
                ASSERT(optString == "bow wow");
                ASSERT( A.theString("requiredOption") == "arf");
                ASSERT( A.theString("optionalOption") == "bow wow");
                ASSERTV(pos, (pos = A.position("requiredOption")) == -2);
                ASSERTV(pos, (pos = A.position("optionalOption")) == -2);

                reqString.clear();    optString.clear();

                ASSERT(0 == mB.parse(3, opt1Flags));

                ASSERT( B.isSpecified("requiredOption"));
                ASSERT( B.isSpecified("optionalOption"));
                ASSERT(reqString == "arf");
                ASSERT(optString == "meow");
                ASSERT( B.theString("requiredOption") == "arf");
                ASSERT( B.theString("optionalOption") == "meow");
                ASSERTV(pos, (pos = B.position("requiredOption")) == -2);
                ASSERTV(pos, (pos = B.position("optionalOption")) == 1);

                reqString.clear();    optString.clear();

                ASSERT(0 == mC.parse(3, opt2Flags));

                ASSERT( C.isSpecified("requiredOption"));
                ASSERT( C.isSpecified("optionalOption"));
                ASSERT(reqString == "woof");
                ASSERT(optString == "bow wow");
                ASSERT( C.theString("requiredOption") == "woof");
                ASSERT( C.theString("optionalOption") == "bow wow");
                ASSERTV(pos, (pos = C.position("requiredOption")) == 1);
                ASSERTV(pos, (pos = C.position("optionalOption")) == -2);

                reqString.clear();    optString.clear();

                int rcd = mD.parse(5,  allFlags);
                ASSERT(0 == rcd);

                ASSERT( D.isSpecified("requiredOption"));
                ASSERT( D.isSpecified("optionalOption"));
                ASSERT(reqString == "woof");
                ASSERT(optString == "meow");
                ASSERT( D.theString("requiredOption") == "woof");
                ASSERT( D.theString("optionalOption") == "meow");
                ASSERTV(pos, (pos = D.position("requiredOption")) == 1);
                ASSERTV(pos, (pos = D.position("optionalOption")) == 3);

                reqString.clear();    optString.clear();
            }

            u::unsetEnvironmentVariable("BALCL_COMMANDLINE_REQUIRED");
            u::unsetEnvironmentVariable("BALCL_COMMANDLINE_OPTIONAL");
        }
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // DRQS 166843299
        //   `balcl_commandline`: runtime error observed.
        //
        // Concerns:
        // 1. The observed runtime error is (first) reproduced and is no longer
        //    observed in the updated component.
        //
        // 2. Flags specified with `e_REQUIRED` behave the same as flags
        //    specified with `e_OPTIONAL`.
        //
        // 3. Options with `e_REQUIRED` are allowed for all option values.
        //
        // Plan:
        // 1. Create a test case that demonstrated the problem (core dump)
        //    before the fix but now passes.  (C-1)
        //
        // 2. Create an option specification having two flags: one specified
        //    with `e_REQUIRED`, the other with `e_OPTIONAL`.  Use the
        //    specification in two scenarios: one where the flags are supplied
        //    on the command line, the other where they are not.  Confirm that
        //    the flag with `e_OPTIONAL` shows the expected value in both
        //    scenarios and the flag with `e_REQUIRED` always shows a value
        //    that matches the other flag.  (C-2)
        //
        // 3. Create a series of option specifications where `e_REQUIRED` is
        //    specified in each case while the option type takes on all allowed
        //    types.  Show that the `isValidOptionSpecificationTable` function
        //    returns `true` for each of these option specifications.  (C-3)
        //
        // Testing:
        //   CONCERN: DRQS 166843299
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "DRQS 166843299" << endl
                          << "==============" << endl;

        if (veryVerbose) {
            cout << "Demonstrate the presence/absence of the runtime error."
                 << endl;
        }
        {
            const OptionInfo SPEC_TABLE[] = {
                {
                    "r|reverse",                 // tag
                    "isReverse",                 // name
                    "sort in reverse order",     // description
                    TypeInfo(Ot::k_BOOL),        // flag option
                    OccurrenceInfo::e_REQUIRED,  // occurrence required
                    ""                           // env var name
                }
            };

            ASSERT(true == Obj::isValidOptionSpecificationTable(SPEC_TABLE));

            Obj obj(SPEC_TABLE);

            const char *const  emptyCommandLine[] = { "programName" };

            int rc = obj.parse(1, emptyCommandLine);
            ASSERT(0 == rc);
        }

        if (veryVerbose) {
            cout << "Flags with `e_REQUIRED` or `e_OPTIONAL` behave same."
                 << endl;
        }
        {
            const OptionInfo SPEC_TABLE[] = {
                {
                    "r|required",                // tag
                    "requiredOption",            // name
                    "Oxymoron",                  // description
                    TypeInfo(Ot::k_BOOL),        // flag option
                    OccurrenceInfo::e_REQUIRED,  // occurrence required
                    ""                           // env var name
                }
              , {
                    "o|optional",                // tag
                    "optionalOption",            // name
                    "Redundant",                 // description
                    TypeInfo(Ot::k_BOOL),        // flag option
                    OccurrenceInfo::e_OPTIONAL,  // occurrence required
                    ""                           // env var name
                }
            };

            ASSERT(Obj::isValidOptionSpecificationTable(SPEC_TABLE));

            const char *const sansFlags[] = { "programName"        };
            const char *const avecFlags[] = { "programName", "-ro" };

            Obj mX(SPEC_TABLE); const Obj& X = mX;
            Obj mY(SPEC_TABLE); const Obj& Y = mY;

            int rcx = mX.parse(1, sansFlags);
            int rcy = mY.parse(2, avecFlags);

            ASSERT(0 == rcx);
            ASSERT(0 == rcy);

            ASSERT(false == X.theBool("requiredOption"));
            ASSERT(false == X.theBool("optionalOption"));

            ASSERT(true  == Y.theBool("requiredOption"));
            ASSERT(true  == Y.theBool("optionalOption"));
        }

        if (veryVerbose) {
            cout << "Check that `e_REQUIRED` and be configured for each type."
                 << endl;
        }
        {
            const TypeInfo    ARRAY[] = { TypeInfo(Ot::k_BOOL)
                                        , TypeInfo(Ot::k_CHAR)
                                        , TypeInfo(Ot::k_INT)
                                        , TypeInfo(Ot::k_INT64)
                                        , TypeInfo(Ot::k_DOUBLE)
                                        , TypeInfo(Ot::k_STRING)
                                        , TypeInfo(Ot::k_DATETIME)
                                        , TypeInfo(Ot::k_DATE)
                                        , TypeInfo(Ot::k_TIME)
                                        , TypeInfo(Ot::k_CHAR_ARRAY)
                                        , TypeInfo(Ot::k_INT_ARRAY)
                                        , TypeInfo(Ot::k_INT64_ARRAY)
                                        , TypeInfo(Ot::k_DOUBLE_ARRAY)
                                        , TypeInfo(Ot::k_STRING_ARRAY)
                                        , TypeInfo(Ot::k_DATETIME_ARRAY)
                                        , TypeInfo(Ot::k_DATE_ARRAY)
                                        , TypeInfo(Ot::k_TIME_ARRAY)
                                        };
            const bsl::size_t numARRAY = sizeof ARRAY / sizeof *ARRAY;

            for (bsl::size_t i = 0; i < numARRAY; ++i) {
                const TypeInfo   TYPE_INFO = ARRAY[i];
                const OptionInfo optInfo   = { "f|flag"
                                             , "flagName"
                                             , "Flag description."
                                             , TYPE_INFO
                                             , OccurrenceInfo::e_REQUIRED
                                             , ""
                                             };
                if (veryVerbose) {
                    P_(i) P_(optInfo.d_typeInfo) P(optInfo.d_defaultInfo)
                }

                ASSERT(Obj::isValidOptionSpecificationTable(&optInfo, 1));
            }
        }
      } break;
      case 20: {
        ;    // Unused
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING OPTIONAL LINKED VARIABLES
        //
        // Concerns:
        // 1. Linked variables to `bsl::optional` objects (optional linked
        //    variables) can be configured for each of the eight supported
        //    types.
        //
        // 2. Optional linked variables are left in a no-value state if
        //    associated options are not specified on the command line (input
        //    to the `parse` method).
        //
        // 3. Optional linked variables are left with the expected value when
        //    the command line is populated with fields for the associated
        //    option.
        //
        // 4. Optional linked variables do not interfere with the operation of
        //    non-optional linked variables to the 17 supported option types.
        //
        // 5. The `validate` method, called either explicitly or implicitly in
        //    the constructor, detects disallowed configurations of optional
        //    linked variables:
        //    - optional linked variables for "required" option.
        //    - optional linked variables with a default value.
        //    - optional linked variables for a `bool` option (flag).
        //
        // Plan:
        // 1. Create a specification table featuring options for each of the 17
        //    supported option types, each having a linked variable to the same
        //    type.  The 8 option types that allow links to `bsl::optional`
        //    objects have a second option entry so there are 25 entries in
        //    total.  Use the specification table to construct a
        //    `balcl::CommandLine` object.  (C-1,4).
        //
        // 2. Confirm that each of the `bsl::optional` objects are in a
        //    no-value state (`false == has_value()`).  Initialize each of the
        //    other linked objects to a distinct non-default value except for
        //    the `bool` option that is initialized to `false`.  Construct
        //    command-line input in which *none* of the options defined in the
        //    specification table are mentioned and pass that input to the
        //    `parse` method.  Afterwards, confirm that each of the optional
        //    linked variables are still in a no-value state (i.e.,
        //    `false == has_value()`) and that each of the other linked objects
        //    has their initial value.  (C-2, 4)
        //
        // 3. Construct command-line input having a field that specifies a
        //    value for each of the options having optional linked variables,
        //    each distinct from the initial value.  Pass that input to the
        //    `parse` method of a (freshly constructed) `balcl::CommandLine`
        //    object.  Confirm that each of the linked variables has a value
        //    and that the value matches that expected based on the field input
        //    and that the non-`bsl::optional` linked object has its expected
        //    value.  (C-3, 4)
        //
        // 4. Create a specification table that intentionally defines the
        //    invalid combinations of optional linked variables and
        //    incompatible occurrence objects (i.e., `e_REQUIRED` or having a
        //    default value).  Confirm that these specification fail the
        //    `validation` method and generate the expected error message.
        //    (C-5)
        //
        // Testing:
        //   TESTING OPTIONAL LINKED VARIABLES
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING OPTIONAL LINKED VARIABLES" << endl
                          << "=================================" << endl;

        ASSERT(!oLinkedChar    .has_value());
        ASSERT(!oLinkedInt     .has_value());
        ASSERT(!oLinkedInt64   .has_value());
        ASSERT(!oLinkedDouble  .has_value());
        ASSERT(!oLinkedString  .has_value());
        ASSERT(!oLinkedDatetime.has_value());
        ASSERT(!oLinkedDate    .has_value());
        ASSERT(!oLinkedTime    .has_value());

        // Initialize each with a non-default value.
        linkedBool          =  false;  // `bool` option requires `false`
        linkedChar          =  valueChar;
        linkedInt           =  valueInt;
        linkedInt64         =  valueInt64;
        linkedDouble        =  valueDouble;
        linkedString        =  valueString;
        linkedDatetime      =  valueDatetime;
        linkedDate          =  valueDate;
        linkedTime          =  valueTime;
        linkedCharArray     =  valueCharArray;
        linkedIntArray      =  valueIntArray;
        linkedInt64Array    =  valueInt64Array;
        linkedDoubleArray   =  valueDoubleArray;
        linkedStringArray   =  valueStringArray;
        linkedDatetimeArray =  valueDatetimeArray;
        linkedDateArray     =  valueDateArray;

#define OPTINFO_ENTRY(LETTER, ADDRESS)                                        \
            {   #LETTER"|"#LETTER"Long",                                      \
                "Name_"#LETTER,                                               \
                "Description for "#LETTER,                                    \
                TypeInfo(ADDRESS),                                            \
                OccurrenceInfo::e_OPTIONAL,                                   \
                ""                                                            \
            }                                                                 \

        const OptionInfo SPECS[] = {

            // links to `bsl::optional` objects
            OPTINFO_ENTRY(a, &oLinkedChar)
          , OPTINFO_ENTRY(b, &oLinkedInt)
          , OPTINFO_ENTRY(c, &oLinkedInt64)
          , OPTINFO_ENTRY(d, &oLinkedDouble)
          , OPTINFO_ENTRY(e, &oLinkedString)
          , OPTINFO_ENTRY(f, &oLinkedDatetime)
          , OPTINFO_ENTRY(g, &oLinkedDate)
          , OPTINFO_ENTRY(h, &oLinkedTime)

            // links to non-`bsl::optional` objects
          , OPTINFO_ENTRY(i, &linkedBool)
          , OPTINFO_ENTRY(j, &linkedChar)
          , OPTINFO_ENTRY(k, &linkedInt)
          , OPTINFO_ENTRY(l, &linkedInt64)
          , OPTINFO_ENTRY(m, &linkedDouble)
          , OPTINFO_ENTRY(n, &linkedString)
          , OPTINFO_ENTRY(o, &linkedDatetime)
          , OPTINFO_ENTRY(p, &linkedDate)
          , OPTINFO_ENTRY(q, &linkedTime)
          , OPTINFO_ENTRY(r, &linkedCharArray)
          , OPTINFO_ENTRY(s, &linkedIntArray)
          , OPTINFO_ENTRY(t, &linkedInt64Array)
          , OPTINFO_ENTRY(u, &linkedDoubleArray)
          , OPTINFO_ENTRY(v, &linkedStringArray)
          , OPTINFO_ENTRY(w, &linkedDatetimeArray)
          , OPTINFO_ENTRY(x, &linkedDateArray)
          , OPTINFO_ENTRY(y, &linkedTimeArray)
        };

#undef OPTINFO_ENTRY

        bsl::ostringstream oss;

        ASSERT(balcl::CommandLine::isValidOptionSpecificationTable(SPECS,
                                                                   oss));
        ASSERT(oss.str().empty());

        if (verbose) cout << "No Option unspecified." << endl;

        Obj mX(SPECS); const Obj& X = mX;

        const char *const    emptyCommandLine[] = { "programName" };
        const bsl::size_t numEmptyCommandLine = sizeof  emptyCommandLine
                                              / sizeof *emptyCommandLine;

        int retParseX = mX.parse(numEmptyCommandLine, emptyCommandLine, oss);

        ASSERT(0 == retParseX);
        ASSERT(oss.str().empty());
        ASSERT(X.isParsed());
        ASSERT(X.isValid());

        ASSERT(!oLinkedChar    .has_value());
        ASSERT(!oLinkedInt     .has_value());
        ASSERT(!oLinkedInt64   .has_value());
        ASSERT(!oLinkedDouble  .has_value());
        ASSERT(!oLinkedString  .has_value());
        ASSERT(!oLinkedDatetime.has_value());
        ASSERT(!oLinkedDate    .has_value());
        ASSERT(!oLinkedTime    .has_value());

        ASSERT(linkedBool          ==  false);  // unchanged
        ASSERT(linkedChar          ==  valueChar);
        ASSERT(linkedInt           ==  valueInt);
        ASSERT(linkedInt64         ==  valueInt64);
        ASSERT(linkedDouble        ==  valueDouble);
        ASSERT(linkedString        ==  valueString);
        ASSERT(linkedDatetime      ==  valueDatetime);
        ASSERT(linkedDate          ==  valueDate);
        ASSERT(linkedTime          ==  valueTime);
        ASSERT(linkedCharArray     ==  valueCharArray);
        ASSERT(linkedIntArray      ==  valueIntArray);
        ASSERT(linkedInt64Array    ==  valueInt64Array);
        ASSERT(linkedDoubleArray   ==  valueDoubleArray);
        ASSERT(linkedStringArray   ==  valueStringArray);
        ASSERT(linkedDatetimeArray ==  valueDatetimeArray);
        ASSERT(linkedDateArray     ==  valueDateArray);

        if (verbose) cout << "Each option specified." << endl;

        const bool           argBool   = true;
        const char           argChar   = 'a';
        const int            argInt    = 123654;
        const Int64          argInt64  = 987654321;
        const double         argDouble = 0.376739501953125;
        const bsl::string    argString  ("someString");
        const bdlt::Datetime argDatetime(2008, 7, 22,  4,  6,  8);
        const bdlt::Date     argDate    (2007, 8, 22);
        const bdlt::Time     argTime                 ( 8,  6,  4);

        const bsl::vector<char>           argCharArray    (2, argChar);
        const bsl::vector<int>            argIntArray     (2, argInt);
        const bsl::vector<Int64>          argInt64Array   (2, argInt64);
        const bsl::vector<double>         argDoubleArray  (2, argDouble);
        const bsl::vector<bsl::string>    argStringArray  (2, argString);
        const bsl::vector<bdlt::Datetime> argDatetimeArray(2, argDatetime);
        const bsl::vector<bdlt::Date>     argDateArray    (2, argDate);
        const bsl::vector<bdlt::Time>     argTimeArray    (2, argTime);

        ASSERT(linkedBool          != argBool);
        ASSERT(linkedChar          != argChar);
        ASSERT(linkedInt           != argInt);
        ASSERT(linkedInt64         != argInt64);
        ASSERT(linkedDouble        != argDouble);
        ASSERT(linkedString        != argString);
        ASSERT(linkedDatetime      != argDatetime);
        ASSERT(linkedDate          != argDate);
        ASSERT(linkedTime          != argTime);
        ASSERT(linkedCharArray     != argCharArray);
        ASSERT(linkedIntArray      != argIntArray);
        ASSERT(linkedInt64Array    != argInt64Array);
        ASSERT(linkedDoubleArray   != argDoubleArray);
        ASSERT(linkedStringArray   != argStringArray);
        ASSERT(linkedDatetimeArray != argDatetimeArray);
        ASSERT(linkedDateArray     != argDateArray);
        ASSERT(linkedTimeArray     != argTimeArray);

        const char *const populatedCommandLine[] = {
               "programName"

               // options linked to `bsl::optional` objects
             , "-a", "a"
             , "-b", "123654"
             , "-c", "987654321"
             , "-d", "0.376739501953125"
             , "-e", "someString"
             , "-f", "2008-07-22T04:06:08"
             , "-g", "2007-08-22"
             , "-h", "08:06:04"

               // scalar options linked to non-`bsl::optional` objects
             , "-i"  // `bool` option (flag)
             , "-j", "a"
             , "-k", "123654"
             , "-l", "987654321"
             , "-m", "0.376739501953125"
             , "-n", "someString"
             , "-o", "2008-07-22T04:06:08"
             , "-p", "2007-08-22"
             , "-q", "08:06:04"

               // array options linked to non-`bsl::optional` objects
             , "-r", "a",                   "-r", "a"
             , "-s", "123654",              "-s", "123654"
             , "-t", "987654321",           "-t", "987654321"
             , "-u", "0.376739501953125",   "-u", "0.376739501953125"
             , "-v", "someString",          "-v", "someString"
             , "-w", "2008-07-22T04:06:08", "-w", "2008-07-22T04:06:08"
             , "-x", "2007-08-22",          "-x", "2007-08-22"
             , "-y", "08:06:04",            "-y", "08:06:04"
             };

        const bsl::size_t numPopulatedCommandLine
                                               = sizeof  populatedCommandLine
                                               / sizeof *populatedCommandLine;

        Obj mY(SPECS); const Obj& Y = mY;

        int retParseY = mY.parse(numPopulatedCommandLine,
                                 populatedCommandLine,
                                 oss);

        ASSERT(0 == retParseY);
        ASSERT(oss.str().empty());
        ASSERT(Y.isParsed());
        ASSERT(Y.isValid());

        ASSERT(oLinkedChar    .has_value());
        ASSERT(oLinkedInt     .has_value());
        ASSERT(oLinkedInt64   .has_value());
        ASSERT(oLinkedDouble  .has_value());
        ASSERT(oLinkedString  .has_value());
        ASSERT(oLinkedDatetime.has_value());
        ASSERT(oLinkedDate    .has_value());
        ASSERT(oLinkedTime    .has_value());

        ASSERT(oLinkedChar          == argChar);
        ASSERT(oLinkedInt           == argInt);
        ASSERT(oLinkedInt64         == argInt64);
        ASSERT(oLinkedDouble        == argDouble);
        ASSERT(oLinkedString        == argString);
        ASSERT(oLinkedDatetime      == argDatetime);
        ASSERT(oLinkedDate          == argDate);
        ASSERT(oLinkedTime          == argTime);

        ASSERT( linkedBool          == argBool);
        ASSERT( linkedChar          == argChar);
        ASSERT( linkedInt           == argInt);
        ASSERT( linkedInt64         == argInt64);
        ASSERT( linkedDouble        == argDouble);
        ASSERT( linkedString        == argString);
        ASSERT( linkedDatetime      == argDatetime);
        ASSERT( linkedDate          == argDate);
        ASSERT( linkedTime          == argTime);

        ASSERT( linkedCharArray     == argCharArray);
        ASSERT( linkedIntArray      == argIntArray);
        ASSERT( linkedInt64Array    == argInt64Array);
        ASSERT( linkedDoubleArray   == argDoubleArray);
        ASSERT( linkedStringArray   == argStringArray);
        ASSERT( linkedDatetimeArray == argDatetimeArray);
        ASSERT( linkedDateArray     == argDateArray);
        ASSERT( linkedTimeArray     == argTimeArray);

        if (verbose) cout << "Testing invalid specs." << endl;

#ifdef BDE_BUILD_TARGET_EXC
        bsls::Assert::setFailureHandler(&u::throwInvalidSpec);

        static const struct {
            int         d_line;
            int         d_numSpecs;
            OptionInfo  d_specTable[MAX_SPEC_SIZE];
            const char *d_message_p;
        } DATA[] = {
#define NL "\n"
            { L_, 1, {
                          {
                              "a|aLong",                       // tag-option
                              "Name_a",                        // name
                              "Description_a",                 // description
                              TypeInfo(&oLinkedInt),
                              OccurrenceInfo::e_REQUIRED,
                              ""                               // env var name
                          }  // optional link and "required".
                      }
            , "Link to 'bsl::optional' object disallowed "
              "for option configured as \"required\"."                       NL
              "1st option."                                                  NL
            }
          , { L_, 1, {
                          {
                              "a|aLong",                       // tag-option
                              "Name_a",                        // name
                              "Description_a",                 // description
                              TypeInfo(&oLinkedString),
                              OccurrenceInfo(bsl::string("default-string")),
                              ""                               // env var name
                          }  // optional link and default value
                      }
            , "Link to 'bsl::optional' object disallowed "
              "for option having a configured default value."                NL
              "1st option."                                                  NL
            }
#undef NL
        };

        bsl::size_t NUM_DATA = sizeof DATA/ sizeof *DATA;

        for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {

            const int                LINE     = DATA[ti].d_line;
            const int                NUM_SPEC = DATA[ti].d_numSpecs;
            const OptionInfo * const SPEC     = DATA[ti].d_specTable;
            const char       * const MSG      = DATA[ti].d_message_p;

            ASSERTV(LINE, NUM_SPEC <= MAX_SPEC_SIZE);

            if (veryVerbose) {
                T_ P_(LINE) P(NUM_SPECS)
                if (0 < NUM_SPEC) { T_ T_ P(SPEC[0]) }
                if (1 < NUM_SPEC) { T_ T_ P(SPEC[1]) }
                if (2 < NUM_SPEC) { T_ T_ P(SPEC[2]) }
                if (3 < NUM_SPEC) { T_ T_ P(SPEC[3]) }
            }

            bsl::stringstream ossValidate;

            ASSERTV(LINE,
                    false == Obj::isValidOptionSpecificationTable(
                                                                 SPEC,
                                                                 NUM_SPEC,
                                                                 ossValidate));
            ASSERTV(LINE,
                    false == Obj::isValidOptionSpecificationTable(SPEC,
                                                                  NUM_SPEC));
            ASSERTV(LINE,
                    MSG,   ossValidate.str(),
                    MSG == ossValidate.str());

            bsl::stringstream oss;
            bool              exceptionCaught = false;

            try {
                Obj mX(SPEC, NUM_SPEC, oss);
            }
            catch (const u::InvalidSpec& e) {
                if (veryVerbose) { T_ T_ P(oss.str()) }
                exceptionCaught = true;
            }

            ASSERTV(LINE, exceptionCaught);
            ASSERTV(LINE, ossValidate.str() == oss.str());
        }
#else
        if (verbose) cout
                      << endl
                      << "===========================================" << endl
                      << "Skipping Test: invalid optional link specs." << endl
                      << "===========================================" << endl;

#endif // BDE_BUILD_TARGET_EXC

      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING `printUsage`
        //
        // Concerns:
        // 1. The `printUsage` method properly formats the given variables.
        //
        // 2. Names of flags are omitted.
        //
        // 3. Empty non-option argument tags are omitted.
        //
        // 4. All tags/names/descriptions are indented equally.
        //
        // 5. Tags/names/descriptions exceeding 80 characters are handled
        //    correctly.
        //
        // 6. Exceeding the limit of exactly 30 characters triggers the
        //    shifting mechanism.
        //
        // 7. All descriptions are shifted to a new line simultaneously (even
        //    if only one tag/name pair exceed the limit).
        //
        // 8. Exceeding the limit by the hidden `e_FLAG` name does not
        //    trigger the mechanism.
        //
        // 9. Multiple description lines are indented equally.
        //
        // 10. Overloads that do not accept a `stream` argument write to
        //    `bsl::cerr`.
        //
        // 11. After `parse` returns successfully, `argv[0]` is used in lieu of
        //    the default "programName" but the value supplied to the
        //    `printUsage" overloads that accept `programName' always has
        //    precedence.
        //
        // Plan:
        // 1. Set up a variety of variables and options and verify that
        //    `printUsage` formats them correctly.
        //
        // 2. Configure `bsl::cerr` to write to a `bsl::ostringstream` and
        //    confirm the expected results.  (C-10)
        //
        // 3. Invoke `printUsage` both before and after `parse` and via
        //    overloads that take a user-supplied value, and not.  Confirm
        //    expected result in each case.  (C-11)
        //
        // Testing:
        //   void printUsage() const;
        //   void printUsage(const string& pName);
        //   void printUsage(bsl::ostream& stream) const;
        //   void printUsage(bsl::ostream& stream, const string& pName) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING `printUsage`" << endl
                                  << "====================" << endl;

        if (veryVerbose) cout << "Complex case\n";
        {
            balcl::OptionInfo specTable[] = {
              {
                "r|reverse",
                "isReverse",
                "sort in reverse order",
                balcl::TypeInfo(Ot::k_BOOL),
                balcl::OccurrenceInfo(),
                ""
              },
              {
                "i|insensitivetocase",
                "isCaseInsensitive",
                "be case insensitive while sorting",
                balcl::TypeInfo(Ot::k_BOOL),
                balcl::OccurrenceInfo(),
                ""
              },
              {
                "u|uniq",
                "isUniq",
                "discard duplicate lines",
                balcl::TypeInfo(Ot::k_BOOL),
                balcl::OccurrenceInfo(),
                ""
              },
              {
                "a|algorithm",
                "sortAlgo",
                "sorting algorithm",
                balcl::TypeInfo(Ot::k_STRING),
                balcl::OccurrenceInfo(bsl::string("quickSort")),
                ""
              },
              {
                "o|outputfile",
                "outputFile",
                "output file with a very long option description so we can "
                "see the line wrapping behavior",
                balcl::TypeInfo(Ot::k_STRING),
                balcl::OccurrenceInfo::e_REQUIRED,
                ""
              },
              {
                "",
                "fileList",
                "files to be sorted",
                balcl::TypeInfo(Ot::k_STRING_ARRAY),
                balcl::OccurrenceInfo::e_REQUIRED,
                ""
              }
            };

            balcl::CommandLine cmdLine(specTable);

            bsl::stringstream ss;
            cmdLine.printUsage(ss);                                     // TEST

            bsl::streambuf     *errorStreambuf = bsl::cerr.rdbuf();
            bsl::ostringstream  es;
            bsl::cerr.rdbuf(es.rdbuf());
            cmdLine.printUsage();                                       // TEST

            if (veryVerbose) cout << ss.str() << endl;

            const char *EXPECTED = "\n"
"Usage: programName [-r|reverse] [-i|insensitivetocase] [-u|uniq]\n"
"                   [-a|algorithm <sortAlgo>] -o|outputfile <outputFile>\n"
"                   [<fileList>]+\n"
"Where:\n"
"  -r | --reverse\n"
"          sort in reverse order\n"
"  -i | --insensitivetocase\n"
"          be case insensitive while sorting\n"
"  -u | --uniq\n"
"          discard duplicate lines\n"
"  -a | --algorithm          <sortAlgo>\n"
"          sorting algorithm (default: quickSort)\n"
"  -o | --outputfile         <outputFile>\n"
"          output file with a very long option description so we can see the\n"
"          line wrapping behavior\n"
"                            <fileList>\n"
"          files to be sorted\n";

            ASSERTV(ss.str(), EXPECTED, EXPECTED == ss.str());
            ASSERTV(es.str(), EXPECTED, EXPECTED == es.str());

            if (veryVerbose) {
                cout << "User-supplied name has precedence over default."
                     << endl;
            }

            const char *EXPECTED_USER_SUPPLIED = "\n"
"Usage: KilroyWasHere [-r|reverse] [-i|insensitivetocase] [-u|uniq]\n"
"                     [-a|algorithm <sortAlgo>] -o|outputfile <outputFile>\n"
"                     [<fileList>]+\n"
"Where:\n"
"  -r | --reverse\n"
"          sort in reverse order\n"
"  -i | --insensitivetocase\n"
"          be case insensitive while sorting\n"
"  -u | --uniq\n"
"          discard duplicate lines\n"
"  -a | --algorithm          <sortAlgo>\n"
"          sorting algorithm (default: quickSort)\n"
"  -o | --outputfile         <outputFile>\n"
"          output file with a very long option description so we can see the\n"
"          line wrapping behavior\n"
"                            <fileList>\n"
"          files to be sorted\n";

            ss.str(""); ss.clear();
            es.str(""); es.clear();

            cmdLine.printUsage(ss, "KilroyWasHere");                    // TEST
            cmdLine.printUsage(    "KilroyWasHere");                    // TEST

            ASSERTV(ss.str(), EXPECTED_USER_SUPPLIED == ss.str());
            ASSERTV(es.str(), EXPECTED_USER_SUPPLIED == es.str());

            if (veryVerbose) {
                cout
                    << "After successful `parse`, `argv[0]` is has precedence."
                    << endl;
            }

            const char *pName  = "myProgram";
            const char *argv[] = { pName
                                 , "--outputfile"
                                 , "myOutputfile"
                                 , "myInputfile1"
                                 , "myInputfile2"
                                 };
            const int   argc   = 5;

            bsl::ostringstream parseErrors;
            int                rc = cmdLine.parse(argc, argv, parseErrors);
            ASSERTV(rc, 0 == rc);
            ASSERTV(parseErrors.str(),
                    parseErrors.str().empty());

            const char *EXPECTED_POST_PARSE = "\n"
"Usage: myProgram [-r|reverse] [-i|insensitivetocase] [-u|uniq]\n"
"                 [-a|algorithm <sortAlgo>] -o|outputfile <outputFile>\n"
"                 [<fileList>]+\n"
"Where:\n"
"  -r | --reverse\n"
"          sort in reverse order\n"
"  -i | --insensitivetocase\n"
"          be case insensitive while sorting\n"
"  -u | --uniq\n"
"          discard duplicate lines\n"
"  -a | --algorithm          <sortAlgo>\n"
"          sorting algorithm (default: quickSort)\n"
"  -o | --outputfile         <outputFile>\n"
"          output file with a very long option description so we can see the\n"
"          line wrapping behavior\n"
"                            <fileList>\n"
"          files to be sorted\n";

            ss.str(""); ss.clear();
            es.str(""); es.clear();

            cmdLine.printUsage(ss);                                     // TEST
            cmdLine.printUsage();                                       // TEST

            ASSERTV(ss.str(), EXPECTED_POST_PARSE == ss.str());
            ASSERTV(es.str(), EXPECTED_POST_PARSE == es.str());

            if (veryVerbose) {
                cout <<
                "After successful `parse`, supplied name still has precedence."
                     << endl;
            }

            ss.str(""); ss.clear();
            es.str(""); es.clear();

            cmdLine.printUsage(ss, "KilroyWasHere");                    // TEST
            cmdLine.printUsage(    "KilroyWasHere");                    // TEST

            ASSERTV(ss.str(), EXPECTED_USER_SUPPLIED == ss.str());
            ASSERTV(es.str(), EXPECTED_USER_SUPPLIED == es.str());

            bsl::cerr.rdbuf(errorStreambuf);
        }

        if (veryVerbose) cout << "Medium case\n";
        {
            balcl::OptionInfo specTable[] = {
              {
                "w|woof",
                "woof",
                "grrowll",
                balcl::TypeInfo(Ot::k_BOOL),
                balcl::OccurrenceInfo(),
                ""
              },
              {
                "a|arf",
                "arf",
                "arrrrrrrrrrf",
                balcl::TypeInfo(Ot::k_BOOL),
                balcl::OccurrenceInfo(),
                ""
              },
              {
                "meow",
                "meow",
                "merrrrower",
                balcl::TypeInfo(Ot::k_BOOL),
                balcl::OccurrenceInfo(),
                ""
              },
              {
                "n|number",
                "number",
                "#",
                balcl::TypeInfo(Ot::k_INT),
                balcl::OccurrenceInfo(),
                ""
              },
              {
                "s|size",
                "size",
                "size in bytes",
                balcl::TypeInfo(Ot::k_INT),
                balcl::OccurrenceInfo::e_REQUIRED,
                ""
              },
              {
                "m|meters",
                "meters",
                "distance",
                balcl::TypeInfo(Ot::k_INT),
                balcl::OccurrenceInfo(),
                ""
              },
              {
                "radius",
                "radius",
                "half diameter",
                balcl::TypeInfo(Ot::k_INT),
                balcl::OccurrenceInfo(),
                ""
              },
            };

            balcl::CommandLine cmdLine(specTable);

            bsl::stringstream ss;
            cmdLine.printUsage(ss);

            if (veryVerbose) cout << ss.str() << endl;

            const char *EXPECTED = "\n"
"Usage: programName [-w|woof] [-a|arf] [--meow] [-n|number <number>]\n"
"                   -s|size <size> [-m|meters <meters>] [--radius <radius>]\n"
"Where:\n"
"  -w | --woof              grrowll\n"
"  -a | --arf               arrrrrrrrrrf\n"
"       --meow              merrrrower\n"
"  -n | --number  <number>  #\n"
"  -s | --size    <size>    size in bytes\n"
"  -m | --meters  <meters>  distance\n"
"       --radius  <radius>  half diameter\n";

            ASSERTV(ss.str(), EXPECTED == ss.str());
        }

        if (veryVerbose) cout << "Simple pathological case (fixed)\n";
        {
            balcl::OptionInfo specTable[] = {
              {
                "reverse",
                "isReverse",
                "sort in reverse order",
                balcl::TypeInfo(Ot::k_BOOL),
                balcl::OccurrenceInfo(),
                ""
              }
            };

            balcl::CommandLine cmdLine(specTable);

            bsl::stringstream ss;
            cmdLine.printUsage(ss);

            if (veryVerbose) cout << ss.str() << endl;

            const char *EXPECTED = "\n"
"Usage: programName [--reverse]\n"
"Where:\n"
"       --reverse      sort in reverse order\n";

            ASSERTV(ss.str(), EXPECTED == ss.str());
        }

        if (veryVerbose)
            cout << "Testing description shift to the new line." << endl;
        {
            // Descriptions are shifted to the new line, if total length of the
            // longest tag and longest name exceeds 15 characters.  Formally,
            // the hardcoded value in the code is 30 (`k_NEW_LINE_LIMIT`
            // constant), but spaces and punctuation characters take 15 of
            // them.  So we are testing scenarios, when `tag + name` are less
            // than 15 characters, equal to 15 characters, and exceed 15
            // characters.

            // Longest tag length + longest name length = 14
            {
                balcl::OptionInfo specTable[] = {
                  {
                    "123456",    // length = 6
                    "12345678",  // length = 8
                    "1234567890 1234567890 1234567890 1234567890"
                                         " 1234567890 1234567890",
                    balcl::TypeInfo(Ot::k_STRING),
                    balcl::OccurrenceInfo(),
                    ""
                  },
                  {
                      "123",
                      "1234",
                      "12",
                      balcl::TypeInfo(Ot::k_BOOL),
                      balcl::OccurrenceInfo(),
                      ""
                  },
                  {
                      "",
                      "12345",
                      "123",
                      balcl::TypeInfo(Ot::k_STRING_ARRAY),
                      balcl::OccurrenceInfo(),
                      ""
                  }
                };

                balcl::CommandLine cmdLine(specTable);

                bsl::stringstream ss;
                cmdLine.printUsage(ss);

                if (veryVerbose) cout << ss.str() << endl;

                const char *EXPECTED = "\n"
"Usage: programName [--123456 <12345678>] [--123] [<12345>]*\n"
"Where:\n"
"       --123456  <12345678>  1234567890 1234567890 1234567890 1234567890\n"
"                             1234567890 1234567890\n"
"       --123                 12\n"
"                 <12345>     123\n";

                ASSERTV(ss.str(), EXPECTED == ss.str());

            }

            {
                balcl::OptionInfo specTable[] = {
                  {
                    "12345678",  // length = 8
                    "123456",    // length = 6
                    "1",
                    balcl::TypeInfo(Ot::k_STRING),
                    balcl::OccurrenceInfo(),
                    ""
                  },
                  {
                      "123",
                      "1234",
                      "12",
                      balcl::TypeInfo(Ot::k_BOOL),
                      balcl::OccurrenceInfo(),
                      ""
                  },
                  {
                      "",
                      "12345",
                      "123",
                      balcl::TypeInfo(Ot::k_STRING_ARRAY),
                      balcl::OccurrenceInfo(),
                      ""
                  }
                };

                balcl::CommandLine cmdLine(specTable);

                bsl::stringstream ss;
                cmdLine.printUsage(ss);

                if (veryVerbose) cout << ss.str() << endl;

                const char *EXPECTED = "\n"
"Usage: programName [--12345678 <123456>] [--123] [<12345>]*\n"
"Where:\n"
"       --12345678  <123456>  1\n"
"       --123                 12\n"
"                   <12345>   123\n";

                ASSERTV(ss.str(), EXPECTED == ss.str());

            }

            // Longest tag length + longest name length = 15
            {
                balcl::OptionInfo specTable[] = {
                  {
                      "123",
                      "1234",
                      "12",
                      balcl::TypeInfo(Ot::k_BOOL),
                      balcl::OccurrenceInfo(),
                      ""
                  },
                  {
                    "123456",     // length = 6
                    "123456789",  // length = 9
                    "1234567890 1234567890 1234567890 1234567890"
                                         " 1234567890 1234567890",
                    balcl::TypeInfo(Ot::k_STRING),
                    balcl::OccurrenceInfo(),
                    ""
                  },
                  {
                      "",
                      "12345",
                      "123",
                      balcl::TypeInfo(Ot::k_STRING_ARRAY),
                      balcl::OccurrenceInfo(),
                      ""
                  }
                };

                balcl::CommandLine cmdLine(specTable);

                bsl::stringstream ss;
                cmdLine.printUsage(ss);

                if (veryVerbose) cout << ss.str() << endl;

                const char *EXPECTED = "\n"
"Usage: programName [--123] [--123456 <123456789>] [<12345>]*\n"
"Where:\n"
"       --123                  12\n"
"       --123456  <123456789>  1234567890 1234567890 1234567890 1234567890\n"
"                              1234567890 1234567890\n"
"                 <12345>      123\n";

                ASSERTV(ss.str(), EXPECTED == ss.str());

            }

            {
                balcl::OptionInfo specTable[] = {
                  {
                      "123",
                      "1234",
                      "12",
                      balcl::TypeInfo(Ot::k_BOOL),
                      balcl::OccurrenceInfo(),
                      ""
                  },
                  {
                    "123456789",  // length = 9
                    "123456",     // length = 6
                    "1",
                    balcl::TypeInfo(Ot::k_STRING),
                    balcl::OccurrenceInfo(),
                    ""
                  },
                  {
                      "",
                      "12345",
                      "123",
                      balcl::TypeInfo(Ot::k_STRING_ARRAY),
                      balcl::OccurrenceInfo(),
                      ""
                  }
                };

                balcl::CommandLine cmdLine(specTable);

                bsl::stringstream ss;
                cmdLine.printUsage(ss);

                if (veryVerbose) cout << ss.str() << endl;

                const char *EXPECTED = "\n"
"Usage: programName [--123] [--123456789 <123456>] [<12345>]*\n"
"Where:\n"
"       --123                  12\n"
"       --123456789  <123456>  1\n"
"                    <12345>   123\n";

                ASSERTV(ss.str(), EXPECTED == ss.str());

            }

            // Longest tag length + longest name length = 16
            {
                balcl::OptionInfo specTable[] = {
                  {
                      "123",
                      "1234",
                      "12",
                      balcl::TypeInfo(Ot::k_BOOL),
                      balcl::OccurrenceInfo(),
                      ""
                  },
                  {
                      "",
                      "12345",
                      "123",
                      balcl::TypeInfo(Ot::k_STRING_ARRAY),
                      balcl::OccurrenceInfo(),
                      ""
                  },
                  {
                    "123456",      // length = 6
                    "1234567890",  // length = 10
                    "1234567890 1234567890 1234567890 1234567890"
                              " 1234567890 1234567890 1234567890",
                    balcl::TypeInfo(Ot::k_STRING),
                    balcl::OccurrenceInfo(),
                    ""
                  }

                };

                balcl::CommandLine cmdLine(specTable);

                bsl::stringstream ss;
                cmdLine.printUsage(ss);

                if (veryVerbose) cout << ss.str() << endl;

                const char *EXPECTED = "\n"
"Usage: programName [--123] [--123456 <1234567890>] [<12345>]*\n"
"Where:\n"
"       --123\n"
"          12\n"
"       --123456  <1234567890>\n"
"          1234567890 1234567890 1234567890 1234567890 1234567890 1234567890\n"
"          1234567890\n"
"                 <12345>\n"
"          123\n";

                ASSERTV(ss.str(), EXPECTED == ss.str());

            }

            {
                balcl::OptionInfo specTable[] = {
                  {
                      "123",
                      "1234",
                      "12",
                      balcl::TypeInfo(Ot::k_BOOL),
                      balcl::OccurrenceInfo(),
                      ""
                  },
                  {
                      "",
                      "12345",
                      "123",
                      balcl::TypeInfo(Ot::k_STRING_ARRAY),
                      balcl::OccurrenceInfo(),
                      ""
                  },
                  {
                    "1234567890",  // length = 10
                    "123456",      // length = 6
                    "1",
                    balcl::TypeInfo(Ot::k_STRING),
                    balcl::OccurrenceInfo(),
                    ""
                  },

                };

                balcl::CommandLine cmdLine(specTable);

                bsl::stringstream ss;
                cmdLine.printUsage(ss);

                if (veryVerbose) cout << ss.str() << endl;

                const char *EXPECTED = "\n"
"Usage: programName [--123] [--1234567890 <123456>] [<12345>]*\n"
"Where:\n"
"       --123\n"
"          12\n"
"       --1234567890  <123456>\n"
"          1\n"
"                     <12345>\n"
"          123\n";

                ASSERTV(ss.str(), EXPECTED == ss.str());

            }

            // Flags have tags and names, but flag names aren't displayed in
            // usage explanatory note, so their lengths do not affect
            // description representation.

            // Flag with short tag and long name.
            {
                balcl::OptionInfo specTable[] = {
                  {
                      "12",
                      "123",
                      "1",
                      balcl::TypeInfo(Ot::k_STRING),
                      balcl::OccurrenceInfo(),
                      ""
                  },
                  {
                    "123",                             // length = 3
                    "123456789012345678901234567890",  // length = 30
                    "12",
                    balcl::TypeInfo(Ot::k_BOOL),
                    balcl::OccurrenceInfo(),
                    ""
                  },
                  {
                      "",
                      "12345",
                      "123",
                      balcl::TypeInfo(Ot::k_STRING_ARRAY),
                      balcl::OccurrenceInfo(),
                      ""
                  }
                };

                balcl::CommandLine cmdLine(specTable);

                bsl::stringstream ss;
                cmdLine.printUsage(ss);

                if (veryVerbose) cout << ss.str() << endl;

                const char *EXPECTED = "\n"
"Usage: programName [--12 <123>] [--123] [<12345>]*\n"
"Where:\n"
"       --12   <123>    1\n"
"       --123           12\n"
"              <12345>  123\n";

                ASSERTV(ss.str(), EXPECTED == ss.str());

            }

            // Flag with long tag and short name.
            {
                balcl::OptionInfo specTable[] = {
                  {
                      "12",
                      "123",
                      "1",
                      balcl::TypeInfo(Ot::k_STRING),
                      balcl::OccurrenceInfo(),
                      ""
                  },
                  {
                    "123456789012345678901234567890",  // length = 30
                    "1234",                            // length = 4
                    "12",
                    balcl::TypeInfo(Ot::k_BOOL),
                    balcl::OccurrenceInfo(),
                    ""
                  },
                  {
                      "",
                      "12345",
                      "123",
                      balcl::TypeInfo(Ot::k_STRING_ARRAY),
                      balcl::OccurrenceInfo(),
                      ""
                  }
                };

                balcl::CommandLine cmdLine(specTable);

                bsl::stringstream ss;
                cmdLine.printUsage(ss);

                if (veryVerbose) cout << ss.str() << endl;

                const char *EXPECTED = "\n"
"Usage: programName [--12 <123>] [--123456789012345678901234567890] [<12345>]*"
"\n"
"Where:\n"
"       --12                              <123>\n"
"          1\n"
"       --123456789012345678901234567890\n"
"          12\n"
"                                         <12345>\n"
"          123\n";

                ASSERTV(ss.str(), EXPECTED == ss.str());

            }

            // Non-option arguments have empty tags, but their names can affect
            // description representation.

            // Non-option argument with short name.
            {
                balcl::OptionInfo specTable[] = {
                  {
                      "12",   // length = 2 (tag)
                      "123",
                      "1",
                      balcl::TypeInfo(Ot::k_STRING),
                      balcl::OccurrenceInfo(),
                      ""
                  },
                  {
                      "123",  // length = 3 (tag)
                      "1234",
                      "12",
                      balcl::TypeInfo(Ot::k_BOOL),
                      balcl::OccurrenceInfo(),
                      ""
                  },
                  {
                      "",              // length = 0
                      "123456789012",  // length = 12
                      "1234567890 1234567890 1234567890 1234567890"
                                           " 1234567890 1234567890",
                      balcl::TypeInfo(Ot::k_STRING_ARRAY),
                      balcl::OccurrenceInfo(),
                      ""
                  }
                };

                balcl::CommandLine cmdLine(specTable);

                bsl::stringstream ss;
                cmdLine.printUsage(ss);

                if (veryVerbose) cout << ss.str() << endl;

                const char *EXPECTED = "\n"
"Usage: programName [--12 <123>] [--123] [<123456789012>]*\n"
"Where:\n"
"       --12   <123>           1\n"
"       --123                  12\n"
"              <123456789012>  1234567890 1234567890 1234567890 1234567890\n"
"                              1234567890 1234567890\n";

                ASSERTV(ss.str(), EXPECTED == ss.str());

            }

            // Non-option argument with long name and long description to
            // verify correct description formatting after the new line shift.
            {
                balcl::OptionInfo specTable[] = {
                  {
                      "12",   // length = 2 (tag)
                      "123",
                      "1",
                      balcl::TypeInfo(Ot::k_STRING),
                      balcl::OccurrenceInfo(),
                      ""
                  },
                  {
                      "123",  // length = 3 (tag)
                      "1234",
                      "12",
                      balcl::TypeInfo(Ot::k_BOOL),
                      balcl::OccurrenceInfo(),
                      ""
                  },
                  {
                      "",
                      "1234567890123",  // length = 13 (tag)
                      "12345678901234567890 12345678901234567890 1234567890 "
                      "12345678901234567890 12345678901234567890 1234567890",
                      balcl::TypeInfo(Ot::k_STRING_ARRAY),
                      balcl::OccurrenceInfo(),
                      ""
                  }
                };

                balcl::CommandLine cmdLine(specTable);

                bsl::stringstream ss;
                cmdLine.printUsage(ss);

                if (veryVerbose) cout << ss.str() << endl;

                const char *EXPECTED = "\n"
"Usage: programName [--12 <123>] [--123] [<1234567890123>]*\n"
"Where:\n"
"       --12   <123>\n"
"          1\n"
"       --123\n"
"          12\n"
"              <1234567890123>\n"
"          12345678901234567890 12345678901234567890 1234567890\n"
"          12345678901234567890 12345678901234567890 1234567890\n";

                ASSERTV(ss.str(), EXPECTED == ss.str());

            }

            // Long tag exceeding 80 symbols.
            {
                balcl::OptionInfo specTable[] = {
                  {
                    "1234567890123456789012345678901234567890"
                    "1234567890123456789012345678901234567890"
                    "1234567890",  // length = 90
                    "123",         // length = 3
                    "1",
                    balcl::TypeInfo(Ot::k_STRING),
                    balcl::OccurrenceInfo(),
                    ""
                  },
                  {
                      "123",
                      "1234",
                      "12",
                      balcl::TypeInfo(Ot::k_BOOL),
                      balcl::OccurrenceInfo(),
                      ""
                  },
                  {
                      "",
                      "12345",
                      "123",
                      balcl::TypeInfo(Ot::k_STRING_ARRAY),
                      balcl::OccurrenceInfo(),
                      ""
                  }
                };

                balcl::CommandLine cmdLine(specTable);

                bsl::stringstream ss;
                cmdLine.printUsage(ss);

                if (veryVerbose) cout << ss.str() << endl;

                const char *EXPECTED = "\n"
"Usage: programName [--1234567890123456789012345678901234567890" // no new line
                      "1234567890123456789012345678901234567890" // no new line
                      "1234567890 <123>]\n"
"                   [--123] [<12345>]*\n"
"Where:\n"
"       --12345678901234567890123456789012345678901234567890"    // no new line
         "1234567890123456789012345678901234567890  <123>\n"
"          1\n"
"       --123\n"
"          12\n"
"                                                           "    // no new line
         "                                          <12345>\n"
"          123\n";

                ASSERTV(ss.str(), EXPECTED == ss.str());

            }

            // Long name exceeding 80 symbols.
            {
                balcl::OptionInfo specTable[] = {
                  {
                    "12",          // length = 2
                    "1234567890123456789012345678901234567890"
                    "1234567890123456789012345678901234567890"
                    "1234567890",  // length = 90
                    "1",
                    balcl::TypeInfo(Ot::k_STRING),
                    balcl::OccurrenceInfo(),
                    ""
                  },
                  {
                      "123",
                      "1234",
                      "12",
                      balcl::TypeInfo(Ot::k_BOOL),
                      balcl::OccurrenceInfo(),
                      ""
                  },
                  {
                      "",
                      "12345",
                      "123",
                      balcl::TypeInfo(Ot::k_STRING_ARRAY),
                      balcl::OccurrenceInfo(),
                      ""
                  }
                };

                balcl::CommandLine cmdLine(specTable);

                bsl::stringstream ss;
                cmdLine.printUsage(ss);

                if (veryVerbose) cout << ss.str() << endl;

                const char *EXPECTED = "\n"
"Usage: programName [--12 <123456789012345678901234567890"       // no new line
                          "123456789012345678901234567890"       // no new line
                          "123456789012345678901234567890>]\n"
"                   [--123] [<12345>]*\n"
"Where:\n"
"       --12   <1234567890123456789012345678901234567890"        // no new line
               "1234567890123456789012345678901234567890"        // no new line
               "1234567890>\n"
"          1\n"
"       --123\n"
"          12\n"
"              <12345>\n"
"          123\n";

                ASSERTV(ss.str(), EXPECTED == ss.str());

            }

        }

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING ABILITY TO INPUT VALUE FOR FLAG
        //
        // Concerns:
        // 1. `balcl::CommandLine` handles flags without values identically
        //    regardless of the tag's form.
        //
        // 2. `balcl::CommandLine` fails to parse value for the short tag form
        //    of flag.
        //
        // 3. `balcl::CommandLine` successfully parses value for the long tag
        //    form of flag, inputted using equal sign, but adds warning message
        //    to the log.
        //
        // Plan:
        // 1. Set up a variety of flag representations and verify that
        //    `balcl::CommandLine` parses them in accordance with the
        //    specification.  Verify, that warning message is added, while
        //    `balcl::CommandLine` parses values for the long tag form of flag.
        //    (P-1..3)
        //
        // Testing:
        //   TESTING ABILITY TO INPUT VALUE FOR FLAG
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING ABILITY TO INPUT VALUE FOR FLAG" << endl
                          << "=======================================" << endl;

        if (verbose) cout << "Test command line args in isolation.\n";

        const bsl::size_t npos = bsl::string::npos;

        const int  ARGC_NUM  = 2;      // number of arguments

        bool       aBool     = false;  // variable for the first flag's value
        bool       bBool     = false;  // variable for the second flag's value

        const bool T         = true;   // `true`  alias
        const bool F         = false;  // `false` alias
        const bool NA        = false;  // not applicable

        const int  SUCCESS   =  0;     // successful parsing
        const int  FAILURE   = -1;     // failed parsing
        const int  THROW     = -2;     // BSLS_REVIEW throws

        const OptionInfo SPEC_A[] = {
            {
                "a|aBool",
                "Name_a",
                "Description for a",
                TypeInfo(&aBool),
                OccurrenceInfo::e_OPTIONAL,
                ""
            },
            {
                "b|bBool",
                "Name_b",
                "Description for b",
                TypeInfo(&bBool),
                OccurrenceInfo::e_OPTIONAL,
                ""
            },
        };
        enum { k_TAGS_NUM_A = sizeof SPEC_A / sizeof *SPEC_A };

        U_REVIEW_THROW_GUARD;

        static const struct DataA {
            int         d_line;                   // line
            int         d_expRes;                 // expected parsing result
            bool        d_isWarned;               // warning presence
            const char *d_argv_p[ARGC_NUM];       // list of parameters
            const bool  d_expValues[k_TAGS_NUM_A];// expected values of flags
        } DATA_A[] = {
            //LN   expRes   warn   argv                      expValues
            //--   -------  ----   ---------------------     ---------
            { L_,  SUCCESS,   F,   { "", "-a"            },  {  T,  F } },
            { L_,  FAILURE,   F,   { "", "-atrue"        },  { NA, NA } },
            { L_,  FAILURE,   F,   { "", "-a="           },  { NA, NA } },
            { L_,  FAILURE,   F,   { "", "-a=0"          },  { NA, NA } },
            { L_,  FAILURE,   F,   { "", "-a=false"      },  { NA, NA } },
            { L_,  FAILURE,   F,   { "", "-a=1"          },  { NA, NA } },
            { L_,  FAILURE,   F,   { "", "-a=true"       },  { NA, NA } },
            { L_,  FAILURE,   F,   { "", "-a=woof"       },  { NA, NA } },
            { L_,  SUCCESS,   F,   { "", "-aa"           },  {  T,  F } },
            { L_,  SUCCESS,   F,   { "", "-ab"           },  {  T,  T } },
            { L_,  SUCCESS,   F,   { "", "-ba"           },  {  T,  T } },
            { L_,  FAILURE,   F,   { "", "-abtrue"       },  { NA, NA } },
            { L_,  FAILURE,   F,   { "", "-ab="          },  { NA, NA } },
            { L_,  FAILURE,   F,   { "", "-ab=0"         },  { NA, NA } },
            { L_,  FAILURE,   F,   { "", "-ab=1"         },  { NA, NA } },
            { L_,  FAILURE,   F,   { "", "-ab=true"      },  { NA, NA } },
            { L_,  FAILURE,   F,   { "", "-ab=false"     },  { NA, NA } },
            { L_,  FAILURE,   F,   { "", "-ab=woof"      },  { NA, NA } },
            { L_,  SUCCESS,   F,   { "", "-aab"          },  {  T,  T } },
            { L_,  SUCCESS,   F,   { "", "-aba"          },  {  T,  T } },
            { L_,  FAILURE,   F,   { "", "--a"           },  { NA, NA } },
            { L_,  SUCCESS,   F,   { "", "--aBool"       },  {  T,  F } },
            { L_,  SUCCESS,   F,   { "", "--aBool="      },  {  T,  F } },
            { L_,  THROW,     T,   { "", "--aBool=0"     },  {  T,  F } },
            { L_,  THROW,     T,   { "", "--aBool=1"     },  {  T,  F } },
            { L_,  THROW,     T,   { "", "--aBool=true"  },  {  T,  F } },
            { L_,  THROW,     T,   { "", "--aBool=false" },  {  T,  F } },
            { L_,  THROW,     T,   { "", "--aBool=woof"  },  {  T,  F } },
            { L_,  FAILURE,   F,   { "", "--aBoolbBool"  },  { NA, NA } },
        };
        enum { NUM_DATA_A = sizeof DATA_A / sizeof *DATA_A };

        for (int ti = 0; ti < NUM_DATA_A; ++ti) {
            const DataA&       data       = DATA_A[ti];
            const int          LINE       = data.d_line;
            const int          EXP_RESULT = data.d_expRes;
            const bool         WARNED     = data.d_isWarned;
            const char *const *ARGV       = data.d_argv_p;
            const bool        *EXP        = data.d_expValues;

            aBool = false;
            bBool = false;

            if (veryVerbose) {
                P_(LINE); P_(EXP_RESULT);
                T_ T_ P(ARGV[1]);
            }

            bsl::ostringstream oss;
            Obj                mX(SPEC_A, k_TAGS_NUM_A, oss);
            const Obj&         X = mX;

            bsl::ostringstream oss2;
            int                parseRet = THROW;
            if (THROW == EXP_RESULT) {
                U_REVIEW_FAIL(parseRet = mX.parse(ARGC_NUM, ARGV, oss2));
                if (!k_REVIEW_FAIL_IS_ENABLED) {
                    continue;
                }
            }
            else {
                U_REVIEW_PASS(parseRet = mX.parse(ARGC_NUM, ARGV, oss2));
            }

            ASSERTV(LINE, parseRet, oss2.str(), EXP_RESULT == parseRet);

            if (veryVerbose) {
                T_ T_ P(X)
            }
            for (int k = 0; k < k_TAGS_NUM_A; ++k) {
                if (veryVerbose) { T_ T_ T_ P(k) }
                if (SUCCESS == parseRet) {
                    ASSERTV(LINE, k,  X.theBool(SPEC_A[k].d_name),
                            EXP[k] == X.theBool(SPEC_A[k].d_name));
                }
            }

            const char *WARNING_MESSAGE =
                                       "value has been provided for a boolean";
            bool        isWarned = npos != oss2.view().find(WARNING_MESSAGE);

            ASSERTV(LINE, isWarned, oss2.view(), WARNED == isWarned);
        }

        if (verbose) cout << "Testing setting bools on both cmdline & env.\n";

        const OptionInfo SPEC_B[] = {
            {
                "a|aBool",
                "Name_a",
                "Description for a",
                TypeInfo(&aBool),
                OccurrenceInfo::e_OPTIONAL,
                "ABOOL"
            },
            {
                "b|bBool",
                "Name_b",
                "Description for b",
                TypeInfo(&bBool),
                OccurrenceInfo::e_OPTIONAL,
                "BBOOL"
            },
        };
        enum { k_NUM_SPECS_B = sizeof SPEC_B / sizeof *SPEC_B };

        const char *E  = "a value of \"1\", \"true\", \"0\", or \"false\""
                                                               " is required.";
        const char *ST = "Warning: A string value has been provided"
                             " for a boolean option on the command line -- the"
                                                  " string value was ignored.";
        const char *SH = "does not match any short tag.";
        const char *LO = "does not match any long tag.";
        const char *OK = 0;

        static const struct DataB {
            int         d_line;                 // line
            int         d_expRes;               // expected parsing result
            const char *d_warning;
            const char *d_argv1;                // value of `argv[1]`
            const char *d_envA;
            const char *d_envB;
            const bool  d_expValueA;
            const bool  d_expValueB;
        } DATA_B[] = {
            //LN  Ret Wrn  argv1            envA, envB    a   b
            //--  --- --   ---------------  ----------   --  --
            { L_,  0, OK, "-a",              0,   0,     T,  F },
            { L_, -1, SH,  "-atrue",        "w", "w",    NA, NA },
            { L_, -1, SH,  "-a=",           "w", "w",    NA, NA },
            { L_, -1, SH,  "-a=0",          "w", "w",    NA, NA },
            { L_, -1, SH,  "-a=true",       "w", "w",    NA, NA },
            { L_,  0, OK,  "-aa",             0,   0,     T,  F },
            { L_, -1,  E,  "-aa",             0, "w",     T,  T },
            { L_,  0, OK,  "-ab",             0,   0,     T,  T },
            { L_,  0, OK,  "-ab",           "w", "w",     T,  T },
            { L_,  0, OK,  "-ba",             0,   0,     T,  T },
            { L_,  0, OK,  "-ba",           "w", "w",     T,  T },
            { L_, -1, SH,  "-abtrue",       "w", "w",    NA, NA },
            { L_, -1, SH,  "-ab=",          "w", "w",    NA, NA },
            { L_, -1, SH,  "-ab=0",         "w", "w",    NA, NA },
            { L_, -1, SH,  "-ab=true",      "w", "w",    NA, NA },
            { L_,  0, OK,  "-aab",            0,   0,     T,  T },
            { L_,  0, OK,  "-aab",          "w", "w",     T,  T },
            { L_,  0, OK,  "-aba",            0,   0,     T,  T },
            { L_,  0, OK,  "-aba",          "w", "w",     T,  T },
            { L_, -1, LO,  "--a",           "w", "w",    NA, NA },
            { L_,  0, OK,  "--aBool",         0,   0,     T,  F },
            { L_, -1,  E,  "--aBool",       "w",  "",     T,  F },
            { L_, -1,  E,  "--aBool=",      "w", "w",     T,  T },
            { L_, -2, ST,  "--aBool=0",     "w",   0,     T,  F },
            { L_, -2, ST,  "--aBool=true",  "w",   0,     T,  F },
            { L_, -2, ST,  "--aBool=false", "w",   0,     T,  F },
            { L_, -1, LO,  "--aBoolbBool",  "w", "w",    NA, NA },
            { L_,  0, OK,  "-a",            "w",   0,     T,  F },
            { L_, -1,  E,  "-a",            "w",  "",     T,  F },
            { L_, -1,  E,  "-a",            "w", "t",     T,  T },
            { L_, -1,  E,  "-a",            "w", "T",     T,  F },
            { L_, -1,  E,  "-a",            "w", "f",     T,  F },
            { L_, -1,  E,  "-a",            "w", "F",     T,  F },
            { L_,  0, OK,  "-a",            "w", "1",     T,  T },
            { L_, -1,  E,  "-a",            "w", "0001",  T,  T },
            { L_,  0, OK,  "-a",            "w", "0",     T,  F },
            { L_, -1,  E,  "-a",            "w", "0woof", T,  F },
            { L_,  0, OK,  "-a",            "w", "true",  T,  T },
            { L_, -1,  E,  "-a",            "w", "TRUE",  T,  T },
            { L_,  0, OK,  "-a",            "w", "false", T,  F },
            { L_, -1,  E,  "-a",            "w", "FALSE", T,  F },
            { L_, -1,  E,  "-a",            "w", "woof",  T,  T },
            { L_, -1,  E,  "-a",            "w", "twoo",  T,  T },
            { L_, -1,  E,  "-a",            "w", "fwoo",  T,  T },
            { L_,  0, OK,  "-b",             0,      "w", F,  T },
            { L_, -1,  E,  "-b",            "",      "w", F,  T },
            { L_, -1,  E,  "-b",            "t",     "w", T,  T },
            { L_, -1,  E,  "-b",            "T",     "w", T,  T },
            { L_, -1,  E,  "-b",            "f",     "w", F,  T },
            { L_, -1,  E,  "-b",            "F",     "w", F,  T },
            { L_,  0, OK,  "-b",            "1",     "w", T,  T },
            { L_, -1,  E,  "-b",            "0001",  "w", T,  T },
            { L_,  0, OK,  "-b",            "0",     "w", F,  T },
            { L_,  0, OK,  "-b",            "true",  "w", T,  T },
            { L_, -1,  E,  "-b",            "TRUE",  "w", T,  T },
            { L_,  0, OK,  "-b",            "false", "w", F,  T },
            { L_, -1,  E,  "-b",            "FALSE", "w", F,  T },
            { L_, -1,  E,  "-b",            "woof",  "w", T,  T },
            { L_, -1,  E,  "-b",            "twoo",  "w", T,  T },
            { L_, -1,  E,  "-b",            "fwoo",  "w", T,  T },
        };
        const char *WHITES[] = { "", " ", "\r", "\t", "\n", " \t\r\n " };
        enum { k_NUM_DATA_B = sizeof DATA_B / sizeof *DATA_B,
               k_NUM_WHITES = sizeof WHITES / sizeof *WHITES };

        for (int ii = 0; ii < k_NUM_DATA_B * k_NUM_WHITES; ++ii) {
            const int     wi         = ii / k_NUM_DATA_B;
            const char   *WHITE      = WHITES[wi];
            const int     di         = ii % k_NUM_DATA_B;
            const DataB&  data       = DATA_B[di];
            const int     LINE       = data.d_line;
            const int     EXP_RESULT = data.d_expRes;
            const char   *WARNING    = data.d_warning;
            const char   *ARGV1      = data.d_argv1;
            const char   *ENV_A      = data.d_envA;
            const char   *ENV_B      = data.d_envB;
            const bool    EXP_A      = data.d_expValueA;
            const bool    EXP_B      = data.d_expValueB;

            const char *argv[ARGC_NUM] = { "", ARGV1 };

            bsl::string putenvStrA(&ta);
            bsl::string putenvStrB(&ta);

#ifdef BSLS_PLATFORM_OS_WINDOWS
            enum { e_WINDOWS = true };
#else
            enum { e_WINDOWS = false };
#endif

            if (ENV_A) {
                putenvStrA = WHITE;
                putenvStrA += ENV_A;

                if (e_WINDOWS && -1 == EXP_RESULT && putenvStrA.empty()) {
                    continue;    // `getenv` will return null
                }

                u::setEnvironmentVariable("ABOOL", putenvStrA.c_str());
            }
            else {
                u::unsetEnvironmentVariable("ABOOL");
            }
            if (ENV_B) {
                putenvStrB = WHITE;
                putenvStrB += ENV_B;

                if (e_WINDOWS && -1 == EXP_RESULT && putenvStrB.empty()) {
                    continue;    // `getenv` will return null
                }

                u::setEnvironmentVariable("BBOOL", putenvStrB.c_str());
            }
            else {
                u::unsetEnvironmentVariable("BBOOL");
            }

            if (veryVerbose) {
                P_(LINE); P_(EXP_RESULT); P_(putenvStrA); P_(putenvStrB);
                                                                      P(ARGV1);
            }

            aBool = bBool = false;

            bsl::ostringstream  os(&ta);
            Obj                 mX(SPEC_B, os, &ta);

            bsl::ostringstream  os2(&ta);
            int                 parseRet = -2;
            if (-2 == EXP_RESULT) {
                U_REVIEW_FAIL(parseRet = mX.parse(ARGC_NUM, argv, os2));
                if (!k_REVIEW_FAIL_IS_ENABLED) {
                    continue;
                }
            }
            else {
                U_REVIEW_PASS(parseRet = mX.parse(ARGC_NUM, argv, os2));
            }

            ASSERTV(LINE, wi, EXP_RESULT, parseRet, os2.view(),
                                                       EXP_RESULT == parseRet);

            ASSERTV(LINE, os2.view(),
                          0 != EXP_RESULT || (OK == WARNING || ST == WARNING));

            ASSERTV(LINE, os.view(), os.view().empty());

            if (0 == parseRet) {
                ASSERTV(LINE, EXP_A, aBool, EXP_A == aBool);
                ASSERTV(LINE, EXP_B, bBool, EXP_B == bBool);
            }

            if (WARNING == OK) {
                ASSERTV(LINE, os2.view(), os2.view().empty());
            }
            else {
                ASSERTV(LINE, WARNING, os2.view(),
                                             npos != os2.view().find(WARNING));
            }
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING NON-OPTION TOGGLE `--`
        //
        // Concerns:
        // 1. The use of `--` on the command line must stop the parsing of the
        //    options and everything afterwards, including strings that begin
        //    with `-...` must be parsed as a non-option argument value.
        //
        // Plan:
        // 1. Insert `--` at various places into a command line with the last
        //    non-option argument being multi-valued, measure the number of
        //    arguments that get assigned to that non-option, and verify that
        //    it is as expected.
        //
        // Testing:
        //   TESTING NON-OPTION TOGGLE `--`
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING NON-OPTION TOGGLE `--`" << endl
                          << "==============================" << endl;

        if (verbose) cout << "\n\tTesting non-option argument toggle." << endl;

        bslma::TestAllocator                da;
        bslma::DefaultAllocatorGuard guard(&da);

        const int MAX_ARGC = 16;

        static const struct {
            int          d_line;
            int          d_specIdx;
            const char  *d_argv_p[MAX_ARGC];
            int          d_sizeNonOption2;
        } ARGS[] = {
          // line  specIdx  argv                                         #2
          // ----  -------  ----                                         --
            { L_,  0,       { "", "-i=-1", "-d=-1.0", "-s=string",
                                          "--", "#1",             0 },   0 }
          , { L_,  0,       { "", "-i=-1", "-d=-1.0", "-s=string",
                                          "--", "#1", "#2",       0 },   1 }
          , { L_,  0,       { "", "-i=-1", "-d=-1.0", "-s=string",
                                          "--", "#1", "#2", "#3", 0 },   2 }
          , { L_,  0,       { "", "-i=1", "-d=-1.0", "--", "-s=string",
                                                "#1", "#2", "#3", 0 },   3 }
          , { L_,  0,       { "", "-i=1", "--", "-d=-1.0", "-s=string",
                                                "#1", "#2", "#3", 0 },   4 }
          , { L_,  0,       { "", "--", "-i=1", "-d=-1.0", "-s=string",
                                                "#1", "#2", "#3", 0 },   5 }
          , { L_,  0,       { "", "-I=1", "-I=2", "-I=3", "-i=-1",
                                          "#1", "#2", "#3", "--", 0 },   2 }
          , { L_,  0,       { "", "-I=1", "-I=2", "-I=3", "-i=-1",
                                          "#1", "#2", "--", "#3", 0 },   2 }
          , { L_,  0,       { "", "-I=1", "-I=2", "-I=3", "-i=-1",
                                          "#1", "--", "#2", "#3", 0 },   2 }
          , { L_,  0,       { "", "#1", "#2", "#3", "--",
                                  "-i=1", "-d=-1.0", "-s=string", 0 },   5 }
          , { L_,  0,       { "", "#1", "#2", "#3", "--",
                                  "-i=1", "-d=-1.0", "-s=string", 0 },   5 }
          , { L_,  0,       { "", "#1", "#2", "--", "#3",
                                  "-i=1", "-d=-1.0", "-s=string", 0 },   5 }
          , { L_,  0,       { "", "#1", "--", "#2", "#3",
                                  "-i=1", "-d=-1.0", "-s=string", 0 },   5 }
          , { L_,  0,       { "", "--", "#1", "#2", "#3",
                                  "-i=1", "-d=-1.0", "-s=string", 0 },   5 }
        };
        enum { NUM_ARGS = sizeof ARGS / sizeof *ARGS };

        for (int i = 0; i < NUM_ARGS; ++i) {
            const int           LINE        = ARGS[i].d_line;
            const int           SPEC_IDX    = ARGS[i].d_specIdx;
            const char * const *ARGV        = ARGS[i].d_argv_p;
            const int           ARGC        = u::getArgc(ARGV);
            const bsl::size_t   SIZE        = ARGS[i].d_sizeNonOption2;

            const int           NUM_SPEC    = SPECS[SPEC_IDX].d_numSpecTable;
            const OptionInfo   *SPEC        = SPECS[SPEC_IDX].d_specTable;

            ASSERT(NUM_SPEC <= MAX_SPEC_SIZE);
            ASSERT(ARGC     <= MAX_ARGC);

            if (veryVerbose) {
                T_ P_(LINE) P_(SPEC_IDX) P(ARGC)
                T_ T_ P(ARGV[0])
                if ( 1 < ARGC) { T_ T_ P(ARGV[ 1]) }
                if ( 2 < ARGC) { T_ T_ P(ARGV[ 2]) }
                if ( 3 < ARGC) { T_ T_ P(ARGV[ 3]) }
                if ( 4 < ARGC) { T_ T_ P(ARGV[ 4]) }
                if ( 5 < ARGC) { T_ T_ P(ARGV[ 5]) }
                if ( 6 < ARGC) { T_ T_ P(ARGV[ 6]) }
                if ( 7 < ARGC) { T_ T_ P(ARGV[ 7]) }
                if ( 8 < ARGC) { T_ T_ P(ARGV[ 8]) }
                if ( 9 < ARGC) { T_ T_ P(ARGV[ 9]) }
                if (10 < ARGC) { T_ T_ P(ARGV[10]) }
                if (11 < ARGC) { T_ T_ P(ARGV[11]) }
            }

            bsl::ostringstream           oss(&ta);
            Obj                          mX(SPEC, NUM_SPEC, oss, &ta);
            const Obj&                   X = mX;

            bsl::ostringstream           oss2(&ta);
            int                          parseRet = mX.parse(ARGC, ARGV, oss2);
            ASSERTV(LINE, SPEC_IDX, oss2.view(), 0 == parseRet);

            if (veryVerbose) {
                T_; T_; P(X);
                if (0 < linkedStringArray1.size() &&
                    1 < linkedStringArray1[0].size()) {
                    T_; T_; P(linkedStringArray1[0]);
                }
                if (1 < linkedStringArray1.size() &&
                    1 < linkedStringArray1[1].size()) {
                    T_; T_; P(linkedStringArray1[1]);
                }
                if (1 < linkedStringArray1.size() &&
                    2 < linkedStringArray1[1].size()) {
                    T_; T_; P(linkedStringArray1[2]);
                }
                if (1 < linkedStringArray1.size() &&
                    3 < linkedStringArray1[1].size()) {
                    T_; T_; P(linkedStringArray1[3]);
                }
                if (1 < linkedStringArray1.size() &&
                    4 < linkedStringArray1[1].size()) {
                    T_; T_; P(linkedStringArray1[4]);
                }
            }

            ASSERTV(LINE, SPEC_IDX, SIZE == linkedStringArray1.size());

            ASSERTV(LINE, 0 == da.numAllocations());
        }

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING PARSING OF STRINGS
        //
        // Concerns:
        // 1. `balcl::CommandLine` must parse a string as raw, and not
        //    interpret escape sequences such as `.\bass` (on Windows) while
        //    replacing '\b' with a backspace.
        //
        // Plan:
        // 1. We employ a Table-driven test.
        //
        // Testing:
        //   TESTING PARSING OF STRINGS
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING PARSING OF STRINGS" << endl
                          << "==========================" << endl;

        const int MAX_ARGC = 16;

        static const struct {
            int          d_line;
            int          d_specIdx;
            const char  *d_argv_p[MAX_ARGC];
            const char  *d_expLinkedString_p;
        } ARGS[] = {
          // line  specIdx  argv                            exp
          // ----  -------  ----                            ---
            { L_,  0,       { "", "-s=..\\bass.cfg", 0 },   "..\\bass.cfg"   },
            { L_,  0,       { "", "-s=\"quotes\"", 0 },     "\"quotes\""     },
            { L_,  0,       { "", "-s=\\\"quotes\\\"", 0 }, "\\\"quotes\\\"" }
        };
        enum { NUM_ARGS = sizeof ARGS / sizeof *ARGS };

        for (int i = 0; i < NUM_ARGS; ++i) {
            const int          LINE        = ARGS[i].d_line;
            const int          SPEC_IDX    = ARGS[i].d_specIdx;
            const char *const *ARGV        = ARGS[i].d_argv_p;
            const int          ARGC        = u::getArgc(ARGV);
            const char        *EXP         = ARGS[i].d_expLinkedString_p;

            const int          NUM_SPEC    = SPECS[SPEC_IDX].d_numSpecTable;
            const OptionInfo  *SPEC        = SPECS[SPEC_IDX].d_specTable;

            bslma::TestAllocator         da;
            bslma::DefaultAllocatorGuard guard(&da);

            bsl::ostringstream           oss(&ta);
            Obj                          mX(SPEC, NUM_SPEC, oss, &ta);
            const Obj&                   X = mX;
            ASSERT(oss.view().empty());

            bsl::ostringstream           oss2(&ta);
            int                          parseRet = mX.parse(ARGC, ARGV, oss2);
            ASSERTV(LINE, SPEC_IDX, oss2.view(), 0 == parseRet);

            if (veryVerbose) {
                T_; T_; P(X);
                T_; T_; P_(linkedString); P(EXP);
            }

            ASSERTV(EXP, linkedString, EXP == linkedString);

            ASSERT(0 == da.numAllocations());
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING ORDER OF ARGUMENTS
        //
        // Concerns:
        // 1. `balcl::CommandLine` must have the same value no matter what the
        //    order of the arguments is, except for multi-valued and for the
        //    `--` non-option argument toggle.
        //
        // Plan:
        // 1. Using a helper function that checks if two command lines have
        //    compatible orderings, permute example command lines.
        //
        // Testing:
        //   TESTING ORDER OF ARGUMENTS
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING ORDER OF ARGUMENTS" << endl
                          << "==========================" << endl;

        const int MAX_ARGC = 32;

        if (verbose)
            cout << "\n\tTesting `u::isCompatibleOrdering` helper." << endl;

        static const struct {
            int          d_line;
            bool         d_isCompatible;
            const char  *d_argv_p[MAX_ARGC];
        } DUMMY_ARGS[] = {
          // line  isCompat   argv
          // ----  --------   ----
            { L_,  true,      { "", "-i=-1", "-d=-1.0", "-s=string",
                                          "#1", "#2", "#3", 0 } }  // reference
          , { L_,  true,      { "", "-s=string", "-i=-1", "-d=-1.0",
                                                        "#1", "#2", "#3", 0 } }
          , { L_,  true,      { "", "-i=-1", "-s=string", "-d=-1.0",
                                                        "#1", "#2", "#3", 0 } }
          , { L_,  true,      { "", "#1", "#2", "#3",
                                         "-i=-1", "-d=-1.0", "-s=string", 0 } }
          , { L_,  true,      { "", "#1", "-i=-1",  "#2",
                                            "-d=-1.0", "#3", "-s=string", 0 } }
          , { L_,  false,     { "", "-i=-1", "-d=-1.0", "-s=string",
                                                        "#1", "#3", "#2", 0 } }
          , { L_,  false,     { "", "-i=-1", "-d=-1.0", "-s=string",
                                                        "#2", "#1", "#3", 0 } }
          , { L_,  false,     { "", "-i=-1", "-d=-1.0", "-s=string",
                                                        "#3", "#1", "#2", 0 } }
        };
        enum { NUM_DUMMY_ARGS = sizeof DUMMY_ARGS / sizeof *DUMMY_ARGS };

        // Compare every line above to the first one.

        const char *const *ARGV_REFERENCE = DUMMY_ARGS[0].d_argv_p;
        const int          ARGC_REFERENCE = u::getArgc(ARGV_REFERENCE);

        for (int i = 0; i < NUM_DUMMY_ARGS; ++i) {
            const int          LINE          = DUMMY_ARGS[i].d_line;
            const bool         IS_COMPATIBLE = DUMMY_ARGS[i].d_isCompatible;
            const char *const *ARGV          = DUMMY_ARGS[i].d_argv_p;
            const int          ARGC          = u::getArgc(ARGV);

            if (veryVerbose) {
                T_ P_(LINE) P(IS_COMPATIBLE)
                T_ T_ P(ARGV[0])
                if ( 1 < ARGC) { T_ T_ P(ARGV[ 1]) }
                if ( 2 < ARGC) { T_ T_ P(ARGV[ 2]) }
                if ( 3 < ARGC) { T_ T_ P(ARGV[ 3]) }
                if ( 4 < ARGC) { T_ T_ P(ARGV[ 4]) }
                if ( 5 < ARGC) { T_ T_ P(ARGV[ 5]) }
                if ( 6 < ARGC) { T_ T_ P(ARGV[ 6]) }
                if ( 7 < ARGC) { T_ T_ P(ARGV[ 7]) }
                if ( 8 < ARGC) { T_ T_ P(ARGV[ 8]) }
                if ( 9 < ARGC) { T_ T_ P(ARGV[ 9]) }
                if (10 < ARGC) { T_ T_ P(ARGV[10]) }
                if (11 < ARGC) { T_ T_ P(ARGV[11]) }
            }

            ASSERTV(LINE, ARGC_REFERENCE == ARGC);
            ASSERTV(LINE,
                    IS_COMPATIBLE == u::isCompatibleOrdering(ARGV,
                                                             ARGV_REFERENCE,
                                                             ARGC));
        }

        if (verbose) cout << "\n\tTesting order of arguments." << endl;

        static const struct {
            int          d_line;
            int          d_specIdx;
            const char  *d_argv_p[MAX_ARGC];
        } ARGS[] = {
          // line  specIdx  argv
          // ----  -------  ----
            { L_,  0,       { "", "-i=-1", "-d=-1.0", "-s=string",
                                                       "#1", "#2", "#3", 0 } }
          , { L_,  0,       { "", "-I=1", "-I=2", "-I=3",
                                                       "#1", "#2", "#3", 0 } }
          , { L_,  0,       { "", "-I=1", "-I=2", "-I=3", "-i=-1",
                                                       "#1", "#2", "#3", 0 } }
          , { L_,  0,       { "", "-D=1.0", "-D=2.0", "-D=3.0",
                                                       "#1", "#2", "#3", 0 } }
          , { L_,  0,       { "", "-D=1.0", "-D=2.0", "-D=3.0", "-d=-1.0",
                                                       "#1", "#2", "#3", 0 } }
          , { L_,  0,       { "", "-S=s1", "-S=s2", "-S=s3",
                                                       "#1", "#2", "#3", 0 } }
          , { L_,  0,       { "", "-S=s1", "-S=s2", "-S=s3", "-s=string",
                                                       "#1", "#2", "#3", 0 } }
        };
        enum { NUM_ARGS = sizeof ARGS / sizeof *ARGS };

        for (int i = 0; i < NUM_ARGS; ++i) {
            const int          LINE        = ARGS[i].d_line;
            const int          SPEC_IDX    = ARGS[i].d_specIdx;
            const char *const *ARGV        = ARGS[i].d_argv_p;
            const int          ARGC        = u::getArgc(ARGV);
            const int          NUM_SPEC    = SPECS[SPEC_IDX].d_numSpecTable;
            const OptionInfo  *SPEC        = SPECS[SPEC_IDX].d_specTable;

            bslma::TestAllocator         da;
            bslma::DefaultAllocatorGuard guard(&da);

            ASSERT(NUM_SPEC <= MAX_SPEC_SIZE);
            ASSERT(ARGC     <= MAX_ARGC);

            if (veryVerbose) {
                T_ P_(LINE); P_(SPEC_IDX); P(ARGC);
                T_ T_ P(ARGV[0])
                if ( 1 < ARGC) { T_ T_ P(ARGV[ 1]) }
                if ( 2 < ARGC) { T_ T_ P(ARGV[ 2]) }
                if ( 3 < ARGC) { T_ T_ P(ARGV[ 3]) }
                if ( 4 < ARGC) { T_ T_ P(ARGV[ 4]) }
                if ( 5 < ARGC) { T_ T_ P(ARGV[ 5]) }
                if ( 6 < ARGC) { T_ T_ P(ARGV[ 6]) }
                if ( 7 < ARGC) { T_ T_ P(ARGV[ 7]) }
                if ( 8 < ARGC) { T_ T_ P(ARGV[ 8]) }
                if ( 9 < ARGC) { T_ T_ P(ARGV[ 9]) }
                if (10 < ARGC) { T_ T_ P(ARGV[10]) }
                if (11 < ARGC) { T_ T_ P(ARGV[11]) }
            }

            bsl::ostringstream           oss(&ta);
            Obj                          mX(SPEC, NUM_SPEC, oss, &ta);
            const Obj&                   X = mX;
            ASSERT(oss.view().empty());

            bsl::ostringstream           oss2(&ta);
            int                          parseRet = mX.parse(ARGC, ARGV, oss2);
            ASSERTV(LINE, SPEC_IDX, oss2.view(), 0 == parseRet);
            ASSERT(oss2.view().empty());

            if (veryVerbose) {
                T_ T_ P(X);
            }

            const char *argv[MAX_ARGC];
            bsl::copy(ARGV, ARGV + ARGC, static_cast<const char **>(argv));

            // The array of pointers must be sorted, because otherwise, the
            // loop below might not examine all permutations.  For most
            // compilers, that is how the string statics are laid out, but
            // there is no reason that it be so.

            bsl::sort(argv + 1, argv + ARGC);

            int iterations = 0, compatibleIterations = 0;
            do {
                if (u::isCompatibleOrdering(argv, ARGV, ARGC)) {
                    bsl::ostringstream           oss(&ta);
                    Obj                          mY(SPEC, NUM_SPEC, oss, &ta);
                    const Obj&                   Y = mY;
                    ASSERT(oss.view().empty());

                    bsl::ostringstream           oss2(&ta);

                    // note: argv, not ARGV
                    int                          parseRet =
                                                    mY.parse(ARGC, argv, oss2);
                    ASSERT(oss2.view().empty());

                    ASSERTV(LINE, SPEC_IDX, 0 == parseRet);
                    ASSERTV(LINE, SPEC_IDX, X == Y);  // TEST HERE

                    ++compatibleIterations;
                }
                ++iterations;
            } while (bsl::next_permutation(argv + 1, argv + ARGC));

            if (veryVerbose) {
                T_ T_ P_(iterations) P(compatibleIterations)
            }

            ASSERT(0 == da.numAllocations());
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING MULTIPLY-SPECIFIED FLAGS
        //
        // Concerns:
        // 1. `balcl::CommandLine` must accept multiply-specified flags.
        //
        // Plan:
        // 1. Define a variety of arguments with multiply-specified flags mixed
        //    in with another options.  Make sure the `numSpecified` call
        //    returns the proper number of occurrences.
        //
        // Testing:
        //   TESTING MULTIPLY-SPECIFIED FLAGS
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING MULTIPLY-SPECIFIED FLAGS" << endl
                          << "================================" << endl;

        const int MAX_ARGC = 5;
        static const struct {
            int          d_line;
            int          d_specIdx;
            int          d_argc;
            const char  *d_argv_p[MAX_ARGC];
            const int    d_expNumSpecified[MAX_SPEC_SIZE];
            const int    d_expLinkedInt;
        }         ARGS[] = {
          // Depth refers to `numSpecified`.

          // line  specIdx argc argv                  expNumSpecified expFValue
          // ----  ------- ---- ----                  --------------- ---------
          // depth 1
            { L_,  0,      2,   { "", "-b" },         { 0, 1, 0, 0 },  -1 }
          , { L_,  0,      2,   { "", "--bLong" },    { 0, 1, 0, 0 },  -1 }

          // depth 2
          , { L_,  0,      3,   { "", "-b", "-b" },   { 0, 2, 0, 0 },  -1 }
          , { L_,  0,      2,   { "", "-bb" },        { 0, 2, 0, 0 },  -1 }
          , { L_,  0,      2,   { "", "-abb" },       { 1, 2, 0, 0 },  -1 }
          , { L_,  0,      2,   { "", "-bab" },       { 1, 2, 0, 0 },  -1 }
          , { L_,  0,      2,   { "", "-bba" },       { 1, 2, 0, 0 },  -1 }
          , { L_,  0,      4,   { "", "-b", "-i=1", "-b" },
                                                      { 0, 2, 0, 1 },   1 }
          , { L_,  0,      2,   { "", "-bbi=1" },     { 0, 2, 0, 1 },   1 }
          , { L_,  0,      3,   { "", "-bi=1", "-b" },{ 0, 2, 0, 1 },   1 }
          , { L_,  0,      3,   { "", "--bLong", "--bLong" },
                                                      { 0, 2, 0, 0 },  -1 }
          , { L_,  0,      4,   { "", "--bLong", "-a", "--bLong" },
                                                      { 1, 2, 0, 0 },  -1 }

          // depth 3
          , { L_,  0,      4,   { "", "-b", "-b", "-b" },
                                                      { 0, 3, 0, 0 },  -1 }
          , { L_,  0,      2,   { "", "-bbb" },       { 0, 3, 0, 0 },  -1 }
          , { L_,  0,      2,   { "", "-abbb" },      { 1, 3, 0, 0 },  -1 }
          , { L_,  0,      2,   { "", "-babb" },      { 1, 3, 0, 0 },  -1 }
          , { L_,  0,      2,   { "", "-bbab" },      { 1, 3, 0, 0 },  -1 }
          , { L_,  0,      2,   { "", "-bbba" },      { 1, 3, 0, 0 },  -1 }
          , { L_,  0,      5,   { "", "-b", "-b", "-i=1", "-b" },
                                                      { 0, 3, 0, 1 },   1 }
          , { L_,  0,      2,   { "", "-bbbi=1" },    { 0, 3, 0, 1 },   1 }
          , { L_,  0,      3,   { "", "-bbi=1", "-b" },
                                                      { 0, 3, 0, 1 },   1 }
          , { L_,  0,      3,   { "", "-bi=1", "-bb" },
                                                      { 0, 3, 0, 1 },   1 }
          , { L_,  0,      4,   { "", "--bLong", "--bLong", "--bLong" },
                                                      { 0, 3, 0, 0 },  -1 }
          , { L_,  0,      5,   { "", "--bLong", "-a", "--bLong", "--bLong" },
                                                      { 1, 3, 0, 0 },  -1 }
          , { L_,  0,      5,   { "", "--bLong", "--bLong", "-a", "--bLong" },
                                                      { 1, 3, 0, 0 },  -1 }
        };
        enum { NUM_ARGS = sizeof ARGS / sizeof *ARGS };

        for (int i = 0; i < NUM_ARGS; ++i) {
            const int          LINE           = ARGS[i].d_line;
            const int          SPEC_IDX       = ARGS[i].d_specIdx;
            const int          ARGC           = ARGS[i].d_argc;
            const char *const *ARGV           = ARGS[i].d_argv_p;
            const int         *EXP            = ARGS[i].d_expNumSpecified;
            const int          EXP_LINKED_INT = ARGS[i].d_expLinkedInt;

            const int          NUM_SPEC       = SPECS[SPEC_IDX].d_numSpecTable;
            const OptionInfo  *SPEC           = SPECS[SPEC_IDX].d_specTable;

            ASSERT(NUM_SPEC  <= MAX_SPEC_SIZE);
            ASSERT(ARGC      <= MAX_ARGC);

            if (veryVerbose) {
                T_ P_(LINE) P_(SPEC_IDX) P_(ARGC) P(EXP_LINKED_INT)
                T_ T_ P(ARGV[0]);
                if (1 < ARGC    ) { T_ T_ P(ARGV[1]) }
                if (2 < ARGC    ) { T_ T_ P(ARGV[2]) }
                if (3 < ARGC    ) { T_ T_ P(ARGV[3]) }
                if (0 < NUM_SPEC) { T_ T_ P( EXP[0]) }
                if (1 < NUM_SPEC) { T_ T_ P( EXP[1]) }
                if (2 < NUM_SPEC) { T_ T_ P( EXP[2]) }
                if (3 < NUM_SPEC) { T_ T_ P( EXP[3]) }
            }

            linkedInt = -1;
            bsl::ostringstream           oss(&ta);
            Obj                          mX(SPEC, NUM_SPEC, oss);
            const Obj&                   X = mX;
            ASSERT(oss.view().empty());

            bsl::ostringstream           oss2(&ta);
            int                          parseRet = mX.parse(ARGC, ARGV, oss2);
            ASSERTV(LINE, SPEC_IDX, 0 == parseRet);
            ASSERT(oss2.view().empty());

            if (veryVerbose) {
                T_ T_ P(X)
            }
            for (int k = 0; k < NUM_SPEC; ++k) {
                if (veryVerbose) { T_ T_ T_ P(k) }
                ASSERTV(LINE, SPEC_IDX, k,
                             EXP[k] == X.numSpecified(SPEC[k].d_name));
            }
            ASSERTV(LINE, SPEC_IDX, EXP_LINKED_INT == linkedInt);
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING FLAGS BUNDLING
        //
        // Concerns:
        // 1. `balcl::CommandLine` must be able to parse bundled flags.
        //
        // Plan:
        // 1. Define a variety of arguments following some option
        //    specifications, and verify that they can be parsed successfully
        //    and that any linked variable specified as the last argument of
        //    the bundle (either in a separate argument or attached with
        //    "=value" to the bundle) is set properly.
        //
        // Testing:
        //   TESTING FLAGS BUNDLING
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING FLAGS BUNDLING" << endl
                          << "======================" << endl;

        if (verbose) cout << "Testing bundled flags." << endl;

        const int MAX_ARGC = 4;
        static const struct {
            int          d_line;
            int          d_specIdx;
            int          d_argc;
            const char  *d_argv_p[MAX_ARGC];
            const int    d_expNumSpecified[MAX_SPEC_SIZE];
            const int    d_expLinkedInt;
        }         ARGS[] = {
          // line  specIdx argc argv                expNumSpecified expFValue
          // ----  ------- ---- ----                --------------- ---------
            { L_,  0,      2,   { "", "-a" },       { 1, 0, 0, 0 },  -1 }
          , { L_,  0,      2,   { "", "-b" },       { 0, 1, 0, 0 },  -1 }
          , { L_,  0,      2,   { "", "-c" },       { 0, 0, 1, 0 },  -1 }
          , { L_,  0,      2,   { "", "-ab" },      { 1, 1, 0, 0 },  -1 }
          , { L_,  0,      2,   { "", "-ac" },      { 1, 0, 1, 0 },  -1 }
          , { L_,  0,      2,   { "", "-bc" },      { 0, 1, 1, 0 },  -1 }
          , { L_,  0,      2,   { "", "-abc" },     { 1, 1, 1, 0 },  -1 }
          , { L_,  0,      3,   { "", "-i", "1" },  { 0, 0, 0, 1 },   1 }
          , { L_,  0,      3,   { "", "-ai", "1" }, { 1, 0, 0, 1 },   1 }
          , { L_,  0,      2,   { "", "-ai=1" },    { 1, 0, 0, 1 },   1 }
        };
        enum { NUM_ARGS = sizeof ARGS / sizeof *ARGS };

        for (int i = 0; i < NUM_ARGS; ++i) {
            const int          LINE           = ARGS[i].d_line;
            const int          SPEC_IDX       = ARGS[i].d_specIdx;
            const int          ARGC           = ARGS[i].d_argc;
            const char *const *ARGV           = ARGS[i].d_argv_p;
            const int         *EXP            = ARGS[i].d_expNumSpecified;
            const int          EXP_LINKED_INT = ARGS[i].d_expLinkedInt;

            const int          NUM_SPEC       = SPECS[SPEC_IDX].d_numSpecTable;
            const OptionInfo  *SPEC           = SPECS[SPEC_IDX].d_specTable;

            bslma::TestAllocator         da;
            bslma::DefaultAllocatorGuard guard(&da);

            ASSERT(NUM_SPEC  <= MAX_SPEC_SIZE);
            ASSERT(ARGC      <= MAX_ARGC);

            if (veryVerbose) {
                T_ P_(LINE) P_(SPEC_IDX) P_(ARGC) P(EXP_LINKED_INT)
                T_ T_ P(ARGV[0])
                if (1 < ARGC    ) { T_ T_ P(ARGV[1]) }
                if (2 < ARGC    ) { T_ T_ P(ARGV[2]) }
                if (3 < ARGC    ) { T_ T_ P(ARGV[3]) }
                if (0 < NUM_SPEC) { T_ T_ P( EXP[0]) }
                if (1 < NUM_SPEC) { T_ T_ P( EXP[1]) }
                if (2 < NUM_SPEC) { T_ T_ P( EXP[2]) }
                if (3 < NUM_SPEC) { T_ T_ P( EXP[3]) }
            }

            linkedInt = -1;
            bsl::ostringstream           oss(&ta);
            Obj                          mX(SPEC, NUM_SPEC, oss, &ta);
            const Obj&                   X = mX;
            ASSERT(oss.view().empty());

            bsl::ostringstream           oss2(&ta);
            int                          parseRet = mX.parse(ARGC, ARGV, oss2);
            ASSERTV(LINE, SPEC_IDX, 0 == parseRet);
            ASSERT(oss2.view().empty());

            if (veryVerbose) {
                T_ T_ P(X)
            }
            for (int k = 0; k < NUM_SPEC; ++k) {
                if (veryVerbose) { T_ T_ T_ P(k) }
                ASSERTV(LINE, SPEC_IDX, k,
                        EXP[k] == X.numSpecified(SPEC[k].d_name));
            }
            ASSERTV(LINE, SPEC_IDX, EXP_LINKED_INT == linkedInt);

            ASSERT(0 == da.numAllocations());
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING INVALID OPTION SPECS
        //
        // Concerns:
        // 1. The `balcl::CommandLine` constructors reject invalid option
        //    specifications.
        //
        //   1. Options can be invalid individually (e.g., an option having an
        //      ill-formed tag) and collectively (e.g., each option must have a
        //      unique tag).
        //
        //   2. Invalid options trigger an error message to the specified
        //      output stream (if any).  Valid option specifications do not
        //      generate an error message.
        //
        //   3. QoI: Invalid option specifications trigger invocation of the
        //      assertion failure handler in *all* build modes.
        //
        // 2. The `isValidOptionSpecificationTable` class method concerns:
        //
        //   1. The return value correctly predicts acceptance (rejection)
        //      by the constructors.
        //
        //   2. Error messages obtained from the class methods must
        //      match those generated by the class constructor.
        //
        //   3. Given the same option specification, each overload of the
        //      class method has the same return value and (if it accepts an
        //      output stream) error message.
        //
        // 3. The error messages must correctly describe the error.
        //
        // Plan:
        // 1. Use table-driven testing to define option configurations known to
        //    be valid and not.
        //
        // 2. In exception builds, set the assertion failure handler to the
        //    `u::throwInvalidSpec` helper.  Catch the `u::InvalidSpec`
        //    exceptions from the constructor and compare the messages posted
        //    to the constructor's `stream` to the expected message.
        //
        // 3. Compare the results of the class methods to those generated by
        //    the constructor.
        //
        // 4. Use several ad-hoc tests to confirm that the overloads of
        //    `isValidOptionSpecificationTable` used for statically-initialized
        //    arrays produce the same results as the other other overloads.
        //
        // Testing:
        //   TESTING INVALID OPTION SPECS
        //   isValidOptionSpecificationTable(const Oi (&tbl)[L]);
        //   isValidOptionSpecificationTable(      Oi (&tbl)[L]);
        //   isValidOptionSpecificationTable(const Oi (&tbl)[L], ostream& s);
        //   isValidOptionSpecificationTable(      Oi (&tbl)[L], ostream& s);
        //   isValidOptionSpecificationTable(const Oi *tbl, int l);
        //   isValidOptionSpecificationTable(const Oi *tbl, int l, ostream& s);
        // --------------------------------------------------------------------
#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) cout << endl
                          << "TESTING INVALID OPTION SPECS" << endl
                          << "============================" << endl;

        bsls::Assert::setFailureHandler(&u::throwInvalidSpec);

        if (verbose) cout << "Testing invalid tags." << endl;
        static const struct {
            int         d_line;
            bool        d_isValid;
            const char *d_tag_p;
            const char *d_message_p;
        } TAGS[] = {
          // line valid  tag
          // ---- -----  ---
#define NL "\n"
            { L_, false, "a space",
            "Tag cannot contain spaces."                                     NL
            "Long tag cannot contain spaces."                                NL
            "The error occurred while validating the 1st option."            NL
            }

          , { L_, false, "some tag with spaces",
            "Tag cannot contain spaces."                                     NL
            "Long tag cannot contain spaces."                                NL
            "The error occurred while validating the 1st option."            NL
            }

          , { L_, false, "|",
            "Short tag cannot be empty if '|' present."                      NL
            "Long tag must be 2 or more characters."                         NL
            "The error occurred while validating the 1st option."            NL
            }

          , { L_, false, "|longTag",
            "Short tag cannot be empty if '|' present."                      NL
            "The error occurred while validating the 1st option."            NL
            }

          , { L_, false, "-",
            "Long tag cannot begin with '-'."                                NL
            "Long tag must be 2 or more characters."                         NL
            "The error occurred while validating the 1st option."            NL
            }

          , { L_, false, "-someTag",
            "Long tag cannot begin with '-'."                                NL
            "The error occurred while validating the 1st option."            NL
            }

          , { L_, false, "-|longTag",
            "Short tag cannot be '-'."                                       NL
            "The error occurred while validating the 1st option."            NL
            }

          , { L_, false, "s|longTag|",
            "Long tag cannot contain '|'."                                   NL
            "The error occurred while validating the 1st option."            NL
            }

          , { L_, false, "s|longTag|someOtherTag",
            "Long tag cannot contain '|'."                                   NL
            "The error occurred while validating the 1st option."            NL
            }

          , { L_, false, "s||longTag",
            "Long tag cannot contain '|'."                                   NL
            "The error occurred while validating the 1st option."            NL
            }

          , { L_, false, "s",
            "Long tag must be 2 or more characters."                         NL
            "The error occurred while validating the 1st option."            NL
            }

          , { L_, false, "short|longTag",
            "Short tag must be exactly one character, followed by '|'."      NL
            "The error occurred while validating the 1st option."            NL
            }

          , { L_, false, "ss|longTag",
            "Short tag must be exactly one character, followed by '|'."      NL
            "The error occurred while validating the 1st option."            NL
            }

          , { L_, false, "s|-long",
            "Long tag cannot begin with '-'."                                NL
            "The error occurred while validating the 1st option."            NL
            }

          , { L_, false, "s|l",
            "Long tag must be 2 or more characters."                         NL
            "The error occurred while validating the 1st option."            NL
            }

          // valid tags (for control)
          , { L_, true,  "s|long"                , ""}
          , { L_, true,  "s|long-with-dashes"    , ""}
          , { L_, true,  "0|1234567890"          , ""}
          , { L_, true,  ".|:\"<>?{};!@#$%^&*()_", ""} // yes! valid!
#undef NL
        };
        enum { NUM_TAGS = sizeof TAGS / sizeof *TAGS };

        for (int i = 0; i < NUM_TAGS; ++i) {
            const int          LINE     = TAGS[i].d_line;
            const bool         IS_VALID = TAGS[i].d_isValid;
            const char * const TAG      = TAGS[i].d_tag_p;
            const char * const MSG      = TAGS[i].d_message_p;

            OptionInfo SPEC[] = {
              {
                TAG,                           // non-option
                "SomeValidName",               // name
                "Some description",            // description
                u::createTypeInfo(Ot::e_BOOL),
                OccurrenceInfo::e_OPTIONAL,
                ""                             // env var name
              }
            };

            if (veryVerbose) { T_ T_ P_(LINE) P_(IS_VALID) P(TAG) }

            bsl::stringstream ossValidate;

            ASSERT(IS_VALID == Obj::isValidOptionSpecificationTable(
                                                                 SPEC,
                                                                 1,
                                                                 ossValidate));
            ASSERT(IS_VALID == Obj::isValidOptionSpecificationTable(SPEC,
                                                                    1));
            ASSERTV(LINE,
                    MSG,   ossValidate.str(),
                    MSG == ossValidate.str());

            bsl::stringstream oss;
            bool              exceptionCaught = false;

            try {
                Obj mX(SPEC, 1, oss);
            }
            catch (const u::InvalidSpec& e) {
                if (veryVerbose) { T_ T_ P(oss.str()) }
                exceptionCaught = true;
            }

            ASSERTV(LINE, !IS_VALID         == exceptionCaught);
            ASSERTV(LINE, ossValidate.str() == oss.str());
        }

        if (verbose) cout << "Testing invalid specs." << endl;

        TestConstraint::s_constraintValue = false;  // See constraint-violation
                                                    // entry in `DATA` below.

        static const struct {
            int         d_line;
            int         d_numSpecs;
            OptionInfo  d_specTable[MAX_SPEC_SIZE];
            const char *d_message_p;
        } DATA[] = {
#define NL "\n"
            { L_, 1, {
                          {
                              "",                              // non-option
                              "",                              // name
                              "Some description.",             // description
                              u::createTypeInfo(Ot::e_BOOL),
                              OccurrenceInfo::e_OPTIONAL,
                              ""                               // env var name
                          }  // Invalid name
                      }
            , "Flags cannot have an empty tag."                              NL
              "The error occurred while validating the 1st option."          NL
              "Name cannot be an empty string."                              NL
              "The error occurred while validating the 1st option."          NL
            }
          , { L_, 1, {
                          {
                              "",                              // non-option
                              "SomeName",                      // name
                              "",                              // description
                              u::createTypeInfo(Ot::e_BOOL),
                              OccurrenceInfo::e_OPTIONAL,
                              ""                               // env var name
                          }  // Invalid description
                      }
            , "Flags cannot have an empty tag."                              NL
              "The error occurred while validating the 1st option."          NL
              "Description cannot be an empty string."                       NL
              "The error occurred while validating the 1st option."          NL
            }

          , { L_, 1, {
                          {
                              "",                              // non-option
                              "SomeName",                      // name
                              "Some description",              // description
                              u::createTypeInfo(Ot::e_BOOL),
                              OccurrenceInfo::e_OPTIONAL,
                              ""                               // env var name
                          }  // Non-options cannot be of type `bool`.
                      }
            , "Flags cannot have an empty tag."                              NL
              "The error occurred while validating the 1st option."          NL
            }

          , { L_, 1, {
                          {
                              "",                                // non-option
                              "SomeName",                        // name
                              "Some description",                // description
                              u::createTypeInfo(Ot::e_CHAR),     // type `char`
                              u::createOccurrenceInfo(
                                                    OccurrenceInfo::e_OPTIONAL,
                                                    Ot::e_INT,  // type `int`
                                                    &valueInt),
                              ""                                // env var name
                          }  // Type of default value does not match type info.
                      }

            , "The type of default value does not match the type specified for"
              " the option."                                                 NL
              "The error occurred while validating the 1st option."          NL
            }

          , { L_, 1, {
                          {
                              "",                                // non-option
                              "SomeName",                        // name
                              "Some description",                // description
                              u::createTypeInfo(Ot::e_INT,
                                                0,        // no linked variable
                                                false,    // so not `optional`
                                                &testIntConstraint),
                              u::createOccurrenceInfo(
                                                    OccurrenceInfo::e_OPTIONAL,
                                                    Ot::e_INT,  // type `int`
                                                    &valueInt),
                              ""                                 // env var
                          }  // The default value does not meet constraint.
                      }
            , "Constraint Functor: error message."                           NL
              "Error: default value violates constraint."                    NL
              "The error occurred while validating the 1st option."          NL
            }

          , { L_, 2, {
                          {
                              "s|long1",                         // non-option
                              "SomeName",                        // name
                              "Some description",                // description
                              u::createTypeInfo(Ot::e_BOOL),
                              OccurrenceInfo::e_OPTIONAL,
                              "MYAPP_LONG1"
                          },
                          {
                              "s|long2",                         // non-option
                              "SomeOtherName",                   // name
                              "Some description",                // description
                              u::createTypeInfo(Ot::e_BOOL),
                              OccurrenceInfo::e_OPTIONAL,
                              "MYAPP_LONG2"
                          }
                      }  // Short tags must be unique.
            , "Error: short tags for the 2nd and 1st options are equal."     NL
            }

          , { L_, 2, {
                          {
                              "a|long",                          // non-option
                              "SomeName",                        // name
                              "Some description",                // description
                              u::createTypeInfo(Ot::e_BOOL),
                              OccurrenceInfo::e_OPTIONAL,
                              "MYAPP_A1"
                          },
                          {
                              "b|long",                          // non-option
                              "SomeOtherName",                   // name
                              "Some description",                // description
                              u::createTypeInfo(Ot::e_BOOL),
                              OccurrenceInfo::e_OPTIONAL,
                              "MYAPP_B2"
                          }
                      }  // Long tags must be unique.
            , "Error: long tags for the 2nd and 1st options are equal."      NL
            }

          , { L_, 2, {
                          {
                              "",                                // non-option
                              "SomeCommonName",                  // name
                              "Some description",                // description
                              u::createTypeInfo(Ot::e_BOOL),
                              OccurrenceInfo::e_OPTIONAL,
                              ""                                 // env var
                          },
                          {
                              "",                                // non-option
                              "SomeCommonName",                  // name
                              "Some description",                // description
                              u::createTypeInfo(Ot::e_BOOL),
                              OccurrenceInfo::e_OPTIONAL,
                              ""                                 // env var
                          }
                      }  // Names must be unique.
            , "Flags cannot have an empty tag."                              NL
              "The error occurred while validating the 1st option."          NL
              "Flags cannot have an empty tag."                              NL
              "The error occurred while validating the 2nd option."          NL
              "Error: The names for the 2nd and 1st options are equal."      NL
            }

          , { L_, 2, {
                          {
                              "",                                // non-option
                              "SomeCommonName1",                 // name
                              "Some description",                // description
                              u::createTypeInfo(Ot::e_INT),
                              u::createOccurrenceInfo(           // *optional*
                                                    OccurrenceInfo::e_OPTIONAL,
                                                    Ot::e_INT,
                                                    &valueInt),
                              ""                                 // env var
                          },
                          {
                              "",                                // non-option
                              "SomeCommonName2",                 // name
                              "Some description",                // description
                              u::createTypeInfo(Ot::e_INT),
                              OccurrenceInfo::e_REQUIRED,        // *required*
                              ""                                 // env var
                          }
                      }  // Defaulted non-option argument cannot be followed by
                         // required non-options.
            , "Error: A default value was provided for the previous non-option"
              " argument, specified as"                                      NL
              "the 1st option, but not for this non-option."                 NL
              "The error occurred while validating the 2nd option."          NL
            }

          , { L_, 2, {
                          {
                              "",                                // non-option
                              "SomeCommonName1",                 // name
                              "Some description",                // description
                              u::createTypeInfo(Ot::e_INT_ARRAY),// *array*
                              OccurrenceInfo::e_REQUIRED,
                              "_NON_OPTION_1"
                          },
                          {
                              "",                                // non-option
                              "SomeCommonName2",                 // name
                              "Some description",                // description
                              u::createTypeInfo(Ot::e_INT),      // *scalar)
                              OccurrenceInfo::e_REQUIRED,
                              "_NON_OPTION_2"
                          }
                      }
                         // Array non-options cannot be followed by other
                         // non-options.
            , "Error: A multi-valued non-option argument was already specified"
              " as the 1st option."                                          NL
              "The error occurred while validating the 2nd option."          NL
            }

          , { L_, 2, {
                          {
                              "a|arf",                           // non-option
                              "arf",                             // name
                              "Arf description",                 // description
                              u::createTypeInfo(Ot::e_BOOL),     // bool
                              OccurrenceInfo::e_OPTIONAL,
                              "MYAPP"
                          },
                          {
                              "m|meow",                          // non-option
                              "meow",                             // name
                              "Meow description",                // description
                              u::createTypeInfo(Ot::e_BOOL),     // bool
                              OccurrenceInfo::e_OPTIONAL,
                              "MYAPP"
                          }
                      }
                         // Array non-options cannot be followed by other
                         // non-options.
                         , "Error: multiple environment"
                                           " variables with name \"MYAPP\"." NL
            }

          , { L_, 1, {
                          {
                              "a|arf",                           // non-option
                              "arf",                             // name
                              "Arf description",                 // description
                              u::createTypeInfo(Ot::e_BOOL),     // bool
                              OccurrenceInfo::e_OPTIONAL,
                              "1MYAPP_ARF"
                          }
                      }
                         // Array non-options cannot be followed by other
                         // non-options.
                        , "Error: environment variable name"
                              " \"1MYAPP_ARF\" contains invalid characters." NL
            }

          , { L_, 1, {
                          {
                              "a|arf",                           // non-option
                              "arf",                             // name
                              "Arf description",                 // description
                              u::createTypeInfo(Ot::e_BOOL),     // bool
                              OccurrenceInfo::e_OPTIONAL,
                              "MYAPP_ARF="
                          }
                      }
                         // Array non-options cannot be followed by other
                         // non-options.
                     , "Error: environment variable name \"MYAPP_ARF=\""
                                             " contains invalid characters." NL
            }
#undef NL
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int i = 0; i < NUM_DATA; ++i) {
            const int                LINE     = DATA[i].d_line;
            const int                NUM_SPEC = DATA[i].d_numSpecs;
            const OptionInfo * const SPEC     = DATA[i].d_specTable;
            const char       * const MSG      = DATA[i].d_message_p;

            ASSERTV(LINE, NUM_SPEC <= MAX_SPEC_SIZE);

            if (veryVerbose) {
                T_ P_(LINE) P(NUM_SPECS)
                if (0 < NUM_SPEC) { T_ T_ P(SPEC[0]) }
                if (1 < NUM_SPEC) { T_ T_ P(SPEC[1]) }
                if (2 < NUM_SPEC) { T_ T_ P(SPEC[2]) }
                if (3 < NUM_SPEC) { T_ T_ P(SPEC[3]) }
            }

            bsl::stringstream ossValidate;

            ASSERTV(LINE,
                    false == Obj::isValidOptionSpecificationTable(
                                                                 SPEC,
                                                                 NUM_SPEC,
                                                                 ossValidate));
            ASSERTV(LINE,
                    false == Obj::isValidOptionSpecificationTable(SPEC,
                                                                  NUM_SPEC));
            ASSERTV(LINE,
                    MSG,   ossValidate.str(),
                    MSG == ossValidate.str());

            bsl::stringstream oss;
            bool              exceptionCaught = false;

            try {
                Obj mX(SPEC, NUM_SPEC, oss);
            }
            catch (const u::InvalidSpec& e) {
                if (veryVerbose) { T_ T_ P(oss.str()) }
                exceptionCaught = true;
            }

            ASSERTV(LINE, exceptionCaught);
            ASSERTV(LINE, ossValidate.str() == oss.str());
        }
#else
        if (verbose) cout << endl
                          << "====================================" << endl
                          << "Skipping Test: invalid option specs." << endl
                          << "====================================" << endl;
#endif // BDE_BUILD_TARGET_EXC

        if (veryVerbose) { cout <<
            "isValidOptionSpecificationTable: additional overloads" << endl; }
        {
            const OptionInfo OI0 =
                          {
                              "s|long1",                         // non-option
                              "SomeName",                        // name
                              "Some description",                // description
                              u::createTypeInfo(Ot::e_BOOL),
                              OccurrenceInfo::e_OPTIONAL,
                              ""                                // env var name
                          };
            const OptionInfo OI1 =
                          {
                              "s|long2",                         // non-option
                              "SomeOtherName",                   // name
                              "Some description",                // description
                              u::createTypeInfo(Ot::e_BOOL),
                              OccurrenceInfo::e_OPTIONAL,
                              ""                                // env var name
                          };

                  OptionInfo table[] = { OI0, OI1 };
            const OptionInfo TABLE[] = { OI0, OI1 };

            const bsl::size_t NUM_TABLE = sizeof  TABLE / sizeof *TABLE;

            bsl::ostringstream ossConst;
            bsl::ostringstream ossNonConst;

            ASSERT(!Obj::isValidOptionSpecificationTable(TABLE));
            ASSERT(!Obj::isValidOptionSpecificationTable(table));
            ASSERT(!Obj::isValidOptionSpecificationTable(TABLE, ossConst));
            ASSERT(!Obj::isValidOptionSpecificationTable(table, ossNonConst));

            bsl::ostringstream ossExpected;
            ASSERT(!Obj::isValidOptionSpecificationTable(TABLE,
                                                         NUM_TABLE,
                                                         ossExpected));

            ASSERT(ossExpected.str() == ossConst   .str());
            ASSERT(ossExpected.str() == ossNonConst.str());
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING ADDITIONAL MANIPULATORS
        //
        // Concerns:
        // 1. The two-parameter overload of the `parse` method produces the
        //    same result as the three-parameter overload and sends the same
        //    error message to `bsl::cerr`.
        //
        // Plan:
        // 1. Ad-hoc test: Using four identical test objects, exercise each of
        //    the two methods for a successful parse and an unsuccessful parse.
        //
        // 2. Use the `rdbuf` idiom to capture the output to `bsl::cerr` in an
        //    `bsl::ostringstream` object for comparison to the output of the
        //    three-argument overload.
        //
        // Testing:
        //   int parse(int argc, const char *const argv[]);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING ADDITIONAL MANIPULATORS" << endl
                          << "===============================" << endl;

        if (veryVerbose) cout << "`parse` Method: two-parameter overload"
                              << endl;
        {
            const OptionInfo OI =
                             { "f|flag"                   // tag
                             , "Flag"                     // name
                             , "description"              // description
                             , TypeInfo(Ot::k_BOOL)       // no linked variable
                             , OccurrenceInfo::e_OPTIONAL // occurrence
                             , ""                         // env var name
                             };

            const OptionInfo TABLE[] = { OI };

            const int NUM_TABLE = static_cast<int>(sizeof  TABLE
                                                 / sizeof *TABLE);
            ASSERT(1 == NUM_TABLE);

            const OptionInfo * const OIP = TABLE;

            if (veryVeryVerbose) cout << "successful parse" << endl;

            const char * const ARGVok[] = { "progname" };
            const int          ARGCok   = sizeof ARGVok / sizeof *ARGVok;

            Obj mZok3(OIP, NUM_TABLE); const Obj& Zok3 = mZok3;
            ASSERT( Zok3.isValid());
            ASSERT(!Zok3.isParsed());

            Obj mZok2(OIP, NUM_TABLE); const Obj& Zok2 = mZok2;
            ASSERT( Zok2.isValid());
            ASSERT(!Zok2.isParsed());

            bsl::ostringstream ossParseOk3;
            bsl::ostringstream ossParseOk2;

            bsl::streambuf *errorStreamBuf = cerr.rdbuf();
            cerr.rdbuf(ossParseOk2.rdbuf());  // Redirect `cerr`.

            int retParseZok3 = mZok3.parse(ARGCok, ARGVok, ossParseOk3);
            int retParseZok2 = mZok2.parse(ARGCok, ARGVok);

            ASSERT(0 == retParseZok3);
            ASSERT( ossParseOk3.str().empty());
            ASSERT( Zok3.isValid());
            ASSERT( Zok3.isParsed());

            ASSERT(0 == retParseZok2);
            ASSERT( ossParseOk2.str().empty());
            ASSERT( Zok2.isValid());
            ASSERT( Zok2.isParsed());

            cerr.rdbuf(errorStreamBuf);  // Undo `cerr` re-direction.

            if (veryVeryVerbose) cout << "unsuccessful parse" << endl;

            const char * const ARGVng[] = { "progname"
                                          , "--some-invalid-option"
                                          };
            const int          ARGCng   = sizeof ARGVng / sizeof *ARGVng;

            Obj mZng3(OIP, NUM_TABLE); const Obj& Zng3 = mZng3;
            ASSERT( Zng3.isValid());
            ASSERT(!Zng3.isParsed());

            Obj mZng2(OIP, NUM_TABLE); const Obj& Zng2 = mZng2;
            ASSERT( Zng2.isValid());
            ASSERT(!Zng2.isParsed());

            bsl::ostringstream ossParseNg3;
            bsl::ostringstream ossParseNg2;

            cerr.rdbuf(ossParseNg2.rdbuf());  // Redirect `cerr`.

            int  retParseZng3 = mZng3.parse(ARGCng, ARGVng, ossParseNg3);
            int  retParseZng2 = mZng2.parse(ARGCng, ARGVng);

            ASSERT(0 != retParseZng3);
            ASSERT(!ossParseNg3.str().empty());
            ASSERT(!Zng3.isValid());
            ASSERT(!Zng3.isParsed());

            ASSERT(0 != retParseZng2);
            ASSERT(!ossParseNg2.str().empty());
            ASSERT(!Zng2.isValid());
            ASSERT(!Zng2.isParsed());

            ASSERT(ossParseNg3.str() == ossParseNg2.str());

            cerr.rdbuf(errorStreamBuf);  // Undo `cerr` re-direction.
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ADDITIONAL ACCESSORS
        //
        // Concerns:
        // 1. Each accessor returns the expected value.
        //
        // 2. Each accessor returns values that are consistent with the others.
        //    For example, when a `numSpecified` returns non-zero, the
        //    `isSpecified` accessors return `true`.
        //
        // 3. QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        // 1. Use a table-driven approach based on option configurations and
        //    input values obtained from the `u::generateTestData` and
        //    `u::generateParseInput` helper functions, respectively.
        //
        // 2. Accessors that return option values are tested in cases where
        //    the options configuration defines a default value.  Those
        //    default values are defined in this test framework in regularly
        //    named static variables at file scope that are readily compared
        //    to the values returned by the accessors.
        //
        //    - Boolean options (a.k.a.  "flags") are disallowed default values
        //      and are tested for consistency with the results of
        //      `isSpecified` method.
        //
        // 3. Do negative tests of defensive checks using `BSLS_ASSERTTEST_*`
        //    macros.  (C-2)
        //
        // Testing:
        //   ~CommandLineOptionsHandle();
        //   CommandLineOptionsHandle options() const;
        //   bool hasOption(const bsl::string& name) const;
        //   bool hasValue(const bsl::string& name) const;
        //   bool isSpecified(const string& name) const;
        //   bool isSpecified(const string& name, int *count) const;
        //   int numSpecified(const string& name) const;
        //   const vector<int>& positions(const string& name) const;
        //   int position(const bsl::string& name) const;
        //   CommandLineOptionsHandle specifiedOptions() const;
        //   OptionType::Enum type(const bsl::string name) const;
        //   bool theBool(const bsl::string& name) const;
        //   char theChar(const bsl::string& name) const;
        //   int theInt(const bsl::string& name) const;
        //   Int64 theInt64(const bsl::string& name) const;
        //   double theDouble(const bsl::string& name) const;
        //   const string& theString(const string& name) const;
        //   const Datetime& theDatetime(const string& name) const;
        //   const Date& theDate(const string& name) const;
        //   const Time& theTime(const string& name) const;
        //   const vector<char>& theCharArray(const string& name) const;
        //   const vector<int>& theIntArray(const string& name) const;
        //   const vector<Int64>& theInt64Array(const string& name) const;
        //   const vector<double>& theDoubleArray(const string& name) const;
        //   const vector<string>& theStringArray(const string& name) const;
        //   const vector<Datetime>& theDatetimeArray(const string& nom) const;
        //   const vector<Date>& theDateArray(const string& name) const;
        //   const vector<Time>& theTimeArray(const string& name) const;
        //
        //   int index(const char *name) const;
        //   const char *name(size_t index) const;
        //   size_t numOptions() const;
        //   OptionType::Enum type(size_t index) const;
        //   OptionType::Enum type(const char *name) const;
        //   const OptionValue& value(size_t index) const;
        //   const OptionValue& value(const char *name) const;
        //   template <class TYPE> const TYPE& the(size_t index) const;
        //   template <class TYPE> const TYPE& the(const char *name) const;
        //   bool theBool(const char *name) const;
        //   char theChar(const char *name) const;
        //   int theInt(const char *name) const;
        //   Int64 theInt64(const char *name) const;
        //   double theDouble(const char *name) const;
        //   const string& theString(const char *name) const;
        //   const Datetime& theDatetime(const char *name) const;
        //   const Date& theDate(const char *name) const;
        //   const Time& theTime(const char *name) const;
        //   const vector<char>& theCharArray(const char *name) const;
        //   const vector<int>& theIntArray(const char *name) const;
        //   const vector<Int64>& theInt64Array(const char *name)
        //   const vector<double>& theDoubleArray(const char *name) const;
        //   const vector<string>& theStringArray(const char *name) const;
        //   const vector<Datetime>& theDatetimeArray(const char *name)
        //   const vector<Date>& theDateArray(const char *name) const;
        //   const vector<Time>& theTimeArray(const char *name) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING ADDITIONAL ACCESSORS" << endl
                          << "============================" << endl;

        PREAMBLE_OPTIONS_ARGUMENTS

        const int DELTA = 1; // do each

        int countHasDefaultValue = 0;
        int countIsNotSpecified  = 0;
        int countBinaryTrue      = 0;
        int countBinaryFalse     = 0;

        typedef bsl::map<ElemType, int> TypeTally;

        TypeTally typeTally;

// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
        for (int n = 0; n < 4; ++n) {
        for (int i = 0; i < (n ? NUM_OPTIONS - n : 1); i += DELTA) {
            OptionInfo        specTable[4];
            const OptionInfo *SPEC_TABLE = specTable;

            for (int j = 0; j < n; ++j) {
                if (veryVerbose) {
                    T_ T_ T_ P_(j)
                }
                int index = i + j * NUM_OPTIONS;
                specTable[j] = OPTIONS[index];
            }

            if (veryVerbose) {
                T_ T_ P_(i) P(n)
                for (int j = 0; j < n; ++j) {
                    T_ T_ T_ P_(j) P(SPEC_TABLE[j])
                }
            }

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mZ(SPEC_TABLE, n, &scratch);  const Obj& Z = mZ;

            GENERATE_PARSE_INPUT(n, i)

            bsl::ostringstream ossParse;
            int                parseRetZ = mZ.parse(
                                             static_cast<int>(ARG_PTRS.size()),
                                             &ARG_PTRS[0],
                                             ossParse);
            ASSERT(0 == parseRetZ);
            ASSERT(ossParse.str().empty());
            ASSERT(Z.isValid());
            ASSERT(Z.isParsed());

            const OptionsHandle OH = Z.         options();  // ACTION
            const OptionsHandle SH = Z.specifiedOptions();  // ACTION

            ASSERT(static_cast<bsl::size_t>(n) == OH.numOptions());  // ACTION

            // Confirm named options
            for (int j = 0; j < n; ++j) {
                const OptionInfo&  OI   = SPEC_TABLE[j];
                const bsl::string& name = OI.d_name;
                const ElemType     type = OI.d_typeInfo.type();

                if (veryVerbose) {
                    T_ T_ T_ P_(j) P_(name) P_(type) P(OI)
                }

                {
                    // Confirm non-successful returns of `index` and
                    // `hasOption`.
                    ASSERT(-1    == OH.index(""));                    // ACTION
                    ASSERT(-1    == OH.index("XXX"));                 // ACTION
                    ASSERT(false == Z.hasOption(bsl::string("")));    // ACTION
                    ASSERT(false == Z.hasOption(bsl::string("XXX"))); // ACTION
                }

                ASSERT(true == Z.hasOption(name));                    // ACTION
                ASSERT(type == Z.type(name));                         // ACTION
                ASSERT(j    == OH.index(name.c_str()));               // ACTION
                ASSERT(0    == bsl::strcmp(name.c_str(), OH.name(j)));// ACTION
                ASSERT(type == OH.type(j));                           // ACTION
                ASSERT(type == OH.type(name.c_str()));                // ACTION

                bool isThere  = Z.isSpecified(name);         // ACTION
                int  count    = 0;
                bool isThere2 = Z.isSpecified(name, &count); // ACTION

                ASSERT(isThere  == isThere2);
                ASSERT(count    == Z.numSpecified(name));    // ACTION
                if (!isThere) {
                    ASSERT(0 == count);
                }

                if (isThere) {
                    if (veryVeryVerbose) {
                        for (bsl::size_t i = 0;
                                         i < ARG_PTRS.size();
                                       ++i) {
                            P_(i) P(ARG_PTRS[i])
                        }
                    }
                    if (Ot::isArrayType(type)) {
                        if (veryVeryVerbose) {
                            P_(name)  P(Z.positions(name).size())
                            for (bsl::size_t i = 0;
                                             i < Z.positions(name).size();
                                           ++i) {
                                P_(i) P(Z.positions(name)[i])
                            }
                        }
                        ASSERT(static_cast<bsl::size_t>(count)
                                     == Z.positions(name).size());  // ACTION
                    } else {
                        if (veryVeryVerbose) {
                            P_(name)  P(Z.position(name))
                        }
                        ASSERT(-1    != Z.position(name));          // ACTION
                    }
                } else {
                    if (Ot::isArrayType(type)) {
                        ASSERT( 0 == Z.positions(name).size());     // ACTION
                    } else {
                        ASSERT(-1 == Z.position(name));             // ACTION
                    }
                }

                if (OI.d_defaultInfo.hasDefaultValue()) {
                    // That value was defined in a file-scope symbol.

                    ++countHasDefaultValue;

                    const char *const namS = name.c_str();

                    if (!Z.isSpecified(name)) {  // ACTION

                        ++countIsNotSpecified;

                        ASSERT(true  == SH.value(j).isNull());  // ACTION
                        ASSERT(false == OH.value(j).isNull());  // ACTION

                        ASSERT(true  == SH.value(namS).isNull());  // ACTION
                        ASSERT(false == OH.value(namS).isNull());  // ACTION

                        ASSERT(OI.d_defaultInfo.defaultValue() == OH.value(j));
                                                                      // ACTION
                        ASSERT(OI.d_defaultInfo.defaultValue() == OH.value(
                                                                        namS));
                                                                      // ACTION

#define CASE(ENUM, ALIAS)                                                     \
    case ENUM: {                                                              \
      ASSERT(value##ALIAS ==  Z.the##ALIAS(name));                            \
      ASSERT(value##ALIAS == OH.the##ALIAS(namS));                            \
                                                                              \
      typedef Ot::EnumToType<ENUM>::type VT;                                  \
      ASSERT(value##ALIAS ==  OH            .the<VT>(namS));                  \
      ASSERT(value##ALIAS ==  OH.value(j)   .the<VT>());                      \
      ASSERT(value##ALIAS ==  OH.value(namS).the<VT>());                      \
                                                                              \
      typeTally[ENUM] += 1;                                                   \
    }; break;                                                                 \

                        switch (type) {
                          case Ot::e_VOID: {
                            ASSERTV("Not reached", 0);
                          } break;
                          case Ot::e_BOOL: {
                            ASSERTV("Not reached", 0);
                                                    // default value disallowed
                          } break;

                          CASE(Ot::e_CHAR,         Char)
                          CASE(Ot::e_INT,           Int)
                          CASE(Ot::e_INT64,       Int64)
                          CASE(Ot::e_DOUBLE,     Double)
                          CASE(Ot::e_STRING,     String)
                          CASE(Ot::e_DATETIME, Datetime)
                          CASE(Ot::e_DATE,         Date)
                          CASE(Ot::e_TIME,         Time)

                          CASE(Ot::e_CHAR_ARRAY,         CharArray)
                          CASE(Ot::e_INT_ARRAY,           IntArray)
                          CASE(Ot::e_INT64_ARRAY,       Int64Array)
                          CASE(Ot::e_DOUBLE_ARRAY,     DoubleArray)
                          CASE(Ot::e_STRING_ARRAY,     StringArray)
                          CASE(Ot::e_DATETIME_ARRAY, DatetimeArray)
                          CASE(Ot::e_DATE_ARRAY,         DateArray)
                          CASE(Ot::e_TIME_ARRAY,         TimeArray)

                          default: {
                            BSLS_ASSERT_INVOKE_NORETURN("!Reached");
                          } break;
                        };
#undef CASE
                    }
                }

                if (Ot::e_BOOL == OI.d_typeInfo.type()) {
                    if (Z.isSpecified(name)) {
                        ASSERT(true  ==  Z.theBool(name));         // ACTION
                        ASSERT(true  == OH.theBool(name.c_str())); // ACTION
                        ASSERT(true  == SH.theBool(name.c_str())); // ACTION
                        ++countBinaryTrue;
                    } else {
                        ASSERT(false ==  Z.theBool(name));         // ACTION
                        ASSERT(false == OH.theBool(name.c_str())); // ACTION
                        ASSERT(true  == SH.value(j).isNull());
                        ASSERT(true  == SH.value(name.c_str()).isNull());
                                                                   // ACTION
                        ++countBinaryFalse;
                    }
                }
            }
        }
        }
// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line

        if (veryVeryVerbose) {
            P_(countBinaryTrue) P(countBinaryFalse)
        }

        ASSERTV(countBinaryTrue,  0 < countBinaryTrue);
        ASSERTV(countBinaryFalse, 0 < countBinaryFalse);

        if (veryVeryVerbose) {
            for (TypeTally::const_iterator itr = typeTally.begin(),
                                           end = typeTally.end();
                                           end != itr; ++itr) {
                cout << itr->first  << ": "
                     << itr->second << endl;
            }
            P(countHasDefaultValue);
            P(countIsNotSpecified);
        }

        const bsl::size_t NUM_TYPES = 1 + static_cast<bsl::size_t>(
                                                             Ot::e_TIME_ARRAY);

        bsl::size_t typeTallySize = typeTally.size();
        ASSERT(typeTallySize == NUM_TYPES
                                   -1 // `e_VOID`
                                   -1 // `e_BOOL`
                                   );

        if (veryVerbose) cout << "Negative Testing" << endl;
        {
#define ENTRY(TYPE, ALIAS)                                                    \
                             {         "tag"#ALIAS                            \
                             ,        "name"#ALIAS                            \
                             , "description"#ALIAS                            \
                             , TypeInfo(Ot::k_##TYPE)                         \
                             , OccurrenceInfo(value##ALIAS)                   \
                             , ""                                             \
                             }                                                \

            const OptionInfo TABLE[] = {
                             {         "tagBool"          // tag
                             ,        "nameBool"          // name
                             , "descriptionBool"          // description
                             , TypeInfo(Ot::k_BOOL)       // no linked variable
                             , OccurrenceInfo::e_OPTIONAL // occurrence
                             , ""                         // env var name
                             }

                           , ENTRY(CHAR,         Char)
                           , ENTRY(INT,           Int)
                           , ENTRY(INT64,       Int64)
                           , ENTRY(DOUBLE,     Double)
                           , ENTRY(STRING,     String)
                           , ENTRY(DATETIME, Datetime)
                           , ENTRY(DATE,         Date)
                           , ENTRY(TIME,         Time)

                           , ENTRY(CHAR_ARRAY,         CharArray)
                           , ENTRY(INT_ARRAY,           IntArray)
                           , ENTRY(INT64_ARRAY,       Int64Array)
                           , ENTRY(DOUBLE_ARRAY,     DoubleArray)
                           , ENTRY(STRING_ARRAY,     StringArray)
                           , ENTRY(DATETIME_ARRAY, DatetimeArray)
                           , ENTRY(DATE_ARRAY,         DateArray)
                           , ENTRY(TIME_ARRAY,         TimeArray)
            };
#undef ENTRY

            const int NUM_TABLE = static_cast<int>(sizeof  TABLE
                                                 / sizeof *TABLE);

            if (veryVeryVerbose) {
                for (int i = 0; i < NUM_TABLE; ++i) {
                    P_(i) P(TABLE[i])
                }
            }

            const OptionInfo * const OIP = TABLE;

            // Create a valid object, `Z`

            const char * const ARGV[] = { "progname" };
            const int          ARGC   = sizeof ARGV / sizeof *ARGV;

            Obj mZ(OIP, NUM_TABLE); const Obj& Z = mZ;
            ASSERT( Z.isValid());
            ASSERT(!Z.isParsed());

            {
                bsls::AssertTestHandlerGuard hGa;

                ASSERT_FAIL(Z.         options());
                ASSERT_FAIL(Z.specifiedOptions());

                ASSERT_FAIL(Z.theBool         ("nameBool"         ));
                ASSERT_FAIL(Z.theChar         ("nameChar"         ));
                ASSERT_FAIL(Z.theInt          ("nameInt"          ));
                ASSERT_FAIL(Z.theInt64        ("nameInt64"        ));
                ASSERT_FAIL(Z.theDouble       ("nameDouble"       ));
                ASSERT_FAIL(Z.theString       ("nameString"       ));
                ASSERT_FAIL(Z.theDatetime     ("nameDatetime"     ));
                ASSERT_FAIL(Z.theDate         ("nameDate"         ));
                ASSERT_FAIL(Z.theTime         ("nameTime"         ));
                ASSERT_FAIL(Z.theCharArray    ("nameCharArray"    ));
                ASSERT_FAIL(Z.theIntArray     ("nameIntArray"     ));
                ASSERT_FAIL(Z.theInt64Array   ("nameInt64Array"   ));
                ASSERT_FAIL(Z.theDoubleArray  ("nameDoubleArray"  ));
                ASSERT_FAIL(Z.theStringArray  ("nameStringArray"  ));
                ASSERT_FAIL(Z.theDatetimeArray("nameDatetimeArray"));
                ASSERT_FAIL(Z.theDateArray    ("nameDateArray"    ));
                ASSERT_FAIL(Z.theTimeArray    ("nameTimeArray"    ));

                // Per contract, the following accessors work in on objects in
                // unparsed state (though the results are not meaningful).

                ASSERT_PASS(Z.isSpecified("nameBool"         ));
                ASSERT_PASS(Z.isSpecified("nameChar"         ));
                ASSERT_PASS(Z.isSpecified("nameInt"          ));
                ASSERT_PASS(Z.isSpecified("nameInt64"        ));
                ASSERT_PASS(Z.isSpecified("nameDouble"       ));
                ASSERT_PASS(Z.isSpecified("nameString"       ));
                ASSERT_PASS(Z.isSpecified("nameDatetime"     ));
                ASSERT_PASS(Z.isSpecified("nameDate"         ));
                ASSERT_PASS(Z.isSpecified("nameTime"         ));
                ASSERT_PASS(Z.isSpecified("nameCharArray"    ));
                ASSERT_PASS(Z.isSpecified("nameIntArray"     ));
                ASSERT_PASS(Z.isSpecified("nameInt64Array"   ));
                ASSERT_PASS(Z.isSpecified("nameDoubleArray"  ));
                ASSERT_PASS(Z.isSpecified("nameStringArray"  ));
                ASSERT_PASS(Z.isSpecified("nameDatetimeArray"));
                ASSERT_PASS(Z.isSpecified("nameDateArray"    ));
                ASSERT_PASS(Z.isSpecified("nameTimeArray"    ));

                int count = 0;
                ASSERT_PASS(Z.isSpecified("nameBool"         , &count));
                ASSERT_PASS(Z.isSpecified("nameChar"         , &count));
                ASSERT_PASS(Z.isSpecified("nameInt"          , &count));
                ASSERT_PASS(Z.isSpecified("nameInt64"        , &count));
                ASSERT_PASS(Z.isSpecified("nameDouble"       , &count));
                ASSERT_PASS(Z.isSpecified("nameString"       , &count));
                ASSERT_PASS(Z.isSpecified("nameDatetime"     , &count));
                ASSERT_PASS(Z.isSpecified("nameDate"         , &count));
                ASSERT_PASS(Z.isSpecified("nameTime"         , &count));
                ASSERT_PASS(Z.isSpecified("nameCharArray"    , &count));
                ASSERT_PASS(Z.isSpecified("nameIntArray"     , &count));
                ASSERT_PASS(Z.isSpecified("nameInt64Array"   , &count));
                ASSERT_PASS(Z.isSpecified("nameDoubleArray"  , &count));
                ASSERT_PASS(Z.isSpecified("nameStringArray"  , &count));
                ASSERT_PASS(Z.isSpecified("nameDatetimeArray", &count));
                ASSERT_PASS(Z.isSpecified("nameDateArray"    , &count));
                ASSERT_PASS(Z.isSpecified("nameTimeArray"    , &count));

                ASSERT_PASS(Z.numSpecified("nameBool"         ));
                ASSERT_PASS(Z.numSpecified("nameChar"         ));
                ASSERT_PASS(Z.numSpecified("nameInt"          ));
                ASSERT_PASS(Z.numSpecified("nameInt64"        ));
                ASSERT_PASS(Z.numSpecified("nameDouble"       ));
                ASSERT_PASS(Z.numSpecified("nameString"       ));
                ASSERT_PASS(Z.numSpecified("nameDatetime"     ));
                ASSERT_PASS(Z.numSpecified("nameDate"         ));
                ASSERT_PASS(Z.numSpecified("nameTime"         ));
                ASSERT_PASS(Z.numSpecified("nameCharArray"    ));
                ASSERT_PASS(Z.numSpecified("nameIntArray"     ));
                ASSERT_PASS(Z.numSpecified("nameInt64Array"   ));
                ASSERT_PASS(Z.numSpecified("nameDoubleArray"  ));
                ASSERT_PASS(Z.numSpecified("nameStringArray"  ));
                ASSERT_PASS(Z.numSpecified("nameDatetimeArray"));
                ASSERT_PASS(Z.numSpecified("nameDateArray"    ));
                ASSERT_PASS(Z.numSpecified("nameTimeArray"    ));

                ASSERT_PASS(Z.position("nameBool"         ));
                ASSERT_PASS(Z.position("nameChar"         ));
                ASSERT_PASS(Z.position("nameInt"          ));
                ASSERT_PASS(Z.position("nameInt64"        ));
                ASSERT_PASS(Z.position("nameDouble"       ));
                ASSERT_PASS(Z.position("nameString"       ));
                ASSERT_PASS(Z.position("nameDatetime"     ));
                ASSERT_PASS(Z.position("nameDate"         ));
                ASSERT_PASS(Z.position("nameTime"         ));
                ASSERT_FAIL(Z.position("nameCharArray"    ));  // non-scalar
                ASSERT_FAIL(Z.position("nameIntArray"     ));  // "        "
                ASSERT_FAIL(Z.position("nameInt64Array"   ));  // "        "
                ASSERT_FAIL(Z.position("nameDoubleArray"  ));  // "        "
                ASSERT_FAIL(Z.position("nameStringArray"  ));  // "        "
                ASSERT_FAIL(Z.position("nameDatetimeArray"));  // "        "
                ASSERT_FAIL(Z.position("nameDateArray"    ));  // "        "
                ASSERT_FAIL(Z.position("nameTimeArray"    ));  // "        "
            }

            bsl::ostringstream ossParse;
            int                retParseZ = mZ.parse(ARGC, ARGV, ossParse);

            ASSERT(0 == retParseZ);
            ASSERT( ossParse.str().empty());
            ASSERT( Z.isValid());
            ASSERT( Z.isParsed());

            {
                bsls::AssertTestHandlerGuard hGa;

                ASSERT_PASS(Z.         options());
                ASSERT_PASS(Z.specifiedOptions());

                ASSERT_PASS(Z.theBool         ("nameBool"         ));
                ASSERT_PASS(Z.theChar         ("nameChar"         ));
                ASSERT_PASS(Z.theInt          ("nameInt"          ));
                ASSERT_PASS(Z.theInt64        ("nameInt64"        ));
                ASSERT_PASS(Z.theDouble       ("nameDouble"       ));
                ASSERT_PASS(Z.theString       ("nameString"       ));
                ASSERT_PASS(Z.theDatetime     ("nameDatetime"     ));
                ASSERT_PASS(Z.theDate         ("nameDate"         ));
                ASSERT_PASS(Z.theTime         ("nameTime"         ));
                ASSERT_PASS(Z.theCharArray    ("nameCharArray"    ));
                ASSERT_PASS(Z.theIntArray     ("nameIntArray"     ));
                ASSERT_PASS(Z.theInt64Array   ("nameInt64Array"   ));
                ASSERT_PASS(Z.theDoubleArray  ("nameDoubleArray"  ));
                ASSERT_PASS(Z.theStringArray  ("nameStringArray"  ));
                ASSERT_PASS(Z.theDatetimeArray("nameDatetimeArray"));
                ASSERT_PASS(Z.theDateArray    ("nameDateArray"    ));
                ASSERT_PASS(Z.theTimeArray    ("nameTimeArray"    ));

                // Unknown option names.
                ASSERT_FAIL(Z.theBool         ("nameBool"         "X" ));
                ASSERT_FAIL(Z.theChar         ("nameChar"         "X" ));
                ASSERT_FAIL(Z.theInt          ("nameInt"          "X" ));
                ASSERT_FAIL(Z.theInt64        ("nameInt64"        "X" ));
                ASSERT_FAIL(Z.theDouble       ("nameDouble"       "X" ));
                ASSERT_FAIL(Z.theString       ("nameString"       "X" ));
                ASSERT_FAIL(Z.theDatetime     ("nameDatetime"     "X" ));
                ASSERT_FAIL(Z.theDate         ("nameDate"         "X" ));
                ASSERT_FAIL(Z.theTime         ("nameTime"         "X" ));
                ASSERT_FAIL(Z.theCharArray    ("nameCharArray"    "X" ));
                ASSERT_FAIL(Z.theIntArray     ("nameIntArray"     "X" ));
                ASSERT_FAIL(Z.theInt64Array   ("nameInt64Array"   "X" ));
                ASSERT_FAIL(Z.theDoubleArray  ("nameDoubleArray"  "X" ));
                ASSERT_FAIL(Z.theStringArray  ("nameStringArray"  "X" ));
                ASSERT_FAIL(Z.theDatetimeArray("nameDatetimeArray""X" ));
                ASSERT_FAIL(Z.theDateArray    ("nameDateArray"    "X" ));
                ASSERT_FAIL(Z.theTimeArray    ("nameTimeArray"    "X" ));

                // Mismatch option type (by name) with method type.
                ASSERT_FAIL(Z.theBool         ("nameChar"         ));
                ASSERT_FAIL(Z.theChar         ("nameInt"          ));
                ASSERT_FAIL(Z.theInt          ("nameInt64"        ));
                ASSERT_FAIL(Z.theInt64        ("nameDouble"       ));
                ASSERT_FAIL(Z.theDouble       ("nameString"       ));
                ASSERT_FAIL(Z.theString       ("nameDatetime"     ));
                ASSERT_FAIL(Z.theDatetime     ("nameDate"         ));
                ASSERT_FAIL(Z.theDate         ("nameTime"         ));
                ASSERT_FAIL(Z.theTime         ("nameCharArray"    ));
                ASSERT_FAIL(Z.theCharArray    ("nameIntArray"     ));
                ASSERT_FAIL(Z.theIntArray     ("nameInt64Array"   ));
                ASSERT_FAIL(Z.theInt64Array   ("nameDoubleArray"  ));
                ASSERT_FAIL(Z.theDoubleArray  ("nameStringArray"  ));
                ASSERT_FAIL(Z.theStringArray  ("nameDatetimeArray"));
                ASSERT_FAIL(Z.theDatetimeArray("nameDateArray"    ));
                ASSERT_FAIL(Z.theDateArray    ("nameTimeArray"    ));
                ASSERT_FAIL(Z.theTimeArray    ("nameBool"         ));

                const OptionsHandle OH = Z.options();

                ASSERT_PASS(OH.theBool         ("nameBool"         ));
                ASSERT_PASS(OH.theChar         ("nameChar"         ));
                ASSERT_PASS(OH.theInt          ("nameInt"          ));
                ASSERT_PASS(OH.theInt64        ("nameInt64"        ));
                ASSERT_PASS(OH.theDouble       ("nameDouble"       ));
                ASSERT_PASS(OH.theString       ("nameString"       ));
                ASSERT_PASS(OH.theDatetime     ("nameDatetime"     ));
                ASSERT_PASS(OH.theDate         ("nameDate"         ));
                ASSERT_PASS(OH.theTime         ("nameTime"         ));
                ASSERT_PASS(OH.theCharArray    ("nameCharArray"    ));
                ASSERT_PASS(OH.theIntArray     ("nameIntArray"     ));
                ASSERT_PASS(OH.theInt64Array   ("nameInt64Array"   ));
                ASSERT_PASS(OH.theDoubleArray  ("nameDoubleArray"  ));
                ASSERT_PASS(OH.theStringArray  ("nameStringArray"  ));
                ASSERT_PASS(OH.theDatetimeArray("nameDatetimeArray"));
                ASSERT_PASS(OH.theDateArray    ("nameDateArray"    ));
                ASSERT_PASS(OH.theTimeArray    ("nameTimeArray"    ));

                // Unknown option names.
                ASSERT_FAIL(OH.theBool         ("nameBool"         "X" ));
                ASSERT_FAIL(OH.theChar         ("nameChar"         "X" ));
                ASSERT_FAIL(OH.theInt          ("nameInt"          "X" ));
                ASSERT_FAIL(OH.theInt64        ("nameInt64"        "X" ));
                ASSERT_FAIL(OH.theDouble       ("nameDouble"       "X" ));
                ASSERT_FAIL(OH.theString       ("nameString"       "X" ));
                ASSERT_FAIL(OH.theDatetime     ("nameDatetime"     "X" ));
                ASSERT_FAIL(OH.theDate         ("nameDate"         "X" ));
                ASSERT_FAIL(OH.theTime         ("nameTime"         "X" ));
                ASSERT_FAIL(OH.theCharArray    ("nameCharArray"    "X" ));
                ASSERT_FAIL(OH.theIntArray     ("nameIntArray"     "X" ));
                ASSERT_FAIL(OH.theInt64Array   ("nameInt64Array"   "X" ));
                ASSERT_FAIL(OH.theDoubleArray  ("nameDoubleArray"  "X" ));
                ASSERT_FAIL(OH.theStringArray  ("nameStringArray"  "X" ));
                ASSERT_FAIL(OH.theDatetimeArray("nameDatetimeArray""X" ));
                ASSERT_FAIL(OH.theDateArray    ("nameDateArray"    "X" ));
                ASSERT_FAIL(OH.theTimeArray    ("nameTimeArray"    "X" ));

                // Mismatch option type (by name) with method type.
                ASSERT_FAIL(OH.theBool         ("nameChar"         ));
                ASSERT_FAIL(OH.theChar         ("nameInt"          ));
                ASSERT_FAIL(OH.theInt          ("nameInt64"        ));
                ASSERT_FAIL(OH.theInt64        ("nameDouble"       ));
                ASSERT_FAIL(OH.theDouble       ("nameString"       ));
                ASSERT_FAIL(OH.theString       ("nameDatetime"     ));
                ASSERT_FAIL(OH.theDatetime     ("nameDate"         ));
                ASSERT_FAIL(OH.theDate         ("nameTime"         ));
                ASSERT_FAIL(OH.theTime         ("nameCharArray"    ));
                ASSERT_FAIL(OH.theCharArray    ("nameIntArray"     ));
                ASSERT_FAIL(OH.theIntArray     ("nameInt64Array"   ));
                ASSERT_FAIL(OH.theInt64Array   ("nameDoubleArray"  ));
                ASSERT_FAIL(OH.theDoubleArray  ("nameStringArray"  ));
                ASSERT_FAIL(OH.theStringArray  ("nameDatetimeArray"));
                ASSERT_FAIL(OH.theDatetimeArray("nameDateArray"    ));
                ASSERT_FAIL(OH.theDateArray    ("nameTimeArray"    ));
                ASSERT_FAIL(OH.theTimeArray    ("nameBool"         ));

                // The `the` (accessor) method template.

#define THE_PASS(ENUM, ALIAS)                                                 \
    ASSERT_PASS(OH.the<Ot::EnumToType<ENUM>::type>("name"#ALIAS));            \

                THE_PASS(Ot::e_BOOL          , Bool)
                THE_PASS(Ot::e_CHAR          , Char)
                THE_PASS(Ot::e_INT           , Int)
                THE_PASS(Ot::e_INT64         , Int64)
                THE_PASS(Ot::e_DOUBLE        , Double)
                THE_PASS(Ot::e_STRING        , String)
                THE_PASS(Ot::e_DATETIME      , Datetime)
                THE_PASS(Ot::e_DATE          , Date)
                THE_PASS(Ot::e_TIME          , Time)
                THE_PASS(Ot::e_CHAR_ARRAY    , CharArray)
                THE_PASS(Ot::e_INT_ARRAY     , IntArray)
                THE_PASS(Ot::e_INT64_ARRAY   , Int64Array)
                THE_PASS(Ot::e_DOUBLE_ARRAY  , DoubleArray)
                THE_PASS(Ot::e_STRING_ARRAY  , StringArray)
                THE_PASS(Ot::e_DATETIME_ARRAY, DatetimeArray)
                THE_PASS(Ot::e_DATE_ARRAY    , DateArray)
                THE_PASS(Ot::e_TIME_ARRAY    , TimeArray)
#undef THE_PASS

#define THE_FAIL(ENUM, ALIAS)                                                 \
    ASSERT_FAIL(OH.the<Ot::EnumToType<ENUM>::type>("name"#ALIAS "X"));        \

                THE_FAIL(Ot::e_BOOL          , Bool)
                THE_FAIL(Ot::e_CHAR          , Char)
                THE_FAIL(Ot::e_INT           , Int)
                THE_FAIL(Ot::e_INT64         , Int64)
                THE_FAIL(Ot::e_DOUBLE        , Double)
                THE_FAIL(Ot::e_STRING        , String)
                THE_FAIL(Ot::e_DATETIME      , Datetime)
                THE_FAIL(Ot::e_DATE          , Date)
                THE_FAIL(Ot::e_TIME          , Time)
                THE_FAIL(Ot::e_CHAR_ARRAY    , CharArray)
                THE_FAIL(Ot::e_INT_ARRAY     , IntArray)
                THE_FAIL(Ot::e_INT64_ARRAY   , Int64Array)
                THE_FAIL(Ot::e_DOUBLE_ARRAY  , DoubleArray)
                THE_FAIL(Ot::e_STRING_ARRAY  , StringArray)
                THE_FAIL(Ot::e_DATETIME_ARRAY, DatetimeArray)
                THE_FAIL(Ot::e_DATE_ARRAY    , DateArray)
                THE_FAIL(Ot::e_TIME_ARRAY    , TimeArray)

#undef THE_FAIL

#define THE_FAIL(ENUM, ALIAS)                                                 \
    ASSERT_FAIL(OH.the<Ot::EnumToType<ENUM>::type>("name"#ALIAS));            \

                THE_FAIL(Ot::e_BOOL          , Char)
                THE_FAIL(Ot::e_CHAR          , Int)
                THE_FAIL(Ot::e_INT           , Int64)
                THE_FAIL(Ot::e_INT64         , Double)
                THE_FAIL(Ot::e_DOUBLE        , String)
                THE_FAIL(Ot::e_STRING        , Datetime)
                THE_FAIL(Ot::e_DATETIME      , Date)
                THE_FAIL(Ot::e_DATE          , Time)
                THE_FAIL(Ot::e_TIME          , CharArray)
                THE_FAIL(Ot::e_CHAR_ARRAY    , IntArray)
                THE_FAIL(Ot::e_INT_ARRAY     , Int64Array)
                THE_FAIL(Ot::e_INT64_ARRAY   , DoubleArray)
                THE_FAIL(Ot::e_DOUBLE_ARRAY  , StringArray)
                THE_FAIL(Ot::e_STRING_ARRAY  , DatetimeArray)
                THE_FAIL(Ot::e_DATETIME_ARRAY, DateArray)
                THE_FAIL(Ot::e_DATE_ARRAY    , TimeArray)
                THE_FAIL(Ot::e_TIME_ARRAY    , Bool)
#undef THE_FAIL

                // Indexed oriented accessors

                const bsl::size_t NUM_OPTIONS = OH.numOptions();

                ASSERT(0 < NUM_OPTIONS);

                ASSERT_PASS(OH.name(NUM_OPTIONS - 1));
                ASSERT_FAIL(OH.name(NUM_OPTIONS    ));

                ASSERT_PASS(OH.type(NUM_OPTIONS - 1));
                ASSERT_FAIL(OH.type(NUM_OPTIONS    ));

                ASSERT_PASS(OH.value(NUM_OPTIONS - 1));
                ASSERT_FAIL(OH.value(NUM_OPTIONS    ));
            }
        }

        if (veryVerbose) cout << "Negative Testing: ACCESSORS" << endl;
        {
            typedef bool                            Bool;
            typedef char                            Char;
            typedef int                             Int;
            typedef bsls::Types::Int64              Int64;
            typedef double                          Double;
            typedef bsl::string                     String;
            typedef bdlt::Datetime                  Datetime;
            typedef bdlt::Date                      Date;
            typedef bdlt::Time                      Time;
            typedef bsl::vector<char>               CharArray;
            typedef bsl::vector<int>                IntArray;
            typedef bsl::vector<bsls::Types::Int64> Int64Array;
            typedef bsl::vector<double>             DoubleArray;
            typedef bsl::vector<bsl::string>        StringArray;
            typedef bsl::vector<bdlt::Datetime>     DatetimeArray;
            typedef bsl::vector<bdlt::Date>         DateArray;
            typedef bsl::vector<bdlt::Time>         TimeArray;

            // Specification table:
            //  - all supported types
            //  - each optional
            //  - no default values
            balcl::OptionInfo specTable[] = {
              {
                "a|aLongTag"
              , "Bool" // name
              , "description:  Bool"
              , TypeInfo(Ot::k_BOOL)
              , OccurrenceInfo::e_OPTIONAL
              , ""
              }
            , {
                "b|bLongTag"
              , "Char" // name
              , "description:  Char"
              , TypeInfo(Ot::k_CHAR)
              , OccurrenceInfo::e_OPTIONAL
              , ""
              }
            , {
                "c|cLongTag"
              , "Int" // name
              , "description:  Int"
              , TypeInfo(Ot::k_INT)
              , OccurrenceInfo::e_OPTIONAL
              , ""
              }
            , {
                "d|dLongTag"
              , "Int64" // name
              , "description:  Int64"
              , TypeInfo(Ot::k_INT64)
              , OccurrenceInfo::e_OPTIONAL
              , ""
              }
            , {
                "e|eLongTag"
              , "Double" // name
              , "description:  Double"
              , TypeInfo(Ot::k_DOUBLE)
              , OccurrenceInfo::e_OPTIONAL
              , ""
              }
            , {
                "f|fLongTag"
              , "String" // name
              , "description:  String"
              , TypeInfo(Ot::k_STRING)
              , OccurrenceInfo::e_OPTIONAL
              , ""
              }
            , {
                "g|gLongTag"
              , "Datetime" // name
              , "description:  Datetime"
              , TypeInfo(Ot::k_DATETIME)
              , OccurrenceInfo::e_OPTIONAL
              , ""
              }
            , {
                "h|hLongTag"
              , "Date" // name
              , "description:  Date"
              , TypeInfo(Ot::k_DATE)
              , OccurrenceInfo::e_OPTIONAL
              , ""
              }
            , {
                "i|iLongTag"
              , "Time" // name
              , "description:  Time"
              , TypeInfo(Ot::k_TIME)
              , OccurrenceInfo::e_OPTIONAL
              , ""
              }
            , {
                "j|jLongTag"
              , "CharArray" // name
              , "description:  CharArray"
              , TypeInfo(Ot::k_CHAR_ARRAY)
              , OccurrenceInfo::e_OPTIONAL
              , ""
              }
            , {
                "k|kLongTag"
              , "IntArray" // name
              , "description:  IntArray"
              , TypeInfo(Ot::k_INT_ARRAY)
              , OccurrenceInfo::e_OPTIONAL
              , ""
              }
            , {
                "l|lLongTag"
              , "Int64Array" // name
              , "description:  Int64Array"
              , TypeInfo(Ot::k_INT64_ARRAY)
              , OccurrenceInfo::e_OPTIONAL
              , ""
              }
            , {
                "m|mLongTag"
              , "DoubleArray" // name
              , "description:  DoubleArray"
              , TypeInfo(Ot::k_DOUBLE_ARRAY)
              , OccurrenceInfo::e_OPTIONAL
              , ""
              }
            , {
                "n|nLongTag"
              , "StringArray" // name
              , "description:  StringArray"
              , TypeInfo(Ot::k_STRING_ARRAY)
              , OccurrenceInfo::e_OPTIONAL
              , ""
              }
            , {
                "o|oLongTag"
              , "DatetimeArray" // name
              , "description:  DatetimeArray"
              , TypeInfo(Ot::k_DATETIME_ARRAY)
              , OccurrenceInfo::e_OPTIONAL
              , ""
              }
            , {
                "p|pLongTag"
              , "DateArray" // name
              , "description:  DateArray"
              , TypeInfo(Ot::k_DATE_ARRAY)
              , OccurrenceInfo::e_OPTIONAL
              , ""
              }
            , {
                "r|rLongTag"
              , "TimeArray" // name
              , "description:  TimeArray"
              , TypeInfo(Ot::k_TIME_ARRAY)
              , OccurrenceInfo::e_OPTIONAL
              , ""
              }
            };

            bsl::size_t numSpecTable = sizeof specTable / sizeof *specTable;
            ASSERT(17 == numSpecTable);

            bsl::ostringstream oss;

            bool isValidSpec = Obj::isValidOptionSpecificationTable(specTable,
                                                                    oss);
            ASSERT(isValidSpec);
            ASSERT(oss.str().empty());

            Obj mX(specTable); const Obj& X = mX;

            const char *const    emptyCommandLine[] = { "programName" };
            const bsl::size_t numEmptyCommandLine = sizeof  emptyCommandLine
                                                  / sizeof *emptyCommandLine;

            int retParse = mX.parse(numEmptyCommandLine,
                                    emptyCommandLine,
                                    oss);

            ASSERT(0 == retParse);
            ASSERT(X.isParsed());
            ASSERT(X.isValid());

            // Test Handle accessors.  Use `specifiedOptions` so we can have a
            // boolean option without a value.
            OptionsHandle SH = X.specifiedOptions();
            ASSERT(numSpecTable == SH.numOptions());

            const bsl::size_t  badIndex = SH.numOptions() + 1;
            const char        *badName  = "badName";

            bsls::AssertTestHandlerGuard hGa;

            ASSERT_FAIL(SH.name(badIndex));

            ASSERT_FAIL(SH.type(badIndex));
            ASSERT_FAIL(SH.type(badName));

            ASSERT_FAIL(SH.value(badIndex));
            ASSERT_FAIL(SH.value(badName));

            ASSERT_FAIL(X.hasValue(badName));
            ASSERT_FAIL(X.type    (badName));

            for (bsl::size_t i = 0; i < SH.numOptions(); ++i) {
                Ot::Enum    type = SH.type(i);
                const char *name = SH.name(i);

                if (veryVerbose) {
                    T_ P_(i) P_(type) P(name)
                }

                if (!SH.value(name).isNull()) {
                    P_(i) P_(type) P(name)
                }

                switch (type) {
                  case Ot::e_VOID: {
                    ASSERTV("!Reached", 0);
                  } break;
                  case Ot::e_BOOL: {
                    // Good Type; bad arguments
                    ASSERT_FAIL(SH.the<Bool>(badIndex));
                    ASSERT_FAIL(SH.the<Bool>(badName));
                    ASSERT_FAIL(SH.theBool(badName));
                    ASSERT_FAIL(X .theBool(bsl::string(badName)));

                    // Bad Type; good argument
                    ASSERT_FAIL(SH.theChar  (name));
                    ASSERT_FAIL(SH.the<Char>(name));
                    ASSERT_FAIL(SH.the<Char>(i));
                    ASSERT_FAIL(X .theChar(bsl::string(name)));

                    // No value
                    ASSERT_FAIL(SH.the<Bool>(i));
                    ASSERT_FAIL(SH.the<Bool>(name));
                    ASSERT_FAIL(SH.theBool(name));
                    ASSERT_PASS(X .theBool(bsl::string(name)));
                        // No flag so the bool option is `false`.
                  } break;
                  case Ot::e_CHAR: {
                    // Good Type; bad arguments
                    ASSERT_FAIL(SH.the<Char>(badIndex));
                    ASSERT_FAIL(SH.the<Char>(badName));
                    ASSERT_FAIL(SH.theChar(badName));
                    ASSERT_FAIL(X .theChar(bsl::string(badName)));

                    // Bad Type; good argument
                    ASSERT_FAIL(SH.theInt  (name));
                    ASSERT_FAIL(SH.the<int>(name));
                    ASSERT_FAIL(SH.the<Int>(i));
                    ASSERT_FAIL(X .theInt(bsl::string(name)));

                    // No value
                    ASSERT_FAIL(SH.the<Char>(i));
                    ASSERT_FAIL(SH.the<Char>(name));
                    ASSERT_FAIL(SH.theChar(name));
                    ASSERT_FAIL(X .theChar(bsl::string(name)));
                  } break;
                  case Ot::e_INT: {
                    // Good Type; bad arguments
                    ASSERT_FAIL(SH.the<Int>(badIndex));
                    ASSERT_FAIL(SH.the<Int>(badName));
                    ASSERT_FAIL(SH.theInt(badName));
                    ASSERT_FAIL(X .theInt(bsl::string(badName)));

                    // Bad Type; good argument
                    ASSERT_FAIL(SH.theInt64  (name));
                    ASSERT_FAIL(SH.the<Int64>(name));
                    ASSERT_FAIL(SH.the<Int64>(i));
                    ASSERT_FAIL(X .theInt64(bsl::string(name)));

                    // No value
                    ASSERT_FAIL(SH.the<Int>(i));
                    ASSERT_FAIL(SH.the<Int>(name));
                    ASSERT_FAIL(SH.theInt(name));
                    ASSERT_FAIL(X .theInt(bsl::string(name)));
                  } break;
                  case Ot::e_INT64: {
                    // Good Type; bad arguments
                    ASSERT_FAIL(SH.the<Int64>(badIndex));
                    ASSERT_FAIL(SH.the<Int64>(badName));
                    ASSERT_FAIL(SH.theInt64(badName));
                    ASSERT_FAIL(X .theInt64(bsl::string(badName)));

                    // Bad Type; good argument
                    ASSERT_FAIL(SH.theDouble  (name));
                    ASSERT_FAIL(SH.the<Double>(name));
                    ASSERT_FAIL(SH.the<Double>(i));
                    ASSERT_FAIL(X .theDouble(bsl::string(name)));

                    // No value
                    ASSERT_FAIL(SH.the<Int64>(i));
                    ASSERT_FAIL(SH.the<Int64>(name));
                    ASSERT_FAIL(SH.theInt64(name));
                    ASSERT_FAIL(X .theInt64(bsl::string(name)));
                  } break;
                  case Ot::e_DOUBLE: {
                    // Good Type; bad arguments
                    ASSERT_FAIL(SH.the<Double>(badIndex));
                    ASSERT_FAIL(SH.the<Double>(badName));
                    ASSERT_FAIL(SH.theDouble(badName));
                    ASSERT_FAIL(X .theDouble(bsl::string(badName)));

                    // Bad Type; good argument
                    ASSERT_FAIL(SH.theString  (name));
                    ASSERT_FAIL(SH.the<String>(name));
                    ASSERT_FAIL(SH.the<String>(i));
                    ASSERT_FAIL(X .theString(bsl::string(name)));

                    // No value
                    ASSERT_FAIL(SH.the<Double>(i));
                    ASSERT_FAIL(SH.the<Double>(name));
                    ASSERT_FAIL(SH.theDouble(name));
                    ASSERT_FAIL(X .theDouble(bsl::string(name)));
                  } break;
                  case Ot::e_STRING: {
                    // Good Type; bad arguments
                    ASSERT_FAIL(SH.the<String>(badIndex));
                    ASSERT_FAIL(SH.the<String>(badName));
                    ASSERT_FAIL(SH.theString(badName));
                    ASSERT_FAIL(X .theString(bsl::string(badName)));

                    // Bad Type; good argument
                    ASSERT_FAIL(SH.theDatetime  (name));
                    ASSERT_FAIL(SH.the<Datetime>(name));
                    ASSERT_FAIL(SH.the<Datetime>(i));
                    ASSERT_FAIL(X .theDatetime(bsl::string(name)));

                    // No value
                    ASSERT_FAIL(SH.the<String>(i));
                    ASSERT_FAIL(SH.the<String>(name));
                    ASSERT_FAIL(SH.theString(name));
                    ASSERT_FAIL(X .theString(bsl::string(name)));
                  } break;
                  case Ot::e_DATETIME: {
                    // Good Type; bad arguments
                    ASSERT_FAIL(SH.the<Datetime>(badIndex));
                    ASSERT_FAIL(SH.the<Datetime>(badName));
                    ASSERT_FAIL(SH.theDatetime(badName));
                    ASSERT_FAIL(X .theDatetime(bsl::string(badName)));

                    // Bad Type; good argument
                    ASSERT_FAIL(SH.theDate  (name));
                    ASSERT_FAIL(SH.the<Date>(name));
                    ASSERT_FAIL(SH.the<Date>(i));
                    ASSERT_FAIL(X .theDate(bsl::string(name)));

                    // No value
                    ASSERT_FAIL(SH.the<Datetime>(i));
                    ASSERT_FAIL(SH.the<Datetime>(name));
                    ASSERT_FAIL(SH.theDatetime(name));
                    ASSERT_FAIL(X .theDatetime(bsl::string(name)));
                  } break;
                  case Ot::e_DATE: {
                    // Good Type; bad arguments
                    ASSERT_FAIL(SH.the<Date>(badIndex));
                    ASSERT_FAIL(SH.the<Date>(badName));
                    ASSERT_FAIL(SH.theDate(badName));
                    ASSERT_FAIL(X .theDate(bsl::string(badName)));

                    // Bad Type; good argument
                    ASSERT_FAIL(SH.theTime  (name));
                    ASSERT_FAIL(SH.the<Time>(name));
                    ASSERT_FAIL(SH.the<Time>(i));
                    ASSERT_FAIL(X .theTime(bsl::string(name)));

                    // No value
                    ASSERT_FAIL(SH.the<Date>(i));
                    ASSERT_FAIL(SH.the<Date>(name));
                    ASSERT_FAIL(SH.theDate(name));
                    ASSERT_FAIL(X .theDate(bsl::string(name)));
                  } break;
                  case Ot::e_TIME: {
                    // Good Type; bad arguments
                    ASSERT_FAIL(SH.the<Time>(badIndex));
                    ASSERT_FAIL(SH.the<Time>(badName));
                    ASSERT_FAIL(SH.theTime(badName));
                    ASSERT_FAIL(X .theTime(bsl::string(badName)));

                    // Bad Type; good argument
                    ASSERT_FAIL(SH.theCharArray  (name));
                    ASSERT_FAIL(SH.the<CharArray>(name));
                    ASSERT_FAIL(SH.the<CharArray>(i));
                    ASSERT_FAIL(X .theCharArray(bsl::string(name)));

                    // No value
                    ASSERT_FAIL(SH.the<Time>(i));
                    ASSERT_FAIL(SH.the<Time>(name));
                    ASSERT_FAIL(SH.theTime(name));
                    ASSERT_FAIL(X .theTime(bsl::string(name)));
                  } break;
                  case Ot::e_CHAR_ARRAY: {
                    // Good Type; bad arguments
                    ASSERT_FAIL(SH.the<CharArray>(badIndex));
                    ASSERT_FAIL(SH.the<CharArray>(badName));
                    ASSERT_FAIL(SH.theCharArray(badName));
                    ASSERT_FAIL(X .theCharArray(bsl::string(badName)));

                    // Bad Type; good argument
                    ASSERT_FAIL(SH.theIntArray  (name));
                    ASSERT_FAIL(SH.the<IntArray>(name));
                    ASSERT_FAIL(SH.the<IntArray>(i));
                    ASSERT_FAIL(X .theIntArray(bsl::string(name)));

                    // No value
                    ASSERT_FAIL(SH.the<CharArray>(i));
                    ASSERT_FAIL(SH.the<CharArray>(name));
                    ASSERT_FAIL(SH.theCharArray(name));
                    ASSERT_FAIL(X .theCharArray(bsl::string(name)));
                  } break;
                  case Ot::e_INT_ARRAY: {
                    // Good Type; bad arguments
                    ASSERT_FAIL(SH.the<IntArray>(badIndex));
                    ASSERT_FAIL(SH.the<IntArray>(badName));
                    ASSERT_FAIL(SH.theIntArray(badName));
                    ASSERT_FAIL(X .theIntArray(bsl::string(badName)));

                    // Bad Type; good argument
                    ASSERT_FAIL(SH.theInt64Array  (name));
                    ASSERT_FAIL(SH.the<Int64Array>(name));
                    ASSERT_FAIL(SH.the<Int64Array>(i));
                    ASSERT_FAIL(X .theInt64Array(bsl::string(name)));

                    // No value
                    ASSERT_FAIL(SH.the<IntArray>(i));
                    ASSERT_FAIL(SH.the<IntArray>(name));
                    ASSERT_FAIL(SH.theIntArray(name));
                    ASSERT_FAIL(X .theIntArray(bsl::string(name)));
                  } break;
                  case Ot::e_INT64_ARRAY: {
                    // Good Type; bad arguments
                    ASSERT_FAIL(SH.the<Int64Array>(badIndex));
                    ASSERT_FAIL(SH.the<Int64Array>(badName));
                    ASSERT_FAIL(SH.theInt64Array(badName));
                    ASSERT_FAIL(X .theInt64Array(bsl::string(badName)));

                    // Bad Type; good argument
                    ASSERT_FAIL(SH.theDoubleArray  (name));
                    ASSERT_FAIL(SH.the<DoubleArray>(name));
                    ASSERT_FAIL(SH.the<DoubleArray>(i));
                    ASSERT_FAIL(X .theDoubleArray(bsl::string(name)));

                    // No value
                    ASSERT_FAIL(SH.the<Int64Array>(i));
                    ASSERT_FAIL(SH.the<Int64Array>(name));
                    ASSERT_FAIL(SH.theInt64Array(name));
                    ASSERT_FAIL(X .theInt64Array(bsl::string(name)));
                  } break;
                  case Ot::e_DOUBLE_ARRAY: {
                    // Good Type; bad arguments
                    ASSERT_FAIL(SH.the<DoubleArray>(badIndex));
                    ASSERT_FAIL(SH.the<DoubleArray>(badName));
                    ASSERT_FAIL(SH.theDoubleArray(badName));
                    ASSERT_FAIL(X .theDoubleArray(bsl::string(badName)));

                    // Bad Type; good argument
                    ASSERT_FAIL(SH.theStringArray  (name));
                    ASSERT_FAIL(SH.the<StringArray>(name));
                    ASSERT_FAIL(SH.the<StringArray>(i));
                    ASSERT_FAIL(X .theStringArray(bsl::string(name)));

                    // No value
                    ASSERT_FAIL(SH.the<DoubleArray>(i));
                    ASSERT_FAIL(SH.the<DoubleArray>(name));
                    ASSERT_FAIL(SH.theDoubleArray(name));
                    ASSERT_FAIL(X .theDoubleArray(bsl::string(name)));
                  } break;
                  case Ot::e_STRING_ARRAY: {
                    // Good Type; bad arguments
                    ASSERT_FAIL(SH.the<StringArray>(badIndex));
                    ASSERT_FAIL(SH.the<StringArray>(badName));
                    ASSERT_FAIL(SH.theStringArray(badName));
                    ASSERT_FAIL(X .theStringArray(bsl::string(badName)));

                    // Bad Type; good argument
                    ASSERT_FAIL(SH.theDatetimeArray  (name));
                    ASSERT_FAIL(SH.the<DatetimeArray>(name));
                    ASSERT_FAIL(SH.the<DatetimeArray>(i));
                    ASSERT_FAIL(X .theDatetimeArray(bsl::string(name)));

                    // No value
                    ASSERT_FAIL(SH.the<StringArray>(i));
                    ASSERT_FAIL(SH.the<StringArray>(name));
                    ASSERT_FAIL(SH.theStringArray(name));
                    ASSERT_FAIL(X .theStringArray(bsl::string(name)));
                  } break;
                  case Ot::e_DATETIME_ARRAY: {
                    // Good Type; bad arguments
                    ASSERT_FAIL(SH.the<DatetimeArray>(badIndex));
                    ASSERT_FAIL(SH.the<DatetimeArray>(badName));
                    ASSERT_FAIL(SH.theDatetimeArray(badName));
                    ASSERT_FAIL(X .theDatetimeArray(bsl::string(badName)));

                    // Bad Type; good argument
                    ASSERT_FAIL(SH.theDateArray  (name));
                    ASSERT_FAIL(SH.the<DateArray>(name));
                    ASSERT_FAIL(SH.the<DateArray>(i));
                    ASSERT_FAIL(X .theDateArray(bsl::string(name)));

                    // No value
                    ASSERT_FAIL(SH.the<DatetimeArray>(i));
                    ASSERT_FAIL(SH.the<DatetimeArray>(name));
                    ASSERT_FAIL(SH.theDatetimeArray(name));
                    ASSERT_FAIL(X .theDatetimeArray(bsl::string(name)));
                  } break;
                  case Ot::e_DATE_ARRAY: {
                    // Good Type; bad arguments
                    ASSERT_FAIL(SH.the<DateArray>(badIndex));
                    ASSERT_FAIL(SH.the<DateArray>(badName));
                    ASSERT_FAIL(SH.theDateArray(badName));
                    ASSERT_FAIL(X .theDateArray(bsl::string(badName)));

                    // Bad Type; good argument
                    ASSERT_FAIL(SH.theTimeArray  (name));
                    ASSERT_FAIL(SH.the<TimeArray>(name));
                    ASSERT_FAIL(SH.the<TimeArray>(i));
                    ASSERT_FAIL(X .theTimeArray(bsl::string(name)));

                    // No value
                    ASSERT_FAIL(SH.the<DateArray>(i));
                    ASSERT_FAIL(SH.the<DateArray>(name));
                    ASSERT_FAIL(SH.theDateArray(name));
                    ASSERT_FAIL(X .theDateArray(bsl::string(name)));
                  } break;
                  case Ot::e_TIME_ARRAY: {
                    // Good Type; bad arguments
                    ASSERT_FAIL(SH.the<TimeArray>(badIndex));
                    ASSERT_FAIL(SH.the<TimeArray>(badName));
                    ASSERT_FAIL(SH.theTimeArray(badName));
                    ASSERT_FAIL(X .theTimeArray(bsl::string(badName)));

                    // Bad Type; good argument
                    ASSERT_FAIL(SH.theBool  (name));
                    ASSERT_FAIL(SH.the<Bool>(name));
                    ASSERT_FAIL(SH.the<Bool>(i));
                    ASSERT_FAIL(X .theBool(bsl::string(name)));

                    // No value
                    ASSERT_FAIL(SH.the<TimeArray>(i));
                    ASSERT_FAIL(SH.the<TimeArray>(name));
                    ASSERT_FAIL(SH.theTimeArray(name));
                    ASSERT_FAIL(X .theTimeArray(bsl::string(name)));
                  } break;
                }
            }

            // Specification table:
            //  - all supported types
            //  - each type has a default value (except `bool`, of course).

            balcl::OptionInfo specTable2[] = {
              {
                "a|aLongTag"
              , "Bool" // name
              , "description:  Bool"
              , TypeInfo(Ot::k_BOOL)
              , OccurrenceInfo::e_OPTIONAL
              , ""
              }
            , {
                "b|bLongTag"
              , "Char" // name
              , "description:  Char"
              , TypeInfo(Ot::k_CHAR)
              , OccurrenceInfo(Char())
              , ""
              }
            , {
                "c|cLongTag"
              , "Int" // name
              , "description:  Int"
              , TypeInfo(Ot::k_INT)
              , OccurrenceInfo(Int())
              , ""
              }
            , {
                "d|dLongTag"
              , "Int64" // name
              , "description:  Int64"
              , TypeInfo(Ot::k_INT64)
              , OccurrenceInfo(Int64())
              , ""
              }
            , {
                "e|eLongTag"
              , "Double" // name
              , "description:  Double"
              , TypeInfo(Ot::k_DOUBLE)
              , OccurrenceInfo(Double())
              , ""
              }
            , {
                "f|fLongTag"
              , "String" // name
              , "description:  String"
              , TypeInfo(Ot::k_STRING)
              , OccurrenceInfo(String())
              , ""
              }
            , {
                "g|gLongTag"
              , "Datetime" // name
              , "description:  Datetime"
              , TypeInfo(Ot::k_DATETIME)
              , OccurrenceInfo(Datetime())
              , ""
              }
            , {
                "h|hLongTag"
              , "Date" // name
              , "description:  Date"
              , TypeInfo(Ot::k_DATE)
              , OccurrenceInfo(Date())
              , ""
              }
            , {
                "i|iLongTag"
              , "Time" // name
              , "description:  Time"
              , TypeInfo(Ot::k_TIME)
              , OccurrenceInfo(Time())
              , ""
              }
            , {
                "j|jLongTag"
              , "CharArray" // name
              , "description:  CharArray"
              , TypeInfo(Ot::k_CHAR_ARRAY)
              , OccurrenceInfo(CharArray())
              , ""
              }
            , {
                "k|kLongTag"
              , "IntArray" // name
              , "description:  IntArray"
              , TypeInfo(Ot::k_INT_ARRAY)
              , OccurrenceInfo(IntArray())
              , ""
              }
            , {
                "l|lLongTag"
              , "Int64Array" // name
              , "description:  Int64Array"
              , TypeInfo(Ot::k_INT64_ARRAY)
              , OccurrenceInfo(Int64Array())
              , ""
              }
            , {
                "m|mLongTag"
              , "DoubleArray" // name
              , "description:  DoubleArray"
              , TypeInfo(Ot::k_DOUBLE_ARRAY)
              , OccurrenceInfo(DoubleArray())
              , ""
              }
            , {
                "n|nLongTag"
              , "StringArray" // name
              , "description:  StringArray"
              , TypeInfo(Ot::k_STRING_ARRAY)
              , OccurrenceInfo(StringArray())
              , ""
              }
            , {
                "o|oLongTag"
              , "DatetimeArray" // name
              , "description:  DatetimeArray"
              , TypeInfo(Ot::k_DATETIME_ARRAY)
              , OccurrenceInfo(DatetimeArray())
              , ""
              }
            , {
                "p|pLongTag"
              , "DateArray" // name
              , "description:  DateArray"
              , TypeInfo(Ot::k_DATE_ARRAY)
              , OccurrenceInfo(DateArray())
              , ""
              }
            , {
                "r|rLongTag"
              , "TimeArray" // name
              , "description:  TimeArray"
              , TypeInfo(Ot::k_TIME_ARRAY)
              , OccurrenceInfo(TimeArray())
              , ""
              }
            };

            bsl::size_t numSpecTable2 = sizeof specTable / sizeof *specTable;
            ASSERT(17 == numSpecTable2);

            bsl::ostringstream oss2;

            bool isValidSpec2 = Obj::isValidOptionSpecificationTable(
                                                                    specTable2,
                                                                    oss2);
            ASSERT(isValidSpec2);
            ASSERT(oss2.str().empty());

            Obj mY(specTable2); const Obj& Y = mY;

            const char *const    commandLine[] = { "programName", "-a" };
            const bsl::size_t numCommandLine   = sizeof  commandLine
                                               / sizeof *commandLine;

            for (int pass = 1; pass <= 2; ++ pass ) {

                if (2 == pass) {
                    int retParse2 = mY.parse(numCommandLine,
                                             commandLine,
                                             oss2);
                    ASSERT(0 == retParse2);
                    ASSERT(Y.isParsed());
                    ASSERT(Y.isValid());
                }

                switch (pass) {
                  case 1: {
                    ASSERT(!Y.isParsed());
                  } break;
                  case 2: {
                    ASSERT( Y.isParsed());
                  } break;
                }

                for (bsl::size_t i = 0; i < numSpecTable2; ++i) {
                    Ot::Enum    type = specTable2[i].d_typeInfo.type();
                    const char *name = specTable2[i].d_name.c_str();

                    if (veryVerbose) {
                        T_ P_(i) P_(type) P(name)
                    }

                    if (Y.isParsed()) {
                        ASSERT_PASS(Y.hasValue(name));
                    } else {
                        ASSERT_FAIL(Y.hasValue(name));
                    }

                    switch (type) {
                      case Ot::e_VOID: {
                        ASSERTV("!Reached", 0);
                      } break;
                      case Ot::e_BOOL: {
                        if (Y.isParsed()) {
                            ASSERT_PASS(Y.theBool(name));
                        } else {
                            ASSERT_FAIL(Y.theBool(name));
                        }
                      } break;
                      case Ot::e_CHAR: {
                        if (Y.isParsed()) {
                            ASSERT_PASS(Y.theChar(name));
                        } else {
                            ASSERT_FAIL(Y.theChar(name));
                        }
                      } break;
                      case Ot::e_INT: {
                        if (Y.isParsed()) {
                            ASSERT_PASS(Y.theInt(name));
                        } else {
                            ASSERT_FAIL(Y.theInt(name));
                        }
                      } break;
                      case Ot::e_INT64: {
                        if (Y.isParsed()) {
                            ASSERT_PASS(Y.theInt64(name));
                        } else {
                            ASSERT_FAIL(Y.theInt64(name));
                        }
                      } break;
                      case Ot::e_DOUBLE: {
                        if (Y.isParsed()) {
                            ASSERT_PASS(Y.theDouble(name));
                        } else {
                            ASSERT_FAIL(Y.theDouble(name));
                        }
                      } break;
                      case Ot::e_STRING: {
                        if (Y.isParsed()) {
                            ASSERT_PASS(Y.theString(name));
                        } else {
                            ASSERT_FAIL(Y.theString(name));
                        }
                      } break;
                      case Ot::e_DATETIME: {
                        if (Y.isParsed()) {
                            ASSERT_PASS(Y.theDatetime(name));
                        } else {
                            ASSERT_FAIL(Y.theDatetime(name));
                        }
                      } break;
                      case Ot::e_DATE: {
                        if (Y.isParsed()) {
                            ASSERT_PASS(Y.theDate(name));
                        } else {
                            ASSERT_FAIL(Y.theDate(name));
                        }
                      } break;
                      case Ot::e_TIME: {
                        if (Y.isParsed()) {
                            ASSERT_PASS(Y.theTime(name));
                        } else {
                            ASSERT_FAIL(Y.theTime(name));
                        }
                      } break;
                      case Ot::e_CHAR_ARRAY: {
                        if (Y.isParsed()) {
                            ASSERT_PASS(Y.theCharArray(name));
                        } else {
                            ASSERT_FAIL(Y.theCharArray(name));
                        }
                      } break;
                      case Ot::e_INT_ARRAY: {
                        if (Y.isParsed()) {
                            ASSERT_PASS(Y.theIntArray(name));
                        } else {
                            ASSERT_FAIL(Y.theIntArray(name));
                        }
                      } break;
                      case Ot::e_INT64_ARRAY: {
                        if (Y.isParsed()) {
                            ASSERT_PASS(Y.theInt64Array(name));
                        } else {
                            ASSERT_FAIL(Y.theInt64Array(name));
                        }
                      } break;
                      case Ot::e_DOUBLE_ARRAY: {
                        if (Y.isParsed()) {
                            ASSERT_PASS(Y.theDoubleArray(name));
                        } else {
                            ASSERT_FAIL(Y.theDoubleArray(name));
                        }
                      } break;
                      case Ot::e_STRING_ARRAY: {
                        if (Y.isParsed()) {
                            ASSERT_PASS(Y.theStringArray(name));
                        } else {
                            ASSERT_FAIL(Y.theStringArray(name));
                        }
                      } break;
                      case Ot::e_DATETIME_ARRAY: {
                        if (Y.isParsed()) {
                            ASSERT_PASS(Y.theDatetimeArray(name));
                        } else {
                            ASSERT_FAIL(Y.theDatetimeArray(name));
                        }
                      } break;
                      case Ot::e_DATE_ARRAY: {
                        if (Y.isParsed()) {
                            ASSERT_PASS(Y.theDateArray(name));
                        } else {
                            ASSERT_FAIL(Y.theDateArray(name));
                        }
                      } break;
                      case Ot::e_TIME_ARRAY: {
                        if (Y.isParsed()) {
                            ASSERT_PASS(Y.theTimeArray(name));
                        } else {
                            ASSERT_FAIL(Y.theTimeArray(name));
                        }
                      } break;
                    }
                }
            }
        }

        if (veryVerbose) cout << "Additional tests `position`/`positions`"
                              << endl;
        {
            // Specify allowed options.

            const OptionInfo TABLE[] = {
                 { "a|aLong"
                 , "aName"
                 , "Description for a"
                 , TypeInfo(Ot::k_BOOL)
                 , OccurrenceInfo::e_OPTIONAL
                 , ""
                 }

               , { "b|bLong"
                 , "bName"
                 , "Description for b"
                 , TypeInfo(Ot::k_BOOL)
                 , OccurrenceInfo::e_OPTIONAL
                 , ""
                 }

               , { "c|cLong"
                 , "cName"
                 , "Description for c"
                 , TypeInfo(Ot::k_INT)
                 , OccurrenceInfo::e_OPTIONAL
                 , ""
                 }
               , { "d|dLong"
                 , "dName"
                 , "Description for d"
                 , TypeInfo(Ot::k_INT_ARRAY)
                 , OccurrenceInfo::e_OPTIONAL
                 , ""
                 }
               , { ""    // non-option
                 , "eName"
                 , "Description for e"
                 , TypeInfo(Ot::k_STRING)
                 , OccurrenceInfo::e_OPTIONAL
                 , ""
                 }
               , { ""    // array non-option, must be last
                 , "fName"
                 , "Description for f"
                 , TypeInfo(Ot::k_STRING_ARRAY)
                 , OccurrenceInfo::e_OPTIONAL
                 , ""
                 }
            };

            enum { k_NUM_TABLE = sizeof TABLE / sizeof *TABLE };

            ASSERT(Obj::isValidOptionSpecificationTable(TABLE));

            // Retain `OptionInfo` data in map for convenient reference later.

            typedef bsl::map<bsl::string, OptionInfo> OptionInfoMap;

            OptionInfoMap  oim; const OptionInfoMap& Oim = oim;

            for (int i = 0; i < k_NUM_TABLE; ++i) {
                const OptionInfo *oiPtr = &TABLE[i];

                typedef OptionInfoMap::iterator   Iterator;
                typedef bsl::pair<Iterator, bool> InsertReturn;

                InsertReturn iret = oim.insert(bsl::make_pair(oiPtr->d_name,
                                                              *oiPtr));
                ASSERT(true == iret.second);
            }

            // Define test cases

            enum { k_MAX_ARGC           = 10
                 , k_MAX_UNIQUE_OPTIONS =  5 };

            typedef struct {
                const char *d_optionName_p;
                int         d_numPositions;
                int         d_positions[k_MAX_ARGC];
            } NamePositions;

            static struct {
                int            d_line;
                const char    *d_commandLine_p;
                int            d_numExpects;
                NamePositions  d_expects[k_MAX_UNIQUE_OPTIONS];
            } DATA[] = {
                { // one flag
                  L_, "-a",
                            1, {
                                 { "aName", 1,  { 1 } }
                               }
                }
              , { // two flags
                  L_, "-a -b",
                            2, {
                                 { "aName", 1,  { 1 } }
                               , { "bName", 1,  { 2 } }
                               }
                }
              , { // two flags grouped
                  L_, "-ab",
                            2, {
                                 { "aName", 1,  { 1 } }
                               , { "bName", 1,  { 1 } }
                               }
                }
              , { // option with value
                  L_, "-c 1",
                            1, {
                                 { "cName", 1,  { 1 } }
                               }
                }
              , { // option with value using "assign" syntax
                  L_, "-c=1",
                            1, {
                                 { "cName", 1,  { 1 } }
                               }
                }
              , { // option with value via long tag
                  L_, "--cLong 1",
                            1, {
                                 { "cName", 1,  { 1 } }
                               }
                }
              , { // option with value and flags
                  L_, "-b --cLong 1 -a",
                            3, {
                                 { "aName", 1,  { 4 } }
                               , { "cName", 1,  { 2 } }
                               , { "bName", 1,  { 1 } }
                               }
                }
              , { // array of options having value
                  L_, "-d 1 -d 2",
                            1, {
                                 { "dName", 2,  { 1, 3 } }
                               }
                }
              , { // array of options having value using "assign" syntax
                  L_, "-d=1 -d=2",
                            1, {
                                 { "dName", 2,  { 1, 2 } }
                               }
                }
              , { // array of options having value, and intermixed flag
                  L_, "-d 1 -a  -d 2",
                            2, {
                                 { "aName", 1,  { 3 }    }
                               , { "dName", 2,  { 1, 4 } }
                               }
                }
              , { // non-option (string)
                  L_, "hello",
                            1, {
                                 { "eName", 1,  { 1 } }
                               }
                }
              , { // non-option (string) bracketed by flags
                  L_, "-a hello -b",
                            3, {
                                 { "aName", 1,  { 1 } }
                               , { "bName", 1,  { 3 } }
                               , { "eName", 1,  { 2 } }
                               }
                }
              , { // non-option (string) with non-option toggle
                  L_, "-a -b -- hello",
                            3, {
                                 { "aName", 1,  { 1 } }
                               , { "bName", 1,  { 2 } }
                               , { "eName", 1,  { 4 } }
                               }
                }
              , { // non-option (string) and array non-option (string)
                  L_, "hello how are you today",
                            2, {
                                 { "eName", 1,  { 1 }          }
                               , { "fName", 4,  { 2, 3, 4, 5 } }
                               }
                }
              , { // non-option (string), array non-option, and flags
                  L_, "hello -a how are -b you today",
                            3, {
                                 { "aName", 1,  { 2 }          }
                               , { "bName", 1,  { 5 }          }
                               , { "eName", 1,  { 1 }          }
                               , { "fName", 4,  { 3, 4, 6, 7 } }
                               }
                }
              , { // non-option (string), array non-option, option, and toggle
                  L_, "hello -d 1 -c 2 -d 3 -- how are you today",
                            3, {
                                 { "cName", 1,  { 4 }              }
                               , { "dName", 2,  { 2, 6 }           }
                               , { "eName", 1,  { 1 }              }
                               , { "fName", 4,  { 9, 10, 11, 12  } }
                               }
                }
            };

            enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

            // Run tests.

            for (int ti = 0; ti < k_NUM_DATA; ++ti) {
                const int                  LINE     = DATA[ti].d_line;
                const char          *const CMD      = DATA[ti].d_commandLine_p;
                const int                  NUM_EXPS = DATA[ti].d_numExpects;
                const NamePositions *const EXPS     = DATA[ti].d_expects;

                if (veryVerbose) { T_ P_(LINE) P(CMD) }

                Obj mX(TABLE); const Obj& X = mX;

                ASSERT( X.isValid());
                ASSERT(!X.isParsed());

                char cmdLine[1024];
                bsl::strcpy(cmdLine, "programName");
                bsl::strcat(cmdLine, " ");
                bsl::strcat(cmdLine, CMD);

                int         argc;
                const char *argv[u::MAX_ARGS];
                int         rc  = u::parseCommandLine(cmdLine, argc, argv);
                ASSERT(0 == rc);

                if (veryVeryVerbose) {
                    for (int i = 0; i < argc; ++i) {
                        T_ P_(i) P(argv[i])
                    }
                }

                int parseRet = mX.parse(argc, argv);
                ASSERT(0 == parseRet);
                ASSERT( X.isValid());
                ASSERT( X.isParsed());

                for (int j = 0; j < NUM_EXPS; ++j) {
                    const NamePositions *EXP = &EXPS[j];

                    if (veryVeryVerbose) {
                        T_ T_ P_(j)
                              P_(EXP->d_optionName_p)
                              P( EXP->d_numPositions)

                        for (int k = 0; k < EXP->d_numPositions; ++k) {
                            T_ T_ T_ P_(k)
                                      P(EXP->d_positions[k])
                        }
                    }

                    typedef OptionInfoMap::const_iterator ConstIterator;

                    bsl::string optionName(EXP->d_optionName_p);

                    ConstIterator itr = Oim.find(optionName);
                    ASSERT(Oim.cend() != itr);

                    ElemType type = itr->second.d_typeInfo.type();

                    if (veryVeryVerbose) { P(type) }

                    if (Ot::isArrayType(type)) {
                        const bsl::vector<int> posV =  X.positions(optionName);
                                                                      // ACTION

                        ASSERTV(EXP->d_numPositions,
                                posV.size(),
                                static_cast<bsl::size_t>(EXP->d_numPositions)
                             == posV.size());
                        for (int k = 0; k < EXP->d_numPositions; ++k) {
                            ASSERTV(k,
                                    EXP->d_positions[k],
                                    posV[k],
                                    EXP->d_positions[k] == posV[k]);
                        }
                    } else {
                        ASSERT(1 ==  EXP->d_numPositions);

                        const int              pos  =  X.position (optionName);
                                                                      // ACTION
                        ASSERT(EXP->d_positions[0] == pos);

                        const bsl::vector<int> posV =  X.positions(optionName);
                                                                      // ACTION
                        ASSERT(1                   == posV.size());
                        ASSERT(EXP->d_positions[0] == posV[0]);
                    }
                }
            }
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING ADDITIONAL CONSTRUCTORS
        //
        // Concerns:
        // 1. Each of the additional constructors creates objects equal to
        //    those created by the "Primary Manipulator" constructor (see TC
        //    3).
        //
        // 2. Each of the additional constructors allows the explicit
        //    specification of an object allocator, no specification, or
        //    0-specification.
        //
        // 3. Each of the additional constructors are exception-safe.
        //
        // Plan:
        // 1. Use the "footprint" idiom to exercise each of the additional five
        //    constructors by each of their three allowed invocations: no
        //    specified allocator, a 0-specified allocator, and an explicitly
        //    specified allocator.
        //
        //   1. Where necessary to avoid ambiguity or inadvertent invocation of
        //      and unintended constructor, supply the 0-specified allocator as
        //      a allocator pointer, not as an `int` value.  (C-2).
        //
        //   2. Use the `allocator` accessor to confirm that the object has the
        //      intended allocator.  (C-2)
        //
        // 2. Use the `BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*` macros to confirm
        //    that memory allocation is exception safe.  (C-3)
        //
        //    - Note that the last time through this macro loop corresponds to
        //      the "normal" (no exception) test case.
        //
        // 3. Compare each created object with a reference object created using
        //    the Primary Manipulator constructor.  For a meaningful
        //    comparison, invoke the `parse` method of each created test object
        //    with the same input that is passed to the reference object.
        //    (C-1)
        //
        // Testing:
        //   CommandLine(const Oi (&table)[LEN], ostream& stream, *bA = 0);
        //   CommandLine(      Oi (&table)[LEN], ostream& stream, *bA = 0);
        //   CommandLine(const Oi (&table)[LEN], *bA = 0);
        //   CommandLine(      Oi (&table)[LEN], *bA = 0);
        //   CommandLine(const Oi *table, int len, ostream& stream, *bA = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING ADDITIONAL CONSTRUCTORS" << endl
                                  << "===============================" << endl;

        bsl::vector<bsl::string>  files;

        const OptionInfo OI = { "",                      // non-option
                                "fileList",              // name
                                "files to be sorted",    // description
                                balcl::TypeInfo(&files), // link
                                balcl::OccurrenceInfo(), // occurrence
                                ""                       // env var name
                              };

              OptionInfo table[] = { OI };
        const OptionInfo TABLE[] = { OI };

        const int NUM_TABLE = static_cast<int>(sizeof  TABLE
                                             / sizeof *TABLE);
        ASSERT(1 == NUM_TABLE);

        const OptionInfo * const OIP = TABLE;

        const char * const ARGV[] = { "progname", "filename" };
        const int          ARGC   = sizeof ARGV / sizeof *ARGV;

        // Create reference object using CTOR proven in TC 3

        Obj  mZ(OIP, NUM_TABLE); const Obj& Z = mZ;

        bsl::ostringstream ossParse;
        int                retParseZ = mZ.parse(ARGC, ARGV, ossParse);
        ASSERT(0 == retParseZ);
        ASSERT(ossParse.str().empty());

        for (char cfg = 'a'; cfg <= 'o'; ++cfg) {
            const char CONFIG = cfg;

            if (veryVerbose) {
                T_ T_ P(CONFIG);
            }

            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
            bslma::TestAllocator da("default",   veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            bslma::TestAllocator *objAllocatorPtr = 'a' == CONFIG ? &da :
                                                    'b' == CONFIG ? &da :
                                                    'c' == CONFIG ? &sa : // SA

                                                    'd' == CONFIG ? &da :
                                                    'e' == CONFIG ? &da :
                                                    'f' == CONFIG ? &sa : // SA

                                                    'g' == CONFIG ? &da :
                                                    'h' == CONFIG ? &da :
                                                    'i' == CONFIG ? &sa : // SA

                                                    'j' == CONFIG ? &da :
                                                    'k' == CONFIG ? &da :
                                                    'l' == CONFIG ? &sa : // SA

                                                    'm' == CONFIG ? &da :
                                                    'n' == CONFIG ? &da :
                                                    'o' == CONFIG ? &sa : // SA

                                                    0;    // error
            ASSERTV(CONFIG, objAllocatorPtr);

            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = 'c' == CONFIG ? da :
                                        'f' == CONFIG ? da :
                                        'i' == CONFIG ? da :
                                        'l' == CONFIG ? da :
                                        'o' == CONFIG ? da :
                                                        sa ;  // other

            bslma::TestAllocatorMonitor  oam(&oa);
            bslma::TestAllocatorMonitor noam(&noa);

            int                 completeExceptionCount = 0;
            Obj                *objPtr                 = 0;
            bsl::ostringstream  ossCtor;

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {

                bslma::Allocator *baNull = 0;  // for disambiguation

                // ACTION
                objPtr = 'a' == CONFIG ? new (fa) Obj(TABLE)                 :
                         'b' == CONFIG ? new (fa) Obj(TABLE, baNull)         :
                         'c' == CONFIG ? new (fa) Obj(TABLE, &sa)            :

                         'd' == CONFIG ? new (fa) Obj(TABLE, ossCtor)        :
                         'e' == CONFIG ? new (fa) Obj(TABLE, ossCtor, 0)     :
                         'f' == CONFIG ? new (fa) Obj(TABLE, ossCtor, &sa)   :

                         'g' == CONFIG ? new (fa) Obj(table)                 :
                         'h' == CONFIG ? new (fa) Obj(table, baNull)         :
                         'i' == CONFIG ? new (fa) Obj(table, &sa)            :

                         'j' == CONFIG ? new (fa) Obj(table, ossCtor)        :
                         'k' == CONFIG ? new (fa) Obj(table, ossCtor, 0)     :
                         'l' == CONFIG ? new (fa) Obj(table, ossCtor, &sa)   :

                         'm'  == CONFIG ? new (fa) Obj(OIP, 1, ossCtor)      :
                         'n'  == CONFIG ? new (fa) Obj(OIP, 1, ossCtor, 0)   :
                         'o'  == CONFIG ? new (fa) Obj(OIP, 1, ossCtor, &sa) :

                         0;    // error

                ++completeExceptionCount;
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERTV(CONFIG, 1 == completeExceptionCount);
            ASSERTV(CONFIG, objPtr);

            Obj& mX = *objPtr;  const Obj& X = mX;

            ASSERTV(CONFIG, &oa == X.allocator());
            ASSERTV(CONFIG,  X.isValid());
            ASSERTV(CONFIG, !X.isParsed());
            ASSERTV(CONFIG, ossCtor.str().empty());  // unused or unwritten

            bsl::ostringstream ossParse;
            int                retParseX = mX.parse(ARGC, ARGV, ossParse);

            ASSERTV(CONFIG, 0 == retParseX);
            ASSERTV(CONFIG, ossParse.str().empty());
            ASSERTV(CONFIG,  X.isValid());
            ASSERTV(CONFIG,  X.isParsed());

            ASSERT(Z == X)

            fa.deleteObject(objPtr);  // Clean up

            ASSERTV(CONFIG,  oam.isInUseSame());
            ASSERTV(CONFIG, noam.isInUseSame());
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY ASSIGNMENT
        //
        // Concerns:
        // 1. The two operators have the expected signatures.
        //
        // 2. One object can be assigned to another irrespective of the (valid)
        //    state of each of those objects.
        //
        //   1. Objects in parsed and unparsed states can be assigned to each
        //      other.
        //
        // 3. Alias-safety: An object an be assigned to itself.
        //
        //   1. QoI: Self-assignment does not allocate.
        //
        // 4. The allocator of the assigned-to object (`lhs`) is preserved.
        //
        // 5. The assignment operation returns a reference to the `lhs` object.
        //
        // 6. The operation does not change the `rhs`.
        //
        // 7. QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        // 1. Use the "pointer-to-method" idiom to have the compiler check the
        //    signature.  (C-1)
        //
        // 2. For a representative set of objects assign each object with
        //    itself and to every other object in the set.
        //    The representative set used is
        //    obtained user the `u::generateTestData` helper function.
        //    Every 5th entry is tested to keep the cross product to a
        //    manageable size.  (C-2)
        //
        // 3. Use different (test) allocators for `lhs` and `rhs` objects.
        //    Confirm that the `lhs` object retains its original allocator.
        //    (C-3)
        //
        // 4. Use `bslma::TestAllocatorMonitor` objects to confirm that no
        //    memory is allocated.  (C-4)
        //
        // 5. Compare the address of the returned value (a reference, as shown
        //    in P-1) to the address of the `lhs`.  (C-5)
        //
        // 6. Create a duplicate of the `rhs` object that is not used in the
        //    assignment operation.  Confirm that the `rhs` compares equal to
        //    this spare object both before and after the assignment operation.
        //
        // 7. For each option configuration, create `lhs` and `rhs` objects
        //    in the parsed and unparsed state.  Test all four combinations
        //    of assignments between parsed and unparsed objects.
        //
        //   1. As all unparsed objects compare unequal, meaningful comparison
        //      changing them to a parsed state by invoking the `parse` method
        //      with the same (valid) arguments.  (C-2.1)
        //
        // 8. Do negative tests of defensive checks using `BSLS_ASSERTTEST_*`
        //    macros.  (C-7)
        //
        // Testing:
        //   CommandLine& operator=(const CommandLine& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "COPY ASSIGNMENT" << endl
                                  << "===============" << endl;

        if (veryVerbose) cout
                 << endl
                 << "Verify that the signatures and return types are standard."
                 << endl;
        {
            using namespace balcl;

            typedef Obj& (Obj::* operatorPtr)(const Obj&);

            operatorPtr operatorAssignment = &Obj::operator=;

            (void) operatorAssignment; // quash potential compiler warnings
        }

        if (verbose)
                 cout << "Assign each pair of values (u, v) in W X W." << endl;

        PREAMBLE_OPTIONS_ARGUMENTS

        const int DELTA = 5; // reduce huge cross product

// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
        for (int n1 = 0; n1 < 4; ++n1) {
        for (int i1 = 0; i1 < (n1 ? NUM_OPTIONS - n1 : 1); i1 += DELTA) {
            OptionInfo        specTable1[4];
            const OptionInfo *SPEC_TABLE1 = specTable1;

            for (int j = 0; j < n1; ++j) {
                if (veryVerbose) {
                    T_ T_ T_ P_(j) P(n1)
                }
                int index = i1 + j * NUM_OPTIONS;
                specTable1[j] = OPTIONS[index];
            }

            if (veryVerbose) {
                T_ T_ P_(i1) P(n1)
                for (int j = 0; j < n1; ++j) {
                    T_ T_ T_ P_(j) P(SPEC_TABLE1[j])
                }
            }

            bslma::TestAllocator saX("suppliedX", veryVeryVeryVerbose);

            Obj mX(SPEC_TABLE1, n1, &saX);  const Obj& X = mX;
            Obj mZ(SPEC_TABLE1, n1, &saX);  const Obj& Z = mZ;

            ASSERT(!X.isParsed());

            bslma::TestAllocatorMonitor samX(&saX);

            // Unparsed identity assignment

            Obj *mRux = &(mX = X);    // ACTION

            ASSERT(samX.isTotalSame());

            ASSERT(mRux == &mX);
            ASSERT(&saX  == X.allocator());
            ASSERT(&saX  == Z.allocator());

            // Put objects into parsed state for meaningful comparison.

            GENERATE_PARSE_INPUT(n1, i1)

            const bsl::vector<const char *>& ARG_PTRS1 = ARG_PTRS;

            bsl::ostringstream ossErr;
            int                parseRetux = mX.parse(
                                            static_cast<int>(ARG_PTRS1.size()),
                                            &ARG_PTRS1[0],
                                            ossErr);
            int                parseRetZ = mZ.parse(
                                            static_cast<int>(ARG_PTRS1.size()),
                                            &ARG_PTRS1[0],
                                            ossErr);
            ASSERT(0 == parseRetux);
            ASSERT(0 == parseRetZ);
            ASSERT(ossErr.str().empty());
            ASSERT(X.isParsed());
            ASSERT(Z.isParsed());

            ASSERT(Z == X);

            // Repeat with objects in parsed state.

            bslma::TestAllocatorMonitor samPX(&saX);

            Obj *mRpx = &(mX = X);    // ACTION

            ASSERT(samPX.isTotalSame());

            ASSERT(mRpx == &X);
            ASSERT(&saX == X.allocator());
            ASSERT(&saX == Z.allocator());

            ASSERT(Z == X);

            for (int n2 = 0; n2 < 4; ++n2) {
            for (int i2 = 0; i2 < (n2 ? NUM_OPTIONS - n2 : 1); i2 += DELTA) {

                // Prepare `rhs` objects, unparsed and parsed.

                bslma::TestAllocator saX("suppliedX", veryVeryVeryVerbose);
                bslma::TestAllocator saY("suppliedY", veryVeryVeryVerbose);

                const Obj UX(SPEC_TABLE1, n1, &saX);
                ASSERT(!UX.isParsed());

                Obj mPX(SPEC_TABLE1, n1, &saX); const Obj& PX = mPX;
                ASSERT(!PX.isParsed());

                bsl::ostringstream ossErrPX;
                int                parseRetpx = mPX.parse(
                                            static_cast<int>(ARG_PTRS1.size()),
                                            &ARG_PTRS1[0],
                                            ossErrPX);
                ASSERT(0 == parseRetpx)
                ASSERT(ossErrPX.str().empty());
                ASSERT( PX.isParsed());

                // Prepare `lhs` objects, unparsed and parsed.

                OptionInfo        specTable2[4];
                const OptionInfo *SPEC_TABLE2 = specTable2;

                for (int j = 0; j < n2; ++j) {
                    if (veryVerbose) {
                        T_ T_ T_ P_(j) P(n2)
                    }
                    int index = i2 + j * NUM_OPTIONS;
                    specTable2[j] = OPTIONS[index];
                }

                if (veryVerbose) {
                    T_ T_ P_(i2) P(n2)
                    for (int j = 0; j < n2; ++j) {
                        T_ T_ T_ P_(j) P(SPEC_TABLE2[j])
                    }
                }

                const Obj UY(SPEC_TABLE2, n2, &saY);
                ASSERT(!UY.isParsed());

                Obj mPY(SPEC_TABLE2, n2, &saY); const Obj& PY = mPY;
                ASSERT(!PY.isParsed());

                GENERATE_PARSE_INPUT(n2, i2)

                const bsl::vector<const char *>& ARG_PTRS2 = ARG_PTRS;

                bsl::ostringstream ossErrPY;
                int                parseRetpy = mPY.parse(
                                            static_cast<int>(ARG_PTRS2.size()),
                                            &ARG_PTRS2[0],
                                            ossErrPY);
                ASSERT(0 == parseRetpy)
                ASSERT(ossErrPY.str().empty());
                ASSERT( PY.isParsed());

                {   // parsed <= parsed
                    Obj mY(PY, &saY);  const Obj& Y = mY;
                    Obj mX(PX, &saX);  const Obj& X = mX;

                    ASSERT( Y.isParsed());
                    ASSERT( X.isParsed());

                    Obj *mR = &(mY = X);    // ACTION

                    ASSERT(mR   == &Y);
                    ASSERT(&saY ==  Y.allocator());
                    ASSERT(X    ==  Y);
                    ASSERT(Z    ==  X);
                }

                {   // unparsed <= parsed
                    Obj mY(UY, &saY);  const Obj& Y = mY;
                    Obj mX(PX, &saX);  const Obj& X = mX;

                    ASSERT(!Y.isParsed());
                    ASSERT( X.isParsed());

                    Obj *mR = &(mY = X);    // ACTION

                    ASSERT(mR   == &Y);
                    ASSERT(&saY ==  Y.allocator());

                    ASSERT( Y.isParsed());
                    ASSERT( X.isParsed());

                    ASSERT(X    ==  Y);
                    ASSERT(Z    ==  X);
                }

                {   // parsed <= unparsed
                    Obj mY(PY, &saY);  const Obj& Y = mY;
                    Obj mX(UX, &saX);  const Obj& X = mX;

                    ASSERT( Y.isParsed());
                    ASSERT(!X.isParsed());

                    Obj *mR = &(mY = X);    // ACTION

                    ASSERT(mR   == &Y);
                    ASSERT(&saY ==  Y.allocator());

                    ASSERT(!Y.isParsed());
                    ASSERT(!X.isParsed());

                    // Parse both for meaningful comparison.

                    bsl::ostringstream ossErr;
                    int                parseRetX = mX.parse(
                                            static_cast<int>(ARG_PTRS1.size()),
                                            &ARG_PTRS1[0],
                                            ossErr);
                    int                parseRetY = mY.parse(
                                            static_cast<int>(ARG_PTRS1.size()),
                                            &ARG_PTRS1[0],
                                            ossErr);
                    ASSERT(0 == parseRetX);
                    ASSERT(0 == parseRetY);
                    ASSERT(ossErr.str().empty());
                    ASSERT(X.isParsed());
                    ASSERT(Y.isParsed());

                    ASSERT(X    ==  Y);
                    ASSERT(X    ==  Z);
                }

                {   // unparsed <= unparsed
                    Obj mY(UY, &saY);  const Obj& Y = mY;
                    Obj mX(UX, &saX);  const Obj& X = mX;

                    ASSERT(!Y.isParsed());
                    ASSERT(!X.isParsed());

                    Obj *mR = &(mY = X);    // ACTION

                    ASSERT(mR   == &Y);
                    ASSERT(&saY ==  Y.allocator());

                    ASSERT(!Y.isParsed());
                    ASSERT(!X.isParsed());

                    // Parse both for meaningful comparison.

                    bsl::ostringstream ossErr;
                    int                parseRetX = mX.parse(
                                            static_cast<int>(ARG_PTRS1.size()),
                                            &ARG_PTRS1[0],
                                            ossErr);
                    int                parseRetY = mY.parse(
                                            static_cast<int>(ARG_PTRS1.size()),
                                            &ARG_PTRS1[0],
                                            ossErr);
                    ASSERT(0 == parseRetX);
                    ASSERT(0 == parseRetY);
                    ASSERT(ossErr.str().empty());
                    ASSERT(X.isParsed());
                    ASSERT(Y.isParsed());

                    ASSERT(X    ==  Y);
                    ASSERT(X    ==  Z);
                }
            }
            }
        }
        }
// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line

        if (veryVerbose) cout << "Negative Testing" << endl;
        {
            const OptionInfo OI =
                             { "f|flag"                   // tag
                             , "Flag"                     // name
                             , "description"              // description
                             , TypeInfo(Ot::k_BOOL)       // no linked variable
                             , OccurrenceInfo::e_OPTIONAL // occurrence
                             , ""                         // env var name
                             };

            const OptionInfo TABLE[] = { OI };

            const int NUM_TABLE = static_cast<int>(sizeof  TABLE
                                                 / sizeof *TABLE);
            ASSERT(1 == NUM_TABLE);

            const OptionInfo * const OIP = TABLE;

            // Create a valid object, `Zok`

            const char * const ARGVok[] = { "progname" };
            const int          ARGCok   = sizeof ARGVok / sizeof *ARGVok;

            Obj mZok(OIP, NUM_TABLE); const Obj& Zok = mZok;
            ASSERT( Zok.isValid());
            ASSERT(!Zok.isParsed());

            bsl::ostringstream ossParseOk;
            int                retParseZok = mZok.parse(ARGCok,
                                                        ARGVok,
                                                        ossParseOk);
            ASSERT(0 == retParseZok);
            ASSERT( ossParseOk.str().empty());
            ASSERT( Zok.isValid());
            ASSERT( Zok.isParsed());

            // Create an invalid object, `Zng`

            const char * const ARGVng[] = { "progname"
                                          , "--some-invalid-option"
                                          };
            const int          ARGCng   = sizeof ARGVng / sizeof *ARGVng;

            Obj mZng(OIP, NUM_TABLE); const Obj& Zng = mZng;
            ASSERT( Zng.isValid());
            ASSERT(!Zng.isParsed());

            bsl::ostringstream ossParseNg;
            int                retParseZng = mZng.parse(ARGCng,
                                                        ARGVng,
                                                        ossParseNg);
            ASSERT(0 != retParseZng);
            ASSERT(!ossParseNg.str().empty());
            ASSERT(!Zng.isValid());
            ASSERT(!Zng.isParsed());

            bsls::AssertTestHandlerGuard hGa;

            ASSERT_PASS(mZok = Zok);
            ASSERT_FAIL(mZok = Zng);
            ASSERT_FAIL(mZng = Zok);
            ASSERT_FAIL(mZng = Zng);
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        //
        // Concerns:
        // 1. Equality: The copy constructor creates a new object that compares
        //    equal to the original object, irrespective of the state of the
        //    original object:
        //
        //   1. `original.isParsed()`: the copy `isParsed()` and compares equal
        //      to the original.
        //
        //   2. `!original.isParsed()`: the copy is `!isParsed()` and if the
        //      copy and the original are parsed using the same arguments, the
        //      resulting objects compare equal.
        //
        // 2. Allocators:
        //   1. The allocator of the created object depends on its constructor
        //      argument (not the allocator of the original object).
        //
        //   2. If the allocator argument of the object is 0 or not specified,
        //      the new object uses the default allocator, otherwise, the
        //      specified allocator is used.
        //
        //   3. The constructor is exception safe.
        //
        // 3. The original object is unchanged when copied.
        //
        // 4. QoI: Asserted precondition violations are detected when enabled.
        //
        // Plans:
        // 1. Do table-driven testing using the tables described in {Input
        //    Tables} and the `u::generateTestData` helper function.  Construct
        //    two objects for each combination of input.  One is passed to the
        //    copy constructor (the "original" object) the other is unused.
        //    Confirm that the original remains the same as the unused object.
        //    (C-3)
        //
        // 2. Use `operator==` to confirm the equality of the new object
        //    when they are in the parsed state.  (C-1).
        //
        // 3. Repeat each test for the cases of an unspecified allocator, a
        //    0-allocator, and an explicitly supplied allocator.  (C-2.1)
        //
        //   1. Confirm the allocator of the new object using the `allocator`
        //      accessor.  (C-2.2)
        //
        //   2. Use `bslma::TestAllocatorMonitor` objects to confirm that
        //      memory allocation, when expected, occurs in the expected
        //      allocator, and no allocation occurs from the "other" allocator.
        //
        //   3. Use the `BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*` macros to
        //      confirm that memory allocation is exception safe.
        //
        //      - Note that the last time through this macro loop corresponds
        //        to the "normal" (no exception) test case.
        //
        // 4. Repeat each test for an original object in an unparsed state
        //    and in an unparsed state.  For the unparsed tests invoke the
        //    `parse` methods of original and copy with identical, valid
        //    arguments so a meaningful comparison can be done.  (Recall that
        //    comparisons in an unparsed state always fail.)
        //
        // 5. Do negative tests of defensive checks using `BSLS_ASSERTTEST_*`
        //    macros.  (C-4)
        //
        // Testing:
        //   CommandLine(const CommandLine& original, *bA = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;

        PREAMBLE_OPTIONS_ARGUMENTS

        const int DELTA = 5; // reduce huge cross product

// BDE_VERIFY pragma: -TP21    // Loops must contain very verbose action
// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
        for (int n = 0; n < 4; ++n) {
        for (int i = 0; i < (n ? NUM_OPTIONS - n : 1); i += DELTA) {
            OptionInfo        specTable[4];
            const OptionInfo *SPEC_TABLE = specTable;

            for (int j = 0; j < n; ++j) {
                int index = i + j * NUM_OPTIONS;
                specTable[j] = OPTIONS[index];
            }

            if (veryVerbose) {
                T_ T_ P_(i) P(n)
                for (int j = 0; j < n; ++j) {
                    T_ T_ T_ P_(j) P(SPEC_TABLE[j])
                }
            }

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mZZ(SPEC_TABLE, n, &scratch);  const Obj& ZZ = mZZ;

            GENERATE_PARSE_INPUT(n, i)

            bsl::ostringstream ossErr;
            int                parseRetZZ = mZZ.parse(
                                             static_cast<int>(ARG_PTRS.size()),
                                             &ARG_PTRS[0],
                                             ossErr);
            ASSERT(0 == parseRetZZ);
            ASSERT(ossErr.str().empty());
            ASSERT(ZZ.isParsed()); ASSERT(ZZ.isValid());

            for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
                const char CONFIG = cfg;

                if (veryVerbose) {
                    T_ T_ P(CONFIG);
                }

                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                bslma::TestAllocator *objAllocatorPtr = 'a' == CONFIG ? &da :
                                                        'b' == CONFIG ? &da :
                                                        'c' == CONFIG ? &sa :
                                                        0   ; // error
                ASSERTV(CONFIG, objAllocatorPtr)

                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = 'c' == CONFIG ? da : sa;

                bslma::TestAllocatorMonitor  oam(&oa);
                bslma::TestAllocatorMonitor noam(&noa);

                int  completeExceptionCount = 0;
                Obj *objPtr                 = 0;

                // Create an unparsed "original", `Z`.

                Obj mZ(SPEC_TABLE, n, &scratch);  const Obj& Z = mZ;
                ASSERT(!Z.isParsed()); ASSERT(Z.isValid());

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {

                    objPtr = 'a' == CONFIG ? new (fa) Obj(Z)      : // ACTION
                             'b' == CONFIG ? new (fa) Obj(Z, 0)   : // ACTION
                             'c' == CONFIG ? new (fa) Obj(Z, &sa) : // ACTION
                             0                                    ; // error

                    ++completeExceptionCount;
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(CONFIG, 1 == completeExceptionCount);
                ASSERTV(CONFIG, objPtr);

                Obj& mUX = *objPtr;  const Obj& UX = mUX;

                ASSERTV(CONFIG, &oa == UX.allocator());
                ASSERTV(CONFIG,  UX.isValid());
                ASSERTV(CONFIG, !UX.isParsed());

                // Parse the copy and the original (so they can be compared).

                bsl::ostringstream ossErr;
                int                parseRetZ  = mZ.parse(
                                             static_cast<int>(ARG_PTRS.size()),
                                             &ARG_PTRS[0],
                                             ossErr);
                int                parseRetUX = mUX.parse(
                                             static_cast<int>(ARG_PTRS.size()),
                                             &ARG_PTRS[0],
                                             ossErr);
                ASSERT(0 == parseRetZ);
                ASSERT(0 == parseRetUX);
                ASSERT(ossErr.str().empty());
                ASSERT( Z.isParsed()); ASSERT( Z.isValid());
                ASSERT(UX.isParsed()); ASSERT(UX.isValid());

                // Vet the object value.
                ASSERT(Z == UX);

                // Vet the original object (`Z`) state.
                ASSERT(Z             == ZZ);
                ASSERT(Z.allocator() == ZZ.allocator());

                fa.deleteObject(objPtr);  // Clean up

                ASSERTV(CONFIG,  oam.isInUseSame());
                ASSERTV(CONFIG, noam.isInUseSame());

                // Now, copy the (now parsed) original, `Z`.

                ASSERT( Z.isParsed());

                completeExceptionCount = 0;
                objPtr                 = 0;

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {

                    objPtr = 'a' == CONFIG ? new (fa) Obj(Z)      : // ACTION
                             'b' == CONFIG ? new (fa) Obj(Z, 0)   : // ACTION
                             'c' == CONFIG ? new (fa) Obj(Z, &sa) : // ACTION
                             0                                    ; // error

                    ++completeExceptionCount;
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(CONFIG, 1 == completeExceptionCount);
                ASSERTV(CONFIG, objPtr);

                Obj& mPX = *objPtr;  const Obj& PX = mPX;

                ASSERTV(CONFIG, &oa == PX.allocator());
                ASSERTV(CONFIG,  PX.isValid());
                ASSERTV(CONFIG,  PX.isParsed());

                // Vet the object value.
                ASSERT(Z == PX);

                // Vet the original object (`Z`) state again.
                ASSERT(Z             == ZZ);
                ASSERT(Z.allocator() == ZZ.allocator());

                fa.deleteObject(objPtr);  // Clean up again.

                ASSERTV(CONFIG,  oam.isInUseSame());
                ASSERTV(CONFIG, noam.isInUseSame());
            }
        }
        }
// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line

        if (veryVerbose) cout << "Negative Testing" << endl;
        {
            const OptionInfo OI =
                             { "f|flag"                   // tag
                             , "Flag"                     // name
                             , "description"              // description
                             , TypeInfo(Ot::k_BOOL)       // no linked variable
                             , OccurrenceInfo::e_OPTIONAL // occurrence
                             , ""                         // env var name
                             };

            const OptionInfo TABLE[] = { OI };

            const int NUM_TABLE = static_cast<int>(sizeof  TABLE
                                                 / sizeof *TABLE);
            ASSERT(1 == NUM_TABLE);

            const OptionInfo * const OIP = TABLE;

            // Create a valid object, `Zok`

            const char * const ARGVok[] = { "progname" };
            const int          ARGCok   = sizeof ARGVok / sizeof *ARGVok;

            Obj mZok(OIP, NUM_TABLE); const Obj& Zok = mZok;
            ASSERT( Zok.isValid());
            ASSERT(!Zok.isParsed());

            bsl::ostringstream ossParseOk;
            int                retParseZok = mZok.parse(ARGCok,
                                                        ARGVok,
                                                        ossParseOk);
            ASSERT(0 == retParseZok);
            ASSERT( ossParseOk.str().empty());
            ASSERT( Zok.isValid());
            ASSERT( Zok.isParsed());

            // Create an invalid object, `Zng`

            const char * const ARGVng[] = { "progname"
                                          , "--some-invalid-option"
                                          };
            const int          ARGCng   = sizeof ARGVng / sizeof *ARGVng;

            Obj mZng(OIP, NUM_TABLE); const Obj& Zng = mZng;
            ASSERT( Zng.isValid());
            ASSERT(!Zng.isParsed());

            bsl::ostringstream ossParseNg;
            int                retParseZng = mZng.parse(ARGCng,
                                                        ARGVng,
                                                        ossParseNg);
            ASSERT(0 != retParseZng);
            ASSERT(!ossParseNg.str().empty());
            ASSERT(!Zng.isValid());
            ASSERT(!Zng.isParsed());

            bsls::AssertTestHandlerGuard hGa;

            ASSERT_PASS((Obj(Zok)));
            ASSERT_FAIL((Obj(Zng)));
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        //   Ensure that `==` and `!=` are the operational definition of value.
        //
        // Concerns:
        // 1. Salient Members:
        //
        //   1. Two objects, `X` and `Y`, compare equal if and only if both:
        //      - both accept the same options
        //      - are both in the parsed state
        //      - both hold the same option values
        //
        //   2. The object's allocator is not salient.
        //
        // 2. Mathematical Properties:
        //
        //   1. The operators provide the property of identity:
        //      - `true  == (X == X)`
        //      - `false == (X != X)`
        //
        //   2. The operators provide the property of commutativity:
        //      - `X == Y` if and only if `Y == X`
        //      - `X != Y` if and only if `Y != X`
        //
        //   3. Each of these two operators is the inverse of the other:
        //      - `X != Y` if and only if `!(X == Y)`
        //
        // 3. Non-modifiable objects can be compared (i.e., `const` objects and
        //    `const` references).
        //
        // 4. The two operators have standard signatures and return types.
        //
        // 5. No memory allocation occurs as a result of comparison (e.g., the
        //    arguments are not passed by value).
        //
        // 6. An `balcl::CommandLine` object in a unparsed state does not
        //    compare equal to any other, not even itself.
        //
        // 7. Comparisons between option handles match those of the (parsed)
        //    objects from which the handles are obtained.  The results should
        //    be the same for handles obtained using the `options` and the
        //    `specifiedOptions` methods.
        //
        // Plan:
        // 1. Use the respective addresses of `operator==` and `operator!=` to
        //    initialize function pointers having the appropriate signatures
        //    and return types for the two homogeneous, free
        //    equality-comparison operators defined in this component.
        //    (C-3..4)
        //
        // 2. Use the `u::generateTestData` helper to generate a large table of
        //    unique option configurations.  Use the cross product of those
        //    configurations to create `balcl::CommandLine` objects.
        //
        //    - Note that a complete cross product was impractically large
        //      (about 250,000) so every fifth entry was chosen bringing that
        //      count to about 10,000.
        //
        // 3. For each test of equality, create a parallel test that checks
        //    inequality (the inverse operator), and (when the two arguments
        //    are different) also create a test case where the two arguments
        //    are switched (showing commutativity).  (C-2)
        //
        // 4. Repeat each test of two distinct objects.  In one have the object
        //    share an object allocator, in the other have them use different
        //    allocators.  Confirm that the results are the same.
        //
        // 5. Compare each (unique) test object to itself can confirm that it
        //    fails in the unparsed state and passes in the parsed state.
        //
        // 7. Repeat each test of two distinct objects so that one, the other,
        //    and both are in unparsed states.  Confirm that all such tests are
        //    failed.
        //
        // 8. Confirm that tests of two distinct objects that are both in a
        //    parsed state compare equal only if they are created from the same
        //    table entries.
        //
        // 7. Shadow each comparison between parsed objects with analogous
        //    comparisons between option handles obtained from those objects.
        //    Each option handle comparison is done for handles obtained using
        //    both the `options` and `specifiedOptions` methods.  (C-7)
        //
        //    - This a "bootstrap" test of the `CommandLineOptionsHandle` type
        //      (done here to avoid duplication of the equality testing
        //      framework).  Tests of this type are completed in TC 9.
        //
        //    - Equality comparison of `CommandLineOptionsHandle` objects have
        //      an internal precondition (and defensive check) that they refer
        //      to `CommandLine` objects in a parsed state.  Negative testing
        //      of this defensive check is impractical because creation of such
        //      handles is guarded by the defensive checks the `CommandLine`
        //      methods `options` and `specifiedOptions`.  Those are tested in
        //      TC 9.
        //
        // Testing:
        //   bool operator==(const CommandLine& lhs, const CommandLine& rhs);
        //   bool operator!=(const CommandLine& lhs, const CommandLine& rhs);
        //   bool operator==(const CommandLineOptionsHandle& lhs, rhs);
        //   bool operator!=(const CommandLineOptionsHandle& lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EQUALITY-COMPARISON OPERATORS" << endl
                          << "=============================" << endl;

        if (verbose) cout <<
               "Verify that signatures and return values are standard" << endl;
        {
            using namespace balcl;
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            operatorPtr operatorEq = operator==;
            operatorPtr operatorNe = operator!=;

            (void)operatorEq;  // Quash potential compiler warnings.
            (void)operatorNe;
        }

        if (verbose)
                cout << "Compare each pair of values (u, v) in W X W." << endl;

        PREAMBLE_OPTIONS_ARGUMENTS

        const int DELTA = 5; // reduce huge cross product

// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
        for (int n1 = 0; n1 < 4; ++n1) {
        for (int i1 = 0; i1 < (n1 ? NUM_OPTIONS - n1 : 1); i1 += DELTA) {
            OptionInfo        specTable1[4];
            const OptionInfo *SPEC_TABLE1 = specTable1;

            for (int j = 0; j < n1; ++j) {
                int index = i1 + j * NUM_OPTIONS;
                specTable1[j] = OPTIONS[index];
            }

            if (veryVerbose) {
                T_ T_ P_(i1) P(n1)
                for (int j = 0; j < n1; ++j) {
                    T_ T_ T_ P_(j) P(SPEC_TABLE1[j])
                }
            }
            bslma::TestAllocator sa1("supplied1", veryVeryVeryVerbose);
            bslma::TestAllocator sa2("supplied2", veryVeryVeryVerbose);

            bslma::TestAllocator *saX = &sa1;

            Obj mX(SPEC_TABLE1, n1, saX);  const Obj& X = mX;

            ASSERT(!X.isParsed());

            bslma::TestAllocatorMonitor samUX(saX);

            // Unparsed identity comparison

            ASSERT(!(X == X));   // ACTION
            ASSERT( (X != X));   // ACTION

            ASSERT(samUX.isTotalSame());

            GENERATE_PARSE_INPUT(n1, i1)

            const bsl::vector<const char *>& ARG_PTRS1 = ARG_PTRS;

            bsl::ostringstream ossErr;
            int                parseRet = mX.parse(
                                            static_cast<int>(ARG_PTRS1.size()),
                                            &ARG_PTRS1[0],
                                            ossErr);
            ASSERT(0 == parseRet);
            ASSERT(ossErr.str().empty());
            ASSERT(X.isParsed());

            // Parsed identity comparison  -- reverse of unparsed results

            bslma::TestAllocatorMonitor samPX(saX);

            ASSERT( (X == X));   // ACTION
            ASSERT(!(X != X));   // ACTION

            ASSERT(samPX.isTotalSame());

            // Identity tests for handles obtained from `X` (now parsed).

            const OptionsHandle OH = X.         options();
            const OptionsHandle SH = X.specifiedOptions();

            bslma::TestAllocatorMonitor samHX(saX);

            ASSERT( (OH == OH));  // ACTION
            ASSERT(!(OH != OH));  // ACTION

            ASSERT( (SH == SH));  // ACTION
            ASSERT(!(SH != SH));  // ACTION

            ASSERT(samHX.isTotalSame());

            for (int n2 = 0; n2 < 4; ++n2) {
            for (int i2 = 0; i2 < (n2 ? NUM_OPTIONS - n2 : 1); i2 += DELTA) {

                const Obj UX(SPEC_TABLE1, n1, saX);
                ASSERT(!UX.isParsed());

                OptionInfo        specTable2[4];
                const OptionInfo *SPEC_TABLE2 = specTable2;

                for (int j = 0; j < n2; ++j) {
                    int index = i2 + j * NUM_OPTIONS;
                    specTable2[j] = OPTIONS[index];
                }

                if (veryVerbose) {
                    T_ T_ P_(i2) P(n2)
                    for (int j = 0; j < n2; ++j) {
                        T_ T_ T_ P_(j) P(SPEC_TABLE2[j])
                    }
                }

                for (int k = 0; k < 2; ++k) {

                    if (veryVerbose) { T_ T_ T_  P(k) }

                    bslma::TestAllocator *saY = k % 2
                                              ? &sa1  //     same as `X`
                                              : &sa2; // not same as `X`

                    Obj mY(SPEC_TABLE2, n2, saY);  const Obj& Y = mY;

                    ASSERT(!Y.isParsed());

                    bslma::TestAllocatorMonitor samY(saY);

                    // Unparsed Y and Parsed X.  All fail.

                    ASSERT(!( X ==  Y));  // ACTION
                    ASSERT( ( X !=  Y));  // ACTION
                    ASSERT(!( Y ==  X));  // ACTION
                    ASSERT( ( Y !=  X));  // ACTION

                    // Y parsed  and UX (unparsed).  All fail.

                    ASSERT(!(UX ==  Y));  // ACTION
                    ASSERT( (UX !=  Y));  // ACTION
                    ASSERT(!( Y == UX));  // ACTION
                    ASSERT( ( Y != UX));  // ACTION

                    ASSERT(samY.isTotalSame());
                }

                GENERATE_PARSE_INPUT(n2, i2)

                const bsl::vector<const char *>& ARG_PTRS2 = ARG_PTRS;

                for (int k = 0; k < 2; ++k) {

                    if (veryVerbose) { T_ T_ T_  P(k) }

                    bslma::TestAllocator *saY = k % 2
                                              ? &sa1  //     same as `X`
                                              : &sa2; // not same as `X`

                    Obj mY(SPEC_TABLE2, n2, saY);  const Obj& Y = mY;

                    ASSERT(!Y.isParsed());

                    bsl::ostringstream ossErr;
                    int                parseRet = mY.parse(
                                            static_cast<int>(ARG_PTRS2.size()),
                                            &ARG_PTRS2[0],
                                            ossErr);
                    ASSERT(0 == parseRet);
                    ASSERT(ossErr.str().empty());
                    ASSERT(Y.isParsed());

                    // Equality of parsed objects depend on contents.

                    const bool isSame  = n1 == n2
                                      && i1 == i2;

                    bslma::TestAllocatorMonitor samY(saY);

                    ASSERT( isSame == (X == Y));  // ACTION
                    ASSERT(!isSame == (X != Y));  // ACTION
                    ASSERT( isSame == (Y == X));  // ACTION
                    ASSERT(!isSame == (Y != X));  // ACTION

                    ASSERT(samY.isTotalSame());

                    // Tests for handles obtained from `X` and `Y`.

                    const OptionsHandle OHX = X.         options();
                    const OptionsHandle SHX = X.specifiedOptions();

                    const OptionsHandle OHY = Y.         options();
                    const OptionsHandle SHY = Y.specifiedOptions();

                    bslma::TestAllocatorMonitor samHX(saX);
                    bslma::TestAllocatorMonitor samHY(saY);

                    ASSERT( isSame == (OHX == OHY));  // ACTION
                    ASSERT(!isSame == (OHX != OHY));  // ACTION
                    ASSERT( isSame == (OHY == OHX));  // ACTION
                    ASSERT(!isSame == (OHY != OHX));  // ACTION

                    ASSERT( isSame == (SHX == SHY));  // ACTION
                    ASSERT(!isSame == (SHX != SHY));  // ACTION
                    ASSERT( isSame == (SHY == SHX));  // ACTION
                    ASSERT(!isSame == (SHY != SHX));  // ACTION

                    ASSERT(samHX.isTotalSame());
                    ASSERT(samHY.isTotalSame());
                }
            }
            }
        }
        }
// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR
        //
        // Concerns:
        // 1. The `print` method writes to the specified `ostream` in the
        //    expected format.
        //
        // 2. `operator<<` produces the same results as `print` when level and
        //    spaces-per-level arguments have their default value.
        //
        // 3. The return values of `print` and `operator<<` reference the
        //    stream argument.
        //
        // 4. The signature and return types of `print` and `operator<<` are
        //    standard.
        //
        // 5. The `level` and `spacesPerLevel` parameters have the correct
        //    default values.
        //
        // 6. The methods show the expected behaviors for objects in both
        //    parsed and unparsed states.
        //
        // Plan:
        // 1. Use the "function address" idiom to confirm the signatures.
        //
        // 2. Confirm that `bdlb::HasPrintMethod<TypeInfo>::value` is `true`
        //    using a compile-time assertion at file scope.
        //
        // 3. Run tests for objects in both parsed and unparsed states.
        //
        // 4. Use `u::normalizeIndentation` helper function to confirm that
        //    output using non-default values of `level` and `spacesPerLevel`
        //    have those indentations and are otherwise the same.
        //
        // Testing:
        //   ostream& print(ostream& stream, int level = 0, int spl = 4) const;
        //   ostream& operator<<(ostream& stream, const CommandLine& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRINT AND OUTPUT OPERATOR" << endl
                          << "=========================" << endl;

        if (verbose) cout
                 << "Verify that the signatures and return types are standard."
                 << endl;

        {
            using namespace balcl;
            using bsl::ostream;

            typedef ostream& (Obj::*funcPtr)(ostream&, int, int) const;
            typedef ostream& (*operatorPtr)(ostream&, const Obj&);

            funcPtr     printMember = &Obj::print;
            operatorPtr operatorOp  = operator<<;

            (void)printMember;  // quash potential compiler warnings
            (void)operatorOp;
        }

        if (verbose) cout << "Check output." << endl;

        PREAMBLE_OPTIONS_ARGUMENTS

        const int MAX_OPTIONS = static_cast<int>(OPTIONS.size());

        // BDE_VERIFY pragma: -IND01   // Possibly mis-indented line

        for (int n = 0; n < 4; ++n) {
        for (int i = 0; i < (n ? NUM_OPTIONS - n : 1); ++i) {
            OptionInfo        specTable[4];
            const OptionInfo *SPEC_TABLE = specTable;

            for (int j = 0; j < n; ++j) {
                int index = i + j * NUM_OPTIONS;

                ASSERTV(index, MAX_OPTIONS, 0     <= index);
                ASSERTV(index, MAX_OPTIONS, index <  MAX_OPTIONS);

                specTable[j] = OPTIONS[index];
            }

            if (veryVerbose) {
                T_ T_ P_(i) P(n)
                for (int j = 0; j < n; ++j) {
                    T_ T_ T_ P_(j) P(SPEC_TABLE[j])
                }
            }

            Obj mX(SPEC_TABLE, n);  const Obj& X = mX;

            ASSERT(!X.isParsed());

            { // Unparsed object

                bsl::ostringstream ossMethod;
                bsl::ostringstream ossOperator;

                ASSERT(&ossMethod   == &X.print(ossMethod));  // ACTION
                ASSERT(&ossOperator == &(ossOperator << X));  // ACTION

                if (veryVeryVerbose) {
                    P(ossMethod.str())
                    P(ossOperator.str())
                }

                ASSERT("UNPARSED\n"      == ossMethod.str());
                ASSERT(ossOperator.str() == ossMethod.str());

                ASSERT(0 * 4 == bsl::strspn(ossMethod  .str().c_str(), " "));
                ASSERT(0 * 4 == bsl::strspn(ossOperator.str().c_str(), " "));

                bsl::string stringMethod  (ossMethod  .str());
                bsl::string stringOperator(ossOperator.str());

                bsl::ostringstream ossIndent1;
                bsl::ostringstream ossIndent2;

                X.print(ossIndent1, 2);    // ACTION
                X.print(ossIndent2, 1, 5); // ACTION

                bsl::string stringIndent1(ossIndent1.str());
                bsl::string stringIndent2(ossIndent2.str());

                ASSERT(2 * 4 == bsl::strspn(stringIndent1.c_str(), " "));
                ASSERT(1 * 5 == bsl::strspn(stringIndent2.c_str(), " "));

                bsl::string stringIndentNormalized1;
                bsl::string stringIndentNormalized2;

                u::normalizeIndentation(&stringIndentNormalized1,
                                        stringIndent1,
                                        2,
                                        4);
                u::normalizeIndentation(&stringIndentNormalized2,
                                        stringIndent2,
                                        1,
                                        5);

                ASSERTV(stringMethod,   stringIndentNormalized1,
                        stringMethod == stringIndentNormalized1);
                ASSERTV(stringMethod,   stringIndentNormalized2,
                        stringMethod == stringIndentNormalized2);
            }

            // Parsed object

            GENERATE_PARSE_INPUT(n, i)

            bsl::ostringstream ossErr;
            int                parseRet = mX.parse(
                                             static_cast<int>(ARG_PTRS.size()),
                                             &ARG_PTRS[0],
                                             ossErr);
            ASSERT(0 == parseRet);
            ASSERT(ossErr.str().empty());

            ASSERT(X.isParsed());

            { // Parsed object

                bsl::ostringstream ossMethod;
                bsl::ostringstream ossOperator;

                ASSERT(&ossMethod   == &X.print(ossMethod));  // ACTION
                ASSERT(&ossOperator == &(ossOperator << X));  // ACTION

                if (veryVeryVerbose) {
                    P(ossMethod.str())
                    P(ossOperator.str())
                }

                ASSERT(ossOperator.str() == ossMethod.str());

                bsl::string stringMethod  (ossMethod  .str());
                bsl::string stringOperator(ossOperator.str());

                ASSERT(0 * 4 == bsl::strspn(stringMethod  .c_str(), " "));
                ASSERT(0 * 4 == bsl::strspn(stringOperator.c_str(), " "));

                ASSERTV(stringMethod,   stringOperator,
                        stringMethod == stringOperator);

                bsl::ostringstream ossIndent1;
                bsl::ostringstream ossIndent2;

                X.print(ossIndent1, 2);    // ACTION
                X.print(ossIndent2, 1, 5); // ACTION

                bsl::string stringIndent1(ossIndent1.str());
                bsl::string stringIndent2(ossIndent2.str());

                ASSERT(2 * 4 == bsl::strspn(stringIndent1.c_str(), " "));
                ASSERT(1 * 5 == bsl::strspn(stringIndent2.c_str(), " "));

                bsl::string stringIndentNormalized1;
                bsl::string stringIndentNormalized2;

                u::normalizeIndentation(&stringIndentNormalized1,
                                        stringIndent1,
                                        2,
                                        4);
                u::normalizeIndentation(&stringIndentNormalized2,
                                        stringIndent2,
                                        1,
                                        5);

                ASSERTV(stringMethod,   stringIndentNormalized1,
                        stringMethod == stringIndentNormalized1);
                ASSERTV(stringMethod,   stringIndentNormalized2,
                        stringMethod == stringIndentNormalized2);
            }
        }
        }
// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS, BASIC ASSESSORS, AND DTOR
        //
        // Concerns:
        // 1. Constructor
        //
        //   1. An object allocator can be specified, a 0-specified allocator
        //      specified, or no allocator can be specified.
        //
        //   2. The create object behaves as expected irrespective of how the
        //      allocator is specified.
        //
        //   3. Initially, the object state is
        //      `false == isValid() == isParsed()`.
        //
        //  4. Constructor arguments can be `const`-qualified (except for the
        //     allocator argument).
        //
        // 2. Allocators
        //
        //   1. The expected allocators are used.
        //
        //   2. Allocation is exception safe.
        //
        //   3. The `bslma::UsesBslmaAllocator` trait is set for this class.
        //
        // 3. Basic Accessors
        //
        //   1. The return status of the `parse` method indicates success when
        //      given command options valid for the defined options, and
        //      indicates failure otherwise.
        //
        //  2. Accessors are `const`-qualified.
        //
        //  3. The accessors `isParsed` and `isValid` how the expected state
        //     of the object.
        //
        //  4. Accessor arguments can be `const`-qualified.
        //
        // 4. QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        // 1. Use `u::generateTestData` to create a table of valid option
        //    configurations and command-line arguments corresponding to each
        //    option.  For each element in the generated table in the grange
        //    `0 < numBaseOptions`, create configurations having 0, 1, 2, and 3
        //    options.
        //
        // 2. For each configuration create an `balcl::CommandLine` object
        //    three different ways: specifying an object allocator, not
        //    specifying and specifying a 0-allocator.
        //
        // 3. The `parse` manipulator is tested by creating the test object
        //    twice.  The `parse` method of the first is invoked using valid
        //    command-line options.  The second is invoked using command-line
        //    options that have been crudely invalidated.
        //
        //   1. Note that two objects are required because invoking `parse`
        //      more than once on these objects is disallowed.
        //
        //   2. Confirm the expected return value of `parse`.
        //
        //   3. Use accessors to confirm the expected state of the object.
        //
        // 4. Basic accessors:
        //
        //   1. Always invoke the basic accessors on a `const` reference to the
        //      to the object under test.  If the accessors are not
        //      `const`-qualified, the test driver does not compile.
        //
        //   2. Pass accessor allocator arguments by `const` reference.  If the
        //      arguments are not `const`-qualified, the test driver does not
        //      compile.
        //
        // 5. Use `bslma::TestAllocatorMonitor` objects to confirm allocations
        //    from the intended allocators.
        //
        //   1. Confirm that
        //      `bslma::UsesBslmaAllocator<balcl::CommandLine>::value` is
        //      `true` in a compile-time assertion at file scope.
        //
        //   2. Use `BSLMA_TESTALLOCATOR_EXCEPTION*` macros to check
        //      allocations in the presence of exceptions.
        //
        // 6. Negative testing of precondition tests are addressed in
        //    TC 11 (TESTING INVALID OPTION SPECS).  (C-4)
        //
        // Testing:
        //   CommandLine(const Oi *table, int len, *bA = 0);
        //   ~CommandLine();
        //   int parse(int argc, const char *const argv[], ostream& stream);
        //   bool isValid() const;
        //   bool isParsed() const;
        //   bslma::Allocator *allocator() const;
        // --------------------------------------------------------------------

        if (verbose) cout
                  << endl
                  << "PRIMARY MANIPULATORS, BASIC ASSESSORS, AND DTOR" << endl
                  << "===============================================" << endl;

        PREAMBLE_OPTIONS_ARGUMENTS

        const int MAX_OPTIONS = static_cast<int>(OPTIONS.size());

// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
        for (int n = 0; n < 4;                         ++n) {
        for (int i = 0; i < (n ? NUM_OPTIONS - n : 1); ++i) {

            OptionInfo        specTable[4];
            const OptionInfo *SPEC_TABLE = specTable;

            GENERATE_PARSE_INPUT(n, i)

            // Create bad parse input.
            bsl::vector<const char *> badArgPtrs(ARG_PTRS);
            badArgPtrs.push_back("--some-invalid-option");
            const bsl::vector<const char *>& BAD_ARG_PTRS = badArgPtrs;

            for (int j = 0; j < n; ++j) {
                int index = i + j * NUM_OPTIONS;

                ASSERTV(index, MAX_OPTIONS, 0     <= index);
                ASSERTV(index, MAX_OPTIONS, index <  MAX_OPTIONS);

                specTable[j] = OPTIONS[index];
            }

            if (veryVerbose) {
                P_(i) P(n)
                for (int j = 0; j < n; ++j) {
                    T_ P_(j) P(SPEC_TABLE[j])
                }
                P_(i) P_(n) P(ARG_STRING)
                P_(i) P_(n) P(ARG_PTRS.size())
                for (int k = 0; k < static_cast<int>(ARG_PTRS.size()); ++k) {
                    T_ P_(k) P(ARG_PTRS[k])
                }
            }

            ASSERT(Obj::isValidOptionSpecificationTable(SPEC_TABLE, n));

            for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
                const char CONFIG = cfg;

                if (veryVerbose) {
                    T_ T_ P(CONFIG)
                }

                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                bslma::TestAllocator *objAllocatorPtr  = 'a' == CONFIG ? &da
                                                       : 'b' == CONFIG ? &da
                                                       : 'c' == CONFIG ? &sa
                                                       : 0;
                ASSERTV(CONFIG, objAllocatorPtr);

                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = 'c' == CONFIG ? da : sa;

                bslma::TestAllocatorMonitor  oam(&oa);
                bslma::TestAllocatorMonitor noam(&noa);

                Obj *objPtr = 0;

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {

                    objPtr = 'a' == CONFIG ? new (fa) Obj(SPEC_TABLE, n)
                           : 'b' == CONFIG ? new (fa) Obj(SPEC_TABLE, n, 0)
                           : 'c' == CONFIG ? new (fa) Obj(SPEC_TABLE, n, &sa)
                           : 0;   // ACTION
                    ASSERTV(CONFIG, objPtr);

                }  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                Obj&  mX = *objPtr;  const Obj& X = mX;

                // Vet allocator

                ASSERTV(CONFIG, &oa == X.allocator());

                // Vet object's state.

                if (veryVerbose) { T_ T_ P_(i) P_(n) P(X) }

                ASSERTV(CONFIG, !X.isParsed());
                ASSERTV(CONFIG,  X.isValid());

                bsl::ostringstream           oss;
                bslma::TestAllocatorMonitor  oamP( &oa);
                bslma::TestAllocatorMonitor noamP(&noa);

                int parseRet = mX.parse(static_cast<int>(ARG_PTRS.size()),
                                        &ARG_PTRS[0],
                                        oss);  // ACTION

                ASSERTV(CONFIG,  oamP.isTotalUp());
                ASSERTV(CONFIG, noamP.isTotalSame());

                ASSERTV(CONFIG, 0 == parseRet);
                bool fail = !oss.str().empty();
                ASSERTV(CONFIG, ARG_STRING, oss.str(), !fail);
                ASSERTV(CONFIG,  X.isParsed());
                ASSERTV(CONFIG,  X.isValid());

                fa.deleteObject(objPtr); // Clean up

                ASSERTV(CONFIG,  oam.isInUseSame());
                ASSERTV(CONFIG, noam.isInUseSame());

                // Repeat with invalid parse input.

                objPtr = 'a' == CONFIG ? new (fa) Obj(SPEC_TABLE, n)
                       : 'b' == CONFIG ? new (fa) Obj(SPEC_TABLE, n, 0)
                       : 'c' == CONFIG ? new (fa) Obj(SPEC_TABLE, n, &sa)
                       : 0;   // ACTION
                ASSERTV(CONFIG, objPtr);

                Obj&  mY = *objPtr;  const Obj& Y = mY;

                ASSERTV(CONFIG, !Y.isParsed());
                ASSERTV(CONFIG,  Y.isValid());

                {
                    bsl::ostringstream oss;
                    parseRet = mY.parse(static_cast<int>(BAD_ARG_PTRS.size()),
                                        &BAD_ARG_PTRS[0],
                                        oss);  // ACTION

                    ASSERTV(CONFIG, 0 != parseRet);
                    ASSERTV(CONFIG, oss.str(), !oss.str().empty());
                }

                ASSERTV(CONFIG, !Y.isParsed());
                ASSERTV(CONFIG, !Y.isValid());

                fa.deleteObject(objPtr); // Clean up again

                ASSERTV(CONFIG,  oam.isInUseSame());
                ASSERTV(CONFIG, noam.isInUseSame());
            }
        }
        }
// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING `u::parseCommandLine` TESTING UTILITY
        //   Exercise the basic functionality.
        //
        // Concerns:
        // 1. That the `u::parseCommandLine` testing utility function separates
        //    a command line as needed into its `argc` and `argv`.
        //
        // Plan:
        // 1. It is enough to test with zero, one, or two arguments on the
        //    command line, and test with various spacing and separation
        //    characters.  For orthogonal perturbations, add a header or a
        //    trailer or both.
        //
        // Testing:
        //   TESTING `u::parseCommandLine` TESTING UTILITY
        // --------------------------------------------------------------------

        if (verbose) cout
                    << endl
                    << "TESTING `u::parseCommandLine` TESTING UTILITY" << endl
                    << "=============================================" << endl;

        const char *ONECMD[] = { "oneCommand" };
        const char *TWOCMD[] = { "two", "commands" };

        const char HEADER[]  = " \t ";
        const int  NHEADER   = static_cast<int>(sizeof HEADER ) - 1; // length

        const char TRAILER[] = " \t ";
        const int  NTRAILER  = static_cast<int>(sizeof TRAILER) - 1; // length

        static const struct {
            int          d_line;
            int          d_retCode;
            int          d_argc;
            const char **d_argv_p;
            const char  *d_cmdLine_p;
        } DATA[] = {
            // LINE  RET  ARGC  ARGV  CMD_LINE
            // ----  ---  ----  ----  --------
            {  L_,    -1,   0,  0,    "\n"                                   },
            {  L_,    -1,   0,  0,    "oneIllTerminatedCommand\n"            },
            {  L_,    -1,   0,  0,    "t o o   m a n y   c o m m a n d s . . ."
              " . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ." },

            {  L_,    0,    0,  0,    ""                                     },

            {  L_,    0,    1,  ONECMD,
                                      "oneCommand"                           },

            {  L_,    0,    2,  TWOCMD, "two commands"                       },
            {  L_,    0,    2,  TWOCMD, "two\tcommands"                      },
            {  L_,    0,    2,  TWOCMD, "two \t commands"                    },
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int i=0; i < NUM_DATA; ++i) {
            const int           LINE     = DATA[i].d_line;
            const int           RET      = DATA[i].d_retCode;
            const int           ARGC     = DATA[i].d_argc;
            const char * const *ARGV     = DATA[i].d_argv_p;
            const char *        CMD_LINE = DATA[i].d_cmdLine_p;

            {
                const int  NLINE = static_cast<int>(bsl::strlen(CMD_LINE));
                char      *cmdLine = new char[NLINE+1];
                bsl::strcpy(cmdLine, CMD_LINE);
                if (veryVerbose) { T_ P_(LINE) P(cmdLine) }

                int         argc;
                const char *argv[u::MAX_ARGS];
                const int   ret = u::parseCommandLine(cmdLine, argc, argv, 10);

                ASSERTV(LINE, RET  == ret);
                if (0 == ret) {
                    ASSERTV(LINE, ARGC == argc);
                    for (int i=0; i < argc; ++i) {
                        ASSERTV(LINE, 0 == bsl::strcmp(ARGV[i], argv[i]));
                    }
                }
                delete[] cmdLine;
            }

            {
                const int  NLINE = static_cast<int>(bsl::strlen(CMD_LINE));
                char      *cmdLine = new char[NLINE+NHEADER+1];
                bsl::strcpy(cmdLine,         HEADER);
                bsl::strcpy(cmdLine+NHEADER, CMD_LINE);
                if (veryVerbose) { T_ P_(LINE) P(cmdLine) }

                int         argc;
                const char *argv[u::MAX_ARGS];
                const int   ret = u::parseCommandLine(cmdLine, argc, argv, 10);

                ASSERTV(LINE, RET  == ret);
                if (0 == ret) {
                    ASSERTV(LINE, ARGC == argc);
                    for (int i=0; i < argc; ++i) {
                        ASSERTV(LINE, 0 == bsl::strcmp(ARGV[i], argv[i]));
                    }
                }
                delete[] cmdLine;
            }

            {
                const int  NLINE = static_cast<int>(bsl::strlen(CMD_LINE));
                char      *cmdLine = new char[NLINE+NTRAILER+1];
                bsl::strcpy(cmdLine,       CMD_LINE);
                bsl::strcpy(cmdLine+NLINE, TRAILER);
                if (veryVerbose) { T_ P_(LINE) P(cmdLine) }

                int         argc;
                const char *argv[u::MAX_ARGS];
                const int   ret = u::parseCommandLine(cmdLine, argc, argv, 10);

                ASSERTV(LINE, RET  == ret);
                if (0 == ret) {
                    ASSERTV(LINE, ARGC == argc);
                    for (int i=0; i < argc; ++i) {
                        ASSERTV(LINE, 0 == bsl::strcmp(ARGV[i], argv[i]));
                    }
                }
                delete[] cmdLine;
            }

            {
                const int  NLINE = static_cast<int>(bsl::strlen(CMD_LINE));
                char      *cmdLine = new char[NHEADER+NLINE+NTRAILER+1];
                bsl::strcpy(cmdLine,               HEADER);
                bsl::strcpy(cmdLine+NHEADER,       CMD_LINE);
                bsl::strcpy(cmdLine+NHEADER+NLINE, TRAILER);
                if (veryVerbose) { T_ P_(LINE) P(cmdLine) }

                int         argc;
                const char *argv[u::MAX_ARGS];

                const int ret = u::parseCommandLine(cmdLine, argc, argv, 10);
                ASSERTV(LINE, RET  == ret);
                if (0 == ret) {
                    ASSERTV(LINE, ARGC == argc);
                    for (int i=0; i < argc; ++i) {
                        ASSERTV(LINE, 0 == bsl::strcmp(ARGV[i], argv[i]));
                    }
                }
                delete[] cmdLine;
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   Exercise the basic functionality.
        //
        // Concerns:
        // 1. Basic functionality must work as advertised for most common usage
        //    of this component.
        //
        // Plan:
        // 1. Various ad hoc use of this component.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        const int defaultNonOptionValue = 12345;

        int longOptionOnlyValue = -1;

        int nonOptionValue1 = -1;
        int nonOptionValue2 = -1;
        int nonOptionValue3 = -1;

        balcl::OptionInfo specTable[] = {
          {
            "f|flag",                                     // tag
            "genericFlag",                                // name
            "provide generic option for breathing test",  // description
            balcl::TypeInfo(balcl::OptionType::k_BOOL),   // no linked variable
            balcl::OccurrenceInfo::e_REQUIRED,            // occurrence info
            ""                                            // env var name
          },
          {
            "s|long-option-with-dashes",                  // tag
            "longOptionValue",                            // name
            "provide generic option for breathing test",  // description
            balcl::TypeInfo(balcl::OptionType::k_INT_ARRAY),
                                                          // no linked variable
            balcl::OccurrenceInfo::e_REQUIRED,            // occurrence info
            ""                                            // env var name
          },
          {
            "long-option-only",                           // tag
            "longOptionOnlyValue",                        // name
            "provide generic option for breathing test",  // description
            balcl::TypeInfo(&longOptionOnlyValue),        // linked variable
            balcl::OccurrenceInfo::e_HIDDEN,              // occurrence info
            ""                                            // env var name
          },
          {
            "",                                           // tag
            "nonOptionValue1",                            // name
            "provide generic option for breathing test",  // description
            balcl::TypeInfo(&nonOptionValue1),            // linked variable
            balcl::OccurrenceInfo::e_REQUIRED,            // occurrence info
            ""                                            // env var name
          },
          {
            "",                                           // tag
            "nonOptionValue2",                            // name
            "provide generic option for breathing test",  // description
            balcl::TypeInfo(&nonOptionValue2),            // linked variable
            balcl::OccurrenceInfo(defaultNonOptionValue), // occurrence info
            ""                                            // env var name
          },
          {
            "",                                           // tag
            "nonOptionValue3",                            // name
            "provide generic option for breathing test",  // description
            balcl::TypeInfo(&nonOptionValue3),            // linked variable
            balcl::OccurrenceInfo(defaultNonOptionValue), // occurrence info
            ""                                            // env var name
          }
        };
        enum { numSpecTable = sizeof specTable / sizeof *specTable };

        if (verbose) cout << "\tCreating command line object." << endl;
        {
            balcl::CommandLine cmdLine(specTable, numSpecTable);

            if (verbose) {
                cmdLine.printUsage();
            }

            ASSERT(!cmdLine.isParsed());
        }

        if (verbose) cout << "\n\tParse a valid command line." << endl;
        {
            balcl::CommandLine cmdLine(specTable, numSpecTable);

            const char *argv[] = {
                "someTaskName",
                "54321",  // required non-option argument 1
                "-s", "1",
                "-f",
                "--long-option-with-dashes", "2",
                "98765",  // optional non-option argument 2
                "-s=3",
                "--long-option-only", "123",
                "--long-option-with-dashes=4"
            };
            enum { argc = sizeof argv / sizeof *argv };

            if (verbose) {
                cout << "ARGUMENTS = \"" << argv[0];
                for (int i = 1; i < argc; ++i) {
                    cout << " " << argv[i];
                }
                cout << "\"" << endl;
            }

            bsl::ostringstream errorStream;
            ASSERT(0 == cmdLine.parse(argc, argv, errorStream));
            ASSERT(cmdLine.isParsed());

            if (verbose) {
                cout << "PARSING OUTPUT = ";
                if (errorStream.str().empty()) {
                    cout << "<empty>" << endl;
                } else {
                    cout << errorStream.str();
                }
                cout << "LINKED VARIABLES = " << endl;
                T_ P(defaultNonOptionValue)
                T_ P(longOptionOnlyValue)
                T_ P(nonOptionValue1)
                T_ P(nonOptionValue2)
                T_ P(nonOptionValue3)
                cout << "ROW BINDING = ";
                if (cmdLine.isParsed()) {
                    cmdLine.print(cout, -2, 4);
                } else {
                    cout << "<not parsed>" << endl;
                }
            }

            ASSERT(123   == longOptionOnlyValue);
            ASSERT(54321 == nonOptionValue1);
            ASSERT(98765 == nonOptionValue2);
            ASSERT(defaultNonOptionValue == nonOptionValue3);
        }

        if (verbose) cout << "\n\tParse an invalid command line." << endl;
        {
            balcl::CommandLine cmdLine(specTable, numSpecTable);

            longOptionOnlyValue = -1;
            nonOptionValue1     = -1;
            nonOptionValue2     = -1;
            nonOptionValue3     = -1;

            const char *argv[] = {
                "someOtherTaskName",
                "--long-option-only", "1234",
                // "54321",  // required non-option argument 1
                "-s", "1",
                // "-f",
                "--long-option-with-dashes", "2",
                // "98765",  // optional non-option argument 2
                "-s=3",
                "--long-option-with-dashes=4"
            };
            enum { argc = sizeof argv / sizeof *argv };

            if (verbose) {
                cout << "ARGUMENTS = \"" << argv[0];
                for (int i = 1; i < argc; ++i) {
                    cout << " " << argv[i];
                }
                cout << "\"" << endl;
            }

            bsl::ostringstream errorStream;
            ASSERT(0 != cmdLine.parse(argc, argv, errorStream));
            ASSERT(!cmdLine.isParsed());

            if (verbose) {
                cout << "PARSING OUTPUT = ";
                if (errorStream.str().empty()) {
                    cout << "<empty>" << endl;
                } else {
                    cout << errorStream.str();
                }
                cout << "LINKED VARIABLES = " << endl;
                T_ P(defaultNonOptionValue)
                T_ P(longOptionOnlyValue)
                T_ P(nonOptionValue1)
                T_ P(nonOptionValue2)
                T_ P(nonOptionValue3)
                cout << "ROW BINDING = ";
                if (cmdLine.isParsed()) {
                    cmdLine.print(cout, -2, 4);
                } else {
                    cout << "<not parsed>" << endl;
                }
            }

            ASSERT(-1 == longOptionOnlyValue);
            ASSERT(-1 == nonOptionValue1);
            ASSERT(-1 == nonOptionValue2);
            ASSERT(-1 == nonOptionValue3);
        }
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
// Copyright 2020 Bloomberg Finance L.P.
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
