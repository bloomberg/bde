// bslstl_hashtable.cpp                                               -*-C++-*-
#include <bslstl_hashtable.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslstl_equalto.h>  // for testing only
#include <bslstl_hash.h>     // for testing only

#include <bsls_nativestd.h>

#include <algorithm>         // 'lower_bound'

namespace BloombergLP
{
namespace bslstl
{

                    // --------------------------
                    // class HashTable_ImpDetails
                    // --------------------------

bslalg::HashTableBucket *HashTable_ImpDetails::defaultBucketAddress()
{
    static bslalg::HashTableBucket s_bucket = {0 , 0};
                                                  // Aggregative initialization
                                                  // of a POD should be thread-
                                                  // safe static initializationi
    // These two tests should not be necessary, but will catch corruption in
    // components that try to write to the shared bucket.
    BSLS_ASSERT_SAFE(!s_bucket.first());
    BSLS_ASSERT_SAFE(!s_bucket.last());

    return &s_bucket;
}

native_std::size_t HashTable_ImpDetails::nextPrime(native_std::size_t n)
{   // An abbreviated list of prime numbers in the domain of 32-bit
    // unsigned integers.  Essentially, a subset where each successive
    // element is the next prime after doubling.
    // Note that at least one of these numbers was mis-computed and
    // undershoots, messing up the doubling pattern, not critical while the
    // code remains proof-of-concept code.

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

    const native_std::size_t *result = native_std::lower_bound(s_beginPrimes,
                                                               s_endPrimes,
                                                               n);
    if (s_endPrimes == result) {
        StdExceptUtil::throwLengthError("HashTable ran out of prime numbers.");
    }

    return *result;
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
