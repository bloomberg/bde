#ifndef INCLUDED_BSLFWD_BSLS_TIMEUTIL
#define INCLUDED_BSLFWD_BSLS_TIMEUTIL

#ifndef BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

#ifdef BDE_BUILD_TARGET_BSL_NAMESPACES

namespace BloombergLP {
    namespace bsls {
        class TimeUtil;
    }

    typedef bsls::TimeUtil bsls_TimeUtil;
}

#else

namespace BloombergLP {
    class bsls_TimeUtil;
}

#endif

#endif
