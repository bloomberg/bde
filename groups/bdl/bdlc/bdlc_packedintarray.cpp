// bdlc_packedintarray.cpp                                            -*-C++-*-
#include <bdlc_packedintarray.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlc_packedintarray,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bslma_default.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_types.h>

#include <bsl_cstdint.h>

namespace BloombergLP {
namespace bdlc {

                      // -------------------------------
                      // struct PackedIntArrayImp_Signed
                      // -------------------------------

int PackedIntArrayImp_Signed::
                            requiredBytesPerElement(EightByteStorageType value)
{
    if (value >= bsl::numeric_limits<bsl::int8_t>::min()) {
        if (value > bsl::numeric_limits<bsl::int16_t>::max()) {
            if (value > bsl::numeric_limits<bsl::int32_t>::max()) {
                return 8;                                             // RETURN
            }
            else {
                return 4;                                             // RETURN
            }
        }
        else {
            if (value > bsl::numeric_limits<bsl::int8_t>::max()) {
                return 2;                                             // RETURN
            }
            else {
                return 1;                                             // RETURN
            }
        }
    }
    else {
        if (value >= bsl::numeric_limits<bsl::int16_t>::min()) {
            return 2;                                                 // RETURN
        }
        else if (value >= bsl::numeric_limits<bsl::int32_t>::min()) {
            return 4;                                                 // RETURN
        }
    }
    return 8;                                                         // RETURN
}

                     // ---------------------------------
                     // struct PackedIntArrayImp_Unsigned
                     // ---------------------------------

int PackedIntArrayImp_Unsigned::
                            requiredBytesPerElement(EightByteStorageType value)
{
    if (value > bsl::numeric_limits<bsl::uint16_t>::max()) {
        if (value > bsl::numeric_limits<bsl::uint32_t>::max()) {
            return 8;                                                 // RETURN
        }
        else {
            return 4;                                                 // RETURN
        }
    }
    else if (value > bsl::numeric_limits<bsl::uint8_t>::max()) {
        return 2;                                                     // RETURN
    }
    return 1;                                                         // RETURN
}

                          // ------------------------
                          // struct PackedIntArrayImp
                          // ------------------------

// PRIVATE MANIPULATORS
template <class STORAGE>
void PackedIntArrayImp<STORAGE>::expandImp(int         requiredBytesPerElement,
                                           bsl::size_t requiredCapacityInBytes)
{
    BSLS_ASSERT(requiredBytesPerElement > d_bytesPerElement);

    // Compute next capacity level.

    requiredCapacityInBytes = nextCapacityGE(requiredCapacityInBytes,
                                             d_capacityInBytes);

    // Allocate new memory.

    void *src = d_storage_p;
    d_storage_p = d_allocator_p->allocate(requiredCapacityInBytes);
    d_capacityInBytes = requiredCapacityInBytes;

    // Copy existing data.

    int srcBytesPerElement = d_bytesPerElement;
    d_bytesPerElement = requiredBytesPerElement;
    replaceImp(d_storage_p,
               0,
               d_bytesPerElement,
               src,
               0,
               srcBytesPerElement,
               d_length);

    // Deallocate original memory.

    d_allocator_p->deallocate(src);
}

template <class STORAGE>
void PackedIntArrayImp<STORAGE>::replaceImp(bsl::size_t dstIndex,
                                            ElementType value)
{
    BSLS_ASSERT(dstIndex < d_length);
    BSLS_ASSERT(STORAGE::requiredBytesPerElement(value) <= d_bytesPerElement);

    switch (d_bytesPerElement) {
      case 1: {
        static_cast<typename STORAGE::OneByteStorageType *>
            (d_storage_p)[dstIndex] =
                      static_cast<typename STORAGE::OneByteStorageType>(value);
      } break;
      case 2: {
        static_cast<typename STORAGE::TwoByteStorageType *>
            (d_storage_p)[dstIndex] =
                      static_cast<typename STORAGE::TwoByteStorageType>(value);
      } break;
      case 4: {
        static_cast<typename STORAGE::FourByteStorageType *>
            (d_storage_p)[dstIndex] =
                     static_cast<typename STORAGE::FourByteStorageType>(value);
      } break;
      case 8: {
        static_cast<typename STORAGE::EightByteStorageType *>
            (d_storage_p)[dstIndex] =
                    static_cast<typename STORAGE::EightByteStorageType>(value);
      } break;
      default: {
        // Only the above values are valid so this case should never happen.

        BSLS_ASSERT_OPT("Invalid value for 'd_bytesPerElement'." && 0);
      } break;
    }
}

template <class STORAGE>
void PackedIntArrayImp<STORAGE>::replaceImp(void        *dst,
                                            bsl::size_t  dstIndex,
                                            int          dstBytesPerElement,
                                            void        *src,
                                            bsl::size_t  srcIndex,
                                            int          srcBytesPerElement,
                                            bsl::size_t  numElements)
{
    // Assert 'dstIndex + numElements <= d_length' without risk of overflow.
    BSLS_ASSERT(numElements <= d_length);
    BSLS_ASSERT(dstIndex    <= d_length - numElements);

    BSLS_ASSERT(srcBytesPerElement != dstBytesPerElement);
    BSLS_ASSERT(   (static_cast<char *>(dst) + dstIndex * dstBytesPerElement
                                         <= static_cast<char *>(src)
                                                + srcIndex * srcBytesPerElement
                    && static_cast<char *>(dst)
                                + (dstIndex + numElements) * dstBytesPerElement
                                         <= static_cast<char *>(src)
                                               + srcIndex * srcBytesPerElement)
                || (static_cast<char *>(dst) + dstIndex * dstBytesPerElement
                                         >= static_cast<char *>(src)
                                                     + (srcIndex + numElements)
                                                           * srcBytesPerElement
                         && static_cast<char *>(dst)
                                + (dstIndex + numElements) * dstBytesPerElement
                                         >= static_cast<char *>(src)
                                                     + (srcIndex + numElements)
                                                          * srcBytesPerElement)
               || (dst == src
                   && dstIndex >= srcIndex
                   && dstBytesPerElement > srcBytesPerElement));

    switch (dstBytesPerElement) {
      case 1: {
        typename STORAGE::OneByteStorageType *d =
                      static_cast<typename STORAGE::OneByteStorageType *>(dst);
        d += dstIndex;
        switch (srcBytesPerElement) {
          case 2: {
            typename STORAGE::TwoByteStorageType *s =
                      static_cast<typename STORAGE::TwoByteStorageType *>(src);
            s += srcIndex;
            s += numElements;
            d += numElements;
            for (bsl::size_t i = 0; i < numElements; ++i) {
                *--d = static_cast<typename STORAGE::OneByteStorageType>(*--s);
            }
          } break;
          case 4: {
            typename STORAGE::FourByteStorageType *s =
                     static_cast<typename STORAGE::FourByteStorageType *>(src);
            s += srcIndex;
            s += numElements;
            d += numElements;
            for (bsl::size_t i = 0; i < numElements; ++i) {
                *--d = static_cast<typename STORAGE::OneByteStorageType>(*--s);
            }
          } break;
          case 8: {
            typename STORAGE::EightByteStorageType *s =
                    static_cast<typename STORAGE::EightByteStorageType *>(src);
            s += srcIndex;
            s += numElements;
            d += numElements;
            for (bsl::size_t i = 0; i < numElements; ++i) {
                *--d = static_cast<typename STORAGE::OneByteStorageType>(*--s);
            }
          } break;
          default: {
            // Only the above values are valid so this case should never
            // happen.

            BSLS_ASSERT_OPT("Invalid value for 'srcBytesPerElement'." && 0);
          } break;
        }
      } break;
      case 2: {
        typename STORAGE::TwoByteStorageType *d =
                      static_cast<typename STORAGE::TwoByteStorageType *>(dst);
        d += dstIndex;
        switch (srcBytesPerElement) {
          case 1: {
            typename STORAGE::OneByteStorageType *s =
                      static_cast<typename STORAGE::OneByteStorageType *>(src);
            s += srcIndex;
            s += numElements;
            d += numElements;
            for (bsl::size_t i = 0; i < numElements; ++i) {
                *--d = static_cast<typename STORAGE::TwoByteStorageType>(*--s);
            }
          } break;
          case 4: {
            typename STORAGE::FourByteStorageType *s =
                     static_cast<typename STORAGE::FourByteStorageType *>(src);
            s += srcIndex;
            s += numElements;
            d += numElements;
            for (bsl::size_t i = 0; i < numElements; ++i) {
                *--d = static_cast<typename STORAGE::TwoByteStorageType>(*--s);
            }
          } break;
          case 8: {
            typename STORAGE::EightByteStorageType *s =
                    static_cast<typename STORAGE::EightByteStorageType *>(src);
            s += srcIndex;
            s += numElements;
            d += numElements;
            for (bsl::size_t i = 0; i < numElements; ++i) {
                *--d = static_cast<typename STORAGE::TwoByteStorageType>(*--s);
            }
          } break;
          default: {
            // Only the above values are valid so this case should never
            // happen.

            BSLS_ASSERT_OPT("Invalid value for 'srcBytesPerElement'." && 0);
          } break;
        }
      } break;
      case 4: {
        typename STORAGE::FourByteStorageType *d =
                     static_cast<typename STORAGE::FourByteStorageType *>(dst);
        d += dstIndex;
        switch (srcBytesPerElement) {
          case 1: {
            typename STORAGE::OneByteStorageType *s =
                      static_cast<typename STORAGE::OneByteStorageType *>(src);
            s += srcIndex;
            s += numElements;
            d += numElements;
            for (bsl::size_t i = 0; i < numElements; ++i) {
                *--d =
                      static_cast<typename STORAGE::FourByteStorageType>(*--s);
            }
          } break;
          case 2: {
            typename STORAGE::TwoByteStorageType *s =
                      static_cast<typename STORAGE::TwoByteStorageType *>(src);
            s += srcIndex;
            s += numElements;
            d += numElements;
            for (bsl::size_t i = 0; i < numElements; ++i) {
                *--d =
                      static_cast<typename STORAGE::FourByteStorageType>(*--s);
            }
          } break;
          case 8: {
            typename STORAGE::EightByteStorageType *s =
                    static_cast<typename STORAGE::EightByteStorageType *>(src);
            s += srcIndex;
            s += numElements;
            d += numElements;
            for (bsl::size_t i = 0; i < numElements; ++i) {
                *--d =
                      static_cast<typename STORAGE::FourByteStorageType>(*--s);
            }
          } break;
          default: {
            // Only the above values are valid so this case should never
            // happen.

            BSLS_ASSERT_OPT("Invalid value for 'srcBytesPerElement'." && 0);
          } break;
        }
      } break;
      case 8: {
        typename STORAGE::EightByteStorageType *d =
                    static_cast<typename STORAGE::EightByteStorageType *>(dst);
        d += dstIndex;
        switch (srcBytesPerElement) {
          case 1: {
            typename STORAGE::OneByteStorageType *s =
                      static_cast<typename STORAGE::OneByteStorageType *>(src);
            s += srcIndex;
            s += numElements;
            d += numElements;
            for (bsl::size_t i = 0; i < numElements; ++i) {
                *--d =
                     static_cast<typename STORAGE::EightByteStorageType>(*--s);
            }
          } break;
          case 2: {
            typename STORAGE::TwoByteStorageType *s =
                      static_cast<typename STORAGE::TwoByteStorageType *>(src);
            s += srcIndex;
            s += numElements;
            d += numElements;
            for (bsl::size_t i = 0; i < numElements; ++i) {
                *--d =
                     static_cast<typename STORAGE::EightByteStorageType>(*--s);
            }
          } break;
          case 4: {
            typename STORAGE::FourByteStorageType *s =
                     static_cast<typename STORAGE::FourByteStorageType *>(src);
            s += srcIndex;
            s += numElements;
            d += numElements;
            for (bsl::size_t i = 0; i < numElements; ++i) {
                *--d =
                     static_cast<typename STORAGE::EightByteStorageType>(*--s);
            }
          } break;
          default: {
            // Only the above values are valid so this case should never
            // happen.

            BSLS_ASSERT_OPT("Invalid value for 'srcBytesPerElement'." && 0);
          } break;
        }
      } break;
      default: {
        // Only the above values are valid so this case should never happen.

        BSLS_ASSERT_OPT("Invalid value for 'dstBytesPerElement'." && 0);
      } break;
    }
}

template <class STORAGE>
int PackedIntArrayImp<STORAGE>::
      requiredBytesPerElement(bsl::size_t index, bsl::size_t numElements) const
{
    // Assert 'index + numElements <= d_length' without risk of overflow.
    BSLS_ASSERT(numElements <= d_length);
    BSLS_ASSERT(index       <= d_length - numElements);

    bsl::size_t endIndex = index + numElements;

    int requiredBytesPerElement = 1;
    switch (d_bytesPerElement) {
      case 1: {
      } break;
      case 2: {
        typename STORAGE::TwoByteStorageType *s =
              static_cast<typename STORAGE::TwoByteStorageType *>(d_storage_p);
        for (bsl::size_t i = index; i < endIndex; ++i) {
            int rbpe = STORAGE::requiredBytesPerElement(
                                               static_cast<ElementType>(s[i]));
            if (requiredBytesPerElement < rbpe) {
                requiredBytesPerElement = rbpe;
                break;
            }
        }
      } break;
      case 4: {
        typename STORAGE::FourByteStorageType *s =
             static_cast<typename STORAGE::FourByteStorageType *>(d_storage_p);
        for (bsl::size_t i = index; i < endIndex; ++i) {
            int rbpe = STORAGE::requiredBytesPerElement(
                                               static_cast<ElementType>(s[i]));
            if (requiredBytesPerElement < rbpe) {
                requiredBytesPerElement = rbpe;
                if (4 == requiredBytesPerElement) {
                    break;
                }
            }
        }
      } break;
      case 8: {
        typename STORAGE::EightByteStorageType *s =
            static_cast<typename STORAGE::EightByteStorageType *>(d_storage_p);
        for (bsl::size_t i = index; i < endIndex; ++i) {
            int rbpe = STORAGE::requiredBytesPerElement(
                                               static_cast<ElementType>(s[i]));
            if (requiredBytesPerElement < rbpe) {
                requiredBytesPerElement = rbpe;
                if (8 == requiredBytesPerElement) {
                    break;
                }
            }
        }
      } break;
      default: {
        // Only the above values are valid so this case should never happen.

        BSLS_ASSERT_OPT("Invalid value for 'd_bytesPerElement'." && 0);
      } break;
    }
    return requiredBytesPerElement;
}

// PRIVATE ACCESSORS
template <class STORAGE>
bool PackedIntArrayImp<STORAGE>::isEqualImp(
                                 const PackedIntArrayImp<STORAGE>& other) const
{
    BSLS_ASSERT(d_length == other.d_length);
    BSLS_ASSERT(d_bytesPerElement != other.d_bytesPerElement);

    switch (d_bytesPerElement) {
      case 1: {
        typename STORAGE::OneByteStorageType *t =
              static_cast<typename STORAGE::OneByteStorageType *>(d_storage_p);
        switch (other.d_bytesPerElement) {
          case 2: {
            typename STORAGE::TwoByteStorageType *o =
                           static_cast<typename STORAGE::TwoByteStorageType *>(
                                                            other.d_storage_p);
            for (bsl::size_t i = 0; i < d_length; ++i) {
                if (static_cast<ElementType>(t[i]) !=
                                              static_cast<ElementType>(o[i])) {
                    return false;                                     // RETURN
                }
            }
          } break;
          case 4: {
            typename STORAGE::FourByteStorageType *o =
                          static_cast<typename STORAGE::FourByteStorageType *>(
                                                            other.d_storage_p);
            for (bsl::size_t i = 0; i < d_length; ++i) {
                if (static_cast<ElementType>(t[i]) !=
                                              static_cast<ElementType>(o[i])) {
                    return false;                                     // RETURN
                }
            }
          } break;
          case 8: {
            typename STORAGE::EightByteStorageType *o =
                         static_cast<typename STORAGE::EightByteStorageType *>(
                                                            other.d_storage_p);
            for (bsl::size_t i = 0; i < d_length; ++i) {
                if (static_cast<ElementType>(t[i]) !=
                                              static_cast<ElementType>(o[i])) {
                    return false;                                     // RETURN
                }
            }
          } break;
          default: {
            // Only the above values are valid so this case should never
            // happen.

            BSLS_ASSERT_OPT("Invalid value for 'd_bytesPerElement'." && 0);
          } break;
        }
      } break;
      case 2: {
        typename STORAGE::TwoByteStorageType *t =
              static_cast<typename STORAGE::TwoByteStorageType *>(d_storage_p);
        switch (other.d_bytesPerElement) {
          case 1: {
            typename STORAGE::OneByteStorageType *o =
                           static_cast<typename STORAGE::OneByteStorageType *>(
                                                            other.d_storage_p);
            for (bsl::size_t i = 0; i < d_length; ++i) {
                if (static_cast<ElementType>(t[i]) !=
                                              static_cast<ElementType>(o[i])) {
                    return false;                                     // RETURN
                }
            }
          } break;
          case 4: {
            typename STORAGE::FourByteStorageType *o =
                          static_cast<typename STORAGE::FourByteStorageType *>(
                                                            other.d_storage_p);
            for (bsl::size_t i = 0; i < d_length; ++i) {
                if (static_cast<ElementType>(t[i]) !=
                                              static_cast<ElementType>(o[i])) {
                    return false;                                     // RETURN
                }
            }
          } break;
          case 8: {
            typename STORAGE::EightByteStorageType *o =
                         static_cast<typename STORAGE::EightByteStorageType *>(
                                                            other.d_storage_p);
            for (bsl::size_t i = 0; i < d_length; ++i) {
                if (static_cast<ElementType>(t[i]) !=
                                              static_cast<ElementType>(o[i])) {
                    return false;                                     // RETURN
                }
            }
          } break;
          default: {
            // Only the above values are valid so this case should never
            // happen.

            BSLS_ASSERT_OPT("Invalid value for 'd_bytesPerElement'." && 0);
          } break;
        }
      } break;
      case 4: {
        typename STORAGE::FourByteStorageType *t =
             static_cast<typename STORAGE::FourByteStorageType *>(d_storage_p);
        switch (other.d_bytesPerElement) {
          case 1: {
            typename STORAGE::OneByteStorageType *o =
                           static_cast<typename STORAGE::OneByteStorageType *>(
                                                            other.d_storage_p);
            for (bsl::size_t i = 0; i < d_length; ++i) {
                if (static_cast<ElementType>(t[i]) !=
                                              static_cast<ElementType>(o[i])) {
                    return false;                                     // RETURN
                }
            }
          } break;
          case 2: {
            typename STORAGE::TwoByteStorageType *o =
                           static_cast<typename STORAGE::TwoByteStorageType *>(
                                                            other.d_storage_p);
            for (bsl::size_t i = 0; i < d_length; ++i) {
                if (static_cast<ElementType>(t[i]) !=
                                              static_cast<ElementType>(o[i])) {
                    return false;                                     // RETURN
                }
            }
          } break;
          case 8: {
            typename STORAGE::EightByteStorageType *o =
                         static_cast<typename STORAGE::EightByteStorageType *>(
                                                            other.d_storage_p);
            for (bsl::size_t i = 0; i < d_length; ++i) {
                if (static_cast<ElementType>(t[i]) !=
                                              static_cast<ElementType>(o[i])) {
                    return false;                                     // RETURN
                }
            }
          } break;
          default: {
            // Only the above values are valid so this case should never
            // happen.

            BSLS_ASSERT_OPT("Invalid value for 'd_bytesPerElement'." && 0);
          } break;
        }
      } break;
      case 8: {
        typename STORAGE::EightByteStorageType *t =
            static_cast<typename STORAGE::EightByteStorageType *>(d_storage_p);
        switch (other.d_bytesPerElement) {
          case 1: {
            typename STORAGE::OneByteStorageType *o =
                           static_cast<typename STORAGE::OneByteStorageType *>(
                                                            other.d_storage_p);
            for (bsl::size_t i = 0; i < d_length; ++i) {
                if (static_cast<ElementType>(t[i]) !=
                                              static_cast<ElementType>(o[i])) {
                    return false;                                     // RETURN
                }
            }
          } break;
          case 2: {
            typename STORAGE::TwoByteStorageType *o =
                           static_cast<typename STORAGE::TwoByteStorageType *>(
                                                            other.d_storage_p);
            for (bsl::size_t i = 0; i < d_length; ++i) {
                if (static_cast<ElementType>(t[i]) !=
                                              static_cast<ElementType>(o[i])) {
                    return false;                                     // RETURN
                }
            }
          } break;
          case 4: {
            typename STORAGE::FourByteStorageType *o =
                          static_cast<typename STORAGE::FourByteStorageType *>(
                                                            other.d_storage_p);
            for (bsl::size_t i = 0; i < d_length; ++i) {
                if (static_cast<ElementType>(t[i]) !=
                                              static_cast<ElementType>(o[i])) {
                    return false;                                     // RETURN
                }
            }
          } break;
          default: {
            // Only the above values are valid so this case should never
            // happen.

            BSLS_ASSERT_OPT("Invalid value for 'd_bytesPerElement'." && 0);
          } break;
        }
      } break;
      default: {
        // Only the above values are valid so this case should never happen.

        BSLS_ASSERT_OPT("Invalid value for 'd_BytesPerElement'." && 0);
      } break;
    }
    return true;
}

// CREATORS
template <class STORAGE>
PackedIntArrayImp<STORAGE>::PackedIntArrayImp(bslma::Allocator *basicAllocator)
: d_storage_p(0)
, d_length(0)
, d_bytesPerElement(1)
, d_capacityInBytes(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

template <class STORAGE>
PackedIntArrayImp<STORAGE>::PackedIntArrayImp(bsl::size_t       numElements,
                                              ElementType       value,
                                              bslma::Allocator *basicAllocator)
: d_storage_p(0)
, d_length(numElements)
, d_bytesPerElement(1)
, d_capacityInBytes(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (0 == d_length) return;                                        // RETURN

    d_bytesPerElement = STORAGE::requiredBytesPerElement(value);

    // Test for potential overflow.
    BSLS_ASSERT(k_MAX_CAPACITY / d_bytesPerElement >= d_length);

    d_capacityInBytes = nextCapacityGE(d_length * d_bytesPerElement, 0);

    d_storage_p = d_allocator_p->allocate(d_capacityInBytes);

    switch (d_bytesPerElement) {
      case 1: {
        typename STORAGE::OneByteStorageType *s =
              static_cast<typename STORAGE::OneByteStorageType *>(d_storage_p);
        for (bsl::size_t i = 0; i < d_length; ++i) {
            s[i] = static_cast<typename STORAGE::OneByteStorageType>(value);
        }
      } break;
      case 2: {
        typename STORAGE::TwoByteStorageType *s =
              static_cast<typename STORAGE::TwoByteStorageType *>(d_storage_p);
        for (bsl::size_t i = 0; i < d_length; ++i) {
            s[i] = static_cast<typename STORAGE::TwoByteStorageType>(value);
        }
      } break;
      case 4: {
        typename STORAGE::FourByteStorageType *s =
             static_cast<typename STORAGE::FourByteStorageType *>(d_storage_p);
        for (bsl::size_t i = 0; i < d_length; ++i) {
            s[i] = static_cast<typename STORAGE::FourByteStorageType>(value);
        }
      } break;
      case 8: {
        typename STORAGE::EightByteStorageType *s =
            static_cast<typename STORAGE::EightByteStorageType *>(d_storage_p);
        for (bsl::size_t i = 0; i < d_length; ++i) {
            s[i] = static_cast<typename STORAGE::EightByteStorageType>(value);
        }
      } break;
      default: {
        // Only the above values are valid so this case should never happen.

        BSLS_ASSERT_OPT("Invalid value for 'd_bytesPerElement'." && 0);
      } break;
    }
}

template <class STORAGE>
PackedIntArrayImp<STORAGE>::PackedIntArrayImp(
                             const PackedIntArrayImp<STORAGE>&  original,
                             bslma::Allocator                  *basicAllocator)
: d_length(original.d_length)
, d_bytesPerElement(original.d_bytesPerElement)
, d_capacityInBytes(nextCapacityGE(d_length * d_bytesPerElement, 0))
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_storage_p = d_allocator_p->allocate(d_capacityInBytes);
    bsl::memcpy(d_storage_p,
                original.d_storage_p,
                d_length * d_bytesPerElement);
}

template <class STORAGE>
PackedIntArrayImp<STORAGE>::~PackedIntArrayImp()
{
    d_allocator_p->deallocate(d_storage_p);
}

// MANIPULATORS
template <class STORAGE>
PackedIntArrayImp<STORAGE>& PackedIntArrayImp<STORAGE>::
                               operator=(const PackedIntArrayImp<STORAGE>& rhs)
{
    if (&rhs != this) {
        // Ensure sufficient capacity.

        bsl::size_t numBytes = rhs.d_bytesPerElement * rhs.d_length;
        if (numBytes > d_capacityInBytes) {
            // Compute next capacity level.

            bsl::size_t requiredCapacityInBytes =
                                   nextCapacityGE(numBytes, d_capacityInBytes);

            // Allocate new memory.
            void *dst = d_allocator_p->allocate(requiredCapacityInBytes);

            // Deallocate original memory.

            d_allocator_p->deallocate(d_storage_p);

            // Update storage and capacity.

            d_storage_p = dst;
            d_capacityInBytes = requiredCapacityInBytes;
        }

        // Copy the array.

        d_length = rhs.d_length;
        d_bytesPerElement = rhs.d_bytesPerElement;
        bsl::memcpy(d_storage_p, rhs.d_storage_p, numBytes);
    }
    return *this;
}

template <class STORAGE>
void PackedIntArrayImp<STORAGE>::append(ElementType value)
{
    // Note that the limit on capacity precludes an overflow check here.
    bsl::size_t newLength = d_length + 1;

    // Prepare storage for the operation.

    int rbpe = STORAGE::requiredBytesPerElement(value);
    if (d_bytesPerElement >= rbpe) {
        // Test for potential overflow.
        BSLS_ASSERT(k_MAX_CAPACITY / d_bytesPerElement >= newLength);

        bsl::size_t requiredCapacityInBytes = d_bytesPerElement * newLength;
        if (requiredCapacityInBytes > d_capacityInBytes) {
            reserveCapacityImp(requiredCapacityInBytes);
        }
    }
    else {
        // Test for potential overflow.
        BSLS_ASSERT(k_MAX_CAPACITY / rbpe >= newLength);

        bsl::size_t requiredCapacityInBytes = rbpe * newLength;

        if (requiredCapacityInBytes > d_capacityInBytes) {
            expandImp(rbpe, requiredCapacityInBytes);
        }
        else {
            int srcBytesPerElement = d_bytesPerElement;
            d_bytesPerElement = rbpe;
            replaceImp(d_storage_p,
                       0,
                       d_bytesPerElement,
                       d_storage_p,
                       0,
                       srcBytesPerElement,
                       d_length);
        }
    }

    // Append the 'value'.

    bsl::size_t dstIndex = d_length;
    d_length = newLength;
    replaceImp(dstIndex, value);
}

template <class STORAGE>
void PackedIntArrayImp<STORAGE>::append(
                                 const PackedIntArrayImp<STORAGE>& srcArray,
                                 bsl::size_t                       srcIndex,
                                 bsl::size_t                       numElements)
{
    // Assert 'srcIndex + numElements <= srcArray.length()' without risk of
    // overflow.
    BSLS_ASSERT(numElements <= srcArray.length());
    BSLS_ASSERT(srcIndex    <= srcArray.length() - numElements);

    // Note that the limit on capacity precludes an overflow check here.
    bsl::size_t newLength = d_length + numElements;

    // Prepare storage for the operation.

    if (d_bytesPerElement >= srcArray.d_bytesPerElement) {
        // Test for potential overflow.
        BSLS_ASSERT(k_MAX_CAPACITY / d_bytesPerElement >= newLength);

        bsl::size_t requiredCapacityInBytes = d_bytesPerElement * newLength;

        if (requiredCapacityInBytes > d_capacityInBytes) {
            reserveCapacityImp(requiredCapacityInBytes);
        }
    }
    else {
        int rbpe = srcArray.requiredBytesPerElement(srcIndex, numElements);
        if (d_bytesPerElement >= rbpe) {
            // Test for potential overflow.
            BSLS_ASSERT(k_MAX_CAPACITY / d_bytesPerElement >= newLength);

            bsl::size_t requiredCapacityInBytes =
                                                 d_bytesPerElement * newLength;

            if (requiredCapacityInBytes > d_capacityInBytes) {
                reserveCapacityImp(requiredCapacityInBytes);
            }
        }
        else {
            // Test for potential overflow.
            BSLS_ASSERT(k_MAX_CAPACITY / rbpe >= newLength);

            bsl::size_t requiredCapacityInBytes = rbpe * newLength;

            if (requiredCapacityInBytes > d_capacityInBytes) {
                expandImp(rbpe, requiredCapacityInBytes);
            }
            else {
                int srcBytesPerElement = d_bytesPerElement;
                d_bytesPerElement = rbpe;
                replaceImp(d_storage_p,
                           0,
                           d_bytesPerElement,
                           d_storage_p,
                           0,
                           srcBytesPerElement,
                           d_length);
            }
        }
    }

    // Append the values.

    if (d_bytesPerElement == srcArray.d_bytesPerElement) {
        bsl::memcpy(address() + d_length * d_bytesPerElement,
                    srcArray.address() + srcIndex * d_bytesPerElement,
                    numElements * d_bytesPerElement);
        d_length = newLength;
    }
    else {
        bsl::size_t dstIndex = d_length;
        d_length = newLength;
        replaceImp(d_storage_p,
                   dstIndex,
                   d_bytesPerElement,
                   srcArray.d_storage_p,
                   srcIndex,
                   srcArray.d_bytesPerElement,
                   numElements);
    }
}

template <class STORAGE>
void PackedIntArrayImp<STORAGE>::insert(bsl::size_t dstIndex,
                                        ElementType value)
{
    BSLS_ASSERT(dstIndex <= d_length);

    // Note that the limit on capacity precludes an overflow check here.
    bsl::size_t newLength = d_length + 1;

    int rbpe = STORAGE::requiredBytesPerElement(value);
    rbpe = (rbpe >= d_bytesPerElement ? rbpe : d_bytesPerElement);

    // Test for potential overflow.
    BSLS_ASSERT(k_MAX_CAPACITY / rbpe >= newLength);

    bsl::size_t requiredCapacityInBytes = rbpe * newLength;

    if (requiredCapacityInBytes > d_capacityInBytes) {
        // Compute next capacity level.

        requiredCapacityInBytes = nextCapacityGE(requiredCapacityInBytes,
                                                 d_capacityInBytes);

        // Allocate new memory.

        void *dst = d_allocator_p->allocate(requiredCapacityInBytes);

        // Copy existing data and update 'd_length'.

        if (d_bytesPerElement == rbpe) {
            bsl::memcpy(static_cast<char *>(dst)
                                          + (dstIndex + 1) * d_bytesPerElement,
                        address() + dstIndex * d_bytesPerElement,
                        (d_length - dstIndex) * d_bytesPerElement);
            bsl::memcpy(dst,
                        d_storage_p,
                        dstIndex * d_bytesPerElement);
            d_length = newLength;
        }
        else {
            bsl::size_t oldLength = d_length;
            d_length = newLength;

            replaceImp(dst,
                       dstIndex + 1,
                       rbpe,
                       d_storage_p,
                       dstIndex,
                       d_bytesPerElement,
                       oldLength - dstIndex);

            replaceImp(dst,
                       0,
                       rbpe,
                       d_storage_p,
                       0,
                       d_bytesPerElement,
                       dstIndex);
        }

        // Deallocate original memory and store location of new memory.

        d_allocator_p->deallocate(d_storage_p);
        d_storage_p = dst;
    }
    else {
        // Copy existing data and update 'd_length'.

        if (d_bytesPerElement == rbpe) {
            bsl::memmove(address() + (dstIndex + 1) * d_bytesPerElement,
                         address() + dstIndex * d_bytesPerElement,
                         (d_length - dstIndex) * d_bytesPerElement);
            d_length = newLength;
        }
        else {
            bsl::size_t oldLength = d_length;
            d_length = newLength;

            replaceImp(d_storage_p,
                       dstIndex + 1,
                       rbpe,
                       d_storage_p,
                       dstIndex,
                       d_bytesPerElement,
                       oldLength - dstIndex);

            replaceImp(d_storage_p,
                       0,
                       rbpe,
                       d_storage_p,
                       0,
                       d_bytesPerElement,
                       dstIndex);
        }
    }

    // Update 'd_bytesPerElement'.

    d_bytesPerElement = rbpe;

    // Place new data.

    replaceImp(dstIndex, value);
}

template <class STORAGE>
void PackedIntArrayImp<STORAGE>::insert(
                                 bsl::size_t                       dstIndex,
                                 const PackedIntArrayImp<STORAGE>& srcArray,
                                 bsl::size_t                       srcIndex,
                                 bsl::size_t                       numElements)
{
    BSLS_ASSERT(dstIndex <= d_length);

    // Assert 'srcIndex + numElements <= srcArray.d_length' without risk of
    // overflow.
    BSLS_ASSERT(numElements <= srcArray.d_length);
    BSLS_ASSERT(srcIndex    <= srcArray.d_length - numElements);

    // Note that the limit on capacity precludes an overflow check here.
    bsl::size_t newLength = d_length + numElements;

    int rbpe;
    {
        // Determine required bytes per element.

        if (d_bytesPerElement >= srcArray.d_bytesPerElement) {
            rbpe = d_bytesPerElement;
        }
        else {
            int v = srcArray.requiredBytesPerElement(srcIndex, numElements);
            rbpe = (d_bytesPerElement >= v ? d_bytesPerElement : v);
        }
    }

    // Test for potential overflow.
    BSLS_ASSERT(k_MAX_CAPACITY / rbpe >= newLength);

    bsl::size_t requiredCapacityInBytes = rbpe * newLength;

    if (requiredCapacityInBytes > d_capacityInBytes) {
        // Compute next capacity level.

        requiredCapacityInBytes = nextCapacityGE(requiredCapacityInBytes,
                                                 d_capacityInBytes);

        // Allocate new memory.

        void *dst = d_allocator_p->allocate(requiredCapacityInBytes);

        // Copy existing data and update 'd_length'.

        if (d_bytesPerElement == rbpe) {
            bsl::memcpy(static_cast<char *>(dst)
                                + (dstIndex + numElements) * d_bytesPerElement,
                        address() + dstIndex * d_bytesPerElement,
                        (d_length - dstIndex) * d_bytesPerElement);
            bsl::memcpy(dst,
                        d_storage_p,
                        dstIndex * d_bytesPerElement);
            d_length = newLength;
        }
        else {
            bsl::size_t oldLength = d_length;
            d_length = newLength;

            replaceImp(dst,
                       dstIndex + numElements,
                       rbpe,
                       d_storage_p,
                       dstIndex,
                       d_bytesPerElement,
                       oldLength - dstIndex);

            replaceImp(dst,
                       0,
                       rbpe,
                       d_storage_p,
                       0,
                       d_bytesPerElement,
                       dstIndex);
        }

        // Place new data.

        if (rbpe == srcArray.d_bytesPerElement) {
            bsl::memcpy(static_cast<char *>(dst) + dstIndex * rbpe,
                        srcArray.address() + srcIndex * rbpe,
                        numElements * rbpe);
        }
        else {
            replaceImp(dst,
                       dstIndex,
                       rbpe,
                       srcArray.d_storage_p,
                       srcIndex,
                       srcArray.d_bytesPerElement,
                       numElements);
        }

        // Deallocate original memory and store location of new memory.

        d_allocator_p->deallocate(d_storage_p);
        d_storage_p = dst;

        // Update 'd_bytesPerElement'.

        d_bytesPerElement = rbpe;
    }
    else if (this != &srcArray) {
        // Copy existing data and update 'd_length'.

        if (d_bytesPerElement == rbpe) {
            bsl::memmove(address()
                                + (dstIndex + numElements) * d_bytesPerElement,
                         address() + dstIndex * d_bytesPerElement,
                         (d_length - dstIndex) * d_bytesPerElement);
            d_length = newLength;
        }
        else {
            bsl::size_t oldLength = d_length;
            d_length = newLength;

            replaceImp(d_storage_p,
                       dstIndex + numElements,
                       rbpe,
                       d_storage_p,
                       dstIndex,
                       d_bytesPerElement,
                       oldLength - dstIndex);

            replaceImp(d_storage_p,
                       0,
                       rbpe,
                       d_storage_p,
                       0,
                       d_bytesPerElement,
                       dstIndex);
        }

        // Place new data.

        if (rbpe == srcArray.d_bytesPerElement) {
            bsl::memcpy(address() + dstIndex * rbpe,
                        srcArray.address() + srcIndex * rbpe,
                        numElements * rbpe);
        }
        else {
            replaceImp(d_storage_p,
                       dstIndex,
                       rbpe,
                       srcArray.d_storage_p,
                       srcIndex,
                       srcArray.d_bytesPerElement,
                       numElements);
        }

        // Update 'd_bytesPerElement'.

        d_bytesPerElement = rbpe;
    }
    else {
        // Perform the operation without allocation while aliased.
        bsl::memmove(address() + (dstIndex + numElements) * d_bytesPerElement,
                     address() + dstIndex * d_bytesPerElement,
                     (d_length - dstIndex) * d_bytesPerElement);

        if (srcIndex + numElements <= dstIndex) {
            bsl::memcpy(address() + dstIndex * d_bytesPerElement,
                        address() + srcIndex * d_bytesPerElement,
                        numElements * d_bytesPerElement);
        }
        else if (srcIndex >= dstIndex) {
            bsl::memcpy(address() + dstIndex * d_bytesPerElement,
                        address()
                                + (srcIndex + numElements) * d_bytesPerElement,
                        numElements * d_bytesPerElement);
        }
        else {
            bsl::size_t n1 = dstIndex - srcIndex;
            bsl::size_t n2 = numElements - n1;
            bsl::memcpy(address() + dstIndex * d_bytesPerElement,
                        address() + srcIndex * d_bytesPerElement,
                        n1 * d_bytesPerElement);
            bsl::memcpy(address() + (dstIndex + n1) * d_bytesPerElement,
                        address()
                                + (dstIndex + numElements) * d_bytesPerElement,
                        n2 * d_bytesPerElement);
        }

        d_length = newLength;
    }
}

template <class STORAGE>
void PackedIntArrayImp<STORAGE>::replace(bsl::size_t dstIndex,
                                         ElementType value)
{
    BSLS_ASSERT(dstIndex < d_length);

    // Prepare storage for the operation.

    int rbpe = STORAGE::requiredBytesPerElement(value);
    if (d_bytesPerElement < rbpe) {
        // Test for potential overflow.
        BSLS_ASSERT(k_MAX_CAPACITY / rbpe >= d_length);

        bsl::size_t requiredCapacityInBytes = rbpe * d_length;

        if (requiredCapacityInBytes > d_capacityInBytes) {
            expandImp(rbpe, requiredCapacityInBytes);
        }
        else {
            int srcBytesPerElement = d_bytesPerElement;
            d_bytesPerElement = rbpe;
            replaceImp(d_storage_p,
                       0,
                       d_bytesPerElement,
                       d_storage_p,
                       0,
                       srcBytesPerElement,
                       d_length);
        }
    }

    // Replace the 'value'.

    replaceImp(dstIndex, value);
}

template <class STORAGE>
void PackedIntArrayImp<STORAGE>::replace(
                                 bsl::size_t                       dstIndex,
                                 const PackedIntArrayImp<STORAGE>& srcArray,
                                 bsl::size_t                       srcIndex,
                                 bsl::size_t                       numElements)
{
    // Assert 'dstIndex + numElements <= d_length' without risk of overflow.
    BSLS_ASSERT(numElements <= d_length);
    BSLS_ASSERT(dstIndex    <= d_length - numElements);

    // Assert 'srcIndex + numElements <= srcArray.d_length' without risk of
    // overflow.
    BSLS_ASSERT(numElements <= srcArray.d_length);
    BSLS_ASSERT(srcIndex    <= srcArray.d_length - numElements);

    if (this != &srcArray) {
        if (d_bytesPerElement == srcArray.d_bytesPerElement) {
            bsl::memcpy(address() + dstIndex * d_bytesPerElement,
                        srcArray.address() + srcIndex * d_bytesPerElement,
                        numElements * d_bytesPerElement);
        }
        else {
            int rbpe;
            {
                // Determine required bytes per element.

                if (d_bytesPerElement >= srcArray.d_bytesPerElement) {
                    rbpe = d_bytesPerElement;
                }
                else {
                    int v = srcArray.requiredBytesPerElement(srcIndex,
                                                             numElements);
                    rbpe = (d_bytesPerElement >= v ? d_bytesPerElement : v);
                }
            }

            // Test for potential overflow.
            BSLS_ASSERT(k_MAX_CAPACITY / rbpe >= d_length);

            bsl::size_t requiredCapacityInBytes = rbpe * d_length;

            bsl::size_t ti = dstIndex + numElements;

            if (requiredCapacityInBytes > d_capacityInBytes) {
                // Compute next capacity level.

                requiredCapacityInBytes =
                                        nextCapacityGE(requiredCapacityInBytes,
                                                       d_capacityInBytes);

                // Allocate new memory.

                void *dst = d_allocator_p->allocate(requiredCapacityInBytes);

                // Copy existing data.

                if (d_bytesPerElement == rbpe) {
                    bsl::memcpy(static_cast<char *>(dst)
                                                      + ti * d_bytesPerElement,
                                address() + ti * d_bytesPerElement,
                                (d_length - ti) * d_bytesPerElement);
                    bsl::memcpy(dst,
                                d_storage_p,
                                dstIndex * d_bytesPerElement);
                }
                else {
                    replaceImp(dst,
                               ti,
                               rbpe,
                               d_storage_p,
                               ti,
                               d_bytesPerElement,
                               d_length - ti);

                    replaceImp(dst,
                               0,
                               rbpe,
                               d_storage_p,
                               0,
                               d_bytesPerElement,
                               dstIndex);
                }

                // Place new data.

                if (rbpe == srcArray.d_bytesPerElement) {
                    bsl::memcpy(static_cast<char *>(dst) + dstIndex * rbpe,
                                srcArray.address() + srcIndex * rbpe,
                                numElements * rbpe);
                }
                else {
                    replaceImp(dst,
                               dstIndex,
                               rbpe,
                               srcArray.d_storage_p,
                               srcIndex,
                               srcArray.d_bytesPerElement,
                               numElements);
                }

                // Deallocate original memory and store location of new memory.

                d_allocator_p->deallocate(d_storage_p);
                d_storage_p = dst;

                // Update 'd_bytesPerElement'.

                d_bytesPerElement = rbpe;
            }
            else {
                // Copy existing data.

                if (d_bytesPerElement != rbpe) {
                    replaceImp(d_storage_p,
                               ti,
                               rbpe,
                               d_storage_p,
                               ti,
                               d_bytesPerElement,
                               d_length - ti);

                    replaceImp(d_storage_p,
                               0,
                               rbpe,
                               d_storage_p,
                               0,
                               d_bytesPerElement,
                               dstIndex);
                }

                // Place new data.

                if (rbpe == srcArray.d_bytesPerElement) {
                    bsl::memcpy(address() + dstIndex * rbpe,
                                srcArray.address() + srcIndex * rbpe,
                                numElements * rbpe);
                }
                else {
                    replaceImp(d_storage_p,
                               dstIndex,
                               rbpe,
                               srcArray.d_storage_p,
                               srcIndex,
                               srcArray.d_bytesPerElement,
                               numElements);
                }

                // Update 'd_bytesPerElement'.

                d_bytesPerElement = rbpe;
            }
        }
    }
    else {
        bsl::memmove(address() + dstIndex * d_bytesPerElement,
                     address() + srcIndex * d_bytesPerElement,
                     numElements * d_bytesPerElement);
    }
}

template <class STORAGE>
void PackedIntArrayImp<STORAGE>::reserveCapacityImp(
                                           bsl::size_t requiredCapacityInBytes)
{
    // Compute next capacity level.

    requiredCapacityInBytes = nextCapacityGE(requiredCapacityInBytes,
                                             d_capacityInBytes);

    if (requiredCapacityInBytes > d_capacityInBytes) {

        // Allocate new memory.

        void *src = d_storage_p;
        d_storage_p = d_allocator_p->allocate(requiredCapacityInBytes);
        d_capacityInBytes = requiredCapacityInBytes;

        // Copy existing data.

        bsl::memcpy(d_storage_p, src, d_length * d_bytesPerElement);

        // Deallocate original memory.

        d_allocator_p->deallocate(src);
    }
}

// ACCESSORS
template <class STORAGE>
typename PackedIntArrayImp<STORAGE>::ElementType
                PackedIntArrayImp<STORAGE>::operator[](bsl::size_t index) const
{
    BSLS_ASSERT(index < d_length);

    switch (d_bytesPerElement) {
      case 1: {
        typename STORAGE::OneByteStorageType *s =
                            static_cast<typename STORAGE::OneByteStorageType *>
                                                                 (d_storage_p);
        return static_cast<ElementType>(s[index]);                    // RETURN
      } break;
      case 2: {
        typename STORAGE::TwoByteStorageType *s =
                            static_cast<typename STORAGE::TwoByteStorageType *>
                                                                 (d_storage_p);
        return static_cast<ElementType>(s[index]);                    // RETURN
      } break;
      case 4: {
        typename STORAGE::FourByteStorageType *s =
                           static_cast<typename STORAGE::FourByteStorageType *>
                                                                 (d_storage_p);
        return static_cast<ElementType>(s[index]);                    // RETURN
      } break;
      case 8: {
        typename STORAGE::EightByteStorageType *s =
                          static_cast<typename STORAGE::EightByteStorageType *>
                                                                 (d_storage_p);
        return static_cast<ElementType>(s[index]);                    // RETURN
      } break;
      default: {
        // Only the above values are valid so this case should never happen.

        BSLS_ASSERT_OPT("Invalid value for 'd_bytesPerElement'." && 0);
      } break;
    }
    return 0;  // Note that this RETURN is never reached.
}

template <class STORAGE>
bsl::ostream& PackedIntArrayImp<STORAGE>::print(
                                            bsl::ostream& stream,
                                            int           level,
                                            int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    switch (d_bytesPerElement) {
      case 1: {
        typename STORAGE::OneByteStorageType *s =
                            static_cast<typename STORAGE::OneByteStorageType *>
                                                                 (d_storage_p);
        for (bsl::size_t i = 0; i < d_length; ++i) {
            printer.printValue(static_cast<ElementType>(s[i]));
        }
      } break;
      case 2: {
        typename STORAGE::TwoByteStorageType *s =
                            static_cast<typename STORAGE::TwoByteStorageType *>
                                                                 (d_storage_p);
        for (bsl::size_t i = 0; i < d_length; ++i) {
            printer.printValue(static_cast<ElementType>(s[i]));
        }
      } break;
      case 4: {
        typename STORAGE::FourByteStorageType *s =
                           static_cast<typename STORAGE::FourByteStorageType *>
                                                                 (d_storage_p);
        for (bsl::size_t i = 0; i < d_length; ++i) {
            printer.printValue(static_cast<ElementType>(s[i]));
        }
      } break;
      case 8: {
        typename STORAGE::EightByteStorageType *s =
                          static_cast<typename STORAGE::EightByteStorageType *>
                                                                 (d_storage_p);
        for (bsl::size_t i = 0; i < d_length; ++i) {
            printer.printValue(static_cast<ElementType>(s[i]));
        }
      } break;
      default: {
        // Only the above values are valid so this case should never happen.

        BSLS_ASSERT_OPT("Invalid value for 'd_bytesPerElement'." && 0);
      } break;
    }
    printer.end();
    return stream;
}

template class PackedIntArrayImp<PackedIntArrayImp_Signed>;
template class PackedIntArrayImp<PackedIntArrayImp_Unsigned>;

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
