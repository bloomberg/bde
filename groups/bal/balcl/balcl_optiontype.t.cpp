// balcl_optiontype.t.cpp                                             -*-C++-*-
#include <balcl_optiontype.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_time.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bslmf_issame.h>    // 'bsl::is_same' (see 'bsl_type_traits.h' below)

#include <bsls_platform.h>   // 'BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC'
#include <bsls_types.h>      // 'bsls::Types::Int64'

#include <bsl_cstddef.h>     // 'bsl::size_t'
#include <bsl_cstdlib.h>     // 'bsl::atoi'
#include <bsl_cstring.h>     // 'bsl::strcmp'
#include <bsl_iostream.h>
#include <bsl_ostream.h>     // 'operator<<'
#include <bsl_sstream.h>
#include <bsl_string.h>
//#include <bsl_type_traits.h> // 'bsl::is_same' (disallowed pre-C++11)
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The class under test, 'balcl::OptionType', has several different kinds of
// facilities.  It is a namespace for:
//: o A set of 'typedef's.
//: o An enumeration (that defines an enumerator for each 'typedef').
//: o A statically-initialized null pointer for each of those types.
//: o Utility functions.
//
// We use standard test techniques for an enumeration, a utility, etc.,
// respectively.
//
// Global Concerns:
//: o No methods or free operators allocate memory.
//
// Global Assumptions:
//: o All CLASS METHODS and the '<<' free operator are 'const' thread-safe.
// ----------------------------------------------------------------------------
// TYPES
// [ 1] enum Enum { ... };
//
// CLASS METHODS
// [ 4] Enum fromArrayType(Enum arrayType);
// [ 4] bool isArrayType(Enum type);
// [ 4] Enum toArrayType(Enum type);
// [ 2] ostream& print(ostream& s, Enum val, int level = 0, int sPL = 4);
// [ 1] const char *toAscii(OptionType::Enum val);
//
// FREE OPERATORS
// [ 3] operator<<(ostream& s, OptionType::Enum val);
// ----------------------------------------------------------------------------
// [ 6] CONCERN: Type aliases are defined as expected.
// [ 6] CONCERN: 'OptionType::EnumToType' types
// [ 6] CONCERN: 'OptionType::TypeToEnum' enumerators
// [ 5] CONCERN: static data
// [ 7] CONCERN: HISTORIC TEST
// [ 8] USAGE EXAMPLE

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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef balcl::OptionType Obj;
typedef Obj::Enum         Enum;

// ============================================================================
//                      GLOBAL CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

const int NUM_ENUMERATORS = 18;

#define UNKNOWN_FORMAT "(* UNKNOWN *)"

// ============================================================================
//                              HELPER FUNCTIONS
// ----------------------------------------------------------------------------

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

#define MATCH_OPTION_TYPE(ELEM_TYPE, TYPE)                                    \
    template <>                                                               \
    struct CheckOptionType<(int)ELEM_TYPE, TYPE> {                            \
        bool operator()() const { return true; }                              \
    };                                                                        \
    // This macro defines a specialization of the 'CheckOptionType' class
    // template whose boolean functor returns 'true' for the parameterized
    // 'ELEM_TYPE' matching the parameterized 'TYPE'.

#define MATCH_OPTION_TYPE_PAIR(ELEM_TYPE, TYPE)                               \
                                                                              \
    MATCH_OPTION_TYPE(ELEM_TYPE, TYPE)                                        \
                                                                              \
    template <>                                                               \
    struct CheckOptionType<(int)ELEM_TYPE##_ARRAY, bsl::vector<TYPE> > {      \
        bool operator()() const { return true; }                              \
    };
    // This macro defines *two* specializations of the 'CheckOptionType' class
    // template whose boolean functor returns 'true', the first for the
    // parameterized 'ELEM_TYPE' matching the parameterized 'TYPE', and the
    // second for the corresponding array type.

MATCH_OPTION_TYPE(Obj::e_BOOL,     bool)

MATCH_OPTION_TYPE_PAIR(Obj::e_CHAR,     char)
MATCH_OPTION_TYPE_PAIR(Obj::e_INT,      int)
MATCH_OPTION_TYPE_PAIR(Obj::e_INT64,    bsls::Types::Int64)
MATCH_OPTION_TYPE_PAIR(Obj::e_DOUBLE,   double)
MATCH_OPTION_TYPE_PAIR(Obj::e_STRING,   bsl::string)
MATCH_OPTION_TYPE_PAIR(Obj::e_DATETIME, bdlt::Datetime)
MATCH_OPTION_TYPE_PAIR(Obj::e_DATE,     bdlt::Date)
MATCH_OPTION_TYPE_PAIR(Obj::e_TIME,     bdlt::Time)

#undef MATCH_OPTION_TYPE
#undef MATCH_OPTION_TYPE_PAIR

template <int ELEM_TYPE, class TYPE>
bool checkOptionType(const TYPE *optionTypeValue)
    // Return 'true' if the specified 'ELEM_TYPE' corresponds to the specified
    // 'TYPE' (as defined by the 'MATCH_OPTION_TYPE_*' macro invocations above)
    // and if the specified 'optionTypeValue' is a null pointer whose (template
    // parameter) 'TYPE' matches the 'balcl::OptionType' described by the
    // (template parameter) 'ELEM_TYPE' enumerator.
{
    CheckOptionType<ELEM_TYPE, TYPE> checker;
    return checker() && (TYPE *)0 == optionTypeValue;
}

// ============================================================================
//                       CLASSES FOR USAGE EXAMPLE
// ----------------------------------------------------------------------------

namespace Example2 {
                        // ======================
                        // class MyMultitypeValue
                        // ======================

class MyMultitypeValue {

  public:
    // CREATORS
    MyMultitypeValue();
        // Create a 'MyMultitypeValue' object having type 'e_VOID'.

    explicit MyMultitypeValue(Enum type);
        // Create a 'MyMultitypeValue' object having the specified 'type' and
        // the default constructed value of that type.

    // MANIPULATORS
    void append(const MyMultitypeValue& element);
        // Append the specified 'element' to this object.  The behavior is
        // undefined unless this object is an "array" type.

    void setType(Enum type);
        // Set the type of this object to the specified 'type' and the value
        // to the default construction value of that type.
};

                        // ----------------------
                        // class MyMultitypeValue
                        // ----------------------

// CREATORS
MyMultitypeValue::MyMultitypeValue()
{
}

MyMultitypeValue::MyMultitypeValue(Enum)
{
}

// MANIPULATORS
void MyMultitypeValue::append(const MyMultitypeValue&)
{
}

void MyMultitypeValue::setType(Enum)
{
}

                        // ===========================
                        // struct MyMultitypeValueUtil
                        // ===========================

struct MyMultitypeValueUtil {

    static int parse(MyMultitypeValue        *result,
                     bsl::ostream&            input,
                     balcl::OptionType::Enum  type);
        // Load into the specified 'result' the value represented in the
        // specified 'input' that is formatted according to the specified
        // 'type'.

    static int parseScalar(MyMultitypeValue        *result,
                           bsl::ostream&            input,
                           balcl::OptionType::Enum  type);
        // Load into the specified 'result' the value represented in the
        // specified 'input' that is formatted according to the specified
        // 'type'.  The behavior is undefined unless 'type' is one of the
        // scalar (i.e., non-array) types.
};

                        // ---------------------------
                        // struct MyMultitypeValueUtil
                        // ---------------------------

int MyMultitypeValueUtil::parseScalar(MyMultitypeValue                  *,
                                      bsl::ostream&                      ,
                                      balcl::OptionType::Enum  )
{
    cout << "MyMultitypeValueUtil::parseScalar: called" << endl;
    return 99;
}

///Example 2: Utility Methods
/// - - - - - - - - - - - - -
// In a software system devoted to assembling option values of various types,
// the code is often governed in terms of the enumerated values
// ('balcl::OptionType::Enum') corresponding to the various types.  In
// particular, in order to assemble an option value of one of the array types
// (e.g., 'balcl::OptionType::e_STRING_ARRAY'), one must first construct the
// constitute elements.
//
// Suppose we have a class, 'MyMultitypeValue', that can, at runtime, be set to
// contain a value of one of the types named by 'balcl::OptionType'.  We may
// want to initialize a 'MyMultitypeValue' object from an input stream using a
// utility function 'MyMultitypeValueUtil::parse':
//..
    int MyMultitypeValueUtil::parse(MyMultitypeValue        *result,
                                    bsl::ostream&            input,
                                    balcl::OptionType::Enum  type)
    {
        BSLS_ASSERT(result);

        result->setType(type);
//..
// If 'type' is not one of the array types, as determined by the
// 'balcl::OptionType::isArrayType' method, one calls
// 'MyMultitypeValueUtil::parseScalar':
//..
        if (!balcl::OptionType::isArrayType(type)) {
            return MyMultitypeValueUtil::parseScalar(result, input, type);
                                                                      // RETURN
        } else {
//..
// Otherwise, we have an array type.  In this case, we must call 'parseScalar'
// repeatedly and build a vector of those scalar values.  The scalar type can
// be calculated from the given array type by the
// 'balcl::OptionType::fromArrayType' method:
//..
            balcl::OptionType::Enum scalarType =
                                        balcl::OptionType::fromArrayType(type);

            MyMultitypeValue element(scalarType);

            int rc;
            while (0 == (rc = MyMultitypeValueUtil::parseScalar(&element,
                                                                input,
                                                                scalarType))) {
                result->append(element);
            }
            return rc;                                                // RETURN
        }
    }
//..
}  // close namespace Example2

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int             test = argc > 1 ? atoi(argv[1]) : 0;
    const bool         verbose = argc > 2;
    const bool     veryVerbose = argc > 3;
    const bool veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator defaultAllocator(veryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    bslma::TestAllocator globalAllocator(veryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 8: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
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
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;
        cout << "Example 1" << endl;
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code provide a simple illustration of
// 'balcl::OptionType' usage.
//
// First, we create a variable 'value' of type 'balcl::OptionType::Enum' and
// initialize it to the value 'balcl::OptionType::e_STRING':
//..
    balcl::OptionType::Enum value = balcl::OptionType::e_STRING;
//..
// Next, we store a pointer to its ASCII representation in a variable
// 'asciiValue' of type 'const char *':
//..
    const char *asciiValue = balcl::OptionType::toAscii(value);
    ASSERT(0 == bsl::strcmp(asciiValue, "STRING"));
//..
// Finally, we print the value to 'bsl::cout':
//..
if (veryVerbose)
    bsl::cout << value << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  STRING
//..

        // Need not show next lines in header file.

        if (veryVerbose) {
            bsl::cout << "Example 2" << bsl::endl;
        }
        Example2::MyMultitypeValue mmtv;
        ostringstream              os;
        ASSERT(99 == Example2::MyMultitypeValueUtil::parse(&mmtv,
                                                           os,
                                                           Obj::e_INT_ARRAY));
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'balcl::OptionType'
        //   This class was formerly defined in another component.  This test
        //   case was preserved from that component to confirm compatibility.
        //
        // Concerns:
        //: 1 That the class data members have well-defined names that match
        //:   the 'balcl::OptionType' element type enumeration names,
        //:   and that their types match the expected
        //:   'balcl::OptionType' type.
        //
        // Plan:
        //: 1 Simply test the names of the class data members and (although it
        //:   is not documented), assert that their value is 0.
        //
        // Testing:
        //   CONCERN: HISTORIC TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'balcl::OptionType'" << endl
                          << "===========================" << endl;
       //v--^
         ASSERT(checkOptionType<Obj::e_BOOL>          (Obj::k_BOOL));

         ASSERT(checkOptionType<Obj::e_CHAR>          (Obj::k_CHAR));
         ASSERT(checkOptionType<Obj::e_INT>           (Obj::k_INT));
         ASSERT(checkOptionType<Obj::e_INT64>         (Obj::k_INT64));
         ASSERT(checkOptionType<Obj::e_DOUBLE>        (Obj::k_DOUBLE));
         ASSERT(checkOptionType<Obj::e_STRING>        (Obj::k_STRING));
         ASSERT(checkOptionType<Obj::e_DATETIME>      (Obj::k_DATETIME));
         ASSERT(checkOptionType<Obj::e_DATE>          (Obj::k_DATE));
         ASSERT(checkOptionType<Obj::e_TIME>          (Obj::k_TIME));

         ASSERT(checkOptionType<Obj::e_CHAR_ARRAY>    (Obj::k_CHAR_ARRAY));
         ASSERT(checkOptionType<Obj::e_INT_ARRAY>     (Obj::k_INT_ARRAY));
         ASSERT(checkOptionType<Obj::e_INT64_ARRAY>   (Obj::k_INT64_ARRAY));
         ASSERT(checkOptionType<Obj::e_DOUBLE_ARRAY>  (Obj::k_DOUBLE_ARRAY));
         ASSERT(checkOptionType<Obj::e_STRING_ARRAY>  (Obj::k_STRING_ARRAY));
         ASSERT(checkOptionType<Obj::e_DATETIME_ARRAY>(Obj::k_DATETIME_ARRAY));
         ASSERT(checkOptionType<Obj::e_DATE_ARRAY>    (Obj::k_DATE_ARRAY));
         ASSERT(checkOptionType<Obj::e_TIME_ARRAY>    (Obj::k_TIME_ARRAY));
       //^--v
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING ALIASES AND ENUM-TO-TYPE MAPPINGS
        //
        // Concerns:
        //: 1 The type aliases are defined as expected.
        //: 2 The metafunctions produce the expected values.
        //
        // Plan:
        //: 1 Individually test each case.
        //
        // Testing:
        //   CONCERN: Type aliases are defined as expected.
        //   CONCERN: 'OptionType::EnumToType' types
        //   CONCERN: 'OptionType::TypeToEnum' enumerators
        // --------------------------------------------------------------------

        if (verbose) cout
                        << endl
                        << "TESTING ALIASES AND ENUM-TO-TYPE MAPPINGS" << endl
                        << "=========================================" << endl;

#define IS_TDEF(TYPE_ALIAS, TYPE)                                       \
   bsl::is_same<TYPE_ALIAS, TYPE>::value

        if (veryVerbose) cout << "Type aliases are defined as expected."
                              << endl;
//v-----^
  ASSERT((IS_TDEF(void              , void                           ))); //00
  ASSERT((IS_TDEF(Obj::Bool         , bool                           ))); //01
  ASSERT((IS_TDEF(Obj::Char         , char                           ))); //02
  ASSERT((IS_TDEF(Obj::Int          , int                            ))); //03
  ASSERT((IS_TDEF(Obj::Int64        , bsls::Types::Int64             ))); //04
  ASSERT((IS_TDEF(Obj::Double       , double                         ))); //05
  ASSERT((IS_TDEF(Obj::String       , bsl::string                    ))); //06
  ASSERT((IS_TDEF(Obj::Datetime     , bdlt::Datetime                 ))); //07
  ASSERT((IS_TDEF(Obj::Date         , bdlt::Date                     ))); //08
  ASSERT((IS_TDEF(Obj::Time         , bdlt::Time                     ))); //09
  ASSERT((IS_TDEF(Obj::CharArray    , bsl::vector<char>              ))); //10
  ASSERT((IS_TDEF(Obj::IntArray     , bsl::vector<int>               ))); //11
  ASSERT((IS_TDEF(Obj::Int64Array   , bsl::vector<bsls::Types::Int64>))); //12
  ASSERT((IS_TDEF(Obj::DoubleArray  , bsl::vector<double>            ))); //13
  ASSERT((IS_TDEF(Obj::StringArray  , bsl::vector<string>            ))); //14
  ASSERT((IS_TDEF(Obj::DatetimeArray, bsl::vector<bdlt::Datetime>    ))); //15
  ASSERT((IS_TDEF(Obj::DateArray    , bsl::vector<bdlt::Date>        ))); //16
  ASSERT((IS_TDEF(Obj::TimeArray    , bsl::vector<bdlt::Time>        ))); //17
//^-----v
#undef IS_TDEF

        if (veryVerbose) cout <<  "Testing 'OptionType::TypeToEnum'" << endl;

#define IS_ENUM_OK(ENUM, TYPE) ENUM == Obj::TypeToEnum<TYPE>::value
        ASSERT(IS_ENUM_OK(Obj::e_VOID          , void              )); //00
        ASSERT(IS_ENUM_OK(Obj::e_BOOL          , Obj::Bool         )); //01
        ASSERT(IS_ENUM_OK(Obj::e_CHAR          , Obj::Char         )); //02
        ASSERT(IS_ENUM_OK(Obj::e_INT           , Obj::Int          )); //03
        ASSERT(IS_ENUM_OK(Obj::e_INT64         , Obj::Int64        )); //04
        ASSERT(IS_ENUM_OK(Obj::e_DOUBLE        , Obj::Double       )); //05
        ASSERT(IS_ENUM_OK(Obj::e_STRING        , Obj::String       )); //06
        ASSERT(IS_ENUM_OK(Obj::e_DATETIME      , Obj::Datetime     )); //07
        ASSERT(IS_ENUM_OK(Obj::e_DATE          , Obj::Date         )); //08
        ASSERT(IS_ENUM_OK(Obj::e_TIME          , Obj::Time         )); //09
        ASSERT(IS_ENUM_OK(Obj::e_CHAR_ARRAY    , Obj::CharArray    )); //10
        ASSERT(IS_ENUM_OK(Obj::e_INT_ARRAY     , Obj::IntArray     )); //11
        ASSERT(IS_ENUM_OK(Obj::e_INT64_ARRAY   , Obj::Int64Array   )); //12
        ASSERT(IS_ENUM_OK(Obj::e_DOUBLE_ARRAY  , Obj::DoubleArray  )); //13
        ASSERT(IS_ENUM_OK(Obj::e_STRING_ARRAY  , Obj::StringArray  )); //14
        ASSERT(IS_ENUM_OK(Obj::e_DATETIME_ARRAY, Obj::DatetimeArray)); //15
        ASSERT(IS_ENUM_OK(Obj::e_DATE_ARRAY    , Obj::DateArray    )); //16
        ASSERT(IS_ENUM_OK(Obj::e_TIME_ARRAY    , Obj::TimeArray    )); //17

        if (veryVerbose) cout << "Testing 'OptionType::EnumToType'"
                              << endl;

#define IS_CLASS_OK(TYPE, ENUM)                                               \
            bsl::is_same<TYPE,                                                \
                         balcl::OptionType::EnumToType<ENUM>::type>::value
        ASSERT((IS_CLASS_OK(void              , Obj::e_VOID          ))); // 00
        ASSERT((IS_CLASS_OK(Obj::Bool         , Obj::e_BOOL          ))); // 01
        ASSERT((IS_CLASS_OK(Obj::Char         , Obj::e_CHAR          ))); // 02
        ASSERT((IS_CLASS_OK(Obj::Int          , Obj::e_INT           ))); // 03
        ASSERT((IS_CLASS_OK(Obj::Int64        , Obj::e_INT64         ))); // 04
        ASSERT((IS_CLASS_OK(Obj::Double       , Obj::e_DOUBLE        ))); // 05
        ASSERT((IS_CLASS_OK(Obj::String       , Obj::e_STRING        ))); // 06
        ASSERT((IS_CLASS_OK(Obj::Datetime     , Obj::e_DATETIME      ))); // 07
        ASSERT((IS_CLASS_OK(Obj::Date         , Obj::e_DATE          ))); // 08
        ASSERT((IS_CLASS_OK(Obj::Time         , Obj::e_TIME          ))); // 09
        ASSERT((IS_CLASS_OK(Obj::CharArray    , Obj::e_CHAR_ARRAY    ))); // 10
        ASSERT((IS_CLASS_OK(Obj::IntArray     , Obj::e_INT_ARRAY     ))); // 11
        ASSERT((IS_CLASS_OK(Obj::Int64Array   , Obj::e_INT64_ARRAY   ))); // 12
        ASSERT((IS_CLASS_OK(Obj::DoubleArray  , Obj::e_DOUBLE_ARRAY  ))); // 13
        ASSERT((IS_CLASS_OK(Obj::StringArray  , Obj::e_STRING_ARRAY  ))); // 14
        ASSERT((IS_CLASS_OK(Obj::DatetimeArray, Obj::e_DATETIME_ARRAY))); // 15
        ASSERT((IS_CLASS_OK(Obj::DateArray    , Obj::e_DATE_ARRAY    ))); // 16
        ASSERT((IS_CLASS_OK(Obj::TimeArray    , Obj::e_TIME_ARRAY    ))); // 17
#undef IS_CLASS_OK

        if (veryVerbose) cout << "Testing \"Round Trip\"" << endl;

#define IS_IDENTITY(ENUM) \
 ENUM == Obj::TypeToEnum<balcl::OptionType::EnumToType<ENUM>::type>::value
        ASSERT((IS_IDENTITY(Obj::e_VOID          ))); // 00
        ASSERT((IS_IDENTITY(Obj::e_BOOL          ))); // 01
        ASSERT((IS_IDENTITY(Obj::e_CHAR          ))); // 02
        ASSERT((IS_IDENTITY(Obj::e_INT           ))); // 03
        ASSERT((IS_IDENTITY(Obj::e_INT64         ))); // 04
        ASSERT((IS_IDENTITY(Obj::e_DOUBLE        ))); // 05
        ASSERT((IS_IDENTITY(Obj::e_STRING        ))); // 06
        ASSERT((IS_IDENTITY(Obj::e_DATETIME      ))); // 07
        ASSERT((IS_IDENTITY(Obj::e_DATE          ))); // 08
        ASSERT((IS_IDENTITY(Obj::e_TIME          ))); // 09
        ASSERT((IS_IDENTITY(Obj::e_CHAR_ARRAY    ))); // 10
        ASSERT((IS_IDENTITY(Obj::e_INT_ARRAY     ))); // 11
        ASSERT((IS_IDENTITY(Obj::e_INT64_ARRAY   ))); // 12
        ASSERT((IS_IDENTITY(Obj::e_DOUBLE_ARRAY  ))); // 13
        ASSERT((IS_IDENTITY(Obj::e_STRING_ARRAY  ))); // 14
        ASSERT((IS_IDENTITY(Obj::e_DATETIME_ARRAY))); // 15
        ASSERT((IS_IDENTITY(Obj::e_DATE_ARRAY    ))); // 16
        ASSERT((IS_IDENTITY(Obj::e_TIME_ARRAY    ))); // 17
#undef IS_IDENTITY

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING STATIC DATA
        //
        // Concerns:
        //: 1 The static data members (all pointers) are all 0-initialized.
        //
        // Plan:
        //: 1 Examine each data member.
        //
        // Testing:
        //   CONCERN: static data
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING STATIC DATA" << endl
                                  << "===================" << endl;

        ASSERT(0 == Obj::k_BOOL);           // 01
        ASSERT(0 == Obj::k_CHAR);           // 02
        ASSERT(0 == Obj::k_INT);            // 03
        ASSERT(0 == Obj::k_INT64);          // 04
        ASSERT(0 == Obj::k_DOUBLE);         // 05
        ASSERT(0 == Obj::k_STRING);         // 06
        ASSERT(0 == Obj::k_DATETIME);       // 07
        ASSERT(0 == Obj::k_DATE);           // 08
        ASSERT(0 == Obj::k_TIME);           // 09
        ASSERT(0 == Obj::k_CHAR_ARRAY);     // 00
        ASSERT(0 == Obj::k_INT_ARRAY);      // 11
        ASSERT(0 == Obj::k_INT64_ARRAY);    // 12
        ASSERT(0 == Obj::k_DOUBLE_ARRAY);   // 13
        ASSERT(0 == Obj::k_STRING_ARRAY);   // 14
        ASSERT(0 == Obj::k_DATETIME_ARRAY); // 15
        ASSERT(0 == Obj::k_DATE_ARRAY);     // 16
        ASSERT(0 == Obj::k_TIME_ARRAY);     // 17

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING '*ArrayType' METHODS
        //
        // Concerns:
        //: 1 Each of these class methods returns the expected value for all
        //:   possible inputs.
        //
        // Plan:
        //: 1 Compare output to expected results for all possible inputs.
        //
        // Testing:
        //   Enum fromArrayType(Enum arrayType);
        //   bool isArrayType(Enum type);
        //   Enum toArrayType(Enum type);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING '*ArrayType' METHODS" << endl
                                  << "============================" << endl;

        if (veryVerbose) cout << "Testing 'fromArrayType'" << endl;

        static const struct {
            int  d_lineNum;
            Enum d_type;
            Enum d_fromType;
        } DATA_FROM[] = {
        //  LINE  TYPE                   FROM_TYPE
        //  ----  ---------------------  ---------------
          { L_,   Obj::e_VOID          , Obj::e_VOID     }  // 00
        , { L_,   Obj::e_BOOL          , Obj::e_VOID     }  // 01
        , { L_,   Obj::e_CHAR          , Obj::e_VOID     }  // 02
        , { L_,   Obj::e_INT           , Obj::e_VOID     }  // 03
        , { L_,   Obj::e_INT64         , Obj::e_VOID     }  // 04
        , { L_,   Obj::e_DOUBLE        , Obj::e_VOID     }  // 05
        , { L_,   Obj::e_STRING        , Obj::e_VOID     }  // 06
        , { L_,   Obj::e_DATETIME      , Obj::e_VOID     }  // 07
        , { L_,   Obj::e_DATE          , Obj::e_VOID     }  // 08
        , { L_,   Obj::e_TIME          , Obj::e_VOID     }  // 09
        , { L_,   Obj::e_CHAR_ARRAY    , Obj::e_CHAR     }  // 10
        , { L_,   Obj::e_INT_ARRAY     , Obj::e_INT      }  // 11
        , { L_,   Obj::e_INT64_ARRAY   , Obj::e_INT64    }  // 12
        , { L_,   Obj::e_DOUBLE_ARRAY  , Obj::e_DOUBLE   }  // 13
        , { L_,   Obj::e_STRING_ARRAY  , Obj::e_STRING   }  // 14
        , { L_,   Obj::e_DATETIME_ARRAY, Obj::e_DATETIME }  // 15
        , { L_,   Obj::e_DATE_ARRAY    , Obj::e_DATE     }  // 16
        , { L_,   Obj::e_TIME_ARRAY    , Obj::e_TIME     }  // 17
        };

        const bsl::size_t NUM_DATA_FROM = sizeof DATA_FROM / sizeof *DATA_FROM;

        for (bsl::size_t ti = 0; ti < NUM_DATA_FROM; ++ti) {
            const bsl::size_t LINE      = DATA_FROM[ti].d_lineNum;
            const Enum        TYPE      = DATA_FROM[ti].d_type;
            const Enum        FROM_TYPE = DATA_FROM[ti].d_fromType;

            if (veryVerbose) {
                P_(LINE) P_(TYPE) P(FROM_TYPE)
            }

            ASSERT(FROM_TYPE == Obj::fromArrayType(TYPE));
        }

        if (veryVerbose) cout << "Testing 'isArrayType'" << endl;

        static const struct {
            int  d_lineNum;
            Enum d_type;
            bool d_isArray;
        } DATA_IS[] = {
        //  LINE  TYPE                   IS_ARRAY
        //  ----  ---------------------  --------
          { L_,   Obj::e_VOID          , false    }  // 00
        , { L_,   Obj::e_BOOL          , false    }  // 01
        , { L_,   Obj::e_CHAR          , false    }  // 02
        , { L_,   Obj::e_INT           , false    }  // 03
        , { L_,   Obj::e_INT64         , false    }  // 04
        , { L_,   Obj::e_DOUBLE        , false    }  // 05
        , { L_,   Obj::e_STRING        , false    }  // 06
        , { L_,   Obj::e_DATETIME      , false    }  // 07
        , { L_,   Obj::e_DATE          , false    }  // 08
        , { L_,   Obj::e_TIME          , false    }  // 09
        , { L_,   Obj::e_CHAR_ARRAY    , true     }  // 10
        , { L_,   Obj::e_INT_ARRAY     , true     }  // 11
        , { L_,   Obj::e_INT64_ARRAY   , true     }  // 12
        , { L_,   Obj::e_DOUBLE_ARRAY  , true     }  // 13
        , { L_,   Obj::e_STRING_ARRAY  , true     }  // 14
        , { L_,   Obj::e_DATETIME_ARRAY, true     }  // 15
        , { L_,   Obj::e_DATE_ARRAY    , true     }  // 16
        , { L_,   Obj::e_TIME_ARRAY    , true     }  // 17
        };

        const bsl::size_t NUM_DATA_IS = sizeof DATA_IS / sizeof *DATA_IS;

        for (bsl::size_t ti = 0; ti < NUM_DATA_IS; ++ti) {
            const bsl::size_t LINE     = DATA_IS[ti].d_lineNum;
            const Enum        TYPE     = DATA_IS[ti].d_type;
            const bool        IS_ARRAY = DATA_IS[ti].d_isArray;

            if (veryVerbose) {
                P_(LINE) P_(TYPE)  P(IS_ARRAY)
            }

            ASSERT(IS_ARRAY  == Obj::isArrayType(TYPE));
        }

        if (veryVerbose) cout << "Testing 'toArrayType'" << endl;

        static const struct {
            int  d_lineNum;
            Enum d_type;
            Enum d_toType;
        } DATA_TO[] = {
        //  LINE  TYPE                   TO_TYPE
        //  ----  ---------------------  ---------------------
          { L_,   Obj::e_VOID          , Obj::e_VOID           }  // 00
        , { L_,   Obj::e_BOOL          , Obj::e_VOID /* NB */  }  // 01
        , { L_,   Obj::e_CHAR          , Obj::e_CHAR_ARRAY     }  // 02
        , { L_,   Obj::e_INT           , Obj::e_INT_ARRAY      }  // 03
        , { L_,   Obj::e_INT64         , Obj::e_INT64_ARRAY    }  // 04
        , { L_,   Obj::e_DOUBLE        , Obj::e_DOUBLE_ARRAY   }  // 05
        , { L_,   Obj::e_STRING        , Obj::e_STRING_ARRAY   }  // 06
        , { L_,   Obj::e_DATETIME      , Obj::e_DATETIME_ARRAY }  // 07
        , { L_,   Obj::e_DATE          , Obj::e_DATE_ARRAY     }  // 08
        , { L_,   Obj::e_TIME          , Obj::e_TIME_ARRAY     }  // 09
        , { L_,   Obj::e_CHAR_ARRAY    , Obj::e_VOID           }  // 10
        , { L_,   Obj::e_INT_ARRAY     , Obj::e_VOID           }  // 11
        , { L_,   Obj::e_INT64_ARRAY   , Obj::e_VOID           }  // 12
        , { L_,   Obj::e_DOUBLE_ARRAY  , Obj::e_VOID           }  // 13
        , { L_,   Obj::e_STRING_ARRAY  , Obj::e_VOID           }  // 14
        , { L_,   Obj::e_DATETIME_ARRAY, Obj::e_VOID           }  // 15
        , { L_,   Obj::e_DATE_ARRAY    , Obj::e_VOID           }  // 16
        , { L_,   Obj::e_TIME_ARRAY    , Obj::e_VOID           }  // 17
        };

        const bsl::size_t NUM_DATA_TO = sizeof DATA_TO / sizeof *DATA_TO;

        for (bsl::size_t ti = 0; ti < NUM_DATA_TO; ++ti) {
            const bsl::size_t LINE    = DATA_TO[ti].d_lineNum;
            const Enum        TYPE    = DATA_TO[ti].d_type;
            const Enum        TO_TYPE = DATA_TO[ti].d_toType;

            if (veryVerbose) {
                P_(LINE) P_(TYPE) P(TO_TYPE)
            }

            const Enum Result = Obj::toArrayType(TYPE);

            ASSERTV(TYPE, TO_TYPE, Result, TO_TYPE == Result);
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT ('<<') OPERATOR
        //
        // Concerns:
        //: 1 The '<<' operator writes the output to the specified stream.
        //:
        //: 2 The '<<' operator writes the string representation of each
        //:   enumerator in the intended format.
        //:
        //: 3 The '<<' operator writes a distinguished string when passed an
        //:   out-of-band value.
        //:
        //: 4 The output produced by 'stream << value' is the same as that
        //:   produced by 'Obj::print(stream, value, 0, -1)'.
        //:
        //: 5 There is no output when the stream is invalid.
        //:
        //: 6 The '<<' operator has the expected signature.
        //:
        //: 7 The '<<' operator returns the supplied 'ostream'.
        //
        // Plan:
        //: 1 Verify that the '<<' operator produces the expected results for
        //:   each enumerator.  (C-1..2)
        //:
        //: 2 Verify that the '<<' operator writes a distinguished string when
        //:   passed an out-of-band value.  (C-3)
        //:
        //: 3 Verify that 'stream << value' writes the same output as
        //:   'Obj::print(stream, value, 0, -1)'.  (C-4)
        //:
        //: 4 Verify that the address of the returned 'stream' is the same as
        //:   the supplied 'stream'.  (C-7)
        //:
        //: 5 Verify that there is no output when the stream is invalid.  (C-5)
        //:
        //: 6 Take the address of the '<<' (free) operator and use the result
        //:   to initialize a variable of the appropriate type.  (C-6)
        //
        // Testing:
        //   operator<<(ostream& s, OptionType::Enum val);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING OUTPUT ('<<') OPERATOR" << endl
                                  << "==============================" << endl;

// Suppress outside 'enum' range warnings.

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif

        static const struct {
            int         d_lineNum;  // source line number
            Enum        d_value;    // enumerator value
            const char *d_exp_p;    // expected result
        } DATA[] = {
            // line   enumerator value     expected output
            // ----   ----------------     -----------------
            {  L_,    Obj::e_VOID          , "VOID"           }  // 00
          , {  L_,    Obj::e_BOOL          , "BOOL"           }  // 01
          , {  L_,    Obj::e_CHAR          , "CHAR"           }  // 02
          , {  L_,    Obj::e_INT           , "INT"            }  // 03
          , {  L_,    Obj::e_INT64         , "INT64"          }  // 04
          , {  L_,    Obj::e_DOUBLE        , "DOUBLE"         }  // 05
          , {  L_,    Obj::e_STRING        , "STRING"         }  // 06
          , {  L_,    Obj::e_DATETIME      , "DATETIME"       }  // 07
          , {  L_,    Obj::e_DATE          , "DATE"           }  // 08
          , {  L_,    Obj::e_TIME          , "TIME"           }  // 09
          , {  L_,    Obj::e_CHAR_ARRAY    , "CHAR_ARRAY"     }  // 10
          , {  L_,    Obj::e_INT_ARRAY     , "INT_ARRAY"      }  // 11
          , {  L_,    Obj::e_INT64_ARRAY   , "INT64_ARRAY"    }  // 12
          , {  L_,    Obj::e_DOUBLE_ARRAY  , "DOUBLE_ARRAY"   }  // 13
          , {  L_,    Obj::e_STRING_ARRAY  , "STRING_ARRAY"   }  // 14
          , {  L_,    Obj::e_DATETIME_ARRAY, "DATETIME_ARRAY" }  // 15
          , {  L_,    Obj::e_DATE_ARRAY    , "DATE_ARRAY"     }  // 16
          , {  L_,    Obj::e_TIME_ARRAY    , "TIME_ARRAY"     }  // 17

          , {  L_,    static_cast<Enum>(-1), UNKNOWN_FORMAT   }
          , {  L_,    static_cast<Enum>(NUM_ENUMERATORS), UNKNOWN_FORMAT   }

          , {  L_,    static_cast<Enum>(-5)             , UNKNOWN_FORMAT   }
          , {  L_,    static_cast<Enum>(99)             , UNKNOWN_FORMAT   }
        };

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif

        const int   NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting '<<' operator." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int    LINE  = DATA[ti].d_lineNum;
            const Enum   VALUE = DATA[ti].d_value;
            const string EXP   = DATA[ti].d_exp_p;

            if (veryVerbose) { T_; P_(ti); P(VALUE); }
            if (veryVerbose) cout << "EXPECTED FORMAT: " << EXP << endl;

            bslma::TestAllocator scratch("scratch", veryVeryVerbose);
            ostringstream        os(&scratch);

            ASSERTV(LINE, &os == &(os << VALUE));

            if (veryVerbose) cout << "  ACTUAL FORMAT: " << os.str() << endl;

            ASSERTV(LINE, ti, EXP, os.str(), EXP == os.str());

            if (veryVerbose) cout << "\tVerify that the output matches that of"
                                  << " print with level = 0 and spl = -1."
                                  << endl;
            {
                ostringstream out(&scratch);

                Obj::print(out, VALUE, 0, -1);

                ASSERTV(LINE, ti, os.str(), out.str(), os.str() == out.str());
            }
        }

        if (verbose) cout << "\tNothing is written to a bad stream." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int    LINE  = DATA[ti].d_lineNum;
            const Enum   VALUE = DATA[ti].d_value;
            const string EXP   = DATA[ti].d_exp_p;

            if (veryVerbose) { T_; P_(ti); P(VALUE); }

            bslma::TestAllocator scratch("scratch", veryVeryVerbose);
            ostringstream        os(&scratch); os.setstate(ios::failbit);

            ASSERTV(LINE, &os == &(os << VALUE));

            ASSERTV(LINE, ti, os.str(), os.str().empty());
        }

        if (verbose) cout << "\nVerify '<<' operator signature." << endl;

        {
            using namespace balcl;

            typedef bsl::ostream& (*FuncPtr)(bsl::ostream&, Enum);

            const FuncPtr FP = &operator<<;

            (void)FP;   // quash potential compiler warning
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'print'
        //
        // Concerns:
        //: 1 The 'print' method writes the output to the specified stream.
        //:
        //: 2 The 'print' method writes the string representation of each
        //:   enumerator in the intended format taking the 'level' and
        //:   'spacesPerLevel' parameter values into account.
        //:
        //: 3 The optional 'level' and 'spacesPerLevel' parameters have the
        //:   correct default values.
        //:
        //: 4 The 'print' method writes a distinguished string when passed an
        //:   out-of-band value.
        //:
        //: 5 There is no output when the stream is invalid.
        //:
        //: 6 The 'print' method has the expected signature.
        //:
        //: 7 The 'print' method returns the supplied 'ostream'.
        //
        // Plan:
        //: 1 Verify that the 'print' method produces the expected results for
        //:   each enumerator.  (C-1..3)
        //:
        //: 2 Verify that the 'print' method writes a distinguished string when
        //:   passed an out-of-band value.  (C-4)
        //:
        //: 3 Verify that the address of the returned 'stream' is the same as
        //:   the supplied 'stream'.  (C-7)
        //:
        //: 4 Verify that there is no output when the stream is invalid.  (C-5)
        //:
        //: 5 Take the address of the 'print' (class) method and use the
        //:   result to initialize a variable of the appropriate type.  (C-6)
        //
        // Testing:
        //   ostream& print(ostream& s, Enum val, int level = 0, int sPL = 4);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'print'" << endl
                                  << "===============" << endl;

// suppress outside 'enum' range warnings

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif

        static const struct {
            int         d_lineNum;  // source line number
            int         d_level;    // level
            int         d_spl;      // spaces per level
            Enum        d_value;    // enumerator value
            const char *d_exp_p;    // expected result
        } DATA[] = {
#define NL "\n"
            //line level spl enumerator value             expected result
            //---- ----- --- ----------------------       ----------------
            { L_,    0,   4, Obj::e_VOID,                 "VOID"         NL }
          , { L_,    0,   4, Obj::e_INT64,                "INT64"        NL }
          , { L_,    0,   4, Obj::e_DOUBLE,               "DOUBLE"       NL }
          , { L_,    0,   4, Obj::e_STRING,               "STRING"       NL }
          , { L_,    0,   4, Obj::e_DATETIME,             "DATETIME"     NL }
          , { L_,    0,   4, Obj::e_CHAR_ARRAY,           "CHAR_ARRAY"   NL }

          , { L_,    0,   4, static_cast<Enum>(NUM_ENUMERATORS + 1),
                                                          UNKNOWN_FORMAT NL }
          , { L_,    0,   4, static_cast<Enum>(-1),       UNKNOWN_FORMAT NL }
          , { L_,    0,   4, static_cast<Enum>(-5),       UNKNOWN_FORMAT NL }
          , { L_,    0,   4, static_cast<Enum>(99),       UNKNOWN_FORMAT NL }

          , { L_,    0,  -1, Obj::e_INT64,                "INT64"           }
          , { L_,    0,   0, Obj::e_STRING,               "STRING"       NL }
          , { L_,    0,   2, Obj::e_DATETIME,             "DATETIME"     NL }
          , { L_,    1,   1, Obj::e_INT64,                " INT64"       NL }
          , { L_,    1,   2, Obj::e_INT64,                "  INT64"      NL }
          , { L_,   -1,   2, Obj::e_INT64,                "INT64"        NL }
          , { L_,   -2,   1, Obj::e_INT64,                "INT64"        NL }
          , { L_,    2,   1, Obj::e_STRING,               "  STRING"     NL }
          , { L_,    1,   3, Obj::e_STRING,               "   STRING"    NL }
#undef NL
        };

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting 'print'." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int    LINE  = DATA[ti].d_lineNum;
            const int    LEVEL = DATA[ti].d_level;
            const int    SPL   = DATA[ti].d_spl;
            const Enum   VALUE = DATA[ti].d_value;
            const string EXP   = DATA[ti].d_exp_p;

            if (veryVerbose) { T_; P_(ti); P(VALUE); }
            if (veryVerbose) cout << "EXPECTED FORMAT: " << EXP << endl;

            bslma::TestAllocator scratch("scratch", veryVeryVerbose);
            ostringstream        os(&scratch);

            ASSERTV(LINE, &os == &Obj::print(os, VALUE, LEVEL, SPL));
            ASSERTV(LINE, ti, EXP, os.str(), EXP == os.str());

            if (veryVerbose) cout << "  ACTUAL FORMAT: " << os.str() << endl;

            if (0 == LEVEL && 4 == SPL) {
                if (veryVerbose)
                    cout << "\tRepeat for 'print' default arguments." << endl;

                bslma::TestAllocator scratch("scratch", veryVeryVerbose);
                ostringstream        os(&scratch);

                ASSERTV(LINE, &os == &Obj::print(os, VALUE));
                ASSERTV(LINE, ti, EXP, os.str(), EXP == os.str());

                if (veryVerbose) cout << "  ACTUAL FORMAT: " << os.str()
                                      << endl;
            }
        }

        if (verbose) cout << "\tNothing is written to a bad stream." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int    LINE  = DATA[ti].d_lineNum;
            const int    LEVEL = DATA[ti].d_level;
            const int    SPL   = DATA[ti].d_spl;
            const Enum   VALUE = DATA[ti].d_value;
            const string EXP   = DATA[ti].d_exp_p;

            if (veryVerbose) { T_; P_(ti); P_(LEVEL); P_(SPL); P(VALUE); }

            bslma::TestAllocator scratch("scratch", veryVeryVerbose);
            ostringstream        os(&scratch); os.setstate(ios::failbit);

            ASSERTV(LINE, &os == &Obj::print(os, VALUE));
            ASSERTV(LINE, ti, os.str(), os.str().empty());
        }

        if (verbose) cout << "\nVerify 'print' signature." << endl;

        {
            typedef bsl::ostream& (*FuncPtr)(bsl::ostream&, Enum, int, int);

            const FuncPtr FP = &Obj::print;

            (void)FP;   // quash potential compiler warning
        }
      } break;
      case 1: {
        // -------------------------------------------------------------------
        // TESTING 'enum' AND 'toAscii'
        //
        // Concerns:
        //: 1 The enumerator values are sequential, starting from 0.
        //:
        //: 2 The 'toAscii' method returns the expected string representation
        //:   for each enumerator.
        //:
        //: 3 The 'toAscii' method returns a distinguished string when passed
        //:   an out-of-band value.
        //:
        //: 4 The string returned by 'toAscii' is non-modifiable.
        //:
        //: 5 The 'toAscii' method has the expected signature.
        //
        // Plan:
        //: 1 Verify that the enumerator values are sequential, starting from
        //:   0.  (C-1)
        //:
        //: 2 Verify that the 'toAscii' method returns the expected string
        //:   representation for each enumerator.  (C-2)
        //:
        //: 3 Verify that the 'toAscii' method returns a distinguished string
        //:   when passed an out-of-band value.  (C-3)
        //:
        //: 4 Take the address of the 'toAscii' (class) method and use the
        //:   result to initialize a variable of the appropriate type.
        //:   (C-4..5)
        //
        // Testing:
        //   enum Enum { ... };
        //   const char *toAscii(OptionType::Enum val);
        // -------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'enum' AND 'toAscii'" << endl
                                  << "============================" << endl;

// suppress outside 'enum' range warnings

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif

        static const struct {
            int         d_lineNum;  // source line number
            Enum        d_value;    // enumerator value
            const char *d_exp_p;    // expected result
        } DATA[] = {
            // line   enumerator value       expected output
            // ----   ----------------       ----------------
            {  L_,    Obj::e_VOID,           "VOID"            }  // 00
          , {  L_,    Obj::e_BOOL,           "BOOL"            }  // 01
          , {  L_,    Obj::e_CHAR,           "CHAR"            }  // 02
          , {  L_,    Obj::e_INT,            "INT"             }  // 03
          , {  L_,    Obj::e_INT64,          "INT64"           }  // 04
          , {  L_,    Obj::e_DOUBLE,         "DOUBLE"          }  // 05
          , {  L_,    Obj::e_STRING,         "STRING"          }  // 06
          , {  L_,    Obj::e_DATETIME,       "DATETIME"        }  // 07
          , {  L_,    Obj::e_DATE,           "DATE"            }  // 08
          , {  L_,    Obj::e_TIME,           "TIME"            }  // 19
          , {  L_,    Obj::e_CHAR_ARRAY,     "CHAR_ARRAY"      }  // 10
          , {  L_,    Obj::e_INT_ARRAY,      "INT_ARRAY"       }  // 11
          , {  L_,    Obj::e_INT64_ARRAY,    "INT64_ARRAY"     }  // 12
          , {  L_,    Obj::e_DOUBLE_ARRAY,   "DOUBLE_ARRAY"    }  // 13
          , {  L_,    Obj::e_STRING_ARRAY,   "STRING_ARRAY"    }  // 14
          , {  L_,    Obj::e_DATETIME_ARRAY, "DATETIME_ARRAY"  }  // 15
          , {  L_,    Obj::e_DATE_ARRAY,     "DATE_ARRAY"      }  // 16
          , {  L_,    Obj::e_TIME_ARRAY,     "TIME_ARRAY"      }  // 17

          , {  L_,    static_cast<Enum>(-1),                  UNKNOWN_FORMAT }
          , {  L_,    static_cast<Enum>(NUM_ENUMERATORS),     UNKNOWN_FORMAT }

          , {  L_,    static_cast<Enum>(-5),                  UNKNOWN_FORMAT }
          , {  L_,    static_cast<Enum>(99),                  UNKNOWN_FORMAT }
        };

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nVerify enumerator values are sequential."
                          << endl;

        for (int ti = 0; ti < NUM_ENUMERATORS; ++ti) {
            const Enum VALUE = DATA[ti].d_value;

            if (veryVerbose) { T_; P_(ti); P(VALUE); }

            ASSERTV(ti, VALUE, ti == static_cast<int>(VALUE));
        }

        if (verbose) cout << "\nTesting 'toAscii'." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE  = DATA[ti].d_lineNum;
            const Enum  VALUE = DATA[ti].d_value;
            const char *EXP   = DATA[ti].d_exp_p;

            const char *result = Obj::toAscii(VALUE);

            if (veryVerbose) { T_; P_(ti); P_(VALUE); P_(EXP); P(result); }

            ASSERTV(LINE, ti, strlen(EXP) == strlen(result));
            ASSERTV(LINE, ti,           0 == strcmp(EXP, result));
        }

        if (verbose) cout << "\nVerify 'toAscii' signature." << endl;

        {
            typedef const char *(*FuncPtr)(Enum);

            const FuncPtr FP = &Obj::toAscii;

            (void) FP;   // quash potential compiler warning
        }

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    ASSERT(0 == defaultAllocator.numBlocksTotal());
    ASSERT(0 ==  globalAllocator.numBlocksTotal());

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
