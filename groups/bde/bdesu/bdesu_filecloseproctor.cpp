// bdesu_filecloseproctor.cpp                                         -*-C++-*-
#include <bdesu_filecloseproctor.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdesu_filecloseproctor_cpp,"$Id$ $CSID$")

namespace BloombergLP {

// MANIPULATORS
void bdesu_FileCloseProctor::closeAndRelease()
{
    if (bdesu_FileUtil::INVALID_FD != d_descriptor) {
        int rc = bdesu_FileUtil::close(d_descriptor);    (void) rc;
        BSLS_ASSERT(0 == rc && "close failed");

        d_descriptor = bdesu_FileUtil::INVALID_FD;
    }
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
