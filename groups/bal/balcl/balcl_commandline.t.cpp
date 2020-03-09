// balcl_commandline.t.cpp                                            -*-C++-*-
#include <balcl_commandline.h>

#include <balcl_constraint.h>
#include <balcl_option.h>
#include <balcl_optioninfo.h>
#include <balcl_optiontype.h>
#include <balcl_optionvalue.h>
#include <balcl_typeinfo.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_time.h>

#include <bdlb_printmethods.h>  // 'bdlb::HasPrintMethod'

#include <bslim_testutil.h>

#include <bslmf_assert.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bsls_platform.h>
#include <bsls_types.h> // 'bsls::Types::Int64'

#include <bsl_algorithm.h>  // 'bsl::fill'
#include <bsl_functional.h> // 'bsl::function'
#include <bsl_iostream.h>
#include <bsl_ostream.h>    // 'operator<<'
#include <bsl_sstream.h>
#include <bsl_stdexcept.h>
#include <bsl_streambuf.h>
#include <bsl_string.h>     // 'bslstl::StringRef'
#include <bsl_vector.h>

#include <bsl_cstdlib.h>
#include <bsl_cstddef.h> // 'bsl::size_t'
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
// This is a somewhat large component with many classes.  The main class,
// 'balcl::CommandLine', has in-core value semantics and supports the
// 'bslma::Allocator' memory model.  A slight complication is that there is an
// invalid state (null state) and valid states (after 'parse' returns
// successfully).  Nevertheless, we follow the usual value-semantic type test
// driver, simplified from the fact that there are no manipulators (once
// created and parsed, a 'balcl::CommandLine' object does not change value).
//
// Testing 'balcl::commandline' is divided into 2 parts (apart from breathing
// test and usage example).  The first part tests the classes themselves
// (including the supporting classes), and the second tests the various
// sections in the component-level documentation.  Each supporting class has
// in-core value semantics as well, and so we follow the test sequence for such
// types except that we do without the generating functions (but note that we
// have 'create...Info' functions for brevity).  For testing that the various
// examples given in the documentation are valid and work as expected, we have
// a utility that parses a command line and fills up the 'argc' and 'argv' in
// the same way as a Unix shell would.  Then we use this utility to
// systematically test all the examples given in the documentation, in addition
// to examples selected to test the particular concern at hand.  For instance,
// for the section "Flags bundling", we recreate a specification that conforms
// to the documentation usage, and check that parsing the example command lines
// produces valid equivalent results; we also exercise a mix of optional and
// non-optional arguments, etc.
//
// As a note, we made all the usage strings part of this test driver.  This may
// seem like an overkill and an annoyance (if the usage formatting changes, all
// the strings in the test driver will need to be changed too).  In fact, that
// is a *good* thing since it forces the implementer to manually verify that
// all these usage strings are properly aligned, human-readable, etc.:
// something that an automatic test driver cannot do.  Besides finding obvious
// errors in the documentation, of course.
//
// ----------------------------------------------------------------------------
// balcl::CommandLine
// ----------------------------------------------------------------------------
// CREATORS
// [ 3] CommandLine(const Cloi (&table)[LEN], ostream&  stream, *bA = 0);
// [ 3] CommandLine(Cloi (&table)[LEN], ostream& stream, *bA = 0);
// [ 3] CommandLine(const Cloi (&table)[LEN], *bA = 0);
// [ 3] CommandLine(Cloi (&table)[LEN], *bA = 0);
// [ 3] CommandLine(const Cloi *table, int len, *bA = 0);
// [ 3] CommandLine(const Cloi *table, int len, ostream& stream, *bA = 0);
// [ 3] CommandLine(const CommandLine&  original, *bA = );
// [ 3] ~CommandLine();
//
// MANIPULATORS
// [ 3] CommandLine& operator=(const CommandLine& rhs);
// [ 3] int parse(int argc, const char *const argv[]);
// [ 3] int parse(int argc, const char *const argv[], ostream& stream);
//
// ACCESSORS
// [ 3] CommandLineOptionsHandle options() const;
// [ 3] bool isValid() const;
// [ 3] bool isParsed() const;
// [ 3] bool isSpecified(const string& name) const;
// [ 3] bool isSpecified(const string& name, int *count) const;
// [ 3] int numSpecified(const string& name) const;
// [ 3] const vector<int>& positions(const string& name) const;
// [ 3] int position(const bsl::string& name) const;
// [ 3] CommandLineOptionsHandle specifiedOptions() const;
// [ 3] bool theBool(const bsl::string& name) const;
// [ 3] char theChar(const bsl::string& name) const;
// [ 3] int theInt(const bsl::string& name) const;
// [ 3] Int64 theInt64(const bsl::string& name) const;
// [ 3] double theDouble(const bsl::string& name) const;
// [ 3] const string& theString(const string& name) const;
// [ 3] const Datetime& theDatetime(const string& name) const;
// [ 3] const Date& theDate(const string& name) const;
// [ 3] const Time& theTime(const string& name) const;
// [ 3] const vector<char>& theCharArray(const string& name) const;
// [ 3] const vector<int>& theIntArray(const string& name) const;
// [ 3] const vector<Int64>& theInt64Array(const string& name) const;
// [ 3] const vector<double>& theDoubleArray(const string& name) const;
// [ 3] const vector<string>& theStringArray(const string& name) const;
// [ 3] const vector<Datetime>& theDatetimeArray(const string& nom) const;
// [ 3] const vector<Date>& theDateArray(const string& name) const;
// [ 3] const vector<Time>& theTimeArray(const string& name) const;
// [10] void printUsage() const;
// [10] void printUsage(bsl::ostream& stream) const;
//
// [ 3] bslma::Allocator *allocator() const;
// [ 3] ostream& print(ostream& stream, int level = 0, int spl = 4) const;
//
// FREE OPERATORS
// [ 3] bool operator==(const CommandLine& lhs, const CommandLine& rhs);
// [ 3] bool operator!=(const CommandLine& lhs, const CommandLine& rhs);
// [ 3] ostream& operator<<(ostream& stream, const CommandLine& rhs);
// ----------------------------------------------------------------------------
// balcl::CommandLineOptionsHandle
// ----------------------------------------------------------------------------
// ACCESSORS
// [ 3] const char *name(size_t index) const;
// [ 3] size_t numOptions() const;
// [ 3] OptionType::Enum type(size_t index) const;
// [ 3] const OptionValue& value(size_t index) const;
// [ 3] template <class TYPE> const TYPE& the(const char *name) const;
// [ 3] bool theBool(const char *name) const;
// [ 3] char theChar(const char *name) const;
// [ 3] int theInt(const char *name) const;
// [ 3] Int64 theInt64(const char *name) const;
// [ 3] double theDouble(const char *name) const;
// [ 3] const string& theString(const char *name) const;
// [ 3] const Datetime& theDatetime(const char *name) const;
// [ 3] const Date& theDate(const char *name) const;
// [ 3] const Time& theTime(const char *name) const;
// [ 3] const vector<char>& theCharArray(const char *name) const;
// [ 3] const vector<int>& theIntArray(const char *name) const;
// [ 3] const vector<Int64>& theInt64Array(const char *name)
// [ 3] const vector<double>& theDoubleArray(const char *name) const;
// [ 3] const vector<string>& theStringArray(const char *name) const;
// [ 3] const vector<Datetime>& theDatetimeArray(const char *name)
// [ 3] const vector<Date>& theDateArray(const char *name) const;
// [ 3] const vector<Time>& theTimeArray(const char *name) const;
//
// FREE OPERATORS
// [ 3] bool operator==(const CommandLineOptionsHandle& lhs, rhs);
// [ 3] bool operator!=(const CommandLineOptionsHandle& lhs, rhs);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] TESTING 'u::parseCommandLine' TESTING UTILITY
// [ 3] TESTING 'balcl::CommandLine'
// [ 3] TESTING 'balcl::CommandLineOptionsHandle'
// [ 4] TESTING INVALID OPTION SPECS
// [ 5] TESTING FLAGS BUNDLING
// [ 6] TESTING MULTIPLY-SPECIFIED FLAGS
// [ 7] TESTING ORDER OF ARGUMENTS
// [ 8] TESTING PARSING OF STRINGS
// [ 9] TESTING NON-OPTION TOGGLE '--'
// [10] TESTING 'printUsage'
// [11] TESTING ABILITY TO INPUT VALUE FOR FLAG
// [12] USAGE EXAMPLE

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

typedef balcl::CommandLine                Obj;

typedef balcl::Constraint                 Constraint;
typedef balcl::OccurrenceInfo             OccurrenceInfo;
typedef OccurrenceInfo::OccurrenceType    OccurrenceType;
typedef balcl::Option                     Option;
typedef balcl::OptionInfo                 OptionInfo;
typedef balcl::OptionType                 OptionType;
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
 , { L_, Ot::e_BOOL,           0,                    0                      }
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

#undef VP

// ATTRIBUTES FOR 'balcl::OccurrenceInfo'
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

int getArgc(const char * const *argv)
    // Return the number of arguments (non-null pointers) found starting at
    // the specified 'argv'.
{
    int argc = 0;

    while (argv[++argc]) {
        ;
    }

    return argc;
}

                         // =========================
                         // function generateArgument
                         // =========================

int generateArgument(bsl::string       *argString,
                     const OptionInfo&  optionInfo,
                     int                seed = 0)
    // Generate into the specified 'argString' a command-line string suitable
    // to be parsed by a 'balcl::CommandLine' object having the specified
    // 'optionInfo'.  Optionally specified a 'seed' for changing the return
    // value of 'argString' in a pseudo-random fashion.  Return 0 upon success,
    // and non-zero otherwise.
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
            *argString = optionFlag + separator + // "2008/07/22_04:06:08";
                                                     "2008-07-22T04:06:08";
          } break;
          case Ot::e_DATE: {
            *argString = optionFlag + separator + // "2007/08/22";
                                                     "2007-08-22";
          } break;
          case Ot::e_TIME: {
            *argString = optionFlag + separator + "08:06:04";
          } break;
          case Ot::e_CHAR_ARRAY: {
            *argString = optionFlag + separator + "a";
            *argString += ' ';
            *argString += optionFlag + separator + "z";
          } break;
          case Ot::e_INT_ARRAY: {
            *argString = optionFlag + separator + "123654";
            *argString += ' ';
            *argString += optionFlag + separator + "654321";
          } break;
          case Ot::e_INT64_ARRAY: {
            *argString = optionFlag + separator + "987654321";
            *argString += ' ';
            *argString += optionFlag + separator + "192837465";
          } break;
          case Ot::e_DOUBLE_ARRAY: {
            *argString = optionFlag + separator + "0.376739501953125";
            *argString += ' ';
            *argString += optionFlag + separator + "0.623260498046875";
          } break;
          case Ot::e_STRING_ARRAY: {
            *argString = optionFlag + separator + "someString";
            *argString += ' ';
            *argString += optionFlag + separator + "someOtherString";
          } break;
          case Ot::e_DATETIME_ARRAY: {
            *argString = optionFlag + separator + // "2008/07/22_04:06:08";
                                                     "2008-07-22T04:06:08";
            *argString += ' ';
            *argString += optionFlag + separator + // "2006/05/11_02:04:06";
                                                      "2006-05-11T02:04:06";
          } break;
          case Ot::e_DATE_ARRAY: {
            *argString = optionFlag + separator + // "2007/08/22";
                                                     "2007-08-22";
            *argString += ' ';
            *argString += optionFlag + separator + // "2005/06/11";
                                                      "2005-06-11";
          } break;
          case Ot::e_TIME_ARRAY: {
            *argString = optionFlag + separator + "08:06:04";
            *argString += ' ';
            *argString += optionFlag + separator + "02:04:06";
          } break;
          default: ASSERT(0);
        };
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

int parseCommandLine(char       *cmdLine,
                     int&        argc,
                     const char *argv[],
                     int         maxArgs = MAX_ARGS)
    // Parse the specified modifiable 'cmdLine' as would a Unix shell, by
    // replacing every space by a character '\0' and recording the beginning of
    // each field into an array that is loaded into the specified 'argv', and
    // load the number of fields into the specified modifiable 'argc'.
    // Optionally specify 'maxArgs', the maximum allowed number of arguments.
    // Return 0 on success, non-zero if the command line exceeds 'maxArgs'
    // number of arguments, or on failure.  Note that the 'cmdLine' must be
    // null-terminated and may not contain a carriage return or newline.
    // Fields must be separated by either a space or a tab character.
{
    // If passing null string (not even empty), failure.

    if (!cmdLine) {
        return -1;                                                    // RETURN
    }

    // Initialize 'argc'.

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
OccurrenceInfo createOccurrenceInfo(OccurrenceType  occurrenceType,
                                    ElemType        type,
                                    const void     *variable)
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

#ifdef BDE_BUILD_TARGET_EXC

                         // =========================
                         // function throwInvalidSpec
                         // =========================

typedef bsl::logic_error InvalidSpec;

void throwInvalidSpec(const char *text, const char *file, int line)
    // Throw an exception of type 'InvalidSpec' containing the specified 'text'
    // and referring to the specified 'file' at the specified 'line' number.
{
    bsl::string errorMsg = "In file: ";
    errorMsg += file;
    errorMsg += ", at line: ";
    errorMsg += static_cast<char>(line);
    errorMsg += "\nAn invalid 'balcl::OptionInfo' was encountered.";
    errorMsg += "\nThe following assertion failed: ";
    errorMsg += text;
    throw u::InvalidSpec(errorMsg.c_str());
}

#endif // BDE_BUILD_TARGET_EXC

                       // =============================
                       // function isCompatibleOrdering
                       // =============================

bool isCompatibleOrdering(const char *const *argv1,
                          const char *const *argv2,
                          int                argc)
    // Return 'true' if all the arguments specifying non-option argument values
    // and multi-valued options held in the specified 'argv1' array of the
    // specified length 'argc' occur in the same order as in the specified
    // 'argv2' array of the same length, and 'false' otherwise.  The behavior
    // is undefined unless the arguments are either "-S=..." for multi-valued
    // short options (single capitalized letter), "-s=..." for singly-valued
    // short options (non-capitalized), or non-option argument values.
{
    bool nonOptionSeenFlag = false;
    bool multiOptionSeenFlag[26];
    bsl::fill(multiOptionSeenFlag, multiOptionSeenFlag + 26, false);

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
            bsl::vector<bsl::string> filter1;
            bsl::vector<bsl::string> filter2;

            filter1.push_back(argv1[i]);
            for (int j = i + 1; j < argc; ++j) {
                if ((argv1[i][0] == '-' && argv1[j][0] == '-' &&
                                                 argv1[j][1] == shortOptionTag)
                 || (argv1[i][0] != '-' && argv1[j][0] != '-')) {
                    filter1.push_back(argv1[j]);
                }
            }
            for (int j = 1; j < argc; ++j) {
                if ((argv1[i][0] == '-' && argv2[j][0] == '-' &&
                                                 argv2[j][1] == shortOptionTag)
                 || (argv1[i][0] != '-' && argv2[j][0] != '-')) {
                    filter2.push_back(argv2[j]);
                }
            }
            if (filter1 != filter2) {
                return false;                                         // RETURN
            }
        }
    }
    return true;
}

}  // close namespace u
}  // close unnamed namespace

// ============================================================================
//                  USAGE EXAMPLE CLASSES AND FUNCTIONS
// ----------------------------------------------------------------------------

namespace BALCL_COMMANDLINE_USAGE_EXAMPLE {

// BDE_VERIFY pragma: -FD01  // Avoid contract for 'main' below.

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using Command Line Features In Concert
///- - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to design a sorting utility named 'mysort' that has the
// following syntax:
//..
//  usage: mysort  [-r|reverse] [-i|insensitivetocase] [-u|uniq]
//                 [-a|algorithm sortAlgo] -o|outputfile <outputFile>
//                 [<file>]*
//                            // Sort the specified files (in 'fileList'),
//                            // using the specified sorting algorithm and
//                            // write the output to the specified output file.
//
//     option                               note
//  ============  ====================================================
//  -a|algorithm  (1) Value (provided on command line) of this option must
//                    be one among "quickSort", "insertionSort", "shellSort".
//                (2) If not provided, default value will be "quickSort".
//
//  -o|outfile    (1) This option must not be omitted on command line.
//..
// We choose the non-option argument to be an array of 'bsl::string' so as to
// accommodate multiple files.
//
// These options may be used incorrectly, as the following examples show:
//..
//             INCORRECT USE                        REASON FOR INACCURACY
//  ===========================================  ============================
//  $ mysort -riu -o myofile -aDUMBSORT f1 f2    Incorrect because 'DUMBSORT'
//                                               is not among valid values
//                                               for the -a option.
//
//  $ mysort -riu f1 f2                          Incorrect because no value
//                                               is provided for the -o option.
//..
// In order to enforce the constraint on the sorting algorithms that are
// supported, our application provides the following free function:
//..
    bool isValidAlgorithm(const bsl::string *algo, bsl::ostream& stream)
        // Return 'true' if the specified 'algo' is one among "quickSort",
        // "insertionSort", and "shellSort"; otherwise, output to the specified
        // 'stream' an appropriate error message and return 'false'.
    {
        if ("quickSort" == *algo || "insertionSort" == *algo
         || "shellSort" == *algo) {
            return true;                                              // RETURN
        }
        stream << "Error: sorting algorithm must be either "
                  "'quickSort', 'insertionSort', or 'shellSort'.\n";
        return false;
    }
//..
// Using this function, we can now use a 'balcl::CommandLine' object to parse
// command-line options.  The proper usage is shown below.  First we declare
// the variables to be linked to the options.  If they are needed at global
// scope, we could declare them as global variables, but we prefer to declare
// them as local variables inside 'main':
//..
    int main(int argc, const char *argv[]) {
//..
// Note that it is important that variables that will be bound to optional
// command-line arguments be initialized to their default value, otherwise
// their value will unspecified if a value isn't provided on the command line
// (unless a default is specified via 'balcl::OccurrenceInfo'):
//..
        bool isReverse         = false;
        bool isCaseInsensitive = false;
        bool isUniq            = false;

        bsl::string outFile;
        bsl::string sortAlgo;

        bsl::vector<bsl::string> files;
//..
// Next, we build up an option specification table as follows:
//..
        // build constraint for sortAlgo option
        balcl::Constraint::StringConstraint validAlgoConstraint;
        validAlgoConstraint = &isValidAlgorithm;

        // option specification table
        balcl::OptionInfo specTable[] = {
          {
            "r|reverse",                                     // tag
            "isReverse",                                     // name
            "sort in reverse order",                         // description
            balcl::TypeInfo(&isReverse),                     // link
            balcl::OccurrenceInfo::e_OPTIONAL                // occurrence info
          },
          {
            "i|insensitivetocase",                           // tag
            "isCaseInsensitive",                             // name
            "be case insensitive while sorting",             // description
            balcl::TypeInfo(&isCaseInsensitive),             // link
            balcl::OccurrenceInfo::e_OPTIONAL                // occurrence info
          },
          {
            "u|uniq",                                        // tag
            "isUniq",                                        // name
            "discard duplicate lines",                       // description
            balcl::TypeInfo(&isUniq),                        // link
            balcl::OccurrenceInfo::e_OPTIONAL                // occurrence info
          },
          {
            "a|algorithm",                                   // tag
            "sortAlgo",                                      // name
            "sorting algorithm",                             // description
            balcl::TypeInfo(&sortAlgo, validAlgoConstraint),
                                                             // link and
                                                             // constraint
            balcl::OccurrenceInfo(bsl::string("quickSort"))
                                                             // default
                                                             // algorithm
          },
          {
            "o|outputfile",                                  // tag
            "outputFile",                                    // name
            "output file",                                   // description
            balcl::TypeInfo(&outFile),                       // link
            balcl::OccurrenceInfo::e_REQUIRED                // occurrence info
          },
          {
            "",                                              // non-option
            "fileList",                                      // name
            "files to be sorted",                            // description
            balcl::TypeInfo(&files),                         // link
            balcl::OccurrenceInfo::e_OPTIONAL                // occurrence info
          }
        };
//..
// We can now create a command-line specification and parse the command-line
// options:
//..
        // Create command-line specification.
        balcl::CommandLine cmdLine(specTable);

        // Parse command-line options; if failure, print usage.
        if (cmdLine.parse(argc, argv)) {
            cmdLine.printUsage();
            return -1;                                                // RETURN
        }
//..
// Upon successful parsing, the 'cmdLine' object will acquire a value that
// conforms to the specified constraints.  We can examine these values as
// follows:
//..
        // If successful, obtain command-line option values.
        balcl::CommandLineOptionsHandle options = cmdLine.options();

        // Access through linked variable.
        bsl::cout << outFile << bsl::endl;

        // Access through *theType* methods.
        ASSERT(cmdLine.theString("outputFile") == outFile);

        // Access through 'options'.
        ASSERT(options.theString("outputFile") == outFile);

        // Check that required option has been specified once.
        ASSERT(cmdLine.isSpecified("outputFile"));

        int count = -1;
        ASSERT(cmdLine.isSpecified("outputFile", &count));
        ASSERT(1 == count);

        return 0;
    }
//..
// For instance, the following command lines:
//..
//  $ mysort -omyofile f1 f2 f3
//  $ mysort -ainsertionSort f1 f2 f3 -riu -o myofile outputFile
//  $ mysort --algorithm insertionSort --outputfile myofile f1 f2 f3 --uniq
//..
// will all produce the same output on 'stdout'.

// BDE_VERIFY pragma: -FD01  // Avoid contract for 'main' above.
//
}  // close namespace BALCL_COMMANDLINE_USAGE_EXAMPLE

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
      case 12: {
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

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

        using namespace BALCL_COMMANDLINE_USAGE_EXAMPLE;

        static const struct {
            int         d_line;
            int         d_retCode;
            const char *d_cmdLine_p;
        } DATA[] = {
            // LINE RET CMD_LINE
            // ---- --- ---------------
            {  L_, -1, "mysort -riu -o myofile -aDUMBSORT f1 f2"      },
            {  L_, -1, "mysort -riu f1 f2"                            },
            {  L_,  0, "mysort -omyofile f1 f2 f3"                    },
            {  L_,  0, "mysort -ainsertionSort f1 f2 f3"
                                                      " -riu -o myofile"     },
            {  L_,  0, "mysort --algorithm insertionSort"
                                    " --outputfile myofile  f1 f2 f3 --uniq" },
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE     = DATA[i].d_line;
            const int   RET      = DATA[i].d_retCode;
            const char *CMD_LINE = DATA[i].d_cmdLine_p;

            char       *cmdLine  = new char[bsl::strlen(CMD_LINE)+1];
            bsl::strcpy(cmdLine, CMD_LINE);
            if (veryVerbose) { T_ P_(LINE) P(cmdLine) }

            int         argc;
            const char *argv[u::MAX_ARGS];
            u::parseCommandLine(cmdLine, argc, argv);

            const int    ARGC = argc;
            const char **ARGV = argv;
            LOOP_ASSERT(LINE,
                        RET == BALCL_COMMANDLINE_USAGE_EXAMPLE::main(ARGC,
                                                                    ARGV));
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING ABILITY TO INPUT VALUE FOR FLAG
        //
        // Concerns:
        //: 1 'balcl::CommandLine' handles flags without values identically
        //:   regardless of the tag's form.
        //:
        //: 2 'balcl::CommandLine' fails to parse value for the short tag form
        //:   of flag.
        //:
        //: 3 'balcl::CommandLine' successfully parses value for the long tag
        //:   form of flag, inputted using equal sign, but adds warning message
        //:   to the log.
        //
        // Plan:
        //: 1 Set up a variety of flags representations and verify that
        //:   'balcl::CommandLine' parses them in accordance with the
        //:   specification.  Verify, that warning message is added, while
        //:   'balcl::CommandLine' parses values for the long tag form of flag.
        //:   (P-1..3)
        //
        // Testing:
        //   TESTING ABILITY TO INPUT VALUE FOR FLAG
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING ABILITY TO INPUT VALUE FOR FLAG" << endl
                          << "=======================================" << endl;

        const int  ARGC_NUM  = 2;      // number of arguments
        const int  TAGS_NUM  = 2;      // number of available options

        bool       aBool     = false;  // variable for the first flag's value
        bool       bBool     = false;  // variable for the second flag's value

        const bool T         = true;   // 'true'  alias
        const bool F         = false;  // 'false' alias
        const bool NA        = false;  // not applicable

        const int  SUCCESS   =  0;     // successful parsing
        const int  FAILURE   = -1;     // failed parsing

        const OptionInfo SPEC[TAGS_NUM] = {
            {
                "a|aBool",
                "Name_a",
                "Description for a",
                TypeInfo(&aBool),
                OccurrenceInfo::e_OPTIONAL
            },
            {
                "b|bBool",
                "Name_b",
                "Description for b",
                TypeInfo(&bBool),
                OccurrenceInfo::e_OPTIONAL
            },
        };

        static const struct {
            int         d_line;                 // line
            int         d_expRes;               // expected parsing result
            bool        d_isWarned;             // warning presence
            const char *d_argv_p[ARGC_NUM];     // list of parameters
            const bool  d_expValues[TAGS_NUM];  // expected values of flags
        } DATA[] = {
            //LN   expRes   warn   argv                      expValues
            //--   -------  ----   ---------------------     ---------
            { L_,  SUCCESS,   F,   { "", "-a"            },  {  T,  F } },
            { L_,  FAILURE,   F,   { "", "-atrue"        },  { NA, NA } },
            { L_,  FAILURE,   F,   { "", "-a="           },  { NA, NA } },
            { L_,  FAILURE,   F,   { "", "-a=0"          },  { NA, NA } },
            { L_,  FAILURE,   F,   { "", "-a=true"       },  { NA, NA } },
            { L_,  SUCCESS,   F,   { "", "-aa"           },  {  T,  F } },
            { L_,  SUCCESS,   F,   { "", "-ab"           },  {  T,  T } },
            { L_,  SUCCESS,   F,   { "", "-ba"           },  {  T,  T } },
            { L_,  FAILURE,   F,   { "", "-abtrue"       },  { NA, NA } },
            { L_,  FAILURE,   F,   { "", "-ab="          },  { NA, NA } },
            { L_,  FAILURE,   F,   { "", "-ab=0"         },  { NA, NA } },
            { L_,  FAILURE,   F,   { "", "-ab=true"      },  { NA, NA } },
            { L_,  SUCCESS,   F,   { "", "-aab"          },  {  T,  T } },
            { L_,  SUCCESS,   F,   { "", "-aba"          },  {  T,  T } },
            { L_,  FAILURE,   F,   { "", "--a"           },  { NA, NA } },
            { L_,  SUCCESS,   F,   { "", "--aBool"       },  {  T,  F } },
            { L_,  SUCCESS,   F,   { "", "--aBool="      },  {  T,  F } },
            { L_,  SUCCESS,   T,   { "", "--aBool=0"     },  {  T,  F } },
            { L_,  SUCCESS,   T,   { "", "--aBool=true"  },  {  T,  F } },
            { L_,  SUCCESS,   T,   { "", "--aBool=false" },  {  T,  F } },
            { L_,  FAILURE,   F,   { "", "--aBoolbBool"  },  { NA, NA } },
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int i = 0; i < NUM_DATA; ++i) {
            const int          LINE       = DATA[i].d_line;
            const int          EXP_RESULT = DATA[i].d_expRes;
            const bool         WARNED     = DATA[i].d_isWarned;
            const char *const *ARGV       = DATA[i].d_argv_p;
            const bool        *EXP        = DATA[i].d_expValues;

            aBool = false;
            bBool = false;

            if (veryVerbose) {
                P_(LINE); P_(EXP_RESULT);
                T_ T_ P(ARGV[1]);
            }

            bsl::ostringstream oss;
            Obj                mX(SPEC, TAGS_NUM, oss);
            const Obj&         X = mX;

            bsl::ostringstream oss2;
            int                parseRet = mX.parse(ARGC_NUM, ARGV, oss2);
            LOOP3_ASSERT(LINE, parseRet, oss2.str(), EXP_RESULT == parseRet);

            if (veryVerbose) {
                T_ T_ P(X)
            }
            for (int k = 0; k < TAGS_NUM; ++k) {
                if (veryVerbose) { T_ T_ T_ P(k) }
                if (SUCCESS == parseRet) {
                    LOOP3_ASSERT(LINE, k, X.theBool(SPEC[k].d_name),
                                 EXP[k] == X.theBool(SPEC[k].d_name));
                }
            }

            // Next section is valid only till values for flags are allowed.
            // Only the result of parsing will be verified after their
            // deprecation.

            const char *WARNING_MESSAGE =
                             "Warning: A value has been provided for the flag";
            bool        isWarned =
                          bsl::string::npos == oss2.str().find(WARNING_MESSAGE)
                        ? false
                        : true;

            LOOP_ASSERT(LINE, WARNED == isWarned);
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'printUsage'
        //
        // Concerns:
        //: 1 The 'printUsage' properly formats the given variables.
        //:
        //: 2 Names of flags are omitted.
        //:
        //: 3 Empty non-option argument tags are omitted.
        //:
        //: 4 All tags/names/descriptions are indented equally.
        //:
        //: 5 Tags/names/descriptions exceeding 80 characters are handled
        //:   correctly.
        //:
        //: 6 Exceeding the limit of exactly 30 characters triggers shifting
        //:   mechanism.
        //:
        //: 7 All descriptions are shifted to a new line simultaneously (even
        //:   if only one 'tag/name' pair exceed the limit).
        //:
        //: 8 Exceeding the limit by the hidden 'e_FLAG' name does not
        //:   trigger the mechanism.
        //:
        //: 9 Multiple description lines are indented equally.
        //:
        //:10 The no argument overload write to 'bsl::cerr'.
        //
        // Plan:
        //: 1 Set up a variety of variables and options and verify that
        //:   'printUsage' formats them correctly.
        //:
        //: 2 Configure 'bsl::cerr' to write to a 'bsl::ostringstream' and
        //:   confirm the expected results.  (C-10)
        //
        // Testing:
        //   TESTING 'printUsage'
        //   void printUsage() const;
        //   void printUsage(bsl::ostream& stream) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'printUsage'" << endl
                                  << "====================" << endl;

        if (veryVerbose) cout << "Complex case\n";
        {
            balcl::OptionInfo specTable[] = {
              {
                "r|reverse",
                "isReverse",
                "sort in reverse order",
                balcl::TypeInfo(balcl::OptionType::k_BOOL),
                balcl::OccurrenceInfo()
              },
              {
                "i|insensitivetocase",
                "isCaseInsensitive",
                "be case insensitive while sorting",
                balcl::TypeInfo(balcl::OptionType::k_BOOL),
                balcl::OccurrenceInfo()
              },
              {
                "u|uniq",
                "isUniq",
                "discard duplicate lines",
                balcl::TypeInfo(balcl::OptionType::k_BOOL),
                balcl::OccurrenceInfo()
              },
              {
                "a|algorithm",
                "sortAlgo",
                "sorting algorithm",
                balcl::TypeInfo(balcl::OptionType::k_STRING),
                balcl::OccurrenceInfo(bsl::string("quickSort"))
              },
              {
                "o|outputfile",
                "outputFile",
                "output file with a very long option description so we can "
                "see the line wrapping behavior",
                balcl::TypeInfo(balcl::OptionType::k_STRING),
                balcl::OccurrenceInfo::e_REQUIRED
              },
              {
                "",
                "fileList",
                "files to be sorted",
                balcl::TypeInfo(balcl::OptionType::k_STRING_ARRAY),
                balcl::OccurrenceInfo::e_REQUIRED
              }
            };

            balcl::CommandLine cmdLine(specTable);

            bsl::stringstream ss;
            cmdLine.printUsage(ss);

            bsl::streambuf     *errorStreambuf = bsl::cerr.rdbuf();
            bsl::ostringstream  es;
            bsl::cerr.rdbuf(es.rdbuf());
            cmdLine.printUsage();

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

            LOOP_ASSERT(ss.str(), EXPECTED == ss.str());
            LOOP_ASSERT(es.str(), EXPECTED == es.str());

            bsl::cerr.rdbuf(errorStreambuf);
        }

        if (veryVerbose) cout << "Medium case\n";
        {
            balcl::OptionInfo specTable[] = {
              {
                "w|woof",
                "woof",
                "grrowll",
                balcl::TypeInfo(
                                     balcl::OptionType::k_BOOL),
                balcl::OccurrenceInfo()
              },
              {
                "a|arf",
                "arf",
                "arrrrrrrrrrf",
                balcl::TypeInfo(
                                     balcl::OptionType::k_BOOL),
                balcl::OccurrenceInfo()
              },
              {
                "meow",
                "meow",
                "merrrrower",
                balcl::TypeInfo(
                                     balcl::OptionType::k_BOOL),
                balcl::OccurrenceInfo()
              },
              {
                "n|number",
                "number",
                "#",
                balcl::TypeInfo(
                                      balcl::OptionType::k_INT),
                balcl::OccurrenceInfo()
              },
              {
                "s|size",
                "size",
                "size in bytes",
                balcl::TypeInfo(
                                      balcl::OptionType::k_INT),
                balcl::OccurrenceInfo::e_REQUIRED
              },
              {
                "m|meters",
                "meters",
                "distance",
                balcl::TypeInfo(
                                      balcl::OptionType::k_INT),
                balcl::OccurrenceInfo()
              },
              {
                "radius",
                "radius",
                "half diameter",
                balcl::TypeInfo(
                                      balcl::OptionType::k_INT),
                balcl::OccurrenceInfo()
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

            LOOP_ASSERT(ss.str(), EXPECTED == ss.str());
        }

        if (veryVerbose) cout << "Simple pathological case (fixed)\n";
        {
            balcl::OptionInfo specTable[] = {
              {
                "reverse",
                "isReverse",
                "sort in reverse order",
                balcl::TypeInfo(
                                     balcl::OptionType::k_BOOL),
                balcl::OccurrenceInfo()
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

            LOOP_ASSERT(ss.str(), EXPECTED == ss.str());
        }

        if (veryVerbose)
            cout << "Testing description shift to the new line." << endl;
        {
            // Descriptions are shifted to the new line, if total length of the
            // longest tag and longest name exceeds 15 symbols.  Formally, the
            // hardcoded value in the code is 30 ('NEW_LINE_LIMIT' constant),
            // but spaces and service symbols take 15 of them.  So we are
            // testing scenarios, when 'tag + name' are less then 15 symbols,
            // equal to 15 symbols and exceed 15 symbols.

            // Longest tag length + longest name length = 14
            {
                balcl::OptionInfo specTable[] = {
                  {
                    "123456",    // length = 6
                    "12345678",  // length = 8
                    "1234567890 1234567890 1234567890 1234567890"
                                         " 1234567890 1234567890",
                    balcl::TypeInfo(
                                   balcl::OptionType::k_STRING),
                    balcl::OccurrenceInfo()
                  },
                  {
                      "123",
                      "1234",
                      "12",
                      balcl::TypeInfo(
                                     balcl::OptionType::k_BOOL),
                      balcl::OccurrenceInfo()
                  },
                  {
                      "",
                      "12345",
                      "123",
                      balcl::TypeInfo(
                             balcl::OptionType::k_STRING_ARRAY),
                      balcl::OccurrenceInfo()
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

                LOOP_ASSERT(ss.str(), EXPECTED == ss.str());

            }

            {
                balcl::OptionInfo specTable[] = {
                  {
                    "12345678",  // length = 8
                    "123456",    // length = 6
                    "1",
                    balcl::TypeInfo(
                                   balcl::OptionType::k_STRING),
                    balcl::OccurrenceInfo()
                  },
                  {
                      "123",
                      "1234",
                      "12",
                      balcl::TypeInfo(
                                     balcl::OptionType::k_BOOL),
                      balcl::OccurrenceInfo()
                  },
                  {
                      "",
                      "12345",
                      "123",
                      balcl::TypeInfo(
                             balcl::OptionType::k_STRING_ARRAY),
                      balcl::OccurrenceInfo()
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

                LOOP_ASSERT(ss.str(), EXPECTED == ss.str());

            }

            // Longest tag length + longest name length = 15
            {
                balcl::OptionInfo specTable[] = {
                  {
                      "123",
                      "1234",
                      "12",
                      balcl::TypeInfo(
                                     balcl::OptionType::k_BOOL),
                      balcl::OccurrenceInfo()
                  },
                  {
                    "123456",     // length = 6
                    "123456789",  // length = 9
                    "1234567890 1234567890 1234567890 1234567890"
                                         " 1234567890 1234567890",
                    balcl::TypeInfo(
                                   balcl::OptionType::k_STRING),
                    balcl::OccurrenceInfo()
                  },
                  {
                      "",
                      "12345",
                      "123",
                      balcl::TypeInfo(
                             balcl::OptionType::k_STRING_ARRAY),
                      balcl::OccurrenceInfo()
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

                LOOP_ASSERT(ss.str(), EXPECTED == ss.str());

            }

            {
                balcl::OptionInfo specTable[] = {
                  {
                      "123",
                      "1234",
                      "12",
                      balcl::TypeInfo(
                                     balcl::OptionType::k_BOOL),
                      balcl::OccurrenceInfo()
                  },
                  {
                    "123456789",  // length = 9
                    "123456",     // length = 6
                    "1",
                    balcl::TypeInfo(
                                   balcl::OptionType::k_STRING),
                    balcl::OccurrenceInfo()
                  },
                  {
                      "",
                      "12345",
                      "123",
                      balcl::TypeInfo(
                             balcl::OptionType::k_STRING_ARRAY),
                      balcl::OccurrenceInfo()
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

                LOOP_ASSERT(ss.str(), EXPECTED == ss.str());

            }

            // Longest tag length + longest name length = 16
            {
                balcl::OptionInfo specTable[] = {
                  {
                      "123",
                      "1234",
                      "12",
                      balcl::TypeInfo(
                                     balcl::OptionType::k_BOOL),
                      balcl::OccurrenceInfo()
                  },
                  {
                      "",
                      "12345",
                      "123",
                      balcl::TypeInfo(
                             balcl::OptionType::k_STRING_ARRAY),
                      balcl::OccurrenceInfo()
                  },
                  {
                    "123456",      // length = 6
                    "1234567890",  // length = 10
                    "1234567890 1234567890 1234567890 1234567890"
                              " 1234567890 1234567890 1234567890",
                    balcl::TypeInfo(
                                   balcl::OptionType::k_STRING),
                    balcl::OccurrenceInfo()
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

                LOOP_ASSERT(ss.str(), EXPECTED == ss.str());

            }

            {
                balcl::OptionInfo specTable[] = {
                  {
                      "123",
                      "1234",
                      "12",
                      balcl::TypeInfo(
                                     balcl::OptionType::k_BOOL),
                      balcl::OccurrenceInfo()
                  },
                  {
                      "",
                      "12345",
                      "123",
                      balcl::TypeInfo(
                             balcl::OptionType::k_STRING_ARRAY),
                      balcl::OccurrenceInfo()
                  },
                  {
                    "1234567890",  // length = 10
                    "123456",      // length = 6
                    "1",
                    balcl::TypeInfo(
                                   balcl::OptionType::k_STRING),
                    balcl::OccurrenceInfo()
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

                LOOP_ASSERT(ss.str(), EXPECTED == ss.str());

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
                      balcl::TypeInfo(
                                   balcl::OptionType::k_STRING),
                      balcl::OccurrenceInfo()
                  },
                  {
                    "123",                             // length = 3
                    "123456789012345678901234567890",  // length = 30
                    "12",
                    balcl::TypeInfo(
                                     balcl::OptionType::k_BOOL),
                    balcl::OccurrenceInfo()
                  },
                  {
                      "",
                      "12345",
                      "123",
                      balcl::TypeInfo(
                             balcl::OptionType::k_STRING_ARRAY),
                      balcl::OccurrenceInfo()
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

                LOOP_ASSERT(ss.str(), EXPECTED == ss.str());

            }

            // Flag with long tag and short name.
            {
                balcl::OptionInfo specTable[] = {
                  {
                      "12",
                      "123",
                      "1",
                      balcl::TypeInfo(
                                   balcl::OptionType::k_STRING),
                      balcl::OccurrenceInfo()
                  },
                  {
                    "123456789012345678901234567890",  // length = 30
                    "1234",                            // length = 4
                    "12",
                    balcl::TypeInfo(
                                     balcl::OptionType::k_BOOL),
                    balcl::OccurrenceInfo()
                  },
                  {
                      "",
                      "12345",
                      "123",
                      balcl::TypeInfo(
                             balcl::OptionType::k_STRING_ARRAY),
                      balcl::OccurrenceInfo()
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

                LOOP_ASSERT(ss.str(), EXPECTED == ss.str());

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
                      balcl::TypeInfo(
                                   balcl::OptionType::k_STRING),
                      balcl::OccurrenceInfo()
                  },
                  {
                      "123",  // length = 3 (tag)
                      "1234",
                      "12",
                      balcl::TypeInfo(
                                     balcl::OptionType::k_BOOL),
                      balcl::OccurrenceInfo()
                  },
                  {
                      "",              // length = 0
                      "123456789012",  // length = 12
                      "1234567890 1234567890 1234567890 1234567890"
                                           " 1234567890 1234567890",
                      balcl::TypeInfo(
                             balcl::OptionType::k_STRING_ARRAY),
                      balcl::OccurrenceInfo()
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

                LOOP_ASSERT(ss.str(), EXPECTED == ss.str());

            }

            // Non-option argument with long name and long description to
            // verify correct description formatting after the new line shift.
            {
                balcl::OptionInfo specTable[] = {
                  {
                      "12",   // length = 2 (tag)
                      "123",
                      "1",
                      balcl::TypeInfo(
                                   balcl::OptionType::k_STRING),
                      balcl::OccurrenceInfo()
                  },
                  {
                      "123",  // length = 3 (tag)
                      "1234",
                      "12",
                      balcl::TypeInfo(
                                     balcl::OptionType::k_BOOL),
                      balcl::OccurrenceInfo()
                  },
                  {
                      "",
                      "1234567890123",  // length = 13 (tag)
                      "12345678901234567890 12345678901234567890 1234567890 "
                      "12345678901234567890 12345678901234567890 1234567890",
                      balcl::TypeInfo(
                             balcl::OptionType::k_STRING_ARRAY),
                      balcl::OccurrenceInfo()
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

                LOOP_ASSERT(ss.str(), EXPECTED == ss.str());

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
                    balcl::TypeInfo(
                                   balcl::OptionType::k_STRING),
                    balcl::OccurrenceInfo()
                  },
                  {
                      "123",
                      "1234",
                      "12",
                      balcl::TypeInfo(
                                     balcl::OptionType::k_BOOL),
                      balcl::OccurrenceInfo()
                  },
                  {
                      "",
                      "12345",
                      "123",
                      balcl::TypeInfo(
                             balcl::OptionType::k_STRING_ARRAY),
                      balcl::OccurrenceInfo()
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

                LOOP_ASSERT(ss.str(), EXPECTED == ss.str());

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
                    balcl::TypeInfo(
                                   balcl::OptionType::k_STRING),
                    balcl::OccurrenceInfo()
                  },
                  {
                      "123",
                      "1234",
                      "12",
                      balcl::TypeInfo(
                                     balcl::OptionType::k_BOOL),
                      balcl::OccurrenceInfo()
                  },
                  {
                      "",
                      "12345",
                      "123",
                      balcl::TypeInfo(
                             balcl::OptionType::k_STRING_ARRAY),
                      balcl::OccurrenceInfo()
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

                LOOP_ASSERT(ss.str(), EXPECTED == ss.str());

            }

        }

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING NON-OPTION TOGGLE '--'
        //
        // Concerns:
        //: 1 The use of '--' on the command line must stop the parsing of the
        //:   options and everything afterwards, including strings that begin
        //:   with '-...' must be parsed as a non-option argument value.
        //
        // Plan:
        //: 1 Insert '--' at various places into a command line with the last
        //:   non-option argument being multi-valued, measure the number of
        //:   arguments that get assigned to that non-option, and verify that
        //:   it is as expected.
        //
        // Testing:
        //   TESTING NON-OPTION TOGGLE '--'
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING NON-OPTION TOGGLE '--'" << endl
                          << "==============================" << endl;

        if (verbose) cout << "\n\tTesting non-option argument toggle." << endl;

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

            bsl::ostringstream oss;
            Obj                mX(SPEC, NUM_SPEC, oss);  const Obj& X = mX;

            bsl::ostringstream oss2;
            int                parseRet = mX.parse(ARGC, ARGV, oss2);
            LOOP3_ASSERT(LINE, SPEC_IDX, oss2.str(), 0 == parseRet);

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

            LOOP2_ASSERT(LINE, SPEC_IDX, SIZE == linkedStringArray1.size());
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING PARSING OF STRINGS
        //
        // Concerns:
        //: 1 'balcl::CommandLine' must parse a string as raw, and not
        //:   interpret escape sequences such as '.\bass' (on Windows) while
        //:   replacing '\b' with a backspace.
        //
        // Plan:
        //: 1 We employ a Table-driven test.
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

            bsl::ostringstream oss;
            Obj                mX(SPEC, NUM_SPEC, oss);  const Obj& X = mX;

            bsl::ostringstream oss2;
            int                parseRet = mX.parse(ARGC, ARGV, oss2);
            LOOP3_ASSERT(LINE, SPEC_IDX, oss2.str(), 0 == parseRet);

            if (veryVerbose) {
                T_; T_; P(X);
                T_; T_; P_(linkedString); P(EXP);
            }

            LOOP2_ASSERT(EXP, linkedString, EXP == linkedString);
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING ORDER OF ARGUMENTS
        //
        // Concerns:
        //: 1 'balcl::CommandLine' must have the same value no matter what the
        //:   order of the arguments is, except for multi-valued and for the
        //:   '--' non-option argument toggle.
        //
        // Plan:
        //: 1 Using a helper function that checks if two command lines have
        //:   compatible orderings, permute example command lines.
        //
        // Testing:
        //   TESTING ORDER OF ARGUMENTS
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING ORDER OF ARGUMENTS" << endl
                          << "==========================" << endl;

        const int MAX_ARGC = 16;

        if (verbose)
            cout << "\n\tTesting 'u::isCompatibleOrdering' helper." << endl;

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
            const bool          IS_COMPATIBLE = DUMMY_ARGS[i].d_isCompatible;
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

            LOOP_ASSERT(LINE, ARGC_REFERENCE == ARGC);
            LOOP_ASSERT(LINE,
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

            bsl::ostringstream oss;
            Obj                mX(SPEC, NUM_SPEC, oss);  const Obj& X = mX;

            bsl::ostringstream oss2;
            int                parseRet = mX.parse(ARGC, ARGV, oss2);
            LOOP3_ASSERT(LINE, SPEC_IDX, oss2.str(), 0 == parseRet);

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
                    bsl::ostringstream oss;
                    Obj                mY(SPEC, NUM_SPEC, oss);
                    const Obj&         Y = mY;

                    bsl::ostringstream oss2;
                    int                parseRet = mY.parse(ARGC, argv, oss2);
                        // note: argv, not ARGV

                    LOOP2_ASSERT(LINE, SPEC_IDX, 0 == parseRet);
                    LOOP2_ASSERT(LINE, SPEC_IDX, X == Y);  // TEST HERE

                    ++compatibleIterations;
                }
                ++iterations;
            } while (bsl::next_permutation(argv + 1, argv + ARGC));

            if (veryVerbose) {
                T_ T_ P_(iterations) P(compatibleIterations)
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING MULTIPLY-SPECIFIED FLAGS
        //
        // Concerns:
        //: 1 'balcl::CommandLine' must accept multiply-specified flags.
        //
        // Plan:
        //: 1 Define a variety of arguments with multiply-specified flags mixed
        //:   in with another options.  Make sure the 'numSpecified' call
        //:   returns the proper number of occurrences.
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
          // Depth refers to 'numSpecified'.

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
            bsl::ostringstream oss;
            Obj                mX(SPEC, NUM_SPEC, oss);  const Obj& X = mX;

            bsl::ostringstream oss2;
            int                parseRet = mX.parse(ARGC, ARGV, oss2);
            LOOP2_ASSERT(LINE, SPEC_IDX, 0 == parseRet);

            if (veryVerbose) {
                T_ T_ P(X)
            }
            for (int k = 0; k < NUM_SPEC; ++k) {
                if (veryVerbose) { T_ T_ T_ P(k) }
                LOOP3_ASSERT(LINE, SPEC_IDX, k,
                             EXP[k] == X.numSpecified(SPEC[k].d_name));
            }
            LOOP2_ASSERT(LINE, SPEC_IDX, EXP_LINKED_INT == linkedInt);
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING FLAGS BUNDLING
        //
        // Concerns:
        //: 1 'balcl::CommandLine' must be able to parse bundled flags.
        //
        // Plan:
        //: 1 Define a variety of arguments following some option
        //:   specifications, and verify that they can be parsed successfully
        //:   and that any linked variable specified as the last argument of
        //:   the bundle (either in a separate argument or attached with
        //:   "=value" to the bundle) is set properly.
        //
        // Testing:
        //   TESTING FLAGS BUNDLING
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING FLAGS BUNDLING" << endl
                          << "======================" << endl;

        if (verbose) cout << "\tTesting bundled flags." << endl;

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
            bsl::ostringstream oss;
            Obj                mX(SPEC, NUM_SPEC, oss);  const Obj& X = mX;

            bsl::ostringstream oss2;
            int                parseRet = mX.parse(ARGC, ARGV, oss2);
            LOOP2_ASSERT(LINE, SPEC_IDX, 0 == parseRet);

            if (veryVerbose) {
                T_ T_ P(X)
            }
            for (int k = 0; k < NUM_SPEC; ++k) {
                if (veryVerbose) { T_ T_ T_ P(k) }
                LOOP3_ASSERT(LINE, SPEC_IDX, k,
                             EXP[k] == X.numSpecified(SPEC[k].d_name));
            }
            LOOP2_ASSERT(LINE, SPEC_IDX, EXP_LINKED_INT == linkedInt);
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING INVALID OPTION SPECS
        //
        // Concerns:
        //: 1 'balcl::CommandLine' must be able to correctly reject invalid
        //:   option specs.
        //
        // Plan:
        //: 1 Draft a table of invalid specs and verify that the component
        //:   asserts, by redirecting the assert handler to a local helper that
        //:   throws an exception that we can catch in the code below.  Note
        //:   that it is especially interesting to run this test case in very
        //:   verbose mode, and to check the appropriateness of the error
        //:   messages in the 'oss.str()' output.
        //
        // Testing:
        //   TESTING INVALID OPTION SPECS
        // --------------------------------------------------------------------
#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) cout << endl
                          << "TESTING INVALID OPTION SPECS" << endl
                          << "============================" << endl;

        bsls::Assert::setFailureHandler(&u::throwInvalidSpec);

        if (verbose) cout << "\tTesting invalid tags." << endl;

        static const struct {
            int         d_line;
            bool        d_isValid;
            const char *d_tag_p;
        } TAGS[] = {
          // line valid  tag
          // ---- -----  ---
            { L_, false, "a space"                  }
          , { L_, false, "some tag with spaces"     }
          , { L_, false, "|"                        }
          , { L_, false, "|longTag"                 }
          , { L_, false, "-"                        }
          , { L_, false, "-someTag"                 }
          , { L_, false, "-|longTag"                }
          , { L_, false, "s|longTag|"               }
          , { L_, false, "s|longTag|someOtherTag"   }
          , { L_, false, "s||longTag"               }
          , { L_, false, "s"                        }
          , { L_, false, "short|longTag"            }
          , { L_, false, "ss|longTag"               }
          , { L_, false, "s|-long"                  }
          , { L_, false, "s|l"                      }
          // valid tags (for control)
          , { L_, true,  "s|long"                   }
          , { L_, true,  "s|long-with-dashes"       }
          , { L_, true,  "0|1234567890"             }
          , { L_, true,  ".|:\"<>?{};!@#$%^&*()_"   } // yes! valid!
        };
        enum { NUM_TAGS = sizeof TAGS / sizeof *TAGS };

        for (int i = 0; i < NUM_TAGS; ++i) {
            const int   LINE     = TAGS[i].d_line;
            const bool  IS_VALID = TAGS[i].d_isValid;
            const char *TAG      = TAGS[i].d_tag_p;

            bsl::stringstream oss; oss << endl; // for cleaner presentation
            bool              exceptionCaught = false;

            OptionInfo SPEC[] = {
              {
                TAG,                 // non-option
                "SomeValidName",     // name
                "Some description",  // description
                u::createTypeInfo(Ot::e_BOOL),
                OccurrenceInfo::e_OPTIONAL
              }
            };

            if (veryVerbose) { T_ T_ P_(LINE) P_(IS_VALID) P(TAG) }

            bsl::stringstream ossValidate; ossValidate << endl; // match 'oss'

            ASSERT(IS_VALID == Obj::isValidOptionSpecificationTable(
                                                                 SPEC,
                                                                 1,
                                                                 ossValidate));
            ASSERT(IS_VALID == Obj::isValidOptionSpecificationTable(
                                                                 SPEC,
                                                                 1));

            try {
                Obj mX(SPEC, 1, oss);
            }
            catch (const u::InvalidSpec& e) {
                if (veryVerbose) { T_ T_ P(oss.str()) }
                exceptionCaught = true;
            }

            LOOP_ASSERT(LINE, !IS_VALID         == exceptionCaught);
            LOOP_ASSERT(LINE, ossValidate.str() == oss.str());
        }

        if (verbose) cout << "\n\tTesting invalid specs." << endl;

        static const struct {
            int        d_line;
            int        d_numSpecs;
            OptionInfo d_specTable[MAX_SPEC_SIZE];
        } DATA[] = {
            { L_, 1, {
                          {
                              "",                              // non-option
                              "",                              // name
                              "Some description.",             // description
                              u::createTypeInfo(Ot::e_BOOL),
                              OccurrenceInfo::e_OPTIONAL
                          }  // Invalid name
                      }
            },
            { L_, 1, {
                          {
                              "",                              // non-option
                              "SomeName",                      // name
                              "",                              // description
                              u::createTypeInfo(Ot::e_BOOL),
                              OccurrenceInfo::e_OPTIONAL
                          }  // Invalid description
                      }
            },
            { L_, 1, {
                          {
                              "",                              // non-option
                              "SomeName",                      // name
                              "Some description",              // description
                              u::createTypeInfo(Ot::e_BOOL),
                              OccurrenceInfo::e_OPTIONAL
                          }  // Non-options cannot be of type 'bool'.
                      }
            },
            { L_, 1, {
                          {
                              "",                                // non-option
                              "SomeName",                        // name
                              "Some description",                // description
                              u::createTypeInfo(Ot::e_CHAR),
                              u::createOccurrenceInfo(
                                                    OccurrenceInfo::e_OPTIONAL,
                                                    Ot::e_INT,
                                                    &defaultInt)
                          }  // Type of default value does not match type info.
                      }
            },
            { L_, 2, {
                          {
                              "s|long1",                         // non-option
                              "SomeName",                        // name
                              "Some description",                // description
                              u::createTypeInfo(Ot::e_BOOL),
                              OccurrenceInfo::e_OPTIONAL
                          },
                          {
                              "s|long2",                         // non-option
                              "SomeOtherName",                   // name
                              "Some description",                // description
                              u::createTypeInfo(Ot::e_BOOL),
                              OccurrenceInfo::e_OPTIONAL
                          }
                      }  // Short tags must be unique.
            },
            { L_, 2, {
                          {
                              "a|long",                          // non-option
                              "SomeName",                        // name
                              "Some description",                // description
                              u::createTypeInfo(Ot::e_BOOL),
                              OccurrenceInfo::e_OPTIONAL
                          },
                          {
                              "b|long",                          // non-option
                              "SomeOtherName",                   // name
                              "Some description",                // description
                              u::createTypeInfo(Ot::e_BOOL),
                              OccurrenceInfo::e_OPTIONAL
                          }
                      }  // Long tags must be unique.
            },
            { L_, 2, {
                          {
                              "",                                // non-option
                              "SomeCommonName",                  // name
                              "Some description",                // description
                              u::createTypeInfo(Ot::e_BOOL),
                              OccurrenceInfo::e_OPTIONAL
                          },
                          {
                              "",                                // non-option
                              "SomeCommonName",                  // name
                              "Some description",                // description
                              u::createTypeInfo(Ot::e_BOOL),
                              OccurrenceInfo::e_OPTIONAL
                          }
                      }  // Names must be unique.
            },
            { L_, 2, {
                          {
                              "",                                // non-option
                              "SomeCommonName",                  // name
                              "Some description",                // description
                              u::createTypeInfo(Ot::e_INT),
                              u::createOccurrenceInfo(
                                                    OccurrenceInfo::e_OPTIONAL,
                                                    Ot::e_INT,
                                                    &defaultInt)
                          },
                          {
                              "",                                // non-option
                              "SomeCommonName",                  // name
                              "Some description",                // description
                              u::createTypeInfo(Ot::e_INT),
                              OccurrenceInfo::e_REQUIRED
                          }
                      }  // Defaulted non-option argument cannot be followed by
                         // required non-options.
            },
            { L_, 2, {
                          {
                              "",                                // non-option
                              "SomeCommonName",                  // name
                              "Some description",                // description
                              u::createTypeInfo(Ot::e_INT_ARRAY),
                              OccurrenceInfo::e_REQUIRED
                          },
                          {
                              "",                                // non-option
                              "SomeCommonName",                  // name
                              "Some description",                // description
                              u::createTypeInfo(Ot::e_INT),
                              OccurrenceInfo::e_REQUIRED
                          }
                      }  // Array non-options cannot be followed by other
                         // non-options.
            }
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int i = 0; i < NUM_DATA; ++i) {
            const int         LINE      = DATA[i].d_line;
            const int         NUM_SPEC  = DATA[i].d_numSpecs;
            const OptionInfo *SPEC      = DATA[i].d_specTable;

            LOOP_ASSERT(LINE, NUM_SPEC <= MAX_SPEC_SIZE);

            if (veryVerbose) {
                T_ P_(LINE) P_(NUM_SPECS)
                if (0 < NUM_SPEC) { T_ T_ P(SPEC[0]) }
                if (1 < NUM_SPEC) { T_ T_ P(SPEC[1]) }
                if (2 < NUM_SPEC) { T_ T_ P(SPEC[2]) }
                if (3 < NUM_SPEC) { T_ T_ P(SPEC[3]) }
            }

            bsl::stringstream oss; oss << endl; // for cleaner presentation
            bool              exceptionCaught = false;

            bsl::stringstream ossValidate; ossValidate << endl; // match 'oss'

            ASSERT(false == Obj::isValidOptionSpecificationTable(SPEC,
                                                                 NUM_SPEC,
                                                                 ossValidate));
            ASSERT(false == Obj::isValidOptionSpecificationTable(SPEC,
                                                                 NUM_SPEC));

            try {
                Obj mX(SPEC, NUM_SPEC, oss);
            }
            catch (const u::InvalidSpec& e) {
                if (veryVerbose) { T_ T_ P(oss.str()) }
                exceptionCaught = true;
            }

            LOOP_ASSERT(LINE, exceptionCaught);
            LOOP_ASSERT(LINE, ossValidate.str() == oss.str());
        }
#else
        if (verbose) cout << endl
                          << "====================================" << endl
                          << "Skipping Test: invalid option specs." << endl
                          << "====================================" << endl;
#endif // BDE_BUILD_TARGET_EXC
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'balcl::CommandLine'
        //
        // Concerns:
        //: 1 'balcl::CommandLine' is an in-core value-semantic type, version
        //:   of 'balcl::OptionInfo', that can be initialized and converted to
        //:   its simpler 'struct' variant.  We need to make sure it is
        //:   exception safe, uses 'bslma' allocators properly so it can be
        //:   stored in a container, and has a value as expected.  Also, this
        //:   object has an null state until it is "parsed", then has a
        //:   "proper" value.
        //
        // As such, we test each value in valid states only and parse with a
        // matching collection of argument values For the primary accessors, we
        // choose:
        //..
        //  balcl::CommandLine(const balcl::OptionInfo *specTable,
        //                    int                                size,
        //                    bslma::Allocator                  *alloc = 0);
        //  int parse(int argc, const char * const argv[]);
        //..
        // For the basic accessors, we choose:
        //..
        //  balcl::CommandLineOptionsHandle options() const;
        //  bool isParsed() const;
        //  bool isValid() const;
        //..
        //
        // Plan:
        //: 1 Follow the standard value-semantic test driver sequence.  In
        //:   order to generate data, we leverage the 'OptionInfo' values from
        //:   case 7, and append them into an array that we feed to
        //:   'CommandLine'.  Note that all the 'OptionInfo' are valid (i.e.,
        //:   they can be used to initialize a 'CommandLine' object) except for
        //:   the empty name and empty description.  Note however that the data
        //:   gets generated in a certain order, whereas a certain variety is
        //:   desirable (e.g., option + non-option, in either order).  For
        //:   this, we pre-shuffle the array in a random fashion and record the
        //:   patterns to make sure we have all desirable patterns.
        //
        // Testing:
        //   TESTING 'balcl::CommandLine'
        //   CommandLine(const Cloi (&table)[LEN], ostream&  stream, *bA = 0);
        //   CommandLine(Cloi (&table)[LEN], ostream& stream, *bA = 0);
        //   CommandLine(const Cloi (&table)[LEN], *bA = 0);
        //   CommandLine(Cloi (&table)[LEN], *bA = 0);
        //   CommandLine(const Cloi *table, int len, *bA = 0);
        //   CommandLine(const Cloi *table, int len, ostream& stream, *bA = 0);
        //   CommandLine(const CommandLine&  original, *bA = );
        //   ~CommandLine();
        //   CommandLine& operator=(const CommandLine& rhs);
        //   int parse(int argc, const char *const argv[]);
        //   int parse(int argc, const char *const argv[], ostream& stream);
        //   CommandLineOptionsHandle options() const;
        //   bool isValid() const;
        //   bool isParsed() const;
        //   bool isSpecified(const string& name) const;
        //   bool isSpecified(const string& name, int *count) const;
        //   int numSpecified(const string& name) const;
        //   const vector<int>& positions(const string& name) const;
        //   int position(const bsl::string& name) const;
        //   CommandLineOptionsHandle specifiedOptions() const;
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
        //   bslma::Allocator *allocator() const;
        //   ostream& print(ostream& stream, int level = 0, int spl = 4) const;
        //   bool operator==(const CommandLine& lhs, const CommandLine& rhs);
        //   bool operator!=(const CommandLine& lhs, const CommandLine& rhs);
        //   ostream& operator<<(ostream& stream, const CommandLine& rhs);
        //
        //   TESTING 'balcl::CommandLineOptionsHandle'
        //   const char *name(size_t index) const;
        //   size_t numOptions() const;
        //   OptionType::Enum type(size_t index) const;
        //   const OptionValue& value(size_t index) const;
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
        //   bool operator==(const CommandLineOptionsHandle& lhs, rhs);
        //   bool operator!=(const CommandLineOptionsHandle& lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'balcl::CommandLine'" << endl
                          << "===========================" << endl;

        bslma::TestAllocator testAllocator(veryVeryVeryVerbose);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Building test data (once and for all).  Note that we do not use all
        // of the available test data since it was already tested in cases 3,
        // 6, and 8, so we limit ourselves to three distinct tags, a single
        // description, and don't care about hidden options.  Note also that we
        // always choose the longest string, in order to make sure that
        // allocation is triggered (for strings that use small-string
        // optimizations).

        bsl::vector<OptionInfo>        options;
        const bsl::vector<OptionInfo>& OPTIONS = options;

        bsl::vector<bsl::string>        arguments;
        const bsl::vector<bsl::string>& ARGUMENTS = arguments;

        char uniqueIdent[] = "_AAAA"; // 26^4 == 456976, enough for NUM_OPTIONS
        ASSERT(4 <= NUM_OPTION_TAGS);

// BDE_VERIFY pragma: -TP21    // Loops must contain very verbose action
// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
        for (int i = 0; i < 4;                          ++i) {
        for (int t = 0; t < NUM_OPTION_TYPEINFO;        ++t) {
        for (int m = 0; m < NUM_OPTION_OCCURRENCES - 1; ++m) {

            if (veryVerbose) { T_ P_(i) P_(t) P(m) }

            const int n = t % NUM_OPTION_DEFAULT_VALUES;
                // Instead of:
                //..
                //  for (int n = 0; n < NUM_OPTION_DEFAULT_VALUES; ++n)
                //..
                // Valid only because of the way we organized the table data:
                //..
                   ASSERT(OPTION_TYPEINFO[t].d_type ==
                                              OPTION_DEFAULT_VALUES[n].d_type);
                //..
            const char     *TAG        = OPTION_TAGS[i].d_tag_p;
            const char     *NAME       = "SOME UNIQUE NAME";
            const char     *DESC       = "SOME VERY LONG DESCRIPTION...";
            const ElemType  TYPE       = OPTION_TYPEINFO[t].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[t].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[t].d_constraint_p;

            const OccurrenceType OTYPE = OPTION_OCCURRENCES[
                                                    t % NUM_OPTION_OCCURRENCES]
                                                                       .d_type;

            const void  *DEFAULT_VALUE = OPTION_DEFAULT_VALUES[n].d_value_p;

            if (TYPE != OPTION_DEFAULT_VALUES[n].d_type) {
                continue;
            }

            const TypeInfo       TYPE_INFO       = u::createTypeInfo(
                                                                   TYPE,
                                                                   VARIABLE,
                                                                   CONSTRAINT);
            const OccurrenceInfo OCCURRENCE_INFO = u::createOccurrenceInfo(
                                                                OTYPE,
                                                                TYPE,
                                                                DEFAULT_VALUE);

            const balcl::OptionInfo OPTION_INFO = {
                TAG,
                bsl::string(NAME) + uniqueIdent,
                DESC,
                TYPE_INFO,
                OCCURRENCE_INFO
            };

            if (Ot::e_BOOL == TYPE_INFO.type() && OPTION_INFO.d_tag.empty()) {
                // Flags cannot have an empty tag, or in other words
                // non-options cannot be of type 'bool'.  Skip this.

                continue;
            }

            if (OccurrenceInfo::e_HIDDEN == OCCURRENCE_INFO.occurrenceType()
             && OPTION_INFO.d_tag.empty()) {
                // A non-option argument cannot be hidden.  Skip this.

                continue;
            }

            // Generate the (table) option info.

            options.push_back(OPTION_INFO);

            // Generate a matching command line argument for parsing.

            bsl::string arg;  const bsl::string& ARG = arg;

            u::generateArgument(&arg, OPTION_INFO, i);
            arguments.push_back(ARG);

            // Guarantee unique names.

            for (int k = 1; ++(uniqueIdent[k]) > 'Z'; ++k) {
                uniqueIdent[k] = 'A';
            }

        }
        }
        }
// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line
// BDE_VERIFY pragma: +TP21    // Loops must contain very verbose action
        const int NUM_OPTIONS = static_cast<int>(options.size());

        if (verbose) cout << "\n\tBuilt " << NUM_OPTIONS << " options."
                          << endl;

        int  limit            = 0;
        bool arrayNonOption   = false;
        bool defaultNonOption = false;

        // Add second layer (for 2-option specs).

        for (int i = 0; i < NUM_OPTIONS; ++i) {
            if (veryVerbose) { T_ P(i) }
            limit = 0;
              arrayNonOption = options[i].d_tag.empty()
                             && Ot::isArrayType(options[i].d_typeInfo.type());
            defaultNonOption =  options[i].d_tag.empty()
                             && options[i].d_defaultInfo.hasDefaultValue();

            int j = ((i + 37) * (i + 101)) % NUM_OPTIONS;
            while ((options[i].d_tag[0] == options[j].d_tag[0]
                                       && !options[j].d_tag.empty())
                || (  arrayNonOption && options[j].d_tag.empty())
                || (defaultNonOption && options[j].d_tag.empty()
                 && !options[j].d_defaultInfo.hasDefaultValue())) {
                // Tags (long and short, if not empty) must be distinct; if
                // there is a previous non-option argument of array type then
                // this cannot be a non-option; and if a previous non-option
                // argument had a default value, then this non-option argument
                // must have a default value.

                if (veryVerbose) { T_ T_ P(j)  }

                j = (j + 1) % NUM_OPTIONS;
                if (++limit == NUM_OPTIONS) {
                    break;
                }
            }
            if (limit == NUM_OPTIONS) {
                break;
            }
              options.push_back(  options[j]);
            arguments.push_back(arguments[j]);
        }
        if (limit == NUM_OPTIONS) {
            ASSERT(0 && "***CRITICAL ERROR***  Infinite loop in data.");
            break;
        }

        if (verbose) cout << "\tAdded another (shuffled) "
                          << NUM_OPTIONS << " options." << endl;

        ASSERT(static_cast<bsl::size_t>(2 * NUM_OPTIONS) ==   options.size());
        ASSERT(static_cast<bsl::size_t>(2 * NUM_OPTIONS) == arguments.size());

        // Add third layer (for 3-option specs).

        for (int i = 0, j = NUM_OPTIONS; i < NUM_OPTIONS; ++i, ++j) {
            if (veryVerbose) { T_ P(i) }
            limit = 0;
              arrayNonOption = (options[i].d_tag.empty()
                             && Ot::isArrayType(options[i].d_typeInfo.type()))
                            || (options[j].d_tag.empty()
                             && Ot::isArrayType(options[j].d_typeInfo.type()));

            defaultNonOption = (options[i].d_tag.empty()
                             && options[i].d_defaultInfo.hasDefaultValue())
                            || (options[j].d_tag.empty()
                             && options[j].d_defaultInfo.hasDefaultValue());

            int k = ((i + 107) * (i + 293)) % NUM_OPTIONS;

            while (  options[i].d_name == options[k].d_name
                ||   options[j].d_name == options[k].d_name
                || ( options[i].d_tag[0] == options[k].d_tag[0]
                 && !options[k].d_tag.empty())
                || ( options[j].d_tag[0] == options[k].d_tag[0]
                 && !options[k].d_tag.empty())
                || (   arrayNonOption && options[k].d_tag.empty())
                || ( defaultNonOption && options[k].d_tag.empty()
                 && !options[k].d_defaultInfo.hasDefaultValue())) {

                // Names and tags must be distinct, if there is a previous
                // non-option argument of array type then this cannot be a
                // non-option argument, and if a previous non-option argument
                // had a default value, then this non-option argument must have
                // a default value.

                if (veryVerbose) { T_ T_ P(k) }

                k = (k + 1) % NUM_OPTIONS;
                if (++limit == NUM_OPTIONS) {
                    break;
                }
            }
            if (limit == NUM_OPTIONS) {
                break;
            }
              options.push_back(  options[k]);
            arguments.push_back(arguments[k]);
        }
        if (limit == NUM_OPTIONS) {
            ASSERT(0 && "***CRITICAL ERROR***  Infinite loop in data.");
            break;
        }

        if (verbose) cout << "\tAdded another (shuffled) "
                          << NUM_OPTIONS << " options." << endl;

        ASSERT(static_cast<bsl::size_t>(3 * NUM_OPTIONS) ==   options.size());
        ASSERT(static_cast<bsl::size_t>(3 * NUM_OPTIONS) == arguments.size());

        const int MAX_OPTIONS = static_cast<int>(options.size());

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 2 OF VALUE-SEMANTIC TEST DRIVER
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP)

        if (verbose) cout << "\n\tTesting primary manipulators." << endl;

        if (verbose) cout << "\t\tWithout passing in an allocator." << endl;

// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
        for (int n = 0; n < 4;                         ++n) {
        for (int i = 0; i < (n ? NUM_OPTIONS - n : 1); ++i) {

            OptionInfo        specTable[4];
            const OptionInfo *SPEC_TABLE = specTable;

            bsl::string               argString;  // for verbose display
            bsl::vector<bsl::string>  argStrings; // collection of strings
            bsl::vector<const char *> argPtrs;    // ptrs into argStrings
            argStrings.reserve(n + 5);            // argStrings must be stable

            argString = "\"programName";
            argStrings.push_back("programName");
            argPtrs.push_back(&(argStrings.back()[0]));

            for (int j = 0; j < n; ++j) {
                int index = i + j * NUM_OPTIONS;

                ASSERTV(index, MAX_OPTIONS, 0     <= index);
                ASSERTV(index, MAX_OPTIONS, index <  MAX_OPTIONS);

                specTable[j] = OPTIONS[index];

                if (!ARGUMENTS[index].empty()) {
                    const int   MAX_ARGC = 16;
                    const char *argv[MAX_ARGC];

                    int argc = -1;
                    argStrings.push_back(ARGUMENTS[index]);
                    ASSERT(0 == u::parseCommandLine(&(argStrings.back()[0]),
                                                    argc,
                                                    argv,
                                                    MAX_ARGC));

                    for (int k = 0; k < argc; ++k) {
                        argString += " ";
                        argString += argv[k];
                        argPtrs.push_back(argv[k]);
                    }
                }
            }
            argString += "\"";

            if (veryVerbose) {
                T_ T_ P_(i) P(n)
                for (int j = 0; j < n; ++j) {
                    T_ T_ T_ P_(j) P(SPEC_TABLE[j])
                }
                T_ T_ P_(i) P_(n) P(argString)
                T_ T_ P_(i) P_(n) P(argPtrs.size())
                for (int k = 0; k < static_cast<int>(argPtrs.size()); ++k) {
                    T_ T_ T_ P_(k) P(argPtrs[k])
                }
            }

            ASSERT(Obj::isValidOptionSpecificationTable(SPEC_TABLE, n));

            Obj mX(SPEC_TABLE, n);  const Obj& X = mX;  // TEST HERE

            ASSERT(k_DFLT_ALLOC == X.allocator());

            if (veryVerbose) { T_ T_ P_(i) P_(n) P(X) }
            LOOP2_ASSERT(n, i, !X.isParsed());

            bsl::ostringstream oss;
            int                parseRet = mX.parse(
                                              static_cast<int>(argPtrs.size()),
                                              &argPtrs[0],
                                              oss);
            LOOP3_ASSERT(n, i, oss.str(), 0 == parseRet);

            if (veryVerbose) { T_ T_ P_(i) P_(n) P(X) }
            LOOP2_ASSERT(n, i, X.isParsed());
        }
        }
// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line

        bslma::TestAllocator         defaultAllocator(veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&defaultAllocator);

        if (verbose) cout << "\t\tPassing in an allocator." << endl
                           << "\t\t\tWith no exceptions." << endl;

        const bool HAS_BSLMA_ALLOCATOR_TRAIT =
                                         bslma::UsesBslmaAllocator<Obj>::value;


        ASSERT(HAS_BSLMA_ALLOCATOR_TRAIT);

        for (int n = 0; n < 4; ++n) {
            for (int i = 0; i < (n ? NUM_OPTIONS - n : 1); ++i) {
                OptionInfo        specTable[4];
                const OptionInfo *SPEC_TABLE = specTable;

                bsl::string               argString;  // for verbose display
                bsl::vector<bsl::string>  argStrings; // collection of strings
                bsl::vector<const char *> argPtrs;    // ptrs into argStrings
                argStrings.reserve(n + 5);  // argStrings must be stable

                argString = "\"programName";
                argStrings.push_back("programName");
                argPtrs.push_back(&(argStrings.back()[0]));

                for (int j = 0; j < n; ++j) {
                    specTable[j] = OPTIONS[i + j * NUM_OPTIONS];

                    if (!ARGUMENTS[i + j * NUM_OPTIONS].empty()) {
                        const int   MAX_ARGC = 16;
                        const char *argv[MAX_ARGC];

                        int argc = -1;
                        argStrings.push_back(ARGUMENTS[i + j * NUM_OPTIONS]);
                        ASSERT(0 == u::parseCommandLine(
                                                       &(argStrings.back()[0]),
                                                       argc,
                                                       argv,
                                                       MAX_ARGC));

                        for (int k = 0; k < argc; ++k) {
                            argString += " ";
                            argString += argv[k];
                            argPtrs.push_back(argv[k]);
                        }
                    }
                }
                argString += "\"";

                if (veryVerbose) {
                    T_ T_ P_(i) P(n)
                    for (int j = 0; j < n; ++j) {
                        T_ T_ T_ P_(j) P(SPEC_TABLE[j])
                    }
                    T_ T_ P_(i) P_(n) P(argString)
                    T_ T_ P_(i) P_(n) P(argPtrs.size())
                    for (int k = 0; k < static_cast<int>(argPtrs.size()); ++k)
                    {
                        T_ T_ T_ P_(k) P(argPtrs[k])
                    }
                }

                {
                    Obj        mX(SPEC_TABLE, n, &testAllocator);  // TEST HERE
                    const Obj& X = mX;

                    ASSERT(&testAllocator == X.allocator());

                    if (veryVerbose) { T_ T_ P_(i) P_(n) P(X) }
                    LOOP2_ASSERT(n, i, !X.isParsed());

                    bsl::ostringstream oss;
                    int                parseRet = mX.parse(
                                              static_cast<int>(argPtrs.size()),
                                              &argPtrs[0],
                                              oss);
                    LOOP3_ASSERT(n, i, oss.str(), 0 == parseRet);

                    if (veryVerbose) { T_ T_ P_(i) P_(n) P(X) }
                    LOOP2_ASSERT(n, i, X.isParsed());
                }
                ASSERT(0 == testAllocator.numMismatches());
                ASSERT(0 == testAllocator.numBytesInUse());
                ASSERT(0 == defaultAllocator.numMismatches());
            }
        }

        if (verbose) cout << "\t\t\tWith exceptions." << endl;

        for (int i = 0; i < NUM_OPTIONS - 3; i += 21) {
            // For exception testing, take a much smaller portion of the data
            // since what matters is not so much the value itself but the
            // allocation patterns that are not much affected by the data.

            const int n = 3;

            OptionInfo        specTable[4];
            const OptionInfo *SPEC_TABLE = specTable;

            bsl::vector<bsl::string>  argStrings; // collection of strings
            bsl::vector<const char *> argPtrs;    // ptrs into argStrings
            argStrings.reserve(n + 5);  // argStrings must be stable

            argStrings.push_back("programName");
            argPtrs.push_back(&(argStrings.back()[0]));
            for (int j = 0; j < n; ++j) {
                specTable[j] = OPTIONS[i + j * NUM_OPTIONS];
                if (!ARGUMENTS[i + j * NUM_OPTIONS].empty()) {
                    const int   MAX_ARGC = 16;
                    const char *argv[MAX_ARGC];

                    int argc = -1;
                    argStrings.push_back(ARGUMENTS[i + j * NUM_OPTIONS]);
                    ASSERT(0 == u::parseCommandLine(&(argStrings.back()[0]),
                                                    argc,
                                                    argv,
                                                    MAX_ARGC));

                    for (int k = 0; k < argc; ++k) {
                        argPtrs.push_back(argv[k]);
                    }
                }
            }

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {

                Obj mX(SPEC_TABLE, n, &testAllocator); const Obj& X = mX;
                                                                   // TEST HERE

                if (veryVerbose) { T_ T_ P_(i) P_(n) P(X) }
                LOOP2_ASSERT(n, i, !X.isParsed());

                bsl::ostringstream oss;
                int                parseRet = mX.parse(
                                              static_cast<int>(argPtrs.size()),
                                              &argPtrs[0],
                                              oss);
                LOOP3_ASSERT(n, i, oss.str(), 0 == parseRet);

                if (veryVerbose) { T_ T_ P_(i) P_(n) P(X) }
                LOOP2_ASSERT(n, i, X.isParsed());

            }  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(0 ==    testAllocator.numBytesInUse());
            ASSERT(0 ==    testAllocator.numMismatches());
            ASSERT(0 == defaultAllocator.numMismatches());
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 4 OF VALUE-SEMANTIC TEST DRIVER
        // TESTING BASIC ACCESSORS (and more accessors)

        if (verbose) cout << "\n\tTesting basic accessors." << endl;

        for (int n = 0; n < 4; ++n) {
            for (int i = 0; i < (n ? NUM_OPTIONS - n : 1); ++i) {
                OptionInfo        specTable[4];
                const OptionInfo *SPEC_TABLE = specTable;

                bsl::string               argString;  // for verbose display
                bsl::vector<bsl::string>  argStrings; // collection of strings
                bsl::vector<const char *> argPtrs;    // ptrs into argStrings
                argStrings.reserve(n + 5);  // argStrings must be stable

                argString = "\"programName";
                argStrings.push_back("programName");
                argPtrs.push_back(&(argStrings.back()[0]));
                for (int j = 0; j < n; ++j) {
                    specTable[j] = OPTIONS[i + j * NUM_OPTIONS];
                    if (!ARGUMENTS[i + j * NUM_OPTIONS].empty()) {
                        const int   MAX_ARGC = 16;
                        const char *argv[MAX_ARGC];

                        int argc = -1;
                        argStrings.push_back(ARGUMENTS[i + j * NUM_OPTIONS]);
                        ASSERT(0 == u::parseCommandLine(
                                                       &(argStrings.back()[0]),
                                                       argc,
                                                       argv,
                                                       MAX_ARGC));

                        for (int k = 0; k < argc; ++k) {
                            argString += " ";
                            argString += argv[k];
                            argPtrs.push_back(argv[k]);
                        }
                    }
                }
                argString += "\"";

                argStrings.push_back("--some-invalid-option");
                argPtrs.push_back(&(argStrings.back()[0]));

                {
                    Obj mX(SPEC_TABLE, n);  const Obj& X = mX;

                    if (veryVerbose) { T_ T_ P_(i) P_(n) P(X) }

                    ASSERT(!X.isParsed());   // TEST HERE
                    ASSERT(!X.isValid());  // TEST HERE

                    bsl::ostringstream oss;
                    int                parseRet = mX.parse(
                                              static_cast<int>(argPtrs.size()),
                                              &argPtrs[0],
                                              oss);
                    LOOP_ASSERT(oss.str(), 0 != parseRet);

                    if (veryVerbose) { T_ T_ P_(i) P_(n) P(X) }

                    ASSERT(!X.isParsed()); // TEST HERE
                    ASSERT(!X.isValid());  // TEST HERE
                }
            }
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 5 OF VALUE-SEMANTIC TEST DRIVER
        //
        // TESTING OUTPUT (<<) OPERATOR AND PRINT

        if (verbose)
            cout << "\n\tTesting output operator and 'print'." << endl;

        for (int n = 0; n < 4; ++n) {
            for (int i = 0; i < (n ? NUM_OPTIONS - n : 1); ++i) {
                OptionInfo        specTable[4];
                const OptionInfo *SPEC_TABLE = specTable;

                bsl::string               argString;  // for verbose display
                bsl::vector<bsl::string>  argStrings; // collection of strings
                bsl::vector<const char *> argPtrs;    // ptrs into argStrings
                argStrings.reserve(n + 5);  // argStrings must be stable

                argString = "\"programName";
                argStrings.push_back("programName");
                argPtrs.push_back(&(argStrings.back()[0]));
                for (int j = 0; j < n; ++j) {
                    specTable[j] = OPTIONS[i + j * NUM_OPTIONS];
                    if (!ARGUMENTS[i + j * NUM_OPTIONS].empty()) {
                        const int   MAX_ARGC = 16;
                        const char *argv[MAX_ARGC];

                        int argc = -1;
                        argStrings.push_back(ARGUMENTS[i + j * NUM_OPTIONS]);
                        ASSERT(0 == u::parseCommandLine(
                                                       &(argStrings.back()[0]),
                                                       argc,
                                                       argv,
                                                       MAX_ARGC));

                        for (int k = 0; k < argc; ++k) {
                            argString += " ";
                            argString += argv[k];
                            argPtrs.push_back(argv[k]);
                        }
                    }
                }
                argString += "\"";

                Obj mX(SPEC_TABLE, n);  const Obj& X = mX;

                if (veryVerbose) { T_ T_ P_(i) P_(n) P(X) }
                LOOP2_ASSERT(n, i, !X.isParsed());

                bsl::ostringstream oss1, oss2;
                X.print(oss1);
                oss2 << X;
                LOOP2_ASSERT(n, i, oss1.str() == oss2.str());
                LOOP2_ASSERT(n, i, "UNPARSED" == oss2.str());

                bsl::ostringstream oss;
                int                parseRet = mX.parse(
                                              static_cast<int>(argPtrs.size()),
                                              &argPtrs[0],
                                              oss);
                LOOP3_ASSERT(n, i, oss.str(), 0 == parseRet);

                if (veryVerbose) { T_ T_ P_(i) P_(n) P(X) }
                LOOP2_ASSERT(n, i, X.isParsed());

                bsl::ostringstream oss3, oss4;
                X.print(oss3);
                oss4 << X;
                LOOP2_ASSERT(n, i, oss3.str() == oss4.str());
            }
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 6 OF VALUE-SEMANTIC TEST DRIVER
        //
        // TESTING EQUALITY OPERATOR
        //   We do not test the entire cross-product 'W x W', that is too huge.
        //   Choose a reasonable cross section.

        if (verbose) cout << "\n\tTesting equality operator." << endl;

// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
        for (int n = 0; n < 4;                         ++n     ) {
        for (int i = 0; i < (n ? NUM_OPTIONS - n : 1); i += 102) {
            OptionInfo        specTable[4];
            const OptionInfo *SPEC_TABLE = specTable;

            bsl::string               argString;  // for verbose display
            bsl::vector<bsl::string>  argStrings; // collection of strings
            bsl::vector<const char *> argPtrs;    // ptrs into argStrings
            argStrings.reserve(n + 5);  // argStrings must be stable

            argString = "\"programName";
            argStrings.push_back("programName");
            argPtrs.push_back(&(argStrings.back()[0]));
            for (int j = 0; j < n; ++j) {
                specTable[j] = OPTIONS[i + j * NUM_OPTIONS];
                if (!ARGUMENTS[i + j * NUM_OPTIONS].empty()) {
                    const int   MAX_ARGC = 16;
                    const char *argv[MAX_ARGC];

                    int argc = -1;
                    argStrings.push_back(ARGUMENTS[i + j * NUM_OPTIONS]);
                    ASSERT(0 == u::parseCommandLine(&(argStrings.back()[0]),
                                                    argc,
                                                    argv,
                                                    MAX_ARGC));

                    for (int k = 0; k < argc; ++k) {
                        argString += " ";
                        argString += argv[k];
                        argPtrs.push_back(argv[k]);
                    }
                }
            }
            argString += "\"";

            Obj mX(SPEC_TABLE, n);  const Obj& X = mX;

            ASSERT(!X.isParsed());

            int doItOnce = 2;
            do {
                if (veryVerbose) { T_ T_ P_(i) P_(n) P(X) }
                ASSERT((doItOnce < 2) == X.isParsed());

                for (int m = 0; m < 4;                         ++m)     {
                for (int h = 0; h < (m ? NUM_OPTIONS - m : 1); h += 51) {
                    OptionInfo        specTable2[4];
                    const OptionInfo *SPEC_TABLE2 = specTable2;

                    bsl::string               argString2;
                    bsl::vector<bsl::string>  argStrings2;
                    bsl::vector<const char *> argPtrs2;
                    argStrings2.reserve(m + 1);

                    argString2 = "\"programName";
                    argStrings2.push_back("programName");
                    argPtrs2.push_back(&(argStrings2.back()[0]));
                    for (int j = 0; j < m; ++j) {
                        specTable2[j] = OPTIONS[h + j * NUM_OPTIONS];
                        if (!ARGUMENTS[h + j * NUM_OPTIONS].empty()) {
                            const int   MAX_ARGC = 16;
                            const char *argv[MAX_ARGC];

                            int argc = -1;
                            argStrings2.push_back(
                                               ARGUMENTS[h + j * NUM_OPTIONS]);
                            ASSERT(0 == u::parseCommandLine(
                                                      &(argStrings2.back()[0]),
                                                      argc,
                                                      argv,
                                                      MAX_ARGC));

                            for (int k = 0; k < argc; ++k) {
                                argString2 += " ";
                                argString2 += argv[k];
                                argPtrs2.push_back(argv[k]);
                            }
                        }
                    }
                    argString2 += "\"";

                    Obj mY(SPEC_TABLE2, m);  const Obj& Y = mY;

                    if (veryVerbose) { T_ T_ T_ P_(h) P_(m) P(Y) }
                    ASSERT(!Y.isParsed());

                    LOOP4_ASSERT(i, n, h, m, !(X == Y));  // TEST HERE
                    LOOP4_ASSERT(i, n, h, m,   X != Y);   // TEST HERE

                    bsl::ostringstream oss2;
                    int                parseRet2 = mY.parse(
                                             static_cast<int>(argPtrs2.size()),
                                             &argPtrs2[0],
                                             oss2);
                    LOOP_ASSERT(oss2.str(), 0 == parseRet2);

                    if (veryVerbose) { T_ T_ T_ P_(h) P_(m) P(Y) }
                    ASSERT(Y.isParsed());

                    // Test equality.

                    if (!X.isParsed()) {
                        LOOP4_ASSERT(i, n, h, m, !(X == Y));  // TEST HERE
                        LOOP4_ASSERT(i, n, h, m,   X != Y);   // TEST HERE
                    } else {
                        bool isSame = X == Y; // true;

                        LOOP4_ASSERT(i, n, h, m, isSame == (X == Y));  // HERE
                        LOOP4_ASSERT(i, n, h, m, isSame != (X != Y));  // HERE
                    }
                }
                }

                if (--doItOnce) {
                    // First time, --doItOnce == 1 and so we perform the parse.
                    // The second time, --doItOnce == 0 and we do not, and also
                    // we exit the loop below.

                    bsl::ostringstream oss;
                    int                parseRet = mX.parse(
                                              static_cast<int>(argPtrs.size()),
                                              &argPtrs[0],
                                              oss);
                    LOOP_ASSERT(oss.str(), 0 == parseRet);
                    ASSERT(X.isParsed());
                }
            } while (doItOnce);
        }
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 7 OF VALUE-SEMANTIC TEST DRIVER
        // TESTING COPY CONSTRUCTOR

        if (verbose) cout << "\n\tTesting copy constructor." << endl;

        if (verbose) cout << "\t\tWithout passing in an allocator." << endl;

// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
        for (int n = 0; n < 4;                         ++n) {
        for (int i = 0; i < (n ? NUM_OPTIONS - n : 1); ++i) {
            OptionInfo        specTable[4];
            const OptionInfo *SPEC_TABLE = specTable;

            bsl::string               argString;  // for verbose display
            bsl::vector<bsl::string>  argStrings; // collection of strings
            bsl::vector<const char *> argPtrs;    // ptrs into argStrings
            argStrings.reserve(n + 5);            // argStrings must be stable

            argString = "\"programName";
            argStrings.push_back("programName");
            argPtrs.push_back(&(argStrings.back()[0]));

            for (int j = 0; j < n; ++j) {
                specTable[j] = OPTIONS[i + j * NUM_OPTIONS];

                if (!ARGUMENTS[i + j * NUM_OPTIONS].empty()) {
                    const int   MAX_ARGC = 16;
                    const char *argv[MAX_ARGC];

                    int argc = -1;
                    argStrings.push_back(ARGUMENTS[i + j * NUM_OPTIONS]);
                    ASSERT(0 == u::parseCommandLine(&(argStrings.back()[0]),
                                                    argc,
                                                    argv,
                                                    MAX_ARGC));

                    for (int k = 0; k < argc; ++k) {
                        argString += " ";
                        argString += argv[k];
                        argPtrs.push_back(argv[k]);
                    }
                }
            }
            argString += "\"";

            Obj mZ(SPEC_TABLE, n); const Obj& Z = mZ;

            {
                Obj mX(Z); const Obj& X = mX; // TEST HERE

                if (veryVerbose) { T_ T_ P_(i) P_(n) P(X) }
                LOOP2_ASSERT(n, i, !X.isParsed());
                LOOP2_ASSERT(n, i, Z != X);  // weird, but not parsed yet
            }

            bsl::ostringstream oss;
            int                parseRet = mZ.parse(
                                              static_cast<int>(argPtrs.size()),
                                              &argPtrs[0],
                                              oss);
            LOOP3_ASSERT(n, i, oss.str(), 0 == parseRet);

            if (veryVerbose) { T_ T_ P_(i) P_(n) P(Z) }
            LOOP2_ASSERT(n, i, Z.isParsed());

            {
                Obj mX(Z); const Obj& X = mX; // TEST HERE

                if (veryVerbose) { T_ T_ P_(i) P_(n) P(X) }
                LOOP2_ASSERT(n, i, X.isParsed());
                LOOP2_ASSERT(n, i, Z == X);
            }
        }
        }
// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line

        if (verbose) cout << "\t\tPassing in an allocator." << endl
                           << "\t\t\tWith no exceptions." << endl;

// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
        for (int n = 0; n < 4;                         ++n) {
        for (int i = 0; i < (n ? NUM_OPTIONS - n : 1); ++i) {
            OptionInfo        specTable[4];
            const OptionInfo *SPEC_TABLE = specTable;

            bsl::string               argString;  // for verbose display
            bsl::vector<bsl::string>  argStrings; // collection of strings
            bsl::vector<const char *> argPtrs;    // ptrs into argStrings
            argStrings.reserve(n + 5);  // argStrings must be stable

            argString = "\"programName";
            argStrings.push_back("programName");
            argPtrs.push_back(&(argStrings.back()[0]));

            for (int j = 0; j < n; ++j) {
                specTable[j] = OPTIONS[i + j * NUM_OPTIONS];

                if (!ARGUMENTS[i + j * NUM_OPTIONS].empty()) {
                    const int   MAX_ARGC = 16;
                    const char *argv[MAX_ARGC];

                    int argc = -1;
                    argStrings.push_back(ARGUMENTS[i + j * NUM_OPTIONS]);
                    ASSERT(0 == u::parseCommandLine(&(argStrings.back()[0]),
                                                    argc,
                                                    argv,
                                                    MAX_ARGC));

                    for (int k = 0; k < argc; ++k) {
                        argString += " ";
                        argString += argv[k];
                        argPtrs.push_back(argv[k]);
                    }
                }
            }
            argString += "\"";

            Obj mZ(SPEC_TABLE, n); const Obj& Z = mZ;

            {
                Obj mX(Z, &testAllocator); const Obj& X = mX; // TEST HERE

                if (veryVerbose) { T_ T_ P_(i) P_(n) P(X) }
                LOOP2_ASSERT(n, i, !X.isParsed());
                LOOP2_ASSERT(n, i, Z != X);  // weird, but not parsed yet
            }
            ASSERT(0 == testAllocator.numMismatches());
            ASSERT(0 == testAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numMismatches());

            bsl::ostringstream oss;
            int                parseRet = mZ.parse(
                                              static_cast<int>(argPtrs.size()),
                                              &argPtrs[0],
                                              oss);
            LOOP3_ASSERT(n, i, oss.str(), 0 == parseRet);

            if (veryVerbose) { T_ T_ P_(i) P_(n) P(Z) }
            LOOP2_ASSERT(n, i, Z.isParsed());

            {
                Obj mX(Z, &testAllocator); const Obj& X = mX; // TEST HERE

                if (veryVerbose) { T_ T_ P_(i) P_(n) P(X) }
                LOOP2_ASSERT(n, i, X.isParsed());
                LOOP2_ASSERT(n, i, Z == X);
            }
            ASSERT(0 == testAllocator.numMismatches());
            ASSERT(0 == testAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numMismatches());
        }
        }
// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line

        if (verbose) cout << "\t\t\tWith exceptions." << endl;

        for (int i = 0; i < NUM_OPTIONS - 3; i += 21) {
            // For exception testing, take a much smaller portion of the data
            // since what matters is not so much the value itself but the
            // allocation patterns that are not much affected by the data.

            const int n = 3;

            OptionInfo        specTable[4];
            const OptionInfo *SPEC_TABLE = specTable;

            bsl::vector<bsl::string>  argStrings; // collection of strings
            bsl::vector<const char *> argPtrs;    // ptrs into argStrings
            argStrings.reserve(n + 5);  // argStrings must be stable

            argStrings.push_back("programName");
            argPtrs.push_back(&(argStrings.back()[0]));
            for (int j = 0; j < n; ++j) {
                specTable[j] = OPTIONS[i + j * NUM_OPTIONS];
                if (!ARGUMENTS[i + j * NUM_OPTIONS].empty()) {
                    const int   MAX_ARGC = 16;
                    const char *argv[MAX_ARGC];

                    int argc = -1;
                    argStrings.push_back(ARGUMENTS[i + j * NUM_OPTIONS]);
                    ASSERT(0 == u::parseCommandLine(&(argStrings.back()[0]),
                                                    argc,
                                                    argv,
                                                    MAX_ARGC));

                    for (int k = 0; k < argc; ++k) {
                        argPtrs.push_back(argv[k]);
                    }
                }
            }

            Obj mZ(SPEC_TABLE, n); const Obj& Z = mZ; // TEST HERE

            if (veryVerbose) { T_ T_ P_(i) P_(n) P(Z) }
            LOOP2_ASSERT(n, i, !Z.isParsed());

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {

                Obj mX(Z, &testAllocator); const Obj& X = mX; // TEST HERE

                if (veryVerbose) { T_ T_ P_(i) P_(n) P(X) }
                LOOP2_ASSERT(n, i, !X.isParsed());
                LOOP2_ASSERT(n, i, Z != X);  // weird, but not parsed yet

            }  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(0 == testAllocator.numMismatches());
            ASSERT(0 == testAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numMismatches());

            bsl::ostringstream oss;
            int                parseRet = mZ.parse(
                                              static_cast<int>(argPtrs.size()),
                                              &argPtrs[0],
                                              oss);
            LOOP3_ASSERT(n, i, oss.str(), 0 == parseRet);

            if (veryVerbose) { T_ T_ P_(i) P_(n) P(Z) }
            LOOP2_ASSERT(n, i, Z.isParsed());

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {

                Obj mX(Z, &testAllocator); const Obj& X = mX; // TEST HERE

                if (veryVerbose) { T_ T_ P_(i) P_(n) P(X) }
                LOOP2_ASSERT(n, i, X.isParsed());
                LOOP2_ASSERT(n, i, Z == X);

            }  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(0 == testAllocator.numMismatches());
            ASSERT(0 == testAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numMismatches());
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 9 OF VALUE-SEMANTIC TEST DRIVER
        // TESTING ASSIGNMENT OPERATOR

        if (verbose) cout << "\n\tTesting assignment operator." << endl;

        if (verbose) cout << "\t\tTesting assignment u = v." << endl;

// BDE_VERIFY pragma: -TP21    // Loops must contain very verbose action
// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
        for (int n = 0; n < 4;                         ++n   ) {
        for (int i = 0; i < (n ? NUM_OPTIONS - n : 1); i += 7) {

            OptionInfo        specTable[4];
            const OptionInfo *SPEC_TABLE = specTable;

            bsl::string               argString;  // for verbose display
            bsl::vector<bsl::string>  argStrings; // collection of strings
            bsl::vector<const char *> argPtrs;    // ptrs into argStrings
            argStrings.reserve(n + 5);  // argStrings must be stable

            argString = "\"programName";
            argStrings.push_back("programName");
            argPtrs.push_back(&(argStrings.back()[0]));
            for (int j = 0; j < n; ++j) {
                specTable[j] = OPTIONS[i + j * NUM_OPTIONS];
                if (!ARGUMENTS[i + j * NUM_OPTIONS].empty()) {
                    const int   MAX_ARGC = 16;
                    const char *argv[MAX_ARGC];

                    int argc = -1;
                    argStrings.push_back(ARGUMENTS[i + j * NUM_OPTIONS]);
                    ASSERT(0 == u::parseCommandLine(&(argStrings.back()[0]),
                                                    argc,
                                                    argv,
                                                    MAX_ARGC));

                    for (int k = 0; k < argc; ++k) {
                        argString += " ";
                        argString += argv[k];
                        argPtrs.push_back(argv[k]);
                    }
                }
            }
            argString += "\"";

            Obj mX(SPEC_TABLE, n);  const Obj& X = mX;

            int iteration = 4;
            do {
                if (veryVerbose) { T_ T_ P_(i) P_(n) P(X) }
                ASSERT((iteration <= 2) == X.isParsed());

                for (int m = 0; m < 4;                         ++m)      {
                for (int h = 0; h < (m ? NUM_OPTIONS - m : 1); h += 167) {
                    OptionInfo        specTable2[4];
                    const OptionInfo *SPEC_TABLE2 = specTable2;

                    bsl::string               argString2;
                    bsl::vector<bsl::string>  argStrings2;
                    bsl::vector<const char *> argPtrs2;
                    argStrings2.reserve(m + 1);

                    argString2 = "\"programName";
                    argStrings2.push_back("programName");
                    argPtrs2.push_back(&(argStrings2.back()[0]));
                    for (int j = 0; j < m; ++j) {
                        specTable2[j] = OPTIONS[h + j * NUM_OPTIONS];
                        if (!ARGUMENTS[h + j * NUM_OPTIONS].empty()) {
                            const int   MAX_ARGC = 16;
                            const char *argv[MAX_ARGC];

                            int argc = -1;
                            argStrings2.push_back(
                                               ARGUMENTS[h + j * NUM_OPTIONS]);
                            ASSERT(0 == u::parseCommandLine(
                                                      &(argStrings2.back()[0]),
                                                      argc,
                                                      argv,
                                                      MAX_ARGC));

                            for (int k = 0; k < argc; ++k) {
                                argString2 += " ";
                                argString2 += argv[k];
                                argPtrs2.push_back(argv[k]);
                            }
                        }
                    }
                    argString2 += "\"";

                    Obj mY(SPEC_TABLE2, m);  const Obj& Y = mY;

                    if (iteration % 2) {
                        // Every other time, parse Y.

                        bsl::ostringstream oss2;
                        int                parseRet2 = mY.parse(
                                             static_cast<int>(argPtrs2.size()),
                                             &argPtrs2[0],
                                             oss2);
                        LOOP_ASSERT(oss2.str(), 0 == parseRet2);

                        ASSERT(Y.isParsed());
                    }

                    if (veryVerbose) { T_ T_ T_ P_(h) P_(m) P(Y) }

                    mY = X;   // TEST HERE

                    if (!X.isParsed()) {
                        LOOP4_ASSERT(i, n, h, m, !(X == Y));
                        LOOP4_ASSERT(i, n, h, m,   X != Y);

                        // Assert that both can be parsed and yield identical
                        // result, but do it on a copy of X.

                        Obj mZ(X);  const Obj& Z = mZ;
                        ASSERT(0 == mY.parse(static_cast<int>(argPtrs.size()),
                                             &argPtrs[0]));
                        ASSERT(0 == mZ.parse(static_cast<int>(argPtrs.size()),
                                             &argPtrs[0]));

                        LOOP4_ASSERT(i, n, h, m,  (Z == Y));
                    } else {
                        LOOP4_ASSERT(i, n, h, m,  (X == Y));
                    }
                }
                }

                if (2 == --iteration) {
                    // First two times, --iteration > 2 and so we perform the
                    // parse after the second time.  When --iteration == 0, we
                    // exit the loop below.

                    bsl::ostringstream oss;
                    int                parseRet = mX.parse(
                                              static_cast<int>(argPtrs.size()),
                                              &argPtrs[0],
                                              oss);
                    LOOP_ASSERT(oss.str(), 0 == parseRet);
                    ASSERT(X.isParsed());
                }

            } while (iteration);
        }
        }
// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line
// BDE_VERIFY pragma: +TP21    // Loops must contain very verbose action

        if (verbose) cout << "\t\tTesting self-assignment (Aliasing)." << endl;

// BDE_VERIFY pragma: -TP21    // Loops must contain very verbose action
// BDE_VERIFY pragma: -IND01   // Possibly mis-indented line
        for (int n = 0; n < 4;               ++n) {
        for (int i = 0; i < NUM_OPTIONS - n; ++i) {

            OptionInfo        specTable[4];
            const OptionInfo *SPEC_TABLE = specTable;

            bsl::string               argString;  // for verbose display
            bsl::vector<bsl::string>  argStrings; // collection of strings
            bsl::vector<const char *> argPtrs;    // ptrs into argStrings
            argStrings.reserve(n + 5);  // argStrings must be stable

            argString = "\"programName";
            argStrings.push_back("programName");
            argPtrs.push_back(&(argStrings.back()[0]));
            for (int j = 0; j < n; ++j) {
                specTable[j] = OPTIONS[i + j * NUM_OPTIONS];
                if (!ARGUMENTS[i + j * NUM_OPTIONS].empty()) {
                    const int   MAX_ARGC = 16;
                    const char *argv[MAX_ARGC];

                    int argc = -1;
                    argStrings.push_back(ARGUMENTS[i + j * NUM_OPTIONS]);
                    ASSERT(0 == u::parseCommandLine(&(argStrings.back()[0]),
                                                    argc,
                                                    argv,
                                                    MAX_ARGC));

                    for (int k = 0; k < argc; ++k) {
                        argString += " ";
                        argString += argv[k];
                        argPtrs.push_back(argv[k]);
                    }
                }
            }
            argString += "\"";

            Obj mX(SPEC_TABLE, n);  const Obj& X = mX;
            ASSERT(!X.isParsed());

            {
                Obj mY(X);  const Obj& Y = mY;
                mY = Y;  // TEST HERE

                LOOP2_ASSERT(i, n, X != Y);  // weird, but not parsed yet

                // Assert that both can be parsed and yield identical result.

                bsl::ostringstream oss;
                int                parseRet = mX.parse(
                                              static_cast<int>(argPtrs.size()),
                                              &argPtrs[0],
                                              oss);
                LOOP_ASSERT(oss.str(), 0 == parseRet);
                ASSERT(X.isParsed());

                bsl::ostringstream oss2;
                int                parseRet2 = mY.parse(
                                              static_cast<int>(argPtrs.size()),
                                              &argPtrs[0],
                                              oss2);
                LOOP_ASSERT(oss2.str(), 0 == parseRet2);
                ASSERT(Y.isParsed());

                LOOP2_ASSERT(i, n, X == Y);
            }

            {
                Obj mY(X);  const Obj& Y = mY;
                mY = Y;  // TEST HERE

                LOOP2_ASSERT(i, n, X == Y);
            }
        }
        }
// BDE_VERIFY pragma: +IND01   // Possibly mis-indented line
// BDE_VERIFY pragma: +TP21    // Loops must contain very verbose action

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // TESTING ADDITIONAL CONSTRUCTORS

        if (verbose) cout << "\nTesting additional constructors." << endl;
        {
            bsl::vector<bsl::string>  files;
            OptionInfo                specTable[1];
#ifndef BSLS_PLATFORM_CMP_SUN
            const OptionInfo          SPEC_TABLE[1] =
#else
            // For some reason, SunPro doesn't like const OptionInfo[]...
            // We get  >> Assertion:  unexpected array kind in
            // type_builder::visit_array (../lnk/v2mangler.cc, line 1287)

            OptionInfo                SPEC_TABLE[1] =
#endif
            {
              {
                "",                                          // non-option
                "fileList",                                  // name
                "files to be sorted",                        // description
                balcl::TypeInfo(&files),            // link
                balcl::OccurrenceInfo()             // occurrence
              }
            };
            const int                 argc = 2;
            const char               *argv[argc] = {
                "progname",
                "filename"
            };

            specTable[0] = SPEC_TABLE[0];

            bsl::ostringstream oss2;
            Obj                mZ(static_cast<const OptionInfo *>(specTable),
                                  1);
            const Obj&         Z = mZ;

            ASSERT(0 == mZ.parse(argc, argv, oss2));

            {
                Obj mX(specTable);  const Obj& X = mX;  // TEST HERE
                ASSERT(!X.isParsed());

                bsl::ostringstream oss;
                int                parseRet = mX.parse(argc, argv, oss);
                LOOP_ASSERT(oss.str(), 0 == parseRet);
                ASSERT(X.isParsed());
                ASSERT(Z == X);
            }
            {
                Obj mX(SPEC_TABLE);  const Obj& X = mX;  // TEST HERE
                ASSERT(!X.isParsed());

                bsl::ostringstream oss;
                int                parseRet = mX.parse(argc, argv, oss);
                LOOP_ASSERT(oss.str(), 0 == parseRet);
                ASSERT(X.isParsed());
            }
            {
                Obj mX(specTable, &testAllocator); const Obj& X = mX;
                                                                   // TEST HERE
                ASSERT(!X.isParsed());

                bsl::ostringstream oss;
                int                parseRet = mX.parse(argc, argv, oss);
                LOOP_ASSERT(oss.str(), 0 == parseRet);
                ASSERT(X.isParsed());
                ASSERT(Z == X);
            }
            ASSERT(0 == testAllocator.numMismatches());
            ASSERT(0 == testAllocator.numBytesInUse());
            {
                Obj mX(SPEC_TABLE, &testAllocator); const Obj& X = mX;
                                                                   // TEST HERE
                ASSERT(!X.isParsed());

                bsl::ostringstream oss;
                int                parseRet = mX.parse(argc, argv, oss);
                LOOP_ASSERT(oss.str(), 0 == parseRet);
                ASSERT(X.isParsed());
            }
            ASSERT(0 == testAllocator.numMismatches());
            ASSERT(0 == testAllocator.numBytesInUse());
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // TESTING ADDITIONAL ACCESSORS

        if (verbose) cout << "\n\tTesting additional accessors." << endl;

        int countUnspecified = 0;

        for (int n = 0; n < 4; ++n) {
            for (int i = 0; i < (n ? NUM_OPTIONS - n : 1); ++i) {
                OptionInfo        specTable[4];
                const OptionInfo *SPEC_TABLE = specTable;

                bsl::string               argString;  // for verbose display
                bsl::vector<bsl::string>  argStrings; // collection of strings
                bsl::vector<const char *> argPtrs;    // ptrs into argStrings
                argStrings.reserve(n + 5);  // argStrings must be stable

                argString = "\"programName";
                argStrings.push_back("programName");
                argPtrs.push_back(&(argStrings.back()[0]));
                for (int j = 0; j < n; ++j) {
                    specTable[j] = OPTIONS[i + j * NUM_OPTIONS];
                    argStrings.push_back(ARGUMENTS[i + j * NUM_OPTIONS]);
                    if (!argStrings.back().empty()) {
                        const int   MAX_ARGC = 16;
                        const char *argv[MAX_ARGC];

                        int argc = -1;
                        ASSERT(0 == u::parseCommandLine(
                                                       &(argStrings.back()[0]),
                                                       argc,
                                                       argv,
                                                       MAX_ARGC));

                        for (int k = 0; k < argc; ++k) {
                            argString += " ";
                            argString += argv[k];
                            argPtrs.push_back(argv[k]);
                        }
                    }
                }
                argString += "\"";

                Obj mX(SPEC_TABLE, n);  const Obj& X = mX;

                bsl::ostringstream oss;
                int                parseRet = mX.parse(
                                              static_cast<int>(argPtrs.size()),
                                              &argPtrs[0],
                                              oss);
                LOOP_ASSERT(oss.str(), 0 == parseRet);

                LOOP_ASSERT(n, static_cast<bsl::size_t>(n) ==
                                                     X.options().numOptions());

                if (veryVerbose) { T_ T_ P_(i) P_(n) P(X) }
                if (veryVerbose) { T_ T_ T_ P(argString) }

                // Test name-based accessors.

                int pos = 1;
                for (int j = 0; j < n; ++j) {
                    const bsl::string& name = specTable[j].d_name;

                    LOOP_ASSERT(name,  0 == bsl::strcmp(name.c_str(),
                                                        X.options().name(j)));

                    if (veryVerbose) {
                        T_ T_ T_ P(name)
                        T_ T_ T_ P(X.numSpecified(name))
                        if (Ot::isArrayType(specTable[j].d_typeInfo.type())) {
                            T_ T_ T_ P(X.positions(name).size())
                            if (0 < X.positions(name).size()) {
                                T_ T_ T_ P(X.positions(name)[0])
                            }
                            if (1 < X.positions(name).size()) {
                                T_ T_ T_ P(X.positions(name)[1])
                            }
                        } else {
                            T_ T_ T_ P(X.position(name))
                        }
                    }

                    if (argStrings[j + 1].empty()) {
                        LOOP2_ASSERT(n, i, 0 == X.numSpecified(name));
                        if (Ot::isArrayType(specTable[j].d_typeInfo.type())) {
                            LOOP2_ASSERT(n, i, X.positions(name).empty());
                        } else {
                            LOOP2_ASSERT(n, i, -1 == X.position(name));
                        }
                        if (specTable[j].d_defaultInfo.hasDefaultValue()) {
    //v---------------------^
      const char *namS = name.c_str();
      Ot::Enum    type = specTable[j].d_typeInfo.type();

      LOOP2_ASSERT(n, i, type == X.options().type(j));

      if (!X.isSpecified(name)) {
          ASSERT(specTable[j].d_defaultInfo.defaultValue() ==
                                                         X.options().value(j));
                                                          // has default value
          ASSERT(false ==          X.options().value(j).isNull());
          ASSERT(true  == X.specifiedOptions().value(j).isNull());
          ++countUnspecified;
      }

      switch (type) {
        case Ot::e_BOOL: {
          LOOP2_ASSERT(n, i,                         X.theBool(name));
          LOOP2_ASSERT(n, i,                         X.options()
                                                      .theBool(namS));

          typedef Ot::EnumToType<Ot::e_BOOL>::type VT;
          LOOP2_ASSERT(n, i,                         X.options()
                                                      .the<VT>(namS));
          LOOP2_ASSERT(n, i,                         X.options()
                                                      .value(j)
                                                      .the<VT>());
        } break;
        case Ot::e_CHAR: {
          LOOP2_ASSERT(n, i, defaultChar          == X.theChar(name));
          LOOP2_ASSERT(n, i, defaultChar          == X.options()
                                                      .theChar(namS));

          typedef Ot::EnumToType<Ot::e_CHAR>::type VT;
          LOOP2_ASSERT(n, i, defaultChar          == X.options()
                                                      .the<VT>(namS));
          LOOP2_ASSERT(n, i, defaultChar          == X.options()
                                                      .value(j)
                                                      .the<VT>());
        } break;
        case Ot::e_INT: {
          LOOP2_ASSERT(n, i, defaultInt           == X.theInt(name));
          LOOP2_ASSERT(n, i, defaultInt           == X.options()
                                                      .theInt(namS));

          typedef Ot::EnumToType<Ot::e_INT>::type VT;
          LOOP2_ASSERT(n, i, defaultInt           == X.options()
                                                      .the<VT>(namS));
          LOOP2_ASSERT(n, i, defaultInt           == X.options()
                                                      .value(j)
                                                      .the<VT>());
        } break;
        case Ot::e_INT64: {
          LOOP2_ASSERT(n, i, defaultInt64         == X.theInt64(name));
          LOOP2_ASSERT(n, i, defaultInt64         == X.options()
                                                      .theInt64(namS));

          typedef Ot::EnumToType<Ot::e_INT64>::type VT;
          LOOP2_ASSERT(n, i, defaultInt64         == X.options()
                                                      .the<VT>(namS));
          LOOP2_ASSERT(n, i, defaultInt64         == X.options()
                                                      .value(j)
                                                      .the<VT>());
        } break;
        case Ot::e_DOUBLE: {
          LOOP2_ASSERT(n, i, defaultDouble        == X.theDouble(name));
          LOOP2_ASSERT(n, i, defaultDouble        == X.options()
                                                      .theDouble(namS));

          typedef Ot::EnumToType<Ot::e_DOUBLE>::type VT;
          LOOP2_ASSERT(n, i, defaultDouble        == X.options()
                                                      .the<VT>(namS));
          LOOP2_ASSERT(n, i, defaultDouble        == X.options()
                                                      .value(j)
                                                      .the<VT>());
        } break;
        case Ot::e_STRING: {
          LOOP2_ASSERT(n, i, defaultString        == X.theString(name));
          LOOP2_ASSERT(n, i, defaultString        == X.options()
                                                      .theString(namS));

          typedef Ot::EnumToType<Ot::e_STRING>::type VT;
          LOOP2_ASSERT(n, i, defaultString        == X.options()
                                                      .the<VT>(namS));
          LOOP2_ASSERT(n, i, defaultString        == X.options()
                                                      .value(j)
                                                      .the<VT>());
        } break;
        case Ot::e_DATETIME: {
          LOOP2_ASSERT(n, i, defaultDatetime      == X.theDatetime(name));
          LOOP2_ASSERT(n, i, defaultDatetime      == X.options()
                                                      .theDatetime(namS));

          typedef Ot::EnumToType<Ot::e_DATETIME>::type VT;
          LOOP2_ASSERT(n, i, defaultDatetime      == X.options()
                                                      .the<VT>(namS));
          LOOP2_ASSERT(n, i, defaultDatetime      == X.options()
                                                      .value(j)
                                                      .the<VT>());
        } break;
        case Ot::e_DATE: {
          LOOP2_ASSERT(n, i, defaultDate          == X.theDate(name));
          LOOP2_ASSERT(n, i, defaultDate          == X.options()
                                                      .theDate(namS));

          typedef Ot::EnumToType<Ot::e_DATE>::type VT;
          LOOP2_ASSERT(n, i, defaultDate          == X.options()
                                                      .the<VT>(namS));
          LOOP2_ASSERT(n, i, defaultDate          == X.options()
                                                      .value(j)
                                                      .the<VT>());
        } break;
        case Ot::e_TIME: {
          LOOP2_ASSERT(n, i, defaultTime          == X.theTime(name));
          LOOP2_ASSERT(n, i, defaultTime          == X.options()
                                                      .theTime(namS));

          typedef Ot::EnumToType<Ot::e_TIME>::type VT;
          LOOP2_ASSERT(n, i, defaultTime          == X.options()
                                                      .the<VT>(namS));
          LOOP2_ASSERT(n, i, defaultTime          == X.options()
                                                      .value(j)
                                                      .the<VT>());
        } break;
        case Ot::e_CHAR_ARRAY: {
          LOOP2_ASSERT(n, i, defaultCharArray     == X.theCharArray(name));
          LOOP2_ASSERT(n, i, defaultCharArray     == X.options()
                                                      .theCharArray(namS));

          typedef Ot::EnumToType<Ot::e_CHAR_ARRAY>::type VT;
          LOOP2_ASSERT(n, i, defaultCharArray     == X.options()
                                                      .the<VT>(namS));
          LOOP2_ASSERT(n, i, defaultCharArray     == X.options()
                                                      .value(j)
                                                      .the<VT>());
        } break;
        case Ot::e_INT_ARRAY: {
          LOOP2_ASSERT(n, i, defaultIntArray      == X.theIntArray(name));
          LOOP2_ASSERT(n, i, defaultIntArray      == X.options()
                                                      .theIntArray(namS));

          typedef Ot::EnumToType<Ot::e_INT_ARRAY>::type VT;
          LOOP2_ASSERT(n, i, defaultIntArray      == X.options()
                                                      .the<VT>(namS));
          LOOP2_ASSERT(n, i, defaultIntArray      == X.options()
                                                      .value(j)
                                                      .the<VT>());
        } break;
        case Ot::e_INT64_ARRAY: {
          LOOP2_ASSERT(n, i, defaultInt64Array    == X.theInt64Array(name));
          LOOP2_ASSERT(n, i, defaultInt64Array    == X.options()
                                                      .theInt64Array(namS));

          typedef Ot::EnumToType<Ot::e_INT64_ARRAY>::type VT;
          LOOP2_ASSERT(n, i, defaultInt64Array    == X.options()
                                                      .the<VT>(namS));
          LOOP2_ASSERT(n, i, defaultInt64Array    == X.options()
                                                      .value(j)
                                                      .the<VT>());
        } break;
        case Ot::e_DOUBLE_ARRAY: {
          LOOP2_ASSERT(n, i, defaultDoubleArray   == X.theDoubleArray(name));
          LOOP2_ASSERT(n, i, defaultDoubleArray   == X.options()
                                                      .theDoubleArray(namS));

          typedef Ot::EnumToType<Ot::e_DOUBLE_ARRAY>::type VT;
          LOOP2_ASSERT(n, i, defaultDoubleArray   == X.options()
                                                      .the<VT>(namS));
          LOOP2_ASSERT(n, i, defaultDoubleArray   == X.options()
                                                      .value(j)
                                                      .the<VT>());
        } break;
        case Ot::e_STRING_ARRAY: {
          LOOP2_ASSERT(n, i, defaultStringArray   == X.theStringArray(name));
          LOOP2_ASSERT(n, i, defaultStringArray   == X.options()
                                                      .theStringArray(namS));

          typedef Ot::EnumToType<Ot::e_STRING_ARRAY>::type VT;
          LOOP2_ASSERT(n, i, defaultStringArray   == X.options()
                                                      .the<VT>(namS));
          LOOP2_ASSERT(n, i, defaultStringArray   == X.options()
                                                      .value(j)
                                                      .the<VT>());
        } break;
        case Ot::e_DATETIME_ARRAY: {
          LOOP2_ASSERT(n, i, defaultDatetimeArray == X.theDatetimeArray(name));
          LOOP2_ASSERT(n, i, defaultDatetimeArray == X.options()
                                                      .theDatetimeArray(namS));

          typedef Ot::EnumToType<Ot::e_DATETIME_ARRAY>::type VT;
          LOOP2_ASSERT(n, i, defaultDatetimeArray == X.options()
                                                      .the<VT>(namS));
          LOOP2_ASSERT(n, i, defaultDatetimeArray == X.options()
                                                      .value(j)
                                                      .the<VT>());
        } break;
        case Ot::e_DATE_ARRAY: {
          LOOP2_ASSERT(n, i, defaultDateArray     == X.theDateArray(name));
          LOOP2_ASSERT(n, i, defaultDateArray     == X.options()
                                                      .theDateArray(namS));

          typedef Ot::EnumToType<Ot::e_DATE_ARRAY>::type VT;
          LOOP2_ASSERT(n, i, defaultDateArray     == X.options()
                                                      .the<VT>(namS));
          LOOP2_ASSERT(n, i, defaultDateArray     == X.options()
                                                      .value(j)
                                                      .the<VT>());
        } break;
        case Ot::e_TIME_ARRAY: {
          LOOP2_ASSERT(n, i, defaultTimeArray     == X.theTimeArray(name));
          LOOP2_ASSERT(n, i, defaultTimeArray     == X.options()
                                                      .theTimeArray(namS));

          typedef Ot::EnumToType<Ot::e_TIME_ARRAY>::type VT;
          LOOP2_ASSERT(n, i, defaultTimeArray     == X.options()
                                                      .the<VT>(namS));
          LOOP2_ASSERT(n, i, defaultTimeArray     == X.options()
                                                      .value(j)
                                                      .the<VT>());
        } break;
        default: ASSERT(0);
      };
    //^---------------------v
                        }
                    } else {
                        if (Ot::isArrayType(specTable[j].d_typeInfo.type())) {
                            LOOP2_ASSERT(n, i, 2 == X.numSpecified(name));
                            LOOP2_ASSERT(n, i, 2 == X.positions(name).size());
                            LOOP2_ASSERT(n, i, pos   <= X.positions(name)[0]);
                            LOOP2_ASSERT(n, i, ++pos <= X.positions(name)[1]);
                        } else {
                            LOOP2_ASSERT(n, i, 1 == X.numSpecified(name));
                            LOOP2_ASSERT(n, i, pos <= X.position(name));
                        }
                        ++pos;
                    }
                }

                // Test 'printUsage'.

                bsl::ostringstream oss2;
                X.printUsage(oss2);
                if (veryVerbose) {
                    bsl::string usage = oss2.str().substr(1, 34) + "...";
                    T_ T_ T_ P(usage)
                }
                ASSERT(0 < oss2.str().length());
            }
        }

        ASSERT(0 < countUnspecified);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // TESTING ADDITIONAL CONSTRUCTORS

        if (verbose) cout << "\n\tTesting additional constructors." << endl;

        const int INDEX0 = NUM_OPTIONS * 0;
        const int INDEX1 = NUM_OPTIONS * 1;
        const int INDEX2 = NUM_OPTIONS * 2;

        const char *ARGS[] = { "E"                 // OPTIONS[INDEX0]
                             , "--bb"              // OPTIONS[INDEX1]
                             , "987654321"         // "             "
                             , "--bb"              // "             "
                             , "192837465"         // "             "
                             , "--cdefghijklmab"   // OPTIONS[INDEX2]
                             };

        if (veryVerbose) cout << "\tLength 1" << endl;
        {
            bsl::ostringstream ossX, ossY;

            OptionInfo       mTable[] = { OPTIONS[INDEX0] };
            const OptionInfo  Table[] = { OPTIONS[INDEX0] };

            int NUM_OPTIONS = sizeof Table / sizeof *Table;

            ASSERT(Obj::isValidOptionSpecificationTable(mTable));
            ASSERT(Obj::isValidOptionSpecificationTable( Table));

            Obj mX(mTable, ossX);         const Obj& X = mX;
            Obj mY( Table, ossY);         const Obj& Y = mY;
            Obj mZ( Table, NUM_OPTIONS);  const Obj& Z = mZ;

            ASSERT("" == ossX.str());
            ASSERT("" == ossY.str());

            // Selected to satisfy 'OPTIONS[0]'.
            const char *Argv[] = { "aProgramName"
                                 , ARGS[0]
                                 };

            const bsl::size_t Argc = sizeof Argv / sizeof *Argv;

            int rcX = mX.parse(Argc, Argv);
            int rcY = mY.parse(Argc, Argv);
            int rcZ = mZ.parse(Argc, Argv);

            ASSERT(0 == rcX); ASSERT(X.isParsed());
            ASSERT(0 == rcY); ASSERT(Y.isParsed());
            ASSERT(0 == rcZ); ASSERT(Z.isParsed());

            ASSERT(X == Y);
            ASSERT(Y == Z);
        }

        if (veryVerbose) cout << "\tLength 2" << endl;
        {
            bsl::ostringstream ossX, ossY;

            OptionInfo       mTable[] = { OPTIONS[INDEX0]
                                        , OPTIONS[INDEX1]
                                        };
            const OptionInfo  Table[] = { OPTIONS[INDEX0]
                                        , OPTIONS[INDEX1]
                                        };

            int NUM_OPTIONS = sizeof Table / sizeof *Table;

            ASSERT(Obj::isValidOptionSpecificationTable(mTable));
            ASSERT(Obj::isValidOptionSpecificationTable( Table));

            Obj mX(mTable, ossX);         const Obj& X = mX;
            Obj mY( Table, ossY);         const Obj& Y = mY;
            Obj mZ( Table, NUM_OPTIONS);  const Obj& Z = mZ;

            ASSERT("" == ossX.str());
            ASSERT("" == ossY.str());

            // Selected to satisfy 'OPTIONS[1]'.
            const char *Argv[] = { "anotherProgramName"
                                 , ARGS[0]
                                 , ARGS[1]
                                 , ARGS[2]
                                 , ARGS[3]
                                 , ARGS[4]
                                 };

            const bsl::size_t Argc = sizeof Argv / sizeof *Argv;

            int rcX = mX.parse(Argc, Argv);
            int rcY = mY.parse(Argc, Argv);
            int rcZ = mZ.parse(Argc, Argv);

            ASSERT(0 == rcX); ASSERT(X.isParsed());
            ASSERT(0 == rcY); ASSERT(Y.isParsed());
            ASSERT(0 == rcZ); ASSERT(Z.isParsed());

            ASSERT(X == Y);
            ASSERT(Y == Z);
        }

        if (veryVerbose) cout << "\tLength 3" << endl;
        {
            bsl::ostringstream ossX, ossY;

            OptionInfo       mTable[] = { OPTIONS[INDEX0]
                                        , OPTIONS[INDEX1]
                                        , OPTIONS[INDEX2]
                                        };
            const OptionInfo  Table[] = { OPTIONS[INDEX0]
                                        , OPTIONS[INDEX1]
                                        , OPTIONS[INDEX2]
                                        };

            int  NUM_OPTIONS = sizeof Table / sizeof *Table;

            ASSERT(Obj::isValidOptionSpecificationTable(mTable));
            ASSERT(Obj::isValidOptionSpecificationTable( Table));

            Obj mX(mTable, ossX);         const Obj& X = mX;
            Obj mY( Table, ossY);         const Obj& Y = mY;
            Obj mZ( Table, NUM_OPTIONS);  const Obj& Z = mZ;

            ASSERT("" == ossX.str());
            ASSERT("" == ossY.str());

            const char *Argv[] = { "yetAnotherProgramName"
                                 , ARGS[0]
                                 , ARGS[1]
                                 , ARGS[2]
                                 , ARGS[3]
                                 , ARGS[4]
                                 , ARGS[5]
                                 };

            const bsl::size_t Argc = sizeof Argv / sizeof *Argv;

            int rcX = mX.parse(Argc, Argv);
            int rcY = mY.parse(Argc, Argv);
            int rcZ = mZ.parse(Argc, Argv);

            ASSERT(0 == rcX);  ASSERT(X.isParsed());
            ASSERT(0 == rcY);  ASSERT(Y.isParsed());
            ASSERT(0 == rcZ);  ASSERT(Z.isParsed());

            ASSERT(X == Y);
            ASSERT(Y == Z);
        }
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'u::parseCommandLine' TESTING UTILITY
        //   Exercise the basic functionality.
        //
        // Concerns:
        //: 1 That the 'u::parseCommandLine' testing utility function separates
        //:   a command line as needed into its 'argc' and 'argv'.
        //
        // Plan:
        //: 1 It is enough to test with zero, one, or two arguments on the
        //:   command line, and test with various spacing and separation
        //:   characters.  For orthogonal perturbations, add a header or a
        //:   trailer or both.
        //
        // Testing:
        //   TESTING 'u::parseCommandLine' TESTING UTILITY
        // --------------------------------------------------------------------

        if (verbose) cout
                    << endl
                    << "TESTING 'u::parseCommandLine' TESTING UTILITY" << endl
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

                LOOP_ASSERT(LINE, RET  == ret);
                if (0 == ret) {
                    LOOP_ASSERT(LINE, ARGC == argc);
                    for (int i=0; i < argc; ++i) {
                        LOOP_ASSERT(LINE, 0 == bsl::strcmp(ARGV[i], argv[i]));
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

                LOOP_ASSERT(LINE, RET  == ret);
                if (0 == ret) {
                    LOOP_ASSERT(LINE, ARGC == argc);
                    for (int i=0; i < argc; ++i) {
                        LOOP_ASSERT(LINE, 0 == bsl::strcmp(ARGV[i], argv[i]));
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

                LOOP_ASSERT(LINE, RET  == ret);
                if (0 == ret) {
                    LOOP_ASSERT(LINE, ARGC == argc);
                    for (int i=0; i < argc; ++i) {
                        LOOP_ASSERT(LINE, 0 == bsl::strcmp(ARGV[i], argv[i]));
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
                LOOP_ASSERT(LINE, RET  == ret);
                if (0 == ret) {
                    LOOP_ASSERT(LINE, ARGC == argc);
                    for (int i=0; i < argc; ++i) {
                        LOOP_ASSERT(LINE, 0 == bsl::strcmp(ARGV[i], argv[i]));
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
        //: 1 Basic functionality must work as advertised for most common usage
        //:   of this component.
        //
        // Plan:
        //: 1 Various ad hoc use of this component.
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
            "f|flag",                                        // tag
            "genericFlag",                                   // name
            "provide generic option for breathing test",     // description
            balcl::TypeInfo(
                                     balcl::OptionType::k_BOOL),
                                                          // no linked variable
            balcl::OccurrenceInfo::e_REQUIRED // occurrence info
          },
          {
            "s|long-option-with-dashes",                     // tag
            "longOptionValue",                               // name
            "provide generic option for breathing test",     // description
            balcl::TypeInfo(balcl::OptionType::k_INT_ARRAY),
                                                          // no linked variable
            balcl::OccurrenceInfo::e_REQUIRED // occurrence info
          },
          {
            "long-option-only",                              // tag
            "longOptionOnlyValue",                           // name
            "provide generic option for breathing test",     // description
            balcl::TypeInfo(&longOptionOnlyValue), // linked variable
            balcl::OccurrenceInfo::e_HIDDEN   // occurrence info
          },
          {
            "",                                              // tag
            "nonOptionValue1",                               // name
            "provide generic option for breathing test",     // description
            balcl::TypeInfo(&nonOptionValue1),     // linked variable
            balcl::OccurrenceInfo::e_REQUIRED // occurrence info
          },
          {
            "",                                           // tag
            "nonOptionValue2",                            // name
            "provide generic option for breathing test",  // description
            balcl::TypeInfo(&nonOptionValue2),  // linked variable
            balcl::OccurrenceInfo(defaultNonOptionValue)
                                                          // occurrence info
          },
          {
            "",                                           // tag
            "nonOptionValue3",                            // name
            "provide generic option for breathing test",  // description
            balcl::TypeInfo(&nonOptionValue3),  // linked variable
            balcl::OccurrenceInfo(defaultNonOptionValue)
                                                          // occurrence info
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
