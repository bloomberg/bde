// bslstp_hashmap.t.cpp                                               -*-C++-*-
#include <bslstp_hashmap.h>

template class bsl::hash_map<short, char>;

int main() {
    // Smoke test
    bsl::hash_map<int, int> x;
    x[5] = 6;
    x.erase(x.begin());

    return -1; // Empty test driver
}
