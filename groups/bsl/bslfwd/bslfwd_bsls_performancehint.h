#ifndef INCLUDED_BSLFWD_BSLS_PERFORMANCEHINT
#define INCLUDED_BSLFWD_BSLS_PERFORMANCEHINT

#ifndef BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

#ifdef BDE_BUILD_TARGET_BSL_NAMESPACES

namespace BloombergLP {
    namespace bsls {
        class PerformanceHint;
    }

    typedef bsls::PerformanceHint bsls_PerformanceHint;
}

#else

namespace BloombergLP {
    class bsls_PerformanceHint;
}

#endif

#endif
