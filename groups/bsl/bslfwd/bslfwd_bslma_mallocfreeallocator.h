#ifndef INCLUDED_BSLFWD_BSLMA_MALLOCFREEALLOCATOR
#define INCLUDED_BSLFWD_BSLMA_MALLOCFREEALLOCATOR

#ifndef BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

#ifdef BDE_BUILD_TARGET_BSL_NAMESPACES

namespace BloombergLP {
    namespace bslma {
        class MallocFreeAllocator;
    }

    typedef bslma::MallocFreeAllocator bslma_MallocFreeAllocator;
}

#else

namespace BloombergLP {
    class bslma_MallocFreeAllocator;
}

#endif

#endif
