// bdeat_testvaluefunctions.cpp                  -*-C++-*-

#include <bdeat_testvaluefunctions.h>

#ifdef BDE_FOR_TESTING_ONLY
// These dependencies need to be here for the the bde_build.pl script to
// generate the proper makefiles, but do not need to be compiled into the
// component's .o file.  The symbol BDE_FOR_TESTING_ONLY should remain
// undefined, and is here only because '#if 0' is optimized away by the
// bde_build.pl script.

#include <bdeat_attributeinfo.h>      // for testing only
#include <bdeat_enumeratorinfo.h>     // for testing only
#include <bdeat_enumfunctions.h>      // for testing only
#include <bdeat_choicefunctions.h>    // for testing only
#include <bdeat_sequencefunctions.h>  // for testing only
#include <bdeat_typetraits.h>         // for testing only
#include <bdeat_valuetypefunctions.h> // for testing only

namespace BloombergLP {

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
