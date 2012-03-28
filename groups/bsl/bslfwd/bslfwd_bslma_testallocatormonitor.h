#ifndef INCLUDED_BSLFWD_BSLMA_TESTALLOCATORMONITOR
#define INCLUDED_BSLFWD_BSLMA_TESTALLOCATORMONITOR

#ifndef BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

#ifdef BDE_BUILD_TARGET_BSL_NAMESPACES

namespace BloombergLP {
    namespace bslma {
        class TestAllocatorMonitor;
    }

    typedef bslma::TestAllocatorMonitor bslma_TestAllocatorMonitor;
}

#else

namespace BloombergLP {
    class bslma_TestAllocatorMonitor;
}

#endif

#endif
