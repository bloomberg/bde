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
#include <bsls_assert.h>

#include <bsl_cctype.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//                                  TBD doc
//-----------------------------------------------------------------------------
// [ 4] int lookupEnumeratorInfo(bdlat_EnumeratorInfo *info, TYPE value);
// [ 3] struct IsEnumeration
// [ 2] const char *className(TYPE);
// [ 2] int numEnumerators(TYPE);
//-----------------------------------------------------------------------------
// [ 1] METHOD FORWARDING TEST
// [ 2] TESTING META-FUNCTIONS
// [ 5] USAGE EXAMPLE

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

namespace Obj = bdlat_EnumFunctions;

// ============================================================================
//                            CLASSES FOR TESTING
// ----------------------------------------------------------------------------

enum my_Enum {};

// geom_polygontype.h   -*-C++-*-

namespace BloombergLP {
namespace geom {

struct PolygonType {

    // TYPES
    enum Value {
        TRIANGLE  = 0,  // triangle
        RECTANGLE = 1,  // rectangle
        RHOMBUS   = 2   // rhombus
    };

    enum {
        NUM_ENUMERATORS = 3
    };

    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class (i.e., "PolygonType")

    static const bdlat_EnumeratorInfo ENUMERATOR_INFO_ARRAY[];
        // enumerator information for each enumerator

    // CLASS METHODS
    static int fromInt(Value *result, int number);
        // Load into the specified 'result' the enumerator matching the
        // specified 'number'.  Return 0 on success, and a non-zero value with
        // no effect on 'result' if 'number' does not match any enumerator.

    static int fromString(Value      *result,
                          const char *string,
                          int         stringLength);
        // Load into the specified 'result' the enumerator matching the
        // specified 'string' and 'stringLength'.  Return 0 on success, and a
        // non-zero value with no effect on 'result' if 'string' and
        // 'stringLength' do not match any enumerator.

    static const char *toString(Value value);
        // Return the string representation exactly matching the enumerator
        // name corresponding to the specified enumeration 'value'.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

// CLASS METHODS

inline
int PolygonType::fromInt(PolygonType::Value *result, int number)
{
    *result = (PolygonType::Value)number;
    return 0;
}

int PolygonType::fromString(PolygonType::Value *result,
                            const char         *string,
                            int                 stringLength)
{
    *result = RHOMBUS;
    return 0;
}

inline
const char *PolygonType::toString(PolygonType::Value value)
{
    return "rhombus";
}

const char PolygonType::CLASS_NAME[] = "PolygonType";

const bdlat_EnumeratorInfo PolygonType::ENUMERATOR_INFO_ARRAY[] = {
    {
        PolygonType::TRIANGLE,
        "Triangle",                 // name
        sizeof("Triangle") - 1,     // name length
        "TBD: provide annotation"   // annotation
    },
    {
        PolygonType::RECTANGLE,
        "Rectangle",                // name
        sizeof("Rectangle") - 1,    // name length
        "TBD: provide annotation"   // annotation
    },
    {
        PolygonType::RHOMBUS,
        "Rhombus",                  // name
        sizeof("Rhombus") - 1,      // name length
        "TBD: provide annotation"   // annotation
    }
};

}  // close namespace geom

// TYPE TRAITS
template <>
struct bdlat_IsBasicEnumeration<geom::PolygonType::Value> : bsl::true_type
{
};

template <>
struct bdlat_BasicEnumerationWrapper<geom::PolygonType::Value> :
    geom::PolygonType
{
    typedef geom::PolygonType Wrapper;
};

}  // close enterprise namespace

// ============================================================================
//                   BAS_CODEGEN.PL-GENERATED ENUMERATION
// ----------------------------------------------------------------------------
namespace BloombergLP {

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
bsl::ostream& PrimaryColor::print(bsl::ostream&      stream,
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

}  // close enterprise namespace

namespace BloombergLP {
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
                            const char           *string,
                            int                   stringLength)
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

}  // close namespace test
}  // close enterprise namespace


// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

namespace BloombergLP {

namespace mine {

enum MyEnum {
    RED   = 1,
    GREEN = 2,
    BLUE  = 3
};
//..
// We can now make 'MyEnum' expose "enumeration" behavior by implementing
// all the necessary 'bdlat_enum*' functions for 'MyEnum' inside the 'mine
// namespace'.  First we should forward declare all the functions that we
// will implement inside the 'mine' namespace:
//..
    // MANIPULATORS
    int bdlat_enumFromInt(MyEnum *result, int number);
        // Load into the specified 'result' the enumerator matching the
        // specified 'number'.  Return 0 on success, and a non-zero value with
        // no effect on 'result' if 'number' does not match any enumerator.

    int bdlat_enumFromString(MyEnum     *result,
                             const char *string,
                             int         stringLength);
        // Load into the specified 'result' the enumerator matching the
        // specified 'string' of the specified 'stringLength'.  Return 0 on
        // success, and a non-zero value with no effect on 'result' if 'string'
        // and 'stringLength' do not match any enumerator.

    // ACCESSORS
    void bdlat_enumToInt(int *result, const MyEnum& value);
        // Return the integer representation exactly matching the enumerator
        // name corresponding to the specified enumeration 'value'.

    void bdlat_enumToString(bsl::string *result, const MyEnum& value);
        // Return the string representation exactly matching the enumerator
        // name corresponding to the specified enumeration 'value'.

}  // close namespace mine
//..
// Next, we provide the definitions for each of these functions:
//..
// MANIPULATORS

inline
int mine::bdlat_enumFromInt(MyEnum *result, int number)
{
    enum { SUCCESS = 0, NOT_FOUND = -1 };

    switch (number) {
      case RED: {
        *result = RED;

        return SUCCESS;                                               // RETURN
      }
      case GREEN: {
        *result = GREEN;

        return SUCCESS;                                               // RETURN
      }
      case BLUE: {
        *result = BLUE;

        return SUCCESS;                                               // RETURN
      }
      default: {
        return NOT_FOUND;                                             // RETURN
      }
    }
}

inline
int mine::bdlat_enumFromString(MyEnum     *result,
                               const char *string,
                               int         stringLength)
{
    enum { SUCCESS = 0, NOT_FOUND = -1 };

    if (bdlb::String::areEqualCaseless("red", string, stringLength)) {
        *result = RED;

        return SUCCESS;                                               // RETURN
    }

    if (bdlb::String::areEqualCaseless("green", string, stringLength)) {
        *result = GREEN;

        return SUCCESS;                                               // RETURN
    }

    if (bdlb::String::areEqualCaseless("blue", string, stringLength)) {
        *result = BLUE;

        return SUCCESS;                                               // RETURN
    }

    return NOT_FOUND;
}

// ACCESSORS

void mine::bdlat_enumToInt(int *result, const MyEnum& value)
{
    *result = static_cast<int>(value);
}

void mine::bdlat_enumToString(bsl::string *result, const MyEnum &value)
{
    switch (value) {
      case RED: {
        *result = "RED";
      } break;
      case GREEN: {
        *result = "GREEN";
      } break;
      case BLUE: {
        *result = "BLUE";
      } break;
      default: {
        *result = "UNKNOWN";
      } break;
    }
}
//..
// Finally, we need to specialize the 'IsEnum' meta-function in the
// 'bdlat_EnumFunctions' namespace for the 'mine::MyEnum' type.  This makes the
// 'bdeat' infrastructure recognize 'MyEnum' as an enumeration abstraction:
//..
namespace bdlat_EnumFunctions {

    template <>
    struct IsEnumeration<mine::MyEnum> {
        enum { VALUE = 1 };
    };

}  // close namespace bdlat_EnumFunctions
}  // close enterprise namespace
//..
// The 'bdeat' infrastructure (and any component that uses this infrastructure)
// will now recognize 'MyEnum' as an "enumeration" type.  For example, suppose
// we have the following XML data:
//..
//  <?xml version='1.0' encoding='UTF-8' ?>
//  <MyEnum>GREEN</MyEnum>
//..
// Using the 'balxml_decoder' component, we can load this XML data into a
// 'MyEnum' object:
//..
//  #include <balxml_decoder.h>
//
//  void decodeMyEnumFromXML(bsl::istream& inputData)
//  {
//      using namespace BloombergLP;
//
//      MyEnum object = 0;
//
//      balxml::DecoderOptions options;
//      balxml::MiniReader     reader;
//      balxml::ErrorInfo      errInfo;
//
//      balxml::Decoder decoder(&options, &reader, &errInfo);
//      int result = decoder.decode(inputData, &object);
//
//      assert(0     == result);
//      assert(GREEN == object);
//  }
//..
// Note that the 'bdeat' framework can be used for functionality other than
// encoding/decoding into XML.  When 'mine::MyEnum' is plugged into the
// framework, then it will be automatically usable within the framework.
// For example, the following snippets of code will convert a string from a
// stream and load it into a 'mine::MyEnum' object:
//..
template <class TYPE>
int readMyEnum(bsl::istream& stream, TYPE *object)
{
    bsl::string value;
    stream >> value;

    return bdlat_enumFromString(object, value.c_str(), value.length());
}
//..
// Now we have a generic function that takes an input stream and a 'Cusip'
// object, and inputs its value.  We can use this generic function as
// follows:
//..
void usageExample()
{
    using namespace BloombergLP;

    bsl::stringstream ss;
    mine::MyEnum object;

    ss << "GREEN" << bsl::endl << "BROWN" << bsl::endl;

    ASSERT(0           == readMyEnum(ss, &object));
    ASSERT(mine::GREEN == object);

    ASSERT(0           != readMyEnum(ss, &object));
}
//..
//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
//  int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
        case 4: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   That the usage example compiles
        //   and runs as expected.
        //
        // Plan:
        //   Copy-paste the example and change
        //   the assert into ASSERT.
        //
        // Testing:
        //   Usage Example
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

        usageExample();
      } break;

      case 3: {
        // --------------------------------------------------------------------
        // TESTING CONVERSION TO STRING
        //
        // Concerns:
        //   An invalid enumerated value should be convertible to a string.
        //   The string representation of an invalid enum should not
        //   match the string representation of any of the valid enumerators.
        //
        // Testing:
        //   bdlat_enumToString
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Conversion to String"
                          << "\n============================" << endl;

        // bas_codegen.pl-generated enumerations
        test::PrimaryColor::Value c1 = test::PrimaryColor::RED;
        test::PrimaryColor::Value c2 = (test::PrimaryColor::Value)-1;

        bsl::string c1string, c2string;
        bdlat_EnumFunctions::toString(&c1string, c1);
        bdlat_EnumFunctions::toString(&c2string, c2);

        if (veryVerbose) {
            bsl::cout << "c1 = " << c1string << bsl::endl;
            bsl::cout << "c2 = " << c2string << bsl::endl;
        }

        ASSERT("RED" == c1string);
        ASSERT((c2string != "RED") &&
               (c2string != "GREEN") &&
               (c2string != "BLUE"));
      } break;

      case 2: {
        // --------------------------------------------------------------------
        // TESTING META-FUNCTIONS
        //
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   struct IsEnumeration
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting meta-functions"
                          << "\n======================" << endl;

        ASSERT(0 == Obj::IsEnumeration<int>::VALUE);
        ASSERT(1 == Obj::IsEnumeration<geom::PolygonType::Value>::VALUE);

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING METHOD FORWARDING
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Method Forwarding" << endl
                          << "=========================" << endl;

        using namespace geom;

        PolygonType::Value mX;  const PolygonType::Value& X = mX;
        int mN;  const int& N = mN;

        mX = PolygonType::RHOMBUS;   ASSERT(X == PolygonType::RHOMBUS);
        mN = PolygonType::TRIANGLE;  ASSERT(N == PolygonType::TRIANGLE);

        ASSERT(0 == bdlat_EnumFunctions::fromInt(&mX, N));
                                     ASSERT(X == PolygonType::TRIANGLE);
                                     ASSERT(N == PolygonType::TRIANGLE);

        const char *const S = "rhombus";
        ASSERT(0 == bdlat_EnumFunctions::fromString(&mX, S, 7));
                                     ASSERT(X == PolygonType::RHOMBUS);

        bsl::string toString;
        bdlat_EnumFunctions::toString(&toString, X);
        ASSERT(toString == "rhombus");
        ASSERT(X        == PolygonType::RHOMBUS);

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
