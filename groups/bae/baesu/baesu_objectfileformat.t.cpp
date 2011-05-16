// baesu_objectfileformat.t.cpp                                       -*-C++-*-

#include <baesu_objectfileformat.h>

#include <bsl_iostream.h>
#include <bsl_cstdlib.h>     // atoi()

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

static int testStatus = 0;

//=============================================================================
//                    HELPER FUNCTIONS FOR USAGE EXAMPLE
//=============================================================================

// 'typeTest' is a template function that will return 1 if passed an object of
// types 'baesu_ObjectFileFormat::{Elf,Xcoff,Windows}' appropriate for the
// current platform and 0 otherwise.

template <typename TYPE>
int typeTest(const TYPE &) {
    return 0;
}

#if defined(BAESU_OBJECTFILEFORMAT_RESOLVER_ELF)

int typeTest(const baesu_ObjectFileFormat::Elf &)
{
    return 1;
}

#endif
#if defined(BAESU_OBJECTFILEFORMAT_RESOLVER_XCOFF)

int typeTest(const baesu_ObjectFileFormat::Xcoff &)
{
    return 1;
}

#endif
#if defined(BAESU_OBJECTFILEFORMAT_RESOLVER_WINDOWS)

int typeTest(const baesu_ObjectFileFormat::Windows &)
{
    return 1;
}

#endif

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
// [ 1] Ensure that ThreadPolicy is set.
// [ 1] Ensure that exactly one of each THREADS type is set.
//==========================================================================

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 1: {
        // --------------------------------------------------------------------
        // Usage Example:
        //   Make sure that /baesu_ObjectFileFormat::ResolverPolicy' is defined
        //   to be 'Elf', 'Xcoff', or 'Windows', and that exactly one of
        //   'BAESU_OBJECTFILEFORMAT_RESOLVER_{ELF,XCOFF,WINDOWS}' is defined.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Usage Example" <<
                             endl << "-------------" << endl;

        // First, verify ResolverPolicy

        baesu_ObjectFileFormat::Policy policy;
        BSLS_ASSERT(1 == typeTest(policy));

        // Finally, verify 1 'BAESU_OBJECTFILEFORMAT_RESOLVER_*' #define
        // exists

        int count = 0;
#if defined(BAESU_OBJECTFILEFORMAT_RESOLVER_ELF)
        ++count;
        BSLS_ASSERT(1 == BAESU_OBJECTFILEFORMAT_RESOLVER_ELF);
#endif
#if defined(BAESU_OBJECTFILEFORMAT_RESOLVER_XCOFF)
        ++count;
        BSLS_ASSERT(1 == BAESU_OBJECTFILEFORMAT_RESOLVER_XCOFF);
#endif
#if defined(BAESU_OBJECTFILEFORMAT_RESOLVER_WINDOWS)
        ++count;
        BSLS_ASSERT(1 == BAESU_OBJECTFILEFORMAT_RESOLVER_WINDOWS);
#endif
        BSLS_ASSERT(1 == count);
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
