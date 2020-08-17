// balcl_optionvalue.t.cpp                                            -*-C++-*-
#include <balcl_optionvalue.h>

#include <balcl_optiontype.h>

#include <bdlb_printmethods.h>

#include <bdlt_date.h>
#include <bdlt_time.h>
#include <bdlt_datetime.h>

#include <bslim_testutil.h>

#include <bsltf_templatetestfacility.h>

#include <bslma_default.h>  // 'bslma::globalAllocator'
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_assert.h>
#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_nameof.h>

#include <bsl_cstdlib.h>  // 'bsl::atoi'
#include <bsl_cstring.h>  // 'bsl::strcmp'
#include <bsl_iostream.h>
#include <bsl_ostream.h>  // 'operator<<'
#include <bsl_sstream.h>  // 'bsl::ostringstream'
#include <bsl_string.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements a single, value-semantic class, that
// provides access to a single value that:
//
//: o Can be of any of the types (17) defined by 'balcl::OptionType'.
//: o Can be in an unspecified (null) state.
//: o Can be in a unset state.
//
// By design, the type of these (type variant) objects can be changed by using
// a method, 'setType', that is implemented by 'reset'ing the object to its
// unset state.  Although this feature is less than optimal for a class of
// general use, the behavior suffices for its use in 'balcl_commandline' and
// saves us the need to explore the large cross product of supported types for
// each operation.
//
// The class under test is based on two other classes 'bdlb::Variant' and
// 'bdlb::NullableValue' that are "configured" and wrapped in a manner suitable
// for the needs of 'balcl_commandline'.  As those classes are themselves
// thoroughly tested, our concerns for this class are primarily:
//
//: o Have we provided the intended signatures?
//: o Are we correctly forwarding to the other classes?
//: o Have we implemented the intended precondition checks?
//: o Have we providing 'const' qualification were expected?
//: o Have we defined the expected default values for optional arguments?
//
// We follow the general pattern for testing a value-semantic class.  Where
// per-type testing is needed, we use the 'bsltf_templatetestfacility' so tests
// need not be explicitly written for each type.
//
// Primary Manipulators:
//: o void reset();
//: o template <class TYPE> void set(const TYPE& value);
//: o void setNull();
//: o void setType(OptionType::Enum type);
//
// Basic Accessors:
//: o bool hasNonVoidType() const;
//: o bool isNull() const;
//: o OptionType::Enum type() const;
//: o template <class TYPE> const TYPE& the() const;
//: o bslma::Allocator *allocator() const;
//
// ----------------------------------------------------------------------------
// CREATORS
// [ 3] OptionValue();
// [ 3] OptionValue(bslma::Allocator *basicAllocator);
// [ 4] OptionValue(Ot::Enum type, Allocator *bA = 0);
// [ 4] OptionValue(bool           v, *bA = 0);
// [ 4] OptionValue(char           v, *bA = 0);
// [ 4] OptionValue(int            v, *bA = 0);
// [ 4] OptionValue(Int64          v, *bA = 0);
// [ 4] OptionValue(double         v, *bA = 0);
// [ 4] OptionValue(const string&  v, *bA = 0);
// [ 4] OptionValue(Datetime       v, *bA = 0);
// [ 4] OptionValue(Date           v, *bA = 0);
// [ 4] OptionValue(Time           v, *bA = 0);
// [ 4] OptionValue(const vector<char>&     v, *bA = 0);
// [ 4] OptionValue(const vector<int>&      v, *bA = 0);
// [ 4] OptionValue(const vector<Int64>&    v, *bA = 0);
// [ 4] OptionValue(const vector<double>&   v, *bA = 0);
// [ 4] OptionValue(const vector<string>&   v, *bA = 0);
// [ 4] OptionValue(const vector<Datetime>& v, *bA = 0);
// [ 4] OptionValue(const vector<Date>&     v, *bA = 0);
// [ 4] OptionValue(const vector<Time>&     v, *bA = 0);
// [ 6] OptionValue(const OV& o, Allocator *bA = 0);
// [ 3] ~OptionValue() = default;
//
// MANIPULATORS
// [ 8] operator=(const OptionValue& rhs);
// [ 3] void reset();
// [ 3] template <class TYPE> void set(const TYPE& value);
// [ 3] void setNull();
// [ 3] void setType(OptionType::Enum type);
// [ 9] template <class TYPE> TYPE& the();
//
// [ 7] void swap(OptionValue& other);
//
// ACCESSORS
// [ 3] bool hasNonVoidType() const;
// [ 3] bool isNull() const;
// [ 3] OptionType::Enum type() const;
// [ 3] template <class TYPE> const TYPE& the() const;
//
// [ 3] bslma::Allocator *allocator() const;
// [ 3] ostream& print(ostream& s, int level = 0, int sPL = 4) const;
//
// FREE OPERATORS
// [ 5] bool operator==(const OptionValue& lhs, rhs);
// [ 5] bool operator!=(const OptionValue& lhs, rhs);
// [ 3] operator<<(bsl::ostream& s, const OptionValue& d);
//
// FREE FUNCTIONS
// [ 7] void swap(OptionValue& a, b);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [10] USAGE EXAMPLES
// [ 2] CONCERN: HELPER 'u::shiftType'

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
//                        GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef balcl::OptionValue Obj;
typedef balcl::OptionType  Ot;

// ============================================================================
//                                TYPE TRAITS
// ----------------------------------------------------------------------------

BSLMF_ASSERT(bslma::UsesBslmaAllocator<Obj>::value);
BSLMF_ASSERT(bdlb::HasPrintMethod     <Obj>::value);

// ============================================================================
//                       GLOBAL OBJECTS SHARED BY TEST CASES
// ----------------------------------------------------------------------------

// STATIC DATA
static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

                        // =============
                        // struct ValueA
                        // =============

template <class TYPE>
struct ValueA
{
    static TYPE s_value;
};

template <>
struct ValueA<Ot::Bool>          { static Ot::Bool          s_value; };
template <>
struct ValueA<Ot::Char>          { static Ot::Char          s_value; };
template <>
struct ValueA<Ot::Int>           { static Ot::Int           s_value; };
template <>
struct ValueA<Ot::Int64>         { static Ot::Int64         s_value; };
template <>
struct ValueA<Ot::Double>        { static Ot::Double        s_value; };
template <>
struct ValueA<Ot::String>        { static Ot::String        s_value; };
template <>
struct ValueA<Ot::Datetime>      { static Ot::Datetime      s_value; };
template <>
struct ValueA<Ot::Date>          { static Ot::Date          s_value; };
template <>
struct ValueA<Ot::Time>          { static Ot::Time          s_value; };
template <>
struct ValueA<Ot::CharArray>     { static Ot::CharArray     s_value; };
template <>
struct ValueA<Ot::IntArray>      { static Ot::IntArray      s_value; };
template <>
struct ValueA<Ot::Int64Array>    { static Ot::Int64Array    s_value; };
template <>
struct ValueA<Ot::DoubleArray>   { static Ot::DoubleArray   s_value; };
template <>
struct ValueA<Ot::StringArray>   { static Ot::StringArray   s_value; };
template <>
struct ValueA<Ot::DatetimeArray> { static Ot::DatetimeArray s_value; };
template <>
struct ValueA<Ot::DateArray>     { static Ot::DateArray     s_value; };
template <>
struct ValueA<Ot::TimeArray>     { static Ot::TimeArray     s_value; };

                        // -------------
                        // struct ValueA
                        // -------------

Ot::Bool          ValueA<Ot::Bool>         ::s_value = true;
Ot::Char          ValueA<Ot::Char>         ::s_value = 'a';
Ot::Int           ValueA<Ot::Int>          ::s_value =  1;
Ot::Int64         ValueA<Ot::Int64>        ::s_value =
                                                     static_cast<Ot::Int64>(2);
Ot::Double        ValueA<Ot::Double>       ::s_value =
                                                  static_cast<Ot::Double>(3.0);
Ot::String        ValueA<Ot::String>       ::s_value = bsl::string("Hello");
Ot::Datetime      ValueA<Ot::Datetime>     ::s_value =
                                            bdlt::Datetime( 1, 1, 2, 24, 0, 0);
Ot::Date          ValueA<Ot::Date>         ::s_value = bdlt::Date( 1, 1, 3);
Ot::Time          ValueA<Ot::Time>         ::s_value = bdlt::Time( 0, 0, 1);

Ot::CharArray     ValueA<Ot::CharArray>    ::s_value =
                              bsl::vector<Ot::Char>(1,
                                                    ValueA<Ot::Char>::s_value);

Ot::IntArray      ValueA<Ot::IntArray>     ::s_value =
                                bsl::vector<Ot::Int>(1,
                                                     ValueA<Ot::Int>::s_value);

Ot::Int64Array    ValueA<Ot::Int64Array>   ::s_value =
                            bsl::vector<Ot::Int64>(1,
                                                   ValueA<Ot::Int64>::s_value);

Ot::DoubleArray   ValueA<Ot::DoubleArray>  ::s_value =
                          bsl::vector<Ot::Double>(1,
                                                  ValueA<Ot::Double>::s_value);

Ot::StringArray   ValueA<Ot::StringArray>  ::s_value =
                          bsl::vector<Ot::String>(1,
                                                  ValueA<Ot::String>::s_value);

Ot::DatetimeArray ValueA<Ot::DatetimeArray>::s_value =
                      bsl::vector<Ot::Datetime>(1,
                                                ValueA<Ot::Datetime>::s_value);

Ot::DateArray     ValueA<Ot::DateArray>    ::s_value =
                              bsl::vector<Ot::Date>(1,
                                                    ValueA<Ot::Date>::s_value);

Ot::TimeArray     ValueA<Ot::TimeArray>    ::s_value =
                              bsl::vector<Ot::Time>(1,
                                                    ValueA<Ot::Time>::s_value);

                        // =============
                        // struct ValueB
                        // =============

template <class TYPE>
struct ValueB
{
    static TYPE s_value;
};

template <>
struct ValueB<Ot::Bool>          { static Ot::Bool          s_value; };
template <>
struct ValueB<Ot::Char>          { static Ot::Char          s_value; };
template <>
struct ValueB<Ot::Int>           { static Ot::Int           s_value; };
template <>
struct ValueB<Ot::Int64>         { static Ot::Int64         s_value; };
template <>
struct ValueB<Ot::Double>        { static Ot::Double        s_value; };
template <>
struct ValueB<Ot::String>        { static Ot::String        s_value; };
template <>
struct ValueB<Ot::Datetime>      { static Ot::Datetime      s_value; };
template <>
struct ValueB<Ot::Date>          { static Ot::Date          s_value; };
template <>
struct ValueB<Ot::Time>          { static Ot::Time          s_value; };
template <>
struct ValueB<Ot::CharArray>     { static Ot::CharArray     s_value; };
template <>
struct ValueB<Ot::IntArray>      { static Ot::IntArray      s_value; };
template <>
struct ValueB<Ot::Int64Array>    { static Ot::Int64Array    s_value; };
template <>
struct ValueB<Ot::DoubleArray>   { static Ot::DoubleArray   s_value; };
template <>
struct ValueB<Ot::StringArray>   { static Ot::StringArray   s_value; };
template <>
struct ValueB<Ot::DatetimeArray> { static Ot::DatetimeArray s_value; };
template <>
struct ValueB<Ot::DateArray>     { static Ot::DateArray     s_value; };
template <>
struct ValueB<Ot::TimeArray>     { static Ot::TimeArray     s_value; };

                        // -------------
                        // struct ValueB
                        // -------------

Ot::Bool          ValueB<Ot::Bool>         ::s_value = false;
Ot::Char          ValueB<Ot::Char>         ::s_value = 'b';
Ot::Int           ValueB<Ot::Int>          ::s_value =  4;
Ot::Int64         ValueB<Ot::Int64>        ::s_value =
                                                     static_cast<Ot::Int64>(5);
Ot::Double        ValueB<Ot::Double>       ::s_value =
                                                  static_cast<Ot::Double>(6.0);
Ot::String        ValueB<Ot::String>       ::s_value = bsl::string("World!");
Ot::Datetime      ValueB<Ot::Datetime>     ::s_value =
                                            bdlt::Datetime( 1, 1, 4, 24, 0, 0);
Ot::Date          ValueB<Ot::Date>         ::s_value = bdlt::Date( 1, 1, 5);
Ot::Time          ValueB<Ot::Time>         ::s_value = bdlt::Time( 0, 0, 2);

Ot::CharArray     ValueB<Ot::CharArray>    ::s_value =
                              bsl::vector<Ot::Char>(2,
                                                    ValueB<Ot::Char>::s_value);

Ot::IntArray      ValueB<Ot::IntArray>     ::s_value =
                                bsl::vector<Ot::Int>(2,
                                                     ValueB<Ot::Int>::s_value);

Ot::Int64Array    ValueB<Ot::Int64Array>   ::s_value =
                            bsl::vector<Ot::Int64>(2,
                                                   ValueB<Ot::Int64>::s_value);

Ot::DoubleArray   ValueB<Ot::DoubleArray>  ::s_value =
                          bsl::vector<Ot::Double>(2,
                                                  ValueB<Ot::Double>::s_value);

Ot::StringArray   ValueB<Ot::StringArray>  ::s_value =
                          bsl::vector<Ot::String>(2,
                                                  ValueB<Ot::String>::s_value);

Ot::DatetimeArray ValueB<Ot::DatetimeArray>::s_value =
                      bsl::vector<Ot::Datetime>(2,
                                                ValueB<Ot::Datetime>::s_value);

Ot::DateArray     ValueB<Ot::DateArray>    ::s_value =
                              bsl::vector<Ot::Date>(2,
                                                    ValueB<Ot::Date>::s_value);

Ot::TimeArray     ValueB<Ot::TimeArray>    ::s_value =
                              bsl::vector<Ot::Time>(2,
                                                    ValueB<Ot::Time>::s_value);

#define DEFINE_OBJECT_TEST_SET                                                \
  const Obj Unset;                                                            \
                                                                              \
  Obj mNullBool         (Ot::e_BOOL          ); mNullBool         .setNull(); \
  Obj mNullChar         (Ot::e_CHAR          ); mNullChar         .setNull(); \
  Obj mNullInt          (Ot::e_INT           ); mNullInt          .setNull(); \
  Obj mNullInt64        (Ot::e_INT64         ); mNullInt64        .setNull(); \
  Obj mNullDouble       (Ot::e_DOUBLE        ); mNullDouble       .setNull(); \
  Obj mNullString       (Ot::e_STRING        ); mNullString       .setNull(); \
  Obj mNullDatetime     (Ot::e_DATETIME      ); mNullDatetime     .setNull(); \
  Obj mNullDate         (Ot::e_DATE          ); mNullDate         .setNull(); \
  Obj mNullTime         (Ot::e_TIME          ); mNullTime         .setNull(); \
  Obj mNullCharArray    (Ot::e_CHAR_ARRAY    ); mNullCharArray    .setNull(); \
  Obj mNullIntArray     (Ot::e_INT_ARRAY     ); mNullIntArray     .setNull(); \
  Obj mNullInt64Array   (Ot::e_INT64_ARRAY   ); mNullInt64Array   .setNull(); \
  Obj mNullDoubleArray  (Ot::e_DOUBLE_ARRAY  ); mNullDoubleArray  .setNull(); \
  Obj mNullStringArray  (Ot::e_STRING_ARRAY  ); mNullStringArray  .setNull(); \
  Obj mNullDatetimeArray(Ot::e_DATETIME_ARRAY); mNullDatetimeArray.setNull(); \
  Obj mNullDateArray    (Ot::e_DATE_ARRAY    ); mNullDateArray    .setNull(); \
  Obj mNullTimeArray    (Ot::e_TIME_ARRAY    ); mNullTimeArray    .setNull(); \
                                                                              \
  const Obj& NullBool          = mNullBool;          (void)NullBool;          \
  const Obj& NullChar          = mNullChar;          (void)NullChar;          \
  const Obj& NullInt           = mNullInt;           (void)NullInt;           \
  const Obj& NullInt64         = mNullInt64;         (void)NullInt64;         \
  const Obj& NullDouble        = mNullDouble;        (void)NullDouble;        \
  const Obj& NullString        = mNullString;        (void)NullString;        \
  const Obj& NullDatetime      = mNullDatetime;      (void)NullDatetime;      \
  const Obj& NullDate          = mNullDate;          (void)NullDate;          \
  const Obj& NullTime          = mNullTime;          (void)NullTime;          \
  const Obj& NullCharArray     = mNullCharArray;     (void)NullCharArray;     \
  const Obj& NullIntArray      = mNullIntArray;      (void)NullIntArray;      \
  const Obj& NullInt64Array    = mNullInt64Array;    (void)NullInt64Array;    \
  const Obj& NullDoubleArray   = mNullDoubleArray;   (void)NullDoubleArray;   \
  const Obj& NullStringArray   = mNullStringArray;   (void)NullStringArray;   \
  const Obj& NullDatetimeArray = mNullDatetimeArray; (void)NullDatetimeArray; \
  const Obj& NullDateArray     = mNullDateArray;     (void)NullDateArray;     \
  const Obj& NullTimeArray     = mNullTimeArray;     (void)NullTimeArray;     \
                                                                              \
  Obj mDfltBool         (Ot::e_BOOL          );                               \
  Obj mDfltChar         (Ot::e_CHAR          );                               \
  Obj mDfltInt          (Ot::e_INT           );                               \
  Obj mDfltInt64        (Ot::e_INT64         );                               \
  Obj mDfltDouble       (Ot::e_DOUBLE        );                               \
  Obj mDfltString       (Ot::e_STRING        );                               \
  Obj mDfltDatetime     (Ot::e_DATETIME      );                               \
  Obj mDfltDate         (Ot::e_DATE          );                               \
  Obj mDfltTime         (Ot::e_TIME          );                               \
  Obj mDfltCharArray    (Ot::e_CHAR_ARRAY    );                               \
  Obj mDfltIntArray     (Ot::e_INT_ARRAY     );                               \
  Obj mDfltInt64Array   (Ot::e_INT64_ARRAY   );                               \
  Obj mDfltDoubleArray  (Ot::e_DOUBLE_ARRAY  );                               \
  Obj mDfltStringArray  (Ot::e_STRING_ARRAY  );                               \
  Obj mDfltDatetimeArray(Ot::e_DATETIME_ARRAY);                               \
  Obj mDfltDateArray    (Ot::e_DATE_ARRAY    );                               \
  Obj mDfltTimeArray    (Ot::e_TIME_ARRAY    );                               \
                                                                              \
  const Obj& DfltBool          = mDfltBool;          (void)DfltBool;          \
  const Obj& DfltChar          = mDfltChar;          (void)DfltChar;          \
  const Obj& DfltInt           = mDfltInt;           (void)DfltInt;           \
  const Obj& DfltInt64         = mDfltInt64;         (void)DfltInt64;         \
  const Obj& DfltDouble        = mDfltDouble;        (void)DfltDouble;        \
  const Obj& DfltString        = mDfltString;        (void)DfltString;        \
  const Obj& DfltDatetime      = mDfltDatetime;      (void)DfltDatetime;      \
  const Obj& DfltDate          = mDfltDate;          (void)DfltDate;          \
  const Obj& DfltTime          = mDfltTime;          (void)DfltTime;          \
  const Obj& DfltCharArray     = mDfltCharArray;     (void)DfltCharArray;     \
  const Obj& DfltIntArray      = mDfltIntArray;      (void)DfltIntArray;      \
  const Obj& DfltInt64Array    = mDfltInt64Array;    (void)DfltInt64Array;    \
  const Obj& DfltDoubleArray   = mDfltDoubleArray;   (void)DfltDoubleArray;   \
  const Obj& DfltStringArray   = mDfltStringArray;   (void)DfltStringArray;   \
  const Obj& DfltDatetimeArray = mDfltDatetimeArray; (void)DfltDatetimeArray; \
  const Obj& DfltDateArray     = mDfltDateArray;     (void)DfltDateArray;     \
  const Obj& DfltTimeArray     = mDfltTimeArray;     (void)DfltTimeArray;     \
                                                                              \
  Obj mVal1Bool              (ValueA<Ot::Bool         >::s_value);            \
  Obj mVal1Char              (ValueA<Ot::Char         >::s_value);            \
  Obj mVal1Int               (ValueA<Ot::Int          >::s_value);            \
  Obj mVal1Int64             (ValueA<Ot::Int64        >::s_value);            \
  Obj mVal1Double            (ValueA<Ot::Double       >::s_value);            \
  Obj mVal1String            (ValueA<Ot::String       >::s_value);            \
  Obj mVal1Datetime          (ValueA<Ot::Datetime     >::s_value);            \
  Obj mVal1Date              (ValueA<Ot::Date         >::s_value);            \
  Obj mVal1Time              (ValueA<Ot::Time         >::s_value);            \
  Obj mVal1CharArray         (ValueA<Ot::CharArray    >::s_value);            \
  Obj mVal1IntArray          (ValueA<Ot::IntArray     >::s_value);            \
  Obj mVal1Int64Array        (ValueA<Ot::Int64Array   >::s_value);            \
  Obj mVal1DoubleArray       (ValueA<Ot::DoubleArray  >::s_value);            \
  Obj mVal1StringArray       (ValueA<Ot::StringArray  >::s_value);            \
  Obj mVal1DatetimeArray     (ValueA<Ot::DatetimeArray>::s_value);            \
  Obj mVal1DateArray         (ValueA<Ot::DateArray    >::s_value);            \
  Obj mVal1TimeArray         (ValueA<Ot::TimeArray    >::s_value);            \
                                                                              \
  const Obj& Val1Bool          = mVal1Bool;          (void)Val1Bool;          \
  const Obj& Val1Char          = mVal1Char;          (void)Val1Char;          \
  const Obj& Val1Int           = mVal1Int;           (void)Val1Int;           \
  const Obj& Val1Int64         = mVal1Int64;         (void)Val1Int64;         \
  const Obj& Val1Double        = mVal1Double;        (void)Val1Double;        \
  const Obj& Val1String        = mVal1String;        (void)Val1String;        \
  const Obj& Val1Datetime      = mVal1Datetime;      (void)Val1Datetime;      \
  const Obj& Val1Date          = mVal1Date;          (void)Val1Date;          \
  const Obj& Val1Time          = mVal1Time;          (void)Val1Time;          \
  const Obj& Val1CharArray     = mVal1CharArray;     (void)Val1CharArray;     \
  const Obj& Val1IntArray      = mVal1IntArray;      (void)Val1IntArray;      \
  const Obj& Val1Int64Array    = mVal1Int64Array;    (void)Val1Int64Array;    \
  const Obj& Val1DoubleArray   = mVal1DoubleArray;   (void)Val1DoubleArray;   \
  const Obj& Val1StringArray   = mVal1StringArray;   (void)Val1StringArray;   \
  const Obj& Val1DatetimeArray = mVal1DatetimeArray; (void)Val1DatetimeArray; \
  const Obj& Val1DateArray     = mVal1DateArray;     (void)Val1DateArray;     \
  const Obj& Val1TimeArray     = mVal1TimeArray;     (void)Val1TimeArray;     \
                                                                              \
  Obj mVal2Bool              (ValueB<Ot::Bool         >::s_value);            \
  Obj mVal2Char              (ValueB<Ot::Char         >::s_value);            \
  Obj mVal2Int               (ValueB<Ot::Int          >::s_value);            \
  Obj mVal2Int64             (ValueB<Ot::Int64        >::s_value);            \
  Obj mVal2Double            (ValueB<Ot::Double       >::s_value);            \
  Obj mVal2String            (ValueB<Ot::String       >::s_value);            \
  Obj mVal2Datetime          (ValueB<Ot::Datetime     >::s_value);            \
  Obj mVal2Date              (ValueB<Ot::Date         >::s_value);            \
  Obj mVal2Time              (ValueB<Ot::Time         >::s_value);            \
  Obj mVal2CharArray         (ValueB<Ot::CharArray    >::s_value);            \
  Obj mVal2IntArray          (ValueB<Ot::IntArray     >::s_value);            \
  Obj mVal2Int64Array        (ValueB<Ot::Int64Array   >::s_value);            \
  Obj mVal2DoubleArray       (ValueB<Ot::DoubleArray  >::s_value);            \
  Obj mVal2StringArray       (ValueB<Ot::StringArray  >::s_value);            \
  Obj mVal2DatetimeArray     (ValueB<Ot::DatetimeArray>::s_value);            \
  Obj mVal2DateArray         (ValueB<Ot::DateArray    >::s_value);            \
  Obj mVal2TimeArray         (ValueB<Ot::TimeArray    >::s_value);            \
                                                                              \
  const Obj& Val2Bool          = mVal2Bool;          (void)Val2Bool;          \
  const Obj& Val2Char          = mVal2Char;          (void)Val2Char;          \
  const Obj& Val2Int           = mVal2Int;           (void)Val2Int;           \
  const Obj& Val2Int64         = mVal2Int64;         (void)Val2Int64;         \
  const Obj& Val2Double        = mVal2Double;        (void)Val2Double;        \
  const Obj& Val2String        = mVal2String;        (void)Val2String;        \
  const Obj& Val2Datetime      = mVal2Datetime;      (void)Val2Datetime;      \
  const Obj& Val2Date          = mVal2Date;          (void)Val2Date;          \
  const Obj& Val2Time          = mVal2Time;          (void)Val2Time;          \
  const Obj& Val2CharArray     = mVal2CharArray;     (void)Val2CharArray;     \
  const Obj& Val2IntArray      = mVal2IntArray;      (void)Val2IntArray;      \
  const Obj& Val2Int64Array    = mVal2Int64Array;    (void)Val2Int64Array;    \
  const Obj& Val2DoubleArray   = mVal2DoubleArray;   (void)Val2DoubleArray;   \
  const Obj& Val2StringArray   = mVal2StringArray;   (void)Val2StringArray;   \
  const Obj& Val2DatetimeArray = mVal2DatetimeArray; (void)Val2DatetimeArray; \
  const Obj& Val2DateArray     = mVal2DateArray;     (void)Val2DateArray;     \
  const Obj& Val2TimeArray     = mVal2TimeArray;     (void)Val2TimeArray;     \

// ============================================================================
//                              HELPER FUNCTIONS
// ----------------------------------------------------------------------------

namespace {
namespace u {

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

void setValueDAB(Obj *objPtr, char cfg, int offset = 0)
    // Set the value of the object at the specified 'objPtr' to one of three
    // values as indicated by the specified 'cfg'.  Optionally specify an
    // 'offset' from 'objPtr->type()' for the type of the value being set.  The
    // three allowed values for 'cfg' are:
    //
    //: 'D': The default value for 'objPtr->type()'.
    //: 'A': The 'ValueA'      for 'objPtr->type()'.
    //: 'B': The 'ValueB'      for 'objPtr->type()'.
    //
    // The behavior is undefined if 'cfg' has a value other that 'D', 'A', or
    // 'B', or if 'Ot::e_VOID == objPtr->type()' when '0 != offset'.
{
    ///Implementation Note
    ///-------------------
    // Preconditions are checked using 'ASSERT' instead of 'BSLS_ASSERT*' to
    // guarantee that there are no misleading exceptions thrown when this
    // function is used in negative tests.

    ASSERT(objPtr);
    ASSERT(Ot::e_VOID != objPtr->type() || 0 == offset);
    ASSERT('D' == cfg
        || 'A' == cfg
        || 'B' == cfg);

    Ot::Enum type = shiftType(objPtr->type(), offset);

    switch (type) {
      case Ot::e_VOID: {
        ASSERT(!"Reached");
      } break;
      case Ot::e_BOOL: {
        switch (cfg) {
          case 'D': {
              objPtr->set(Ot::Bool());
          } break;
          case 'A': {
              objPtr->set(ValueA<Ot::Bool>::s_value);
          } break;
          case 'B': {
              objPtr->set(ValueB<Ot::Bool>::s_value);
          } break;
        }
      } break;
      case Ot::e_CHAR: {
        switch (cfg) {
          case 'D': {
              objPtr->set(Ot::Char());
          } break;
          case 'A': {
              objPtr->set(ValueA<Ot::Char>::s_value);
          } break;
          case 'B': {
              objPtr->set(ValueB<Ot::Char>::s_value);
          } break;
        }
      } break;
      case Ot::e_INT: {
        switch (cfg) {
          case 'D': {
              objPtr->set(Ot::Int());
          } break;
          case 'A': {
              objPtr->set(ValueA<Ot::Int>::s_value);
          } break;
          case 'B': {
              objPtr->set(ValueB<Ot::Int>::s_value);
          } break;
        }
      } break;
      case Ot::e_INT64: {
        switch (cfg) {
          case 'D': {
              objPtr->set(Ot::Int64());
          } break;
          case 'A': {
              objPtr->set(ValueA<Ot::Int64>::s_value);
          } break;
          case 'B': {
              objPtr->set(ValueB<Ot::Int64>::s_value);
          } break;
        }
      } break;
      case Ot::e_DOUBLE: {
        switch (cfg) {
          case 'D': {
              objPtr->set(Ot::Double());
          } break;
          case 'A': {
              objPtr->set(ValueA<Ot::Double>::s_value);
          } break;
          case 'B': {
              objPtr->set(ValueB<Ot::Double>::s_value);
          } break;
        }
      } break;
      case Ot::e_STRING: {
        switch (cfg) {
          case 'D': {
              objPtr->set(Ot::String());
          } break;
          case 'A': {
              objPtr->set(ValueA<Ot::String>::s_value);
          } break;
          case 'B': {
              objPtr->set(ValueB<Ot::String>::s_value);
          } break;
        }
      } break;
      case Ot::e_DATETIME: {
        switch (cfg) {
          case 'D': {
              objPtr->set(Ot::Datetime());
          } break;
          case 'A': {
              objPtr->set(ValueA<Ot::Datetime>::s_value);
          } break;
          case 'B': {
              objPtr->set(ValueB<Ot::Datetime>::s_value);
          } break;
        }
      } break;
      case Ot::e_DATE: {
        switch (cfg) {
          case 'D': {
              objPtr->set(Ot::Date());
          } break;
          case 'A': {
              objPtr->set(ValueA<Ot::Date>::s_value);
          } break;
          case 'B': {
              objPtr->set(ValueB<Ot::Date>::s_value);
          } break;
        }
      } break;
      case Ot::e_TIME: {
        switch (cfg) {
          case 'D': {
              objPtr->set(Ot::Time());
          } break;
          case 'A': {
              objPtr->set(ValueA<Ot::Time>::s_value);
          } break;
          case 'B': {
              objPtr->set(ValueB<Ot::Time>::s_value);
          } break;
        }
      } break;
      case Ot::e_CHAR_ARRAY: {
        switch (cfg) {
          case 'D': {
              objPtr->set(Ot::CharArray());
          } break;
          case 'A': {
              objPtr->set(ValueA<Ot::CharArray>::s_value);
          } break;
          case 'B': {
              objPtr->set(ValueB<Ot::CharArray>::s_value);
          } break;
        }
      } break;
      case Ot::e_INT_ARRAY: {
        switch (cfg) {
          case 'D': {
              objPtr->set(Ot::IntArray());
          } break;
          case 'A': {
              objPtr->set(ValueA<Ot::IntArray>::s_value);
          } break;
          case 'B': {
              objPtr->set(ValueB<Ot::IntArray>::s_value);
          } break;
        }
      } break;
      case Ot::e_INT64_ARRAY: {
        switch (cfg) {
          case 'D': {
              objPtr->set(Ot::Int64Array());
          } break;
          case 'A': {
              objPtr->set(ValueA<Ot::Int64Array>::s_value);
          } break;
          case 'B': {
              objPtr->set(ValueB<Ot::Int64Array>::s_value);
          } break;
        }
      } break;
      case Ot::e_DOUBLE_ARRAY: {
        switch (cfg) {
          case 'D': {
              objPtr->set(Ot::DoubleArray());
          } break;
          case 'A': {
              objPtr->set(ValueA<Ot::DoubleArray>::s_value);
          } break;
          case 'B': {
              objPtr->set(ValueB<Ot::DoubleArray>::s_value);
          } break;
        }
      } break;
      case Ot::e_STRING_ARRAY: {
        switch (cfg) {
          case 'D': {
              objPtr->set(Ot::StringArray());
          } break;
          case 'A': {
              objPtr->set(ValueA<Ot::StringArray>::s_value);
          } break;
          case 'B': {
              objPtr->set(ValueB<Ot::StringArray>::s_value);
          } break;
        }
      } break;
      case Ot::e_DATETIME_ARRAY: {
        switch (cfg) {
          case 'D': {
              objPtr->set(Ot::DatetimeArray());
          } break;
          case 'A': {
              objPtr->set(ValueA<Ot::DatetimeArray>::s_value);
          } break;
          case 'B': {
              objPtr->set(ValueB<Ot::DatetimeArray>::s_value);
          } break;
        }
      } break;
      case Ot::e_DATE_ARRAY: {
        switch (cfg) {
          case 'D': {
              objPtr->set(Ot::DateArray());
          } break;
          case 'A': {
              objPtr->set(ValueA<Ot::DateArray>::s_value);
          } break;
          case 'B': {
              objPtr->set(ValueB<Ot::DateArray>::s_value);
          } break;
        }
      } break;
      case Ot::e_TIME_ARRAY: {
        switch (cfg) {
          case 'D': {
              objPtr->set(Ot::TimeArray());
          } break;
          case 'A': {
              objPtr->set(ValueA<Ot::TimeArray>::s_value);
          } break;
          case 'B': {
              objPtr->set(ValueB<Ot::TimeArray>::s_value);
          } break;
        }
      } break;
    }
}

template <class TYPE>
bool checkValueDAB()
    // Return 'true' if the three values for the template parameter 'TYPE':
    //: o 'TYPE()
    //: o 'ValueA<TYPE>::s_value'
    //: o 'ValueB<TYPE>::s_value'
    // are distinct from each other, and 'false' otherwise.  The classes
    // 'ValueA<TYPE>' and 'ValueB<TYPE>' are defined at file scope.
{
    const TYPE D = TYPE();
    const TYPE A(ValueA<TYPE>::s_value);
    const TYPE B(ValueB<TYPE>::s_value);

    return D != A
        && D != B
        && A != B;
}

template <>
bool checkValueDAB<bool>()
    // Return 'true' if the two values:
    //: o 'ValueA<bool>::s_value'
    //: o 'ValueB<bool>::s_value'
    // are distinct from each other and 'bool() == ValueB<bool>::s_value', and
    // 'false' otherwise.  The classes 'ValueA<bool>' and 'ValueB<bool>' are
    // defined at file scope.
{
    const bool D = bool();
    const bool A(ValueA<bool>::s_value);
    const bool B(ValueB<bool>::s_value);

    return D != A
        && D == B   // only two distinct values of 'bool'
        && A != B;
}

bool checkAllValuesDAB()
    // Return 'true' if the function template 'checkValueDAB' returns 'true'
    // for every option type specified in 'balcl::OptionType', and 'false'
    // otherwise.
{
    return checkValueDAB<Ot::Bool>()
        && checkValueDAB<Ot::Char>()
        && checkValueDAB<Ot::Int>()
        && checkValueDAB<Ot::Int64>()
        && checkValueDAB<Ot::Double>()
        && checkValueDAB<Ot::String>()
        && checkValueDAB<Ot::Datetime>()
        && checkValueDAB<Ot::Date>()
        && checkValueDAB<Ot::Time>()
        && checkValueDAB<Ot::CharArray>()
        && checkValueDAB<Ot::IntArray>()
        && checkValueDAB<Ot::Int64Array>()
        && checkValueDAB<Ot::DoubleArray>()
        && checkValueDAB<Ot::StringArray>()
        && checkValueDAB<Ot::DatetimeArray>()
        && checkValueDAB<Ot::DateArray>()
        && checkValueDAB<Ot::TimeArray>();
}

bool hasValueDAB(const Obj& obj, char cfg, int offset = 0)

    // Return 'true' if the specified 'obj' has a value corresponding to the
    // value associated with the specified 'cfg', and 'false' otherwise.
    // Optionally specify an 'offset' from 'obj.type()' for the type of the
    // value sought from 'obj'.  The three allowed values for 'cfg' are:
    //
    //: 'D': The default value for 'objPtr->type()'.
    //: 'A': The 'ValueA'      for 'objPtr->type()'.
    //: 'B': The 'ValueB'      for 'objPtr->type()'.
    //
    // The behavior is undefined if 'cfg' has a value other that 'D', 'A', or
    // 'B', or if 'Ot::e_VOID == obj.type()' when '0 != offset'.
{
    ///Implementation Note
    ///-------------------
    // Preconditions are checked using 'ASSERT' instead of 'BSLS_ASSERT*' to
    // guarantee that there are no misleading exceptions thrown when this
    // function is used in negative tests.

    ASSERT(Ot::e_VOID != obj.type() || 0 == offset);
    ASSERT('D' == cfg
        || 'A' == cfg
        || 'B' == cfg);

    Ot::Enum type = shiftType(obj.type(), offset);

    switch (type) {
      case Ot::e_VOID: {
        ASSERT(!"Reached");
      } break;
      case Ot::e_BOOL: {
        switch (cfg) {
          case 'D': {
            return Ot::Bool() == obj.the<Ot::Bool>();                 // RETURN
          } break;
          case 'A': {
            return ValueA<Ot::Bool>::s_value == obj.the<Ot::Bool>();  // RETURN
          } break;
          case 'B': {
            return ValueB<Ot::Bool>::s_value == obj.the<Ot::Bool>();  // RETURN
          } break;
        }
      } break;
      case Ot::e_CHAR: {
        switch (cfg) {
          case 'D': {
            return Ot::Char() == obj.the<Ot::Char>();                 // RETURN
          } break;
          case 'A': {
            return ValueA<Ot::Char>::s_value == obj.the<Ot::Char>();  // RETURN
          } break;
          case 'B': {
            return ValueB<Ot::Char>::s_value == obj.the<Ot::Char>();  // RETURN
          } break;
        }
      } break;
      case Ot::e_INT: {
        switch (cfg) {
          case 'D': {
            return Ot::Int() == obj.the<Ot::Int>();                   // RETURN
          } break;
          case 'A': {
            return ValueA<Ot::Int>::s_value == obj.the<Ot::Int>();    // RETURN
          } break;
          case 'B': {
            return ValueB<Ot::Int>::s_value == obj.the<Ot::Int>();    // RETURN
          } break;
        }
      } break;
      case Ot::e_INT64: {
        switch (cfg) {
          case 'D': {
            return Ot::Int64() == obj.the<Ot::Int64>();               // RETURN
          } break;
          case 'A': {
            return ValueA<Ot::Int64>::s_value == obj.the<Ot::Int64>();// RETURN
          } break;
          case 'B': {
            return ValueB<Ot::Int64>::s_value == obj.the<Ot::Int64>();// RETURN
          } break;
        }
      } break;
      case Ot::e_DOUBLE: {
        switch (cfg) {
          case 'D': {
            return Ot::Double() == obj.the<Ot::Double>();             // RETURN
          } break;
          case 'A': {
            return ValueA<Ot::Double>::s_value == obj.the<Ot::Double>();
                                                                      // RETURN
          } break;
          case 'B': {
            return ValueB<Ot::Double>::s_value == obj.the<Ot::Double>();
                                                                      // RETURN
          } break;
        }
      } break;
      case Ot::e_STRING: {
        switch (cfg) {
          case 'D': {
            return Ot::String() == obj.the<Ot::String>();             // RETURN
          } break;
          case 'A': {
            return ValueA<Ot::String>::s_value == obj.the<Ot::String>();
                                                                      // RETURN
          } break;
          case 'B': {
            return ValueB<Ot::String>::s_value == obj.the<Ot::String>();
                                                                      // RETURN
          } break;
        }
      } break;
      case Ot::e_DATETIME: {
        switch (cfg) {
          case 'D': {
            return Ot::Datetime() == obj.the<Ot::Datetime>();         // RETURN
          } break;
          case 'A': {
            return ValueA<Ot::Datetime>::s_value == obj.the<Ot::Datetime>();
                                                                      // RETURN
          } break;
          case 'B': {
            return ValueB<Ot::Datetime>::s_value == obj.the<Ot::Datetime>();
                                                                      // RETURN
          } break;
        }
      } break;
      case Ot::e_DATE: {
        switch (cfg) {
          case 'D': {
            return Ot::Date() == obj.the<Ot::Date>();                 // RETURN
          } break;
          case 'A': {
            return ValueA<Ot::Date>::s_value == obj.the<Ot::Date>();  // RETURN
          } break;
          case 'B': {
            return ValueB<Ot::Date>::s_value == obj.the<Ot::Date>();  // RETURN
          } break;
        }
      } break;
      case Ot::e_TIME: {
        switch (cfg) {
          case 'D': {
            return Ot::Time() == obj.the<Ot::Time>();                 // RETURN
          } break;
          case 'A': {
            return ValueA<Ot::Time>::s_value == obj.the<Ot::Time>();  // RETURN
          } break;
          case 'B': {
            return ValueB<Ot::Time>::s_value == obj.the<Ot::Time>();  // RETURN
          } break;
        }
      } break;
      case Ot::e_CHAR_ARRAY: {
        switch (cfg) {
          case 'D': {
            return Ot::CharArray() == obj.the<Ot::CharArray>();       // RETURN
          } break;
          case 'A': {
            return ValueA<Ot::CharArray>::s_value == obj.the<Ot::CharArray>();
                                                                      // RETURN
          } break;
          case 'B': {
            return ValueB<Ot::CharArray>::s_value == obj.the<Ot::CharArray>();
                                                                      // RETURN
          } break;
        }
      } break;
      case Ot::e_INT_ARRAY: {
        switch (cfg) {
          case 'D': {
            return Ot::IntArray() == obj.the<Ot::IntArray>();         // RETURN
          } break;
          case 'A': {
            return ValueA<Ot::IntArray>::s_value == obj.the<Ot::IntArray>();
                                                                      // RETURN
          } break;
          case 'B': {
            return ValueB<Ot::IntArray>::s_value == obj.the<Ot::IntArray>();
                                                                      // RETURN
          } break;
        }
      } break;
      case Ot::e_INT64_ARRAY: {
        switch (cfg) {
          case 'D': {
            return Ot::Int64Array() == obj.the<Ot::Int64Array>();     // RETURN
          } break;
          case 'A': {
            return ValueA<Ot::Int64Array>::s_value ==
                                                     obj.the<Ot::Int64Array>();
                                                                      // RETURN
          } break;
          case 'B': {
            return ValueB<Ot::Int64Array>::s_value ==
                                                     obj.the<Ot::Int64Array>();
                                                                      // RETURN
          } break;
        }
      } break;
      case Ot::e_DOUBLE_ARRAY: {
        switch (cfg) {
          case 'D': {
            return Ot::DoubleArray() == obj.the<Ot::DoubleArray>();   // RETURN
          } break;
          case 'A': {
            return ValueA<Ot::DoubleArray>::s_value ==
                                                    obj.the<Ot::DoubleArray>();
                                                                      // RETURN
          } break;
          case 'B': {
            return ValueB<Ot::DoubleArray>::s_value ==
                                                    obj.the<Ot::DoubleArray>();
                                                                      // RETURN
          } break;
        }
      } break;
      case Ot::e_STRING_ARRAY: {
        switch (cfg) {
          case 'D': {
            return Ot::StringArray() == obj.the<Ot::StringArray>();   // RETURN
          } break;
          case 'A': {
            return ValueA<Ot::StringArray>::s_value ==
                                                    obj.the<Ot::StringArray>();
                                                                      // RETURN
          } break;
          case 'B': {
            return ValueB<Ot::StringArray>::s_value ==
                                                    obj.the<Ot::StringArray>();
                                                                      // RETURN
          } break;
        }
      } break;
      case Ot::e_DATETIME_ARRAY: {
        switch (cfg) {
          case 'D': {
            return Ot::DatetimeArray() == obj.the<Ot::DatetimeArray>();
                                                                      // RETURN
          } break;
          case 'A': {
            return ValueA<Ot::DatetimeArray>::s_value
                                               == obj.the<Ot::DatetimeArray>();
                                                                      // RETURN
          } break;
          case 'B': {
            return ValueB<Ot::DatetimeArray>::s_value ==
                                                  obj.the<Ot::DatetimeArray>();
                                                                      // RETURN
          } break;
        }
      } break;
      case Ot::e_DATE_ARRAY: {
        switch (cfg) {
          case 'D': {
            return Ot::DateArray() == obj.the<Ot::DateArray>();       // RETURN
          } break;
          case 'A': {
            return ValueA<Ot::DateArray>::s_value == obj.the<Ot::DateArray>();
                                                                      // RETURN
          } break;
          case 'B': {
            return ValueB<Ot::DateArray>::s_value == obj.the<Ot::DateArray>();
                                                                      // RETURN
          } break;
        }
      } break;
      case Ot::e_TIME_ARRAY: {
        switch (cfg) {
          case 'D': {
            return Ot::TimeArray() == obj.the<Ot::TimeArray>();       // RETURN
          } break;
          case 'A': {
            return ValueA<Ot::TimeArray>::s_value == obj.the<Ot::TimeArray>();
                                                                      // RETURN
          } break;
          case 'B': {
            return ValueB<Ot::TimeArray>::s_value == obj.the<Ot::TimeArray>();
                                                                      // RETURN
          } break;
        }
      } break;
    }

    ASSERT(!"Reached");
    return false; // Suppress warning.
}

bslma::Allocator *getContainedAllocator(const Obj& obj)
    // Return the allocator of the specified 'obj'.  The behavior is undefined
    // unless 'OptionType::EnumToType<ENUM>::type' is an allocating type where
    // 'ENUM' matches 'obj.type()'.
{
    BSLS_ASSERT(Ot::e_VOID     != obj.type()
             && Ot::e_BOOL     != obj.type()
             && Ot::e_CHAR     != obj.type()
             && Ot::e_INT      != obj.type()
             && Ot::e_INT64    != obj.type()
             && Ot::e_DOUBLE   != obj.type()
             && Ot::e_DATETIME != obj.type()
             && Ot::e_DATE     != obj.type()
             && Ot::e_TIME     != obj.type());

    switch (obj.type()) {
      case Ot::e_VOID: {
        BSLS_ASSERT(!"Reached");
      } break;
      case Ot::e_BOOL: {
        BSLS_ASSERT(!"Reached");
      } break;
      case Ot::e_CHAR: {
        BSLS_ASSERT(!"Reached");
      } break;
      case Ot::e_INT: {
        BSLS_ASSERT(!"Reached");
      } break;
      case Ot::e_INT64: {
        BSLS_ASSERT(!"Reached");
      } break;
      case Ot::e_DOUBLE: {
        BSLS_ASSERT(!"Reached");
      } break;
      case Ot::e_DATETIME: {
        BSLS_ASSERT(!"Reached");
      } break;
      case Ot::e_DATE: {
        BSLS_ASSERT(!"Reached");
      } break;
      case Ot::e_TIME: {
        BSLS_ASSERT(!"Reached");
      } break;
      case Ot::e_STRING: {
        return obj.the<Ot::String>().get_allocator().mechanism();     // RETURN
      } break;
      case Ot::e_CHAR_ARRAY: {
        return obj.the<Ot::CharArray>().get_allocator().mechanism();  // RETURN
      } break;
      case Ot::e_INT_ARRAY: {
        return obj.the<Ot::IntArray>().get_allocator().mechanism();   // RETURN
      } break;
      case Ot::e_INT64_ARRAY: {
        return obj.the<Ot::Int64Array>().get_allocator().mechanism(); // RETURN
      } break;
      case Ot::e_DOUBLE_ARRAY: {
        return obj.the<Ot::DoubleArray>().get_allocator().mechanism();// RETURN
      } break;
      case Ot::e_STRING_ARRAY: {
        return obj.the<Ot::StringArray>().get_allocator().mechanism();// RETURN
      } break;
      case Ot::e_DATETIME_ARRAY: {
        return obj.the<Ot::DatetimeArray>().get_allocator().mechanism();
                                                                      // RETURN
      } break;
      case Ot::e_DATE_ARRAY: {
        return obj.the<Ot::DateArray>().get_allocator().mechanism();  // RETURN
      } break;
      case Ot::e_TIME_ARRAY: {
        return obj.the<Ot::TimeArray>().get_allocator().mechanism();  // RETURN
      } break;
    }

    BSLS_ASSERT(!"Reached");
    return 0; // Suppress warning.
}

bool checkAllocator(const Obj&        obj,
                    bool              isAllocatingType,
                    bslma::Allocator *expectedAllocator)
    // Return 'true' if the specified 'expectedAllocator' is properly
    // "installed" in the specified 'obj' according to the specified
    // 'isAllocatingType', and 'false' otherwise.  There are two requirements
    // for correctly "installed":
    //
    //: 1 The 'obj' should use 'expectedAllocator' (unconditionally).
    //:
    //: 2 If 'obj' has a value and the value's type 'isAllocatingType' (i.e.,
    //:   'Ot::e_STRING' and any of the array types) then that value must use
    //:   the 'expectedAllocator'.
{
    if (expectedAllocator != obj.allocator()) {
        return false;                                                 // RETURN
    }

    if (isAllocatingType) {

        if (expectedAllocator != getContainedAllocator(obj)) {
            return false;                                             // RETURN
        }
    }

    return true;
}

int checkPrint(const Obj& obj)
    // Return 0 if streaming the specified 'obj' seems correct, and a non-zero
    // value otherwise.  Correctness is decided based on several heuristics
    // that vary according to the state (unset or not), type, and null stater
    // (or not) of 'obj'.
{
    const Ot::Enum type = obj.type();

    bsl::ostringstream ossStream;

    ASSERT(&ossStream == &(ossStream << obj));  // ACTION

    if (veryVeryVerbose) {
        cout << "STREAM:"       << endl
             << "|"
             << ossStream.str()
             << "|"             << endl;
    }

    // Check length
    bsl::size_t streamLength = ossStream.str().size();

    if (Ot::e_VOID == type) {
        if (0 != streamLength) {
           return 1;                                                  // RETURN
        }
    } else if (Ot::e_STRING == type
            && false        == obj.isNull()
            && true         == u::hasValueDAB(obj, 'D')) {
        if (0 < streamLength) {
            return 2;                                                 // RETURN
        }
    } else if (0 == streamLength) {
        return 3;                                                     // RETURN
    }

    if (obj.hasNonVoidType() && obj.isNull()) {
        if ("NULL" != ossStream.str()) {
            return 4;                                                 // RETURN
        }
    } else if (Ot::isArrayType(type)) {
        const char expFirstChar = Ot::e_CHAR_ARRAY == type ? '"': '[';
        const char  expLastChar = Ot::e_CHAR_ARRAY == type ? '"': ']';

        const char firstChar = ossStream.str().c_str()[0];
        const char  lastChar = ossStream.str().c_str()[streamLength - 1];

        if (expFirstChar != firstChar) {
            return 5;                                                 // RETURN
        }
        if (expLastChar != lastChar) {
            return 6;                                                 // RETURN
        }
    }

    return 0;
}

}  // close namespace u
}  // close unnamed namespace

// ============================================================================
//                              TEMPLATE TEST FACILITY
// ----------------------------------------------------------------------------

#define ALL_SUPPORTED_TYPES                                                   \
        balcl::OptionType::Bool,                                              \
        balcl::OptionType::Char,                                              \
        balcl::OptionType::Int,                                               \
        balcl::OptionType::Int64,                                             \
        balcl::OptionType::Double,                                            \
        balcl::OptionType::String,                                            \
        balcl::OptionType::Datetime,                                          \
        balcl::OptionType::Date,                                              \
        balcl::OptionType::Time,                                              \
        balcl::OptionType::CharArray,                                         \
        balcl::OptionType::IntArray,                                          \
        balcl::OptionType::Int64Array,                                        \
        balcl::OptionType::DoubleArray,                                       \
        balcl::OptionType::StringArray,                                       \
        balcl::OptionType::DatetimeArray,                                     \
        balcl::OptionType::DateArray,                                         \
        balcl::OptionType::TimeArray
    // This macro lists all of the types supported by the class under test.

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

// ============================================================================
//                              TEST DRIVER TEMPLATE
// ----------------------------------------------------------------------------

// BDE_VERIFY pragma: -FABC01  // not in alphabetic order

                        // =================
                        // struct TestDriver
                        // =================

template <class TYPE>
struct TestDriver {
    static void testCase9();
        // Test modifiable access.

    static void testCase4();
        // Test value constructors

    static void testCase3();
        // Test default constructors, primary manipulators, basic accessors,
        // and the destructor;

    static void testCase1();
        // Breathing test.
};

                        // -----------------
                        // struct TestDriver
                        // -----------------

template <class TYPE>
void TestDriver<TYPE>::testCase9()
{
    Ot::Enum type = Ot::TypeToEnum<TYPE>::value;

    if (verbose) {
        cout << endl << "testCase9: ";
        P_(type)
        P(bsls::NameOf<TYPE>());
    }

    bool                 isAllocatingType =
                                        bslma::UsesBslmaAllocator<TYPE>::value;
    bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

    Obj mX(type, &sa);  const Obj& X = mX;

    mX.the<TYPE>() = ValueA<TYPE>::s_value;  // ACTION

    ASSERT(true  == X.hasNonVoidType());
    ASSERT(type  == X.type());
    ASSERT(false == X.isNull());
    ASSERT(true  == u::hasValueDAB(X, 'A'));
    ASSERT(true  == u::checkAllocator(X, isAllocatingType, &sa));

    mX.the<TYPE>() = ValueB<TYPE>::s_value;  // ACTION

    ASSERT(true  == X.hasNonVoidType());
    ASSERT(type  == X.type());
    ASSERT(false == X.isNull());
    ASSERT(true  == u::hasValueDAB(X, 'B'));
    ASSERT(true  == u::checkAllocator(X, isAllocatingType, &sa));

    mX.the<TYPE>() = TYPE();               // ACTION

    ASSERT(true  == X.hasNonVoidType());
    ASSERT(type  == X.type());
    ASSERT(false == X.isNull());
    ASSERT(true  == u::hasValueDAB(X, 'D'));
    ASSERT(true  == u::checkAllocator(X, isAllocatingType, &sa));

    if (verbose) cout << endl << "Negative Testing." << endl;
    {
        bsls::AssertTestHandlerGuard hG;

        {
            Obj mZ;  // unset state
            ASSERT_FAIL(mZ.the<TYPE>() = ValueA<TYPE>::s_value);

            mZ.setType(u::shiftType(type, -1));
            ASSERT_FAIL(mZ.the<TYPE>() = ValueA<TYPE>::s_value);

            mZ.setType(u::shiftType(type,  0));
            ASSERT_PASS(mZ.the<TYPE>() = ValueA<TYPE>::s_value);

            mZ.setType(u::shiftType(type,  1));
            ASSERT_FAIL(mZ.the<TYPE>() = ValueA<TYPE>::s_value);

            mZ.setNull();
            ASSERT_FAIL(mZ.the<TYPE>() = ValueA<TYPE>::s_value);
        }
    }
}

template <class TYPE>
void TestDriver<TYPE>::testCase4()
{
    Ot::Enum type = Ot::TypeToEnum<TYPE>::value;

    if (verbose) {
        cout << endl << "testCase4: ";
        P_(type)
        P(bsls::NameOf<TYPE>());
    }

    bool           isAllocatingType = bslma::UsesBslmaAllocator<TYPE>::value;
    const Ot::Enum ENUMERATOR       = Ot::TypeToEnum<TYPE>::value;

    const TYPE ARRAY[] = { TYPE()
                         , ValueA<TYPE>::s_value
                         , ValueB<TYPE>::s_value
                         };

    const bsl::size_t k_NUM_ELEMENTS = sizeof ARRAY / sizeof *ARRAY;

    for (bsl::size_t i = 0; i < k_NUM_ELEMENTS; ++i) {
        const TYPE VALUE = ARRAY[i];

        if (veryVerbose) {
            T_ P(i)
        }

        char LETTER = 0 == i ? 'D' :
                      1 == i ? 'A' :
                      2 == i ? 'B' :
                      /*else*/ -1  ;

        for (char cfg = 'a'; cfg <= 'f'; ++cfg) {
            const char CONFIG = cfg;  // how we specify the allocator

            if (veryVerbose) {
                T_ T_ P(CONFIG);
            }

            bslma::TestAllocator         fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator         sa("supplied",  veryVeryVeryVerbose);
            bslma::TestAllocator         da("default",   veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&da);

            Obj                  *objPtr          = 0;
            bslma::TestAllocator *objAllocatorPtr = 0;

            if (veryVeryVerbose) {
                cout << "\t\t" "VALUE CTOR" << endl;
            }

            switch (CONFIG) {
              case 'a': {
                objPtr = new (fa) Obj(VALUE);             // ACTION
                objAllocatorPtr = &da;
              } break;
              case 'b': {
                objPtr = new (fa) Obj(VALUE, 0);          // ACTION
                objAllocatorPtr = &da;
              } break;
              case 'c': {
                objPtr = new (fa) Obj(VALUE, &sa);        // ACTION
                objAllocatorPtr = &sa;
              } break;
              case 'd': {
                objPtr = new (fa) Obj(ENUMERATOR);        // ACTION
                objAllocatorPtr = &da;
              } break;
              case 'e': {
                objPtr = new (fa) Obj(ENUMERATOR, 0);     // ACTION
                objAllocatorPtr = &da;
              } break;
              case 'f': {
                objPtr = new (fa) Obj(ENUMERATOR, &sa);  // ACTION
                objAllocatorPtr = &sa;
              } break;
              default: {
                ASSERTV(CONFIG, !"Bad allocator config.");
              } break;
            }

            if ('c' < CONFIG) {
                LETTER = 'D';
            }

            Obj&                   mX = *objPtr;  const Obj& X = mX;
            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

            (void)noa;

            ASSERT(true  == X.hasNonVoidType());
            ASSERT(type  == X.type());
            ASSERT(false == X.isNull());
            ASSERT(true  == u::hasValueDAB(X, LETTER));
            ASSERT(true  == u::checkAllocator(X, isAllocatingType, &oa));

            fa.deleteObject(objPtr);
        }
    }
}

template <class TYPE>
void TestDriver<TYPE>::testCase3()
{
    Ot::Enum type = Ot::TypeToEnum<TYPE>::value;

    if (verbose) {
        cout << endl << "testCase3: ";
        P_(type)
        P(bsls::NameOf<TYPE>());
    }

    bool isAllocatingType = bslma::UsesBslmaAllocator<TYPE>::value;

    for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
        const char CONFIG = cfg;  // how we specify the allocator

        if (veryVerbose) {
            P(CONFIG);
        }

        bslma::TestAllocator         fa("footprint", veryVeryVeryVerbose);
        bslma::TestAllocator         sa("supplied",  veryVeryVeryVerbose);
        bslma::TestAllocator         da("default",   veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        Obj                  *objPtr          = 0;
        bslma::TestAllocator *objAllocatorPtr = 0;

        if (veryVeryVerbose) {
            cout << "\t\t" "DFLT CTOR" << endl;
        }

        switch (CONFIG) {
          case 'a': {
            objPtr = new (fa) Obj();  // ACTION
            objAllocatorPtr = &da;
          } break;
          case 'b': {
            objPtr = new (fa) Obj(&sa);
            objAllocatorPtr = &sa;
          } break;
          default: {
            ASSERTV(CONFIG, !"Bad allocator config.");
          } break;
        }

        Obj&                   mX = *objPtr;  const Obj& X = mX;
        bslma::TestAllocator&  oa = *objAllocatorPtr;
        bslma::TestAllocator& noa = 'a' == CONFIG ? sa : da;

        (void)noa;

        ASSERT(false      == X.hasNonVoidType());
        ASSERT(Ot::e_VOID == X.type());
        ASSERT(0          == u::checkPrint(X));

        if (veryVeryVerbose) {
            cout << "\t\t" "setType" << ": "     << type
                 <<        " [" << static_cast<int>(type) << "]" << endl;
        }
        {
            mX.setType(type);

            ASSERT(true  == X.hasNonVoidType());
            ASSERT(type  == X.type());
            ASSERT(false == X.isNull());
            ASSERT(true  == u::hasValueDAB(X, 'D'));
            ASSERT(true  == u::checkAllocator(X, isAllocatingType, &oa));
            ASSERT(0     == u::checkPrint(X));
        }

        if (veryVeryVerbose) {
            cout << "\t\t" "setValue: A" << endl;
        }
        {
            u::setValueDAB(&mX, 'A');

            ASSERT(true  == X.hasNonVoidType());
            ASSERT(type  == X.type());
            ASSERT(false == X.isNull());
            ASSERT(true  == u::hasValueDAB(X, 'A'));
            ASSERT(true  == u::checkAllocator(X, isAllocatingType, &oa));
            ASSERT(0     == u::checkPrint(X));
        }

        if (veryVeryVerbose) {
            cout << "\t\t" "setValue: B" << endl;
        }
        {
            u::setValueDAB(&mX, 'B');

            ASSERT(true  == X.hasNonVoidType());
            ASSERT(type  == X.type());
            ASSERT(false == X.isNull());
            ASSERT(true  == u::hasValueDAB(X, 'B'));
            ASSERT(true  == u::checkAllocator(X, isAllocatingType, &oa));
            ASSERT(0     == u::checkPrint(X));
        }

        if (veryVeryVerbose) {
            cout << "\t\t" "setValue: D" << endl;
        }
        {
            u::setValueDAB(&mX, 'D');

            ASSERT(true  == X.hasNonVoidType());
            ASSERT(type  == X.type());
            ASSERT(false == X.isNull());
            ASSERT(true  == u::hasValueDAB(X, 'D'));
            ASSERT(true  == u::checkAllocator(X, isAllocatingType, &oa));
            ASSERT(0     == u::checkPrint(X));
        }

        if (veryVeryVerbose) {
            cout << "\t\t" "setNull" << endl;
        }
        {
            mX.setNull();

            ASSERT(true  == X.hasNonVoidType());
            ASSERT(type  == X.type());
            ASSERT(true  == X.isNull());
            ASSERT(0     == u::checkPrint(X));
                // That is all we can check without a value.
        }

        if (veryVeryVerbose) {
            cout << "\t\t" "setValue: A (second time)" << endl;
        }
        {
            u::setValueDAB(&mX, 'A');

            ASSERT(true  == X.hasNonVoidType());
            ASSERT(type  == X.type());
            ASSERT(false == X.isNull());
            ASSERT(true  == u::hasValueDAB(X, 'A'));
            ASSERT(true  == u::checkAllocator(X, isAllocatingType, &oa));
            ASSERT(0     == u::checkPrint(X));
        }

        if (veryVeryVerbose) {
            cout << "\t\t" "setType" << ": "     << Ot::e_VOID
                 <<        " (" << static_cast<int>(Ot::e_VOID) << ")" << endl;
        }
        {
            mX.setType(Ot::e_VOID);

            ASSERT(false       == X.hasNonVoidType());
            ASSERT(Ot::e_VOID  == X.type());
            ASSERT(0           == u::checkPrint(X));
        }

        if (veryVeryVerbose) {
            cout << "\t\t" "setType" << ": "     << type
                 <<        " [" << static_cast<int>(type) << "]"
                           " (second time)" << endl;
        }
        {
            mX.setType(type);

            ASSERT(true  == X.hasNonVoidType());
            ASSERT(type  == X.type());
            ASSERT(false == X.isNull());
            ASSERT(true  == u::hasValueDAB(X, 'D'));
            ASSERT(true  == u::checkAllocator(X, isAllocatingType, &oa));
            ASSERT(0     == u::checkPrint(X));
        }

        if (veryVeryVerbose) {
            cout << "\t\t" "setValue: B (second time)" << endl;
        }

        {
            u::setValueDAB(&mX, 'B');

            ASSERT(true  == X.hasNonVoidType());
            ASSERT(type  == X.type());
            ASSERT(false == X.isNull());
            ASSERT(true  == u::hasValueDAB(X, 'B'));
            ASSERT(true  == u::checkAllocator(X, isAllocatingType, &oa));
            ASSERT(0     == u::checkPrint(X));
        }

        if (veryVeryVerbose) {
            cout << "\t\t" "reset" << endl;
        }

        mX.reset();

        ASSERT(false      == X.hasNonVoidType());
        ASSERT(Ot::e_VOID == X.type());
        ASSERT(0          == u::checkPrint(X));

        if (veryVeryVerbose) {
            cout << "\t\t" "delete" << endl;
        }

        fa.deleteObject(objPtr);
    }

    if (verbose) cout << endl << "Negative Testing." << endl;
    {
        bsls::AssertTestHandlerGuard hG;

        if (veryVerbose) cout << endl << "The 'isNull'/'setNull' methods."
                              << endl;
        {
            Obj mX; const Obj& X = mX;

            ASSERT_FAIL( X. isNull());
            ASSERT_FAIL(mX.setNull());

            mX.setType(type);

            ASSERT_PASS( X. isNull());
            ASSERT_PASS(mX.setNull());
        }
        if (veryVerbose) cout << endl << "The 'set' methods." << endl;
        {
            Obj mX;

            ASSERT_FAIL(u::setValueDAB(&mX, 'A',  0));

            mX.setType(type);

            ASSERT_FAIL(u::setValueDAB(&mX, 'A', -1));
            ASSERT_PASS(u::setValueDAB(&mX, 'A',  0));
            ASSERT_FAIL(u::setValueDAB(&mX, 'A',  1));
        }

        if (veryVerbose) cout << endl << "The 'the' methods." << endl;
        {
            Obj mX; const Obj& X = mX;

            ASSERT_FAIL(u::hasValueDAB(X, 'A',  0));

            mX.setType(type);
            u::setValueDAB(&mX, 'B');

            ASSERT_FAIL(u::hasValueDAB(X, 'B', -1));
            ASSERT_PASS(u::hasValueDAB(X, 'B',  0));
            ASSERT_FAIL(u::hasValueDAB(X, 'B',  1));

            mX.setNull();

            ASSERT_FAIL(u::hasValueDAB(X, 'B',  0));
        }
    }
}

template <class TYPE>
void TestDriver<TYPE>::testCase1()
{

    Ot::Enum type = Ot::TypeToEnum<TYPE>::value;

    if (verbose) {
        cout << endl << "testCase1: ";
        P_(type)
        P(bsls::NameOf<TYPE>());
    }

    const TYPE D = TYPE();  // default value;
    const TYPE A = ValueA<TYPE>::s_value;

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (verbose) cout << "\n 1. Create an object 'w' (enum ctor)."
                         "\t\t{ w:D             }" << endl;

    Obj mW(type);  const Obj& W = mW;

    if (verbose) cout << "\ta. Check initial value of 'w'." << endl;
    if (veryVeryVerbose) { T_ T_ P(W) }

    ASSERT(true == W.hasNonVoidType());
    ASSERT(type == W.type());
    ASSERT(D    == W.the<TYPE>());

    if (verbose) cout <<
              "\tb. Try equality operators: 'w' <op> 'w'." << endl;

    ASSERT(1 == (W == W));        ASSERT(0 == (W != W));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (verbose) cout << "\n 2. Create an object 'x' (copy from 'w')."
                         "\t\t{ w:D x:D         }" << endl;

    Obj mX(W);  const Obj& X = mX;

    if (verbose) cout << "\ta. Check initial value of 'x'." << endl;
    if (veryVeryVerbose) { T_ T_ P(X) }

    ASSERT(true == X.hasNonVoidType());
    ASSERT(type == X.type());
    ASSERT(D    == X.the<TYPE>());

    if (verbose) cout <<
               "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

    ASSERT(1 == (X == W));        ASSERT(0 == (X != W));
    ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (verbose) cout << "\n 3. Set 'x' to 'A' (value distinct from 'D')."
                         "\t\t{ w:D x:A         }" << endl;

    mX.set(A);

    if (verbose) cout << "\ta. Check new value of 'x'." << endl;
    if (veryVeryVerbose) { T_ T_ P(X) }

    ASSERT(A == X.the<TYPE>());

    if (verbose) cout <<
         "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

    ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
    ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (verbose) cout << "\n 4. Create an object 'y' (init. to 'A')."
                         "\t\t{ w:D x:A y:A     }" << endl;

    Obj mY(A);  const Obj& Y = mY;

    if (verbose) cout << "\ta. Check initial value of 'y'." << endl;
    if (veryVeryVerbose) { T_ T_ P(Y) }

    ASSERT(A == Y.the<TYPE>());

    if (verbose) cout <<
         "\tb. Try equality operators: 'y' <op> 'w', 'x', 'y'" << endl;

    ASSERT(0 == (Y == W));        ASSERT(1 == (Y != W));
    ASSERT(1 == (Y == X));        ASSERT(0 == (Y != X));
    ASSERT(1 == (Y == Y));        ASSERT(0 == (Y != Y));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (verbose) cout << "\n 5. Create an object 'z' (copy from 'y')."
                         "\t\t{ w:D x:A y:A z:A }" << endl;

    Obj mZ(Y);  const Obj& Z = mZ;

    if (verbose) cout << "\ta. Check initial value of 'z'." << endl;
    if (veryVeryVerbose) { T_ T_ P(Z) }

    ASSERT(A == Z.the<TYPE>());

    if (verbose) cout <<
        "\tb. Try equality operators: 'z' <op> 'w', 'x', 'y', 'z'." << endl;

    ASSERT(0 == (Z == W));        ASSERT(1 == (Z != W));
    ASSERT(1 == (Z == X));        ASSERT(0 == (Z != X));
    ASSERT(1 == (Z == Y));        ASSERT(0 == (Z != Y));
    ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (verbose) cout << "\n 6. Set 'z' to 'D' (the default value)."
                         "\t\t\t{ w:D x:A y:A z:D }" << endl;

    mZ.set(D);

    if (verbose) cout << "\ta. Check new value of 'z'." << endl;
    if (veryVeryVerbose) { T_ T_ P(Z) }

    ASSERT(D == Z.the<TYPE>());

    if (verbose) cout <<
        "\tb. Try equality operators: 'z' <op> 'w', 'x', 'y', 'z'." << endl;

    ASSERT(1 == (Z == W));        ASSERT(0 == (Z != W));
    ASSERT(0 == (Z == X));        ASSERT(1 == (Z != X));
    ASSERT(0 == (Z == Y));        ASSERT(1 == (Z != Y));
    ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (verbose) cout << "\n 7. Assign 'w' from 'x'."
                         "\t\t\t\t{ w:A x:A y:A z:D }" << endl;
    mW = X;

    if (verbose) cout << "\ta. Check new value of 'w'." << endl;
    if (veryVeryVerbose) { T_ T_ P(W) }

    ASSERT(A == W.the<TYPE>());

    if (verbose) cout <<
        "\tb. Try equality operators: 'w' <op> 'w', 'x', 'y', 'z'." << endl;

    ASSERT(1 == (W == W));        ASSERT(0 == (W != W));
    ASSERT(1 == (W == X));        ASSERT(0 == (W != X));
    ASSERT(1 == (W == Y));        ASSERT(0 == (W != Y));
    ASSERT(0 == (W == Z));        ASSERT(1 == (W != Z));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (verbose) cout << "\n 8. Assign 'w' from 'z'."
                         "\t\t\t\t{ w:D x:A y:A z:D }" << endl;
    mW = Z;

    if (verbose) cout << "\ta. Check new value of 'w'." << endl;
    if (veryVeryVerbose) { T_ T_ P(W) }

    ASSERT(D == W.the<TYPE>());

    if (verbose) cout <<
        "\tb. Try equality operators: 'x' <op> 'w', 'x', 'y', 'z'." << endl;

    ASSERT(1 == (W == W));        ASSERT(0 == (W != W));
    ASSERT(0 == (W == X));        ASSERT(1 == (W != X));
    ASSERT(0 == (W == Y));        ASSERT(1 == (W != Y));
    ASSERT(1 == (W == Z));        ASSERT(0 == (W != Z));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (verbose) cout << "\n 9. Assign 'x' from 'x' (aliasing)."
                         "\t\t\t{ w:D x:A y:A z:D }" << endl;
    mX = X;

    if (verbose) cout << "\ta. Check (same) value of 'x'." << endl;
    if (veryVeryVerbose) { T_ T_ P(X) }

    ASSERT(A == X.the<TYPE>());

    if (verbose) cout <<
        "\tb. Try equality operators: 'x' <op> 'w', 'x', 'y', 'z'." << endl;

    ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
    ASSERT(1 == (X == X));        ASSERT(0 == (X != X));
    ASSERT(1 == (X == Y));        ASSERT(0 == (X != Y));
    ASSERT(0 == (X == Z));        ASSERT(1 == (X != Z));
}

// BDE_VERIFY pragma: +FABC01  // not in alphabetic order

// ============================================================================
//                              USAGE EXAMPLES
// ----------------------------------------------------------------------------

// BDE_VERIFY pragma: -FABC01  // not in alphabetic order
// BDE_VERIFY pragma: -FD01:   // Function declaration requires contract

namespace example1 {
int main() {
// Do not show line above in header.

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of 'balcl::OptionValue'
/// - - - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to create and use a
// 'balcl::OptionValue' object.  Note that 'balcl::OptionValue' objects are
// typically used in a description of a sequence of command-line options (see
// {'balcl_optiontype'}).
//
// First, we create a default 'balcl::OptionValue', 'valueA', and observe that
// it is in the unset state:
//..
    balcl::OptionValue valueA;

    ASSERT(false                     == valueA.hasNonVoidType());
    ASSERT(balcl::OptionType::e_VOID == valueA.type());
//..
// Next, we create a second 'balcl::OptionValue' having the value 5, and then
// confirm its value and observe that it does not compare equal to the
// 'valueA':
//..
    balcl::OptionValue valueB(5);

    ASSERT(true                     == valueB.hasNonVoidType());
    ASSERT(balcl::OptionType::e_INT == valueB.type());
    ASSERT(5                        == valueB.the<int>());

    ASSERT(valueA != valueB);
//..
// Then, we call the 'reset' method of 'valueB' resetting it to the unset
// state, and observe that 'valueA' now compares equal to 'valueB':
//..
    valueB.reset();

    ASSERT(valueA == valueB);
//..
// Now, we change the type of 'valueA' so that it can be hold a 'double' value:
//..
    valueA.setType(balcl::OptionType::e_DOUBLE);
    ASSERT(true                        == valueA.hasNonVoidType());
    ASSERT(balcl::OptionType::e_DOUBLE == valueA.type());
    ASSERT(double()                    == valueA.the<double>());

    valueA.set(6.0);
    ASSERT(6.0                         == valueA.the<double>());
//..
// Finally, we set the object to the null state.  Notice that the type of that
// value is not changed:
//..
    valueA.setNull();
    ASSERT(true                        == valueA.isNull());
    ASSERT(balcl::OptionType::e_DOUBLE == valueA.type());
//..

    // Do not show line below in header.
    return 0;
}
}  // close namespace example1

namespace example2 {

static void setOutputDir(const char *)
{
}

static void setVerbosityLevel(int)
{
}

static void setCaseInsensitivityFlag(bool)
{
}

typedef struct MyOptionDescription {
    int d_arbitraryData;
} MyOptionDescription;

enum { NUM_OPTIONS = 1 }; // any non-zero value

// Do not show code above in the header file.

//
///Example 2: Interpreting Option Parser Results
///- - - - - - - - - - - - - - - - - - - - - - -
// Command-line options have values of many different types (e.g., 'int',
// 'double', string, date) or their values may not be specified -- after all,
// some command-line options may be *optional*.  The 'balcl::OptionValue' class
// can be used to represent such values.
//
// First, we define 'MyCommandLineParser', a simple command-line argument
// parser.  This class accepts a description (e.g., option name, value type) of
// allowable options on construction and provides a 'parse' method that accepts
// 'argc' and 'argv', the values made available (by the operating system) to
// 'main':
//..
                        // =========================
                        // class MyCommandLineParser
                        // =========================

    class MyCommandLineParser {
        // ...

      public:
        // CREATORS
        MyCommandLineParser(const MyOptionDescription *descriptions,
                            bsl::size_t                count);
            // Create an object that can parse command-line arguments that
            // satisfy the specified 'descriptions', an array containing the
            // specified 'count' elements.

        // ...

        // MANIPULATORS
        int parse(int argc, const char **argv);
            // Parse the command-line options in the specified 'argv', an array
            // having the specified 'argc' elements.  Return 0 on success --
            // i.e., the options were compatible with the option descriptions
            // specified on construction -- and a non-zero value otherwise.

        // ...
//..
// After a successful call to the 'parse' method, the results are available by
// several accessors.  Note that the 'index' of a result corresponds to the
// index of that option in the description provided on construction:
//..
        // ACCESSORS
        bool isParsed() const;
            // Return 'true' if the most recent call to 'parsed' was successful
            // and 'false' otherwise.

        const char *name (bsl::size_t index) const;
            // Return of the name of the parsed option at the specified 'index'
            // position.  The behavior is undefined unless
            // '0 <= index < numOptions()' and 'true == isParsed()'

        const balcl::OptionValue& value(bsl::size_t index) const;
            // Return a 'const' reference to the value (possibly in a null
            // state) of the parsed option at the specified 'index' position.
            // The behavior is undefined unless '0 <= index < numOptions()' and
            // 'true == isParsed()'.

        bsl::size_t numOptions() const;
            // Return the number of parsed options.  The behavior is undefined
            // unless 'true == isParsed()'.

        // ...
    };
//..
// Note that neither our option description nor our parser support the concept
// of default values for options that are not entered on the command line.
//
// Then, we create a description having three allowable options (elided), a
// parser object, and invoke 'parse' on the arguments available from 'main':
//..
    int main(int argc, const char **argv)
    {
        MyOptionDescription optionDescriptions[NUM_OPTIONS] = {
            // ...
        };

        MyCommandLineParser parser(optionDescriptions, NUM_OPTIONS);

        int rc = parser.parse(argc, argv);
        ASSERT(0    == rc);
        ASSERT(true == parser.isParsed());
//..
// Now, we examine the value of each defined option:
//..
        for (bsl::size_t i = 0; i < parser.numOptions(); ++i) {
            const char                *name  = parser.name(i);
            const balcl::OptionValue&  value = parser.value(i);
//..
// Since our (toy) parser has no feature for handling default values for
// options that are not specified on the command line, we must handle those
// explicitly.
//
// If the option named "outputDir" was set, we use that value; otherwise, we
// set a default value, the current directory:
//..
            if (0 == bsl::strcmp("outputDir", name)) {
                setOutputDir(value.isNull()
                             ? "."
                             : value.the<bsl::string>().c_str());
            }
//..
// If the option named "verbosityLevel" was set we use that value; otherwise,
// we set a default value, '1':
//..
            if (0 == bsl::strcmp("verbosityLevel", name)) {
                setVerbosityLevel(value.isNull()
                                  ? 1
                                  : value.the<int>());
            }
//..
// The option named "caseInsensitive" has no associated value.  If that option
// appeared on the command line, the value of the program flag is set to
// 'true', otherwise ('false == isNull()') that flag is set to 'false':
//..
            if (0 == bsl::strcmp("caseInsensitive", name)) {
                setCaseInsensitivityFlag(value.isNull()
                                         ? false
                                         : true);
            }
        }
//..
// Finally, we continue with the execution of our program using the values
// obtained from the command-line options:
//..
        // ...

        return 0;
    }
//..

// Do not show code below in the header file.

                        // -------------------------
                        // class MyCommandLineParser
                        // -------------------------

// CREATORS
MyCommandLineParser::MyCommandLineParser(const MyOptionDescription *,
                                         bsl::size_t                )
{
}

// MANIPULATORS
int MyCommandLineParser::parse(int, const char **)
{
    return 0;
}

// ACCESSORS
bool MyCommandLineParser::isParsed() const
{
    return true;
}

const char *MyCommandLineParser::name(bsl::size_t) const
{
    return 0;
}

const balcl::OptionValue& MyCommandLineParser::value(bsl::size_t) const
{
    static balcl::OptionValue s_value(bslma::Default::globalAllocator(0));
    return s_value;
}

bsl::size_t MyCommandLineParser::numOptions() const
{
    return 0;
}

}  // close namespace example2

// BDE_VERIFY pragma: +FD01:   // Function declaration requires contract
// BDE_VERIFY pragma: +FABC01  // not in alphabetic order

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, const char *argv[])  {

    int            test = argc > 1 ? bsl::atoi(argv[1]) : 0;
                verbose = argc > 2; (void)             verbose;
            veryVerbose = argc > 3; (void)         veryVerbose;
        veryVeryVerbose = argc > 4; (void)     veryVeryVerbose;
    veryVeryVeryVerbose = argc > 5; (void) veryVeryVeryVerbose;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 10: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLES
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, replace
        //:   leading comment characters with spaces, replace 'assert' with
        //:   'ASSERT', and insert 'if (veryVerbose)' before all output
        //:   operations.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLES
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLES" << endl
                                  << "==============" << endl;

        int rc1 = example1::main();
        ASSERT(0 == rc1);

        int rc2 = example2::main(argc, argv);
        ASSERT(0 == rc2);

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // MODIFIABLE ACCESS
        //
        // Concerns:
        //: 1 The 'the' manipulator provides non-'const' access to the value
        //:   for each supported type.
        //:
        //: 2 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 For each supported option type, create an object having a value
        //:   of that type and use the 'the' manipulator to access and change
        //:   that value to several other distinct values.  Confirm the changes
        //:   using equality comparison.
        //:
        //: 2 Use negative testing to confirm the precondition checks that
        //:   disallow access to objects that do not hold a value (i.e., unset
        //:   objects or objects in a null state), to access to that value via
        //:   a non-matching type.
        //
        // Testing:
        //   template <class TYPE> TYPE& the();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "MODIFIABLE ACCESS" << endl
                          << "=================" << endl;

        RUN_EACH_TYPE(TestDriver, testCase9, ALL_SUPPORTED_TYPES);

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // COPY ASSIGNMENT
        //
        // Concerns:
        //: 1 The operator has the expected signature.
        //:
        //: 2 One object can be assigned to another irrespective of the
        //:   state/type/value of each of those objects.
        //:
        //: 3 The allocator of the assigned to object (lhs) is preserved.
        //
        // Plan:
        //: 1 Use the "pointer-to-method" idiom to have the compiler check the
        //:   signature.
        //:
        //: 2 For a representative set of objects (see the
        //:   'DEFINE_OBJECT_TEST_SET' macro), assign each object with itself
        //:   and to every other object.  Use equality comparison to confirm
        //:   that each object is in the expected state afterward.
        //:
        //: 3 Use an ad hoc test to assign on object to another that is using a
        //:   different allocator.  Confirm that the 'lhs' object retains its
        //:   original allocator.
        //
        // Testing:
        //   operator=(const OptionValue& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY ASSIGNMENT" << endl
                          << "===============" << endl;

        if (veryVerbose) cout << endl << "Check signature" << endl;
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            (void)operatorAssignment;  // quash potential compiler warning
        }

        if (veryVerbose) cout << endl << "Check operator" << endl;

        DEFINE_OBJECT_TEST_SET

        const Obj ARRAY[] = { Unset
                            , NullBool
                            , NullChar
                            , NullInt
                            , NullInt64
                            , NullDouble
                            , NullString
                            , NullDatetime
                            , NullDate
                            , NullTime
                            , NullCharArray
                            , NullIntArray
                            , NullInt64Array
                            , NullDoubleArray
                            , NullStringArray
                            , NullDatetimeArray
                            , NullDateArray
                            , NullTimeArray
                            , DfltBool
                            , DfltChar
                            , DfltInt
                            , DfltInt64
                            , DfltDouble
                            , DfltString
                            , DfltDatetime
                            , DfltDate
                            , DfltTime
                            , DfltCharArray
                            , DfltIntArray
                            , DfltInt64Array
                            , DfltDoubleArray
                            , DfltStringArray
                            , DfltDatetimeArray
                            , DfltDateArray
                            , DfltTimeArray
                            , Val1Bool
                            , Val1Char
                            , Val1Int
                            , Val1Int64
                            , Val1Double
                            , Val1String
                            , Val1Datetime
                            , Val1Date
                            , Val1Time
                            , Val1CharArray
                            , Val1IntArray
                            , Val1Int64Array
                            , Val1DoubleArray
                            , Val1StringArray
                            , Val1DatetimeArray
                            , Val1DateArray
                            , Val1TimeArray
                            , Val2Bool
                            , Val2Char
                            , Val2Int
                            , Val2Int64
                            , Val2Double
                            , Val2String
                            , Val2Datetime
                            , Val2Date
                            , Val2Time
                            , Val2CharArray
                            , Val2IntArray
                            , Val2Int64Array
                            , Val2DoubleArray
                            , Val2StringArray
                            , Val2DatetimeArray
                            , Val2DateArray
                            , Val2TimeArray
                            };

        const bsl::size_t NUM_ELEMENTS = sizeof ARRAY / sizeof *ARRAY;

        for (bsl::size_t i = 0; i < NUM_ELEMENTS; ++i) {

            Obj mX = ARRAY[i]; const Obj& X = mX;
            Obj mY = ARRAY[i]; const Obj& Y = mY;

            ASSERT(&X == &(mX = X)); // ACTION

            ASSERT(X == Y);

            for (bsl::size_t j = 0; j < NUM_ELEMENTS; ++j) {

                Obj lhs = ARRAY[i];  const Obj& Lhs = lhs;
                Obj rhs = ARRAY[j];  const Obj& Rhs = rhs;
                                     const Obj  OrigRhs(Rhs);

                ASSERT(&Lhs == &(lhs = Rhs));  // ACTION

                ASSERT(Lhs     == Rhs);
                ASSERT(OrigRhs == Rhs);
            }
        }

        // Check allocator preservation
        {
            bslma::TestAllocator saLhs("rhs", veryVeryVeryVerbose);
            bslma::TestAllocator saRhs("lhs", veryVeryVeryVerbose);

            Obj lhs(ValueA<Ot::StringArray>::s_value, &saLhs);
            Obj rhs(ValueB<Ot::StringArray>::s_value, &saRhs );

            lhs = rhs; // ACTION

            const bool isAllocatingType = true;

            ASSERT(true == u::checkAllocator(lhs, isAllocatingType, &saLhs));
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTION
        //
        // Concerns:
        //: 1 The member function and the free function have the expected
        //:   signatures.
        //:
        //: 2 Arguments are correctly forwarded to the corresponding methods
        //:   of the underlying classes.
        //
        // Plan:
        //: 1 Use the "pointer to function" and "pointer to member-function"
        //:   idioms to have the compiler check the signatures.  (C-1)
        //:
        //: 2 For a representative set of objects (see the
        //:   'DEFINE_OBJECT_TEST_SET' macro), for each of these two functions,
        //:   swap each object with itself and with every other object.  Use
        //:   equality comparison to confirm that each object is in the
        //:   expected state afterward.
        //
        // Testing:
        //   void swap(OptionValue& other);
        //   void swap(OptionValue& a, b);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SWAP MEMBER AND FREE FUNCTION" << endl
                          << "=============================" << endl;

        if (veryVerbose) cout << endl << "Check signatures" << endl;
        {
            typedef void (Obj::*funcPtr)(Obj&);
            typedef void (*freeFuncPtr)(Obj&, Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     memberSwap = &Obj::swap;
            freeFuncPtr freeSwap   = swap;

            (void)memberSwap;  // quash potential compiler warnings
            (void)freeSwap;
        }

        if (veryVerbose) cout << endl << "Check operators" << endl;

        DEFINE_OBJECT_TEST_SET

        const Obj ARRAY[] = { Unset
                            , NullBool
                            , NullChar
                            , NullString
                            , NullStringArray
                            , DfltBool
                            , DfltChar
                            , DfltString
                            , DfltStringArray
                            , Val1Bool
                            , Val1Char
                            , Val1String
                            , Val1StringArray
                            , Val2Bool
                            , Val2Char
                            , Val2String
                            , Val2StringArray
                            };

        const bsl::size_t NUM_ELEMENTS = sizeof ARRAY / sizeof *ARRAY;

        for (bsl::size_t i = 0; i < NUM_ELEMENTS; ++i) {

            Obj mX = ARRAY[i]; const Obj& X = mX;
            Obj mY = ARRAY[i]; const Obj& Y = mY;

            if (veryVerbose) {
                P_(i) P(mX);
            }

            mX.swap(mX);  // ACTION
            ASSERT(X == Y);

            swap(mX, mX); // ACTION
            ASSERT(X == Y);

            for (bsl::size_t j = 0; j < NUM_ELEMENTS; ++j) {

                Obj lhs = ARRAY[i];   const Obj OrigLhs(lhs);
                Obj rhs = ARRAY[j];   const Obj OrigRhs(rhs);

                if (veryVerbose) {
                    P_(i) P_(lhs) P(rhs)
                }

                // member swap
                {
                    lhs.swap(rhs);  // ACTION

                    ASSERT(lhs == OrigRhs);
                    ASSERT(rhs == OrigLhs);
                }

                // free function swap (back)
                {
                    swap(lhs, rhs);  // ACTION

                    ASSERT(lhs == OrigLhs);
                    ASSERT(rhs == OrigRhs);
                }
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        //
        // Concerns:
        //: 1 The copy constructor duplicates the state/type/value of the
        //:   original object.
        //:
        //: 2 The allocator of the created object depends on its constructor
        //:   argument (not the allocator of the original object).
        //
        // Plans:
        //: 1 Use the copy constructor to duplicate a representative set of
        //:   objects (see the 'DEFINE_OBJECT_TEST_SET' macro).  Confirm the
        //:   state of the created objects using equality comparison.  (C-1)
        //:
        //: 2 Ad hoc test (C-2).
        //
        // Testing:
        //   OptionValue(const OV& o, Allocator *bA = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;

        DEFINE_OBJECT_TEST_SET

        {
            const Obj A(NullBool);         ASSERT(NullBool        == A);
            const Obj B(NullChar);         ASSERT(NullChar        == B);
            const Obj C(NullString);       ASSERT(NullString      == C);
            const Obj D(NullStringArray);  ASSERT(NullStringArray == D);
        }

        {
            const Obj A(Val1Bool);         ASSERT(Val1Bool        == A);
            const Obj B(Val1Char);         ASSERT(Val1Char        == B);
            const Obj C(Val1String);       ASSERT(Val1String      == C);
            const Obj D(Val1StringArray);  ASSERT(Val1StringArray == D);
        }

        // Check allocator
        {
            bslma::TestAllocator saOrig("orig", veryVeryVeryVerbose);
            bslma::TestAllocator saCopy("copy", veryVeryVeryVerbose);

            const Obj Orig(ValueB<Ot::StringArray>::s_value, &saOrig);
            const Obj Copy(Orig,                             &saCopy); //ACTION

            ASSERT(Orig == Copy);

            const bool isAllocatingType = true;

            ASSERT(true == u::checkAllocator(Copy, isAllocatingType, &saCopy));
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        //
        // Concerns:
        //: 1 The equality comparison operations have the expected signatures.
        //:
        //: 2 Equality requires agreement in state (set/unset, null/not-null),
        //:   type, and value, irrespective of supported type.
        //:
        //: 3 The equality operators provide the expected behavior with respect
        //:   to identity, transitivity, commutativity, and negation.
        //
        // Plans:
        //: 1 Use the "pointer to function" idiom to have the compiler check
        //:   the signatures.  (C-1)
        //:
        //: 2 Use a series of ad hoc tests on a representative set of objects
        //:   (see the 'DEFINE_OBJECT_TEST_SET' macro) to confirm equality (or
        //:   not) as expected.  (C-2)
        //:
        //: 3 Use macros to implicit replicate each check into several
        //:   equivalent tests that show commutativity, negation, etc.
        //:   Identity test are part of P-2.  (C-3)
        //
        // Testing:
        //   bool operator==(const OptionValue& lhs, rhs);
        //   bool operator!=(const OptionValue& lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EQUALITY-COMPARISON OPERATORS" << endl
                          << "=============================" << endl;

        if (veryVerbose) cout << endl << "Check signatures" << endl;
        {
            using namespace balcl;
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            // Verify that the signatures and return types are standard.

            operatorPtr operatorEq = operator==;
            operatorPtr operatorNe = operator!=;

            (void)operatorEq;  // quash potential compiler warnings
            (void)operatorNe;
        }

        if (veryVerbose) cout << endl << "Check operations" << endl;

#define COMBO_EQ(LHS, RHS)   ASSERT(  (LHS) == (RHS));                        \
                             ASSERT(  (RHS) == (LHS));                        \
                             ASSERT(!((LHS) != (RHS)));                       \
                             ASSERT(!((RHS) != (LHS)));

#define COMBO_NE(LHS, RHS)   ASSERT(  (LHS) != (RHS));                        \
                             ASSERT(  (RHS) != (LHS));                        \
                             ASSERT(!((LHS) == (RHS)));                       \
                             ASSERT(!((RHS) == (LHS)));

        DEFINE_OBJECT_TEST_SET

        // No match with 'Unset'
        COMBO_NE(Unset            , NullBool);
        COMBO_NE(Unset            , NullChar);
        COMBO_NE(Unset            , NullInt);
        COMBO_NE(Unset            , NullInt64);
        COMBO_NE(Unset            , NullDouble);
        COMBO_NE(Unset            , NullString);
        COMBO_NE(Unset            , NullDatetime);
        COMBO_NE(Unset            , NullDate);
        COMBO_NE(Unset            , NullTime);
        COMBO_NE(Unset            , NullCharArray);
        COMBO_NE(Unset            , NullIntArray);
        COMBO_NE(Unset            , NullInt64Array);
        COMBO_NE(Unset            , NullDoubleArray);
        COMBO_NE(Unset            , NullStringArray);
        COMBO_NE(Unset            , NullDatetimeArray);
        COMBO_NE(Unset            , NullDateArray);
        COMBO_NE(Unset            , NullTimeArray);

        // Identity
        COMBO_EQ(NullBool         , NullBool         );
        COMBO_EQ(NullChar         , NullChar         );
        COMBO_EQ(NullInt          , NullInt          );
        COMBO_EQ(NullInt64        , NullInt64        );
        COMBO_EQ(NullDouble       , NullDouble       );
        COMBO_EQ(NullString       , NullString       );
        COMBO_EQ(NullDatetime     , NullDatetime     );
        COMBO_EQ(NullDate         , NullDate         );
        COMBO_EQ(NullTime         , NullTime         );
        COMBO_EQ(NullCharArray    , NullCharArray    );
        COMBO_EQ(NullIntArray     , NullIntArray     );
        COMBO_EQ(NullInt64Array   , NullInt64Array   );
        COMBO_EQ(NullDoubleArray  , NullDoubleArray  );
        COMBO_EQ(NullStringArray  , NullStringArray  );
        COMBO_EQ(NullDatetimeArray, NullDatetimeArray);
        COMBO_EQ(NullDateArray    , NullDateArray    );
        COMBO_EQ(NullTimeArray    , NullTimeArray    );

        // Must match on type.
        COMBO_NE(NullBool         , NullChar          );
        COMBO_NE(NullChar         , NullInt           );
        COMBO_NE(NullInt          , NullInt64         );
        COMBO_NE(NullInt64        , NullDouble        );
        COMBO_NE(NullDouble       , NullString        );
        COMBO_NE(NullString       , NullDatetime      );
        COMBO_NE(NullDatetime     , NullDate          );
        COMBO_NE(NullDate         , NullTime          );
        COMBO_NE(NullTime         , NullCharArray     );
        COMBO_NE(NullCharArray    , NullIntArray      );
        COMBO_NE(NullIntArray     , NullInt64Array    );
        COMBO_NE(NullInt64Array   , NullDoubleArray   );
        COMBO_NE(NullDoubleArray  , NullStringArray   );
        COMBO_NE(NullStringArray  , NullDatetimeArray );
        COMBO_NE(NullDatetimeArray, NullDateArray     );
        COMBO_NE(NullDateArray    , NullTimeArray     );
        COMBO_NE(NullTimeArray    , NullBool          );

        // No match with 'Unset'
        COMBO_NE(Unset            , DfltBool         );
        COMBO_NE(Unset            , DfltChar         );
        COMBO_NE(Unset            , DfltInt          );
        COMBO_NE(Unset            , DfltInt64        );
        COMBO_NE(Unset            , DfltDouble       );
        COMBO_NE(Unset            , DfltString       );
        COMBO_NE(Unset            , DfltDatetime     );
        COMBO_NE(Unset            , DfltDate         );
        COMBO_NE(Unset            , DfltTime         );
        COMBO_NE(Unset            , DfltCharArray    );
        COMBO_NE(Unset            , DfltIntArray     );
        COMBO_NE(Unset            , DfltInt64Array   );
        COMBO_NE(Unset            , DfltDoubleArray  );
        COMBO_NE(Unset            , DfltStringArray  );
        COMBO_NE(Unset            , DfltDatetimeArray);
        COMBO_NE(Unset            , DfltDateArray    );
        COMBO_NE(Unset            , DfltTimeArray    );

        // No match with Null;
        COMBO_NE(NullBool         , DfltBool         );
        COMBO_NE(NullChar         , DfltChar         );
        COMBO_NE(NullInt          , DfltInt          );
        COMBO_NE(NullInt64        , DfltInt64        );
        COMBO_NE(NullDouble       , DfltDouble       );
        COMBO_NE(NullString       , DfltString       );
        COMBO_NE(NullDatetime     , DfltDatetime     );
        COMBO_NE(NullDate         , DfltDate         );
        COMBO_NE(NullTime         , DfltTime         );
        COMBO_NE(NullCharArray    , DfltCharArray    );
        COMBO_NE(NullIntArray     , DfltIntArray     );
        COMBO_NE(NullInt64Array   , DfltInt64Array   );
        COMBO_NE(NullDoubleArray  , DfltDoubleArray  );
        COMBO_NE(NullStringArray  , DfltStringArray  );
        COMBO_NE(NullDatetimeArray, DfltDatetimeArray);
        COMBO_NE(NullDateArray    , DfltDateArray    );
        COMBO_NE(NullTimeArray    , DfltTimeArray    );

        // Identity
        COMBO_EQ(DfltBool         , DfltBool         );
        COMBO_EQ(DfltChar         , DfltChar         );
        COMBO_EQ(DfltInt          , DfltInt          );
        COMBO_EQ(DfltInt64        , DfltInt64        );
        COMBO_EQ(DfltDouble       , DfltDouble       );
        COMBO_EQ(DfltString       , DfltString       );
        COMBO_EQ(DfltDatetime     , DfltDatetime     );
        COMBO_EQ(DfltDate         , DfltDate         );
        COMBO_EQ(DfltTime         , DfltTime         );
        COMBO_EQ(DfltCharArray    , DfltCharArray    );
        COMBO_EQ(DfltIntArray     , DfltIntArray     );
        COMBO_EQ(DfltInt64Array   , DfltInt64Array   );
        COMBO_EQ(DfltDoubleArray  , DfltDoubleArray  );
        COMBO_EQ(DfltStringArray  , DfltStringArray  );
        COMBO_EQ(DfltDatetimeArray, DfltDatetimeArray);
        COMBO_EQ(DfltDateArray    , DfltDateArray    );
        COMBO_EQ(DfltTimeArray    , DfltTimeArray    );

        // Must match on type.
        COMBO_NE(DfltBool         , DfltChar          );
        COMBO_NE(DfltChar         , DfltInt           );
        COMBO_NE(DfltInt          , DfltInt64         );
        COMBO_NE(DfltInt64        , DfltDouble        );
        COMBO_NE(DfltDouble       , DfltString        );
        COMBO_NE(DfltString       , DfltDatetime      );
        COMBO_NE(DfltDatetime     , DfltDate          );
        COMBO_NE(DfltDate         , DfltTime          );
        COMBO_NE(DfltTime         , DfltCharArray     );
        COMBO_NE(DfltCharArray    , DfltIntArray      );
        COMBO_NE(DfltIntArray     , DfltInt64Array    );
        COMBO_NE(DfltInt64Array   , DfltDoubleArray   );
        COMBO_NE(DfltDoubleArray  , DfltStringArray   );
        COMBO_NE(DfltStringArray  , DfltDatetimeArray );
        COMBO_NE(DfltDatetimeArray, DfltDateArray     );
        COMBO_NE(DfltDateArray    , DfltTimeArray     );
        COMBO_NE(DfltTimeArray    , DfltBool          );

        // No match with 'Unset'
        COMBO_NE(Unset            , Val1Bool         );
        COMBO_NE(Unset            , Val1Char         );
        COMBO_NE(Unset            , Val1Int          );
        COMBO_NE(Unset            , Val1Int64        );
        COMBO_NE(Unset            , Val1Double       );
        COMBO_NE(Unset            , Val1String       );
        COMBO_NE(Unset            , Val1Datetime     );
        COMBO_NE(Unset            , Val1Date         );
        COMBO_NE(Unset            , Val1Time         );
        COMBO_NE(Unset            , Val1CharArray    );
        COMBO_NE(Unset            , Val1IntArray     );
        COMBO_NE(Unset            , Val1Int64Array   );
        COMBO_NE(Unset            , Val1DoubleArray  );
        COMBO_NE(Unset            , Val1StringArray  );
        COMBO_NE(Unset            , Val1DatetimeArray);
        COMBO_NE(Unset            , Val1DateArray    );
        COMBO_NE(Unset            , Val1TimeArray    );

        COMBO_NE(Unset            , Val2Bool         );
        COMBO_NE(Unset            , Val2Char         );
        COMBO_NE(Unset            , Val2Int          );
        COMBO_NE(Unset            , Val2Int64        );
        COMBO_NE(Unset            , Val2Double       );
        COMBO_NE(Unset            , Val2String       );
        COMBO_NE(Unset            , Val2Datetime     );
        COMBO_NE(Unset            , Val2Date         );
        COMBO_NE(Unset            , Val2Time         );
        COMBO_NE(Unset            , Val2CharArray    );
        COMBO_NE(Unset            , Val2IntArray     );
        COMBO_NE(Unset            , Val2Int64Array   );
        COMBO_NE(Unset            , Val2DoubleArray  );
        COMBO_NE(Unset            , Val2StringArray  );
        COMBO_NE(Unset            , Val2DatetimeArray);
        COMBO_NE(Unset            , Val2DateArray    );
        COMBO_NE(Unset            , Val2TimeArray    );

        // No match with Null;
        COMBO_NE(NullBool         , Val1Bool         );
        COMBO_NE(NullChar         , Val1Char         );
        COMBO_NE(NullInt          , Val1Int          );
        COMBO_NE(NullInt64        , Val1Int64        );
        COMBO_NE(NullDouble       , Val1Double       );
        COMBO_NE(NullString       , Val1String       );
        COMBO_NE(NullDatetime     , Val1Datetime     );
        COMBO_NE(NullDate         , Val1Date         );
        COMBO_NE(NullTime         , Val1Time         );
        COMBO_NE(NullCharArray    , Val1CharArray    );
        COMBO_NE(NullIntArray     , Val1IntArray     );
        COMBO_NE(NullInt64Array   , Val1Int64Array   );
        COMBO_NE(NullDoubleArray  , Val1DoubleArray  );
        COMBO_NE(NullStringArray  , Val1StringArray  );
        COMBO_NE(NullDatetimeArray, Val1DatetimeArray);
        COMBO_NE(NullDateArray    , Val1DateArray    );
        COMBO_NE(NullTimeArray    , Val1TimeArray    );

        COMBO_NE(NullBool         , Val2Bool         );
        COMBO_NE(NullChar         , Val2Char         );
        COMBO_NE(NullInt          , Val2Int          );
        COMBO_NE(NullInt64        , Val2Int64        );
        COMBO_NE(NullDouble       , Val2Double       );
        COMBO_NE(NullString       , Val2String       );
        COMBO_NE(NullDatetime     , Val2Datetime     );
        COMBO_NE(NullDate         , Val2Date         );
        COMBO_NE(NullTime         , Val2Time         );
        COMBO_NE(NullCharArray    , Val2CharArray    );
        COMBO_NE(NullIntArray     , Val2IntArray     );
        COMBO_NE(NullInt64Array   , Val2Int64Array   );
        COMBO_NE(NullDoubleArray  , Val2DoubleArray  );
        COMBO_NE(NullStringArray  , Val2StringArray  );
        COMBO_NE(NullDatetimeArray, Val2DatetimeArray);
        COMBO_NE(NullDateArray    , Val2DateArray    );
        COMBO_NE(NullTimeArray    , Val2TimeArray    );

        // Identity
        COMBO_EQ(Val1Bool       , Val1Bool);
        COMBO_EQ(Val1Char       , Val1Char);
        COMBO_EQ(Val1String     , Val1String);
        COMBO_EQ(Val1StringArray, Val1StringArray);

        COMBO_EQ(Val2Bool       , Val2Bool);
        COMBO_EQ(Val2Char       , Val2Char);
        COMBO_EQ(Val2String     , Val2String);
        COMBO_EQ(Val2StringArray, Val2StringArray);

        COMBO_EQ(Val1Bool         , Val1Bool         );
        COMBO_EQ(Val1Char         , Val1Char         );
        COMBO_EQ(Val1Int          , Val1Int          );
        COMBO_EQ(Val1Int64        , Val1Int64        );
        COMBO_EQ(Val1Double       , Val1Double       );
        COMBO_EQ(Val1String       , Val1String       );
        COMBO_EQ(Val1Datetime     , Val1Datetime     );
        COMBO_EQ(Val1Date         , Val1Date         );
        COMBO_EQ(Val1Time         , Val1Time         );
        COMBO_EQ(Val1CharArray    , Val1CharArray    );
        COMBO_EQ(Val1IntArray     , Val1IntArray     );
        COMBO_EQ(Val1Int64Array   , Val1Int64Array   );
        COMBO_EQ(Val1DoubleArray  , Val1DoubleArray  );
        COMBO_EQ(Val1StringArray  , Val1StringArray  );
        COMBO_EQ(Val1DatetimeArray, Val1DatetimeArray);
        COMBO_EQ(Val1DateArray    , Val1DateArray    );
        COMBO_EQ(Val1TimeArray    , Val1TimeArray    );

        COMBO_EQ(Val2Bool         , Val2Bool         );
        COMBO_EQ(Val2Char         , Val2Char         );
        COMBO_EQ(Val2Int          , Val2Int          );
        COMBO_EQ(Val2Int64        , Val2Int64        );
        COMBO_EQ(Val2Double       , Val2Double       );
        COMBO_EQ(Val2String       , Val2String       );
        COMBO_EQ(Val2Datetime     , Val2Datetime     );
        COMBO_EQ(Val2Date         , Val2Date         );
        COMBO_EQ(Val2Time         , Val2Time         );
        COMBO_EQ(Val2CharArray    , Val2CharArray    );
        COMBO_EQ(Val2IntArray     , Val2IntArray     );
        COMBO_EQ(Val2Int64Array   , Val2Int64Array   );
        COMBO_EQ(Val2DoubleArray  , Val2DoubleArray  );
        COMBO_EQ(Val2StringArray  , Val2StringArray  );
        COMBO_EQ(Val2DatetimeArray, Val2DatetimeArray);
        COMBO_EQ(Val2DateArray    , Val2DateArray    );
        COMBO_EQ(Val2TimeArray    , Val2TimeArray    );

        // Must match on value.
        COMBO_NE(Val1Bool         , Val2Bool         );
        COMBO_NE(Val1Char         , Val2Char         );
        COMBO_NE(Val1Int          , Val2Int          );
        COMBO_NE(Val1Int64        , Val2Int64        );
        COMBO_NE(Val1Double       , Val2Double       );
        COMBO_NE(Val1String       , Val2String       );
        COMBO_NE(Val1Datetime     , Val2Datetime     );
        COMBO_NE(Val1Date         , Val2Date         );
        COMBO_NE(Val1Time         , Val2Time         );
        COMBO_NE(Val1CharArray    , Val2CharArray    );
        COMBO_NE(Val1IntArray     , Val2IntArray     );
        COMBO_NE(Val1Int64Array   , Val2Int64Array   );
        COMBO_NE(Val1DoubleArray  , Val2DoubleArray  );
        COMBO_NE(Val1StringArray  , Val2StringArray  );
        COMBO_NE(Val1DatetimeArray, Val2DatetimeArray);
        COMBO_NE(Val1DateArray    , Val2DateArray    );
        COMBO_NE(Val1TimeArray    , Val2TimeArray    );

#undef COMBO_EQ
#undef COMBO_NE

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // VALUE CONSTRUCTORS
        //
        // Concerns:
        //: 1 Each of the value constructors can be invoked with no specified
        //:   allocator (and receive the default allocator), or with an
        //:   explicit 0 argument (and also receive the default allocator), or
        //:   with an explicitly specified allocator.
        //:
        //: 2 Each object is created with the intended type, value, and
        //:   allocator.
        //
        // Plans:
        //: 1 Use the "footprint" idiom to repeat each test for a test object
        //:   created using each constructor and argument pattern of interest.
        //:   (C-1)
        //:
        //: 2 Use the (test) basic allocators (sometimes via helper functions)
        //:   to confirm that the objects were created as expected.  (C-2)
        //
        // Testing:
        //   OptionValue(Ot::Enum type, Allocator *bA = 0);
        //   OptionValue(bool           v, *bA = 0);
        //   OptionValue(char           v, *bA = 0);
        //   OptionValue(int            v, *bA = 0);
        //   OptionValue(Int64          v, *bA = 0);
        //   OptionValue(double         v, *bA = 0);
        //   OptionValue(const string&  v, *bA = 0);
        //   OptionValue(Datetime       v, *bA = 0);
        //   OptionValue(Date           v, *bA = 0);
        //   OptionValue(Time           v, *bA = 0);
        //   OptionValue(const vector<char>&     v, *bA = 0);
        //   OptionValue(const vector<int>&      v, *bA = 0);
        //   OptionValue(const vector<Int64>&    v, *bA = 0);
        //   OptionValue(const vector<double>&   v, *bA = 0);
        //   OptionValue(const vector<string>&   v, *bA = 0);
        //   OptionValue(const vector<Datetime>& v, *bA = 0);
        //   OptionValue(const vector<Date>&     v, *bA = 0);
        //   OptionValue(const vector<Time>&     v, *bA = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "VALUE CONSTRUCTORS" << endl
                          << "==================" << endl;

        RUN_EACH_TYPE(TestDriver, testCase4, ALL_SUPPORTED_TYPES);

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR, PRIMARY MANIPULATORS, BASIC ACCESSORS, AND DTOR
        //
        // Concerns:
        //: 1 The test values (defined at file scope) 'D, 'A', and 'B' are
        //:   different from each other for every supported option type
        //:   except 'bool' (which can have only two distinct values).
        //:
        //: 2 We test both constructors that create unset objects and confirm
        //:   that each receives the intended allocator.
        //:
        //:   o Note that we do not test the 'Obj(basicAllocator *)'
        //:     constructor with a 0 value because, in that case, the compiler
        //:     dispatches to the value constructor that accepts an 'int' (a
        //:     better match, no conversion needed), not the intended
        //:     allocator.
        //:
        //: 3 Use the primary manipulators to run the created object through
        //:   various states: unset, typed with the default value (of the
        //:   option type), typed but no specified value (the "null" state),
        //:   and at least two different non-default values.
        //:
        //: 4 The basic accessors are in agreement with the expected state/
        //:   value of the object.
        //:
        //: 5 The print and 'operator<<' stream operators have the expected
        //:   signatures.  Also, the 'print' method has the expected default
        //:   values for 'level' and 'spacesPerLevel'.
        //:
        //: 6 All accessors are 'const' qualified.
        //:
        //: 7 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the helper function 'u::checkAllValuesDAB' to confirm that
        //:   'D', 'A', and 'B' have distinct values for all option types
        //:   (except 'bool').
        //
        //: 2 Use the "footprint" idiom to repeat each test for a test object
        //:   created using each constructor and argument pattern of interest.
        //:   (C-2)
        //:
        //: 3 The state of the object is changed using the primary manipulators
        //:   'setType', 'reset', 'set', 'setNull'.  (C-3)
        //:
        //:   o Note that 'setValue' is invoked via the 'u::setValueDAB' helper
        //:     function.
        //:
        //: 4 Use each of the listed basic accessors to confirm object state.
        //:   (C-4)
        //:
        //:   o Note that the 'template const TYPE& the<TYPE>' accessor is
        //:     invoked via the 'u::hasValueDAB' helper function.
        //:
        //: 5 Use the "pointer to member-function/operator" idiom.  Test the
        //:   default values in an ad-hoc test.  Note that we are primarily
        //:   testing the correct delegation to the print methods of the
        //:   underlying classes.
        //:
        //: 6 Always invoke accessors on 'const' qualified objects or
        //:   'const'-references to objects.  (If the accessor were not
        //:   'const' qualified, compilation would fail.)
        //:
        //: 7 Use 'BSLS_ASSERTTEST_*' facilities for negative testing.  (C-7)
        //
        // Testing:
        //   OptionValue();
        //   OptionValue(bslma::Allocator *basicAllocator);
        //   ~OptionValue() = default;
        //   void reset();
        //   template <class TYPE> void set(const TYPE& value);
        //   void setNull();
        //   void setType(OptionType::Enum type);
        //   bool hasNonVoidType() const;
        //   bool isNull() const;
        //   OptionType::Enum type() const;
        //   template <class TYPE> const TYPE& the() const;
        //   bslma::Allocator *allocator() const;
        //   ostream& print(ostream& s, int level = 0, int sPL = 4) const;
        //   operator<<(bsl::ostream& s, const OptionValue& d);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
    << "DEFAULT CTOR, PRIMARY MANIPULATORS, BASIC ACCESSORS, AND DTOR" << endl
    << "=============================================================" << endl;

        if (veryVerbose) cout << endl
                              << "Check test values 'D', 'A', and 'B'"
                              << endl;

        ASSERT(u::checkAllValuesDAB());

        RUN_EACH_TYPE(TestDriver, testCase3, ALL_SUPPORTED_TYPES);

        if (veryVerbose) cout << endl
                              << "Check signatures of 'print' and 'operator<<'"
                              << endl;
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

        if (veryVerbose) cout << endl << "Test return value of 'print'"
                                      << endl;
        {
            bsl::ostringstream oss;

            Obj mX(Ot::e_INT);  mX.setNull(); const Obj& X = mX;

            ASSERT(&oss == &(X.print(oss)));  // ACTION

        }

        if (veryVerbose) cout << endl << "Test default values of 'print'"
                                      << endl;
        {
            bsl::ostringstream oss;

            Obj mX(Ot::e_INT);  mX.setNull(); const Obj& X = mX;

            X.print(oss, 2, 5);  // ACTION  Specify all parameters.
            ASSERT("          NULL\n" == oss.str());
                  //----^----|  indented by 10

            oss.str(""); oss.clear();

            X.print(oss, 2);     // ACTION  'spacesPerLevel' default to 4.
            ASSERT(  "        NULL\n" == oss.str());
                    //----^---  indented by 8

            oss.str(""); oss.clear();

            X.print(oss);        // ACTION  'level' default to 0.
            ASSERT(          "NULL\n" == oss.str());
                              // no indentation
        }

      } break;
      case 2: {
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
           //LINE   INPUT            PERT  OUTPUT
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
            const int        LINE = DATA[ti].d_line;
            const Ot::Enum  INPUT = DATA[ti].d_input;
            const int        PERT = DATA[ti].d_offset;
            const Ot::Enum OUTPUT = DATA[ti].d_output;

            if (veryVerbose) {
                T_ P_(ti) P_(LINE) P_(INPUT) P_(PERT) P(OUTPUT)
            }

            ASSERT(OUTPUT == u::shiftType(INPUT, PERT));
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
        //: 1 Create an object 'w' (enum ctor).          { w:D             }
        //: 2 Create an object 'x' (copy from 'w').      { w:D x:D         }
        //: 3 Set 'x' to 'A' (value distinct from 'D').  { w:D x:A         }
        //: 4 Create an object 'y' (init. to 'A').       { w:D x:A y:A     }
        //: 5 Create an object 'z' (copy from 'y').      { w:D x:A y:A z:A }
        //: 6 Set 'z' to 'D' (the default value).        { w:D x:A y:A z:D }
        //: 7 Assign 'w' from 'x'.                       { w:A x:A y:A z:D }
        //: 8 Assign 'w' from 'z'.                       { w:D x:A y:A z:D }
        //: 9 Assign 'x' from 'x' (aliasing).            { w:D x:A y:A z:D }
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        RUN_EACH_TYPE(TestDriver, testCase1, ALL_SUPPORTED_TYPES);

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      } break;
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

#undef DEFINE_OBJECT_TEST_SET

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
