// bslstp_slist.t.cpp                                                 -*-C++-*-
#include <bsls_platform.h>

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif

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
