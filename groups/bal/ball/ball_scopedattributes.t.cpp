// ball_scopedattributes.t.cpp                                        -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <ball_scopedattributes.h>

#include <ball_attributecontext.h>
#include <ball_attributecontainer.h>

#include <bdlb_print.h>

#include <bslim_testutil.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_assert.h>
#include <bsls_types.h>

#include <bsl_climits.h>
#include <bsl_cstring.h>
#include <bsl_cstdlib.h>
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
//                  GLOBAL TYPEDEFS/CONSTANTS/VARIABLES FOR TESTING
//-----------------------------------------------------------------------------
int verbose;
int veryVerbose;
int veryVeryVerbose;

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------

// 'AttributeSet', as defined in the 'ball_attributecontainer' component
// documentation.

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

      private:
        // NOT IMPLEMENTED
        AttributeSet(const AttributeSet&);
        AttributeSet& operator=(const AttributeSet&);

      public:
        // TRAITS
        BSLMF_NESTED_TRAIT_DECLARATION(AttributeSet,
                                       bslma::UsesBslmaAllocator);

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
        char EL = (spacesPerLevel < 0) ? ' ' : '\n';
        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "[" << EL;

        bsl::set<ball::Attribute>::const_iterator it = d_set.begin();
        for (; it != d_set.end(); ++it) {
            it->print(stream, level+1, spacesPerLevel);
        }
        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]" << EL;
        return stream;
    }

//=============================================================================
//                         CASE 3 RELATED ENTITIES
//-----------------------------------------------------------------------------

//=============================================================================
//                         CASE 2 RELATED ENTITIES
//-----------------------------------------------------------------------------
//=============================================================================
//                         CASE 1 RELATED ENTITIES
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
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
        //   all 'cout' statements in non-verbose mode, and add streaming to
        //   a buffer to test programmatically the printing examples.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting usage example"
                          << "\n====================="
                          << endl;
///USAGE
///-----
// In the following code fragment, we will use a 'ball::ScopedAttributes' to
// install a 'ball::AttributeContainer' in the current context.
//
// We first create the current attribute context and two attributes:
//..
    ball::AttributeContext *context = ball::AttributeContext::getContext();

    ball::Attribute a1("uuid", 4044457);
    ball::Attribute a2("name", "Gang Chen");
    ASSERT(false == context->hasAttribute(a1));
    ASSERT(false == context->hasAttribute(a2));
//..
// Now we create an 'AttributeSet' and add the two attributes to this set,
// then we use a 'ball::ScopedAttributes to install these attributes in the
// current thread's attribute context.
//
// Note that we use the 'AttributeSet' implementation of the
// 'ball::AttributeContainer' protocol defined in the component documentation
// for 'ball_attributecontainer' (the 'ball' package provides a similar class
// in the 'ball_defaultattributecontainer' component).
//..
    {
        AttributeSet attributes;
        attributes.insert(a1);
        attributes.insert(a2);
        ball::ScopedAttributes attributeGuard(&attributes);
        ASSERT(true == context->hasAttribute(a1));
        ASSERT(true == context->hasAttribute(a2));
//..
// When 'attributeGuard' goes out of scope and is destroyed, 'attributes'
// are removed from the current thread's attribute context, which prevents the
// attribute context from referring to an invalid memory address (on the
// stack).
//..
    }
    ASSERT(!context->hasAttribute(a1));
    ASSERT(!context->hasAttribute(a2));
//..

      } break;

      default: {
          cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
          testStatus = -1;
      }
    };

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
