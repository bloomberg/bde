// bdecs_finiteautomatonutil.cpp         -*-C++-*-

#include <bdecs_finiteautomatonutil.h>
#include <bdecs_finiteautomaton.h>
#include <assert.h>

namespace BloombergLP {

void bdecs_FiniteAutomatonUtil::repeat(bdecs_FiniteAutomaton *fa, int numMatch)
{
    assert(0 <= numMatch);

    bdecs_FiniteAutomaton tmp(*fa);
    fa->removeAll();
    for (int i = 0; i < numMatch; ++i) {
        *fa += tmp;
    }
}

void bdecs_FiniteAutomatonUtil::repeat(bdecs_FiniteAutomaton *fa,
                                       int                    minNumMatch,
                                       int                    maxNumMatch)
{
    assert(0 <= minNumMatch);
    assert(0 <= maxNumMatch);
    assert(minNumMatch <= maxNumMatch);

    bdecs_FiniteAutomaton tmp(*fa);
    fa->removeAll();
    int i;
    for (i = 0; i < minNumMatch; ++i) {
        *fa += tmp;
    }
    tmp |= bdecs_FiniteAutomaton();
    for (; i < maxNumMatch; ++i) {
        *fa += tmp;
    }
}

void bdecs_FiniteAutomatonUtil::repeatMin(bdecs_FiniteAutomaton *fa,
                                          int                    numMatch)
{
    assert(0 <= numMatch);

    bdecs_FiniteAutomaton tmp(*fa);
    fa->removeAll();
    for (int i = 0; i < numMatch; ++i) {
        *fa += tmp;
    }
    tmp.assignKleeneStar();
    *fa += tmp;
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
