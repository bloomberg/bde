#ifndef INCLUDED_BSLFWD_BSLMA_DEFAULTALLOCATORGUARD
#define INCLUDED_BSLFWD_BSLMA_DEFAULTALLOCATORGUARD

#ifndef BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

#ifdef BDE_BUILD_TARGET_BSL_NAMESPACES

namespace BloombergLP {
    namespace bslma {
        class DefaultAllocatorGuard;
    }

    typedef bslma::DefaultAllocatorGuard bslma_DefaultAllocatorGuard;
}

#else

namespace BloombergLP {
    class bslma_DefaultAllocatorGuard;
}

#endif

#endif
