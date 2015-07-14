// btlso_streamsocketfactory.cpp          -*-C++-*-
#include <btlso_streamsocketfactory.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlso_streamsocketfactory_cpp,"$Id$ $CSID$")

#ifdef BTE_FOR_TESTING_ONLY
// These dependencies need to be here for the the bde_build.pl script to
// generate the proper makefiles, but do not need to be compiled into the
// component's .o file.  The symbol BTE_FOR_TESTING_ONLY should remain
// undefined, and is here only because '#if 0' is optimized away by the
// bde_build.pl script.

#include <btlso_streamsocket.h>             // for testing only
#include <btlso_socketoptutil.h>            // for testing only
#include <btlso_sockethandle.h>             // for testing only
#include <btlso_platform.h>                 // for testing only
#include <btlso_flag.h>                     // for testing only
#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
