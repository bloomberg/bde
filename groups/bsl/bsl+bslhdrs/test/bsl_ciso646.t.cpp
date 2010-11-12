#include <bsl_ciso646.h>
#include <ciso646>
#ifdef std
#   error std was not expected to be a macro
#endif
namespace std { }
int main() { return 0; }
