#undef BSL_OVERRIDES_STD
#include <bsl_iterator.h>
#include <iterator>
#ifdef std
#   error std was not expected to be a macro
#endif

// Verify that iterator adapters get included by bsl_iterator.h

template <typename T, typename ITER_IMP>
class CheckForwardIteratorAdapter
    : BloombergLP::bslstl::ForwardIterator<T, ITER_IMP>
{};

template <typename T, typename ITER_IMP>
class CheckBidirectionalIteratorAdapter
    : BloombergLP::bslstl::BidirectionalIterator<T, ITER_IMP>
{};

template <typename T, typename ITER_IMP>
class CheckRandomAccessIteratorAdapter
    : BloombergLP::bslstl::RandomAccessIterator<T, ITER_IMP>
{};

namespace std { }
int main() { return 0; }

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
