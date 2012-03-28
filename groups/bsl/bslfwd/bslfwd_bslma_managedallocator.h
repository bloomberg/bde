#ifndef INCLUDED_BSLFWD_BSLMA_MANAGEDALLOCATOR
#define INCLUDED_BSLFWD_BSLMA_MANAGEDALLOCATOR

#ifndef BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

#ifdef BDE_BUILD_TARGET_BSL_NAMESPACES

namespace BloombergLP {
    namespace bslma {
        class ManagedAllocator;
    }

    typedef bslma::ManagedAllocator bslma_ManagedAllocator;
}

#else

namespace BloombergLP {
    class bslma_ManagedAllocator;
}

#endif

#endif
