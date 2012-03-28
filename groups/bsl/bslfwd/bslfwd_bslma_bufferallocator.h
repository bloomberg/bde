#ifndef INCLUDED_BSLFWD_BSLMA_BUFFERALLOCATOR
#define INCLUDED_BSLFWD_BSLMA_BUFFERALLOCATOR

#ifndef BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

#ifdef BDE_BUILD_TARGET_BSL_NAMESPACES

namespace BloombergLP {
    namespace bslma {
        class BufferAllocator;
    }

    typedef bslma::BufferAllocator bslma_BufferAllocator;
}

#else

namespace BloombergLP {
    class bslma_BufferAllocator;
}

#endif

#endif
