#include <bsl_string.h>
#include <string>

#ifdef std
#   error std was not expected to be a macro
#endif

namespace std { }

int main()
{
    // make sure 'bslstl_StringRef' is included by 'bsl_string.h'
    BloombergLP::bslstl::StringRef *p = NULL;
    (void) p;
    return 0;
}

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
