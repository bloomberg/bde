#undef BSL_OVERRIDES_STD
#include <bsl_iosfwd.h>
#include <iosfwd>
#ifdef std
#   error std was not expected to be a macro
#endif
namespace std { }
int main() { return 0; }
