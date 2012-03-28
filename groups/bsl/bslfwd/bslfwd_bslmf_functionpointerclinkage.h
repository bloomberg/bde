#ifndef INCLUDED_BSLFWD_BSLMF_FUNCTIONPOINTERCLINKAGE
#define INCLUDED_BSLFWD_BSLMF_FUNCTIONPOINTERCLINKAGE

#ifndef BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

#ifdef BDE_BUILD_TARGET_BSL_NAMESPACES

namespace BloombergLP {
    namespace bslmf {
        class FunctionPointerCLinkage;
    }

    typedef bslmf::FunctionPointerCLinkage bslmf_FunctionPointerCLinkage;
}

#else

namespace BloombergLP {
    class bslmf_FunctionPointerCLinkage;
}

#endif

#endif
