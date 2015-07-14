// btlsc_channelallocator.cpp   -*-C++-*-
#include <btlsc_channelallocator.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlsc_channelallocator_cpp,"$Id$ $CSID$")

#ifdef BTE_FOR_TESTING_ONLY
// These dependencies need to be here for the the bde_build.pl script to
// generate the proper makefiles, but do not need to be compiled into the
// component's .o file.  The symbol BTE_FOR_TESTING_ONLY should remain
// undefined, and is here only because '#if 0' is optimized away by the
// bde_build.pl script.

#include <btlsc_channel.h>                  // for testing only
#include <btlsc_timedchannel.h>             // for testing only
#include <btlsc_flag.h>                     // for testing only
#endif

namespace BloombergLP {

namespace btlsc {
    ChannelAllocator::~ChannelAllocator() { }
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
