// baesu_objectfileformat.t.cpp                                       -*-C++-*-

#include <baesu_objectfileformat.h>

#include <bslmf_issame.h>

#include <bsl_iostream.h>
#include <bsl_cstdlib.h>     // atoi()

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
// STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
// STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
                    << J << "\t" \
                    << #K << ": " << K <<  "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I, J, K, M, X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
                    << J << "\t" << #K << ": " << K << "\t" \
                    << #M << ": " << M << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
// SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

//=============================================================================
// GLOBAL HELPER TYPES, CLASSES, and CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef baesu_ObjectFileFormat          Obj;

//=============================================================================
//                    HELPER FUNCTIONS FOR USAGE EXAMPLE
//=============================================================================

// 'typeTest' is a template function that will return non-zero if passed an
// object of types 'baesu_ObjectFileFormat::{Elf,Xcoff,Windows}' appropriate
// for the current platform and 0 otherwise.

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

//==========================================================================
//                             TEST PLAN
//--------------------------------------------------------------------------
//                            * Overview *
// Since this component implements CPP macro's and typedefs, which may or may
// not be defined, there is not too much to test in this driver.  Since
// correctness will be affected by compile-time switches during the build
// process, any compile-time tests we come up with should probably reside
// directly in the header or implementation file.
//--------------------------------------------------------------------------
// [ 1] Ensure that if a 'RESOLVER' id is defined, it has the value '1'.
// [ 2] Ensure that exactly one of the 'RESOLVER' ids is defined.
// [ 3] That 'Obj::Policy' is appropriate for the platform.
// [ 4] That 'Obj::Policy' is approrpiate for the 'RESOLVER' id.
//==========================================================================

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // TYPETEST TEST / USAGE
        //
        // Concerns:
        //   That the resolver policy exists and is appropriate.
        //
        // Plan:
        //   Use the template function 'typeTest' to identify which resolver
        //   policy is set.
        // --------------------------------------------------------------------

        baesu_ObjectFileFormat::Policy policy;
        ASSERT(typeTest(policy) > 0);

#if defined(BAESU_OBJECTFILEFORMAT_RESOLVER_ELF)
        ASSERT(1 == typeTest(policy));
#endif

#if defined(BAESU_OBJECTFILEFORMAT_RESOLVER_XCOFF)
        ASSERT(2 == typeTest(policy));
#endif

#if defined(BAESU_OBJECTFILEFORMAT_RESOLVER_WINDOWS)
        ASSERT(3 == typeTest(policy));
#endif
      }  break;
      case 3: {
        // --------------------------------------------------------------------
        // PROPER RESOLVER POLICY DEFINED
        //
        // Concerns:
        //   That all resolver policies exist, and that 'Obj::Policy' is
        //   appropriately defined for the platform.
        //
        // Plan:
        //   Use 'bslmf_IsSame' to verify types match appropriately.
        // --------------------------------------------------------------------

        if (verbose) cout << "Proper resolver policy defined\n"
                             "==============================\n";

#if defined(BSLS_PLATFORM__OS_SOLARIS) || \
    defined(BSLS_PLATFORM__OS_LINUX)   || \
    defined(BSLS_PLATFORM__OS_HPUX)

        ASSERT(1 == (bslmf_IsSame<Obj::Policy, Obj::Elf>()));
        ASSERT(0 == (bslmf_IsSame<Obj::Policy, Obj::Xcoff>()));
        ASSERT(0 == (bslmf_IsSame<Obj::Policy, Obj::Windows>()));

#endif

#if defined(BSLS_PLATFORM__OS_AIX)

        ASSERT(0 == (bslmf_IsSame<Obj::Policy, Obj::Elf>()));
        ASSERT(1 == (bslmf_IsSame<Obj::Policy, Obj::Xcoff>()));
        ASSERT(0 == (bslmf_IsSame<Obj::Policy, Obj::Windows>()));

#endif

#if defined(BSLS_PLATFORM__OS_WINDOWS)

        ASSERT(0 == (bslmf_IsSame<Obj::Policy, Obj::Elf>()));
        ASSERT(0 == (bslmf_IsSame<Obj::Policy, Obj::Xcoff>()));
        ASSERT(1 == (bslmf_IsSame<Obj::Policy, Obj::Windows>()));

#endif
      }  break;
      case 2: {
        // --------------------------------------------------------------------
        // UNIQUE #define DEFINED
        //
        // Concerns:
        //   Ensure that exactly one object file format #define is defined.
        //
        // Plan:
        //   Increment a count once for each object file format #define and
        //   then verify it's been incremented once.
        // --------------------------------------------------------------------

        if (verbose) cout << "Unique #define defined\n"
                             "======================\n";

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

        ASSERT(1 == count);
      }  break;
      case 1: {
        // --------------------------------------------------------------------
        // IDENTIFIER TEST
        //
        // Concern:
        //   That that 'RESOLVER' identifiers, when defined, have the value
        //   '1', that they are appropriate for the platform, and that never
        //   are there more than one defined at a time.
        //
        // Plan:
        //   For each platform set, check that the appropriate object file
        //   format '#define' is defined, the others are not, and that the
        //   '#define' that is defined evaluates to '1'.
        // --------------------------------------------------------------------

        if (verbose) cout << "Usage Example\n"
                             "=============\n";

#if defined(BSLS_PLATFORM__OS_SOLARIS) || \
    defined(BSLS_PLATFORM__OS_LINUX)   || \
    defined(BSLS_PLATFORM__OS_HPUX)

        ASSERT(1 == BAESU_OBJECTFILEFORMAT_RESOLVER_ELF);

# if defined(BAESU_OBJECTFILEFORMAT_RESOLVER_XCOFF) || \
     defined(BAESU_OBJECTFILEFORMAT_RESOLVER_WINDOWS)
#  error multiple file formats defined
# endif

#elif defined(BSLS_PLATFORM__OS_AIX)

        ASSERT(1 == BAESU_OBJECTFILEFORMAT_RESOLVER_XCOFF);

# if defined(BAESU_OBJECTFILEFORMAT_RESOLVER_ELF) || \
     defined(BAESU_OBJECTFILEFORMAT_RESOLVER_WINDOWS)
#  error multiple file formats defined
# endif

#elif defined(BSLS_PLATFORM__OS_WINDOWS)

        ASSERT(1 == BAESU_OBJECTFILEFORMAT_RESOLVER_WINDOWS);

# if defined(BAESU_OBJECTFILEFORMAT_RESOLVER_ELF) || \
     defined(BAESU_OBJECTFILEFORMAT_RESOLVER_XCOFF)
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
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
