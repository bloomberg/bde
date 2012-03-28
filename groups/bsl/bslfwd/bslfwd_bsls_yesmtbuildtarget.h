#ifndef INCLUDED_BSLFWD_BSLS_YESMTBUILDTARGET
#define INCLUDED_BSLFWD_BSLS_YESMTBUILDTARGET

#ifndef BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

#ifdef BDE_BUILD_TARGET_BSL_NAMESPACES

namespace BloombergLP {
    namespace bsls {
        class YesMtBuildTarget;
    }

    typedef bsls::YesMtBuildTarget bsls_YesMtBuildTarget;
}

#else

namespace BloombergLP {
    class bsls_YesMtBuildTarget;
}

#endif

#endif
