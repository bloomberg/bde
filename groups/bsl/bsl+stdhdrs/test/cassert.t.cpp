#ifndef BSL_OVERRIDES_STD
#define BSL_OVERRIDES_STD
#endif
#include <cassert>
#ifndef std
# error std was expected to be a macro
#endif
int main() { return 0; }
