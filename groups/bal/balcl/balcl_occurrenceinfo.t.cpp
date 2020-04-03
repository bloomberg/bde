// balcl_occurrenceinfo.t.cpp                                         -*-C++-*-

#include <balcl_occurrenceinfo.h>

#include <balcl_optiontype.h>
#include <balcl_optionvalue.h>

#include <bdlb_numericparseutil.h>
#include <bdlb_printmethods.h>     // 'bdlb::HasPrintMethod'

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_time.h>

#include <bslim_testutil.h>

#include <bslma_default.h>  // 'bslma::globalAllocator'
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_platform.h>  // 'BSLS_PLATFORM_CPU_32_BIT'
#include <bsls_types.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>    // 'bsl::strcmp'
#include <bsl_iostream.h>
#include <bsl_ostream.h>    // 'bsl::ostream<<'
#include <bsl_string.h>     // 'bslstl::StringRef'
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// The class under test is a complex-constrained (value-semantic) attribute
// class that is tested by the commonly-used idioms employed for such classes.
//
// Primary Manipulators
//: o OccurrenceInfo(OccurrenceType type, *bA = 0);
//: o void setDefaultValue(const OptionValue& defaultValue);
//: o void setHidden();
//
// Basic Accessors
//: o const OptionValue& defaultValue() const;
//: o bool hasDefaultValue() const;
//: o bool isHidden() const;
//: o bool isRequired() const;
//: o OccurrenceType occurrenceType() const;
//: o bslma::Allocator *allocator() const;
//
///Input Tables
///------------
// There are two input tables (defined at file scope) that are used in many of
// the case cases.  The cross product of these tables qualitatively covers the
// space of possible inputs.  The two tables are:
//
//: 1 'OPTION_OCCURRENCES': an entry for each of the three allowed values of
//:   'OccurrenceType'.
//:
//: 2 'OPTION_DEFAULT_VALUES': an entry for each of the allowed option types
//:   and the address of a value of that type to be used as a default option
//:   value.
//:
//:   o None of these "default" values correspond to the default value of their
//:     respective type.
//:
//:   o The value chosen for 'Ot::e_STRING' is sufficiently long to exceed the
//:     short-string optimization.
//:
//:   o A helper function, 'u::setOptionValue' (defined below), is provided to
//:     convert the "value" field of 'OPTION_DEFAULT_VALUES' into an argument
//:     for the 'setDefaultValue' method.
//
// ----------------------------------------------------------------------------
// CREATORS
// [ 8] OccurrenceInfo();
// [ 8] OccurrenceInfo(bslma::Allocator *basicAllocator);
// [ 2] OccurrenceInfo(OccurrenceType type, *bA = 0);
// [ 7] OccurrenceInfo(char                    dflt, *bA = 0);
// [ 7] OccurrenceInfo(int                     dflt, *bA = 0);
// [ 7] OccurrenceInfo(Int64                   dflt, *bA = 0);
// [ 7] OccurrenceInfo(double                  dflt, *bA = 0);
// [ 7] OccurrenceInfo(const string&           dflt, *bA = 0);
// [ 7] OccurrenceInfo(const Datetime&         dflt, *bA = 0);
// [ 7] OccurrenceInfo(const Date&             dflt, *bA = 0);
// [ 7] OccurrenceInfo(const Time&             dflt, *bA = 0);
// [ 7] OccurrenceInfo(const vector<char>&     dflt, *bA = 0);
// [ 7] OccurrenceInfo(const vector<int>&      dflt, *bA = 0);
// [ 7] OccurrenceInfo(const vector<Int64>&    dflt, *bA = 0);
// [ 7] OccurrenceInfo(const vector<double>&   dflt, *bA = 0);
// [ 7] OccurrenceInfo(const vector<string>&   dflt, *bA = 0);
// [ 7] OccurrenceInfo(const vector<Datetime>& dflt, *bA = 0);
// [ 7] OccurrenceInfo(const vector<Date>&     dflt, *bA = 0);
// [ 7] OccurrenceInfo(const vector<Time>&     dflt, *bA = 0);
// [ 5] OccurrenceInfo(const Oi& original, *bA = 0);
// [ 2] ~OccurrenceInfo();
//
// MANIPULATORS
// [ 6] OccurrenceInfo& operator=(const Oi& rhs);
// [ 2] void setDefaultValue(const OptionValue& defaultValue);
// [ 2] void setHidden();
//
// ACCESSORS
// [ 2] const OptionValue& defaultValue() const;
// [ 2] bool hasDefaultValue() const;
// [ 2] bool isHidden() const;
// [ 2] bool isRequired() const;
// [ 2] OccurrenceType occurrenceType() const;
//
// [ 2] bslma::Allocator *allocator() const;
// [ 3] ostream& print(ostream& stream, int level = 0, int spl = 4) const;
//
// FREE OPERATORS
// [ 4] bool operator==(const OccurrenceInfo& lhs, rhs)
// [ 4] bool operator!=(const OccurrenceInfo& lhs, rhs)
// [ 3] operator<<(ostream& stream, const OccurrenceInfo& rhs);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST

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

#define ASSERT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)

// ============================================================================
//                      GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef balcl::OccurrenceInfo   Obj;

typedef Obj::OccurrenceType     OccurType;
typedef balcl::OptionType       Ot;
typedef balcl::OptionType::Enum ElemType;
typedef balcl::OptionValue      OptionValue;

typedef bsls::Types::Int64      Int64;

// ============================================================================
//                                TYPE TRAITS
// ----------------------------------------------------------------------------

BSLMF_ASSERT(bslma::UsesBslmaAllocator<Obj>::value);
BSLMF_ASSERT(bdlb::HasPrintMethod<Obj>::value);

// ============================================================================
//          GLOBAL CONSTANTS/VARIABLES/FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

enum { k_DATETIME_FIELD_WIDTH = 25
     ,     k_DATE_FIELD_WIDTH =  9
     ,     k_TIME_FIELD_WIDTH = 15 };

// ATTRIBUTES FOR 'balcl::OccurrenceInfo'
static const struct {
    int        d_line;  // line number
    OccurType  d_type;  // name attribute
} OPTION_OCCURRENCES[] = {
    { L_, Obj::e_REQUIRED }
  , { L_, Obj::e_OPTIONAL }
  , { L_, Obj::e_HIDDEN   }
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

#define GA bslma::Default::globalAllocator()

bool                        defaultBool          = false;
char                        defaultChar          = 'D';
short                       defaultShort         = 1234;
int                         defaultInt           = 1234567;
Int64                       defaultInt64         = 123456789LL;
float                       defaultFloat         = 0.125;     // 1/8
double                      defaultDouble        = 0.015625;  // 1/64
bsl::string                 defaultString(SUFFICIENTLY_LONG_STRING, GA);
bdlt::Datetime              defaultDatetime(1234, 12, 3, 4, 5, 6);
bdlt::Date                  defaultDate(1234, 4, 6);
bdlt::Time                  defaultTime(7, 8, 9, 10);
bsl::vector<char>           defaultCharArray    (1, defaultChar,     GA);
bsl::vector<short>          defaultShortArray   (1, defaultShort,    GA);
bsl::vector<int>            defaultIntArray     (1, defaultInt,      GA);
bsl::vector<Int64>          defaultInt64Array   (1, defaultInt64,    GA);
bsl::vector<float>          defaultFloatArray   (1, defaultFloat,    GA);
bsl::vector<double>         defaultDoubleArray  (1, defaultDouble,   GA);
bsl::vector<bsl::string>    defaultStringArray  (1, defaultString,   GA);
bsl::vector<bdlt::Datetime> defaultDatetimeArray(1, defaultDatetime, GA);
bsl::vector<bdlt::Date>     defaultDateArray    (1, defaultDate,     GA);
bsl::vector<bdlt::Time>     defaultTimeArray    (1, defaultTime,     GA);

static const struct {
    int             d_line;   // line number
    ElemType        d_type;   // option type
    const void     *d_value_p;  // default value attribute(s)
} OPTION_DEFAULT_VALUES[] = {
    { L_, Ot::e_BOOL,            0                     }
  , { L_, Ot::e_CHAR,            &defaultChar          }
  , { L_, Ot::e_INT,             &defaultInt           }
  , { L_, Ot::e_INT64,           &defaultInt64         }
  , { L_, Ot::e_DOUBLE,          &defaultDouble        }
  , { L_, Ot::e_STRING,          &defaultString        }
  , { L_, Ot::e_DATETIME,        &defaultDatetime      }
  , { L_, Ot::e_DATE,            &defaultDate          }
  , { L_, Ot::e_TIME,            &defaultTime          }
  , { L_, Ot::e_CHAR_ARRAY,      &defaultCharArray     }
  , { L_, Ot::e_INT_ARRAY,       &defaultIntArray      }
  , { L_, Ot::e_INT64_ARRAY,     &defaultInt64Array    }
  , { L_, Ot::e_DOUBLE_ARRAY,    &defaultDoubleArray   }
  , { L_, Ot::e_STRING_ARRAY,    &defaultStringArray   }
  , { L_, Ot::e_DATETIME_ARRAY,  &defaultDatetimeArray }
  , { L_, Ot::e_DATE_ARRAY,      &defaultDateArray     }
  , { L_, Ot::e_TIME_ARRAY,      &defaultTimeArray     }
};
enum { NUM_OPTION_DEFAULT_VALUES = sizeof  OPTION_DEFAULT_VALUES
                                 / sizeof *OPTION_DEFAULT_VALUES };

// ============================================================================
//                          HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

namespace {
namespace u {

int monthStrToInt(const char *input)
    // Return the integer month value ('[1 .. 12]') corresponding to the three
    // letter month representation found at the specified 'input'.  The
    // representation for each month matches that generated by the 'toAscii'
    // method of 'bdlt::MonthOfYear'.
{
    BSLS_ASSERT(input);

    ASSERT(3 == bsl::strlen(input));

    return 0 == bsl::strcmp("JAN", input) ?  1 :
           0 == bsl::strcmp("FEB", input) ?  2 :
           0 == bsl::strcmp("MAR", input) ?  3 :
           0 == bsl::strcmp("APR", input) ?  4 :
           0 == bsl::strcmp("MAY", input) ?  5 :
           0 == bsl::strcmp("JUN", input) ?  6 :
           0 == bsl::strcmp("JUL", input) ?  7 :
           0 == bsl::strcmp("AUG", input) ?  8 :
           0 == bsl::strcmp("SEP", input) ?  9 :
           0 == bsl::strcmp("OCT", input) ? 10 :
           0 == bsl::strcmp("NOV", input) ? 11 :
           0 == bsl::strcmp("DEC", input) ? 12 :
           /* error */                      -1 ;
}

int parseDate(bdlt::Date *result, const bslstl::StringRef& input)
    // Set the specified 'result' to the date value represented at the
    // specified 'input'.  The expected input format matches that of the
    // 'print' method of 'bdlt::Date' (e.g., '01JAN1970').
{
    ASSERT(9 == input.size());

    bslstl::StringRef   dayField(input.begin() + 0, input.begin() + 2);
    bslstl::StringRef monthField(input.begin() + 2, input.begin() + 5);
    bslstl::StringRef  yearField(input.begin() + 5, input.begin() + 9);

    int   dayAsInt = bsl::atoi    (bsl::string(  dayField).c_str());
    int monthAsInt = monthStrToInt(bsl::string(monthField).c_str());
    int  yearAsInt = bsl::atoi    (bsl::string( yearField).c_str());

    if (bdlt::Date::isValidYearMonthDay(yearAsInt, monthAsInt, dayAsInt)) {
        *result = bdlt::Date(yearAsInt, monthAsInt, dayAsInt);
        return 0;                                                     // RETURN
    } else {
        return -1;                                                    // RETURN
    }
}

int parseTime(bdlt::Time *result, const bslstl::StringRef& input)
    // Set the specified 'result' to the time of day value represented at the
    // specified 'input'.  The expected input format matches that of the
    // 'print' method of 'bdlt::Time' (e.g., '12:34:56.123456').
{
    ASSERT(k_TIME_FIELD_WIDTH == input.size());

    bslstl::StringRef        hourField(input.begin() + 0, input.begin() +  2);
    bslstl::StringRef      minuteField(input.begin() + 3, input.begin() +  5);
    bslstl::StringRef      secondField(input.begin() + 6, input.begin() +  8);
    bslstl::StringRef microsecondField(input.begin() + 9, input.begin() + 15);

    int        hourAsInt = bsl::atoi(bsl::string(       hourField).c_str());
    int      minuteAsInt = bsl::atoi(bsl::string(     minuteField).c_str());
    int      secondAsInt = bsl::atoi(bsl::string(     secondField).c_str());
    int microsecondAsInt = bsl::atoi(bsl::string(microsecondField).c_str());

    int millisecondAsInt = microsecondAsInt / 1000;
        microsecondAsInt = microsecondAsInt % 1000;

    if (bdlt::Time::isValid(hourAsInt,
                            minuteAsInt,
                            secondAsInt,
                            microsecondAsInt,
                            microsecondAsInt)) {

        *result = bdlt::Time(hourAsInt,
                             minuteAsInt,
                             secondAsInt,
                             millisecondAsInt,
                             microsecondAsInt);
        return  0;                                                    // RETURN
    } else {
        return -1;                                                    // RETURN
    }
}

int parseDatetime(bdlt::Datetime *result, const bslstl::StringRef& input)
    // Set the specified 'result' to the datetime value represented at the
    // specified 'input'.  The expected input format matches that of the
    // 'parseDate' and 'parseTime' methods described above, with the two values
    // separated by an '_' (e.g., '01JAN1970_12:34:56.123456').
{
    ASSERT(k_DATETIME_FIELD_WIDTH == input.size());

    bslstl::StringRef dateField(input.begin() + 0,
                                input.begin() + k_DATE_FIELD_WIDTH);
    bslstl::StringRef timeField(input.begin() + k_DATE_FIELD_WIDTH + 1,
                                input.begin() + k_DATETIME_FIELD_WIDTH);

    bdlt::Date date;
    int        rc = parseDate(&date, dateField);
    if (0 != rc) {
        return -1;                                                    // RETURN
    }

    bdlt::Time time;
    rc = parseTime(&time, timeField);
    if (0 != rc) {
        return -2;                                                    // RETURN
    }

    *result = bdlt::Datetime(date, time);
    return 0;
}

                            // ====================
                            // struct ParserImpUtil
                            // ====================

int skipRequiredToken(const char **endPos,
                      const char  *inputString,
                      const char  *token)
    // Skip past the value of the specified 'token' in the specified
    // 'inputString'.  Store in the specified '*endPos' the address of the
    // non-modifiable character in 'inputString' immediately following the
    // successfully matched text, or the position at which the match failure
    // was detected.  Return 0 if 'token' is found, and a non-zero value
    // otherwise.
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(inputString);
    BSLS_ASSERT(token);

    *endPos = inputString;
    while ('\0' != *token && *token == **endPos) {
        ++*endPos;
        ++token;
    }

    return '\0' != *token;  // return "is null char"
}

int skipWhiteSpace(const char **endPos,
                   const char  *inputString)
    // Skip over any combination of C-style comments, C++-style comments, and
    // characters for which 'isspace' returns true in the specified
    // 'inputString'.  Store in the specified '*endPos' the address of the
    // non-modifiable character in 'inputString' immediately following the
    // successfully matched text, or the position at which the match failure
    // was detected.  Return 0 on success and a non-zero value otherwise.  If a
    // C++-style comment terminates with '\0', 'endPos' will point *at* the
    // '\0' and not past it.  The behavior is undefined if either argument is
    // 0.
    //
    // A parse failure can occur for the following reason:
    //..
    //   '\0' is found before a C-style comment is terminated with '*/'.
    //..
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(inputString);

    while (1) {
        while (isspace(static_cast<unsigned char>(*inputString))) {
            ++inputString;
        }
        if (*inputString != '/') {
            *endPos = inputString;
            return 0;                                                 // RETURN
        }
        else {
            ++inputString;
            if ('*' == *inputString) {
                // This is a C comment.
                ++inputString;
                while (1) {
                    if ('\0' == *inputString) {
                        // Comment is erroneous.
                        *endPos = inputString;
                        return 1;                                     // RETURN
                    }
                    if ('*' == *inputString && '/' == *(inputString + 1)) {
                        // Found end of comment.
                        inputString += 2;
                        break;
                    }
                    ++inputString;
                }
            } else if ('/' == *inputString) {
                // This is a C++ comment.
                ++inputString;
                while (1) {
                    if ('\n' == *inputString) {
                        // End of string means end of comment.
                        ++inputString;
                        break;
                    }
                    if ('\0' == *inputString) {
                        // Reached end of string.
                        *endPos = inputString;
                        return 0;                                     // RETURN
                    }
                    ++inputString;
                }
            } else {
                *endPos = inputString - 1;
                return 0;                                             // RETURN
            }
        }
    }
}

                          // ==================
                          // struct TypesParser
                          // ==================

struct TypesParser {

    enum { LOCAL_SUCCESS = 0, LOCAL_FAILURE = 1 };

    template <class TYPE>
    static int parse(TYPE        *result,
                     const char **endPos,
                     const char  *inputString);
        // Load into the specified 'result' the 'TYPE' value represented at the
        // specified 'inputString'.  The specified '*endPos' is set to one past
        // the parsed value or to the position at which a parse error is
        // detected.  Return 0 on success and a non-zero value otherwise.  The
        // supported types are:
        //: o 'int'
        //: o 'Int64'
        //: o 'double'
        //: o 'bsl::string'
        //: o 'bdlt::Datetime'
        //: o 'bdlt::Date'
        //: o 'bdlt::Time'

    static int parseQuotedString(bsl::string  *result,
                                 const char  **endPos,
                                 const char   *inputString);
        // Load into the specified 'result' the next double quote (i.e., '"')
        // delimited sequence of characters found at the specified
        // 'inputString'.  The specified '*endPos' is set to one past the
        // parsed value or to the position at which a parse error is detected.
        // Return 0 on success and a non-zero value otherwise.
};

                          // ------------------
                          // struct TypesParser
                          // ------------------

template <>
int TypesParser::parse(int         *result,
                       const char **endPos,
                       const char  *inputString)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(inputString);

    bslstl::StringRef remainder;
    int               status = bdlb::NumericParseUtil::parseInt(result,
                                                                &remainder,
                                                                inputString);
    *endPos = remainder.data();
    return status;
}

template <>
int TypesParser::parse(Int64       *result,
                       const char **endPos,
                       const char  *inputString)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(inputString);

    bslstl::StringRef remainder;
    int               status = bdlb::NumericParseUtil::parseInt64(result,
                                                                  &remainder,
                                                                  inputString);
    *endPos = remainder.data();
    return status;
}

template <>
int TypesParser::parse(double      *result,
                       const char **endPos,
                       const char  *inputString)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(inputString);

    char   *myEndPtr;
    double  retval =  bsl::strtod(inputString, &myEndPtr);
    if (0 == retval && myEndPtr == inputString) {
        return LOCAL_FAILURE;                                         // RETURN
    } else {
        *endPos = myEndPtr;
        *result = retval;
        return LOCAL_SUCCESS;                                         // RETURN
    }
}

template <>
int TypesParser::parse(bsl::string  *result,
                       const char  **endPos,
                       const char   *inputString)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(inputString);

    bsl::istringstream input(inputString);
    bsl::istream       is(input.rdbuf());

    is >> *result;
    *endPos = inputString + result->size();
    return 0;
}

template <>
int TypesParser::parse(bdlt::Datetime  *result,
                       const char     **endPos,
                       const char      *inputString)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(inputString);
    BSLS_ASSERT(k_DATETIME_FIELD_WIDTH <= bsl::strlen(inputString));

    const char        *end = inputString + k_DATETIME_FIELD_WIDTH;
    bslstl::StringRef  input(inputString, end);

    int rc = parseDatetime(result, input);

    if (0 != rc) {
        return LOCAL_FAILURE;                                         // RETURN
    } else {
        *endPos = end;
        return LOCAL_SUCCESS;                                         // RETURN
    }
}

template <>
int TypesParser::parse(bdlt::Date  *result,
                       const char **endPos,
                       const char  *inputString)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(inputString);
    BSLS_ASSERT(9 <= bsl::strlen(inputString));

    const char        *end = inputString + 9;
    bslstl::StringRef  input(inputString, end);

    int rc = parseDate(result, input);

    if (0 != rc) {
        return LOCAL_FAILURE;                                         // RETURN
    } else {
        *endPos = end;
        return LOCAL_SUCCESS;                                         // RETURN
    }
}

template <>
int TypesParser::parse(bdlt::Time  *result,
                       const char **endPos,
                       const char  *inputString)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(inputString);
    BSLS_ASSERT(k_TIME_FIELD_WIDTH <= bsl::strlen(inputString));

    const char        *end = inputString + k_TIME_FIELD_WIDTH;
    bslstl::StringRef  input(inputString, end);

    int rc = parseTime(result, input);

    if (0 != rc) {
        return LOCAL_FAILURE;                                         // RETURN
    } else {
        *endPos = end;
        return LOCAL_SUCCESS;                                         // RETURN
    }
}

int TypesParser::parseQuotedString(bsl::string  *result,
                                   const char  **endPos,
                                   const char   *inputString)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(inputString);

    const char *ptr = inputString;

    if (0 == *ptr) {
        *endPos = ptr;
        return -1;                                                    // RETURN
    }

    const char *leftQuote = bsl::strchr(ptr, '"');
    if (!leftQuote) {
        *endPos = ptr;
        return -2;                                                    // RETURN
    }

    ++ptr;

    if (0 == *ptr) {
        *endPos = ptr;
        return -3;                                                    // RETURN
    }

    const char *rightQuote = bsl::strchr(ptr, '"');
    if (!rightQuote) {
        *endPos = ptr;
        return -4;                                                    // RETURN
    }

    result->assign(leftQuote + 1, rightQuote);
    *endPos = rightQuote + 1;
    return 0;
}

                          // =========================
                          // struct ArrayParserImpUtil
                          // =========================

struct ArrayParserImpUtil {

    enum { LOCAL_SUCCESS = 0, LOCAL_FAILURE = 1 };

    template <class TYPE>
    static int parse(bsl::vector<TYPE>  *result,
                     const char        **endPos,
                     const char         *inputString);
        // Append to the specified 'result' the sequence of values obtained by
        // the '[', ']' delimited, whitespace separated sequence of
        // representations found at the specified 'inputString'.  The specified
        // '*endPos' is set just past the terminating ']' character, or the
        // position in which a parse failure is found.  Return 0 on success and
        // a non-zero value otherwise.
};

                          // -------------------------
                          // struct ArrayParserImpUtil
                          // -------------------------

template <class TYPE>
int ArrayParserImpUtil::parse(bsl::vector<TYPE>  *result,
                              const char        **endPos,
                              const char         *inputString)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(inputString);

    result->clear();

    if ('[' != *inputString) {
        *endPos = inputString;
        return LOCAL_FAILURE;                                         // RETURN
    }
    ++inputString;

    skipWhiteSpace(endPos, inputString);

    TYPE value;
    while (']' != **endPos) {
        if (u::TypesParser::parse(&value, endPos, *endPos)) {
            return LOCAL_FAILURE;                                     // RETURN
        }
        result->push_back(value);
        inputString = *endPos;
        skipWhiteSpace(endPos, inputString);
        if (inputString == *endPos && ']' != **endPos) {
            return LOCAL_FAILURE;                                     // RETURN
        }
    }
    ++*endPos;
    return LOCAL_SUCCESS;
}

                     // =================================
                     // function template checkOptionType
                     // =================================

template <int ELEM_TYPE, class TYPE>
struct CheckOptionType {
    // This general definition of the 'CheckOptionType' class template provides
    // a boolean functor that returns 'false'.

    bool operator()() const
        // Return 'false'.
    { return false; }
};

#define MATCH_OPTION_TYPE(ELEM_TYPE, TYPE)                                   \
    template <>                                                              \
    struct CheckOptionType<(int)ELEM_TYPE, TYPE> {                           \
        bool operator()() const { return true; }                             \
    };                                                                       \
    // This macro defines a specialization of the 'CheckOptionType' class for
    // template parameters 'ELEM_TYPE' and 'TYPE'.  Instances of this template
    // are boolean functors that always return 'true'.

#define MATCH_OPTION_TYPE_PAIR(ELEM_TYPE, TYPE)                              \
                                                                             \
    MATCH_OPTION_TYPE(ELEM_TYPE, TYPE)                                       \
                                                                             \
    template <>                                                              \
    struct CheckOptionType<(int)ELEM_TYPE##_ARRAY, bsl::vector<TYPE> > {     \
        bool operator()() const { return true; }                             \
    };
    // This macro defines two specializations of the 'CheckOptionType' class.
    // One specialization is for the template parameters 'ELEM_TYPE' and 'TYPE'
    // (see the definition of the 'MATCH_OPTION_TYPE' macro above).  The other
    // specialization is for 'Ot::toArray(ELEM_TYPE)' and 'TYPE'.  Instances of
    // either of these specializations are boolean functors that always return
    // 'true'.

// Define for each of the valid combinations of 'ELEM_TYPE' and 'TYPE' a
// specialization of the 'CheckOptionType' 'struct' template that returns
// 'true'.  Any other instantiations of that 'struct' template return 'false'.

MATCH_OPTION_TYPE(Ot::e_BOOL,     bool)

MATCH_OPTION_TYPE_PAIR(Ot::e_CHAR,     char)
MATCH_OPTION_TYPE_PAIR(Ot::e_INT,      int)
MATCH_OPTION_TYPE_PAIR(Ot::e_INT64,    bsls::Types::Int64)
MATCH_OPTION_TYPE_PAIR(Ot::e_DOUBLE,   double)
MATCH_OPTION_TYPE_PAIR(Ot::e_STRING,   bsl::string)
MATCH_OPTION_TYPE_PAIR(Ot::e_DATETIME, bdlt::Datetime)
MATCH_OPTION_TYPE_PAIR(Ot::e_DATE,     bdlt::Date)
MATCH_OPTION_TYPE_PAIR(Ot::e_TIME,     bdlt::Time)

#undef MATCH_OPTION_TYPE
#undef MATCH_OPTION_TYPE_PAIR

template <int ELEM_TYPE, class TYPE>
bool checkOptionType(TYPE *optionTypeValue)
    // Return 'true' if the specified 'optionTypeValue' is a null pointer whose
    // parameterized 'TYPE' matches the 'balcl::OptionType' described by the
    // parameterized 'ELEM_TYPE' constant.
{
    CheckOptionType<ELEM_TYPE, TYPE> checker;
    return checker() && (TYPE *)0 == optionTypeValue;
}

// BDE_VERIFY pragma: +FABC01  // Function ... not in alphabetic order

void setOptionValue(OptionValue *dst, const void *src, ElemType type)
    // Set the value at the specified 'dst' to the value found at the specified
    // 'src' that is of the specified 'type'.  The behavior is undefined unless
    // 'src' can be cast to a pointer of
    // 'balcl::OptionType::EnumToType<type>::type',
    // 'balcl::OptionType::e_VOID != type', and 'dst->type() == type'.
{
    BSLS_ASSERT(dst);
    BSLS_ASSERT(src);
    BSLS_ASSERT(Ot::e_VOID != type);
    BSLS_ASSERT(dst->type()        == type);

#define CASE(ENUM)                                                            \
    case ENUM: {                                                              \
      dst->set(*(static_cast<const Ot::EnumToType<ENUM>::type *>(src)));      \
    } break;                                                                  \

    switch (type) {
      case Ot::e_VOID: {
        BSLS_ASSERT(!"reachable");
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
    }
#undef CASE
}

// BDE_VERIFY pragma: +AR01  // Type using allocator is returned by value

                        // ============================
                        // function parseOccurrenceInfo
                        // ============================

int parseOccurrenceInfo(const char **endpos,
                        const Obj&   occurrenceInfo,
                        const char  *input)
    // Parse the specified 'input' for a value and verify that this value
    // matches the specified 'occurrenceInfo' and return 0 if parsing and
    // verification succeed.  Return a non-zero value if parsing fails or if
    // the value parsed does not match 'occurrenceInfo', and return in the
    // specified 'endpos' the first unsuccessful parsing position.
{
    enum { SUCCESS = 0, FAILURE = -1 };

    if (skipWhiteSpace(&input, input)) {
        *endpos = input;
        return FAILURE;                                               // RETURN
    }
    if (*input != '{') {
        if (Obj::e_REQUIRED == occurrenceInfo.occurrenceType()) {
            if (skipRequiredToken(endpos, input, "REQUIRED")) {
                return FAILURE;                                       // RETURN
            }
            return SUCCESS;                                           // RETURN
        }
        if (Obj::e_OPTIONAL == occurrenceInfo.occurrenceType()) {
            if (skipRequiredToken(endpos, input, "OPTIONAL")) {
                return FAILURE;                                       // RETURN
            }
            return SUCCESS;                                           // RETURN
        }
        if (Obj::e_HIDDEN == occurrenceInfo.occurrenceType()) {
            if (skipRequiredToken(endpos, input, "HIDDEN")) {
                return FAILURE;                                       // RETURN
            }
            return SUCCESS;                                           // RETURN
        }
        *endpos = input;
        return FAILURE;                                               // RETURN
    } else {
        ++input;
        if (Obj::e_OPTIONAL == occurrenceInfo.occurrenceType()) {
            if (skipWhiteSpace   (&input, input)
             || skipRequiredToken(&input, input, "OPTIONAL")) {
                *endpos = input;
                return FAILURE;                                       // RETURN
            }
        } else {
            if (skipWhiteSpace   (&input, input)
             || skipRequiredToken(&input, input, "HIDDEN")) {
                *endpos = input;
                return FAILURE;                                       // RETURN
            }
        }
        if (occurrenceInfo.hasDefaultValue()) {
            if (skipWhiteSpace   (&input, input)
             || skipRequiredToken(&input, input, "DEFAULT_TYPE")) {
                *endpos = input;
                return FAILURE;                                       // RETURN
            }
            if (skipWhiteSpace   (&input, input)
             || skipRequiredToken(&input, input,
                          Ot::toAscii(occurrenceInfo.defaultValue().type()))) {
                *endpos = input;
                return FAILURE;                                       // RETURN
            }
            if (skipWhiteSpace   (&input, input)
             || skipRequiredToken(&input, input, "DEFAULT_VALUE")
             || skipWhiteSpace   (&input, input)) {
                *endpos = input;
                return FAILURE;                                       // RETURN
            }
            switch (occurrenceInfo.defaultValue().type()) {
              case Ot::e_CHAR: {
                char charValue = *input;
                if (charValue !=
                               occurrenceInfo.defaultValue().the<Ot::Char>()) {
                    return FAILURE;                                   // RETURN
                }
                *endpos = ++input;
              } break;
              case Ot::e_INT: {
                int intValue;
                if (u::TypesParser::parse(&intValue, endpos, input)
                 || intValue != occurrenceInfo.defaultValue().the<Ot::Int>()) {
                    return FAILURE;                                   // RETURN
                }
              } break;
              case Ot::e_INT64: {
                Int64 int64Value;
                if (u::TypesParser::parse(&int64Value, endpos, input)
                 || int64Value !=
                              occurrenceInfo.defaultValue().the<Ot::Int64>()) {
                    return FAILURE;                                   // RETURN
                }
              } break;
              case Ot::e_DOUBLE: {
                double doubleValue;
                if (u::TypesParser::parse(&doubleValue, endpos, input)
                 || doubleValue !=
                             occurrenceInfo.defaultValue().the<Ot::Double>()) {
                    // There is no guaranteed round-trip on floating point I/O.
                    // Return 'FAILURE'.
                }
              } break;
              case Ot::e_STRING: {
                bsl::string stringValue;
                if (u::TypesParser::parse(&stringValue, endpos, input)
                 || stringValue !=
                             occurrenceInfo.defaultValue().the<Ot::String>()) {
                    return FAILURE;                                   // RETURN
                }
              } break;
              case Ot::e_DATETIME: {
                bdlt::Datetime datetimeValue;
                if (u::TypesParser::parse(&datetimeValue, endpos, input)
                 || datetimeValue !=
                           occurrenceInfo.defaultValue().the<Ot::Datetime>()) {
                    // Incomprehensibly, 'DATETIME' isn't a round trip for I/O,
                    // because 'DATE' isn't either (see below).
                    if (0 == (*endpos = bsl::strchr(input, '}'))) {
                        return FAILURE;                               // RETURN
                    }
                }
              } break;
              case Ot::e_DATE: {
                bdlt::Date dateValue;
                if (u::TypesParser::parse(&dateValue, endpos, input)
                 || dateValue !=
                               occurrenceInfo.defaultValue().the<Ot::Date>()) {
                    // Incomprehensibly, 'DATE' isn't a round trip for I/O, it
                    // prints as, e.g., 06APR1234, but wants to be parsed as
                    // 1234-04-06.  Go figure...
                    if (0 == (*endpos = bsl::strchr(input, '}'))) {
                        return FAILURE;                               // RETURN
                    }
                }
              } break;
              case Ot::e_TIME: {
                bdlt::Time timeValue;
                if (u::TypesParser::parse(&timeValue, endpos, input)
                 || timeValue !=
                               occurrenceInfo.defaultValue().the<Ot::Time>()) {
                    return FAILURE;                                   // RETURN
                }
              } break;
              case Ot::e_CHAR_ARRAY: {
                bsl::string charArrayAsString;
                if (u::TypesParser::parseQuotedString(
                                          &charArrayAsString, endpos, input)) {
                    // 'CHAR_ARRAY' isn't round trip for I/O, in fact, printing
                    // a 'CHAR_ARRAY' amounts to printing a string delimited by
                    // double quotes, whereas parsing requires parsing a
                    // sequence of white-separated 'CHAR's enclosed by '[' ']'.
                    // Use 'parseQuotedString' instead.
                    return FAILURE - 1;                               // RETURN
                }
                bsl::vector<char> charArrayValue(charArrayAsString.begin(),
                                                 charArrayAsString.end());
                if (charArrayValue !=
                          occurrenceInfo.defaultValue().the<Ot::CharArray>()) {
                    return FAILURE - 2;                               // RETURN
                }
              } break;
              case Ot::e_INT_ARRAY: {
                bsl::vector<int> intArrayValue;
                if (ArrayParserImpUtil::parse(&intArrayValue, endpos, input)
                 || intArrayValue !=
                           occurrenceInfo.defaultValue().the<Ot::IntArray>()) {
                    return FAILURE;                                   // RETURN
                }
              } break;
              case Ot::e_INT64_ARRAY: {
                bsl::vector<Int64> int64ArrayValue;
                if (ArrayParserImpUtil::parse(&int64ArrayValue, endpos, input)
                 || int64ArrayValue !=
                         occurrenceInfo.defaultValue().the<Ot::Int64Array>()) {
                    return FAILURE;                                   // RETURN
                }
              } break;
              case Ot::e_DOUBLE_ARRAY: {
                bsl::vector<double> doubleArrayValue;
                if (ArrayParserImpUtil::parse(&doubleArrayValue, endpos, input)
                 || doubleArrayValue !=
                        occurrenceInfo.defaultValue().the<Ot::DoubleArray>()) {
                    // There is no guaranteed round-trip on floating point I/O.
                }
              } break;
              case Ot::e_STRING_ARRAY: {
                bsl::vector<bsl::string> stringArrayValue;
                if (ArrayParserImpUtil::parse(&stringArrayValue, endpos, input)
                 || stringArrayValue !=
                        occurrenceInfo.defaultValue().the<Ot::StringArray>()) {
                    return FAILURE;                                   // RETURN
                }
              } break;
              case Ot::e_DATETIME_ARRAY: {
                bsl::vector<bdlt::Datetime> datetimeArrayValue;
                if (ArrayParserImpUtil::parse(&datetimeArrayValue,
                                              endpos,
                                              input)
                 || datetimeArrayValue !=
                      occurrenceInfo.defaultValue().the<Ot::DatetimeArray>()) {
                    // Incomprehensibly, ... see 'DATETIME' above.

                    if ( 0 == (*endpos = bsl::strchr(input, '}'))) {
                        return FAILURE;                               // RETURN
                    }
                }
              } break;
              case Ot::e_DATE_ARRAY: {
                bsl::vector<bdlt::Date> dateArrayValue;
                if (ArrayParserImpUtil::parse(&dateArrayValue, endpos, input)
                 || dateArrayValue !=
                          occurrenceInfo.defaultValue().the<Ot::DateArray>()) {
                    // Incomprehensibly, ... see 'DATETIME' above.

                    if ( 0 == (*endpos = bsl::strchr(input, '}'))) {
                        return FAILURE;                               // RETURN
                    }
                }
              } break;
              case Ot::e_TIME_ARRAY: {
                bsl::vector<bdlt::Time> timeArrayValue;
                if (ArrayParserImpUtil::parse(&timeArrayValue, endpos, input)
                 || timeArrayValue !=
                          occurrenceInfo.defaultValue().the<Ot::TimeArray>()) {
                    return FAILURE;                                   // RETURN
                }
              } break;
              default: {
                ASSERT(0);
              } break;
            };
        }
        input = *endpos;  // catch up with previous parsing
        if (skipWhiteSpace   (&input, input)
         || skipRequiredToken(&input, input, "}")) {
            *endpos = input;
            return FAILURE;                                           // RETURN
        }
    }

    *endpos = input;
    return SUCCESS;
}

bool isAllocatingType(ElemType type)
    // Return 'true' if the specified 'type' is an allocating type, and 'false'
    // otherwise.
{
    return Ot::e_STRING == type || Ot::isArrayType(type);
}

template <Ot::Enum ELEM_TYPE>
void checkCtor(const Obj&  reference,
               const void *value,
               bool        veryVerbose,
               bool        veryVeryVeryVerbose)
    // Set a test failure if any of three temporary 'Obj's created by this
    // function do not equal the specified 'reference' object, or if any of
    // those temporary objects do not have their expected allocator.  Each of
    // the temporary objects is created using the value found at the specified
    // 'value' in concert with the (default) value constructor for 'ELEM_TYPE',
    // the difference being the way the object allocator is specified:
    //: 1 no specified allocator
    //: 2 0-specified allocator
    //: 3 an explicitly specified allocator
    // Log each temporary object creation if the specified 'veryVerbose' is
    // set.  Put the allocators used in trace mode if the specified
    // 'veryVeryVeryVerbose' is set.
{
    for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
        const char CONFIG = cfg;  // how we specify the allocator

        if (veryVerbose) {
            T_ P(CONFIG);
        }

        bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
        bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
        bslma::TestAllocator da("default",   veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        Obj                  *objPtr           = 0;
        bslma::TestAllocator *objAllocatorPtr  = 0;

        switch (CONFIG) {
          case 'a': {
            objAllocatorPtr = &da;
          } break;
          case 'b': {
            objAllocatorPtr = &da;
          } break;
          case 'c': {
            objAllocatorPtr = &sa;
          } break;
          default: {
            ASSERTV(CONFIG, !"Bad allocator config.");
          } break;
        }

        ASSERTV(CONFIG, objAllocatorPtr);

        bslma::TestAllocator&  oa = *objAllocatorPtr;
        bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

        bslma::TestAllocatorMonitor  oam( &oa);
        bslma::TestAllocatorMonitor noam(&noa);

        typedef const typename Ot::EnumToType<ELEM_TYPE>::type *ValuePtr;

        switch (CONFIG) {
          case 'a': {
            objPtr = new (fa) Obj(*static_cast<ValuePtr>(value));    // ACTION
          } break;
          case 'b': {
            objPtr = new (fa) Obj(*static_cast<ValuePtr>(value), 0); // ACTION
          } break;
          case 'c': {
            objPtr = new (fa) Obj(*static_cast<ValuePtr>(value), &sa);
                                                                      // ACTION
          } break;
          default: {
            ASSERTV(CONFIG, !"Bad allocator config.");
          } break;
        }

        if (u::isAllocatingType(ELEM_TYPE)) {
            ASSERTV(CONFIG, oam.isTotalUp());
        }

        ASSERTV(CONFIG, objPtr);
        Obj&  mX = *objPtr;  const Obj& X = mX;

        ASSERTV(CONFIG, &oa       == X.allocator());
        ASSERTV(CONFIG, reference == X);

        fa.deleteObject(objPtr);  // Clean up

        ASSERTV(CONFIG,  oam.isInUseSame());
        ASSERTV(CONFIG, noam.isInUseSame());
    }
}

}  // close namespace u
}  // close unnamed namespace

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, const char *argv[])
{
    int                test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int             verbose = argc > 2;
    int         veryVerbose = argc > 3;
    int     veryVeryVerbose = argc > 4; (void) veryVeryVerbose;
    int veryVeryVeryVerbose = argc > 5;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    bslma::TestAllocator globalAllocator(veryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 8: {
        // --------------------------------------------------------------------
        // DEFAULT CONSTRUCTORS
        //
        // Concerns:
        //: 1 Objects created using the default constructors have the expected
        //:   value and use the expected allocators.
        //
        // Plan:
        //: 1 Ad-hoc tests.
        //
        // Testing:
        //   OccurrenceInfo();
        //   OccurrenceInfo(bslma::Allocator *basicAllocator);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "DEFAULT CONSTRUCTORS" << endl
                          << "====================" << endl;

        bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
        bslma::TestAllocator da("default",   veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        Obj mX; const Obj& X = mX;

        ASSERT(false      == X.isRequired());
        ASSERT(false      == X.isHidden());
        ASSERT(false      == X.defaultValue().hasNonVoidType());
        ASSERT(Ot::e_VOID == X.defaultValue().type());
        ASSERT(&da        == X.defaultValue().allocator());
        ASSERT(&da        == X.allocator());

        Obj mY(&sa); const Obj& Y = mY;
        ASSERT(false      == Y.isRequired());
        ASSERT(false      == Y.isHidden());
        ASSERT(false      == Y.defaultValue().hasNonVoidType());
        ASSERT(Ot::e_VOID == Y.defaultValue().type());
        ASSERT(&sa        == Y.defaultValue().allocator());
        ASSERT(&sa        == Y.allocator());

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // VALUE CONSTRUCTORS
        //
        // Concerns:
        //: 1 Each of the (default) value constructors can be invoked with
        //:   three different ways of specifying the object's allocator:
        //:   1 no specification (default allocator)
        //:   2 0-specification  (default allocator)
        //:   3 an explicit specification
        //:
        //: 2 The object created by the (default) value constructors is equal
        //:   to the object created using the "type" constructor and then
        //:   using the 'setDefaultValue' method.
        //
        // Plan:
        //: 1 For each entry in the 'OPTION_DEFAULT_VALUES' table except for
        //:   'Ot::e_BOOL' (which is disallowed default values), create a
        //:   "reference" object by using the "type" constructor and then
        //:   calling 'setDefaultValue'.
        //:
        //: 2 Call the 'u::checkCtor' helper function for the object's type.
        //:   That function creates three temporary objects (using no
        //:   specification, 0-specification, and explicit allocator
        //:   specification) and compares each to the reference object.  The
        //:   function also confirms that each object has the expected
        //:   allocator.
        //
        // Testing:
        //   OccurrenceInfo(char                    dflt, *bA = 0);
        //   OccurrenceInfo(int                     dflt, *bA = 0);
        //   OccurrenceInfo(Int64                   dflt, *bA = 0);
        //   OccurrenceInfo(double                  dflt, *bA = 0);
        //   OccurrenceInfo(const string&           dflt, *bA = 0);
        //   OccurrenceInfo(const Datetime&         dflt, *bA = 0);
        //   OccurrenceInfo(const Date&             dflt, *bA = 0);
        //   OccurrenceInfo(const Time&             dflt, *bA = 0);
        //   OccurrenceInfo(const vector<char>&     dflt, *bA = 0);
        //   OccurrenceInfo(const vector<int>&      dflt, *bA = 0);
        //   OccurrenceInfo(const vector<Int64>&    dflt, *bA = 0);
        //   OccurrenceInfo(const vector<double>&   dflt, *bA = 0);
        //   OccurrenceInfo(const vector<string>&   dflt, *bA = 0);
        //   OccurrenceInfo(const vector<Datetime>& dflt, *bA = 0);
        //   OccurrenceInfo(const vector<Date>&     dflt, *bA = 0);
        //   OccurrenceInfo(const vector<Time>&     dflt, *bA = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "VALUE CONSTRUCTORS" << endl
                          << "==================" << endl;

        for (int j = 0; j < NUM_OPTION_DEFAULT_VALUES; ++j) {

            const int       LINE1   = OPTION_OCCURRENCES[1].d_line;
            const OccurType OTYPE   = Obj::e_OPTIONAL;

            const int       LINE2   = OPTION_DEFAULT_VALUES[j].d_line;
            const ElemType  ETYPE   = OPTION_DEFAULT_VALUES[j].d_type;
            const void     *ADDRESS = OPTION_DEFAULT_VALUES[j].d_value_p;

            if (veryVerbose) { T_ P_(LINE1) P(LINE2) }

            Obj mX(OTYPE);  const Obj& X = mX;

            if (OTYPE != Obj::e_REQUIRED && ADDRESS) {
                OptionValue DEFAULT_VALUE(ETYPE);
                u::setOptionValue(&DEFAULT_VALUE, ADDRESS, ETYPE);

                mX.setDefaultValue(DEFAULT_VALUE);
            }


            if (ADDRESS) {

#define CASE(ENUM)                                                            \
case ENUM: {                                                                  \
  u::checkCtor<ENUM>(X, ADDRESS, veryVerbose, veryVeryVeryVerbose);           \
} break;                                                                      \

                switch (ETYPE) {
                  case Ot::e_VOID: {
                    BSLS_ASSERT(!"reachable");
                  } break;
                  case Ot::e_BOOL: {
                    BSLS_ASSERT(!"reachable");
                  } break;
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
                }
#undef CASE
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // COPY ASSIGNMENT
        //
        // Concerns:
        //: 1 The operator has the expected signature.
        //:
        //: 2 One object can be assigned to another irrespective of the value
        //:   of each of those objects.
        //:
        //: 3 Alias-safety: An object an be assigned to itself.
        //:
        //: 4 The allocator of the assigned-to object ('lhs') is preserved.
        //:
        //: 5 The assignment operation returns a reference to the 'lhs' object.
        //:
        //: 6 The operation does not change the 'rhs'.
        //
        // Plan:
        //: 1 Use the "pointer-to-method" idiom to have the compiler check the
        //:   signature.  (C-1)
        //:
        //: 2 For a representative set of objects (see the 'OPTION_TYPEINFO'
        //:   table), assign each object with itself and to every other object.
        //:   Use equality comparison to confirm that each object is in the
        //:   expected state afterward.  (C-2)
        //:
        //: 3 Use an ad hoc test to assign one object to another that is using
        //:   a different allocator.  Confirm that the 'lhs' object retains its
        //:   original allocator.  (C-3)
        //:
        //: 4 Use 'bslma::TestAllocatorMonitor' objects to confirm that no
        //:   memory is allocated.  (C-4)
        //:
        //: 5 Compare the address of the returned value (a reference, as shown
        //:   in P-1) to the address of the 'lhs'.  (C-5)
        //:
        //: 6 Create a duplicate of the 'rhs' object that is not used in the
        //:   assignment operation.  Confirm that the 'rhs' compares equal to
        //:   this spare object both before and after the assignment operation.
        //
        // Testing:
        //   OccurrenceInfo& operator=(const Oi& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "COPY ASSIGNMENT" << endl
                                  << "===============" << endl;

        if (verbose) cout
                   << endl
                   << "Verify that the signature and return type are standard."
                   << endl;
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            (void)operatorAssignment;  // quash potential compiler warning
        }

        if (verbose) cout << endl << "Testing assignment operator." << endl;

// BDE_VERIFY pragma: -TP21    // Loops must contain very verbose action
// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
        for (int i = 0; i < NUM_OPTION_OCCURRENCES;    ++i) {
        for (int j = 0; j < NUM_OPTION_DEFAULT_VALUES; ++j) {

            const int       LINE1    = OPTION_OCCURRENCES[i].d_line;
            const OccurType OTYPE1   = OPTION_OCCURRENCES[i].d_type;

            const int       LINE2    = OPTION_DEFAULT_VALUES[j].d_line;
            const ElemType  ETYPE2   = OPTION_DEFAULT_VALUES[j].d_type;
            const void     *ADDRESS1 = OPTION_DEFAULT_VALUES[j].d_value_p;

            if (veryVerbose) { T_ P_(LINE1) P_(OTYPE1)
                                  P_(LINE2)  P(ETYPE2) }

            Obj mX(OTYPE1);  const Obj& X = mX;
            Obj mZ(OTYPE1);  const Obj& Z = mZ;

            bslma::Allocator *xa = X.allocator();

            if (OTYPE1 != Obj::e_REQUIRED && ADDRESS1) {
                OptionValue DEFAULT_VALUE1(ETYPE2);
                u::setOptionValue(&DEFAULT_VALUE1, ADDRESS1, ETYPE2);

                mX.setDefaultValue(DEFAULT_VALUE1);
                mZ.setDefaultValue(DEFAULT_VALUE1);
            }

            // Testing self-assignment (alias safety).

            Obj *mR = &(mX = X);  // ACTION

            // Vet "assigned-to" object.

            ASSERT(mR == &mX);
            ASSERT(Z  == X);
            ASSERT(xa == X.allocator());

            for (int k = 0; k < NUM_OPTION_OCCURRENCES;    ++k) {
            for (int l = 0; l < NUM_OPTION_DEFAULT_VALUES; ++l) {

                const int       LINE3    = OPTION_OCCURRENCES[k].d_line;
                const OccurType OTYPE3   = OPTION_OCCURRENCES[k].d_type;

                const int       LINE4    = OPTION_DEFAULT_VALUES[l].d_line;
                const ElemType  ETYPE4   = OPTION_DEFAULT_VALUES[l].d_type;
                const void     *ADDRESS2 = OPTION_DEFAULT_VALUES[l].d_value_p;

                if (veryVerbose) { T_ P_(LINE3) P_(OTYPE3)
                                      P_(LINE4)  P(ETYPE4) }

                bslma::TestAllocator saX("suppliedX", veryVeryVeryVerbose);
                bslma::TestAllocator saY("suppliedY", veryVeryVeryVerbose);

                Obj mX(OTYPE1, &saX);  const Obj& X = mX;
                Obj mZ(OTYPE1, &saX);  const Obj& Z = mZ;

                Obj mY(OTYPE3, &saY);  const Obj& Y = mY;

                if (OTYPE1 != Obj::e_REQUIRED && ADDRESS1) {
                    OptionValue DEFAULT_VALUE1(ETYPE2);
                    u::setOptionValue(&DEFAULT_VALUE1, ADDRESS1, ETYPE2);

                    mX.setDefaultValue(DEFAULT_VALUE1);
                    mZ.setDefaultValue(DEFAULT_VALUE1);
                }

                if (OTYPE3 != Obj::e_REQUIRED && ADDRESS2) {
                    OptionValue DEFAULT_VALUE2(ETYPE4);
                    u::setOptionValue(&DEFAULT_VALUE2, ADDRESS2, ETYPE4);

                    mY.setDefaultValue(DEFAULT_VALUE2);
                }

                Obj *mR = &(mY = X);  // ACTION

                // Vet results

                ASSERTV(LINE1, LINE2, LINE3, LINE4, mR   == &mY);
                ASSERTV(LINE1, LINE2, LINE3, LINE4, X    == Y);
                ASSERTV(LINE1, LINE2, LINE3, LINE4, &saY == Y.allocator());
                ASSERTV(LINE1, LINE2, LINE3, LINE4, X    == Z);
                ASSERTV(LINE1, LINE2, LINE3, LINE4, X.allocator()
                                                         == Z.allocator());
            }
            }
        }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        //
        // Concerns:
        //: 1 Equality: The copy constructor creates a new object that compares
        //:   equal to the original object.
        //
        //: 2 Allocators:
        //:   1 The allocator of the created object depends on its constructor
        //:     argument (not the allocator of the original object).
        //:
        //:   2 If the allocator argument of the object is 0 or not specified,
        //:     the new object uses the default allocator, otherwise, the
        //:     specified allocator is used.
        //:
        //:   3 The class is exception safe.
        //
        // Plans:
        //: 1 Do table-driven testing using the 'OPTION_TYPEINFO' array of
        //:   representative values described in earlier tests.
        //:
        //: 2 Use 'operator==' to confirm the equality of the new object (C-1).
        //:
        //: 3 Repeat each test for the cases of an unspecified allocator, a
        //:   0-allocator, and an explicitly supplied allocator.  (C-2.1)
        //
        //:   1 Confirm the allocator of the new object using the 'allocator'
        //:     accessor.  (C-2.2)
        //:
        //:   2 Use 'bslma::TestAllocatorMonitor' objects to confirm that
        //:     memory allocation, when expected, occurs in the expected
        //:     allocator, and no allocation occurs from the "other" allocator.
        //:
        //:   3 The only allocating member is of type 'balcl::OptionValue'
        //:     (used to hold the default option value).  That type was
        //:     shown exception safe in its test driver.  (C-2.3)
        //
        // Testing:
        //   OccurrenceInfo(const Oi& original, *bA = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;

// BDE_VERIFY pragma: -TP21    // Loops must contain very verbose action
// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
        for (int i = 0; i < NUM_OPTION_OCCURRENCES;    ++i) {
        for (int j = 0; j < NUM_OPTION_DEFAULT_VALUES; ++j) {

            const int       LINE1   = OPTION_OCCURRENCES[i].d_line;
            const OccurType OTYPE   = OPTION_OCCURRENCES[i].d_type;

            const int       LINE2   = OPTION_DEFAULT_VALUES[j].d_line;
            const ElemType  ETYPE   = OPTION_DEFAULT_VALUES[j].d_type;
            const void     *ADDRESS = OPTION_DEFAULT_VALUES[j].d_value_p;

            if (veryVerbose) { T_ P_(LINE1) P_(OTYPE)
                                  P_(LINE2)  P(ETYPE) }

            bslma::TestAllocator         da("default", veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(OTYPE);  const Obj& X = mX;

            if (OTYPE != Obj::e_REQUIRED && ADDRESS) {
                OptionValue DEFAULT_VALUE(ETYPE);
                u::setOptionValue(&DEFAULT_VALUE, ADDRESS, ETYPE);

                mX.setDefaultValue(DEFAULT_VALUE);
            }

            for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
                const char CONFIG = cfg;  // how we specify the allocator

                if (veryVerbose) {
                    T_ T_ P(CONFIG)
                }

                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj                  *objPtr           = 0;
                bslma::TestAllocator *objAllocatorPtr  = 0;

                switch (CONFIG) {
                  case 'a': {
                    objAllocatorPtr = &da;
                  } break;
                  case 'b': {
                    objAllocatorPtr = &da;
                  } break;
                  case 'c': {
                    objAllocatorPtr = &sa;
                  } break;
                  default: {
                    ASSERTV(CONFIG, !"Bad allocator config.");
                  } break;
                }

                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

                bslma::TestAllocatorMonitor  oam(&oa);
                bslma::TestAllocatorMonitor noam(&noa);

                switch (CONFIG) {
                  case 'a': {
                    objPtr = new (fa) Obj(X);       // ACTION
                  } break;
                  case 'b': {
                    objPtr = new (fa) Obj(X, 0);    // ACTION
                  } break;
                  case 'c': {
                    objPtr = new (fa) Obj(X, &sa);  // ACTION
                  } break;
                  default: {
                    ASSERTV(CONFIG, !"Bad allocator config.");
                  } break;
                }

                // Check expected allocator usage.

                if (Obj::e_REQUIRED != OTYPE && u::isAllocatingType(ETYPE)) {
                  ASSERTV(LINE1, LINE2, CONFIG, oam.isTotalUp());
                }
                ASSERTV(LINE1, LINE2, CONFIG, noam.isTotalSame());

                // Vet copied object.

                Obj&  mY = *objPtr;  const Obj& Y = mY;

                ASSERTV(LINE1, LINE2, CONFIG, X   == Y);
                ASSERTV(LINE1, LINE2, CONFIG, &oa == Y.allocator());

                fa.deleteObject(objPtr); // Clean up
            }
        }
        }

// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line
// BDE_VERIFY pragma: +TP21    // Loops must contain very verbose action

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //
        // Concerns:
        //: 1 Salient Members:
        //:
        //:   1 Two objects, 'X' and 'Y', compare equal if and only if each of
        //:     their corresponding salient attributes respectively compare
        //:     equal.
        //:
        //:   2 All salient attributes participate in the comparison.
        //
        //:   3 The object's allocator is not salient.
        //:
        //: 2 Mathematical Properties:
        //:
        //:   1 The operators provide the property of identity:
        //:     o 'true  == (X == X)'
        //:     o 'false == (X != X)'
        //:
        //:   2 The operators provide the property of commutativity:
        //:     o 'X == Y' if and only if 'Y == X'
        //:     o 'X != Y' if and only if 'Y != X'
        //:
        //:   3 Each of these two operators is the inverse of the other:
        //:     o 'X != Y' if and only if '!(X == Y)'
        //:
        //: 3 Non-modifiable objects can be compared (i.e., 'const' objects and
        //:   'const' references).
        //:
        //: 4 The two operators have standard signatures and return types.
        //
        //: 5 No memory allocation occurs as a result of comparison (e.g., the
        //:   arguments are not passed by value).
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free
        //:   equality-comparison operators defined in this component.
        //:   (C-3..4)
        //:
        //: 2 Using the tables 'OPTION_OCCURRENCES' and 'OPTION_DEFAULT_VALUES'
        //:   (see {Input Tables}), create a series of test-object pairs, that
        //:   serve as operands to 'operator==' and 'operator!='.  Since each
        //:   row of the two input tables is unique, equality is expected when
        //:   both objects of a pair happened to be created from the same table
        //:   rows (i.e., same type and same default value), with the exception
        //:   of objects having the 'Obj::e_REQUIRED' type.  Objects of that
        //:   type are disallowed default values so row agreement in table
        //:   'OPTION_OCCURRENCES' suffices for equality.  (C-1.1..2)
        //:
        //: 3 For each in the series, check for equality (and failure of
        //:   inequality) when compared to itself.  (C-2.1)
        //:
        //: 4 For each test of equality between two distinct objects, create a
        //:   parallel test that checks inequality (the inverse operator), and
        //:   (when the two arguments are different) also create a test case
        //:   where the two arguments are switched (showing commutativity).
        //:   (C-2.2..3)
        //:
        //: 5 Install a test allocator as the default allocator.  Create a test
        //:   allocator monitor object before each group of operator tests and
        //:   confirm afterwards that the 'isTotalSame' returns 'true' (showing
        //:   that no allocations occurred when exercising the operators).
        //:   (C-5)
        //:
        //: 6 Repeat each test between two objects so that the objects use the
        //:   same allocator in one test and use different allocators in the
        //:   other.  Results should not change and thereby show that the
        //:   object allocator is not salient to equality.  (C-1.3)
        //
        // Testing:
        //   bool operator==(const OccurrenceInfo& lhs, rhs)
        //   bool operator!=(const OccurrenceInfo& lhs, rhs)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EQUALITY-COMPARISON OPERATORS" << endl
                          << "=============================" << endl;

        if (verbose) cout <<
                "Check that signatures and return values are standard" << endl;
        {
            using namespace balcl;
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            // Verify that the signatures and return types are standard.

            operatorPtr operatorEq = operator==;
            operatorPtr operatorNe = operator!=;

            (void)operatorEq;  // Quash potential compiler warnings.
            (void)operatorNe;
        }

        if (verbose) cout << "\n\tTesting equality operator." << endl;

        if (verbose)
            cout << "\t\tCompare each pair of values (u, v) in W X W." << endl;

// BDE_VERIFY pragma: -TP21    // Loops must contain very verbose action
// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
        for (int i = 0; i < NUM_OPTION_OCCURRENCES;    ++i) {
        for (int j = 0; j < NUM_OPTION_DEFAULT_VALUES; ++j) {

            const int       LINE1    = OPTION_OCCURRENCES[i].d_line;
            const OccurType OTYPE1   = OPTION_OCCURRENCES[i].d_type;

            const int       LINE2    = OPTION_DEFAULT_VALUES[j].d_line;
            const ElemType  ETYPE2   = OPTION_DEFAULT_VALUES[j].d_type;
            const void     *ADDRESS1 = OPTION_DEFAULT_VALUES[j].d_value_p;

            if (veryVerbose) { T_ P_(LINE1) P_(OTYPE1)
                                  P_(LINE2)  P(ETYPE2) }

            bslma::TestAllocator sa1("supplied1", veryVeryVeryVerbose);
            bslma::TestAllocator sa2("supplied2", veryVeryVeryVerbose);

            bslma::TestAllocator         da("default", veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&da);

            bslma::TestAllocator *saX = &sa1;

            Obj mX(OTYPE1, saX);  const Obj& X = mX;

            if (OTYPE1 != Obj::e_REQUIRED && ADDRESS1) {
                OptionValue DEFAULT_VALUE1(ETYPE2);
                u::setOptionValue(&DEFAULT_VALUE1, ADDRESS1, ETYPE2);

                mX.setDefaultValue(DEFAULT_VALUE1);
            }

            bslma::TestAllocatorMonitor dam (&da);
            bslma::TestAllocatorMonitor samX(saX);

            ASSERTV(LINE1,  (X == X));   // ACTION
            ASSERTV(LINE1, !(X != X));   // ACTION

            ASSERTV(LINE1, dam .isTotalSame());
            ASSERTV(LINE1, samX.isTotalSame());

            for (int k = 0; k < NUM_OPTION_OCCURRENCES;    ++k) {
            for (int l = 0; l < NUM_OPTION_DEFAULT_VALUES; ++l) {

                const int       LINE3    = OPTION_OCCURRENCES[k].d_line;
                const OccurType OTYPE3   = OPTION_OCCURRENCES[k].d_type;

                const int       LINE4    = OPTION_DEFAULT_VALUES[l].d_line;
                const ElemType  ETYPE4   = OPTION_DEFAULT_VALUES[l].d_type;
                const void     *ADDRESS2 = OPTION_DEFAULT_VALUES[l].d_value_p;

                if (veryVerbose) { T_ P_(LINE3) P_(OTYPE3)
                                      P_(LINE4)  P(ETYPE4) }

                for (int m = 0; m < 2; ++m) {
                    if (veryVerbose) { T_ T_ T_  P(k) }

                    bslma::TestAllocator *saY = m % 2
                                              ? &sa1  //     same as 'X'
                                              : &sa2; // not same as 'X'

                    Obj mY(OTYPE3, saY);  const Obj& Y = mY;

                    if (OTYPE3 != Obj::e_REQUIRED && ADDRESS2) {
                        OptionValue DEFAULT_VALUE2(ETYPE4);
                        u::setOptionValue(&DEFAULT_VALUE2, ADDRESS2, ETYPE4);

                        mY.setDefaultValue(DEFAULT_VALUE2);
                    }

                    bool isSame = (OTYPE1 != Obj::e_REQUIRED &&
                                   OTYPE3 != Obj::e_REQUIRED)
                                ? i == k && j == l
                                : i == k
                                ;
         //v--------^
           bslma::TestAllocatorMonitor dam (&da);
           bslma::TestAllocatorMonitor samY(saY);

           ASSERTV(LINE1, LINE2, LINE3, LINE4, isSame ==  (X == Y));  // ACTION
           ASSERTV(LINE1, LINE2, LINE3, LINE4, isSame == !(X != Y));  // ACTION
           ASSERTV(LINE1, LINE2, LINE3, LINE4, isSame ==  (Y == X));  // ACTION
           ASSERTV(LINE1, LINE2, LINE3, LINE4, isSame == !(Y != X));  // ACTION

           ASSERTV(LINE1, LINE2, LINE3, LINE4, samY.isTotalSame());
           ASSERTV(LINE1, LINE2, LINE3, LINE4,  dam.isTotalSame());
         //^--------v
                }
            }
            }
        }
        }
// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line
// BDE_VERIFY pragma: +TP21    // Loops must contain very verbose action
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR
        //
        // Concerns:
        //: 1 The 'print' method writes to the specified 'ostream' in the
        //:   expected format.
        //:
        //: 2 The 'print' method produces the expected output for the given
        //:   object.
        //:
        //: 3 'operator<<' produces the same results as 'print' when level and
        //:   spaces-per-level arguments have their default value.
        //:
        //: 4 The return values of 'print' and 'operator<<' reference the
        //:   stream argument.
        //:
        //: 5 The signature and return types of 'print' and 'operator<<' are
        //:   standard.
        //:
        //: 6 Optional Arguments:
        //:
        //:   1 The 'print' method indents according to 'level' and
        //:     'spacesPerLevel'.
        //:
        //:   2 The default values for 'level' and 'spacesPerLevel' are 0 and
        //:     4, respectively.
        //:
        //: 7 The class has set the 'bdlb::HasPrintMethod' trait.
        //
        // Plan:
        //: 1 Use the "function address" idiom to confirm the signatures.
        //:   (C-5)
        //:
        //: 2 Confirm that 'bdlb::HasPrintMethod<OccurrenceInfo>::value' is
        //:   'true' using a compile-time assertion at file scope.  (C-7)
        //:
        //: 3 For the cross product of the two input tables (see {Input
        //:   Tables}):
        //:
        //:   1 Use 'bsl::ostringstream' objects to confirm that the output of
        //:     'print' (with default arguments) and 'operator<<' are
        //:     identical.  (C-3)
        //:
        //:   2 Use the 'u::parseOccurrenceInfo' helper function to confirm
        //:     that output of 'print' matches the expected output for the
        //:     given object.  (C-2)
        //:
        //:   3 Use 'bsl::strspn' to confirm that indentation matches 'level'
        //:     and 'spacesPerLevel' and that the default values are as
        //:     expected.  (C-6)
        //
        // Testing:
        //   ostream& print(ostream& stream, int level = 0, int spl = 4) const;
        //   operator<<(ostream& stream, const OccurrenceInfo& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRINT AND OUTPUT OPERATOR" << endl
                          << "=========================" << endl;

        if (verbose) cout
                 << "Verify that the signatures and return types are standard."
                 << endl;
        {
            using namespace balcl;
            typedef ostream& (Obj::*funcPtr)(ostream&, int, int) const;
            typedef ostream& (*operatorPtr)(ostream&, const Obj&);


            funcPtr     printMember = &Obj::print;
            operatorPtr operatorOp  = operator<<;

            (void)printMember;  // quash potential compiler warnings
            (void)operatorOp;
        }

        if (verbose)
            cout << "\n\tTesting output operator and 'print'." << endl;

// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
        for (int i = 0; i < NUM_OPTION_OCCURRENCES;    ++i) {
        for (int j = 0; j < NUM_OPTION_DEFAULT_VALUES; ++j) {

            const int       LINE1   = OPTION_OCCURRENCES[i].d_line;
            const OccurType OTYPE   = OPTION_OCCURRENCES[i].d_type;

            const int       LINE2   = OPTION_DEFAULT_VALUES[j].d_line;
            const ElemType  ETYPE   = OPTION_DEFAULT_VALUES[j].d_type;
            const void     *ADDRESS = OPTION_DEFAULT_VALUES[j].d_value_p;

            Obj mX(OTYPE);  const Obj& X = mX;

            if (OTYPE != Obj::e_REQUIRED && ADDRESS) {
                OptionValue DEFAULT_VALUE(ETYPE);
                u::setOptionValue(&DEFAULT_VALUE, ADDRESS, ETYPE);

                mX.setDefaultValue(DEFAULT_VALUE);
            }

            bsl::ostringstream ossMethod;
            bsl::ostringstream ossOperator;

            ASSERTV(LINE1, LINE2, &ossMethod   == &X.print(ossMethod));  // ACT
            ASSERTV(LINE1, LINE2, &ossOperator == &(ossOperator << X));  // ACT

            bsl::string stringMethod  (ossMethod  .str());
            bsl::string stringOperator(ossOperator.str());

            ASSERT(0 * 4 == bsl::strspn(stringMethod  .c_str(), " "));
            ASSERT(0 * 4 == bsl::strspn(stringOperator.c_str(), " "));

            ASSERTV(LINE1, LINE2, stringMethod,   stringOperator,
                                  stringMethod == stringOperator);

            if (veryVerbose) {
                T_ T_ P(X)
                T_ T_ P(stringMethod)
                T_ T_ P(stringOperator)
            }

            const char *output   = stringMethod.c_str();
            const int   parseRet = u::parseOccurrenceInfo(&output, X, output);
            LOOP2_ASSERT(parseRet, output, 0 == parseRet);

            bsl::ostringstream ossIndent1;
            bsl::ostringstream ossIndent2;

            X.print(ossIndent1, 2);    // ACTION
            X.print(ossIndent2, 1, 5); // ACTION

            bsl::string stringIndent1(ossIndent1.str());
            bsl::string stringIndent2(ossIndent2.str());

            ASSERT(2 * 4 == bsl::strspn(stringIndent1.c_str(), " "));
            ASSERT(1 * 5 == bsl::strspn(stringIndent2.c_str(), " "));
        }
        }
// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TYPE CTOR, PRIMARY MANIPULATORS, BASIC ACCESSORS & DTOR
        //
        // Concerns:
        //: 1 Each object created using the (occurrence) type constructor has
        //:   the expected attributes of occurrence type, default value, and
        //:   allocator.
        //:   1 The type constructor does not allocate.
        //:   2 The allocator can be specified, or not, or specified as 0.
        //:
        //: 2 The object allocates from the intended allocator, and no other.
        //:   1 Allocation is exception safe.
        //:   2 The 'bslma::UsesBslmaAllocator' trait is set for this class.
        //:
        //: 3 The basic accessors provide a view of the object state that is
        //:   consistent with the state of the object set by the constructor
        //:   and the primary manipulators.
        //:
        //: 4 The basic accessors are 'const'-qualified.
        //:
        //: 5 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using a table-driven organization, use the "footprint" idiom to
        //:   invoke the constructor specifying for the allocator: nothing, 0,
        //:   and a supplied allocator.  (C-1.2)
        //
        //:   1 In each scenario, the object uses a 'bslma::TestAllocator'
        //:     (explicitly, or via the currently installed default allocator).
        //:
        //:   2 Use a 'bslma::TestAllocatorMonitor' object to confirm that
        //:     allocation occurs when expected.  (C-1.1)
        //:
        //:   2 After creation, each object is subject to identical tests,
        //:     thereby demonstrating that each creation produces identical
        //:     results.  (C-3)
        //:
        //:   3 Confirm that
        //:     'bslma::UsesBslmaAllocator<balcl::TypeInfo>::value' is 'true'
        //:     in a compile-time assertion at file scope.  (C-2.2)
        //:
        //:   4 The implementation of 'Obj' does not allocate directly so there
        //:     is no need for exception testing other than confirmation that
        //:     the allocator is forwarded to the allocating subordinate types
        //:     that are assumed to be exception safe. (C-2.1)
        //:
        //: 2 Always invoke the basic accessors on a 'const'-reference to the
        //:   object under test.  Absence of compiler error confirms that the
        //:   accessors are 'const' qualified.  (C-4)
        //:
        //: 3 Use 'BSLS_ASSERTTEST_*' facilities for negative testing.  (C-5)
        //
        // Testing:
        //   OccurrenceInfo(OccurrenceType type, *bA = 0);
        //   ~OccurrenceInfo();
        //   void setDefaultValue(const OptionValue& defaultValue);
        //   void setHidden();
        //   const OptionValue& defaultValue() const;
        //   bool hasDefaultValue() const;
        //   bool isHidden() const;
        //   bool isRequired() const;
        //   OccurrenceType occurrenceType() const;
        //   bslma::Allocator *allocator() const;
        // --------------------------------------------------------------------

        if (verbose) cout
          << endl
          << "TYPE CTOR, PRIMARY MANIPULATORS, BASIC ACCESSORS & DTOR" << endl
          << "=======================================================" << endl;

        if (veryVerbose) cout << "Positive Testing" << endl;

// BDE_VERIFY pragma: -TP21    // Loops must contain very verbose action
// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line

//v-----^
  for (int i = 0; i < NUM_OPTION_OCCURRENCES;    ++i) {
  for (int j = 0; j < NUM_OPTION_DEFAULT_VALUES; ++j) {

      const int       LINE1   = OPTION_OCCURRENCES[i].d_line;
      const OccurType OTYPE   = OPTION_OCCURRENCES[i].d_type;

      const int       LINE2   = OPTION_DEFAULT_VALUES[j].d_line;
      const ElemType  ETYPE   = OPTION_DEFAULT_VALUES[j].d_type;
      const void     *ADDRESS = OPTION_DEFAULT_VALUES[j].d_value_p;

      if (veryVerbose) {
          T_ P_(i) P(j)
      }

      for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
          const char CONFIG = cfg;  // how we specify the allocator

          if (veryVerbose) {
              T_ T_ P(CONFIG)
          }

          bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);

          bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
          bslma::TestAllocator da("default",   veryVeryVeryVerbose);

          bslma::DefaultAllocatorGuard dag(&da);

          bslma::TestAllocatorMonitor sam(&sa);
          bslma::TestAllocatorMonitor dam(&da);

          Obj                  *objPtr           = 0;
          bslma::TestAllocator *objAllocatorPtr  = 0;

          switch (CONFIG) {
            case 'a': {
              objAllocatorPtr = &da;
            } break;
            case 'b': {
              objAllocatorPtr = &da;
            } break;
            case 'c': {
              objAllocatorPtr = &sa;
            } break;
            default: {
              ASSERTV(CONFIG, !"Bad allocator config.");
            } break;
          }

          bslma::TestAllocator&  oa = *objAllocatorPtr;
          bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

          bslma::TestAllocatorMonitor  oam(&oa);
          bslma::TestAllocatorMonitor noam(&noa);

          switch (CONFIG) {
            case 'a': {
              objPtr = new (fa) Obj(OTYPE);       // ACTION
            } break;
            case 'b': {
              objPtr = new (fa) Obj(OTYPE, 0);    // ACTION
            } break;
            case 'c': {
              objPtr = new (fa) Obj(OTYPE, &sa);  // ACTION
            } break;
            default: {
              ASSERTV(CONFIG, !"Bad allocator config.");
            } break;
          }

          ASSERTV(LINE1, LINE2,  oam.isTotalSame());
          ASSERTV(LINE1, LINE2, noam.isTotalSame());

          Obj&  mX = *objPtr;  const Obj& X = mX;

          ASSERTV(LINE1, LINE2,  false  == X.hasDefaultValue());
          ASSERTV(LINE1, LINE2,  OTYPE  == X.occurrenceType());
          ASSERTV(LINE1, LINE2, (OTYPE  == Obj::e_HIDDEN)   == X.isHidden());
          ASSERTV(LINE1, LINE2, (OTYPE  == Obj::e_REQUIRED) == X.isRequired());
          ASSERTV(LINE1, LINE2,  &oa    == X.allocator());

          if (OTYPE != Obj::e_REQUIRED) {

              if (ADDRESS) {
                  OptionValue dfltOptionValue(ETYPE);
                  u::setOptionValue(&dfltOptionValue, ADDRESS, ETYPE);

                  if (veryVerbose) {
                      T_ T_ P(dfltOptionValue)
                  }

                  bslma::TestAllocatorMonitor oam(&oa);

                  mX.setDefaultValue(dfltOptionValue);  // ACTION

                  if (u::isAllocatingType(ETYPE)) {
                      ASSERTV(LINE1, LINE2, oam.isTotalUp());
                  }

                  ASSERTV(LINE1, LINE2, true       == X.hasDefaultValue());
                  ASSERTV(LINE1, LINE2, OTYPE      == X.occurrenceType());
                  ASSERTV(LINE1, LINE2, ETYPE      == X.defaultValue().
                                                                     type());
                  ASSERTV(LINE1, LINE2, dfltOptionValue
                                                   == X.defaultValue());
                  ASSERTV(LINE1, LINE2, (OTYPE     == Obj::e_HIDDEN)
                                                   == X.isHidden());
                  ASSERTV(LINE1, LINE2, false      == X.isRequired());
                  ASSERTV(LINE1, LINE2, &oa        == X.allocator());
              }

              mX.setHidden();
              ASSERTV(LINE1, LINE2, true          == X.isHidden());
              ASSERTV(LINE1, LINE2, Obj::e_HIDDEN == X.occurrenceType());
          }

          fa.deleteObject(objPtr);

          ASSERTV(LINE1, LINE2,  oam.isInUseSame());
          ASSERTV(LINE1, LINE2, noam.isInUseSame());
      }
  }
  }
//^-----v

// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
// BDE_VERIFY pragma: +TP21    // Loops must contain very verbose action

        if (veryVerbose) cout << "Negative Testing" << endl;

        bsls::AssertTestHandlerGuard hG;

        {
            Obj mX(Obj::e_REQUIRED);
            Obj mY(Obj::e_OPTIONAL);
            Obj mZ(Obj::e_HIDDEN);

            ASSERT_FAIL(mX.setHidden());
            ASSERT_PASS(mY.setHidden());
            ASSERT_PASS(mZ.setHidden());
        }

// BDE_VERIFY pragma: -TP21    // Loops must contain very verbose action
// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line

//v-----^
  for (int i = 0; i < NUM_OPTION_OCCURRENCES;    ++i) {
  for (int j = 0; j < NUM_OPTION_DEFAULT_VALUES; ++j) {

      const int       LINE1   = OPTION_OCCURRENCES[i].d_line;
      const OccurType OTYPE   = OPTION_OCCURRENCES[i].d_type;

      const int       LINE2   = OPTION_DEFAULT_VALUES[j].d_line;
      const ElemType  ETYPE   = OPTION_DEFAULT_VALUES[j].d_type;
      const void     *ADDRESS = OPTION_DEFAULT_VALUES[j].d_value_p;

      if (veryVerbose) {
          T_ P_(i) P_(LINE1) P_(OTYPE) P_(j) P_(ETYPE) P(LINE2)
      }

      {
          Obj mX(OTYPE);

          if (OTYPE == Obj::e_REQUIRED) {
            ASSERT_FAIL(mX.setHidden());
          } else {
            ASSERT_PASS(mX.setHidden());
          }
      }

      if (ADDRESS) {
          Obj mX(OTYPE);

          OptionValue defaultOptionValue(ETYPE);
          u::setOptionValue(&defaultOptionValue, ADDRESS, ETYPE);

          if (OTYPE == Obj::e_REQUIRED) {
              ASSERT_FAIL(mX.setDefaultValue(defaultOptionValue));
          } else {
              ASSERT_PASS(mX.setDefaultValue(defaultOptionValue));
          }
      }

      {
          Obj mX(OTYPE);

          OptionValue nullOptionValue(ETYPE);
          nullOptionValue.setNull();

          ASSERT_FAIL(mX.setDefaultValue(nullOptionValue));
      }

      {
          Obj         mX(OTYPE);

          OptionValue boolOptionValue(Ot::e_BOOL);

          ASSERT_FAIL(mX.setDefaultValue(boolOptionValue));
      }

      {
          Obj mX(OTYPE);

          OptionValue unsetOptionValue;
          ASSERT(Ot::e_VOID == unsetOptionValue.type());

          ASSERT_FAIL(mX.setDefaultValue(unsetOptionValue));
      }
  }
  }
//^-----v

// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
// BDE_VERIFY pragma: +TP21    // Loops must contain very verbose action

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
        //: 1 Ad-hoc testing.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        Obj mX(Obj::e_REQUIRED);  const Obj& X = mX;
        Obj mY(Obj::e_HIDDEN);    const Obj& Y = mY;
        Obj mZ(Obj::e_OPTIONAL);  const Obj& Z = mZ;

        ASSERT(Obj::e_REQUIRED == X.occurrenceType());
        ASSERT(Obj::e_HIDDEN   == Y.occurrenceType());
        ASSERT(Obj::e_OPTIONAL == Z.occurrenceType());

        ASSERT( X.isRequired());
        ASSERT(!Y.isRequired());
        ASSERT(!Z.isRequired());

        ASSERT(!X.isHidden());
        ASSERT( Y.isHidden());
        ASSERT(!Z.isHidden());

        ASSERT(!X.hasDefaultValue());
        ASSERT(!Y.hasDefaultValue());
        ASSERT(!Z.hasDefaultValue());

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    ASSERT(0 == globalAllocator.numBlocksTotal());

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
