// bslstp_hashset.t.cpp                                               -*-C++-*-

#include <bslstp_hashset.h>

template class bsl::hash_set<long>;

/// Smoke test
int main() {
    bsl::hash_set<int> x;
    x.insert(1);
    x.erase(x.begin());

    return -1; // Empty test driver
}
