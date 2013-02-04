#ifndef BSL_OVERRIDES_STD
#define BSL_OVERRIDES_STD
#endif
#include <ios>
#ifndef std
# error std was expected to be a macro
#endif
int main() { return 0; }

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
