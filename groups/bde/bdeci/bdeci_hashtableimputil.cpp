// bdeci_hashtableimputil.cpp              -*-C++-*-
#include <bdeci_hashtableimputil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdeci_hashtableimputil_cpp,"$Id$ $CSID$")

#include <bsls_platformutil.h>

namespace BloombergLP {

int bdeci_HashtableImpUtil::sizes[] =
{
             1,
             5,
            23,
            97,
           389,
          1559,
          6247,
         24989,
         99961,
        399851,
       1599407,
       6397631,
      25590553,
     102362243,
     409448981,
    1637795933,
    2147483647
};

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
