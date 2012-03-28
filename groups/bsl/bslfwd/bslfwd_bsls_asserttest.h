#ifndef INCLUDED_BSLFWD_BSLS_ASSERTTEST
#define INCLUDED_BSLFWD_BSLS_ASSERTTEST

#ifndef BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

#ifdef BDE_BUILD_TARGET_BSL_NAMESPACES

namespace BloombergLP {
    namespace bsls {
        class AssertTest;
    }

    typedef bsls::AssertTest bsls_AssertTest;
}

#else

namespace BloombergLP {
    class bsls_AssertTest;
}

#endif

#endif
