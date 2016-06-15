// bslstp_alloc.cpp                                                   -*-C++-*-
#ifndef BDE_OPENSOURCE_PUBLICATION // STP

#include <bslstp_alloc.h>
#include <bslma_stdallocator.h>
#include <bslma_stdallocator.h>

namespace bsl {

// Force instantiation.
template class allocator<char>;
template class _STLP_alloc_proxy<char *,char, allocator<char> >;

}  // close namespace bsl

#endif  // BDE_OPENSOURCE_PUBLICATION -- STP
