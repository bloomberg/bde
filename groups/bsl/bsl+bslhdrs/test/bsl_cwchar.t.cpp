#include <bsl_cwchar.h>
#include <cwchar>
#ifdef std
#   error std was not expected to be a macro
#endif
namespace std { }
int main() { return 0; }
