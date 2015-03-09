#ifndef BSL_OVERRIDES_STD
#define BSL_OVERRIDES_STD
#endif
// Include all standard headers in reverse alphabetical order

#include <vector>
#include <valarray>
#include <utility>
#include <unordered_set>
#include <unordered_map>
#include <typeinfo>
#include <typeindex>
#include <tuple>
#include <thread>
#include <strstream>
#include <string>
#include <streambuf>
#include <stdexcept>
#include <stack>
#include <sstream>
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
#include <slist>
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
#include <system_error>
#include <set>
#include <scoped_allocator>
#include <regex>
#include <ratio>
#include <random>
#include <queue>
#include <ostream>
#include <numeric>
#include <new>
#include <mutex>
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
#include <initializer_list>
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
#include <hash_set>
#include <hash_map>
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
#include <future>
#include <functional>
#include <fstream>
#include <exception>
#include <deque>
#include <cwctype>
#include <cwchar>
#include <cuchar>
#include <ctime>
#include <ctgmath>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <cstdbool>
#include <cstdarg>
#include <csignal>
#include <csetjmp>
#include <condition_variable>
//#include <complex>
#include <cmath>
#include <clocale>
#include <climits>
#include <ciso646>
#include <cinttypes>
#include <chrono>
#include <cfloat>
#include <cfenv>
#include <cerrno>
#include <cctype>
#include <cassert>
#include <bitset>
#include <atomic>
#include <array>
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
// Copyright 2013 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
