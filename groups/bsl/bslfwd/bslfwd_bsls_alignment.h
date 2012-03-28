#ifndef INCLUDED_BSLFWD_BSLS_ALIGNMENT
#define INCLUDED_BSLFWD_BSLS_ALIGNMENT

#ifndef BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

#ifdef BDE_BUILD_TARGET_BSL_NAMESPACES

namespace BloombergLP {
    namespace bsls {
        class Alignment;
    }

    typedef bsls::Alignment bsls_Alignment;
}

#else

namespace BloombergLP {
    class bsls_Alignment;
}

#endif

#endif
