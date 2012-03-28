#ifndef INCLUDED_BSLFWD_BSLS_UTIL
#define INCLUDED_BSLFWD_BSLS_UTIL

#ifndef BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

#ifdef BDE_BUILD_TARGET_BSL_NAMESPACES

namespace BloombergLP {
    namespace bsls {
        class Util;
    }

    typedef bsls::Util bsls_Util;
}

#else

namespace BloombergLP {
    class bsls_Util;
}

#endif

#endif
