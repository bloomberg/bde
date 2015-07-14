// bdlsu_filedescriptorguard.cpp                                      -*-C++-*-
#include <bdlsu_filedescriptorguard.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdesu_filecloseproctor_cpp,"$Id$ $CSID$")

namespace BloombergLP {

namespace bdlsu {
// MANIPULATORS
void FileDescriptorGuard::closeAndRelease()
{
    BSLS_ASSERT(FilesystemUtil::k_INVALID_FD != d_descriptor);

    int rc = FilesystemUtil::close(d_descriptor);
    BSLS_ASSERT(0 == rc && "close failed");
    (void) rc;    // suppress unuwed warning in opt build

    d_descriptor = FilesystemUtil::k_INVALID_FD;
}
}  // close package namespace

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
