#ifndef INCLUDED_BSLFWD_BSLMA_TESTALLOCATOR
#define INCLUDED_BSLFWD_BSLMA_TESTALLOCATOR

#ifndef BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

#ifdef BDE_BUILD_TARGET_BSL_NAMESPACES

namespace BloombergLP {
    namespace bslma {
        class TestAllocator;
    }

    typedef bslma::TestAllocator bslma_TestAllocator;
}

#else

namespace BloombergLP {
    class bslma_TestAllocator;
}

#endif

#endif
