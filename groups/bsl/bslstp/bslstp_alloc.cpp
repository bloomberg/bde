#include <bslstp_alloc.h>
#include <bslstl_allocator.h>

#if !defined(BSL_PUBLISHED) || 1 == BSL_PUBLISHED

namespace bsl {

// Force instantiation.
template class allocator<char>;
template class _STLP_alloc_proxy<char *,char, allocator<char> >;

}  // close namespace bsl

#endif // #if !defined(BSL_PUBLISHED) || 1 == BSL_PUBLISHED
