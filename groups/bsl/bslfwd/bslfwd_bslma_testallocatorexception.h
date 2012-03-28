#ifndef INCLUDED_BSLFWD_BSLMA_TESTALLOCATOREXCEPTION
#define INCLUDED_BSLFWD_BSLMA_TESTALLOCATOREXCEPTION

#ifndef BSLFWD_BUILDTARGET
#include <bslfwd_buildtarget.h>
#endif

#ifdef BDE_BUILD_TARGET_BSL_NAMESPACES

namespace BloombergLP {
    namespace bslma {
        class TestAllocatorException;
    }

    typedef bslma::TestAllocatorException bslma_TestAllocatorException;
}

#else

namespace BloombergLP {
    class bslma_TestAllocatorException;
}

#endif

#endif
