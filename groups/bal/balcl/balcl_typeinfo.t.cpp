// balcl_typeinfo.t.cpp                                               -*-C++-*-

#include <balcl_typeinfo.h>

#include <balcl_constraint.h>
#include <balcl_optionvalue.h>
#include <balcl_optiontype.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_time.h>
#include <bdlb_chartype.h>
#include <bdlb_printmethods.h>

#include <bslim_testutil.h>

#include <bslmf_assert.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bsls_assert.h>
#include <bsls_objectbuffer.h>
#include <bsls_types.h> // 'bsls::Types::Int64'

#include <bslalg_typetraits.h>

#include <bsl_functional.h> // 'bsl::function'
#include <bsl_iostream.h>
#include <bsl_ostream.h>    // 'operator<<'
#include <bsl_sstream.h>
#include <bsl_stdexcept.h>
#include <bsl_string.h>     // 'bslstl::StringRef'
#include <bsl_vector.h>

#include <bsl_cstdlib.h>
#include <bsl_cassert.h>

using namespace BloombergLP;
using bsl::cerr;
using bsl::cout;
using bsl::endl;

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// ----------------------------------------------------------------------------
// balcl::TypeInfo
// ----------------------------------------------------------------------------
// CREATORS
// [ 1] TypeInfo();
// [ 1] TypeInfo(bslma::Allocator *basicAllocator);
// [ 1] TypeInfo(bool             *v, *bA = 0);
// [ 1] TypeInfo(char             *v, *bA = 0);
// [ 1] TypeInfo(char             *v, CharC&     c, *bA = 0);
// [ 1] TypeInfo(int              *v, *bA = 0);
// [ 1] TypeInfo(int              *v, IntC&      c, *bA = 0);
// [ 1] TypeInfo(Int64            *v, *bA = 0);
// [ 1] TypeInfo(Int64            *v, Int64C&    c, *bA = 0);
// [ 1] TypeInfo(double           *v, *bA = 0);
// [ 1] TypeInfo(double           *v, DoubleC&   c, *bA = 0);
// [ 1] TypeInfo(string           *v, *bA = 0);
// [ 1] TypeInfo(string           *v, StringC&   c, *bA = 0);
// [ 1] TypeInfo(Datetime         *v, *bA = 0);
// [ 1] TypeInfo(Datetime         *v, DatetimeC& c, *bA = 0);
// [ 1] TypeInfo(Date             *v, *bA = 0);
// [ 1] TypeInfo(Date             *v, DateC&     c, *bA = 0);
// [ 1] TypeInfo(Time             *v, *bA = 0);
// [ 1] TypeInfo(Time             *v, TimeC&     c, *bA = 0);
// [ 1] TypeInfo(vector<char>     *v, *bA = 0);
// [ 1] TypeInfo(vector<char>     *v, CharC&     c, *bA = 0);
// [ 1] TypeInfo(vector<int>      *v, *bA = 0);
// [ 1] TypeInfo(vector<int>      *v, IntC&      c, *bA = 0);
// [ 1] TypeInfo(vector<Int64>    *v, *bA = 0);
// [ 1] TypeInfo(vector<Int64>    *v, Int64C&    c, *bA = 0);
// [ 1] TypeInfo(vector<double>   *v, *bA = 0);
// [ 1] TypeInfo(vector<double>   *v, DoubleC&   c, *bA = 0);
// [ 1] TypeInfo(vector<string>   *v, *bA = 0);
// [ 1] TypeInfo(vector<string>   *v, StringC&   c, *bA = 0);
// [ 1] TypeInfo(vector<Datetime> *v, *bA = 0);
// [ 1] TypeInfo(vector<Datetime> *v, DatetimeC& c, *bA = 0);
// [ 1] TypeInfo(vector<Date>     *v, *bA = 0);
// [ 1] TypeInfo(vector<Date>     *v, DateC&     c, *bA = 0);
// [ 1] TypeInfo(vector<Time>     *v, *bA = 0);
// [ 1] TypeInfo(vector<Time>     *v, TimeC&     c, *bA = 0);
// [ 1] TypeInfo(const TypeInfo& original, *bA = 0);
// [ 1] ~TypeInfo();
//
// MANIPULATORS
// [ 1] TypeInfo& operator=(const TypeInfo& rhs);
// [ 1] void resetConstraint();
// [ 1] void resetLinkedVariableAndConstraint();
// [ 1] void setConstraint(const Clc::CharConstraint&     constraint);
// [ 1] void setConstraint(const Clc::IntConstraint&      constraint);
// [ 1] void setConstraint(const Clc::Int64Constraint&    constraint);
// [ 1] void setConstraint(const Clc::DoubleConstraint&   constraint);
// [ 1] void setConstraint(const Clc::StringConstraint&   constraint);
// [ 1] void setConstraint(const Clc::DatetimeConstraint& constraint);
// [ 1] void setConstraint(const Clc::DateConstraint&     constraint);
// [ 1] void setConstraint(const Clc::TimeConstraint&     constraint);
// [ 1] void setConstraint(const shared_ptr<Constraint>& constraint);
// [ 1] void setLinkedVariable(bool             *variable);
// [ 1] void setLinkedVariable(char             *variable);
// [ 1] void setLinkedVariable(int              *variable);
// [ 1] void setLinkedVariable(Int64            *variable);
// [ 1] void setLinkedVariable(double           *variable);
// [ 1] void setLinkedVariable(string           *variable);
// [ 1] void setLinkedVariable(Datetime         *variable);
// [ 1] void setLinkedVariable(Date             *variable);
// [ 1] void setLinkedVariable(Time             *variable);
// [ 1] void setLinkedVariable(vector<char>     *variable);
// [ 1] void setLinkedVariable(vector<int>      *variable);
// [ 1] void setLinkedVariable(vector<Int64>    *variable);
// [ 1] void setLinkedVariable(vector<double>   *variable);
// [ 1] void setLinkedVariable(vector<string>   *variable);
// [ 1] void setLinkedVariable(vector<Datetime> *variable);
// [ 1] void setLinkedVariable(vector<Date>     *variable);
// [ 1] void setLinkedVariable(vector<Time>     *variable);
//
// ACCESSORS
// [ 1] shared_ptr<Constraint> constraint() const;
// [ 1] void *linkedVariable() const;
// [ 1] OptionType::Enum type() const;
//
// [ 1] bslma::Allocator *allocator() const;
// [ 1] ostream& print(ostream& stream, int level = 0, int spl = 4) const;
//
// FREE OPERATORS
// [ 1] bool operator==(const TypeInfo& lhs, rhs);
// [ 1] bool operator!=(const TypeInfo& lhs, rhs);
// [ 1] operator<<(ostream& stream, const TypeInfo& rhs);
// ----------------------------------------------------------------------------
// balcl::TypeInfoUtil
// ----------------------------------------------------------------------------
// [ 1] parse(OptionValue& el, ostream& strm, const string& str) const;
// [ 1] bool satisfiesConstraint(const Clov& e, TypeInfo tf);
// [ 1] bool satisfiesConstraint(const Clov& e, ostream& s, TypeInfo tf);
// [ 1] bool satisfiesConstraint(const void *v, TypeInfo tf);
// [ 1] bool satisfiesConstraint(const void *v, ostream& s, TypeInfo tf);
// [ 1] parse(OptionValue& e, ostream& s, const string& st, TypeInfo tf);
// ----------------------------------------------------------------------------
// [XX] BREATHING TEST
// [ 1] TESTING 'balcl::TypeInfo'
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


typedef balcl::Constraint       Constraint;
typedef balcl::OptionType       OptionType;
typedef balcl::TypeInfo         TypeInfo;
typedef balcl::TypeInfoUtil     TypeInfoUtil;
typedef balcl::OptionValue      OptionValue;

typedef balcl::OptionType       Ot;
typedef balcl::OptionType::Enum ElemType;

typedef bsls::Types::Int64      Int64;

// ============================================================================
//                                TYPE TRAITS
// ----------------------------------------------------------------------------

BSLMF_ASSERT(bslma::UsesBslmaAllocator<TypeInfo>::value);
BSLMF_ASSERT(bdlb::HasPrintMethod<TypeInfo>::value);

// ============================================================================
//          GLOBAL CONSTANTS/VARIABLES/FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

enum { k_DATETIME_FIELD_WIDTH = 25
     ,     k_DATE_FIELD_WIDTH =  9
     ,     k_TIME_FIELD_WIDTH = 15 };

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

struct OptConstraint {

    // CLASS DATA
    static bool s_constraintValue;
        // Global return value (for easier control).

    // DATA
    char d_buffer[1 + sizeof(bsl::function<bool(const void   *,
                                                bsl::ostream *)>)];
         // This data member ensures that the 'bsl::function' object that
         // contains this object has to allocate.
};

bool OptConstraint::s_constraintValue = true;

struct OptCharConstraint : public OptConstraint {
    bool operator()(const char *, bsl::ostream&) const
        // Return 'true'.
    {
        return s_constraintValue;
    }
} optCharConstraint;

#if 0
struct OptShortConstraint : public OptConstraint {
    bool operator()(const short *, bsl::ostream&) const
        // Return 'true'.
    {
        return s_constraintValue;
    }
} optShortConstraint;
#endif // 0

struct OptIntConstraint : public OptConstraint {
    bool operator()(const int *, bsl::ostream&) const
        // Return 'true'.
    {
        return s_constraintValue;
    }
} optIntConstraint;

struct OptInt64Constraint : public OptConstraint {
    bool operator()(const bsls::Types::Int64 *, bsl::ostream&) const
        // Return 'true'.
    {
        return s_constraintValue;
    }
} optInt64Constraint;

#if 0
struct OptFloatConstraint : public OptConstraint {
    bool operator()(const float *, bsl::ostream&) const
        // Return 'true'.
    {
        return s_constraintValue;
    }
} optFloatConstraint;
#endif // 0

struct OptDoubleConstraint : public OptConstraint {
    bool operator()(const double *, bsl::ostream&) const
        // Return 'true'.
    {
        return s_constraintValue;
    }
} optDoubleConstraint;

struct OptStringConstraint : public OptConstraint {
    bool operator()(const bsl::string *, bsl::ostream&) const
        // Return 'true'.
    {
        return s_constraintValue;
    }
} optStringConstraint;

struct OptDatetimeConstraint : public OptConstraint {

    bool operator()(const bdlt::Datetime *, bsl::ostream&) const
        // Return 'true'.
    {
        return s_constraintValue;
    }
} optDatetimeConstraint;

struct OptDateConstraint : public OptConstraint {

    bool operator()(const bdlt::Date *, bsl::ostream&) const
        // Return 'true'.
    {
        return s_constraintValue;
    }
} optDateConstraint;

struct OptTimeConstraint : public OptConstraint {
    bool operator()(const bdlt::Time *, bsl::ostream&) const
        // Return 'true'.
    {
        return s_constraintValue;
    }
} optTimeConstraint;

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
 , { L_, Ot::e_BOOL,           0,                    0                     }
 , { L_, Ot::e_CHAR,           0,                    &optCharConstraint    }
 , { L_, Ot::e_INT,            0,                    &optIntConstraint     }
 , { L_, Ot::e_INT64,          0,                    &optInt64Constraint   }
 , { L_, Ot::e_DOUBLE,         0,                    &optDoubleConstraint  }
 , { L_, Ot::e_STRING,         0,                    &optStringConstraint  }
 , { L_, Ot::e_DATETIME,       0,                    &optDatetimeConstraint}
 , { L_, Ot::e_DATE,           0,                    &optDateConstraint    }
 , { L_, Ot::e_TIME,           0,                    &optTimeConstraint    }
 , { L_, Ot::e_CHAR_ARRAY,     0,                    &optCharConstraint    }
 , { L_, Ot::e_INT_ARRAY,      0,                    &optIntConstraint     }
 , { L_, Ot::e_INT64_ARRAY,    0,                    &optInt64Constraint   }
 , { L_, Ot::e_DOUBLE_ARRAY,   0,                    &optDoubleConstraint  }
 , { L_, Ot::e_STRING_ARRAY,   0,                    &optStringConstraint  }
 , { L_, Ot::e_DATETIME_ARRAY, 0,                    &optDatetimeConstraint}
 , { L_, Ot::e_DATE_ARRAY,     0,                    &optDateConstraint    }
 , { L_, Ot::e_TIME_ARRAY,     0,                    &optTimeConstraint    }
 , { L_, Ot::e_BOOL,           &linkedBool,          0                     }
 , { L_, Ot::e_CHAR,           &linkedChar,          &optCharConstraint    }
 , { L_, Ot::e_INT,            &linkedInt,           &optIntConstraint     }
 , { L_, Ot::e_INT64,          &linkedInt64,         &optInt64Constraint   }
 , { L_, Ot::e_DOUBLE,         &linkedDouble,        &optDoubleConstraint  }
 , { L_, Ot::e_STRING,         &linkedString,        &optStringConstraint  }
 , { L_, Ot::e_DATETIME,       &linkedDatetime,      &optDatetimeConstraint}
 , { L_, Ot::e_DATE,           &linkedDate,          &optDateConstraint    }
 , { L_, Ot::e_TIME,           &linkedTime,          &optTimeConstraint    }
 , { L_, Ot::e_CHAR_ARRAY,     &linkedCharArray,     &optCharConstraint    }
 , { L_, Ot::e_INT_ARRAY,      &linkedIntArray,      &optIntConstraint     }
 , { L_, Ot::e_INT64_ARRAY,    &linkedInt64Array,    &optInt64Constraint   }
 , { L_, Ot::e_DOUBLE_ARRAY,   &linkedDoubleArray,   &optDoubleConstraint  }
 , { L_, Ot::e_STRING_ARRAY,   &linkedStringArray,   &optStringConstraint  }
 , { L_, Ot::e_DATETIME_ARRAY, &linkedDatetimeArray, &optDatetimeConstraint}
 , { L_, Ot::e_DATE_ARRAY,     &linkedDateArray,     &optDateConstraint    }
 , { L_, Ot::e_TIME_ARRAY,     &linkedTimeArray,     &optTimeConstraint    }
};
enum { NUM_OPTION_TYPEINFO = sizeof  OPTION_TYPEINFO
                           / sizeof *OPTION_TYPEINFO };

#undef VP

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

// ============================================================================
//                          HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

namespace {

                            // ==========================
                            // struct local_ParserImpUtil
                            // ==========================

struct local_ParserImpUtil {

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

                            // --------------------------
                            // struct local_ParserImpUtil
                            // --------------------------

int local_ParserImpUtil::skipRequiredToken(const char **endPos,
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

int local_ParserImpUtil::skipWhiteSpace(const char **endPos,
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

                           // ======================
                           // function setConstraint
                           // ======================

void setConstraint(TypeInfo *typeInfo, ElemType type, void *address)
    // Set the constraint of the specified 'typeInfo' to the function at the
    // specified 'address' of the signature corresponding to the specified
    // 'type'.  The behavior is undefined unless
    // 'balcl::OptionType::e_VOID != type' and
    // 'balcl::OptionType::e_BOOL != type'.
{
    BSLS_ASSERT(typeInfo);
    BSLS_ASSERT(balcl::OptionType::e_VOID != type);
    BSLS_ASSERT(balcl::OptionType::e_BOOL != type);
    BSLS_ASSERT(address);

    switch (type) {
      case Ot::e_VOID: {
        ASSERT(!"Reached");
      } break;
      case Ot::e_BOOL: {
        ASSERT(!"Reached");
      } break;
      case Ot::e_CHAR:
      case Ot::e_CHAR_ARRAY: {
        typeInfo->setConstraint(balcl::Constraint::CharConstraint(
                             *reinterpret_cast<OptCharConstraint *>(address)));
      } break;
      case Ot::e_INT:
      case Ot::e_INT_ARRAY: {
        typeInfo->setConstraint(balcl::Constraint::IntConstraint(
                              *reinterpret_cast<OptIntConstraint *>(address)));
      } break;
      case Ot::e_INT64:
      case Ot::e_INT64_ARRAY: {
        typeInfo->setConstraint(balcl::Constraint::Int64Constraint(
                            *reinterpret_cast<OptInt64Constraint *>(address)));
      } break;
      case Ot::e_DOUBLE:
      case Ot::e_DOUBLE_ARRAY: {
        typeInfo->setConstraint(balcl::Constraint::DoubleConstraint(
                           *reinterpret_cast<OptDoubleConstraint *>(address)));
      } break;
      case Ot::e_STRING:
      case Ot::e_STRING_ARRAY: {
        typeInfo->setConstraint(balcl::Constraint::StringConstraint(
                           *reinterpret_cast<OptStringConstraint *>(address)));
      } break;
      case Ot::e_DATETIME:
      case Ot::e_DATETIME_ARRAY: {
        typeInfo->setConstraint(balcl::Constraint::DatetimeConstraint(
                         *reinterpret_cast<OptDatetimeConstraint *>(address)));
      } break;
      case Ot::e_DATE:
      case Ot::e_DATE_ARRAY: {
        typeInfo->setConstraint(balcl::Constraint::DateConstraint(
                             *reinterpret_cast<OptDateConstraint *>(address)));
      } break;
      case Ot::e_TIME:
      case Ot::e_TIME_ARRAY: {
        typeInfo->setConstraint(balcl::Constraint::TimeConstraint(
                             *reinterpret_cast<OptTimeConstraint *>(address)));
      } break;
    };
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

                          // ==========================
                          // function constructTypeInfo
                          // ==========================

TypeInfo *constructTypeInfo(void *buffer,  ElemType type, void *variable)
    // Return the address of a 'TypeInfo' object created in the specified
    // 'buffer' and having the specified 'type'.  If 'variable' is not 0, that
    // address is linked to the option.  There is no constraint on the value.
    // The returned object will use the currently defined default allocator.
    // The caller is required to explicitly invoke the destructor of the
    // returned object.  The behavior is undefined unless unless
    // 'balcl::OptionType::e_VOID != type' and 'variable' can be cast to
    // 'balcl::OptionType::EnumToType<type>::type'.
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(balcl::OptionType::e_VOID != type);

    TypeInfo *ptr = 0;

    switch (type) {
//v---^
  case Ot::e_VOID: {
    BSLS_ASSERT(!"Reached: 'e_VOID'");
  } break;
  case Ot::e_BOOL: {
    ptr = new (buffer) TypeInfo(static_cast<Ot::Bool          *>(variable));
  } break;
  case Ot::e_CHAR: {
    ptr = new (buffer) TypeInfo(static_cast<Ot::Char          *>(variable));
  } break;
  case Ot::e_INT: {
    ptr = new (buffer) TypeInfo(static_cast<Ot::Int           *>(variable));
  } break;
  case Ot::e_INT64: {
    ptr = new (buffer) TypeInfo(static_cast<Ot::Int64         *>(variable));
  } break;
  case Ot::e_DOUBLE: {
    ptr = new (buffer) TypeInfo(static_cast<Ot::Double        *>(variable));
  } break;
  case Ot::e_STRING: {
    ptr = new (buffer) TypeInfo(static_cast<Ot::String        *>(variable));
  } break;
  case Ot::e_DATETIME: {
    ptr = new (buffer) TypeInfo(static_cast<Ot::Datetime      *>(variable));
  } break;
  case Ot::e_DATE: {
    ptr = new (buffer) TypeInfo(static_cast<Ot::Date          *>(variable));
  } break;
  case Ot::e_TIME: {
    ptr = new (buffer) TypeInfo(static_cast<Ot::Time          *>(variable));
  } break;
  case Ot::e_CHAR_ARRAY: {
    ptr = new (buffer) TypeInfo(static_cast<Ot::CharArray     *>(variable));
  } break;
  case Ot::e_INT_ARRAY: {
    ptr = new (buffer) TypeInfo(static_cast<Ot::IntArray      *>(variable));
  } break;
  case Ot::e_INT64_ARRAY: {
    ptr = new (buffer) TypeInfo(static_cast<Ot::Int64Array    *>(variable));
  } break;
  case Ot::e_DOUBLE_ARRAY: {
    ptr = new (buffer) TypeInfo(static_cast<Ot::DoubleArray   *>(variable));
  } break;
  case Ot::e_STRING_ARRAY: {
    ptr = new (buffer) TypeInfo(static_cast<Ot::StringArray   *>(variable));
  } break;
  case Ot::e_DATETIME_ARRAY: {
    ptr = new (buffer) TypeInfo(static_cast<Ot::DatetimeArray *>(variable));
  } break;
  case Ot::e_DATE_ARRAY: {
    ptr = new (buffer) TypeInfo(static_cast<Ot::DateArray     *>(variable));
  } break;
  case Ot::e_TIME_ARRAY: {
    ptr = new (buffer) TypeInfo(static_cast<Ot::TimeArray     *>(variable));
  } break;
//^---v
    }
    return ptr;
}

TypeInfo *constructTypeInfo(void     *buffer,
                            ElemType  type,
                            void     *variable,
                            void     *constraint)
    // Return the address of a 'TypeInfo' object created in the specified
    // 'buffer' and having the specified 'type' and 'constraint', If 'variable'
    // is not 0, that address is linked to the option.  The returned object
    // uses the currently defined default allocator.  The caller is required to
    // explicitly invoke the destructor of the returned object.  The behavior
    // is undefined unless unless 'balcl::OptionType::e_VOID != type',
    // 'balcl::OptionType::e_BOOL != type', 'variable' can be cast to
    // 'balcl::OptionType::EnumToType<type>::type', and 'constraint' can be
    // cast to the type defined by 'Constraint' for 'type'.
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(balcl::OptionType::e_VOID != type);
    BSLS_ASSERT(balcl::OptionType::e_BOOL != type);
    BSLS_ASSERT(constraint);

    TypeInfo *ptr = 0;

    switch (type) {
//v---^
  case Ot::e_VOID: {
    BSLS_ASSERT(!"Reached: 'e_VOID'");
  } break;
  case Ot::e_BOOL: {
    BSLS_ASSERT(!"Reached: 'e_BOOL'");
  } break;
  case Ot::e_CHAR: {
    ptr = new (buffer) TypeInfo(static_cast<Ot::Char          *>(variable),
                       *static_cast<Constraint::CharConstraint *>(constraint));
  } break;
  case Ot::e_INT: {
    ptr = new (buffer) TypeInfo(static_cast<Ot::Int           *>(variable),
                        *static_cast<Constraint::IntConstraint *>(constraint));
  } break;
  case Ot::e_INT64: {
    ptr = new (buffer) TypeInfo(static_cast<Ot::Int64         *>(variable),
                      *static_cast<Constraint::Int64Constraint *>(constraint));
  } break;
  case Ot::e_DOUBLE: {
    ptr = new (buffer) TypeInfo(static_cast<Ot::Double        *>(variable),
                     *static_cast<Constraint::DoubleConstraint *>(constraint));
  } break;
  case Ot::e_STRING: {
    ptr = new (buffer) TypeInfo(static_cast<Ot::String        *>(variable),
                     *static_cast<Constraint::StringConstraint *>(constraint));
  } break;
  case Ot::e_DATETIME: {
    ptr = new (buffer) TypeInfo(static_cast<Ot::Datetime      *>(variable),
                   *static_cast<Constraint::DatetimeConstraint *>(constraint));
  } break;
  case Ot::e_DATE: {
    ptr = new (buffer) TypeInfo(static_cast<Ot::Date          *>(variable),
                       *static_cast<Constraint::DateConstraint *>(constraint));
  } break;
  case Ot::e_TIME: {
    ptr = new (buffer) TypeInfo(static_cast<Ot::Time          *>(variable),
                       *static_cast<Constraint::TimeConstraint *>(constraint));
  } break;
  case Ot::e_CHAR_ARRAY: {
    ptr = new (buffer) TypeInfo(static_cast<Ot::CharArray     *>(variable),
                       *static_cast<Constraint::CharConstraint *>(constraint));
  } break;
  case Ot::e_INT_ARRAY: {
    ptr = new (buffer) TypeInfo(static_cast<Ot::IntArray      *>(variable),
                        *static_cast<Constraint::IntConstraint *>(constraint));
  } break;
  case Ot::e_INT64_ARRAY: {
    ptr = new (buffer) TypeInfo(static_cast<Ot::Int64Array    *>(variable),
                      *static_cast<Constraint::Int64Constraint *>(constraint));
  } break;
  case Ot::e_DOUBLE_ARRAY: {
    ptr = new (buffer) TypeInfo(static_cast<Ot::DoubleArray   *>(variable),
                     *static_cast<Constraint::DoubleConstraint *>(constraint));
  } break;
  case Ot::e_STRING_ARRAY: {
    ptr = new (buffer) TypeInfo(static_cast<Ot::StringArray   *>(variable),
                     *static_cast<Constraint::StringConstraint *>(constraint));
  } break;
  case Ot::e_DATETIME_ARRAY: {
    ptr = new (buffer) TypeInfo(static_cast<Ot::DatetimeArray *>(variable),
                   *static_cast<Constraint::DatetimeConstraint *>(constraint));
  } break;
  case Ot::e_DATE_ARRAY: {
    ptr = new (buffer) TypeInfo(static_cast<Ot::DateArray     *>(variable),
                       *static_cast<Constraint::DateConstraint *>(constraint));
  } break;
  case Ot::e_TIME_ARRAY: {
    ptr = new (buffer) TypeInfo(static_cast<Ot::TimeArray     *>(variable),
                       *static_cast<Constraint::TimeConstraint *>(constraint));
  } break;
//^---v
    }
    return ptr;
}

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
    typedef local_ParserImpUtil Parser;
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

                          // =======================
                          // function setOptionValue
                          // =======================

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

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'balcl::TypeInfo'
        //
        // Concerns:
        //: 1 'balcl::TypeInfo' is a simple unconstrained attribute
        //:   class that takes a 'bslma' allocator.  The concerns are generally
        //:   the same as for any value-semantic type that takes a 'bslma'
        //:   allocator.
        //
        // This class has redundant manipulators and accessors, so we choose
        // the primary manipulators to be:
        //..
        //  balcl::OccurrenceInfo(bslma::Allocator *allocator);
        //  operator=(.... *variable);
        //  void setConstraint(
        //                const balcl::Constraint::... & constraint);
        //  void resetConstraint();
        //..
        // and the primary accessors to be:
        //..
        //  balcl::OptionType () const;
        //  balcl::Constraint *constraint() const;
        //  void *linkedVariable() const;
        //..
        //
        // Plan:
        //: 1 We follow the standard structure of a value-semantic test driver.
        //:   Note that the 'constraint' accessor returns a pointer to a local
        //:   object, owned by the test object, and therefore we cannot know
        //:   the return value, but we can however verify that it is not the
        //:   same value as the default constraint object.
        //
        // Testing:
        //   TESTING 'balcl::TypeInfo'
        //   TypeInfo();
        //   TypeInfo(bslma::Allocator *basicAllocator);
        //   TypeInfo(bool             *v, *bA = 0);
        //   TypeInfo(char             *v, *bA = 0);
        //   TypeInfo(char             *v, CharC&     c, *bA = 0);
        //   TypeInfo(int              *v, *bA = 0);
        //   TypeInfo(int              *v, IntC&      c, *bA = 0);
        //   TypeInfo(Int64            *v, *bA = 0);
        //   TypeInfo(Int64            *v, Int64C&    c, *bA = 0);
        //   TypeInfo(double           *v, *bA = 0);
        //   TypeInfo(double           *v, DoubleC&   c, *bA = 0);
        //   TypeInfo(string           *v, *bA = 0);
        //   TypeInfo(string           *v, StringC&   c, *bA = 0);
        //   TypeInfo(Datetime         *v, *bA = 0);
        //   TypeInfo(Datetime         *v, DatetimeC& c, *bA = 0);
        //   TypeInfo(Date             *v, *bA = 0);
        //   TypeInfo(Date             *v, DateC&     c, *bA = 0);
        //   TypeInfo(Time             *v, *bA = 0);
        //   TypeInfo(Time             *v, TimeC&     c, *bA = 0);
        //   TypeInfo(vector<char>     *v, *bA = 0);
        //   TypeInfo(vector<char>     *v, CharC&     c, *bA = 0);
        //   TypeInfo(vector<int>      *v, *bA = 0);
        //   TypeInfo(vector<int>      *v, IntC&      c, *bA = 0);
        //   TypeInfo(vector<Int64>    *v, *bA = 0);
        //   TypeInfo(vector<Int64>    *v, Int64C&    c, *bA = 0);
        //   TypeInfo(vector<double>   *v, *bA = 0);
        //   TypeInfo(vector<double>   *v, DoubleC&   c, *bA = 0);
        //   TypeInfo(vector<string>   *v, *bA = 0);
        //   TypeInfo(vector<string>   *v, StringC&   c, *bA = 0);
        //   TypeInfo(vector<Datetime> *v, *bA = 0);
        //   TypeInfo(vector<Datetime> *v, DatetimeC& c, *bA = 0);
        //   TypeInfo(vector<Date>     *v, *bA = 0);
        //   TypeInfo(vector<Date>     *v, DateC&     c, *bA = 0);
        //   TypeInfo(vector<Time>     *v, *bA = 0);
        //   TypeInfo(vector<Time>     *v, TimeC&     c, *bA = 0);
        //   TypeInfo(const TypeInfo& original, *bA = 0);
        //   ~TypeInfo();
        //   TypeInfo& operator=(const TypeInfo& rhs);
        //   void resetConstraint();
        //   void resetLinkedVariableAndConstraint();
        //   void setConstraint(const Clc::CharConstraint&     constraint);
        //   void setConstraint(const Clc::IntConstraint&      constraint);
        //   void setConstraint(const Clc::Int64Constraint&    constraint);
        //   void setConstraint(const Clc::DoubleConstraint&   constraint);
        //   void setConstraint(const Clc::StringConstraint&   constraint);
        //   void setConstraint(const Clc::DatetimeConstraint& constraint);
        //   void setConstraint(const Clc::DateConstraint&     constraint);
        //   void setConstraint(const Clc::TimeConstraint&     constraint);
        //   void setConstraint(const shared_ptr<Constraint>& constraint);
        //   void setLinkedVariable(bool             *variable);
        //   void setLinkedVariable(char             *variable);
        //   void setLinkedVariable(int              *variable);
        //   void setLinkedVariable(Int64            *variable);
        //   void setLinkedVariable(double           *variable);
        //   void setLinkedVariable(string           *variable);
        //   void setLinkedVariable(Datetime         *variable);
        //   void setLinkedVariable(Date             *variable);
        //   void setLinkedVariable(Time             *variable);
        //   void setLinkedVariable(vector<char>     *variable);
        //   void setLinkedVariable(vector<int>      *variable);
        //   void setLinkedVariable(vector<Int64>    *variable);
        //   void setLinkedVariable(vector<double>   *variable);
        //   void setLinkedVariable(vector<string>   *variable);
        //   void setLinkedVariable(vector<Datetime> *variable);
        //   void setLinkedVariable(vector<Date>     *variable);
        //   void setLinkedVariable(vector<Time>     *variable);
        //   shared_ptr<Constraint> constraint() const;
        //   void *linkedVariable() const;
        //   parse(OptionValue& el, ostream& strm, const string& str) const;
        //   OptionType::Enum type() const;
        //   bslma::Allocator *allocator() const;
        //   ostream& print(ostream& stream, int level = 0, int spl = 4) const;
        //   bool operator==(const TypeInfo& lhs, rhs);
        //   bool operator!=(const TypeInfo& lhs, rhs);
        //   operator<<(ostream& stream, const TypeInfo& rhs);
        //   bool satisfiesConstraint(const Clov& e, TypeInfo tf);
        //   bool satisfiesConstraint(const Clov& e, ostream& s, TypeInfo tf);
        //   bool satisfiesConstraint(const void *v, TypeInfo tf);
        //   bool satisfiesConstraint(const void *v, ostream& s, TypeInfo tf);
        //   parse(OptionValue& e, ostream& s, const string& st, TypeInfo tf);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'balcl::TypeInfo'" << endl
                          << "=========================" << endl;

        typedef TypeInfo Obj;

        bslma::TestAllocator    testAllocator(veryVeryVeryVerbose);
        bslma::TestAllocator defaultAllocator(veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard guard(&defaultAllocator);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 2 OF VALUE-SEMANTIC TEST DRIVER
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP)

        if (verbose) cout << "\n\tTesting primary manipulators." << endl;

        if (verbose) cout << "\t\tWithout passing in an allocator." << endl;

        for (int i = 0; i < NUM_OPTION_TYPEINFO; ++i) {
            const int       LINE       = OPTION_TYPEINFO[i].d_line;
            const ElemType  TYPE       = OPTION_TYPEINFO[i].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[i].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[i].d_constraint_p;

            Obj mX;  const Obj& X = mX;      // TEST HERE

            if (veryVerbose) {
                T_ T_ T_ P_(LINE) P_(i) P(TYPE)
                T_ T_ T_ P_(VARIABLE) P(CONSTRAINT)
                T_ T_ T_ P_(LINE) P_(i) P(X)
            }
            LOOP_ASSERT(LINE, Ot::e_STRING == X.type());
            LOOP_ASSERT(LINE, !X.linkedVariable());

            setType(&mX, TYPE);

            LOOP_ASSERT(LINE, TYPE == X.type());
            LOOP_ASSERT(LINE, !X.linkedVariable());

            bsl::shared_ptr<balcl::TypeInfoConstraint> DEFAULT_CONSTRAINT
                                                              = X.constraint();

            if (VARIABLE) {
                setLinkedVariable(&mX, TYPE, VARIABLE);

                if (veryVerbose) {
                    T_ T_ T_ P_(LINE) P_(i) P(X)
                }
                LOOP_ASSERT(LINE, TYPE               == X.type());
                LOOP_ASSERT(LINE, VARIABLE           == X.linkedVariable());
                LOOP_ASSERT(LINE, DEFAULT_CONSTRAINT == X.constraint());

                mX.resetLinkedVariableAndConstraint();

                LOOP_ASSERT(LINE, TYPE == X.type());
                LOOP_ASSERT(LINE, !X.linkedVariable());
                LOOP_ASSERT(LINE, DEFAULT_CONSTRAINT == X.constraint());
            }

            mX.resetLinkedVariableAndConstraint();

            LOOP_ASSERT(LINE, TYPE == X.type());
            LOOP_ASSERT(LINE, !X.linkedVariable());
            LOOP_ASSERT(LINE, DEFAULT_CONSTRAINT == X.constraint());

            if (CONSTRAINT) {
                setConstraint(&mX, TYPE, CONSTRAINT);

                if (veryVerbose) {
                    T_ T_ T_ P_(LINE) P_(i) P(X)
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
                    T_ T_ T_ P_(LINE) P_(i) P(X)
                }
                LOOP_ASSERT(LINE, TYPE               == X.type());
                LOOP_ASSERT(LINE, VARIABLE           == X.linkedVariable());
                LOOP_ASSERT(LINE, DEFAULT_CONSTRAINT != X.constraint());

                mX.resetLinkedVariableAndConstraint();

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
            const ElemType  TYPE       = OPTION_TYPEINFO[i].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[i].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[i].d_constraint_p;

            const bsls::Types::Int64 NUM_BYTES =
                                              defaultAllocator.numBytesInUse();
            {
                Obj mX(&testAllocator);  const Obj& X = mX;

                ASSERT(&testAllocator == X.allocator());

                createTypeInfo(&mX, TYPE, VARIABLE, CONSTRAINT);

                Obj mY(&testAllocator);  const Obj& Y = mY; setType(&mY, TYPE);

                bsl::shared_ptr<balcl::TypeInfoConstraint>
                                           DEFAULT_CONSTRAINT = Y.constraint();

                if (veryVerbose) {
                    T_ T_ T_ P_(LINE) P_(i) P(X)
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
            const ElemType  TYPE       = OPTION_TYPEINFO[i].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[i].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[i].d_constraint_p;

            Obj mY(&testAllocator);  const Obj& Y = mY; setType(&mY, TYPE);

            bsl::shared_ptr<balcl::TypeInfoConstraint> DEFAULT_CONSTRAINT
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
            const ElemType  TYPE       = OPTION_TYPEINFO[i].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[i].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[i].d_constraint_p;

            Obj mX;  const Obj& X = mX;

            createTypeInfo(&mX, TYPE, VARIABLE, CONSTRAINT);

            Obj mY(&testAllocator);  const Obj& Y = mY; setType(&mY, TYPE);

            bsl::shared_ptr<balcl::TypeInfoConstraint> DEFAULT_CONSTRAINT
                                                              = Y.constraint();

            if (veryVerbose) {
                T_ T_ P_(LINE) P_(TYPE) P_(VARIABLE) P(CONSTRAINT)
                T_ T_ P_(LINE) P(X)
            }
            LOOP_ASSERT(LINE, TYPE       == X.type());
            LOOP_ASSERT(LINE, VARIABLE   == X.linkedVariable());

            if (CONSTRAINT) {
                LOOP_ASSERT(LINE, DEFAULT_CONSTRAINT != X.constraint());
            } else {
                LOOP_ASSERT(LINE, DEFAULT_CONSTRAINT == X.constraint());
            }

            if (CONSTRAINT) {
                const int   j = i % NUM_OPTION_DEFAULT_VALUES;
                const void *VALUE = OPTION_DEFAULT_VALUES[j].d_value_p;
                ASSERT(TYPE == OPTION_DEFAULT_VALUES[j].d_type);

                OptionValue ELEMENT(TYPE);
                setOptionValue(&ELEMENT, VALUE, TYPE);

                bsl::ostringstream ossElement, ossValue;

          //v---^
            OptConstraint::s_constraintValue = true;
            ASSERT( TypeInfoUtil::satisfiesConstraint(ELEMENT, X            ));
            ASSERT( TypeInfoUtil::satisfiesConstraint(ELEMENT, X, ossElement));
            ASSERT( TypeInfoUtil::satisfiesConstraint(VALUE,   X            ));
            ASSERT( TypeInfoUtil::satisfiesConstraint(VALUE,   X, ossValue  ));

            OptConstraint::s_constraintValue = false;
            ASSERT(!TypeInfoUtil::satisfiesConstraint(ELEMENT, X            ));
            ASSERT(!TypeInfoUtil::satisfiesConstraint(ELEMENT, X, ossElement));
            ASSERT(!TypeInfoUtil::satisfiesConstraint(VALUE,   X            ));
            ASSERT(!TypeInfoUtil::satisfiesConstraint(VALUE,   X, ossValue  ));

            OptConstraint::s_constraintValue = true;
            ASSERT( TypeInfoUtil::satisfiesConstraint(ELEMENT, X            ));
            ASSERT( TypeInfoUtil::satisfiesConstraint(ELEMENT, X, ossElement));
            ASSERT( TypeInfoUtil::satisfiesConstraint(VALUE,   X            ));
            ASSERT( TypeInfoUtil::satisfiesConstraint(VALUE,   X, ossValue  ));
          //^---v
            }
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 5 OF VALUE-SEMANTIC TEST DRIVER
        // TESTING OUTPUT (<<) OPERATOR AND PRINT

        if (verbose)
            cout << "\n\tTesting output operator and 'print'." << endl;

        for (int i = 0; i < NUM_OPTION_TYPEINFO; ++i) {
            const int       LINE       = OPTION_TYPEINFO[i].d_line;
            const ElemType  TYPE       = OPTION_TYPEINFO[i].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[i].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[i].d_constraint_p;

            Obj mX;  const Obj& X = mX;

            ASSERT(&defaultAllocator == X.allocator());

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
                T_ T_ P(X)
                T_ T_ P(ss1)
                T_ T_ P(ss2)
            }

            const char *output = ss1.c_str();
            const int   ret    = parseTypeInfo(&output, X, output);
            LOOP2_ASSERT(LINE, ret, 0 == ret);
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // CASE 6 OF VALUE-SEMANTIC TEST DRIVER
        // TESTING EQUALITY OPERATOR
        //
        // Note: Unless they share the constraint, two objects will always
        // differ.  We test both.

        if (verbose) cout << "\n\tTesting equality operator." << endl;

        if (verbose)
            cout << "\t\tCompare each pair of values (u, v) in W X W." << endl;

        for (int i = 0; i < NUM_OPTION_TYPEINFO; ++i) {
            const int       LINE1 = OPTION_TYPEINFO[i].d_line;
            const ElemType  TYPE1 = OPTION_TYPEINFO[i].d_type;
            void       *VARIABLE1 = OPTION_TYPEINFO[i].d_linkedVariable_p;
            void     *CONSTRAINT1 = OPTION_TYPEINFO[i].d_constraint_p;

            if (veryVerbose) { T_ P_(LINE1) P(TYPE1) }

            Obj mX;  const Obj& X = mX;

            createTypeInfo(&mX, TYPE1, VARIABLE1, CONSTRAINT1);

            for (int j = 0; j < NUM_OPTION_TYPEINFO; ++j) {
                const int       LINE2 = OPTION_TYPEINFO[j].d_line;
                const ElemType  TYPE2 = OPTION_TYPEINFO[j].d_type;
                void       *VARIABLE2 = OPTION_TYPEINFO[j].d_linkedVariable_p;
                void     *CONSTRAINT2 = OPTION_TYPEINFO[j].d_constraint_p;

                if (veryVerbose) { T_ T_ P_(LINE2) P(TYPE2) }

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
            const ElemType  TYPE       = OPTION_TYPEINFO[i].d_type;
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
            const ElemType  TYPE       = OPTION_TYPEINFO[i].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[i].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[i].d_constraint_p;

            Obj mX;  const Obj& X = mX;

            createTypeInfo(&mX, TYPE, VARIABLE, CONSTRAINT);

            const bsls::Types::Int64 NUM_BYTES =
                                              defaultAllocator.numBytesInUse();
            const bsls::Types::Int64 NUM_ALLOC =
                                             defaultAllocator.numAllocations();
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
            const ElemType  TYPE       = OPTION_TYPEINFO[i].d_type;
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
            const ElemType  TYPE1 = OPTION_TYPEINFO[i].d_type;
            void       *VARIABLE1 = OPTION_TYPEINFO[i].d_linkedVariable_p;
            void     *CONSTRAINT1 = OPTION_TYPEINFO[i].d_constraint_p;

            Obj mX;  const Obj& X = mX;

            createTypeInfo(&mX, TYPE1, VARIABLE1, CONSTRAINT1);

            for (int j = 0; j < NUM_OPTION_TYPEINFO; ++j) {
                const int       LINE2 = OPTION_TYPEINFO[j].d_line;
                const ElemType  TYPE2 = OPTION_TYPEINFO[j].d_type;
                void       *VARIABLE2 = OPTION_TYPEINFO[j].d_linkedVariable_p;
                void     *CONSTRAINT2 = OPTION_TYPEINFO[j].d_constraint_p;

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
            const ElemType  TYPE = OPTION_TYPEINFO[i].d_type;

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

        if (verbose) cout << "\nTesting non-default constructors.\n";

        for (int i = 0; i < NUM_OPTION_TYPEINFO; ++i) {
            const int       LINE       = OPTION_TYPEINFO[i].d_line;
            const ElemType  TYPE       = OPTION_TYPEINFO[i].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[i].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[i].d_constraint_p;

            if (veryVerbose) {
                P_(LINE)
                P_(TYPE)
                P_(VARIABLE)
                P_(CONSTRAINT)
            }

            bsls::ObjectBuffer<TypeInfo> xObjectBuffer;

            TypeInfo *xPtr = constructTypeInfo(xObjectBuffer.buffer(),
                                               TYPE,
                                               VARIABLE);
            Obj&      mX = *xPtr; const Obj X = mX;
            ASSERT(TYPE     ==                     X.type());
            ASSERT(VARIABLE == static_cast<void *>(X.linkedVariable()));

            if (Ot::e_BOOL != TYPE && 0 != CONSTRAINT) {
                bsls::ObjectBuffer<TypeInfo> yObjectBuffer;

                TypeInfo *yPtr = constructTypeInfo(yObjectBuffer.buffer(),
                                                   TYPE,
                                                   VARIABLE,
                                                   CONSTRAINT);
                Obj&      mY = *yPtr; const Obj Y = mY;
                ASSERT(TYPE     ==                     Y.type());
                ASSERT(VARIABLE == static_cast<void *>(Y.linkedVariable()));
                ASSERT(0        != static_cast<void *>(Y.constraint().ptr()));

                yPtr->~TypeInfo();
            }

            xPtr->~TypeInfo();
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
