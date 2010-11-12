// bdex_versionfunctions.t.cpp                  -*-C++-*-

#include <bdex_versionfunctions.h>

#include <bsls_platformutil.h>     // for testing only

#include <bsl_cstdlib.h>
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
// This component returns version number for various data types.  In this test
// case we will try to verify that the version information returned by this
// component matches the expected value for different types.  The types that
// are of interest are all the fundamental types, bsl::string, bsl::vector,
// enum types and value-semantic types.  Note that an exhaustive test is not
// necessary and that all the test conditions will be tested in the breathing
// test.
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] USAGE
//=============================================================================

//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " \
                          << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J \
                         << "\t" << #K << ": " << K << "\n";           \
                aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J \
                         << "\t" << #K << ": " << K << "\t" << #L << ": " \
                         << L << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J    \
                         << "\t" << #K << ": " << K << "\t" << #L << ": " \
                         << L << "\t" << #M << ": " << M << "\n";         \
               aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J     \
                         << "\t" << #K << ": " << K << "\t" << #L << ": "  \
                         << L << "\t" << #M << ": " << M << "\t" << #N     \
                         << ": " << N << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl; // Print ID and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;  // Quote ID literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << flush; // P(X) w/o '\n'
#define L_ __LINE__                                // current Line number
#define T_ bsl::cout << "\t" << flush;             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bsls_PlatformUtil::Int64  Int64;
typedef bsls_PlatformUtil::Uint64 Uint64;

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

//=============================================================================
//                  GLOBAL CLASSES FOR TESTING
//-----------------------------------------------------------------------------

enum my_TestEnum {
    EnumA = INT_MIN,
    EnumB = -1,
    EnumC = 0,
    EnumD = 1,
    EnumE = INT_MAX
};

class my_TestClass {
    // This test class is used for testing the version functionality.

  public:

    enum {
        VERSION = 999
    };

    // CREATORS
    my_TestClass() { }
    ~my_TestClass() { }

    // ACCESSORS
    int maxSupportedBdexVersion() const { return VERSION; }

};

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

 enum my_Enum {
     ENUM_VALUE1,
     ENUM_VALUE2,
     ENUM_VALUE3,
     ENUM_VALUE4
 };

 class my_Class {
   public:
     enum {
       VERSION = 874
     };

     // ACCESSORS
     int maxSupportedBdexVersion() const { return VERSION; }
 };

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

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
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

// This component will be used by clients to find out the version number for
// various types in a convenient manner.  Consider the list of fundamental
// types below:
//..
{
    char               c;
    int                i;
    double             d;
    bsl::string        s;
//..
// Calling the 'maxSupportedVersion' function on these types will return the
// value BDEX_NO_VERSION_NUMBER declared in this namespace.
//..
   using namespace bdex_VersionFunctions;

   ASSERT(BDEX_NO_VERSION_NUMBER == maxSupportedVersion(c));
   ASSERT(BDEX_NO_VERSION_NUMBER == maxSupportedVersion(i));
   ASSERT(BDEX_NO_VERSION_NUMBER == maxSupportedVersion(d));
   ASSERT(BDEX_NO_VERSION_NUMBER == maxSupportedVersion(s));
}
//..
// Now consider that the user has declared an enum type and wants to find out
// the 'bdex' version number for it.  The 'maxSupportedVersion' function is
// specialized to return BDEX_NO_VERSION_NUMBER for enum types.
//..
 {
     using namespace bdex_VersionFunctions;

     my_Enum e = ENUM_VALUE1;
     ASSERT(BDEX_NO_VERSION_NUMBER == maxSupportedVersion(e));
 }
//..
// If a class defines the 'maxSupportedBdexVersion' function then the
// 'maxSupportedVersion' function in this component calls that function to
// find out the version number.  Consider the 'my_Class' value-semantic type
// below:
//..
 {
     using namespace bdex_VersionFunctions;

     my_Class c;
     ASSERT(my_Class::VERSION == maxSupportedVersion(c));
 }
//..
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   That the correct version number is returned for all data types.
        //
        // Plan:
        //   Create variable of all types under test and verify that the
        //   'maxSupportedVersion' function returns the expected version.  The
        //   types that should be tested include all the fundamental types,
        //   bsl::string, bsl::vector, enum types and value-semantic type.
        //
        // Testing:
        //   This Test Case exercises basic functionality.
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nBREATHING TEST"
                               << "\n==============" << bsl::endl;

        const char                        XA = 'x';
        const signed char                 XB = 'x';
        const unsigned char               XC = 'x';

        const short                       XD = 12;
        const unsigned short              XE = 12;

        const int                         XF = 103;
        const unsigned int                XG = 103;

        const Int64                       XH = 10004;
        const Uint64                      XI = 10004;

        const float                       XJ = 105.5;
        const double                      XK = 106.006;

        const bsl::string                 XL = "one-seven---";
        const my_TestClass                XM;

        bsl::vector<char>                 XN_;
        const bsl::vector<char>&          XN = XN_;

        bsl::vector<my_TestClass>         XO_;
        const bsl::vector<my_TestClass>&  XO = XO_;

        bsl::vector<char>                 XP_;
        const bsl::vector<char>&          XP = XP_;
        XP_.push_back(XA);
        XP_.push_back(XA);

        bsl::vector<short>                XQ_;
        const bsl::vector<short>&         XQ = XQ_;
        XQ_.push_back(XD);
        XQ_.push_back(XD);

        bsl::vector<int>                  XR_;
        const bsl::vector<int>&           XR = XR_;
        XR_.push_back(XF);
        XR_.push_back(XF);

        bsl::vector<Int64>                XS_;
        const bsl::vector<Int64>&         XS = XS_;
        XS_.push_back(XH);
        XS_.push_back(XH);

        bsl::vector<float>                XT_;
        const bsl::vector<float>&         XT = XT_;
        XT_.push_back(XJ);
        XT_.push_back(XJ);

        bsl::vector<double>               XU_;
        const bsl::vector<double>&        XU = XU_;
        XU_.push_back(XK);
        XU_.push_back(XK);

        bsl::vector<bsl::string>          XV_;
        const bsl::vector<bsl::string>&   XV = XV_;
        XV_.push_back(XL);
        XV_.push_back(XL);

        bsl::vector<my_TestClass>        XW_;
        const bsl::vector<my_TestClass>& XW = XW_;
        XW_.push_back(XM);
        XW_.push_back(XM);

        my_TestEnum                      XX = (my_TestEnum) INT_MIN;
        my_TestEnum                      XY = (my_TestEnum) 0;
        my_TestEnum                      XZ = (my_TestEnum) INT_MAX;

        bsl::vector<my_TestEnum>         XXA;
        bsl::vector<my_TestEnum>         XXB;
        XXB.push_back(XX);
        XXB.push_back(XY);

        if (verbose) cout << "\nTesting 'maxSupportedVersion'" << endl;
        {
            using namespace bdex_VersionFunctions;

            const int VERSION = BDEX_NO_VERSION_NUMBER;

            ASSERT(VERSION               == maxSupportedVersion(XA));
            ASSERT(VERSION               == maxSupportedVersion(XB));
            ASSERT(VERSION               == maxSupportedVersion(XC));
            ASSERT(VERSION               == maxSupportedVersion(XD));
            ASSERT(VERSION               == maxSupportedVersion(XE));
            ASSERT(VERSION               == maxSupportedVersion(XF));
            ASSERT(VERSION               == maxSupportedVersion(XG));
            ASSERT(VERSION               == maxSupportedVersion(XH));
            ASSERT(VERSION               == maxSupportedVersion(XI));
            ASSERT(VERSION               == maxSupportedVersion(XJ));
            ASSERT(VERSION               == maxSupportedVersion(XK));
            ASSERT(VERSION               == maxSupportedVersion(XL));
            ASSERT(my_TestClass::VERSION == maxSupportedVersion(XM));
            ASSERT(1                     == maxSupportedVersion(XN));
            ASSERT(1                     == maxSupportedVersion(XO));
            ASSERT(1                     == maxSupportedVersion(XP));
            ASSERT(1                     == maxSupportedVersion(XQ));
            ASSERT(1                     == maxSupportedVersion(XR));
            ASSERT(1                     == maxSupportedVersion(XS));
            ASSERT(1                     == maxSupportedVersion(XT));
            ASSERT(1                     == maxSupportedVersion(XU));
            ASSERT(1                     == maxSupportedVersion(XV));
            ASSERT(my_TestClass::VERSION == maxSupportedVersion(XW));
            ASSERT(VERSION               == maxSupportedVersion(XX));
            ASSERT(VERSION               == maxSupportedVersion(XY));
            ASSERT(VERSION               == maxSupportedVersion(XZ));
            ASSERT(1                     == maxSupportedVersion(XXA));
            ASSERT(1                     == maxSupportedVersion(XXB));
        }
      } break;

      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
