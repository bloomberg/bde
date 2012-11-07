// bsls_platform.cpp                                                  -*-C++-*-
#include <bsls_platform.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

                        // --------------------
                        // class bsls::Platform
                        // --------------------

// This class has no implementation; it is merely a place-holder for this
// component's logical name space.

// The compiler flags set up by the 'bsls_platform' header file will be sanity
// checked against those those passed in by the build tool.

// Ensure compiler flags are set when they have to be.
#if defined(BSLS_PLATFORM_CMP_MSVC)
    #if defined(BDE_BUILD_TARGET_EXC) && !defined(_CPPUNWIND)
        #error "Exception build without exception handling enabled (/GX)."
    #endif
#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
