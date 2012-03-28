#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#define INCLUDED_BSLFWD_BSLMA_ALLOCATOR

#ifndef BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

#ifdef BDE_BUILD_TARGET_BSL_NAMESPACES

namespace BloombergLP {
    namespace bslma {
        class Allocator;
    }

    typedef bslma::Allocator bslma_Allocator;
}

#else

namespace BloombergLP {
    class bslma_Allocator;
}

#endif

#define bdema_Allocator bslma_Allocator

#endif
