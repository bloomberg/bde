// bdecs_finiteautomaton.h             -*-C++-*-
#ifndef INCLUDED_BDECS_FINITEAUTOMATON
#define INCLUDED_BDECS_FINITEAUTOMATON

//@PURPOSE: Provide a finite automaton.
//
//@CLASSES:
//              bdecs_FiniteAutomaton: manager for a finite automaton
//   bdecs_FiniteAutomatonAccumulator: traverser of a finite automaton
//
//@AUTHOR: Jeffrey Mendelsohn (jmendels)
//
//@SEE_ALSO: bdecs_finiteautomatonutil, bdecs_regex
//
//@DESCRIPTION: This component implements a finite automaton.  The intended use
// of this component is to define a finite automaton using the methods of
// 'bdecs_FiniteAutomaton' and then traverse the automaton using
// 'bdecs_FiniteAutomatonAccumulator' to determine if a given input is matched
// by the finite automaton, may be matched by the finite automaton
// (i.e., further input may provide a match), or can never be matched by the
// finite automaton.
//
///USAGE
///-----
// The following snippets of code illustrate how to create and use a
// 'bdecs_FiniteAutomaton' and a 'bdecs_FiniteAutomatonAccumulator'.  First
// create a 'bdecs_FiniteAutomaton' 'f' that matches zero length inputs:
//..
//      bdecs_FiniteAutomaton f(&testAllocator);
//      {
//          bdecs_FiniteAutomatonAccumulator a(f);
//          assert(0 == a.isError());
//          assert(1 == a.isAccepting());
//          a.transition('c');
//          assert(1 == a.isError());
//          assert(0 == a.isAccepting());
//      }
//..
// Now modify 'f' to accept the sequence "abc".
//..
//      f.appendSequence("abc", 3);
//      {
//          bdecs_FiniteAutomatonAccumulator a(f);
//          assert(0 == a.isError());
//          assert(0 == a.isAccepting());
//          a.transition('a');
//          a.transition('b');
//          a.transition('c');
//          assert(0 == a.isError());
//          assert(1 == a.isAccepting());
//          a.transition('c');
//          assert(1 == a.isError());
//          assert(0 == a.isAccepting());
//      }
//..
// Finally apply the Kleene Star operation to the finite automaton (require it
// to match zero or more instances of inputs that it originally matched):
//..
//      f.assignKleeneStar();
//      {
//          bdecs_FiniteAutomatonAccumulator a(f);
//          assert(0 == a.isError());
//          assert(1 == a.isAccepting());
//          a.transition('a');
//          a.transition('b');
//          a.transition('c');
//          assert(0 == a.isError());
//          assert(1 == a.isAccepting());
//          a.transition('a');
//          a.transition('b');
//          a.transition('c');
//          assert(0 == a.isError());
//          assert(1 == a.isAccepting());
//          a.transition('c');
//          assert(1 == a.isError());
//          assert(0 == a.isAccepting());
//      }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEMA_ALLOCATOR
#include <bdema_allocator.h>
#endif

#ifndef INCLUDED_BDEMA_DEFAULT
#include <bdema_default.h>
#endif

#ifndef INCLUDED_BDEMA_POOL
#include <bdema_pool.h>
#endif

#ifndef INCLUDED_BDEIMP_DUFFSDEVICE
#include <bdeimp_duffsdevice.h>
#endif

#ifndef INCLUDED_BDEA_PTRARRAY
#include <bdea_ptrarray.h>
#endif

#ifndef INCLUDED_BDEC2_INTMAP
#include <bdec2_intmap.h>
#endif

#ifndef INCLUDED_IOSFWD
#include <iosfwd>
#define INCLUDED_IOSFWD
#endif

namespace BloombergLP {

                  // =================================
                  // class bdecs_FiniteAutomaton_State
                  // =================================

// This class could not be put into the class and duplicated in the .cpp due to
// templating issues.

struct bdecs_FiniteAutomaton_State {
    enum {
        SIZE = 256,       // number of possible transition values
        ACCEPTING = 1,    // status value indicating accepting state
        NOT_ACCEPTING = 0 // status value indicating not an accepting state
    };
    int  d_transitionTable[SIZE];
    char d_status;

    bdecs_FiniteAutomaton_State() : d_status(NOT_ACCEPTING) {
        bdeimp_DuffsDevice<int>::initializeRaw(d_transitionTable, 0, SIZE);
    }
};

                     // ===========================
                     // class bdecs_FiniteAutomaton
                     // ===========================

class bdecs_FiniteAutomaton {
    // This class implements a temporally-efficient finite automaton.

    friend class bdecs_FiniteAutomatonAccumulator;

    int                                         d_startState;
    bdea_PtrArray<bdecs_FiniteAutomaton_State>  d_statePtrs;
    bdema_Allocator                            *d_allocator_p;
    bdema_Pool                                  d_pool;

private:
    void nfa2dfa(const bdec2_IntMap<int>& epsilonTrans);
        // NFA-to-DFA conversion function.  This function is used
        // by 'append', 'appendSequence', 'appendSet', 'operator+=',
        // 'operator|=' and 'assignedKleeneStar' to remove epsilon transitions
        // from NFAs.  'epsilonTrans' describes the epsilon transitions which
        // make this automaton nondeterministic.

    void printImpl(std::ostream& stream) const;
        // Write the specified 'finiteAutomaton' to the specified output
        // 'stream' in some reasonable format.

public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    static int maxSupportedVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)
        //
        // DEPRECATED: replaced by 'maxSupportedBdexVersion()'

    // CREATORS
    bdecs_FiniteAutomaton(bdema_Allocator *basicAllocator = 0);
        // Create a finite automaton that accepts only the empty (null) string.
        // Optionally specify a 'basicAllocator' used to supply  memory.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.

    bdecs_FiniteAutomaton(const bdecs_FiniteAutomaton&  original,
                          bdema_Allocator              *basicAllocator = 0);
        // Create a finite automaton having the identical accepting criteria as
        // the specified 'original' finite automaton.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~bdecs_FiniteAutomaton();
        // Destroy this object.

    // MANIPULATORS
    bdecs_FiniteAutomaton& operator=(const bdecs_FiniteAutomaton& rhs);
        // Assign to this finite automaton the exact accepting criteria of the
        // specified 'rhs' finite automaton and return a reference to this
        // modifiable finite automaton.

    bdecs_FiniteAutomaton& operator+=(const bdecs_FiniteAutomaton& rhs);
        // Assign to this finite automaton a finite automaton that accepts
        // inputs that for some index of the input data, the elements of the
        // input up to the index are accepted by the original value of this
        // finite automaton and the residual elements of the input are accepted
        // by the specified 'rhs' finite automaton.  Return a reference to
        // this modifiable finite automaton.

    bdecs_FiniteAutomaton& operator|=(const bdecs_FiniteAutomaton& rhs);
        // Assign to this finite automaton a finite automaton that accepts
        // inputs that are accepted by one or both of the original value of
        // this finite automaton and the specified 'rhs' finite automaton.
        // Return a reference to this modifiable finite automaton.

    void append(char accept);
        // Append to this finite automaton a finite automaton that accepts the
        // specified 'accept' as input.

    void appendSet(const char *accept, int length);
        // Append to this finite automaton a finite automaton that accepts any
        // one of the values in the specified array 'accept' of specified
        // 'length' as input.

    void appendSequence(const char *accept, int length);
        // Append to this finite automaton a finite automaton that accepts the
        // input sequence defined by the specified array 'accept' of specified
        // 'length'.

    void assignKleeneStar();
        // Assign to this finite automaton a finite automaton that accepts
        // any arbitrary non-negative number of inputs the are accepted by the
        // original value of this finite automaton.

    bdecs_FiniteAutomaton kleeneStar();
        // Return a finite automaton that accepts any arbitrary non-negative
        // number of inputs that are accepted by this finite automaton.

    void removeAll();
        // Assign to this finite automaton one that accepts only the empty
        // (null) string.

    void invalidate();
        // Mark this finite automaton as invalid.  This function has no
        // effect if this finite automaton is already invalid.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    template <class STREAM>
    STREAM& streamInRaw(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.
        //
        // DEPRECATED: replaced by 'bdexStreamIn(stream, version)'

    // ACCESSORS
    int isValid() const;
        // Return non-zero if this finite automaton is valid, and 0 otherwise.

    void print() const;
        // Write this finite automaton to 'stdout'.

    std::ostream& streamOut(std::ostream& stream) const;
        // TBD doc

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

    template <class STREAM>
    STREAM& streamOutRaw(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.
        //
        // DEPRECATED: replaced by 'bdexStreamOut(stream, version)'
};

// FREE OPERATORS
inline
std::ostream& operator<<(std::ostream&                stream,
                         const bdecs_FiniteAutomaton& finiteAutomaton);
    // TBD doc

inline
bdecs_FiniteAutomaton operator+(const bdecs_FiniteAutomaton& lhs,
                                const bdecs_FiniteAutomaton& rhs);
    // Return a finite automaton that accepts inputs that for some index of the
    // input data, the elements of the input up to the index are accepted by
    // the specified 'lhs' finite automaton and the residual elements of the
    // input are accepted by the specified 'rhs' finite automaton.

inline
bdecs_FiniteAutomaton operator|(const bdecs_FiniteAutomaton& lhs,
                                const bdecs_FiniteAutomaton& rhs);
    // Return a finite automaton that accepts inputs that are accepted by one
    // or both of the specified 'lhs' finite automaton and the specified 'rhs'
    // finite automaton.

                // ======================================
                // class bdecs_FiniteAutomatonAccumulator
                // ======================================

class bdecs_FiniteAutomatonAccumulator {
    // This class implements an accumulator for 'bdecs_FiniteAutomaton'.
    // An accumulator is given an input (as specified by a sequence of
    // 'transition()' method calls) and reports (via the 'isAccepting()'
    // and 'isError()' methods) the state of the associated
    // 'bdecs_FiniteAutomaton'.

    int d_state;
    const bdea_PtrArray<bdecs_FiniteAutomaton_State>& d_statePtrs;

  public:
    bdecs_FiniteAutomatonAccumulator(const bdecs_FiniteAutomaton& fa)
      : d_state(fa.d_startState),
        d_statePtrs(fa.d_statePtrs) { }
    ~bdecs_FiniteAutomatonAccumulator() { }

    void transition(char data);
        // Transition from the current state of the associated finite automaton
        // to the one indicated by the associated transition table for the
        // current state and the specified 'data'.

    int isAccepting();
        // Return 1 if the current state is an accepting state, and 0
        // otherwise.

    int isError();
        // Return 1 if the current state is the error state, and 0 otherwise.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                                // -------------
                                // CLASS METHODS
                                // -------------

inline
int bdecs_FiniteAutomaton::maxSupportedBdexVersion()
{
    return 1;
}

inline
int bdecs_FiniteAutomaton::maxSupportedVersion()
{
    return maxSupportedBdexVersion();
}

                        // ------------
                        // MANIPULATORS
                        // ------------

inline
void bdecs_FiniteAutomaton::invalidate()
{
    if (d_startState != 0) {
        removeAll();
        d_startState = 0;
    }
}

// TBD is this finite automaton always left in a valid state?
template <class STREAM>
inline
STREAM& bdecs_FiniteAutomaton::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            int nStates;
            stream.getInt32(nStates);
            if (!stream) {
                return stream;                                   // RETURN
            }

            int start;
            stream.getInt32(start);
            if (!stream) {
                return stream;                                   // RETURN
            }

            removeAll();
            d_pool.reserveCapacity(nStates);
            d_statePtrs.reserveCapacity(nStates);

            d_startState = start;
            for (int i = 0; i < nStates; ++i) {
                d_statePtrs.append(new (d_pool.allocate())
                                                bdecs_FiniteAutomaton_State());
                stream.getInt8(d_statePtrs[i]->d_status);
                if (!stream) {
                    return stream;                               // RETURN
                }
                stream.getArrayInt32(d_statePtrs[i]->d_transitionTable,
                                     bdecs_FiniteAutomaton_State::SIZE);
                if (!stream) {
                    return stream;                               // RETURN
                }
            }
          } break;
          default: {
            stream.invalidate();
          } break;
        }
    }

    return stream;
}

template <class STREAM>
inline
STREAM& bdecs_FiniteAutomaton::streamInRaw(STREAM& stream, int version)
{
    return bdexStreamIn(stream, version);
}

                                // ---------
                                // ACCESSORS
                                // ---------

inline
int bdecs_FiniteAutomaton::isValid() const
{
    return d_startState != 0;
}

inline
std::ostream& bdecs_FiniteAutomaton::streamOut(std::ostream& stream) const
{
    printImpl(stream);
    return stream;
}

template <class STREAM>
inline
STREAM& bdecs_FiniteAutomaton::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        const int nStates = d_statePtrs.length();
        stream.putInt32(nStates);
        stream.putInt32(d_startState);
        for (int i = 0; i < nStates; ++i) {
            stream.putInt8(d_statePtrs[i]->d_status);
            stream.putArrayInt32(d_statePtrs[i]->d_transitionTable,
                                 bdecs_FiniteAutomaton_State::SIZE);
        }
      } break;
    }
    return stream;
}

template <class STREAM>
inline
STREAM& bdecs_FiniteAutomaton::streamOutRaw(STREAM& stream, int version) const
{
    return bdexStreamOut(stream, version);
}

inline
void bdecs_FiniteAutomatonAccumulator::transition(char data)
{
    d_state = d_statePtrs[d_state]->d_transitionTable[(unsigned char)data];
}

inline
int bdecs_FiniteAutomatonAccumulator::isAccepting()
{
    return d_statePtrs[d_state]->d_status ==
                                        bdecs_FiniteAutomaton_State::ACCEPTING;
}

inline
int bdecs_FiniteAutomatonAccumulator::isError()
{
    return 0 == d_state;
}

                                // --------------
                                // FREE OPERATORS
                                // --------------

inline
std::ostream& operator<<(std::ostream&                stream,
                         const bdecs_FiniteAutomaton& finiteAutomaton)
{
    return finiteAutomaton.streamOut(stream);
}

inline
bdecs_FiniteAutomaton operator+(const bdecs_FiniteAutomaton& lhs,
                                const bdecs_FiniteAutomaton& rhs)
{
    bdecs_FiniteAutomaton tmp(lhs);
    tmp += rhs;
    return tmp;
}

inline
bdecs_FiniteAutomaton operator|(const bdecs_FiniteAutomaton& lhs,
                                const bdecs_FiniteAutomaton& rhs)
{
    bdecs_FiniteAutomaton tmp(lhs);
    tmp |= rhs;
    return tmp;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
