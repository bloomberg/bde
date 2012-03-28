#ifndef INCLUDED_BSLFWD_BSLMA_NEWDELETEALLOCATOR
#define INCLUDED_BSLFWD_BSLMA_NEWDELETEALLOCATOR

#ifndef BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

#ifdef BDE_BUILD_TARGET_BSL_NAMESPACES

namespace BloombergLP {
    namespace bslma {
        class NewDeleteAllocator;
    }

    typedef bslma::NewDeleteAllocator bslma_NewDeleteAllocator;
}

#else

namespace BloombergLP {
    class bslma_NewDeleteAllocator;
}

#endif

#endif
