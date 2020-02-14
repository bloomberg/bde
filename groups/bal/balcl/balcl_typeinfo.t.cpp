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
#include <bdlb_tokenizer.h>

#include <bslim_testutil.h>

#include <bslmf_assert.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_objectbuffer.h>
#include <bsls_types.h>     // 'bsls::Types::Int64'

#include <bsl_functional.h> // 'bsl::function'
#include <bsl_iostream.h>
#include <bsl_ostream.h>    // 'operator<<'
#include <bsl_sstream.h>
#include <bsl_stdexcept.h>
#include <bsl_string.h>     // 'bslstl::StringRef'
#include <bsl_vector.h>

#include <bsl_cstdlib.h>
#include <bsl_cassert.h>
#include <bsl_cstring.h>    // 'bsl::strspn'

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// The 'balcl::TypeInfo' is a (value-semantic) attribute class.  The three
// salient attributes are:
//..
//  Type                     Description
//  ------------------------ --------------------
//  OptionType::Enum         option types
//  void *                   linked variable
//  bsl::share_ptr<TypeInfo> constraint (functor)
//..
// As some of these are (process-dependent) addresses, this is categorized as
// an in-core VST.
//
// The 'constraint' attribute, managed via 'bsl::share_ptr', lends this class
// some interesting behaviors that are not typically seen in attribute classes.
//
//: o Two objects created some the same constituent attributes do not compare
//:   equal when a non-default constraint is specified.
//:
//: o Although the constructor allocates when there is a constraint, the copy
//:   constructor and copy assignment never allocate.  Instead of creating a
//:   new functor, the usage count on the shared pointer in the original (rhs)
//:   object is merely incremented.
//
// This component also defines two other classes, 'balcl::TypeInfoContract', a
// functor that is not visible outside of this component except as an opaque
// type, and 'balcl::TypeInfoUtil', a utility 'struct' that must reside in this
// component because of its dependency on 'balcl::TypeInfoContract'.
//
// Testing follows the general pattern of a VST interspersed with tests of the
// utility 'struct' and 'balcl::TypeInfoContract'.  Note that the latter class
// cannot be directly tested because it has no methods visible to clients.
//
// The VST's Primary Manipulators are:
//: o void resetConstraint();
//: o void resetLinkedVariableAndConstraint();
//: o void setConstraint    (/* overloaded for  8 supported types */);
//: o void setLinkedVariable(/ *overloaded for 17 supported types */);
//
// The Basic Accessors are:
//: o shared_ptr<Constraint> constraint() const;
//: o void *linkedVariable() const;
//: o OptionType::Enum type() const;
//: o bslma::Allocator *allocator() const;  // non-salient
//
// ----------------------------------------------------------------------------
// balcl::TypeInfo
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] TypeInfo();
// [ 2] TypeInfo(bslma::Allocator *basicAllocator);
// [ 8] TypeInfo(bool             *v, *bA = 0);
// [ 8] TypeInfo(char             *v, *bA = 0);
// [ 8] TypeInfo(char             *v, CharC&     c, *bA = 0);
// [ 8] TypeInfo(int              *v, *bA = 0);
// [ 8] TypeInfo(int              *v, IntC&      c, *bA = 0);
// [ 8] TypeInfo(Int64            *v, *bA = 0);
// [ 8] TypeInfo(Int64            *v, Int64C&    c, *bA = 0);
// [ 8] TypeInfo(double           *v, *bA = 0);
// [ 8] TypeInfo(double           *v, DoubleC&   c, *bA = 0);
// [ 8] TypeInfo(string           *v, *bA = 0);
// [ 8] TypeInfo(string           *v, StringC&   c, *bA = 0);
// [ 8] TypeInfo(Datetime         *v, *bA = 0);
// [ 8] TypeInfo(Datetime         *v, DatetimeC& c, *bA = 0);
// [ 8] TypeInfo(Date             *v, *bA = 0);
// [ 8] TypeInfo(Date             *v, DateC&     c, *bA = 0);
// [ 8] TypeInfo(Time             *v, *bA = 0);
// [ 8] TypeInfo(Time             *v, TimeC&     c, *bA = 0);
// [ 8] TypeInfo(vector<char>     *v, *bA = 0);
// [ 8] TypeInfo(vector<char>     *v, CharC&     c, *bA = 0);
// [ 8] TypeInfo(vector<int>      *v, *bA = 0);
// [ 8] TypeInfo(vector<int>      *v, IntC&      c, *bA = 0);
// [ 8] TypeInfo(vector<Int64>    *v, *bA = 0);
// [ 8] TypeInfo(vector<Int64>    *v, Int64C&    c, *bA = 0);
// [ 8] TypeInfo(vector<double>   *v, *bA = 0);
// [ 8] TypeInfo(vector<double>   *v, DoubleC&   c, *bA = 0);
// [ 8] TypeInfo(vector<string>   *v, *bA = 0);
// [ 8] TypeInfo(vector<string>   *v, StringC&   c, *bA = 0);
// [ 8] TypeInfo(vector<Datetime> *v, *bA = 0);
// [ 8] TypeInfo(vector<Datetime> *v, DatetimeC& c, *bA = 0);
// [ 8] TypeInfo(vector<Date>     *v, *bA = 0);
// [ 8] TypeInfo(vector<Date>     *v, DateC&     c, *bA = 0);
// [ 8] TypeInfo(vector<Time>     *v, *bA = 0);
// [ 8] TypeInfo(vector<Time>     *v, TimeC&     c, *bA = 0);
// [ 6] TypeInfo(const TypeInfo& original, *bA = 0);
// [ 2] ~TypeInfo();
//
// MANIPULATORS
// [ 7] TypeInfo& operator=(const TypeInfo& rhs);
// [ 2] void resetConstraint();
// [ 2] void resetLinkedVariableAndConstraint();
// [ 2] void setConstraint(const Clc::CharConstraint&     constraint);
// [ 2] void setConstraint(const Clc::IntConstraint&      constraint);
// [ 2] void setConstraint(const Clc::Int64Constraint&    constraint);
// [ 2] void setConstraint(const Clc::DoubleConstraint&   constraint);
// [ 2] void setConstraint(const Clc::StringConstraint&   constraint);
// [ 2] void setConstraint(const Clc::DatetimeConstraint& constraint);
// [ 2] void setConstraint(const Clc::DateConstraint&     constraint);
// [ 2] void setConstraint(const Clc::TimeConstraint&     constraint);
// [ 2] void setConstraint(const shared_ptr<Constraint>& constraint);
// [ 2] void setLinkedVariable(bool             *variable);
// [ 2] void setLinkedVariable(char             *variable);
// [ 2] void setLinkedVariable(int              *variable);
// [ 2] void setLinkedVariable(Int64            *variable);
// [ 2] void setLinkedVariable(double           *variable);
// [ 2] void setLinkedVariable(string           *variable);
// [ 2] void setLinkedVariable(Datetime         *variable);
// [ 2] void setLinkedVariable(Date             *variable);
// [ 2] void setLinkedVariable(Time             *variable);
// [ 2] void setLinkedVariable(vector<char>     *variable);
// [ 2] void setLinkedVariable(vector<int>      *variable);
// [ 2] void setLinkedVariable(vector<Int64>    *variable);
// [ 2] void setLinkedVariable(vector<double>   *variable);
// [ 2] void setLinkedVariable(vector<string>   *variable);
// [ 2] void setLinkedVariable(vector<Datetime> *variable);
// [ 2] void setLinkedVariable(vector<Date>     *variable);
// [ 2] void setLinkedVariable(vector<Time>     *variable);
//
// ACCESSORS
// [ 2] shared_ptr<Constraint> constraint() const;
// [ 2] void *linkedVariable() const;
// [ 2] OptionType::Enum type() const;
//
// [ 2] bslma::Allocator *allocator() const;
// [ 4] ostream& print(ostream& stream, int level = 0, int spl = 4) const;
//
// FREE OPERATORS
// [ 5] bool operator==(const TypeInfo& lhs, rhs);
// [ 5] bool operator!=(const TypeInfo& lhs, rhs);
// [ 4] operator<<(ostream& stream, const TypeInfo& rhs);
// ----------------------------------------------------------------------------
// balcl::TypeInfoUtil
// ----------------------------------------------------------------------------
// [ 9] parseAndValidate(OV *e, string& i, TypeInfo& tf, ostream& s);
// [ 3] bool satisfiesConstraint(const Clov& e,             TypeInfo tf);
// [ 3] bool satisfiesConstraint(const Clov& e, ostream& s, TypeInfo tf);
// [ 3] bool satisfiesConstraint(const void *v,             TypeInfo tf);
// [ 3] bool satisfiesConstraint(const void *v, ostream& s, TypeInfo tf);
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

typedef balcl::TypeInfo           Obj;
typedef balcl::TypeInfoConstraint ObjConstraint;
typedef balcl::TypeInfoUtil       ObjUtil;

typedef balcl::Constraint         Constraint;
typedef balcl::OptionType         OptionType;
typedef balcl::OptionValue        OptionValue;

typedef balcl::OptionType       Ot;
typedef balcl::OptionType::Enum ElemType;

typedef bsls::Types::Int64      Int64;

// ============================================================================
//                                TYPE TRAITS
// ----------------------------------------------------------------------------

BSLMF_ASSERT(bslma::UsesBslmaAllocator<Obj>::value);
BSLMF_ASSERT(bdlb::HasPrintMethod<Obj>::value);

// ============================================================================
//          GLOBAL CONSTANTS/VARIABLES/FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

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
    bool operator()(const char *, bsl::ostream& stream) const
        // Return 's_constraintValue'.
    {
        if (!s_constraintValue) {
            stream << "error" << flush;
        }
        return s_constraintValue;
    }
} optCharConstraint;

struct OptIntConstraint : public OptConstraint {
    bool operator()(const int *, bsl::ostream& stream) const
        // Return 's_constraintValue'.
    {
        if (!s_constraintValue) {
            stream << "error" << flush;
        }
        return s_constraintValue;
    }
} optIntConstraint;

struct OptInt64Constraint : public OptConstraint {
    bool operator()(const bsls::Types::Int64 *, bsl::ostream& stream) const
        // Return 's_constraintValue'.
    {
        if (!s_constraintValue) {
            stream << "error" << flush;
        }
        return s_constraintValue;
    }
} optInt64Constraint;

struct OptDoubleConstraint : public OptConstraint {
    bool operator()(const double *, bsl::ostream& stream) const
        // Return 's_constraintValue'.
    {
        if (!s_constraintValue) {
            stream << "error" << flush;
        }
        return s_constraintValue;
    }
} optDoubleConstraint;

struct OptStringConstraint : public OptConstraint {
    bool operator()(const bsl::string *, bsl::ostream& stream) const
        // Return 's_constraintValue'.
    {
        if (!s_constraintValue) {
            stream << "error" << flush;
        }
        return s_constraintValue;
    }
} optStringConstraint;

struct OptDatetimeConstraint : public OptConstraint {

    bool operator()(const bdlt::Datetime *, bsl::ostream& stream) const
        // Return 's_constraintValue'.
    {
        if (!s_constraintValue) {
            stream << "error" << flush;
        }
        return s_constraintValue;
    }
} optDatetimeConstraint;

struct OptDateConstraint : public OptConstraint {

    bool operator()(const bdlt::Date *, bsl::ostream& stream) const
        // Return 's_constraintValue'.
    {
        if (!s_constraintValue) {
            stream << "error" << flush;
        }
        return s_constraintValue;
    }
} optDateConstraint;

struct OptTimeConstraint : public OptConstraint {
    bool operator()(const bdlt::Time *, bsl::ostream& stream) const
        // Return 's_constraintValue'.
    {
        if (!s_constraintValue) {
            stream << "error" << flush;
        }
        return s_constraintValue;
    }
} optTimeConstraint;

const struct {
    int       d_line;              // line number
    ElemType  d_type;              // option type
    void     *d_linkedVariable_p;  // linked variable attribute(s)
    void     *d_constraint_p;      // linked variable attribute(s)
} OPTION_TYPEINFO[] = {
    // This array provides has an entry for every supported option type with a
    // linked variable and not, and having a constraint and not.

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
int                         defaultInt           = 1234567;
Int64                       defaultInt64         = 123456789LL;
double                      defaultDouble        = 0.015625;  // 1/64
bsl::string                 defaultString        ( "ABCDEFGHIJ"    , GA);
bdlt::Datetime              defaultDatetime(1234, 12, 3, 4, 5, 6);
bdlt::Date                  defaultDate(1234, 4, 6);
bdlt::Time                  defaultTime(7, 8, 9, 10);
bsl::vector<char>           defaultCharArray    (1, defaultChar    , GA);
bsl::vector<int>            defaultIntArray     (1, defaultInt     , GA);
bsl::vector<Int64>          defaultInt64Array   (1, defaultInt64   , GA);
bsl::vector<double>         defaultDoubleArray  (1, defaultDouble  , GA);
bsl::vector<bsl::string>    defaultStringArray  (1, defaultString  , GA);
bsl::vector<bdlt::Datetime> defaultDatetimeArray(1, defaultDatetime, GA);
bsl::vector<bdlt::Date>     defaultDateArray    (1, defaultDate    , GA);
bsl::vector<bdlt::Time>     defaultTimeArray    (1, defaultTime    , GA);

static const struct {
    int         d_line;     // line number
    ElemType    d_type;     // option type
    const void *d_value_p;  // default value attribute(s)
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

char                        parsedChar          = 'a';
int                         parsedInt           = 123654;
Int64                       parsedInt64         = 987654321LL;

double                      parsedDouble        = 0.376739501953125;

bsl::string                 parsedString        ( "someString"   , GA);
bdlt::Datetime              parsedDatetime  (2008,  7, 22,  4,  6,  8);
bdlt::Date                  parsedDate      (2007,  8, 22);
bdlt::Time                  parsedTime                    ( 8,  6,  4);
bsl::vector<char>           parsedCharArray    (1, parsedChar    , GA);
bsl::vector<int>            parsedIntArray     (1, parsedInt     , GA);
bsl::vector<Int64>          parsedInt64Array   (1, parsedInt64   , GA);
bsl::vector<double>         parsedDoubleArray  (1, parsedDouble  , GA);
bsl::vector<bsl::string>    parsedStringArray  (1, parsedString  , GA);
bsl::vector<bdlt::Datetime> parsedDatetimeArray(1, parsedDatetime, GA);
bsl::vector<bdlt::Date>     parsedDateArray    (1, parsedDate    , GA);
bsl::vector<bdlt::Time>     parsedTimeArray    (1, parsedTime    , GA);

static const struct {
    int         d_line;     // line number
    ElemType    d_type;     // option type
    const void *d_value_p;  // default value attribute(s)
    const char *d_input_p;  // default value attribute(s)
} PARSABLE_VALUES[] = {

  { L_, Ot::e_CHAR,           &parsedChar,           "a"                  }
, { L_, Ot::e_INT,            &parsedInt,           "123654"              }
, { L_, Ot::e_INT64,          &parsedInt64,         "987654321"           }
, { L_, Ot::e_DOUBLE,         &parsedDouble,        "0.376739501953125"   }
, { L_, Ot::e_STRING,         &parsedString,        "someString"          }
, { L_, Ot::e_DATETIME,       &parsedDatetime,      "2008-07-22T04:06:08" }
, { L_, Ot::e_DATE,           &parsedDate,          "2007-08-22"          }
, { L_, Ot::e_TIME,           &parsedTime,          "08:06:04"            }

, { L_, Ot::e_CHAR_ARRAY,     &parsedCharArray,     "a"                   }
, { L_, Ot::e_INT_ARRAY,      &parsedIntArray,      "123654"              }
, { L_, Ot::e_INT64_ARRAY,    &parsedInt64Array,    "987654321"           }
, { L_, Ot::e_DOUBLE_ARRAY,   &parsedDoubleArray,   "0.376739501953125"   }
, { L_, Ot::e_STRING_ARRAY,   &parsedStringArray,   "someString"          }
, { L_, Ot::e_DATETIME_ARRAY, &parsedDatetimeArray, "2008-07-22T04:06:08" }
, { L_, Ot::e_DATE_ARRAY,     &parsedDateArray,     "2007-08-22"          }
, { L_, Ot::e_TIME_ARRAY,     &parsedTimeArray,     "08:06:04"            }
};

enum { NUM_PARSABLE_VALUES = sizeof  PARSABLE_VALUES
                           / sizeof *PARSABLE_VALUES };

static const struct {
    int       d_line;          // line number
    ElemType  d_type;          // option type
    void     *d_constraint_p;  // test constraint
} PARSABLE_CONSTRAINTS[] = {
   //LINE TYPE                  CONSTRAINT
   //---- -------------------   ----------------------
   { L_,  Ot::e_CHAR,           &optCharConstraint     }
 , { L_,  Ot::e_INT,            &optIntConstraint      }
 , { L_,  Ot::e_INT64,          &optInt64Constraint    }
 , { L_,  Ot::e_DOUBLE,         &optDoubleConstraint   }
 , { L_,  Ot::e_STRING,         &optStringConstraint   }
 , { L_,  Ot::e_DATETIME,       &optDatetimeConstraint }
 , { L_,  Ot::e_DATE,           &optDateConstraint     }
 , { L_,  Ot::e_TIME,           &optTimeConstraint     }
 , { L_,  Ot::e_CHAR_ARRAY,     &optCharConstraint     }
 , { L_,  Ot::e_INT_ARRAY,      &optIntConstraint      }
 , { L_,  Ot::e_INT64_ARRAY,    &optInt64Constraint    }
 , { L_,  Ot::e_DOUBLE_ARRAY,   &optDoubleConstraint   }
 , { L_,  Ot::e_STRING_ARRAY,   &optStringConstraint   }
 , { L_,  Ot::e_DATETIME_ARRAY, &optDatetimeConstraint }
 , { L_,  Ot::e_DATE_ARRAY,     &optDateConstraint     }
 , { L_,  Ot::e_TIME_ARRAY,     &optTimeConstraint     }
};

enum { NUM_PARSABLE_CONSTRAINTS = sizeof  PARSABLE_CONSTRAINTS
                                / sizeof *PARSABLE_CONSTRAINTS };

BSLMF_ASSERT(static_cast<int>(NUM_PARSABLE_VALUES)
          == static_cast<int>(NUM_PARSABLE_CONSTRAINTS));

#undef GA

// ============================================================================
//                          HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

namespace {
namespace u {

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

                           // ======================
                           // function setConstraint
                           // ======================

void setConstraint(Obj *typeInfo, ElemType type, const void *address)
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
                            *static_cast<const OptCharConstraint *>(address)));
      } break;
      case Ot::e_INT:
      case Ot::e_INT_ARRAY: {
        typeInfo->setConstraint(balcl::Constraint::IntConstraint(
                             *static_cast<const OptIntConstraint *>(address)));
      } break;
      case Ot::e_INT64:
      case Ot::e_INT64_ARRAY: {
        typeInfo->setConstraint(balcl::Constraint::Int64Constraint(
                           *static_cast<const OptInt64Constraint *>(address)));
      } break;
      case Ot::e_DOUBLE:
      case Ot::e_DOUBLE_ARRAY: {
        typeInfo->setConstraint(balcl::Constraint::DoubleConstraint(
                          *static_cast<const OptDoubleConstraint *>(address)));
      } break;
      case Ot::e_STRING:
      case Ot::e_STRING_ARRAY: {
        typeInfo->setConstraint(balcl::Constraint::StringConstraint(
                          *static_cast<const OptStringConstraint *>(address)));
      } break;
      case Ot::e_DATETIME:
      case Ot::e_DATETIME_ARRAY: {
        typeInfo->setConstraint(balcl::Constraint::DatetimeConstraint(
                        *static_cast<const OptDatetimeConstraint *>(address)));
      } break;
      case Ot::e_DATE:
      case Ot::e_DATE_ARRAY: {
        typeInfo->setConstraint(balcl::Constraint::DateConstraint(
                            *static_cast<const OptDateConstraint *>(address)));
      } break;
      case Ot::e_TIME:
      case Ot::e_TIME_ARRAY: {
        typeInfo->setConstraint(balcl::Constraint::TimeConstraint(
                            *static_cast<const OptTimeConstraint *>(address)));
      } break;
    };
}

                         // ==========================
                         // function setLinkedVariable
                         // ==========================

void setLinkedVariable(Obj *typeInfo, ElemType type, void *address)
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

void setType(Obj *typeInfo, ElemType type)
    // Set the 'balcl::OptionType' element of the specified 'typeInfo' to the
    // specified 'type'.  The behavior is undefined unless
    // 'balcl::OptionType::e_VOID != type').  Note that this resets both the
    // linked variable and constraint of 'typeInfo'.
{
    switch (type) {
      case Ot::e_VOID: {
        BSLS_ASSERT(!"reachable");
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

                          // ====================
                          // function setTypeInfo
                          // ====================

void setTypeInfo(Obj      *typeInfo,
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

Obj *constructTypeInfo(void *buffer,  ElemType type, void *variable)
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

    Obj *ptr = 0;

    switch (type) {
//v---^
  case Ot::e_VOID: {
    BSLS_ASSERT(!"Reached: 'e_VOID'");
  } break;
  case Ot::e_BOOL: {
    ptr = new (buffer) Obj(static_cast<Ot::Bool          *>(variable));
  } break;
  case Ot::e_CHAR: {
    ptr = new (buffer) Obj(static_cast<Ot::Char          *>(variable));
  } break;
  case Ot::e_INT: {
    ptr = new (buffer) Obj(static_cast<Ot::Int           *>(variable));
  } break;
  case Ot::e_INT64: {
    ptr = new (buffer) Obj(static_cast<Ot::Int64         *>(variable));
  } break;
  case Ot::e_DOUBLE: {
    ptr = new (buffer) Obj(static_cast<Ot::Double        *>(variable));
  } break;
  case Ot::e_STRING: {
    ptr = new (buffer) Obj(static_cast<Ot::String        *>(variable));
  } break;
  case Ot::e_DATETIME: {
    ptr = new (buffer) Obj(static_cast<Ot::Datetime      *>(variable));
  } break;
  case Ot::e_DATE: {
    ptr = new (buffer) Obj(static_cast<Ot::Date          *>(variable));
  } break;
  case Ot::e_TIME: {
    ptr = new (buffer) Obj(static_cast<Ot::Time          *>(variable));
  } break;
  case Ot::e_CHAR_ARRAY: {
    ptr = new (buffer) Obj(static_cast<Ot::CharArray     *>(variable));
  } break;
  case Ot::e_INT_ARRAY: {
    ptr = new (buffer) Obj(static_cast<Ot::IntArray      *>(variable));
  } break;
  case Ot::e_INT64_ARRAY: {
    ptr = new (buffer) Obj(static_cast<Ot::Int64Array    *>(variable));
  } break;
  case Ot::e_DOUBLE_ARRAY: {
    ptr = new (buffer) Obj(static_cast<Ot::DoubleArray   *>(variable));
  } break;
  case Ot::e_STRING_ARRAY: {
    ptr = new (buffer) Obj(static_cast<Ot::StringArray   *>(variable));
  } break;
  case Ot::e_DATETIME_ARRAY: {
    ptr = new (buffer) Obj(static_cast<Ot::DatetimeArray *>(variable));
  } break;
  case Ot::e_DATE_ARRAY: {
    ptr = new (buffer) Obj(static_cast<Ot::DateArray     *>(variable));
  } break;
  case Ot::e_TIME_ARRAY: {
    ptr = new (buffer) Obj(static_cast<Ot::TimeArray     *>(variable));
  } break;
//^---v
    }
    return ptr;
}

Obj *constructTypeInfo(void     *buffer,
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

    Obj *ptr = 0;

    switch (type) {
//v---^
  case Ot::e_VOID: {
    BSLS_ASSERT(!"Reached: 'e_VOID'");
  } break;
  case Ot::e_BOOL: {
    BSLS_ASSERT(!"Reached: 'e_BOOL'");
  } break;
  case Ot::e_CHAR: {
    ptr = new (buffer) Obj(static_cast<Ot::Char          *>(variable),
                       *static_cast<Constraint::CharConstraint *>(constraint));
  } break;
  case Ot::e_INT: {
    ptr = new (buffer) Obj(static_cast<Ot::Int           *>(variable),
                        *static_cast<Constraint::IntConstraint *>(constraint));
  } break;
  case Ot::e_INT64: {
    ptr = new (buffer) Obj(static_cast<Ot::Int64         *>(variable),
                      *static_cast<Constraint::Int64Constraint *>(constraint));
  } break;
  case Ot::e_DOUBLE: {
    ptr = new (buffer) Obj(static_cast<Ot::Double        *>(variable),
                     *static_cast<Constraint::DoubleConstraint *>(constraint));
  } break;
  case Ot::e_STRING: {
    ptr = new (buffer) Obj(static_cast<Ot::String        *>(variable),
                     *static_cast<Constraint::StringConstraint *>(constraint));
  } break;
  case Ot::e_DATETIME: {
    ptr = new (buffer) Obj(static_cast<Ot::Datetime      *>(variable),
                   *static_cast<Constraint::DatetimeConstraint *>(constraint));
  } break;
  case Ot::e_DATE: {
    ptr = new (buffer) Obj(static_cast<Ot::Date          *>(variable),
                       *static_cast<Constraint::DateConstraint *>(constraint));
  } break;
  case Ot::e_TIME: {
    ptr = new (buffer) Obj(static_cast<Ot::Time          *>(variable),
                       *static_cast<Constraint::TimeConstraint *>(constraint));
  } break;
  case Ot::e_CHAR_ARRAY: {
    ptr = new (buffer) Obj(static_cast<Ot::CharArray     *>(variable),
                       *static_cast<Constraint::CharConstraint *>(constraint));
  } break;
  case Ot::e_INT_ARRAY: {
    ptr = new (buffer) Obj(static_cast<Ot::IntArray      *>(variable),
                        *static_cast<Constraint::IntConstraint *>(constraint));
  } break;
  case Ot::e_INT64_ARRAY: {
    ptr = new (buffer) Obj(static_cast<Ot::Int64Array    *>(variable),
                      *static_cast<Constraint::Int64Constraint *>(constraint));
  } break;
  case Ot::e_DOUBLE_ARRAY: {
    ptr = new (buffer) Obj(static_cast<Ot::DoubleArray   *>(variable),
                     *static_cast<Constraint::DoubleConstraint *>(constraint));
  } break;
  case Ot::e_STRING_ARRAY: {
    ptr = new (buffer) Obj(static_cast<Ot::StringArray   *>(variable),
                     *static_cast<Constraint::StringConstraint *>(constraint));
  } break;
  case Ot::e_DATETIME_ARRAY: {
    ptr = new (buffer) Obj(static_cast<Ot::DatetimeArray *>(variable),
                   *static_cast<Constraint::DatetimeConstraint *>(constraint));
  } break;
  case Ot::e_DATE_ARRAY: {
    ptr = new (buffer) Obj(static_cast<Ot::DateArray     *>(variable),
                       *static_cast<Constraint::DateConstraint *>(constraint));
  } break;
  case Ot::e_TIME_ARRAY: {
    ptr = new (buffer) Obj(static_cast<Ot::TimeArray     *>(variable),
                       *static_cast<Constraint::TimeConstraint *>(constraint));
  } break;
//^---v
    }
    return ptr;
}

                          // ========================
                           // function parseTypeInfo
                          // ========================

int parseTypeInfo(const char **endpos,
                  const Obj&   typeInfo,
                  const char  *input)
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
        if (Parser::skipWhiteSpace   (&input, input)
         || Parser::skipRequiredToken(&input, input, "VARIABLE")
         || Parser::skipWhiteSpace   (&input, input))
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

    if (Parser::skipWhiteSpace   (&input, input)
     || Parser::skipRequiredToken(&input, input, "CONSTRAINT")
     || Parser::skipWhiteSpace   (&input, input))
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

                          // =============================
                          // function normalizeIndentation
                          // =============================

void normalizeIndentation(bsl::string        *output,
                          const bsl::string&  input,
                          int                 level,
                          int                 spacesPerLevel)
    // Load to the specified 'output' a "normalized" version of the specified
    // 'input' where 'input' is the output of the 'TypeInfo' 'print' method
    // called with the specified 'level' and 'spacesPerlevel'.  The
    // "normalized' version corresponds to calling 'print' with 'level' and
    // 'spacesPerLevel' having the values 0 and 4, respectively.  The behavior
    // is undefined unless 'level' and 'spacesPerLevel' are both non-negative,
    // and unless 'output->empty()'.
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

        if (i < lines.size() - 1) {  // no newline on last line
            line += '\n';
        }

        (*output) += line;
    }
}

#define CASE(ENUM)                                                            \
    case ENUM: {                                                              \
      return      element.the<      Ot::EnumToType<ENUM>::type  >()           \
             == *(static_cast<const Ot::EnumToType<ENUM>::type *>(value));    \
    } break;

bool areEqualValues(const OptionValue& element, const void *value)
    // Return 'true' if the value of the specified 'element' is the same as
    // that found at the specified 'value', and 'false' value otherwise.  The
    // behavior is undefined unless
    // 'balcl::OptionType::e_VOID != element.type()',
    // 'balcl::OptionType::e_BOOL != element.type()', and 'value' can be
    // (validly) cast to 'balcl::OptionType::EnumToType<ENUM>::type *' where
    // 'ENUM' matches 'element.type()'.
{
    BSLS_ASSERT(value);

    switch (element.type()) {
      case Ot::e_VOID: {
        BSLS_ASSERT(!"Reached: e_VOID");
      } break;
      case Ot::e_BOOL: {
        BSLS_ASSERT(!"Reached: e_BOOL");
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
      default: {
        BSLS_ASSERT(!"Reached: Unkown");
      } break;
    }

    return false;
}
#undef CASE

#define CASE_SCALAR(ENUM)                                                     \
    case ENUM: {                                                              \
          stream << *(static_cast<const Ot::EnumToType<ENUM>::type *>(value)) \
                 << endl;                                                     \
    } break;

#define CASE_ARRAY(ENUM)                                                      \
    case ENUM: {                                                              \
          stream                                                              \
            << (*(static_cast<const Ot::EnumToType<ENUM>::type *>(value)))[0] \
            << endl;                                                          \
    } break;

void printValue(ostream& stream, Ot::Enum type, const void *value)
{
    BSLS_ASSERT(value);

    switch (type) {
      case Ot::e_VOID: {
        BSLS_ASSERT(!"Reached: e_VOID");
      } break;
      case Ot::e_BOOL: {
        BSLS_ASSERT(!"Reached: e_BOOL");
      } break;
      CASE_SCALAR(Ot::e_CHAR)
      CASE_SCALAR(Ot::e_INT)
      CASE_SCALAR(Ot::e_INT64)
      CASE_SCALAR(Ot::e_DOUBLE)
      CASE_SCALAR(Ot::e_STRING)
      CASE_SCALAR(Ot::e_DATETIME)
      CASE_SCALAR(Ot::e_DATE)
      CASE_SCALAR(Ot::e_TIME)
      CASE_ARRAY( Ot::e_CHAR_ARRAY)
      CASE_ARRAY( Ot::e_INT_ARRAY)
      CASE_ARRAY( Ot::e_INT64_ARRAY)
      CASE_ARRAY( Ot::e_DOUBLE_ARRAY)
      CASE_ARRAY( Ot::e_STRING_ARRAY)
      CASE_ARRAY( Ot::e_DATETIME_ARRAY)
      CASE_ARRAY( Ot::e_DATE_ARRAY)
      CASE_ARRAY( Ot::e_TIME_ARRAY)
      default: {
        BSLS_ASSERT(!"Reached: Unkown");
      } break;
    }
}
#undef CASE_SCALAR
#undef CASE_ARRAY

const char *strdiffpos(const char *str1, const char *str2)
{
    BSLS_ASSERT(str1);
    BSLS_ASSERT(str2);

    const char *p1 = str1;
    const char *p2 = str2;

    for ( ; *p1 && *p2; ++p1, ++p2) {
        if (*p1 != *p2) {
            return p1;
        }
    }

    if (0 == *p1 && 0 == *p2) {
        return 0;
    }

    BSLS_ASSERT(*p1 || *p2);  // one is longer
    return p1;
}

#undef CASE

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

    switch (test) { case 0:  // Zero is always the leading case.
      case 9: {
        // -------------------------------------------------------------------
        // 'balcl::TypeInfoUtil': 'parseAndValidate'
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // -------------------------------------------------------------------

        if (verbose) cout
                        << endl
                        << "'balcl::TypeInfoUtil': 'parseAndValidate'" << endl
                        << "-----------------------------------------" << endl;

        if (veryVerbose) cout << "\tCheck basic behavior" << endl;

        for (int i = 0; i < NUM_PARSABLE_VALUES; ++i) {
            const int              LINE  = PARSABLE_VALUES[i].d_line;
            const ElemType         TYPE  = PARSABLE_VALUES[i].d_type;
            const void     * const VALUE = PARSABLE_VALUES[i].d_value_p;
            const char     * const INPUT = PARSABLE_VALUES[i].d_input_p;

            if (veryVerbose) {
                P_(LINE) P_(TYPE) P_(VALUE) P(INPUT)
            }

            OptionValue             element(TYPE);
            bsls::ObjectBuffer<Obj> xObjectBuffer;

            Obj *objPtr = u::constructTypeInfo(xObjectBuffer.buffer(),
                                               TYPE,
                                               0);  // no linked variable
                                                    // no constraint
            bsl::ostringstream oss;

            bool wasParsedOK = ObjUtil::parseAndValidate(&element,
                                                         bsl::string(INPUT),
                                                         *objPtr,
                                                         oss);
            ASSERTV(LINE, wasParsedOK);
            ASSERTV(LINE, oss.str().empty());

            if (veryVerbose) {
                P(element)
                u::printValue(cout, TYPE, VALUE);
            }

            ASSERTV(LINE, u::areEqualValues(element, VALUE));

            const int              LINE_C     = PARSABLE_CONSTRAINTS[i].d_line;
            const ElemType         TYPE_C     = PARSABLE_CONSTRAINTS[i].d_type;
            const void     * const CONSTRAINT = PARSABLE_CONSTRAINTS[i].
                                                                d_constraint_p;

            ASSERTV(LINE, LINE_C, TYPE == TYPE_C);

            // Set a constraint that should pass.

            u::setConstraint(objPtr, TYPE_C, CONSTRAINT);

            OptConstraint::s_constraintValue = true;
            oss.str(""); oss.clear();
            element.reset(); element.setType(TYPE);

            wasParsedOK = ObjUtil::parseAndValidate(&element,
                                                    bsl::string(INPUT),
                                                    *objPtr,
                                                    oss);
            ASSERTV(LINE, wasParsedOK);
            ASSERTV(LINE, oss.str().empty());
            ASSERTV(LINE, u::areEqualValues(element, VALUE));

            // Set a constraint that should not pass.

            OptConstraint::s_constraintValue = false;
            oss.str(""); oss.clear();
            element.reset(); element.setType(TYPE);

            wasParsedOK = ObjUtil::parseAndValidate(&element,
                                                    bsl::string(INPUT),
                                                    *objPtr,
                                                    oss);
            ASSERTV(LINE, !wasParsedOK);
            ASSERTV(LINE, !oss.str().empty());

            objPtr->~Obj();
        }

        if (veryVerbose) cout << "\tNegative testing." << endl;

        bsls::AssertTestHandlerGuard hG;

        for (int i = 0; i < NUM_PARSABLE_VALUES; ++i) {
            const int              LINE  = PARSABLE_VALUES[i].d_line;
            const ElemType         TYPE  = PARSABLE_VALUES[i].d_type;
            const void     * const VALUE = PARSABLE_VALUES[i].d_value_p;
            const char     * const INPUT = PARSABLE_VALUES[i].d_input_p;

            if (veryVerbose) {
                P_(LINE) P_(TYPE) P_(VALUE) P(INPUT)
            }

            bsls::ObjectBuffer<Obj> xObjectBuffer;

            Obj *objPtr = u::constructTypeInfo(xObjectBuffer.buffer(),
                                               TYPE,
                                               0);  // no linked variable
                                                         // no constraint
            bsl::ostringstream oss;
            OptionValue        elementOK(TYPE);

            ASSERT_PASS(ObjUtil::parseAndValidate(&elementOK,
                                                  bsl::string(INPUT),
                                                  *objPtr,
                                                  oss));

            ASSERT_FAIL(ObjUtil::parseAndValidate(0,
                                                  bsl::string(INPUT),
                                                  *objPtr,
                                                  oss));

            const ElemType TYPE_OFFSET = Ot::e_TIME_ARRAY == TYPE
                                       ? Ot::e_CHAR
                                       : static_cast<ElemType>(
                                            static_cast<int>(TYPE) + 1);

            OptionValue elementNG(TYPE_OFFSET);

            ASSERT_FAIL(ObjUtil::parseAndValidate(&elementNG,
                                                  bsl::string(INPUT),
                                                  *objPtr,
                                                  oss));
        }
      } break;
      case 8: {
        // -------------------------------------------------------------------
        // TESTING ADDITIONAL CONSTRUCTORS
        // Concerns:
        //
        // Plan:
        //
        // Testing:
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
        // -------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING ADDITIONAL CONSTRUCTORS" <<endl
                          << "-------------------------------" <<endl;

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

            bsls::ObjectBuffer<Obj> xObjectBuffer;

            Obj *xPtr = u::constructTypeInfo(xObjectBuffer.buffer(),
                                             TYPE,
                                             VARIABLE);  // ACTION
            Obj& mX = *xPtr; const Obj X = mX;

            ASSERT(TYPE     ==                     X.type());
            ASSERT(VARIABLE == static_cast<void *>(X.linkedVariable()));

            // TBD: check allocator.  Pass allocator or 0 to 'construct...'

            if (Ot::e_BOOL != TYPE && 0 != CONSTRAINT) {
                bsls::ObjectBuffer<Obj> yObjectBuffer;

                Obj *yPtr = u::constructTypeInfo(yObjectBuffer.buffer(),
                                                      TYPE,
                                                      VARIABLE,
                                                      CONSTRAINT);  // ACTION
                Obj&  mY = *yPtr; const Obj Y = mY;

                ASSERT(TYPE     ==                     Y.type());
                ASSERT(VARIABLE == static_cast<void *>(Y.linkedVariable()));
                ASSERT(0        != static_cast<void *>(Y.constraint().ptr()));

                yPtr->~Obj();
            }

            xPtr->~Obj();
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY-ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, such that the two objects subsequently
        //   have the same value.
        //
        // Concerns:
        //: 1 The assignment operator can change the value of any modifiable
        //:   target object to that of any source object.
        //:
        //: 2 The allocator address held by the target object is unchanged.
        //:
        //: 3 Any memory allocation is from the target object's allocator.
        //:
        //: 4 The signature and return type are standard.
        //:
        //: 5 The reference returned is to the target object (i.e., '*this').
        //:
        //: 6 The value of the source object is not modified.
        //:
        //: 7 The allocator address held by the source object is unchanged.
        //:
        //: 8 QoI: Assigning a source object having the default-constructed
        //:   value allocates no memory.
        //:
        //: 9 Any memory allocation is exception neutral.
        //:
        //:10 Assigning an object to itself behaves as expected (alias-safety).
        //:
        //:11 Every object releases any allocated memory at destruction.
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a member-function
        //:   pointer having the appropriate signature and return type for the
        //:   copy-assignment operator defined in this component.  (C-4)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for all typical
        //:     implementations of individual attribute types: ('Y') "Yes",
        //:     ('N') "No", or ('?') "implementation-dependent".
        //:
        //: 4 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-3:  (C-1..2, 5..8, 11)
        //:
        //:   1 Use the value constructor and a "scratch" allocator to create
        //:     two 'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop that iterates over each row 'R2'
        //:     (representing a distinct object value, 'W') in the table
        //:     described in P-3:
        //:
        //:   3 For each of the iterations (P-4.2):  (C-1..2, 5..8, 11)
        //:
        //:     1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:     2 Use the value constructor and 'oa' to create a modifiable
        //:       'Obj', 'mX', having the value 'W'.
        //:
        //:     3 Assign 'mX' from 'Z' in the presence of injected exceptions
        //:       (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:
        //:     4 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-5)
        //:
        //:     5 Use the equality-comparison operator to verify that: (C-1, 6)
        //:
        //:       1 The target object, 'mX', now has the same value as that of
        //:         'Z'.  (C-1)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'.  (C-6)
        //:
        //:     6 Use the 'allocator' accessor of both 'mX' and 'Z' to verify
        //:       that the respective allocator addresses held by the target
        //:       and source objects are unchanged.  (C-2, 7)
        //:
        //:     7 Use the appropriate test allocators to verify that:
        //:       (C-8, 11)
        //:
        //:       1 For an object that (a) is initialized with a value that did
        //:         NOT require memory allocation, and (b) is then assigned a
        //:         value that DID require memory allocation, the target object
        //:         DOES allocate memory from its object allocator only
        //:         (irrespective of the specific number of allocations or the
        //:         total amount of memory allocated); also cross check with
        //:         what is expected for 'mX' and 'Z'.
        //:
        //:       2 An object that is assigned a value that did NOT require
        //:         memory allocation, does NOT allocate memory from its object
        //:         allocator; also cross check with what is expected for 'Z'.
        //:
        //:       3 No additional memory is allocated by the source object.
        //:         (C-8)
        //:
        //:       4 All object memory is released when the object is destroyed.
        //:         (C-11)
        //:
        //: 5 Repeat steps similar to those described in P-2 except that, this
        //:   time, there is no inner loop (as in P-4.2); instead, the source
        //:   object, 'Z', is a reference to the target object, 'mX', and both
        //:   'mX' and 'ZZ' are initialized to have the value 'V'.  For each
        //:   row (representing a distinct object value, 'V') in the table
        //:   described in P-3:  (C-9..10)
        //:
        //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor and 'oa' to create a modifiable 'Obj'
        //:     'mX'; also use the value constructor and a distinct "scratch"
        //:     allocator to create a 'const' 'Obj' 'ZZ'.
        //:
        //:   3 Let 'Z' be a 'const' reference to 'mX'.
        //:
        //:   4 Assign 'mX' from 'Z' in the presence of injected exceptions
        //:     (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:     (C-9)
        //:
        //:   5 Verify that the address of the return value is the same as that
        //:     of 'mX'.
        //:
        //:   6 Use the equality-comparison operator to verify that the
        //:     target object, 'Z', still has the same value as that of 'ZZ'.
        //:     (C-10)
        //:
        //:   7 Use the 'allocator' accessor of 'mX' to verify that it is still
        //:     the object allocator.
        //:
        //:   8 Use the appropriate test allocators to verify that:
        //:
        //:     1 Any memory that is allocated is from the object allocator.
        //:
        //:     2 No additional (e.g., temporary) object memory is allocated
        //:       when assigning an object value that did NOT initially require
        //:       allocated memory.
        //:
        //:     3 All object memory is released when the object is destroyed.
        //:
        //: 6 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-3)
        //
        // Testing:
        //   operator=(const baltzo::LocalTimeDescriptor& rhs);
        //   CONCERN: All memory allocation is from the object's allocator.
        //   CONCERN: All memory allocation is exception neutral.
        //   CONCERN: Object value is independent of the object allocator.
        //   CONCERN: There is no temporary allocation from any allocator.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY-ASSIGNMENT OPERATOR" << endl
                          << "========================" << endl;

        if (verbose) cout <<
           "\nVerify that the signature and return type are standard." << endl;
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            (void)operatorAssignment;  // quash potential compiler warning
        }

        if (verbose) cout << "\n\tTesting assignment operator." << endl;

        for (int i = 0; i < NUM_OPTION_TYPEINFO; ++i) {
            const int       LINE1 = OPTION_TYPEINFO[i].d_line;
            const ElemType  TYPE1 = OPTION_TYPEINFO[i].d_type;
            void       *VARIABLE1 = OPTION_TYPEINFO[i].d_linkedVariable_p;
            void     *CONSTRAINT1 = OPTION_TYPEINFO[i].d_constraint_p;

            if (veryVerbose) {
                T_ P_(LINE1) P_(i) P(TYPE1)
                T_ P_(VARIABLE1) P(CONSTRAINT1)
            }

            Obj mX;  const Obj& X = mX;
            Obj mZ;  const Obj& Z = mZ;

            u::setTypeInfo(&mX, TYPE1, VARIABLE1, CONSTRAINT1);
            u::setTypeInfo(&mZ, TYPE1, VARIABLE1, CONSTRAINT1);

            mZ.setConstraint(X.constraint());  // shared constraint

            ASSERT(X == Z);

            ASSERT(&X == &(mX = X));  // ACTION

            ASSERT(X == Z);

            for (int j = 0; j < NUM_OPTION_TYPEINFO; ++j) {
                const int       LINE2 = OPTION_TYPEINFO[j].d_line;
                const ElemType  TYPE2 = OPTION_TYPEINFO[j].d_type;
                void       *VARIABLE2 = OPTION_TYPEINFO[j].d_linkedVariable_p;
                void     *CONSTRAINT2 = OPTION_TYPEINFO[j].d_constraint_p;

                if (veryVerbose) {
                    T_ T_ P_(LINE2) P_(i) P(TYPE2)
                    T_ T_ P_(VARIABLE2) P(CONSTRAINT2)
                }

                bslma::TestAllocator saX("saX", veryVeryVeryVerbose);
                bslma::TestAllocator saY("saY", veryVeryVeryVerbose);

                Obj mX(&saX);  const Obj& X = mX;
                u::setTypeInfo(&mX, TYPE1, VARIABLE1, CONSTRAINT1);

                Obj mY(&saY);  const Obj& Y = mY;
                u::setTypeInfo(&mY, TYPE2, VARIABLE2, CONSTRAINT2);

                bslma::TestAllocatorMonitor samX(&saX);
                bslma::TestAllocatorMonitor samY(&saY);

                ASSERT(&Y == &(mY = X));  // ACTION

                LOOP2_ASSERT(LINE1, LINE2, samX.isTotalSame());
                LOOP2_ASSERT(LINE1, LINE2, samY.isTotalSame());

                LOOP2_ASSERT(LINE1, LINE2, X    == Y);
                LOOP2_ASSERT(LINE1, LINE2, &saX == X.allocator());
                LOOP2_ASSERT(LINE1, LINE2, &saY == Y.allocator());
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the two objects have the same value.
        //
        // Concerns:
        //: 1 The copy constructor (with or without a supplied allocator)
        //:   creates an object having the same value as that of the supplied
        //:   original object.
        //:
        //: 2 If an allocator is NOT supplied to the copy constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object (i.e., the
        //:   allocator of the original object is never copied).
        //:
        //: 3 If an allocator IS supplied to the copy constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 5 Supplying an allocator to the copy constructor has no effect
        //:   on subsequent object values.
        //:
        //: 6 Any memory allocation is from the object allocator.
        //:
        //: 7 There is no temporary memory allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 The original object is passed as a 'const' reference.
        //:
        //:10 The value of the original object is unchanged.
        //:
        //:11 The allocator address held by the original object is unchanged.
        //:
        //:12 QoI: Copying an object having the default-constructed value
        //:   allocates no memory.
        //:
        //:13 Any memory allocation is exception neutral.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for all typical
        //:     implementations of individual attribute types: ('Y') "Yes",
        //:     ('N') "No", or ('?') "implementation-dependent".
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1:  (C-1..12)
        //:
        //:   1 Use the value constructor and a "scratch" allocator to create
        //:     two 'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop creating three distinct objects in turn,
        //:     each using the copy constructor on 'Z' from P-2.1, but
        //:     configured differently: (a) without passing an allocator,
        //:     (b) passing a null allocator address explicitly, and (c)
        //:     passing the address of a test allocator distinct from the
        //:     default.
        //:
        //:   3 For each of these three iterations (P-2.2):  (C-1..12)
        //:
        //:     1 Create three 'bslma::TestAllocator' objects, and install one
        //:       as the current default allocator (note that a ubiquitous test
        //:       allocator is already installed as the global allocator).
        //:
        //:     2 Use the copy constructor to dynamically create an object 'X',
        //:       with its object allocator configured appropriately (see
        //:       P-2.2), supplying it the 'const' object 'Z' (see P-2.1); use
        //:       a distinct test allocator for the object's footprint.  (C-9)
        //:
        //:     3 Use the equality-comparison operator to verify that:
        //:       (C-1, 5, 10)
        //:
        //:       1 The newly constructed object, 'X', has the same value as
        //:         that of 'Z'.  (C-1, 5)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'.  (C-10)
        //:
        //:     4 Use the 'allocator' accessor of each underlying attribute
        //:       capable of allocating memory to ensure that its object
        //:       allocator is properly installed; also use the 'allocator'
        //:       accessor of 'X' to verify that its object allocator is
        //:       properly installed, and use the 'allocator' accessor of 'Z'
        //:       to verify that the allocator address that it holds is
        //:       unchanged.  (C-6, 11)
        //:
        //:     5 Use the appropriate test allocators to verify that:  (C-2..4,
        //:       7..8, 12)
        //:
        //:       1 An object that IS expected to allocate memory does so
        //:         from the object allocator only (irrespective of the
        //:         specific number of allocations or the total amount of
        //:         memory allocated).  (C-2, 4)
        //:
        //:       2 An object that is expected NOT to allocate memory doesn't.
        //:         (C-12)
        //:
        //:       3 If an allocator was supplied at construction (P-2.1c), the
        //:         current default allocator doesn't allocate any memory.
        //:         (C-3)
        //:
        //:       4 No temporary memory is allocated from the object allocator.
        //:         (C-7)
        //:
        //:       5 All object memory is released when the object is destroyed.
        //:         (C-8)
        //:
        //: 3 Test again, using the data of P-1, but this time just for the
        //:   supplied allocator configuration (P-2.2c), and create the object
        //:   as an automatic variable in the presence of injected exceptions
        //:   (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:   (C-13)
        //
        // Testing:
        //   TypeInfo(const TypeInfo& original, *bA = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;

        bslma::TestAllocator         sa("supplied", veryVeryVeryVerbose);
        bslma::TestAllocator         da("default",  veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout << "\t\tWithout passing in an allocator." << endl;

        for (int i = 0; i < NUM_OPTION_TYPEINFO; ++i) {
            const int       LINE       = OPTION_TYPEINFO[i].d_line;
            const ElemType  TYPE       = OPTION_TYPEINFO[i].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[i].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[i].d_constraint_p;

            Obj mX;  const Obj& X = mX;

            u::setTypeInfo(&mX, TYPE, VARIABLE, CONSTRAINT);

            bslma::TestAllocatorMonitor dam(&da);

            Obj mY(X);  const Obj& Y = mY;  // ACTION

            LOOP_ASSERT(LINE, dam.isTotalSame());

            LOOP_ASSERT(LINE, X    == Y);
            LOOP_ASSERT(LINE, &da  == Y.allocator());
        }

        if (verbose) cout << "\t\tPassing in an allocator." << endl;

        for (int i = 0; i < NUM_OPTION_TYPEINFO; ++i) {
            const int       LINE       = OPTION_TYPEINFO[i].d_line;
            const ElemType  TYPE       = OPTION_TYPEINFO[i].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[i].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[i].d_constraint_p;

            Obj mX;  const Obj& X = mX;

            u::setTypeInfo(&mX, TYPE, VARIABLE, CONSTRAINT);

            bslma::TestAllocatorMonitor dam(&da);
            bslma::TestAllocatorMonitor sam(&sa);

            {
                Obj mY(X, &sa);  const Obj& Y = mY;  // ACTION

                LOOP_ASSERT(LINE, X   == Y);
                LOOP_ASSERT(LINE, &sa == Y.allocator());
            }

            LOOP_ASSERT(LINE, sam.isInUseSame());
            LOOP_ASSERT(LINE, sam.isTotalSame());
            LOOP_ASSERT(LINE, dam.isInUseSame());
            LOOP_ASSERT(LINE, dam.isTotalSame());
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if each of
        //:   their corresponding salient attributes respectively compares
        //:   equal.
        //:
        //: 2 All salient attributes participate in the comparison.
        //:
        //: 3 No non-salient attributes (i.e., 'allocator') participate.
        //:
        //: 4 'true  == (X == X)'  (i.e., identity)
        //:
        //: 5 'false == (X != X)'  (i.e., identity)
        //:
        //: 6 'X == Y' if and only if 'Y == X'  (i.e., commutativity)
        //:
        //: 7 'X != Y' if and only if 'Y != X'  (i.e., commutativity)
        //:
        //: 8 'X != Y' if and only if '!(X == Y)'
        //:
        //: 9 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //:10 Non-modifiable objects can be compared (i.e., 'const' objects and
        //:   'const' references).
        //:
        //:11 No memory allocation occurs as a result of comparison (e.g., the
        //:   arguments are not passed by value).
        //:
        //:12 The equality operator's signature and return type are standard.
        //:
        //:13 The inequality operator's signature and return type are standard.
        //
        // Plan:
        //
        // Testing:
        //   bool operator==(const TypeInfo& lhs, rhs);
        //   bool operator!=(const TypeInfo& lhs, rhs);
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

            (void)operatorEq;  // quash potential compiler warnings
            (void)operatorNe;
        }

        if (verbose)
            cout << "\t\tCompare each pair of values (u, v) in W X W." << endl;

        for (int i = 0; i < NUM_OPTION_TYPEINFO; ++i) {
            const int       LINE1 = OPTION_TYPEINFO[i].d_line;
            const ElemType  TYPE1 = OPTION_TYPEINFO[i].d_type;
            void       *VARIABLE1 = OPTION_TYPEINFO[i].d_linkedVariable_p;
            void     *CONSTRAINT1 = OPTION_TYPEINFO[i].d_constraint_p;

            if (veryVerbose) { T_ P_(LINE1) P(TYPE1) }

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            u::setTypeInfo(&mX, TYPE1, VARIABLE1, CONSTRAINT1);

            bslma::TestAllocatorMonitor sam(&sa);

            LOOP_ASSERT(LINE1,  (X == X));   // ACTION
            LOOP_ASSERT(LINE1, !(X != X));   // ACTION

            LOOP_ASSERT(LINE1, sam.isTotalSame());

            for (int j = 0; j < NUM_OPTION_TYPEINFO; ++j) {
                const int       LINE2 = OPTION_TYPEINFO[j].d_line;
                const ElemType  TYPE2 = OPTION_TYPEINFO[j].d_type;
                void       *VARIABLE2 = OPTION_TYPEINFO[j].d_linkedVariable_p;
                void     *CONSTRAINT2 = OPTION_TYPEINFO[j].d_constraint_p;

                if (veryVerbose) { T_ T_ P_(LINE2) P(TYPE2) }

                Obj mY(&sa);  const Obj& Y = mY;

                u::setTypeInfo(&mY, TYPE2, VARIABLE2, CONSTRAINT2);

                bool isSame = (TYPE1       == TYPE2)
                           && (VARIABLE1   == VARIABLE2)
                           && (CONSTRAINT1 == 0)
                           && (CONSTRAINT2 == 0);

                bslma::TestAllocatorMonitor sam(&sa);

                LOOP2_ASSERT(LINE1, LINE2,  isSame == (X == Y));   // ACTION
                LOOP2_ASSERT(LINE1, LINE2, !isSame == (X != Y));   // ACTION

                LOOP2_ASSERT(LINE1, LINE2, sam.isTotalSame());

                if (TYPE1 == TYPE2) {
                    mY.setConstraint(X.constraint());  // shared constraints
                                                       // compare equal.

                    isSame = (TYPE1     == TYPE2)
                          && (VARIABLE1 == VARIABLE2);

                    bslma::TestAllocatorMonitor sam(&sa);

                    LOOP2_ASSERT(LINE1, LINE2,  isSame == (X == Y));
                    LOOP2_ASSERT(LINE1, LINE2, !isSame == (X != Y));

                    LOOP2_ASSERT(LINE1, LINE2, sam.isTotalSame());
                }
            }
        }
        // TBD: Show that allocator is not salient.
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR
        //
        // Concerns:
        //: 1 The 'print' method writes to the specified 'ostream' in the
        //:   expected format.
        //:
        //: 2 'operator<<' produces the same results as 'print' when level and
        //:   spaces-per-level arguments have their default value.
        //:
        //: 3 The return values of 'print' and 'operator<<' reference the
        //:   stream argument.
        //:
        //: 4 The signature and return types of 'print' and 'operator<<' are
        //:   standard.
        //:
        //: 5 The 'level' and 'spacesPerLevel' parameters have the correct
        //:   default values.
        //
        // Plan:
        //: 1 Use the "function address" idiom to confirm the signatures.
        //
        //: 2 Confirm that 'bdlb::HasPrintMethod<TypeInfo>::value' is 'true'
        //:   using a compile-time assertion.
        //
        // Testing:
        //   ostream& print(ostream& stream, int level = 0, int spl = 4) const;
        //   operator<<(ostream& stream, const TypeInfo& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRINT AND OUTPUT OPERATOR" << endl
                          << "-------------------------" << endl;

        if (veryVerbose) cout << "\tConfirm signatures." << endl;
        {
            using namespace balcl;
            typedef ostream& (Obj::*funcPtr)(ostream&, int, int) const;
            typedef ostream& (*operatorPtr)(ostream&, const Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     printMember = &Obj::print;
            operatorPtr operatorOp  = operator<<;

            (void)printMember;  // quash potential compiler warnings
            (void)operatorOp;
        }

        if (veryVerbose) cout << "\tCheck output" << endl;

        for (int i = 0; i < NUM_OPTION_TYPEINFO; ++i) {
            const int       LINE       = OPTION_TYPEINFO[i].d_line;
            const ElemType  TYPE       = OPTION_TYPEINFO[i].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[i].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[i].d_constraint_p;

            Obj mX;  const Obj& X = mX;

            u::setTypeInfo(&mX, TYPE, VARIABLE, CONSTRAINT);

            bsl::ostringstream ossMethod;
            bsl::ostringstream ossOperator;

            ASSERTV(LINE, &ossMethod   == &X.print(ossMethod));  // ACTION
            ASSERTV(LINE, &ossOperator == &(ossOperator << X));  // ACTION

            bsl::string stringMethod  (ossMethod  .str());
            bsl::string stringOperator(ossOperator.str());
                // Note: Convert to strings because 'oss[12].str()' are
                // temporaries andthose do not work well with
                // 'oss[12].str().c_str()'.

            ASSERT(0 * 4 == bsl::strspn(stringMethod  .c_str(), " "));
            ASSERT(0 * 4 == bsl::strspn(stringOperator.c_str(), " "));

            ASSERTV(LINE, stringMethod,   stringOperator,
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

            ASSERTV(stringMethod == stringIndentNormalized1);
            ASSERTV(stringMethod == stringIndentNormalized2);

            const char *output = stringMethod.c_str();
            const int   ret    = u::parseTypeInfo(&output, X, output);
            LOOP2_ASSERT(LINE, ret, 0 == ret);
        }

        if (veryVerbose) cout << "\tNegative testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            Obj mX; const Obj& X = mX;

            bsl::ostringstream oss;

            ASSERT_PASS(X.print(oss, 0,  1));
            ASSERT_PASS(X.print(oss, 0,  0));
            ASSERT_FAIL(X.print(oss, 0, -1));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // 'balcl::TypeInfoUtil': 'satisfiesConstraint'
        //
        // Concerns:
        //: 1 Each of the 'satisfiesConstraint' overloads returns the
        //:   expected result for the given value and constraint.
        //:
        //: 2 'const'-qualification:
        //:   1 The first two parameters are 'const' qualified.
        //:   2 The third parameter (stream), if present, is not 'const'
        //:     qualified.
        //
        // Plan:
        //: 1 Use 'OPTION_TYPEINFO', a table of representative inputs,
        //:   that includes an entry for each supported type having a
        //:   constraint.
        //:
        //:   o Note that that constraints defined in the table can be set via
        //:     public static variable to unconditionally return 'true' or
        //:     'false'.
        //
        //: 2 Also use  'OPTION_DEFAULT_VALUES', a table of values
        //:   for each supported option type.  Note that none of those values
        //:   is the default value for the type.
        //:
        //: 3 For each entry in 'OPTION_TYPEINFO' that defines a constraint,
        //:   create an object, create a value of the appropriate type
        //:   from 'OPTION_DEFAULT_VALUES', call 'satisfiesConstraint',
        //:   and confirm that the result matches that expected per the
        //:   current value of the constraint's static variable.  Toggle
        //:   the static variable and retest.
        //:
        //: 4 The first two arguments are always passed by 'const' reference.
        //:   The 'stream' argument is shown to be non-'const' qualified
        //:   by having the test constraint write an error message when
        //:   it returns 'false'.
        //
        // Testing:
        //   bool satisfiesConstraint(const Clov& e,             TypeInfo tf);
        //   bool satisfiesConstraint(const Clov& e, ostream& s, TypeInfo tf);
        //   bool satisfiesConstraint(const void *v,             TypeInfo tf);
        //   bool satisfiesConstraint(const void *v, ostream& s, TypeInfo tf);
        // --------------------------------------------------------------------

        if (verbose) cout
                     << endl
                     << "'balcl::TypeInfoUtil': 'satisfiesConstraint'" << endl
                     << "--------------------------------------------" << endl;

        for (int i = 0; i < NUM_OPTION_TYPEINFO; ++i) {
            const int       LINE       = OPTION_TYPEINFO[i].d_line;
            const ElemType  TYPE       = OPTION_TYPEINFO[i].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[i].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[i].d_constraint_p;

            if (veryVerbose) {
                T_ P_(LINE) P_(i) P(TYPE)
                T_ P_(VARIABLE) P(CONSTRAINT)
            }

            if (CONSTRAINT) {
                Obj mX;  const Obj& X = mX;

                u::setTypeInfo(&mX, TYPE, VARIABLE, CONSTRAINT);

                const int   j = i % NUM_OPTION_DEFAULT_VALUES;
                const void *VALUE = OPTION_DEFAULT_VALUES[j].d_value_p;

                ASSERT(TYPE == OPTION_DEFAULT_VALUES[j].d_type);

                OptionValue ELEMENT(TYPE);
                u::setOptionValue(&ELEMENT, VALUE, TYPE);

                bsl::ostringstream ossElement, ossValue;

          //v---^
            OptConstraint::s_constraintValue = true;
            ASSERT( ObjUtil::satisfiesConstraint(ELEMENT, X            ));
            ASSERT( ObjUtil::satisfiesConstraint(ELEMENT, X, ossElement));
            ASSERT( ObjUtil::satisfiesConstraint(VALUE,   X            ));
            ASSERT( ObjUtil::satisfiesConstraint(VALUE,   X, ossValue  ));

            ASSERTV(LINE, ossElement.str(),  ossElement.str().empty());
            ASSERTV(LINE, ossValue  .str(),  ossValue  .str().empty());

            ossElement.str(""); ossElement.clear();
            ossValue  .str(""); ossValue  .clear();

            OptConstraint::s_constraintValue = false;
            ASSERT(!ObjUtil::satisfiesConstraint(ELEMENT, X            ));
            ASSERT(!ObjUtil::satisfiesConstraint(ELEMENT, X, ossElement));
            ASSERT(!ObjUtil::satisfiesConstraint(VALUE,   X            ));
            ASSERT(!ObjUtil::satisfiesConstraint(VALUE,   X, ossValue  ));

            ASSERTV(LINE, ossElement.str(), !ossElement.str().empty());
            ASSERTV(LINE, ossValue  .str(), !ossValue  .str().empty());

            ossElement.str(""); ossElement.clear();
            ossValue  .str(""); ossValue  .clear();

            OptConstraint::s_constraintValue = true;
            ASSERT( ObjUtil::satisfiesConstraint(ELEMENT, X            ));
            ASSERT( ObjUtil::satisfiesConstraint(ELEMENT, X, ossElement));
            ASSERT( ObjUtil::satisfiesConstraint(VALUE,   X            ));
            ASSERT( ObjUtil::satisfiesConstraint(VALUE,   X, ossValue  ));

            ASSERTV(LINE, ossElement.str(),  ossElement.str().empty());
            ASSERTV(LINE,   ossValue.str(),    ossValue.str().empty());
          //^---v
            } else {
                if (veryVerbose) {
                    cout << "\tSkip: Has no constraint" << endl;
                }
            }

            // TBD: Negative testing
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR, DTOR, PRIMARY MANIPULATORS, BASIC ACCESSORS
        //
        // Concerns:
        //: 1 The default constructed object has the expected type, linked
        //:   variable (none), and contract.
        //:
        //:   1 Note that the initial constraint is the address of a non-public
        //:     type that should accept all possible values of the object's
        //:     type; however, we have no way to check that directly.  Instead,
        //:     we merely note the address of that object and confirm that the
        //:     primary manipulators that should change that address actually
        //:     do so.
        //:
        //: 2 The object allocates from the intended allocator, and no other.
        //:
        //:   1 Allocation is exception safe.
        //:
        //:   2 The 'bslma::UsesBslmaAllocator' trait is set for this class.
        //:
        //: 3 The basic accessors provide a view of the object state that is
        //:   consistent with the state of the objects set by the constructor
        //:   and the primary manipulators.
        //:
        //: 4 The basic accessors are 'const'-qualified.
        //
        // Plan:
        //: 1 Create, 'OPTION_TYPEINFO', a table representing the space of
        //:   object attributes consisting of every supported option type,
        //:   having a linked variable, having a contract, having both a linked
        //:   variable and contract, and having neither.
        //:
        //: 2 Use each default constructor (one specifying an object allocator
        //:   and the other using the default allocator) to create an object.
        //:   Using the basic accessors, confirm that the object is in its
        //:   expected state.
        //:
        //:   1 Confirm that
        //:     'bslma::UsesBslmaAllocator<balcl::TypeInfo>::value' is 'true'
        //:     in a compile-time assertion.
        //:
        //:   2 Always invoke the basic accessors on a 'const'-reference to the
        //:     object under test.  If the accessors are not 'const'-qualified,
        //:     there is a compile failure.
        //:
        //: 3 Use 'u::setLinkedVariable' and 'u::setConstraint' to set each of
        //:   those attributes, if available for the table entry.  Confirm that
        //:   the accessors show the changed state.
        //:
        //: 4 Use methods 'resetLinkedVariableAndConstraint' and
        //:   'resetConstraint' to undo the change in P-3.  Confirm using the
        //:   basic accessors.
        //:
        //: 5 Use 'BSLMA_TESTALLOCATOR_EXCEPTION*' macros to check allocations
        //:   in the presence of exceptions.
        //
        // Testing:
        //   TypeInfo();
        //   TypeInfo(bslma::Allocator *basicAllocator);
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
        //   OptionType::Enum type() const;
        //   bslma::Allocator *allocator() const;
        // --------------------------------------------------------------------

        if (verbose) cout
        << endl
        << "DEFAULT CTOR, DTOR, PRIMARY MANIPULATORS, BASIC ACCESSORS" << endl
        << "---------------------------------------------------------" << endl;

        bslma::TestAllocator         sa("supplied", veryVeryVeryVerbose);
        bslma::TestAllocator         da("default",  veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout << "\t\tWithout passing in an allocator." << endl;

        for (int i = 0; i < NUM_OPTION_TYPEINFO; ++i) {
            const int       LINE       = OPTION_TYPEINFO[i].d_line;
            const ElemType  TYPE       = OPTION_TYPEINFO[i].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[i].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[i].d_constraint_p;

            Obj mX;  const Obj& X = mX;      // ACTION

            if (veryVerbose) {
                T_ T_ T_ P_(LINE) P_(i) P(TYPE)
                T_ T_ T_ P_(VARIABLE) P(CONSTRAINT)
                T_ T_ T_ P_(LINE) P_(i) P(X)
            }
            LOOP_ASSERT(LINE, Ot::e_STRING == X.type());
            LOOP_ASSERT(LINE, 0            == X.linkedVariable());
            LOOP_ASSERT(LINE, &da          == X.allocator());

            u::setType(&mX, TYPE);

            LOOP_ASSERT(LINE, TYPE == X.type());
            LOOP_ASSERT(LINE, 0    == X.linkedVariable());

            bsl::shared_ptr<ObjConstraint> DEFAULT_CONSTRAINT = X.constraint();

            if (VARIABLE) {
                u::setLinkedVariable(&mX, TYPE, VARIABLE);  // ACTION

                if (veryVerbose) {
                    T_ T_ T_ P_(LINE) P_(i) P(X)
                }
                LOOP_ASSERT(LINE, TYPE               == X.type());
                LOOP_ASSERT(LINE, VARIABLE           == X.linkedVariable());
                LOOP_ASSERT(LINE, DEFAULT_CONSTRAINT == X.constraint());

                mX.resetLinkedVariableAndConstraint();     // ACTION

                LOOP_ASSERT(LINE, TYPE               == X.type());
                LOOP_ASSERT(LINE, 0                  == X.linkedVariable());
                LOOP_ASSERT(LINE, DEFAULT_CONSTRAINT == X.constraint());
            }

            mX.resetLinkedVariableAndConstraint();

            LOOP_ASSERT(LINE, TYPE               == X.type());
            LOOP_ASSERT(LINE, 0                  == X.linkedVariable());
            LOOP_ASSERT(LINE, DEFAULT_CONSTRAINT == X.constraint());

            if (CONSTRAINT) {
                bslma::TestAllocatorMonitor dam(&da);

                u::setConstraint(&mX, TYPE, CONSTRAINT);

                LOOP_ASSERT(LINE, dam.isTotalUp());

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
                u::setLinkedVariable(&mX, TYPE, VARIABLE);
                u::setConstraint    (&mX, TYPE, CONSTRAINT);

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

        if (verbose) cout << "\t\tPassing in an allocator; no exceptions"
                          << endl;

        for (int i = 0; i < NUM_OPTION_TYPEINFO; ++i) {
            const int       LINE       = OPTION_TYPEINFO[i].d_line;
            const ElemType  TYPE       = OPTION_TYPEINFO[i].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[i].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[i].d_constraint_p;

            Obj mX(&sa);  const Obj& X = mX;

            LOOP_ASSERT(LINE, Ot::e_STRING == X.type());
            LOOP_ASSERT(LINE, 0            == X.linkedVariable());
            LOOP_ASSERT(LINE, &sa          == X.allocator());

            bsl::shared_ptr<ObjConstraint> DEFAULT_CONSTRAINT = X.constraint();

            bslma::TestAllocatorMonitor sam(&sa);

            u::setTypeInfo(&mX, TYPE, VARIABLE, CONSTRAINT);  // ACTION

            if (CONSTRAINT) {
                LOOP_ASSERT(LINE, sam.isTotalUp());
            } else {
                LOOP_ASSERT(LINE, sam.isTotalSame());
            }

            LOOP_ASSERT(LINE, TYPE               == X.type());
            LOOP_ASSERT(LINE, VARIABLE           == X.linkedVariable());

            // Note that the test for 'satisfiesConstraint' demonstrates that
            // 'setConstraint' does install the intended constraint.
#if 0
            LOOP_ASSERT(LINE, DEFAULT_CONSTRAINT != X.constraint());
            if (CONSTRAINT) {
                LOOP_ASSERT(LINE, DEFAULT_CONSTRAINT != X.constraint());
            }
            else {
                LOOP_ASSERT(LINE, DEFAULT_CONSTRAINT == X.constraint());
            }
#endif // 0
        }

        if (verbose) cout << "\t\t\tWith exceptions." << endl;

        for (int i = 0; i < NUM_OPTION_TYPEINFO; ++i) {
            const int       LINE       = OPTION_TYPEINFO[i].d_line;
            const ElemType  TYPE       = OPTION_TYPEINFO[i].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[i].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[i].d_constraint_p;

            Obj mY(&sa);  const Obj& Y = mY; u::setType(&mY, TYPE);

            bsl::shared_ptr<ObjConstraint> DEFAULT_CONSTRAINT = Y.constraint();
            bslma::TestAllocatorMonitor sam(&sa);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                Obj mX(&sa);  const Obj& X = mX;

                u::setTypeInfo(&mX, TYPE, VARIABLE, CONSTRAINT);  // ACTION

                LOOP_ASSERT(LINE, TYPE     == X.type());
                LOOP_ASSERT(LINE, VARIABLE == X.linkedVariable());

                if (CONSTRAINT) {
                    LOOP_ASSERT(LINE, DEFAULT_CONSTRAINT != X.constraint());
                } else {
                    LOOP_ASSERT(LINE, DEFAULT_CONSTRAINT == X.constraint());
                }

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            LOOP_ASSERT(LINE, sam.isInUseSame());
        }

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

        Obj i(OptionType::k_INT);
        Obj b(OptionType::k_BOOL);
        Obj s(OptionType::k_STRING_ARRAY);

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
