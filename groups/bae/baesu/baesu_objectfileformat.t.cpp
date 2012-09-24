// baesu_objectfileformat.t.cpp                                       -*-C++-*-
#include <baesu_objectfileformat.h>

#include <bslmf_issame.h>

#include <bsl_iostream.h>
#include <bsl_cstdlib.h>     // atoi()

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                   TEST PLAN
//-----------------------------------------------------------------------------
// [ 1] Identifier test
// [ 2] Unique objectfileformat #define defined
// [ 3] Proper resolver policy defined
// [ 4] Usage example
//-----------------------------------------------------------------------------
//==========================================================================
//                             TEST PLAN
//--------------------------------------------------------------------------
//                             Overview
//                             --------
// Since this component implements 'CPP' macro's and 'typedef's, which may or
// may not be defined, there is not too much to test in this driver.  Since
// correctness will be affected by compile-time switches during the build
// process, any compile-time tests we come up with should probably reside
// directly in the header or implementation file.
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// [ 1] Ensure that if a 'RESOLVER' id is defined, it has the value '1'.
// [ 2] Ensure that exactly one of the 'RESOLVER' ids is defined.
// [ 3] That 'Obj::Policy' is appropriate for the platform.
// [ 4] USAGE EXAMPLE

// ============================================================================
//                    STANDARD BDE ASSERT TEST MACROS
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
# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) {                                                 \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": "                 \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                                               \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"     \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                                             \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\n";                    \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                                           \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" <<                  \
       #M << ": " << M << "\n";                                               \
       aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef baesu_ObjectFileFormat          Obj;

//=============================================================================
//                    HELPER FUNCTIONS FOR USAGE EXAMPLE
//=============================================================================

///Example 1: Accessing 'baesu_ObjectFileFormat' at Run Time
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The templated (specialized) 'typeTest' function returns a unique, non-zero
// value when passed an object of types
// 'baesu_ObjectFileFormat::{Elf,Xcoff,Windows}', and 0 otherwise.
//..
template <typename TYPE>
int typeTest(const TYPE &)
{
    return 0;
}

int typeTest(const baesu_ObjectFileFormat::Elf &)
{
    return 1;
}

int typeTest(const baesu_ObjectFileFormat::Xcoff &)
{
    return 2;
}

int typeTest(const baesu_ObjectFileFormat::Windows &)
{
    return 3;
}

int typeTest(const baesu_ObjectFileFormat::MachO &)
{
    return 4;
}
//..

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    // bool         veryVerbose = argc > 3;
    // bool     veryVeryVerbose = argc > 4;
    // bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

// We define an object 'policy' of type 'baesu_ObjectFileFormat::Policy', which
// will be of type '...::Elf', '...::Xcoff', or '...::Windows' appropriate for
// the platform.
//..
        baesu_ObjectFileFormat::Policy policy;
//..
// We now test it using 'typeTest':
//..
        ASSERT(typeTest(policy) > 0);

    #if defined(BAESU_OBJECTFILEFORMAT_RESOLVER_ELF)
        ASSERT(1 == typeTest(policy));
    #elif defined(BAESU_OBJECTFILEFORMAT_RESOLVER_XCOFF)
        ASSERT(2 == typeTest(policy));
    #elif defined(BAESU_OBJECTFILEFORMAT_RESOLVER_WINDOWS)
        ASSERT(3 == typeTest(policy));
    #elif defined(BAESU_OBJECTFILEFORMAT_RESOLVER_MACHO)
        ASSERT(4 == typeTest(policy));
    #else
        #error No resolver format defined
    #endif
//..

      }  break;
      case 3: {
        // --------------------------------------------------------------------
        // PROPER RESOLVER POLICY DEFINED
        //
        // Concerns:
        //: 1 That all resolver policies exist, and that 'Obj::Policy' is
        //:   appropriately defined for the platform.
        //
        // Plan:
        //: 1 Use 'bslmf_IsSame' to verify types match appropriately.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PROPER RESOLVER POLICY DEFINED" << endl
                          << "==============================" << endl;

#if defined(BSLS_PLATFORM_OS_SOLARIS) || \
    defined(BSLS_PLATFORM_OS_LINUX)   || \
    defined(BSLS_PLATFORM_OS_HPUX)

        ASSERT(1 == (bslmf_IsSame<Obj::Policy, Obj::Elf>()));
        ASSERT(0 == (bslmf_IsSame<Obj::Policy, Obj::Xcoff>()));
        ASSERT(0 == (bslmf_IsSame<Obj::Policy, Obj::Windows>()));
        ASSERT(0 == (bslmf_IsSame<Obj::Policy, Obj::MachO>()));

#elif defined(BSLS_PLATFORM_OS_AIX)

        ASSERT(0 == (bslmf_IsSame<Obj::Policy, Obj::Elf>()));
        ASSERT(1 == (bslmf_IsSame<Obj::Policy, Obj::Xcoff>()));
        ASSERT(0 == (bslmf_IsSame<Obj::Policy, Obj::Windows>()));
        ASSERT(0 == (bslmf_IsSame<Obj::Policy, Obj::MachO>()));

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

        ASSERT(0 == (bslmf_IsSame<Obj::Policy, Obj::Elf>()));
        ASSERT(0 == (bslmf_IsSame<Obj::Policy, Obj::Xcoff>()));
        ASSERT(1 == (bslmf_IsSame<Obj::Policy, Obj::Windows>()));
        ASSERT(0 == (bslmf_IsSame<Obj::Policy, Obj::MachO>()));

#elif defined(BSLS_PLATFORM_OS_DARWIN)

        ASSERT(0 == (bslmf_IsSame<Obj::Policy, Obj::Elf>()));
        ASSERT(0 == (bslmf_IsSame<Obj::Policy, Obj::Xcoff>()));
        ASSERT(0 == (bslmf_IsSame<Obj::Policy, Obj::Windows>()));
        ASSERT(1 == (bslmf_IsSame<Obj::Policy, Obj::MachO>()));

#else
#       error Unrecognized platform
#endif
      }  break;
      case 2: {
        // --------------------------------------------------------------------
        // UNIQUE '#define' DEFINED
        //
        // Concerns:
        //: 1 Ensure that exactly one object file format '#define' is defined.
        //
        // Plan:
        //: 1 Increment a count once for each object file format #define and
        //:   then verify it's been incremented once.  (C-1)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "UNIQUE '#define' DEFINED" << endl
                          << "========================" << endl;

        int count = 0;

#if defined(BAESU_OBJECTFILEFORMAT_RESOLVER_ELF)
        ++count;
#endif

#if defined(BAESU_OBJECTFILEFORMAT_RESOLVER_XCOFF)
        ++count;
#endif

#if defined(BAESU_OBJECTFILEFORMAT_RESOLVER_WINDOWS)
        ++count;
#endif

#if defined(BAESU_OBJECTFILEFORMAT_RESOLVER_MACHO)
        ++count;
#endif

        ASSERT(1 == count);
      }  break;
      case 1: {
        // --------------------------------------------------------------------
        // RESOLVER
        //
        // Concern:
        //: 1 That that 'RESOLVER' identifiers, when defined, have the value
        //:   '1', that they are appropriate for the platform, and that never
        //:   are there more than one defined at a time.
        //
        // Plan:
        //: 1 For each platform set, check that the appropriate object file
        //:   format '#define' is defined, the others are not, and that the
        //:   '#define' that is defined evaluates to '1'.  (C-1)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "RESOLVER" << endl
                          << "========" << endl;

#if defined(BSLS_PLATFORM_OS_SOLARIS) || \
    defined(BSLS_PLATFORM_OS_LINUX)   || \
    defined(BSLS_PLATFORM_OS_HPUX)

        ASSERT(1 == BAESU_OBJECTFILEFORMAT_RESOLVER_ELF);

# if defined(BAESU_OBJECTFILEFORMAT_RESOLVER_XCOFF) || \
     defined(BAESU_OBJECTFILEFORMAT_RESOLVER_WINDOWS) || \
     defined(BAESU_OBJECTFILEFORMAT_RESOLVER_MACHO)
#  error multiple file formats defined
# endif

#elif defined(BSLS_PLATFORM_OS_AIX)

        ASSERT(1 == BAESU_OBJECTFILEFORMAT_RESOLVER_XCOFF);

# if defined(BAESU_OBJECTFILEFORMAT_RESOLVER_ELF) || \
     defined(BAESU_OBJECTFILEFORMAT_RESOLVER_WINDOWS) || \
     defined(BAESU_OBJECTFILEFORMAT_RESOLVER_MACHO)
#  error multiple file formats defined
# endif

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

        ASSERT(1 == BAESU_OBJECTFILEFORMAT_RESOLVER_WINDOWS);

# if defined(BAESU_OBJECTFILEFORMAT_RESOLVER_ELF) || \
     defined(BAESU_OBJECTFILEFORMAT_RESOLVER_XCOFF) || \
     defined(BAESU_OBJECTFILEFORMAT_RESOLVER_MACHO)
#  error multiple file formats defined
# endif


#elif defined(BSLS_PLATFORM_OS_DARWIN)

        ASSERT(1 == BAESU_OBJECTFILEFORMAT_RESOLVER_MACHO);

# if defined(BAESU_OBJECTFILEFORMAT_RESOLVER_ELF) || \
     defined(BAESU_OBJECTFILEFORMAT_RESOLVER_XCOFF) || \
     defined(BAESU_OBJECTFILEFORMAT_RESOLVER_WINDOWS)
#  error multiple file formats defined
# endif

#else
# error unrecognized platform
#endif

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
