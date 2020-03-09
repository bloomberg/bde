// balcl_option.t.cpp                                                 -*-C++-*-

#include <balcl_option.h>

#include <balcl_constraint.h>
#include <balcl_optionvalue.h>
#include <balcl_optiontype.h>
#include <balcl_typeinfo.h>
#include <balcl_optioninfo.h>
#include <balcl_occurrenceinfo.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_time.h>

#include <bdlb_chartype.h>
#include <bdlb_numericparseutil.h>
#include <bdlb_printmethods.h> // 'bdlb::HasPrintMethod'

#include <bslim_testutil.h>

#include <bslmf_assert.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bsls_assert.h>
#include <bsls_types.h> // 'bsls::Types::Int64'

#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>     // 'bslstl::StringRef'
#include <bsl_vector.h>

#include <bsl_cstdlib.h>  // 'bsl::atoi'
#include <bsl_cassert.h>
#include <bsl_cstring.h> // 'bsl::strcmp'

using namespace BloombergLP;
using bsl::cerr;
using bsl::cout;
using bsl::endl;
using bsl::flush;

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// ----------------------------------------------------------------------------
// balcl::Option
// ----------------------------------------------------------------------------
// CREATORS
// [ 1] Option(bslma::Allocator *basicAllocator = 0);
// [ 1] Option(const Option& info, *bA = 0);
// [ 1] Option(const Option& original, *bA = 0);
// [ 1] ~Option();
//
// MANIPULATORS
// [ 1] Option& operator=(const Option&     rhs);
// [ 1] Option& operator=(const OptionInfo& rhs);
//
// ACCESSORS
// [ 1] operator const BloombergLP::balcl::Option &() const;
// [ 1] Option::ArgType argType() const;
// [ 1] const string& description() const;
// [ 1] bool isArray() const;
// [ 1] bool isLongTagValid(const char *longTag, ostream& stream) const;
// [ 1] bool isTagValid(ostream& stream) const;
// [ 1] bool isDescriptionValid(ostream& stream) const;
// [ 1] bool isNameValid(ostream& stream) const;
// [ 1] const char *longTag() const;
// [ 1] const string& name() const;
// [ 1] const OccurrenceInfo& occurrenceInfo() const;
// [ 1] char shortTag() const;
// [ 1] const string& tagString() const;
// [ 1] const TypeInfo& typeInfo() const;
//
// [ 1] bslma::Allocator *allocator() const;
// [ 1] ostream& print(ostream& stream, int level = 0, int spl = 4) const;
//
// FREE OPERATORS
// [ 1] bool operator==(const Option& lhs, rhs);
// [ 1] bool operator!=(const Option& lhs, rhs);
// [ 1] ostream& operator<<(ostream& strm, const Option& rhs);
// ----------------------------------------------------------------------------
// [xx] BREATHING TEST
// [ 1] TESTING 'balcl::Option'
// [xx] USAGE EXAMPLE

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

typedef balcl::Option                     Obj;

typedef balcl::Constraint                 Constraint;
typedef balcl::OccurrenceInfo             OccurrenceInfo;
typedef OccurrenceInfo::OccurrenceType    OccurType;
typedef balcl::OptionInfo                 OptionInfo;
typedef balcl::OptionType                 OptionType;
typedef balcl::TypeInfo                   TypeInfo;
typedef balcl::OptionValue                OptionValue;

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

enum { k_DATETIME_FIELD_WIDTH = 25
     ,     k_DATE_FIELD_WIDTH =  9
     ,     k_TIME_FIELD_WIDTH = 15 };

// ATTRIBUTES FOR 'balcl::Option'
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

static const struct {
    int         d_line;    // line number
    const char *d_name_p;  // name attribute
} OPTION_NAMES[] = {
    { L_, "" }
  , { L_, "A" }
  , { L_, "B" }
  , { L_, "ABCDEFGHIJKLMNOPQRSTUVWXYZ" }
};
enum { NUM_OPTION_NAMES = sizeof OPTION_NAMES / sizeof *OPTION_NAMES };

static const struct {
    int         d_line;           // line number
    const char *d_description_p;  // description attribute
} OPTION_DESCRIPTIONS[] = {
    { L_, "" }
  , { L_, "A" }
  , { L_, "B" }
  , { L_, "ABCDEFGHIJKLMNOPQRSTUVWXYZ" }
};
enum { NUM_OPTION_DESCRIPTIONS = sizeof  OPTION_DESCRIPTIONS
                               / sizeof *OPTION_DESCRIPTIONS };

// ATTRIBUTES FOR 'balcl::TypeInfo'
bool                        linkedBool;
char                        linkedChar;
short                       linkedShort;
int                         linkedInt;
Int64                       linkedInt64;
float                       linkedFloat;
double                      linkedDouble;
bsl::string                 linkedString(   bslma::Default::globalAllocator());
bdlt::Datetime              linkedDatetime;
bdlt::Date                  linkedDate;
bdlt::Time                  linkedTime;
bsl::vector<char>           linkedCharArray(bslma::Default::globalAllocator());
bsl::vector<short>          linkedShortArray(
                                            bslma::Default::globalAllocator());
bsl::vector<int>            linkedIntArray( bslma::Default::globalAllocator());
bsl::vector<Int64>          linkedInt64Array(
                                            bslma::Default::globalAllocator());
bsl::vector<float>          linkedFloatArray(
                                            bslma::Default::globalAllocator());
bsl::vector<double>         linkedDoubleArray(
                                            bslma::Default::globalAllocator());
bsl::vector<bsl::string>    linkedStringArray(
                                            bslma::Default::globalAllocator());
bsl::vector<bdlt::Datetime> linkedDatetimeArray(
                                            bslma::Default::globalAllocator());
bsl::vector<bdlt::Date>     linkedDateArray(bslma::Default::globalAllocator());
bsl::vector<bdlt::Time>     linkedTimeArray(bslma::Default::globalAllocator());


                        // =====================
                        // struct TestConstraint
                        // =====================

struct TestConstraint {
    // This 'struct' provides a namespace for functions, one for each
    // constraint type, used to initialize 'Constraint' objects for testing.

  private:
    static bool commonLogic(bsl::ostream& stream);
        // Return 's_constraintValue' and if 'false == s_contraintValue' output
        // an error message to the specified 'stream'.

  public:
    // PUBLIC CLASS DATA
    static bool s_constraintValue;
        // Global return value (for easier control).

    static bool     charFunc(const char           *, bsl::ostream& stream);
    static bool      intFunc(const int            *, bsl::ostream& stream);
    static bool    int64Func(const Int64          *, bsl::ostream& stream);
    static bool   doubleFunc(const double         *, bsl::ostream& stream);
    static bool   stringFunc(const bsl::string    *, bsl::ostream& stream);
    static bool datetimeFunc(const bdlt::Datetime *, bsl::ostream& stream);
    static bool     dateFunc(const bdlt::Date     *, bsl::ostream& stream);
    static bool     timeFunc(const bdlt::Time     *, bsl::ostream& stream);
        // Return 's_constraintValue' and if 'false == s_contraintValue' output
        // an error message to the specified 'stream'.  Note that the first
        // argument is ignored.
};

                        // ---------------------
                        // struct TestConstraint
                        // ---------------------

bool TestConstraint::commonLogic(bsl::ostream& stream)
{
    if (!s_constraintValue) {
        stream << "error" << flush;
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
   { L_, Ot::e_BOOL,           0,                    0                     }
 , { L_, Ot::e_CHAR,           0,                    0                     }
 , { L_, Ot::e_INT,            0,                    0                     }
 , { L_, Ot::e_INT64,          0,                    0                     }
 , { L_, Ot::e_DOUBLE,         0,                    0                     }
 , { L_, Ot::e_STRING,         0,                    0                     }
 , { L_, Ot::e_DATETIME,       0,                    0                     }
 , { L_, Ot::e_DATE,           0,                    0                     }
 , { L_, Ot::e_TIME,           0,                    0                     }
 , { L_, Ot::e_CHAR_ARRAY,     0,                    0                     }
 , { L_, Ot::e_INT_ARRAY,      0,                    0                     }
 , { L_, Ot::e_INT64_ARRAY,    0,                    0                     }
 , { L_, Ot::e_DOUBLE_ARRAY,   0,                    0                     }
 , { L_, Ot::e_STRING_ARRAY,   0,                    0                     }
 , { L_, Ot::e_DATETIME_ARRAY, 0,                    0                     }
 , { L_, Ot::e_DATE_ARRAY,     0,                    0                     }
 , { L_, Ot::e_TIME_ARRAY,     0,                    0                     }
 , { L_, Ot::e_BOOL,           &linkedBool,          0                     }
 , { L_, Ot::e_CHAR,           &linkedChar,          0                     }
 , { L_, Ot::e_INT,            &linkedInt,           0                     }
 , { L_, Ot::e_INT64,          &linkedInt64,         0                     }
 , { L_, Ot::e_DOUBLE,         &linkedDouble,        0                     }
 , { L_, Ot::e_STRING,         &linkedString,        0                     }
 , { L_, Ot::e_DATETIME,       &linkedDatetime,      0                     }
 , { L_, Ot::e_DATE,           &linkedDate,          0                     }
 , { L_, Ot::e_TIME,           &linkedTime,          0                     }
 , { L_, Ot::e_CHAR_ARRAY,     &linkedCharArray,     0                     }
 , { L_, Ot::e_INT_ARRAY,      &linkedIntArray,      0                     }
 , { L_, Ot::e_INT64_ARRAY,    &linkedInt64Array,    0                     }
 , { L_, Ot::e_DOUBLE_ARRAY,   &linkedDoubleArray,   0                     }
 , { L_, Ot::e_STRING_ARRAY,   &linkedStringArray,   0                     }
 , { L_, Ot::e_DATETIME_ARRAY, &linkedDatetimeArray, 0                     }
 , { L_, Ot::e_DATE_ARRAY,     &linkedDateArray,     0                     }
 , { L_, Ot::e_TIME_ARRAY,     &linkedTimeArray,     0                     }
 , { L_, Ot::e_BOOL,           0,                    // &testBoolConstraint }
                                                        0                     }
 , { L_, Ot::e_CHAR,           0,                    &testCharConstraint    }
 , { L_, Ot::e_INT,            0,                    &testIntConstraint     }
 , { L_, Ot::e_INT64,          0,                    &testInt64Constraint   }
 , { L_, Ot::e_DOUBLE,         0,                    &testDoubleConstraint  }
 , { L_, Ot::e_STRING,         0,                    &testStringConstraint  }
 , { L_, Ot::e_DATETIME,       0,                    &testDatetimeConstraint}
 , { L_, Ot::e_DATE,           0,                    &testDateConstraint    }
 , { L_, Ot::e_TIME,           0,                    &testTimeConstraint    }
 , { L_, Ot::e_CHAR_ARRAY,     0,                    &testCharConstraint    }
 , { L_, Ot::e_INT_ARRAY,      0,                    &testIntConstraint     }
 , { L_, Ot::e_INT64_ARRAY,    0,                    &testInt64Constraint   }
 , { L_, Ot::e_DOUBLE_ARRAY,   0,                    &testDoubleConstraint  }
 , { L_, Ot::e_STRING_ARRAY,   0,                    &testStringConstraint  }
 , { L_, Ot::e_DATETIME_ARRAY, 0,                    &testDatetimeConstraint}
 , { L_, Ot::e_DATE_ARRAY,     0,                    &testDateConstraint    }
 , { L_, Ot::e_TIME_ARRAY,     0,                    &testTimeConstraint    }
 , { L_, Ot::e_BOOL,           &linkedBool,          0                     }
 , { L_, Ot::e_CHAR,           &linkedChar,          &testCharConstraint    }
 , { L_, Ot::e_INT,            &linkedInt,           &testIntConstraint     }
 , { L_, Ot::e_INT64,          &linkedInt64,         &testInt64Constraint   }
 , { L_, Ot::e_DOUBLE,         &linkedDouble,        &testDoubleConstraint  }
 , { L_, Ot::e_STRING,         &linkedString,        &testStringConstraint  }
 , { L_, Ot::e_DATETIME,       &linkedDatetime,      &testDatetimeConstraint}
 , { L_, Ot::e_DATE,           &linkedDate,          &testDateConstraint    }
 , { L_, Ot::e_TIME,           &linkedTime,          &testTimeConstraint    }
 , { L_, Ot::e_CHAR_ARRAY,     &linkedCharArray,     &testCharConstraint    }
 , { L_, Ot::e_INT_ARRAY,      &linkedIntArray,      &testIntConstraint     }
 , { L_, Ot::e_INT64_ARRAY,    &linkedInt64Array,    &testInt64Constraint   }
 , { L_, Ot::e_DOUBLE_ARRAY,   &linkedDoubleArray,   &testDoubleConstraint  }
 , { L_, Ot::e_STRING_ARRAY,   &linkedStringArray,   &testStringConstraint  }
 , { L_, Ot::e_DATETIME_ARRAY, &linkedDatetimeArray, &testDatetimeConstraint}
 , { L_, Ot::e_DATE_ARRAY,     &linkedDateArray,     &testDateConstraint    }
 , { L_, Ot::e_TIME_ARRAY,     &linkedTimeArray,     &testTimeConstraint    }
};
enum { NUM_OPTION_TYPEINFO = sizeof  OPTION_TYPEINFO
                           / sizeof *OPTION_TYPEINFO };

// ATTRIBUTES FOR 'balcl::OccurrenceInfo'
static const struct {
    int        d_line;  // line number
    OccurType  d_type;  // name attribute
} OPTION_OCCURRENCES[] = {
    { L_, OccurrenceInfo::e_REQUIRED }
  , { L_, OccurrenceInfo::e_OPTIONAL }
  , { L_, OccurrenceInfo::e_HIDDEN   }
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

bool                     linkedBoolA;
bool                     linkedBoolB;
bool                     linkedBoolC;
bsl::string              linkedString1     (bslma::Default::globalAllocator());
bsl::vector<bsl::string> linkedStringArray1(bslma::Default::globalAllocator());

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
                     OccurrenceInfo::e_OPTIONAL
                 },
                 {
                     "b|bLong",
                     "Name_b",
                     "Description for b",
                     TypeInfo(&linkedBoolB),
                     OccurrenceInfo::e_OPTIONAL
                 },
                 {
                     "c|cLong",
                     "Name_c",
                     "Description for c",
                     TypeInfo(&linkedBoolC),
                     OccurrenceInfo::e_OPTIONAL
                 },
                 {
                     "i|iLong",
                     "Name_i",
                     "Description for i",
                     TypeInfo(&linkedInt),
                     OccurrenceInfo::e_OPTIONAL
                 },
                 {
                     "d|dLong",
                     "Name_d",
                     "Description for d",
                     TypeInfo(&linkedDouble),
                     OccurrenceInfo::e_OPTIONAL
                 },
                 {
                     "s|sLong",
                     "Name_s",
                     "Description for s",
                     TypeInfo(&linkedString),
                     OccurrenceInfo::e_OPTIONAL
                 },
                 {
                     "I|ILong",
                     "Name_I",
                     "Description for I",
                     TypeInfo(&linkedIntArray),
                     OccurrenceInfo::e_OPTIONAL
                 },
                 {
                     "D|DLong",
                     "Name_D",
                     "Description for D",
                     TypeInfo(&linkedDoubleArray),
                     OccurrenceInfo::e_OPTIONAL
                 },
                 {
                     "S|SLong",
                     "Name_S",
                     "Description for S",
                     TypeInfo(&linkedStringArray),
                     OccurrenceInfo::e_OPTIONAL
                 },
                 {
                     "",
                     "Name1",
                     "Description1",
                     TypeInfo(&linkedString1),
                     OccurrenceInfo::e_OPTIONAL
                 },
                 {
                     "",
                     "Name2",
                     "Description2",
                     TypeInfo(&linkedStringArray1),
                     OccurrenceInfo::e_OPTIONAL
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

// BDE_VERIFY pragma: -FABC01  // Function ... not in alphabetic order
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
// BDE_VERIFY pragma: +FABC01  // Function ... not in alphabetic order

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

int ParserImpUtil::skipWhiteSpace(const char **endPos, const char *inputString)
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
        if (TypesParser::parse(&value, endPos, *endPos)) {
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

// BDE_VERIFY pragma: +FABC01  // Function ... not in alphabetic order

                        // ======================
                        // function setConstraint
                        // ======================

void setConstraint(TypeInfo *typeInfo, ElemType type, const void *address)
    // Set the constraint of the specified 'typeInfo' to the function at the
    // specified 'address' of the signature corresponding to the specified
    // 'type'.  The behavior is undefined unless 'Ot::e_VOID != type' and
    // 'Ot::e_BOOL != type'.
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
        ASSERT(!"Reached");
      } break;
      case Ot::e_BOOL: {
        ASSERT(!"Reached");
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
        BSLS_ASSERT(!"Reached");
      } break;
    };

#undef CASE

}

                         // ==========================
                         // function setLinkedVariable
                         // ==========================

void setLinkedVariable(TypeInfo *typeInfo, ElemType type, void *address)
    // Set the linked variable of the specified 'typeInfo' to the variable at
    // the specified 'address'.  The behavior is undefined unless 'address' can
    // be cast to a pointer to the type associated with the specified 'type'
    // (i.e., 'balcl::OptionType::EnumToType<type>::type *').
{
    switch (type) {
      case Ot::e_BOOL: {
        typeInfo->setLinkedVariable(static_cast<bool *>             (address));
      } break;
      case Ot::e_CHAR: {
        typeInfo->setLinkedVariable(static_cast<char *>             (address));
      } break;
      case Ot::e_INT: {
        typeInfo->setLinkedVariable(static_cast<int *>              (address));
      } break;
      case Ot::e_INT64: {
        typeInfo->setLinkedVariable(static_cast<Int64 *>            (address));
      } break;
      case Ot::e_DOUBLE: {
        typeInfo->setLinkedVariable(static_cast<double *>           (address));
      } break;
      case Ot::e_STRING: {
        typeInfo->setLinkedVariable(static_cast<bsl::string *>      (address));
      } break;
      case Ot::e_DATETIME: {
        typeInfo->setLinkedVariable(static_cast<bdlt::Datetime *>   (address));
      } break;
      case Ot::e_DATE: {
        typeInfo->setLinkedVariable(static_cast<bdlt::Date *>       (address));
      } break;
      case Ot::e_TIME: {
        typeInfo->setLinkedVariable(static_cast<bdlt::Time *>       (address));
      } break;
      case Ot::e_CHAR_ARRAY: {
        typeInfo->setLinkedVariable(static_cast<bsl::vector<char> *>(address));
      } break;
      case Ot::e_INT_ARRAY: {
        typeInfo->setLinkedVariable(static_cast<bsl::vector<int> *> (address));
      } break;
      case Ot::e_INT64_ARRAY: {
        typeInfo->setLinkedVariable(static_cast<bsl::vector<Int64> *>(
                                                                     address));
      } break;
      case Ot::e_DOUBLE_ARRAY: {
        typeInfo->setLinkedVariable(static_cast<bsl::vector<double> *>(
                                                                     address));
      } break;
      case Ot::e_STRING_ARRAY: {
        typeInfo->setLinkedVariable(static_cast<bsl::vector<bsl::string> *>(
                                                                     address));
      } break;
      case Ot::e_DATETIME_ARRAY: {
        typeInfo->setLinkedVariable(static_cast<bsl::vector<bdlt::Datetime> *>(
                                                                     address));
      } break;
      case Ot::e_DATE_ARRAY: {
        typeInfo->setLinkedVariable(static_cast<bsl::vector<bdlt::Date> *>(
                                                                     address));
      } break;
      case Ot::e_TIME_ARRAY: {
        typeInfo->setLinkedVariable(static_cast<bsl::vector<bdlt::Time> *>(
                                                                     address));
      } break;
      default: {
        ASSERT(0);
      } break;
    };
}

                         // ================
                         // function setType
                         // ================

void setType(TypeInfo *typeInfo, ElemType type)
    // Set the 'balcl::OptionType' element of the specified 'typeInfo' to the
    // specified 'type'.  The behavior is undefined unless
    // 'balcl::OptionType::e_VOID != type').  Note that this resets both the
    // linked variable and constraint of 'typeInfo'.
{
    switch (type) {
      case Ot::e_VOID: {
        BSLS_ASSERT(!"Not reachable.");
      } break;
      case Ot::e_BOOL: {
        typeInfo->setLinkedVariable(Ot::k_BOOL);
      } break;
      case Ot::e_CHAR: {
        typeInfo->setLinkedVariable(Ot::k_CHAR);
      } break;
      case Ot::e_INT: {
        typeInfo->setLinkedVariable(Ot::k_INT);
      } break;
      case Ot::e_INT64: {
        typeInfo->setLinkedVariable(Ot::k_INT64);
      } break;
      case Ot::e_DOUBLE: {
        typeInfo->setLinkedVariable(Ot::k_DOUBLE);
      } break;
      case Ot::e_STRING: {
        typeInfo->setLinkedVariable(Ot::k_STRING);
      } break;
      case Ot::e_DATETIME: {
        typeInfo->setLinkedVariable(Ot::k_DATETIME);
      } break;
      case Ot::e_DATE: {
        typeInfo->setLinkedVariable(Ot::k_DATE);
      } break;
      case Ot::e_TIME: {
        typeInfo->setLinkedVariable(Ot::k_TIME);
      } break;
      case Ot::e_CHAR_ARRAY: {
        typeInfo->setLinkedVariable(Ot::k_CHAR_ARRAY);
      } break;
      case Ot::e_INT_ARRAY: {
        typeInfo->setLinkedVariable(Ot::k_INT_ARRAY);
      } break;
      case Ot::e_INT64_ARRAY: {
        typeInfo->setLinkedVariable(Ot::k_INT64_ARRAY);
      } break;
      case Ot::e_DOUBLE_ARRAY: {
        typeInfo->setLinkedVariable(Ot::k_DOUBLE_ARRAY);
      } break;
      case Ot::e_STRING_ARRAY: {
        typeInfo->setLinkedVariable(Ot::k_STRING_ARRAY);
      } break;
      case Ot::e_DATETIME_ARRAY: {
        typeInfo->setLinkedVariable(Ot::k_DATETIME_ARRAY);
      } break;
      case Ot::e_DATE_ARRAY: {
        typeInfo->setLinkedVariable(Ot::k_DATE_ARRAY);
      } break;
      case Ot::e_TIME_ARRAY: {
        typeInfo->setLinkedVariable(Ot::k_TIME_ARRAY);
      } break;
    };
}

                          // =======================
                          // function createTypeInfo
                          // =======================

void createTypeInfo(TypeInfo *typeInfo,
                    ElemType  type,
                    void     *variable = 0,
                    void     *constraint = 0)
    // Set the specified 'typeInfo' to have the specified 'type'.  Optionally
    // specify 'variable', the address of a linked variable and 'constraint',
    // the address of a constraint.  The behavior is undefined unless unless
    // 'balcl::OptionType::e_VOID != type', 'variable' is 0 or can be cast to
    // 'balcl::OptionType::EnumToType<type>::type', 'constraint' is 0 or can be
    // cast to the type defined by 'Constraint' for 'type', and if
    // 'balcl::OptionType::e_BOOL == type' then 'constraint' must be 0.
{
    BSLS_ASSERT(typeInfo);

    if (balcl::OptionType::e_BOOL == type) {
        BSLS_ASSERT(0 == constraint);
    }

    setType(typeInfo, type);

    if (variable) {
        setLinkedVariable(typeInfo, type, variable);
    }
    if (constraint) {
        setConstraint(typeInfo, type, constraint);
    }
}

// BDE_VERIFY pragma: -AR01  // Type using allocator is returned by value
TypeInfo createTypeInfo(ElemType  type,
                        void     *variable = 0,
                        void     *constraint = 0)
    // Return (by value) 'TypeInfo' object in the specified having the
    // specified 'type'.  Optionally specify 'variable', an address to be
    // linked to the option.  Optionally specify 'constraint' on the value of
    // the option.  The returned object uses the currently defined default
    // allocator.  The behavior is undefined unless unless
    // 'balcl::OptionType::e_VOID != type',
    // 'balcl::OptionType::e_BOOL != type', 'variable' is 0 or can be cast to
    // 'balcl::OptionType::EnumToType<type>::type', and 'constraint' is 0 or
    // can be cast to the type defined by 'Constraint' for 'type' .
{
    TypeInfo result;
    createTypeInfo(&result, type, variable, constraint);
    return result;
}
// BDE_VERIFY pragma: +AR01  // Type using allocator is returned by value

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

                          // =============================
                          // function createOccurrenceInfo
                          // =============================

// BDE_VERIFY pragma: -AR01  // Type using allocator is returned by value
OccurrenceInfo createOccurrenceInfo(OccurType   occurrenceType,
                                    ElemType    type,
                                    const void *variable)
    // Return (by value) an 'OccurrenceInfo' object having the specified
    // 'occurrenceType' and having a default value of the specified 'type' and
    // a value determined by the specified 'variable'.  The returned object
    // uses the currently define default allocator.  The behavior is undefined
    // unless 'balcl::OptionType::e_VOID != type' and 'variable' can be cast to
    // a pointer of 'balcl::OptionType::EnumToType<type>::type',
{
    BSLS_ASSERT(balcl::OptionType::e_VOID != type);

    OccurrenceInfo result(occurrenceType);

    if (occurrenceType != OccurrenceInfo::e_REQUIRED && variable) {
        OptionValue defaultValue(type);
        setOptionValue(&defaultValue, variable, type);

        result.setDefaultValue(defaultValue);
    }

    return result;
}
// BDE_VERIFY pragma: +AR01  // Type using allocator is returned by value

                          // ========================
                           // function parseTypeInfo
                          // ========================

int parseTypeInfo(const char      **endpos,
                  const TypeInfo&   typeInfo,
                  const char       *input)
    // Parse the specified 'input' for a value and verify that this value
    // matches the specified 'typeInfo' (at least in 'balcl::OptionType' type)
    // and return 0 if parsing and verification succeed.  Return a non-zero
    // value if parsing fails or if the value parsed does not match 'typeInfo',
    // and return in the specified 'endpos' the first unsuccessful parsing
    // position.
{
    typedef ParserImpUtil Parser;
    enum { SUCCESS = 0, FAILURE = -1 };

    bsl::string ptrString;

    if (Parser::skipWhiteSpace   (&input, input)
     || Parser::skipRequiredToken(&input, input, "{")
     || Parser::skipWhiteSpace   (&input, input)
     || Parser::skipRequiredToken(&input, input, "TYPE")
     || Parser::skipWhiteSpace   (&input, input)
     || Parser::skipRequiredToken(&input, input, Ot::toAscii(typeInfo.type())))
    {
        *endpos = input;
        return FAILURE;                                               // RETURN
    }

    if (typeInfo.linkedVariable()) {
        if (Parser::skipWhiteSpace(&input, input)
         || Parser::skipRequiredToken(&input, input, "VARIABLE")
         || Parser::skipWhiteSpace(&input, input))
        {
            *endpos = input;
            return FAILURE;                                           // RETURN
        }
        else {
            // Retrieve the pointer address.
            ptrString.clear();
            while(!bdlb::CharType::isSpace(*input)) {
                ptrString += *input;
                ++input;
            }

            bsl::ostringstream oss;
            oss << static_cast<void *>(typeInfo.linkedVariable());
            if (oss.str() != ptrString) {
                *endpos = input;
                return FAILURE;                                       // RETURN
            }
        }
    }

    if (Parser::skipWhiteSpace(&input, input)
     || Parser::skipRequiredToken(&input, input, "CONSTRAINT")
     || Parser::skipWhiteSpace(&input, input))
    {
        *endpos = input;
        return FAILURE;                                               // RETURN
    }
    else {
        // Retrieve the pointer address.
        ptrString.clear();
        while(!bdlb::CharType::isSpace(*input)) {
            ptrString += *input;
            ++input;
        }

        bsl::ostringstream oss;
        oss << static_cast<void *>(typeInfo.constraint().get());
        if (oss.str() != ptrString) {
            *endpos = input;
            return FAILURE;                                           // RETURN
        }
    }

    if (Parser::skipWhiteSpace   (&input, input)
     || Parser::skipRequiredToken(&input, input, "}"))
    {
        *endpos = input;
        return FAILURE;                                               // RETURN
    }

    *endpos = input;
    return SUCCESS;
}

                        // ============================
                        // function parseOccurrenceInfo
                        // ============================

int parseOccurrenceInfo(const char            **endpos,
                        const OccurrenceInfo&   occurrenceInfo,
                        const char             *input)
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
        if (OccurrenceInfo::e_REQUIRED ==
                                             occurrenceInfo.occurrenceType()) {
            if (Parser::skipRequiredToken(endpos, input, "REQUIRED")) {
                return FAILURE;                                       // RETURN
            }
            return SUCCESS;                                           // RETURN
        }
        if (OccurrenceInfo::e_OPTIONAL ==
                                             occurrenceInfo.occurrenceType()) {
            if (Parser::skipRequiredToken(endpos, input, "OPTIONAL")) {
                return FAILURE;                                       // RETURN
            }
            return SUCCESS;                                           // RETURN
        }
        if (OccurrenceInfo::e_HIDDEN == occurrenceInfo.occurrenceType()) {
            if (Parser::skipRequiredToken(endpos, input, "HIDDEN")) {
                return FAILURE;                                       // RETURN
            }
            return SUCCESS;                                           // RETURN
        }
        *endpos = input;
        return FAILURE;                                               // RETURN
    } else {
        ++input; // Parser::skipRequiredToken(&input, input, "{");
        if (OccurrenceInfo::e_OPTIONAL ==
                                             occurrenceInfo.occurrenceType()) {
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
                if (TypesParser::parse(&intValue, endpos, input)
                 || intValue != occurrenceInfo.defaultValue().the<Ot::Int>()) {
                    return FAILURE;                                   // RETURN
                }
              } break;
              case Ot::e_INT64: {
                Int64 int64Value;
                if (TypesParser::parse(&int64Value, endpos, input)
                 || int64Value !=
                              occurrenceInfo.defaultValue().the<Ot::Int64>()) {
                    return FAILURE;                                   // RETURN
                }
              } break;
              case Ot::e_DOUBLE: {
                double doubleValue;
                if (TypesParser::parse(&doubleValue, endpos, input)
                 || doubleValue !=
                             occurrenceInfo.defaultValue().the<Ot::Double>()) {
                    // There is no guaranteed round-trip on floating point I/O.
                    // Return 'FAILURE'.
                }
              } break;
              case Ot::e_STRING: {
                bsl::string stringValue;
                if (TypesParser::parse(&stringValue, endpos, input)
                 || stringValue !=
                             occurrenceInfo.defaultValue().the<Ot::String>()) {
                    return FAILURE;                                   // RETURN
                }
              } break;
              case Ot::e_DATETIME: {
                bdlt::Datetime datetimeValue;
                if (TypesParser::parse(&datetimeValue, endpos, input)
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
                if (TypesParser::parse(&dateValue, endpos, input)
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
                if (TypesParser::parse(&timeValue, endpos, input)
                 || timeValue !=
                               occurrenceInfo.defaultValue().the<Ot::Time>()) {
                    return FAILURE;                                   // RETURN
                }
              } break;
              case Ot::e_CHAR_ARRAY: {
                bsl::string charArrayAsString;
                if (TypesParser::parseQuotedString(
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

                        // ====================
                        // function parseOption
                        // ====================

int parseOption(const char **endpos,
                const Obj&   option,
                const char  *input)
    // Parse the specified 'input' for a value and verify that this value
    // matches the specified 'option' and return 0 if parsing and verification
    // succeed.  Return a non-zero value if parsing fails or if the value
    // parsed does not match 'option', and return in the specified 'endpos' the
    // first unsuccessful parsing position.
{
    typedef ParserImpUtil Parser;
    enum { SUCCESS = 0, FAILURE = -1 };

    bsl::string tagString, nameString, descriptionString;

    if (Parser::skipWhiteSpace   (&input, input)
     || Parser::skipRequiredToken(&input, input, "{")
     || Parser::skipWhiteSpace   (&input, input))
    {
        *endpos = input;
        return FAILURE;                                               // RETURN
    }

    if (Parser::skipRequiredToken(&input, input, "NON_OPTION")) {
        if (Parser::skipRequiredToken(&input, input, "TAG")
         || Parser::skipWhiteSpace   (&input, input)
         || TypesParser::parseQuotedString(&tagString, &input, input)
         || tagString != option.tagString())
        {
            *endpos = input;
            return FAILURE - 1;                                       // RETURN
        }
    }

    if (Parser::skipWhiteSpace   (&input, input)
     || Parser::skipRequiredToken(&input, input, "NAME")
     || Parser::skipWhiteSpace   (&input, input)
     || TypesParser::parseQuotedString(&nameString, &input, input)
     || nameString != option.name())
    {
        *endpos = input;
        return FAILURE - 2;                                           // RETURN
    }

    if (Parser::skipWhiteSpace   (&input, input)
     || Parser::skipRequiredToken(&input, input, "DESCRIPTION")
     || Parser::skipWhiteSpace   (&input, input)
     || TypesParser::parseQuotedString(&descriptionString, &input, input)
     || descriptionString != option.description())
    {
        *endpos = input;
        return FAILURE - 3;                                           // RETURN
    }
    if (Parser::skipWhiteSpace   (&input, input)
     || Parser::skipRequiredToken(&input, input, "TYPE_INFO"))
    {
        *endpos = input;
        return FAILURE - 4;                                           // RETURN
    }
    if (parseTypeInfo(&input, option.typeInfo(), input))
    {
        *endpos = input;
        return FAILURE - 5;                                           // RETURN
    }
    if (Parser::skipWhiteSpace   (&input, input)
     || Parser::skipRequiredToken(&input, input, "OCCURRENCE_INFO"))
    {
        *endpos = input;
        return FAILURE - 6;                                           // RETURN
    }

    if (parseOccurrenceInfo(&input, option.occurrenceInfo(), input))
    {
        *endpos = input;
        return FAILURE - 7;                                           // RETURN
    }

    if (Parser::skipWhiteSpace   (&input, input)
     || Parser::skipRequiredToken(&input, input, "}"))
    {
        *endpos = input;
        return FAILURE - 8;                                           // RETURN
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

int main(int argc, const char *argv[])  {
    int                test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int             verbose = argc > 2;
    int         veryVerbose = argc > 3;
    int     veryVeryVerbose = argc > 4;
    int veryVeryVeryVerbose = argc > 5;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    bslma::Allocator *const k_DFLT_ALLOC = bslma::Default::defaultAllocator();

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'balcl::Option'
        //
        // Concerns:
        //: 1 'balcl::Option' is a full-blown in-core
        //:   value-semantic version of 'balcl::OptionInfo', that can
        //:   be initialized and converted to its simpler 'struct' variant.  We
        //:   need to make sure it is exception safe, uses 'bslma' allocators
        //:   properly so it can be stored in a container, and has a value as
        //:   expected.
        //
        // Plan:
        //: 1 Follow the standard attribute test driver sequence.  Beyond that,
        //:   there are no special concerns.
        //
        // We choose as primary manipulator the constructor:
        //..
        //  balcl::Option(const OptionInfo&);
        //..
        //  and as basic accessors the attribute methods:
        //..
        //  bsl::string tagString() const;
        //  bsl::string name() const;
        //  bsl::string description() const;
        //  TypeInfo typeInfo() const;
        //  OccurrenceInfo occurrenceInfo() const;
        //..
        //
        // Testing:
        //  TESTING 'balcl::Option'
        //  Option(bslma::Allocator *basicAllocator = 0);
        //  Option(const Option& info, *bA = 0);
        //  Option(const Option& original, *bA = 0);
        //  ~Option();
        //  Option& operator=(const Option&     rhs);
        //  Option& operator=(const OptionInfo& rhs);
        //  operator const BloombergLP::balcl::Option &() const;
        //  Option::ArgType argType() const;
        //  const string& description() const;
        //  bool isArray() const;
        //  bool isLongTagValid(const char *longTag, ostream& stream) const;
        //  bool isTagValid(ostream& stream) const;
        //  bool isDescriptionValid(ostream& stream) const;
        //  bool isNameValid(ostream& stream) const;
        //  const char *longTag() const;
        //  const string& name() const;
        //  const OccurrenceInfo& occurrenceInfo() const;
        //  char shortTag() const;
        //  const string& tagString() const;
        //  const TypeInfo& typeInfo() const;
        //  bslma::Allocator *allocator() const;
        //  ostream& print(ostream& stream, int level = 0, int spl = 4) const;
        //  bool operator==(const Option& lhs, rhs);
        //  bool operator!=(const Option& lhs, rhs);
        //  ostream& operator<<(ostream& strm, const Option& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'balcl::Option'" << endl
                          << "=======================" << endl;

        bslma::TestAllocator testAllocator(veryVeryVeryVerbose);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 2 OF VALUE-SEMANTIC TEST DRIVER
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP)

        if (verbose) cout << "\n\tTesting primary manipulators." << endl;

        if (verbose) cout << "\t\tWithout passing in an allocator." << endl;

// BDE_VERIFY pragma: -TP21    // Loops must contain very verbose action
// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
        for (int i = 0; i < NUM_OPTION_TAGS;         ++i) {
        for (int j = 0; j < NUM_OPTION_NAMES;        ++j) {
        for (int k = 0; k < NUM_OPTION_DESCRIPTIONS; ++k) {
        for (int l = 0; l < NUM_OPTION_TYPEINFO;     ++l) {
        for (int m = 0; m < NUM_OPTION_OCCURRENCES;  ++m) {
            const int n = l % NUM_OPTION_DEFAULT_VALUES;
                // Instead of:
                //..
                //  for (int n = 0; n < NUM_OPTION_DEFAULT_VALUES; ++n)
                //..
                // Valid only because of the way we organized the table data:
                //..
                    ASSERT(OPTION_TYPEINFO[l].d_type ==
                                              OPTION_DEFAULT_VALUES[n].d_type);
                //..
            const int       LINE1      = OPTION_TAGS[i].d_line;
            const char     *TAG        = OPTION_TAGS[i].d_tag_p;

            const int       LINE2      = OPTION_NAMES[j].d_line;
            const char     *NAME       = OPTION_NAMES[j].d_name_p;

            const int       LINE3      = OPTION_DESCRIPTIONS[k].d_line;
            const char     *DESC       = OPTION_DESCRIPTIONS[k].
                                                               d_description_p;

            const int       LINE4      = OPTION_TYPEINFO[l].d_line;
            const ElemType  TYPE       = OPTION_TYPEINFO[l].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[l].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[l].d_constraint_p;

            const int       LINE5      = OPTION_OCCURRENCES[l].d_line;
            const OccurType OTYPE      = OPTION_OCCURRENCES[l].d_type;

            const void     *DEFAULT_VALUE = OPTION_DEFAULT_VALUES[n].d_value_p;

            if (TYPE != OPTION_DEFAULT_VALUES[n].d_type) {
                continue;
            }

            const TypeInfo       EXP_TYPEINFO        = u::createTypeInfo(
                                                                   TYPE,
                                                                   VARIABLE,
                                                                   CONSTRAINT);
            const OccurrenceInfo EXP_OCCURRENCE_INFO = u::createOccurrenceInfo(
                                                                OTYPE,
                                                                TYPE,
                                                                DEFAULT_VALUE);
            const OptionInfo     EXP = { TAG,
                                         NAME,
                                         DESC,
                                         EXP_TYPEINFO,
                                         EXP_OCCURRENCE_INFO
                                       };

            Obj mX(EXP);  const Obj& X = mX;      // TEST HERE

            ASSERT(k_DFLT_ALLOC == X.allocator());

            if (veryVerbose) {
                T_ P_(LINE1) P_(LINE2) P_(LINE3) P_(LINE4) P(LINE5)
                T_ P(EXP)
                T_ P(X)
            }
            LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,
                         TAG == X.tagString());
            LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,
                         NAME == X.name());
            LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,
                         DESC == X.description());
            LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,
                         EXP_TYPEINFO == X.typeInfo());
            LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,
                         EXP_OCCURRENCE_INFO == X.occurrenceInfo());
        }
        }
        }
        }
        }
// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line
// BDE_VERIFY pragma: +TP21    // Loops must contain very verbose action

        bslma::TestAllocator         defaultAllocator(veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&defaultAllocator);

        if (verbose) cout << "\t\tPassing in an allocator." << endl
                           << "\t\t\tWith no exceptions." << endl;

        const bool HAS_BSLMA_ALLOCATOR_TRAIT =
                                         bslma::UsesBslmaAllocator<Obj>::value;

        ASSERT(HAS_BSLMA_ALLOCATOR_TRAIT);

// BDE_VERIFY pragma: -TP21    // Loops must contain very verbose action
// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
        for (int i = 0; i < NUM_OPTION_TAGS;         ++i) {
        for (int j = 0; j < NUM_OPTION_NAMES;        ++j) {
        for (int k = 0; k < NUM_OPTION_DESCRIPTIONS; ++k) {
        for (int l = 0; l < NUM_OPTION_TYPEINFO;     ++l) {
        for (int m = 0; m < NUM_OPTION_OCCURRENCES;  ++m) {
            const int n = l % NUM_OPTION_DEFAULT_VALUES;
                // Instead of:
                //..
                //  for (int n = 0; n < NUM_OPTION_DEFAULT_VALUES; ++n)
                //..
                // Valid only because of the way we organized the table data:
                //..
                   ASSERT(OPTION_TYPEINFO[l].d_type ==
                                              OPTION_DEFAULT_VALUES[n].d_type);
                //..
                //
            const int       LINE1      = OPTION_TAGS[i].d_line;
            const char     *TAG        = OPTION_TAGS[i].d_tag_p;

            const int       LINE2      = OPTION_NAMES[j].d_line;
            const char     *NAME       = OPTION_NAMES[j].d_name_p;

            const int       LINE3      = OPTION_DESCRIPTIONS[k].d_line;
            const char     *DESC       = OPTION_DESCRIPTIONS[k].
                                                               d_description_p;

            const int       LINE4      = OPTION_TYPEINFO[l].d_line;
            const ElemType  TYPE       = OPTION_TYPEINFO[l].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[l].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[l].d_constraint_p;

            const int       LINE5      = OPTION_OCCURRENCES[m].d_line;
            const OccurType OTYPE      = OPTION_OCCURRENCES[m].d_type;

            const void *DEFAULT_VALUE  = OPTION_DEFAULT_VALUES[n].d_value_p;

            if (TYPE != OPTION_DEFAULT_VALUES[n].d_type) {
                continue;
            }

            const TypeInfo       EXP_TYPEINFO        = u::createTypeInfo(
                                                                   TYPE,
                                                                   VARIABLE,
                                                                   CONSTRAINT);
            const OccurrenceInfo EXP_OCCURRENCE_INFO = u::createOccurrenceInfo(
                                                                OTYPE,
                                                                TYPE,
                                                                DEFAULT_VALUE);
            const OptionInfo     EXP = { TAG,
                                         NAME,
                                         DESC,
                                         EXP_TYPEINFO,
                                         EXP_OCCURRENCE_INFO
                                       };

            const bsls::Types::Int64 NUM_BYTES =
                                              defaultAllocator.numBytesInUse();
            {
                Obj mX(EXP, &testAllocator);  const Obj& X = mX;

                ASSERT(&testAllocator == X.allocator());

                LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,
                             TAG == X.tagString());
                LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,
                             NAME == X.name());
                LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,
                             DESC == X.description());
                LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,
                             EXP_TYPEINFO == X.typeInfo());
                LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,
                             EXP_OCCURRENCE_INFO == X.occurrenceInfo());
            }
            LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,
                         0 == testAllocator.numBytesInUse());
            LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,
                         0 == testAllocator.numMismatches());
            LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,
                         NUM_BYTES == defaultAllocator.numBytesInUse());
        }
        }
        }
        }
        }
// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line
// BDE_VERIFY pragma: +TP21    // Loops must contain very verbose action

        if (verbose) cout << "\t\t\tWith exceptions." << endl;

// BDE_VERIFY pragma: -TP21    // Loops must contain very verbose action
// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
        for (int i = 0; i < NUM_OPTION_TAGS;         ++i) {
        for (int j = 0; j < NUM_OPTION_NAMES;        ++j) {
        for (int k = 0; k < NUM_OPTION_DESCRIPTIONS; ++k) {
        for (int l = 0; l < NUM_OPTION_TYPEINFO;     ++l) {
        for (int m = 0; m < NUM_OPTION_OCCURRENCES;  ++m) {
            const int n = l % NUM_OPTION_DEFAULT_VALUES;
                // Instead of:
                //..
                //  for (int n = 0; n < NUM_OPTION_DEFAULT_VALUES; ++n)
                //..
                // Valid only because of the way we organized the table data:
                //..
                   ASSERT(OPTION_TYPEINFO[l].d_type ==
                                              OPTION_DEFAULT_VALUES[n].d_type);
                //..
            const int       LINE1      = OPTION_TAGS[i].d_line;
            const char     *TAG        = OPTION_TAGS[i].d_tag_p;

            const int       LINE2      = OPTION_NAMES[j].d_line;
            const char     *NAME       = OPTION_NAMES[j].d_name_p;

            const int       LINE3      = OPTION_DESCRIPTIONS[k].d_line;
            const char     *DESC       = OPTION_DESCRIPTIONS[k].
                                                               d_description_p;

            const int       LINE4      = OPTION_TYPEINFO[l].d_line;
            const ElemType  TYPE       = OPTION_TYPEINFO[l].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[l].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[l].d_constraint_p;

            const int       LINE5      = OPTION_OCCURRENCES[m].d_line;
            const OccurType OTYPE      = OPTION_OCCURRENCES[m].d_type;

            const void  *DEFAULT_VALUE = OPTION_DEFAULT_VALUES[n].d_value_p;

            if (TYPE != OPTION_DEFAULT_VALUES[n].d_type) {
                continue;
            }

            const TypeInfo       EXP_TYPEINFO        = u::createTypeInfo(
                                                                   TYPE,
                                                                   VARIABLE,
                                                                   CONSTRAINT);
            const OccurrenceInfo EXP_OCCURRENCE_INFO = u::createOccurrenceInfo(
                                                                OTYPE,
                                                                TYPE,
                                                                DEFAULT_VALUE);
            const OptionInfo     EXP = { TAG,
                                         NAME,
                                         DESC,
                                         EXP_TYPEINFO,
                                         EXP_OCCURRENCE_INFO
                                       };

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                Obj mX(EXP, &testAllocator);  const Obj& X = mX;

                ASSERT(&testAllocator == X.allocator());

                LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,
                             TAG == X.tagString());
                LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,
                             NAME == X.name());
                LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,
                             DESC == X.description());
                LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,
                             EXP_TYPEINFO == X.typeInfo());
                LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,
                             EXP_OCCURRENCE_INFO == X.occurrenceInfo());

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }
        }
        }
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
        for (int i = 0; i < NUM_OPTION_TAGS;         ++i) {
        for (int j = 0; j < NUM_OPTION_NAMES;        ++j) {
        for (int k = 0; k < NUM_OPTION_DESCRIPTIONS; ++k) {
        for (int l = 0; l < NUM_OPTION_TYPEINFO;     ++l) {
        for (int m = 0; m < NUM_OPTION_OCCURRENCES;  ++m) {

            const int n = l % NUM_OPTION_DEFAULT_VALUES;
                // Instead of:
                //..
                //  for (int n = 0; n < NUM_OPTION_DEFAULT_VALUES; ++n)
                //..
                // Valid only because of the way we organized the table data:
                //..
                   ASSERT(OPTION_TYPEINFO[l].d_type ==
                                              OPTION_DEFAULT_VALUES[n].d_type);
                //..
            const int       LINE1      = OPTION_TAGS[i].d_line;
            const char     *TAG        = OPTION_TAGS[i].d_tag_p;

            const int       LINE2      = OPTION_NAMES[j].d_line;
            const char     *NAME       = OPTION_NAMES[j].d_name_p;

            const int       LINE3      = OPTION_DESCRIPTIONS[k].d_line;
            const char     *DESC       = OPTION_DESCRIPTIONS[k].
                                                               d_description_p;

            const int       LINE4      = OPTION_TYPEINFO[l].d_line;
            const ElemType  TYPE       = OPTION_TYPEINFO[l].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[l].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[l].d_constraint_p;

            const int       LINE5      = OPTION_OCCURRENCES[m].d_line;
            const OccurType OTYPE      = OPTION_OCCURRENCES[m].d_type;

            const void     *DEFAULT_VALUE = OPTION_DEFAULT_VALUES[n].d_value_p;

            if (TYPE != OPTION_DEFAULT_VALUES[n].d_type) {
                continue;
            }

            const TypeInfo       EXP_TYPE_INFO       = u::createTypeInfo(
                                                                   TYPE,
                                                                   VARIABLE,
                                                                   CONSTRAINT);
            const OccurrenceInfo EXP_OCCURRENCE_INFO = u::createOccurrenceInfo(
                                                                OTYPE,
                                                                TYPE,
                                                                DEFAULT_VALUE);

            const balcl::OptionInfo EXP = { TAG,
                                            NAME,
                                            DESC,
                                            EXP_TYPE_INFO,
                                            EXP_OCCURRENCE_INFO
                                          };

            Obj mX(EXP); const Obj& X = mX;

            if (veryVerbose) { T_ T_ P(X) }
            LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,
                         TAG  == X.tagString());
            LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,
                         NAME == X.name());
            LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,
                         DESC == X.description());
            LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,
                         EXP_TYPE_INFO == X.typeInfo());
            LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,
                         EXP_OCCURRENCE_INFO == X.occurrenceInfo());

            const int FLAG = X.tagString().empty()
                           ? OptionInfo::e_NON_OPTION
                           : (Ot::e_BOOL == TYPE
                             ? OptionInfo::e_FLAG
                             : OptionInfo::e_OPTION);
            LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,
                         FLAG == X.argType());

            const bool IS_ARRAY = OptionType::isArrayType(TYPE);
            LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,
                         IS_ARRAY == X.isArray());

            bsl::ostringstream oss;
            LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,
                        !X.name().empty() == X.isNameValid(oss));
            LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,
                        !X.description().empty() == X.isDescriptionValid(oss));
        }
        }
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

// BDE_VERIFY pragma: -TP21    // Loops must contain very verbose action
// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
        for (int i = 0; i < NUM_OPTION_TAGS;         ++i) {
        for (int j = 0; j < NUM_OPTION_NAMES;        ++j) {
        for (int k = 0; k < NUM_OPTION_DESCRIPTIONS; ++k) {
        for (int l = 0; l < NUM_OPTION_TYPEINFO;     ++l) {
        for (int m = 0; m < NUM_OPTION_OCCURRENCES;  ++m) {
            const int n = l % NUM_OPTION_DEFAULT_VALUES;
                // Instead of:
                //..
                //  for (int n = 0; n < NUM_OPTION_DEFAULT_VALUES; ++n)
                //..
                // Valid only because of the way we organized the table data:
                //..
                   ASSERT(OPTION_TYPEINFO[l].d_type ==
                                              OPTION_DEFAULT_VALUES[n].d_type);
                //..
            const int       LINE1      = OPTION_TAGS[i].d_line;
            const char     *TAG        = OPTION_TAGS[i].d_tag_p;

            const int       LINE2      = OPTION_NAMES[j].d_line;
            const char     *NAME       = OPTION_NAMES[j].d_name_p;

            const int       LINE3      = OPTION_DESCRIPTIONS[k].d_line;
            const char     *DESC       = OPTION_DESCRIPTIONS[k].
                                                               d_description_p;

            const int       LINE4      = OPTION_TYPEINFO[l].d_line;
            const ElemType  TYPE       = OPTION_TYPEINFO[l].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[l].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[l].d_constraint_p;

            const int       LINE5      = OPTION_OCCURRENCES[m].d_line;
            const OccurType OTYPE      = OPTION_OCCURRENCES[m].d_type;

            const void  *DEFAULT_VALUE = OPTION_DEFAULT_VALUES[n].d_value_p;

            if (TYPE != OPTION_DEFAULT_VALUES[n].d_type) {
                continue;
            }

            const OptionInfo EXP = { TAG,
                                     NAME,
                                     DESC,
                                     u::createTypeInfo(TYPE,
                                                       VARIABLE,
                                                       CONSTRAINT),
                                     u::createOccurrenceInfo(OTYPE,
                                                             TYPE,
                                                             DEFAULT_VALUE)
                                   };

            Obj mX(EXP); const Obj& X = mX;

            bsl::ostringstream oss1, oss2, oss3;
            X.print(oss1);                  // TEST HERE
            X.print(oss2, 2, 8);            // TEST HERE
            oss3 << X;                      // TEST HERE

            bsl::string ss1(oss1.str()), ss2(oss2.str()), ss3(oss3.str());
            LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5, ss1 != ss2);
            LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5, ss1 == ss3);
                // Note: oss[123].str() are temporaries!  Doesn't work well
                // with oss[123].str().c_str();  except on SunPro where a
                // temporary lives till the end of the block.

            if (veryVerbose) {
                T_ T_ P(X)
                T_ T_ P(ss1)
                T_ T_ P(ss2)
            }

            const char *output = ss1.c_str();

            int parseRet = u::parseOption(&output, X, output);
            LOOP4_ASSERT(X, ss1, output, parseRet, 0 == parseRet);

            output = ss3.c_str();

            parseRet = u::parseOption(&output, X, output);
            LOOP4_ASSERT(X, ss3, output, parseRet, 0 == parseRet);
        }
        }
        }
        }
        }
// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line
// BDE_VERIFY pragma: +TP21    // Loops must contain very verbose action

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 6 OF VALUE-SEMANTIC TEST DRIVER
        // TESTING EQUALITY OPERATOR
        //   Note: Unless they share the constraint, two objects will always
        //   differ.  We limit the testing since there are just too many values
        //   to test otherwise: 2 tags, 2 names, 2 descriptions suffice.  We
        //   also limit ourselves to TypeInfo with no linked variables and no
        //   constraint, and no array types.  Finally, we limit ourselves to
        //   required or optional occurrences only (not hidden), and no array
        //   default values.

        if (verbose) cout << "\n\tTesting equality operator." << endl;

        if (verbose)
            cout << "\t\tCompare each pair of values (u, v) in W X W." << endl;

// BDE_VERIFY pragma: -TP21    // Loops must contain very verbose action
// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
        for (int i1 = 0; i1 < 2;                          ++i1) {
        for (int j1 = 1; j1 < 3;                          ++j1) {
        for (int k1 = 0; k1 < 2;                          ++k1) {
        for (int l1 = 0; l1 < NUM_OPTION_TYPEINFO / 8;    ++l1) {
        for (int m1 = 0; m1 < NUM_OPTION_OCCURRENCES - 1; ++m1) {
            int n1 = l1;
                // Instead of:
                //..
                //  for (int n1 = 0; n1 < NUM_OPTION_DEFAULT_VALUES / 2; ++n1)
                //..
                // Valid only because of the way we organized the table data:
                //..
                   ASSERT(OPTION_TYPEINFO[l1].d_type ==
                                             OPTION_DEFAULT_VALUES[n1].d_type);
                //..
            const int       LINE1 = OPTION_TAGS[i1].d_line;
            const char     *TAG1  = OPTION_TAGS[i1].d_tag_p;

            const int       LINE2 = OPTION_NAMES[j1].d_line;
            const char     *NAME1 = OPTION_NAMES[j1].d_name_p;

            const int       LINE3 = OPTION_DESCRIPTIONS[k1].d_line;
            const char     *DESC1 = OPTION_DESCRIPTIONS[k1].d_description_p;

            const int       LINE4   = OPTION_TYPEINFO[l1].d_line;
            const ElemType  TYPE1   = OPTION_TYPEINFO[l1].d_type;

            void *VARIABLE1         = OPTION_TYPEINFO[l1].d_linkedVariable_p;
            void *CONSTRAINT1       = OPTION_TYPEINFO[l1].d_constraint_p;

            const int       LINE5   = OPTION_OCCURRENCES[m1].d_line;
            const OccurType OTYPE1  = OPTION_OCCURRENCES[m1].d_type;

            const void *DEFAULT_VALUE1 = OPTION_DEFAULT_VALUES[n1].d_value_p;

            if (TYPE1 != OPTION_DEFAULT_VALUES[n1].d_type) {
                continue;
            }

            const balcl::OptionInfo EXP1 = {
                              TAG1,
                              NAME1,
                              DESC1,
                              u::createTypeInfo(TYPE1, VARIABLE1, CONSTRAINT1),
                              u::createOccurrenceInfo(OTYPE1,
                                                      TYPE1,
                                                      DEFAULT_VALUE1)
                            };

            Obj mX(EXP1); const Obj& X = mX;

            for (int i2 = 0; i2 < 2; ++i2) {
            for (int j2 = 0; j2 < 2; ++j2) {
            for (int k2 = 0; k2 < 2; ++k2) {
            for (int l2 = 0; l2 < NUM_OPTION_TYPEINFO / 8; ++l2) {
            for (int m2 = 0; m2 < NUM_OPTION_OCCURRENCES - 1; ++m2) {
                int n2 = l2;
                    // Instead of:
                    //..
                    //  for (int n2 = 0; n2 < NUM_OPTION_DEFAULT_VALUES / 2;
                    //                                                      ...
                    //..
                    // and only because of the way we organized the table data:
                    //..
                       ASSERT(OPTION_TYPEINFO[l2].d_type ==
                                             OPTION_DEFAULT_VALUES[n2].d_type);
                    //..
                    //
                const int       LINE6  = OPTION_TAGS[i2].d_line;
                const char     *TAG2   = OPTION_TAGS[i2].d_tag_p;

                const int       LINE7  = OPTION_NAMES[j2].d_line;
                const char     *NAME2  = OPTION_NAMES[j2].d_name_p;

                const int       LINE8  = OPTION_DESCRIPTIONS[k2].d_line;
                const char     *DESC2  = OPTION_DESCRIPTIONS[k2].
                                                               d_description_p;

                const int      LINE9  = OPTION_TYPEINFO[l2].d_line;
                const ElemType TYPE2  = OPTION_TYPEINFO[l2].d_type;

                void *VARIABLE2       = OPTION_TYPEINFO[l2].d_linkedVariable_p;
                void *CONSTRAINT2     = OPTION_TYPEINFO[l2].d_constraint_p;

                const int       LINE10 = OPTION_OCCURRENCES[m2].d_line;
                const OccurType OTYPE2 = OPTION_OCCURRENCES[m2].d_type;

                const void *DEFAULT_VALUE2 = OPTION_DEFAULT_VALUES[n2].
                                                                     d_value_p;

                if (TYPE2 != OPTION_DEFAULT_VALUES[n2].d_type) {
                    continue;
                }

                const balcl::OptionInfo EXP2 = {
                         TAG2,
                         NAME2,
                         DESC2,
                         u::createTypeInfo(TYPE2, VARIABLE2, CONSTRAINT2),
                         u::createOccurrenceInfo(OTYPE2, TYPE2, DEFAULT_VALUE2)
                       };

                Obj mY(EXP2); const Obj& Y = mY;

                bool isSame = X.name()           == Y.name()
                           && X.tagString()      == Y.tagString()
                           && X.description()    == Y.description()
                           && X.typeInfo()       == Y.typeInfo()
                           && X.occurrenceInfo() == Y.occurrenceInfo();

                // Too many LINE*, but if one tests fails, the other will too!
                // So we use that to display *all* the parameters in two
                // different asserts.

                LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,
                             isSame == (X == Y));   // TEST HERE
                LOOP5_ASSERT(LINE6, LINE7, LINE8, LINE9, LINE10,
                             !isSame == (X != Y));   // TEST HERE
            }
            }
            }
            }
            }
        }
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
        for (int i = 0; i < NUM_OPTION_TAGS;         ++i) {
        for (int j = 0; j < NUM_OPTION_NAMES;        ++j) {
        for (int k = 0; k < NUM_OPTION_DESCRIPTIONS; ++k) {
        for (int l = 0; l < NUM_OPTION_TYPEINFO;     ++l) {
        for (int m = 0; m < NUM_OPTION_OCCURRENCES;  ++m) {
            const int n = l % NUM_OPTION_DEFAULT_VALUES;
                // Instead of:
                //..
                //  for (int n = 0; n < NUM_OPTION_DEFAULT_VALUES; ++n)
                //..
                // Valid only because of the way we organized the table data:
                //..
                    ASSERT(OPTION_TYPEINFO[l].d_type ==
                                              OPTION_DEFAULT_VALUES[n].d_type);
                //..
            const int       LINE1      = OPTION_TAGS[i].d_line;
            const char     *TAG        = OPTION_TAGS[i].d_tag_p;

            const int       LINE2      = OPTION_NAMES[j].d_line;
            const char     *NAME       = OPTION_NAMES[j].d_name_p;

            const int       LINE3      = OPTION_DESCRIPTIONS[k].d_line;
            const char     *DESC       = OPTION_DESCRIPTIONS[k].
                                                               d_description_p;

            const int       LINE4      = OPTION_TYPEINFO[l].d_line;
            const ElemType  TYPE       = OPTION_TYPEINFO[l].d_type;

            void           *VARIABLE   = OPTION_TYPEINFO[l].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[l].d_constraint_p;

            const int       LINE5      = OPTION_OCCURRENCES[m].d_line;
            const OccurType OTYPE      = OPTION_OCCURRENCES[m].d_type;

            const void     *DEFAULT_VALUE = OPTION_DEFAULT_VALUES[n].d_value_p;

            if (TYPE != OPTION_DEFAULT_VALUES[n].d_type) {
                continue;
            }

            const balcl::OptionInfo EXP = {
                                 TAG,
                                 NAME,
                                 DESC,
                                 u::createTypeInfo(TYPE, VARIABLE, CONSTRAINT),
                                 u::createOccurrenceInfo(OTYPE,
                                                         TYPE,
                                                         DEFAULT_VALUE)
                               };

            Obj mX(EXP); const Obj& X = mX;

            Obj mY(X);   const Obj& Y = mY;  // TEST HERE

            if (veryVerbose) { T_ T_ P_(X) P(Y) }

            LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,   X == Y);
            LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5, !(X != Y));
        }
        }
        }
        }
        }
// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line
// BDE_VERIFY pragma: +TP21    // Loops must contain very verbose action

        if (verbose) cout << "\t\tPassing in an allocator." << endl
                           << "\t\t\tWith no exceptions." << endl;

// BDE_VERIFY pragma: -TP21    // Loops must contain very verbose action
// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
        for (int i = 0; i < NUM_OPTION_TAGS;         ++i) {
        for (int j = 0; j < NUM_OPTION_NAMES;        ++j) {
        for (int k = 0; k < NUM_OPTION_DESCRIPTIONS; ++k) {
        for (int l = 0; l < NUM_OPTION_TYPEINFO;     ++l) {
        for (int m = 0; m < NUM_OPTION_OCCURRENCES;  ++m) {
            const int n = l % NUM_OPTION_DEFAULT_VALUES;
                // Instead of:
                //..
                //  for (int n = 0; n < NUM_OPTION_DEFAULT_VALUES; ++n)
                //..
                // Valid only because of the way we organized the table data:
                //..
                   ASSERT(OPTION_TYPEINFO[l].d_type ==
                                              OPTION_DEFAULT_VALUES[n].d_type);
                //..
            const int       LINE1      = OPTION_TAGS[i].d_line;
            const char     *TAG        = OPTION_TAGS[i].d_tag_p;

            const int       LINE2      = OPTION_NAMES[j].d_line;
            const char     *NAME       = OPTION_NAMES[j].d_name_p;

            const int       LINE3      = OPTION_DESCRIPTIONS[k].d_line;
            const char     *DESC       = OPTION_DESCRIPTIONS[k].
                                                               d_description_p;

            const int       LINE4      = OPTION_TYPEINFO[l].d_line;
            const ElemType  TYPE       = OPTION_TYPEINFO[l].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[l].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[l].d_constraint_p;

            const int       LINE5      = OPTION_OCCURRENCES[m].d_line;
            const OccurType OTYPE      = OPTION_OCCURRENCES[m].d_type;

            const void     *DEFAULT_VALUE = OPTION_DEFAULT_VALUES[n].d_value_p;

            if (TYPE != OPTION_DEFAULT_VALUES[n].d_type) {
                continue;
            }

            const balcl::OptionInfo EXP = {
                                 TAG,
                                 NAME,
                                 DESC,
                                 u::createTypeInfo(TYPE, VARIABLE, CONSTRAINT),
                                 u::createOccurrenceInfo(OTYPE,
                                                         TYPE,
                                                         DEFAULT_VALUE)
                               };

            Obj mX(EXP); const Obj& X = mX;

            const bsls::Types::Int64 NUM_BYTES =
                                              defaultAllocator.numBytesInUse();
            const bsls::Types::Int64 NUM_ALLOC =
                                             defaultAllocator.numAllocations();
            {
                Obj mY(X, &testAllocator);  const Obj& Y = mY;  // TEST HERE

                LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5, X == Y);
            }
            LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,
                         0 == testAllocator.numBytesInUse());
            LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,
                         0 == testAllocator.numMismatches());
            LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,
                         NUM_BYTES == defaultAllocator.numBytesInUse());
            LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,
                         NUM_ALLOC == defaultAllocator.numAllocations());
        }
        }
        }
        }
        }
// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line
// BDE_VERIFY pragma: +TP21    // Loops must contain very verbose action

        if (verbose) cout << "\t\t\tWith exceptions." << endl;

// BDE_VERIFY pragma: -TP21    // Loops must contain very verbose action
// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
        for (int i = 0; i < NUM_OPTION_TAGS;         ++i) {
        for (int j = 0; j < NUM_OPTION_NAMES;        ++j) {
        for (int k = 0; k < NUM_OPTION_DESCRIPTIONS; ++k) {
        for (int l = 0; l < NUM_OPTION_TYPEINFO;     ++l) {
        for (int m = 0; m < NUM_OPTION_OCCURRENCES;  ++m) {
            const int n = l % NUM_OPTION_DEFAULT_VALUES;
                // Instead of:
                //..
                //  for (int n = 0; n < NUM_OPTION_DEFAULT_VALUES; ++n)
                //..
                // Valid only because of the way we organized the table data:
                //..
                   ASSERT(OPTION_TYPEINFO[l].d_type ==
                                              OPTION_DEFAULT_VALUES[n].d_type);
                //..
            const char     *TAG        = OPTION_TAGS[i].d_tag_p;
            const char     *NAME       = OPTION_NAMES[j].d_name_p;
            const char     *DESC       = OPTION_DESCRIPTIONS[k].
                                                               d_description_p;
            const ElemType  TYPE       = OPTION_TYPEINFO[l].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[l].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[l].d_constraint_p;

            const OccurType OTYPE      = OPTION_OCCURRENCES[m].d_type;

            const void *DEFAULT_VALUE  = OPTION_DEFAULT_VALUES[n].d_value_p;

            if (TYPE != OPTION_DEFAULT_VALUES[n].d_type) {
                continue;
            }

            const balcl::OptionInfo EXP = {
                                 TAG,
                                 NAME,
                                 DESC,
                                 u::createTypeInfo(TYPE, VARIABLE, CONSTRAINT),
                                 u::createOccurrenceInfo(OTYPE,
                                                         TYPE,
                                                         DEFAULT_VALUE)
                               };

            Obj mX(EXP); const Obj& X = mX;

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                Obj mY(X, &testAllocator);  const Obj& Y = mY;  // TEST HERE

                ASSERT(X == Y);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }
        }
        }
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
        for (int i1 = 0; i1 < 2;                          ++i1) {
        for (int j1 = 0; j1 < 2;                          ++j1) {
        for (int k1 = 0; k1 < 2;                          ++k1) {
        for (int l1 = 0; l1 < NUM_OPTION_TYPEINFO / 8;    ++l1) {
        for (int m1 = 0; m1 < NUM_OPTION_OCCURRENCES - 1; ++m1) {
            const int n1 = l1;
                // Instead of:
                //..
                //  for (int n1 = 0; n1 < NUM_OPTION_DEFAULT_VALUES / 2; ++n1)
                //..
                // and only because of the way we organized the table data:
                //..
                   ASSERT(OPTION_TYPEINFO[l1].d_type ==
                                             OPTION_DEFAULT_VALUES[n1].d_type);
                //..
            const int       LINE1 = OPTION_TAGS[i1].d_line;
            const char     *TAG1  = OPTION_TAGS[i1].d_tag_p;

            const int       LINE2 = OPTION_NAMES[j1].d_line;
            const char     *NAME1 = OPTION_NAMES[j1].d_name_p;

            const int       LINE3  = OPTION_DESCRIPTIONS[k1].d_line;
            const char     *DESC1  = OPTION_DESCRIPTIONS[k1].d_description_p;

            const int       LINE4  = OPTION_TYPEINFO[l1].d_line;
            const ElemType  TYPE1  = OPTION_TYPEINFO[l1].d_type;

            void *VARIABLE1        = OPTION_TYPEINFO[l1].d_linkedVariable_p;
            void *CONSTRAINT1      = OPTION_TYPEINFO[l1].d_constraint_p;

            const int       LINE5  = OPTION_OCCURRENCES[m1].d_line;
            const OccurType OTYPE1 = OPTION_OCCURRENCES[m1].d_type;

            const void *DEFAULT_VALUE1 = OPTION_DEFAULT_VALUES[n1].d_value_p;

            if (TYPE1 != OPTION_DEFAULT_VALUES[n1].d_type) {
                continue;
            }

            const balcl::OptionInfo EXP1 = {
                TAG1,
                NAME1,
                DESC1,
                u::createTypeInfo(TYPE1, VARIABLE1, CONSTRAINT1),
                u::createOccurrenceInfo(OTYPE1, TYPE1, DEFAULT_VALUE1)
            };

            Obj mX(EXP1); const Obj& X = mX;

            for (int i2 = 0; i2 < 2; ++i2) {
            for (int j2 = 0; j2 < 2; ++j2) {
            for (int k2 = 0; k2 < 2; ++k2) {
            for (int l2 = 0; l2 < NUM_OPTION_TYPEINFO / 8; ++l2) {
            for (int m2 = 0; m2 < NUM_OPTION_OCCURRENCES - 1; ++m2) {
                const int n2 = l2;
                    // Instead of:
                    //..
                    //  for (int n2 = 0; n2 < NUM_OPTION_DEFAULT_VALUES / 2;
                    //                                                      ...
                    //..
                    // and only because of the way we organized the table data:
                    //..
                       ASSERT(OPTION_TYPEINFO[l2].d_type ==
                                             OPTION_DEFAULT_VALUES[n2].d_type);
                    //..
                const int       LINE6 = OPTION_TAGS[i2].d_line;
                const char     *TAG2  = OPTION_TAGS[i2].d_tag_p;

                const int       LINE7 = OPTION_NAMES[j2].d_line;
                const char     *NAME2 = OPTION_NAMES[j2].d_name_p;

                const int       LINE8 = OPTION_DESCRIPTIONS[k2].d_line;
                const char     *DESC2 = OPTION_DESCRIPTIONS[k2].
                                                               d_description_p;

                const int       LINE9 = OPTION_TYPEINFO[l2].d_line;
                const ElemType  TYPE2 = OPTION_TYPEINFO[l2].d_type;
                void       *VARIABLE2 = OPTION_TYPEINFO[l2].d_linkedVariable_p;
                void     *CONSTRAINT2 = OPTION_TYPEINFO[l2].d_constraint_p;

                const int       LINE10 = OPTION_OCCURRENCES[m2].d_line;
                const OccurType OTYPE2 = OPTION_OCCURRENCES[m2].d_type;

                const void *DEFAULT_VALUE2 = OPTION_DEFAULT_VALUES[n2]
                                                                    .d_value_p;

                if (TYPE2 != OPTION_DEFAULT_VALUES[n2].d_type) {
                    continue;
                }

                const balcl::OptionInfo EXP2 = {
                    TAG2,
                    NAME2,
                    DESC2,
                    u::createTypeInfo(TYPE2, VARIABLE2, CONSTRAINT2),
                    u::createOccurrenceInfo(OTYPE2, TYPE2, DEFAULT_VALUE2)
                };

                Obj mY(EXP2); const Obj& Y = mY;

                mY = X;  // TEST HERE

                LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,    X == Y);
                LOOP5_ASSERT(LINE6, LINE7, LINE8, LINE9, LINE10, !(X != Y));
            }
            }
            }
            }
            }
        }
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
        for (int i = 0; i < NUM_OPTION_TAGS;         ++i) {
        for (int j = 0; j < NUM_OPTION_NAMES;        ++j) {
        for (int k = 0; k < NUM_OPTION_DESCRIPTIONS; ++k) {
        for (int l = 0; l < NUM_OPTION_TYPEINFO;     ++l) {
        for (int m = 0; m < NUM_OPTION_OCCURRENCES;  ++m) {
            const int n = l % NUM_OPTION_DEFAULT_VALUES;
                // Instead of:
                //..
                //  for (int n = 0; n < NUM_OPTION_DEFAULT_VALUES; ++n)
                //..
                // Valid only because of the way we organized the table data:
                //..
                   ASSERT(OPTION_TYPEINFO[l].d_type ==
                                              OPTION_DEFAULT_VALUES[n].d_type);
                //..
            const int       LINE1      = OPTION_TAGS[i].d_line;
            const char     *TAG        = OPTION_TAGS[i].d_tag_p;

            const int       LINE2      = OPTION_NAMES[j].d_line;
            const char     *NAME       = OPTION_NAMES[j].d_name_p;

            const int       LINE3      = OPTION_DESCRIPTIONS[k].d_line;
            const char     *DESC       = OPTION_DESCRIPTIONS[k].
                                                               d_description_p;

            const int       LINE4      = OPTION_TYPEINFO[l].d_line;
            const ElemType  TYPE       = OPTION_TYPEINFO[l].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[l].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[l].d_constraint_p;

            const int       LINE5      = OPTION_OCCURRENCES[m].d_line;
            const OccurType OTYPE      = OPTION_OCCURRENCES[m].d_type;

            const void     *DEFAULT_VALUE = OPTION_DEFAULT_VALUES[n].d_value_p;

            if (TYPE != OPTION_DEFAULT_VALUES[n].d_type) {
                continue;
            }

            const balcl::OptionInfo EXP = {
                                 TAG,
                                 NAME,
                                 DESC,
                                 u::createTypeInfo(TYPE, VARIABLE, CONSTRAINT),
                                 u::createOccurrenceInfo(OTYPE,
                                                         TYPE,
                                                         DEFAULT_VALUE)
                               };

            Obj mX(EXP); const Obj& X = mX;

            const Obj Z(X);

            mX = X;  // TEST HERE

            LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5, Z == X);
        }
        }
        }
        }
        }
// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line
// BDE_VERIFY pragma: +TP21    // Loops must contain very verbose action

        if (verbose) cout << "\t\tTesting conversion and assignment." << endl;

// BDE_VERIFY pragma: -TP21    // Loops must contain very verbose action
// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
        for (int i1 = 0; i1 < 2;                          ++i1) {
        for (int j1 = 1; j1 < 3;                          ++j1) {
        for (int k1 = 0; k1 < 2;                          ++k1) {
        for (int l1 = 0; l1 < NUM_OPTION_TYPEINFO / 8;    ++l1) {
        for (int m1 = 0; m1 < NUM_OPTION_OCCURRENCES - 1; ++m1) {
            int n1 = l1;
                // Instead of:
                //..
                //  for (int n1 = 0; n1 < NUM_OPTION_DEFAULT_VALUES / 2; ++n1)
                //..
                // Valid only because of the way we organized the table data:
                //..
                   ASSERT(OPTION_TYPEINFO[l1].d_type ==
                                             OPTION_DEFAULT_VALUES[n1].d_type);
                //..
            const int       LINE1 = OPTION_TAGS[i1].d_line;
            const char     *TAG1  = OPTION_TAGS[i1].d_tag_p;

            const int       LINE2 = OPTION_NAMES[j1].d_line;
            const char     *NAME1 = OPTION_NAMES[j1].d_name_p;

            const int       LINE3 = OPTION_DESCRIPTIONS[k1].d_line;
            const char     *DESC1 = OPTION_DESCRIPTIONS[k1].d_description_p;

            const int       LINE4   = OPTION_TYPEINFO[l1].d_line;
            const ElemType  TYPE1   = OPTION_TYPEINFO[l1].d_type;

            void *VARIABLE1         = OPTION_TYPEINFO[l1].d_linkedVariable_p;
            void *CONSTRAINT1       = OPTION_TYPEINFO[l1].d_constraint_p;

            const int       LINE5   = OPTION_OCCURRENCES[m1].d_line;
            const OccurType OTYPE1  = OPTION_OCCURRENCES[m1].d_type;

            const void *DEFAULT_VALUE1 = OPTION_DEFAULT_VALUES[n1].d_value_p;

            if (TYPE1 != OPTION_DEFAULT_VALUES[n1].d_type) {
                continue;
            }

            const balcl::OptionInfo EXP1 = {
                              TAG1,
                              NAME1,
                              DESC1,
                              u::createTypeInfo(TYPE1, VARIABLE1, CONSTRAINT1),
                              u::createOccurrenceInfo(OTYPE1,
                                                      TYPE1,
                                                      DEFAULT_VALUE1)
                            };

            Obj mX(EXP1); const Obj& X = mX;

            LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5, EXP1 == X); // TEST

            for (int i2 = 0; i2 < 2; ++i2) {
            for (int j2 = 0; j2 < 2; ++j2) {
            for (int k2 = 0; k2 < 2; ++k2) {
            for (int l2 = 0; l2 < NUM_OPTION_TYPEINFO / 8; ++l2) {
            for (int m2 = 0; m2 < NUM_OPTION_OCCURRENCES - 1; ++m2) {
                int n2 = l2;
                    // Instead of:
                    //..
                    //  for (int n2 = 0; n2 < NUM_OPTION_DEFAULT_VALUES / 2;
                    //                                                      ...
                    //..
                    // and only because of the way we organized the table data:
                    //..
                       ASSERT(OPTION_TYPEINFO[l2].d_type ==
                                             OPTION_DEFAULT_VALUES[n2].d_type);
                    //..
                    //
                const int       LINE6  = OPTION_TAGS[i2].d_line;
                const char     *TAG2   = OPTION_TAGS[i2].d_tag_p;

                const int       LINE7  = OPTION_NAMES[j2].d_line;
                const char     *NAME2  = OPTION_NAMES[j2].d_name_p;

                const int       LINE8  = OPTION_DESCRIPTIONS[k2].d_line;
                const char     *DESC2  = OPTION_DESCRIPTIONS[k2].
                                                               d_description_p;

                const int      LINE9  = OPTION_TYPEINFO[l2].d_line;
                const ElemType TYPE2  = OPTION_TYPEINFO[l2].d_type;

                void *VARIABLE2       = OPTION_TYPEINFO[l2].d_linkedVariable_p;
                void *CONSTRAINT2     = OPTION_TYPEINFO[l2].d_constraint_p;

                const int       LINE10 = OPTION_OCCURRENCES[m2].d_line;
                const OccurType OTYPE2 = OPTION_OCCURRENCES[m2].d_type;

                const void *DEFAULT_VALUE2 = OPTION_DEFAULT_VALUES[n2].
                                                                     d_value_p;

                if (TYPE2 != OPTION_DEFAULT_VALUES[n2].d_type) {
                    continue;
                }

                const balcl::OptionInfo EXP2 = {
                    TAG2,
                    NAME2,
                    DESC2,
                    u::createTypeInfo(TYPE2, VARIABLE2, CONSTRAINT2),
                    u::createOccurrenceInfo(OTYPE2, TYPE2, DEFAULT_VALUE2)
                };

                Obj mY(EXP2); const Obj& Y = mY;

                mY = EXP1;  // TEST ASSIGNMENT

                LOOP5_ASSERT(LINE6, LINE7, LINE8, LINE9, LINE10, X == Y);
            }
            }
            }
            }
            }
        }
        }
        }
        }
        }
// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line
// BDE_VERIFY pragma: +TP21    // Loops must contain very verbose action

        if (verbose) cout << "\n\tTesting default constructor." << endl;
        {
            Obj mX(&testAllocator); const Obj& X = mX;

            bsl::ostringstream oss;

            ASSERT(OptionInfo::e_NON_OPTION   == X.argType());
            ASSERT(""                              == X.description());
            ASSERT(false                           == X.isArray());
            ASSERT(true                            == X.isTagValid(oss));
            ASSERT(false                           == X.isDescriptionValid(
                                                                         oss));
            ASSERT(true                            == X.isTagValid(oss));
            ASSERT(false                           == X.isNameValid(oss));
            ASSERT(""                              == X.name());
            ASSERT(OccurrenceInfo::e_OPTIONAL == X.occurrenceInfo());
            ASSERT(""                              == X.tagString());
            ASSERT(TypeInfo()                      == X.typeInfo());
        }

        if (verbose) cout << "\n\tTesting 'isLongTagValid'." << endl;
        {
            bsl::ostringstream oss;

            Obj mX; const Obj& X = mX;

            ASSERT(false == X.isLongTagValid("-abc", oss));
            ASSERT(""    != oss.str()); oss.str("");

            ASSERT(true == X.isLongTagValid("a-bc",  oss));
            ASSERT(""    == oss.str()); oss.str("");

            ASSERT(false == X.isLongTagValid("a|bc", oss));
            ASSERT(""    != oss.str()); oss.str("");

            ASSERT(false == X.isLongTagValid("a",    oss));
            ASSERT(""    != oss.str()); oss.str("");

            ASSERT(true == X.isLongTagValid("ab",    oss));
            ASSERT(""   == oss.str()); oss.str("");
        }

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
