// bdld_datum.cpp                                                     -*-C++-*-
#include <bdld_datum.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdld_datum_cpp,"$Id$ $CSID$")

///Implementation Notes
///--------------------
///
///Implementation on 32-bit platforms
///----------------------------------
// The following paragraphs describe how `Datum` represents all the values
// while maintaining the smallest memory footprint.
//
///IEEE 754 `double` format
///- - - - - - - - - - - - -
// IEEE 754 `double` format uses 1 bit to store the sign, 11 bits to store the
// exponent and the remaining 52 bits to store the fraction part of a
// double-precision floating point number.  The way these bits are rendered
// depends on whether the platform is little-endian or big-endian.  If all the
// bits in the exponent part of a `double` value are 1, then the value
// represents one of the following special `double` values:
//
// * Infinity          - fraction part is 0
// * Negative Infinity - fraction part is 0 and sign bit is 1
// * NaN               - fraction part is a non-zero number
// * Negative NaN      - fraction part is a non-zero number and sign bit is 1
//
// Following is the list of special `double` values on MS Visual Studio and
// GNU C++, for reference.
//
// GNU C++:
// ```
// Quiet NaN         = 7ff8000000000000
// Signaling NaN     = 7ff4000000000000
// Infinity          = 7ff0000000000000
// Negative Infinity = fff0000000000000
// ```
// MS Visual Studio
// ```
// Quiet NaN         = 7ff8000000000000
// Signaling NaN     = 7ff8000000000001
// Infinity          = 7ff0000000000000
// Negative Infinity = fff0000000000000
// Indeterminate     = fff8000000000000
// ```
//
///How values are stored
///- - - - - - - - - - -
// When representing a value of a type other than `double`, `Datum` stores data
// in 48 bits out of the 52 bits of the fraction part and sets all the bits in
// the exponent part to 1, to indicate a special `double` value.  `Datum` uses
// the remaining 4 bits in the fraction part to distinguish between the 16
// different encoding types.  Due to alignment issues, only 4 of the available
// 6 bytes are used to store data in most cases.  Values of 4-byte-aligned
// types like `int`, `bool`, and `bdlt::Date` are stored inline.  Error code
// values that do not have an associated error message are also stored inline.
// Values of larger fixed length types like `Int64`, `bdlt::Datetime`,
// `bdlt::DatetimeInterval` and `Decimal64` are allocated externally and held
// by pointer.  Values of variable length types like strings, binary data,
// error values (having both code and message), arrays of `Datum` objects, and
// maps of `Datum` objects (keyed by strings) are allocated externally and held
// by pointer.  User-defined objects are also held by pointer.  Commonly used
// short length strings (6 or fewer characters in length), short
// `DatetimeInterval` ([-2^47 microseconds, +2^47 microseconds], approximately
// +/- 1628 days), near `Datetime` (now +/- approximately 89 years), small
// `Int64` ([-2^47, +2^47]) and small binary values are stored directly in the
// 6 bytes of storage available in the fraction part.
//
///Implementation on 64-bit platforms
///----------------------------------
// The following paragraph describes the implementation of `Datum` on 64-bit
// platforms.
//
///How values are stored
///- - - - - - - - - - -
// `Datum` stores data inside a 16 byte unsigned character array, two first
// bytes of which are used to distinguish between the different types that can
// be held.  Values of 4-byte-aligned types like `int`, `bool`, and
// `bdlt::Date` are stored inline.  Values of 8 byte aligned types like
// `Int64`, `bdlt::Datetime`, `bdlt::DatetimeInterval` and `Decimal64` are also
// stored inline.  Error code values that do not have an associated error
// message are also stored inline.  Values of variable length types like
// strings, error values (having both code and message), arrays of `Datum`
// objects, and maps of `Datum` objects (keyed by strings) are allocated
// externally and held by pointer.  User-defined objects are also held by
// pointer.  Commonly used short length strings (14 or fewer characters in
// length) and binary values are stored directly in the 14 bytes of available
// storage.
//
///Further Notes
///-------------
// * On 32-bit platforms, memory is allocated in `copyString` only if the
//   string has more than 6 characters (not including the terminating null
//   character), storing the pointer to the allocated string.  Note that the
//   string value passed in is copied.  If the string has fewer than 6
//   characters, a copy is stored inline.  On 64-bit platforms, memory is
//   allocated in `copyString` only if the string has more than 13 characters
//   (not including the terminating null character).  The maximum length for
//   storing without allocation is held in `Datum::k_SHORTSTRING_SIZE`.
//
// * On 32-bit platforms, NaN value is stored as a separate type and cannot be
//   directly retrieved as a `double` value.
//
// * On 64-bit platforms, zero-initialized Datums are not a valid Datum value.
//   This behavior is implemented specifically to help identify the accidental
//   use of uninitialized datum values, which are often 0 initialized for
//   static data.  A similar mechanism is not implemented for 32-bit platforms
//   because of negative performance implications.
//
// * `DatumMapRef::find()` does a binary search if the map is sorted.
//    Otherwise it does a linear search.
//
///R-value and forwarding references
///- - - - - - - - - - - - - - - - -
// As you may see in the `bldl_datummaker.h` file  variadic overloads (for
// example the `pushBackHelper`) do not support perfect forwarding of their
// arguments.  In case `bdld::Datum` will ever support move semantics to some
// (any) of its directly supported types (`create*` function arguments)
// variadic functions in the `bdld::DatumMaker` component shall be updated to
// support perfect forwarding using the `BSLS_COMPILERFEATURES_FORWARD`, and
// `BSLS_COMPILERFEATURES_FORWARDING_REF` macros.

#include <bdlb_print.h>
#include <bdlb_printmethods.h>

#include <bdldfp_decimal.h>
#include <bdldfp_decimalconvertutil.h>

#include <bdlt_currenttime.h>

#include <bslim_printer.h>
#include <bslma_allocator.h>
#include <bslma_usesbslmaallocator.h>
#include <bslmf_assert.h>
#include <bslmf_isbitwiseequalitycomparable.h>
#include <bsls_alignmentutil.h>
#include <bsls_annotation.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_cstddef.h>
#include <bsl_memory.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>
#include <bsl_utility.h>

namespace {

using namespace BloombergLP;
using namespace bdld;

/// Return `true` if key in the specified `lhs` is less than key in the
/// specified `rhs` and `false` otherwise.
bool compareLess(const DatumMapEntry& lhs, const DatumMapEntry& rhs);

/// Clone the elements in the specified `array`.  Use the specified
/// `allocator` to allocate memory (if needed).  Any dynamically allocated
/// memory inside the `Datum` objects within `array` is also deep-copied.
static Datum copyArray(const DatumArrayRef&        array,
                       const Datum::AllocatorType& allocator);

/// Clone the elements in the specified int `map`.  Use the specified
/// `allocator` to allocate memory (if needed).  Any dynamically allocated
/// memory inside the `Datum` objects within `map` is also deep-copied.
static Datum copyIntMap(const DatumIntMapRef&       map,
                        const Datum::AllocatorType& allocator);

/// Clone the elements in the specified `map`.  Use the specified
/// `allocator` to allocate memory (if needed).  Any dynamically allocated
/// memory inside the `Datum` objects within `map` is also deep-copied.  The
/// keys in the elements within `map` are also cloned.
static Datum copyMapOwningKeys(const DatumMapRef&          map,
                               const Datum::AllocatorType& allocator);

/// Return a pointer to a `Datum` object if the specified `key` exists in
/// the specified `map` or 0 otherwise.  Find the key using binary search
/// and return the first match in case of multiple matches.
static const Datum *findElementBinary(const bslstl::StringRef& key,
                                      const DatumMapRef&       map);

/// Return a pointer to a `Datum` object if the specified `key` exists in
/// the specified `map` or 0 otherwise.  Find the key using linear search.
static const Datum *findElementLinear(const bslstl::StringRef& key,
                                      const DatumMapRef&       map);

                         // ========================
                         // class Datum_ArrayProctor
                         // ========================

/// This component-local mechanism class provides a specialized proctor
/// object that, upon destruction and unless the `release` method has been
/// called, destroys the elements in an array of `ELEMENT` objects and also
/// deallocates the memory allocated for the entire array.  The elements
/// destroyed are delimited by the "guarded" range `[ begin(), end() )`.
template <class ELEMENT>
class Datum_ArrayProctor {

  private:
    // DATA
    void        *d_base_p;     // base address of the array of 'Datum' objects

    bsl::size_t  d_size;       // number of bytes in the base allocation

    ELEMENT     *d_begin_p;    // starting address of the range in the array to
                               // be managed

    ELEMENT     *d_end_p;      // address of the first element beyond the last
                               // element in the range to be managed

    bool         d_released;   // flag indicating whether the object has been
                               // released explicitly

    Datum::AllocatorType
                 d_allocator;  // allocator used to manage memory for
                               // the array of 'Datum' objects

    // PRIVATE MANIPULATORS

    /// Destroy the contiguous sequence of `Datum` objects managed by this
    /// range proctor (if any) by invoking `Datum::destroy` on each
    /// (managed) object.  The memory allocated for the whole array is also
    /// deallocated.
    void destroy();
  private:
    // NOT IMPLEMENTED
    Datum_ArrayProctor(const Datum_ArrayProctor&);
    Datum_ArrayProctor& operator=(const Datum_ArrayProctor&);

  public:
    // CREATORS

    /// Create an array exception proctor object for the array of `Datum`
    /// objects at the specified `base` delimited by the range specified by
    /// `[ begin, end )`.  The memory inside the array is managed using the
    /// specified `allocator`.  The behavior is undefined unless
    /// `0 != base`, `0 != begin`, `0 != basicAllocator`, `begin <= end` (if
    /// `0 != end`), and each element in the range `[ begin, end )` has been
    /// initialized.
    Datum_ArrayProctor(void                        *base,
                       bsl::size_t                  size,
                       ELEMENT                     *begin,
                       ELEMENT                     *end,
                       const Datum::AllocatorType&  allocator);

    /// Destroy this range proctor along with the contiguous sequence of
    /// `Datum` objects it manages (if any) by invoking `Datum::destroy` on
    /// each (managed) object.  The memory allocated for the whole array is
    /// also deallocated.
    ~Datum_ArrayProctor();

    // MANIPULATORS

    /// Move the end pointer by the optionally specified `offset`, and
    /// return the new end pointer.
    ELEMENT *moveEnd(bsl::ptrdiff_t offset = 1);

    /// Set `d_released` flag to `true`, indicating that proctor shouldn't
    /// destroy managed objects on it's destruction.
    void release();
};

                         // ------------------------
                         // class Datum_ArrayProctor
                         // ------------------------

// CREATORS
template <class ELEMENT>
Datum_ArrayProctor<ELEMENT>::Datum_ArrayProctor(
                                        void                        *base,
                                        bsl::size_t                  size,
                                        ELEMENT                     *begin,
                                        ELEMENT                     *end,
                                        const Datum::AllocatorType&  allocator)
: d_base_p(base)
, d_size(size)
, d_begin_p(begin)
, d_end_p(end)
, d_released(false)
, d_allocator(allocator)
{
    BSLS_ASSERT(base);
    BSLS_ASSERT(begin);
    BSLS_ASSERT(!end || begin <= end);
}

template <class ELEMENT>
Datum_ArrayProctor<ELEMENT>::~Datum_ArrayProctor()
{
    if (!d_released) {
        // Proctor hasn't been released explicitly, so we need to destroy
        // managed objects and deallocate memory.

        destroy();
    }
}

// MANIPULATORS
template <class ELEMENT>
void Datum_ArrayProctor<ELEMENT>::destroy()
{
    BSLS_ASSERT(d_begin_p <= d_end_p);

    for (ELEMENT *ptr = d_begin_p; ptr < d_end_p; ++ptr) {
        Datum::destroy(*ptr, d_allocator);
    }

    bslma::AllocatorUtil::deallocateBytes(d_allocator, d_base_p, d_size);

}

template <>
void Datum_ArrayProctor<DatumMapEntry>::destroy()
{
    BSLS_ASSERT(d_begin_p <= d_end_p);

    for (DatumMapEntry *ptr = d_begin_p; ptr < d_end_p; ++ptr) {
        Datum::destroy(ptr->value(), d_allocator);
    }

    bslma::AllocatorUtil::deallocateBytes(d_allocator, d_base_p, d_size);
}

template <>
void Datum_ArrayProctor<DatumIntMapEntry>::destroy()
{
    BSLS_ASSERT(d_begin_p <= d_end_p);

    for (DatumIntMapEntry *ptr = d_begin_p; ptr < d_end_p; ++ptr) {
        Datum::destroy(ptr->value(), d_allocator);
    }

    bslma::AllocatorUtil::deallocateBytes(d_allocator, d_base_p, d_size);
}

template <class ELEMENT>
ELEMENT *Datum_ArrayProctor<ELEMENT>::moveEnd(bsl::ptrdiff_t offset)
{
    d_end_p += offset;
    return d_end_p;
}

template <class ELEMENT>
void Datum_ArrayProctor<ELEMENT>::release()
{
    d_released = true;
}

                          // =======================
                          // class Datum_CopyVisitor
                          // =======================

/// This component-local class provides a visitor to visit and copy value
/// within a `Datum` object and create a new `Datum` object out of it.  This
/// class implements "deep-copy" of `Datum` objects.  Note that this class
/// has implicit copy constructor and copy-assignment operator.
class Datum_CopyVisitor {

  private:
    // DATA
    Datum                *d_result_p;   // pointer where the newly created
                                        // 'Datum' object is to be stored (not
                                        // owned)

    Datum::AllocatorType  d_allocator;  // allocator used to allocate memory
                                        // for 'd_result_p' (if needed)

  public:
    // CREATORS

    /// Create a `Datum_CopyVisitor` object with the specified `result` and
    /// `basicAllocator`.
    Datum_CopyVisitor(Datum                       *result,
                      const Datum::AllocatorType&  allocator);

    // MANIPULATORS

    /// Create a `Datum` object using the specified `value` and copy it into
    /// `d_result_p`.
    void operator()(bslmf::Nil value);

    /// Create a `Datum` object using the specified `value` and copy it into
    /// `d_result_p`.
    void operator()(const bdlt::Date& value);

    /// Create a `Datum` object using the specified `value` and copy it into
    /// `d_result_p`.
    void operator()(const bdlt::Datetime& value);

    /// Create a `Datum` object using the specified `value` and copy it into
    /// `d_result_p`.
    void operator()(const bdlt::DatetimeInterval& value);

    /// Create a `Datum` object using the specified `value` and copy it into
    /// `d_result_p`.
    void operator()(const bdlt::Time& value);

    /// Create a `Datum` object using the specified `value` and copy it into
    /// `d_result_p`.
    void operator()(const bslstl::StringRef& value);

    /// Create a `Datum` object using the specified `value` and copy it into
    /// `d_result_p`.
    void operator()(bool value);

    /// Create a `Datum` object using the specified `value` and copy it into
    /// `d_result_p`.
    void operator()(bsls::Types::Int64 value);

    /// Create a `Datum` object using the specified `value` and copy it into
    /// `d_result_p`.
    void operator()(double value);

    /// Create a `Datum` object using the specified `value` and copy it into
    /// `d_result_p`.
    void operator()(const DatumError& value);

    /// Create a `Datum` object using the specified `value` and copy it into
    /// `d_result_p`.
    void operator()(int value);

    /// Create a `Datum` object using the specified `value` and copy it into
    /// `d_result_p`.
    void operator()(const DatumUdt& value);

    /// Create a `Datum` object using the specified `value` and copy it into
    /// `d_result_p`.
    void operator()(const DatumArrayRef& value);

    /// Create a `Datum` object using the specified `value` and copy it into
    /// `d_result_p`.
    void operator()(const DatumIntMapRef& value);

    /// Create a `Datum` object using the specified `value` and copy it into
    /// `d_result_p`.
    void operator()(const DatumMapRef& value);

    /// Create a `Datum` object using the specified `value` and copy it into
    /// `d_result_p`.
    void operator()(const DatumBinaryRef& value);

    /// Create a `Datum` object using the specified `value` and copy it into
    /// `d_result_p`.
    void operator()(bdldfp::Decimal64 value);
};

                          // -----------------------
                          // class Datum_CopyVisitor
                          // -----------------------

// CREATORS
Datum_CopyVisitor::Datum_CopyVisitor(Datum                       *result,
                                     const Datum::AllocatorType&  allocator)
: d_result_p(result)
, d_allocator(allocator)
{
}

// MANIPULATORS
void Datum_CopyVisitor::operator()(bslmf::Nil value)
{
    (void)value;
    *d_result_p = Datum::createNull();
}

void Datum_CopyVisitor::operator()(const bdlt::Date& value)
{
    *d_result_p = Datum::createDate(value);
}

void Datum_CopyVisitor::operator()(const bdlt::Datetime& value)
{
    *d_result_p = Datum::createDatetime(value, d_allocator);
}

void Datum_CopyVisitor::operator()(const bdlt::DatetimeInterval& value)
{
    *d_result_p = Datum::createDatetimeInterval(value, d_allocator);
}

void Datum_CopyVisitor::operator()(const bdlt::Time& value)
{
    *d_result_p = Datum::createTime(value);
}

void Datum_CopyVisitor::operator()(const bslstl::StringRef& value)
{
    *d_result_p = Datum::copyString(value.data(),
                                    value.length(),
                                    d_allocator);
}

void Datum_CopyVisitor::operator()(bool value)
{
    *d_result_p = Datum::createBoolean(value);
}

void Datum_CopyVisitor::operator()(bsls::Types::Int64 value)
{
    *d_result_p = Datum::createInteger64(value, d_allocator);
}

void Datum_CopyVisitor::operator()(double value)
{
    *d_result_p = Datum::createDouble(value);
}

void Datum_CopyVisitor::operator()(const DatumError& value)
{
    *d_result_p = Datum::createError(value.code(),
                                     value.message(),
                                     d_allocator);
}

void Datum_CopyVisitor::operator()(int value)
{
    *d_result_p = Datum::createInteger(value);
}

void Datum_CopyVisitor::operator()(const DatumUdt& value)
{
    *d_result_p = Datum::createUdt(value.data(), value.type());
}

void Datum_CopyVisitor::operator()(const DatumArrayRef& value)
{
    *d_result_p = copyArray(value, d_allocator);
}

void Datum_CopyVisitor::operator()(const DatumMapRef& value)
{
    *d_result_p = copyMapOwningKeys(value, d_allocator);
}

void Datum_CopyVisitor::operator()(const DatumIntMapRef& value)
{
    *d_result_p = copyIntMap(value, d_allocator);
}

void Datum_CopyVisitor::operator()(const DatumBinaryRef& value)
{
    *d_result_p = Datum::copyBinary(value.data(),
                                    value.size(),
                                    d_allocator);
}

void Datum_CopyVisitor::operator()(bdldfp::Decimal64 value)
{
    *d_result_p = Datum::createDecimal64(value, d_allocator);
}

                         // =========================
                         // class Datum_StreamVisitor
                         // =========================

/// This component-local class provides a visitor to visit and stream value
/// within an `Datum` object.  Note that this class has implicit copy-
/// constructor and copy-assignment operator.
class Datum_StreamVisitor {

  private:
    // DATA
    bsl::ostream& d_stream;         // reference to the output stream
    int           d_level;          // number of indentation levels
    int           d_spacesPerLevel; // number of spaces per indentation level

  public:
    // CREATORS

    /// Create a `StreamVisitor` object with the specified `stream` at the
    /// specified indentation `level`.  The specified `spacesPerLevel`
    /// defines the number of spaces per indentation level.
    explicit Datum_StreamVisitor(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel);


    // MANIPULATORS

    /// Write the specified `value` into `d_stream`.
    void operator()(bslmf::Nil value) const;

    /// Write the specified `value` into `d_stream`.
    void operator()(bool value) const;

    /// Write the specified `value` into `d_stream`.
    void operator()(const bslstl::StringRef& value) const;

    /// Write the specified `value` into `d_stream`.
    template <class BDLD_TYPE>
    void operator()(const BDLD_TYPE& value) const;
};

                         // -------------------------
                         // class Datum_StreamVisitor
                         // -------------------------
// CREATORS
Datum_StreamVisitor::Datum_StreamVisitor(bsl::ostream& stream,
                                         int           level,
                                         int           spacesPerLevel)
: d_stream(stream)
, d_level(level)
, d_spacesPerLevel(spacesPerLevel)
{
}

// MANIPULATORS
void Datum_StreamVisitor::operator()(bslmf::Nil value) const
{
    (void)value;
    if (!d_stream.bad()) {
        bdlb::Print::indent(d_stream, d_level, d_spacesPerLevel);
        d_stream << "nil";
        if (0 <= d_spacesPerLevel) {
            d_stream << '\n';
        }
    }
}

void Datum_StreamVisitor::operator()(bool value) const
{
    if (!d_stream.bad()) {
        bdlb::Print::indent(d_stream, d_level, d_spacesPerLevel);
        d_stream << (value ? "true" : "false");
        if (0 <= d_spacesPerLevel) {
            d_stream << '\n';
        }
    }
}

void Datum_StreamVisitor::operator()(const bslstl::StringRef& value) const
{
    if (!d_stream.bad()) {
        bdlb::Print::indent(d_stream, d_level, d_spacesPerLevel);
        d_stream << '"' << value << '"';
        if (0 <= d_spacesPerLevel) {
            d_stream << '\n';
        }
    }
}

template <class BDLD_TYPE>
void Datum_StreamVisitor::operator()(const BDLD_TYPE& value) const
{
    if (!d_stream.bad()) {
        bdlb::PrintMethods::print(d_stream, value, d_level, d_spacesPerLevel);
    }
}

// ============================================================================
//                              Utility Functions
// ----------------------------------------------------------------------------

bool compareIntLess(const DatumIntMapEntry& lhs, const DatumIntMapEntry& rhs)
{
    return lhs.key() < rhs.key();
}

bool compareLess(const DatumMapEntry& lhs, const DatumMapEntry& rhs)
{
    return lhs.key() < rhs.key();
}

static
Datum copyArray(const DatumArrayRef&        array,
                const Datum::AllocatorType& allocator)
{
    DatumMutableArrayRef ref;
    if (array.length()) {
        Datum::createUninitializedArray(&ref, array.length(), allocator);

        // Track the allocated memory and destroy it if any of the allocations
        // inside the for loop throws.
        Datum_ArrayProctor<Datum> proctor(ref.length(),
                                          (array.length() + 1) * sizeof(Datum),
                                          ref.data(),
                                          ref.data(),
                                          allocator);

        // Copy the new elements.
        for (DatumArrayRef::SizeType i = 0; i < array.length(); ++i) {
            ref.data()[i] = array[i].clone(allocator);
            proctor.moveEnd();
        }

        *ref.length() = array.length();
        proctor.release();
    }
    return Datum::adoptArray(ref);
}

static
Datum copyIntMap(const DatumIntMapRef&       map,
                 const Datum::AllocatorType& allocator)
{
    DatumMutableIntMapRef ref;

    if (map.size()) {

        Datum::createUninitializedIntMap(&ref,
                                         map.size(),
                                         allocator);

        // Track the allocated memory and destroy it if any of the allocations
        // inside the for loop throws.
        Datum_ArrayProctor<DatumIntMapEntry> proctor(
                                   ref.size(),
                                   (map.size() + 1) * sizeof(DatumIntMapEntry),
                                   ref.data(),
                                   ref.data(),
                                   allocator);
        // Copy the new elements.

        for (DatumMapRef::SizeType i = 0; i < map.size(); ++i) {
            ref.data()[i] =
                       DatumIntMapEntry(map[i].key(),
                                        map[i].value().clone(allocator));
            proctor.moveEnd();
        }

        *ref.size() += map.size();
        proctor.release();
    }

    return Datum::adoptIntMap(ref);
}

static
Datum copyMapOwningKeys(const DatumMapRef&          map,
                        const Datum::AllocatorType& allocator)
{
    DatumMutableMapOwningKeysRef ref;

    if (map.size()) {
        DatumMutableMapOwningKeysRef::SizeType totalSizeOfKeys = 0;
        for (DatumMapRef::SizeType i = 0; i < map.size(); ++i) {
            totalSizeOfKeys += map[i].key().length();
        }

        Datum::createUninitializedMap(&ref,
                                      map.size(),
                                      totalSizeOfKeys,
                                      allocator);

        // Track the allocated memory and destroy it if any of the allocations
        // inside the for loop throws.
        Datum_ArrayProctor<DatumMapEntry> proctor(
                    ref.size(),
                    ref.allocatedSize(),
                    ref.data(),
                    ref.data(),
                    allocator);

        // Copy the new elements.
        char *nextKeyPos = ref.keys();

        for (DatumMapRef::SizeType i = 0; i < map.size(); ++i) {
            bsl::memcpy(nextKeyPos,
                        map[i].key().data(),
                        map[i].key().length());
            bslstl::StringRef newKey(nextKeyPos,
                                     static_cast<int>(map[i].key().length()));
            ref.data()[i] =
                       DatumMapEntry(newKey,
                                     map[i].value().clone(allocator));
            nextKeyPos += map[i].key().length();
            proctor.moveEnd();
        }

        *ref.size() += map.size();
        proctor.release();
    }

    return Datum::adoptMap(ref);
}

static
const Datum *findElementBinary(int key, const DatumIntMapRef& map)
{
    // NOTE: dummy 'Datum' value is used for search.
    const DatumIntMapEntry *lower =
                              bsl::lower_bound(map.data(),
                                               map.data() + map.size(),
                                               DatumIntMapEntry(key, Datum()),
                                               compareIntLess);
    if (lower != (map.data() + map.size()) && lower->key() == key) {
        return &lower->value();                                       // RETURN
    }
    return 0;
}

static
const Datum *findElementLinear(int key, const DatumIntMapRef& map)
{
    for (DatumIntMapRef::SizeType i = 0; i < map.size(); ++i) {
        if (key == map[i].key()) {
            return &map[i].value();                                   // RETURN
        }
    }
    return 0;
}

static
const Datum *findElementBinary(const bslstl::StringRef& key,
                               const DatumMapRef&       map)
{
    // NOTE: dummy 'Datum' value is used for search.
    const DatumMapEntry *lower =
                              bsl::lower_bound(map.data(),
                                               map.data() + map.size(),
                                               DatumMapEntry(key, Datum()),
                                               compareLess);
    if (lower != (map.data() + map.size()) && lower->key() == key) {
        return &lower->value();                                       // RETURN
    }
    return 0;
}

static
const Datum *findElementLinear(const bslstl::StringRef& key,
                               const DatumMapRef&       map)
{
    for (DatumMapRef::SizeType i = 0; i < map.size(); ++i) {
        if (key == map[i].key()) {
            return &map[i].value();                                   // RETURN
        }
    }
    return 0;
}

}  // close unnamed namespace

// ============================================================================
//                               Implementation
// ----------------------------------------------------------------------------

namespace BloombergLP {
namespace bdld {

                                // -----------
                                // class Datum
                                // -----------

#ifdef BSLS_PLATFORM_CPU_32_BIT
// Sanity checks.
BSLMF_ASSERT(sizeof(Datum) == sizeof(double));
#else   // end - 32 bit / begin - 64 bit
BSLMF_ASSERT(sizeof(Datum) == 2 * sizeof(void *));
BSLMF_ASSERT(sizeof(bdlt::Datetime) <= sizeof(void*));
#endif  // end - 64 bit

// Platform independent sanity checks
BSLMF_ASSERT(sizeof(bdlt::Date) <= sizeof(int));
BSLMF_ASSERT(sizeof(bdlt::Time) <= sizeof(long long));
BSLMF_ASSERT(sizeof(Datum_MapHeader) <= sizeof(DatumMapEntry));

// CLASS METHODS
Datum Datum::createDecimal64(bdldfp::Decimal64    value,
                             const AllocatorType& allocator)
{
    using namespace bdldfp;

    Datum result;

#ifdef BSLS_PLATFORM_CPU_32_BIT
    result.d_exp.d_value = (k_DOUBLE_MASK | e_INTERNAL_EXTENDED)
                            << k_TYPE_MASK_BITS;

    switch (DecimalUtil::classify(value))
    {
      case FP_NAN:
        result.d_as.d_ushort = e_EXTENDED_INTERNAL_DECIMAL64_SPECIAL;
        result.d_as.d_int = e_DECIMAL64_SPECIAL_NAN;
        return result;                                                // RETURN
      case FP_INFINITE:
        result.d_as.d_ushort = e_EXTENDED_INTERNAL_DECIMAL64_SPECIAL;
        result.d_as.d_int = value > BDLDFP_DECIMAL_DD(0.)
            ? e_DECIMAL64_SPECIAL_INFINITY
            : e_DECIMAL64_SPECIAL_NEGATIVE_INFINITY;
        return result;                                                // RETURN
      default:
        unsigned char buffer[9];
        unsigned char* end = DecimalConvertUtil::
            decimal64ToVariableWidthEncoding(buffer, value);
        if (end - buffer <= static_cast<int>(sizeof(int))) {
            result.d_as.d_ushort = e_EXTENDED_INTERNAL_DECIMAL64;
            result.d_as.d_int = Datum_Helpers::load<int>(buffer, 0);
            return result;                                            // RETURN
        }
        else {
            void* mem = AllocUtil::newObject<bdldfp::Decimal64>(allocator,
                                                                value);
            return createExtendedDataObject(
                                           e_EXTENDED_INTERNAL_DECIMAL64_ALLOC,
                                           mem);
        }
    }
#else   // end - 32 bit / begin - 64 bit
    // Avoidance of compiler warning.  We don't need to allocate memory to
    // store Decimal64 values on 64-bit platform.
    (void)allocator;

    result.d_as.d_type = e_INTERNAL_DECIMAL64;
    new (result.theInlineStorage()) bdldfp::Decimal64(value);
    return result;
#endif  // end - 64 bit
}

Datum Datum::createError(int                      code,
                         const bslstl::StringRef& message,
                         const AllocatorType&     allocator)
{
    if (message.isEmpty()) {  // do not allocate memory if 'message' is empty
        return createError(code);                                     // RETURN
    }

    // Allocate extra bytes for length of the string and the error code to be
    // stored before the string.

    static const int align  = sizeof(int);
    const SizeType   length = static_cast<int>(message.length());

    // Allocate extra bytes (if needed) to make the allocated size a multiple
    // of 'align'.  See 'copyString' for more explanation.

    const SizeType  memlen = (length + align - 1) & ~(align - 1);
    void           *mem    = AllocUtil::allocateBytes(allocator,
                                                      memlen + 2 * sizeof(int),
                                                      align);

    *static_cast<int *>(mem) = code;
    *(static_cast<int *>(mem) + 1) = static_cast<int>(length);
    char *data = static_cast<char *>(mem) + 2 * sizeof(int);
    bsl::memcpy(data, message.data(), length);
#ifdef BSLS_PLATFORM_CPU_32_BIT
    return createExtendedDataObject(e_EXTENDED_INTERNAL_ERROR_ALLOC, mem);
#else   // end - 32 bit / begin - 64 bit
    return createDatum(e_INTERNAL_ERROR_ALLOC, mem);
#endif  // end - 64 bit
}

Datum Datum::copyBinary(const void            *value,
                        SizeType               size,
                        const AllocatorType&   allocator)
{
    BSLS_ASSERT(0 == size || value);

#ifdef BSLS_PLATFORM_CPU_32_BIT
    const SizeType sizeSize = sizeof(double);
    void *mem = AllocUtil::allocateBytes(allocator, sizeSize + size);
    new (mem) SizeType(size);
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(size)) {
        bsl::memcpy(reinterpret_cast<char *>(mem) + sizeSize, value, size);
    }
    return createExtendedDataObject(e_EXTENDED_INTERNAL_BINARY_ALLOC, mem);
#else   // end - 32 bit / begin - 64 bit
    Datum result;

    if (static_cast<unsigned>(size) <= k_SMALLBINARY_SIZE) {
        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(size)) {
            bsl::memcpy(result.d_data.buffer(), value, size);
        }
        else {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        }
        result.d_as.d_type = e_INTERNAL_BINARY;
        result.d_data.buffer()[k_SMALLBINARY_SIZE_OFFSET] =
                                                       static_cast<char>(size);
        return result;                                                // RETURN
    }

    BSLS_ASSERT(size <= static_cast<SizeType>(
                                             bsl::numeric_limits<int>::max()));
    size = static_cast<int>(size);

    result.d_as.d_type  = e_INTERNAL_BINARY_ALLOC;
    result.d_as.d_int32 = static_cast<int>(size);
    result.d_as.d_ptr   = AllocUtil::allocateBytes(allocator, size);
    bsl::memcpy(result.d_as.d_ptr, value, size);

    return result;
#endif  // end - 64 bit
}

Datum Datum::copyString(const char           *string,
                        SizeType              length,
                        const AllocatorType&  allocator)
{
    BSLS_ASSERT(string || 0 == length);

    Datum result;

#ifdef BSLS_PLATFORM_CPU_32_BIT
    result.d_double = 0;  // Zero the whole of 'result.d_data'.

    // Check for short string.
    if (length <= sizeof(result.d_string5.d_chars)) {
        result.d_string5.d_exponent = k_DOUBLE_MASK | e_INTERNAL_SHORTSTRING;
        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(length)) {
            bsl::memcpy(result.d_string5.d_chars, string, length);
        }
        result.d_string5.d_length = static_cast<char>(length);
        return result;                                                // RETURN
    }

    if (length == sizeof(result.d_string6.d_chars)) {
        result.d_string6.d_exponent
            = k_DOUBLE_MASK | e_INTERNAL_LONGEST_SHORTSTRING;
        bsl::memcpy(result.d_string6.d_chars, string, length);
        return result;                                                // RETURN
    }

    result.d_as.d_exponent = k_DOUBLE_MASK | e_INTERNAL_STRING;

    // Allocate extra bytes for length of the string to be stored before the
    // string itself.

    SizeType size = length + sizeof(SizeType);
    const SizeType align = sizeof(SizeType);

    // Allocate extra bytes (if needed) to make the allocated size a multiple
    // of 'align'.

    size = (size + align - 1) & ~(align - 1);  // mask off lower bits
    void *mem = AllocUtil::allocateBytes(allocator, size, align);

    *static_cast<SizeType *>(mem) = length;
    char *data = static_cast<char *>(mem) + sizeof(SizeType);

    bsl::memcpy(data, string, length);
    result.d_as.d_cvp = mem;
#else   // end - 32 bit / begin - 64 bit
    if (static_cast<unsigned>(length) <= k_SHORTSTRING_SIZE) {
        char *inlineString =
            reinterpret_cast<char *>(result.theInlineStorage());
        *inlineString++ = static_cast<char>(length);
        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(length)) {
            bsl::memcpy(inlineString, string, length);
        }
        result.d_as.d_type = e_INTERNAL_SHORTSTRING;
        return result;                                                // RETURN
    }

    BSLS_ASSERT(length <= bsl::numeric_limits<unsigned int>::max());

    result.d_as.d_type  = e_INTERNAL_STRING;
    result.d_as.d_int32 = static_cast<int>(length);
    result.d_as.d_ptr   = AllocUtil::allocateBytes(allocator, length);
    bsl::memcpy(result.d_as.d_ptr, string, length);
#endif  // end - 64 bit

    return result;
}

void Datum::createUninitializedArray(DatumMutableArrayRef *result,
                                     SizeType              capacity,
                                     const AllocatorType&  allocator)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(capacity < bsl::numeric_limits<SizeType>::max()/
                                                              sizeof(Datum)-1);

    // Allocate one extra element to store length of the array.

    void     *mem = AllocUtil::allocateBytes(allocator,
                                             sizeof(Datum) * (capacity + 1));
    SizeType *length = static_cast<SizeType *>(mem);

    // Store length of the array in the front.

    *length = 0;
    *result = DatumMutableArrayRef(static_cast<Datum *>(mem) + 1,
                                   length,
                                   capacity);
}

void Datum::createUninitializedMap(DatumMutableMapRef   *result,
                                   SizeType              capacity,
                                   const AllocatorType&  allocator)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(capacity < bsl::numeric_limits<SizeType>::max()/
                                                      sizeof(DatumMapEntry)-1);
    BSLMF_ASSERT(sizeof(DatumMapEntry) >= sizeof(Datum_MapHeader));

    // Allocate extra elements to store size of the map and a flag to determine
    // if the map is sorted or not.

    void *mem = AllocUtil::allocateBytes(
                                       allocator,
                                       sizeof(DatumMapEntry) * (capacity + 1));

    // Store map header in the front (1 DatumMapEntry).
    Datum_MapHeader *header = static_cast<Datum_MapHeader *>(mem);

    header->d_size          = 0;
    header->d_capacity      = capacity;
    header->d_allocatedSize = sizeof(DatumMapEntry) * (capacity + 1);
    header->d_sorted        = false;
    header->d_ownsKeys      = false;

    *result = DatumMutableMapRef(static_cast<DatumMapEntry *>(mem) + 1,
                                 &header->d_size,
                                 &header->d_sorted);
}

void Datum::createUninitializedIntMap(DatumMutableIntMapRef *result,
                                      SizeType               capacity,
                                      const AllocatorType&   allocator)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(capacity < bsl::numeric_limits<SizeType>::max() /
                                                 sizeof(DatumIntMapEntry) - 1);
    BSLMF_ASSERT(sizeof(DatumIntMapEntry) >= sizeof(Datum_IntMapHeader));

    // Allocate one extra element to store size of the map and a flag to
    // determine if the map is sorted or not.
    void * mem = AllocUtil::allocateBytes(
                                    allocator,
                                    sizeof(DatumIntMapEntry) * (capacity + 1));

    // Store map header in the front ( 1 DatumMapEntry ).
    Datum_IntMapHeader *header = static_cast<Datum_IntMapHeader *>(mem);

    header->d_size     = 0;
    header->d_capacity = capacity;
    header->d_sorted   = false;

    *result = DatumMutableIntMapRef(static_cast<DatumIntMapEntry *>(mem) + 1,
                                    &header->d_size,
                                    &header->d_sorted);
}

void Datum::createUninitializedMap(DatumMutableMapOwningKeysRef *result,
                                   SizeType                      capacity,
                                   SizeType                      keysCapacity,
                                   const AllocatorType&          allocator)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(capacity < (bsl::numeric_limits<SizeType>::max()-keysCapacity)/
                                                      sizeof(DatumMapEntry)-1);
    BSLMF_ASSERT(sizeof(DatumMapEntry) >= sizeof(Datum_MapHeader));

    // Allocate one extra element to store size of the map and a flag to
    // determine if the map is sorted or not.
    SizeType bufferSize =
        bsls::AlignmentUtil::roundUpToMaximalAlignment(
                        sizeof(DatumMapEntry) * (capacity + 1) + keysCapacity);
    void *mem = AllocUtil::allocateBytes(allocator, bufferSize);

    // Store map header in the front ( 1 DatumMapEntry ).
    Datum_MapHeader *header = static_cast<Datum_MapHeader *>(mem);

    header->d_size          = 0;
    header->d_capacity      = capacity;
    header->d_allocatedSize = bufferSize;
    header->d_sorted        = false;
    header->d_ownsKeys      = true;

    char *keysMem = static_cast<char *>(mem)
                                    + (sizeof(DatumMapEntry) * (capacity + 1));

    *result = DatumMutableMapOwningKeysRef(
                                         static_cast<DatumMapEntry *>(mem) + 1,
                                         &header->d_size,
                                         bufferSize,
                                         keysMem,
                                         &header->d_sorted);
}

char *Datum::createUninitializedString(Datum                *result,
                                       SizeType              length,
                                       const AllocatorType&  allocator)
{
    BSLS_ASSERT(result);

#ifdef BSLS_PLATFORM_CPU_32_BIT
    result->d_double = 0;  // zero the whole of 'result->d_data'

    // check for short string
    if (length <= sizeof(result->d_string5.d_chars)) {
        result->d_string5.d_exponent = k_DOUBLE_MASK | e_INTERNAL_SHORTSTRING;
        result->d_string5.d_length = static_cast<char>(length);
        return result->d_string5.d_chars;                             // RETURN
    }

    if (length == sizeof(result->d_string6.d_chars)) {
        result->d_string6.d_exponent
            = k_DOUBLE_MASK | e_INTERNAL_LONGEST_SHORTSTRING;
        return result->d_string6.d_chars;                             // RETURN
    }

    result->d_as.d_exponent = k_DOUBLE_MASK | e_INTERNAL_STRING;

    // allocate extra bytes for storing length of the string before the string
    // itself

    void *mem = AllocUtil::allocateBytes(allocator, sizeof(length) + length);
    Datum_Helpers::store(mem, 0, length);
    result->d_as.d_cvp = mem;

    return static_cast<char *>(mem) + sizeof(length);                 // RETURN
#else   // end - 32 bit / begin - 64 bit
    if (static_cast<unsigned>(length) <= k_SHORTSTRING_SIZE) {
        char *str = reinterpret_cast<char *>(result->theInlineStorage());
        *str++ = static_cast<char>(length);
        result->d_as.d_type = e_INTERNAL_SHORTSTRING;
        return str;                                                   // RETURN
    }

    BSLS_ASSERT(length <= bsl::numeric_limits<unsigned int>::max());

    result->d_as.d_type = e_INTERNAL_STRING;
    result->d_as.d_int32 = static_cast<int>(length);
    result->d_as.d_ptr = AllocUtil::allocateBytes(allocator, length);
    return static_cast<char *>(result->d_as.d_ptr);
#endif  // end - 64 bit
}

const char *Datum::dataTypeToAscii(DataType type)
{
#define CASE(X) case(e_##X): return #X;
    switch (type) {
        CASE(NIL);
        CASE(INTEGER);
        CASE(DOUBLE);
        CASE(STRING);
        CASE(BOOLEAN);
        CASE(ERROR);
        CASE(DATE);
        CASE(TIME);
        CASE(DATETIME);
        CASE(DATETIME_INTERVAL);
        CASE(INTEGER64);
        CASE(USERDEFINED);
        CASE(ARRAY);
        CASE(MAP);
        CASE(INT_MAP);
        CASE(BINARY);
        CASE(DECIMAL64);
        default: return "(* UNKNOWN *)";
    }
#undef CASE
}

void Datum::destroy(const Datum& value, const AllocatorType& allocator)
{
    const InternalDataType type = value.internalType();
#ifdef BSLS_PLATFORM_CPU_32_BIT
    switch (type) {
      case e_INTERNAL_EXTENDED: {
        const ExtendedInternalDataType extendedType =
                                                  value.extendedInternalType();
        switch (extendedType) {
          case e_EXTENDED_INTERNAL_MAP          :
            BSLS_ANNOTATION_FALLTHROUGH;
          case e_EXTENDED_INTERNAL_OWNED_MAP    : {
            DatumMapRef values = value.theMap();
            for (SizeType i = 0; i < values.size(); ++i) {
                destroy(values[i].value(), allocator);
            }
            value.safeDeallocateBytes(allocator, value.theMapAllocNumBytes());
          } break;
          case e_EXTENDED_INTERNAL_INT_MAP: {
            DatumIntMapRef values = value.theIntMap();
            for (SizeType i = 0; i < values.size(); ++i) {
                destroy(values[i].value(), allocator);
            }
            value.safeDeallocateBytes(allocator,
                                      value.theIntMapAllocNumBytes());
          } break;
          case e_EXTENDED_INTERNAL_ERROR_ALLOC: {
            value.safeDeallocateBytes(allocator,
                                      value.theErrorAllocNumBytes(),
                                      sizeof(int));
          } break;
          case e_EXTENDED_INTERNAL_SREF_ALLOC: {
            value.safeDeallocateBytes(allocator,
                                      sizeof(SizeType) + sizeof(const char*));
          } break;
          case e_EXTENDED_INTERNAL_AREF_ALLOC: {
            value.safeDeallocateBytes(allocator,
                                      sizeof(SizeType) + sizeof(Datum *));
          } break;
          case e_EXTENDED_INTERNAL_DATETIME_ALLOC: {
            AllocUtil::deallocateObject(allocator,
                                        value.allocatedPtr<bdlt::Datetime>());

          } break;
          case e_EXTENDED_INTERNAL_DATETIME_INTERVAL_ALLOC: {
            AllocUtil::deallocateObject(
                                 allocator,
                                 value.allocatedPtr<bdlt::DatetimeInterval>());
          } break;
          case e_EXTENDED_INTERNAL_INTEGER64_ALLOC: {
            AllocUtil::deallocateObject(
                                     allocator,
                                     value.allocatedPtr<bsls::Types::Int64>());
          } break;
          case e_EXTENDED_INTERNAL_BINARY_ALLOC: {
            value.safeDeallocateBytes(allocator,
                                      value.theBinaryAllocNumBytes());
          } break;
          case e_EXTENDED_INTERNAL_DECIMAL64_ALLOC: {
            AllocUtil::deallocateObject(
                                      allocator,
                                      value.allocatedPtr<bdldfp::Decimal64>());
          } break;
          default: {
            // Other enumerators require no destruction.
          } break;
        }
      } break;
      case e_INTERNAL_ARRAY: {
        DatumArrayRef values = value.theArray();
        for (SizeType i = 0; i < values.length(); ++i) {
            destroy(values[i], allocator);
        }
        value.safeDeallocateBytes(allocator,
                                  value.theInternalArrayAllocNumBytes());
      } break;
      case e_INTERNAL_STRING: {
        value.safeDeallocateBytes(allocator,
                                  value.theInternalStringAllocNumBytes(),
                                  sizeof(SizeType));
      } break;
      default: {
        // Other enumerators require no destruction.
      } break;
    }
#else   // end - 32 bit / begin - 64 bit
    switch (type) {
      case e_INTERNAL_MAP      :
        BSLS_ANNOTATION_FALLTHROUGH;
      case e_INTERNAL_OWNED_MAP: {
        DatumMapRef values = value.theMap();
        for (SizeType i = 0; i < values.size(); ++i) {
            destroy(values[i].value(), allocator);
        }
        value.safeDeallocateBytes(allocator, value.theMapAllocNumBytes());
      } break;
      case e_INTERNAL_INT_MAP: {
        DatumIntMapRef values = value.theIntMap();
        for (SizeType i = 0; i < values.size(); ++i) {
            destroy(values[i].value(), allocator);
        }
        value.safeDeallocateBytes(allocator, value.theIntMapAllocNumBytes());
      } break;
      case e_INTERNAL_ARRAY: {
        DatumArrayRef values = value.theArray();
        for (SizeType i = 0; i < values.length(); ++i) {
            destroy(values[i], allocator);
        }
        value.safeDeallocateBytes(allocator,
                                  value.theInternalArrayAllocNumBytes());
      } break;
      case e_INTERNAL_BINARY_ALLOC: {
        value.safeDeallocateBytes(allocator, value.theBinaryAllocNumBytes());
      } break;
      case e_INTERNAL_ERROR_ALLOC: {
        value.safeDeallocateBytes(allocator, value.theErrorAllocNumBytes());
      } break;
      case e_INTERNAL_STRING: {
        value.safeDeallocateBytes(allocator,
                                  value.theInternalStringAllocNumBytes());
      } break;
      default: {
      } break; // Other enumerators require no special handling.
    }
#endif  // end - 64 bit
}

// ACCESSORS
Datum Datum::clone(const AllocatorType& allocator) const
{
    Datum result;

    Datum_CopyVisitor cv(&result, allocator);
    apply(cv);

    return result;
}

bdldfp::Decimal64 Datum::theDecimal64() const
{
    BSLS_ASSERT(isDecimal64());
#ifdef BSLS_PLATFORM_CPU_32_BIT
    using namespace bdldfp;

    switch (
         BSLS_PERFORMANCEHINT_PREDICT_EXPECT(
             extendedInternalType(),
             e_EXTENDED_INTERNAL_DECIMAL64)) {
      case e_EXTENDED_INTERNAL_DECIMAL64: {
          Decimal64 value;
          DecimalConvertUtil::decimal64FromVariableWidthEncoding(
                         &value,
                         reinterpret_cast<const unsigned char *>(&d_as.d_int));
          return value;                                               // RETURN
      }
      case e_EXTENDED_INTERNAL_DECIMAL64_SPECIAL: {
          BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
          switch (d_as.d_int) {
            case e_DECIMAL64_SPECIAL_NAN:
                return bsl::numeric_limits<Decimal64>::quiet_NaN();   // RETURN
            case e_DECIMAL64_SPECIAL_INFINITY:
                return bsl::numeric_limits<Decimal64>::infinity();    // RETURN
            case e_DECIMAL64_SPECIAL_NEGATIVE_INFINITY:
                return -bsl::numeric_limits<Decimal64>::infinity();   // RETURN
          }
      } break;
      case e_EXTENDED_INTERNAL_DECIMAL64_ALLOC: {
          BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
          return *allocatedPtr<const Decimal64>();                    // RETURN
      } break;
      default: {
      } break;
    }

    return Decimal64(); // silence compiler warning
#else   // end - 32 bit / begin - 64 bit
    return *reinterpret_cast<const bdldfp::Decimal64 *>(theInlineStorage());
#endif  // end - 64 bit
}

bsl::ostream& Datum::print(bsl::ostream& stream,
                           int           level,
                           int           spacesPerLevel) const
{
    Datum_StreamVisitor sv(stream, level, spacesPerLevel);
    apply(sv);
    return stream << bsl::flush;
}

                         // -------------------
                         // class DatumArrayRef
                         // -------------------

// ACCESSORS
bsl::ostream& DatumArrayRef::print(bsl::ostream& stream,
                                   int           level,
                                   int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    for (SizeType i = 0; i < d_length; ++i) {
        printer.printValue(d_data_p[i]);
    }

    printer.end();

    return stream << bsl::flush;
}

                          // ----------------------
                          // class DatumIntMapEntry
                          // ----------------------

// ACCESSORS
bsl::ostream& DatumIntMapEntry::print(bsl::ostream& stream,
                                      int           level,
                                      int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    bsl::ostringstream os;
    os << d_key;

    printer.printAttribute(os.str().c_str(), d_value);
    printer.end();

    return stream << bsl::flush;
}

                            // -------------------
                            // class DatumMapEntry
                            // -------------------

// ACCESSORS
bsl::ostream& DatumMapEntry::print(bsl::ostream& stream,
                                   int           level,
                                   int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute(d_key_p, d_value);
    printer.end();

    return stream << bsl::flush;
}

                          // -----------------
                          // class DatumMapRef
                          // -----------------
// ACCESSORS
const Datum *DatumIntMapRef::find(int key) const
{
    return d_sorted ? findElementBinary(key, *this) :
                      findElementLinear(key, *this);
}

const Datum *DatumMapRef::find(const bslstl::StringRef& key) const
{
    return d_sorted ? findElementBinary(key, *this) :
                      findElementLinear(key, *this);
}

bsl::ostream& DatumMapRef::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    for (bsls::Types::size_type i = 0; i < d_size; ++i) {
        printer.printValue(d_data_p[i]);
    }

    printer.end();

    return stream << bsl::flush;
}

bsl::ostream& DatumIntMapRef::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    for (bsls::Types::size_type i = 0; i < d_size; ++i) {
        printer.printValue(d_data_p[i]);
    }

    printer.end();

    return stream << bsl::flush;
}

}  // close package namespace

// FREE OPERATORS
bool bdld::operator==(const Datum& lhs, const Datum& rhs)
{
    const Datum::DataType type = lhs.type();

    if (type != rhs.type()) {
        return false;                                                 // RETURN
    }

    switch (type) {
      case Datum::e_DOUBLE:
        return (lhs.theDouble() == rhs.theDouble());                  // RETURN
      case Datum::e_STRING: {
        bslstl::StringRef            lhsStr = lhs.theString();
        bslstl::StringRef            rhsStr = rhs.theString();
        const bsls::Types::size_type length = lhsStr.length();
        return length == rhsStr.length() &&
               !bsl::memcmp(lhsStr.data(), rhsStr.data(), length);    // RETURN
      }
      case Datum::e_INTEGER:
        return (lhs.theInteger() == rhs.theInteger());                // RETURN
      case Datum::e_BOOLEAN:
        return (lhs.theBoolean() == rhs.theBoolean());                // RETURN
      case Datum::e_NIL:
        return true;                                                  // RETURN
      case Datum::e_ERROR:
        return (lhs.theError() == rhs.theError());                    // RETURN
      case Datum::e_DATE:
        return (lhs.theDate() == rhs.theDate());                      // RETURN
      case Datum::e_TIME:
        return (lhs.theTime() == rhs.theTime());                      // RETURN
      case Datum::e_DATETIME:
        return (lhs.theDatetime() == rhs.theDatetime());              // RETURN
      case Datum::e_DATETIME_INTERVAL:
        return (lhs.theDatetimeInterval() == rhs.theDatetimeInterval());
                                                                      // RETURN
      case Datum::e_INTEGER64:
        return (lhs.theInteger64() == rhs.theInteger64());            // RETURN
      case Datum::e_BINARY:
        return (lhs.theBinary() == rhs.theBinary());                  // RETURN
      case Datum::e_DECIMAL64:
        return (lhs.theDecimal64() == rhs.theDecimal64());            // RETURN
      case Datum::e_ARRAY: {
        DatumArrayRef lval = lhs.theArray();
        DatumArrayRef rval = rhs.theArray();
        return (lval.length() == rval.length())
                    ? ((lval.length() == 0)
                          ? true
                          : bsl::equal(lval.data(),
                                       lval.data() + lval.length(),
                                       rval.data()))
                    : false;                                          // RETURN
        }
      case Datum::e_MAP:
        return (lhs.theMap() == rhs.theMap());                        // RETURN
      case Datum::e_INT_MAP:
        return (lhs.theIntMap() == rhs.theIntMap());                  // RETURN
      case Datum::e_USERDEFINED:
        return (lhs.theUdt() == rhs.theUdt());                        // RETURN
      default:
        return false;                                                 // RETURN
    }
}

bsl::ostream& bdld::operator<<(bsl::ostream& stream, Datum::DataType rhs)
{
    return stream << Datum::dataTypeToAscii(rhs);
}


bool bdld::operator==(const DatumArrayRef& lhs, const DatumArrayRef& rhs)
{
    if (lhs.length() == rhs.length()) {
        for (DatumArrayRef::SizeType i = 0; i < lhs.length(); ++i) {
            if (lhs[i] != rhs[i]) {
                return false;                                         // RETURN
            }
        }
        return true;                                                  // RETURN
    }
    return false;
}

bool bdld::operator==(const DatumMapRef& lhs, const DatumMapRef& rhs)
{
    if (lhs.size() == rhs.size()) {
        for (DatumMapRef::SizeType i = 0; i < lhs.size(); ++i) {
            if (lhs[i] != rhs[i]) {
                return false;                                         // RETURN
            }
        }
        return true;                                                  // RETURN
    }
    return false;
}

bool bdld::operator==(const DatumIntMapRef& lhs, const DatumIntMapRef& rhs)
{
    if (lhs.size() == rhs.size()) {
        for (DatumIntMapRef::SizeType i = 0; i < lhs.size(); ++i) {
            if (lhs[i] != rhs[i]) {
                return false;                                         // RETURN
            }
        }
        return true;                                                  // RETURN
    }
    return false;
}

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
