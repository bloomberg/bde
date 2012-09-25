// bslstl_sstream.t.cpp                                               -*-C++-*-
#include <bslstl_sstream.h>

int main()
{
    // verify that string stream classes are included from bslstl_sstream.h
    bsl::stringbuf stringBuf;           (void) stringBuf;
    bsl::istringstream inputStream;     (void) inputStream;
    bsl::ostringstream outputStream;    (void) outputStream;
    bsl::stringstream inoutStream;      (void) inoutStream;

    return -1;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
