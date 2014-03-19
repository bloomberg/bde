// bdesu_filedescriptorguard.cpp                                      -*-C++-*-
#include <bdesu_filedescriptorguard.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdesu_filecloseproctor_cpp,"$Id$ $CSID$")

namespace BloombergLP {

// MANIPULATORS
void bdesu_FileDescriptorGuard::closeAndRelease()
{
    BSLS_ASSERT(FsUtil::k_INVALID_FD != d_descriptor);

    int rc = FsUtil::close(d_descriptor);
    BSLS_ASSERT(0 == rc && "close failed");
    (void) rc;    // suppress unuwed warning in opt build

    d_descriptor = FsUtil::k_INVALID_FD;
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
