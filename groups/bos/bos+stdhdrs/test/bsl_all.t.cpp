// sanity test inclusion of all c++ wrapping headers without BSL_OVERRIDES_STD
// defined to be sure they compile

#undef BSL_OVERRIDES_STD

#include <bsl_algorithm.h>
#include <bsl_array.h>
#include <bsl_atomic.h>
#include <bsl_bitset.h>
#include <bsl_cassert.h>
#include <bsl_cctype.h>
#include <bsl_cerrno.h>
#include <bsl_cfenv.h>
#include <bsl_cfloat.h>
#include <bsl_chrono.h>
#include <bsl_cinttypes.h>
#include <bsl_ciso646.h>
#include <bsl_climits.h>
#include <bsl_clocale.h>
#include <bsl_cmath.h>
#include <bsl_complex.h>
#include <bsl_condition_variable.h>
#include <bsl_csetjmp.h>
#include <bsl_csignal.h>
#include <bsl_cstdarg.h>
#include <bsl_cstdbool.h>
#include <bsl_cstddef.h>
#include <bsl_cstdint.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_ctgmath.h>
#include <bsl_ctime.h>
//#include <bsl_cuchar.h>
#include <bsl_cwchar.h>
#include <bsl_cwctype.h>
#include <bsl_deque.h>
#include <bsl_exception.h>
//#include <bsl_fstream.h>
#include <bsl_functional.h>
#include <bsl_future.h>
//#include <bsl_hash_map.h>
//#include <bsl_hash_set.h>
#include <bsl_initializer_list.h>
#include <bsl_iomanip.h>
#include <bsl_ios.h>
#include <bsl_iosfwd.h>
#include <bsl_iostream.h>
#include <bsl_istream.h>
#include <bsl_iterator.h>
#include <bsl_limits.h>
#include <bsl_list.h>
#include <bsl_locale.h>
#include <bsl_map.h>
#include <bsl_memory.h>
#include <bsl_mutex.h>
#include <bsl_new.h>
#include <bsl_numeric.h>
#include <bsl_ostream.h>
#include <bsl_queue.h>
#include <bsl_random.h>
#include <bsl_ratio.h>
#include <bsl_regex.h>
#include <bsl_scoped_allocator.h>
#include <bsl_set.h>
//#include <bsl_slist.h>
#include <bsl_sstream.h>
#include <bsl_stack.h>
#include <bsl_stdexcept.h>
#include <bsl_streambuf.h>
#include <bsl_string.h>
//#include <bsl_strstream.h>
#include <bsl_system_error.h>
#include <bsl_thread.h>
#include <bsl_tuple.h>
#include <bsl_type_traits.h>
#include <bsl_typeindex.h>
#include <bsl_typeinfo.h>
#include <bsl_unordered_map.h>
#include <bsl_unordered_set.h>
#include <bsl_utility.h>
#include <bsl_valarray.h>
#include <bsl_vector.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <bitset>
#include <cassert>
#include <cctype>
#include <cerrno>
#include <cfenv>
#include <cfloat>
#include <chrono>
#include <cinttypes>
#include <ciso646>
#include <climits>
#include <clocale>
#include <cmath>
#include <complex>
#include <condition_variable>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstdbool>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctgmath>
#include <ctime>
//#include <cuchar>
#include <cwchar>
#include <cwctype>
#include <deque>
#include <exception>
#include <fstream>
#include <functional>
#include <future>
//#include <hash_map>
//#include <hash_set>
#include <initializer_list>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <iterator>
#include <limits>
#include <list>
#include <locale>
#include <map>
#include <memory>
#include <mutex>
#include <new>
#include <numeric>
#include <ostream>
#include <queue>
#include <random>
#include <ratio>
#include <regex>
#include <scoped_allocator>
#include <set>
//#include <slist>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <streambuf>
#include <string>
//#include <strstream>
#include <system_error>
#include <thread>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <valarray>
#include <vector>


#ifdef std
#   error std was not expected to be a macro
#endif

int main() { return 0; }

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
