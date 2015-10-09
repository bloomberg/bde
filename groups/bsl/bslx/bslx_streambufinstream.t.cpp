// bslx_streambufinstream.t.cpp                                       -*-C++-*-

#include <bslx_streambufinstream.h>
#include <bslx_streambufoutstream.h>  // for testing only

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_ios.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;
using namespace bslx;

// ============================================================================
//                              TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The primary concerns are that the 'typedef' is correct and that the usage
// example works correctly.
// ----------------------------------------------------------------------------
// [ 1] TYPEDEF
// ----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE
// ----------------------------------------------------------------------------

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

// ============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

const int VERSION_SELECTOR = 20131127;

// ============================================================================
//                                 USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Unexternalization
///- - - - - - - - - - - - - - - - -
// Suppose we wish to implement a (deliberately simple) 'MyPerson' class as a
// value-semantic object that supports BDEX externalization and
// unexternalization.  In addition to whatever data and methods that we choose
// to put into our design, we must supply three methods having specific names
// and signatures in order to comply with the BDEX protocol: a class method
// 'maxSupportedBdexVersion', an accessor (i.e., a 'const' method)
// 'bdexStreamOut', and a manipulator (i.e., a non-'const' method)
// 'bdexStreamIn'.  This example shows how to implement those three methods.
//
// In this example we will not worry overly about "good design" of the
// 'MyPerson' component, and we will declare but not implement illustrative
// methods and free operators, except for the three required BDEX methods,
// which are implemented in full.  In particular, we will not make explicit use
// of 'bslma' allocators; a more complete design would do so:
//
// First, we implement 'MyPerson':
//..
    class MyPerson {
        bsl::string d_firstName;
        bsl::string d_lastName;
        int         d_age;

        friend bool operator==(const MyPerson&, const MyPerson&);

      public:
        // CLASS METHODS
        static int maxSupportedBdexVersion(int versionSelector);
            // Return the maximum valid BDEX format version, as indicated by
            // the specified 'versionSelector', to be passed to the
            // 'bdexStreamOut' method.  Note that it is highly recommended that
            // 'versionSelector' be formatted as "YYYYMMDD", a date
            // representation.  Also note that 'versionSelector' should be a
            // *compile*-time-chosen value that selects a format version
            // supported by both externalizer and unexternalizer.  See the
            // 'bslx' package-level documentation for more information on BDEX
            // streaming of value-semantic types and containers.

        // CREATORS
        MyPerson();
            // Create a default person.

        MyPerson(const char *firstName, const char *lastName, int age);
            // Create a person having the specified 'firstName', 'lastName',
            // and 'age'.

        MyPerson(const MyPerson& original);
            // Create a person having the value of the specified 'original'
            // person.

        ~MyPerson();
            // Destroy this object.

        // MANIPULATORS
        MyPerson& operator=(const MyPerson& rhs);
            // Assign to this person the value of the specified 'rhs' person,
            // and return a reference to this person.

        template <class STREAM>
        STREAM& bdexStreamIn(STREAM& stream, int version);
            // Assign to this object the value read from the specified input
            // 'stream' using the specified 'version' format, and return a
            // reference to 'stream'.  If 'stream' is initially invalid, this
            // operation has no effect.  If 'version' is not supported, this
            // object is unaltered and 'stream' is invalidated, but otherwise
            // unmodified.  If 'version' is supported but 'stream' becomes
            // invalid during this operation, this object has an undefined, but
            // valid, state.  Note that no version is read from 'stream'.  See
            // the 'bslx' package-level documentation for more information on
            // BDEX streaming of value-semantic types and containers.

        //...

        // ACCESSORS
        int age() const;
            // Return the age of this person.

        template <class STREAM>
        STREAM& bdexStreamOut(STREAM& stream, int version) const;
            // Write the value of this object, using the specified 'version'
            // format, to the specified output 'stream', and return a reference
            // to 'stream'.  If 'stream' is initially invalid, this operation
            // has no effect.  If 'version' is not supported, 'stream' is
            // invalidated, but otherwise unmodified.  Note that 'version' is
            // not written to 'stream'.  See the 'bslx' package-level
            // documentation for more information on BDEX streaming of
            // value-semantic types and containers.

        const bsl::string& firstName() const;
            // Return the first name of this person.

        const bsl::string& lastName() const;
            // Return the last name of this person.

        //...

    };

    // FREE OPERATORS
    bool operator==(const MyPerson& lhs, const MyPerson& rhs);
        // Return 'true' if the specified 'lhs' and 'rhs' person objects have
        // the same value, and 'false' otherwise.  Two person objects have the
        // same value if they have the same first name, last name, and age.

    bool operator!=(const MyPerson& lhs, const MyPerson& rhs);
        // Return 'true' if the specified 'lhs' and 'rhs' person objects do not
        // have the same value, and 'false' otherwise.  Two person objects
        // differ in value if they differ in first name, last name, or age.

    // ========================================================================
    //                  INLINE FUNCTION DEFINITIONS
    // ========================================================================

    // CLASS METHODS
    inline
    int MyPerson::maxSupportedBdexVersion(int /* versionSelector */) {
        return 1;
    }

    // CREATORS
    inline
    MyPerson::MyPerson()
    : d_firstName("")
    , d_lastName("")
    , d_age(0)
    {
    }

    inline
    MyPerson::MyPerson(const char *firstName, const char *lastName, int age)
    : d_firstName(firstName)
    , d_lastName(lastName)
    , d_age(age)
    {
    }

    inline
    MyPerson::~MyPerson()
    {
    }

    template <class STREAM>
    STREAM& MyPerson::bdexStreamIn(STREAM& stream, int version)
    {
        if (stream) {
            switch (version) {  // switch on the 'bslx' version
              case 1: {
                stream.getString(d_firstName);
                if (!stream) {
                    d_firstName = "stream error";  // *might* be corrupted;
                                                   //  value for testing
                    return stream;                                    // RETURN
                }
                stream.getString(d_lastName);
                if (!stream) {
                    d_lastName = "stream error";  // *might* be corrupted;
                                                  //  value for testing
                    return stream;                                    // RETURN
                }
                stream.getInt32(d_age);
                if (!stream) {
                    d_age = 999;     // *might* be corrupted; value for testing
                    return stream;                                    // RETURN
                }
              } break;
              default: {
                stream.invalidate();
              }
            }
        }
        return stream;
    }

    // ACCESSORS
    inline
    int MyPerson::age() const
    {
        return d_age;
    }

    template <class STREAM>
    STREAM& MyPerson::bdexStreamOut(STREAM& stream, int version) const
    {
        switch (version) {
          case 1: {
            stream.putString(d_firstName);
            stream.putString(d_lastName);
            stream.putInt32(d_age);
          } break;
          default: {
            stream.invalidate();
          } break;
        }
        return stream;
    }

    inline
    const bsl::string& MyPerson::firstName() const
    {
        return d_firstName;
    }

    inline
    const bsl::string& MyPerson::lastName() const
    {
        return d_lastName;
    }

    // FREE OPERATORS
    inline
    bool operator==(const MyPerson& lhs, const MyPerson& rhs)
    {
        return lhs.d_firstName == rhs.d_firstName &&
               lhs.d_lastName  == rhs.d_lastName  &&
               lhs.d_age       == rhs.d_age;
    }

    inline
    bool operator!=(const MyPerson& lhs, const MyPerson& rhs)
    {
        return !(lhs == rhs);
    }

// ============================================================================
//                                 MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 2: {
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

// Then, we can exercise the new 'MyPerson' value-semantic class by
// externalizing and reconstituting an object.  First, create a 'MyPerson'
// 'janeSmith' and a 'bslx::StreambufOutStream' 'outStream':
//..
    MyPerson                 janeSmith("Jane", "Smith", 42);
    bsl::stringbuf           buffer;
    bslx::StreambufOutStream outStream(&buffer, 20131127);
    const int                VERSION = 1;
    outStream.putVersion(VERSION);
    janeSmith.bdexStreamOut(outStream, VERSION);
    ASSERT(outStream.isValid());
//..
// Next, create a 'MyPerson' 'janeCopy' initialized to the default value, and
// assert that 'janeCopy' is different from 'janeSmith':
//..
    MyPerson janeCopy;
    ASSERT(janeCopy != janeSmith);
//..
// Then, create a 'bslx::StreambufInStream' 'inStream' initialized with the
// buffer from the 'bslx::StreambufOutStream' object 'outStream' and
// unexternalize this data into 'janeCopy':
//..
    bslx::StreambufInStream inStream(&buffer);
    int                     version;
    inStream.getVersion(version);
    janeCopy.bdexStreamIn(inStream, version);
    ASSERT(inStream.isValid());
//..
// Finally, 'assert' the obtained values are as expected and display the
// results to 'bsl::stdout':
//..
    ASSERT(version  == VERSION);
    ASSERT(janeCopy == janeSmith);

if (veryVerbose) {
    if (janeCopy == janeSmith) {
        bsl::cout << "Successfully serialized and de-serialized Jane Smith:"
                  << "\n\tFirstName: " << janeCopy.firstName()
                  << "\n\tLastName : " << janeCopy.lastName()
                  << "\n\tAge      : " << janeCopy.age() << bsl::endl;
    }
    else {
        bsl::cout << "Serialization unsuccessful.  'janeCopy' holds:"
                  << "\n\tFirstName: " << janeCopy.firstName()
                  << "\n\tLastName : " << janeCopy.lastName()
                  << "\n\tAge      : " << janeCopy.age() << bsl::endl;
    }
} // if (veryVerbose)
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TYPEDEF
        //   Verify the 'typedef' is correct.
        //
        // Concerns:
        //: 1 The 'typedef' is correct.
        //
        // Plan:
        //: 1 Externalize a few items with 'bslx::StreambufOutStream',
        //:   using a 'bsl::stringbuf', and verify 'bslx::StreambufInStream'
        //:   correctly unexternalizes the values.
        //
        // Testing:
        //   TYPEDEF
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TYPEDEF" << endl
                          << "=======" << endl;

        int         expectedInt    = 3;
        double      expectedDouble = 3.25;
        bsl::string expectedString = "alpha";

        bsl::stringbuf           buffer;
        bslx::StreambufOutStream out(&buffer, 20131127);

        out.putInt32(expectedInt);
        out.putFloat64(expectedDouble);
        out.putString(expectedString);

        int         obtainedInt = 0;
        double      obtainedDouble = 0.0;
        bsl::string obtainedString;

        bslx::StreambufInStream in(&buffer);

        in.getInt32(obtainedInt);
        in.getFloat64(obtainedDouble);
        in.getString(obtainedString);

        ASSERT(expectedInt    == obtainedInt);
        ASSERT(expectedDouble == obtainedDouble);
        ASSERT(expectedString == obtainedString);
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
// Copyright 2014 Bloomberg Finance L.P.
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
