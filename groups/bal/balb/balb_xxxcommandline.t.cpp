// balb_xxxcommandline.t.cpp                                             -*-C++-*-

#include <balb_xxxcommandline.h>

#include <bdlf_bind.h>
#include <bdlf_function.h>
#include <bdlmxxx_binding.h>
#include <bdlmxxx_elemtype.h>
#include <bdlmxxx_schemaaggregateutil.h>
#include <bdlmxxx_schemautil.h>
#include <bdlpuxxx_typesparser.h>
#include <bdlpuxxx_typesparserimputil.h>
#include <bdlt_datetime.h>
#include <bdlt_date.h>
#include <bdlt_time.h>
#include <bdlb_chartype.h>

#include <bslalg_typetraits.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_iostream.h>
#include <bsl_stdexcept.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#include <bsl_cstdlib.h>

using namespace BloombergLP;
using bsl::cerr;
using bsl::cout;
using bsl::endl;
using bsl::flush;

//=============================================================================
//                                   TEST PLAN
//-----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// This is a somewhat large component with many classes.  The main class,
// 'balb::CommandLine', has in-core value semantic and supports the
// 'bslma::Allocator' memory model.  A slight complication is that there is an
// invalid state (unset value) and valid states (after 'parse' returns
// successfully).  Nevertheless, we follow the usual value-semantic type test
// driver, simplified from the fact that there are no manipulators (once
// created and parsed, a 'balb::CommandLine' object does not change value).
//
// Testing 'balb_xxxcommandline' is divided into 2 parts (apart from breathing
// test and usage example).  The first part tests the classes themselves
// (including the supporting classes), and the second tests the various
// sections in the component-level documentation.  Each supporting class has
// in-core value semantics as well, and so we follow the test sequence for such
// types except that we do without the generating functions (but note that we
// have 'create...Info' functions for brevity).  For testing that the various
// examples given in the documentation are valid and work as expected, we have
// a utility which parses a command line and fills up the 'argc' and 'argv' in
// the same way as a Unix shell would.  Then we use this utility to
// systematically test all the examples given in the documentation, in addition
// to examples selected to test the particular concern at hand.  For instance,
// for the section "Flags bundling", we recreate a specification that conforms
// to the documentation usage, and check that parsing the example command lines
// produces valid equivalent results; we also exercise a mix of optional and
// non-optional options, etc.
//
// As a note, we made all the usage strings part of this test driver.  This may
// seem like an overkill and an annoyance (if the usage formatting changes, all
// the strings in the test driver will need to be changed too).  In fact, that
// is a *good* thing since it forces the implementor to manually verify that
// all these usage strings are properly aligned, human-readable, etc. --
// something that an automatic test driver cannot do.  Besides finding obvious
// errors in the documentation, of course.
//-----------------------------------------------------------------------------
// 'balb::CommandLine' public interface:
// ------------------------------------
// CREATORS
// [ 9] balb::CommandLine();
// [ 9] ~balb::CommandLine();
// MANIPULATORS
// [ 9] operator=(balb::CommandLine const&);
// [ 9] balb::CommandLine::parse(...);
// ACCESSORS
// [ 9] balb::CommandLine::data() const;
// [ 9] balb::CommandLine::specifiedData(); // OOPS - non-const!
// [ 9] balb::CommandLine::the*Type*();
// [ 9] balb::CommandLine::printUsage();
// [ 9] balb::CommandLine::print();
// [ 9] operator<<(bsl::ostream&, balb::CommandLine const&);
// FREE OPERATORS
// [ 9] balb::CommandLine::operator==();
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] TESTING PARSE_COMMAND_LINE TESTING UTILITY
// [ 3] TESTING CLASS BAEA_COMMANDLINEOCCURRENCEINFO
// [ 4] TESTING CLASS BAEA_COMMANDLINEOPTIONTYPE
// [ 5] TESTING CLASS BAEA_COMMANDLINECONSTRAINT
// [ 6] TESTING CLASS BAEA_COMMANDLINETYPEINFO
// [ 7] TESTING CLASS BAEA_COMMANDLINEOPTIONINFO
// [ 8] TESTING CLASS BAEA_COMMANDLINEOPTION
// [10] TESTING INVALID OPTION SPECS
// [11] TESTING FLAGS BUNDLING
// [12] TESTING MULTIPLY-SPECIFIED FLAGS
// [13] TESTING ORDER OF ARGUMENTS
// [14] TESTING PARSING OF STRINGS
// [15] TESTING NON-OPTION TOGGLE '--'
// [16] TESTING PRINTUSAGE
// [17] USAGE EXAMPLE
//=============================================================================
//                        STANDARD BDE ASSERT TEST MACROS
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << #M << ": " <<  \
       M << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                       SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

//=============================================================================
//          GLOBAL TYPEDEFS/CONSTANTS/VARIABLES/FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

typedef balb::CommandLine                Obj;

typedef balb::CommandLineConstraint      Constraint;
typedef balb::CommandLineOccurrenceInfo  OccurrenceInfo;
typedef OccurrenceInfo::OccurrenceType  OccurrenceType;
typedef balb::CommandLineOption          Option;
typedef balb::CommandLineOptionInfo      OptionInfo;
typedef balb::CommandLineOptionType      OptionType;
typedef balb::CommandLineTypeInfo        TypeInfo;

typedef bdlmxxx::ElemType                   ET;
typedef bdlmxxx::ElemType::Type             BdemType;
typedef bsls::Types::Int64              Int64;
typedef bsls::Types::Uint64             Uint64;

// ATTRIBUTES FOR 'balb::CommandLineOption'
static const struct {
    int         d_line;  // line number
    const char *d_tag;   // tag attribute
} OPTION_TAGS[] = {
    { L_, "" }
  , { L_, "a|aa" }
  , { L_, "b|bb" }
  , { L_, "cdefghijklmab" }  // note: only first four tags used in case 8
  , { L_, "a|aaabb" }
  , { L_, "b|bbbbb" }
  , { L_, "nopqrstuvwxyz" }
};
const int NUM_OPTION_TAGS = sizeof OPTION_TAGS / sizeof *OPTION_TAGS;

static const struct {
    int         d_line;  // line number
    const char *d_name;  // name attribute
} OPTION_NAMES[] = {
    { L_, "" }
  , { L_, "A" }
  , { L_, "B" }
  , { L_, "ABCDEFGHIJKLMNOPQRSTUVWXYZ" }
};
const int NUM_OPTION_NAMES = sizeof OPTION_NAMES / sizeof *OPTION_NAMES;

static const struct {
    int         d_line;         // line number
    const char *d_description;  // description attribute
} OPTION_DESCRIPTIONS[] = {
    { L_, "" }
  , { L_, "A" }
  , { L_, "B" }
  , { L_, "ABCDEFGHIJKLMNOPQRSTUVWXYZ" }
};
const int NUM_OPTION_DESCRIPTIONS = sizeof OPTION_DESCRIPTIONS /
                                                  sizeof *OPTION_DESCRIPTIONS;

// ATTRIBUTES FOR 'balb::CommandLineTypeInfo'
bool                        linkedBool;
char                        linkedChar;
short                       linkedShort;
int                         linkedInt;
Int64                       linkedInt64;
float                       linkedFloat;
double                      linkedDouble;
bsl::string                 linkedString;
bdlt::Datetime               linkedDatetime;
bdlt::Date                   linkedDate;
bdlt::Time                   linkedTime;
bsl::vector<char>           linkedCharArray;
bsl::vector<short>          linkedShortArray;
bsl::vector<int>            linkedIntArray;
bsl::vector<Int64>          linkedInt64Array;
bsl::vector<float>          linkedFloatArray;
bsl::vector<double>         linkedDoubleArray;
bsl::vector<bsl::string>    linkedStringArray;
bsl::vector<bdlt::Datetime>  linkedDatetimeArray;
bsl::vector<bdlt::Date>      linkedDateArray;
bsl::vector<bdlt::Time>      linkedTimeArray;

struct OptConstraint {

    // CLASS DATA
    static bool constraintValue;
        // Global return value (for easier control).

    // DATA
    char d_buffer[1 + sizeof(bdlf::Function<bool (*)(const void *,
                                                    bsl::ostream*)>)];
         // This data member ensures that the 'bdlf::Function' object which
         // contains this object has to allocate.
};
bool OptConstraint::constraintValue = true;

struct OptCharConstraint : public OptConstraint {
    bool operator()(const char *, bsl::ostream&) const {
        return constraintValue;
    }
} optCharConstraint;

struct OptShortConstraint : public OptConstraint {
    bool operator()(const short *, bsl::ostream&) const {
        return constraintValue;
    }
} optShortConstraint;

struct OptIntConstraint : public OptConstraint {
    bool operator()(const int *, bsl::ostream&) const {
        return constraintValue;
    }
} optIntConstraint;

struct OptInt64Constraint : public OptConstraint {
    bool operator()(const bsls::Types::Int64 *, bsl::ostream&) const {
        return constraintValue;
    }
} optInt64Constraint;

struct OptFloatConstraint : public OptConstraint {
    bool operator()(const float *, bsl::ostream&) const {
        return constraintValue;
    }
} optFloatConstraint;

struct OptDoubleConstraint : public OptConstraint {
    bool operator()(const double *, bsl::ostream&) const {
        return constraintValue;
    }
} optDoubleConstraint;

struct OptStringConstraint : public OptConstraint {
    bool operator()(const bsl::string *, bsl::ostream&) const {
        return constraintValue;
    }
} optStringConstraint;

struct OptDatetimeConstraint : public OptConstraint {

    bool operator()(const bdlt::Datetime *, bsl::ostream&) const {
        return constraintValue;
    }
} optDatetimeConstraint;

struct OptDateConstraint : public OptConstraint {

    bool operator()(const bdlt::Date *, bsl::ostream&) const {
        return constraintValue;
    }
} optDateConstraint;

struct OptTimeConstraint : public OptConstraint {
    bool operator()(const bdlt::Time *, bsl::ostream&) const {
        return constraintValue;
    }
} optTimeConstraint;

const struct {
    int       d_line;              // line number
    BdemType  d_type;              // option type
    void     *d_linkedVariable_p;  // linked variable attribute(s)
    void     *d_constraint_p;      // linked variable attribute(s)
} OPTION_TYPEINFO[] = {
   { L_, ET::BDEM_BOOL,           0,                    0                     }
 , { L_, ET::BDEM_CHAR,           0,                    0                     }
 , { L_, ET::BDEM_SHORT,          0,                    0                     }
 , { L_, ET::BDEM_INT,            0,                    0                     }
 , { L_, ET::BDEM_INT64,          0,                    0                     }
 , { L_, ET::BDEM_FLOAT,          0,                    0                     }
 , { L_, ET::BDEM_DOUBLE,         0,                    0                     }
 , { L_, ET::BDEM_STRING,         0,                    0                     }
 , { L_, ET::BDEM_DATETIME,       0,                    0                     }
 , { L_, ET::BDEM_DATE,           0,                    0                     }
 , { L_, ET::BDEM_TIME,           0,                    0                     }
 , { L_, ET::BDEM_CHAR_ARRAY,     0,                    0                     }
 , { L_, ET::BDEM_SHORT_ARRAY,    0,                    0                     }
 , { L_, ET::BDEM_INT_ARRAY,      0,                    0                     }
 , { L_, ET::BDEM_INT64_ARRAY,    0,                    0                     }
 , { L_, ET::BDEM_FLOAT_ARRAY,    0,                    0                     }
 , { L_, ET::BDEM_DOUBLE_ARRAY,   0,                    0                     }
 , { L_, ET::BDEM_STRING_ARRAY,   0,                    0                     }
 , { L_, ET::BDEM_DATETIME_ARRAY, 0,                    0                     }
 , { L_, ET::BDEM_DATE_ARRAY,     0,                    0                     }
 , { L_, ET::BDEM_TIME_ARRAY,     0,                    0                     }
 , { L_, ET::BDEM_BOOL,           &linkedBool,          0                     }
 , { L_, ET::BDEM_CHAR,           &linkedChar,          0                     }
 , { L_, ET::BDEM_SHORT,          &linkedShort,         0                     }
 , { L_, ET::BDEM_INT,            &linkedInt,           0                     }
 , { L_, ET::BDEM_INT64,          &linkedInt64,         0                     }
 , { L_, ET::BDEM_FLOAT,          &linkedFloat,         0                     }
 , { L_, ET::BDEM_DOUBLE,         &linkedDouble,        0                     }
 , { L_, ET::BDEM_STRING,         &linkedString,        0                     }
 , { L_, ET::BDEM_DATETIME,       &linkedDatetime,      0                     }
 , { L_, ET::BDEM_DATE,           &linkedDate,          0                     }
 , { L_, ET::BDEM_TIME,           &linkedTime,          0                     }
 , { L_, ET::BDEM_CHAR_ARRAY,     &linkedCharArray,     0                     }
 , { L_, ET::BDEM_SHORT_ARRAY,    &linkedShortArray,    0                     }
 , { L_, ET::BDEM_INT_ARRAY,      &linkedIntArray,      0                     }
 , { L_, ET::BDEM_INT64_ARRAY,    &linkedInt64Array,    0                     }
 , { L_, ET::BDEM_FLOAT_ARRAY,    &linkedFloatArray,    0                     }
 , { L_, ET::BDEM_DOUBLE_ARRAY,   &linkedDoubleArray,   0                     }
 , { L_, ET::BDEM_STRING_ARRAY,   &linkedStringArray,   0                     }
 , { L_, ET::BDEM_DATETIME_ARRAY, &linkedDatetimeArray, 0                     }
 , { L_, ET::BDEM_DATE_ARRAY,     &linkedDateArray,     0                     }
 , { L_, ET::BDEM_TIME_ARRAY,     &linkedTimeArray,     0                     }
 , { L_, ET::BDEM_BOOL,           0,                    // &optBoolConstraint }
                                                        0                     }
 , { L_, ET::BDEM_CHAR,           0,                    &optCharConstraint    }
 , { L_, ET::BDEM_SHORT,          0,                    &optShortConstraint   }
 , { L_, ET::BDEM_INT,            0,                    &optIntConstraint     }
 , { L_, ET::BDEM_INT64,          0,                    &optInt64Constraint   }
 , { L_, ET::BDEM_FLOAT,          0,                    &optFloatConstraint   }
 , { L_, ET::BDEM_DOUBLE,         0,                    &optDoubleConstraint  }
 , { L_, ET::BDEM_STRING,         0,                    &optStringConstraint  }
 , { L_, ET::BDEM_DATETIME,       0,                    &optDatetimeConstraint}
 , { L_, ET::BDEM_DATE,           0,                    &optDateConstraint    }
 , { L_, ET::BDEM_TIME,           0,                    &optTimeConstraint    }
 , { L_, ET::BDEM_CHAR_ARRAY,     0,                    &optCharConstraint    }
 , { L_, ET::BDEM_SHORT_ARRAY,    0,                    &optShortConstraint   }
 , { L_, ET::BDEM_INT_ARRAY,      0,                    &optIntConstraint     }
 , { L_, ET::BDEM_INT64_ARRAY,    0,                    &optInt64Constraint   }
 , { L_, ET::BDEM_FLOAT_ARRAY,    0,                    &optFloatConstraint   }
 , { L_, ET::BDEM_DOUBLE_ARRAY,   0,                    &optDoubleConstraint  }
 , { L_, ET::BDEM_STRING_ARRAY,   0,                    &optStringConstraint  }
 , { L_, ET::BDEM_DATETIME_ARRAY, 0,                    &optDatetimeConstraint}
 , { L_, ET::BDEM_DATE_ARRAY,     0,                    &optDateConstraint    }
 , { L_, ET::BDEM_TIME_ARRAY,     0,                    &optTimeConstraint    }
 , { L_, ET::BDEM_BOOL,           &linkedBool,          // &optBoolConstraint }
                                                        0                     }
 , { L_, ET::BDEM_CHAR,           &linkedChar,          &optCharConstraint    }
 , { L_, ET::BDEM_SHORT,          &linkedShort,         &optShortConstraint   }
 , { L_, ET::BDEM_INT,            &linkedInt,           &optIntConstraint     }
 , { L_, ET::BDEM_INT64,          &linkedInt64,         &optInt64Constraint   }
 , { L_, ET::BDEM_FLOAT,          &linkedFloat,         &optFloatConstraint   }
 , { L_, ET::BDEM_DOUBLE,         &linkedDouble,        &optDoubleConstraint  }
 , { L_, ET::BDEM_STRING,         &linkedString,        &optStringConstraint  }
 , { L_, ET::BDEM_DATETIME,       &linkedDatetime,      &optDatetimeConstraint}
 , { L_, ET::BDEM_DATE,           &linkedDate,          &optDateConstraint    }
 , { L_, ET::BDEM_TIME,           &linkedTime,          &optTimeConstraint    }
 , { L_, ET::BDEM_CHAR_ARRAY,     &linkedCharArray,     &optCharConstraint    }
 , { L_, ET::BDEM_SHORT_ARRAY,    &linkedShortArray,    &optShortConstraint   }
 , { L_, ET::BDEM_INT_ARRAY,      &linkedIntArray,      &optIntConstraint     }
 , { L_, ET::BDEM_INT64_ARRAY,    &linkedInt64Array,    &optInt64Constraint   }
 , { L_, ET::BDEM_FLOAT_ARRAY,    &linkedFloatArray,    &optFloatConstraint   }
 , { L_, ET::BDEM_DOUBLE_ARRAY,   &linkedDoubleArray,   &optDoubleConstraint  }
 , { L_, ET::BDEM_STRING_ARRAY,   &linkedStringArray,   &optStringConstraint  }
 , { L_, ET::BDEM_DATETIME_ARRAY, &linkedDatetimeArray, &optDatetimeConstraint}
 , { L_, ET::BDEM_DATE_ARRAY,     &linkedDateArray,     &optDateConstraint    }
 , { L_, ET::BDEM_TIME_ARRAY,     &linkedTimeArray,     &optTimeConstraint    }
};
const int NUM_OPTION_TYPEINFO = sizeof OPTION_TYPEINFO /
                                                      sizeof *OPTION_TYPEINFO;

#undef VP

// ATTRIBUTES FOR 'balb::CommandLineOccurrenceInfo'
static const struct {
    int             d_line;  // line number
    OccurrenceType  d_type;  // name attribute
} OPTION_OCCURRENCES[] = {
    { L_, OccurrenceInfo::BAEA_REQUIRED }
  , { L_, OccurrenceInfo::BAEA_OPTIONAL }
  , { L_, OccurrenceInfo::BAEA_HIDDEN   }
};
const int NUM_OPTION_OCCURRENCES = sizeof OPTION_OCCURRENCES /
                                                    sizeof *OPTION_OCCURRENCES;

bool                        defaultBool          = false;
char                        defaultChar          = 'D';
short                       defaultShort         = 1234;
int                         defaultInt           = 1234567;
Int64                       defaultInt64         = 123456789LL;
float                       defaultFloat         = 0.125;     // 1/8
double                      defaultDouble        = 0.015625;  // 1/64
bsl::string                 defaultString        = "ABCDEFGHIJ";
bdlt::Datetime               defaultDatetime(1234, 12, 3, 4, 5, 6);
bdlt::Date                   defaultDate(1234, 4, 6);
bdlt::Time                   defaultTime(7, 8, 9, 10);
bsl::vector<char>           defaultCharArray(1, defaultChar);
bsl::vector<short>          defaultShortArray(1, defaultShort);
bsl::vector<int>            defaultIntArray(1, defaultInt);
bsl::vector<Int64>          defaultInt64Array(1, defaultInt64);
bsl::vector<float>          defaultFloatArray(1, defaultFloat);
bsl::vector<double>         defaultDoubleArray(1, defaultDouble);
bsl::vector<bsl::string>    defaultStringArray(1, defaultString);
bsl::vector<bdlt::Datetime>  defaultDatetimeArray(1, defaultDatetime);
bsl::vector<bdlt::Date>      defaultDateArray(1, defaultDate);
bsl::vector<bdlt::Time>      defaultTimeArray(1, defaultTime);

static const struct {
    int             d_line;   // line number
    ET::Type        d_type;   // option type
    const void     *d_value;  // default value attribute(s)
} OPTION_DEFAULT_VALUES[] = {
    { L_, ET::BDEM_BOOL,            0                     }
  , { L_, ET::BDEM_CHAR,            &defaultChar          }
  , { L_, ET::BDEM_SHORT,           &defaultShort         }
  , { L_, ET::BDEM_INT,             &defaultInt           }
  , { L_, ET::BDEM_INT64,           &defaultInt64         }
  , { L_, ET::BDEM_FLOAT,           &defaultFloat         }
  , { L_, ET::BDEM_DOUBLE,          &defaultDouble        }
  , { L_, ET::BDEM_STRING,          &defaultString        }
  , { L_, ET::BDEM_DATETIME,        &defaultDatetime      }
  , { L_, ET::BDEM_DATE,            &defaultDate          }
  , { L_, ET::BDEM_TIME,            &defaultTime          }
  , { L_, ET::BDEM_CHAR_ARRAY,      &defaultCharArray     }
  , { L_, ET::BDEM_SHORT_ARRAY,     &defaultShortArray    }
  , { L_, ET::BDEM_INT_ARRAY,       &defaultIntArray      }
  , { L_, ET::BDEM_INT64_ARRAY,     &defaultInt64Array    }
  , { L_, ET::BDEM_FLOAT_ARRAY,     &defaultFloatArray    }
  , { L_, ET::BDEM_DOUBLE_ARRAY,    &defaultDoubleArray   }
  , { L_, ET::BDEM_STRING_ARRAY,    &defaultStringArray   }
  , { L_, ET::BDEM_DATETIME_ARRAY,  &defaultDatetimeArray }
  , { L_, ET::BDEM_DATE_ARRAY,      &defaultDateArray     }
  , { L_, ET::BDEM_TIME_ARRAY,      &defaultTimeArray     }
};
const int NUM_OPTION_DEFAULT_VALUES = sizeof OPTION_DEFAULT_VALUES /
                                                 sizeof *OPTION_DEFAULT_VALUES;

bool                     linkedBoolA;
bool                     linkedBoolB;
bool                     linkedBoolC;
bsl::string              linkedString1;
bsl::vector<bsl::string> linkedStringArray1;

const int MAX_SPEC_SIZE = 12;
static const struct {
            int         d_line;
            int         d_numSpecTable;
            OptionInfo d_specTable[MAX_SPEC_SIZE];
} SPECS[] = {
    { L_, 11, {  // Spec #1 (index 0)
                 {
                     "a|aLong",
                     "Name_a",
                     "Description for a",
                     TypeInfo(&linkedBoolA),
                     OccurrenceInfo::BAEA_OPTIONAL
                 },
                 {
                     "b|bLong",
                     "Name_b",
                     "Description for b",
                     TypeInfo(&linkedBoolB),
                     OccurrenceInfo::BAEA_OPTIONAL
                 },
                 {
                     "c|cLong",
                     "Name_c",
                     "Description for c",
                     TypeInfo(&linkedBoolC),
                     OccurrenceInfo::BAEA_OPTIONAL
                 },
                 {
                     "i|iLong",
                     "Name_i",
                     "Description for i",
                     TypeInfo(&linkedInt),
                     OccurrenceInfo::BAEA_OPTIONAL
                 },
                 {
                     "f|fLong",
                     "Name_f",
                     "Description for f",
                     TypeInfo(&linkedFloat),
                     OccurrenceInfo::BAEA_OPTIONAL
                 },
                 {
                     "s|sLong",
                     "Name_s",
                     "Description for s",
                     TypeInfo(&linkedString),
                     OccurrenceInfo::BAEA_OPTIONAL
                 },
                 {
                     "I|ILong",
                     "Name_I",
                     "Description for I",
                     TypeInfo(&linkedIntArray),
                     OccurrenceInfo::BAEA_OPTIONAL
                 },
                 {
                     "F|FLong",
                     "Name_F",
                     "Description for F",
                     TypeInfo(&linkedFloatArray),
                     OccurrenceInfo::BAEA_OPTIONAL
                 },
                 {
                     "S|SLong",
                     "Name_S",
                     "Description for S",
                     TypeInfo(&linkedStringArray),
                     OccurrenceInfo::BAEA_OPTIONAL
                 },
                 {
                     "",
                     "Name1",
                     "Description1",
                     TypeInfo(&linkedString1),
                     OccurrenceInfo::BAEA_OPTIONAL
                 },
                 {
                     "",
                     "Name2",
                     "Description2",
                     TypeInfo(&linkedStringArray1),
                     OccurrenceInfo::BAEA_OPTIONAL
                 }
             }
    }
};
const int NUM_SPECS = sizeof SPECS / sizeof *SPECS;

//=============================================================================
//                          HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

                         // =========================
                         // function generateArgument
                         // =========================

static int generateArgument(bsl::string       *argString,
                            const OptionInfo&  optionInfo,
                            int                seed = 0)
   // Generate into the specified 'argString' a command line string suitable to
   // be parsed by a 'balb::CommandLine' object having the specified
   // 'optionInfo'.  Optionally specified a 'seed' for changing the return
   // value of 'argString' in a pseudo-random fashion.  Return 0 upon success,
   // and non-zero otherwise.
{
    const OccurrenceInfo& occurrenceInfo = optionInfo.d_defaultInfo;
    Option option(optionInfo);

    if (OccurrenceInfo::BAEA_REQUIRED == occurrenceInfo.occurrenceType()
     || 0 != seed % 3)
    {
        bsl::string optionFlag;
        if (OptionInfo::BAEA_NON_OPTION != option.argType()) {
            if (1 == seed % 3 && option.shortTag()) {
                optionFlag = bsl::string("-") + option.shortTag();
            } else {
                optionFlag = bsl::string("--") + option.longTag();
            }
        }
        char separator = ' ';
        if (3 < seed % 7) {
            separator = '=';
        }
        switch (optionInfo.d_typeInfo.type()) {
          case ET::BDEM_BOOL: {
            *argString = optionFlag;
          } break;
          case ET::BDEM_CHAR: {
            *argString = optionFlag + separator + "a";
          } break;
          case ET::BDEM_SHORT: {
            *argString = optionFlag + separator + "125";
          } break;
          case ET::BDEM_INT: {
            *argString = optionFlag + separator + "123654";
          } break;
          case ET::BDEM_INT64: {
            *argString = optionFlag + separator + "987654321";
          } break;
          case ET::BDEM_FLOAT: {
            *argString = optionFlag + separator + "0.2625";
          } break;
          case ET::BDEM_DOUBLE: {
            *argString = optionFlag + separator + "0.376739501953125";
          } break;
          case ET::BDEM_STRING: {
            *argString = optionFlag + separator + "someString";
          } break;
          case ET::BDEM_DATETIME: {
            *argString = optionFlag + separator + "2008/07/22_04:06:08";
          } break;
          case ET::BDEM_DATE: {
            *argString = optionFlag + separator + "2007/08/22";
          } break;
          case ET::BDEM_TIME: {
            *argString = optionFlag + separator + "08:06:04";
          } break;
          case ET::BDEM_CHAR_ARRAY: {
            *argString = optionFlag + separator + "a";
            *argString += ' ';
            *argString += optionFlag + separator + "z";
          } break;
          case ET::BDEM_SHORT_ARRAY: {
            *argString = optionFlag + separator + "125";
            *argString += ' ';
            *argString += optionFlag + separator + "478";
          } break;
          case ET::BDEM_INT_ARRAY: {
            *argString = optionFlag + separator + "123654";
            *argString += ' ';
            *argString += optionFlag + separator + "654321";
          } break;
          case ET::BDEM_INT64_ARRAY: {
            *argString = optionFlag + separator + "987654321";
            *argString += ' ';
            *argString += optionFlag + separator + "192837465";
          } break;
          case ET::BDEM_FLOAT_ARRAY: {
            *argString = optionFlag + separator + "0.2625";
            *argString += ' ';
            *argString += optionFlag + separator + "0.7375";
          } break;
          case ET::BDEM_DOUBLE_ARRAY: {
            *argString = optionFlag + separator + "0.376739501953125";
            *argString += ' ';
            *argString += optionFlag + separator + "0.623260498046875";
          } break;
          case ET::BDEM_STRING_ARRAY: {
            *argString = optionFlag + separator + "someString";
            *argString += ' ';
            *argString += optionFlag + separator + "someOtherString";
          } break;
          case ET::BDEM_DATETIME_ARRAY: {
            *argString = optionFlag + separator + "2008/07/22_04:06:08";
            *argString += ' ';
            *argString += optionFlag + separator + "2006/05/11_02:04:06";
          } break;
          case ET::BDEM_DATE_ARRAY: {
            *argString = optionFlag + separator + "2007/08/22";
            *argString += ' ';
            *argString += optionFlag + separator + "2005/06/11";
          } break;
          case ET::BDEM_TIME_ARRAY: {
            *argString = optionFlag + separator + "08:06:04";
            *argString += ' ';
            *argString += optionFlag + separator + "02:04:06";
          } break;
          default: ASSERT(0);
        };
    }

    // Assert that required arguments must be provided.

    ASSERT(OccurrenceInfo::BAEA_REQUIRED != occurrenceInfo.occurrenceType()
                                                       || !argString->empty());

    return 0;
}

                         // =========================
                         // function parseCommandLine
                         // =========================

const int MAX_ARGS = 512;

static int parseCommandLine(char       *cmdLine,
                            int&        argc,
                            const char *argv[],
                            int         maxArgs = MAX_ARGS)
    // Parse the specified modifiable 'cmdLine' as would a Unix shell, by
    // replacing every space by a character '\0' and recording the beginning of
    // each field into an array which is loaded into the specified 'argv', and
    // load the number of fields into the specified modifiable 'argc'.  Return
    // 0 on success, non-zero if the command line exceeds the specified
    // 'maxArgs' number of arguments or on failure.  Note that the 'cmdLine'
    // must be null-terminated and may not contain a carriage return or
    // newline.  Fields must be separated by either a space or a tab character.
{
    // If passing null string (not even empty), failure.

    if (!cmdLine) {
        return -1;
    }

    // Initialize 'argc'.

    argc = 0;

    // Parse command line.

    while (1) {
        // Skip spaces if any.

        while (' ' == *cmdLine || '\t' == *cmdLine) {             // SPC or TAB
            ++cmdLine;
        }

        // Check for end-of-string.

        if (!*cmdLine) { // null-terminating character                   // EOL
            return 0;    // we're done
        }
        if (0xa == *cmdLine || 0xd == *cmdLine) {                   // NL or CR
            return -1;    // FAILURE
        }

        // Check overflow.

        if (argc >= maxArgs) {
            return -1;
        }

        // Not a space, newline, CR, or null-terminating character:
        // mark beginning of field.

        argv[argc] = cmdLine;
        ++argc;

        // Find the end of this field.

        while (' ' != *cmdLine && '\t' != *cmdLine) {
            if (!*cmdLine) { // null-terminating character
                return 0;    // we're done
            }
            if (0xa == *cmdLine || 0xd == *cmdLine) {
                return -1;    // FAILURE
            }
            ++cmdLine;
        }

        // And mark it.

        *cmdLine = 0; // mark the end of this field
        ++cmdLine; // continue to next field
    }

    return 0; // never reached
}

                     // =================================
                     // function template checkOptionType
                     // =================================

template <int ELEM_TYPE, class TYPE>
struct CheckOptionType {
    // This general definition of the 'CheckOptionType' class template provides
    // a boolean functor that returns 'false'.

    bool operator()() const { return false; }
};

#define MATCH_OPTION_TYPE(ELEM_TYPE, TYPE)                               \
    template <>                                                          \
    struct CheckOptionType<(int)ELEM_TYPE, TYPE> {                       \
        bool operator()() const { return true; }                         \
    };                                                                   \
    template <>                                                          \
    struct CheckOptionType<(int)ELEM_TYPE##_ARRAY, bsl::vector<TYPE> > { \
        bool operator()() const { return true; }                         \
    };
    // This macro defines two specializations of the 'CheckOptionType' class
    // template whose boolean functor returns 'true', one for the parameterized
    // 'ELEM_TYPE' matching the parameterized 'TYPE', and the other for the
    // corresponding array type.

MATCH_OPTION_TYPE(ET::BDEM_BOOL,     bool)

MATCH_OPTION_TYPE(ET::BDEM_CHAR,     char)
MATCH_OPTION_TYPE(ET::BDEM_SHORT,    short)
MATCH_OPTION_TYPE(ET::BDEM_INT,      int)
MATCH_OPTION_TYPE(ET::BDEM_INT64,    bsls::Types::Int64)
MATCH_OPTION_TYPE(ET::BDEM_FLOAT,    float)
MATCH_OPTION_TYPE(ET::BDEM_DOUBLE,   double)
MATCH_OPTION_TYPE(ET::BDEM_STRING,   bsl::string)
MATCH_OPTION_TYPE(ET::BDEM_DATETIME, bdlt::Datetime)
MATCH_OPTION_TYPE(ET::BDEM_DATE,     bdlt::Date)
MATCH_OPTION_TYPE(ET::BDEM_TIME,     bdlt::Time)

#undef MATCH_OPTION_TYPE

template <int ELEM_TYPE, class TYPE>
bool checkOptionType(TYPE *optionTypeValue)
    // Return 'true' if the specified 'optionTypeValue' is a null pointer whose
    // parameterized 'TYPE' matches the 'bdem' type described by the
    // parameterized 'ELEM_TYPE' constant.
{
    CheckOptionType<ELEM_TYPE, TYPE> checker;
    return checker() && (TYPE *)0 == optionTypeValue;
}

                         // =========================
                         // functions test*Constraint
                         // =========================

bool testCharConstraint(const char *, bsl::ostream&)           { return true; }
bool testShortConstraint(const short *, bsl::ostream&)         { return true; }
bool testIntConstraint(const int *, bsl::ostream&)             { return true; }
bool testInt64Constraint(const bsls::Types::Int64 *, bsl::ostream&)
                                                               { return true; }
bool testFloatConstraint(const float *, bsl::ostream&)         { return true; }
bool testDoubleConstraint(const double *, bsl::ostream&)       { return true; }
bool testStringConstraint(const bsl::string *, bsl::ostream&)  { return true; }
bool testDatetimeConstraint(const bdlt::Datetime *, bsl::ostream&)
                                                               { return true; }
bool testDateConstraint(const bdlt::Date *, bsl::ostream&)      { return true; }
bool testTimeConstraint(const bdlt::Time *, bsl::ostream&)      { return true; }
    // These functions, never invoked, provide a default value for the
    // 'balb::CommandLineConstraint' nested 'typedefs'.

                           // ======================
                           // function setConstraint
                           // ======================

void setConstraint(TypeInfo *typeInfo, BdemType type, void *address)
    // Set the constraint of the specified 'typeInfo' to the function at
    // the specified 'address' of the signature corresponding to the specified
    // 'bdem' element 'type'.
{
    switch (type) {
      case ET::BDEM_CHAR:
      case ET::BDEM_CHAR_ARRAY: {
        typeInfo->setConstraint(balb::CommandLineConstraint::CharConstraint(
                             *reinterpret_cast<OptCharConstraint *>(address)));
      } break;
      case ET::BDEM_SHORT:
      case ET::BDEM_SHORT_ARRAY: {
        typeInfo->setConstraint(balb::CommandLineConstraint::ShortConstraint(
                            *reinterpret_cast<OptShortConstraint *>(address)));
      } break;
      case ET::BDEM_INT:
      case ET::BDEM_INT_ARRAY: {
        typeInfo->setConstraint(balb::CommandLineConstraint::IntConstraint(
                              *reinterpret_cast<OptIntConstraint *>(address)));
      } break;
      case ET::BDEM_INT64:
      case ET::BDEM_INT64_ARRAY: {
        typeInfo->setConstraint(balb::CommandLineConstraint::Int64Constraint(
                            *reinterpret_cast<OptInt64Constraint *>(address)));
      } break;
      case ET::BDEM_FLOAT:
      case ET::BDEM_FLOAT_ARRAY: {
        typeInfo->setConstraint(balb::CommandLineConstraint::FloatConstraint(
                            *reinterpret_cast<OptFloatConstraint *>(address)));
      } break;
      case ET::BDEM_DOUBLE:
      case ET::BDEM_DOUBLE_ARRAY: {
        typeInfo->setConstraint(balb::CommandLineConstraint::DoubleConstraint(
                           *reinterpret_cast<OptDoubleConstraint *>(address)));
      } break;
      case ET::BDEM_STRING:
      case ET::BDEM_STRING_ARRAY: {
        typeInfo->setConstraint(balb::CommandLineConstraint::StringConstraint(
                           *reinterpret_cast<OptStringConstraint *>(address)));
      } break;
      case ET::BDEM_DATETIME:
      case ET::BDEM_DATETIME_ARRAY: {
        typeInfo->setConstraint(balb::CommandLineConstraint::DatetimeConstraint(
                         *reinterpret_cast<OptDatetimeConstraint *>(address)));
      } break;
      case ET::BDEM_DATE:
      case ET::BDEM_DATE_ARRAY: {
        typeInfo->setConstraint(balb::CommandLineConstraint::DateConstraint(
                             *reinterpret_cast<OptDateConstraint *>(address)));
      } break;
      case ET::BDEM_TIME:
      case ET::BDEM_TIME_ARRAY: {
        typeInfo->setConstraint(balb::CommandLineConstraint::TimeConstraint(
                             *reinterpret_cast<OptTimeConstraint *>(address)));
      } break;
      default: {
        ASSERT(0);
      } break;
    };
}

                         // ==========================
                         // function setLinkedVariable
                         // ==========================

void setLinkedVariable(TypeInfo *typeInfo, BdemType type, void *address)
    // Set the linked variable of the specified 'typeInfo' to the variable at
    // the specified 'address' of the type corresponding to the specified
    // 'bdem' element 'type'.
{
    switch (type) {
      case ET::BDEM_BOOL: {
        typeInfo->setLinkedVariable((bool *)address);
      } break;
      case ET::BDEM_CHAR: {
        typeInfo->setLinkedVariable((char *)address);
      } break;
      case ET::BDEM_SHORT: {
        typeInfo->setLinkedVariable((short *)address);
      } break;
      case ET::BDEM_INT: {
        typeInfo->setLinkedVariable((int *)address);
      } break;
      case ET::BDEM_INT64: {
        typeInfo->setLinkedVariable((Int64 *)address);
      } break;
      case ET::BDEM_FLOAT: {
        typeInfo->setLinkedVariable((float *)address);
      } break;
      case ET::BDEM_DOUBLE: {
        typeInfo->setLinkedVariable((double *)address);
      } break;
      case ET::BDEM_STRING: {
        typeInfo->setLinkedVariable((bsl::string *)address);
      } break;
      case ET::BDEM_DATETIME: {
        typeInfo->setLinkedVariable((bdlt::Datetime *)address);
      } break;
      case ET::BDEM_DATE: {
        typeInfo->setLinkedVariable((bdlt::Date *)address);
      } break;
      case ET::BDEM_TIME: {
        typeInfo->setLinkedVariable((bdlt::Time *)address);
      } break;
      case ET::BDEM_CHAR_ARRAY: {
        typeInfo->setLinkedVariable((bsl::vector<char>*)address);
      } break;
      case ET::BDEM_SHORT_ARRAY: {
        typeInfo->setLinkedVariable((bsl::vector<short>*)address);
      } break;
      case ET::BDEM_INT_ARRAY: {
        typeInfo->setLinkedVariable((bsl::vector<int>*)address);
      } break;
      case ET::BDEM_INT64_ARRAY: {
        typeInfo->setLinkedVariable((bsl::vector<Int64>*)address);
      } break;
      case ET::BDEM_FLOAT_ARRAY: {
        typeInfo->setLinkedVariable((bsl::vector<float>*)address);
      } break;
      case ET::BDEM_DOUBLE_ARRAY: {
        typeInfo->setLinkedVariable((bsl::vector<double>*)address);
      } break;
      case ET::BDEM_STRING_ARRAY: {
        typeInfo->setLinkedVariable((bsl::vector<bsl::string>*)address);
      } break;
      case ET::BDEM_DATETIME_ARRAY: {
        typeInfo->setLinkedVariable((bsl::vector<bdlt::Datetime>*)address);
      } break;
      case ET::BDEM_DATE_ARRAY: {
        typeInfo->setLinkedVariable((bsl::vector<bdlt::Date>*)address);
      } break;
      case ET::BDEM_TIME_ARRAY: {
        typeInfo->setLinkedVariable((bsl::vector<bdlt::Time>*)address);
      } break;
      default: {
        ASSERT(0);
      } break;
    };
}

                         // ================
                         // function setType
                         // ================

void setType(TypeInfo *typeInfo, BdemType type)
    // Set the 'bdem' element type of the specified 'typeInfo' to the
    // specified 'type'.  Note that this resets both the linked variable and
    // constraint of 'typeInfo'.
{
    switch (type) {
      case ET::BDEM_BOOL: {
        typeInfo->setLinkedVariable(OptionType::BAEA_BOOL);
      } break;
      case ET::BDEM_CHAR: {
        typeInfo->setLinkedVariable(OptionType::BAEA_CHAR);
      } break;
      case ET::BDEM_SHORT: {
        typeInfo->setLinkedVariable(OptionType::BAEA_SHORT);
      } break;
      case ET::BDEM_INT: {
        typeInfo->setLinkedVariable(OptionType::BAEA_INT);
      } break;
      case ET::BDEM_INT64: {
        typeInfo->setLinkedVariable(OptionType::BAEA_INT64);
      } break;
      case ET::BDEM_FLOAT: {
        typeInfo->setLinkedVariable(OptionType::BAEA_FLOAT);
      } break;
      case ET::BDEM_DOUBLE: {
        typeInfo->setLinkedVariable(OptionType::BAEA_DOUBLE);
      } break;
      case ET::BDEM_STRING: {
        typeInfo->setLinkedVariable(OptionType::BAEA_STRING);
      } break;
      case ET::BDEM_DATETIME: {
        typeInfo->setLinkedVariable(OptionType::BAEA_DATETIME);
      } break;
      case ET::BDEM_DATE: {
        typeInfo->setLinkedVariable(OptionType::BAEA_DATE);
      } break;
      case ET::BDEM_TIME: {
        typeInfo->setLinkedVariable(OptionType::BAEA_TIME);
      } break;
      case ET::BDEM_CHAR_ARRAY: {
        typeInfo->setLinkedVariable(OptionType::BAEA_CHAR_ARRAY);
      } break;
      case ET::BDEM_SHORT_ARRAY: {
        typeInfo->setLinkedVariable(OptionType::BAEA_SHORT_ARRAY);
      } break;
      case ET::BDEM_INT_ARRAY: {
        typeInfo->setLinkedVariable(OptionType::BAEA_INT_ARRAY);
      } break;
      case ET::BDEM_INT64_ARRAY: {
        typeInfo->setLinkedVariable(OptionType::BAEA_INT64_ARRAY);
      } break;
      case ET::BDEM_FLOAT_ARRAY: {
        typeInfo->setLinkedVariable(OptionType::BAEA_FLOAT_ARRAY);
      } break;
      case ET::BDEM_DOUBLE_ARRAY: {
        typeInfo->setLinkedVariable(OptionType::BAEA_DOUBLE_ARRAY);
      } break;
      case ET::BDEM_STRING_ARRAY: {
        typeInfo->setLinkedVariable(OptionType::BAEA_STRING_ARRAY);
      } break;
      case ET::BDEM_DATETIME_ARRAY: {
        typeInfo->setLinkedVariable(OptionType::BAEA_DATETIME_ARRAY);
      } break;
      case ET::BDEM_DATE_ARRAY: {
        typeInfo->setLinkedVariable(OptionType::BAEA_DATE_ARRAY);
      } break;
      case ET::BDEM_TIME_ARRAY: {
        typeInfo->setLinkedVariable(OptionType::BAEA_TIME_ARRAY);
      } break;
      default: {
        ASSERT(0);
      } break;
    };
}

                          // =======================
                          // function createTypeInfo
                          // =======================

void createTypeInfo(TypeInfo *typeInfo,
                    BdemType  type,
                    void     *variable = 0,
                    void     *constraint = 0)
{
    setType(typeInfo, type);

    if (variable) {
        setLinkedVariable(typeInfo, type, variable);
    }
    if (constraint) {
        setConstraint(typeInfo, type, constraint);
    }
}

TypeInfo createTypeInfo(BdemType  type,
                        void     *variable = 0,
                        void     *constraint = 0)
{
    TypeInfo result;
    createTypeInfo(&result, type, variable, constraint);
    return result;
}

                          // =============================
                          // function createOccurrenceInfo
                          // =============================

OccurrenceInfo createOccurrenceInfo(OccurrenceType  occurrenceType,
                                    BdemType        type,
                                    const void     *variable)
{
    OccurrenceInfo result(occurrenceType);

    if (occurrenceType != OccurrenceInfo::BAEA_REQUIRED && variable) {
        const bdlmxxx::Descriptor *desc = bdlmxxx::ElemAttrLookup::lookupTable()[type];
        bdlmxxx::ConstElemRef defaultValue(variable, desc);

        result.setDefaultValue(defaultValue);
    }

    return result;
}

                          // ========================
                           // function parseTypeInfo
                          // ========================

int parseTypeInfo(const char      **endpos,
                  const TypeInfo&   typeInfo,
                  const char       *input)
    // Parse the specified 'input' for a value and verify that this value
    // matches the specified 'typeInfo' (at least in 'bdem' type) and return 0
    // if parsing and verification succeed.  Return a non-zero value if parsing
    // fails or if the value parsed does not match 'typeInfo', and return in
    // the specified 'endpos' the first unsuccessful parsing position.
{
    typedef bdlpuxxx::ParserImpUtil Parser;
    enum { SUCCESS = 0, FAILURE = -1 };

    bsl::string ptrString;

    if (Parser::skipWhiteSpace(&input, input)
     || Parser::skipRequiredToken(&input, input, "{")
     || Parser::skipWhiteSpace(&input, input)
     || Parser::skipRequiredToken(&input, input, "TYPE")
     || Parser::skipWhiteSpace(&input, input)
     || Parser::skipRequiredToken(&input, input, ET::toAscii(typeInfo.type())))
    {
        *endpos = input;
        return FAILURE;
    }

    if (typeInfo.linkedVariable()) {
        if (Parser::skipWhiteSpace(&input, input)
         || Parser::skipRequiredToken(&input, input, "VARIABLE")
         || Parser::skipWhiteSpace(&input, input))
        {
            *endpos = input;
            return FAILURE;
        }
        else {
            // Retrieve the pointer address.
            ptrString.clear();
            while(!bdlb::CharType::isSpace(*input)) {
                ptrString += *input;
                ++input;
            }

            bsl::ostringstream oss;
            oss << (void *)typeInfo.linkedVariable();
            if (oss.str() != ptrString) {
                *endpos = input;
                return FAILURE;
            }
        }
    }

    if (Parser::skipWhiteSpace(&input, input)
     || Parser::skipRequiredToken(&input, input, "CONSTRAINT")
     || Parser::skipWhiteSpace(&input, input))
    {
        *endpos = input;
        return FAILURE;
    }
    else {
        // Retrieve the pointer address.
        ptrString.clear();
        while(!bdlb::CharType::isSpace(*input)) {
            ptrString += *input;
            ++input;
        }

        bsl::ostringstream oss;
        oss << (void *)typeInfo.constraint().get();
        if (oss.str() != ptrString) {
            *endpos = input;
            return FAILURE;
        }
    }

    if (Parser::skipWhiteSpace(&input, input)
     || Parser::skipRequiredToken(&input, input, "}"))
    {
        *endpos = input;
        return FAILURE;
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
    typedef bdlpuxxx::ParserImpUtil Parser;
    enum { SUCCESS = 0, FAILURE = -1 };

    if (Parser::skipWhiteSpace(&input, input)) {
        *endpos = input;
        return FAILURE;
    }
    if (*input != '{') {
        if (OccurrenceInfo::BAEA_REQUIRED == occurrenceInfo.occurrenceType()) {
            if (Parser::skipRequiredToken(endpos, input, "REQUIRED")) {
                return FAILURE;
            }
            return SUCCESS;
        }
        if (OccurrenceInfo::BAEA_OPTIONAL == occurrenceInfo.occurrenceType()) {
            if (Parser::skipRequiredToken(endpos, input, "OPTIONAL")) {
                return FAILURE;
            }
            return SUCCESS;
        }
        if (OccurrenceInfo::BAEA_HIDDEN == occurrenceInfo.occurrenceType()) {
            if (Parser::skipRequiredToken(endpos, input, "HIDDEN")) {
                return FAILURE;
            }
            return SUCCESS;
        }
        *endpos = input;
        return FAILURE;
    } else {
        ++input; // Parser::skipRequiredToken(&input, input, "{");
        if (OccurrenceInfo::BAEA_OPTIONAL == occurrenceInfo.occurrenceType()) {
            if (Parser::skipWhiteSpace(&input, input)
             || Parser::skipRequiredToken(&input, input, "OPTIONAL")) {
                *endpos = input;
                return FAILURE;
            }
        } else {
            if (Parser::skipWhiteSpace(&input, input)
             || Parser::skipRequiredToken(&input, input, "HIDDEN")) {
                *endpos = input;
                return FAILURE;
            }
        }
        if (occurrenceInfo.hasDefaultValue()) {
            if (Parser::skipWhiteSpace(&input, input)
             || Parser::skipRequiredToken(&input, input, "DEFAULT_TYPE")) {
                *endpos = input;
                return FAILURE;
            }
            if (Parser::skipWhiteSpace(&input, input)
             || Parser::skipRequiredToken(&input, input,
                          ET::toAscii(occurrenceInfo.defaultValue().type()))) {
                *endpos = input;
                return FAILURE;
            }
            if (Parser::skipWhiteSpace(&input, input)
             || Parser::skipRequiredToken(&input, input, "DEFAULT_VALUE")
             || Parser::skipWhiteSpace(&input, input)) {
                *endpos = input;
                return FAILURE;
            }
            switch (occurrenceInfo.defaultValue().type()) {
              case ET::BDEM_CHAR: {
                char charValue = *input;
                if (charValue != occurrenceInfo.defaultValue().theChar()) {
                    return FAILURE;
                }
                *endpos = ++input;
              } break;
              case ET::BDEM_SHORT: {
                short shortValue;
                if (bdlpuxxx::TypesParser::parseShort(endpos, &shortValue, input)
                 || shortValue != occurrenceInfo.defaultValue().theShort()) {
                    return FAILURE;
                }
              } break;
              case ET::BDEM_INT: {
                int intValue;
                if (bdlpuxxx::TypesParser::parseInt(endpos, &intValue, input)
                 || intValue != occurrenceInfo.defaultValue().theInt()) {
                    return FAILURE;
                }
              } break;
              case ET::BDEM_INT64: {
                Int64 int64Value;
                if (bdlpuxxx::TypesParser::parseInt64(endpos, &int64Value, input)
                 || int64Value != occurrenceInfo.defaultValue().theInt64()) {
                    return FAILURE;
                }
              } break;
              case ET::BDEM_FLOAT: {
                float floatValue;
                if (bdlpuxxx::TypesParser::parseFloat(endpos, &floatValue, input)
                 || floatValue != occurrenceInfo.defaultValue().theFloat()) {
                    // There is no guaranteed round-trip on floating point I/O.
                    // return FAILURE;
                }
              } break;
              case ET::BDEM_DOUBLE: {
                double doubleValue;
                if (bdlpuxxx::TypesParser::parseDouble(endpos, &doubleValue, input)
                 || doubleValue != occurrenceInfo.defaultValue().theDouble()) {
                    // There is no guaranteed round-trip on floating point I/O.
                    // return FAILURE;
                }
              } break;
              case ET::BDEM_STRING: {
                bsl::string stringValue;
                if (bdlpuxxx::TypesParser::parseString(endpos, &stringValue, input)
                 || stringValue != occurrenceInfo.defaultValue().theString()) {
                    return FAILURE;
                }
              } break;
              case ET::BDEM_DATETIME: {
                bdlt::Datetime datetimeValue;
                if (bdlpuxxx::TypesParser::parseDatetime(
                                                 endpos, &datetimeValue, input)
                 || datetimeValue !=
                                 occurrenceInfo.defaultValue().theDatetime()) {
                    // Incomprehensibly, DATETIME isn't a round trip for I/O,
                    // because DATE isn't either (see below).
                    // return FAILURE;
                    if (0 == (*endpos = bsl::strchr(input, '}'))) {
                        return FAILURE;
                    }
                }
              } break;
              case ET::BDEM_DATE: {
                bdlt::Date dateValue;
                if (bdlpuxxx::TypesParser::parseDate(endpos, &dateValue, input)
                 || dateValue != occurrenceInfo.defaultValue().theDate()) {
                    // Incomprehensibly, DATE isn't a round trip for I/O, it
                    // prints as, e.g., 06APR1234, but wants to be parsed as
                    // 1234/04/06.  Go figure...
                    // return FAILURE;
                    if (0 == (*endpos = bsl::strchr(input, '}'))) {
                        return FAILURE;
                    }
                }
              } break;
              case ET::BDEM_TIME: {
                bdlt::Time timeValue;
                if (bdlpuxxx::TypesParser::parseTime(endpos, &timeValue, input)
                 || timeValue != occurrenceInfo.defaultValue().theTime()) {
                    return FAILURE;
                }
              } break;
              case ET::BDEM_CHAR_ARRAY: {
                bsl::string charArrayAsString;
                if (bdlpuxxx::TypesParser::parseQuotedString(
                                          endpos, &charArrayAsString, input)) {
                    // CHAR_ARRAY isn't round trip for I/O, in fact, printing a
                    // CHAR_ARRAY amounts to printing a string delimited by
                    // double quotes, whereas parsing requires parsing a
                    // sequence of white-separated CHARs enclosed by '[' ']'.
                    // Use parseQuotedString instead.
                    return FAILURE - 1;
                }
                bsl::vector<char> charArrayValue(charArrayAsString.begin(),
                                                 charArrayAsString.end());
                if (charArrayValue !=
                                occurrenceInfo.defaultValue().theCharArray()) {
                    return FAILURE - 2;
                }
              } break;
              case ET::BDEM_SHORT_ARRAY: {
                bsl::vector<short> shortArrayValue;
                if (bdlpuxxx::TypesParser::parseShortArray(
                                               endpos, &shortArrayValue, input)
                 || shortArrayValue !=
                               occurrenceInfo.defaultValue().theShortArray()) {
                    return FAILURE;
                }
              } break;
              case ET::BDEM_INT_ARRAY: {
                bsl::vector<int> intArrayValue;
                if (bdlpuxxx::TypesParser::parseIntArray(
                                                 endpos, &intArrayValue, input)
                 || intArrayValue !=
                                 occurrenceInfo.defaultValue().theIntArray()) {
                    return FAILURE;
                }
              } break;
              case ET::BDEM_INT64_ARRAY: {
                bsl::vector<Int64> int64ArrayValue;
                if (bdlpuxxx::TypesParser::parseInt64Array(
                                               endpos, &int64ArrayValue, input)
                 || int64ArrayValue !=
                               occurrenceInfo.defaultValue().theInt64Array()) {
                    return FAILURE;
                }
              } break;
              case ET::BDEM_FLOAT_ARRAY: {
                bsl::vector<float> floatArrayValue;
                if (bdlpuxxx::TypesParser::parseFloatArray(
                                               endpos, &floatArrayValue, input)
                 || floatArrayValue !=
                               occurrenceInfo.defaultValue().theFloatArray()) {
                    // There is no guaranteed round-trip on floating point I/O.
                }
              } break;
              case ET::BDEM_DOUBLE_ARRAY: {
                bsl::vector<double> doubleArrayValue;
                if (bdlpuxxx::TypesParser::parseDoubleArray(
                                              endpos, &doubleArrayValue, input)
                 || doubleArrayValue !=
                              occurrenceInfo.defaultValue().theDoubleArray()) {
                    // There is no guaranteed round-trip on floating point I/O.
                }
              } break;
              case ET::BDEM_STRING_ARRAY: {
                bsl::vector<bsl::string> stringArrayValue;
                if (bdlpuxxx::TypesParser::parseStringArray(
                                              endpos, &stringArrayValue, input)
                 || stringArrayValue !=
                              occurrenceInfo.defaultValue().theStringArray()) {
                    return FAILURE;
                }
              } break;
              case ET::BDEM_DATETIME_ARRAY: {
                bsl::vector<bdlt::Datetime> datetimeArrayValue;
                if (bdlpuxxx::TypesParser::parseDatetimeArray(
                                            endpos, &datetimeArrayValue, input)
                 || datetimeArrayValue !=
                            occurrenceInfo.defaultValue().theDatetimeArray()) {
                    // Incomprehensibly, ... see DATETIME above.

                    if ( 0 == (*endpos = bsl::strchr(input, '}'))) {
                        return FAILURE;
                    }
                }
              } break;
              case ET::BDEM_DATE_ARRAY: {
                bsl::vector<bdlt::Date> dateArrayValue;
                if (bdlpuxxx::TypesParser::parseDateArray(
                                                endpos, &dateArrayValue, input)
                 || dateArrayValue !=
                                occurrenceInfo.defaultValue().theDateArray()) {
                    // Incomprehensibly, ... see DATETIME above.

                    if ( 0 == (*endpos = bsl::strchr(input, '}'))) {
                        return FAILURE;
                    }
                }
              } break;
              case ET::BDEM_TIME_ARRAY: {
                bsl::vector<bdlt::Time> timeArrayValue;
                if (bdlpuxxx::TypesParser::parseTimeArray(
                                                endpos, &timeArrayValue, input)
                 || timeArrayValue !=
                                occurrenceInfo.defaultValue().theTimeArray()) {
                    return FAILURE;
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
            return FAILURE;
        }
    }

    *endpos = input;
    return SUCCESS;
}

                        // ===============================
                        // function parseCommandLineOption
                        // ===============================

int parseCommandLineOption(const char    **endpos,
                           const Option&   commandLineOption,
                           const char     *input)
    // Parse the specified 'input' for a value and verify that this value
    // matches the specified 'commandLineOption' and return 0 if parsing and
    // verification succeed.  Return a non-zero value if parsing fails or if
    // the value parsed does not match 'commandLineOption', and return in the
    // specified 'endpos' the first unsuccessful parsing position.
{
    typedef bdlpuxxx::ParserImpUtil Parser;
    enum { SUCCESS = 0, FAILURE = -1 };

    bsl::string tagString, nameString, descriptionString;

    if (Parser::skipWhiteSpace(&input, input)
     || Parser::skipRequiredToken(&input, input, "{")
     || Parser::skipWhiteSpace(&input, input))
    {
        *endpos = input;
        return FAILURE;
    }

    if (Parser::skipRequiredToken(&input, input, "NON_OPTION")) {
        if (Parser::skipRequiredToken(&input, input, "TAG")
         || Parser::skipWhiteSpace(&input, input)
         || bdlpuxxx::TypesParser::parseQuotedString(&input, &tagString, input)
         || tagString != commandLineOption.tagString())
        {
            *endpos = input;
            return FAILURE - 1;
        }
    }

    if (Parser::skipWhiteSpace(&input, input)
     || Parser::skipRequiredToken(&input, input, "NAME")
     || Parser::skipWhiteSpace(&input, input)
     || bdlpuxxx::TypesParser::parseQuotedString(&input, &nameString, input)
     || nameString != commandLineOption.name())
    {
        *endpos = input;
        return FAILURE - 2;
    }

    if (Parser::skipWhiteSpace(&input, input)
     || Parser::skipRequiredToken(&input, input, "DESCRIPTION")
     || Parser::skipWhiteSpace(&input, input)
     || bdlpuxxx::TypesParser::parseQuotedString(&input, &descriptionString, input)
     || descriptionString != commandLineOption.description())
    {
        *endpos = input;
        return FAILURE - 3;
    }
    if (Parser::skipWhiteSpace(&input, input)
     || Parser::skipRequiredToken(&input, input, "TYPE_INFO"))
    {
        *endpos = input;
        return FAILURE - 4;
    }
    if (parseTypeInfo(&input, commandLineOption.typeInfo(), input))
    {
        *endpos = input;
        return FAILURE - 5;
    }
    if (Parser::skipWhiteSpace(&input, input)
     || Parser::skipRequiredToken(&input, input, "OCCURRENCE_INFO"))
    {
        *endpos = input;
        return FAILURE - 6;
    }

    if (parseOccurrenceInfo(&input, commandLineOption.occurrenceInfo(), input))
    {
        *endpos = input;
        return FAILURE - 7;
    }

    if (Parser::skipWhiteSpace(&input, input)
     || Parser::skipRequiredToken(&input, input, "}"))
    {
        *endpos = input;
        return FAILURE - 8;
    }

    *endpos = input;
    return SUCCESS;
}

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
    errorMsg += line;
    errorMsg += "\nAn invalid 'balb::CommandLineOptionInfo' was encountered.";
    errorMsg += "\nThe following assertion failed: ";
    errorMsg += text;
    throw InvalidSpec(errorMsg);
}

                       // =============================
                       // function isCompatibleOrdering
                       // =============================

bool isCompatibleOrdering(const char *const *argv1,
                          const char *const *argv2,
                          int                argc)
    // Return 'true' if all the arguments specifying non-options values and
    // multi-valued options held in the specified 'argv1' array of the
    // specified length 'argc' occur in the same order as in the specified
    // 'argv2' array of the same length, and 'false' otherwise.  The behavior
    // is undefined unless the arguments are either "-S=..." for multi-valued
    // short options (single capitalized letter), "-s=..." for singly-valued
    // short options (non-capitalized), or non-option values.
{
    bool nonOptionSeenFlag = false;
    bool multiOptionSeenFlag[26];
    bsl::fill(multiOptionSeenFlag, multiOptionSeenFlag + 26, false);

    for (int i = 1; i < argc; ++i) {
        bool isOption = argv1[i][0] == '-';
        char shortOptionTag = isOption ? argv1[i][1] : 0;
        if ((isOption && 'A' <= shortOptionTag && shortOptionTag <= 'Z' &&
                                    !multiOptionSeenFlag[shortOptionTag - 'A'])
         || (!isOption && !nonOptionSeenFlag))
        {
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
                return false;
            }
        }
    }
    return true;
}

//=============================================================================
//          USAGE EXAMPLE CLASSES AND FUNCTIONS
//-----------------------------------------------------------------------------
namespace BAEA_COMMANDLINE_USAGE_EXAMPLE {

///Usage
///-----
// Suppose we want to design a sorting utility named 'mysort' that has the
// following syntax:
//..
//  usage: mysort  [-r|reverse] [-i|insensativetocase] [-u|uniq]
//                 [-a|algorithm sortAlgo] <-o|outputfile outputFile>
//                 fileList...
//                            // Sort the specified files (fileList), using
//                            // the specified sorting algorithm and write
//                            // the output to the specified output file.
//
//     option                               note
//  ============  ====================================================
//  -a|algorithm  (1) Value (provided at command line) of this option must
//                    be one among "quickSort", "insertionSort", "shellSort".
//                (2) If not provided, default value will be "quickSort".
//
//  -o|outfile    (1) This option must not be omitted on command line.
//..
// We choose the non-option to be of array of 'bsl::string' type so as to
// accommodate multiple files.
//
// These options may be used incorrectly, as the following examples show:
//..
//             INCORRECT USE                        REASON FOR INACCURACY
//  ===========================================  ============================
//  $$ mysort -riu -o myofile -aDUMBSORT f1 f2   Incorrect because DUMBSORT
//                                               is not among valid values
//                                               for the -a option.
//
//  $$ mysort -riu f1 f2                         Incorrect because no value
//                                               is provided for the -o option.
//..
// In order to handle these, our application provides the following
// free function:
//..
    bool isValidAlgorithm(const bsl::string *algo, bsl::ostream& os)
        // Return true if the specified 'algo' is one among
        // "quickSort", "insertionSort" and "shellSort" otherwise
        // return false and fill the specified 'os' with an
        // appropriate error message.
    {
        bsl::string quickSortStr("quickSort");
        bsl::string insertionSortStr("insertionSort");
        bsl::string shellSortStr("shellSort");

        if (quickSortStr == *algo) {
            return true;
        }
        if (insertionSortStr == *algo) {
            return true;
        }
        if (shellSortStr == *algo) {
            return true;
        }
        os << "Error: sorting algorithm (" << *algo << ") must be either "
              "quickSort, insertionSort or shellSort.\n";
        return false;
    }
//..
// Using those, we can now use an object of 'balb::CommandLine' type to parse
// the command line options.  The proper usage is shown below.  First we
// declare the variables to be linked with the options.  If they were needed at
// a global level, we could declare as globals, but we prefer to declare them
// as local variables inside 'main':
//..
    int usageExample(int argc, const char * const argv[], bool verbose) {
        // NOTE: char * argv[] here is intentionally non-const to verify that
        // the 'parse' function is able to handle non-const command line
        // arguments correctly.

        // Variables to be linked with options.  Note that it is important that
        // unless a default is specified via 'balb::CommandLineOccurrenceInfo',
        // the variables must be initialized to their default value since
        // unless the option is specified on the command line, the variable's
        // value will be unmodified by 'parse'.

        bool isReverse = false;
        bool isCaseInsensitive = false;
        bool isUniq = false;

        bsl::string outFile;
        bsl::string sortAlgo;

        bsl::vector<bsl::string> files;
//..
// Next we build up an option specification table as follows:
//..
        // build constraint for sortAlgo option
        balb::CommandLineConstraint::StringConstraint validAlgoConstraint;
        validAlgoConstraint = &isValidAlgorithm;

        // option specification table
        balb::CommandLineOptionInfo specTable[] = {
          {
            "r|reverse",                                 // tag
            "isReverse",                                 // name
            "use the reverse sorting order",             // description
            balb::CommandLineTypeInfo(&isReverse)         // link
          },
          {
            "i|insensitivetocase",                       // tag
            "isCaseInsensitive",                         // name
            "use case insensitive while sorting",        // description
            balb::CommandLineTypeInfo(&isCaseInsensitive) // link
          },
          {
            "u|uniq",                                    // tag
            "isUniq",                                    // name
            "don't keep duplicate lines",                // description
            balb::CommandLineTypeInfo(&isUniq)            // link
          },
          {
            "a|algorithm",                               // tag
            "sortAlgo",                                  // name and a default
            "sorting algorithm",                         // description
            balb::CommandLineTypeInfo(&sortAlgo, validAlgoConstraint),
                                                         // link and constraint
            balb::CommandLineOccurrenceInfo(bsl::string("quickSort"))
                                                         // default algorithm
          },
          {
            "o|outputfile",                               // tag
            "outputFile",                                 // name
            "output file",                                // description
            balb::CommandLineTypeInfo(&outFile),           // link
            balb::CommandLineOccurrenceInfo::BAEA_REQUIRED // occurrence info
          },
          {
            "",                                          // non-option
            "file",                                      // name
            "files to be sorted",                        // description
            balb::CommandLineTypeInfo(&files)             // link
          }
        };
        const int numSpecTable = sizeof specTable / sizeof *specTable;
//..
// We can now create a command line specification and parse the command line
// options:
//..
        //======================== CUT HERE ===========================
        // print spec table
        static bool printSpec = false;
        if (verbose && !printSpec) {
            cout << "Printing command line option info:\n"
                    "----------------------------------" << endl;
            for (int i = 0; i < numSpecTable; ++i) {
                P(specTable[i]);
            }
            printSpec = true;
        }
        //======================= RESUME HERE =========================

        // create command line specification
        if (verbose) cout << "Creating command line object:\n"
                             "-----------------------------" << endl;
        balb::CommandLine cmdLine(specTable, numSpecTable);

        // parse command line options; if failure, print usage
        if (verbose) cout << "Parsing command line:\n"
                             "---------------------" << endl;
        bsl::ostringstream oss;  // test driver: redirect output in non-verbose
        if (cmdLine.parse(argc, argv, oss)) {
            if (verbose) { cmdLine.printUsage(); }
            return -1;
        } else {
            if (verbose) { cout << oss.str() << endl; }
        }
//..
// Upon successful parsing, the 'cmdLine' object will acquire a value which
// conforms to the specified constraints.  We can examine these values as
// follows:
//..
        // if success, obtain command line option values
        bdem_ConstRowBinding binding = cmdLine.data();

        // access through linked variable
        if (verbose) {
            bsl::cout << outFile                         << bsl::endl;
            bsl::cout << cmdLine.theString("outputFile") << bsl::endl;
            bsl::cout << binding.theString("outputFile") << bsl::endl;
        }

        // access through *theType* methods
        ASSERT(cmdLine.theString("outputFile") == outFile);

        // access through binding
        ASSERT(binding.theString("outputFile") == outFile);

        // check that required option has been specified once
        ASSERT(cmdLine.isSpecified("outputFile"));

        int count = -1;
        ASSERT(cmdLine.isSpecified("outputFile", &count));
        ASSERT(1 == count);

//..
// We can also print the value of that command line, which is the same as the
// 'binding':
//..
        if (verbose) {
            cout << "cmdLine = "; cmdLine.print(cout, -1, 4);
            cout << endl;
        }

        return 0;
    }
//..
// For instance, the following command lines:
//..
//  $$ mysort -omyofile f1 f2 f3
//  $$ mysort -ainsertionSort f1 f2 f3 -riu -o myofile
//  $$ mysort --algorithm insertionSort  --outputfile myofile  f1 f2 f3 --uniq
//..
// will all produce the same following output "myofile".

}  // close namespace BAEA_COMMANDLINE_USAGE_EXAMPLE

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, const char *argv[])  {
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 17: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE:
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and execute as shown.
        //
        // Plan:
        //   Incorporate the usage example from the header file into the test
        //   driver.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING USAGE EXAMPLE" << endl
                          << "=====================" << endl;

        using namespace BAEA_COMMANDLINE_USAGE_EXAMPLE;

        typedef const char *CChar;

        static const struct {
            int   d_line;
            int   d_retCode;
            const char *d_cmdLine;
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
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE     = DATA[i].d_line;
            const int   RET      = DATA[i].d_retCode;
            const char *CMD_LINE = DATA[i].d_cmdLine;

            char       *cmdLine  = new char[bsl::strlen(CMD_LINE)+1];
            bsl::strcpy(cmdLine, CMD_LINE);
            if (veryVerbose) { T_(); P_(LINE); P(cmdLine); }

            int         argc;
            const char *argv[MAX_ARGS];
            parseCommandLine(cmdLine, argc, argv);

            const int           ARGC = argc;
            const char * const *ARGV = argv;
            LOOP_ASSERT(LINE, RET == usageExample(ARGC, ARGV, veryVerbose));
        }

      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING PRINTUSAGE
        //
        // Concerns:
        //   That 'printUsage' properly formats the given variables.
        //
        // Plan:
        //   Set up a variety of variables and options and verify that
        //   'printUsage' formats them correctly.
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING PRINTUSAGE\n"
                             "==================\n";

        if (veryVerbose) cout << "Complex case\n";
        {
            balb::CommandLineOptionInfo specTable[] = {
              {
                "r|reverse",
                "isReverse",
                "sort in reverse order",
                balb::CommandLineTypeInfo(balb::CommandLineOptionType::BAEA_BOOL)
              },
              {
                "i|insensitivetocase",
                "isCaseInsensitive",
                "be case insensitive while sorting",
                balb::CommandLineTypeInfo(balb::CommandLineOptionType::BAEA_BOOL)
              },
              {
                "u|uniq",
                "isUniq",
                "discard duplicate lines",
                balb::CommandLineTypeInfo(balb::CommandLineOptionType::BAEA_BOOL)
              },
              {
                "a|algorithm",
                "sortAlgo",
                "sorting algorithm",
                balb::CommandLineTypeInfo(
                                      balb::CommandLineOptionType::BAEA_STRING),
                balb::CommandLineOccurrenceInfo(bsl::string("quickSort"))
              },
              {
                "o|outputfile",
                "outputFile",
                "output file",
                balb::CommandLineTypeInfo(
                                      balb::CommandLineOptionType::BAEA_STRING),
                balb::CommandLineOccurrenceInfo::BAEA_REQUIRED
              },
              {
                "",
                "fileList",
                "files to be sorted",
                balb::CommandLineTypeInfo(
                                balb::CommandLineOptionType::BAEA_STRING_ARRAY),
                balb::CommandLineOccurrenceInfo::BAEA_REQUIRED
              }
            };

            balb::CommandLine cmdLine(specTable);

            bsl::stringstream ss;
            cmdLine.printUsage(ss);

            if (veryVerbose) cout << ss.str() << endl;

            const char *EXPECTED = "\n"
"Usage: programName [-r|reverse] [-i|insensitivetocase] [-u|uniq]\n"
"                   [-a|algorithm <sortAlgo>] -o|outputfile <outputFile>\n"
"                   [<fileList>]+\n"
"Where:\n"
"  -r | --reverse                                 sort in reverse order\n"
"  -i | --insensitivetocase                       be case insensitive while\n"
"                                                 sorting\n"
"  -u | --uniq                                    discard duplicate lines\n"
"  -a | --algorithm          <sortAlgo>           sorting algorithm (default:"
"\n"
"                                                 quickSort)\n"
"  -o | --outputfile         <outputFile>         output file\n"
"                            <fileList>           files to be sorted\n";

            LOOP_ASSERT(ss.str(), EXPECTED == ss.str());
        }

        if (veryVerbose) cout << "Medium case\n";
        {
            balb::CommandLineOptionInfo specTable[] = {
              {
                "w|woof",
                "woof",
                "grrowll",
                balb::CommandLineTypeInfo(balb::CommandLineOptionType::BAEA_BOOL)
              },
              {
                "a|arf",
                "arf",
                "arrrrrrrrrrf",
                balb::CommandLineTypeInfo(balb::CommandLineOptionType::BAEA_BOOL)
              },
              {
                "meow",
                "meow",
                "merrrrower",
                balb::CommandLineTypeInfo(balb::CommandLineOptionType::BAEA_BOOL)
              },
              {
                "n|number",
                "number",
                "#",
                balb::CommandLineTypeInfo(balb::CommandLineOptionType::BAEA_INT)
              },
              {
                "s|size",
                "size",
                "size in bytes",
                balb::CommandLineTypeInfo(balb::CommandLineOptionType::BAEA_INT),
                balb::CommandLineOccurrenceInfo::BAEA_REQUIRED
              },
              {
                "m|meters",
                "meters",
                "distance",
                balb::CommandLineTypeInfo(balb::CommandLineOptionType::BAEA_INT)
              },
              {
                "radius",
                "radius",
                "half diameter",
                balb::CommandLineTypeInfo(balb::CommandLineOptionType::BAEA_INT)
              },
            };

            balb::CommandLine cmdLine(specTable);

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
            balb::CommandLineOptionInfo specTable[] = {
              {
                "reverse",
                "isReverse",
                "sort in reverse order",
                balb::CommandLineTypeInfo(balb::CommandLineOptionType::BAEA_BOOL)
              }
            };

            balb::CommandLine cmdLine(specTable);

            bsl::stringstream ss;
            cmdLine.printUsage(ss);

            if (veryVerbose) cout << ss.str() << endl;

            const char *EXPECTED = "\n"
"Usage: programName [--reverse]\n"
"Where:\n"
"       --reverse               sort in reverse order\n";

            LOOP_ASSERT(ss.str(), EXPECTED == ss.str());
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING NON-OPTION TOGGLE '--'
        //
        // Concerns:  The use of '--' on the command line must stop the parsing
        //   of the options and everything afterwards, including strings that
        //   begin with '-...' must be parsed as a non-option value.
        //
        // Plan:  Insert '--' at various places into a command line with the
        //   last non-option being multi-valued, measure the number of
        //   arguments that get assigned to that non-option, and verify that it
        //   is as expected.
        //
        // Testing:
        //   NON-OPTION TOGGLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing non-option toggle." << endl
                          << "==========================" << endl;

        if (verbose) cout << "\n\tTesting non-option toggle." << endl;

        const int MAX_ARGC = 16;

        static const struct {
            int          d_line;
            int          d_specIdx;
            const char  *d_argv[MAX_ARGC];
            int          d_sizeNonOption2;
        } ARGS[] = {
          // line  specIdx  argv                                         #2
          // ----  -------  ----                                         --
            { L_,  0,       { "", "-i=-1", "-f=-1.0", "-s=string",
                                          "--", "#1",             0 },   0 }
          , { L_,  0,       { "", "-i=-1", "-f=-1.0", "-s=string",
                                          "--", "#1", "#2",       0 },   1 }
          , { L_,  0,       { "", "-i=-1", "-f=-1.0", "-s=string",
                                          "--", "#1", "#2", "#3", 0 },   2 }
          , { L_,  0,       { "", "-i=1", "-f=-1.0", "--", "-s=string",
                                                "#1", "#2", "#3", 0 },   3 }
          , { L_,  0,       { "", "-i=1", "--", "-f=-1.0", "-s=string",
                                                "#1", "#2", "#3", 0 },   4 }
          , { L_,  0,       { "", "--", "-i=1", "-f=-1.0", "-s=string",
                                                "#1", "#2", "#3", 0 },   5 }
          , { L_,  0,       { "", "-I=1", "-I=2", "-I=3", "-i=-1",
                                          "#1", "#2", "#3", "--", 0 },   2 }
          , { L_,  0,       { "", "-I=1", "-I=2", "-I=3", "-i=-1",
                                          "#1", "#2", "--", "#3", 0 },   2 }
          , { L_,  0,       { "", "-I=1", "-I=2", "-I=3", "-i=-1",
                                          "#1", "--", "#2", "#3", 0 },   2 }
          , { L_,  0,       { "", "#1", "#2", "#3", "--",
                                  "-i=1", "-f=-1.0", "-s=string", 0 },   5 }
          , { L_,  0,       { "", "#1", "#2", "#3", "--",
                                  "-i=1", "-f=-1.0", "-s=string", 0 },   5 }
          , { L_,  0,       { "", "#1", "#2", "--", "#3",
                                  "-i=1", "-f=-1.0", "-s=string", 0 },   5 }
          , { L_,  0,       { "", "#1", "--", "#2", "#3",
                                  "-i=1", "-f=-1.0", "-s=string", 0 },   5 }
          , { L_,  0,       { "", "--", "#1", "#2", "#3",
                                  "-i=1", "-f=-1.0", "-s=string", 0 },   5 }
        };
        const int NUM_ARGS = sizeof ARGS / sizeof *ARGS;

        for (int i = 0; i < NUM_ARGS; ++i) {
            const int           LINE        = ARGS[i].d_line;
            const int           SPEC_IDX    = ARGS[i].d_specIdx;
            const char * const *ARGV        = ARGS[i].d_argv;
                                              int argc = 0;
                                              while (ARGV[++argc]) ;
            const int           ARGC        = argc;
            const bsl::size_t   SIZE        = ARGS[i].d_sizeNonOption2;

            const int           NUM_SPEC    = SPECS[SPEC_IDX].d_numSpecTable;
            const OptionInfo   *SPEC        = SPECS[SPEC_IDX].d_specTable;

            ASSERT(NUM_SPEC <= MAX_SPEC_SIZE);
            ASSERT(ARGC     <= MAX_ARGC);

            if (veryVerbose) {
                T_(); P_(LINE); P_(SPEC_IDX); P(ARGC);
                T_(); T_(); P(ARGV[0]);
                if (1 < ARGC) { T_(); T_(); P(ARGV[1]); }
                if (2 < ARGC) { T_(); T_(); P(ARGV[2]); }
                if (3 < ARGC) { T_(); T_(); P(ARGV[3]); }
                if (4 < ARGC) { T_(); T_(); P(ARGV[4]); }
                if (5 < ARGC) { T_(); T_(); P(ARGV[5]); }
                if (6 < ARGC) { T_(); T_(); P(ARGV[6]); }
                if (7 < ARGC) { T_(); T_(); P(ARGV[7]); }
                if (8 < ARGC) { T_(); T_(); P(ARGV[8]); }
                if (9 < ARGC) { T_(); T_(); P(ARGV[8]); }
                if (10 < ARGC) { T_(); T_(); P(ARGV[10]); }
                if (11 < ARGC) { T_(); T_(); P(ARGV[11]); }
            }

            bsl::ostringstream oss;
            Obj mX(SPEC, NUM_SPEC, oss);  const Obj& X = mX;

            bsl::ostringstream oss2;
            int parseRet = mX.parse(ARGC, ARGV, oss2);
            LOOP3_ASSERT(LINE, SPEC_IDX, oss2.str(), 0 == parseRet);

            if (veryVerbose) {
                T_(); T_(); P(X);
                if (1 < linkedStringArray1[0].size()) {
                    T_(); T_(); P(linkedStringArray1[0]);
                }
                if (1 < linkedStringArray1[1].size()) {
                    T_(); T_(); P(linkedStringArray1[1]);
                }
                if (2 < linkedStringArray1[1].size()) {
                    T_(); T_(); P(linkedStringArray1[2]);
                }
                if (3 < linkedStringArray1[1].size()) {
                    T_(); T_(); P(linkedStringArray1[3]);
                }
                if (4 < linkedStringArray1[1].size()) {
                    T_(); T_(); P(linkedStringArray1[4]);
                }
            }

            LOOP2_ASSERT(LINE, SPEC_IDX, SIZE == linkedStringArray1.size());
        }

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING PARSING OF STRINGS
        //
        // Concerns: 'balb::CommandLine' must parse a string as raw, and not
        //   interpret escape sequences such as '.\bass' (on Windows) while
        //   replacing '\b' with a backspace.
        //
        // Plan:
        //
        // Testing:
        //   ORDER OF ARGUMENTS
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing parsing of strings." << endl
                          << "===========================" << endl;

        const int MAX_ARGC = 16;

        static const struct {
            int          d_line;
            int          d_specIdx;
            const char  *d_argv[MAX_ARGC];
            const char  *d_expLinkedString;
        } ARGS[] = {
          // line  specIdx  argv                            exp
          // ----  -------  ----                            ---
            { L_,  0,       { "", "-s=..\\bass.cfg", 0 },   "..\\bass.cfg"   },
            { L_,  0,       { "", "-s=\"quotes\"", 0 },     "\"quotes\""     },
            { L_,  0,       { "", "-s=\\\"quotes\\\"", 0 }, "\\\"quotes\\\"" }
        };
        const int NUM_ARGS = sizeof ARGS / sizeof *ARGS;

        for (int i = 0; i < NUM_ARGS; ++i) {
            const int          LINE        = ARGS[i].d_line;
            const int          SPEC_IDX    = ARGS[i].d_specIdx;
            const char *const *ARGV        = ARGS[i].d_argv;
                                             int argc = 0;
                                             while (ARGV[++argc]) ;
            const int          ARGC        = argc;
            const char        *EXP         = ARGS[i].d_expLinkedString;

            const int          NUM_SPEC    = SPECS[SPEC_IDX].d_numSpecTable;
            const OptionInfo  *SPEC        = SPECS[SPEC_IDX].d_specTable;

            bsl::ostringstream oss;
            Obj mX(SPEC, NUM_SPEC, oss);  const Obj& X = mX;

            bsl::ostringstream oss2;
            int parseRet = mX.parse(ARGC, ARGV, oss2);
            LOOP3_ASSERT(LINE, SPEC_IDX, oss2.str(), 0 == parseRet);

            if (veryVerbose) {
                T_(); T_(); P(X);
                T_(); T_(); P_(linkedString); P(EXP);
            }

            LOOP2_ASSERT(EXP, linkedString, EXP == linkedString);
        }

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING ORDER OF ARGUMENTS
        //
        // Concerns: 'balb::CommandLine' must have the same value no matter what
        //   the order of the arguments is, except for multi-valued and for the
        //   '--' non-option toggle.
        //
        // Plan:  Using a helper function that checks if two command lines have
        // compatible orderings, permute
        //
        // Testing:
        //   ORDER OF ARGUMENTS
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing order of arguments." << endl
                          << "===========================" << endl;

        const int MAX_ARGC = 16;

        if (verbose)
            cout << "\n\tTesting 'isCompatibleOrdering' helper." << endl;

        static const struct {
            int          d_line;
            bool         d_isCompatible;
            const char  *d_argv[MAX_ARGC];
        } DUMMY_ARGS[] = {
          // line  isCompat   argv
          // ----  --------   ----
            { L_,  true,      { "", "-i=-1", "-f=-1.0", "-s=string",
                                          "#1", "#2", "#3", 0 } }  // reference
          , { L_,  true,      { "", "-s=string", "-i=-1", "-f=-1.0",
                                                        "#1", "#2", "#3", 0 } }
          , { L_,  true,      { "", "-i=-1", "-s=string", "-f=-1.0",
                                                        "#1", "#2", "#3", 0 } }
          , { L_,  true,      { "", "#1", "#2", "#3",
                                         "-i=-1", "-f=-1.0", "-s=string", 0 } }
          , { L_,  true,      { "", "#1", "-i=-1",  "#2",
                                            "-f=-1.0", "#3", "-s=string", 0 } }
          , { L_,  false,     { "", "-i=-1", "-f=-1.0", "-s=string",
                                                        "#1", "#3", "#2", 0 } }
          , { L_,  false,     { "", "-i=-1", "-f=-1.0", "-s=string",
                                                        "#2", "#1", "#3", 0 } }
          , { L_,  false,     { "", "-i=-1", "-f=-1.0", "-s=string",
                                                        "#3", "#1", "#2", 0 } }
        };
        const int NUM_DUMMY_ARGS = sizeof DUMMY_ARGS / sizeof *DUMMY_ARGS;

        // Compare every line above to the first one.

        const char *const *ARGV_REFERENCE = DUMMY_ARGS[0].d_argv;
                                            int argc0 = 0;
                                            while (ARGV_REFERENCE[++argc0]) ;
        const int          ARGC_REFERENCE = argc0;

        for (int i = 0; i < NUM_DUMMY_ARGS; ++i) {
            const int          LINE          = DUMMY_ARGS[i].d_line;
            const int          IS_COMPATIBLE = DUMMY_ARGS[i].d_isCompatible;
            const char *const *ARGV          = DUMMY_ARGS[i].d_argv;
                                               int argc = 0;
                                               while (ARGV[++argc]) ;
            const int          ARGC          = argc;

            if (veryVerbose) {
                T_(); P_(LINE); P(IS_COMPATIBLE);
                T_(); T_(); P(ARGV[0]);
                if (1 < ARGC) { T_(); T_(); P(ARGV[1]); }
                if (2 < ARGC) { T_(); T_(); P(ARGV[2]); }
                if (3 < ARGC) { T_(); T_(); P(ARGV[3]); }
                if (4 < ARGC) { T_(); T_(); P(ARGV[4]); }
                if (5 < ARGC) { T_(); T_(); P(ARGV[5]); }
                if (6 < ARGC) { T_(); T_(); P(ARGV[6]); }
                if (7 < ARGC) { T_(); T_(); P(ARGV[7]); }
                if (8 < ARGC) { T_(); T_(); P(ARGV[8]); }
                if (9 < ARGC) { T_(); T_(); P(ARGV[8]); }
                if (10 < ARGC) { T_(); T_(); P(ARGV[10]); }
                if (11 < ARGC) { T_(); T_(); P(ARGV[11]); }
            }

            LOOP_ASSERT(LINE, ARGC_REFERENCE == ARGC);
            LOOP_ASSERT(LINE,
                        IS_COMPATIBLE == isCompatibleOrdering(ARGV,
                                                              ARGV_REFERENCE,
                                                              ARGC));
        }

        if (verbose) cout << "\n\tTesting order of arguments." << endl;

        static const struct {
            int          d_line;
            int          d_specIdx;
            const char  *d_argv[MAX_ARGC];
        } ARGS[] = {
          // line  specIdx  argv
          // ----  -------  ----
            { L_,  0,       { "", "-i=-1", "-f=-1.0", "-s=string",
                                                       "#1", "#2", "#3", 0 } }
          , { L_,  0,       { "", "-I=1", "-I=2", "-I=3",
                                                       "#1", "#2", "#3", 0 } }
          , { L_,  0,       { "", "-I=1", "-I=2", "-I=3", "-i=-1",
                                                       "#1", "#2", "#3", 0 } }
          , { L_,  0,       { "", "-F=1.0", "-F=2.0", "-F=3.0",
                                                       "#1", "#2", "#3", 0 } }
          , { L_,  0,       { "", "-F=1.0", "-F=2.0", "-F=3.0", "-f=-1.0",
                                                       "#1", "#2", "#3", 0 } }
          , { L_,  0,       { "", "-S=s1", "-S=s2", "-S=s3",
                                                       "#1", "#2", "#3", 0 } }
          , { L_,  0,       { "", "-S=s1", "-S=s2", "-S=s3", "-s=string",
                                                       "#1", "#2", "#3", 0 } }
        };
        const int NUM_ARGS = sizeof ARGS / sizeof *ARGS;

        for (int i = 0; i < NUM_ARGS; ++i) {
            const int          LINE        = ARGS[i].d_line;
            const int          SPEC_IDX    = ARGS[i].d_specIdx;
            const char *const *ARGV        = ARGS[i].d_argv;
                                             int argc = 0;
                                             while (ARGV[++argc]) ;
            const int          ARGC        = argc;

            const int          NUM_SPEC    = SPECS[SPEC_IDX].d_numSpecTable;
            const OptionInfo  *SPEC        = SPECS[SPEC_IDX].d_specTable;

            ASSERT(NUM_SPEC <= MAX_SPEC_SIZE);
            ASSERT(ARGC     <= MAX_ARGC);

            if (veryVerbose) {
                T_(); P_(LINE); P_(SPEC_IDX); P(ARGC);
                T_(); T_(); P(ARGV[0]);
                if (1 < ARGC) { T_(); T_(); P(ARGV[1]); }
                if (2 < ARGC) { T_(); T_(); P(ARGV[2]); }
                if (3 < ARGC) { T_(); T_(); P(ARGV[3]); }
                if (4 < ARGC) { T_(); T_(); P(ARGV[4]); }
                if (5 < ARGC) { T_(); T_(); P(ARGV[5]); }
                if (6 < ARGC) { T_(); T_(); P(ARGV[6]); }
                if (7 < ARGC) { T_(); T_(); P(ARGV[7]); }
                if (8 < ARGC) { T_(); T_(); P(ARGV[8]); }
                if (9 < ARGC) { T_(); T_(); P(ARGV[8]); }
                if (10 < ARGC) { T_(); T_(); P(ARGV[10]); }
                if (11 < ARGC) { T_(); T_(); P(ARGV[11]); }
            }

            bsl::ostringstream oss;
            Obj mX(SPEC, NUM_SPEC, oss);  const Obj& X = mX;

            bsl::ostringstream oss2;
            int parseRet = mX.parse(ARGC, ARGV, oss2);
            LOOP3_ASSERT(LINE, SPEC_IDX, oss2.str(), 0 == parseRet);

            if (veryVerbose) {
                T_(); T_(); P(X);
            }

            const char *argv[MAX_ARGC];
            bsl::copy(ARGV, ARGV + ARGC, (const char **)argv);

            // The array of pointers must be sorted, because otherwise, the
            // loop below might not examine all permutations.  For most
            // compilers, that is how the string statics are laid out, but
            // there is no reason that it be so.

            bsl::sort(argv + 1, argv + ARGC);

            int iterations = 0, compatibleIterations = 0;
            do {
                if (isCompatibleOrdering(argv, ARGV, ARGC)) {
                    bsl::ostringstream oss;
                    Obj mY(SPEC, NUM_SPEC, oss);  const Obj& Y = mY;

                    bsl::ostringstream oss2;
                    int parseRet = mY.parse(ARGC, argv, oss2);
                        // note: argv, not ARGV

                    LOOP2_ASSERT(LINE, SPEC_IDX, 0 == parseRet);
                    LOOP2_ASSERT(LINE, SPEC_IDX, X == Y);  // TEST HERE

                    ++compatibleIterations;
                }
                ++iterations;
            } while (bsl::next_permutation(argv + 1, argv + ARGC));

            if (veryVerbose) {
                T_(); T_(); P_(iterations); P(compatibleIterations);
            }
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING MULTIPLY-SPECIFIED FLAGS
        //
        // Concerns: 'balb::CommandLine' must accept multiply-specified flags.
        //
        // Plan:  Define a variety of arguments with multiply-specified flags
        //   mixed in with another options.  Make sure the 'numSpecified' call
        //   returns the proper number of occurrences.
        //
        // Testing:
        //   MULTIPLY-SPECIFIED FLAGS
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing multiply-specified flags." << endl
                          << "=================================" << endl;

        const int MAX_ARGC = 5;
        static const struct {
            int          d_line;
            int          d_specIdx;
            int          d_argc;
            const char  *d_argv[MAX_ARGC];
            const int    d_expNumSpecified[MAX_SPEC_SIZE];
            const int    d_expLinkedInt;
        } ARGS[] = {
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
        const int NUM_ARGS = sizeof ARGS / sizeof *ARGS;

        for (int i = 0; i < NUM_ARGS; ++i) {
            const int          LINE           = ARGS[i].d_line;
            const int          SPEC_IDX       = ARGS[i].d_specIdx;
            const int          ARGC           = ARGS[i].d_argc;
            const char *const *ARGV           = ARGS[i].d_argv;
            const int         *EXP            = ARGS[i].d_expNumSpecified;
            const int          EXP_LINKED_INT = ARGS[i].d_expLinkedInt;

            const int          NUM_SPEC       = SPECS[SPEC_IDX].d_numSpecTable;
            const OptionInfo  *SPEC           = SPECS[SPEC_IDX].d_specTable;

            ASSERT(NUM_SPEC  <= MAX_SPEC_SIZE);
            ASSERT(ARGC      <= MAX_ARGC);

            if (verbose) {
                T_(); P_(LINE); P_(SPEC_IDX); P_(ARGC); P(EXP_LINKED_INT);
                T_(); T_(); P(ARGV[0]);
                if (1 < ARGC) { T_(); T_(); P(ARGV[1]); }
                if (2 < ARGC) { T_(); T_(); P(ARGV[2]); }
                if (3 < ARGC) { T_(); T_(); P(ARGV[3]); }
                if (0 < NUM_SPEC) { T_(); T_(); P(EXP[0]); }
                if (1 < NUM_SPEC) { T_(); T_(); P(EXP[1]); }
                if (2 < NUM_SPEC) { T_(); T_(); P(EXP[2]); }
                if (3 < NUM_SPEC) { T_(); T_(); P(EXP[3]); }
            }

            linkedInt = -1;
            bsl::ostringstream oss;
            Obj mX(SPEC, NUM_SPEC, oss);  const Obj& X = mX;

            bsl::ostringstream oss2;
            int parseRet = mX.parse(ARGC, ARGV, oss2);
            LOOP2_ASSERT(LINE, SPEC_IDX, 0 == parseRet);

            if (veryVerbose) {
                T_(); T_(); P(X);
            }
            for (int k = 0; k < NUM_SPEC; ++k) {
                LOOP3_ASSERT(LINE, SPEC_IDX, k,
                             EXP[k] == X.numSpecified(SPEC[k].d_name));
            }
            LOOP2_ASSERT(LINE, SPEC_IDX, EXP_LINKED_INT == linkedInt);
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING FLAGS BUNDLING
        //
        // Concerns: 'balb::CommandLine' must be able to parse bundled flags.
        //
        // Plan:  Define a variety of arguments following some option
        //   specifications, and verify that they can be parsed successfully
        //   and that any linked variable specified as the last argument of the
        //   bundle (either in a separate argument or attached with "=value" to
        //   the bundle) is set properly.
        //
        // Testing:
        //   FLAGS BUNDLING
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing flags bundling." << endl
                          << "=======================" << endl;

        if (verbose) cout << "\tTesting bundled flags." << endl;

        const int MAX_ARGC = 4;
        static const struct {
            int          d_line;
            int          d_specIdx;
            int          d_argc;
            const char  *d_argv[MAX_ARGC];
            const int    d_expNumSpecified[MAX_SPEC_SIZE];
            const int    d_expLinkedInt;
        } ARGS[] = {
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
        const int NUM_ARGS = sizeof ARGS / sizeof *ARGS;

        for (int i = 0; i < NUM_ARGS; ++i) {
            const int          LINE           = ARGS[i].d_line;
            const int          SPEC_IDX       = ARGS[i].d_specIdx;
            const int          ARGC           = ARGS[i].d_argc;
            const char *const *ARGV           = ARGS[i].d_argv;
            const int         *EXP            = ARGS[i].d_expNumSpecified;
            const int          EXP_LINKED_INT = ARGS[i].d_expLinkedInt;

            const int          NUM_SPEC       = SPECS[SPEC_IDX].d_numSpecTable;
            const OptionInfo  *SPEC           = SPECS[SPEC_IDX].d_specTable;

            ASSERT(NUM_SPEC  <= MAX_SPEC_SIZE);
            ASSERT(ARGC      <= MAX_ARGC);

            if (verbose) {
                T_(); P_(LINE); P_(SPEC_IDX); P_(ARGC); P(EXP_LINKED_INT);
                T_(); T_(); P(ARGV[0]);
                if (1 < ARGC) { T_(); T_(); P(ARGV[1]); }
                if (2 < ARGC) { T_(); T_(); P(ARGV[2]); }
                if (3 < ARGC) { T_(); T_(); P(ARGV[3]); }
                if (0 < NUM_SPEC) { T_(); T_(); P(EXP[0]); }
                if (1 < NUM_SPEC) { T_(); T_(); P(EXP[1]); }
                if (2 < NUM_SPEC) { T_(); T_(); P(EXP[2]); }
                if (3 < NUM_SPEC) { T_(); T_(); P(EXP[3]); }
            }

            linkedInt = -1;
            bsl::ostringstream oss;
            Obj mX(SPEC, NUM_SPEC, oss);  const Obj& X = mX;

            bsl::ostringstream oss2;
            int parseRet = mX.parse(ARGC, ARGV, oss2);
            LOOP2_ASSERT(LINE, SPEC_IDX, 0 == parseRet);

            if (veryVerbose) {
                T_(); T_(); P(X);
            }
            for (int k = 0; k < NUM_SPEC; ++k) {
                LOOP3_ASSERT(LINE, SPEC_IDX, k,
                             EXP[k] == X.numSpecified(SPEC[k].d_name));
            }
            LOOP2_ASSERT(LINE, SPEC_IDX, EXP_LINKED_INT == linkedInt);
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING INVALID OPTION SPECS
        //
        // Concerns: 'balb::CommandLine' must be able to correctly reject
        //   invalid option specs.
        //
        // Plan:  Draft a table of invalid specs and verify that the component
        //   asserts, by redirecting the assert handler to a local helper which
        //   throws an exception which we can catch in the code below.  Note
        //   that it is especially interesting to run this test case in very
        //   verbose mode, and to check the appropriateness of the error
        //   messages in the 'oss.str()' output.
        //
        // Testing:
        //   INVALID OPTION SPECS
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing invalid option specs." << endl
                          << "=============================" << endl;

        bsls::Assert::setFailureHandler(&throwInvalidSpec);

        if (verbose) cout << "\tTesting invalid tags." << endl;

        static const struct {
            int         d_line;
            bool        d_isValid;
            const char *d_tag;
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
        const int NUM_TAGS = sizeof TAGS / sizeof *TAGS;

        for (int i = 0; i < NUM_TAGS; ++i) {
            const int   LINE     = TAGS[i].d_line;
            const bool  IS_VALID = TAGS[i].d_isValid;
            const char *TAG      = TAGS[i].d_tag;

            bsl::stringstream oss; oss << endl; // for cleaner presentation
            bool exceptionCaught = false;

            OptionInfo SPEC[] = {
              {
                TAG,                 // non-option
                "SomeValidName",     // name
                "Some description",  // description
                createTypeInfo(ET::BDEM_BOOL),
                OccurrenceInfo::BAEA_OPTIONAL
              }
            };

            if (verbose) { T_(); T_(); P_(LINE); P_(IS_VALID); P(TAG); }

            try {
                Obj mX(SPEC, 1, oss);
            }
            catch (const InvalidSpec& e) {
                if (veryVerbose) { T_(); T_(); P(oss.str()); }
                exceptionCaught = true;
            }

            LOOP_ASSERT(LINE, !IS_VALID == exceptionCaught);
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
                              createTypeInfo(ET::BDEM_BOOL),
                              OccurrenceInfo::BAEA_OPTIONAL
                          }  // Invalid name
                      }
            },
            { L_, 1, {
                          {
                              "",                              // non-option
                              "SomeName",                      // name
                              "",                              // description
                              createTypeInfo(ET::BDEM_BOOL),
                              OccurrenceInfo::BAEA_OPTIONAL
                          }  // Invalid description
                      }
            },
            { L_, 1, {
                          {
                              "",                              // non-option
                              "SomeName",                      // name
                              "Some description",              // description
                              createTypeInfo(ET::BDEM_BOOL),
                              OccurrenceInfo::BAEA_OPTIONAL
                          }  // Non-options cannot be of type 'bool'.
                      }
            },
            { L_, 1, {
                          {
                              "",                                // non-option
                              "SomeName",                        // name
                              "Some description",                // description
                              createTypeInfo(ET::BDEM_CHAR),
                              createOccurrenceInfo(
                                                 OccurrenceInfo::BAEA_OPTIONAL,
                                                 ET::BDEM_INT,
                                                 &defaultInt)
                          }  // Type of default value does not match type info.
                      }
            },
            { L_, 2, {
                          {
                              "s|long1",                         // non-option
                              "SomeName",                        // name
                              "Some description",                // description
                              createTypeInfo(ET::BDEM_BOOL),
                              OccurrenceInfo::BAEA_OPTIONAL
                          },
                          {
                              "s|long2",                         // non-option
                              "SomeOtherName",                   // name
                              "Some description",                // description
                              createTypeInfo(ET::BDEM_BOOL),
                              OccurrenceInfo::BAEA_OPTIONAL
                          }
                      }  // Short tags must be unique.
            },
            { L_, 2, {
                          {
                              "a|long",                          // non-option
                              "SomeName",                        // name
                              "Some description",                // description
                              createTypeInfo(ET::BDEM_BOOL),
                              OccurrenceInfo::BAEA_OPTIONAL
                          },
                          {
                              "b|long",                          // non-option
                              "SomeOtherName",                   // name
                              "Some description",                // description
                              createTypeInfo(ET::BDEM_BOOL),
                              OccurrenceInfo::BAEA_OPTIONAL
                          }
                      }  // Long tags must be unique.
            },
            { L_, 2, {
                          {
                              "",                                // non-option
                              "SomeCommonName",                  // name
                              "Some description",                // description
                              createTypeInfo(ET::BDEM_BOOL),
                              OccurrenceInfo::BAEA_OPTIONAL
                          },
                          {
                              "",                                // non-option
                              "SomeCommonName",                  // name
                              "Some description",                // description
                              createTypeInfo(ET::BDEM_BOOL),
                              OccurrenceInfo::BAEA_OPTIONAL
                          }
                      }  // Names must be unique.
            },
            { L_, 2, {
                          {
                              "",                                // non-option
                              "SomeCommonName",                  // name
                              "Some description",                // description
                              createTypeInfo(ET::BDEM_INT),
                              createOccurrenceInfo(
                                                 OccurrenceInfo::BAEA_OPTIONAL,
                                                 ET::BDEM_INT,
                                                 &defaultInt)
                          },
                          {
                              "",                                // non-option
                              "SomeCommonName",                  // name
                              "Some description",                // description
                              createTypeInfo(ET::BDEM_INT),
                              OccurrenceInfo::BAEA_REQUIRED
                          }
                      }  // Defaulted non-option cannot be followed by
                         // required non-options.
            },
            { L_, 2, {
                          {
                              "",                                // non-option
                              "SomeCommonName",                  // name
                              "Some description",                // description
                              createTypeInfo(ET::BDEM_INT_ARRAY),
                              OccurrenceInfo::BAEA_REQUIRED
                          },
                          {
                              "",                                // non-option
                              "SomeCommonName",                  // name
                              "Some description",                // description
                              createTypeInfo(ET::BDEM_INT),
                              OccurrenceInfo::BAEA_REQUIRED
                          }
                      }  // Array non-options cannot be followed by other
                         // non-options.
            }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int         LINE      = DATA[i].d_line;
            const int         NUM_SPEC  = DATA[i].d_numSpecs;
            const OptionInfo *SPEC      = DATA[i].d_specTable;

            LOOP_ASSERT(LINE, NUM_SPEC <= MAX_SPEC_SIZE);

            if (veryVerbose) {
                T_(); P_(LINE); P_(NUM_SPECS);
                if (0 < NUM_SPEC) { T_(); T_(); P(SPEC[0]); }
                if (1 < NUM_SPEC) { T_(); T_(); P(SPEC[1]); }
                if (2 < NUM_SPEC) { T_(); T_(); P(SPEC[2]); }
                if (3 < NUM_SPEC) { T_(); T_(); P(SPEC[3]); }
            }

            bsl::stringstream oss; oss << endl; // for cleaner presentation
            bool exceptionCaught = false;

            try {
                Obj mX(SPEC, NUM_SPEC, oss);
            }
            catch (const InvalidSpec& e) {
                if (veryVerbose) { T_(); T_(); P(oss.str()); }
                exceptionCaught = true;
            }

            LOOP_ASSERT(LINE, exceptionCaught);
        }

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING balb::CommandLine
        //
        // Concerns: 'balb::CommandLine' is an in-core value-semantic type,
        //   version of 'balb::CommandLineOptionInfo', which can be initialized
        //   and converted to its simpler 'struct' variant.  We need to make
        //   sure it is exception safe, uses 'bdema' allocators properly so it
        //   can be stored in a container, and has a value as expected.  Also,
        //   this object has an unset value until it is "parsed", then has a
        //   "proper" value.  As such, we test each value in valid states only
        //   and parse with a matching collection of argument values
        //   For the primary accessors, we choose:
        //     - balb::CommandLine(const balb::CommandLineOptionInfo *specTable,
        //                        int size, alloc = 0);
        //     - int parse(int argc, const char * const argv[]);
        //   For the basic accessors, we choose:
        //     - bdem_ConstRowBinding data() const;
        //     - bool isParsed() const;
        //     - bool isInvalid() const;
        //
        // Plan:  Follow the standard value-semantic test driver sequence.  In
        //   order to generate data, we leverage the 'OptionInfo' values from
        //   case 7, and append them  into an array which we feed to
        //   'CommandLine'.  Note that all the 'OptionInfo' are valid (i.e.,
        //   they can be used to initialize a 'CommandLine' object) except for
        //   the empty name and empty description.  Note however that the data
        //   gets generated in a certain order, whereas a certain variety is
        //   desirable (e.g., option + non-option, in either order).  For this,
        //   we pre-shuffle the array in a random fashion and record the
        //   patterns to make sure we have all desirable patterns.
        //
        // Testing:
        //   class balb::CommandLineOption;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "balb::CommandLineOption" << endl
                          << "======================" << endl;

        typedef balb::CommandLine Obj;

        bslma::TestAllocator testAllocator(veryVeryVerbose);

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

        for (int i = 0; i < 4; ++i) {
        for (int l = 0; l < NUM_OPTION_TYPEINFO; ++l) {
        for (int m = 0; m < NUM_OPTION_OCCURRENCES - 1; ++m) {
            const int n = l % NUM_OPTION_DEFAULT_VALUES;
                // Instead of:
                //   for (int n = 0; n < NUM_OPTION_DEFAULT_VALUES; ++n)
                // Valid only because of the way we organized the table data:
                //..
                   ASSERT(OPTION_TYPEINFO[l].d_type ==
                                              OPTION_DEFAULT_VALUES[n].d_type);
                //..
            const char     *TAG        = OPTION_TAGS[i].d_tag;
            const char     *NAME       = "SOME UNIQUE NAME";
            const char     *DESC       = "SOME VERY LONG DESCRIPTION...";
            const BdemType  TYPE       = OPTION_TYPEINFO[l].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[l].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[l].d_constraint_p;

            const OccurrenceType OTYPE    = OPTION_OCCURRENCES[l].d_type;
            const void     *DEFAULT_VALUE = OPTION_DEFAULT_VALUES[n].d_value;

            if (TYPE != OPTION_DEFAULT_VALUES[n].d_type) {
                continue;
            }

            const TypeInfo       TYPE_INFO =
                                    createTypeInfo(TYPE, VARIABLE, CONSTRAINT);
            const OccurrenceInfo OCCURRENCE_INFO =
                              createOccurrenceInfo(OTYPE, TYPE, DEFAULT_VALUE);

            const balb::CommandLineOptionInfo OPTION_INFO = {
                TAG,
                bsl::string(NAME) + uniqueIdent,
                DESC,
                TYPE_INFO,
                OCCURRENCE_INFO
            };

            if (ET::BDEM_BOOL == TYPE_INFO.type()
             && OPTION_INFO.d_tag.empty()) {
                // Flags cannot have an empty tag, or in other words
                // non-options cannot be of type 'bool'.  Skip this.

                continue;
            }

            if (OccurrenceInfo::BAEA_HIDDEN == OCCURRENCE_INFO.occurrenceType()
             && OPTION_INFO.d_tag.empty()) {
                // A non-option cannot be hidden.  Skip this.

                continue;
            }

            // Generate the (table) option info.

            options.push_back(OPTION_INFO);

            // Generate a matching command line argument for parsing.

            bsl::string arg;  const bsl::string& ARG = arg;

            generateArgument(&arg, OPTION_INFO, i);
            arguments.push_back(ARG);

            // Guarantee unique names.

            for (int k = 1; ++(uniqueIdent[k]) > 'Z'; ++k) {
                uniqueIdent[k] = 'A';
            }

        }
        }
        }
        const int NUM_OPTIONS = options.size();

        if (verbose) cout << "\n\tBuilt " << NUM_OPTIONS << " options."
                          << endl;

        int limit;
        bool arrayNonOption = false;
        bool defaultNonOption = false;

        // Add second layer (for 2-option specs).

        for (int i = 0; i < NUM_OPTIONS; ++i) {
            limit = 0;
            arrayNonOption = options[i].d_tag.empty() &&
                                 ET::isArrayType(options[i].d_typeInfo.type());
            defaultNonOption = options[i].d_tag.empty() &&
                                    options[i].d_defaultInfo.hasDefaultValue();

            int j = ((i + 37) * (i + 101)) % NUM_OPTIONS;
            while ((options[i].d_tag[0] == options[j].d_tag[0] &&
                                                     !options[j].d_tag.empty())
                || (arrayNonOption && options[j].d_tag.empty())
                || (defaultNonOption && options[j].d_tag.empty() &&
                                !options[j].d_defaultInfo.hasDefaultValue())) {
                // Tags (long and short, if not empty) must be distinct; if
                // there is a previous non-option of array type then this
                // cannot be a non-option; and if a previous non-option had a
                // default value, then this non-option must have a default
                // value.

                j = (j + 1) % NUM_OPTIONS;
                if (++limit == NUM_OPTIONS) {
                    break;
                }
            }
            if (limit == NUM_OPTIONS) {
                break;
            }
            options.push_back(options[j]);
            arguments.push_back(arguments[j]);
        }
        if (limit == NUM_OPTIONS) {
            ASSERT(0 && "***CRITICAL ERROR***  Infinite loop in data.");
            break;
        }

        if (verbose) cout << "\tAdded another (shuffled) "
                          << NUM_OPTIONS << " options." << endl;

        // Add third layer (for 3-option specs).

        for (int i = 0, j = NUM_OPTIONS; i < NUM_OPTIONS; ++i, ++j) {
            limit = 0;
            arrayNonOption = (options[i].d_tag.empty() &&
                                ET::isArrayType(options[i].d_typeInfo.type()))
                          || (options[j].d_tag.empty() &&
                                ET::isArrayType(options[j].d_typeInfo.type()));
            defaultNonOption = (options[i].d_tag.empty() &&
                                   options[i].d_defaultInfo.hasDefaultValue())
                            || (options[j].d_tag.empty() &&
                                   options[j].d_defaultInfo.hasDefaultValue());

            int k = ((i + 107) * (i + 293)) % NUM_OPTIONS;
            while (options[i].d_name == options[k].d_name
                || options[j].d_name == options[k].d_name
                || (options[i].d_tag[0] == options[k].d_tag[0] &&
                                                     !options[k].d_tag.empty())
                || (options[j].d_tag[0] == options[k].d_tag[0] &&
                                                     !options[k].d_tag.empty())
                || (arrayNonOption && options[k].d_tag.empty())
                || (defaultNonOption && options[k].d_tag.empty() &&
                                !options[k].d_defaultInfo.hasDefaultValue())) {
                // Names and tags must be distinct, if there is a previous
                // non-option of array type then this cannot be a non-option,
                // and if a previous non-option had a default value, then this
                // non-option must have a default value.

                k = (k + 1) % NUM_OPTIONS;
                if (++limit == NUM_OPTIONS) {
                    break;
                }
            }
            if (limit == NUM_OPTIONS) {
                break;
            }
            options.push_back(options[k]);
            arguments.push_back(arguments[k]);
        }
        if (limit == NUM_OPTIONS) {
            ASSERT(0 && "***CRITICAL ERROR***  Infinite loop in data.");
            break;
        }

        if (verbose) cout << "\tAdded another (shuffled) "
                          << NUM_OPTIONS << " options." << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 2 OF VALUE-SEMANTIC TEST DRIVER
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP)

        if (verbose) cout << "\n\tTesting primary manipulators." << endl;

        if (verbose) cout << "\t\tWithout passing in an allocator." << endl;

        for (int n = 0; n < 4; ++n) {
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
                    ASSERT(0 == parseCommandLine(&(argStrings.back()[0]),
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
                T_(); T_(); P_(i); P(n);
                for (int j = 0; j < n; ++j) {
                    T_(); T_(); T_(); P_(j); P(SPEC_TABLE[j]);
                }
                T_(); T_(); P_(i); P_(n); P(argString);
                T_(); T_(); P_(i); P_(n); P(argPtrs.size());
                for (int k = 0; k < (int)argPtrs.size(); ++k) {
                    T_(); T_(); T_(); P_(k); P(argPtrs[k]);
                }
            }

            Obj mX(SPEC_TABLE, n);  const Obj& X = mX;  // TEST HERE

            if (veryVerbose) { T_(); T_(); P_(i); P_(n); P(X); }
            LOOP2_ASSERT(n, i, !X.isParsed());

            bsl::ostringstream oss;
            int parseRet = mX.parse(argPtrs.size(), &argPtrs[0], oss);
            LOOP3_ASSERT(n, i, oss.str(), 0 == parseRet);

            if (veryVerbose) { T_(); T_(); P_(i); P_(n); P(X); }
            LOOP2_ASSERT(n, i, X.isParsed());
        }
        }

        bslma::TestAllocator defaultAllocator(veryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&defaultAllocator);

        if (verbose) cout << "\t\tPassing in an allocator." << endl
                           << "\t\t\tWith no exceptions." << endl;

        const bool HAS_BSLMA_ALLOCATOR_TRAIT =
             bslalg::HasTrait<Obj, bslalg::TypeTraitUsesBslmaAllocator>::VALUE;

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
                        ASSERT(0 == parseCommandLine(&(argStrings.back()[0]),
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
                    T_(); T_(); P_(i); P(n);
                    for (int j = 0; j < n; ++j) {
                        T_(); T_(); T_(); P_(j); P(SPEC_TABLE[j]);
                    }
                    T_(); T_(); P_(i); P_(n); P(argString);
                    T_(); T_(); P_(i); P_(n); P(argPtrs.size());
                    for (int k = 0; k < (int)argPtrs.size(); ++k) {
                        T_(); T_(); T_(); P_(k); P(argPtrs[k]);
                    }
                }

                {
                    Obj mX(SPEC_TABLE, n, &testAllocator);  // TEST HERE
                    const Obj& X = mX;

                    if (veryVerbose) { T_(); T_(); P_(i); P_(n); P(X); }
                    LOOP2_ASSERT(n, i, !X.isParsed());

                    bsl::ostringstream oss;
                    int parseRet = mX.parse(argPtrs.size(), &argPtrs[0], oss);
                    LOOP3_ASSERT(n, i, oss.str(), 0 == parseRet);

                    if (veryVerbose) { T_(); T_(); P_(i); P_(n); P(X); }
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
            // allocation patterns which are not much affected by the data.

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
                    ASSERT(0 == parseCommandLine(&(argStrings.back()[0]),
                                                 argc,
                                                 argv,
                                                 MAX_ARGC));

                    for (int k = 0; k < argc; ++k) {
                        argPtrs.push_back(argv[k]);
                    }
                }
            }

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {

                Obj mX(SPEC_TABLE, n, &testAllocator);  // TEST HERE
                const Obj& X = mX;

                if (veryVerbose) { T_(); T_(); P_(i); P_(n); P(X); }
                LOOP2_ASSERT(n, i, !X.isParsed());

                bsl::ostringstream oss;
                int parseRet = mX.parse(argPtrs.size(), &argPtrs[0], oss);
                LOOP3_ASSERT(n, i, oss.str(), 0 == parseRet);

                if (veryVerbose) { T_(); T_(); P_(i); P_(n); P(X); }
                LOOP2_ASSERT(n, i, X.isParsed());

            }  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(0 == testAllocator.numMismatches());
            ASSERT(0 == testAllocator.numBytesInUse());
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
                        ASSERT(0 == parseCommandLine(&(argStrings.back()[0]),
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

                {
                    Obj mX(SPEC_TABLE, n);  const Obj& X = mX;

                    if (veryVerbose) { T_(); T_(); P_(i); P_(n); P(X); }

                    ASSERT(!X.isParsed());   // TEST HERE
                    ASSERT(!X.isInvalid());  // TEST HERE

                    bsl::ostringstream oss;
                    int parseRet = mX.parse(argPtrs.size(), &argPtrs[0], oss);
                    LOOP_ASSERT(oss.str(), 0 == parseRet);

                    if (veryVerbose) { T_(); T_(); P_(i); P_(n); P(X); }

                    ASSERT(X.isParsed());    // TEST HERE
                    ASSERT(!X.isInvalid());  // TEST HERE

                    bdem_ConstRowBinding rowBinding(X.data());  // TEST HERE
                }

                argStrings.push_back("--some-invalid-option");
                argPtrs.push_back(&(argStrings.back()[0]));

                {
                    Obj mX(SPEC_TABLE, n);  const Obj& X = mX;

                    if (veryVerbose) { T_(); T_(); P_(i); P_(n); P(X); }

                    ASSERT(!X.isParsed());   // TEST HERE
                    ASSERT(!X.isInvalid());  // TEST HERE

                    bsl::ostringstream oss;
                    int parseRet = mX.parse(argPtrs.size(), &argPtrs[0], oss);
                    LOOP_ASSERT(oss.str(), 0 != parseRet);

                    if (veryVerbose) { T_(); T_(); P_(i); P_(n); P(X); }

                    ASSERT(!X.isParsed());    // TEST HERE
                    ASSERT(X.isInvalid());  // TEST HERE
                }
            }
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 5 OF VALUE-SEMANTIC TEST DRIVER
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
                        ASSERT(0 == parseCommandLine(&(argStrings.back()[0]),
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

                if (veryVerbose) { T_(); T_(); P_(i); P_(n); P(X); }
                LOOP2_ASSERT(n, i, !X.isParsed());

                bsl::ostringstream oss1, oss2;
                X.print(oss1);
                oss2 << X;
                LOOP2_ASSERT(n, i, oss1.str() == oss2.str());
                LOOP2_ASSERT(n, i, "UNPARSED" == oss2.str());

                bsl::ostringstream oss;
                int parseRet = mX.parse(argPtrs.size(), &argPtrs[0], oss);
                LOOP3_ASSERT(n, i, oss.str(), 0 == parseRet);

                if (veryVerbose) { T_(); T_(); P_(i); P_(n); P(X); }
                LOOP2_ASSERT(n, i, X.isParsed());

                bsl::ostringstream oss3, oss4;
                X.print(oss3);
                oss4 << X;
                LOOP2_ASSERT(n, i, oss3.str() == oss4.str());
            }
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 6 OF VALUE-SEMANTIC TEST DRIVER
        // TESTING EQUALITY OPERATOR
        // We do not test the entire cross-product 'W x W', that is too huge.
        // Choose a reasonable cross section.

        if (verbose) cout << "\n\tTesting equality operator." << endl;

        for (int n = 0; n < 4; ++n) {
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
                    ASSERT(0 == parseCommandLine(&(argStrings.back()[0]),
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
                if (veryVerbose) { T_(); T_(); P_(i); P_(n); P(X); }
                ASSERT((doItOnce < 2) == X.isParsed());

                for (int m = 0; m < 4; ++m) {
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
                            ASSERT(0 == parseCommandLine(
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

                    if (veryVerbose) { T_(); T_(); T_(); P_(h); P_(m); P(Y); }
                    ASSERT(!Y.isParsed());

                    LOOP4_ASSERT(i, n, h, m, !(X == Y));  // TEST HERE
                    LOOP4_ASSERT(i, n, h, m,   X != Y);   // TEST HERE

                    bsl::ostringstream oss2;
                    int parseRet2 = mY.parse(argPtrs2.size(),
                                             &argPtrs2[0],
                                             oss2);
                    LOOP_ASSERT(oss2.str(), 0 == parseRet2);

                    if (veryVerbose) { T_(); T_(); T_(); P_(h); P_(m); P(Y); }
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
                    int parseRet = mX.parse(argPtrs.size(), &argPtrs[0], oss);
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

        for (int n = 0; n < 4; ++n) {
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
                    ASSERT(0 == parseCommandLine(&(argStrings.back()[0]),
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

            Obj mZ(SPEC_TABLE, n);
            const Obj& Z = mZ;

            {
                Obj mX(Z);  // TEST HERE
                const Obj& X = mX;

                if (veryVerbose) { T_(); T_(); P_(i); P_(n); P(X); }
                LOOP2_ASSERT(n, i, !X.isParsed());
                LOOP2_ASSERT(n, i, Z != X);  // weird, but not parsed yet
            }

            bsl::ostringstream oss;
            int parseRet = mZ.parse(argPtrs.size(), &argPtrs[0], oss);
            LOOP3_ASSERT(n, i, oss.str(), 0 == parseRet);

            if (veryVerbose) { T_(); T_(); P_(i); P_(n); P(Z); }
            LOOP2_ASSERT(n, i, Z.isParsed());

            {
                Obj mX(Z);  // TEST HERE
                const Obj& X = mX;

                if (veryVerbose) { T_(); T_(); P_(i); P_(n); P(X); }
                LOOP2_ASSERT(n, i, X.isParsed());
                LOOP2_ASSERT(n, i, Z == X);
            }
        }
        }

        if (verbose) cout << "\t\tPassing in an allocator." << endl
                           << "\t\t\tWith no exceptions." << endl;

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
                        ASSERT(0 == parseCommandLine(&(argStrings.back()[0]),
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

                Obj mZ(SPEC_TABLE, n);
                const Obj& Z = mZ;

                {
                    Obj mX(Z, &testAllocator);  // TEST HERE
                    const Obj& X = mX;

                    if (veryVerbose) { T_(); T_(); P_(i); P_(n); P(X); }
                    LOOP2_ASSERT(n, i, !X.isParsed());
                    LOOP2_ASSERT(n, i, Z != X);  // weird, but not parsed yet
                }
                ASSERT(0 == testAllocator.numMismatches());
                ASSERT(0 == testAllocator.numBytesInUse());
                ASSERT(0 == defaultAllocator.numMismatches());

                bsl::ostringstream oss;
                int parseRet = mZ.parse(argPtrs.size(), &argPtrs[0], oss);
                LOOP3_ASSERT(n, i, oss.str(), 0 == parseRet);

                if (veryVerbose) { T_(); T_(); P_(i); P_(n); P(Z); }
                LOOP2_ASSERT(n, i, Z.isParsed());

                {
                    Obj mX(Z, &testAllocator);  // TEST HERE
                    const Obj& X = mX;

                    if (veryVerbose) { T_(); T_(); P_(i); P_(n); P(X); }
                    LOOP2_ASSERT(n, i, X.isParsed());
                    LOOP2_ASSERT(n, i, Z == X);
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
            // allocation patterns which are not much affected by the data.

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
                    ASSERT(0 == parseCommandLine(&(argStrings.back()[0]),
                                                 argc,
                                                 argv,
                                                 MAX_ARGC));

                    for (int k = 0; k < argc; ++k) {
                        argPtrs.push_back(argv[k]);
                    }
                }
            }

            Obj mZ(SPEC_TABLE, n);  // TEST HERE
            const Obj& Z = mZ;

            if (veryVerbose) { T_(); T_(); P_(i); P_(n); P(Z); }
            LOOP2_ASSERT(n, i, !Z.isParsed());

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {

                    Obj mX(Z, &testAllocator);  // TEST HERE
                    const Obj& X = mX;

                    if (veryVerbose) { T_(); T_(); P_(i); P_(n); P(X); }
                    LOOP2_ASSERT(n, i, !X.isParsed());
                    LOOP2_ASSERT(n, i, Z != X);  // weird, but not parsed yet

            }  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(0 == testAllocator.numMismatches());
            ASSERT(0 == testAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numMismatches());

            bsl::ostringstream oss;
            int parseRet = mZ.parse(argPtrs.size(), &argPtrs[0], oss);
            LOOP3_ASSERT(n, i, oss.str(), 0 == parseRet);

            if (veryVerbose) { T_(); T_(); P_(i); P_(n); P(Z); }
            LOOP2_ASSERT(n, i, Z.isParsed());

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {

                    Obj mX(Z, &testAllocator);  // TEST HERE
                    const Obj& X = mX;

                    if (veryVerbose) { T_(); T_(); P_(i); P_(n); P(X); }
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

        for (int n = 0; n < 4; ++n) {
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
                    ASSERT(0 == parseCommandLine(&(argStrings.back()[0]),
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
                if (veryVerbose) { T_(); T_(); P_(i); P_(n); P(X); }
                ASSERT((iteration <= 2) == X.isParsed());

                for (int m = 0; m < 4; ++m) {
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
                            ASSERT(0 == parseCommandLine(
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
                        int parseRet2 = mY.parse(argPtrs2.size(),
                                                 &argPtrs2[0],
                                                 oss2);
                        LOOP_ASSERT(oss2.str(), 0 == parseRet2);

                        ASSERT(Y.isParsed());
                    }

                    if (veryVerbose) { T_(); T_(); T_(); P_(h); P_(m); P(Y); }

                    mY = X;   // TEST HERE

                    if (!X.isParsed()) {
                        LOOP4_ASSERT(i, n, h, m, !(X == Y));
                        LOOP4_ASSERT(i, n, h, m,   X != Y);

                        // Assert that both can be parsed and yield identical
                        // result, but do it on a copy of X.

                        Obj mZ(X);  const Obj& Z = mZ;
                        ASSERT(0 == mY.parse(argPtrs.size(), &argPtrs[0]));
                        ASSERT(0 == mZ.parse(argPtrs.size(), &argPtrs[0]));

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
                    int parseRet = mX.parse(argPtrs.size(), &argPtrs[0], oss);
                    LOOP_ASSERT(oss.str(), 0 == parseRet);
                    ASSERT(X.isParsed());
                }

            } while (iteration);
        }
        }

        if (verbose) cout << "\t\tTesting self-assignment (Aliasing)." << endl;

        for (int n = 0; n < 4; ++n) {
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
                    ASSERT(0 == parseCommandLine(&(argStrings.back()[0]),
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
                int parseRet = mX.parse(argPtrs.size(), &argPtrs[0], oss);
                LOOP_ASSERT(oss.str(), 0 == parseRet);
                ASSERT(X.isParsed());

                bsl::ostringstream oss2;
                int parseRet2 = mY.parse(argPtrs.size(), &argPtrs[0], oss2);
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

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // TESTING ADDITIONAL CONSTRUCTORS

        if (verbose) cout << "\nTesting additional constructors." << endl;
        {
            bsl::vector<bsl::string> files;
            OptionInfo       specTable[1];
#ifndef BSLS_PLATFORM_CMP_SUN
            const OptionInfo SPEC_TABLE[1] =
#else
            // For some reason, SunPro doesn't like const OptionInfo[]...
            // We get  >> Assertion:  unexpected array kind in
            // type_builder::visit_array (../lnk/v2mangler.cc, line 1287)

            OptionInfo SPEC_TABLE[1] =
#endif
            {
              {
                "",                                          // non-option
                "fileList",                                  // name
                "files to be sorted",                        // description
                balb::CommandLineTypeInfo(&files)             // link
              }
            };
            const int   argc = 2;
            const char *argv[argc] = {
                "progname",
                "filename"
            };
            specTable[0] = SPEC_TABLE[0];

            bsl::ostringstream oss2;
            Obj mZ((const OptionInfo *)specTable, 1);  const Obj& Z = mZ;
            ASSERT(0 == mZ.parse(argc, argv, oss2));

            {
                Obj mX(specTable);  const Obj& X = mX;  // TEST HERE
                ASSERT(!X.isParsed());

                bsl::ostringstream oss;
                int parseRet = mX.parse(argc, argv, oss);
                LOOP_ASSERT(oss.str(), 0 == parseRet);
                ASSERT(X.isParsed());
                ASSERT(Z == X);
            }
            {
                Obj mX(SPEC_TABLE);  const Obj& X = mX;  // TEST HERE
                ASSERT(!X.isParsed());

                bsl::ostringstream oss;
                int parseRet = mX.parse(argc, argv, oss);
                LOOP_ASSERT(oss.str(), 0 == parseRet);
                ASSERT(X.isParsed());
            }
            {
                Obj mX(specTable, &testAllocator);  // TEST HERE
                const Obj& X = mX;
                ASSERT(!X.isParsed());

                bsl::ostringstream oss;
                int parseRet = mX.parse(argc, argv, oss);
                LOOP_ASSERT(oss.str(), 0 == parseRet);
                ASSERT(X.isParsed());
                ASSERT(Z == X);
            }
            ASSERT(0 == testAllocator.numMismatches());
            ASSERT(0 == testAllocator.numBytesInUse());
            {
                Obj mX(SPEC_TABLE, &testAllocator);  // TEST HERE
                const Obj& X = mX;
                ASSERT(!X.isParsed());

                bsl::ostringstream oss;
                int parseRet = mX.parse(argc, argv, oss);
                LOOP_ASSERT(oss.str(), 0 == parseRet);
                ASSERT(X.isParsed());
            }
            ASSERT(0 == testAllocator.numMismatches());
            ASSERT(0 == testAllocator.numBytesInUse());
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // TESTING ADDITIONAL ACCESSORS

        if (verbose) cout << "\n\tTesting additional accessors." << endl;

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
                        ASSERT(0 == parseCommandLine(&(argStrings.back()[0]),
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
                int parseRet = mX.parse(argPtrs.size(), &argPtrs[0], oss);
                LOOP_ASSERT(oss.str(), 0 == parseRet);

                if (veryVerbose) { T_(); T_(); P_(i); P_(n); P(X); }
                if (veryVerbose) { T_(); T_(); T_(); P(argString); }

                // Test name-based accessors.

                int pos = 1;
                for (int j = 0; j < n; ++j) {
                    const bsl::string& name = specTable[j].d_name;
                    if (veryVerbose) {
                        T_(); T_(); T_(); P(name);
                        T_(); T_(); T_(); P(X.numSpecified(name));
                        if (ET::isArrayType(specTable[j].d_typeInfo.type())) {
                            T_(); T_(); T_(); P(X.positions(name).size());
                            if (0 < X.positions(name).size()) {
                                T_(); T_(); T_(); P(X.positions(name)[0]);
                            }
                            if (1 < X.positions(name).size()) {
                                T_(); T_(); T_(); P(X.positions(name)[1]);
                            }
                        } else {
                            T_(); T_(); T_(); P(X.position(name));
                        }
                    }

                    if (argStrings[j + 1].empty()) {
                        LOOP2_ASSERT(n, i, 0 == X.numSpecified(name));
                        if (ET::isArrayType(specTable[j].d_typeInfo.type())) {
                            LOOP2_ASSERT(n, i, X.positions(name).empty());
                        } else {
                            LOOP2_ASSERT(n, i, -1 == X.position(name));
                        }
                        if (specTable[j].d_defaultInfo.hasDefaultValue()) {
                // v--------+
                switch (specTable[j].d_typeInfo.type()) {
                  case ET::BDEM_BOOL: {
                    LOOP2_ASSERT(n, i, X.theBool(name));
                  } break;
                  case ET::BDEM_CHAR: {
                    LOOP2_ASSERT(n, i, defaultChar == X.theChar(name));
                  } break;
                  case ET::BDEM_SHORT: {
                    LOOP2_ASSERT(n, i, defaultShort == X.theShort(name));
                  } break;
                  case ET::BDEM_INT: {
                    LOOP2_ASSERT(n, i, defaultInt == X.theInt(name));
                  } break;
                  case ET::BDEM_INT64: {
                    LOOP2_ASSERT(n, i, defaultInt64 == X.theInt64(name));
                  } break;
                  case ET::BDEM_FLOAT: {
                    LOOP2_ASSERT(n, i, defaultFloat == X.theFloat(name));
                  } break;
                  case ET::BDEM_DOUBLE: {
                    LOOP2_ASSERT(n, i, defaultDouble == X.theDouble(name));
                  } break;
                  case ET::BDEM_STRING: {
                    LOOP2_ASSERT(n, i, defaultString == X.theString(name));
                  } break;
                  case ET::BDEM_DATETIME: {
                    LOOP2_ASSERT(n, i, defaultDatetime == X.theDatetime(name));
                  } break;
                  case ET::BDEM_DATE: {
                    LOOP2_ASSERT(n, i, defaultDate == X.theDate(name));
                  } break;
                  case ET::BDEM_TIME: {
                    LOOP2_ASSERT(n, i, defaultTime == X.theTime(name));
                  } break;
                  case ET::BDEM_CHAR_ARRAY: {
                    LOOP2_ASSERT(n, i,
                                 defaultCharArray == X.theCharArray(name));
                  } break;
                  case ET::BDEM_SHORT_ARRAY: {
                    LOOP2_ASSERT(n, i,
                                defaultShortArray == X.theShortArray(name));
                  } break;
                  case ET::BDEM_INT_ARRAY: {
                    LOOP2_ASSERT(n, i,
                                 defaultIntArray == X.theIntArray(name));
                  } break;
                  case ET::BDEM_INT64_ARRAY: {
                    LOOP2_ASSERT(n, i,
                                 defaultInt64Array == X.theInt64Array(name));
                  } break;
                  case ET::BDEM_FLOAT_ARRAY: {
                    LOOP2_ASSERT(n, i,
                                 defaultFloatArray == X.theFloatArray(name));
                  } break;
                  case ET::BDEM_DOUBLE_ARRAY: {
                    LOOP2_ASSERT(n, i,
                                 defaultDoubleArray == X.theDoubleArray(name));
                  } break;
                  case ET::BDEM_STRING_ARRAY: {
                    LOOP2_ASSERT(n, i,
                                 defaultStringArray == X.theStringArray(name));
                  } break;
                  case ET::BDEM_DATETIME_ARRAY: {
                    LOOP2_ASSERT(n, i,
                             defaultDatetimeArray == X.theDatetimeArray(name));
                  } break;
                  case ET::BDEM_DATE_ARRAY: {
                    LOOP2_ASSERT(n, i,
                                 defaultDateArray == X.theDateArray(name));
                  } break;
                  case ET::BDEM_TIME_ARRAY: {
                    LOOP2_ASSERT(n, i,
                                 defaultTimeArray == X.theTimeArray(name));
                  } break;
                  default: ASSERT(0);
                };
                // ---------v
                        }
                    } else {
                        if (ET::isArrayType(specTable[j].d_typeInfo.type())) {
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
                    T_(); T_(); T_(); P(usage);
                }
                ASSERT(0 < oss2.str().length());
            }
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING balb::CommandLineOption
        //
        // Concerns: 'balb::CommandLineOption' is a full-blown in-core
        //   value-semantic version of 'balb::CommandLineOptionInfo', which can
        //   be initialized and converted to its simpler 'struct' variant.  We
        //   need to make sure it is exception safe, uses 'bdema' allocators
        //   properly so it can be stored in a container, and has a value as
        //   expected.
        //
        // Plan:  Follow the standard attribute test driver sequence.  We
        //   choose as primary manipulator the constructor:
        //     - balb::CommandLineOption(const OptionInfo&);
        //   and as basic accessors the attribute methods:
        //      - bsl::string tagString() const;
        //      - bsl::string name() const;
        //      - bsl::string description() const;
        //      - TypeInfo typeInfo() const;
        //       - OccurrenceInfo occurrenceInfo() const;
        //   Beyond that, there are no special concerns.
        //
        // Testing:
        //   class balb::CommandLineOption;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "balb::CommandLineOption" << endl
                          << "======================" << endl;

        typedef balb::CommandLineOption Obj;

        bslma::TestAllocator testAllocator(veryVeryVerbose);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 2 OF VALUE-SEMANTIC TEST DRIVER
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP)

        if (verbose) cout << "\n\tTesting primary manipulators." << endl;

        if (verbose) cout << "\t\tWithout passing in an allocator." << endl;

        for (int i = 0; i < NUM_OPTION_TAGS; ++i) {
        for (int j = 0; j < NUM_OPTION_NAMES; ++j) {
        for (int k = 0; k < NUM_OPTION_DESCRIPTIONS; ++k) {
        for (int l = 0; l < NUM_OPTION_TYPEINFO; ++l) {
        for (int m = 0; m < NUM_OPTION_OCCURRENCES; ++m) {
            const int n = l % NUM_OPTION_DEFAULT_VALUES;
                // Instead of:
                //   for (int n = 0; n < NUM_OPTION_DEFAULT_VALUES; ++n)
                // Valid only because of the way we organized the table data:
                //..
                   ASSERT(OPTION_TYPEINFO[l].d_type ==
                                              OPTION_DEFAULT_VALUES[n].d_type);
                //..
            const int       LINE1      = OPTION_TAGS[i].d_line;
            const char     *TAG        = OPTION_TAGS[i].d_tag;
            const int       LINE2      = OPTION_NAMES[j].d_line;
            const char     *NAME       = OPTION_NAMES[j].d_name;
            const int       LINE3      = OPTION_DESCRIPTIONS[k].d_line;
            const char     *DESC       = OPTION_DESCRIPTIONS[k].d_description;
            const int       LINE4      = OPTION_TYPEINFO[l].d_line;
            const BdemType  TYPE       = OPTION_TYPEINFO[l].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[l].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[l].d_constraint_p;

            const int            LINE5    = OPTION_OCCURRENCES[l].d_line;
            const OccurrenceType OTYPE    = OPTION_OCCURRENCES[l].d_type;
            const void     *DEFAULT_VALUE = OPTION_DEFAULT_VALUES[n].d_value;

            if (TYPE != OPTION_DEFAULT_VALUES[n].d_type) {
                continue;
            }

            const TypeInfo       EXP_TYPEINFO =
                                    createTypeInfo(TYPE, VARIABLE, CONSTRAINT);
            const OccurrenceInfo EXP_OCCURRENCE_INFO =
                              createOccurrenceInfo(OTYPE, TYPE, DEFAULT_VALUE);
            const balb::CommandLineOptionInfo EXP = {
                TAG,
                NAME,
                DESC,
                EXP_TYPEINFO,
                EXP_OCCURRENCE_INFO
            };

            Obj mX(EXP);  const Obj& X = mX;      // TEST HERE

            if (veryVerbose) {
                T_(); P_(LINE1); P_(LINE2); P_(LINE3); P_(LINE4); P(LINE5);
                T_(); P(EXP);
                T_(); P(X);
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

        bslma::TestAllocator defaultAllocator(veryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&defaultAllocator);

        if (verbose) cout << "\t\tPassing in an allocator." << endl
                           << "\t\t\tWith no exceptions." << endl;

        const bool HAS_BSLMA_ALLOCATOR_TRAIT =
             bslalg::HasTrait<Obj, bslalg::TypeTraitUsesBslmaAllocator>::VALUE;

        ASSERT(HAS_BSLMA_ALLOCATOR_TRAIT);

        for (int i = 0; i < NUM_OPTION_TAGS; ++i) {
        for (int j = 0; j < NUM_OPTION_NAMES; ++j) {
        for (int k = 0; k < NUM_OPTION_DESCRIPTIONS; ++k) {
        for (int l = 0; l < NUM_OPTION_TYPEINFO; ++l) {
        for (int m = 0; m < NUM_OPTION_OCCURRENCES; ++m) {
            const int n = l % NUM_OPTION_DEFAULT_VALUES;
                // Instead of:
                //   for (int n = 0; n < NUM_OPTION_DEFAULT_VALUES; ++n)
                // Valid only because of the way we organized the table data:
                //..
                   ASSERT(OPTION_TYPEINFO[l].d_type ==
                                              OPTION_DEFAULT_VALUES[n].d_type);
                //..
            const int       LINE1      = OPTION_TAGS[i].d_line;
            const char     *TAG        = OPTION_TAGS[i].d_tag;
            const int       LINE2      = OPTION_NAMES[j].d_line;
            const char     *NAME       = OPTION_NAMES[j].d_name;
            const int       LINE3      = OPTION_DESCRIPTIONS[k].d_line;
            const char     *DESC       = OPTION_DESCRIPTIONS[k].d_description;
            const int       LINE4      = OPTION_TYPEINFO[l].d_line;
            const BdemType  TYPE       = OPTION_TYPEINFO[l].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[l].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[l].d_constraint_p;

            const int            LINE5    = OPTION_OCCURRENCES[m].d_line;
            const OccurrenceType OTYPE    = OPTION_OCCURRENCES[m].d_type;
            const void     *DEFAULT_VALUE = OPTION_DEFAULT_VALUES[n].d_value;

            if (TYPE != OPTION_DEFAULT_VALUES[n].d_type) {
                continue;
            }

            const TypeInfo       EXP_TYPEINFO =
                                    createTypeInfo(TYPE, VARIABLE, CONSTRAINT);
            const OccurrenceInfo EXP_OCCURRENCE_INFO =
                              createOccurrenceInfo(OTYPE, TYPE, DEFAULT_VALUE);
            const balb::CommandLineOptionInfo EXP = {
                TAG,
                NAME,
                DESC,
                EXP_TYPEINFO,
                EXP_OCCURRENCE_INFO
            };

            const int NUM_BYTES = defaultAllocator.numBytesInUse();
            {
                Obj mX(EXP, &testAllocator);  const Obj& X = mX;

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

        if (verbose) cout << "\t\t\tWith exceptions." << endl;

        for (int i = 0; i < NUM_OPTION_TAGS; ++i) {
        for (int j = 0; j < NUM_OPTION_NAMES; ++j) {
        for (int k = 0; k < NUM_OPTION_DESCRIPTIONS; ++k) {
        for (int l = 0; l < NUM_OPTION_TYPEINFO; ++l) {
        for (int m = 0; m < NUM_OPTION_OCCURRENCES; ++m) {
            const int n = l % NUM_OPTION_DEFAULT_VALUES;
                // Instead of:
                //   for (int n = 0; n < NUM_OPTION_DEFAULT_VALUES; ++n)
                // Valid only because of the way we organized the table data:
                //..
                   ASSERT(OPTION_TYPEINFO[l].d_type ==
                                              OPTION_DEFAULT_VALUES[n].d_type);
                //..
            const int       LINE1      = OPTION_TAGS[i].d_line;
            const char     *TAG        = OPTION_TAGS[i].d_tag;
            const int       LINE2      = OPTION_NAMES[j].d_line;
            const char     *NAME       = OPTION_NAMES[j].d_name;
            const int       LINE3      = OPTION_DESCRIPTIONS[k].d_line;
            const char     *DESC       = OPTION_DESCRIPTIONS[k].d_description;
            const int       LINE4      = OPTION_TYPEINFO[l].d_line;
            const BdemType  TYPE       = OPTION_TYPEINFO[l].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[l].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[l].d_constraint_p;

            const int            LINE5    = OPTION_OCCURRENCES[m].d_line;
            const OccurrenceType OTYPE    = OPTION_OCCURRENCES[m].d_type;
            const void     *DEFAULT_VALUE = OPTION_DEFAULT_VALUES[n].d_value;

            if (TYPE != OPTION_DEFAULT_VALUES[n].d_type) {
                continue;
            }

            const TypeInfo       EXP_TYPEINFO =
                                    createTypeInfo(TYPE, VARIABLE, CONSTRAINT);
            const OccurrenceInfo EXP_OCCURRENCE_INFO =
                              createOccurrenceInfo(OTYPE, TYPE, DEFAULT_VALUE);
            const balb::CommandLineOptionInfo EXP = {
                TAG,
                NAME,
                DESC,
                EXP_TYPEINFO,
                EXP_OCCURRENCE_INFO
            };

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                Obj mX(EXP, &testAllocator);  const Obj& X = mX;

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
        ASSERT(0 == testAllocator.numBytesInUse());
        ASSERT(0 == testAllocator.numMismatches());

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 4 OF VALUE-SEMANTIC TEST DRIVER
        // TESTING BASIC ACCESSORS

        if (verbose) cout << "\n\tTesting basic accessors." << endl;

        for (int i = 0; i < NUM_OPTION_TAGS; ++i) {
        for (int j = 0; j < NUM_OPTION_NAMES; ++j) {
        for (int k = 0; k < NUM_OPTION_DESCRIPTIONS; ++k) {
        for (int l = 0; l < NUM_OPTION_TYPEINFO; ++l) {
        for (int m = 0; m < NUM_OPTION_OCCURRENCES; ++m) {
            const int n = l % NUM_OPTION_DEFAULT_VALUES;
                // Instead of:
                //   for (int n = 0; n < NUM_OPTION_DEFAULT_VALUES; ++n)
                // Valid only because of the way we organized the table data:
                //..
                   ASSERT(OPTION_TYPEINFO[l].d_type ==
                                              OPTION_DEFAULT_VALUES[n].d_type);
                //..
            const int       LINE1      = OPTION_TAGS[i].d_line;
            const char     *TAG        = OPTION_TAGS[i].d_tag;
            const int       LINE2      = OPTION_NAMES[j].d_line;
            const char     *NAME       = OPTION_NAMES[j].d_name;
            const int       LINE3      = OPTION_DESCRIPTIONS[k].d_line;
            const char     *DESC       = OPTION_DESCRIPTIONS[k].d_description;
            const int       LINE4      = OPTION_TYPEINFO[l].d_line;
            const BdemType  TYPE       = OPTION_TYPEINFO[l].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[l].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[l].d_constraint_p;

            const int            LINE5    = OPTION_OCCURRENCES[m].d_line;
            const OccurrenceType OTYPE    = OPTION_OCCURRENCES[m].d_type;
            const void     *DEFAULT_VALUE = OPTION_DEFAULT_VALUES[n].d_value;

            if (TYPE != OPTION_DEFAULT_VALUES[n].d_type) {
                continue;
            }

            const TypeInfo       EXP_TYPE_INFO       = createTypeInfo(
                                                   TYPE, VARIABLE, CONSTRAINT);
            const OccurrenceInfo EXP_OCCURRENCE_INFO = createOccurrenceInfo(
                                                   OTYPE, TYPE, DEFAULT_VALUE);

            const balb::CommandLineOptionInfo EXP = {
                TAG,
                NAME,
                DESC,
                EXP_TYPE_INFO,
                EXP_OCCURRENCE_INFO
            };

            Obj mX(EXP); const Obj& X = mX;

            if (veryVerbose) { T_(); T_(); P(X); }
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
                           ? OptionInfo::BAEA_NON_OPTION
                           : (ET::BDEM_BOOL == TYPE
                             ? OptionInfo::BAEA_FLAG
                             : OptionInfo::BAEA_OPTION);
            LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,
                         FLAG == X.argType());

            const bool IS_ARRAY = bdlmxxx::ElemType::isArrayType(TYPE);
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

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 5 OF VALUE-SEMANTIC TEST DRIVER
        // TESTING OUTPUT (<<) OPERATOR AND PRINT

        if (verbose)
            cout << "\n\tTesting output operator and 'print'." << endl;

        typedef bdlpuxxx::ParserImpUtil Parser;

        for (int i = 0; i < NUM_OPTION_TAGS; ++i) {
        for (int j = 0; j < NUM_OPTION_NAMES; ++j) {
        for (int k = 0; k < NUM_OPTION_DESCRIPTIONS; ++k) {
        for (int l = 0; l < NUM_OPTION_TYPEINFO; ++l) {
        for (int m = 0; m < NUM_OPTION_OCCURRENCES; ++m) {
            const int n = l % NUM_OPTION_DEFAULT_VALUES;
                // Instead of:
                //   for (int n = 0; n < NUM_OPTION_DEFAULT_VALUES; ++n)
                // Valid only because of the way we organized the table data:
                //..
                   ASSERT(OPTION_TYPEINFO[l].d_type ==
                                              OPTION_DEFAULT_VALUES[n].d_type);
                //..
            const int       LINE1      = OPTION_TAGS[i].d_line;
            const char     *TAG        = OPTION_TAGS[i].d_tag;
            const int       LINE2      = OPTION_NAMES[j].d_line;
            const char     *NAME       = OPTION_NAMES[j].d_name;
            const int       LINE3      = OPTION_DESCRIPTIONS[k].d_line;
            const char     *DESC       = OPTION_DESCRIPTIONS[k].d_description;
            const int       LINE4      = OPTION_TYPEINFO[l].d_line;
            const BdemType  TYPE       = OPTION_TYPEINFO[l].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[l].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[l].d_constraint_p;

            const int            LINE5    = OPTION_OCCURRENCES[m].d_line;
            const OccurrenceType OTYPE    = OPTION_OCCURRENCES[m].d_type;
            const void     *DEFAULT_VALUE = OPTION_DEFAULT_VALUES[n].d_value;

            if (TYPE != OPTION_DEFAULT_VALUES[n].d_type) {
                continue;
            }

            const balb::CommandLineOptionInfo EXP = {
                TAG,
                NAME,
                DESC,
                createTypeInfo(TYPE, VARIABLE, CONSTRAINT),
                createOccurrenceInfo(OTYPE, TYPE, DEFAULT_VALUE)
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
                T_(); T_(); P(X);
                T_(); T_(); P(ss1);
                T_(); T_(); P(ss2);
            }

            const char *output = ss1.c_str();

            int parseRet = parseCommandLineOption(&output, X, output);
            LOOP4_ASSERT(X, ss1, output, parseRet, 0 == parseRet);

            output = ss3.c_str();

            parseRet = parseCommandLineOption(&output, X, output);
            LOOP4_ASSERT(X, ss3, output, parseRet, 0 == parseRet);
        }
        }
        }
        }
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 6 OF VALUE-SEMANTIC TEST DRIVER
        // TESTING EQUALITY OPERATOR
        // Note: Unless they share the constraint, two objects will always
        // differ.  We limit the testing since there are just too many values
        // to test otherwise: 2 tags, 2 names, 2 descriptions suffice.  We also
        // limit ourselves to TypeInfo with no linked variables and no
        // constraint, and no array types.  Finally, we limit ourselves to
        // required or optional occurrences only (not hidden), and no array
        // default values.

        if (verbose) cout << "\n\tTesting equality operator." << endl;

        if (verbose)
            cout << "\t\tCompare each pair of values (u, v) in W X W." << endl;

        for (int i1 = 0; i1 < 2; ++i1) {
        for (int j1 = 1; j1 < 3; ++j1) {
        for (int k1 = 0; k1 < 2; ++k1) {
        for (int l1 = 0; l1 < NUM_OPTION_TYPEINFO / 8; ++l1) {
        for (int m1 = 0; m1 < NUM_OPTION_OCCURRENCES - 1; ++m1) {
            int n1 = l1;
                // Instead of:
                //   for (int n1 = 0; n1 < NUM_OPTION_DEFAULT_VALUES / 2; ++n1)
                // Valid only because of the way we organized the table data:
                //..
                   ASSERT(OPTION_TYPEINFO[l1].d_type ==
                                             OPTION_DEFAULT_VALUES[n1].d_type);
                //..
            const int       LINE1 = OPTION_TAGS[i1].d_line;
            const char     *TAG1  = OPTION_TAGS[i1].d_tag;
            const int       LINE2 = OPTION_NAMES[j1].d_line;
            const char     *NAME1 = OPTION_NAMES[j1].d_name;
            const int       LINE3 = OPTION_DESCRIPTIONS[k1].d_line;
            const char     *DESC1 = OPTION_DESCRIPTIONS[k1].d_description;
            const int       LINE4 = OPTION_TYPEINFO[l1].d_line;
            const BdemType  TYPE1 = OPTION_TYPEINFO[l1].d_type;
            const int       LINE5 = OPTION_OCCURRENCES[m1].d_line;

            const OccurrenceType OTYPE1 = OPTION_OCCURRENCES[m1].d_type;

            void       *VARIABLE1   = OPTION_TYPEINFO[l1].d_linkedVariable_p;
            void       *CONSTRAINT1 = OPTION_TYPEINFO[l1].d_constraint_p;

            const void *DEFAULT_VALUE1 = OPTION_DEFAULT_VALUES[n1].d_value;

            if (TYPE1 != OPTION_DEFAULT_VALUES[n1].d_type) {
                continue;
            }

            const balb::CommandLineOptionInfo EXP1 = {
                TAG1,
                NAME1,
                DESC1,
                createTypeInfo(TYPE1, VARIABLE1, CONSTRAINT1),
                createOccurrenceInfo(OTYPE1, TYPE1, DEFAULT_VALUE1)
            };

            Obj mX(EXP1); const Obj& X = mX;

            for (int i2 = 0; i2 < 2; ++i2) {
            for (int j2 = 0; j2 < 2; ++j2) {
            for (int k2 = 0; k2 < 2; ++k2) {
            for (int l2 = 0; l2 < NUM_OPTION_TYPEINFO / 8; ++l2) {
            for (int m2 = 0; m2 < NUM_OPTION_OCCURRENCES - 1; ++m2) {
                int n2 = l2;
                    // instead of:
                    // for (int n2 = 0; n2 < NUM_OPTION_DEFAULT_VALUES / 2; ...
                    // and only because of the way we organized the table data:
                    //..
                       ASSERT(OPTION_TYPEINFO[l2].d_type ==
                                             OPTION_DEFAULT_VALUES[n2].d_type);
                    //..
                const int       LINE6  = OPTION_TAGS[i2].d_line;
                const char     *TAG2   = OPTION_TAGS[i2].d_tag;
                const int       LINE7  = OPTION_NAMES[j2].d_line;
                const char     *NAME2  = OPTION_NAMES[j2].d_name;
                const int       LINE8  = OPTION_DESCRIPTIONS[k2].d_line;
                const char     *DESC2  = OPTION_DESCRIPTIONS[k2].d_description;
                const int       LINE9  = OPTION_TYPEINFO[l2].d_line;
                const BdemType  TYPE2  = OPTION_TYPEINFO[l2].d_type;
                const int       LINE10 = OPTION_OCCURRENCES[m2].d_line;

                const OccurrenceType OTYPE2 = OPTION_OCCURRENCES[m2].d_type;

                void *VARIABLE2   = OPTION_TYPEINFO[l2].d_linkedVariable_p;
                void *CONSTRAINT2 = OPTION_TYPEINFO[l2].d_constraint_p;

                const void *DEFAULT_VALUE2 = OPTION_DEFAULT_VALUES[n2].d_value;

                if (TYPE2 != OPTION_DEFAULT_VALUES[n2].d_type) {
                    continue;
                }

                const balb::CommandLineOptionInfo EXP2 = {
                    TAG2,
                    NAME2,
                    DESC2,
                    createTypeInfo(TYPE2, VARIABLE2, CONSTRAINT2),
                    createOccurrenceInfo(OTYPE2, TYPE2, DEFAULT_VALUE2)
                };

                Obj mY(EXP2); const Obj& Y = mY;

                bool isSame = X.name() == Y.name()
                           && X.tagString() == Y.tagString()
                           && X.description() == Y.description()
                           && X.typeInfo() == Y.typeInfo()
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

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 7 OF VALUE-SEMANTIC TEST DRIVER
        // TESTING COPY CONSTRUCTOR

        if (verbose) cout << "\n\tTesting copy constructor." << endl;

        if (verbose) cout << "\t\tWithout passing in an allocator." << endl;

        for (int i = 0; i < NUM_OPTION_TAGS; ++i) {
        for (int j = 0; j < NUM_OPTION_NAMES; ++j) {
        for (int k = 0; k < NUM_OPTION_DESCRIPTIONS; ++k) {
        for (int l = 0; l < NUM_OPTION_TYPEINFO; ++l) {
        for (int m = 0; m < NUM_OPTION_OCCURRENCES; ++m) {
            const int n = l % NUM_OPTION_DEFAULT_VALUES;
                // Instead of:
                //   for (int n = 0; n < NUM_OPTION_DEFAULT_VALUES; ++n)
                // Valid only because of the way we organized the table data:
                //..
                   ASSERT(OPTION_TYPEINFO[l].d_type ==
                                              OPTION_DEFAULT_VALUES[n].d_type);
                //..
            const int       LINE1      = OPTION_TAGS[i].d_line;
            const char     *TAG        = OPTION_TAGS[i].d_tag;
            const int       LINE2      = OPTION_NAMES[j].d_line;
            const char     *NAME       = OPTION_NAMES[j].d_name;
            const int       LINE3      = OPTION_DESCRIPTIONS[k].d_line;
            const char     *DESC       = OPTION_DESCRIPTIONS[k].d_description;
            const int       LINE4      = OPTION_TYPEINFO[l].d_line;
            const BdemType  TYPE       = OPTION_TYPEINFO[l].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[l].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[l].d_constraint_p;

            const int            LINE5    = OPTION_OCCURRENCES[m].d_line;
            const OccurrenceType OTYPE    = OPTION_OCCURRENCES[m].d_type;
            const void     *DEFAULT_VALUE = OPTION_DEFAULT_VALUES[n].d_value;

            if (TYPE != OPTION_DEFAULT_VALUES[n].d_type) {
                continue;
            }

            const balb::CommandLineOptionInfo EXP = {
                TAG,
                NAME,
                DESC,
                createTypeInfo(TYPE, VARIABLE, CONSTRAINT),
                createOccurrenceInfo(OTYPE, TYPE, DEFAULT_VALUE)
            };

            Obj mX(EXP); const Obj& X = mX;

            Obj mY(X);  const Obj& Y = mY;  // TEST HERE

            if (veryVerbose) { T_(); T_(); P_(X); P(Y); }

            LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5,   X == Y);
            LOOP5_ASSERT(LINE1, LINE2, LINE3, LINE4, LINE5, !(X != Y));
        }
        }
        }
        }
        }

        if (verbose) cout << "\t\tPassing in an allocator." << endl
                           << "\t\t\tWith no exceptions." << endl;

        for (int i = 0; i < NUM_OPTION_TAGS; ++i) {
        for (int j = 0; j < NUM_OPTION_NAMES; ++j) {
        for (int k = 0; k < NUM_OPTION_DESCRIPTIONS; ++k) {
        for (int l = 0; l < NUM_OPTION_TYPEINFO; ++l) {
        for (int m = 0; m < NUM_OPTION_OCCURRENCES; ++m) {
            const int n = l % NUM_OPTION_DEFAULT_VALUES;
                // Instead of:
                //   for (int n = 0; n < NUM_OPTION_DEFAULT_VALUES; ++n)
                // Valid only because of the way we organized the table data:
                //..
                   ASSERT(OPTION_TYPEINFO[l].d_type ==
                                              OPTION_DEFAULT_VALUES[n].d_type);
                //..
            const int       LINE1      = OPTION_TAGS[i].d_line;
            const char     *TAG        = OPTION_TAGS[i].d_tag;
            const int       LINE2      = OPTION_NAMES[j].d_line;
            const char     *NAME       = OPTION_NAMES[j].d_name;
            const int       LINE3      = OPTION_DESCRIPTIONS[k].d_line;
            const char     *DESC       = OPTION_DESCRIPTIONS[k].d_description;
            const int       LINE4      = OPTION_TYPEINFO[l].d_line;
            const BdemType  TYPE       = OPTION_TYPEINFO[l].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[l].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[l].d_constraint_p;

            const int            LINE5    = OPTION_OCCURRENCES[m].d_line;
            const OccurrenceType OTYPE    = OPTION_OCCURRENCES[m].d_type;
            const void     *DEFAULT_VALUE = OPTION_DEFAULT_VALUES[n].d_value;

            if (TYPE != OPTION_DEFAULT_VALUES[n].d_type) {
                continue;
            }

            const balb::CommandLineOptionInfo EXP = {
                TAG,
                NAME,
                DESC,
                createTypeInfo(TYPE, VARIABLE, CONSTRAINT),
                createOccurrenceInfo(OTYPE, TYPE, DEFAULT_VALUE)
            };

            Obj mX(EXP); const Obj& X = mX;

            const int NUM_BYTES = defaultAllocator.numBytesInUse();
            const int NUM_ALLOC = defaultAllocator.numAllocations();
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

        if (verbose) cout << "\t\t\tWith exceptions." << endl;

        for (int i = 0; i < NUM_OPTION_TAGS; ++i) {
        for (int j = 0; j < NUM_OPTION_NAMES; ++j) {
        for (int k = 0; k < NUM_OPTION_DESCRIPTIONS; ++k) {
        for (int l = 0; l < NUM_OPTION_TYPEINFO; ++l) {
        for (int m = 0; m < NUM_OPTION_OCCURRENCES; ++m) {
            const int n = l % NUM_OPTION_DEFAULT_VALUES;
                // Instead of:
                //   for (int n = 0; n < NUM_OPTION_DEFAULT_VALUES; ++n)
                // Valid only because of the way we organized the table data:
                //..
                   ASSERT(OPTION_TYPEINFO[l].d_type ==
                                              OPTION_DEFAULT_VALUES[n].d_type);
                //..
            const char     *TAG        = OPTION_TAGS[i].d_tag;
            const char     *NAME       = OPTION_NAMES[j].d_name;
            const char     *DESC       = OPTION_DESCRIPTIONS[k].d_description;
            const BdemType  TYPE       = OPTION_TYPEINFO[l].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[l].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[l].d_constraint_p;

            const OccurrenceType OTYPE    = OPTION_OCCURRENCES[m].d_type;
            const void     *DEFAULT_VALUE = OPTION_DEFAULT_VALUES[n].d_value;

            if (TYPE != OPTION_DEFAULT_VALUES[n].d_type) {
                continue;
            }

            const balb::CommandLineOptionInfo EXP = {
                TAG,
                NAME,
                DESC,
                createTypeInfo(TYPE, VARIABLE, CONSTRAINT),
                createOccurrenceInfo(OTYPE, TYPE, DEFAULT_VALUE)
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
        ASSERT(0 == testAllocator.numBytesInUse());
        ASSERT(0 == testAllocator.numMismatches());

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 9 OF VALUE-SEMANTIC TEST DRIVER
        // TESTING ASSIGNMENT OPERATOR

        if (verbose) cout << "\n\tTesting assignment operator." << endl;

        if (verbose) cout << "\t\tTesting assignment u = v." << endl;

        for (int i1 = 0; i1 < 2; ++i1) {
        for (int j1 = 0; j1 < 2; ++j1) {
        for (int k1 = 0; k1 < 2; ++k1) {
        for (int l1 = 0; l1 < NUM_OPTION_TYPEINFO / 8; ++l1) {
        for (int m1 = 0; m1 < NUM_OPTION_OCCURRENCES - 1; ++m1) {
            const int n1 = l1;
                // instead of:
                // for (int n1 = 0; n1 < NUM_OPTION_DEFAULT_VALUES / 2; ++n1)
                // and only because of the way we organized the table data:
                //..
                   ASSERT(OPTION_TYPEINFO[l1].d_type ==
                                             OPTION_DEFAULT_VALUES[n1].d_type);
                //..
            const int       LINE1 = OPTION_TAGS[i1].d_line;
            const char     *TAG1  = OPTION_TAGS[i1].d_tag;
            const int       LINE2 = OPTION_NAMES[j1].d_line;
            const char     *NAME1 = OPTION_NAMES[j1].d_name;
            const int       LINE3 = OPTION_DESCRIPTIONS[k1].d_line;
            const char     *DESC1 = OPTION_DESCRIPTIONS[k1].d_description;
            const int       LINE4 = OPTION_TYPEINFO[l1].d_line;
            const BdemType  TYPE1 = OPTION_TYPEINFO[l1].d_type;
            const int       LINE5 = OPTION_OCCURRENCES[m1].d_line;

            const OccurrenceType OTYPE1 = OPTION_OCCURRENCES[m1].d_type;

            void       *VARIABLE1   = OPTION_TYPEINFO[l1].d_linkedVariable_p;
            void       *CONSTRAINT1 = OPTION_TYPEINFO[l1].d_constraint_p;

            const void *DEFAULT_VALUE1 = OPTION_DEFAULT_VALUES[n1].d_value;

            if (TYPE1 != OPTION_DEFAULT_VALUES[n1].d_type) {
                continue;
            }

            const balb::CommandLineOptionInfo EXP1 = {
                TAG1,
                NAME1,
                DESC1,
                createTypeInfo(TYPE1, VARIABLE1, CONSTRAINT1),
                createOccurrenceInfo(OTYPE1, TYPE1, DEFAULT_VALUE1)
            };

            Obj mX(EXP1); const Obj& X = mX;

            for (int i2 = 0; i2 < 2; ++i2) {
            for (int j2 = 0; j2 < 2; ++j2) {
            for (int k2 = 0; k2 < 2; ++k2) {
            for (int l2 = 0; l2 < NUM_OPTION_TYPEINFO / 8; ++l2) {
            for (int m2 = 0; m2 < NUM_OPTION_OCCURRENCES - 1; ++m2) {
                const int n2 = l2;
                    // instead of:
                    // for (int n2 = 0; n2 < NUM_OPTION_DEFAULT_VALUES / 2; ...
                    // and only because of the way we organized the table data:
                    //..
                       ASSERT(OPTION_TYPEINFO[l2].d_type ==
                                             OPTION_DEFAULT_VALUES[n2].d_type);
                    //..
                const int       LINE6  = OPTION_TAGS[i2].d_line;
                const char     *TAG2   = OPTION_TAGS[i2].d_tag;
                const int       LINE7  = OPTION_NAMES[j2].d_line;
                const char     *NAME2  = OPTION_NAMES[j2].d_name;
                const int       LINE8  = OPTION_DESCRIPTIONS[k2].d_line;
                const char     *DESC2  = OPTION_DESCRIPTIONS[k2].d_description;
                const int       LINE9  = OPTION_TYPEINFO[l2].d_line;
                const BdemType  TYPE2  = OPTION_TYPEINFO[l2].d_type;
                const int       LINE10 = OPTION_OCCURRENCES[m2].d_line;

                const OccurrenceType OTYPE2 = OPTION_OCCURRENCES[m2].d_type;

                void *VARIABLE2   = OPTION_TYPEINFO[l2].d_linkedVariable_p;
                void *CONSTRAINT2 = OPTION_TYPEINFO[l2].d_constraint_p;

                const void *DEFAULT_VALUE2 = OPTION_DEFAULT_VALUES[n2].d_value;

                if (TYPE2 != OPTION_DEFAULT_VALUES[n2].d_type) {
                    continue;
                }

                const balb::CommandLineOptionInfo EXP2 = {
                    TAG2,
                    NAME2,
                    DESC2,
                    createTypeInfo(TYPE2, VARIABLE2, CONSTRAINT2),
                    createOccurrenceInfo(OTYPE2, TYPE2, DEFAULT_VALUE2)
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
        ASSERT(0 == testAllocator.numBytesInUse());
        ASSERT(0 == testAllocator.numMismatches());

        if (verbose) cout << "\t\tTesting self-assignment (Aliasing)." << endl;

        for (int i = 0; i < NUM_OPTION_TAGS; ++i) {
        for (int j = 0; j < NUM_OPTION_NAMES; ++j) {
        for (int k = 0; k < NUM_OPTION_DESCRIPTIONS; ++k) {
        for (int l = 0; l < NUM_OPTION_TYPEINFO; ++l) {
        for (int m = 0; m < NUM_OPTION_OCCURRENCES; ++m) {
            const int n = l % NUM_OPTION_DEFAULT_VALUES;
                // Instead of:
                //   for (int n = 0; n < NUM_OPTION_DEFAULT_VALUES; ++n)
                // Valid only because of the way we organized the table data:
                //..
                   ASSERT(OPTION_TYPEINFO[l].d_type ==
                                              OPTION_DEFAULT_VALUES[n].d_type);
                //..
            const int       LINE1      = OPTION_TAGS[i].d_line;
            const char     *TAG        = OPTION_TAGS[i].d_tag;
            const int       LINE2      = OPTION_NAMES[j].d_line;
            const char     *NAME       = OPTION_NAMES[j].d_name;
            const int       LINE3      = OPTION_DESCRIPTIONS[k].d_line;
            const char     *DESC       = OPTION_DESCRIPTIONS[k].d_description;
            const int       LINE4      = OPTION_TYPEINFO[l].d_line;
            const BdemType  TYPE       = OPTION_TYPEINFO[l].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[l].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[l].d_constraint_p;

            const int            LINE5    = OPTION_OCCURRENCES[m].d_line;
            const OccurrenceType OTYPE    = OPTION_OCCURRENCES[m].d_type;
            const void     *DEFAULT_VALUE = OPTION_DEFAULT_VALUES[n].d_value;

            if (TYPE != OPTION_DEFAULT_VALUES[n].d_type) {
                continue;
            }

            const balb::CommandLineOptionInfo EXP = {
                TAG,
                NAME,
                DESC,
                createTypeInfo(TYPE, VARIABLE, CONSTRAINT),
                createOccurrenceInfo(OTYPE, TYPE, DEFAULT_VALUE)
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
        ASSERT(0 == testAllocator.numBytesInUse());
        ASSERT(0 == testAllocator.numMismatches());

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING balb::CommandLineOptionInfo
        //
        // Concerns: 'balb::CommandLineOptionInfo' is merely a 'struct' with no
        //   user-defined member functions, and five members.  We need only
        //   verify that it can be initialized on the stack.
        //
        // Plan:  Use stack-created instances with a variable number of
        //   initializers.  Run stack-initialization in a loop to make sure all
        //   memory gets de-allocated properly.
        //
        // Testing:
        //   class balb::CommandLineOptionInfo;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "balb::CommandLineOptionInfo" << endl
                          << "==========================" << endl;

        bslma::TestAllocator defaultAllocator(veryVeryVerbose);

        bool linkedFlag;
        bsl::string defaultValue("default");
        bsl::string tagString("genericTag");

        bslma::DefaultAllocatorGuard guard(&defaultAllocator);

        for (int i = 0; i < 10; ++i) {

        balb::CommandLineOptionInfo specTable[] = {
          {
          },
          {
            tagString                                     // tag
          },
          {
            "s|longTag"                                   // tag
          },
          {
            "s|longTag",                                  // tag
            "option name"                                 // name
          },
          {
            "s|longTag",                                  // tag
            "option name",                                // name
            "option description"                          // description
          },
          {
            "s|longTag",                                  // tag
            "option name",                                // name
            "option description",                         // description
            balb::CommandLineTypeInfo(&linkedFlag)         // linked variable
          },
          {
            "s|longTag",                                  // tag
            "option name",                                // name
            "option description",                         // description
            balb::CommandLineTypeInfo(&linkedFlag),        // linked variable
            balb::CommandLineOccurrenceInfo::BAEA_REQUIRED // occurrence info
          },
          {
            "s|longTag",                                  // tag
            "option name",                                // name
            "option description",                         // description
            balb::CommandLineTypeInfo(&linkedFlag),        // linked variable
            balb::CommandLineOccurrenceInfo(defaultValue)  // occurrence info
          }
        };
        (void)specTable;

        }
        ASSERT(0 == defaultAllocator.numBytesInUse());
        ASSERT(0 == defaultAllocator.numMismatches());

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING balb::CommandLineTypeInfo
        //
        // Concerns:  'balb::CommandLineTypeInfo' is a simple unconstrained
        //   attribute class that takes a 'bdema' allocator.  The concerns are
        //   generally the same as for any value-semantic type that takes a
        //   'bdema' allocator.
        //
        // Plan: This class has redundant manipulators and accessors, so we
        //   choose the primary manipulators to be:
        //     - balb::CommandLineOccurrenceInfo(bslma::Allocator *allocator);
        //     - operator=(.... *variable);
        //     - void setConstraint(
        //                 const balb::CommandLineConstraint::... & constraint);
        //     - void resetConstraint();
        //   and the primary accessors to be:
        //     - bdlmxxx::ElemType::Type elementType() const;
        //     - balb::CommandLine_Constraint *constraint() const;
        //     - void *linkedVariable() const;
        //   We follow the standard structure of a value-semantic test driver.
        //   Note that the 'constraint' accessor returns a pointer to a local
        //   object, owned by the test object, and therefore we cannot know the
        //   return value, but we can however verify that it is not the same
        //   value as the default constraint object.
        //
        // Testing:
        //   class balb::CommandLineTypeInfo;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "balb::CommandLineTypeInfo" << endl
                          << "========================" << endl;

        typedef TypeInfo Obj;

        bslma::TestAllocator testAllocator(veryVeryVerbose);
        bslma::TestAllocator defaultAllocator(veryVeryVerbose);

        bslma::DefaultAllocatorGuard guard(&defaultAllocator);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 2 OF VALUE-SEMANTIC TEST DRIVER
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP)

        if (verbose) cout << "\n\tTesting primary manipulators." << endl;

        if (verbose) cout << "\t\tWithout passing in an allocator." << endl;

        for (int i = 0; i < NUM_OPTION_TYPEINFO; ++i) {
            const int       LINE       = OPTION_TYPEINFO[i].d_line;
            const BdemType  TYPE       = OPTION_TYPEINFO[i].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[i].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[i].d_constraint_p;

            Obj mX;  const Obj& X = mX;      // TEST HERE

            if (veryVerbose) {
                T_(); T_(); T_(); P_(LINE); P_(i); P(TYPE);
                T_(); T_(); T_(); P_(VARIABLE); P(CONSTRAINT);
                T_(); T_(); T_(); P_(LINE); P_(i); P(X);
            }
            LOOP_ASSERT(LINE, ET::BDEM_STRING == X.type());
            LOOP_ASSERT(LINE, !X.linkedVariable());

            setType(&mX, TYPE);

            LOOP_ASSERT(LINE, TYPE == X.type());
            LOOP_ASSERT(LINE, !X.linkedVariable());

            bsl::shared_ptr<balb::CommandLine_Constraint> DEFAULT_CONSTRAINT
                                                              = X.constraint();

            if (VARIABLE) {
                setLinkedVariable(&mX, TYPE, VARIABLE);

                if (veryVerbose) {
                    T_(); T_(); T_(); P_(LINE); P_(i); P(X);
                }
                LOOP_ASSERT(LINE, TYPE               == X.type());
                LOOP_ASSERT(LINE, VARIABLE           == X.linkedVariable());
                LOOP_ASSERT(LINE, DEFAULT_CONSTRAINT == X.constraint());

                mX.resetLinkedVariable();

                LOOP_ASSERT(LINE, TYPE == X.type());
                LOOP_ASSERT(LINE, !X.linkedVariable());
                LOOP_ASSERT(LINE, DEFAULT_CONSTRAINT == X.constraint());
            }

            mX.resetLinkedVariable();

            LOOP_ASSERT(LINE, TYPE == X.type());
            LOOP_ASSERT(LINE, !X.linkedVariable());
            LOOP_ASSERT(LINE, DEFAULT_CONSTRAINT == X.constraint());

            if (CONSTRAINT) {
                setConstraint(&mX, TYPE, CONSTRAINT);

                if (veryVerbose) {
                    T_(); T_(); T_(); P_(LINE); P_(i); P(X);
                }
                LOOP_ASSERT(LINE, TYPE               == X.type());
                LOOP_ASSERT(LINE, 0                  == X.linkedVariable());
                LOOP_ASSERT(LINE, DEFAULT_CONSTRAINT != X.constraint());

                mX.resetConstraint();

                LOOP_ASSERT(LINE, TYPE               == X.type());
                LOOP_ASSERT(LINE, 0                  == X.linkedVariable());
                LOOP_ASSERT(LINE, DEFAULT_CONSTRAINT == X.constraint());
            }

            mX.resetConstraint();

            LOOP_ASSERT(LINE, TYPE               == X.type());
            LOOP_ASSERT(LINE, 0                  == X.linkedVariable());
            LOOP_ASSERT(LINE, DEFAULT_CONSTRAINT == X.constraint());

            if (VARIABLE && CONSTRAINT) {
                setLinkedVariable(&mX, TYPE, VARIABLE);
                setConstraint(&mX, TYPE, CONSTRAINT);

                if (veryVerbose) {
                    T_(); T_(); T_(); P_(LINE); P_(i); P(X);
                }
                LOOP_ASSERT(LINE, TYPE               == X.type());
                LOOP_ASSERT(LINE, VARIABLE           == X.linkedVariable());
                LOOP_ASSERT(LINE, DEFAULT_CONSTRAINT != X.constraint());

                mX.resetLinkedVariable();

                LOOP_ASSERT(LINE, TYPE               == X.type());
                LOOP_ASSERT(LINE, 0                  == X.linkedVariable());
                LOOP_ASSERT(LINE, DEFAULT_CONSTRAINT == X.constraint());
            }
        }

        if (verbose) cout << "\t\tPassing in an allocator." << endl
                           << "\t\t\tWith no exceptions." << endl;

        const bool HAS_BSLMA_ALLOCATOR_TRAIT =
             bslalg::HasTrait<Obj, bslalg::TypeTraitUsesBslmaAllocator>::VALUE;

        ASSERT(HAS_BSLMA_ALLOCATOR_TRAIT);

        for (int i = 0; i < NUM_OPTION_TYPEINFO; ++i) {
            const int       LINE       = OPTION_TYPEINFO[i].d_line;
            const BdemType  TYPE       = OPTION_TYPEINFO[i].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[i].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[i].d_constraint_p;

            const int NUM_BYTES = defaultAllocator.numBytesInUse();
            {
                Obj mX(&testAllocator);  const Obj& X = mX;

                createTypeInfo(&mX, TYPE, VARIABLE, CONSTRAINT);

                Obj mY(&testAllocator);  const Obj& Y = mY; setType(&mY, TYPE);
                bsl::shared_ptr<balb::CommandLine_Constraint> DEFAULT_CONSTRAINT
                                                              = Y.constraint();

                if (veryVerbose) {
                    T_(); T_(); T_(); P_(LINE); P_(i); P(X);
                }
                LOOP_ASSERT(LINE, TYPE     == X.type());
                LOOP_ASSERT(LINE, VARIABLE == X.linkedVariable());
                if (CONSTRAINT) {
                    LOOP_ASSERT(LINE, DEFAULT_CONSTRAINT != X.constraint());
                } else {
                    LOOP_ASSERT(LINE, DEFAULT_CONSTRAINT == X.constraint());
                }
            }
            LOOP_ASSERT(LINE, 0 == testAllocator.numBytesInUse());
            LOOP_ASSERT(LINE, 0 == testAllocator.numMismatches());
            LOOP_ASSERT(LINE, NUM_BYTES == defaultAllocator.numBytesInUse());
            // Note: setConstraint might allocate with 'defaultAllocator'.
        }

        if (verbose) cout << "\t\t\tWith exceptions." << endl;

        for (int i = 0; i < NUM_OPTION_TYPEINFO; ++i) {
            const int       LINE       = OPTION_TYPEINFO[i].d_line;
            const BdemType  TYPE       = OPTION_TYPEINFO[i].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[i].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[i].d_constraint_p;

            Obj mY(&testAllocator);  const Obj& Y = mY; setType(&mY, TYPE);
            bsl::shared_ptr<balb::CommandLine_Constraint> DEFAULT_CONSTRAINT
                                                              = Y.constraint();

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                Obj mX(&testAllocator);  const Obj& X = mX;

                createTypeInfo(&mX, TYPE, VARIABLE, CONSTRAINT);

                LOOP_ASSERT(LINE, TYPE     == X.type());
                LOOP_ASSERT(LINE, VARIABLE == X.linkedVariable());
                if (CONSTRAINT) {
                    LOOP_ASSERT(LINE, DEFAULT_CONSTRAINT != X.constraint());
                } else {
                    LOOP_ASSERT(LINE, DEFAULT_CONSTRAINT == X.constraint());
                }

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }
        ASSERT(0 == testAllocator.numBytesInUse());
        ASSERT(0 == testAllocator.numMismatches());

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 4 OF VALUE-SEMANTIC TEST DRIVER
        // TESTING BASIC ACCESSORS

        if (verbose) cout << "\n\tTesting basic accessors." << endl;

        for (int i = 0; i < NUM_OPTION_TYPEINFO; ++i) {
            const int       LINE       = OPTION_TYPEINFO[i].d_line;
            const BdemType  TYPE       = OPTION_TYPEINFO[i].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[i].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[i].d_constraint_p;

            Obj mX;  const Obj& X = mX;

            createTypeInfo(&mX, TYPE, VARIABLE, CONSTRAINT);

            Obj mY(&testAllocator);  const Obj& Y = mY; setType(&mY, TYPE);
            bsl::shared_ptr<balb::CommandLine_Constraint> DEFAULT_CONSTRAINT
                                                              = Y.constraint();

            if (veryVerbose) {
                T_(); T_(); P_(LINE); P_(TYPE); P_(VARIABLE); P(CONSTRAINT);
                T_(); T_(); P_(LINE); P(X);
            }
            LOOP_ASSERT(LINE, TYPE       == X.type());
            LOOP_ASSERT(LINE, VARIABLE   == X.linkedVariable());

            if (CONSTRAINT) {
                LOOP_ASSERT(LINE, DEFAULT_CONSTRAINT != X.constraint());
            } else {
                LOOP_ASSERT(LINE, DEFAULT_CONSTRAINT == X.constraint());
            }

            if (CONSTRAINT) {
                const int j = i % NUM_OPTION_DEFAULT_VALUES;
                const void *VALUE = OPTION_DEFAULT_VALUES[j].d_value;
                ASSERT(TYPE == OPTION_DEFAULT_VALUES[j].d_type);

                const bdlmxxx::Descriptor *DESCTOR =
                                     bdlmxxx::ElemAttrLookup::lookupTable()[TYPE];
                bdlmxxx::ConstElemRef ELEMENT(VALUE, DESCTOR);

                OptConstraint::constraintValue = true;
                ASSERT(X.checkConstraint(ELEMENT));
                ASSERT(X.checkConstraint(VALUE));
                OptConstraint::constraintValue = false;
                ASSERT(!X.checkConstraint(ELEMENT));
                ASSERT(!X.checkConstraint(VALUE));
                OptConstraint::constraintValue = true;
                ASSERT(X.checkConstraint(ELEMENT));
                ASSERT(X.checkConstraint(VALUE));
            }
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 5 OF VALUE-SEMANTIC TEST DRIVER
        // TESTING OUTPUT (<<) OPERATOR AND PRINT

        if (verbose)
            cout << "\n\tTesting output operator and 'print'." << endl;

        for (int i = 0; i < NUM_OPTION_TYPEINFO; ++i) {
            const int       LINE       = OPTION_TYPEINFO[i].d_line;
            const BdemType  TYPE       = OPTION_TYPEINFO[i].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[i].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[i].d_constraint_p;

            Obj mX;  const Obj& X = mX;

            createTypeInfo(&mX, TYPE, VARIABLE, CONSTRAINT);

            bsl::ostringstream oss1, oss2;
            X.print(oss1);                  // TEST HERE
            oss2 << X;                      // TEST HERE

            bsl::string ss1(oss1.str()), ss2(oss2.str());
            LOOP3_ASSERT(LINE, ss1, ss2, ss1 == ss2);
                // Note: oss[12].str() are temporaries!  Doesn't work well with
                // oss[12].str().c_str();  except on SunPro where temporary
                // lives till the end of the block.

            if (veryVerbose) {
                T_(); T_(); P(X);
                T_(); T_(); P(ss1);
                T_(); T_(); P(ss2);
            }

            const char *output = ss1.c_str();
            const int   ret    = parseTypeInfo(&output, X, output);
            LOOP2_ASSERT(LINE, ret, 0 == ret);
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 6 OF VALUE-SEMANTIC TEST DRIVER
        // TESTING EQUALITY OPERATOR
        // Note: Unless they share the constraint, two objects will always
        // differ.  We test both.

        if (verbose) cout << "\n\tTesting equality operator." << endl;

        if (verbose)
            cout << "\t\tCompare each pair of values (u, v) in W X W." << endl;

        for (int i = 0; i < NUM_OPTION_TYPEINFO; ++i) {
            const int       LINE1 = OPTION_TYPEINFO[i].d_line;
            const BdemType  TYPE1 = OPTION_TYPEINFO[i].d_type;

            void *VARIABLE1   = OPTION_TYPEINFO[i].d_linkedVariable_p;
            void *CONSTRAINT1 = OPTION_TYPEINFO[i].d_constraint_p;

            Obj mX;  const Obj& X = mX;

            createTypeInfo(&mX, TYPE1, VARIABLE1, CONSTRAINT1);

            for (int j = 0; j < NUM_OPTION_TYPEINFO; ++j) {
                const int       LINE2 = OPTION_TYPEINFO[j].d_line;
                const BdemType  TYPE2 = OPTION_TYPEINFO[j].d_type;

                void *VARIABLE2   = OPTION_TYPEINFO[j].d_linkedVariable_p;
                void *CONSTRAINT2 = OPTION_TYPEINFO[j].d_constraint_p;

                Obj mY;  const Obj& Y = mY;

                createTypeInfo(&mY, TYPE2, VARIABLE2, CONSTRAINT2);

                bool isSame = (TYPE1 == TYPE2) && (VARIABLE1 == VARIABLE2) &&
                              (CONSTRAINT1 == 0) && (CONSTRAINT2 == 0);
                LOOP2_ASSERT(LINE1, LINE2,  isSame == (X == Y));   // TEST HERE
                LOOP2_ASSERT(LINE1, LINE2, !isSame == (X != Y));   // TEST HERE

                if (TYPE1 == TYPE2) {
                    mY.setConstraint(X.constraint());

                    isSame = (TYPE1 == TYPE2) && (VARIABLE1 == VARIABLE2);
                    LOOP2_ASSERT(LINE1, LINE2,  isSame == (X == Y));
                    LOOP2_ASSERT(LINE1, LINE2, !isSame == (X != Y));
                }
            }
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 7 OF VALUE-SEMANTIC TEST DRIVER
        // TESTING COPY CONSTRUCTOR

        if (verbose) cout << "\n\tTesting copy constructor." << endl;

        if (verbose) cout << "\t\tWithout passing in an allocator." << endl;

        for (int i = 0; i < NUM_OPTION_TYPEINFO; ++i) {
            const int       LINE       = OPTION_TYPEINFO[i].d_line;
            const BdemType  TYPE       = OPTION_TYPEINFO[i].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[i].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[i].d_constraint_p;

            Obj mX;  const Obj& X = mX;

            createTypeInfo(&mX, TYPE, VARIABLE, CONSTRAINT);

            Obj mY(X);  const Obj& Y = mY;  // TEST HERE

            LOOP_ASSERT(LINE, X == Y);
        }

        if (verbose) cout << "\t\tPassing in an allocator." << endl
                           << "\t\t\tWith no exceptions." << endl;

        for (int i = 0; i < NUM_OPTION_TYPEINFO; ++i) {
            const int       LINE       = OPTION_TYPEINFO[i].d_line;
            const BdemType  TYPE       = OPTION_TYPEINFO[i].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[i].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[i].d_constraint_p;

            Obj mX;  const Obj& X = mX;

            createTypeInfo(&mX, TYPE, VARIABLE, CONSTRAINT);

            const int NUM_BYTES = defaultAllocator.numBytesInUse();
            const int NUM_ALLOC = defaultAllocator.numAllocations();
            {
                Obj mY(X, &testAllocator);  const Obj& Y = mY;  // TEST HERE

                LOOP_ASSERT(LINE, X == Y);
            }
            LOOP_ASSERT(LINE, 0 == testAllocator.numBytesInUse());
            LOOP_ASSERT(LINE, 0 == testAllocator.numMismatches());
            LOOP_ASSERT(LINE, NUM_BYTES == defaultAllocator.numBytesInUse());
            LOOP_ASSERT(LINE, NUM_ALLOC == defaultAllocator.numAllocations());
        }

        if (verbose) cout << "\t\t\tWith exceptions." << endl;

        for (int i = 0; i < NUM_OPTION_TYPEINFO; ++i) {
            const BdemType  TYPE       = OPTION_TYPEINFO[i].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[i].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[i].d_constraint_p;

            Obj mX;  const Obj& X = mX;

            createTypeInfo(&mX, TYPE, VARIABLE, CONSTRAINT);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                Obj mY(X, &testAllocator);  const Obj& Y = mY;  // TEST HERE

                ASSERT(X == Y);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }
        ASSERT(0 == testAllocator.numBytesInUse());
        ASSERT(0 == testAllocator.numMismatches());

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 9 OF VALUE-SEMANTIC TEST DRIVER
        // TESTING ASSIGNMENT OPERATOR

        if (verbose) cout << "\n\tTesting assignment operator." << endl;

        if (verbose) cout << "\t\tTesting assignment u = v." << endl;

        for (int i = 0; i < NUM_OPTION_TYPEINFO; ++i) {
            const int       LINE1 = OPTION_TYPEINFO[i].d_line;
            const BdemType  TYPE1 = OPTION_TYPEINFO[i].d_type;

            void *VARIABLE1   = OPTION_TYPEINFO[i].d_linkedVariable_p;
            void *CONSTRAINT1 = OPTION_TYPEINFO[i].d_constraint_p;

            Obj mX;  const Obj& X = mX;

            createTypeInfo(&mX, TYPE1, VARIABLE1, CONSTRAINT1);

            for (int j = 0; j < NUM_OPTION_TYPEINFO; ++j) {
                const int       LINE2 = OPTION_TYPEINFO[j].d_line;
                const BdemType  TYPE2 = OPTION_TYPEINFO[j].d_type;

                void *VARIABLE2   = OPTION_TYPEINFO[j].d_linkedVariable_p;
                void *CONSTRAINT2 = OPTION_TYPEINFO[j].d_constraint_p;

                Obj mY;  const Obj& Y = mY;

                createTypeInfo(&mY, TYPE2, VARIABLE2, CONSTRAINT2);

                mY = X;  // TEST HERE

                LOOP2_ASSERT(LINE1, LINE2, X == Y);
            }
        }
        ASSERT(0 == testAllocator.numBytesInUse());
        ASSERT(0 == testAllocator.numMismatches());

        if (verbose) cout << "\t\tTesting self-assignment (Aliasing)." << endl;

        for (int i = 0; i < NUM_OPTION_TYPEINFO; ++i) {
            const int       LINE = OPTION_TYPEINFO[i].d_line;
            const BdemType  TYPE = OPTION_TYPEINFO[i].d_type;

            void *VARIABLE   = OPTION_TYPEINFO[i].d_linkedVariable_p;
            void *CONSTRAINT = OPTION_TYPEINFO[i].d_constraint_p;

            Obj mX;  const Obj& X = mX;

            createTypeInfo(&mX, TYPE, VARIABLE, CONSTRAINT);

            Obj mZ(X);  const Obj& Z = mZ;

            mX = X;  // TEST HERE

            LOOP_ASSERT(LINE, Z == X);
        }
        ASSERT(0 == testAllocator.numBytesInUse());
        ASSERT(0 == testAllocator.numMismatches());

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // TESTING ADDITIONAL CONSTRUCTORS

        if (verbose) cout << "\nTesting default-value constructors.";

        ASSERT(0 == testAllocator.numBytesInUse());
        ASSERT(0 == testAllocator.numMismatches());

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING CLASS 'balb::CommandLineConstraint'
        //
        // Concerns:  This class provides a namespace for a set of useful
        //   constraints for each of the 'bdem' element types.  We need only
        //   make sure that the types correspond to the expected function
        //   types.
        //
        // Plan:  Instantiate a constraint for each type, and load a function
        //   pointer of the appropriate prototype.
        //
        // Testing:
        //   class balb::CommandLineConstraint;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "balb::CommandLineConstraint" << endl
                          << "==========================" << endl;

        Constraint::CharConstraint        charF(&testCharConstraint);
        Constraint::ShortConstraint       shortF(&testShortConstraint);
        Constraint::IntConstraint         intF(&testIntConstraint);
        Constraint::Int64Constraint       int64F(&testInt64Constraint);
        Constraint::FloatConstraint       floatF(&testFloatConstraint);
        Constraint::DoubleConstraint      doubleF(&testDoubleConstraint);
        Constraint::StringConstraint      stringF(&testStringConstraint);
        Constraint::DatetimeConstraint    datetimeF(&testDatetimeConstraint);
        Constraint::DateConstraint        dateF(&testDateConstraint);
        Constraint::TimeConstraint        timeF(&testTimeConstraint);

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING CLASS 'balb::CommandLineOptionType'
        //
        // Concerns:  That the class data members have well-defined names that
        //   match the 'bdem' element type enumeration names, and that their
        //   types match the expected 'bdem' type.
        //
        // Plan:  Simply test the names of the class data members and (although
        //   it is not documented), assert that their value is 0.
        //
        // Testing:
        //   class balb::CommandLineOptionType;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "balb::CommandLineOptionType" << endl
                          << "==========================" << endl;

         ASSERT(checkOptionType<ET::BDEM_BOOL>    (OptionType::BAEA_BOOL));

         ASSERT(checkOptionType<ET::BDEM_CHAR>    (OptionType::BAEA_CHAR));
         ASSERT(checkOptionType<ET::BDEM_SHORT>   (OptionType::BAEA_SHORT));
         ASSERT(checkOptionType<ET::BDEM_INT>     (OptionType::BAEA_INT));
         ASSERT(checkOptionType<ET::BDEM_INT64>   (OptionType::BAEA_INT64));
         ASSERT(checkOptionType<ET::BDEM_FLOAT>   (OptionType::BAEA_FLOAT));
         ASSERT(checkOptionType<ET::BDEM_DOUBLE>  (OptionType::BAEA_DOUBLE));
         ASSERT(checkOptionType<ET::BDEM_STRING>  (OptionType::BAEA_STRING));
         ASSERT(checkOptionType<ET::BDEM_DATETIME>(OptionType::BAEA_DATETIME));
         ASSERT(checkOptionType<ET::BDEM_DATE>    (OptionType::BAEA_DATE));
         ASSERT(checkOptionType<ET::BDEM_TIME>    (OptionType::BAEA_TIME));

         ASSERT(checkOptionType<ET::BDEM_CHAR_ARRAY>(
                                             OptionType::BAEA_CHAR_ARRAY));
         ASSERT(checkOptionType<ET::BDEM_SHORT_ARRAY>(
                                             OptionType::BAEA_SHORT_ARRAY));
         ASSERT(checkOptionType<ET::BDEM_INT_ARRAY>(
                                             OptionType::BAEA_INT_ARRAY));
         ASSERT(checkOptionType<ET::BDEM_INT64_ARRAY>(
                                             OptionType::BAEA_INT64_ARRAY));
         ASSERT(checkOptionType<ET::BDEM_FLOAT_ARRAY>(
                                             OptionType::BAEA_FLOAT_ARRAY));
         ASSERT(checkOptionType<ET::BDEM_DOUBLE_ARRAY>(
                                             OptionType::BAEA_DOUBLE_ARRAY));
         ASSERT(checkOptionType<ET::BDEM_STRING_ARRAY>(
                                             OptionType::BAEA_STRING_ARRAY));
         ASSERT(checkOptionType<ET::BDEM_DATETIME_ARRAY>(
                                             OptionType::BAEA_DATETIME_ARRAY));
         ASSERT(checkOptionType<ET::BDEM_DATE_ARRAY>(
                                             OptionType::BAEA_DATE_ARRAY));
         ASSERT(checkOptionType<ET::BDEM_TIME_ARRAY>(
                                             OptionType::BAEA_TIME_ARRAY));

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING CLASS 'balb::CommandLineOccurrenceInfo'
        //
        // Concerns:  'balb::CommandLineOccurrenceInfo' is a simple
        //   unconstrained attribute class that takes a 'bdema' allocator.
        //   The concerns are generally the same as for any value-semantic type
        //   that takes a 'bdema' allocator.
        //
        // Plan: This class has redundant manipulators and accessors, so we
        //   choose the primary manipulators to be:
        //     - balb::CommandLineOccurrenceInfo(bslma::Allocator *allocator);
        //     - balb::CommandLineOccurrenceInfo(OccurrenceType    type,
        //                                      bslma::Allocator *allocator);
        //     - void setDefaultValue(bdlmxxx::ConstElemRef value);
        //   and the primary accessors to be:
        //     - OccurrenceType occurrenceType() const;
        //     - bdlmxxx::ConstElemRef hasDefaultValue() const;
        //     - bdlmxxx::ConstElemRef defaultValue() const;
        //   We follow the standard structure of a value-semantic test driver,
        //   with the twist that we test a very simple non-primary manipulator
        //   ('setHidden') in case 4, to avoid having to write a separate test
        //   case for it.
        //
        // Testing:
        //   class balb::CommandLineOccurrenceInfo;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "balb::CommandLineOccurrenceInfo" << endl
                          << "==============================" << endl;

        typedef OccurrenceInfo  Obj;

        bslma::TestAllocator testAllocator(veryVeryVerbose);
        bslma::TestAllocator defaultAllocator(veryVeryVerbose);

        bslma::DefaultAllocatorGuard guard(&defaultAllocator);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 2 OF VALUE-SEMANTIC TEST DRIVER
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP)

        if (verbose) cout << "\n\tTesting primary manipulators." << endl;

        if (verbose) cout << "\t\tWithout passing in an allocator." << endl;

        for (int i = 0; i < NUM_OPTION_OCCURRENCES; ++i) {
        for (int j = 0; j < NUM_OPTION_DEFAULT_VALUES; ++j) {
            const int            LINE1 = OPTION_OCCURRENCES[i].d_line;
            const OccurrenceType OTYPE = OPTION_OCCURRENCES[i].d_type;
            const int            LINE2 = OPTION_DEFAULT_VALUES[j].d_line;
            const BdemType       ETYPE = OPTION_DEFAULT_VALUES[j].d_type;

            const void            *ADDRESS = OPTION_DEFAULT_VALUES[j].d_value;
            const bdlmxxx::Descriptor *DESCTOR =
                                    bdlmxxx::ElemAttrLookup::lookupTable()[ETYPE];

            Obj mX(OTYPE);  const Obj& X = mX;      // TEST HERE

            if (veryVerbose) {
                T_(); T_(); T_(); P_(i); P_(j); P(X);
            }
            LOOP2_ASSERT(LINE1, LINE2, !X.hasDefaultValue());
            LOOP2_ASSERT(LINE1, LINE2, OTYPE == X.occurrenceType());

            if (OTYPE != OccurrenceInfo::BAEA_REQUIRED && ADDRESS) {
                bdlmxxx::ConstElemRef DEFAULT_VALUE(ADDRESS, DESCTOR);

                mX.setDefaultValue(DEFAULT_VALUE);  // AND HERE

                if (veryVerbose) {
                    T_(); T_(); T_(); P_(i); P_(j); P(X);
                }
                LOOP2_ASSERT(LINE1, LINE2, X.hasDefaultValue());
                LOOP2_ASSERT(LINE1, LINE2, OTYPE == X.occurrenceType());
                LOOP2_ASSERT(LINE1, LINE2, ETYPE == X.defaultValue().type());
                LOOP2_ASSERT(LINE1, LINE2, DEFAULT_VALUE == X.defaultValue());
            }
        }
        }

        if (verbose) cout << "\t\tPassing in an allocator." << endl
                           << "\t\t\tWith no exceptions." << endl;

        const bool HAS_BSLMA_ALLOCATOR_TRAIT =
             bslalg::HasTrait<Obj, bslalg::TypeTraitUsesBslmaAllocator>::VALUE;

        ASSERT(HAS_BSLMA_ALLOCATOR_TRAIT);

        for (int i = 0; i < NUM_OPTION_OCCURRENCES; ++i) {
        for (int j = 0; j < NUM_OPTION_DEFAULT_VALUES; ++j) {
            const int            LINE1 = OPTION_OCCURRENCES[i].d_line;
            const OccurrenceType OTYPE = OPTION_OCCURRENCES[i].d_type;
            const int            LINE2 = OPTION_DEFAULT_VALUES[j].d_line;
            const ET::Type ETYPE = OPTION_DEFAULT_VALUES[j].d_type;

            const void            *ADDRESS = OPTION_DEFAULT_VALUES[j].d_value;
            const bdlmxxx::Descriptor *DESCTOR =
                                    bdlmxxx::ElemAttrLookup::lookupTable()[ETYPE];

            const int NUM_BYTES = defaultAllocator.numBytesInUse();
            const int NUM_ALLOC = defaultAllocator.numAllocations();
            {
                Obj mX(OTYPE, &testAllocator);  const Obj& X = mX;

                if (veryVerbose) {
                    T_(); T_(); T_(); P_(i); P_(j); P(X);
                }
                LOOP2_ASSERT(LINE1, LINE2, !X.hasDefaultValue());
                LOOP2_ASSERT(LINE1, LINE2, OTYPE == X.occurrenceType());

                if (OTYPE != OccurrenceInfo::BAEA_REQUIRED && ADDRESS) {
                    bdlmxxx::ConstElemRef DEFAULT_VALUE(ADDRESS, DESCTOR);

                    mX.setDefaultValue(DEFAULT_VALUE);

                    if (veryVerbose) {
                        T_(); T_(); T_(); P_(i); P_(j); P(X);
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

        if (verbose) cout << "\t\t\tWith exceptions." << endl;

        for (int i = 0; i < NUM_OPTION_OCCURRENCES; ++i) {
        for (int j = 0; j < NUM_OPTION_DEFAULT_VALUES; ++j) {
            const int            LINE1 = OPTION_OCCURRENCES[i].d_line;
            const OccurrenceType OTYPE = OPTION_OCCURRENCES[i].d_type;
            const int            LINE2 = OPTION_DEFAULT_VALUES[j].d_line;
            const BdemType       ETYPE = OPTION_DEFAULT_VALUES[j].d_type;

            const void            *ADDRESS = OPTION_DEFAULT_VALUES[j].d_value;
            const bdlmxxx::Descriptor *DESCTOR =
                                    bdlmxxx::ElemAttrLookup::lookupTable()[ETYPE];

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                Obj mX(OTYPE, &testAllocator);  const Obj& X = mX;

                LOOP2_ASSERT(LINE1, LINE2, !X.hasDefaultValue());
                LOOP2_ASSERT(LINE1, LINE2, OTYPE == X.occurrenceType());

                if (OTYPE != OccurrenceInfo::BAEA_REQUIRED && ADDRESS) {
                    bdlmxxx::ConstElemRef DEFAULT_VALUE(ADDRESS, DESCTOR);

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
        ASSERT(0 == testAllocator.numBytesInUse());
        ASSERT(0 == testAllocator.numMismatches());

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 4 OF VALUE-SEMANTIC TEST DRIVER
        // TESTING BASIC ACCESSORS

        if (verbose) cout << "\n\tTesting basic accessors." << endl;

        for (int i = 0; i < NUM_OPTION_OCCURRENCES; ++i) {
        for (int j = 0; j < NUM_OPTION_DEFAULT_VALUES; ++j) {
            const int            LINE1 = OPTION_OCCURRENCES[i].d_line;
            const OccurrenceType OTYPE = OPTION_OCCURRENCES[i].d_type;
            const int            LINE2 = OPTION_DEFAULT_VALUES[j].d_line;
            const BdemType       ETYPE = OPTION_DEFAULT_VALUES[j].d_type;

            const void            *ADDRESS = OPTION_DEFAULT_VALUES[j].d_value;
            const bdlmxxx::Descriptor *DESCTOR =
                                    bdlmxxx::ElemAttrLookup::lookupTable()[ETYPE];

            Obj mX(OTYPE);  const Obj& X = mX;

            LOOP2_ASSERT(LINE1, LINE2, !X.hasDefaultValue());
            LOOP2_ASSERT(LINE1, LINE2, OTYPE == X.occurrenceType());
            LOOP2_ASSERT(LINE1, LINE2,
                   (OTYPE == OccurrenceInfo::BAEA_HIDDEN)   == X.isHidden());
            LOOP2_ASSERT(LINE1, LINE2,
                   (OTYPE == OccurrenceInfo::BAEA_REQUIRED) == X.isRequired());

            if (OTYPE != OccurrenceInfo::BAEA_REQUIRED && ADDRESS) {
                bdlmxxx::ConstElemRef DEFAULT_VALUE(ADDRESS, DESCTOR);

                mX.setDefaultValue(DEFAULT_VALUE);

                LOOP2_ASSERT(LINE1, LINE2, X.hasDefaultValue());
                LOOP2_ASSERT(LINE1, LINE2, OTYPE == X.occurrenceType());
                LOOP2_ASSERT(LINE1, LINE2, ETYPE == X.defaultValue().type());
                LOOP2_ASSERT(LINE1, LINE2, DEFAULT_VALUE == X.defaultValue());
                LOOP2_ASSERT(LINE1, LINE2, ADDRESS != X.defaultValue().data());

                mX.setHidden();  // TEST EXTRA MANIPULATOR

                LOOP2_ASSERT(LINE1, LINE2,
                            OccurrenceInfo::BAEA_HIDDEN == X.occurrenceType());

                LOOP2_ASSERT(LINE1, LINE2, X.hasDefaultValue());
                LOOP2_ASSERT(LINE1, LINE2, ETYPE == X.defaultValue().type());
                LOOP2_ASSERT(LINE1, LINE2, DEFAULT_VALUE == X.defaultValue());
                LOOP2_ASSERT(LINE1, LINE2, ADDRESS != X.defaultValue().data());
                LOOP2_ASSERT(LINE1, LINE2, X.isHidden());
            }
        }
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 5 OF VALUE-SEMANTIC TEST DRIVER
        // TESTING OUTPUT (<<) OPERATOR AND PRINT

        if (verbose)
            cout << "\n\tTesting output operator and 'print'." << endl;

        typedef bdlpuxxx::ParserImpUtil Parser;

        for (int i = 0; i < NUM_OPTION_OCCURRENCES; ++i) {
        for (int j = 0; j < NUM_OPTION_DEFAULT_VALUES; ++j) {
            const OccurrenceType OTYPE = OPTION_OCCURRENCES[i].d_type;
            const BdemType       ETYPE = OPTION_DEFAULT_VALUES[j].d_type;

            const void            *ADDRESS = OPTION_DEFAULT_VALUES[j].d_value;
            const bdlmxxx::Descriptor *DESCTOR =
                                    bdlmxxx::ElemAttrLookup::lookupTable()[ETYPE];

            Obj mX(OTYPE);  const Obj& X = mX;

            if (OTYPE != OccurrenceInfo::BAEA_REQUIRED && ADDRESS) {
                bdlmxxx::ConstElemRef DEFAULT_VALUE(ADDRESS, DESCTOR);

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
                T_(); T_(); P(X);
                T_(); T_(); P(ss1);
                T_(); T_(); P(ss2);
            }

            const char *output = ss1.c_str();
            const int parseRet = parseOccurrenceInfo(&output, X, output);
            LOOP2_ASSERT(parseRet, output, 0 == parseRet);
        }
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 6 OF VALUE-SEMANTIC TEST DRIVER
        // TESTING EQUALITY OPERATOR

        if (verbose) cout << "\n\tTesting equality operator." << endl;

        if (verbose)
            cout << "\t\tCompare each pair of values (u, v) in W X W." << endl;

        for (int i = 0; i < NUM_OPTION_OCCURRENCES; ++i) {
        for (int j = 0; j < NUM_OPTION_DEFAULT_VALUES; ++j) {
            const int            LINE1  = OPTION_OCCURRENCES[i].d_line;
            const OccurrenceType OTYPE1 = OPTION_OCCURRENCES[i].d_type;
            const int            LINE2  = OPTION_DEFAULT_VALUES[j].d_line;
            const BdemType       ETYPE2 = OPTION_DEFAULT_VALUES[j].d_type;

            const void            *ADDRESS1 = OPTION_DEFAULT_VALUES[j].d_value;
            const bdlmxxx::Descriptor *DESCTOR1 =
                                    bdlmxxx::ElemAttrLookup::lookupTable()[ETYPE2];

            Obj mX(OTYPE1);  const Obj& X = mX;

            if (OTYPE1 != OccurrenceInfo::BAEA_REQUIRED && ADDRESS1) {
                bdlmxxx::ConstElemRef DEFAULT_VALUE1(ADDRESS1, DESCTOR1);

                mX.setDefaultValue(DEFAULT_VALUE1);
            }

            for (int k = 0; k < NUM_OPTION_OCCURRENCES; ++k) {
            for (int l = 0; l < NUM_OPTION_DEFAULT_VALUES; ++l) {
                const int            LINE3  = OPTION_OCCURRENCES[k].d_line;
                const OccurrenceType OTYPE3 = OPTION_OCCURRENCES[k].d_type;
                const int            LINE4  = OPTION_DEFAULT_VALUES[l].d_line;
                const BdemType       ETYPE4 = OPTION_DEFAULT_VALUES[l].d_type;

                const void *ADDRESS2 = OPTION_DEFAULT_VALUES[l].d_value;
                const bdlmxxx::Descriptor *DESCTOR2 =
                                    bdlmxxx::ElemAttrLookup::lookupTable()[ETYPE4];

                Obj mY(OTYPE3);  const Obj& Y = mY;

                if (OTYPE3 != OccurrenceInfo::BAEA_REQUIRED && ADDRESS2) {
                    bdlmxxx::ConstElemRef DEFAULT_VALUE2(ADDRESS2, DESCTOR2);

                    mY.setDefaultValue(DEFAULT_VALUE2);
                }

                bool isSame = (i == k);
                if (OTYPE1 != OccurrenceInfo::BAEA_REQUIRED
                 && OTYPE3 != OccurrenceInfo::BAEA_REQUIRED) {
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

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 7 OF VALUE-SEMANTIC TEST DRIVER
        // TESTING COPY CONSTRUCTOR

        if (verbose) cout << "\n\tTesting copy constructor." << endl;

        if (verbose) cout << "\t\tWithout passing in an allocator." << endl;

        for (int i = 0; i < NUM_OPTION_OCCURRENCES; ++i) {
        for (int j = 0; j < NUM_OPTION_DEFAULT_VALUES; ++j) {
            const int            LINE1 = OPTION_OCCURRENCES[i].d_line;
            const OccurrenceType OTYPE = OPTION_OCCURRENCES[i].d_type;
            const int            LINE2 = OPTION_DEFAULT_VALUES[j].d_line;
            const BdemType       ETYPE = OPTION_DEFAULT_VALUES[j].d_type;

            const void            *ADDRESS = OPTION_DEFAULT_VALUES[j].d_value;
            const bdlmxxx::Descriptor *DESCTOR =
                                    bdlmxxx::ElemAttrLookup::lookupTable()[ETYPE];

            Obj mX(OTYPE);  const Obj& X = mX;

            if (OTYPE != OccurrenceInfo::BAEA_REQUIRED && ADDRESS) {
                bdlmxxx::ConstElemRef DEFAULT_VALUE(ADDRESS, DESCTOR);

                mX.setDefaultValue(DEFAULT_VALUE);
            }

            Obj mY(X);  const Obj& Y = mY;  // TEST HERE

            LOOP2_ASSERT(LINE1, LINE2, X == Y);
        }
        }

        if (verbose) cout << "\t\tPassing in an allocator." << endl
                           << "\t\t\tWith no exceptions." << endl;

        for (int i = 0; i < NUM_OPTION_OCCURRENCES; ++i) {
        for (int j = 0; j < NUM_OPTION_DEFAULT_VALUES; ++j) {
            const int            LINE1 = OPTION_OCCURRENCES[i].d_line;
            const OccurrenceType OTYPE = OPTION_OCCURRENCES[i].d_type;
            const int            LINE2 = OPTION_DEFAULT_VALUES[j].d_line;
            const BdemType       ETYPE = OPTION_DEFAULT_VALUES[j].d_type;

            const void            *ADDRESS = OPTION_DEFAULT_VALUES[j].d_value;
            const bdlmxxx::Descriptor *DESCTOR =
                                     bdlmxxx::ElemAttrLookup::lookupTable()[ETYPE];

            Obj mX(OTYPE);  const Obj& X = mX;

            if (OTYPE != OccurrenceInfo::BAEA_REQUIRED && ADDRESS) {
                bdlmxxx::ConstElemRef DEFAULT_VALUE(ADDRESS, DESCTOR);

                mX.setDefaultValue(DEFAULT_VALUE);
            }

            const int NUM_BYTES = defaultAllocator.numBytesInUse();
            const int NUM_ALLOC = defaultAllocator.numAllocations();
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

        if (verbose) cout << "\t\t\tWith exceptions." << endl;

        for (int i = 0; i < NUM_OPTION_OCCURRENCES; ++i) {
        for (int j = 0; j < NUM_OPTION_DEFAULT_VALUES; ++j) {
            const OccurrenceType OTYPE = OPTION_OCCURRENCES[i].d_type;
            const BdemType       ETYPE = OPTION_DEFAULT_VALUES[j].d_type;

            const void            *ADDRESS = OPTION_DEFAULT_VALUES[j].d_value;
            const bdlmxxx::Descriptor *DESCTOR =
                                     bdlmxxx::ElemAttrLookup::lookupTable()[ETYPE];

            Obj mX(OTYPE);  const Obj& X = mX;

            if (OTYPE != OccurrenceInfo::BAEA_REQUIRED && ADDRESS) {
                bdlmxxx::ConstElemRef DEFAULT_VALUE(ADDRESS, DESCTOR);

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
        ASSERT(0 == testAllocator.numBytesInUse());
        ASSERT(0 == testAllocator.numMismatches());

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 9 OF VALUE-SEMANTIC TEST DRIVER
        // TESTING ASSIGNMENT OPERATOR

        if (verbose) cout << "\n\tTesting assignment operator." << endl;

        if (verbose) cout << "\t\tTesting assignment u = v." << endl;

        for (int i = 0; i < NUM_OPTION_OCCURRENCES; ++i) {
        for (int j = 0; j < NUM_OPTION_DEFAULT_VALUES; ++j) {
            const int            LINE1  = OPTION_OCCURRENCES[i].d_line;
            const OccurrenceType OTYPE1 = OPTION_OCCURRENCES[i].d_type;
            const int            LINE2  = OPTION_DEFAULT_VALUES[j].d_line;
            const BdemType       ETYPE2 = OPTION_DEFAULT_VALUES[j].d_type;

            const void            *ADDRESS1 = OPTION_DEFAULT_VALUES[j].d_value;
            const bdlmxxx::Descriptor *DESCTOR1 =
                                    bdlmxxx::ElemAttrLookup::lookupTable()[ETYPE2];

            Obj mX(OTYPE1);  const Obj& X = mX;

            if (OTYPE1 != OccurrenceInfo::BAEA_REQUIRED && ADDRESS1) {
                bdlmxxx::ConstElemRef DEFAULT_VALUE1(ADDRESS1, DESCTOR1);

                mX.setDefaultValue(DEFAULT_VALUE1);
            }

            for (int k = 0; k < NUM_OPTION_OCCURRENCES; ++k) {
            for (int l = 0; l < NUM_OPTION_DEFAULT_VALUES; ++l) {
                const int            LINE3  = OPTION_OCCURRENCES[k].d_line;
                const OccurrenceType OTYPE3 = OPTION_OCCURRENCES[k].d_type;
                const int            LINE4  = OPTION_DEFAULT_VALUES[l].d_line;
                const BdemType       ETYPE4 = OPTION_DEFAULT_VALUES[l].d_type;

                const void *ADDRESS2 = OPTION_DEFAULT_VALUES[l].d_value;
                const bdlmxxx::Descriptor *DESCTOR2 =
                                    bdlmxxx::ElemAttrLookup::lookupTable()[ETYPE4];

                Obj mY(OTYPE3, &testAllocator);  const Obj& Y = mY;

                if (OTYPE3 != OccurrenceInfo::BAEA_REQUIRED && ADDRESS2) {
                    bdlmxxx::ConstElemRef DEFAULT_VALUE2(ADDRESS2, DESCTOR2);

                    mY.setDefaultValue(DEFAULT_VALUE2);
                }

                mY = X;  // TEST HERE

                LOOP4_ASSERT(LINE1, LINE2, LINE3, LINE4, X == Y);
            }
            }
        }
        }
        ASSERT(0 == testAllocator.numBytesInUse());
        ASSERT(0 == testAllocator.numMismatches());

        if (verbose) cout << "\t\tTesting self-assignment (Aliasing)." << endl;

        for (int i = 0; i < NUM_OPTION_OCCURRENCES; ++i) {
        for (int j = 0; j < NUM_OPTION_DEFAULT_VALUES; ++j) {
            const int              LINE1   = OPTION_OCCURRENCES[i].d_line;
            const OccurrenceType   OTYPE   = OPTION_OCCURRENCES[i].d_type;
            const int              LINE2   = OPTION_DEFAULT_VALUES[j].d_line;
            const BdemType         ETYPE   = OPTION_DEFAULT_VALUES[j].d_type;
            const void            *ADDRESS = OPTION_DEFAULT_VALUES[j].d_value;
            const bdlmxxx::Descriptor *DESCTOR =
                                     bdlmxxx::ElemAttrLookup::lookupTable()[ETYPE];

            Obj mX(OTYPE);  const Obj& X = mX;

            if (OTYPE != OccurrenceInfo::BAEA_REQUIRED && ADDRESS) {
                bdlmxxx::ConstElemRef DEFAULT_VALUE(ADDRESS, DESCTOR);

                mX.setDefaultValue(DEFAULT_VALUE);
            }

            Obj mZ(X);  const Obj& Z = mZ;

            mX = X;  // TEST HERE

            LOOP2_ASSERT(LINE1, LINE2, Z == X);
        }
        }
        ASSERT(0 == testAllocator.numBytesInUse());
        ASSERT(0 == testAllocator.numMismatches());

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // TESTING ADDITIONAL CONSTRUCTORS

        if (verbose) cout << "\nTesting default-value constructors.";

        for (int j = 0; j < NUM_OPTION_DEFAULT_VALUES; ++j) {
            const int              LINE1   = OPTION_OCCURRENCES[1].d_line;
            const OccurrenceType   OTYPE   = OccurrenceInfo::BAEA_OPTIONAL;
            const int              LINE2   = OPTION_DEFAULT_VALUES[j].d_line;
            const BdemType         ETYPE   = OPTION_DEFAULT_VALUES[j].d_type;
            const void            *ADDRESS = OPTION_DEFAULT_VALUES[j].d_value;
            const bdlmxxx::Descriptor *DESCTOR =
                                     bdlmxxx::ElemAttrLookup::lookupTable()[ETYPE];

            Obj mX(OTYPE);  const Obj& X = mX;

            if (OTYPE != OccurrenceInfo::BAEA_REQUIRED && ADDRESS) {
                bdlmxxx::ConstElemRef DEFAULT_VALUE(ADDRESS, DESCTOR);

                mX.setDefaultValue(DEFAULT_VALUE);
            }

            if (ADDRESS) {
                switch (ETYPE) {
                  case ET::BDEM_BOOL: {
                    Obj mY(*(const bool *)ADDRESS, &testAllocator);
                    const Obj& Y = mY;
                    LOOP2_ASSERT(LINE1, LINE2, X == Y);
                  } break;
                  case ET::BDEM_CHAR: {
                    Obj mY(*(const char *)ADDRESS, &testAllocator);
                    const Obj& Y = mY;
                    LOOP2_ASSERT(LINE1, LINE2, X == Y);
                  } break;
                  case ET::BDEM_SHORT: {
                    Obj mY(*(const short *)ADDRESS, &testAllocator);
                    const Obj& Y = mY;
                    LOOP2_ASSERT(LINE1, LINE2, X == Y);
                  } break;
                  case ET::BDEM_INT: {
                    Obj mY(*(const int *)ADDRESS, &testAllocator);
                    const Obj& Y = mY;
                    LOOP2_ASSERT(LINE1, LINE2, X == Y);
                  } break;
                  case ET::BDEM_INT64: {
                    Obj mY(*(const Int64 *)ADDRESS, &testAllocator);
                    const Obj& Y = mY;
                    LOOP2_ASSERT(LINE1, LINE2, X == Y);
                  } break;
                  case ET::BDEM_FLOAT: {
                    Obj mY(*(const float *)ADDRESS, &testAllocator);
                    const Obj& Y = mY;
                    LOOP2_ASSERT(LINE1, LINE2, X == Y);
                  } break;
                  case ET::BDEM_DOUBLE: {
                    Obj mY(*(const double *)ADDRESS, &testAllocator);
                    const Obj& Y = mY;
                    LOOP2_ASSERT(LINE1, LINE2, X == Y);
                  } break;
                  case ET::BDEM_STRING: {
                    Obj mY(*(const bsl::string *)ADDRESS, &testAllocator);
                    const Obj& Y = mY;
                    LOOP2_ASSERT(LINE1, LINE2, X == Y);
                  } break;
                  case ET::BDEM_DATETIME: {
                    Obj mY(*(const bdlt::Datetime *)ADDRESS, &testAllocator);
                    const Obj& Y = mY;
                    LOOP2_ASSERT(LINE1, LINE2, X == Y);
                  } break;
                  case ET::BDEM_DATE: {
                    Obj mY(*(const bdlt::Date *)ADDRESS, &testAllocator);
                    const Obj& Y = mY;
                    LOOP2_ASSERT(LINE1, LINE2, X == Y);
                  } break;
                  case ET::BDEM_TIME: {
                    Obj mY(*(const bdlt::Time *)ADDRESS, &testAllocator);
                    const Obj& Y = mY;
                    LOOP2_ASSERT(LINE1, LINE2, X == Y);
                  } break;
                  case ET::BDEM_CHAR_ARRAY: {
                    Obj mY(*(const bsl::vector<char> *)ADDRESS,
                             &testAllocator);
                    const Obj& Y = mY;
                    LOOP2_ASSERT(LINE1, LINE2, X == Y);
                  } break;
                  case ET::BDEM_SHORT_ARRAY: {
                    Obj mY(*(const bsl::vector<short> *)ADDRESS,
                           &testAllocator);
                    const Obj& Y = mY;
                    LOOP2_ASSERT(LINE1, LINE2, X == Y);
                  } break;
                  case ET::BDEM_INT_ARRAY: {
                    Obj mY(*(const bsl::vector<int> *)ADDRESS,
                           &testAllocator);
                    const Obj& Y = mY;
                    LOOP2_ASSERT(LINE1, LINE2, X == Y);
                  } break;
                  case ET::BDEM_INT64_ARRAY: {
                    Obj mY(*(const bsl::vector<Int64> *)ADDRESS,
                           &testAllocator);
                    const Obj& Y = mY;
                    LOOP2_ASSERT(LINE1, LINE2, X == Y);
                  } break;
                  case ET::BDEM_FLOAT_ARRAY: {
                    Obj mY(*(const bsl::vector<float> *)ADDRESS,
                           &testAllocator);
                    const Obj& Y = mY;
                    LOOP2_ASSERT(LINE1, LINE2, X == Y);
                  } break;
                  case ET::BDEM_DOUBLE_ARRAY: {
                    Obj mY(*(const bsl::vector<double> *)ADDRESS,
                           &testAllocator);
                    const Obj& Y = mY;
                    LOOP2_ASSERT(LINE1, LINE2, X == Y);
                  } break;
                  case ET::BDEM_STRING_ARRAY: {
                    Obj mY(*(const bsl::vector<bsl::string> *)ADDRESS,
                             &testAllocator);
                    const Obj& Y = mY;
                    LOOP2_ASSERT(LINE1, LINE2, X == Y);
                  } break;
                  case ET::BDEM_DATETIME_ARRAY: {
                    Obj mY(*(const bsl::vector<bdlt::Datetime> *)ADDRESS,
                           &testAllocator);
                    const Obj& Y = mY;
                    LOOP2_ASSERT(LINE1, LINE2, X == Y);
                  } break;
                  case ET::BDEM_DATE_ARRAY: {
                    Obj mY(*(const bsl::vector<bdlt::Date> *)ADDRESS,
                           &testAllocator);
                    const Obj& Y = mY;
                    LOOP2_ASSERT(LINE1, LINE2, X == Y);
                  } break;
                  case ET::BDEM_TIME_ARRAY: {
                    Obj mY(*(const bsl::vector<bdlt::Time> *)ADDRESS,
                           &testAllocator);
                    const Obj& Y = mY;
                    LOOP2_ASSERT(LINE1, LINE2, X == Y);
                  } break;
                  default: {
                    ASSERT(0);
                  } break;
                };
            }
        }
        ASSERT(0 == testAllocator.numBytesInUse());
        ASSERT(0 == testAllocator.numMismatches());

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PARSE_COMMAND_LINE TESTING UTILITY
        //   Exercise the basic functionality.
        //
        // Concerns:
        //   That the 'parseCommandLine' testing utility function separates a
        //   command line as needed into its argc and argv.
        //
        // Plan:
        //   It is enough to test with zero, one, or two arguments on the
        //   command line, and test with various spacing and separation
        //   characters.  For orthogonal perturbations, add a header or a
        //   trailer or both.
        //
        // Testing:
        //   parseCommandLine(...);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting parseCommandLine utility"
                          << "\n================================" << endl;

        const char *ONECMD[] = { "oneCommand" };
        const char *TWOCMD[] = { "two", "commands" };

        const char HEADER[]  = " \t ";
        const int  NHEADER   = sizeof HEADER - 1;  // strlen()

        const char TRAILER[] = " \t ";
        const int  NTRAILER  = sizeof TRAILER - 1; // strlen()

        static const struct {
            int          d_line;
            int          d_retCode;
            int          d_argc;
            const char **d_argv;
            const char  *d_cmdLine;
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
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i=0; i < NUM_DATA; ++i) {
            const int           LINE     = DATA[i].d_line;
            const int           RET      = DATA[i].d_retCode;
            const int           ARGC     = DATA[i].d_argc;
            const char * const *ARGV     = DATA[i].d_argv;
            const char *        CMD_LINE = DATA[i].d_cmdLine;

            {
                const int NLINE = bsl::strlen(CMD_LINE);
                char *cmdLine = new char[NLINE+1];
                bsl::strcpy(cmdLine, CMD_LINE);
                if (verbose) { T_(); P_(LINE); P(cmdLine); }

                int         argc;
                const char *argv[MAX_ARGS];
                const int   ret = parseCommandLine(cmdLine, argc, argv, 10);

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
                const int NLINE = bsl::strlen(CMD_LINE);
                char *cmdLine = new char[NLINE+NHEADER+1];
                bsl::strcpy(cmdLine,         HEADER);
                bsl::strcpy(cmdLine+NHEADER, CMD_LINE);
                if (verbose) { T_(); P_(LINE); P(cmdLine); }

                int         argc;
                const char *argv[MAX_ARGS];
                const int   ret = parseCommandLine(cmdLine, argc, argv, 10);

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
                const int NLINE = bsl::strlen(CMD_LINE);
                char *cmdLine = new char[NLINE+NTRAILER+1];
                bsl::strcpy(cmdLine,       CMD_LINE);
                bsl::strcpy(cmdLine+NLINE, TRAILER);
                if (verbose) { T_(); P_(LINE); P(cmdLine); }

                int         argc;
                const char *argv[MAX_ARGS];
                const int   ret = parseCommandLine(cmdLine, argc, argv, 10);

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
                const int NLINE = bsl::strlen(CMD_LINE);
                char *cmdLine = new char[NHEADER+NLINE+NTRAILER+1];
                bsl::strcpy(cmdLine,               HEADER);
                bsl::strcpy(cmdLine+NHEADER,       CMD_LINE);
                bsl::strcpy(cmdLine+NHEADER+NLINE, TRAILER);
                if (verbose) { T_(); P_(LINE); P(cmdLine); }

                int         argc;
                const char *argv[MAX_ARGS];

                const int ret = parseCommandLine(cmdLine, argc, argv, 10);
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
        // BREATHING TEST:
        //   Exercise the basic functionality.
        //
        // Concerns:
        //   Basic functionality must work as advertised for most common usage
        //   of this component.
        //
        // Plan:
        //
        // Testing:
        //   This Test Case exercises basic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        const int defaultNonOptionValue = 12345;

        int longOptionOnlyValue = -1;

        int nonOptionValue1 = -1;
        int nonOptionValue2 = -1;
        int nonOptionValue3 = -1;

        balb::CommandLineOptionInfo specTable[] = {
          {
            "f|flag",                                     // tag
            "genericFlag",                                // name
            "provide generic option for breathing test",  // description
            balb::CommandLineTypeInfo(balb::CommandLineOptionType::BAEA_BOOL),
                                                          // no linked variable
            balb::CommandLineOccurrenceInfo::BAEA_REQUIRED // occurrence info
          },
          {
            "s|long-option-with-dashes",                  // tag
            "longOptionValue",                            // name
            "provide generic option for breathing test",  // description
            balb::CommandLineTypeInfo(balb::CommandLineOptionType::
                                                               BAEA_INT_ARRAY),
                                                          // no linked variable
            balb::CommandLineOccurrenceInfo::BAEA_REQUIRED // occurrence info
          },
          {
            "long-option-only",                           // tag
            "longOptionOnlyValue",                        // name
            "provide generic option for breathing test",  // description
            balb::CommandLineTypeInfo(&longOptionOnlyValue),
                                                          // linked variable
            balb::CommandLineOccurrenceInfo::BAEA_HIDDEN   // occurrence info
          },
          {
            "",                                           // tag
            "nonOptionValue1",                            // name
            "provide generic option for breathing test",  // description
            balb::CommandLineTypeInfo(&nonOptionValue1),   // linked variable
            balb::CommandLineOccurrenceInfo::BAEA_REQUIRED // occurrence info
          },
          {
            "",                                           // tag
            "nonOptionValue2",                            // name
            "provide generic option for breathing test",  // description
            balb::CommandLineTypeInfo(&nonOptionValue2),   // linked variable
            balb::CommandLineOccurrenceInfo(defaultNonOptionValue)
                                                          // occurrence info
          },
          {
            "",                                           // tag
            "nonOptionValue3",                            // name
            "provide generic option for breathing test",  // description
            balb::CommandLineTypeInfo(&nonOptionValue3),   // linked variable
            balb::CommandLineOccurrenceInfo(defaultNonOptionValue)
                                                          // occurrence info
          }
        };
        const int numSpecTable = sizeof specTable / sizeof *specTable;

        if (verbose) cout << "\tCreating command line object." << endl;
        {
            balb::CommandLine cmdLine(specTable, numSpecTable);

            if (verbose) {
                cmdLine.printUsage();
            }

            ASSERT(!cmdLine.isParsed());
        }

        if (verbose) cout << "\n\tParse a valid command line." << endl;
        {
            balb::CommandLine cmdLine(specTable, numSpecTable);

            const char *argv[] = {
                "someTaskName",
                "54321",  // required non-option 1
                "-s", "1",
                "-f",
                "--long-option-with-dashes", "2",
                "98765",  // optional non-option 2
                "-s=3",
                "--long-option-only", "123",
                "--long-option-with-dashes=4"
            };
            const int argc = sizeof argv / sizeof *argv;

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
                T_(); P(defaultNonOptionValue);
                T_(); P(longOptionOnlyValue);
                T_(); P(nonOptionValue1);
                T_(); P(nonOptionValue2);
                T_(); P(nonOptionValue3);
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
            balb::CommandLine cmdLine(specTable, numSpecTable);

            longOptionOnlyValue = -1;
            nonOptionValue1     = -1;
            nonOptionValue2     = -1;
            nonOptionValue3     = -1;

            const char *argv[] = {
                "someOtherTaskName",
                "--long-option-only", "1234",
                // "54321",  // required non-option 1
                "-s", "1",
                // "-f",
                "--long-option-with-dashes", "2",
                // "98765",  // optional non-option 2
                "-s=3",
                "--long-option-with-dashes=4"
            };
            const int argc = sizeof argv / sizeof *argv;

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
                T_(); P(defaultNonOptionValue);
                T_(); P(longOptionOnlyValue);
                T_(); P(nonOptionValue1);
                T_(); P(nonOptionValue2);
                T_(); P(nonOptionValue3);
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
      case -1: {
        // --------------------------------------------------------------------
        // TEST:
        //   Exercise the basic functionality.

        bsl::string taskname,logFilename;
        int verbosity;
        bsl::string configFilename, destinationOpt;
        bool noAsync;
        balb::CommandLineOptionInfo specTable[] = {
            {
                "p|pull",                           // tag
                "pull",                             // name
                "Destination(s) for pulling config file", // description
                balb::CommandLineTypeInfo(&destinationOpt) // link
            },
            {
                "A|noAsync",                           // tag
                "noAsync",                             // name
                "Do not use any asynchronous fastsend interfaces.  Emergency"
                " use only!", // description
                balb::CommandLineTypeInfo(&noAsync) // link
            },
            {
                "l|logfile",                        // tag
                "logfile",                          // name
                "Logging file name",                // description
                balb::CommandLineTypeInfo(&logFilename) // link
            },
            {
                "t|taskname",                       // tag
                "taskname",                         // name
                "BENV taskname to accept M traps",  // description
                balb::CommandLineTypeInfo(&taskname) // link
            },
            {
                "v|verbosity",                       // tag
                "verbosity",                         // name
                "initial verbosity for the logger",  // description
                balb::CommandLineTypeInfo(&verbosity) // link
            },
           {
                "",                                // indicates non-option
                "filename",                        // name
                "configuration file name",         // description
                balb::CommandLineTypeInfo(&configFilename),   // link
                balb::CommandLineOccuranceInfo::BAEA_REQUIRED
            }

        };
        balb::CommandLine  cmdLine(specTable);
        const char *myArgv[2] = {"blah.tsk", "foo.cfg"};
        cmdLine.parse(2, myArgv);
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
