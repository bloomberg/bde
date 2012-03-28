#ifndef INCLUDED_BSLFWD_BSLS_NOMTBUILDTARGET
#define INCLUDED_BSLFWD_BSLS_NOMTBUILDTARGET

#ifndef BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

#ifdef BDE_BUILD_TARGET_BSL_NAMESPACES

namespace BloombergLP {
    namespace bsls {
        class NoMtBuildTarget;
    }

    typedef bsls::NoMtBuildTarget bsls_NoMtBuildTarget;
}

#else

namespace BloombergLP {
    class bsls_NoMtBuildTarget;
}

#endif

#endif
