// bdec_queue.cpp            -*-C++-*-
#include <bdec_queue.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdec_queue_cpp,"$Id$ $CSID$")

//
///IMPLEMENTATION NOTES
///--------------------
// This component implements a double-ended queue.  The queue is stored in
// memory as an array with 'd_front' indicating the position before the first
// element and 'd_back' the position after the last element; this implies two
// extra array positions are required beyond what is strictly necessary.  This
// technique was chosen since it avoids all ambiguities involving empty and
// full queues, and simplifies efficient implementation via symmetry.
//
// Since a queue stored in this manner is circular, there are two general
// scenarios to consider during every operation (the numbers in the cells are
// index positions):
//..
//                         +---+---+---+---+---+---+---+---+---+---+
//    d_front < d_back     |   |   | 0 | 1 | 2 | 3 |   |   |   |   |
//                         +---+---+---+---+---+---+---+---+---+---+
//                               ^                   ^
//                               |___ d_front        |___ d_back
//
//                         +---+---+---+---+---+---+---+---+---+---+
//    d_front > d_back     | 3 |   |   |   |   |   |   | 0 | 1 | 2 |
//                         +---+---+---+---+---+---+---+---+---+---+
//                               ^                   ^
//                               |___ d_back         |___ d_front
//..

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
