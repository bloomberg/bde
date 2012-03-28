#ifndef INCLUDED_BSLFWD_BSLS_ASSERT
#define INCLUDED_BSLFWD_BSLS_ASSERT

#ifndef BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

#ifdef BDE_BUILD_TARGET_BSL_NAMESPACES

namespace BloombergLP {
    namespace bsls {
        class Assert;
    }

    typedef bsls::Assert bsls_Assert;
}

#else

namespace BloombergLP {
    class bsls_Assert;
}

#endif

#endif
