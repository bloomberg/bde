// bteso_streamsocketfactory.cpp          -*-C++-*-
#include <bteso_streamsocketfactory.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bteso_streamsocketfactory_cpp,"$Id$ $CSID$")

#ifdef BTE_FOR_TESTING_ONLY
// These dependencies need to be here for the the bde_build.pl script to
// generate the proper makefiles, but do not need to be compiled into the
// component's .o file.  The symbol BTE_FOR_TESTING_ONLY should remain
// undefined, and is here only because '#if 0' is optimized away by the
// bde_build.pl script.

#include <bteso_streamsocket.h>             // for testing only
#include <bteso_socketoptutil.h>            // for testing only
#include <bteso_sockethandle.h>             // for testing only
#include <bteso_platform.h>                 // for testing only
#include <bteso_flag.h>                     // for testing only
#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
