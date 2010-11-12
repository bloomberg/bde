#include <bsl_clocale.h>
#include <clocale>
#ifdef std
#   error std was not expected to be a macro
#endif
namespace std { }
int main() { return 0; }
