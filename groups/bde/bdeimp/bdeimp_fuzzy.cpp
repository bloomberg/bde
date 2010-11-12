// bdeimp_fuzzy.cpp          -*-C++-*-
#include <bdeimp_fuzzy.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdeimp_fuzzy_cpp,"$Id$ $CSID$")

namespace BloombergLP {

const double bdeimp_Fuzzy::S_DEFAULT_REL_TOL = 1e-12;  // Default relative and
const double bdeimp_Fuzzy::S_DEFAULT_ABS_TOL = 1e-24;  // absolute tolerances.

static inline
double fabsval(double input)
    // Return the absolute value of the specified 'input'.
{
    return input >= 0.0 ? input : -input;
}

                        // ------------------
                        // class bdeimp_Fuzzy
                        // ------------------

// CLASS METHODS
int bdeimp_Fuzzy::compare(double a, double b, double relTol, double absTol)
{
    enum {
        FUZZY_LESS_THAN    = -1,  // 'a' is fuzzy-less-than    'b'
        FUZZY_EQUALITY     =  0,  // 'a' is fuzzy-equal-to     'b'
        FUZZY_GREATER_THAN = +1   // 'a' is fuzzy-greater-than 'b'
    };

    // First check for the *very* special case of true equality.

    if (a == b) {
        return FUZZY_EQUALITY;                                        // RETURN
    }

    // Next check for special case where the relative difference is "infinite".

    if (a == -b) {
        if (fabsval(a - b) <= absTol) {
            return FUZZY_EQUALITY;                                    // RETURN
        }
        else if (a < b) {
            return FUZZY_LESS_THAN;                                   // RETURN
        }
        else {
            return FUZZY_GREATER_THAN;                                // RETURN
        }
    }

    // Otherwise process the normal case involving both absolute and relative
    // tolerances.

    const double difference = fabsval(a - b);
    const double average    = fabsval((a + b) / 2.0);

    if (difference <= absTol || difference / average <= relTol) {
        return FUZZY_EQUALITY;                                        // RETURN
    }
    else if (a < b) {
        return FUZZY_LESS_THAN;                                       // RETURN
    }
    else {
        return FUZZY_GREATER_THAN;                                    // RETURN
    }
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
