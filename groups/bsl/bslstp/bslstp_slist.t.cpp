// bslstp_slist.t.cpp                                                 -*-C++-*-
#include <bslstp_slist.h>

template class bsl::slist<int>;

int main() {
    // Smoke test
    bsl::slist<int> x;
    x.push_front(3);
    x.push_front(4);
    x.erase(x.begin());
    x.pop_front();

    return -1; // Empty test driver
}
