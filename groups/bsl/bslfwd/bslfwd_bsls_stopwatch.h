#ifndef INCLUDED_BSLFWD_BSLS_STOPWATCH
#define INCLUDED_BSLFWD_BSLS_STOPWATCH

#ifndef BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

#ifdef BDE_BUILD_TARGET_BSL_NAMESPACES

namespace BloombergLP {
    namespace bsls {
        class Stopwatch;
    }

    typedef bsls::Stopwatch bsls_Stopwatch;
}

#else

namespace BloombergLP {
    class bsls_Stopwatch;
}

#endif

#endif
