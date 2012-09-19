#ifndef BDE_OMIT_TRANSITIONAL // DEPRECATED
//#include <bsl_hash_set.h>
#ifdef std
#   error std was not expected to be a macro
#endif
namespace std { }
int main() { return 0; }
#else
int main() { return -1; }
#endif  // BDE_OMIT_TRANSITIONAL -- DEPRECATED
