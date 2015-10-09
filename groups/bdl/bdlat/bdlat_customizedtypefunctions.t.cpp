// bdlat_customizedtypefunctions.t.cpp                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bdlat_customizedtypefunctions.h>

#include <bslim_testutil.h>

#include <bdlat_typetraits.h>
#include <bslalg_typetraits.h>
#include <bslma_allocator.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iosfwd.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#include <bsls_assert.h>

#include <bdlb_printmethods.h>


using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//                                  TBD doc
//-----------------------------------------------------------------------------
// [ 1] METHOD FORWARDING TEST
// [ 2] TESTING META-FUNCTIONS
// [ 3] USAGE EXAMPLE
//-----------------------------------------------------------------------------

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
//                            CLASSES FOR TESTING
// ----------------------------------------------------------------------------

static int globalFlag = 0;

// mine_cusip.h   -*-C++-*-
#ifndef INCLUDED_GEOM_CUSIP
#define INCLUDED_GEOM_CUSIP

//@PURPOSE: TBD: provide purpose.
//
//@CLASSES:
//  Cusip: cusip class
//
//@DESCRIPTION:
//  Identification number for the US and Canada.  'Cusip' is a 9-digit number
//  consisting of 8 digits and a check digit.  The Bloomberg ID will be
//  returned for Corp, Govt, Pfd if a CUSIP is not available.

namespace BloombergLP {

namespace mine {

class Cusip {

  private:
    // PRIVATE DATA MEMBERS
    bsl::string d_value;  // stored value

    // FRIENDS
    friend bool operator==(const Cusip& lhs, const Cusip& rhs);
    friend bool operator!=(const Cusip& lhs, const Cusip& rhs);

  public:
    // TYPES
    typedef bsl::string BaseType;

    // CREATORS
    explicit Cusip(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Cusip' having the default value.  Use the
        // optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Cusip(const Cusip& original, bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Cusip' having the value of the specified
        // 'original' object.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    explicit Cusip(const bsl::string&  value,
                   bslma::Allocator   *basicAllocator = 0);
        // Create an object of type 'Cusip' having the specified 'value'.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    ~Cusip();
        // Destroy this object.

    // MANIPULATORS
    Cusip& operator=(const Cusip& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    void reset();
        // Reset this object to the default value (i.e., its value upon default
        // construction).

    int fromString(const bsl::string& value);
        // Convert from the specified 'value' to this type.  Return 0 if
        // successful and non-zero otherwise.

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.

    const bsl::string& toString() const;
        // Convert this value to 'bsl::string'.
};

// FREE OPERATORS
inline
bool operator==(const Cusip& lhs, const Cusip& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const Cusip& lhs, const Cusip& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Cusip& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and return a
    // reference to the modifiable 'stream'.

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

// CREATORS

inline
Cusip::Cusip(bslma::Allocator *basicAllocator)
: d_value(basicAllocator)
{
}

inline
Cusip::Cusip(const Cusip& original, bslma::Allocator *basicAllocator)
: d_value(original.d_value, basicAllocator)
{
}

inline
Cusip::Cusip(const bsl::string& value, bslma::Allocator *basicAllocator)
: d_value(value, basicAllocator)
{
}

inline
Cusip::~Cusip()
{
}

// MANIPULATORS

inline
Cusip& Cusip::operator=(const Cusip& rhs)
{
    d_value = rhs.d_value;
    return *this;
}

inline
void Cusip::reset()
{
    d_value.erase();
}

inline
int Cusip::fromString(const bsl::string& value)
{
    enum { SUCCESS = 0, FAILURE = -1 };

    globalFlag = 1;

    if (9 < value.size()) {
        return FAILURE;                                               // RETURN
    }

    d_value = value;

    return SUCCESS;
}

// ACCESSORS

inline
bsl::ostream& Cusip::print(bsl::ostream& stream,
                           int           level,
                           int           spacesPerLevel) const
{
    return bdlb::PrintMethods::print(stream, d_value, level, spacesPerLevel);
}

inline
const bsl::string& Cusip::toString() const
{
    globalFlag = 2;

    return d_value;
}

}  // close namespace mine

// TRAITS

BDLAT_DECL_CUSTOMIZEDTYPE_WITH_ALLOCATOR_TRAITS(mine::Cusip)

// FREE OPERATORS

inline
bool mine::operator==(const mine::Cusip& lhs, const mine::Cusip& rhs)
{
    return lhs.d_value == rhs.d_value;
}

inline
bool mine::operator!=(const mine::Cusip& lhs, const mine::Cusip& rhs)
{
    return lhs.d_value != rhs.d_value;
}

inline
bsl::ostream& mine::operator<<(bsl::ostream& stream, const mine::Cusip& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
//                       *End-of-File Block removed.*
// ----------------------------------------------------------------------------

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

namespace Obj = bdlat_CustomizedTypeFunctions;
typedef BloombergLP::mine::Cusip Cusip;

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

namespace BloombergLP {
namespace bdlat_CustomizedTypeFunctions {

    template <>
    struct IsCustomizedType<mine::Cusip> {
        enum { VALUE = 1 };
    };

}  // close namespace bdlat_CustomizedTypeFunctions
}  // close enterprise namespace

template <class TYPE>
int readCusip(bsl::istream& stream, TYPE *object)
{
    bsl::string value;
    stream >> value;

    return bdlat_CustomizedTypeFunctions::convertFromBaseType(object, value);
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
//  int veryVerbose = argc > 3;
//  int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
        case 3: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   That the usage example compiles and runs as expected.
        //
        // Plan:
        //   Copy-paste the example and change the assert into ASSERT.
        //
        // Testing:
        //   Usage Example
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

        {
          bsl::stringstream ss;
          mine::Cusip object;

          ss << "281C82UE\n1234567890\n";

          ASSERT(0          == readCusip(ss, &object));
          ASSERT("281C82UE" == object.toString());

          ASSERT(0          != readCusip(ss, &object));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING META-FUNCTIONS
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   struct IsCustomizedType
        //   struct BaseType
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting meta-functions"
                          << "\n======================" << endl;

        ASSERT(0 == Obj::IsCustomizedType<bsl::string>::VALUE);
        ASSERT(1 == Obj::IsCustomizedType<Cusip>::VALUE);

        typedef Obj::BaseType<Cusip>::Type BaseType;
        ASSERT(1 == (bslmf::IsSame<BaseType, bsl::string>::VALUE));

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // METHOD FORWARDING TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "METHOD FORWARDING TEST" << endl
                          << "======================" << endl;

        Cusip mV;  const Cusip& V = mV;

        const bsl::string dummyValue = "dummy";

        globalFlag = 0;
        ASSERT(0 == bdlat_CustomizedTypeFunctions::convertFromBaseType(
                                                                  &mV,
                                                                  dummyValue));
        ASSERT(1 == globalFlag);

        globalFlag = 0;
        const bsl::string& temp =
                           bdlat_CustomizedTypeFunctions::convertToBaseType(V);
        ASSERT(dummyValue == temp);
        ASSERT(2          == globalFlag);

        globalFlag = 0;
        bsl::string temp2;
        temp2 = bdlat_CustomizedTypeFunctions::convertToBaseType(V);
        ASSERT(dummyValue == temp2);
        ASSERT(2          == globalFlag);

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
