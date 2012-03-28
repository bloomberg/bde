#ifndef INCLUDED_BSLFWD_BSLMF_FUNCTIONPOINTERCPLUSPLUSLINKAGE
#define INCLUDED_BSLFWD_BSLMF_FUNCTIONPOINTERCPLUSPLUSLINKAGE

#ifndef BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

#ifdef BDE_BUILD_TARGET_BSL_NAMESPACES

namespace BloombergLP {
    namespace bslmf {
        class FunctionPointerCPlusPlusLinkage;
    }

    typedef bslmf::FunctionPointerCPlusPlusLinkage bslmf_FunctionPointerCPlusPlusLinkage;
}

#else

namespace BloombergLP {
    class bslmf_FunctionPointerCPlusPlusLinkage;
}

#endif

#endif
