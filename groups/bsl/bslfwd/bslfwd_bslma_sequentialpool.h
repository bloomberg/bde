#ifndef INCLUDED_BSLFWD_BSLMA_SEQUENTIALPOOL
#define INCLUDED_BSLFWD_BSLMA_SEQUENTIALPOOL

#ifndef BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

#ifdef BDE_BUILD_TARGET_BSL_NAMESPACES

namespace BloombergLP {
    namespace bslma {
        class SequentialPool;
    }

    typedef bslma::SequentialPool bslma_SequentialPool;
}

#else

namespace BloombergLP {
    class bslma_SequentialPool;
}

#endif

#endif
