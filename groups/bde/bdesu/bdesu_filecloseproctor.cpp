// bdesu_filecloseproctor.cpp                                         -*-C++-*-
#include <bdesu_filecloseproctor.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdesu_filecloseproctor_cpp,"$Id$ $CSID$")

namespace BloombergLP {

// CREATOPS
bdesu_FileCloseProctor::~bdesu_FileCloseProctor()
{
    if (bdesu_FileUtil::INVALID_FD != d_descriptor) {
        BSLS_ASSERT_OPT(0 == d_file_p);

        int rc = bdesu_FileUtil::close(d_descriptor);
        BSLS_ASSERT_OPT(0 == rc && "~bdesu_FileCloseProctor: close failed.");
    }
    else if (d_file_p) {
        int rc = bsl::fclose(d_file_p);
        BSLS_ASSERT_OPT(0 == rc && "~bdesu_FileCloseProctor: fclose failed.");
    }
}

// MANIPULATORS
void bdesu_FileCloseProctor::closeAndRelease()
{
    if (bdesu_FileUtil::INVALID_FD != d_descriptor) {
        BSLS_ASSERT_OPT(0 == d_file_p);

        int rc = bdesu_FileUtil::close(d_descriptor);
        BSLS_ASSERT_OPT(0 == rc &&
                             "bdesu_FileCloseProctor::release: close failed.");

        d_descriptor = bdesu_FileUtil::INVALID_FD;
    }
    else if (d_file_p) {
        int rc = bsl::fclose(d_file_p);
        BSLS_ASSERT_OPT(0 == rc &&
                            "bdesu_FileCloseProctor::release: fclose failed.");

        d_file_p = 0;
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
