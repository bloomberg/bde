#ifndef INCLUDED_BSLFWD_BSLS_ASSERTFAILUREHANDLERGUARD
#define INCLUDED_BSLFWD_BSLS_ASSERTFAILUREHANDLERGUARD

#ifndef BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

#ifdef BDE_BUILD_TARGET_BSL_NAMESPACES

namespace BloombergLP {
    namespace bsls {
        class AssertFailureHandlerGuard;
    }

    typedef bsls::AssertFailureHandlerGuard bsls_AssertFailureHandlerGuard;
}

#else

namespace BloombergLP {
    class bsls_AssertFailureHandlerGuard;
}

#endif

#endif
