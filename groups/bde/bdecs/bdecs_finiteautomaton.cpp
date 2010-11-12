// bdecs_finiteautomaton.cpp             -*-C++-*-

#include <bdecs_finiteautomaton.h>

#include <bdea_array.h>
#include <bdea_bitarray.h>
#include <bdec_intstack.h>
#include <bdec_intidxset.h>
#include <bdec2_intmap.h>

#include <cassert>
#include <iostream>

                              // --------------
                              // STATIC METHODS
                              // --------------

namespace BloombergLP {

static void copyStates(
               bdea_PtrArray<bdecs_FiniteAutomaton_State>        *dstStatePtrs,
               const bdea_PtrArray<bdecs_FiniteAutomaton_State>&  srcStatePtrs,
               bdema_Pool                                        *d_pool_p)
    // Copy states from 'srcStatePtrs' into 'dstStatePtrs'.  This method is
    // used when one finite automata is merged with another (temporarily
    // producing an NFA).
{
    const int dstLen    = dstStatePtrs->length() - 1;
    const int srcLength = srcStatePtrs.length();

    // Do not copy the 0 state (the error state).

    for (int i = 1; i < srcLength; ++i) {
        dstStatePtrs->append(new (d_pool_p->allocate())
                                                bdecs_FiniteAutomaton_State());
        bdecs_FiniteAutomaton_State *dst = (*dstStatePtrs)[i + dstLen];
        bdecs_FiniteAutomaton_State *src = srcStatePtrs[i];
        for (int j = 0; j < bdecs_FiniteAutomaton_State::SIZE; ++j) {
            if (src->d_transitionTable[j]) {
                dst->d_transitionTable[j] = src->d_transitionTable[j] + dstLen;
            }
        }
        dst->d_status = src->d_status;
    }
}

#ifndef NDEBUG
static int verifyStateHasNoTransitions(bdecs_FiniteAutomaton_State *p)
{
    for (int i = 0; i < bdecs_FiniteAutomaton_State::SIZE; ++i) {
        if (p->d_transitionTable[i]) {
            return 0;
        }
    }
    return 1;
}
#endif

                        // ---------------
                        // PRIVATE METHODS
                        // ---------------

// TBD INEFFICIENT! exponential in the worst case [O(2 sup n) possible subsets]
// TBD certain regular expressions lacking ^ cause the most grief
void bdecs_FiniteAutomaton::nfa2dfa(const bdec2_IntMap<int>& epsilonTrans)
    // Methods which produce NFAs (i.e., because they conceptually
    // introduce epsilon transitions) use this method to convert
    // the finite automaton back to a DFA.  This algorithm is the
    // usual subset construction [Aho, Sethi, and Ullman, pp. 118-9].
{
    int nfaLen = d_statePtrs.length();

    bdea_PtrArray<bdecs_FiniteAutomaton_State> dfaStatePtrs;

    d_pool.reserveCapacity(nfaLen);
    dfaStatePtrs.reserveCapacity(nfaLen);

    // error state and start state of DFA
    dfaStatePtrs.append(new (d_pool.allocate()) bdecs_FiniteAutomaton_State());
    dfaStatePtrs.append(new (d_pool.allocate()) bdecs_FiniteAutomaton_State());
    dfaStatePtrs[1]->d_status = d_statePtrs[d_startState]->d_status;

    // a subset of NFA states
    bdea_BitArray nfaSubsetTmp((bdea_BitArray::Explicit)nfaLen, d_allocator_p);

    // array of NFA state subsets; in 1-1 correspondence with dfaStatePtrs
    bdea_Array<bdea_BitArray> nfaSubsets(
                            (bdea_Array<bdea_BitArray>::InitialCapacity)nfaLen,
                            d_allocator_p);

    nfaSubsets.append(nfaSubsetTmp);  // dummy corresponding to error state
    const int DFA_START = 1;
    nfaSubsetTmp.set1(DFA_START);
    const int *dst = epsilonTrans.lookup(DFA_START);
    if (dst) {
        nfaSubsetTmp.set1(*dst);
        if (d_statePtrs[*dst]->d_status ==
                                      bdecs_FiniteAutomaton_State::ACCEPTING) {
            dfaStatePtrs[1]->d_status = bdecs_FiniteAutomaton_State::ACCEPTING;
        }
    }
    nfaSubsets.append(nfaSubsetTmp);
    nfaSubsetTmp.setAll0();

    // stack of DFA states yet to be explored
    bdec_IntStack expand(bdec_IntStack::InitialCapacity(16), d_allocator_p);
    expand.push(DFA_START);

    int lastDst = 0;
    bdea_BitArray lastSubset((bdea_BitArray::Explicit)nfaLen, d_allocator_p);

    bdec2_IntMap<bdea_Array<int> > nfaSubsetMap(
                       (bdec2_IntMap<bdea_Array<int> >::InitialCapacity)nfaLen,
                       d_allocator_p);

    while (!expand.isEmpty()) {
        int dfaState = expand.top();
        expand.pop();

        for (int j = 0; j < bdecs_FiniteAutomaton_State::SIZE; ++j) {
            int subsetIndex = 0;
            int isAccepting = 0;
            int nonEmpty    = 0;
            int hashValue   = 0;
            bdecs_FiniteAutomaton_State *dfaStatePtr = dfaStatePtrs[dfaState];
            const bdea_BitArray& dfaSubset           = nfaSubsets[dfaState];

            // find subset of NFA states entered from dfaState on 'j'
            while (subsetIndex < nfaLen) {

                subsetIndex = dfaSubset.findBit1AtSmallestIndexGE(subsetIndex);
                if (subsetIndex == -1) {
                    break;
                }

                const int nfaDst =
                                d_statePtrs[subsetIndex]->d_transitionTable[j];
                if (nfaDst && !nfaSubsetTmp[nfaDst]) {
                    nfaSubsetTmp.set1(nfaDst);
                    // TBD possible overflow if > 65535 states
                    hashValue += nfaDst;
                    nonEmpty = 1;
                    if (d_statePtrs[nfaDst]->d_status ==
                                      bdecs_FiniteAutomaton_State::ACCEPTING) {
                        isAccepting = 1;
                    }

                    // check for epsilon transition
                    const int *nfaDstDst = epsilonTrans.lookup(nfaDst);
                    if (nfaDstDst && !nfaSubsetTmp[*nfaDstDst]) {
                        nfaSubsetTmp.set1(*nfaDstDst);
                        // TBD possible overflow if > 65535 states
                        hashValue += *nfaDstDst;
                        if (d_statePtrs[*nfaDstDst]->d_status ==
                                      bdecs_FiniteAutomaton_State::ACCEPTING) {
                            isAccepting = 1;
                        }
                    }
                }
                ++subsetIndex;
            }

            // if empty, then dfaState goes nowhere on 'j'
            if (!nonEmpty) {
                continue;
            }

            // see if this is a new DFA state (NFA subset)
            int dfaDst = -1;
            bdea_Array<int> *array_p = 0;
            if (lastDst && nfaSubsetTmp == lastSubset) {
                dfaDst = lastDst;
            }
            else {
                array_p = nfaSubsetMap.lookup(hashValue);
                if (array_p) {
                    const int len = array_p->length();
                    for (int i = 0; i < len; ++i) {
                        const int dfaState = (*array_p)[i];
                        if (nfaSubsetTmp == nfaSubsets[dfaState]) {
                            dfaDst = dfaState;
                            break;
                        }
                    }
                }
            }

            // if new state, then record it
            if (dfaDst == -1) {
                nfaSubsets.append(nfaSubsetTmp);
                dfaStatePtrs.append(new (d_pool.allocate())
                                                bdecs_FiniteAutomaton_State());
                dfaDst = dfaStatePtrs.length() - 1;
                expand.push(dfaDst);

                if (isAccepting) {
                    dfaStatePtrs[dfaDst]->d_status =
                                        bdecs_FiniteAutomaton_State::ACCEPTING;
                }

                if (array_p) {
                    array_p->append(dfaDst);
                }
                else {
                    bdea_Array<int> array(1, dfaDst, d_allocator_p);
                    nfaSubsetMap.add(hashValue, array);
                }

                lastDst = dfaDst;
                lastSubset = nfaSubsetTmp;
            }
            dfaStatePtr->d_transitionTable[j] = dfaDst;
            nfaSubsetTmp.setAll0();
        }
        lastDst = 0;
        lastSubset.setAll0();
    }

    // replace NFA with the equivalent DFA just constructed

    removeAll();

    const int nStates = dfaStatePtrs.length();

    d_pool.reserveCapacity(nStates - 2);
    d_statePtrs.reserveCapacity(nStates);

    d_pool.deallocate(dfaStatePtrs[0]);
    d_statePtrs[1]->d_status = dfaStatePtrs[1]->d_status;
    bdeimp_DuffsDevice<int>::assignRaw(d_statePtrs[1]->d_transitionTable,
                                       dfaStatePtrs[1]->d_transitionTable,
                                       bdecs_FiniteAutomaton_State::SIZE);
    d_pool.deallocate(dfaStatePtrs[1]);

    for (int i = 2; i < nStates; ++i) {
        d_statePtrs.append(new (d_pool.allocate())
                                                bdecs_FiniteAutomaton_State());
        d_statePtrs[i]->d_status = dfaStatePtrs[i]->d_status;
        bdeimp_DuffsDevice<int>::assignRaw(d_statePtrs[i]->d_transitionTable,
                                           dfaStatePtrs[i]->d_transitionTable,
                                           bdecs_FiniteAutomaton_State::SIZE);
        d_pool.deallocate(dfaStatePtrs[i]);
    }
}

void bdecs_FiniteAutomaton::printImpl(std::ostream& stream) const
{
    const int nStates = d_statePtrs.length();
    for (int i = 0; i < nStates; ++i) {
        stream << i << "   ";
        if (d_startState == i) {
            stream << "S";
        }
        else {
            stream << " ";
        }
        if (d_statePtrs[i]->d_status ==
                                      bdecs_FiniteAutomaton_State::ACCEPTING) {
            stream << "A   ";
        }
        else {
            stream << "    ";
        }
        for (int j = 0; j < bdecs_FiniteAutomaton_State::SIZE; ++j) {
            int value = d_statePtrs[i]->d_transitionTable[j];
            int start = j;
            while (j < bdecs_FiniteAutomaton_State::SIZE &&
                               value == d_statePtrs[i]->d_transitionTable[j]) {
                ++j;
            }
            --j;
            if (value) {
                if (start == j) {
                    stream << j << ":" << value << " ";
                }
                else {
                    stream << start << "-" << j << ":" << value << " ";
                }
            }
        }
        stream << '\n';
    }
}

                                 // --------
                                 // CREATORS
                                 // --------

bdecs_FiniteAutomaton::bdecs_FiniteAutomaton(bdema_Allocator *basicAllocator)
: d_startState(1)
, d_statePtrs(basicAllocator)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
, d_pool(sizeof(bdecs_FiniteAutomaton_State), basicAllocator)
{
    d_pool.reserveCapacity(2);
    d_statePtrs.reserveCapacity(2);
    d_statePtrs.append(new (d_pool.allocate()) bdecs_FiniteAutomaton_State());
    d_statePtrs.append(new (d_pool.allocate()) bdecs_FiniteAutomaton_State());
    d_statePtrs[1]->d_status = bdecs_FiniteAutomaton_State::ACCEPTING;
}

bdecs_FiniteAutomaton::
            bdecs_FiniteAutomaton(const bdecs_FiniteAutomaton&  original,
                                  bdema_Allocator              *basicAllocator)
: d_startState(original.d_startState)
, d_statePtrs(basicAllocator)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
, d_pool(sizeof(bdecs_FiniteAutomaton_State), basicAllocator)
{
    const int nStates = original.d_statePtrs.length();
    d_pool.reserveCapacity(nStates);
    for (int i = 0; i < nStates; ++i) {
        d_statePtrs.append(new (d_pool.allocate())
                                                bdecs_FiniteAutomaton_State());
        d_statePtrs[i]->d_status = original.d_statePtrs[i]->d_status;
        bdeimp_DuffsDevice<int>::
                          assignRaw(d_statePtrs[i]->d_transitionTable,
                                    original.d_statePtrs[i]->d_transitionTable,
                                    bdecs_FiniteAutomaton_State::SIZE);
    }
}

bdecs_FiniteAutomaton::~bdecs_FiniteAutomaton()
{
    assert(1 == d_startState || 0 == d_startState);
    assert(d_startState < d_statePtrs.length());
    assert(d_statePtrs[0]->d_status != bdecs_FiniteAutomaton_State::ACCEPTING);
    assert(verifyStateHasNoTransitions(d_statePtrs[0]));
}

                               // ------------
                               // MANIPULATORS
                               // ------------

bdecs_FiniteAutomaton& bdecs_FiniteAutomaton::
                                    operator=(const bdecs_FiniteAutomaton& rhs)
{
    if (this != &rhs) {
        removeAll();

        const int nStates = rhs.d_statePtrs.length();

        d_pool.reserveCapacity(nStates - 2);
        d_statePtrs.reserveCapacity(nStates);

        d_statePtrs[1]->d_status = rhs.d_statePtrs[1]->d_status;
        bdeimp_DuffsDevice<int>::
                               assignRaw(d_statePtrs[1]->d_transitionTable,
                                         rhs.d_statePtrs[1]->d_transitionTable,
                                         bdecs_FiniteAutomaton_State::SIZE);
        for (int i = 2; i < nStates; ++i) {
            d_statePtrs.append(new (d_pool.allocate())
                                                bdecs_FiniteAutomaton_State());
            d_statePtrs[i]->d_status = rhs.d_statePtrs[i]->d_status;
            bdeimp_DuffsDevice<int>::
                               assignRaw(d_statePtrs[i]->d_transitionTable,
                                         rhs.d_statePtrs[i]->d_transitionTable,
                                         bdecs_FiniteAutomaton_State::SIZE);
        }
        d_startState = rhs.d_startState;
    }

    return *this;
}

bdecs_FiniteAutomaton& bdecs_FiniteAutomaton::
                                   operator+=(const bdecs_FiniteAutomaton& rhs)
{
    if (!isValid() || !rhs.isValid()) {
        return *this;
    }

    bdec2_IntMap<int> epsilonTrans(d_allocator_p);
    const int nStates = d_statePtrs.length();

    // Copy state data (excluding error state) from rhs into this container.
    copyStates(&d_statePtrs, rhs.d_statePtrs, &d_pool);

    // record epsilon transitions (skipping the error state)
    for (int i = 1; i < nStates; ++i) {
        if (d_statePtrs[i]->d_status ==
                                      bdecs_FiniteAutomaton_State::ACCEPTING) {
            d_statePtrs[i]->d_status =
                                    bdecs_FiniteAutomaton_State::NOT_ACCEPTING;
            epsilonTrans.add(i, nStates);
        }
    }

    nfa2dfa(epsilonTrans);

    return *this;
}

bdecs_FiniteAutomaton& bdecs_FiniteAutomaton::
                                   operator|=(const bdecs_FiniteAutomaton& rhs)
{
    if (!isValid() || !rhs.isValid()) {
        return *this;
    }

    bdec2_IntMap<int> epsilonTrans(d_allocator_p);
    const int nStates = d_statePtrs.length();

    // Copy state data (excluding error state) from rhs into this container.
    copyStates(&d_statePtrs, rhs.d_statePtrs, &d_pool);

    epsilonTrans.add(d_startState, nStates);
    nfa2dfa(epsilonTrans);

    return *this;
}

void bdecs_FiniteAutomaton::append(char accept)
{
    if (!isValid()) {
        return;
    }

    bdec2_IntMap<int> epsilonTrans(d_allocator_p);
    const int nStates = d_statePtrs.length();

    // hand-craft the states of an f.a. that matches 'accept'

    bdecs_FiniteAutomaton_State *st;
    bdea_PtrArray<bdecs_FiniteAutomaton_State> statePtrs;

    st = new (d_pool.allocate()) bdecs_FiniteAutomaton_State();  // error (0)
    statePtrs.append(st);
    st = new (d_pool.allocate()) bdecs_FiniteAutomaton_State();  // start (1)
    st->d_transitionTable[(unsigned char)accept] = 2;
    statePtrs.append(st);
    st = new (d_pool.allocate()) bdecs_FiniteAutomaton_State();  // final (2)
    st->d_status = bdecs_FiniteAutomaton_State::ACCEPTING;
    statePtrs.append(st);

    copyStates(&d_statePtrs, statePtrs, &d_pool);

    // record epsilon transitions (skipping the error state)
    for (int i = 1; i < nStates; ++i) {
        if (d_statePtrs[i]->d_status ==
                                      bdecs_FiniteAutomaton_State::ACCEPTING) {
            d_statePtrs[i]->d_status =
                                    bdecs_FiniteAutomaton_State::NOT_ACCEPTING;
            epsilonTrans.add(i, nStates);
        }
    }

    nfa2dfa(epsilonTrans);
}

void bdecs_FiniteAutomaton::appendSet(const char *accept, int length)
{
    if (!isValid() || !length) {
        return;
    }

    bdec2_IntMap<int> epsilonTrans(d_allocator_p);
    const int nStates = d_statePtrs.length();

    // hand-craft the states of an f.a. that matches 'accept'

    bdecs_FiniteAutomaton_State *st;
    bdea_PtrArray<bdecs_FiniteAutomaton_State> statePtrs;

    st = new (d_pool.allocate()) bdecs_FiniteAutomaton_State();  // error (0)
    statePtrs.append(st);
    st = new (d_pool.allocate()) bdecs_FiniteAutomaton_State();  // start (1)
    statePtrs.append(st);
    for (int i = 0; i < length; ++i) {
        st->d_transitionTable[(unsigned char)accept[i]] = 2;
    }
    st = new (d_pool.allocate()) bdecs_FiniteAutomaton_State();
    st->d_status = bdecs_FiniteAutomaton_State::ACCEPTING;
    statePtrs.append(st);

    copyStates(&d_statePtrs, statePtrs, &d_pool);

    // record epsilon transitions (skipping the error state)
    for (int i = 1; i < nStates; ++i) {
        if (d_statePtrs[i]->d_status ==
                                      bdecs_FiniteAutomaton_State::ACCEPTING) {
            d_statePtrs[i]->d_status =
                                    bdecs_FiniteAutomaton_State::NOT_ACCEPTING;
            epsilonTrans.add(i, nStates);
        }
    }

    nfa2dfa(epsilonTrans);
}

void bdecs_FiniteAutomaton::appendSequence(const char *accept, int length)
{
    if (!isValid() || !length) {
        return;
    }

    bdec2_IntMap<int> epsilonTrans(d_allocator_p);
    const int nStates = d_statePtrs.length();

    // hand-craft the states of an f.a. that matches 'accept'

    bdecs_FiniteAutomaton_State *st;
    bdea_PtrArray<bdecs_FiniteAutomaton_State> statePtrs;

    st = new (d_pool.allocate()) bdecs_FiniteAutomaton_State();  // error (0)
    statePtrs.append(st);
    for (int i = 0; i < length; ++i) {
        st = new (d_pool.allocate()) bdecs_FiniteAutomaton_State();
        st->d_transitionTable[(unsigned char)accept[i]] = i+2;
        statePtrs.append(st);
    }
    st = new (d_pool.allocate()) bdecs_FiniteAutomaton_State();  // final
    statePtrs.append(st);
    st->d_status = bdecs_FiniteAutomaton_State::ACCEPTING;

    copyStates(&d_statePtrs, statePtrs, &d_pool);

    // record epsilon transitions (skipping the error state)
    for (int i = 1; i < nStates; ++i) {
        if (d_statePtrs[i]->d_status ==
                                      bdecs_FiniteAutomaton_State::ACCEPTING) {
            d_statePtrs[i]->d_status =
                                    bdecs_FiniteAutomaton_State::NOT_ACCEPTING;
            epsilonTrans.add(i, nStates);
        }
    }

    nfa2dfa(epsilonTrans);
}

void bdecs_FiniteAutomaton::assignKleeneStar()
{
    if (!isValid()) {
        return;
    }

    bdec2_IntMap<int> epsilonTrans(d_allocator_p);
    const int nStates = d_statePtrs.length();

    // record epsilon transitions (skipping the error and start states)
    for (int i = 2; i < nStates; ++i) {
        if (d_statePtrs[i]->d_status ==
                                      bdecs_FiniteAutomaton_State::ACCEPTING) {
            epsilonTrans.add(i, d_startState);
        }
    }
    d_statePtrs[d_startState]->d_status =
                                        bdecs_FiniteAutomaton_State::ACCEPTING;

    nfa2dfa(epsilonTrans);
}

bdecs_FiniteAutomaton bdecs_FiniteAutomaton::kleeneStar()
{
    bdecs_FiniteAutomaton tmp(*this);
    tmp.assignKleeneStar();
    return tmp;
}

void bdecs_FiniteAutomaton::removeAll()
{
    const int nStates = d_statePtrs.length();
    for (int i = 2; i < nStates; ++i) {
        d_pool.deallocate(d_statePtrs[i]);
    }
    d_statePtrs.setLengthRaw(2);
    d_startState = 1;
    d_statePtrs[1]->d_status = bdecs_FiniteAutomaton_State::ACCEPTING;
    bdeimp_DuffsDevice<int>::initializeRaw(d_statePtrs[1]->d_transitionTable,
                                           0,
                                           bdecs_FiniteAutomaton_State::SIZE);
}

                                 // ---------
                                 // ACCESSORS
                                 // ---------

void bdecs_FiniteAutomaton::print() const
{
    printImpl(std::cout);
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
