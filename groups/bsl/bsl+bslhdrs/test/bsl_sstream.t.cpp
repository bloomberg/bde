#ifndef BDE_OSS_TEST
#include <bsl_sstream.h>
#include <sstream>
#ifdef std
#   error std was not expected to be a macro
#endif
namespace std { }
int main() { return 0; }
#else
int main() { return -1; }
#endif  // #ifndef BDE_OSS_TEST
