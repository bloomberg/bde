#ifndef INCLUDED_BSLFWD_BSLMF_ANYTYPE
#define INCLUDED_BSLFWD_BSLMF_ANYTYPE

#ifndef BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

#ifdef BDE_BUILD_TARGET_BSL_NAMESPACES

namespace BloombergLP {
    namespace bslmf {
        class AnyType;
    }

    typedef bslmf::AnyType bslmf_AnyType;
}

#else

namespace BloombergLP {
    class bslmf_AnyType;
}

#endif

#endif
