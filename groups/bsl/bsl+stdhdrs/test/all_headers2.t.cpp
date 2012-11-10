#ifndef BSL_OVERRIDES_STD
#define BSL_OVERRIDES_STD
#endif
// Include all standard headers in reverse alphabetical order

#include <vector>
#include <valarray>
#include <utility>
#include <typeinfo>
#include <strstream>
#include <string>
#include <streambuf>
#include <stdexcept>
#include <stack>
#include <sstream>
#include <slist>
#include <set>
#include <queue>
#include <ostream>
#include <numeric>
#include <new>
#include <memory>
#include <map>
#include <locale>
#include <list>
#include <limits>
#include <iterator>
#include <istream>
#include <iostream>
#include <iosfwd>
#include <ios>
#include <iomanip>
#include <hash_set>
#include <hash_map>
#include <functional>
#include <fstream>
#include <exception>
#include <deque>
#include <cwctype>
#include <cwchar>
#include <ctime>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstddef>
#include <cstdarg>
#include <csignal>
#include <csetjmp>
//#include <complex>
#include <cmath>
#include <clocale>
#include <climits>
#include <ciso646>
#include <cfloat>
#include <cerrno>
#include <cctype>
#include <cassert>
#include <bitset>
#include <algorithm>

#ifndef std
# error std was expected to be a macro
#endif
int main()
{
    std::size_t a = 0;
    std::pair<std::size_t,int> b(a, 0);
    return 0;
}

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
