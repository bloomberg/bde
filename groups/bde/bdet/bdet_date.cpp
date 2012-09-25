// bdet_date.cpp                                                      -*-C++-*-
#include <bdet_date.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdet_date_cpp,"$Id$ $CSID$")

#include <bdeimp_dateutil.h>
#include <bdeu_print.h>

#include <bsls_assert.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>

#include <bsl_cstdio.h>    // 'fprintf'
#include <bsl_ostream.h>

#include <bsl_c_stdio.h>   // 'snprintf'

static const char *const months[] = {
    0,
    "JAN", "FEB", "MAR", "APR",
    "MAY", "JUN", "JUL", "AUG",
    "SEP", "OCT", "NOV", "DEC"
};

namespace BloombergLP {

                        // ---------------
                        // class bdet_Date
                        // ---------------

// MANIPULATORS
int bdet_Date::addDaysIfValid(int numDays)
{
    enum { BDET_SUCCESS = 0, BDET_FAILURE = -1 };

    const int tmpDate = d_date + numDays;

    if (!bdeimp_DateUtil::isValidSerialDate(tmpDate)) {
        return BDET_FAILURE;
    }

    d_date = tmpDate;

    return BDET_SUCCESS;
}

// ACCESSORS
bsl::ostream& bdet_Date::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(stream.bad())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return stream;                                                // RETURN
    }

    // Typical space usage: ddMMMyyyy nil.  Reserve 128 for possible BAD DATE
    // result, which is sufficient space for the bad date verbose message even
    // on a 64-bit system.

    char buffer[128];

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                !bdeimp_DateUtil::isValidSerialDate(d_date))) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

#if defined(BSLS_PLATFORM_CMP_MSVC)
#define snprintf _snprintf
#endif

        snprintf(buffer,
                 sizeof buffer,
                 "*BAD*DATE:%p->d_date=%d",
                 (void *)this,
                 d_date);

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
        bsl::fprintf(stderr, "'bdet_Date' invariant violated: %s.\n", buffer);
#endif
        BSLS_ASSERT_SAFE(
                !"bdet_Date::print attempted on date with invalid state.");
    }
    else {
#endif  // defined(BSLS_ASSERT_OPT_IS_ACTIVE)

        int y, m, d;
        getYearMonthDay(&y, &m, &d);

        const char *const month = months[m];

        buffer[0] = d / 10 + '0';
        buffer[1] = d % 10 + '0';
        buffer[2] = month[0];
        buffer[3] = month[1];
        buffer[4] = month[2];
        buffer[5] = y / 1000 + '0';
        buffer[6] = ((y % 1000) / 100) + '0';
        buffer[7] = ((y % 100) / 10) + '0';
        buffer[8] = y % 10 + '0';
        buffer[9] = 0;

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
    }
#endif  // defined(BSLS_ASSERT_OPT_IS_ACTIVE)

    bdeu_Print::indent(stream, level, spacesPerLevel);

    stream << buffer;

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }

    return stream;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
