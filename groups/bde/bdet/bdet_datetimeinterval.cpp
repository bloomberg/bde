// bdet_datetimeinterval.cpp    -*-C++-*-
#include <bdet_datetimeinterval.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdet_datetimeinterval_cpp,"$Id$ $CSID$")


#include <bsl_cstdio.h>    // 'sprintf'
#include <bsl_ostream.h>

namespace BloombergLP {

struct bdet_DatetimeInterval_Assertions {
    char assertion1[-3 / 2 == -1];  // insure platform has the "right"
    char assertion2[-5 % 4 == -1];  // implementation
};

                        // ---------------------------
                        // class bdet_DatetimeInterval
                        // ---------------------------

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const bdet_DatetimeInterval& timeInterval)
{
    // space usage:  s dd...d_  hh: mm: ss: uuu null
    const int SIZE = 1 + 24   +  3 + 3 + 3 + 3 + 1;
    char buf[SIZE];

    const int d = timeInterval.days();
    const int h = timeInterval.hours();
    const int m = timeInterval.minutes();
    const int s = timeInterval.seconds();
    const int u = timeInterval.milliseconds();

    if (d < 0 || h < 0 || m < 0 || s < 0 || u < 0) {
        bsl::sprintf(buf, "-%d_%02d:%02d:%02d.%03d", -d, -h, -m, -s, -u);
    }
    else {
        bsl::sprintf(buf, "+%d_%02d:%02d:%02d.%03d", d, h, m, s, u);
    }

    return stream << buf;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
