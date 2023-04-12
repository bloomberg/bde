// bdlat_enumfunctions.t.cpp                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bdlat_enumfunctions.h>

#include <bslim_testutil.h>

#include <bdlat_enumeratorinfo.h>
#include <bdlat_typetraits.h>
#include <bdlb_string.h>

#include <bslalg_typetraits.h>
#include <bslmf_assert.h>
#include <bsls_assert.h>

#include <bsl_cctype.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_iterator.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The interface of the 'bdlat_EnumFunctions' component consists of two traits
// classes and seven free function templates, all in the 'bdlat_EnumFunctions'
// namespace.  These classes and functions are defined to have certain
// behaviors depending on whether the type template parameter is a 'bdlat'
// "basic enumeration", a non-"basic enumeration" declared as a 'bdlat'
// enumeration, or neither, and whether or not the type template parameter
// supports a fallback enumerator.  We will test all relevant combinations.
//-----------------------------------------------------------------------------
// [ 2] template <class E> int fromInt(E*, int)
// [ 2] template <class E> int fromString(E*, const char*, int*)
// [ 2] template <class E> void toInt(int*, const E&)
// [ 2] template <class E> void toString(bsl::string*, const E&)
// [ 3] template <class E> int makeFallback(E*)
// [ 3] template <class E> bool hasFallback(const E&)
// [ 3] template <class E> bool isFallback(const E&)
// [ 1] template <class E> struct IsEnumeration
// [ 1] template <class E> struct HasFallbackEnumerator
//-----------------------------------------------------------------------------
// [ 4] USAGE EXAMPLE

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
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

namespace BEF = bdlat_EnumFunctions;

bool     verbose = false;
bool veryVerbose = false;

// ============================================================================
//                            CLASSES FOR TESTING
// ----------------------------------------------------------------------------

namespace BloombergLP {
namespace test {

// The generated ENUMERATOR_INFO_ARRAY doesn't use the enumeration type, so it
// can't be used to test toInt() and fronInt().  We just make our own version
// instead.  Note that we have to declare this at (named) namespace scope in
// order to satisfy C++03 requirements for template parameters.
template <class ENUM>
struct EnumeratorInfo {
    ENUM        d_enumerator;
    int         d_int_value;
    const char *d_name_p;
};

enum PlainEnum {};

enum PolygonType {
    e_RHOMBUS   = 5,  // rhombus
    e_RECTANGLE = 3,  // rectangle
    e_TRIANGLE  = 1   // triangle
};

const int k_NUM_POLYGON_TYPES = 3;

// FREE FUNCTIONS
int bdlat_enumFromInt(PolygonType* result, int number)
    // Load into the specified 'result' the enumerator matching the specified
    // 'number'.  Return 0 on success, and a non-zero value with no effect on
    // 'result' if 'number' does not match any enumerator.
{
    switch (number) {
      case 5: {
        *result = e_RHOMBUS;
        return 0;                                                     // RETURN
      }
      case 3: {
        *result = e_RECTANGLE;
        return 0;                                                     // RETURN
      }
      case 1: {
        *result = e_TRIANGLE;
        return 0;                                                     // RETURN
      }
      default: {
        return -1;                                                    // RETURN
      }
    }
}

int bdlat_enumFromString(PolygonType *result,
                         const char  *string,
                         int          stringLength)
    // Load into the specified 'result' the enumerator matching the specified
    // 'string' of the specified 'stringLength'.  Return 0 on success, and a
    // non-zero value with no effect on 'result' if 'string' and 'stringLength'
    // do not match any enumerator.
{
    const bsl::string s(string, stringLength);
    if ("RHOMBUS" == s) {
        *result = e_RHOMBUS;
        return 0;                                                     // RETURN
    }
    if ("RECTANGLE" == s) {
        *result = e_RECTANGLE;
        return 0;                                                     // RETURN
    }
    if ("TRIANGLE" == s) {
        *result = e_TRIANGLE;
        return 0;                                                     // RETURN
    }
    return -1;
}

void bdlat_enumToInt(int *result, const PolygonType& value)
    // Load into the specified 'result' the integer representation of the
    // enumerator value held by the specified 'value'.
{
    switch (value) {
      case e_RHOMBUS: {
        *result = 5;
        return;                                                       // RETURN
      }
      case e_RECTANGLE: {
        *result = 3;
        return;                                                       // RETURN
      }
      case e_TRIANGLE: {
        *result = 1;
        return;                                                       // RETURN
      }
    }
    ASSERT(!"invalid enumerator");
}

void bdlat_enumToString(bsl::string *result, const PolygonType& value)
    // Load into the specified 'result' the string representation of the
    // enumerator value held by the specified 'value'.
{
    switch (value) {
      case e_RHOMBUS: {
        *result = "RHOMBUS";
        return;                                                       // RETURN
      }
      case e_RECTANGLE: {
        *result = "RECTANGLE";
        return;                                                       // RETURN
      }
      case e_TRIANGLE: {
        *result = "TRIANGLE";
        return;                                                       // RETURN
      }
    }
    ASSERT(!"invalid enumerator");
}

}  // close namespace test

// TYPE TRAITS
namespace bdlat_EnumFunctions {
template <>
struct IsEnumeration<test::PolygonType> {
    enum { VALUE = 1 };
};
}  // close namespace bdlat_EnumFunctions

// ============================================================================
//                         GLOBAL FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------
namespace {
template <class ENUM, int NUM_ENUMERATORS>
void testToAndFrom(const test::EnumeratorInfo<ENUM> (&info)[NUM_ENUMERATORS])
    // Test the 'fromInt', 'toInt', 'fromString', and 'toString' functions in
    // the 'bdlat_EnumFunctions' namespace for the template parameter type
    // 'ENUM', given the specified 'info' pertaining to 'ENUM'.
{
    for (int i = 0; i < NUM_ENUMERATORS; i++) {
        if (veryVerbose) { T_ P(i) }

        ENUM               mX;  const ENUM&        X = mX;
        int                mN;  const int&         N = mN;
        bsl::string        mS;  const bsl::string& S = mS;

        mX = info[i].d_enumerator;

        BEF::toInt(&mN, X);
        ASSERT(info[i].d_int_value == N);

        BEF::toString(&mS, X);
        ASSERT(info[i].d_name_p == S);

        // 'fromInt'/'fromString' with inputs that don't match any enumerator
        ASSERT(0 != BEF::fromInt(&mX, -1));
        ASSERT(info[i].d_enumerator == X);

        ASSERT(0 != BEF::fromString(&mX, "", 0));
        ASSERT(info[i].d_enumerator == X);

        for (int j = 0; j < NUM_ENUMERATORS; j++) {
            if (veryVerbose) { T_ P_(i) P(j) }

            // Put mX in the 'i'th state, then call 'fromInt' to put it into
            // the 'j'th state.
            mX = info[i].d_enumerator;
            ASSERT(0 == BEF::fromInt(&mX, info[j].d_int_value));
            ASSERT(info[j].d_enumerator == X);

            // Same with 'fromString'.
            mX = info[i].d_enumerator;
            ASSERT(0 == BEF::fromString(
                            &mX,
                            info[j].d_name_p,
                            static_cast<int>(bsl::strlen(info[j].d_name_p))));
            ASSERT(info[j].d_enumerator == X);
        }
    }
}
}  // close unnamed namespace

// ============================================================================
//                   BAS_CODEGEN.PL-GENERATED ENUMERATIONS
// ----------------------------------------------------------------------------
namespace test {

                             // ==================
                             // class PrimaryColor
                             // ==================

struct PrimaryColor {
    // TBD: Provide annotation

  public:
    // TYPES
    enum Value {
        BLUE  = 0
      , GREEN = 1
      , RED   = 2
    };

    enum {
        NUM_ENUMERATORS = 3
            // the number of enumerators in the 'Value' enumeration
    };

    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "PrimaryColor")

    static const bdlat_EnumeratorInfo ENUMERATOR_INFO_ARRAY[];
        // enumerator information for each enumerator

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const char *toString(Value value);
        // Return the string representation exactly matching the enumerator
        // name corresponding to the specified enumeration 'value'.

    static int fromString(Value        *result,
                          const char   *string,
                          int           stringLength);
        // Load into the specified 'result' the enumerator matching the
        // specified 'string' of the specified 'stringLength'.  Return 0 on
        // success, and a non-zero value with no effect on 'result' otherwise
        // (i.e., 'string' does not match any enumerator).

    static int fromInt(Value *result, int number);
        // Load into the specified 'result' the enumerator matching the
        // specified 'number'.  Return 0 on success, and a non-zero value with
        // no effect on 'result' otherwise (i.e., 'number' does not match any
        // enumerator).

    template <class STREAM>
    static STREAM& bdexStreamIn(STREAM&  stream,
                                Value&   value,
                                int      version);
        // Assign to the specified 'value' the value read from the specified
        // input 'stream' using the specified 'version' format and return a
        // reference to the modifiable 'stream'.  If 'stream' is initially
        // invalid, this operation has no effect.  If 'stream' becomes invalid
        // during this operation, the 'value' is valid, but its value is
        // undefined.  If the 'version' is not supported, 'stream' is marked
        // invalid, but 'value' is unaltered.  Note that no version is read
        // from 'stream'.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    static bsl::ostream& print(bsl::ostream& stream, Value value);
        // Write to the specified 'stream' the string representation of the
        // specified enumeration 'value'.  Return a reference to the modifiable
        // 'stream'.

    template <class STREAM>
    static STREAM& bdexStreamOut(STREAM&  stream,
                                 Value    value,
                                 int      version);
        // Write the specified 'value' to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.  Optionally specify
        // an explicit 'version' format; by default, the maximum supported
        // version is written to 'stream' and used as the format.  If 'version'
        // is specified, that format is used but *not* written to 'stream'.  If
        // 'version' is not supported, 'stream' is left unmodified.  (See the
        // package-group-level documentation for more information on 'bdex'
        // streaming of container types).
};

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream& stream, PrimaryColor::Value rhs);
    // Format the specified 'rhs' to the specified output 'stream' and return a
    // reference to the modifiable 'stream'.

}  // close namespace test

// TRAITS
BDLAT_DECL_ENUMERATION_TRAITS(test::PrimaryColor)

namespace test {

                          // ========================
                          // class MyEnumWithFallback
                          // ========================

// This class was originally called 'MyEnumerationWithFallback' (see
// 's_baltst_myenumerationwithfallback.cpp'), but its name has been shortened
// here to make certain lines fit into 79 characters.
struct MyEnumWithFallback {

  public:
    // TYPES
    enum Value {
        UNKNOWN = 0
      , VALUE1  = 1
      , VALUE2  = 2
    };

    enum {
        NUM_ENUMERATORS = 3
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_EnumeratorInfo ENUMERATOR_INFO_ARRAY[];

    // CLASS METHODS
    static const char *toString(Value value);
        // Return the string representation exactly matching the enumerator
        // name corresponding to the specified enumeration 'value'.

    static int fromString(Value        *result,
                          const char   *string,
                          int           stringLength);
        // Load into the specified 'result' the enumerator matching the
        // specified 'string' of the specified 'stringLength'.  Return 0 on
        // success, and a non-zero value with no effect on 'result' otherwise
        // (i.e., 'string' does not match any enumerator).

    static int fromString(Value              *result,
                          const bsl::string&  string);
        // Load into the specified 'result' the enumerator matching the
        // specified 'string'.  Return 0 on success, and a non-zero value with
        // no effect on 'result' otherwise (i.e., 'string' does not match any
        // enumerator).

    static int fromInt(Value *result, int number);
        // Load into the specified 'result' the enumerator matching the
        // specified 'number'.  Return 0 on success, and a non-zero value with
        // no effect on 'result' otherwise (i.e., 'number' does not match any
        // enumerator).

    static int makeFallback(Value *result);
        // Load into the specified 'result' the fallback enumerator value and
        // return 0 to indicate success.

    static bool hasFallback(Value);
        // Return 'true' to indicate that this type supports a fallback
        // enumerator.

    static bool isFallback(Value value);
        // Return 'true' if the specified 'value' equals the fallback
        // enumerator, and 'false' otherwise.

    static bsl::ostream& print(bsl::ostream& stream, Value value);
        // Write to the specified 'stream' the string representation of the
        // specified enumeration 'value'.  Return a reference to the modifiable
        // 'stream'.
};

// FREE FUNCTIONS
inline
bsl::ostream& operator<<(bsl::ostream&             stream,
                         MyEnumWithFallback::Value rhs);
    // Format the specified 'rhs' to the specified output 'stream' and return a
    // reference to the modifiable 'stream'.

}  // close namespace test

// TRAITS
BDLAT_DECL_ENUMERATION_WITH_FALLBACK_TRAITS(test::MyEnumWithFallback)

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace test {

                             // ------------------
                             // class PrimaryColor
                             // ------------------

// CLASS METHODS
inline
int PrimaryColor::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1
}

inline
int PrimaryColor::fromInt(PrimaryColor::Value *result, int number)
{
    enum { SUCCESS = 0, NOT_FOUND = 1 };

    switch (number) {
      case PrimaryColor::BLUE:
      case PrimaryColor::GREEN:
      case PrimaryColor::RED:
        *result = (PrimaryColor::Value)number;
        return SUCCESS;                                               // RETURN
      default:
        return NOT_FOUND;                                             // RETURN
    }
}

inline
bsl::ostream& PrimaryColor::print(bsl::ostream&       stream,
                                  PrimaryColor::Value value)
{
    return stream << toString(value);
}

inline
const char *PrimaryColor::toString(PrimaryColor::Value value)
{
    switch (value) {
      case BLUE: {
        return "BLUE";                                                // RETURN
      } break;
      case GREEN: {
        return "GREEN";                                               // RETURN
      } break;
      case RED: {
        return "RED";                                                 // RETURN
      } break;
      default:
        return "(* INVALID ENUMERATOR *)";                            // RETURN
    }

    return 0;                                                         // RETURN
    BSLS_ASSERT_SAFE(!"unreachable");
}

template <class STREAM>
inline
STREAM& PrimaryColor::bdexStreamIn(STREAM&              stream,
                                   PrimaryColor::Value& value,
                                   int                  version)
{
    switch(version) {
      case 1: {
        int readValue;
        stream.getInt32(readValue);
        if (stream) {
            if (fromInt(&value, readValue)) {
                stream.invalidate();   // bad value in stream
            }
        }
      } break;
      default: {
        stream.invalidate();          // unrecognized version number
      } break;
    }
    return stream;
}

template <class STREAM>
inline
STREAM& PrimaryColor::bdexStreamOut(STREAM&             stream,
                                    PrimaryColor::Value value,
                                    int                 version)
{
    switch (version) {
      case 1: {
        stream.putInt32(value);  // Write the value as an int
      } break;
    }
    return stream;
}

}  // close namespace test

// FREE FUNCTIONS

inline
bsl::ostream& test::operator<<(bsl::ostream&             stream,
                               test::PrimaryColor::Value rhs)
{
    return test::PrimaryColor::print(stream, rhs);
}

namespace test {

                          // ------------------------
                          // class MyEnumWithFallback
                          // ------------------------

int MyEnumWithFallback::fromString(Value              *result,
                                   const bsl::string&  string)
{
    return fromString(
        result, string.c_str(), static_cast<int>(string.length()));
}

bsl::ostream& MyEnumWithFallback::print(bsl::ostream&             stream,
                                        MyEnumWithFallback::Value value)
{
    return stream << toString(value);
}

}  // close namespace test

// FREE FUNCTIONS
inline
bsl::ostream& test::operator<<(bsl::ostream&                   stream,
                               test::MyEnumWithFallback::Value rhs)
{
    return test::MyEnumWithFallback::print(stream, rhs);
}

namespace test {

                             // ------------------
                             // class PrimaryColor
                             // ------------------

// CONSTANTS

const char PrimaryColor::CLASS_NAME[] = "PrimaryColor";
    // the name of this class

const bdlat_EnumeratorInfo PrimaryColor::ENUMERATOR_INFO_ARRAY[] = {
    {
        PrimaryColor::BLUE,
        "BLUE",                 // name
        sizeof("BLUE") - 1,     // name length
        ""  // annotation
    },
    {
        PrimaryColor::GREEN,
        "GREEN",                 // name
        sizeof("GREEN") - 1,     // name length
        ""  // annotation
    },
    {
        PrimaryColor::RED,
        "RED",                 // name
        sizeof("RED") - 1,     // name length
        ""  // annotation
    }
};


// CLASS METHODS

int PrimaryColor::fromString(PrimaryColor::Value *result,
                             const char          *string,
                             int                  stringLength)
{

    enum { SUCCESS = 0, NOT_FOUND = 1 };

    switch(stringLength) {
        case 3: {
            if (bsl::toupper(string[0])=='R'
             && bsl::toupper(string[1])=='E'
             && bsl::toupper(string[2])=='D') {
                *result = PrimaryColor::RED;
                return SUCCESS;                                       // RETURN
            }
        } break;
        case 4: {
            if (bsl::toupper(string[0])=='B'
             && bsl::toupper(string[1])=='L'
             && bsl::toupper(string[2])=='U'
             && bsl::toupper(string[3])=='E') {
                *result = PrimaryColor::BLUE;
                return SUCCESS;                                       // RETURN
            }
        } break;
        case 5: {
            if (bsl::toupper(string[0])=='G'
             && bsl::toupper(string[1])=='R'
             && bsl::toupper(string[2])=='E'
             && bsl::toupper(string[3])=='E'
             && bsl::toupper(string[4])=='N') {
                *result = PrimaryColor::GREEN;
                return SUCCESS;                                       // RETURN
            }
        } break;
    }

    return NOT_FOUND;

}

                          // ------------------------
                          // class MyEnumWithFallback
                          // ------------------------

// CONSTANTS
const char MyEnumWithFallback::CLASS_NAME[] = "MyEnumWithFallback";

const bdlat_EnumeratorInfo MyEnumWithFallback::ENUMERATOR_INFO_ARRAY[] = {
    {MyEnumWithFallback::UNKNOWN,
     "UNKNOWN",
     sizeof("UNKNOWN") - 1,
     ""},
    {MyEnumWithFallback::VALUE1,
     "VALUE1",
     sizeof("VALUE1") - 1,
     ""},
    {MyEnumWithFallback::VALUE2,
     "VALUE2",
     sizeof("VALUE2") - 1,
     ""}
};

// CLASS METHODS
int MyEnumWithFallback::fromInt(MyEnumWithFallback::Value *result,
                                int                        number)
{
    switch (number) {
      case MyEnumWithFallback::UNKNOWN:
      case MyEnumWithFallback::VALUE1:
      case MyEnumWithFallback::VALUE2:
        *result = static_cast<MyEnumWithFallback::Value>(number);
        return 0;                                                     // RETURN
      default:
        return -1;                                                    // RETURN
    }
}

int MyEnumWithFallback::fromString(MyEnumWithFallback::Value *result,
                                   const char                *string,
                                   int                        stringLength)
{
    for (int i = 0; i < 3; ++i) {
        const bdlat_EnumeratorInfo& enumeratorInfo =
            MyEnumWithFallback::ENUMERATOR_INFO_ARRAY[i];

        if (stringLength == enumeratorInfo.d_nameLength &&
            0 == bsl::memcmp(enumeratorInfo.d_name_p, string, stringLength)) {
            *result = static_cast<MyEnumWithFallback::Value>(
                enumeratorInfo.d_value);
            return 0;                                                 // RETURN
        }
    }

    return -1;
}

int MyEnumWithFallback::makeFallback(MyEnumWithFallback::Value *result)
{
    *result = UNKNOWN;
    return 0;
}

const char *MyEnumWithFallback::toString(MyEnumWithFallback::Value value)
{
    switch (value) {
      case UNKNOWN: {
        return "UNKNOWN";                                             // RETURN
      }
      case VALUE1: {
        return "VALUE1";                                              // RETURN
      }
      case VALUE2: {
        return "VALUE2";                                              // RETURN
      }
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bool MyEnumWithFallback::hasFallback(MyEnumWithFallback::Value)
{
    return true;
}

bool MyEnumWithFallback::isFallback(MyEnumWithFallback::Value value)
{
    return value == UNKNOWN;
}

}  // close namespace test
}  // close enterprise namespace

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

namespace BloombergLP {

namespace mine {

enum ImageType {
    JPG     = 0,
    PNG     = 1,
    GIF     = 2,
    UNKNOWN = 100
};
//..
// We can now make 'ImageType' expose "enumeration" behavior by implementing
// all the necessary 'bdlat_enum*' functions for 'ImageType' inside the 'mine'
// namespace (*not* by attempting to declare specializations or overloads in
// the 'bdlat_EnumFunctions' namespace).  First we should forward declare all
// the functions that we will implement inside the 'mine' namespace:
//..
// MANIPULATORS
int bdlat_enumFromInt(ImageType *result, int number);
    // Load into the specified 'result' the enumerator matching the specified
    // 'number'.  Return 0 on success, and a non-zero value with no effect on
    // 'result' if 'number' does not match any enumerator.

int bdlat_enumFromString(ImageType  *result,
                         const char *string,
                         int         stringLength);
    // Load into the specified 'result' the enumerator matching the specified
    // 'string' of the specified 'stringLength'.  Return 0 on success, and a
    // non-zero value with no effect on 'result' if 'string' and 'stringLength'
    // do not match any enumerator.

int bdlat_enumMakeFallback(ImageType *result);
    // Load into the specified 'result' the fallback enumerator value and
    // return 0 to indicate success.

// ACCESSORS
void bdlat_enumToInt(int *result, const ImageType& value);
    // Load into the specified 'result' the integer representation of the
    // enumerator value held by the specified 'value'.

void bdlat_enumToString(bsl::string *result, const ImageType& value);
    // Load into the specified 'result' the string representation of the
    // enumerator value held by the specified 'value'.

bool bdlat_enumHasFallback(const ImageType&);
    // Return 'true' to indicate that this type supports a fallback enumerator.

bool bdlat_enumIsFallback(const ImageType& value);
    // Return 'true' if the specified 'value' equals the fallback enumerator,
    // and 'false' otherwise.
}  // close namespace mine
//..
// Next, we provide the definitions for each of these functions:
//..
// MANIPULATORS

inline
int mine::bdlat_enumFromInt(ImageType *result, int number)
{
    enum { SUCCESS = 0, NOT_FOUND = -1 };

    switch (number) {
      case JPG: {
        *result = JPG;
        return SUCCESS;                                               // RETURN
      }
      case PNG: {
        *result = PNG;
        return SUCCESS;                                               // RETURN
      }
      case GIF: {
        *result = GIF;
        return SUCCESS;                                               // RETURN
      }
      case UNKNOWN: {
        *result = UNKNOWN;
        return SUCCESS;                                               // RETURN
      }
      default: {
        return NOT_FOUND;                                             // RETURN
      }
    }
}

inline
int mine::bdlat_enumFromString(ImageType  *result,
                               const char *string,
                               int         stringLength)
{
    enum { SUCCESS = 0, NOT_FOUND = -1 };

    if (bdlb::String::areEqualCaseless("jpg", string, stringLength)) {
        *result = JPG;
        return SUCCESS;                                               // RETURN
    }

    if (bdlb::String::areEqualCaseless("png", string, stringLength)) {
        *result = PNG;
        return SUCCESS;                                               // RETURN
    }

    if (bdlb::String::areEqualCaseless("gif", string, stringLength)) {
        *result = GIF;
        return SUCCESS;                                               // RETURN
    }

    if (bdlb::String::areEqualCaseless("unknown", string, stringLength)) {
        *result = UNKNOWN;
        return SUCCESS;                                               // RETURN
    }

    return NOT_FOUND;
}

inline
int mine::bdlat_enumMakeFallback(ImageType *result)
{
    *result = UNKNOWN;
    return 0;
}

// ACCESSORS

inline
void mine::bdlat_enumToInt(int *result, const ImageType& value)
{
    *result = static_cast<int>(value);
}

inline
void mine::bdlat_enumToString(bsl::string *result, const ImageType& value)
{
    switch (value) {
      case JPG: {
        *result = "JPG";
      } break;
      case PNG: {
        *result = "PNG";
      } break;
      case GIF: {
        *result = "GIF";
      } break;
      case UNKNOWN: {
        *result = "UNKNOWN";
      } break;
      default: {
        *result = "INVALID";
      } break;
    }
}

inline
bool mine::bdlat_enumHasFallback(const ImageType&)
{
    return true;
}

inline
bool mine::bdlat_enumIsFallback(const ImageType& value)
{
    return value == UNKNOWN;
}
//..
// Finally, we need to specialize the 'IsEnumeration' and
// 'HasFallbackEnumerator' meta-functions in the 'bdlat_EnumFunctions'
// namespace for the 'mine::ImageType' type.  This makes the 'bdlat'
// infrastructure recognize 'ImageType' as an enumeration abstraction with a
// fallback enumerator:
//..
namespace bdlat_EnumFunctions {
template <>
struct IsEnumeration<mine::ImageType> {
    enum { VALUE = 1 };
};
template <>
struct HasFallbackEnumerator<mine::ImageType> {
    enum { VALUE = 1 };
};
}  // close namespace bdlat_EnumFunctions
}  // close enterprise namespace
//..
// The 'bdlat' infrastructure (and any component that uses this infrastructure)
// will now recognize 'ImageType' as an "enumeration" type with a fallback
// enumerator.  For example, suppose we have the following XML data:
//..
//  <?xml version='1.0' encoding='UTF-8' ?>
//  <ImageType>PNG</ImageType>
//..
// Using the 'balxml_decoder' component, we can load this XML data into a
// 'ImageType' object:
//..
//  #include <balxml_decoder.h>
//
//  void decodeImageTypeFromXML(bsl::istream& inputData)
//  {
//      using namespace BloombergLP;
//
//      ImageType object = 0;
//
//      balxml::DecoderOptions options;
//      balxml::MiniReader     reader;
//      balxml::ErrorInfo      errInfo;
//
//      balxml::Decoder decoder(&options, &reader, &errInfo);
//      int result = decoder.decode(inputData, &object);
//
//      assert(0   == result);
//      assert(PNG == object);
//  }
//..
// Note that the 'bdlat' framework can be used for functionality other than
// encoding/decoding into XML.  When 'mine::ImageType' is plugged into the
// framework, then it will be automatically usable within the framework. For
// example, consider the following generic functions that read a string from a
// stream and decode its value into a 'bdlat' "enumeration" object:
//..
template <class TYPE>
int readEnum(bsl::istream& stream, TYPE *object)
{
    bsl::string value;
    stream >> value;

    return bdlat_EnumFunctions::fromString(
        object, value.c_str(), static_cast<int>(value.length()));
}

template <class TYPE>
int readEnumOrFallback(bsl::istream& stream, TYPE *object)
{
    const int rc = readEnum(stream, object);
    return (0 == rc) ? rc : bdlat_EnumFunctions::makeFallback(object);
}
//..
// We can use these generic functions with 'mine::ImageType' as follows:
//..
void usageExample()
{
    using namespace BloombergLP;

    bsl::stringstream ss;
    mine::ImageType   object;

    ss << "JPG\nWEBP\nWEBP\n";

    ASSERT(0             == readEnum(ss, &object));
    ASSERT(mine::JPG     == object);

    ASSERT(0             != readEnum(ss, &object));
    ASSERT(mine::JPG     == object);

    ASSERT(0             == readEnumOrFallback(ss, &object));
    ASSERT(mine::UNKNOWN == object);
}
//..

// ============================================================================
//           ADDITIONAL METADATA FOR ENUMERATIONS IN THIS TEST DRIVER
// ----------------------------------------------------------------------------
namespace BloombergLP {

namespace test {
const EnumeratorInfo<PolygonType> k_POLYGON_TYPE_INFO[k_NUM_POLYGON_TYPES] = {
    {e_RHOMBUS,   5, "RHOMBUS"  },
    {e_RECTANGLE, 3, "RECTANGLE"},
    {e_TRIANGLE,  1, "TRIANGLE" }
};

const EnumeratorInfo<PrimaryColor::Value>
k_PRIMARY_COLOR_INFO[PrimaryColor::NUM_ENUMERATORS] = {
    {PrimaryColor::BLUE,  0, "BLUE" },
    {PrimaryColor::GREEN, 1, "GREEN"},
    {PrimaryColor::RED,   2, "RED"  }
};

const EnumeratorInfo<MyEnumWithFallback::Value>
k_MY_ENUM_WITH_FALLBACK_INFO[MyEnumWithFallback::NUM_ENUMERATORS] = {
    {MyEnumWithFallback::UNKNOWN, 0, "UNKNOWN"},
    {MyEnumWithFallback::VALUE1,  1, "VALUE1"},
    {MyEnumWithFallback::VALUE2,  2, "VALUE2"}
};
}  // close namespace test

namespace mine {
const test::EnumeratorInfo<ImageType> k_IMAGE_TYPE_INFO[4] = {
    {mine::JPG,       0, "JPG"    },
    {mine::PNG,       1, "PNG"    },
    {mine::GIF,       2, "GIF"    },
    {mine::UNKNOWN, 100, "UNKNOWN"}
};
}  // close namespace mine

}  // close enterprise namespace

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
        case 4: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header compiles and
        //:   runs as expected.
        //
        // Plan:
        //: 1 Copy-paste the example and change 'assert' to 'ASSERT'.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING USAGE EXAMPLE" "\n"
                          << "=====================" "\n";

        usageExample();
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING FALLBACK ENUMERATOR FUNCTIONS
        //
        // Concerns:
        //: 1 When 'E' is a "basic enumeration" type without a fallback
        //:   enumerator, the 3 functions below work correctly (i.e., by always
        //:   failing) without any additional machinery having to be provided.
        //: 2 When 'E' has been declared as an "enumeration" type by
        //:   specializing 'IsEnumeration' but has not implemented the
        //:   fallback-related customization points, the 3 functions below work
        //:   correctly (i.e., by always failing) and do not attempt to call
        //:   the non-existent customization point overloads.
        //: 3 When 'E' is a "basic enumeration" type declared to have a
        //:   fallback enumerator, the 3 functions below properly delegate to
        //:   the implementations in the wrapper class for 'E'.
        //: 4 When 'E' has been declared as an "enumeration" type by
        //:   specializing 'IsEnumeration', the 3 functions properly delegate
        //:   to the customization point overloads.
        //
        // Plan:
        //: 1 Call the 3 functions listed below for a "basic enumeration" type
        //:   without a fallback enumerator and verify that they meet their
        //:   contracts.  (C-1)
        //: 2 Call the 3 functions listed below for a non-"basic enumeration"
        //:   type as described in C-2 and verify that they meet their
        //:   contracts.  (C-2)
        //: 3 Call the 3 functions listed below for a type that has been
        //:   declared as a "basic enumeration" with a fallback enumerator, and
        //:   verify that they have the behavior expected of the
        //:   implementations in the wrapper class.  (C-3)
        //: 4 Call the 3 functions listed below for a type as described in
        //:   C-4 and verify that they have the behavior expected of the
        //:   customization point overloads.  (C-4)
        //
        // Testing:
        //   template <class E> int makeFallback(E*)
        //   template <class E> bool hasFallback(const E&)
        //   template <class E> bool isFallback(const E&)
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING FALLBACK ENUMERATOR FUNCTIONS" "\n"
                          << "=====================================" "\n";

        {
            if (veryVerbose) {
                cout << "Testing fallback enumerator functions on "
                        "PrimaryColor\n";
            }
            // 'test::PrimaryColor::Value' is a "basic enumeration".  Note that
            // here, the 'Obj' typedef refers to the enclosing class
            // 'test::PrimaryColor'.
            typedef test::PrimaryColor Obj;
            typedef test::EnumeratorInfo<Obj::Value> InfoType;
            const InfoType (&enumeratorInfo)[Obj::NUM_ENUMERATORS] =
                test::k_PRIMARY_COLOR_INFO;
            for (size_t i = 0; i < bsl::size(enumeratorInfo); i++) {
                if (veryVerbose) { T_ P(i) }
                Obj::Value mX;  const Obj::Value& X = mX;
                mX = test::k_PRIMARY_COLOR_INFO[i].d_enumerator;

                ASSERT(0 != BEF::makeFallback(&mX));
                ASSERT(enumeratorInfo[i].d_enumerator == X);
                ASSERT(!BEF::hasFallback(X));
                ASSERT(!BEF::isFallback(X));
            }
        }

        {
            if (veryVerbose) {
                cout << "Testing fallback enumerator functions on "
                        "PolygonType\n";
            }
            // 'test::Polygon' is not a "basic enumeration".
            typedef test::PolygonType Obj;
            typedef test::EnumeratorInfo<Obj> InfoType;
            const InfoType(&enumeratorInfo)[test::k_NUM_POLYGON_TYPES] =
                test::k_POLYGON_TYPE_INFO;
            for (size_t i = 0; i < bsl::size(enumeratorInfo); i++) {
                if (veryVerbose) { T_ P(i) }
                Obj mX;  const Obj& X = mX;
                mX = enumeratorInfo[i].d_enumerator;

                ASSERT(0 != BEF::makeFallback(&mX));
                ASSERT(enumeratorInfo[i].d_enumerator == X);
                ASSERT(!BEF::hasFallback(X));
                ASSERT(!BEF::isFallback(X));
            }
        }

        {
            if (veryVerbose) {
                cout << "Testing fallback enumerator functions on "
                        "MyEnumWithFallback\n";
            }
            // 'test::MyEnumWithFallback::Value' is a "basic enumeration" with
            // a fallback enumerator.  Note that here, the 'Obj' typedef refers
            // to the enclosing class, 'test::MyEnumWithFallback'.
            typedef test::MyEnumWithFallback Obj;
            typedef test::EnumeratorInfo<Obj::Value> InfoType;
            const InfoType (&enumeratorInfo)[Obj::NUM_ENUMERATORS] =
                test::k_MY_ENUM_WITH_FALLBACK_INFO;
            const Obj::Value k_FALLBACK = Obj::UNKNOWN;

            for (size_t i = 0; i < bsl::size(enumeratorInfo); i++) {
                if (veryVerbose) { T_ P(i) }
                Obj::Value mX;  const Obj::Value& X = mX;
                mX = enumeratorInfo[i].d_enumerator;

                ASSERT(BEF::hasFallback(X));
                ASSERT((X == k_FALLBACK) == BEF::isFallback(X));
                ASSERT(0 == BEF::makeFallback(&mX));
                ASSERT(k_FALLBACK == X);
            }
        }

        {
            if (veryVerbose) {
                cout << "Testing fallback enumerator functions on ImageType\n";
            }
            // 'mine::ImageType' is not a "basic enumeration" but has been
            // declared to be an "enumeration" with a fallback enumerator.
            typedef mine::ImageType Obj;
            typedef test::EnumeratorInfo<Obj> InfoType;
            const InfoType (&enumeratorInfo)[4] = mine::k_IMAGE_TYPE_INFO;
            const Obj        k_FALLBACK         = mine::UNKNOWN;

            for (size_t i = 0; i < bsl::size(enumeratorInfo); i++) {
                if (veryVerbose) { T_ P(i) }
                Obj mX;  const Obj& X = mX;
                mX = enumeratorInfo[i].d_enumerator;

                ASSERT(BEF::hasFallback(X));
                ASSERT((X == k_FALLBACK) == BEF::isFallback(X));
                ASSERT(0 == BEF::makeFallback(&mX));
                ASSERT(k_FALLBACK == X);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING COMMON METHOD FORWARDING
        //
        // Concerns:
        //: 1 When 'E' meets the requirements of a "basic enumeration" type and
        //:   is declared as such, the 4 functions listed below work correctly
        //:   without any additional machinery having to be provided.
        //: 2 When 'E' is not a "basic enumeration" type but has been declared
        //:   as an "enumeration" type by specializing 'IsEnumeration' and
        //:   overloading the 'bdlat_enum'-prefixed customization points, the 4
        //:   functions listed below work correctly by delegating to the
        //:   latter.
        //
        // Plan:
        //: 1 Call the 4 functions listed below for a "basic enumeration" type
        //:   and verify that they meet their contracts.  (C-1)
        //: 2 Call the 4 functions listed below for a non-"basic enumeration"
        //:   type as described above and verify that they meet their
        //:   contracts.  (C-2)
        //
        // Testing:
        //   template <class E> int fromInt(E*, int)
        //   template <class E> int fromString(E*, const char*, int*)
        //   template <class E> void toInt(int*, const E&)
        //   template <class E> void toString(bsl::string*, const E&)
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING COMMON METHOD FORWARDING" "\n"
                          << "================================" "\n";

        if (veryVerbose) {
            cout << "Testing method forwarding on PrimaryColor\n";
        }
        // 'test::PrimaryColor::Value' is a "basic enumeration".  Note that
        // here, the 'Obj' typedef refers to the enclosing class
        // 'test::PrimaryColor'.
        testToAndFrom<test::PrimaryColor::Value>(test::k_PRIMARY_COLOR_INFO);

        if (veryVerbose) {
            cout << "Testing method forwarding on PolygonType\n";
        }
        // 'test::Polygon' is not a "basic enumeration". Also, its enumerator
        // values are not consecutive.
        testToAndFrom<test::PolygonType>(test::k_POLYGON_TYPE_INFO);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING META-FUNCTIONS
        //
        // Concerns:
        //: 1 For a type that has not been declared to be an "enumeration"
        //:   type, the 'IsEnumeration' and 'HasFallbackEnumerator'
        //:   meta-functions yield 'false' (even if it is a language
        //:   enumeration type).
        //: 2 For a type that has been declared to be an "enumeration" type,
        //:   but not declared to have a fallback enumerator, the
        //:   'IsEnumeration' meta-function yields true, while the
        //:   'HasFallbackEnumerator' meta-function yields false.
        //: 3 For a type that has been declared to be an "enumeration" type and
        //:   to have a fallback enumerator, the 'IsEnumeration' and
        //:   'HasFallbackEnumerator' meta-functions yield 'true'.
        //
        // Plan:
        //: 1 Statically assert that the 'IsEnumeration' and
        //:   'HasFallbackEnumerator' meta-functions have the correct values
        //:   for a plain enumeration type that has not been declared to be a
        //:   'bdlat' "enumeration" type. (C-1)
        //: 2 Statically assert that the 'IsEnumeration' and
        //:   'HasFallbackEnumerator' meta-functions have the correct values
        //:   for a type that has been declared to be a 'bdlat' "basic
        //:   enumeration" (but not to have a fallback enumerator).  (C-2)
        //: 3 Statically assert that the 'IsEnumeration' and
        //:   'HasFallbackEnumerator' meta-functions have the correct values
        //:   for a type that has been explicitly declared to be a 'bdlat'
        //:   "enumeration" (but not a "basic enumeration"), and that has not
        //:   been declared to have a fallback enumerator.  (C-2)
        //: 4 Statically assert that the 'IsEnumeration' and
        //:   'HasFallbackEnumerator' meta-functions have the correct values
        //:   for a type that has been declared to be a 'bdlat' "basic
        //:   enumeration" with a fallback enumerator.  (C-3)
        //: 5 Statically assert that the 'IsEnumeration' and
        //:   'HasFallbackEnumerator' meta-functions have the correct values
        //:   for a type that has been explicitly declared to be an
        //:   "enumeration" type with a fallback enumerator (but not a 'bdlat'
        //:   "basic enumeration").  (C-3)
        //
        // Testing:
        //   template <class E> struct IsEnumeration
        //   template <class E> struct HasFallbackEnumerator
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING META-FUNCTIONS" "\n"
                          << "======================" "\n";

        // P-1
        BSLMF_ASSERT(0 == BEF::IsEnumeration<test::PlainEnum>::VALUE);
        BSLMF_ASSERT(0 == BEF::HasFallbackEnumerator<test::PlainEnum>::VALUE);
        // P-2
        BSLMF_ASSERT(1 ==
                     BEF::IsEnumeration<test::PrimaryColor::Value>::VALUE);
        BSLMF_ASSERT(
            0 == BEF::HasFallbackEnumerator<test::PrimaryColor::Value>::VALUE);
        // P-3
        BSLMF_ASSERT(1 == BEF::IsEnumeration<test::PolygonType>::VALUE);
        BSLMF_ASSERT(0 ==
                     BEF::HasFallbackEnumerator<test::PolygonType>::VALUE);
        // P-4
        BSLMF_ASSERT(
            1 == BEF::IsEnumeration<test::MyEnumWithFallback::Value>::VALUE);
        BSLMF_ASSERT(1 == BEF::HasFallbackEnumerator<
                              test::MyEnumWithFallback::Value>::VALUE);
        // P-5
        BSLMF_ASSERT(1 == BEF::IsEnumeration<mine::ImageType>::VALUE);
        BSLMF_ASSERT(1 == BEF::HasFallbackEnumerator<mine::ImageType>::VALUE);
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
// Copyright 2015 Bloomberg Finance L.P.
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
