#ifndef BSL_PUBLISHED
//#include <bsl_hash_set.h>
#ifdef std
#   error std was not expected to be a macro
#endif
namespace std { }
int main() { return 0; }
#else
int main() { return -1; }
#endif  // #ifndef BSL_PUBLISHED
