#ifndef INCLUDED_BSLFWD_BSLS_NOEXCBUILDTARGET
#define INCLUDED_BSLFWD_BSLS_NOEXCBUILDTARGET

#ifndef BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

#ifdef BDE_BUILD_TARGET_BSL_NAMESPACES

namespace BloombergLP {
    namespace bsls {
        class NoExcBuildTarget;
    }

    typedef bsls::NoExcBuildTarget bsls_NoExcBuildTarget;
}

#else

namespace BloombergLP {
    class bsls_NoExcBuildTarget;
}

#endif

#endif
