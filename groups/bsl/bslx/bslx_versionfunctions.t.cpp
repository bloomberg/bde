// bslx_versionfunctions.t.cpp                                        -*-C++-*-

#include <bslx_versionfunctions.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_types.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;
using namespace bslx;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test provides BDEX version computation for types.  This
// component will be tested by verifying the return value of the method for a
// variety of 'TYPE'.
//-----------------------------------------------------------------------------
// [ 1] int maxSupportedBdexVersion<TYPE>(const TYPE *, STREAM& stream);
//-----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE
//-----------------------------------------------------------------------------

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

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

//=============================================================================
//                  GLOBAL CLASSES FOR TESTING
//-----------------------------------------------------------------------------

class TestStream {
    // This class implements a subset of the methods required of a
    // BDEX-compliant out stream.  This class is used to provide
    // 'bdexSerializationVersion' when the 'VersionFunctions' methods need to
    // compute a version of a user-defined type.

    int d_serializationVersion;

  public:

    TestStream() : d_serializationVersion(0) {}

    int bdexSerializationVersion() {  return d_serializationVersion;  }

    void setSerializationVersion(int serializationVersion) {
        d_serializationVersion = serializationVersion;
    }
};

enum TestEnum {
    // A simple enumeration used for testing.

    A,
    B,
    C
};

class TestClass {
    // This class implements a subset of the methods required of a
    // BDEX-compliant class.  This class is used to provide
    // 'maxSupportedBdexVersion' when the 'VersionFunctions' methods need to
    // compute a version of a user-defined type.

  public:
    static int maxSupportedBdexVersion(int serializationVersion) {
        if (serializationVersion >= 20131201) {
            return 4;
        }
        else {
            return 3;
        }
    }
};

template <typename TYPE>
struct TestType {
    // This class is a utility for verifying the results of
    // 'bslx::VersionFunctions::maxSupportedBdexVersion' applied to a (template
    // parameter) type 'TYPE', 'const TYPE', 'volatile TYPE', and nested
    // vectors of these types.

    template <class STREAM>
    static void testCV(STREAM& stream, int version)
    {
        using bslx::VersionFunctions::maxSupportedBdexVersion;

        int sv = stream.bdexSerializationVersion();

        TYPE *t = 0;
        const TYPE *ct = 0;
        volatile TYPE *vt = 0;

        ASSERT(maxSupportedBdexVersion( t, sv) == version);
        ASSERT(maxSupportedBdexVersion(ct, sv) == version);
        ASSERT(maxSupportedBdexVersion(vt, sv) == version);
    }

    template <class STREAM>
    static void test(STREAM& stream, int version, int vectorVersion)
    {
        testCV(stream, version);

        TestType<bsl::vector<TYPE> >::testCV(stream, vectorVersion);

        TestType<bsl::vector<bsl::vector<TYPE> > >::testCV(stream,
                                                           vectorVersion);

        TestType<bsl::vector<bsl::vector<bsl::vector<TYPE> > > >::
                                                 testCV(stream, vectorVersion);
    }
};

//=============================================================================
//                                 USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Querying BDEX Version
///- - - - - - - - - - - - - - - -
// This component may be used by clients to query the version number for types
// in a convenient manner.  First, define an 'enum', 'my_Enum':
//..
    enum my_Enum {
        ENUM_VALUE1,
        ENUM_VALUE2,
        ENUM_VALUE3,
        ENUM_VALUE4
    };
//..
// Then, define a BDEX-compliant class, 'my_Class':
//..
    class my_Class {
      public:
        enum {
            VERSION = 1
        };

        // CLASS METHODS
        static int maxSupportedBdexVersion(int) {
            return VERSION;
        }

        //...

    };

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

//..
// Finally, verify the value returned by 'maxSupportedBdexVersion' for some
// fundamental types, 'my_Enum', and 'my_Class' with an arbitrary
// 'serializationVersion':
//..
    using bslx::VersionFunctions::maxSupportedBdexVersion;
    using bslx::VersionFunctions::k_NO_VERSION;

    ASSERT(k_NO_VERSION ==
        maxSupportedBdexVersion(reinterpret_cast<char        *>(0), 20131127));
    ASSERT(k_NO_VERSION ==
        maxSupportedBdexVersion(reinterpret_cast<int         *>(0), 20131127));
    ASSERT(k_NO_VERSION ==
        maxSupportedBdexVersion(reinterpret_cast<double      *>(0), 20131127));
    ASSERT(k_NO_VERSION ==
        maxSupportedBdexVersion(reinterpret_cast<bsl::string *>(0), 20131127));

    ASSERT(k_NO_VERSION ==
        maxSupportedBdexVersion(reinterpret_cast<my_Enum     *>(0), 20131127));

    ASSERT(my_Class::VERSION ==
        maxSupportedBdexVersion(reinterpret_cast<my_Class    *>(0), 20131127));
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'maxSupportedBdexVersion<TYPE>'
        //   Ensure the correct value is returned for all usage groups.
        //
        // Concerns:
        //: 1 The method returns 'k_NO_VERSION' for fundamental types,
        //:   enumerations, and 'bsl::string'.
        //:
        //: 2 The method returns 1 for vectors and nested-vectors of
        //:   fundamental types, enumerations, and 'bsl::string'.
        //:
        //: 3 'const' and 'volatile' are correctly handled by the method.
        //:
        //: 4 'maxSupportedBdexVersion' is appropriately used for other types.
        //
        // Plan:
        //: 1 Define a templatized class 'TestType' that will validate the
        //:   return value against a provided value for 'TYPE', 'const TYPE',
        //:   'volatile TYPE', and against a second provided value for
        //:   'const' and 'volatile' nested vectors of 'TYPE' and use this
        //:   mechanism to test fundamental, enum, and 'bsl::string'.  (C-1..3)
        //:
        //: 2 Use this mechanism with a test class and two different values
        //:   returned by the 'bdexSerializationVersion' of the stream to
        //:   ensure correct return values for user-defined types.  (C-4)
        //
        // Testing:
        //   int maxSupportedBdexVersion<TYPE>(const TYPE *, STREAM& stream);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'maxSupportedBdexVersion<TYPE>'" << endl
                          << "=======================================" << endl;

        using namespace bslx::VersionFunctions;  // needed for 'k_NO_VERSION'

        TestStream stream;

        TestType<char               >::test(stream, k_NO_VERSION, 1);

        TestType<signed char        >::test(stream, k_NO_VERSION, 1);

        TestType<unsigned char      >::test(stream, k_NO_VERSION, 1);

        TestType<short              >::test(stream, k_NO_VERSION, 1);

        TestType<unsigned short     >::test(stream, k_NO_VERSION, 1);

        TestType<int                >::test(stream, k_NO_VERSION, 1);

        TestType<unsigned int       >::test(stream, k_NO_VERSION, 1);

        TestType<bsls::Types::Int64 >::test(stream, k_NO_VERSION, 1);

        TestType<bsls::Types::Uint64>::test(stream, k_NO_VERSION, 1);

        TestType<float              >::test(stream, k_NO_VERSION, 1);

        TestType<double             >::test(stream, k_NO_VERSION, 1);

        TestType<TestEnum           >::test(stream, k_NO_VERSION, 1);

        TestType<bsl::string        >::test(stream, k_NO_VERSION, 1);

        TestType<TestClass          >::test(stream, 3,            3);

        stream.setSerializationVersion(20131201);

        TestType<TestClass          >::test(stream, 4,            4);

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
// Copyright (C) 2013 Bloomberg Finance L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
