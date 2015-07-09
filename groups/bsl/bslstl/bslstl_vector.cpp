// bslstl_vector.cpp                                                  -*-C++-*-
#include <bslstl_vector.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

// IMPLEMENTATION NOTES: The class 'bslstl::Vector' is split in two, for a
// conflation of two reasons:
//
//  1. We want the members 'd_data...' and 'd_capacity' to appear *before* the
//     allocator, which is provided by 'bslstl::ContainerBase' (to potentially
//     take advantage of the empty-base-class-optimization).
//
//  2. The 'bslstl_Vector_Imp' containing these members need only be
//     parameterized by 'VALUE_TYPE' (and not 'ALLOCATOR'), and can provide the
//     iterator and element access methods, leading to shorter debug strings
//     for those methods.
//
// Moreover, in the spirit of template hoisting (providing functionality to all
// all templates in a non-templated utility class), the 'swap' methods is
// implemented below since its definition does not care about the value type.

#include <bsls_assert.h>

#include <bslstl_forwarditerator.h>   // for testing only
#include <bslstl_iterator.h>          // for testing only

#include <string.h>  // for 'memcpy'

namespace bsl {

namespace {
                          // ------------------
                          // struct Vector_Base
                          // ------------------

struct Vector_Base {
    // This 'struct' must have the same layout as a 'Vector_Imp' minus the
    // 'ContainerBase' inherited portion.

    // DATA
    void        *d_dataBegin;
    void        *d_dataEnd;
    std::size_t  d_capacity;
};

}  // close unnamed namespace

                          // ------------------
                          // struct Vector_Util
                          // ------------------

// CLASS METHODS
std::size_t Vector_Util::computeNewCapacity(std::size_t newLength,
                                            std::size_t capacity,
                                            std::size_t maxSize)
{
    BSLS_ASSERT_SAFE(newLength > capacity);
    BSLS_ASSERT_SAFE(newLength <= maxSize);

    capacity += !capacity;
    while (capacity < newLength) {
        std::size_t oldCapacity = capacity;
        capacity *= 2;
        if (capacity < oldCapacity) {
            // We overflowed, e.g., on a 32-bit platform; 'newCapacity' is
            // larger than 2^31.  Terminate the loop.

            return maxSize;                                           // RETURN
        }
    }
    return capacity > maxSize ? maxSize : capacity;
}

void Vector_Util::swap(void *a, void *b)
{
    char c[sizeof(Vector_Base)];
    memcpy(c, a, sizeof(Vector_Base));
    memcpy(a, b, sizeof(Vector_Base));
    memcpy(b, c, sizeof(Vector_Base));
}

}  // close namespace bsl

#ifdef BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE
template class bsl::Vector_ImpBase<bool>;
template class bsl::Vector_ImpBase<char>;
template class bsl::Vector_ImpBase<signed char>;
template class bsl::Vector_ImpBase<unsigned char>;
template class bsl::Vector_ImpBase<short>;
template class bsl::Vector_ImpBase<unsigned short>;
template class bsl::Vector_ImpBase<int>;
template class bsl::Vector_ImpBase<unsigned int>;
template class bsl::Vector_ImpBase<long>;
template class bsl::Vector_ImpBase<unsigned long>;
template class bsl::Vector_ImpBase<long long>;
template class bsl::Vector_ImpBase<unsigned long long>;
template class bsl::Vector_ImpBase<float>;
template class bsl::Vector_ImpBase<double>;
template class bsl::Vector_ImpBase<long double>;
template class bsl::Vector_ImpBase<void *>;  // common base for all
                                             // vectors of pointers
template class bsl::Vector_Imp<bool>;
template class bsl::Vector_Imp<char>;
template class bsl::Vector_Imp<signed char>;
template class bsl::Vector_Imp<unsigned char>;
template class bsl::Vector_Imp<short>;
template class bsl::Vector_Imp<unsigned short>;
template class bsl::Vector_Imp<int>;
template class bsl::Vector_Imp<unsigned int>;
template class bsl::Vector_Imp<long>;
template class bsl::Vector_Imp<unsigned long>;
template class bsl::Vector_Imp<long long>;
template class bsl::Vector_Imp<unsigned long long>;
template class bsl::Vector_Imp<float>;
template class bsl::Vector_Imp<double>;
template class bsl::Vector_Imp<long double>;
template class bsl::Vector_Imp<void *>;  // common base for all
                                         // vectors of pointers
template class bsl::vector<bool>;
template class bsl::vector<char>;
template class bsl::vector<signed char>;
template class bsl::vector<unsigned char>;
template class bsl::vector<short>;
template class bsl::vector<unsigned short>;
template class bsl::vector<int>;
template class bsl::vector<unsigned int>;
template class bsl::vector<long>;
template class bsl::vector<unsigned long>;
template class bsl::vector<long long>;
template class bsl::vector<unsigned long long>;
template class bsl::vector<float>;
template class bsl::vector<double>;
template class bsl::vector<long double>;
template class bsl::vector<void *>;
#endif

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
