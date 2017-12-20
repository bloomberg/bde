// bdlat_valuetypefunctions.t.cpp                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bdlat_valuetypefunctions.h>

#include <bslim_testutil.h>

#include <bdlat_typetraits.h>

#include <bslalg_typetraits.h>

#include <bsls_types.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//                                  TBD doc
//-----------------------------------------------------------------------------
// [ 2] void bdlat_ValueTypeFunctions::reset(TYPE *object);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] USAGE EXAMPLE

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

// ============================================================================
//                       CLASSES AND TYPES FOR TESTING
// ----------------------------------------------------------------------------

enum Enum { E1 = 1, E2 = 2, E3 = 3 };

namespace geom {

class Sequence_Point {

  public:
    BSLALG_DECLARE_NESTED_TRAITS(Sequence_Point, bdlat_TypeTraitBasicSequence);

  private:
    int d_x;  // X coordinate
    int d_y;  // Y coordinate

  public:
    // CREATORS
    Sequence_Point()
    : d_x(0)
    , d_y(0)
    {
    }

    Sequence_Point(const Sequence_Point& original)
    : d_x(original.d_x)
    , d_y(original.d_y)
    {
    }

    ~Sequence_Point()
    {
    }

    // MANIPULATORS
    Sequence_Point& operator=(const Sequence_Point& rhs)
    {
        d_x = rhs.d_x;
        d_y = rhs.d_y;
        return *this;
    }

    void reset()
    {
        d_x = 0;
        d_y = 0;
    }

    int& x()
    {
        return d_x;
    }

    int& y()
    {
        return d_y;
    }

    // ACCESSORS
    int x() const
    {
        return d_x;
    }

    int y() const
    {
        return d_y;
    }
};

class Choice_Point {

  public:
    BSLALG_DECLARE_NESTED_TRAITS(Choice_Point, bdlat_TypeTraitBasicChoice);

  private:
    int d_x;  // X coordinate
    int d_y;  // Y coordinate

  public:
    // CREATORS
    Choice_Point()
    : d_x(0)
    , d_y(0)
    {
    }

    Choice_Point(const Choice_Point& original)
    : d_x(original.d_x)
    , d_y(original.d_y)
    {
    }

    ~Choice_Point()
    {
    }

    // MANIPULATORS
    Choice_Point& operator=(const Choice_Point& rhs)
    {
        d_x = rhs.d_x;
        d_y = rhs.d_y;
        return *this;
    }

    void reset()
    {
        d_x = 0;
        d_y = 0;
    }

    int& x()
    {
        return d_x;
    }

    int& y()
    {
        return d_y;
    }

    // ACCESSORS
    int x() const
    {
        return d_x;
    }

    int y() const
    {
        return d_y;
    }
};

}  // close namespace geom

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

void usageExample()
{
    using namespace BloombergLP;

    int               intVal    = 123;
    float             floatVal  = 34.56;
    bsl::string       stringVal = "Hello";
    bsl::vector<char> vecVal, vecVal2, vecVal3;

    vecVal.push_back('T');
    vecVal.push_back('e');
    vecVal.push_back('s');
    vecVal.push_back('t');

    vecVal2 = vecVal;

    // ASSERT(0 == bdlat_ValueTypeFunctions::assign(&vecVal3, vecVal2));

    bdlat_ValueTypeFunctions::reset(&intVal);
    bdlat_ValueTypeFunctions::reset(&floatVal);
    bdlat_ValueTypeFunctions::reset(&stringVal);
    bdlat_ValueTypeFunctions::reset(&vecVal);

    ASSERT(0 == intVal);
    ASSERT(0 == floatVal);
    ASSERT(stringVal.empty());
    ASSERT(vecVal.empty());

    // ASSERT(vecVal2 == vecVal3);
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
        // USAGE EXAMPLE
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

        usageExample();

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST RESET
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   void bdlat_ValueTypeFunctions::reset(TYPE *object);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'reset'"
                          << "\n===============" << endl;

        {
            Enum mX = E2;
            ASSERT(E1 != mX);
            bdlat_ValueTypeFunctions::reset(&mX);
            // TBD what to ASSERT, if anything
            // ASSERT(E1 == mX);
        }

        {
            bool mX = true;
            ASSERT(false != mX);
            bdlat_ValueTypeFunctions::reset(&mX);
            ASSERT(false == mX);
        }

        {
            char mX = 'a';;
            ASSERT(0 != mX);
            bdlat_ValueTypeFunctions::reset(&mX);
            ASSERT(0 == mX);
        }

        {
            signed char mX = 'a';
            ASSERT(0 != mX);
            bdlat_ValueTypeFunctions::reset(&mX);
            ASSERT(0 == mX);
        }

        {
            unsigned char mX = 'a';
            ASSERT(0 != mX);
            bdlat_ValueTypeFunctions::reset(&mX);
            ASSERT(0 == mX);
        }

        {
            short mX = 77;
            ASSERT(0 != mX);
            bdlat_ValueTypeFunctions::reset(&mX);
            ASSERT(0 == mX);
        }

        {
            unsigned short mX = 77;
            ASSERT(0 != mX);
            bdlat_ValueTypeFunctions::reset(&mX);
            ASSERT(0 == mX);
        }

        {
            int mX = 77;
            ASSERT(0 != mX);
            bdlat_ValueTypeFunctions::reset(&mX);
            ASSERT(0 == mX);
        }

        {
            unsigned int mX = 77;
            ASSERT(0 != mX);
            bdlat_ValueTypeFunctions::reset(&mX);
            ASSERT(0 == mX);
        }

        {
            bsls::Types::Int64  mX = 77;
            ASSERT(0 != mX);
            bdlat_ValueTypeFunctions::reset(&mX);
            ASSERT(0 == mX);
        }

        {
            bsls::Types::Uint64  mX = 77;
            ASSERT(0 != mX);
            bdlat_ValueTypeFunctions::reset(&mX);
            ASSERT(0 == mX);
        }

        {
            float mX = 77;
            ASSERT(0 != mX);
            bdlat_ValueTypeFunctions::reset(&mX);
            ASSERT(0 == mX);
        }

        {
            double mX = 77;
            ASSERT(0 != mX);
            bdlat_ValueTypeFunctions::reset(&mX);
            ASSERT(0 == mX);
        }

        {
            bsl::string mX("foo");  const bsl::string& X = mX;
            ASSERT(0 != X.size());
            bdlat_ValueTypeFunctions::reset(&mX);
            ASSERT(0 == X.size());
        }

        {
            bsl::vector<int> mX;  const bsl::vector<int>& X = mX;
            mX.push_back(77);
            ASSERT(0 != X.size());
            bdlat_ValueTypeFunctions::reset(&mX);
            ASSERT(0 == X.size());
        }

        {
            geom::Sequence_Point mX;  const geom::Sequence_Point& X = mX;
            mX.x() = 77;
            mX.y() = 77;
            ASSERT(0 != X.x() && 0 != X.y());
            bdlat_ValueTypeFunctions::reset(&mX);
            ASSERT(0 == X.x() && 0 == X.y());
        }

        {
            geom::Choice_Point mX;  const geom::Choice_Point& X = mX;
            mX.x() = 77;
            mX.y() = 77;
            ASSERT(0 != X.x() && 0 != X.y());
            bdlat_ValueTypeFunctions::reset(&mX);
            ASSERT(0 == X.x() && 0 == X.y());
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // METHODS TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "METHODS TEST" << endl
                          << "============" << endl;

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
