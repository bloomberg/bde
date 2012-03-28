#ifndef INCLUDED_BSLFWD_BSLS_NO64BITBUILDTARGET
#define INCLUDED_BSLFWD_BSLS_NO64BITBUILDTARGET

#ifndef BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

#ifdef BDE_BUILD_TARGET_BSL_NAMESPACES

namespace BloombergLP {
    namespace bsls {
        class No64BitBuildTarget;
    }

    typedef bsls::No64BitBuildTarget bsls_No64BitBuildTarget;
}

#else

namespace BloombergLP {
    class bsls_No64BitBuildTarget;
}

#endif

#endif
