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
#include <bsl_sstream.h> // 'bsl::ostringstream'
#include <bsl_string.h>
#include <bsl_utility.h> // 'bsl::pair', 'bsl::make_pair'
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

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Implicit "Value Type"
/// - - - - - - - - - - - - - - - -
// Suppose you had a type that defines a "value".
//..
    namespace BloombergLP {
    namespace mine {

    struct MyValueType {
        int    d_int;
        double d_double;
    };

    }  // close package namespace
    }  // close enterprise namespace
//..
// Although our definition of 'MyValueType' was rather terse, several methods
// are implicitly defined by the compiler:
//..
    void f()
    {
        using namespace BloombergLP;

        mine::MyValueType a = { 1, 1.0 };  // aggregate braced initialization
        mine::MyValueType b(a);            // implicit copy constructor

        ASSERT(b.d_int    == a.d_int);
        ASSERT(b.d_double == a.d_double);

        a.d_int    = 2;
        a.d_double = 3.14;

        b = a;                             // implicit copy assignment operator

        ASSERT(b.d_int    == a.d_int);
        ASSERT(b.d_double == a.d_double);
    }
//..
// Notice that the implicitly defined methods include a copy constructor and a
// copy assignment operator thereby implicitly making 'MyValueType' part of the
// 'bdlat' "value" framework.  As such, it can be manipulated using the methods
// of 'bdlat_ValueTypeFunctions':
//..
    void myUsageScenario()
    {
        using namespace BloombergLP;

        mine::MyValueType x = {  7, 10.0 };
        mine::MyValueType y = { 99, -1.0 };

        ASSERT(x.d_int    != y.d_int);
        ASSERT(x.d_double != y.d_double);

        int rc = bdlat_ValueTypeFunctions::assign(&x, y);
        ASSERT(0 == rc);

        ASSERT(x.d_int    == y.d_int);
        ASSERT(x.d_double == y.d_double);

        bdlat_ValueTypeFunctions::reset(&y);

        ASSERT(x.d_int    != y.d_int);
        ASSERT(x.d_double != y.d_double);

        ASSERT(int()    == y.d_int);
        ASSERT(double() == y.d_double);
    }
//..
//
///Example 2: Interacting with Other Types
///- - - - - - - - - - - - - - - - - - - -
// Suppose you want to enhance 'mine::MyValueType' to allow its value to be
// assigned from a 'bsl::pair<int, float>' object?  Do do so, create
// 'your::YourValueType' which has an implicit conversion from
// 'bsl::pair<int, float>':
//..
    namespace BloombergLP {
    namespace your {

    struct YourValueType {

        int    d_int;
        double d_double;

        YourValueType()
        : d_int()
        , d_double() { }

        YourValueType(const YourValueType& original)
        : d_int   (original.d_int)
        , d_double(original.d_double) { }

        YourValueType(int intValue, double doubleValue)
        : d_int   (   intValue)
        , d_double(doubleValue) { }

        YourValueType(const bsl::pair<int, double>& value) // IMPLICIT
        : d_int   (value.first)
        , d_double(value.second) { }

        YourValueType & operator=(const YourValueType& original) {
            d_int    = original.d_int;
            d_double = original.d_double;
            return *this;
        }
    };

    }  // close package namespace
    }  // close enterprise namespace
//..
// Notice that, having defined a constructor, the compiler no longer generates
// the constructors that had been generated implicitly.  Accordingly, we have
// added a default constructor, copy constructor and assignment operator.
// Also, since aggregate initialization is no longer allowed, we have also
// added a value constructor and slightly modified the syntax of initialization
// in function 'g()' below:
//..
    void g()
    {
        using namespace BloombergLP;

        your::YourValueType a(1, 1.0);     // value initialization
        your::YourValueType b(a);          // implicit copy constructor

        ASSERT(b.d_int    == a.d_int);
        ASSERT(b.d_double == a.d_double);

        a.d_int    = 2;
        a.d_double = 3.14;

        b = a;                             // implicit copy assignment operator

        ASSERT(b.d_int    == a.d_int);
        ASSERT(b.d_double == a.d_double);

        bsl::pair<int, double> value(4, 5.0);

        a = value;

        ASSERT(4   == a.d_int);
        ASSERT(5.0 == a.d_double);
    }
//..
// Since both copy construction and assignment are defined, 'YourValueType' can
// be handled by the 'bdlat' "value" infrastructure in much the same way as we
// did for 'MyValueType':
//..
    void yourUsageScenario()
    {
        using namespace BloombergLP;
        int rc;

        your::YourValueType x( 7, 10.0);
        your::YourValueType y(99, -1.0);

        ASSERT(x.d_int    != y.d_int);
        ASSERT(x.d_double != y.d_double);

        rc = bdlat_ValueTypeFunctions::assign(&x, y);
        ASSERT(0 == rc);

        ASSERT(x.d_int    == y.d_int);
        ASSERT(x.d_double == y.d_double);

        bdlat_ValueTypeFunctions::reset(&y);

        ASSERT(x.d_int    != y.d_int);
        ASSERT(x.d_double != y.d_double);

        ASSERT(int()   == y.d_int);
        ASSERT(float() == y.d_double);
//..
// However, since conversion from another type, 'bsl::pair<int, double>', is
// provided, the 'bdlat' "value" infrastructure can also use that type to set
// the value of objects.
//..
        bsl::pair<int, double> value(4, 5.0);

        rc = bdlat_ValueTypeFunctions::assign(&y, value);
        ASSERT(0 == rc);

        ASSERT(value.first  == y.d_int);
        ASSERT(value.second == y.d_double);
//..
// Unsurprisingly, such assignments do not work for arbitrary other types (for
// which conversion is not defined).  What is notable, is that this code does
// compile and fails at run-time.
//..
        // Assign an incompatible type.
        rc = bdlat_ValueTypeFunctions::assign(&y, bsl::string("4, 5.0"));
        ASSERT(0 != rc);
    }
//..
//
///Installing an Atypical "Value" Type
///- - - - - - - - - - - - - - - - - -
// Suppose someone defines a pernicious "value" type, 'their::TheirValueType',
// having neither copy constructor nor copy assignment operator:
//..
    namespace BloombergLP {
    namespace their {

    class TheirValueType {

        // DATA
        int    d_int;
        double d_double;

      private:
        // NOT IMPLEMENTED
        TheirValueType(const TheirValueType& original);   // = delete
        TheirValueType& operator=(const TheirValueType&); // = delete

      public:
        // CREATORS
        TheirValueType()
        : d_int()
        , d_double() { }

        // MANIPULATORS
        void setValue(const bsl::string& valueString);

        // ACCESSORS
        int       intValue() const { return d_int;    }
        double doubleValue() const { return d_double; }
    };

    // MANIPULATORS
    void TheirValueType::setValue(const bsl::string& valueString)
    {
         bsl::string::size_type pos = valueString.find(',');
         BSLS_ASSERT(bsl::string::npos != pos);

         d_int    = bsl::atoi(valueString.c_str());
         d_double = bsl::atof(valueString.c_str() + pos + 1);
    }

    }  // close package namespace
    }  // close enterprise namespace
//..
// Such a type can be used after a fashion (objects created, states changed,
// state changes observed), albeit using syntax that is significantly different
// than we used for 'MyValueType' and 'YourValueType':
//..
    void h()
    {
        using namespace BloombergLP;

        their::TheirValueType a;               // default constructor

        ASSERT(0   == a.   intValue());
        ASSERT(0.0 == a.doubleValue());

    //  their::TheirValueType b(a);       // Error, no copy constructor

        their::TheirValueType c;
    //  c = a;                            // Error, no copy assignment operator

        a.setValue("2, 3.14");

        ASSERT(2    == a.   intValue());
        ASSERT(3.14 == a.doubleValue());
    }
//..
// Since 'TheirValueType' lacks both copy construction and assignment, that
// type is not implicitly supported by the 'bdlat' "value" infrastructure.
//
// However, the 'TheirValueType' can be made compatible with that
// infrastructure if "they" define the required overloads of
// 'bdlat_valueTypeAssign' and 'bdlat_valueTypeReset' in 'their' namespace:
//..
    namespace BloombergLP {
    namespace their {

    int bdlat_valueTypeAssign(TheirValueType        *lhs,
                              const TheirValueType&  rhs)
    {
        BSLS_ASSERT(lhs);

        bsl::ostringstream oss;
        oss << rhs.intValue() << ", " << rhs.doubleValue();

        lhs->setValue(oss.str());
        return 0;
    }

    int bdlat_valueTypeAssign(TheirValueType     *lhs,
                              const bsl::string&  rhs)
    {
        BSLS_ASSERT(lhs);

        lhs->setValue(rhs);
        return 0;
    }

    // Overload for any other 'RHS_TYPE' to return an error.
    template <class RHS_TYPE>
    int bdlat_valueTypeAssign(TheirValueType  *lhs,
                              const RHS_TYPE&  rhs)
    {
        BSLS_ASSERT(lhs);
        (void)lhs;
        (void)rhs;

        return -999;  // Pick a distinctive non-negative value.
    }
    void bdlat_valueTypeReset(TheirValueType *object)
    {
        BSLS_ASSERT(object);

        bsl::ostringstream oss;
        oss << int() << ", " << double();

        object->setValue(oss.str());
    }

    }  // close package namespace
    }  // close enterprise namespace
//..
// Notice that three overloads of 'bdlat_valueTypeAssign' are defined above:
//
//: o The first, the overload that allows 'TheirValueType' to be "assigned" to
//:   itself is required by the 'bdlat' "value" infrastructure.
//:
//: o The second, the overload that allows "assignment" from a 'bsl::string' is
//:   not technically required by the infrastructure, but is a practical
//:   requirement because 'bsl::string' is the only way 'TheirValueType' can be
//:   changed from its default value.
//:
//: o Finally, we provide an overload templated on an arbitrary 'RHS_TYPE so
//:   that, if any other types are passed, the code will compile (as required)
//:   but also unconditionally fail (as required).
//
// With these points of customization in place, 'TheirValueType' can now be
// manipulated by the 'bdlat' "value" infrastructure in much the same manner as
// was done for 'MyValueType' and 'YourValueType':
//..
    void theirUsageScenario()
    {
        using namespace BloombergLP;

        their::TheirValueType x;
        their::TheirValueType y;

        int rc;

        rc = bdlat_ValueTypeFunctions::assign(&x, bsl::string(" 7, 10.0"));
        ASSERT(0 == rc);

        rc = bdlat_ValueTypeFunctions::assign(&y, bsl::string("99, -1.0"));
        ASSERT(0 == rc);

        ASSERT(x.intValue()    != y.intValue());
        ASSERT(x.doubleValue() != y.doubleValue());

        rc = bdlat_ValueTypeFunctions::assign(&x, y);
        ASSERT(0 == rc);

        ASSERT(x.intValue()    == y.intValue());
        ASSERT(x.doubleValue() == y.doubleValue());

        bdlat_ValueTypeFunctions::reset(&y);

        ASSERT(int()   == y.intValue());
        ASSERT(float() == y.doubleValue());

        // Assign an incompatible type.

        bsl::pair<int, double> value(4, 5.0);
        rc = bdlat_ValueTypeFunctions::assign(&y, value);
        ASSERT(   0 != rc);
        ASSERT(-999 == rc);
    }
//..

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

        f();    myUsageScenario();
        g();  yourUsageScenario();
        h(); theirUsageScenario();

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
