#undef BSL_OVERRIDES_STD
#include <bsl_iostream.h>
#include <iostream>
#ifdef std
#   error std was not expected to be a macro
#endif
namespace std { }
int main() { return 0; }

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
