#undef BSL_OVERRIDES_STD
#include <bsl_cstring.h>
#include <cstring>
#ifdef std
#   error std was not expected to be a macro
#endif
namespace std { }
int main() { return 0; }
