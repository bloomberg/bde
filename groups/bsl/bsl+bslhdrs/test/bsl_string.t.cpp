#include <bsl_string.h>
#include <string>
#ifdef std
#   error std was not expected to be a macro
#endif
namespace std { }
int main() { return 0; }
