#ifndef INCLUDED_BSLFWD_BSLMA_INFREQUENTDELETEBLOCKLIST
#define INCLUDED_BSLFWD_BSLMA_INFREQUENTDELETEBLOCKLIST

#ifndef BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

#ifdef BDE_BUILD_TARGET_BSL_NAMESPACES

namespace BloombergLP {
    namespace bslma {
        class InfrequentDeleteBlockList;
    }

    typedef bslma::InfrequentDeleteBlockList bslma_InfrequentDeleteBlockList;
}

#else

namespace BloombergLP {
    class bslma_InfrequentDeleteBlockList;
}

#endif

#endif
