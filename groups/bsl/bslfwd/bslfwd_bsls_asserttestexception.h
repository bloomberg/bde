#ifndef INCLUDED_BSLFWD_BSLS_ASSERTTESTEXCEPTION
#define INCLUDED_BSLFWD_BSLS_ASSERTTESTEXCEPTION

#ifndef BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

#ifdef BDE_BUILD_TARGET_BSL_NAMESPACES

namespace BloombergLP {
    namespace bsls {
        class AssertTestException;
    }

    typedef bsls::AssertTestException bsls_AssertTestException;
}

#else

namespace BloombergLP {
    class bsls_AssertTestException;
}

#endif

#endif
