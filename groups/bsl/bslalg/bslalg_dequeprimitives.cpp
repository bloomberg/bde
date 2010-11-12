// bslalg_dequeprimitives.cpp                  -*-C++-*-
#include <bslalg_dequeprimitives.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

//
// IMPLEMENTATION NOTES:
//
// In case of aliasing, we make a copy of the object through a constructor
// proxy.  This is because the copy constructor might take more than one
// argument.  We cannot just check whether the range is between
// 'fromEnd' and 'position' because there's no way of checking whether
// 'value' is located inside a 'bslstl_Deque' or not.


namespace BloombergLP {

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
