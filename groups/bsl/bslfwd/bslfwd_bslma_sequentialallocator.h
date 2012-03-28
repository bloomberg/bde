#ifndef INCLUDED_BSLFWD_BSLMA_SEQUENTIALALLOCATOR
#define INCLUDED_BSLFWD_BSLMA_SEQUENTIALALLOCATOR

#ifndef BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

#ifdef BDE_BUILD_TARGET_BSL_NAMESPACES

namespace BloombergLP {
    namespace bslma {
        class SequentialAllocator;
    }

    typedef bslma::SequentialAllocator bslma_SequentialAllocator;
}

#else

namespace BloombergLP {
    class bslma_SequentialAllocator;
}

#endif

#endif
