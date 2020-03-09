// balcl_typeinfo.t.cpp                                               -*-C++-*-
#include <balcl_typeinfo.h>

#include <balcl_constraint.h>
#include <balcl_optiontype.h>
#include <balcl_optionvalue.h>

#include <bdlb_chartype.h>
#include <bdlb_printmethods.h>
#include <bdlb_tokenizer.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_time.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_objectbuffer.h>
#include <bsls_types.h>     // 'bsls::Types::Int64'

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>    // 'bsl::strspn'
#include <bsl_functional.h> // 'bsl::function'
#include <bsl_iostream.h>
#include <bsl_ostream.h>    // 'operator<<'
#include <bsl_sstream.h>
#include <bsl_stdexcept.h>
#include <bsl_string.h>     // 'bslstl::StringRef'
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// 'balcl::TypeInfo' is a (value-semantic) attribute class.  The three salient
// attributes are:
//..
//  Type                     Description
//  ------------------------ --------------------
//  balcl::OptionType::Enum  option types
//  void *                   linked variable
//  bsl::share_ptr<TypeInfo> constraint (functor)
//..
// As some of these are (process-dependent) addresses, this is categorized as
// an in-core VST.
//
// The 'constraint' attribute, managed via 'bsl::share_ptr', lends this class
// some interesting behaviors that are not typically seen in attribute classes.
//
//: o Two objects created from the same constituent attributes do not compare
//:   equal when a non-default constraint is specified.
//:
//: o Although the constructor allocates when there is a constraint, the copy
//:   constructor and copy assignment never allocate.  Instead of creating a
//:   new functor, the usage count on the shared pointer in the original (rhs)
//:   object is merely incremented.
//
// This component also defines two other classes, 'balcl::TypeInfoConstraint',
// a functor that is not visible outside of this component except as an opaque
// type, and 'balcl::TypeInfoUtil', a utility 'struct' that must reside in this
// component because of its dependency on 'balcl::TypeInfoConstraint'.
//
// Testing follows the general pattern of a VST interspersed with tests of the
// utility 'struct' and 'balcl::TypeInfoConstraint'.  Note that the latter
// class cannot be directly tested because it has no methods visible to
// clients.
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
// [ 9] TypeInfo(bool             *v, *bA = 0);
// [ 9] TypeInfo(char             *v, *bA = 0);
// [ 9] TypeInfo(char             *v, CharC&     c, *bA = 0);
// [ 9] TypeInfo(int              *v, *bA = 0);
// [ 9] TypeInfo(int              *v, IntC&      c, *bA = 0);
// [ 9] TypeInfo(Int64            *v, *bA = 0);
// [ 9] TypeInfo(Int64            *v, Int64C&    c, *bA = 0);
// [ 9] TypeInfo(double           *v, *bA = 0);
// [ 9] TypeInfo(double           *v, DoubleC&   c, *bA = 0);
// [ 9] TypeInfo(string           *v, *bA = 0);
// [ 9] TypeInfo(string           *v, StringC&   c, *bA = 0);
// [ 9] TypeInfo(Datetime         *v, *bA = 0);
// [ 9] TypeInfo(Datetime         *v, DatetimeC& c, *bA = 0);
// [ 9] TypeInfo(Date             *v, *bA = 0);
// [ 9] TypeInfo(Date             *v, DateC&     c, *bA = 0);
// [ 9] TypeInfo(Time             *v, *bA = 0);
// [ 9] TypeInfo(Time             *v, TimeC&     c, *bA = 0);
// [ 9] TypeInfo(vector<char>     *v, *bA = 0);
// [ 9] TypeInfo(vector<char>     *v, CharC&     c, *bA = 0);
// [ 9] TypeInfo(vector<int>      *v, *bA = 0);
// [ 9] TypeInfo(vector<int>      *v, IntC&      c, *bA = 0);
// [ 9] TypeInfo(vector<Int64>    *v, *bA = 0);
// [ 9] TypeInfo(vector<Int64>    *v, Int64C&    c, *bA = 0);
// [ 9] TypeInfo(vector<double>   *v, *bA = 0);
// [ 9] TypeInfo(vector<double>   *v, DoubleC&   c, *bA = 0);
// [ 9] TypeInfo(vector<string>   *v, *bA = 0);
// [ 9] TypeInfo(vector<string>   *v, StringC&   c, *bA = 0);
// [ 9] TypeInfo(vector<Datetime> *v, *bA = 0);
// [ 9] TypeInfo(vector<Datetime> *v, DatetimeC& c, *bA = 0);
// [ 9] TypeInfo(vector<Date>     *v, *bA = 0);
// [ 9] TypeInfo(vector<Date>     *v, DateC&     c, *bA = 0);
// [ 9] TypeInfo(vector<Time>     *v, *bA = 0);
// [ 9] TypeInfo(vector<Time>     *v, TimeC&     c, *bA = 0);
// [ 7] TypeInfo(const TypeInfo& original, *bA = 0);
// [ 2] ~TypeInfo();
//
// MANIPULATORS
// [ 8] TypeInfo& operator=(const TypeInfo& rhs);
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
// [ 5] ostream& print(ostream& stream, int level = 0, int spl = 4) const;
//
// FREE OPERATORS
// [ 6] bool operator==(const TypeInfo& lhs, rhs);
// [ 6] bool operator!=(const TypeInfo& lhs, rhs);
// [ 5] operator<<(ostream& stream, const TypeInfo& rhs);
// ----------------------------------------------------------------------------
// balcl::TypeInfoUtil
// ----------------------------------------------------------------------------
// [10] parseAndValidate(OV *e, string& i, TypeInfo& tf, ostream& s);
// [ 4] bool satisfiesConstraint(const Clov& e,             TypeInfo tf);
// [ 4] bool satisfiesConstraint(const Clov& e, ostream& s, TypeInfo tf);
// [ 4] bool satisfiesConstraint(const void *v,             TypeInfo tf);
// [ 4] bool satisfiesConstraint(const void *v, ostream& s, TypeInfo tf);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] CONCERN: HELPER 'u::shiftType'

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
typedef balcl::OptionValue        OptionValue;

typedef balcl::OptionType         Ot;
typedef balcl::OptionType::Enum   ElemType;

typedef bsls::Types::Int64        Int64;

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
    // This array has an entry for every supported option type with a linked
    // variable and not, and having a constraint and not.

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
  { L_, Ot::e_CHAR,           &parsedChar,          "a"                   }
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
   { L_,  Ot::e_CHAR,           &testCharConstraint     }
 , { L_,  Ot::e_INT,            &testIntConstraint      }
 , { L_,  Ot::e_INT64,          &testInt64Constraint    }
 , { L_,  Ot::e_DOUBLE,         &testDoubleConstraint   }
 , { L_,  Ot::e_STRING,         &testStringConstraint   }
 , { L_,  Ot::e_DATETIME,       &testDatetimeConstraint }
 , { L_,  Ot::e_DATE,           &testDateConstraint     }
 , { L_,  Ot::e_TIME,           &testTimeConstraint     }
 , { L_,  Ot::e_CHAR_ARRAY,     &testCharConstraint     }
 , { L_,  Ot::e_INT_ARRAY,      &testIntConstraint      }
 , { L_,  Ot::e_INT64_ARRAY,    &testInt64Constraint    }
 , { L_,  Ot::e_DOUBLE_ARRAY,   &testDoubleConstraint   }
 , { L_,  Ot::e_STRING_ARRAY,   &testStringConstraint   }
 , { L_,  Ot::e_DATETIME_ARRAY, &testDatetimeConstraint }
 , { L_,  Ot::e_DATE_ARRAY,     &testDateConstraint     }
 , { L_,  Ot::e_TIME_ARRAY,     &testTimeConstraint     }
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

                        // ==================
                        // function shiftType
                        // ==================

Ot::Enum shiftType(Ot::Enum type, int offset)
    // Return the enumerated value that is the specified 'offset' advanced from
    // the specified 'type' in the sequence '[Ot::e_BOOL .. Ot::e_TIME_ARRAY]'.
    // Offsets that go past the end of the sequence wrap around to
    // 'Ot::e_BOOL'.  The behavior is undefined if 'Ot::e_VOID == type'.  Note
    // that 'offset' can be negative.  Also note that 'Ot::e_VOID' is *not*
    // part of the sequence.
{
    ///Implementation Note
    ///-------------------
    // Preconditions are checked using 'ASSERT' instead of 'BSLS_ASSERT*' to
    // guarantee that there are no misleading exceptions thrown when this
    // function is used in negative tests.

    ASSERT(Ot::e_VOID != type || 0 == offset);

    int typeAsInt  = static_cast<int>(type);
    int numOptions = static_cast<int>(Ot::e_TIME_ARRAY) + 1;

    ASSERT(18 == numOptions);

    // Map range from [1 .. 17] to [0 .. 16].
    --typeAsInt;
    --numOptions;
    offset %= numOptions;

      // Apply offset.
    if (0 < offset) {
        typeAsInt += offset;
    } else {
        typeAsInt += (numOptions + offset);
    }

    typeAsInt %= numOptions;   // Map to   [0 .. 16].
    typeAsInt += 1;            // Map from [0 .. 16] to [1 .. 17].

    return static_cast<Ot::Enum>(typeAsInt);
}

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

int skipWhiteSpace(const char **endPos, const char  *inputString)
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

                        // ======================
                        // function setConstraint
                        // ======================

void setConstraint(Obj *typeInfo, ElemType type, const void *address)
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

void setLinkedVariable(Obj *typeInfo, ElemType type, void *address)
    // Set the linked variable of the specified 'typeInfo' to the variable at
    // the specified 'address'.  The behavior is undefined unless 'address' can
    // be cast to a pointer to the type associated with the specified 'type'
    // (i.e., 'Ot::EnumToType<ENUM>::type *' where 'ENUM' matches any of the
    // enumerators of 'Ot::OptionType' expect 'Ot::e_VOID').
{
    ASSERT(typeInfo);

#define CASE(ENUM)                                                            \
    case ENUM: {                                                              \
      typeInfo->setLinkedVariable(static_cast<Ot::EnumToType<ENUM>::type *>(  \
                                                                   address)); \
    } break;                                                                  \

    switch (type) {
      case Ot::e_VOID: {
        BSLS_ASSERT(!"Reached: 'e_VOID'");
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
        BSLS_ASSERT(!"Reached: Unknown");
      } break;
    };

#undef CASE

}


                        // ================
                        // function setType
                        // ================

void setType(Obj *typeInfo, ElemType type)
    // Set the 'Ot::OptionType' element of the specified 'typeInfo' to the
    // specified 'type'.  The behavior is undefined unless
    // 'Ot::e_VOID != type'.  Note that this resets both the linked variable
    // and constraint of 'typeInfo'.
{
    ASSERT(typeInfo);

#define CASE(ROOT)                                                            \
    case Ot::e_##ROOT: {                                                      \
      typeInfo->setLinkedVariable(Ot::k_##ROOT);                              \
    } break;                                                                  \

    switch (type) {
      case Ot::e_VOID: {
        BSLS_ASSERT(!"Reachable");
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
        BSLS_ASSERT(!"Reached: Unknown");
      } break;
    };

#undef CASE

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
    // the address of a constraint.  The behavior is undefined unless
    // 'Ot::e_VOID != type', 'variable' is 0 or can be cast to
    // 'Ot::EnumToType<type>::type', 'constraint' is 0 or can be cast to the
    // type defined by 'Constraint' for 'type', and if 'Ot::e_BOOL == type'
    // then 'constraint' is 0.
{
    BSLS_ASSERT(typeInfo);

    if (Ot::e_BOOL == type) {
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
    // 'buffer' and having the specified 'type'.  If the specified 'variable'
    // is not 0, that address is linked to the option.  There is no constraint
    // on the value.  The returned object will use the currently defined
    // default allocator.  The caller is required to explicitly invoke the
    // destructor of the returned object.  The behavior is undefined unless
    // 'Ot::e_VOID != type' and 'variable' can be cast to
    // 'Ot::EnumToType<type>::type'.
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(Ot::e_VOID != type);

    Obj *ptr = 0;

#define CASE(ENUM)                                                            \
    case ENUM: {                                                              \
      ptr = new (buffer) Obj(static_cast<Ot::EnumToType<ENUM>::type *>(       \
                                                                  variable)); \
    } break;                                                                  \

    switch (type) {
      case Ot::e_VOID: {
        BSLS_ASSERT(!"Reached: 'e_VOID'");
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
        BSLS_ASSERT(!"Reached: Unknown");
      } break;
    }

#undef CASE

    return ptr;
}

Obj *constructTypeInfo(void             *buffer,
                       ElemType          type,
                       void             *variable,
                       bslma::Allocator *basicAllocator)
    // Return the address of a 'TypeInfo' object created in the specified
    // 'buffer' and having the specified 'type'.  If the specified 'variable'
    // is not 0, that address is linked to the option.  There is no constraint
    // on the value.  The returned object will use the currently defined
    // default allocator.  The caller is required to explicitly invoke the
    // destructor of the returned object.  The behavior is undefined unless
    // 'Ot::e_VOID != type' and 'variable' can be cast to
    // 'Ot::EnumToType<type>::type'.
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(Ot::e_VOID != type);

    Obj *ptr = 0;

#define CASE(ENUM)                                                            \
    case ENUM: {                                                              \
      ptr = new (buffer) Obj(static_cast<Ot::EnumToType<ENUM>::type *>(       \
                                                                   variable), \
                             basicAllocator);                                 \
    } break;                                                                  \

    switch (type) {
      case Ot::e_VOID: {
        BSLS_ASSERT(!"Reached");
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
        BSLS_ASSERT(!"Reached: Unknown");
      } break;
    }

#undef CASE

    return ptr;
}

Obj *constructTypeInfo(void     *buffer,
                       ElemType  type,
                       void     *variable,
                       void     *constraint)
    // Return the address of a 'TypeInfo' object created in the specified
    // 'buffer' and having the specified 'type' and 'constraint'.  If the
    // specified 'variable' is not 0, that address is linked to the option.
    // The returned object uses the currently defined default allocator.  The
    // caller is required to explicitly invoke the destructor of the returned
    // object.  The behavior is undefined unless 'Ot::e_VOID != type',
    // 'Ot::e_BOOL != type', 'variable' can be cast to
    // 'Ot::EnumToType<type>::type', and 'constraint' can be cast to the type
    // defined by 'Constraint' for 'type'.
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(Ot::e_VOID != type);
    BSLS_ASSERT(Ot::e_BOOL != type);
    BSLS_ASSERT(constraint);

    Obj *ptr = 0;

#define CASE(ENUM, CONSTRAINT)                                                \
    case ENUM: {                                                              \
      ptr = new (buffer) Obj(                                                 \
                    static_cast<Ot::EnumToType<ENUM>::type   *>(variable  ),  \
                   *static_cast<const Constraint::CONSTRAINT *>(constraint)); \
    } break;                                                                  \

    switch (type) {
      case Ot::e_VOID: {
        BSLS_ASSERT(!"Reached: 'e_VOID'");
      } break;
      case Ot::e_BOOL: {
        BSLS_ASSERT(!"Reached: 'e_BOOL'");
      } break;

      CASE(Ot::e_CHAR,               CharConstraint)
      CASE(Ot::e_INT,                 IntConstraint)
      CASE(Ot::e_INT64,             Int64Constraint)
      CASE(Ot::e_DOUBLE,           DoubleConstraint)
      CASE(Ot::e_STRING,           StringConstraint)
      CASE(Ot::e_DATETIME,       DatetimeConstraint)
      CASE(Ot::e_DATE,               DateConstraint)
      CASE(Ot::e_TIME,               TimeConstraint)

      CASE(Ot::e_CHAR_ARRAY,         CharConstraint)
      CASE(Ot::e_INT_ARRAY,           IntConstraint)
      CASE(Ot::e_INT64_ARRAY,       Int64Constraint)
      CASE(Ot::e_DOUBLE_ARRAY,     DoubleConstraint)
      CASE(Ot::e_STRING_ARRAY,     StringConstraint)
      CASE(Ot::e_DATETIME_ARRAY, DatetimeConstraint)
      CASE(Ot::e_DATE_ARRAY,         DateConstraint)
      CASE(Ot::e_TIME_ARRAY,         TimeConstraint)

      default: {
        BSLS_ASSERT(!"Reached: Unknown");
      } break;
    }

#undef CASE

    return ptr;
}

Obj *constructTypeInfo(void             *buffer,
                       ElemType          type,
                       void             *variable,
                       void             *constraint,
                       bslma::Allocator *basicAllocator)
    // Return the address of a 'TypeInfo' object created in the specified
    // 'buffer' and having the specified 'type' and 'constraint'.  If the
    // specified 'variable' is not 0, that address is linked to the option.
    // The returned object uses the currently defined default allocator.  The
    // caller is required to explicitly invoke the destructor of the returned
    // object.  The behavior is undefined unless 'Ot::e_VOID != type',
    // 'Ot::e_BOOL != type', 'variable' can be cast to
    // 'Ot::EnumToType<type>::type', and 'constraint' can be cast to the type
    // defined by 'Constraint' for 'type'.
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(Ot::e_VOID != type);
    BSLS_ASSERT(Ot::e_BOOL != type);
    BSLS_ASSERT(constraint);

    Obj *ptr = 0;

#define CASE(ENUM, CONSTRAINT)                                                \
    case ENUM: {                                                              \
      ptr = new (buffer) Obj(                                                 \
                    static_cast<Ot::EnumToType<ENUM>::type   *>(variable  ),  \
                   *static_cast<const Constraint::CONSTRAINT *>(constraint),  \
                   basicAllocator);                                           \
    } break;                                                                  \

    switch (type) {
      case Ot::e_VOID: {
        BSLS_ASSERT(!"Reached: 'e_VOID'");
      } break;
      case Ot::e_BOOL: {
        BSLS_ASSERT(!"Reached: 'e_BOOL'");
      } break;

      CASE(Ot::e_CHAR,               CharConstraint)
      CASE(Ot::e_INT,                 IntConstraint)
      CASE(Ot::e_INT64,             Int64Constraint)
      CASE(Ot::e_DOUBLE,           DoubleConstraint)
      CASE(Ot::e_STRING,           StringConstraint)
      CASE(Ot::e_DATETIME,       DatetimeConstraint)
      CASE(Ot::e_DATE,               DateConstraint)
      CASE(Ot::e_TIME,               TimeConstraint)

      CASE(Ot::e_CHAR_ARRAY,         CharConstraint)
      CASE(Ot::e_INT_ARRAY,           IntConstraint)
      CASE(Ot::e_INT64_ARRAY,       Int64Constraint)
      CASE(Ot::e_DOUBLE_ARRAY,     DoubleConstraint)
      CASE(Ot::e_STRING_ARRAY,     StringConstraint)
      CASE(Ot::e_DATETIME_ARRAY, DatetimeConstraint)
      CASE(Ot::e_DATE_ARRAY,         DateConstraint)
      CASE(Ot::e_TIME_ARRAY,         TimeConstraint)

      default: {
        BSLS_ASSERT(!"Reached: Unknown");
      } break;
    }

#undef CASE

    return ptr;
}
                        // ======================
                        // function parseTypeInfo
                        // ======================

int parseTypeInfo(const char **endpos,
                  const Obj&   typeInfo,
                  const char  *input)
    // Parse the specified 'input' for a value and verify that this value
    // matches the specified 'typeInfo' (at least in 'Ot::OptionType' type) and
    // return 0 if parsing and verification succeed.  Return a non-zero value
    // if parsing fails or if the value parsed does not match 'typeInfo', and
    // return in the specified 'endpos' the first unsuccessful parsing
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

                        // =======================
                        // function setOptionValue
                        // =======================

void setOptionValue(OptionValue *dst, const void *src, ElemType type)
    // Set the value at the specified 'dst' to the value found at the specified
    // 'src' that is of the specified 'type'.  The behavior is undefined unless
    // 'src' can be cast to a pointer of 'Ot::EnumToType<type>::type',
    // 'Ot::e_VOID != type', and 'dst->type() == type'.
{
    BSLS_ASSERT(dst);
    BSLS_ASSERT(src);
    BSLS_ASSERT(Ot::e_VOID != type);
    BSLS_ASSERT(dst->type()        == type);

    switch (type) {
      case Ot::e_VOID: {
        BSLS_ASSERT(!"Reachable");
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
    // called with the specified 'level' and the specified 'spacesPerLevel'.
    // The "normalized" version corresponds to calling 'print' with 'level' and
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

bool areEqualValues(const OptionValue& element, const void *value)
    // Return 'true' if the value of the specified 'element' is the same as
    // that found at the specified 'value', and 'false' otherwise.  The
    // behavior is undefined unless 'Ot::e_VOID != element.type()',
    // 'Ot::e_BOOL != element.type()', and 'value' can be (validly) cast to
    // 'Ot::EnumToType<ENUM>::type *' where 'ENUM' matches 'element.type()'.
{
    BSLS_ASSERT(value);

#define CASE(ENUM)                                                            \
    case ENUM: {                                                              \
      return      element.the<      Ot::EnumToType<ENUM>::type  >()           \
             == *(static_cast<const Ot::EnumToType<ENUM>::type *>(value));    \
    } break;

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
        BSLS_ASSERT(!"Reached: Unknown");
      } break;
    }
#undef CASE

    return false;
}

void printValue(ostream& stream, Ot::Enum type, const void *value)
    // Print to the specified 'stream' the value at the specified 'value'.  The
    // behavior is undefined unless 'value' can be (validly) cast to
    // 'Ot::EnumToType<ENUM>::type *' where 'ENUM' matches the specified
    // 'type'.  Each of the array types are assumed to contain a single
    // element.
{
    BSLS_ASSERT(value);

#define CASE_SCALAR(ENUM)                                                     \
    case ENUM: {                                                              \
          stream << *(static_cast<const Ot::EnumToType<ENUM>::type *>(value)) \
                 << endl;                                                     \
    } break;


#define CASE_ARRAY(ENUM)                                                      \
    case ENUM: {                                                              \
          stream                                                              \
            << (*(static_cast<const Ot::EnumToType<ENUM>::type *>(value)))[0] \
                                                                              \
            << endl;                                                          \
    } break;

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
        BSLS_ASSERT(!"Reached: Unknown");
      } break;
#undef CASE_SCALAR
#undef CASE_ARRAY
    }
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

    switch (test) { case 0:  // Zero is always the leading case.
      case 10: {
        // -------------------------------------------------------------------
        // 'balcl::TypeInfoUtil': 'parseAndValidate'
        //
        // Concerns:
        //: 1 The signature is implemented as expected.
        //:   1 The return value is 'bool'.
        //:   2 The second and third parameters are 'const'-qualified.
        //:   3 The first and fourth parameters are not 'const'-qualified.
        //:
        //: 2 The function returns the expected value for the given object and
        //:   constraint for all supported types.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the "pointer-to-function" idiom for a compile-time check of
        //:   the signature.  (C-1)
        //:
        //: 2 Using a table-drive approach, we iterate through the
        //:   'PARSABLE_VALUES' table to find valid input strings for each
        //:   supported option type.  We employ a test constraint object that
        //:   can be set to pass or fail via a globally visible static data
        //:   member.  We confirm that the function returns the result
        //:   according to the current value of that static data member.  We
        //:   also confirm that the output string, when provided, is set when
        //:   there is a failure and is left empty otherwise.  (C-2)
        //:
        //: 3 Use 'BSLS_ASSERTTEST_*' facilities for negative testing.  (C-3)
        //
        // Testing:
        //   parseAndValidate(OV *e, string& i, TypeInfo& tf, ostream& s);
        // -------------------------------------------------------------------

        if (verbose) cout
                        << endl
                        << "'balcl::TypeInfoUtil': 'parseAndValidate'" << endl
                        << "-----------------------------------------" << endl;

        if (verbose) cout << "Verify the signature and return type ." << endl;
        {
            typedef bool (*functionPtr)(OptionValue        *,
                                        const bsl::string&  ,
                                        const Obj&          ,
                                        bsl::ostream&       );

            functionPtr classMethod = &ObjUtil::parseAndValidate;

            (void)classMethod;  // quash potential compiler warning
        }

        if (veryVerbose) cout << "Check basic behavior" << endl;

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

            TestConstraint::s_constraintValue = true;
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

            TestConstraint::s_constraintValue = false;
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

        if (veryVerbose) cout << "Negative testing." << endl;

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
      case 9: {
        // -------------------------------------------------------------------
        // TESTING ADDITIONAL CONSTRUCTORS
        //
        // Concerns:
        //: 1 Each constructor creates a new object having the expected
        //:   attributes: option type, linked variable (or not), constraint,
        //:   and allocator.
        //:
        //: 2 Each constructor allocates from the intended allocator.
        //:
        //:   1 The supplied allocator is used when a user-defined constraint
        //:     is given.
        //:
        //:   2 Otherwise, the default allocator is used.
        //:
        //: 3 Each constructor provides the basic exception-safety guarantee.
        //
        // Plan:
        //: 1 For each set of attributes appearing in the (representative)
        //:   'OPTION_TYPEINFO' table create a 'TypeInfo' object using the
        //:   (overloaded) 'u::constructTypeInfo' helper functions and then use
        //:   the (proven) accessors to show that the created object has the
        //:   expected attributes.  (C-1)
        //:
        //: 2 Perform these tests for scenarios in which no allocator is
        //:   specified, in which the 0-allocator is specified, and in which an
        //:   allocator is explicitly specified.
        //:
        //: 3 In the later case, use 'BSLMA_TESTALLOCATOR_EXCEPTION_*' macros
        //:   to inject an exception for each allocation during construction.
        //:   (C-3)
        //:
        //:   1 A pair of nested 'BSLMA_TESTALLOCATOR_EXCEPTION_*' macros is
        //:     used because the management of a constraint can use both
        //:     the default allocator and the supplied allocator.
        //:
        //: 4 Use 'bslma::TestAllocatorMonitor' objects to confirm allocations
        //:   from the intended allocators.  (C-2).
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
                P(CONSTRAINT)
            }

            ASSERT(Ot::e_BOOL != TYPE || 0 == CONSTRAINT);

            bsls::ObjectBuffer<Obj>  xObjectBuffer;
            Obj                     *xPtr = 0;
            bslma::Allocator        *xOa  = 0;

            char cfgLimit = CONSTRAINT
                          ? 'f'    // has    constraint
                          : 'c';   // has no constraint

            for (char cfg = 'a'; cfg <= cfgLimit; ++cfg) {
                const char CONFIG = cfg;

                if (veryVerbose) {
                    P(CONFIG)
                }

                bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
                bslma::TestAllocator da("default",  veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                int completeExceptionCount = 0;

                const bool usesSuppliedAllocator = 'f' == cfg;
                    // Supplied allocator used to wrap a user-supplied
                    // constraint.  Otherwise, the implementation uses the
                    // default allocator to create shared pointers the
                    // "always true" constraints.

                bslma::TestAllocatorMonitor dam(&da);
                bslma::TestAllocatorMonitor sam(&sa);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(da) {
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {

                    switch(cfg) {
                      case 'a': {
                        xPtr = u::constructTypeInfo(xObjectBuffer.buffer(),
                                                    TYPE,
                                                    VARIABLE);  // ACTION
                        xOa = &da;
                      } break;
                      case 'b': {
                        xPtr = u::constructTypeInfo(
                                 xObjectBuffer.buffer(),
                                 TYPE,
                                 VARIABLE,
                                 static_cast<bslma::Allocator *>(0)); // ACTION
                        xOa = &da;
                      } break;
                      case 'c': {
                        xPtr = u::constructTypeInfo(xObjectBuffer.buffer(),
                                                    TYPE,
                                                    VARIABLE,
                                                    &sa);  // ACTION
                        xOa = &sa;
                      } break;
                      case 'd': {
                        xPtr = u::constructTypeInfo(xObjectBuffer.buffer(),
                                                    TYPE,
                                                    VARIABLE,
                                                    CONSTRAINT);  // ACTION
                        xOa = &da;
                      } break;
                      case 'e': {
                        xPtr = u::constructTypeInfo(
                                 xObjectBuffer.buffer(),
                                 TYPE,
                                 VARIABLE,
                                 CONSTRAINT,
                                 static_cast<bslma::Allocator *>(0)); // ACTION
                        xOa = &da;
                      } break;
                      case 'f': {
                        xPtr = u::constructTypeInfo(xObjectBuffer.buffer(),
                                                    TYPE,
                                                    VARIABLE,
                                                    CONSTRAINT,
                                                    &sa);  // ACTION
                        xOa = &sa;
                      } break;
                      default: {
                        BSLS_ASSERT(!"Reachable");
                      } break;
                    }
                    ++completeExceptionCount;
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(CONFIG, xPtr);
                ASSERTV(CONFIG, xOa);
                ASSERTV(CONFIG, 1 == completeExceptionCount);

                if (veryVerbose) {
                    P_(CONFIG)
                    P_(dam.isTotalUp())
                     P(sam.isTotalUp())
                }

                // Vet allocator usage.

                if (usesSuppliedAllocator) {
                      ASSERT(sam.isTotalUp());
                } else {
                      ASSERT(dam.isTotalUp());
                }

                // Vet object.

                Obj& mX = *xPtr; const Obj& X = mX;

                ASSERT(TYPE     ==                     X.type());
                ASSERT(VARIABLE == static_cast<void *>(X.linkedVariable()));
                ASSERT(xOa      ==                     X.allocator());

                xPtr->~Obj();

                ASSERT(dam.isInUseSame());
                ASSERT(sam.isInUseSame());
            }
        }
      } break;
      case 8: {
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
        //: 3 The allocator of the assigned-to object ('lhs') is preserved.
        //:
        //: 4 Assignment does not allocate memory.
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
        //   TypeInfo& operator=(const TypeInfo& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "COPY ASSIGNMENT" << endl
                                  << "===============" << endl;

        if (verbose) cout <<
             "Verify that the signature and return type are standard." << endl;
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            operatorPtr operatorAssignment = &Obj::operator=;

            (void)operatorAssignment;  // quash potential compiler warning
        }

        if (verbose) cout << "Testing assignment operator." << endl;

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
            Obj mW;  const Obj& W = mW;

            u::setTypeInfo(&mX, TYPE1, VARIABLE1, CONSTRAINT1);
            u::setTypeInfo(&mZ, TYPE1, VARIABLE1, CONSTRAINT1);
            u::setTypeInfo(&mW, TYPE1, VARIABLE1, CONSTRAINT1);

            mZ.setConstraint(X.constraint());  // shared constraint
            ASSERT(X == Z);
            mW.setConstraint(Z.constraint());  // shared constraint
            ASSERT(W == Z);

            Obj *mR = &(mX = X);  // ACTION
            ASSERT(mR == &mX);
            ASSERT(X  == Z);
            ASSERT(Z  == W);

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

                Obj *mR = &(mY = X);  // ACTION

                LOOP2_ASSERT(LINE1, LINE2, samX.isTotalSame());
                LOOP2_ASSERT(LINE1, LINE2, samY.isTotalSame());

                LOOP2_ASSERT(LINE1, LINE2, mR   == &mY);
                LOOP2_ASSERT(LINE1, LINE2, X    == Y);
                LOOP2_ASSERT(LINE1, LINE2, &saX == X.allocator());
                LOOP2_ASSERT(LINE1, LINE2, &saY == Y.allocator());
            }
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        //
        // Concerns:
        //: 1 Equality: The copy constructor creates a new object that compares
        //:   equal to the original object.
        //:
        //: 2 Allocators:
        //:   1 The allocator of the created object depends on its constructor
        //:     argument (not the allocator of the original object).
        //:
        //:   2 If the allocator argument of the object is 0 or not specified,
        //:     the new object uses the default allocator.
        //:
        //:   3 The copy constructor does not allocate.  Note that this implies
        //:     we need not do exception tests.
        //
        // Plans:
        //: 1 Do table-driven testing using the 'OPTION_TYPEINFO' array of
        //:   representative values described in earlier tests.
        //:
        //: 2 Use 'operator==' to confirm the equality of the new object (C-1).
        //:
        //: 3 Use the "footprint" idiom to repeat each test for the cases of an
        //:   unspecified allocator, a 0-allocator, and an explicitly supplied
        //:   allocator.
        //:
        //:   1 Confirm the allocator of the new object using the 'allocator'
        //:     accessor.  (C-2.1..2)
        //:
        //:   2 Use 'bslma::TestAllocatorMonitor' objects to confirm that no
        //:     memory is allocated by any copy.  (C-2.3)
        //
        // Testing:
        //   TypeInfo(const TypeInfo& original, *bA = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;

        for (int i = 0; i < NUM_OPTION_TYPEINFO; ++i) {
            const int       LINE       = OPTION_TYPEINFO[i].d_line;
            const ElemType  TYPE       = OPTION_TYPEINFO[i].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[i].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[i].d_constraint_p;

            if (veryVerbose) {
                T_ P_(LINE) P_(i) P(TYPE)
                T_ P_(VARIABLE) P(CONSTRAINT)
            }

            Obj mX;  const Obj& X = mX;

            u::setTypeInfo(&mX, TYPE, VARIABLE, CONSTRAINT);

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

                ASSERTV(LINE, CONFIG,  oam.isTotalSame());
                ASSERTV(LINE, CONFIG, noam.isTotalSame());

                // Vet copied object.

                Obj&  mY = *objPtr;  const Obj& Y = mY;

                ASSERTV(LINE, CONFIG, X   == Y);
                ASSERTV(LINE, CONFIG, &oa == Y.allocator());

                fa.deleteObject(objPtr); // Clean up
            }
        }
      } break;
      case 6: {
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
        //: 2 Using the table-driven technique, specify a set of unique object
        //:   values (one per row) in terms of their individual salient
        //:   attributes such that each row differs from the others with
        //:   respect to *one* salient value where the salient attributes are:
        //:   o type
        //:   o linked variable (or not)
        //:   o constraint (or not)
        //:
        //: 3 Objects created from different rows of salient values compare
        //:   unequal.  By taking the cross product of these objects, we
        //:   demonstrate that a difference in *any* individual salient value
        //:   results in inequality, thus demonstrating that each salient value
        //:   contributes to the equality comparison.  Note that objects
        //:   compare equal for only those cases of the cross product when both
        //:   rows are the same.  (C-1)
        //:
        //: 4 For each test of equality, create a parallel test that checks
        //:   inequality (the inverse operator), and (when the two arguments
        //:   are different) also create a test case where the two arguments
        //:   are switched (showing commutativity).  (C-2)
        //:
        //: 5 Install a test allocator as the default allocator.  Create a test
        //:   allocator monitor object before each group of operator tests and
        //:   confirm afterwards that the 'isTotalSame' returns 'true' (showing
        //:   that no allocations occurred when exercising the operators).
        //:
        //: 6 Repeat each test between two objects so that the objects use the
        //:   same allocator in one test and use different allocators in the
        //:   other.  Results should not change and thereby show that the
        //:   object allocator is not salient to equality.  (C-1.3)
        //
        // Testing:
        //   bool operator==(const TypeInfo& lhs, rhs);
        //   bool operator!=(const TypeInfo& lhs, rhs);
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

        for (int i = 0; i < NUM_OPTION_TYPEINFO; ++i) {
            const int       LINE1 = OPTION_TYPEINFO[i].d_line;
            const ElemType  TYPE1 = OPTION_TYPEINFO[i].d_type;
            void       *VARIABLE1 = OPTION_TYPEINFO[i].d_linkedVariable_p;
            void     *CONSTRAINT1 = OPTION_TYPEINFO[i].d_constraint_p;

            if (veryVerbose) { T_ P_(LINE1) P(TYPE1) }

            bslma::TestAllocator sa1("supplied1", veryVeryVeryVerbose);
            bslma::TestAllocator sa2("supplied2", veryVeryVeryVerbose);

            bslma::TestAllocator *saX = &sa1;

            Obj mX(saX);  const Obj& X = mX;

            u::setTypeInfo(&mX, TYPE1, VARIABLE1, CONSTRAINT1);

            bslma::TestAllocatorMonitor samX(saX);

            LOOP_ASSERT(LINE1,  (X == X));   // ACTION
            LOOP_ASSERT(LINE1, !(X != X));   // ACTION

            LOOP_ASSERT(LINE1, samX.isTotalSame());

            for (int j = 0; j < NUM_OPTION_TYPEINFO; ++j) {
                const int       LINE2 = OPTION_TYPEINFO[j].d_line;
                const ElemType  TYPE2 = OPTION_TYPEINFO[j].d_type;
                void       *VARIABLE2 = OPTION_TYPEINFO[j].d_linkedVariable_p;
                void     *CONSTRAINT2 = OPTION_TYPEINFO[j].d_constraint_p;

                if (veryVerbose) { T_ T_ P_(LINE2) P(TYPE2) }

                for (int k = 0; k < 2; ++k) {

                    if (veryVerbose) { T_ T_ T_  P(k) }

                    bslma::TestAllocator *saY = k % 2
                                              ? &sa1  //     same as 'X'
                                              : &sa2; // not same as 'X'

                    Obj mY(saY);  const Obj& Y = mY;

                    u::setTypeInfo(&mY, TYPE2, VARIABLE2, CONSTRAINT2);

                    bool isSame = (TYPE1       == TYPE2)
                               && (VARIABLE1   == VARIABLE2)
                               && (CONSTRAINT1 == 0)
                               && (CONSTRAINT2 == 0);

                    bslma::TestAllocatorMonitor samY(saY);

                    LOOP2_ASSERT(LINE1, LINE2,  isSame == (X == Y));  // ACTION
                    LOOP2_ASSERT(LINE1, LINE2, !isSame == (X != Y));  // ACTION
                    LOOP2_ASSERT(LINE1, LINE2,  isSame == (Y == X));  // ACTION
                    LOOP2_ASSERT(LINE1, LINE2, !isSame == (Y != X));  // ACTION

                    LOOP2_ASSERT(LINE1, LINE2, samY.isTotalSame());

                    if (TYPE1 == TYPE2) {
                        mY.setConstraint(X.constraint());  // Constraint shared
                                                           // so constraints
                                                           // are equal.
                        isSame = (TYPE1     == TYPE2)
                              && (VARIABLE1 == VARIABLE2);

                        bslma::TestAllocatorMonitor samX(saX);
                        bslma::TestAllocatorMonitor samY(saY);

                        LOOP2_ASSERT(LINE1, LINE2,  isSame == (X == Y));
                        LOOP2_ASSERT(LINE1, LINE2, !isSame == (X != Y));
                        LOOP2_ASSERT(LINE1, LINE2,  isSame == (Y == X));
                        LOOP2_ASSERT(LINE1, LINE2, !isSame == (Y != X));

                        LOOP2_ASSERT(LINE1, LINE2, samX.isTotalSame());
                        LOOP2_ASSERT(LINE1, LINE2, samY.isTotalSame());
                    }
                }
            }
        }
      } break;
      case 5: {
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
        //:   using a compile-time assertion at file scope.
        //
        // Testing:
        //   ostream& print(ostream& stream, int level = 0, int spl = 4) const;
        //   operator<<(ostream& stream, const TypeInfo& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRINT AND OUTPUT OPERATOR" << endl
                          << "-------------------------" << endl;

        if (veryVerbose) cout
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

        if (veryVerbose) cout << "Check output" << endl;

        for (int i = 0; i < NUM_OPTION_TYPEINFO; ++i) {
            const int       LINE       = OPTION_TYPEINFO[i].d_line;
            const ElemType  TYPE       = OPTION_TYPEINFO[i].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[i].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[i].d_constraint_p;

            if (veryVerbose) {
                T_ P_(LINE) P_(i) P(TYPE)
                T_ P_(VARIABLE) P(CONSTRAINT)
            }

            Obj mX;  const Obj& X = mX;

            u::setTypeInfo(&mX, TYPE, VARIABLE, CONSTRAINT);

            bsl::ostringstream ossMethod;
            bsl::ostringstream ossOperator;

            ASSERTV(LINE, &ossMethod   == &X.print(ossMethod));  // ACTION
            ASSERTV(LINE, &ossOperator == &(ossOperator << X));  // ACTION

            bsl::string stringMethod  (ossMethod  .str());
            bsl::string stringOperator(ossOperator.str());

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

        if (veryVerbose) cout << "Negative testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            Obj mX; const Obj& X = mX;

            bsl::ostringstream oss;

            ASSERT_PASS(X.print(oss, 0,  1));
            ASSERT_PASS(X.print(oss, 0,  0));
            ASSERT_FAIL(X.print(oss, 0, -1));
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // 'balcl::TypeInfoUtil': 'satisfiesConstraint'
        //
        // Concerns:
        //: 1 Each of the 'satisfiesConstraint' overloads returns the expected
        //:   result for the given value and constraint.
        //:
        //: 2 'const'-qualification:
        //:   1 The first two parameters are 'const' qualified.
        //:   2 The third parameter (stream), if present, is not 'const'
        //:     qualified.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use 'OPTION_TYPEINFO', a table of representative inputs, that
        //:   includes an entry for each supported type having a constraint.
        //:
        //:   o Note that constraints defined in the table can be set via a
        //:     public static variable to unconditionally return 'true' or
        //:     'false'.
        //
        //: 2 Also use 'OPTION_DEFAULT_VALUES', a table of values for each
        //:   supported option type.  Note that none of those values is the
        //:   default value for the type.
        //:
        //: 3 For each entry in 'OPTION_TYPEINFO' that defines a constraint,
        //:   create an object, create a value of the appropriate type from
        //:   'OPTION_DEFAULT_VALUES', call 'satisfiesConstraint', and confirm
        //:   that the result matches that expected per the current value of
        //:   the constraint's static variable.  Toggle the static variable and
        //:   retest.  (C-1)
        //:
        //: 4 The first two arguments are always passed by 'const' reference.
        //:   The 'stream' argument is shown to be non-'const' qualified by
        //:   having the test constraint write an error message when it returns
        //:   'false'.  (C-2)
        //:
        //: 5 Do negative tests of defensive checks using 'BSLS_ASSERTTEST_*'
        //:   macros.  (C-3)
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

            const int   j = i % NUM_OPTION_DEFAULT_VALUES;
            const void *VALUE = OPTION_DEFAULT_VALUES[j].d_value_p;

            const ElemType OTYPE = OPTION_DEFAULT_VALUES[j].d_type;

            ASSERTV(TYPE, OTYPE, TYPE == OTYPE);

            if (CONSTRAINT) {
                Obj mX;  const Obj& X = mX;

                u::setTypeInfo(&mX, TYPE, VARIABLE, CONSTRAINT);

                OptionValue ELEMENT(TYPE);
                u::setOptionValue(&ELEMENT, VALUE, TYPE);

                bsl::ostringstream ossElement, ossValue;

                TestConstraint::s_constraintValue = true;
                ASSERT( ObjUtil::satisfiesConstraint(ELEMENT, X            ));
                ASSERT( ObjUtil::satisfiesConstraint(ELEMENT, X, ossElement));
                ASSERT( ObjUtil::satisfiesConstraint(VALUE,   X            ));
                ASSERT( ObjUtil::satisfiesConstraint(VALUE,   X, ossValue  ));

                ASSERTV(LINE, ossElement.str(),  ossElement.str().empty());
                ASSERTV(LINE, ossValue  .str(),  ossValue  .str().empty());

                ossElement.str(""); ossElement.clear();
                ossValue  .str(""); ossValue  .clear();

                TestConstraint::s_constraintValue = false;
                ASSERT(!ObjUtil::satisfiesConstraint(ELEMENT, X            ));
                ASSERT(!ObjUtil::satisfiesConstraint(ELEMENT, X, ossElement));
                ASSERT(!ObjUtil::satisfiesConstraint(VALUE,   X            ));
                ASSERT(!ObjUtil::satisfiesConstraint(VALUE,   X, ossValue  ));

                ASSERTV(LINE, ossElement.str(), !ossElement.str().empty());
                ASSERTV(LINE, ossValue  .str(), !ossValue  .str().empty());

                ossElement.str(""); ossElement.clear();
                ossValue  .str(""); ossValue  .clear();

                TestConstraint::s_constraintValue = true;
                ASSERT( ObjUtil::satisfiesConstraint(ELEMENT, X            ));
                ASSERT( ObjUtil::satisfiesConstraint(ELEMENT, X, ossElement));
                ASSERT( ObjUtil::satisfiesConstraint(VALUE,   X            ));
                ASSERT( ObjUtil::satisfiesConstraint(VALUE,   X, ossValue  ));

                ASSERTV(LINE, ossElement.str(),  ossElement.str().empty());
                ASSERTV(LINE,   ossValue.str(),    ossValue.str().empty());

                // Negative tests
                bsls::AssertTestHandlerGuard hG;
       //v------^
         ASSERT_PASS( ObjUtil::satisfiesConstraint(ELEMENT,   X            ));
         ASSERT_PASS( ObjUtil::satisfiesConstraint(ELEMENT,   X, ossElement));
       //^------v

                OptionValue ELEMENT_NG(u::shiftType(TYPE, 1));

       //v------^
        ASSERT_FAIL( ObjUtil::satisfiesConstraint(ELEMENT_NG, X            ));
        ASSERT_FAIL( ObjUtil::satisfiesConstraint(ELEMENT_NG, X, ossElement));
       //^------v

            } else {
                Obj mX;  const Obj& X = mX;
                u::setTypeInfo(&mX, TYPE, VARIABLE, 0);  // no constraint

                OptionValue ELEMENT(TYPE);
                if (VALUE) {
                    u::setOptionValue(&ELEMENT, VALUE, TYPE);
                }

                bsl::ostringstream ossElement;

                ASSERT( ObjUtil::satisfiesConstraint(ELEMENT, X            ));
                ASSERT( ObjUtil::satisfiesConstraint(ELEMENT, X, ossElement));

                ASSERTV(LINE, ossElement.str(),  ossElement.str().empty());

                // Negative tests
                bsls::AssertTestHandlerGuard hG;
       //v------^
         ASSERT_PASS( ObjUtil::satisfiesConstraint(ELEMENT,   X            ));
         ASSERT_PASS( ObjUtil::satisfiesConstraint(ELEMENT,   X, ossElement));
       //^------v

                OptionValue ELEMENT_NG(u::shiftType(TYPE, 1));

       //v------^
        ASSERT_FAIL( ObjUtil::satisfiesConstraint(ELEMENT_NG, X            ));
        ASSERT_FAIL( ObjUtil::satisfiesConstraint(ELEMENT_NG, X, ossElement));
       //^------v
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // HELPER: 'u::shiftType'
        //
        // Concerns:
        //: 1 The returned (enumerator) value is offset by the given 'offset'
        //:   from the given (enumerator) value when the enumerators are
        //:   ordered in their sequence of definition.
        //
        //: 2 An offset that extends past 'Ot::e_TIME_ARRAY' continues with
        //:   'Ot::e_BOOL' (and vice versa).
        //:
        //: 3 The offset can be negative.
        //:
        //: 4 The value 'Ot::e_VOID' is ignored in the calculation.
        //
        // Plan:
        //: 1 Using a table-driven test, compare calculated results with
        //:   expected results the extreme values of the sequence.
        //
        // Testing:
        //   CONCERN: HELPER 'u::shiftType'
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "HELPER: 'u::shiftType'" << endl
                          << "======================" << endl;

        static const struct {
            int      d_line;
            Ot::Enum d_input;
            int      d_offset;
            Ot::Enum d_output;
        } DATA[] = {
           //LINE   INPUT            OFFS  OUTPUT
           //----   ---------------- ----  --------------------
            { L_,   Ot::e_BOOL,       -34, Ot::e_BOOL           }
          , { L_,   Ot::e_BOOL,       -17, Ot::e_BOOL           }
          , { L_,   Ot::e_BOOL,         0, Ot::e_BOOL           }
          , { L_,   Ot::e_BOOL,        17, Ot::e_BOOL           }
          , { L_,   Ot::e_BOOL,        34, Ot::e_BOOL           }

          , { L_,   Ot::e_BOOL,        -2, Ot::e_DATE_ARRAY     }
          , { L_,   Ot::e_BOOL,        -1, Ot::e_TIME_ARRAY     }
          , { L_,   Ot::e_BOOL,         0, Ot::e_BOOL           }
          , { L_,   Ot::e_BOOL,         1, Ot::e_CHAR           }
          , { L_,   Ot::e_BOOL,         2, Ot::e_INT            }

          , { L_,   Ot::e_TIME_ARRAY,  -2, Ot::e_DATETIME_ARRAY }
          , { L_,   Ot::e_TIME_ARRAY,  -1, Ot::e_DATE_ARRAY     }
          , { L_,   Ot::e_TIME_ARRAY,   0, Ot::e_TIME_ARRAY     }
          , { L_,   Ot::e_TIME_ARRAY,   1, Ot::e_BOOL           }
          , { L_,   Ot::e_TIME_ARRAY,   2, Ot::e_CHAR           }

          , { L_,   Ot::e_TIME_ARRAY, -34, Ot::e_TIME_ARRAY     }
          , { L_,   Ot::e_TIME_ARRAY, -17, Ot::e_TIME_ARRAY     }
          , { L_,   Ot::e_TIME_ARRAY,   0, Ot::e_TIME_ARRAY     }
          , { L_,   Ot::e_TIME_ARRAY,  17, Ot::e_TIME_ARRAY     }
          , { L_,   Ot::e_TIME_ARRAY,  34, Ot::e_TIME_ARRAY     }
        };

        bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int        LINE   = DATA[ti].d_line;
            const Ot::Enum  INPUT   = DATA[ti].d_input;
            const int        OFFSET = DATA[ti].d_offset;
            const Ot::Enum OUTPUT   = DATA[ti].d_output;

            if (veryVerbose) {
                T_ P_(ti) P_(LINE) P_(INPUT) P_(OFFSET) P(OUTPUT)
            }

            ASSERT(OUTPUT == u::shiftType(INPUT, OFFSET));
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
        //: 1 Create 'OPTION_TYPEINFO', a table representing the space of
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
        //:     in a compile-time assertion at file scope.
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

        if (verbose) cout << "Without passing in an allocator." << endl;

        for (int i = 0; i < NUM_OPTION_TYPEINFO; ++i) {
            const int       LINE       = OPTION_TYPEINFO[i].d_line;
            const ElemType  TYPE       = OPTION_TYPEINFO[i].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[i].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[i].d_constraint_p;

            Obj mX;  const Obj& X = mX;      // ACTION

            if (veryVerbose) {
                T_ P_(LINE) P_(i) P(TYPE)
                T_ P_(VARIABLE) P(CONSTRAINT)
                T_ T_ P_(LINE) P_(i) P(X)
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

                {
                    Obj mX;  const Obj& X = mX;
                    u::setType(&mX, TYPE);
                    u::setConstraint(&mX, TYPE, CONSTRAINT);

                    LOOP_ASSERT(LINE, DEFAULT_CONSTRAINT != X.constraint());
                    mX.setConstraint( DEFAULT_CONSTRAINT);  // ACTION
                    LOOP_ASSERT(LINE, DEFAULT_CONSTRAINT == X.constraint());

                    Obj mY;  const Obj& Y = mY;
                    u::setType(&mY, TYPE);
                    u::setConstraint(&mY, TYPE, CONSTRAINT);

                    LOOP_ASSERT(LINE, DEFAULT_CONSTRAINT != Y.constraint());
                    LOOP_ASSERT(LINE, Y.constraint()     != X.constraint());
                    mX.setConstraint(Y.constraint());  // ACTION
                    LOOP_ASSERT(LINE, DEFAULT_CONSTRAINT != X.constraint());
                    LOOP_ASSERT(LINE, Y.constraint()     == X.constraint());
                }
            }

            mX.resetConstraint();

            LOOP_ASSERT(LINE, TYPE               == X.type());
            LOOP_ASSERT(LINE, 0                  == X.linkedVariable());
            LOOP_ASSERT(LINE, DEFAULT_CONSTRAINT == X.constraint());

            if (VARIABLE && CONSTRAINT) {
                u::setLinkedVariable(&mX, TYPE, VARIABLE);
                u::setConstraint    (&mX, TYPE, CONSTRAINT);

                if (veryVerbose) {
                    T_ T_ P_(LINE) P_(i) P(X)
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

        if (verbose) cout << "Passing in an allocator; no exceptions"
                          << endl;

        for (int i = 0; i < NUM_OPTION_TYPEINFO; ++i) {
            const int       LINE       = OPTION_TYPEINFO[i].d_line;
            const ElemType  TYPE       = OPTION_TYPEINFO[i].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[i].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[i].d_constraint_p;

            if (veryVerbose) {
                T_ P_(LINE) P_(i) P(TYPE)
                T_ P_(VARIABLE) P(CONSTRAINT)
            }

            bslma::TestAllocatorMonitor dam(&da);
            bslma::TestAllocatorMonitor sam(&sa);

            Obj mX(&sa);  const Obj& X = mX;

            LOOP_ASSERT(LINE, sam.isTotalSame());

            LOOP_ASSERT(LINE, Ot::e_STRING == X.type());
            LOOP_ASSERT(LINE, 0            == X.linkedVariable());
            LOOP_ASSERT(LINE, &sa          == X.allocator());

            bsl::shared_ptr<ObjConstraint> DEFAULT_CONSTRAINT = X.constraint();

            u::setTypeInfo(&mX, TYPE, VARIABLE, CONSTRAINT);  // ACTION

            if (CONSTRAINT) {
                LOOP_ASSERT(LINE, sam.isTotalUp());
            } else {
                LOOP_ASSERT(LINE, sam.isTotalSame());
            }
            LOOP_ASSERT(LINE, dam.isTotalUp());

            LOOP_ASSERT(LINE, TYPE     == X.type());
            LOOP_ASSERT(LINE, VARIABLE == X.linkedVariable());

            // Note that the test for 'satisfiesConstraint' (case 4)
            // demonstrates that 'setConstraint' does install the intended
            // constraint.
        }

        if (verbose) cout << "With exceptions." << endl;

        for (int i = 0; i < NUM_OPTION_TYPEINFO; ++i) {
            const int       LINE       = OPTION_TYPEINFO[i].d_line;
            const ElemType  TYPE       = OPTION_TYPEINFO[i].d_type;
            void           *VARIABLE   = OPTION_TYPEINFO[i].d_linkedVariable_p;
            void           *CONSTRAINT = OPTION_TYPEINFO[i].d_constraint_p;

            if (veryVerbose) {
                T_ P_(LINE) P_(i) P(TYPE)
                T_ P_(VARIABLE) P(CONSTRAINT)
            }

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

        Obj mX(Ot::k_INT);           const Obj& X = mX;
        Obj mY(Ot::k_BOOL);          const Obj& Y = mY;
        Obj mZ(Ot::k_STRING_ARRAY);  const Obj& Z = mZ;

        ASSERT(Ot::e_INT          == X.type());
        ASSERT(Ot::e_BOOL         == Y.type());
        ASSERT(Ot::e_STRING_ARRAY == Z.type());

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
