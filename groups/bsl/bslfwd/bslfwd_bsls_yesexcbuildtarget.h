#ifndef INCLUDED_BSLFWD_BSLS_YESEXCBUILDTARGET
#define INCLUDED_BSLFWD_BSLS_YESEXCBUILDTARGET

#ifndef BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

#ifdef BDE_BUILD_TARGET_BSL_NAMESPACES

namespace BloombergLP {
    namespace bsls {
        class YesExcBuildTarget;
    }

    typedef bsls::YesExcBuildTarget bsls_YesExcBuildTarget;
}

#else

namespace BloombergLP {
    class bsls_YesExcBuildTarget;
}

#endif

#endif
