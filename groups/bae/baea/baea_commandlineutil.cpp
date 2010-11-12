// baea_commandlineutil.cpp   -*-C++-*-
#include <baea_commandlineutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baea_commandlineutil_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>

#include <bsl_cstring.h>

namespace BloombergLP {

int baea_CommandLineUtil::splitCommandLineArguments(
        bsl::vector<char *>  *leftArgs,
        bsl::vector<char *>  *rightArgs,
        int                   argc,
        char                **argv,
        const char           *separator)
{
    BSLS_ASSERT(leftArgs  != 0);
    BSLS_ASSERT(rightArgs != 0);
    BSLS_ASSERT(separator != 0);

    if (0 == argc) {
        return -1;
    }

    BSLS_ASSERT(argv != 0);

    leftArgs->push_back(argv[0]);
    rightArgs->push_back(argv[0]);

    int i = 1;  // Skip 'argv[0]'.

    while (i < argc) {
        if (0 == bsl::strcmp(argv[i], separator)) {
            ++i;
            break;
        }
        leftArgs->push_back(argv[i++]);
    }

    while (i < argc) {
        rightArgs->push_back(argv[i++]);
    }

    return 0;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
