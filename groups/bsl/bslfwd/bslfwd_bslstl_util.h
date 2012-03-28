#ifndef INCLUDED_BSLFWD_BSLSTL_UTIL
#define INCLUDED_BSLFWD_BSLSTL_UTIL

#ifndef BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

#ifdef BDE_BUILD_TARGET_BSL_NAMESPACES

namespace BloombergLP {
    namespace bslstl {
        class Util;
    }

    typedef bslstl::Util bslstl_Util;
}

#else

namespace BloombergLP {
    class bslstl_Util;
}

#endif

#endif
