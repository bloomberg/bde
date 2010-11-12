// bdet_monthofyear.cpp               -*-C++-*-

#include <bdet_monthofyear.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdet_monthofyear_cpp,"$Id$ $CSID$")

#include <bdex_testinstream.h>           // for testing only
#include <bdex_testoutstream.h>          // for testing only
#include <bdex_testinstreamexception.h>  // for testing only

namespace BloombergLP {

const char *bdet_MonthOfYear::toAscii(bdet_MonthOfYear::Month month)
{
#define CASE(X) case(X): return #X;
    switch(month) {
      case(BDET_JAN) : return "JAN";
      case(BDET_FEB) : return "FEB";
      case(BDET_MAR) : return "MAR";
      case(BDET_APR) : return "APR";
      case(BDET_MAY) : return "MAY";
      case(BDET_JUN) : return "JUN";
      case(BDET_JUL) : return "JUL";
      case(BDET_AUG) : return "AUG";
      case(BDET_SEP) : return "SEP";
      case(BDET_OCT) : return "OCT";
      case(BDET_NOV) : return "NOV";
      case(BDET_DEC) : return "DEC";
      default: return "(* UNKNOWN *)";
    };
#undef CASE
}

bsl::ostream &operator<<(bsl::ostream& output, bdet_MonthOfYear::Month month)
{
    return output << bdet_MonthOfYear::toAscii(month);
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
