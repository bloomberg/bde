#ifndef INCLUDED_BSLFWD_BSLMF_NIL
#define INCLUDED_BSLFWD_BSLMF_NIL

#ifndef BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

#ifdef BDE_BUILD_TARGET_BSL_NAMESPACES

namespace BloombergLP {
    namespace bslmf {
        class Nil;
    }

    typedef bslmf::Nil bslmf_Nil;
}

#else

namespace BloombergLP {
    class bslmf_Nil;
}

#endif

#endif
