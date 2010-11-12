// bdecs_finiteautomatonutil.h         -*-C++-*-
#ifndef INCLUDED_BDECS_FINITEAUTOMATONUTIL
#define INCLUDED_BDECS_FINITEAUTOMATONUTIL

//@PURPOSE: Provide utility functions for use with finite automata.
//
//@CLASSES:
//   bdecs_FiniteAutomatonUtil: namespace for utility functions
//
//@AUTHOR: Jeffrey Mendelsohn (jmendels)
//
//@SEE_ALSO: bdecs_finiteautomaton, bdecs_FiniteAutomatonAccumulator
//
//@DESCRIPTION: This component provides a namespace for a suite of pure
// procedures for use with finite automata.
//
// The 'repeat' and 'repeatMin' functions provided in this component
// implement concatenation operations on finite automata.  Viewed another
// way, since finite automata accept sets of strings, these methods
// effectively perform set concatenation operations on the underlying sets
// of strings represented by finite automata.  Refer to the description of
// the individual methods for details.
//
///USAGE
///-----
// The following snippets of code illustrate how to use one of the 'repeat'
// methods of this utility component.  First create a 'bdecs_FiniteAutomaton'
// 'fa' which accepts the strings "x", "y", and "z".
//..
//      bdecs_FiniteAutomaton fa;
//      fa.appendSet("xyz", 3);
//..
// Next apply the 3-argument 'repeat' method to 'fa'.
//..
//      bdecs_FiniteAutomatonUtil::repeat(&fa, 5, 10);
//..
// 'fa' now accepts those strings consisting of x's, y's and z's which have
// at least 5 characters but not more than 10 characters.  Test some strings
// that are too short to be accepted by 'fa.
//..
//      bdecs_FiniteAutomatonAccumulator faa(fa);
//      int i = 0;
//
//      do {
//          faa.transition((i % 3) + 'x');    // 'x', 'y' or 'z'
//          assert(0 == faa.isAccepting());
//      } while (++i < 4);
//..
// Now test some strings that are accepted by 'fa'.
//..
//      do {
//          faa.transition((i % 3) + 'x');
//          assert(1 == faa.isAccepting());
//      } while (++i < 10);
//..
// Finally test some strings that are too long.
//..
//      do {
//          faa.transition((i % 3) + 'x');
//          assert(0 == faa.isAccepting());
//      } while (++i < 25);
//..



#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif



namespace BloombergLP {

class bdecs_FiniteAutomaton;

                   // ================================
                   // struct bdecs_FiniteAutomatonUtil
                   // ================================

struct bdecs_FiniteAutomatonUtil {
    static void repeat(bdecs_FiniteAutomaton *fa, int numMatch);
        // Assign to the specified finite automaton 'fa' a finite automaton
        // that accepts an input comprised of the specified 'numMatch' inputs
        // that are accepted by the original finite automaton 'fa'. The
        // behavior is undefined unless 0 <= numMatch.
        //
        // Let X be the set of strings accepted by the specified finite
        // automaton 'fa' and x(j) an arbitrary string in X.
        // After executing:
        //..
        //   bdecs_FiniteAutomatonUtil::repeat(&fa, n);
        //..
        // 'fa' accepts strings of the form:
        //   x(1)x(2)...x(n)
        //
        // Note that if n=0, then the resulting 'fa' will only accept the
        // null string.

    static void repeat(bdecs_FiniteAutomaton *fa,
                       int                    minNumMatch,
                       int                    maxNumMatch);
        // Assign to the specified finite automaton 'fa' a finite automaton
        // that accepts an input comprised of at least the specified
        // 'minNumMatch' but not more than the specified 'maxNumMatch' inputs
        // that are accepted by the original finite automaton 'fa'.  The
        // behavior is undefined unless 0 <= minNumMatch, 0 <= maxNumMatch, and
        // minNumMatch <= maxNumMatch.
        //
        // Let X be the set of strings accepted by the specified finite
        // automaton 'fa' and x(j) an arbitrary string in X.
        // After executing:
        //..
        //   bdecs_FiniteAutomatonUtil::repeat(&fa, m, n);
        //..
        // 'fa' accepts strings of the form:
        //   x(1)x(2)...x(m)
        //   x(1)x(2)...x(m+1)
        //   ...
        //   x(1)x(2)...x(n)
        //
        // Note that if m=0, then the resulting 'fa' will also accept the
        // null string.  Note that if n=0 (implying m=0), then the resulting
        // 'fa' will only accept the null string.

    static void repeatMin(bdecs_FiniteAutomaton *fa, int numMatch);
        // Assign to the specified finite automaton 'fa' a finite automaton
        // that accepts an input comprised of at least the specified 'numMatch'
        // inputs that are accepted by the original finite automaton 'fa'.
        // The behavior is undefined unless 0 <= numMatch.
        //
        // Let X be the set of strings accepted by the specified finite
        // automaton 'fa' and x(j) an arbitrary string in X.
        // After executing:
        //..
        //   bdecs_FiniteAutomatonUtil::repeatMin(&fa, n);
        //..
        // 'fa' accepts strings of the form:
        //   x(1)x(2)...x(n)
        //   x(1)x(2)...x(n+1)
        //   x(1)x(2)...x(n+2)
        //   ...
        //
        // Note that bdecs_FiniteAutomatonUtil::repeatMin(&fa, 0) is logically
        // equivalent to fa.assignKleeneStart().
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
