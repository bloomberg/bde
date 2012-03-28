#ifndef INCLUDED_BSLFWD_BSLMA_DEFAULT
#define INCLUDED_BSLFWD_BSLMA_DEFAULT

#ifndef BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

#ifdef BDE_BUILD_TARGET_BSL_NAMESPACES

namespace BloombergLP {
    namespace bslma {
        class Default;
    }

    typedef bslma::Default bslma_Default;
}

#else

namespace BloombergLP {
    class bslma_Default;
}

#endif

#endif
