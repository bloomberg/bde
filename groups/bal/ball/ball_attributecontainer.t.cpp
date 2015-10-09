// ball_attributecontainer.t.cpp                                      -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <ball_attributecontainer.h>

#include <ball_attribute.h>             // for testing only

#include <bslim_testutil.h>

#include <bslim_printer.h>
#include <bsls_assert.h>
#include <bsls_protocoltest.h>
#include <bsls_types.h>

#include <bsl_cstdlib.h>                      // atoi()
#include <bsl_cstring.h>                      // strcmp(), memcmp(), memcpy()
#include <bsl_iostream.h>
#include <bsl_set.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The enumeration under test is non-standard in that the enumeration values
// are non-consecutive.  Therefore, the standard enumeration test plan is
// modified where necessary to accommodate the non-consecutive nature of this
// enumeration.  Note that the set of enumerator values under test are defined
// by { n << 5 : 1 <= n <= 7 }, so expressions similar to "n << 5" occur
// regularly in the test cases.
//-----------------------------------------------------------------------------
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
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef ball::AttributeContainer Obj;

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Example 1: An Implementation of 'ball::AttributeContainer'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In the following example we develop a 'ball::AttributeContainer'
// implementation specifically intended for a service offline that will perform
// rule-based logging based on a service client's bloomberg "uuid", "luw",
// and "firm number".  We define the class 'ServiceAttributes' that contains 3
// integer 'ball::Attribute' values with the names "uuid", "luw", and
// "firmNumber".
//
// Note that this implementation requires no memory allocation, so it will be
// more efficient than a more general set-based implementation if the container
// is frequently created, destroyed, or modified.  We will develop a
// 'ball::AttributeContainer' implementation that can hold any
// 'ball::Attribute' value in example 2 (and one is provided by the 'ball'
// package in the 'ball_defaultattributecontainer' component).
//..
      // serviceattributes.h

    class ServiceAttributes : public ball::AttributeContainer {
        // Provide a concrete implementation of the 'ball::AttributeContainer'
        // that holds the 'uuid', 'luw', and 'firmNumber' associated with a
        // request to the example service.  This container implementations
        // exposes those properties in attributes named "uuid", "luw", and
        // "firmNumber" respectively.

//..
// Note that we choose to have data members of type 'ball::Attribute' for
// simplicity.  It would be marginally more efficient to hold the values in
// 'int' data members, but the implementation of 'hasValue()' would be less
// readable.
//..
        // DATA
        ball::Attribute d_uuid;        // bloomberg user id
        ball::Attribute d_luw;         // bloomberg luw
        ball::Attribute d_firmNumber;  // bloomberg firm number

        // ...

      public:
        // PUBLIC CONSTANTS
        static const char * const UUID_ATTRIBUTE_NAME;
        static const char * const LUW_ATTRIBUTE_NAME;
        static const char * const FIRMNUMBER_ATTRIBUTE_NAME;
            // The names of the attributes exposed by this attribute container.

        // CREATORS
        ServiceAttributes(int uuid, int luw, int firmNumber);
            // Create a service attributes object with the specified 'uuid',
            // 'luw', and 'firmNumber'.

        virtual ~ServiceAttributes();
            // Destroy this service attributes object.

        // ACCESSORS
        virtual bool hasValue(const ball::Attribute& value) const;
            // Return 'true' if the attribute having specified 'value' exists
            // in this object, and 'false' otherwise.  This implementation will
            // return 'true' if 'value.name()' equals "uuid", "luw", or "firm"
            // and 'value.value()' is an 'int' equal to the corresponding
            // property value supplied at construction.

        virtual bsl::ostream& print(bsl::ostream& stream,
                                    int           level = 0,
                                    int           spacesPerLevel = 4) const;
            // Format this object to the specified output 'stream'.
    };


    // CREATORS
    inline
    ServiceAttributes::ServiceAttributes(int uuid, int luw, int firmNumber)
    : d_uuid(UUID_ATTRIBUTE_NAME, uuid)
    , d_luw(LUW_ATTRIBUTE_NAME, luw)
    , d_firmNumber(FIRMNUMBER_ATTRIBUTE_NAME, firmNumber)
    {
    }

      // serviceattributes.cpp

    // PUBLIC CONSTANTS
    const char * const ServiceAttributes::UUID_ATTRIBUTE_NAME       = "uuid";
    const char * const ServiceAttributes::LUW_ATTRIBUTE_NAME        = "luw";
    const char * const ServiceAttributes::FIRMNUMBER_ATTRIBUTE_NAME =
                                                                  "firmNumber";

    // CREATORS
    ServiceAttributes::~ServiceAttributes()
    {
    }

    // ACCESSORS
    bool ServiceAttributes::hasValue(const ball::Attribute& value) const
    {
        return d_uuid == value || d_luw == value || d_firmNumber == value;
    }

    bsl::ostream& ServiceAttributes::print(bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
    {
        bslim::Printer printer(&stream, level, spacesPerLevel);
        printer.start();
        printer.printAttribute("uuid", d_uuid);
        printer.printAttribute("luw", d_luw);
        printer.printAttribute("firmNumber", d_firmNumber);
        printer.end();
        return stream;
    }
//
///Example 2: A Generic Implementation of 'ball::AttributeContainer'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this second example we define a 'ball::AttributeContainer' that can
// contain any valid 'ball::Attribute' value (a "generic"
// 'ball::AttributeContainer').  In practice, an implementation that can
// contain any attribute values may be less efficient than an implementation
// specifically created for a particular group of attributes needed by an
// application (as seen in example 1).
//
// Note that the 'ball' package provides a similar 'ball::AttributeContainer'
// implementation in the 'ball_defaultattributecontainer' component.
//..
     // attributeset.h

    class AttributeSet : public ball::AttributeContainer {
        // A simple set based implementation of the 'ball::AttributeContainer'
        // protocol used for testing.

//..
// To defined an stl-set (or hash set) for 'ball::Attribute' values, we must
// define a comparison (or hash) operation for attribute values.  Here we
// define a comparison functor that compares attributes by name, then by
// value-type, and finally by value.
//..
        struct AttributeComparator {
            bool operator()(const ball::Attribute& lhs,
                            const ball::Attribute& rhs) const
                // Return 'true' if the specified 'lhs' is ordered before the
                // specified 'rhs'.
            {
                int cmp = bsl::strcmp(lhs.name(), rhs.name());
                if (0 != cmp) {
                    return cmp < 0;                                   // RETURN
                }
                if (lhs.value().typeIndex() != rhs.value().typeIndex()) {
                    return lhs.value().typeIndex() < rhs.value().typeIndex();
                                                                      // RETURN
                }
                switch (lhs.value().typeIndex()) {
                  case 0: // unset
                    return true;
                  case 1: // int
                    return lhs.value().the<int>() < rhs.value().the<int>();
                  case 2: // int64
                    return lhs.value().the<bsls::Types::Int64>()
                        <  rhs.value().the<bsls::Types::Int64>();
                 case 3: // string
                   return lhs.value().the<bsl::string>()
                       <  rhs.value().the<bsl::string>();
                }
                BSLS_ASSERT(false);
                return false;
            }
        };

        // DATA
        bsl::set<ball::Attribute, AttributeComparator> d_set;

        // ...

      public:
        // CREATORS
        AttributeSet(bslma::Allocator *basicAllocator = 0);
            // Create this attribute set.

        virtual ~AttributeSet();
            // Destroy this attribute set.

        // MANIPULATORS
        void insert(const ball::Attribute& value);
            // Add the specified 'value' to this attribute set.

        bool remove(const ball::Attribute& value);
            // Remove the specified 'value' from this attribute set, return
            // 'true' if the attribute was found, and 'false' if 'value' was
            // not a member of this set.

        // ACCESSORS
        virtual bool hasValue(const ball::Attribute& value) const;
            // Return 'true' if the attribute having specified 'value' exists
            // in this object, and 'false' otherwise.

        virtual bsl::ostream& print(bsl::ostream& stream,
                                    int           level = 0,
                                    int           spacesPerLevel = 4) const;
            // Format this object to the specified output 'stream' at the
            // (absolute value of) the optionally specified indentation 'level'
            // and return a reference to 'stream'.
    };

//..
// The 'ball::AttributeContainer' methods are simple wrappers around
// operations on an 'bsl::set'.
//..
    inline
    AttributeSet::AttributeSet(bslma::Allocator *basicAllocator)
    : d_set(AttributeComparator(), basicAllocator)
    {
    }

    // MANIPULATORS
    inline
    void AttributeSet::insert(const ball::Attribute& value)
    {
        d_set.insert(value);
    }

    inline
    bool AttributeSet::remove(const ball::Attribute& value)
    {
        return d_set.erase(value) > 0;
    }

    // attributeset.cpp

    // CREATORS
    AttributeSet::~AttributeSet()
    {
    }

    // ACCESSORS
    bool AttributeSet::hasValue(const ball::Attribute& value) const
    {
        return d_set.find(value) != d_set.end();
    }

    bsl::ostream& AttributeSet::print(bsl::ostream& stream,
                                      int           level,
                                      int           spacesPerLevel) const
    {

        bslim::Printer printer(&stream, level, spacesPerLevel);
        printer.start();

        bsl::set<ball::Attribute>::const_iterator it = d_set.begin();
        for (; it != d_set.end(); ++it) {
            printer.printValue(*it);
        }
        printer.end();

        return stream;
    }

//=============================================================================
//                         GLOBAL CLASSES FOR TESTING
//-----------------------------------------------------------------------------

struct AttributeContainerTest :
                              bsls::ProtocolTestImp<ball::AttributeContainer> {
    bool hasValue(const ball::Attribute&) const        { return markDone(); }
    bsl::ostream& print(bsl::ostream&, int, int) const
                                                      { return markDoneRef(); }
};

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.  Suppress
        //   all 'cout' statements in non-verbose mode, and add streaming to a
        //   buffer to test programmatically the printing examples.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Examples"
                          << "\n======================" << endl;

//..
//
///Example 3. Using a 'ball::AttributeContainer'
///- - - - - - - - - - - - - - - - - - - - - -
// In this final (trivial) example, we demonstrate calling the methods of the
// 'ball::AttributeContainer' protocol.
//
// First we create an object of a concrete type that implements the
// 'ball::AttributeContainer' protocol (e.g., 'ServiceAttributes'
// defined in example 1).  Then we obtain a reference to this object.
//..
    ServiceAttributes serviceAttributes(3938908, 1, 9001);
    const ball::AttributeContainer& attributes = serviceAttributes;
//..
// We use 'hasValue()' to examine the values in the container:
//..
    ASSERT(attributes.hasValue(ball::Attribute("uuid", 3938908)));
    ASSERT(attributes.hasValue(ball::Attribute("luw",  1)));
    ASSERT(attributes.hasValue(ball::Attribute("firmNumber", 9001)));

    ASSERT(!attributes.hasValue(ball::Attribute("uuid", 1391015)));
    ASSERT(!attributes.hasValue(ball::Attribute("luw",  2)));
    ASSERT(!attributes.hasValue(ball::Attribute("wrong name", 3938908)));
//..
// Finally we can print the attribute values in the container:
//..
    bsl::cout << attributes << bsl::endl;
//..
// The resulting output should look like:
//..
// [ [ uuid = 3938908 ]   [ luw = 1 ]   [ firmNumber = 9001 ] ]
//..
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // PROTOCOL TEST
        //
        // Concerns:
        //   'ball::AttributeContainer' defines a proper protocol class.
        //
        // Plan:
        //   Use 'bsls::ProtocolTest' to verify general protocol class concerns
        //   for 'ball::AttributeContainer' as well as each of its methods.
        //
        // Testing:
        //   class ball::AttributeContainer
        // --------------------------------------------------------------------

        bsls::ProtocolTest<AttributeContainerTest> t(veryVerbose);

        ASSERT(t.testAbstract());
        ASSERT(t.testNoDataMembers());
        ASSERT(t.testVirtualDestructor());

        BSLS_PROTOCOLTEST_ASSERT(t, hasValue(ball::Attribute("", 0)));
        BSLS_PROTOCOLTEST_ASSERT(t, print(cout, 0, 0));
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
