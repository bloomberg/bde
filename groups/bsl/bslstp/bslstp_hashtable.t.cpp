// bslstp_hashtable.t.cpp                                             -*-C++-*-
#include <bslstp_hashtable.h>

#include <bslma_bslallocator.h>

template <class T>
struct DumbHasher {
    unsigned operator()(const T&) const { return 1; }
};

template <class T>
struct SimpleEq {
    bool operator()(const T& a, const T& b) const { return a == b; }
};

template <class T>
struct Identity {
    const T& operator()(const T& x) const { return x; }
};

using namespace BloombergLP;

template class bsl::hashtable<int, int, DumbHasher<int>, Identity<int>,
                              SimpleEq<int>, bsl::allocator<int> >;

int main() {
    typedef bsl::hashtable<int, int, DumbHasher<int>, Identity<int>,
                           SimpleEq<int>, bsl::allocator<int> > Ht;

    // Smoke test
    Ht x(10, DumbHasher<int>(), SimpleEq<int>(), Identity<int>());
    x.insert_unique(Ht::value_type(0));
    x.erase(x.begin());

    return -1;
} // Empty test driver
