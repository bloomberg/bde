// bdlcc_fixedqueue.cpp                                                -*-C++-*-

#include <bdlcc_fixedqueue.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlcc_fixedqueue_cpp,"$Id$ $CSID$")

namespace BloombergLP {

///Implementation Note
///===================
// This component is implemented using a ring buffer data structure.  A ring
// buffer is a fixed size buffer that logically wraps around itself. It is the
// ideal container for a fixed sized queue, since this structure imposes a
// strict upper bound on it's internal capacity.
//
// Here is an illustration representing a ring buffer that can hold at most
// twenty items at any instant.
//..
// +---------------------------------------------------------------------+
// | 0| 1| 2| 3| 4| 5| 6| 7| 8| 9| 10| 11| 12| 13| 14| 15| 16| 17| 18| 19|
// +---------------------------------------------------------------------+
//                                 |
//                                 |
//                                 V
//                            +-------+
//                        +---|  9| 10|---+
//                    +---|  8|---+---| 11|---+
//                    |  7|---+       +---| 12|
//                   +----                +------+
//                   |  6|                   | 13|
//                  +----+                   +----+
//                  |  5|                     | 14|
//                  +---+                     +---+
//                  |  4|                     | 15|
//                  +----+                    ----+
//                   |  3|                   | 16|
//                   +-----               +------+
//                    |  2|---+       +---| 17|
//                    +---|  1|---+---| 18|---+
//                        +---|  0| 19|---+
//                            +-------+
//..
// This component defines an implementation of a ring buffer that allows
// concurrent access from multiple reader and writer threads.  The component
// was designed to minimize contention between threads.
//
// Conceptually, this ring buffer could be thought of as two concentric ring
// buffers.  Cells of the outer ring hold an atomic integer which facilitates
// a state machine (sn) whose purpose is to protect access to a value (vn)
// contained at the homogeneous inner cell:
//..
//
//                            +-------+
//                        +---| s9|s10|---+
//                     ---| s8+-------+s11|---
//                  ------|---| v9|v10|---|-------
//                +-------| v8|--- ---|v11|-------+
//                | s7| v7|---+       +---|v12|s12|
//              +---------+               +---------+
//              | s6| v6|                   |v13|s13|
//             +--------+                   +--------+
//             | s5| s5|                     |v14|s14|
//             +-------+                     +-------+
//             | s4| v4|                     |v15|s15|
//             +--------+                    +-------+
//              | s3| v3|                   |v16|s16|
//              +---------                +---------+
//                | s2| v2|---        +---|v17|s17|
//                +--- ---| v1|---+---|v18|-------+
//                  ------|---| v0|v19|---|------
//                     ---| s1|-------|s18|---
//                        +---| s0|s19|---+
//                            +-------+
//..
// The outer ring implemented in the class 'bdlcc::FixedQueueIndexManager'.  The
// inner ring is implemented in the class 'bdlcc::FixedQueue'.  Each
// 'bdlcc::FixedQueue' object holds a 'bdlcc::AtomicRinBufferIndexManager' that it
// uses to manage the state of the elements in 'd_elements'.

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
