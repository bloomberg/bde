// balcl_occurrenceinfo.t.cpp                                         -*-C++-*-

#include <balcl_occurrenceinfo.h>

#include <balcl_optiontype.h>
#include <balcl_optionvalue.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_time.h>

#include <bdlb_numericparseutil.h>
#include <bdlb_printmethods.h>     // 'bdlb::HasPrintMethod'

#include <bslim_testutil.h>

#include <bslmf_assert.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bsls_assert.h>
#include <bsls_types.h>

#include <bsl_iostream.h>
#include <bsl_ostream.h>    // 'bsl::ostream<<'
#include <bsl_string.h>     // 'bslstl::StringRef'
#include <bsl_vector.h>

#include <bsl_cstdlib.h>
#include <bsl_cassert.h>
#include <bsl_cstring.h> // 'bsl::strcmp'

using namespace BloombergLP;
using bsl::cerr;
using bsl::cout;
using bsl::endl;

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
//
// ----------------------------------------------------------------------------
// balcl::OccurrenceInfo
// ----------------------------------------------------------------------------
// CREATORS
// [ 1] OccurrenceInfo();
// [ 1] OccurrenceInfo(bslma::Allocator *basicAllocator);
// [ 1] OccurrenceInfo(OccurrenceType type, *bA = 0);
// [ 1] OccurrenceInfo(char                    dflt, *bA = 0);
// [ 1] OccurrenceInfo(int                     dflt, *bA = 0);
// [ 1] OccurrenceInfo(Int64                   dflt, *bA = 0);
// [ 1] OccurrenceInfo(double                  dflt, *bA = 0);
// [ 1] OccurrenceInfo(const string&           dflt, *bA = 0);
// [ 1] OccurrenceInfo(const Datetime&         dflt, *bA = 0);
// [ 1] OccurrenceInfo(const Date&             dflt, *bA = 0);
// [ 1] OccurrenceInfo(const Time&             dflt, *bA = 0);
// [ 1] OccurrenceInfo(const vector<char>&     dflt, *bA = 0);
// [ 1] OccurrenceInfo(const vector<int>&      dflt, *bA = 0);
// [ 1] OccurrenceInfo(const vector<Int64>&    dflt, *bA = 0);
// [ 1] OccurrenceInfo(const vector<double>&   dflt, *bA = 0);
// [ 1] OccurrenceInfo(const vector<string>&   dflt, *bA = 0);
// [ 1] OccurrenceInfo(const vector<Datetime>& dflt, *bA = 0);
// [ 1] OccurrenceInfo(const vector<Date>&     dflt, *bA = 0);
// [ 1] OccurrenceInfo(const vector<Time>&     dflt, *bA = 0);
// [ 1] OccurrenceInfo(const Cloi& original, *bA = 0);
// [ 1] ~OccurrenceInfo();
//
// MANIPULATORS
// [ 1] OccurrenceInfo& operator=(const Cloi& rhs);
// [ 1] void setDefaultValue(const OptionValue& defaultValue);
// [ 1] void setHidden();
//
// ACCESSORS
// [ 1] const OptionValue& defaultValue() const;
// [ 1] bool hasDefaultValue() const;
// [ 1] bool isHidden() const;
// [ 1] bool isRequired() const;
// [ 1] OccurrenceType occurrenceType() const;
//
// [ 1] bslma::Allocator *allocator() const;
// [ 1] ostream& print(ostream& stream, int level = 0, int spl = 4) const;
//
// FREE OPERATORS
// [ 1] bool operator==(const OccurrenceInfo& lhs, rhs)
// [ 1] bool operator!=(const OccurrenceInfo& lhs, rhs)
// [ 1] operator<<(ostream& stream, const OccurrenceInfo& rhs);
// ----------------------------------------------------------------------------
// [XX] BREATHING TEST
// [ 1] TESTING 'balcl::OccurrenceInfo'
// [XX] USAGE EXAMPLE

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
//                      GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef balcl::OccurrenceInfo    Obj;

typedef Obj::OccurrenceType      OccurType;
typedef balcl::OptionType        Ot;
typedef balcl::OptionType        OptionType;
typedef balcl::OptionType::Enum  ElemType;
typedef balcl::OptionValue       OptionValue;

typedef bsls::Types::Int64       Int64;

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

bool                        defaultBool          = false;
char                        defaultChar          = 'D';
short                       defaultShort         = 1234;
int                         defaultInt           = 1234567;
Int64                       defaultInt64         = 123456789LL;
float                       defaultFloat         = 0.125;     // 1/8
double                      defaultDouble        = 0.015625;  // 1/64
bsl::string                 defaultString        = "ABCDEFGHIJ";
bdlt::Datetime              defaultDatetime(1234, 12, 3, 4, 5, 6);
bdlt::Date                  defaultDate(1234, 4, 6);
bdlt::Time                  defaultTime(7, 8, 9, 10);
bsl::vector<char>           defaultCharArray    (1, defaultChar);
bsl::vector<short>          defaultShortArray   (1, defaultShort);
bsl::vector<int>            defaultIntArray     (1, defaultInt);
bsl::vector<Int64>          defaultInt64Array   (1, defaultInt64);
bsl::vector<float>          defaultFloatArray   (1, defaultFloat);
bsl::vector<double>         defaultDoubleArray  (1, defaultDouble);
bsl::vector<bsl::string>    defaultStringArray  (1, defaultString);
bsl::vector<bdlt::Datetime> defaultDatetimeArray(1, defaultDatetime);
bsl::vector<bdlt::Date>     defaultDateArray    (1, defaultDate);
bsl::vector<bdlt::Time>     defaultTimeArray    (1, defaultTime);

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

    assert (3 == bsl::strlen(input));

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
    assert(9 == input.size());

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
    assert(k_TIME_FIELD_WIDTH == input.size());

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
    assert(k_DATETIME_FIELD_WIDTH == input.size());

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

struct ParserImpUtil {

    enum { LOCAL_SUCCESS = 0, LOCAL_FAILURE = 1 };

    static int skipRequiredToken(const char **endPos,
                                 const char  *inputString,
                                 const char  *token);
        // Skip past the value of the specified 'token' in the specified
        // 'inputString'.  Store in the specified '*endPos' the address of the
        // non-modifiable character in 'inputString' immediately following the
        // successfully matched text, or the position at which the match
        // failure was detected.  Return 0 if 'token' is found, and a non-zero
        // value otherwise.

    static int skipWhiteSpace(const char **endPos,
                              const char  *inputString);
        // Skip over any combination of C-style comments, C++-style comments,
        // and characters for which 'isspace' returns true in the specified
        // 'inputString'.  Store in the specified '*endPos' the address of the
        // non-modifiable character in 'inputString' immediately following the
        // successfully matched text, or the position at which the match
        // failure was detected.  Return 0 on success and a non-zero value
        // otherwise.  If a C++-style comment terminates with '\0', 'endPos'
        // will point *at* the '\0' and not past it.  The behavior is undefined
        // if either argument is 0.
        //
        // A parse failure can occur for the following reason:
        //..
        //   '\0' is found before a C-style comment is terminated with '*/'.
        //..
};

                            // --------------------
                            // struct ParserImpUtil
                            // --------------------

int ParserImpUtil::skipRequiredToken(const char **endPos,
                                     const char  *inputString,
                                     const char  *token)
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

int ParserImpUtil::skipWhiteSpace(const char **endPos,
                                  const char  *inputString)
{
    BSLS_ASSERT(endPos);
    BSLS_ASSERT(inputString);

    while (1) {
        while (isspace(static_cast<unsigned char>(*inputString))) {
            ++inputString;
        }
        if (*inputString != '/') {
            *endPos = inputString;
            return LOCAL_SUCCESS;                                     // RETURN
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
                        return LOCAL_FAILURE;                         // RETURN
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
                        return LOCAL_SUCCESS;                         // RETURN
                    }
                    ++inputString;
                }
            } else {
                *endPos = inputString - 1;
                return LOCAL_SUCCESS;                                 // RETURN
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
        // Load to the specified 'result' the 'TYPE' value represented at the
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
        // Load to the specified 'result' the next double quote (i.e., '"')
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

    ParserImpUtil::skipWhiteSpace(endPos, inputString);

    TYPE value;
    while (']' != **endPos) {
        if (u::TypesParser::parse(&value, endPos, *endPos)) {
            return LOCAL_FAILURE;                                     // RETURN
        }
        result->push_back(value);
        inputString = *endPos;
        ParserImpUtil::skipWhiteSpace(endPos, inputString);
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

#define MATCH_OPTION_TYPE(ELEM_TYPE, TYPE)                               \
    template <>                                                          \
    struct CheckOptionType<(int)ELEM_TYPE, TYPE> {                       \
        bool operator()() const { return true; }                         \
    };                                                                   \
    // This macro defines a specialization of the 'CheckOptionType' class
    // template whose boolean functor returns 'true', one for the parameterized
    // 'ELEM_TYPE' matching the parameterized 'TYPE'.

#define MATCH_OPTION_TYPE_PAIR(ELEM_TYPE, TYPE)                          \
                                                                         \
    MATCH_OPTION_TYPE(ELEM_TYPE, TYPE)                                   \
                                                                         \
    template <>                                                          \
    struct CheckOptionType<(int)ELEM_TYPE##_ARRAY, bsl::vector<TYPE> > { \
        bool operator()() const { return true; }                         \
    };
    // This macro defines two specializations of the 'CheckOptionType' class
    // template whose boolean functor returns 'true', one for the parameterized
    // 'ELEM_TYPE' matching the parameterized 'TYPE', and the other for the
    // corresponding array type.

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
    BSLS_ASSERT(OptionType::e_VOID != type);
    BSLS_ASSERT(dst->type()        == type);

    switch (type) {
      case Ot::e_VOID: {
        BSLS_ASSERT(!"Not reachable.");
      } break;
      case Ot::e_BOOL: {
        dst->set(*(static_cast<const Ot::Bool          *>(src)));
      } break;
      case Ot::e_CHAR: {
        dst->set(*(static_cast<const Ot::Char          *>(src)));
      } break;
      case Ot::e_INT: {
        dst->set(*(static_cast<const Ot::Int           *>(src)));
      } break;
      case Ot::e_INT64: {
        dst->set(*(static_cast<const Ot::Int64         *>(src)));
      } break;
      case Ot::e_DOUBLE: {
        dst->set(*(static_cast<const Ot::Double        *>(src)));
      } break;
      case Ot::e_STRING: {
        dst->set(*(static_cast<const Ot::String        *>(src)));
      } break;
      case Ot::e_DATETIME: {
        dst->set(*(static_cast<const Ot::Datetime      *>(src)));
      } break;
      case Ot::e_DATE: {
        dst->set(*(static_cast<const Ot::Date          *>(src)));
      } break;
      case Ot::e_TIME: {
        dst->set(*(static_cast<const Ot::Time          *>(src)));
      } break;
      case Ot::e_CHAR_ARRAY: {
        dst->set(*(static_cast<const Ot::CharArray     *>(src)));
      } break;
      case Ot::e_INT_ARRAY: {
        dst->set(*(static_cast<const Ot::IntArray      *>(src)));
      } break;
      case Ot::e_INT64_ARRAY: {
        dst->set(*(static_cast<const Ot::Int64Array    *>(src)));
      } break;
      case Ot::e_DOUBLE_ARRAY: {
        dst->set(*(static_cast<const Ot::DoubleArray   *>(src)));
      } break;
      case Ot::e_STRING_ARRAY: {
        dst->set(*(static_cast<const Ot::StringArray   *>(src)));
      } break;
      case Ot::e_DATETIME_ARRAY: {
        dst->set(*(static_cast<const Ot::DatetimeArray *>(src)));
      } break;
      case Ot::e_DATE_ARRAY: {
        dst->set(*(static_cast<const Ot::DateArray     *>(src)));
      } break;
      case Ot::e_TIME_ARRAY: {
        dst->set(*(static_cast<const Ot::TimeArray     *>(src)));
      } break;
    }
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
    typedef ParserImpUtil Parser;

    enum { SUCCESS = 0, FAILURE = -1 };

    if (Parser::skipWhiteSpace(&input, input)) {
        *endpos = input;
        return FAILURE;                                               // RETURN
    }
    if (*input != '{') {
        if (Obj::e_REQUIRED == occurrenceInfo.occurrenceType()) {
            if (Parser::skipRequiredToken(endpos, input, "REQUIRED")) {
                return FAILURE;                                       // RETURN
            }
            return SUCCESS;                                           // RETURN
        }
        if (Obj::e_OPTIONAL == occurrenceInfo.occurrenceType()) {
            if (Parser::skipRequiredToken(endpos, input, "OPTIONAL")) {
                return FAILURE;                                       // RETURN
            }
            return SUCCESS;                                           // RETURN
        }
        if (Obj::e_HIDDEN == occurrenceInfo.occurrenceType()) {
            if (Parser::skipRequiredToken(endpos, input, "HIDDEN")) {
                return FAILURE;                                       // RETURN
            }
            return SUCCESS;                                           // RETURN
        }
        *endpos = input;
        return FAILURE;                                               // RETURN
    } else {
        ++input; // Parser::skipRequiredToken(&input, input, "{");
        if (Obj::e_OPTIONAL == occurrenceInfo.occurrenceType()) {
            if (Parser::skipWhiteSpace(&input, input)
             || Parser::skipRequiredToken(&input, input, "OPTIONAL")) {
                *endpos = input;
                return FAILURE;                                       // RETURN
            }
        } else {
            if (Parser::skipWhiteSpace(&input, input)
             || Parser::skipRequiredToken(&input, input, "HIDDEN")) {
                *endpos = input;
                return FAILURE;                                       // RETURN
            }
        }
        if (occurrenceInfo.hasDefaultValue()) {
            if (Parser::skipWhiteSpace(&input, input)
             || Parser::skipRequiredToken(&input, input, "DEFAULT_TYPE")) {
                *endpos = input;
                return FAILURE;                                       // RETURN
            }
            if (Parser::skipWhiteSpace(&input, input)
             || Parser::skipRequiredToken(&input, input,
                          Ot::toAscii(occurrenceInfo.defaultValue().type()))) {
                *endpos = input;
                return FAILURE;                                       // RETURN
            }
            if (Parser::skipWhiteSpace(&input, input)
             || Parser::skipRequiredToken(&input, input, "DEFAULT_VALUE")
             || Parser::skipWhiteSpace(&input, input)) {
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
        if (Parser::skipWhiteSpace(&input, input)
         || Parser::skipRequiredToken(&input, input, "}")) {
            *endpos = input;
            return FAILURE;                                           // RETURN
        }
    }

    *endpos = input;
    return SUCCESS;
}

}  // close namespace u
}  // close unnamed namespace

// ============================================================================
//                  USAGE EXAMPLE CLASSES AND FUNCTIONS
// ----------------------------------------------------------------------------

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, const char *argv[])
{
    int                test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int             verbose = argc > 2;
    int         veryVerbose = argc > 3;
    int     veryVeryVerbose = argc > 4;
    int veryVeryVeryVerbose = argc > 5;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'balcl::OccurrenceInfo'
        //
        // Concerns:
        //: 1 'balcl::OccurrenceInfo' is a simple unconstrained
        //:   attribute class that takes a 'bslma' allocator.  The concerns are
        //:   generally the same as for any value-semantic type that takes a
        //:   'bslma' allocator.
        //
        // This class has redundant manipulators and accessors, so we choose
        // the primary manipulators to be:
        //..
        //  balcl::OccurrenceInfo(bslma::Allocator *allocator);
        //  balcl::OccurrenceInfo(OccurrenceType    type,
        //                                  bslma::Allocator *allocator);
        //  void setDefaultValue(balcl::OptionValue value);
        //..
        // and the primary accessors to be:
        //..
        //  OccurrenceType occurrenceType() const;
        //  bool hasDefaultValue() const;
        //  balcl::OptionValue& defaultValue() const;
        //..
        //
        // Plan:
        //: 1 We follow the standard structure of a value-semantic test driver,
        //:   with the twist that we test a very simple non-primary manipulator
        //:   ('setHidden') in case 4, to avoid having to write a separate test
        //:   case for it.
        //
        // Testing:
        //   TESTING 'balcl::OccurrenceInfo'
        //   OccurrenceInfo();
        //   OccurrenceInfo(bslma::Allocator *basicAllocator);
        //   OccurrenceInfo(OccurrenceType type, *bA = 0);
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
        //   OccurrenceInfo(const Cloi& original, *bA = 0);
        //   ~OccurrenceInfo();
        //   OccurrenceInfo& operator=(const Cloi& rhs);
        //   void setDefaultValue(const OptionValue& defaultValue);
        //   void setHidden();
        //   const OptionValue& defaultValue() const;
        //   bool hasDefaultValue() const;
        //   bool isHidden() const;
        //   bool isRequired() const;
        //   OccurrenceType occurrenceType() const;
        //   bslma::Allocator *allocator() const;
        //   ostream& print(ostream& stream, int level = 0, int spl = 4) const;
        //   bool operator==(const OccurrenceInfo& lhs, rhs)
        //   bool operator!=(const OccurrenceInfo& lhs, rhs)
        //   operator<<(ostream& stream, const OccurrenceInfo& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout
                        << endl
                        << "TESTING 'balcl::OccurrenceInfo'" << endl
                        << "==============================-" << endl;

        bslma::TestAllocator    testAllocator("test", veryVeryVeryVerbose);
        bslma::TestAllocator defaultAllocator("dflt", veryVeryVeryVerbose);
        bslma::TestAllocator   inputAllocator("iput", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard guard(&defaultAllocator);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 2 OF VALUE-SEMANTIC TEST DRIVER
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP)

        if (verbose) cout << "\n\tTesting primary manipulators." << endl;

        if (verbose) cout << "\t\tWithout passing in an allocator." << endl;

// BDE_VERIFY pragma: -TP21    // Loops must contain very verbose action
// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
        for (int i = 0; i < NUM_OPTION_OCCURRENCES;    ++i) {
        for (int j = 0; j < NUM_OPTION_DEFAULT_VALUES; ++j) {

            const int       LINE1   = OPTION_OCCURRENCES[i].d_line;
            const OccurType OTYPE   = OPTION_OCCURRENCES[i].d_type;

            const int       LINE2   = OPTION_DEFAULT_VALUES[j].d_line;
            const ElemType  ETYPE   = OPTION_DEFAULT_VALUES[j].d_type;
            const void     *ADDRESS = OPTION_DEFAULT_VALUES[j].d_value_p;

            Obj mX(OTYPE);  const Obj& X = mX;      // TEST HERE

            if (veryVerbose) {
                T_ T_ T_ P_(i) P_(j) P(X)
            }
            LOOP2_ASSERT(LINE1, LINE2, !X.hasDefaultValue());
            LOOP2_ASSERT(LINE1, LINE2, OTYPE == X.occurrenceType());

            if (OTYPE != Obj::e_REQUIRED && ADDRESS) {
                OptionValue DEFAULT_VALUE(ETYPE);
                u::setOptionValue(&DEFAULT_VALUE, ADDRESS, ETYPE);

                mX.setDefaultValue(DEFAULT_VALUE);  // AND HERE

                if (veryVerbose) {
                    T_ T_ T_ P_(i) P_(j) P(X)
                }
                LOOP2_ASSERT(LINE1, LINE2, X.hasDefaultValue());
                LOOP2_ASSERT(LINE1, LINE2, OTYPE == X.occurrenceType());
                LOOP2_ASSERT(LINE1, LINE2, ETYPE == X.defaultValue().type());
                LOOP2_ASSERT(LINE1, LINE2, DEFAULT_VALUE == X.defaultValue());
            }
        }
        }
// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
// BDE_VERIFY pragma: +TP21    // Loops must contain very verbose action

        if (verbose) cout << "\t\tPassing in an allocator." << endl
                           << "\t\t\tWith no exceptions." << endl;

        const bool HAS_BSLMA_ALLOCATOR_TRAIT =
                                         bslma::UsesBslmaAllocator<Obj>::value;

        ASSERT(HAS_BSLMA_ALLOCATOR_TRAIT);

// BDE_VERIFY pragma: -TP21    // Loops must contain very verbose action
// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
        for (int i = 0; i < NUM_OPTION_OCCURRENCES;    ++i) {
        for (int j = 0; j < NUM_OPTION_DEFAULT_VALUES; ++j) {

            const int       LINE1   = OPTION_OCCURRENCES[i].d_line;
            const OccurType OTYPE   = OPTION_OCCURRENCES[i].d_type;

            const int       LINE2   = OPTION_DEFAULT_VALUES[j].d_line;
            const ElemType  ETYPE   = OPTION_DEFAULT_VALUES[j].d_type;
            const void     *ADDRESS = OPTION_DEFAULT_VALUES[j].d_value_p;

            const bsls::Types::Int64 NUM_BYTES =
                                              defaultAllocator.numBytesInUse();
            const bsls::Types::Int64 NUM_ALLOC =
                                             defaultAllocator.numAllocations();
            {
                Obj mX(OTYPE, &testAllocator);  const Obj& X = mX;

                if (veryVerbose) {
                    T_ T_ T_ P_(i) P_(j) P(X)
                }
                LOOP2_ASSERT(LINE1, LINE2, !X.hasDefaultValue());
                LOOP2_ASSERT(LINE1, LINE2, OTYPE == X.occurrenceType());

                if (OTYPE != Obj::e_REQUIRED && ADDRESS) {
                    OptionValue DEFAULT_VALUE(ETYPE, &inputAllocator);
                    u::setOptionValue(&DEFAULT_VALUE, ADDRESS, ETYPE);

                    mX.setDefaultValue(DEFAULT_VALUE);

                    if (veryVerbose) {
                        T_ T_ T_ P_(i) P_(j) P(X)
                    }
                    LOOP2_ASSERT(LINE1, LINE2, X.hasDefaultValue());
                    LOOP2_ASSERT(LINE1, LINE2,
                                 OTYPE         == X.occurrenceType());
                    LOOP2_ASSERT(LINE1, LINE2,
                                 ETYPE         == X.defaultValue().type());
                    LOOP2_ASSERT(LINE1, LINE2,
                                 DEFAULT_VALUE == X.defaultValue());
                }
            }

            LOOP2_ASSERT(LINE1, LINE2, 0 == testAllocator.numBytesInUse());
            LOOP2_ASSERT(LINE1, LINE2, 0 == testAllocator.numMismatches());
            LOOP2_ASSERT(LINE1, LINE2,
                         NUM_BYTES == defaultAllocator.numBytesInUse());
            LOOP2_ASSERT(LINE1, LINE2,
                         NUM_ALLOC == defaultAllocator.numAllocations());
        }
        }
// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line
// BDE_VERIFY pragma: +TP21    // Loops must contain very verbose action

        if (verbose) cout << "\t\t\tWith exceptions." << endl;

// BDE_VERIFY pragma: -TP21    // Loops must contain very verbose action
// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
        for (int i = 0; i < NUM_OPTION_OCCURRENCES;    ++i) {
        for (int j = 0; j < NUM_OPTION_DEFAULT_VALUES; ++j) {

            const int       LINE1   = OPTION_OCCURRENCES[i].d_line;
            const OccurType OTYPE   = OPTION_OCCURRENCES[i].d_type;

            const int       LINE2   = OPTION_DEFAULT_VALUES[j].d_line;
            const ElemType  ETYPE   = OPTION_DEFAULT_VALUES[j].d_type;
            const void     *ADDRESS = OPTION_DEFAULT_VALUES[j].d_value_p;

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                Obj mX(OTYPE, &testAllocator);  const Obj& X = mX;

                LOOP2_ASSERT(LINE1, LINE2, !X.hasDefaultValue());
                LOOP2_ASSERT(LINE1, LINE2, OTYPE == X.occurrenceType());

                if (OTYPE != Obj::e_REQUIRED && ADDRESS) {
                    OptionValue DEFAULT_VALUE(ETYPE);
                    u::setOptionValue(&DEFAULT_VALUE, ADDRESS, ETYPE);

                    mX.setDefaultValue(DEFAULT_VALUE);

                    LOOP2_ASSERT(LINE1, LINE2,
                                 OTYPE         == X.occurrenceType());
                    LOOP2_ASSERT(LINE1, LINE2,
                                 ETYPE         == X.defaultValue().type());
                    LOOP2_ASSERT(LINE1, LINE2,
                                 DEFAULT_VALUE == X.defaultValue());
                }
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }
        }
// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line
// BDE_VERIFY pragma: +TP21    // Loops must contain very verbose action
        ASSERT(0 == testAllocator.numBytesInUse());
        ASSERT(0 == testAllocator.numMismatches());

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 4 OF VALUE-SEMANTIC TEST DRIVER
        // TESTING BASIC ACCESSORS

        if (verbose) cout << "\n\tTesting basic accessors." << endl;

// BDE_VERIFY pragma: -TP21    // Loops must contain very verbose action
// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
        for (int i = 0; i < NUM_OPTION_OCCURRENCES;    ++i) {
        for (int j = 0; j < NUM_OPTION_DEFAULT_VALUES; ++j) {

            const int       LINE1   = OPTION_OCCURRENCES[i].d_line;
            const OccurType OTYPE   = OPTION_OCCURRENCES[i].d_type;

            const int       LINE2   = OPTION_DEFAULT_VALUES[j].d_line;
            const ElemType  ETYPE   = OPTION_DEFAULT_VALUES[j].d_type;
            const void     *ADDRESS = OPTION_DEFAULT_VALUES[j].d_value_p;

            Obj mX(OTYPE);  const Obj& X = mX;

            LOOP2_ASSERT(LINE1, LINE2, !X.hasDefaultValue());
            LOOP2_ASSERT(LINE1, LINE2,  OTYPE == X.occurrenceType());
            LOOP2_ASSERT(LINE1, LINE2, (OTYPE == Obj::e_HIDDEN) ==
                                                                 X.isHidden());
            LOOP2_ASSERT(LINE1, LINE2, (OTYPE == Obj::e_REQUIRED) ==
                                                               X.isRequired());

            if (OTYPE != Obj::e_REQUIRED && ADDRESS) {
                OptionValue DEFAULT_VALUE(ETYPE);
                u::setOptionValue(&DEFAULT_VALUE, ADDRESS, ETYPE);

                mX.setDefaultValue(DEFAULT_VALUE);

                LOOP2_ASSERT(LINE1, LINE2, X.hasDefaultValue());
                LOOP2_ASSERT(LINE1, LINE2, OTYPE == X.occurrenceType());
                LOOP2_ASSERT(LINE1, LINE2, ETYPE == X.defaultValue().type());
                LOOP2_ASSERT(LINE1, LINE2, DEFAULT_VALUE == X.defaultValue());
             // LOOP2_ASSERT(LINE1, LINE2, ADDRESS != X.defaultValue().data());

                mX.setHidden();  // TEST EXTRA MANIPULATOR

                LOOP2_ASSERT(LINE1, LINE2, Obj::e_HIDDEN ==
                                                           X.occurrenceType());

                LOOP2_ASSERT(LINE1, LINE2, X.hasDefaultValue());
                LOOP2_ASSERT(LINE1, LINE2, ETYPE == X.defaultValue().type());
                LOOP2_ASSERT(LINE1, LINE2, DEFAULT_VALUE == X.defaultValue());
             // LOOP2_ASSERT(LINE1, LINE2, ADDRESS != X.defaultValue().data());
                LOOP2_ASSERT(LINE1, LINE2, X.isHidden());
            }
        }
        }
// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line
// BDE_VERIFY pragma: +TP21    // Loops must contain very verbose action

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 5 OF VALUE-SEMANTIC TEST DRIVER
        // TESTING OUTPUT (<<) OPERATOR AND PRINT

        if (verbose)
            cout << "\n\tTesting output operator and 'print'." << endl;

// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
        for (int i = 0; i < NUM_OPTION_OCCURRENCES;    ++i) {
        for (int j = 0; j < NUM_OPTION_DEFAULT_VALUES; ++j) {

            const OccurType OTYPE   = OPTION_OCCURRENCES[i].d_type;

            const ElemType  ETYPE   = OPTION_DEFAULT_VALUES[j].d_type;
            const void     *ADDRESS = OPTION_DEFAULT_VALUES[j].d_value_p;

            Obj mX(OTYPE);  const Obj& X = mX;

            if (OTYPE != Obj::e_REQUIRED && ADDRESS) {
                OptionValue DEFAULT_VALUE(ETYPE);
                u::setOptionValue(&DEFAULT_VALUE, ADDRESS, ETYPE);

                mX.setDefaultValue(DEFAULT_VALUE);
            }

            bsl::ostringstream oss1, oss2;
            X.print(oss1);                  // TEST HERE
            oss2 << X;                      // TEST HERE

            bsl::string ss1(oss1.str()), ss2(oss2.str());
            ASSERT(ss1 == ss2);
                // Note: oss[12].str() are temporaries!  Doesn't work well with
                // oss[12].str().c_str();  except on SunPro where temporary
                // lives till the end of the block.

            if (veryVerbose) {
                T_ T_ P(X)
                T_ T_ P(ss1)
                T_ T_ P(ss2)
            }

            const char *output = ss1.c_str();
            const int parseRet = u::parseOccurrenceInfo(&output, X, output);
            LOOP2_ASSERT(parseRet, output, 0 == parseRet);
        }
        }
// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 6 OF VALUE-SEMANTIC TEST DRIVER
        // TESTING EQUALITY OPERATOR

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

            Obj mX(OTYPE1);  const Obj& X = mX;

            if (OTYPE1 != Obj::e_REQUIRED && ADDRESS1) {
                OptionValue DEFAULT_VALUE1(ETYPE2);
                u::setOptionValue(&DEFAULT_VALUE1, ADDRESS1, ETYPE2);

                mX.setDefaultValue(DEFAULT_VALUE1);
            }

            for (int k = 0; k < NUM_OPTION_OCCURRENCES;    ++k) {
            for (int l = 0; l < NUM_OPTION_DEFAULT_VALUES; ++l) {

                const int       LINE3    = OPTION_OCCURRENCES[k].d_line;
                const OccurType OTYPE3   = OPTION_OCCURRENCES[k].d_type;

                const int       LINE4    = OPTION_DEFAULT_VALUES[l].d_line;
                const ElemType  ETYPE4   = OPTION_DEFAULT_VALUES[l].d_type;
                const void     *ADDRESS2 = OPTION_DEFAULT_VALUES[l].d_value_p;

                Obj mY(OTYPE3);  const Obj& Y = mY;

                if (OTYPE3 != Obj::e_REQUIRED && ADDRESS2) {
                    OptionValue DEFAULT_VALUE2(ETYPE4);
                    u::setOptionValue(&DEFAULT_VALUE2, ADDRESS2, ETYPE4);

                    mY.setDefaultValue(DEFAULT_VALUE2);
                }

                bool isSame = (i == k);
                if (OTYPE1 != Obj::e_REQUIRED
                 && OTYPE3 != Obj::e_REQUIRED) {
                    isSame = (i == k) && (j == l);
                }
                LOOP4_ASSERT(LINE1, LINE2, LINE3, LINE4,
                             isSame == (X == Y));        // TEST HERE
                LOOP4_ASSERT(LINE1, LINE2, LINE3, LINE4,
                             !isSame == (X != Y));       // TEST HERE
            }
            }
        }
        }
// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line
// BDE_VERIFY pragma: +TP21    // Loops must contain very verbose action

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 7 OF VALUE-SEMANTIC TEST DRIVER
        // TESTING COPY CONSTRUCTOR

        if (verbose) cout << "\n\tTesting copy constructor." << endl;

        if (verbose) cout << "\t\tWithout passing in an allocator." << endl;

// BDE_VERIFY pragma: -TP21    // Loops must contain very verbose action
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

            Obj mY(X);  const Obj& Y = mY;  // TEST HERE

            LOOP2_ASSERT(LINE1, LINE2, X == Y);
        }
        }
// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line
// BDE_VERIFY pragma: +TP21    // Loops must contain very verbose action

        if (verbose) cout << "\t\tPassing in an allocator." << endl
                           << "\t\t\tWith no exceptions." << endl;

// BDE_VERIFY pragma: -TP21    // Loops must contain very verbose action
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

            const bsls::Types::Int64 NUM_BYTES =
                                              defaultAllocator.numBytesInUse();
            const bsls::Types::Int64 NUM_ALLOC =
                                             defaultAllocator.numAllocations();
            {
                Obj mY(X, &testAllocator);  const Obj& Y = mY;  // TEST HERE

                LOOP2_ASSERT(LINE1, LINE2, X == Y);
            }
            LOOP2_ASSERT(LINE1, LINE2, 0 == testAllocator.numBytesInUse());
            LOOP2_ASSERT(LINE1, LINE2, 0 == testAllocator.numMismatches());
            LOOP2_ASSERT(LINE1, LINE2,
                         NUM_BYTES == defaultAllocator.numBytesInUse());
            LOOP2_ASSERT(LINE1, LINE2,
                         NUM_ALLOC == defaultAllocator.numAllocations());
        }
        }
// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line
// BDE_VERIFY pragma: +TP21    // Loops must contain very verbose action

        if (verbose) cout << "\t\t\tWith exceptions." << endl;

// BDE_VERIFY pragma: -TP21    // Loops must contain very verbose action
// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
        for (int i = 0; i < NUM_OPTION_OCCURRENCES;    ++i) {
        for (int j = 0; j < NUM_OPTION_DEFAULT_VALUES; ++j) {

            const int       LINE1   = OPTION_OCCURRENCES[i].d_line;
            const OccurType OTYPE   = OPTION_OCCURRENCES[i].d_type;

            const int       LINE2   = OPTION_DEFAULT_VALUES[j].d_line;
            const ElemType  ETYPE   = OPTION_DEFAULT_VALUES[j].d_type;
            const void     *ADDRESS = OPTION_DEFAULT_VALUES[j].d_value_p;

            (void) LINE1;
            (void) LINE2;

            Obj mX(OTYPE);  const Obj& X = mX;

            if (OTYPE != Obj::e_REQUIRED && ADDRESS) {
                OptionValue DEFAULT_VALUE(ETYPE);
                u::setOptionValue(&DEFAULT_VALUE, ADDRESS, ETYPE);

                mX.setDefaultValue(DEFAULT_VALUE);
            }

#if !defined(BSLS_PLATFORM_CMP_MSVC)
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
#endif
                Obj mY(X, &testAllocator);  const Obj& Y = mY;  // TEST HERE

                ASSERT(X == Y);
#if !defined(BSLS_PLATFORM_CMP_MSVC)
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
#endif
        }
        }
// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line
// BDE_VERIFY pragma: +TP21    // Loops must contain very verbose action
        ASSERT(0 == testAllocator.numBytesInUse());
        ASSERT(0 == testAllocator.numMismatches());

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 9 OF VALUE-SEMANTIC TEST DRIVER
        // TESTING ASSIGNMENT OPERATOR

        if (verbose) cout << "\n\tTesting assignment operator." << endl;

        if (verbose) cout << "\t\tTesting assignment u = v." << endl;

// BDE_VERIFY pragma: -TP21    // Loops must contain very verbose action
// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
        for (int i = 0; i < NUM_OPTION_OCCURRENCES;    ++i) {
        for (int j = 0; j < NUM_OPTION_DEFAULT_VALUES; ++j) {

            const int       LINE1    = OPTION_OCCURRENCES[i].d_line;
            const OccurType OTYPE1   = OPTION_OCCURRENCES[i].d_type;

            const int       LINE2    = OPTION_DEFAULT_VALUES[j].d_line;
            const ElemType  ETYPE2   = OPTION_DEFAULT_VALUES[j].d_type;
            const void     *ADDRESS1 = OPTION_DEFAULT_VALUES[j].d_value_p;

            Obj mX(OTYPE1);  const Obj& X = mX;

            if (OTYPE1 != Obj::e_REQUIRED && ADDRESS1) {
                OptionValue DEFAULT_VALUE1(ETYPE2);
                u::setOptionValue(&DEFAULT_VALUE1, ADDRESS1, ETYPE2);

                mX.setDefaultValue(DEFAULT_VALUE1);
            }

            for (int k = 0; k < NUM_OPTION_OCCURRENCES;    ++k) {
            for (int l = 0; l < NUM_OPTION_DEFAULT_VALUES; ++l) {

                const int       LINE3    = OPTION_OCCURRENCES[k].d_line;
                const OccurType OTYPE3   = OPTION_OCCURRENCES[k].d_type;

                const int       LINE4    = OPTION_DEFAULT_VALUES[l].d_line;
                const ElemType  ETYPE4   = OPTION_DEFAULT_VALUES[l].d_type;
                const void     *ADDRESS2 = OPTION_DEFAULT_VALUES[l].d_value_p;

                Obj mY(OTYPE3, &testAllocator);  const Obj& Y = mY;

                if (OTYPE3 != Obj::e_REQUIRED && ADDRESS2) {
                    OptionValue DEFAULT_VALUE2(ETYPE4);
                    u::setOptionValue(&DEFAULT_VALUE2, ADDRESS2, ETYPE4);

                    mY.setDefaultValue(DEFAULT_VALUE2);
                }

                mY = X;  // TEST HERE

                LOOP4_ASSERT(LINE1, LINE2, LINE3, LINE4, X == Y);
            }
            }
        }
        }
// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line
// BDE_VERIFY pragma: +TP21    // Loops must contain very verbose action
        ASSERT(0 == testAllocator.numBytesInUse());
        ASSERT(0 == testAllocator.numMismatches());

        if (verbose) cout << "\t\tTesting self-assignment (Aliasing)." << endl;

// BDE_VERIFY pragma: -TP21    // Loops must contain very verbose action
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

            Obj mZ(X);  const Obj& Z = mZ;

            mX = X;  // TEST HERE

            LOOP2_ASSERT(LINE1, LINE2, Z == X);
        }
        }
// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line
// BDE_VERIFY pragma: +TP21    // Loops must contain very verbose action
        ASSERT(0 == testAllocator.numBytesInUse());
        ASSERT(0 == testAllocator.numMismatches());

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // TESTING ADDITIONAL CONSTRUCTORS

        if (verbose) cout << "\nTesting default-value constructors.\n";

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
                switch (ETYPE) {
  //v-------------^
    case Ot::e_BOOL: {
      Obj        mY(*static_cast<const bool *>                       (ADDRESS),
                    &testAllocator);
      const Obj& Y = mY;
      LOOP2_ASSERT(LINE1, LINE2, X == Y);
    } break;
    case Ot::e_CHAR: {
      Obj        mY(*static_cast<const char *>                       (ADDRESS),
                    &testAllocator);
      const Obj& Y = mY;
      LOOP2_ASSERT(LINE1, LINE2, X == Y);
     } break;
    case Ot::e_INT: {
      Obj        mY(*static_cast<const int *>                        (ADDRESS),
                    &testAllocator);
      const Obj& Y = mY;
      LOOP2_ASSERT(LINE1, LINE2, X == Y);
    } break;
    case Ot::e_INT64: {
      Obj        mY(*static_cast<const Int64 *>                      (ADDRESS),
                    &testAllocator);
      const Obj& Y = mY;
      LOOP2_ASSERT(LINE1, LINE2, X == Y);
    } break;
    case Ot::e_DOUBLE: {
      Obj        mY(*static_cast<const double *>                     (ADDRESS),
                    &testAllocator);
      const Obj& Y = mY;
      LOOP2_ASSERT(LINE1, LINE2, X == Y);
    } break;
    case Ot::e_STRING: {
      Obj        mY(*static_cast<const bsl::string *>                (ADDRESS),
                    &testAllocator);
      const Obj& Y = mY;
      LOOP2_ASSERT(LINE1, LINE2, X == Y);
    } break;
    case Ot::e_DATETIME: {
      Obj        mY(*static_cast<const bdlt::Datetime *>             (ADDRESS),
                    &testAllocator);
      const Obj& Y = mY;
      LOOP2_ASSERT(LINE1, LINE2, X == Y);
    } break;
    case Ot::e_DATE: {
      Obj        mY(*static_cast<const bdlt::Date *>                 (ADDRESS),
                    &testAllocator);
      const Obj& Y = mY;
      LOOP2_ASSERT(LINE1, LINE2, X == Y);
    } break;
    case Ot::e_TIME: {
      Obj        mY(*static_cast<const bdlt::Time *>                 (ADDRESS),
                    &testAllocator);
      const Obj& Y = mY;
      LOOP2_ASSERT(LINE1, LINE2, X == Y);
    } break;
    case Ot::e_CHAR_ARRAY: {
      Obj        mY(*static_cast<const bsl::vector<char> *>          (ADDRESS),
                    &testAllocator);
      const Obj& Y = mY;
      LOOP2_ASSERT(LINE1, LINE2, X == Y);
    } break;
    case Ot::e_INT_ARRAY: {
      Obj        mY(*static_cast<const bsl::vector<int> *>           (ADDRESS),
                    &testAllocator);
      const Obj& Y = mY;
      LOOP2_ASSERT(LINE1, LINE2, X == Y);
    } break;
    case Ot::e_INT64_ARRAY: {
      Obj        mY(*static_cast<const bsl::vector<Int64> *>         (ADDRESS),
                    &testAllocator);
      const Obj& Y = mY;
      LOOP2_ASSERT(LINE1, LINE2, X == Y);
    } break;
    case Ot::e_DOUBLE_ARRAY: {
      Obj        mY(*static_cast<const bsl::vector<double> *>        (ADDRESS),
                    &testAllocator);
      const Obj& Y = mY;
      LOOP2_ASSERT(LINE1, LINE2, X == Y);
    } break;
    case Ot::e_STRING_ARRAY: {
      Obj        mY(*static_cast<const bsl::vector<bsl::string> *>   (ADDRESS),
                    &testAllocator);
      const Obj& Y = mY;
      LOOP2_ASSERT(LINE1, LINE2, X == Y);
    } break;
    case Ot::e_DATETIME_ARRAY: {
      Obj        mY(*static_cast<const bsl::vector<bdlt::Datetime> *>(ADDRESS),
                    &testAllocator);
      const Obj& Y = mY;
      LOOP2_ASSERT(LINE1, LINE2, X == Y);
    } break;
    case Ot::e_DATE_ARRAY: {
      Obj        mY(*static_cast<const bsl::vector<bdlt::Date> *>    (ADDRESS),
                   &testAllocator);
      const Obj& Y = mY;
      LOOP2_ASSERT(LINE1, LINE2, X == Y);
    } break;
    case Ot::e_TIME_ARRAY: {
      Obj        mY(*static_cast<const bsl::vector<bdlt::Time> *>    (ADDRESS),
                    &testAllocator);
      const Obj& Y = mY;
      LOOP2_ASSERT(LINE1, LINE2, X == Y);
    } break;
    default: {
      ASSERT(0);
    } break;
  //^-------------v
                };
            }
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // TESTING DEFAULT CONSTRUCTOR

        if (verbose) cout << "\nTesting default constructor.\n";

        Obj mX; const Obj& X = mX;

        ASSERT(false             == X.isRequired());
        ASSERT(false             == X.isHidden());
        ASSERT(false             == X.defaultValue().hasNonVoidType());
        ASSERT(Ot::e_VOID        == X.defaultValue().type());
        ASSERT(&defaultAllocator == X.defaultValue().allocator());
        ASSERT(&defaultAllocator == X.allocator());

        Obj mY(&testAllocator); const Obj& Y = mY;
        ASSERT(false             == Y.isRequired());
        ASSERT(false             == Y.isHidden());
        ASSERT(false             == Y.defaultValue().hasNonVoidType());
        ASSERT(Ot::e_VOID        == Y.defaultValue().type());
        ASSERT(&testAllocator    == Y.defaultValue().allocator());
        ASSERT(&testAllocator    == Y.allocator());

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        ASSERT(0 == testAllocator.numBytesInUse());
        ASSERT(0 == testAllocator.numMismatches());

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
