// bcec_fixedqueue.cpp     -*-C++-*-
#include <bcec_fixedqueue.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcec_fixedqueue_cpp,"$Id$ $CSID$")

#include <bcemt_thread.h>

#include <bdes_bitutil.h>

#include <bsls_assert.h>

#include <bsl_algorithm.h>
#include <bsl_limits.h>

///IMPLEMENTATION NOTES
///--------------------
// The 'bcec_FixedQueue' algorithm is mostly lock-free, except for operations
// that grow the underlying 'bcema_FixedPool' of objects.  The
// 'bcec_FixedQueue' class itself, wraps an underlying queue of integers
// maintained in a 'bcec_FixedQueue_IndexQueue'.  The 'bcec_FixedQueue' stores
// the value of queue'd objects in an internal 'bcema_FixedPool', which
// provides an index for the stored value.  That index is then enqueue'd onto
// the 'bcec_FixedQueue_IndexQueue'.  So, for example, when a client calls
// 'popFront' on a 'bcec_FixedQueue', the queue tries to pop the next index
// from the front of its 'bcec_FixedQueue_IndexQueue' (and will wait, if no
// index is available), and then returns the value at that index in the
// 'bcema_FixedPool'.
//
///'bcec_FixedQueue_IndexQueue'
///- - - - - - - - - - - - - - -
// The 'bcec_FixedQueue_IndexQueue' implements a highly-optimized queue of
// integers (whose values happen to to be indices into a 'bcema_FixedPool').
// Each 'bcec_FixedQueue' has 3 primary data members: 'd_front', 'd_back', and
// 'd_data'.  'd_front' and 'd_back' store the indices, into 'd_data', of the
// current head and tail of the queue.  'd_data' is an array holding the
// queue'd integer values.  Some of the bits of the integers 'd_back',
// 'd_front', and of each integer value in the queue, are reserved for
// managing the queue itself (hence the size of a 'bcec_FixedQueue' is limited
// to '0x01FFFFFF').  The additional constant data members of 'IndexQueue'
// are used to mask and manipulate those management bits.  We will describe an
// example 'IndexQueue' created with a range of 512, and a capacity of 256, on
// a 32-bit platform.  The bits of 'd_back' and 'd_front' are both tied to the
// capacity of the 'IndexQueue' (256) and would be used as follows:
//..
//  |31 30 .  .  . 10 9| 8 7 . . . . . 2 1 |
//  +------------------+-------------------+
//  | Generation Count | Index into d_data |   (8-bits gives a 256 capacity)
//  +--------------------------------------+
//..
// The data members 'd_indexBits' and 'd_numIndexBits' are used to mask and
// manipulate 'd_back' and 'd_front' and would have these values:
//..
//  d_indexBits    = 0x00FF;  (mask for the last 8 bits, i.e., capacity)
//  d_numIndexBits = 8;       (number of 1 bits in 'd_indexBits')
//..
// The bits for each element in the vector 'd_data' are used as follows:
//..
//  |31 30 .  .   .  11|10| 10 9 . . . 2 1 |
//  +------------------+-------------------+ (9-bits gives a range of 512, +1
//  | Generation Count | 0| Queue Data     |  bit for a special 'empty' value)
//  +------------------+--+----------------+
//                      ^^--- indicates whether the queue is disabled
//..
// The data members 'd_dataBits', 'd_sentinel', and 'd_genCountOffset' are
// used to mask and manipulate the elements of 'd_data' and would have these
// values:
//..
//  d_dataBits       = 0x03FF;  (mask for the last 10 bits)
//  d_sentinel       = 0x0400;  (mask for the sentinel bit, indicating the
//                               queue is disabled)
//  d_genCountOffset = 11;      (number of 1 bits in 'd_dataBits' + 1 for
//                               the sentinel indicating the queue is disabled
//..
// Note that in the context of 'FixedQueue', the IndexQueue's data elements
// are indices into a 'bcema_FixedPool', and that the range of those indices
// will be the capacity of the 'FixedQueue', and, by extension, the capacity
// needed in the 'IndexQueue' -- so the range and capacity configuration
// values for the 'IndexQueue' will be the same.
//
// Each element (or "cell") in 'd_data' can be either "empty", or contain a
// data value.  Empty elements in 'd_data' are indicated with the special data
// value of 0, and, as a result, non-empty elements store a value 1 higher than
// they actual value they represent.  In 'tryPushBack', the 'IndexQueue'
// masks 'd_back' to obtain the current array index of the array element one
// past the end of the queue as well as the current generation count of
// 'd_back'.  It then attempts to (atomically) set the indicated cell to: the
// value being pushed (+1, because 0 is the special empty-value) combined
// with the back-generation count.  Before attempting the set, the
// queue first tests the generation count currently stored in the "back"
// element (without using a memory barrier) as a performance optimization.  If
// the test-and-set of the "back" element is successful, 'd_back' is then
// incremented by 1, which, at the end of the array (e.g., index 0x00FF in a
// 256 capacity queue) will reset the index-bits to 0, and increment the
// generation count.  Corresponding logic is performed for 'tryPopFront': the
// 'IndexQueue' copies the value held in the array element indexed by 'd_front'
// to a temporary, and then attempts to set that element to the empty-value (0)
// combined with the generation count of 'd_front'.  If this (atomic)
// test-and-set is successful, 'tryPopFront' increments 'd_front', which, if
// the end of the array has been reached, will effectively wrap to the front
// of the array, and increment the generation count.  
//
// The generation counts for 'd_front' and 'd_back' are independent, and are
// used to avoid ABA problems when the back and front of the queue are near
// each other (i.e., when the queue is either empty or full).  Note that the
// likelihood of such a problem is high because all empty cells have the same
// value, 0, and data values are indices into a 'bcema_FixedQueue' which are
// quickly recycled when released.

namespace BloombergLP {

namespace {

enum {
    DEFAULT_BACKOFF_LEVEL = 2  // determined by experiment
};

void backoff(int *contentionCount, int backoffLevel)
{
    if (0 == backoffLevel) {
        // nothing to do

        return;
    }

    enum { MAX_SPIN_LEVEL = 10 };

    int count = ++(*contentionCount);
    if (count > MAX_SPIN_LEVEL) {
        *contentionCount = 0;
        bcemt_ThreadUtil::yield();  // exhaust time slice
    }
    else {
        int maxSpin = backoffLevel << count;
        for (volatile int spin = 0; spin < maxSpin; ++spin) {
            // spin
        }
    }
}

}  // close unnamed namespace

                       // --------------------------------
                       // class bcec_FixedQueue_IndexQueue
                       // --------------------------------

// CREATORS
bcec_FixedQueue_IndexQueue::bcec_FixedQueue_IndexQueue(
        int              capacity,
        int              range,
        bslma_Allocator *basicAllocator)
: d_indexBits(bdes_BitUtil::roundUpToBinaryPower(capacity) - 1)
, d_numIndexBits(bdes_BitUtil::numSetOne(d_indexBits))
, d_dataBits(
    bsl::max(bdes_BitUtil::roundUpToBinaryPower(range + 1) - 1,
             d_indexBits >> 1)) // TBD: needs comment and tests
, d_genCountOffset(bdes_BitUtil::numSetOne(d_dataBits) + 1) // +1 for sentinel
, d_sentinel(d_dataBits + 1)
, d_backoffLevel(DEFAULT_BACKOFF_LEVEL)
, d_data(basicAllocator)
, d_dataPad()
, d_back(0)
, d_indexPad()
, d_front(0)
{
    BSLS_ASSERT_OPT(capacity > 0);
    BSLS_ASSERT_OPT(range > 0);

    // Fill the queue with the initial empty value.
    d_data.resize(d_indexBits + 1, 0);
}

bcec_FixedQueue_IndexQueue::~bcec_FixedQueue_IndexQueue()
{
}

// MANIPULATORS
int bcec_FixedQueue_IndexQueue::tryPushBack(int data)
{
    BSLS_ASSERT(data >= 0);
    BSLS_ASSERT((unsigned)data < d_dataBits);

    int contentionCount = 0;

    enum { INCREMENT_DELAY = 5 };
    int delay = INCREMENT_DELAY;

    for (;;) {
        const unsigned back = d_back.relaxedLoad();

        const int index = back & d_indexBits;    // back % size;
        const unsigned cur = d_data[index].relaxedLoad();
        const unsigned gen = cur & ~(d_dataBits | d_sentinel);

        const unsigned backGen = (back >> d_numIndexBits) << d_genCountOffset;

        if (gen == backGen) {
            if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(cur & d_sentinel)) {
                // The cell contains the sentinel.  The queue is disabled.

                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
                return -2;
            }

            if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(cur & d_dataBits)) {
                // The queue is full.

                BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
                return -1;
            }

            const unsigned nextGen = gen + (d_sentinel << 1);

            // Note that a data value of 0 in 'd_data' is a special value
            // indicating an empty element, so all integers held in d_data are
            // incremented by 1.
            const unsigned newCell = ((unsigned)data + 1) | nextGen;

            // This test-and-swap has full barrier semantics, but we really
            // need only 'release' semantics.

            if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                  cur == (unsigned) d_data[index].testAndSwap(cur, newCell))) {
                // We put in the new value.  Now we'll increment 'back'.  The
                // order guarantees that no cpu can observe the new 'back'
                // before they observe the 'd_data[index]' update.

                // If 'd_back' is at the end of the queue (e.g., 0x00FF in 256
                // capacity queue)the following increment will 0 the index bits
                // in 'd_back' and increment the generation count.
                d_back.testAndSwap(back, back + 1);


                break;
            }

            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            backoff(&contentionCount, d_backoffLevel);
        }
        else {
            // Another pusher got ahead of us.

            if (!--delay) {
                // We're incrementing the back counter so if the other pusher
                // is scheduled out before incrementing it, we can still move.
                // This is safe enough because back wraps around only if we
                // reach 'UINT_MAX'.

                d_back.testAndSwap(back, back + 1);
                delay = INCREMENT_DELAY;
            }
        }
    }

    return 0;
}

int bcec_FixedQueue_IndexQueue::tryPopFront(int *data)
{
    BSLS_ASSERT(data);

    int contentionCount = 0;

    enum { INCREMENT_DELAY = 5 };
    int delay = INCREMENT_DELAY;

    for (;;) {
        const unsigned front = d_front.relaxedLoad();

        const int index = front & d_indexBits;    // front % size;
        const unsigned cur = d_data[index].relaxedLoad();
        const unsigned gen = cur & ~(d_dataBits | d_sentinel);

        const unsigned frontGen = (front >> d_numIndexBits)
                                                           << d_genCountOffset;
        const unsigned genCountDifference = gen - frontGen;

        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == genCountDifference)) {
            // The queue is empty.

            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            BSLS_ASSERT((cur & d_dataBits) == 0);
            return -1;
        }

        const unsigned val = cur & d_dataBits;
        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                    genCountDifference == (d_sentinel << 1))
         && BSLS_PERFORMANCEHINT_PREDICT_LIKELY(val != 0)) {
            // The cell contains data.

            const unsigned newCell = cur ^ val; // Clear data bits.

            // This test-and-swap has full barrier semantics, but we
            // really only need 'acquire' semantics

            if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                   cur == (unsigned)d_data[index].testAndSwap(cur, newCell))) {
                // We put in the empty marker.  Now we'll increment 'front'.
                // The order guarantees that no cpu can observe the new 'back'
                // before they observe the 'd_data[index]' update.

                // If 'd_front' is at the end of the queue (e.g., 0x00FF in 256
                // capacity queue)the following increment will 0 the index bits
                // in 'd_front' and increment the generation count.
                d_front.testAndSwap(front, front + 1);
                *data = val - 1;
                break;
            }

            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            backoff(&contentionCount, d_backoffLevel);
        }
        else {
            // Another popper got ahead of us.

            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            if (!--delay) {
                d_front.testAndSwap(front, front + 1);
                delay = INCREMENT_DELAY;
            }
        }
    }

    return 0;
}

void bcec_FixedQueue_IndexQueue::disable()
{
    enum { INCREMENT_DELAY = 5 };
    int delay = INCREMENT_DELAY;

    for (;;) {
        const unsigned back = d_back.relaxedLoad();

        const int index = back & d_indexBits;    // back % size;
        const unsigned cur = d_data[index].relaxedLoad();
        const unsigned gen = cur & ~(d_dataBits | d_sentinel);

        const unsigned backGen = (back >> d_numIndexBits) << d_genCountOffset;

        if (gen == backGen) {

            if (cur & d_sentinel) {
                // The cell contains the sentinel.  The queue is disabled.

                break;
            }
            else {
                const unsigned newCell = cur | d_sentinel;

                if (cur == (unsigned)d_data[index].testAndSwap(cur, newCell)) {
                    // We put in the new value.
                    // We do not need to increment d_back.

                    break;
                }
            }
        }
        else {
            // Another pusher got ahead of us.

            if (!--delay) {
                // We're incrementing the back counter so if the other pusher
                // is scheduled out before incrementing it, we can still move.
                // This is safe enough because back wraps around only if we
                // reach 'UINT_MAX'.

                d_back.testAndSwap(back, back + 1);
                delay = INCREMENT_DELAY;
            }
        }
    }
}

void bcec_FixedQueue_IndexQueue::enable()
{
    // remove the sentinel we put at 'back'.
    const unsigned back = d_back;

    const int index = back & d_indexBits;    // back % size;

    unsigned cur = d_data[index];
    while (cur & d_sentinel) {
        const unsigned newCell = cur & ~d_sentinel;

        // If the queue is full, a popping thread may be removing this data
        // value (i.e., 'front' is at this position too).  So we must check
        // the result of testAndSwap.

        if (cur == (unsigned) d_data[index].testAndSwap(cur, newCell)) {
            break;
        }
        cur = d_data[index];
    }
}

bool bcec_FixedQueue_IndexQueue::isEnabled() const
{
    const unsigned back = d_back;

    const int index = back & d_indexBits;    // back % size;
    const unsigned cur = d_data[index];

    return !(cur & d_sentinel);
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
