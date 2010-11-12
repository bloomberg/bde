// btemt_asyncchannel.cpp          -*-C++-*-
#include <btemt_asyncchannel.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(btemt_asyncchannel_cpp,"$Id$ $CSID$")

#ifdef BTE_FOR_TESTING_ONLY
// These dependencies need to be here for the bde_build.pl script to
// generate the proper makefiles, but do not need to be compiled into the
// component's .o file.  The symbol BTE_FOR_TESTING_ONLY should remain
// undefined, and is here only because '#if 0' is optimized away by the
// bde_build.pl script.

#include <btemt_message.h>                  // for testing only
#endif

namespace BloombergLP {

btemt_AsyncChannel::~btemt_AsyncChannel()
{
}

} // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
