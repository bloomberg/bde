#undef BSL_OVERRIDES_STD
#include <bsl_sstream.h>
#include <sstream>
#ifdef std
#   error std was not expected to be a macro
#endif
namespace std { }
int main() { return 0; }
