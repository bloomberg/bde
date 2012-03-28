#ifndef INCLUDED_BSLFWD_BSLS_TYPES
#define INCLUDED_BSLFWD_BSLS_TYPES

#ifndef BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

#ifdef BDE_BUILD_TARGET_BSL_NAMESPACES

namespace BloombergLP {
    namespace bsls {
        class Types;
    }

    typedef bsls::Types bsls_Types;
}

#else

namespace BloombergLP {
    class bsls_Types;
}

#endif

#endif
