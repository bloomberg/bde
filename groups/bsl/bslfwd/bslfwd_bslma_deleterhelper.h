#ifndef INCLUDED_BSLFWD_BSLMA_DELETERHELPER
#define INCLUDED_BSLFWD_BSLMA_DELETERHELPER

#ifndef BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

#ifdef BDE_BUILD_TARGET_BSL_NAMESPACES

namespace BloombergLP {
    namespace bslma {
        class DeleterHelper;
    }

    typedef bslma::DeleterHelper bslma_DeleterHelper;
}

#else

namespace BloombergLP {
    class bslma_DeleterHelper;
}

#endif

#endif
