// bslstl_hashtable.cpp                                               -*-C++-*-
#include <bslstl_hashtable.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_nativestd.h>

#include <algorithm>
#include <cstddef>

namespace BloombergLP
{
namespace bslstl
{

                    // ----------------------------
                    // class HashTable_StaticBucket
                    // ----------------------------

bslalg::HashTableBucket *HashTable_StaticBucket::getDefaultBucketAddress()
{
    static bslalg::HashTableBucket s_bucket = {0}; // Aggregative initialization
                                                   // of a POD should be thread-
                                                   // safe static initialization
    return &s_bucket;
}


                    // -------------------------
                    // class HashTable_PrimeUtil
                    // -------------------------

native_std::size_t HashTable_PrimeUtil::nextPrime(native_std::size_t n)
{   // An abbreviated list of prime numbers in the domain of 32-bit
    // unsigned integers.  Essentially, a subset where each successive
    // element is the next prime after doubling.
    // Note that at least one of these numbers was mis-computed and undershoots,
    // messing up the doubling pattern, not critical while the code remains
    // proof-of-concept code.
    static const native_std::size_t s_primes[] = { 2, 5, 13, 29, 61,
        127, 257, 521, 1049, 2099, 4201, 8419, 16843, 33703, 67409, 134837,
        269513, 539039, 1078081, 2156171, 5312353, 10624709, 21249443,
        42498893, 84997793, 169995589, 339991181, 679982363, 1359964751,
        2719929503u

    };
    static const native_std::size_t s_nPrimes = 
                                         sizeof(s_primes)/sizeof(&s_primes[0]);
    static const native_std::size_t *const s_beginPrimes = s_primes;
    static const native_std::size_t *const s_endPrimes = s_primes + s_nPrimes;

    return *native_std::lower_bound(s_beginPrimes, s_endPrimes, n);
}
    
}  // close package namespace
}  // close enterprise namespace
// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
