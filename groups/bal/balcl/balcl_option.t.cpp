// balcl_option.t.cpp                                                 -*-C++-*-

#include <balcl_option.h>

#include <balcl_constraint.h>
#include <balcl_occurrenceinfo.h>
#include <balcl_optioninfo.h>
#include <balcl_optiontype.h>
#include <balcl_optionvalue.h>
#include <balcl_typeinfo.h>

#include <bdlb_chartype.h>
#include <bdlb_numericparseutil.h>
#include <bdlb_printmethods.h> // 'bdlb::HasPrintMethod'

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_time.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_assert.h>

#include <bsls_asserttest.h>
#include <bsls_types.h>      // 'bsls::Types::Int64'
#include <bsls_platform.h>

#include <bsl_cstdlib.h>     // 'bsl::atoi'
#include <bsl_cstring.h>     // 'bsl::strcmp', 'bsl::strchr', 'bsl::strlen'
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>      // 'bslstl::StringRef'
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// This component provides an unconstrained, in-core (value-semantic) attribute
// class, 'balcl::Option', and generally follows the testing pattern of that
// category.
//
// Primary Manipulators:
//: o Option(const OptionInfo& info, *bA = 0);
//
// Basic Accessors
//: o const string& description() const;
//: o const string& name() const;
//: o const OccurrenceInfo& occurrenceInfo() const;
//: o const string& tagString() const;
//: o const TypeInfo& typeInfo() const;
//: o bslma::Allocator *allocator() const;
//
///Input Tables
///------------
// There are several input tables (defined at file scope) that are used
// throughout this test driver.  The cross product of these tables covers the
// space of qualitatively equivalent inputs.  The tables are:
//
//: 1 'OPTION_TAGS': A table of representative tags that includes entries of
//:   several lengths (including 0-length) that have both short and long tags
//:   and long tags only.
//:
//: 2 'OPTION_NAMES': A table of representative names of several lengths,
//:   including a 0-length name.
//:
//: 3 'OPTION_DESCRIPTIONS': A table of representative descriptions of several
//:   lengths, including a 0-length name.
//:
//: 4 'OPTION_TYPEINFO': This table represents the space of 'TypeInfo'
//:   attributes.  There are entries that cover the cross product of every
//:   supported option type, linked variable (with and without), and constraint
//:   (with and without, where allowed).
//:
//:   o A helper function, 'u::createTypeInfo', is defined to create test
//:     arguments from table entries.
//:
//: 5 'OPTION_OCCURRENCES': This table has an entry for each of the three
//:   allowed values of 'OccurrenceType'.
//:
//:   o A helper function, 'u::createOccurrenceInfo', is defined to create test
//:     arguments from table entries.
//:
//: 6 'OPTION_DEFAULT_VALUES': an entry for each of the allowed option types
//:   and the address of a value of that type to be used as a default option
//:   value.
//:
//:   o None of these "default" values correspond to the default value of their
//:     respective types.
//:
//:   o The value chosen for 'Ot::e_STRING' is sufficiently long to exceed the
//:     short-string optimization.
//:
//:   o A helper function, 'u::setOptionValue', is provided to convert the
//:     "value" field of 'OPTION_DEFAULT_VALUES' into an argument for the
//:     'setDefaultValue' method.
//:
//:   o The entries of 'OPTION_DEFAULT_VALUES' are ordered so that we can
//:     assert 'OPTION_TYPEINFO[l].d_type == OPTION_DEFAULT_VALUES[n].d_type'
//:     where 'n = l % NUM_OPTION_DEFAULT_VALUES' for
//:     '0 <= l < NUM_OPTION_TYPEINFO'.
//
// ----------------------------------------------------------------------------
// CREATORS
// [ 7] Option();
// [ 7] Option(bslma::Allocator *basicAllocator);
// [ 2] Option(const OptionInfo& info, *bA = 0);
// [ 5] Option(const Option& original, *bA = 0);
// [ 2] ~Option();
//
// MANIPULATORS
// [ 6] Option& operator=(const Option&     rhs);
// [ 6] Option& operator=(const OptionInfo& rhs);
//
// ACCESSORS
// [ 2] operator const OptionInfo&() const;
// [ 2] Option::ArgType argType() const;
// [ 2] const string& description() const;
// [ 2] bool isArray() const;
// [ 8] bool isLongTagValid(const char *longTag, ostream& stream) const;
// [ 8] bool isTagValid(ostream& stream) const;
// [ 2] bool isDescriptionValid(ostream& stream) const;
// [ 2] bool isNameValid(ostream& stream) const;
// [ 2] const char *longTag() const;
// [ 2] const string& name() const;
// [ 2] const OccurrenceInfo& occurrenceInfo() const;
// [ 2] char shortTag() const;
// [ 2] const string& tagString() const;
// [ 2] const TypeInfo& typeInfo() const;
//
// [ 2] bslma::Allocator *allocator() const;
// [ 3] ostream& print(ostream& stream, int level = 0, int spl = 4) const;
//
// FREE OPERATORS
// [ 4] bool operator==(const Option& lhs, rhs);
// [ 4] bool operator!=(const Option& lhs, rhs);
// [ 3] ostream& operator<<(ostream& strm, const Option& rhs);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ *] CONCERN: In no case does memory come from the global allocator.

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

typedef balcl::Option                  Obj;

typedef balcl::Constraint              Constraint;
typedef balcl::OccurrenceInfo          OccurrenceInfo;
typedef OccurrenceInfo::OccurrenceType OccurType;
typedef balcl::OptionInfo              OptionInfo;
typedef balcl::TypeInfo                TypeInfo;
typedef balcl::OptionValue             OptionValue;
typedef balcl::OptionType              Ot;
typedef balcl::OptionType::Enum        ElemType;

typedef bsls::Types::Int64             Int64;

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

#ifdef BSLS_PLATFORM_CPU_32_BIT
#define SUFFICIENTLY_LONG_STRING "123456789012345678901234567890123"
#else  // 64_BIT
#define SUFFICIENTLY_LONG_STRING "12345678901234567890123456789012" \
                                 "123456789012345678901234567890123"
#endif
BSLMF_ASSERT(sizeof SUFFICIENTLY_LONG_STRING > sizeof(bsl::string));

// ATTRIBUTES FOR 'balcl::Option'
static const struct {
    int         d_line;   // line number
    const char *d_tag_p;  // tag attribute
} OPTION_TAGS[] = {
    { L_, "" }
  , { L_, "a|aa" }
  , { L_, "b|bb" }
  , { L_, "cdefghijklmab" }
  , { L_, "a|aaabb" }
  , { L_, "b|bbbbb" }
  , { L_, "nopqrstuvwxyz" }
  , { L_, SUFFICIENTLY_LONG_STRING }
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
  , { L_, SUFFICIENTLY_LONG_STRING }
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
  , { L_, SUFFICIENTLY_LONG_STRING }
};
enum { NUM_OPTION_DESCRIPTIONS = sizeof  OPTION_DESCRIPTIONS
                               / sizeof *OPTION_DESCRIPTIONS };

#define GA bslma::Default::globalAllocator()

// ATTRIBUTES FOR 'balcl::TypeInfo'
bool                        linkedBool;
char                        linkedChar;
short                       linkedShort;
int                         linkedInt;
Int64                       linkedInt64;
float                       linkedFloat;
double                      linkedDouble;
bsl::string                 linkedString       (GA);
bdlt::Datetime              linkedDatetime;
bdlt::Date                  linkedDate;
bdlt::Time                  linkedTime;
bsl::vector<char>           linkedCharArray    (GA);
bsl::vector<short>          linkedShortArray   (GA);
bsl::vector<int>            linkedIntArray     (GA);
bsl::vector<Int64>          linkedInt64Array   (GA);
bsl::vector<float>          linkedFloatArray   (GA);
bsl::vector<double>         linkedDoubleArray  (GA);
bsl::vector<bsl::string>    linkedStringArray  (GA);
bsl::vector<bdlt::Datetime> linkedDatetimeArray(GA);
bsl::vector<bdlt::Date>     linkedDateArray    (GA);
bsl::vector<bdlt::Time>     linkedTimeArray    (GA);

#undef GA

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
   { L_, Ot::e_BOOL,           0,                    0                      }
 , { L_, Ot::e_CHAR,           0,                    0                      }
 , { L_, Ot::e_INT,            0,                    0                      }
 , { L_, Ot::e_INT64,          0,                    0                      }
 , { L_, Ot::e_DOUBLE,         0,                    0                      }
 , { L_, Ot::e_STRING,         0,                    0                      }
 , { L_, Ot::e_DATETIME,       0,                    0                      }
 , { L_, Ot::e_DATE,           0,                    0                      }
 , { L_, Ot::e_TIME,           0,                    0                      }
 , { L_, Ot::e_CHAR_ARRAY,     0,                    0                      }
 , { L_, Ot::e_INT_ARRAY,      0,                    0                      }
 , { L_, Ot::e_INT64_ARRAY,    0,                    0                      }
 , { L_, Ot::e_DOUBLE_ARRAY,   0,                    0                      }
 , { L_, Ot::e_STRING_ARRAY,   0,                    0                      }
 , { L_, Ot::e_DATETIME_ARRAY, 0,                    0                      }
 , { L_, Ot::e_DATE_ARRAY,     0,                    0                      }
 , { L_, Ot::e_TIME_ARRAY,     0,                    0                      }
 , { L_, Ot::e_BOOL,           &linkedBool,          0                      }
 , { L_, Ot::e_CHAR,           &linkedChar,          0                      }
 , { L_, Ot::e_INT,            &linkedInt,           0                      }
 , { L_, Ot::e_INT64,          &linkedInt64,         0                      }
 , { L_, Ot::e_DOUBLE,         &linkedDouble,        0                      }
 , { L_, Ot::e_STRING,         &linkedString,        0                      }
 , { L_, Ot::e_DATETIME,       &linkedDatetime,      0                      }
 , { L_, Ot::e_DATE,           &linkedDate,          0                      }
 , { L_, Ot::e_TIME,           &linkedTime,          0                      }
 , { L_, Ot::e_CHAR_ARRAY,     &linkedCharArray,     0                      }
 , { L_, Ot::e_INT_ARRAY,      &linkedIntArray,      0                      }
 , { L_, Ot::e_INT64_ARRAY,    &linkedInt64Array,    0                      }
 , { L_, Ot::e_DOUBLE_ARRAY,   &linkedDoubleArray,   0                      }
 , { L_, Ot::e_STRING_ARRAY,   &linkedStringArray,   0                      }
 , { L_, Ot::e_DATETIME_ARRAY, &linkedDatetimeArray, 0                      }
 , { L_, Ot::e_DATE_ARRAY,     &linkedDateArray,     0                      }
 , { L_, Ot::e_TIME_ARRAY,     &linkedTimeArray,     0                      }
 , { L_, Ot::e_BOOL,           0,                    0 /*cannot constrain */}
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
 , { L_, Ot::e_BOOL,           &linkedBool,          0                      }
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

BSLMF_ASSERT(sizeof SUFFICIENTLY_LONG_STRING > sizeof(bsl::string));

#define GA bslma::Default::globalAllocator()

bool                        defaultBool          = false;
char                        defaultChar          = 'D';
short                       defaultShort         = 1234;
int                         defaultInt           = 1234567;
Int64                       defaultInt64         = 123456789LL;
float                       defaultFloat         = 0.125;     // 1/8
double                      defaultDouble        = 0.015625;  // 1/64
bsl::string                 defaultString(SUFFICIENTLY_LONG_STRING,  GA);
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

#undef GA

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

#define GA bslma::Default::globalAllocator()

bool                     linkedBoolA;
bool                     linkedBoolB;
bool                     linkedBoolC;
bsl::string              linkedString1     (GA);
bsl::vector<bsl::string> linkedStringArray1(GA);

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
    ASSERT(input);

    ASSERT (3 == bsl::strlen(input));

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
// BDE_VERIFY pragma: +FABC01  // Function ... not in alphabetic order

                            // ==========================
                            // function skipRequiredToken
                            // ==========================

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
    ASSERT(endPos);
    ASSERT(inputString);
    ASSERT(token);

    *endPos = inputString;
    while ('\0' != *token && *token == **endPos) {
        ++*endPos;
        ++token;
    }

    return '\0' != *token;  // return "is null char"
}

                            // =======================
                            // function skipWhiteSpace
                            // =======================

int skipWhiteSpace(const char **endPos, const char *inputString)
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
    ASSERT(endPos);
    ASSERT(inputString);

    enum { LOCAL_SUCCESS = 0, LOCAL_FAILURE = 1 };

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
    ASSERT(result);
    ASSERT(endPos);
    ASSERT(inputString);

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
    ASSERT(result);
    ASSERT(endPos);
    ASSERT(inputString);

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
    ASSERT(result);
    ASSERT(endPos);
    ASSERT(inputString);

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
    ASSERT(result);
    ASSERT(endPos);
    ASSERT(inputString);

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
    ASSERT(result);
    ASSERT(endPos);
    ASSERT(inputString);
    ASSERT(k_DATETIME_FIELD_WIDTH <= bsl::strlen(inputString));

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
    ASSERT(result);
    ASSERT(endPos);
    ASSERT(inputString);
    ASSERT(9 <= bsl::strlen(inputString));

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
    ASSERT(result);
    ASSERT(endPos);
    ASSERT(inputString);
    ASSERT(k_TIME_FIELD_WIDTH <= bsl::strlen(inputString));

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
    ASSERT(result);
    ASSERT(endPos);
    ASSERT(inputString);

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
    ASSERT(result);
    ASSERT(endPos);
    ASSERT(inputString);

    result->clear();

    if ('[' != *inputString) {
        *endPos = inputString;
        return LOCAL_FAILURE;                                         // RETURN
    }
    ++inputString;

    skipWhiteSpace(endPos, inputString);

    TYPE value;
    while (']' != **endPos) {
        if (TypesParser::parse(&value, endPos, *endPos)) {
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
    ASSERT(typeInfo);
    ASSERT(Ot::e_VOID != type);
    ASSERT(Ot::e_BOOL != type);
    ASSERT(address);

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
    // (i.e., 'Ot::EnumToType<type>::type *').
{
        ASSERT(typeInfo);

#define CASE(ENUM)                                                           \
     case ENUM: {                                                            \
       typeInfo->setLinkedVariable(static_cast<Ot::EnumToType<ENUM>::type *> \
                                                                 (address)); \
     } break;                                                                \

    switch (type) {
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
        ASSERT(!"Reached");
      } break;
    };

#undef CASE
}

                         // ================
                         // function setType
                         // ================

void setType(TypeInfo *typeInfo, ElemType type)
    // Set the 'balcl::OptionType' element of the specified 'typeInfo' to the
    // specified 'type'.  The behavior is undefined unless
    // 'Ot::e_VOID != type'.  Note that this resets both the linked variable
    // and constraint of 'typeInfo'.
{
    ASSERT(typeInfo);

#define CASE(ENUM)                                                           \
     case Ot::e_##ENUM: {                                                    \
       typeInfo->setLinkedVariable(Ot::k_##ENUM);                            \
     } break;                                                                \

    switch (type) {
      case Ot::e_VOID: {
        ASSERT(!"Reached.");
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
        ASSERT(!"Reached");
      } break;
    };

#undef CASE
}

                          // =======================
                          // function createTypeInfo
                          // =======================

void createTypeInfo(TypeInfo *typeInfo,
                    ElemType  type,
                    void     *variable = 0,
                    void     *constraint = 0)
    // Set the specified 'typeInfo' to have the specified 'type'.  Optionally
    // specify 'variable', the address of a linked variable, and 'constraint',
    // the address of a constraint.  The behavior is undefined unless
    // 'Ot::e_VOID != type', 'variable' is 0 or can be cast to
    // 'Ot::EnumToType<type>::type', 'constraint' is 0 or can be cast to the
    // type defined by 'Constraint' for 'type', and if 'Ot::e_BOOL == type'
    // then 'constraint' must be 0.
{
    ASSERT(typeInfo);

    if (Ot::e_BOOL == type) {
        ASSERT(0 == constraint);
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
    // Return (by value) a 'TypeInfo' object having the specified 'type'.
    // Optionally specify 'variable', an address to be linked to the option.
    // Optionally specify a 'constraint' on the value of the option.  The
    // returned object uses the currently installed default allocator.  The
    // behavior is undefined unless 'Ot::e_VOID != type', 'variable' is 0 or
    // can be cast to 'Ot::EnumToType<type>::type', and 'constraint' is 0 or
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
    // 'src' can be cast to a pointer of 'Ot::EnumToType<type>::type',
    // 'Ot::e_VOID != type', and 'dst->type() == type'.
{
    ASSERT(dst);
    ASSERT(src);
    ASSERT(Ot::e_VOID  != type);
    ASSERT(dst->type() == type);

#define CASE(ENUM)                                                           \
     case ENUM: {                                                            \
       dst->set(*(static_cast<const Ot::EnumToType<ENUM>::type *>(src)));    \
     } break;                                                                \

    switch (type) {
      case Ot::e_VOID: {
        ASSERT(!"Reached.");
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
        ASSERT(!"Reached");
      } break;
    }
#undef CASE
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
    // uses the currently installed default allocator.  The behavior is
    // undefined unless 'Ot::e_VOID != type' and 'variable' can be cast to a
    // pointer of 'Ot::EnumToType<type>::type',
{
    ASSERT(Ot::e_VOID != type);

    OccurrenceInfo result(occurrenceType);

    if (occurrenceType != OccurrenceInfo::e_REQUIRED && variable) {
        OptionValue defaultValue(type);
        setOptionValue(&defaultValue, variable, type);

        result.setDefaultValue(defaultValue);
    }

    return result;
}
// BDE_VERIFY pragma: +AR01  // Type using allocator is returned by value

                          // ======================
                          // function parseTypeInfo
                          // ======================

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
    enum { SUCCESS = 0, FAILURE = -1 };

    bsl::string ptrString;

    if (skipWhiteSpace   (&input, input)
     || skipRequiredToken(&input, input, "{")
     || skipWhiteSpace   (&input, input)
     || skipRequiredToken(&input, input, "TYPE")
     || skipWhiteSpace   (&input, input)
     || skipRequiredToken(&input, input, Ot::toAscii(typeInfo.type())))
    {
        *endpos = input;
        return FAILURE;                                               // RETURN
    }

    if (typeInfo.linkedVariable()) {
        if (skipWhiteSpace   (&input, input)
         || skipRequiredToken(&input, input, "VARIABLE")
         || skipWhiteSpace   (&input, input))
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

    if (skipWhiteSpace   (&input, input)
     || skipRequiredToken(&input, input, "CONSTRAINT")
     || skipWhiteSpace   (&input, input))
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

    if (skipWhiteSpace   (&input, input)
     || skipRequiredToken(&input, input, "}"))
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
    enum { SUCCESS = 0, FAILURE = -1 };

    if (skipWhiteSpace(&input, input)) {
        *endpos = input;
        return FAILURE;                                               // RETURN
    }
    if (*input != '{') {
        if (OccurrenceInfo::e_REQUIRED == occurrenceInfo.occurrenceType()) {
            if (skipRequiredToken(endpos, input, "REQUIRED")) {
                return FAILURE;                                       // RETURN
            }
            return SUCCESS;                                           // RETURN
        }
        if (OccurrenceInfo::e_OPTIONAL == occurrenceInfo.occurrenceType()) {
            if (skipRequiredToken(endpos, input, "OPTIONAL")) {
                return FAILURE;                                       // RETURN
            }
            return SUCCESS;                                           // RETURN
        }
        if (OccurrenceInfo::e_HIDDEN == occurrenceInfo.occurrenceType()) {
            if (skipRequiredToken(endpos, input, "HIDDEN")) {
                return FAILURE;                                       // RETURN
            }
            return SUCCESS;                                           // RETURN
        }
        *endpos = input;
        return FAILURE;                                               // RETURN
    } else {
        ++input;
        if (OccurrenceInfo::e_OPTIONAL ==
                                             occurrenceInfo.occurrenceType()) {
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
        if (skipWhiteSpace   (&input, input)
         || skipRequiredToken(&input, input, "}")) {
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
    enum { SUCCESS = 0, FAILURE = -1 };

    bsl::string tagString, nameString, descriptionString;

    if (skipWhiteSpace   (&input, input)
     || skipRequiredToken(&input, input, "{")
     || skipWhiteSpace   (&input, input))
    {
        *endpos = input;
        return FAILURE;                                               // RETURN
    }

    if (skipRequiredToken(&input, input, "NON_OPTION")) {
        if (skipRequiredToken(&input, input, "TAG")
         || skipWhiteSpace   (&input, input)
         || TypesParser::parseQuotedString(&tagString, &input, input)
         || tagString != option.tagString())
        {
            *endpos = input;
            return FAILURE - 1;                                       // RETURN
        }
    }

    if (skipWhiteSpace   (&input, input)
     || skipRequiredToken(&input, input, "NAME")
     || skipWhiteSpace   (&input, input)
     || TypesParser::parseQuotedString(&nameString, &input, input)
     || nameString != option.name())
    {
        *endpos = input;
        return FAILURE - 2;                                           // RETURN
    }

    if (skipWhiteSpace   (&input, input)
     || skipRequiredToken(&input, input, "DESCRIPTION")
     || skipWhiteSpace   (&input, input)
     || TypesParser::parseQuotedString(&descriptionString, &input, input)
     || descriptionString != option.description())
    {
        *endpos = input;
        return FAILURE - 3;                                           // RETURN
    }
    if (skipWhiteSpace   (&input, input)
     || skipRequiredToken(&input, input, "TYPE_INFO"))
    {
        *endpos = input;
        return FAILURE - 4;                                           // RETURN
    }
    if (parseTypeInfo(&input, option.typeInfo(), input))
    {
        *endpos = input;
        return FAILURE - 5;                                           // RETURN
    }
    if (skipWhiteSpace   (&input, input)
     || skipRequiredToken(&input, input, "OCCURRENCE_INFO"))
    {
        *endpos = input;
        return FAILURE - 6;                                           // RETURN
    }

    if (parseOccurrenceInfo(&input, option.occurrenceInfo(), input))
    {
        *endpos = input;
        return FAILURE - 7;                                           // RETURN
    }

    if (skipWhiteSpace   (&input, input)
     || skipRequiredToken(&input, input, "}"))
    {
        *endpos = input;
        return FAILURE - 8;                                           // RETURN
    }

    *endpos = input;
    return SUCCESS;
}

OptionInfo::ArgType argType(const Obj& obj)
    // Return the argument type (i.e., 'e_FLAG', or 'e_OPTION' or
    // 'e_NON_OPTION' of the specified 'obj'.
{
    if (obj.tagString().empty()) {
        return OptionInfo::e_NON_OPTION;                              // RETURN
    }

    if (Ot::e_BOOL  == obj.typeInfo().type()) {
        return OptionInfo::e_FLAG;                                    // RETURN
    }

    return OptionInfo::e_OPTION;
}

}  // close namespace u
}  // close unnamed namespace

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

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator        ga("global", veryVeryVeryVerbose);
    bslma::TestAllocatorMonitor gam(&ga);

    bslma::Default::setGlobalAllocator(&ga);

    switch (test) { case 0:  // Zero is always the leading case.
      case 8: {
        // --------------------------------------------------------------------
        // VALIDATING ACCESSORS
        //
        // Concerns:
        //: 1 Each accessor returns a 'bool'.
        //:
        //: 2 Each accessor returns the expected value.
        //:
        //: 3 Each accessor is 'const'-qualified.
        //:
        //: 4 Empty tag is disallowed for flags (boolean options).
        //
        // Plan:
        //: 1 Use the "function pointer" idiom to demonstrate that each method
        //:   returns a 'bool', has the expected parameters, and is
        //:   'const'-qualified.  (C-1)
        //:
        //: 2 Using a table-driven organization, confirm that each method
        //:   returns the expected value for a set of inputs that explores
        //:   boundaries between valid and invalid input.  (C-2)
        //:
        //: 3 Always invoke accessors via a 'const' reference to the object
        //:   under test.  Absence of compiler error confirms that the
        //:   accessors are 'const'-qualified.  (C-3)
        //:
        //: 4 Ad-hoc test for a flag option having an empty tag.  (C-4).
        //
        // Testing:
        //   bool isLongTagValid(const char *longTag, ostream& stream) const;
        //   bool isTagValid(ostream& stream) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "VALIDATING ACCESSORS" << endl
                                  << "====================" << endl;

        if (veryVerbose) cout
            << endl
            << "Verify that the signatures and return types match expectation."
            << endl;
        {
            using namespace balcl;

            typedef bool (Obj::*isLongTagValidPtr)(const char *,
                                                   ostream&) const;

            typedef bool (Obj::*isTagValidPtr)(ostream&) const;

            isLongTagValidPtr functionPtrIsLongTagValid = &Obj::isLongTagValid;
                isTagValidPtr functionPtrIsTagValid     = &Obj::isTagValid;

            // quash potential compiler warnings
            (void)functionPtrIsLongTagValid;
            (void)functionPtrIsTagValid;
        }

        if (verbose) cout << "Testing 'isLongTagValid'." << endl;
        {
            struct {
                int         d_line;
                const char *d_longTag;
                bool        d_expected;
            } DATA[] = {
                //LINE LONG_TAG EXP
                //---- -------- -----
                { L_,  "-abc",  false }  // Lead     '-' disallowed.
              , { L_,  "a-bc",  true  }  // Embedded '-'    allowed.
              , { L_,  "abc-",  true  }  // Trailing '-'    allowed.

              , { L_,  "",      false }  // Minimum length is 2
              , { L_,  "a",     false }  // "                 "
              , { L_,  "ab",    true  }  // "                 "
              , { L_,  "abc",   true  }  // "                 "

              , { L_,  "|abc",  false }  // '|' disallowed
              , { L_,  "a|bc",  false }  // "            "
              , { L_,  "ab|c",  false }  // "            "
              , { L_,  "abc|",  false }  // "            "

              , { L_,  "Abc",   true  }  // arbitrary case
              , { L_,  "aBc",   true  }  // arbitrary case
              , { L_,  "abC",   true  }  // arbitrary case
              , { L_,  "AbC",   true  }  // arbitrary case
              , { L_,  "ABC",   true  }  // arbitrary case

              , { L_,  "=bc",   true  }  // non-alphanumeric allowed
              , { L_,  "=_c",   true  }  // "                       "
              , { L_,  "=_!",   true  }  // "                       "

              , { L_,  "a b",   false }
              , { L_,  "12",    true  }
              , { L_,  "1b",    true  }
            };

            enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < k_NUM_DATA; ++ti) {
                const int         LINE     = DATA[ti].d_line;
                const char *const LONG_TAG = DATA[ti].d_longTag;
                const bool        EXP      = DATA[ti].d_expected;

                if (veryVerbose) {
                    T_ P_(LINE) P_(LONG_TAG) P(EXP)
                }

                Obj mX; const Obj& X = mX;

                bsl::ostringstream oss;

                ASSERT(EXP == X.isLongTagValid(LONG_TAG, oss));

                if (EXP) {
                    ASSERT( oss.str().empty())
                } else {
                    ASSERT(!oss.str().empty())
                }

            }
        }

        if (verbose) cout << "Testing 'isTagValid'." << endl;
        {
            struct {
                int         d_line;
                const char *d_longTag;
                bool        d_expected;
            } DATA[] = {
                //LINE TAG       EXP
                //---- -------- -----
                { L_,  "",       true  }  // special case for 'e_BOOL' below
              , { L_,  "a",      false }  // short-only disallowed
              , { L_,  "a|b",    false }  // short w. long (not long enough)
              , { L_,  "a|bc",   true  }  // OK
              , { L_,  "ab",     true  }  // OK: long-only
              , { L_,  "-|bc",   false }  // short tag cannot be '-'

              , { L_,  " ",      false }  // blanks disallowed
              , { L_,  "a|b c ", false }  // blanks disallowed

              , { L_,  "|abc",   false }  // lead '|'      disallowed
              , { L_,  "a|b|c",  false }  // multiple '|'  disallowed
              , { L_,  "ab|cd",  false }  // multiple long disallowed
              , { L_,  "ab|",    false }  // trailing '|'  disallowed

              , { L_,  "ab3",   true  }  // alphanumeric allowed
              , { L_,  "1|ab",  true  }  // "                  "
              , { L_,  "1|23",  true  }  // "                  "

              , { L_,  "=_!",   true  }  // most non-alphanumeric allowed
              , { L_,  "%|@#$", true  }  // most non-alphanumeric allowed
            };

            enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < k_NUM_DATA; ++ti) {
                const int         LINE = DATA[ti].d_line;
                const char *const TAG  = DATA[ti].d_longTag;
                const bool        EXP  = DATA[ti].d_expected;

                if (veryVerbose) {
                    T_ P_(LINE) P_(TAG) P(EXP)
                }

                const OptionInfo OI = { TAG
                                      , ""
                                      , ""
                                      , TypeInfo()
                                      , OccurrenceInfo()
                                      };

                Obj mX(OI); const Obj& X = mX;

                bsl::ostringstream oss;

                ASSERT(EXP == X.isTagValid(oss));

                if (EXP) {
                    ASSERT( oss.str().empty())
                } else {
                    ASSERT(!oss.str().empty())
                }
            }
        }

        if (verbose) cout << "Testing 'isTagValid' for 'e_BOOL'." << endl;
        {
            const OptionInfo OI = { "" // empty tag
                                  , ""
                                  , ""
                                  , TypeInfo(Ot::k_BOOL)
                                  , OccurrenceInfo()
                                  };

            Obj mX(OI); const Obj& X = mX;

            bsl::ostringstream oss;

            ASSERT(Ot::e_BOOL == X.typeInfo().type());
            ASSERT(true       == X.tagString().empty());
            ASSERT(false      == X.isTagValid(oss));
            ASSERT(false      == oss.str().empty());
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // DEFAULT CONSTRUCTOR
        //
        // Concerns:
        //: 1 The default constructor creates an object having the default
        //:   value for each of the object's salient attributes and the
        //:   intended allocator.
        //:
        //: 2 The default constructor allows the explicit specification of the
        //:   object allocator, or no specification, or a 0-specification.
        //:   In the later two cases, the default allocator is used.
        //:
        //: 3 The default constructor unconditionally allocates from the
        //:   default allocator (because of its 'TypeInfo' attribute).
        //:
        //: 4 The default constructor allocates from the intended allocator
        //:   when expected.
        //
        // Plan:
        //: 1 Use the "footprint" idiom to invoke the default constructor
        //:   by each of the ways the object allocator can be specified.
        //:   Each of those objects are subjected to identical tests.
        //:   (C-2).
        //:
        //: 2 Use the accessors methods to confirm that newly constructed
        //:   object uses has the intended values for each salient attributes,
        //:   that it uses the intended allocator, and that the allocator has
        //:   been propagated to each of the allocating members.  (C-1).
        //:
        //: 3 Use 'bslma::TestAllocatorMonitor' objects to confirm that
        //:   memory is allocated as expected and is returned upon destruction
        //:   of the object.  (C-3)
        //:
        //: 4 We need not calculate the expected allocator for each object
        //:   constructed (a complicated function of the many input parameters)
        //:   because the behavior of each subordinate type has already been
        //:   confirmed in their individual test drivers.  Thus it suffices to
        //:   confirm that each attribute reports the expected value and
        //:   reports that it is using the allocator we intend.  (C-4)
        //
        // Testing:
        //   Option();
        //   Option(bslma::Allocator *basicAllocator);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "DEFAULT CONSTRUCTOR" << endl
                                  << "===================" << endl;

        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
            const char CONFIG = cfg;

            if (veryVerbose) {
                T_ P(CONFIG);
            }

            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
            bslma::TestAllocator da("default",   veryVeryVeryVerbose);

            bslma::TestAllocatorMonitor  sam(&sa);
            bslma::TestAllocatorMonitor  dam(&da);
            bslma::DefaultAllocatorGuard dag(&da);

            bslma::TestAllocator *objAllocatorPtr = 'a' == CONFIG ? &da :
                                                    'b' == CONFIG ? &da :
                                                    'c' == CONFIG ? &sa :
                                                    /* error */     0   ;
            ASSERTV(CONFIG, objAllocatorPtr)

            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = 'c' == CONFIG ? da : sa;

            bslma::TestAllocatorMonitor  oam(&oa);
            bslma::TestAllocatorMonitor noam(&noa);

            Obj *objPtr = 'a' == CONFIG ? new (fa) Obj      : // ACTION
                          'b' == CONFIG ? new (fa) Obj(0)   : // ACTION
                          'c' == CONFIG ? new (fa) Obj(&sa) : // ACTION
                          /* error */     0                 ;
            ASSERTV(CONFIG, objPtr);
            ASSERTV(CONFIG, dam.isTotalUp());

            Obj& mX = *objPtr;  const Obj& X = mX;

            // Vet object allocators.

            ASSERTV(CONFIG, &oa == X.allocator());
            ASSERTV(CONFIG, &oa == X.   description().allocator());
            ASSERTV(CONFIG, &oa == X.          name().allocator());
            ASSERTV(CONFIG, &oa == X.     tagString().allocator());
            ASSERTV(CONFIG, &oa == X.      typeInfo().allocator());
            ASSERTV(CONFIG, &oa == X.occurrenceInfo().allocator());

            // Vet object value.

            ASSERT(OptionInfo::e_NON_OPTION   == X.argType());
            ASSERT(""                         == X.description());
            ASSERT(false                      == X.isArray());
            ASSERT(""                         == X.name());
            ASSERT(OccurrenceInfo::e_OPTIONAL == X.occurrenceInfo());
            ASSERT(""                         == X.tagString());
            ASSERT(TypeInfo()                 == X.typeInfo());

            {
                bsl::ostringstream oss;

                ASSERT(true  == X.isTagValid(oss));
                ASSERT( oss.str().empty()); oss.str(""); oss.clear();

                ASSERT(false == X.isDescriptionValid(oss));
                ASSERT(!oss.str().empty()); oss.str(""); oss.clear();

                ASSERT(true  == X.isTagValid(oss));
                ASSERT( oss.str().empty()); oss.str(""); oss.clear();

                ASSERT(false == X.isNameValid(oss));
                ASSERT(!oss.str().empty()); oss.str(""); oss.clear();
            }

            fa.deleteObject(objPtr);  // Clean up

            if (&oa == &da) {
                ASSERTV(CONFIG,  sam.isTotalSame());
            }

            ASSERTV(CONFIG,  oam.isInUseSame());
            ASSERTV(CONFIG, noam.isInUseSame());
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // COPY ASSIGNMENT
        //
        // Concerns:
        //: 1 The two operators have the expected signatures.
        //:
        //: 2 One object can be assigned to another irrespective of the value
        //:   of each of those objects.  After assignment, the 'lhs' compares
        //:   equal to the 'rhs'.
        //:
        //: 3 Alias-safety: An object an be assigned to itself.
        //:
        //: 4 The allocator of the assigned-to object ('lhs') is preserved.
        //:
        //: 5 The assignment operation returns a reference to the 'lhs' object.
        //:
        //: 6 The operation does not change the 'rhs'.
        //:
        //: 7 Assigning an 'OptionInfo' 'struct' to an 'Option' object produces
        //:   the same result as assigning an 'Option' object constructed from
        //:   the 'OptionInfo' 'struct'.
        //
        // Plan:
        //: 1 Use the "pointer-to-method" idiom to have the compiler check the
        //:   signature.  (C-1)
        //:
        //: 2 For a representative set of objects assign each object with
        //:   itself and to every other object.  The representative set used is
        //:   the same table as was used in the test of operator equality in
        //:   which each entry differed from the others in one salient
        //:   attribute.  Using this set demonstrates that each salient
        //:   attribute is handled by the assignment operators.  Use equality
        //:   comparison to confirm that each object is in the expected state
        //:   afterward.  (C-2)
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
        //:
        //: 7 Each assignment operation and test is mirrored by an
        //:   analogous operation using an 'OptionInfo' 'struct' as the 'rhs'.
        //:   (C-7)
        //
        // Testing:
        //   Option& operator=(const Option&     rhs);
        //   Option& operator=(const OptionInfo& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "COPY ASSIGNMENT" << endl
                                  << "===============" << endl;

        if (veryVerbose) cout
                 << endl
                 << "Verify that the signatures and return types are standard."
                 << endl;
        {
            using namespace balcl;

            typedef Obj& (Obj::* classOperatorPtr)(const Obj&);

#if defined(BSLS_PLATFORM_OS_WINDOWS) \
 && defined(BSLS_PLATFORM_CMP_MSVC)   \
 && BSLS_PLATFORM_CMP_VERSION <= 1900 // MSVC 2015 (error)
            typedef Obj& (Obj::*structOperatorPtr)(
                                        const BloombergLP::balcl::OptionInfo&);
#else
            typedef Obj& (Obj::*structOperatorPtr)(const OptionInfo&);
#endif
             classOperatorPtr  classOperatorAssignment = &Obj::operator=;
            structOperatorPtr structOperatorAssignment = &Obj::operator=;

            // quash potential compiler warnings
            (void) classOperatorAssignment;
            (void)structOperatorAssignment;
        }

        if (veryVerbose) cout
                            << endl
                            << "Confirm that all members contribute to results"
                            << endl;
        {
            bslma::TestAllocator sa1("supplied1", veryVeryVeryVerbose);
            bslma::TestAllocator sa2("supplied2", veryVeryVeryVerbose);

            bslma::TestAllocator         da("default", veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&da);

            const char * const   A1 = "tagA1";
            const char * const   B1 = "tagB1";

            const char * const   A2 = "nameA2";
            const char * const   B2 = "nameB2";

            const char * const   A3 = "descriptionA3";
            const char * const   B3 = "descriptionB3";

            char charLinkedVariable;
            bool  intLinkedVariable;

            const TypeInfo       A4 = TypeInfo(&charLinkedVariable);
            const TypeInfo       B4 = TypeInfo(& intLinkedVariable);

            char charDefaultValue = 'a';
            bool  intDefaultValue =  0 ;

            const OccurrenceInfo A5 = OccurrenceInfo(charDefaultValue);
            const OccurrenceInfo B5 = OccurrenceInfo( intDefaultValue);

            const struct {
                int             d_line;
                const char     *d_tag_p;
                const char     *d_name_p;
                const char     *d_desc_p;
                TypeInfo        d_typeInfo;
                OccurrenceInfo  d_occurrenceInfo;
            } DATA[] = {
                //  LINE TAG  NAME DESC TI  OI
                //  ---- ---  ---- ---- --  --
                  { L_,  A1,  A2,  A3,  A4, A5 }  // baseLine

                , { L_,  B1,  A2,  A3,  A4, A5 }
                , { L_,  A1,  B2,  A3,  A4, A5 }
                , { L_,  A1,  A2,  B3,  A4, A5 }
                , { L_,  A1,  A2,  A3,  B4, A5 }
                , { L_,  A1,  A2,  A3,  A4, B5 }
            };

            enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < k_NUM_DATA; ++ti) {
                const int            LINE1       = DATA[ti].d_line;
                const char * const   TAG1        = DATA[ti].d_tag_p;
                const char * const   NAME1       = DATA[ti].d_name_p;
                const char * const   DESC1       = DATA[ti].d_desc_p;
                const TypeInfo       TYPE_INFO1  = DATA[ti].d_typeInfo;
                const OccurrenceInfo OCCUR_INFO1 = DATA[ti].d_occurrenceInfo;

                if (veryVerbose) {
                    T_ P_(LINE1)
                       P_(TAG1)
                       P_(NAME1)
                       P_(DESC1)
                       P_(TYPE_INFO1)
                       P(OCCUR_INFO1)
                }

                const OptionInfo OiX = { TAG1
                                       , NAME1
                                       , DESC1
                                       , TYPE_INFO1
                                       , OCCUR_INFO1
                                       };

                bslma::TestAllocator *saX = &sa1;

                Obj mX(OiX, saX); const Obj& X = mX;  // test    object
                Obj mZ(OiX, saX); const Obj& Z = mZ;  // control object
                Obj mU(OiX, saX); const Obj& U = mU;  // 'struct' test

                // Testing self-assignment (alias safety).

                Obj *mRx = &(mX = X);    // ACTION
                Obj *mRu = &(mU = OiX);  // ACTION

                // Vet "assigned-to" object.

                ASSERT(mRx == &mX);
                ASSERT(Z   == X);
                ASSERT(saX == X.allocator());
                ASSERT(saX == Z.allocator());

                ASSERT(mRu == &mU);
                ASSERT(Z   == U);
                ASSERT(saX == U.allocator());

                for (int tj = 0; tj < k_NUM_DATA; ++tj) {
                    const int            LINE2       = DATA[tj].d_line;
                    const char * const   TAG2        = DATA[tj].d_tag_p;
                    const char * const   NAME2       = DATA[tj].d_name_p;
                    const char * const   DESC2       = DATA[tj].d_desc_p;
                    const TypeInfo       TYPE_INFO2  = DATA[tj].d_typeInfo;
                    const OccurrenceInfo OCCUR_INFO2 = DATA[tj].
                                                              d_occurrenceInfo;
                    if (veryVerbose) {
                        T_ T_ P_(LINE2)
                              P_(TAG2)
                              P_(NAME2)
                              P_(DESC2)
                              P_(TYPE_INFO2)
                              P(OCCUR_INFO2)
                    }

                    const OptionInfo OiY = { TAG2
                                           , NAME2
                                           , DESC2
                                           , TYPE_INFO2
                                           , OCCUR_INFO2
                                           };

                    for (int m = 0; m < 2; ++m) {

                        if (veryVerbose) { T_ T_ T_  P(m) }

                        bslma::TestAllocator *saY = m % 2
                                                  ? &sa1  //     same as 'X'
                                                  : &sa2; // not same as 'X'

                        Obj mY(OiY, saY); const Obj& Y = mY;
                        Obj mV(OiY, saY); const Obj& V = mV;

                        Obj *mRy = &(mY = X);    // ACTION
                        Obj *mRv = &(mV = OiX);  // ACTION

                        // Vet results

                        ASSERTV(LINE1, LINE2, mRy           == &mY);
                        ASSERTV(LINE1, LINE2, X             == Y);
                        ASSERTV(LINE1, LINE2, saY           == Y.allocator());
                        ASSERTV(LINE1, LINE2, X             == Z);
                        ASSERTV(LINE1, LINE2, X.allocator() == Z.allocator());

                        ASSERTV(LINE1, LINE2, mRv           == &mV);
                        ASSERTV(LINE1, LINE2, X             == V);
                        ASSERTV(LINE1, LINE2, saY           == V.allocator());
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
        //:   3 The constructor is exception safe.
        //:
        //:   4 The allocator is used as expected.
        //:
        //: 3 The original object is unchanged when copied.
        //
        // Plans:
        //: 1 Do table-driven testing using the tables described in {Input
        //:   Tables}.  Construct two objects for each combination of input.
        //:   One is passed to the copy constructor (the "original" object) the
        //:   other is unused.  Confirm that the original remains the same as
        //:   the unused object.
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
        //
        //:   3 We need not calculate the expected allocator for each object
        //:     constructed (a complicated function of the many input
        //:     parameters) because the behavior of each subordinate type has
        //:     already been confirmed in their individual test drivers.  Thus
        //:     it suffices to confirm that each attribute reports the expected
        //:     value and reports that it is using the allocator we intend.
        //:     (C-2.4)
        //:
        //:   4 Use the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros to
        //:     confirm that memory allocation is exception safe.
        //:
        //:     o Although each of the subordinate members of this class has
        //:       been shown to be exception safe (in their individual test
        //:       drivers) we must confirm that when one of those member
        //:       constructors fail, the resources allocated by any previously
        //:       successfully created members are recovered.
        //:
        //:     o Note that the last time through this macro loop corresponds
        //:       to the "normal" (no exception) test case.
        //
        // Testing:
        //   Option(const Option& original, *bA = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;

// BDE_VERIFY pragma: -TP21    // Loops must contain very verbose action
// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
        for (int i = 0; i < NUM_OPTION_TAGS;         ++i) {
        for (int j = 0; j < NUM_OPTION_NAMES;        ++j) {
        for (int k = 0; k < NUM_OPTION_DESCRIPTIONS; ++k) {
        for (int l = 0; l < NUM_OPTION_TYPEINFO;     ++l) {
        for (int m = 0; m < NUM_OPTION_OCCURRENCES;  ++m) {

            const int n = l % NUM_OPTION_DEFAULT_VALUES;
            ASSERT(OPTION_DEFAULT_VALUES[n].d_type
                   ==    OPTION_TYPEINFO[l].d_type);

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

            const TypeInfo       TYPEINFO        = u::createTypeInfo(
                                                                   TYPE,
                                                                   VARIABLE,
                                                                   CONSTRAINT);
            const OccurrenceInfo OCCURRENCE_INFO = u::createOccurrenceInfo(
                                                                OTYPE,
                                                                TYPE,
                                                                DEFAULT_VALUE);
            const OptionInfo     OI              = { TAG
                                                   , NAME
                                                   , DESC
                                                   , TYPEINFO
                                                   , OCCURRENCE_INFO
                                                   };

            if (veryVerbose) {
                T_ P_(LINE1) P_(LINE2) P_(LINE3) P_(LINE4) P(LINE5)
                T_ P(OI)
            }

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            const Obj  Z(OI, &scratch);
            const Obj ZZ(OI, &scratch);

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
                                                        /* error */     0   ;
                ASSERTV(CONFIG, objAllocatorPtr)

                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = 'c' == CONFIG ? da : sa;

                bslma::TestAllocatorMonitor  oam(&oa);
                bslma::TestAllocatorMonitor noam(&noa);

                int  completeExceptionCount = 0;
                Obj *objPtr                 = 0;

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {

                    objPtr = 'a' == CONFIG ? new (fa) Obj(Z)      : // ACTION
                             'b' == CONFIG ? new (fa) Obj(Z, 0)   : // ACTION
                             'c' == CONFIG ? new (fa) Obj(Z, &sa) : // ACTION
                             /* error */     0                    ;

                    ++completeExceptionCount;
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(CONFIG, 1 == completeExceptionCount);
                ASSERTV(CONFIG, objPtr);

                Obj& mX = *objPtr;  const Obj& X = mX;

                // Vet object allocators.
                ASSERTV(CONFIG, &oa == X.allocator());
                ASSERTV(CONFIG, &oa == X.   description().allocator());
                ASSERTV(CONFIG, &oa == X.          name().allocator());
                ASSERTV(CONFIG, &oa == X.     tagString().allocator());
                ASSERTV(CONFIG, &oa == X.      typeInfo().allocator());
                ASSERTV(CONFIG, &oa == X.occurrenceInfo().allocator());

                ASSERTV(CONFIG, noam.isTotalSame());

                // Vet the object value.
                ASSERT(Z == X);

                // Vet the original object ('Z') state.
                ASSERT(Z             == ZZ);
                ASSERT(Z.allocator() == ZZ.allocator());

                fa.deleteObject(objPtr);  // Clean up

                ASSERTV(CONFIG,  oam.isInUseSame());
                ASSERTV(CONFIG, noam.isInUseSame());
            }
        }
        }
        }
        }
        }
// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line
// BDE_VERIFY pragma: +TP21    // Loops must contain very verbose action
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        //
        // Concerns:
        //: 1 Salient Members:
        //:
        //:   1 Two objects, 'X' and 'Y', compare equal if and only if each of
        //:     their corresponding salient attributes respectively compare
        //:     equal.
        //:
        //:   2 All salient attributes participate in the comparison.
        //:
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
        //: 2 Using the table-driven technique, specify a set of unique object
        //:   values (one per row) in terms of their individual salient
        //:   attributes such that each row differs from the others with
        //:   respect to *one* salient value.  Thus, objects created from
        //:   different rows of salient values compare unequal.  By taking the
        //:   cross product of these objects, we demonstrate that a difference
        //:   in *any* individual salient value results in inequality, thus
        //:   demonstrating that each salient value contributes to the equality
        //:   comparison.  Note that objects compare equal for only those cases
        //:   of the cross product when both rows are the same.  (C-1)
        //:
        //: 3 For each test of equality, create a parallel test that checks
        //:   inequality (the inverse operator), and (when the two arguments
        //:   are different) also create a test case where the two arguments
        //:   are switched (showing commutativity).  (C-2)
        //:
        //: 4 Install a test allocator as the default allocator.  Create a test
        //:   allocator monitor object before each group of operator tests and
        //:   confirm afterwards that the 'isTotalSame' returns 'true' (showing
        //:   that no allocations occurred when exercising the operators).
        //
        //: 5 Repeat each test between two objects so that the objects use the
        //:   same allocator in one test and use different allocators in the
        //:   other.  Results should not change and thereby show that the
        //:   object allocator is not salient to equality.  (C-1.3)
        //
        // Testing:
        //   bool operator==(const Option& lhs, rhs);
        //   bool operator!=(const Option& lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EQUALITY-COMPARISON OPERATORS" << endl
                          << "=============================" << endl;

        if (veryVerbose) cout
                 << endl
                 << "Verify that the signatures and return types are standard."
                 << endl;
        {
            using namespace balcl;
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            operatorPtr operatorEq = operator==;
            operatorPtr operatorNe = operator!=;

            (void)operatorEq;  // quash potential compiler warnings
            (void)operatorNe;
        }

        if (veryVerbose) cout
                            << endl
                            << "Confirm that all members contribute to results"
                            << endl;
        {
            bslma::TestAllocator sa1("supplied1", veryVeryVeryVerbose);
            bslma::TestAllocator sa2("supplied2", veryVeryVeryVerbose);

            bslma::TestAllocator         da("default", veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&da);

            const char * const   A1 = "tagA1";
            const char * const   B1 = "tagB1";

            const char * const   A2 = "nameA2";
            const char * const   B2 = "nameB2";

            const char * const   A3 = "descriptionA3";
            const char * const   B3 = "descriptionB3";

            char charLinkedVariable;
            bool  intLinkedVariable;

            const TypeInfo       A4 = TypeInfo(&charLinkedVariable);
            const TypeInfo       B4 = TypeInfo(& intLinkedVariable);

            char charDefaultValue = 'a';
            bool  intDefaultValue =  0 ;

            const OccurrenceInfo A5 = OccurrenceInfo(charDefaultValue);
            const OccurrenceInfo B5 = OccurrenceInfo( intDefaultValue);

            const struct {
                int             d_line;
                const char     *d_tag_p;
                const char     *d_name_p;
                const char     *d_desc_p;
                TypeInfo        d_typeInfo;
                OccurrenceInfo  d_occurrenceInfo;
            } DATA[] = {
                //  LINE TAG  NAME DESC TI  OI
                //  ---- ---  ---- ---- --  --
                  { L_,  A1,  A2,  A3,  A4, A5 }  // baseLine

                , { L_,  B1,  A2,  A3,  A4, A5 }
                , { L_,  A1,  B2,  A3,  A4, A5 }
                , { L_,  A1,  A2,  B3,  A4, A5 }
                , { L_,  A1,  A2,  A3,  B4, A5 }
                , { L_,  A1,  A2,  A3,  A4, B5 }
            };

            enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < k_NUM_DATA; ++ti) {
                const int            LINE1       = DATA[ti].d_line;
                const char * const   TAG1        = DATA[ti].d_tag_p;
                const char * const   NAME1       = DATA[ti].d_name_p;
                const char * const   DESC1       = DATA[ti].d_desc_p;
                const TypeInfo       TYPE_INFO1  = DATA[ti].d_typeInfo;
                const OccurrenceInfo OCCUR_INFO1 = DATA[ti].d_occurrenceInfo;

                if (veryVerbose) {
                    T_ P_(LINE1)
                       P_(TAG1)
                       P_(NAME1)
                       P_(DESC1)
                       P_(TYPE_INFO1)
                       P(OCCUR_INFO1)
                }

                const OptionInfo OiX = { TAG1
                                       , NAME1
                                       , DESC1
                                       , TYPE_INFO1
                                       , OCCUR_INFO1
                                       };

                bslma::TestAllocator *saX = &sa1;

                Obj mX(OiX, saX); const Obj& X = mX;

                bslma::TestAllocatorMonitor dam (&da);
                bslma::TestAllocatorMonitor samX(saX);

                ASSERTV(LINE1, X,   X == X );  // identity
                ASSERTV(LINE1, X, !(X != X));  // inverse

                ASSERT(dam .isTotalSame());
                ASSERT(samX.isTotalSame());

                for (int tj = 0; tj < k_NUM_DATA; ++tj) {
                    const int            LINE2       = DATA[tj].d_line;
                    const char * const   TAG2        = DATA[tj].d_tag_p;
                    const char * const   NAME2       = DATA[tj].d_name_p;
                    const char * const   DESC2       = DATA[tj].d_desc_p;
                    const TypeInfo       TYPE_INFO2  = DATA[tj].d_typeInfo;
                    const OccurrenceInfo OCCUR_INFO2 = DATA[tj].
                                                              d_occurrenceInfo;
                    if (veryVerbose) {
                        T_ T_ P_(LINE2)
                              P_(TAG2)
                              P_(NAME2)
                              P_(DESC2)
                              P_(TYPE_INFO2)
                              P(OCCUR_INFO2)
                    }

                    const OptionInfo OiY = { TAG2
                                           , NAME2
                                           , DESC2
                                           , TYPE_INFO2
                                           , OCCUR_INFO2
                                           };

                    const bool EXP = ti == tj;  // expected value for equality
                                                // comparison

                    for (int m = 0; m < 2; ++m) {

                        if (veryVerbose) { T_ T_ T_  P(m) }

                        bslma::TestAllocator *saY = m % 2
                                                  ? &sa1  //     same as 'X'
                                                  : &sa2; // not same as 'X'

                        Obj mY(OiY, saY); const Obj& Y = mY;

                        bslma::TestAllocatorMonitor samX(saX);
                        bslma::TestAllocatorMonitor samY(saY);
                        bslma::TestAllocatorMonitor dam(&da);

                        // commutatively
                        ASSERTV(LINE1, LINE2, X, Y,  EXP == (X == Y));
                        ASSERTV(LINE1, LINE2, Y, X,  EXP == (Y == X));

                        // inverse
                        ASSERTV(LINE1, LINE2, X, Y, !EXP == (X != Y));
                        ASSERTV(LINE1, LINE2, Y, X, !EXP == (Y != X));

                        ASSERT(dam .isTotalSame());
                        ASSERT(samY.isTotalSame());
                        ASSERT(samX.isTotalSame());
                    }
                }
            }
        }
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
        //:   spaces-per-level arguments have their default values.
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
        //:     that the output of 'print' matches the expected output for the
        //:     given object.  (C-2)
        //:
        //:   3 Use 'bsl::strspn' to confirm that indentation matches 'level'
        //:     and 'spacesPerLevel' and that the default values are as
        //:     expected.  (C-6)
        //
        // Testing:
        //   ostream& print(ostream& stream, int level = 0, int spl = 4) const;
        //   ostream& operator<<(ostream& strm, const Option& rhs);
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

        if (verbose) cout << "Testing 'print' and 'operator<<'." << endl;

// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
        for (int i = 0; i < NUM_OPTION_TAGS;         ++i) {
        for (int j = 0; j < NUM_OPTION_NAMES;        ++j) {
        for (int k = 0; k < NUM_OPTION_DESCRIPTIONS; ++k) {
        for (int l = 0; l < NUM_OPTION_TYPEINFO;     ++l) {
        for (int m = 0; m < NUM_OPTION_OCCURRENCES;  ++m) {

            const int n = l % NUM_OPTION_DEFAULT_VALUES;
            ASSERT(OPTION_DEFAULT_VALUES[n].d_type
                   ==    OPTION_TYPEINFO[l].d_type);

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

            const TypeInfo       TYPEINFO        = u::createTypeInfo(
                                                                   TYPE,
                                                                   VARIABLE,
                                                                   CONSTRAINT);
            const OccurrenceInfo OCCURRENCE_INFO = u::createOccurrenceInfo(
                                                                OTYPE,
                                                                TYPE,
                                                                DEFAULT_VALUE);
            const OptionInfo     OI              = { TAG
                                                   , NAME
                                                   , DESC
                                                   , TYPEINFO
                                                   , OCCURRENCE_INFO
                                                   };

            if (veryVerbose) {
                T_ P_(LINE1) P_(LINE2) P_(LINE3) P_(LINE4) P(LINE5)
                T_ P(OI)
            }

            Obj mX(OI); const Obj& X = mX;

            bsl::ostringstream ossMethod;
            bsl::ostringstream ossOperator;

            ASSERT(&ossMethod   == &X.print(ossMethod));  // ACTION
            ASSERT(&ossOperator == &(ossOperator << X));  // ACTION

            bsl::string stringMethod  (ossMethod  .str());
            bsl::string stringOperator(ossOperator.str());

            ASSERT(0 * 4 == bsl::strspn(stringMethod  .c_str(), " "));
            ASSERT(0 * 4 == bsl::strspn(stringOperator.c_str(), " "));

            ASSERT(stringMethod == stringOperator);

            if (veryVerbose) {
                T_ P(X)
                T_ P(stringMethod)
                T_ P(stringOperator)
            }

            const char *output   = stringMethod.c_str();
            const int   parseRet = u::parseOption(&output, X, output);
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
        }
        }
        }
// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATOR (VALUE CTOR), BASIC ACCESSORS & DTOR
        //
        // Concerns:
        //: 1 Each object created using the value constructor has the expected
        //:   attributes: the three strings, the 'typeInfo' and
        //:   'occurrenceInfo' attributes, and the expected allocator.
        //:   1 The constructor allocates when expected.
        //:   2 The allocator can be specified, or not, or specified as 0.
        //:
        //: 2 The object allocates from the intended allocator, and no other.
        //:   1 Allocation is exception safe.
        //:   2 The 'bslma::UsesBslmaAllocator' trait is set for this class.
        //:
        //: 3 The basic accessors provide a view of the object state that is
        //:   consistent with the state of the objects set by the constructor
        //:   and the primary manipulators.
        //:
        //: 4 The non-basic (calculated) accessors return values consistent
        //:   with the object state as determined by the basic allocators.
        //:
        //: 5 All accessors are 'const'-qualified.
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using a table-driven organization (see {Input Tables}), use the
        //:   "footprint" idiom to invoke the constructor specifying for the
        //:   allocator: nothing, 0, and a supplied allocator.  (C-1.2)
        //
        //:   1 In each scenario, the object uses a 'bslma::TestAllocator'
        //:     (explicitly, or via the currently installed default allocator).
        //:
        //:   2 Use 'bslma::TestAllocatorMonitor' object to confirm that
        //:     allocation occurs, or not, as expected.  (C-1.1)
        //:
        //:     1 We need not calculate the expected allocator for each object
        //:       constructed (a complicated function of the many input
        //:       parameters) because the behavior of each subordinate type has
        //:       already been confirmed in their individual test drivers.
        //:       Thus it suffices to confirm that each attribute reports the
        //:       expected value and reports that it is using the allocator we
        //:       intend.
        //:
        //:   2 For each creation path (specified allocator, no specification,
        //:     0-specification), the resulting object is subject to identical
        //:     tests, thereby demonstrating that each creation-path produces
        //:     identical results (modulo the object allocator).
        //:
        //:   3 Confirm that
        //:     'bslma::UsesBslmaAllocator<balcl::TypeInfo>::value' is 'true'
        //:     in a compile-time assertion at file scope.  (C-2.2)
        //:
        //:   4 Use the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros to
        //:     confirm that memory allocation is exception safe.
        //:
        //:     o Although each of the subordinate members of this class has
        //:       been shown to be exception safe (in their individual test
        //:       drivers) we must confirm that when one of those member
        //:       constructors fails, the resources allocated by any previously
        //:       successfully created members are recovered.
        //:
        //:     o Note that the last time through this macro loop corresponds
        //:       to the "normal" (no exception) test case.
        //:
        //: 2 Compare the results of non-basic accessors with values calculated
        //:   from the basic accessors using (independent) test functions.
        //:
        //: 3 Always invoke accessors via a 'const' reference to the object
        //:   under test.  Absence of compiler error confirms that the
        //:   accessors are 'const'-qualified.  (C-5)
        //:
        //: 4 Use 'BSLS_ASSERTTEST_*' facilities for negative testing.  (C-6)
        //
        // Testing:
        //   Option(const OptionInfo& info, *bA = 0);
        //   ~Option();
        //   operator const OptionInfo&() const;
        //   Option::ArgType argType() const;
        //   const string& description() const;
        //   bool isArray() const;
        //   bool isDescriptionValid(ostream& stream) const;
        //   bool isNameValid(ostream& stream) const;
        //   const char *longTag() const;
        //   const string& name() const;
        //   const OccurrenceInfo& occurrenceInfo() const;
        //   char shortTag() const;
        //   const string& tagString() const;
        //   const TypeInfo& typeInfo() const;
        //   bslma::Allocator *allocator() const;
        // --------------------------------------------------------------------

        if (verbose) cout
         << endl
         << "PRIMARY MANIPULATOR (VALUE CTOR), BASIC ACCESSORS & DTOR" << endl
         << "========================================================" << endl;

// BDE_VERIFY pragma: -TP21    // Loops must contain very verbose action
// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
        for (int i = 0; i < NUM_OPTION_TAGS;         ++i) {
        for (int j = 0; j < NUM_OPTION_NAMES;        ++j) {
        for (int k = 0; k < NUM_OPTION_DESCRIPTIONS; ++k) {
        for (int l = 0; l < NUM_OPTION_TYPEINFO;     ++l) {
        for (int m = 0; m < NUM_OPTION_OCCURRENCES;  ++m) {

            const int n = l % NUM_OPTION_DEFAULT_VALUES;
            ASSERT(OPTION_DEFAULT_VALUES[n].d_type
                   ==    OPTION_TYPEINFO[l].d_type);

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

            const TypeInfo       TYPEINFO        = u::createTypeInfo(
                                                                   TYPE,
                                                                   VARIABLE,
                                                                   CONSTRAINT);
            const OccurrenceInfo OCCURRENCE_INFO = u::createOccurrenceInfo(
                                                                OTYPE,
                                                                TYPE,
                                                                DEFAULT_VALUE);
            const OptionInfo     OI              = { TAG
                                                   , NAME
                                                   , DESC
                                                   , TYPEINFO
                                                   , OCCURRENCE_INFO
                                                   };

            if (veryVerbose) {
                T_ P_(LINE1) P_(LINE2) P_(LINE3) P_(LINE4) P(LINE5)
                T_ P(OI)
            }

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
                                                        /* error */     0   ;
                ASSERTV(CONFIG, objAllocatorPtr)

                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = 'c' == CONFIG ? da : sa;

                bslma::TestAllocatorMonitor  oam(&oa);
                bslma::TestAllocatorMonitor noam(&noa);

                int  completeExceptionCount = 0;
                Obj *objPtr                 = 0;

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {

                    objPtr = 'a' == CONFIG ? new (fa) Obj(OI)      : // ACTION
                             'b' == CONFIG ? new (fa) Obj(OI, 0)   : // ACTION
                             'c' == CONFIG ? new (fa) Obj(OI, &sa) : // ACTION
                             /* error */     0                     ;

                    ++completeExceptionCount;
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(CONFIG, 1 == completeExceptionCount);
                ASSERTV(CONFIG, objPtr);

                Obj& mX = *objPtr;  const Obj& X = mX;

                // Vet object allocators.
                ASSERTV(CONFIG, &oa == X.allocator());
                ASSERTV(CONFIG, &oa == X.   description().allocator());
                ASSERTV(CONFIG, &oa == X.          name().allocator());
                ASSERTV(CONFIG, &oa == X.     tagString().allocator());
                ASSERTV(CONFIG, &oa == X.      typeInfo().allocator());
                ASSERTV(CONFIG, &oa == X.occurrenceInfo().allocator());

                // Vet object value.
                ASSERT(DESC            == X.description());
                ASSERT(NAME            == X.name());
                ASSERT(OCCURRENCE_INFO == X.occurrenceInfo());
                ASSERT(TAG             == X.tagString());
                ASSERT(TYPEINFO        == X.typeInfo());

                // Other accessors.

                ASSERT(OI == X);

                const OptionInfo::ArgType ARG_TYPE = u::argType(X);
                const bool                IS_ARRAY = Ot::isArrayType(
                                                          X.typeInfo().type());

                ASSERT(IS_ARRAY == X.isArray());
                ASSERT(ARG_TYPE == X.argType());

                // Negative (and positive) tests
                {
                    const char * const LONG_TAG
                                  = bsl::strchr(X.tagString().c_str(), '|')
                                  ? bsl::strchr(X.tagString().c_str(), '|') + 1
                                  :             X.tagString().c_str();

                    const char SHORT_TAG
                                      = bsl::strchr(X.tagString().c_str(), '|')
                                      ?             X.tagString().c_str()[0]
                                      :             0;

                    bsls::AssertTestHandlerGuard hG;

                    // Redundant parentheses below suppress compilation
                    // warnings on some platforms (Windows/MSVC).

                    if (OptionInfo::e_NON_OPTION == X.argType()) {
                        ASSERT_FAIL(( LONG_TAG == X. longTag()));
                        ASSERT_FAIL((SHORT_TAG == X.shortTag()));
                    } else {
                        ASSERT_PASS(( LONG_TAG == X. longTag()));
                        ASSERT_PASS((SHORT_TAG == X.shortTag()));
                    }
                }

                // Simple validating accessors.
                {
                    bsl::ostringstream oss;

                    if (X.description().empty()) {
                        ASSERT(!X.isDescriptionValid(oss));
                        ASSERT(!oss.str().empty());
                    } else {
                        ASSERT( X.isDescriptionValid(oss));
                        ASSERT( oss.str().empty());
                    }

                    oss.str(""); oss.clear();

                    if (X.name().empty()) {
                        ASSERT(!X.isNameValid(oss));
                        ASSERT(!oss.str().empty());
                    } else {
                        ASSERT( X.isNameValid(oss));
                        ASSERT( oss.str().empty());
                    }
                }

                fa.deleteObject(objPtr);  // Clean up

                ASSERTV(CONFIG,  oam.isInUseSame());
                ASSERTV(CONFIG, noam.isInUseSame());
            }
        }
        }
        }
        }
        }
// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line
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
        //: 1 Ad-hoc testing.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        const OptionInfo OI = { "s|long-tag"
                              , "name"
                              , "description"
                              , TypeInfo(Ot::k_BOOL)
                              , OccurrenceInfo()
                              };

        Obj mX(OI); const Obj& X = mX;

        ASSERT("s|long-tag"               == X.tagString());
        ASSERT("name"                     == X.name());
        ASSERT("description"              == X.description());
        ASSERT(Ot::e_BOOL                 == X.typeInfo().type());
        ASSERT(OccurrenceInfo::e_OPTIONAL == X.occurrenceInfo()
                                              .occurrenceType());
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    ASSERT(gam.isTotalSame());

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
