#ifndef INCLUDED_BSLFWD_BSLS_PLATFORM
#define INCLUDED_BSLFWD_BSLS_PLATFORM

#ifndef BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

#ifdef BDE_BUILD_TARGET_BSL_NAMESPACES

namespace BloombergLP {
    namespace bsls {
        class Platform;
    }

    typedef bsls::Platform bsls_Platform;
}

#else

namespace BloombergLP {
    class bsls_Platform;
}

#endif

#endif
