#include <bsl_memory.h>
#include <memory>
#ifdef std
#   error std was not expected to be a macro
#endif
namespace std { }
int main() { return 0; }
