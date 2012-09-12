#ifndef BDE_OSS_TEST

#include <bslstp_alloc.h>
#include <bslstl_allocator.h>

namespace bsl {

// Force instantiation.
template class allocator<char>;
template class _STLP_alloc_proxy<char *,char, allocator<char> >;

}  // close namespace bsl

#endif // #ifndef BDE_OSS_TEST
