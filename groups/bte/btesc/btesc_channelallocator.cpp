// btesc_channelallocator.cpp   -*-C++-*-
#include <btesc_channelallocator.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(btesc_channelallocator_cpp,"$Id$ $CSID$")

#ifdef BTE_FOR_TESTING_ONLY
// These dependencies need to be here for the the bde_build.pl script to
// generate the proper makefiles, but do not need to be compiled into the
// component's .o file.  The symbol BTE_FOR_TESTING_ONLY should remain
// undefined, and is here only because '#if 0' is optimized away by the
// bde_build.pl script.

#include <btesc_channel.h>                  // for testing only
#include <btesc_timedchannel.h>             // for testing only
#include <btesc_flag.h>                     // for testing only
#endif

namespace BloombergLP {

    btesc_ChannelAllocator::~btesc_ChannelAllocator() { }

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
