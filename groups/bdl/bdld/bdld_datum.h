// bdld_datum.h                                                       -*-C++-*-

#ifndef INCLUDED_BDLD_DATUM
#define INCLUDED_BDLD_DATUM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide a discriminated variant type with a small footprint.
//
//@CLASSES:
//   bdld::Datum: POD type representing general-purpose values
//   bdld::ConstDatumArrayRef: type for const ref to array of datums
//   bdld::ConstDatumMapRef: type for const ref to map of datums
//   bdld::DatumMapEntry: type for entry inside map of datums
//   bdld::ConstDatumBinaryRef: type for const ref to binary value
//
//@SEE ALSO: bdld_error, bdld_datumarrayref, bdld_datummapref,
//           bdld_datummapowningkeysref, bdld_udt
//
//@DESCRIPTION: This component provides a type that can hold a value of one of
// several "vocabulary" types: 'bool', 'int', 'Int64', 'double', 'Decimal64',
// string, 'StringRef', 'Date', 'Time', 'Datetime', 'DatetimeInterval', error
// code with an optional message, array or map reference, binary data. The
// value can also be the singular value 'null'. The class is implemented as a
// POD type, such that instances of the class are bitwise copyable and have
// trivial initialization, assignment and destruction, which makes it efficient
// for creating and copying large numbers of such values.  The class is also
// designed to be compact, especially on a 32-bit platform, which also makes it
// suitable for representing large numbers of such values.  However, because
// not all representable values can be stored in-line in the instance, the
// class requires explicit management of memory associated with the value; see
// the usage examples for more details.
//
///Usage
///-----
///Example 1: Datum Basics
///-----------------------
// The following examples illustrate the construction, manipulation and
// lifecycle of datums.
//
// Datums are created via a set of static methods called 'createTYPE',
// 'copyTYPE' or 'adoptTYPE' where TYPE is any of the supported
// types. The creation methods take a value and sometimes an allocator.
// ..
//  Datum cityName = Datum::copyString("Springfield",
//                                     strlen("Springfield"),
//                                     alloc);
//..
// Here a copy of the string is made. Whether the copy is stored right
// inside the datum, or in memory obtained from the allocator
// depends on the length of the string and the platform.
// The type of the value can be queried using the 'isTYPE' methods The
// value is retrieved via the 'theTYPE' methods:
//..
//  ASSERT(cityName.isString());
//  ASSERT(cityName.theString() == "Springfield");
//..
// As a convenience, datums can be written to standard streams:
//..
//  cout << cityName << "\n";
//..
// The form of the output is not specified and may change at any time. Do not
// use in production code.
//..
// The static method 'destroy' releases the memory obtained from the
// allocator, if any. It is not mandatory to call 'destroy', if the
// memory possibly allocated during 'Datum' creation is reclaimed by
// other means (e.g. by destroying the allocator - see below):
//..
//  Datum::destroy(cityName, alloc);
//..
///
///Example 2: Arrays
///-----------------
//
// The following example illustrates the construction of an array of
// datums. First we create an array of datums:
//..
//  DatumArrayRef bartArray;
//  Datum::createUninitializedArray(&bartArray, 3, alloc);
//  bartArray.data()[0] = Datum::createStringRef("Bart", alloc);
//  bartArray.data()[1] = Datum::createStringRef("Simpson", alloc);
//  bartArray.data()[2] = Datum::createInteger(10);
//  *bartArray.length() = 3;
//..
// Then, we construct the Datum that holds the array itself:
//..
//  Datum bart = Datum::adoptArray(bartArray);
//..
// Note that after the 'bartArray' has been adopted, the 'bartArray' object can
// be destroyed without invalidating the array contained in the datum.
//
// A DatumArray may be adopted by only one datum. If the DatumArray is not
// adopted, it must be destroyed via 'disposeUnitializedArray'.
//
// Now, we can access the contents of the array through the datum:
//..
//  cout << bart.theArray().length() << "\n"; // 3
//  cout << bart.theArray()[0] << "\n";       // "Bart"
//  cout << bart << "\n";                     // [ "Bart" "Simpson" 10 ]
//..
// Finally, we destroy the datum, which releases all memory associated
// with the array.
//..
//  Datum::destroy(bart, alloc);
//..
// Note that the same allocator must be used to create the array, the
// elements, and to destroy the datum.
//
///Example 3: Maps
///---------------
// The following example illustrates the construction of a map of
// datums indexed by string keys. First we create a map of datums:
//..
//  DatumMapRef lisaMap;
//  Datum::createUninitializedMap(&lisaMap, 3, alloc);
//  lisaMap.data()[0] = DatumMapEntry(StringRef("firstName"),
//                                    Datum::createStringRef("Lisa",
//                                                           alloc));
//  lisaMap.data()[1] = DatumMapEntry(StringRef("lastName"),
//                                    Datum::createStringRef("Simpson",
//                                                           alloc));
//  lisaMap.data()[2] = DatumMapEntry(StringRef("age"),
//                                    Datum::createInteger(8));
//  *lisaMap.size() = 3;
//..
// Then, we construct the Datum that holds the map itself:
//..
//  Datum lisa = Datum::adoptMap(lisaMap);
//..
// Note that after the 'lisaMap' has been adopted, the 'lisaMap' object can be
// destroyed without invalidating the map contained in the datum.
//
// A DatumMapRef may be adopted by only one datum. If the DatumMapRef is not
// adopted, it must be destroyed via 'disposeUninitializedMap'.
//
// Now, we can access the contents of the map through the datum:
//
//..
//  cout << lisa.theMap().size() << "\n";             // 3
//  cout << *lisa.theMap().find("firstName") << "\n"; // "Lisa"
//..
// Finally, we destroy the datum, which releases all memory associated
// with the array
//..
//  Datum::destroy(lisa, alloc);
//..
// Note that the same allocator must be used to create the map, the
// elements, and to destroy the datum.
///
///Example 4: Mass Destruction
///---------------------------
//
// The following example illustrates an important idiom: the en masse
// destruction of a series of datums allocated in an arena:
//..
//  {
//      // scope
//      bsls::AlignedBuffer<200> bufferStorage;
//      bdema_BufferedSequentialAllocator arena(bufferStorage.buffer(), 200);
//
//      Datum patty = Datum::copyString("Patty Bouvier",
//                                      strlen("Patty Bouvier"),
//                                      &arena);
//
//      Datum selma = Datum::copyString("Selma Bouvier",
//                                      strlen("Selma Bouvier"),
//                                      &arena);
//      DatumArrayRef maggieArray;
//      Datum::createUninitializedArray(&maggieArray, 2, &arena);
//      maggieArray.data()[0] = Datum::createStringRef("Maggie", &arena);
//      maggieArray.data()[1] = Datum::createStringRef("Simpson", &arena);
//      *maggieArray.length() = 2;
//      Datum maggie = Datum::adoptArray(maggieArray);
//  } // end of scope
//..
// Here all the allocated memory is lodged in the 'arena' allocator. At the end
// of the scope the memory is freed in a single step. Calling 'destroy' for
// each datum individually is neither necessary nor permitted.
//
///Example 5: User-defined, error and binary types
///-----------------------------------------------
// A datum can contain a user-defined type (Udt), consisting in a pointer and
// an integer. The pointer is held, not owned:
//..
//  struct Beer { };
//  struct Donut { };
//  Beer duff;
//  enum Type { e_BEER, e_DONUT };
//  Datum treat = Datum::createUdt(&duff, e_BEER);
//  ASSERT(treat.isUdt());
//  Udt content = treat.theUdt();
//  ASSERT(content.type() == e_BEER);
//  ASSERT(content.data() == &duff);
//..
// A datum can contain an error, consisting in a code and an optional
// StringRef:
//..
//  enum { e_NO_MORE_BEER };
//  Datum error = Datum::createError(e_NO_MORE_BEER, "doh!", alloc);
//  ASSERT(error.isError());
//  Error what = error.theError();
//  ASSERT(what.code() == e_NO_MORE_BEER);
//  ASSERT(what.message() == "doh!");
//  Datum::destroy(error, alloc);
//..
// A datum can contain an arbitrary sequence of bytes:
//..
//  int array[] = { 1, 2, 3 };
//  Datum bob = Datum::copyBinary(array, sizeof array, alloc);
//  array[2] = 666;
//  ASSERT(bob.isBinary());
//  ConstDatumBinaryRef stuff = bob.theBinary();
//  ASSERT(stuff.size() == 3 * sizeof(int));
//  ASSERT(reinterpret_cast<const int*>(stuff.data())[2] == 3);
//  Datum::destroy(bob, alloc);
//..
// Note that the bytes have been copied.
///

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLT_DATE
#include <bdlt_date.h>
#endif

#ifndef INCLUDED_BDLT_TIME
#include <bdlt_time.h>
#endif

#ifndef INCLUDED_BDLT_DATETIME
#include <bdlt_datetime.h>
#endif

#ifndef INCLUDED_BDLT_DATETIMEINTERVAL
#include <bdlt_datetimeinterval.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMAL
#include <bdldfp_decimal.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMF_NIL
#include <bslmf_nil.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNEDBUFFER
#include <bsls_alignedbuffer.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_LIMITS
#include <bsl_limits.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_UTILITY
#include <bsl_utility.h>
#endif

#if !defined(BSLS_PLATFORM_CPU_32_BIT) && !defined(BSLS_PLATFORM_CPU_64_BIT)
#error 'bdld::Datum' supports 32- or 64-bit platforms only.
BSLS_PLATFORM_COMPILER_ERROR;
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC)
#define BDLD_DATUM_DEPRECATED(DECL) __declspec(deprecated) DECL
#define BDLD_DATUM_FORCE_INLINE __forceinline
#elif defined(BSLS_PLATFORM_CMP_GNU)
#define BDLD_DATUM_DEPRECATED(DECL) DECL __attribute__ ((deprecated))
#define BDLD_DATUM_FORCE_INLINE inline
#else
#define BDLD_DATUM_DEPRECATED(DECL) DECL
#define BDLD_DATUM_FORCE_INLINE inline
#endif

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace bdld {

class DatumArrayRef;
class DatumMapEntry;
class DatumMapOwningKeysRef;
class DatumMapRef;
class Error;
class Udt;
class ConstDatumArrayRef;
class ConstDatumMapRef;
class ConstDatumBinaryRef;

                                // ===========
                                // class Datum
                                // ===========

class Datum {
    // This class implements an efficient *in-core* *value-semantic*
    // representation of general-purpose values and their aggregates.  The size
    // of 'Datum' is 8 bytes (same as a 'double') on 32-bit platforms and 16
    // bytes on 64-bit platforms.  On 32-bit platforms, it stores values inside
    // an 8 byte unsigned char array ('d_data').  Any 'double' value (including
    // NaN and infinity values) can be stored inside 'Datum'.  When storing a
    // value of a type other than 'double', the bits in 'd_data' that
    // correspond to the exponent part of a 'double' value are set to 1, with
    // the 4 bits in the fraction part used to indicate the type of value
    // stored.  Note that this approach will not work on 64-bit platform
    // because pointers are 8 bytes instead of 4 bytes in size.  We have a
    // separate implementation for 64-bit platform, which uses a 16 byte
    // unsigned char array ('d_data') to store values.  datums are
    // bitwise copyable and have trivial initialization, assignment and
    // destruction.  Note that only one of the copies of the same 'Datum'
    // object can be passed to 'destroy.  The rest of those copies then become
    // invalid and it is undefined behavior to deep-copy or destroy them.
    // Although, these copies can be used on the left hand side of assignment.
    // When copying a datum that has an array of datums, the
    // values in the array are always cloned.  When copying a datum
    // that has a map of datums, the values in the map are always
    // cloned, but the keys are cloned only if the source datum has a
    // map that owns the keys.

  public:
    // TYPES
    enum DataType {
        // Enumeration used to discriminate among the different externally-
        // exposed types of values that can be stored inside 'Datum'.

          e_NIL                  =  0  // null value
        , e_INTEGER              =  1  // integer value
        , e_REAL                 =  2  // double value
        , e_STRING               =  3  // string value
        , e_BOOLEAN              =  4  // boolean value
        , e_ERROR_VALUE          =  5  // error value
        , e_DATE                 =  6  // date value
        , e_TIME                 =  7  // time value
        , e_DATETIME             =  8  // date+time value
        , e_DATETIME_INTERVAL    =  9  // date+time interval value
        , e_INTEGER64            = 10  // 64-bit integer value
        , e_USERDEFINED          = 11  // pointer to a user-defined object
        , e_ARRAY                = 12  // array
        , e_MAP                  = 13  // map keyed by string values
        , e_BINARY               = 14  // binary data
        , e_DECIMAL64            = 15  // Decimal64
        , k_NUM_TYPES            = 16  // number of distinct enumerated types

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        , BDLD_NIL               = e_NIL
        , BDLD_INTEGER           = e_INTEGER
        , BDLD_REAL              = e_REAL
        , BDLD_STRING            = e_STRING
        , BDLD_BOOLEAN           = e_BOOLEAN
        , BDLD_ERROR_VALUE       = e_ERROR_VALUE
        , BDLD_DATE              = e_DATE
        , BDLD_TIME              = e_TIME
        , BDLD_DATETIME          = e_DATETIME
        , BDLD_DATETIME_INTERVAL = e_DATETIME_INTERVAL
        , BDLD_INTEGER64         = e_INTEGER64
        , BDLD_USERDEFINED       = e_USERDEFINED
        , BDLD_ARRAY             = e_ARRAY
        , BDLD_MAP               = e_MAP
        , BDLD_BINARY            = e_BINARY
        , BDLD_DECIMAL64         = e_DECIMAL64
        , BDLD_NUM_TYPES         = k_NUM_TYPES
#endif
    };

#if defined(BSLS_PLATFORM_CPU_32_BIT)

  private:
    // PRIVATE TYPES
    // 32-bit variation
    enum InternalDataType {
        // Enumeration used to discriminate among the different types of
        // values that can be stored inside 'Datum'.

          e_INTERNAL_INF                 =  0  // +/- infinity value
        , e_INTERNAL_LONGEST_SHORTSTRING =  1  // 6 character string
        , e_INTERNAL_BOOLEAN             =  2  // boolean value
        , e_INTERNAL_SHORTSTRING         =  3  // short string value
        , e_INTERNAL_STRING              =  4  // string value
        , e_INTERNAL_DATE                =  5  // date value
        , e_INTERNAL_TIME                =  6  // time value
        , e_INTERNAL_DATETIME            =  7  // date+time value
        , e_INTERNAL_DATETIME_INTERVAL   =  8  // date+time interval value
        , e_INTERNAL_INTEGER             =  9  // integer value
        , e_INTERNAL_INTEGER64           = 10  // 64-bit integer value
        , e_INTERNAL_USERDEFINED         = 11  // pointer to a user-defined
                                               // object datums
        , e_INTERNAL_ARRAY               = 12  // array of datums
        , e_INTERNAL_STRING_REFERENCE    = 13  // unowned string
        , e_INTERNAL_ARRAY_REFERENCE     = 14  // unowned array of
        , e_INTERNAL_EXTENDED            = 15  // other types that cannot be
                                               // discriminated using the
                                               // existing 4 bit discriminator
        , e_INTERNAL_REAL                = 16  // double value
        , k_NUM_INTERNAL_TYPES           = 17  // number of distinct
                                               // enumerated types
    };

    enum ExtendedInternalDataType {
        // Enumeration used to discriminate among different types of values
        // that map on to the 'e_INTERNAL_EXTENDED' discriminator value
        // inside 'Datum'.  It is used to add any new required types.

          e_EXTENDED_INTERNAL_MAP               = 0  // map of datums
                                                     // keyed by string values
                                                     // that are not owned
        , e_EXTENDED_INTERNAL_OWNED_MAP         = 1  // map of datums
                                                     // keyed by string values
                                                     // that are owned
        , e_EXTENDED_INTERNAL_NAN2              = 2  // NaN double value
        , e_EXTENDED_INTERNAL_ERROR             = 3  // just error code
        , e_EXTENDED_INTERNAL_ERROR_ALLOC       = 4  // error code and
                                                     // allocated

        // We never need to externally allocate the reference types with the
        // 64-bit implementation because we can fit 32 bits of length inline.

        , e_EXTENDED_INTERNAL_SREF_ALLOC        = 5  // allocated string ref
        , e_EXTENDED_INTERNAL_AREF_ALLOC        = 6  // allocated array ref
        , e_EXTENDED_INTERNAL_DATETIME_ALLOC    = 7  // allocated datetime
        , e_EXTENDED_INTERNAL_DATETIME_INTERVAL_ALLOC = 8
                                                     // allocated datetime
                                                     // interval
        , e_EXTENDED_INTERNAL_INTEGER64_ALLOC = 9    // 64-bit integer value
        , e_EXTENDED_INTERNAL_BINARY_ALLOC = 10      // binary data
        , e_EXTENDED_INTERNAL_DECIMAL64         = 11 // Decimal64
        , e_EXTENDED_INTERNAL_DECIMAL64_SPECIAL = 12 // Decimal64 NaN of Inf
        , e_EXTENDED_INTERNAL_DECIMAL64_ALLOC   = 13 // allocated Decimal64
        , e_EXTENDED_INTERNAL_NIL               = 14 // null value
        , k_NUM_EXTENDED_INTERNAL_TYPES         = 15 // number of distinct
                                                     // enumerated extended
                                                     // types
    };

    // PRIVATE CLASS DATA
    // 32-bit variation
    static const unsigned short DOUBLE_MASK       = 0x7ff0U;  // mask value to
                                                              // be stored in
                                                              // the exponent
                                                              // part of
                                                              // 'd_data' to
                                                              // indicate a
                                                              // special
                                                              // 'double' value

    static const int SHORTSTRING_SIZE    = 6;                 // maximum size
                                                              // of short-
                                                              // length strings
                                                              // that are
                                                              // stored
                                                              // "inline"

    static const int DATA_SIZE           = 4;                 // maximum size
                                                              // of small
                                                              // objects that
                                                              // are stored
                                                              // "inline"

    static const int BDLD_TYPE_MASK_BITS = 16;                // how many bits
                                                              // one the
                                                              // internal data
                                                              // type needs to
                                                              // be shifted
                                                              // into place

#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
    // Check if platform is little endian.
    static const int EXPONENT_OFFSET    = 6;  // offset of exponent
                                              // part within the
                                              // storage array
    static const int EXPONENT_LSB       = EXPONENT_OFFSET;
                                              // Low Byte
    static const int EXPONENT_MSB       = EXPONENT_OFFSET + 1;
                                              // High Byte
    static const int DATA_OFFSET        = 0;  // offset of data part
                                              // within the storage
                                              // array
    static const int SHORTSTRING_OFFSET = 0;  // offset where short-
                                              // strings are stored
                                              // in the storage array
    static const int SHORT_OFFSET       = 4;  // offset where (2 byte
                                              // values like)
                                              // discriminator values
                                              // for extended types
                                              // and information for
                                              // user-defined objects
                                              // is stored in the
                                              // storage array
    static const int MASK_OFFSET        = 4;  // offset in the storage
                                              // array where special
                                              // mask value is stored
    static const int NEARDATE_OFFSET    = 4;  // offset in the storage
                                              // array where (short)
                                              // date offset from now
                                              // is stored
                                              // mask value is stored
    static const int TIME_OFFSET        = 0;  // offset in the storage
                                              // array where time is
                                              // stored
#else
    // Check if platform is big endian.
    static const int EXPONENT_OFFSET    = 0;  // offset of exponent
                                              // part within the
                                              // storage array
    static const int EXPONENT_LSB       = EXPONENT_OFFSET + 1;
                                              // Low Byte
    static const int EXPONENT_MSB       = EXPONENT_OFFSET;
                                              // High Byte
    static const int DATA_OFFSET        = 4;  // offset of data part
                                              // within the storage
                                              // array
    static const int SHORTSTRING_OFFSET = 2;  // offset where short-
                                              // strings are stored
                                              // in the storage array
    static const int SHORT_OFFSET       = 2;  // offset where (2 byte
                                              // values like)
                                              // discriminator values
                                              // for extended types
                                              // and information for
                                              // user-defined objects
                                              // is stored in the
                                              // storage array
    static const int MASK_OFFSET        = 0;  // offset in the storage
                                              // array where special
                                              // mask value is stored
    static const int NEARDATE_OFFSET    = 2;  // offset in the storage
                                              // array where (short)
                                              // date offset from now
                                              // is stored
                                              // mask value is stored
    static const int TIME_OFFSET        = 4;  // offset in the storage
                                              // array where time is
                                              // stored
#endif

    enum {
        // Enumeration used to discriminate between the special uncompressible
        // Decimal64 values.

        e_DECIMAL64_SPECIAL_NAN,
        e_DECIMAL64_SPECIAL_INFINITY,
        e_DECIMAL64_SPECIAL_NEGATIVE_INFINITY
    };

    static bdlt::Date s_dateTimeBase;

    // DATA
    // 32-bit variation

    struct ShortString5 {
        // Storage for a string shorter than 6 chars and its length.
#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
        char                d_chars[5]; // the string's characters
        char                d_length;   // the string's length
        unsigned short      d_exponent; // the exponent inside the double
#else
        unsigned short      d_exponent; // the exponent inside the double
        char                d_chars[5]; // the string's characters
        char                d_length;   // the string's length
#endif
    };

    struct ShortString6 {
        // Storage for a string of exactly 6 chars. Length is implicit.
#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
        char                d_chars[6]; // the string's characters
        unsigned short      d_exponent; // the exponent inside the double
#else
        unsigned short      d_exponent; // the exponent inside the double
        char                d_chars[6]; // the string's characters
#endif
    };

    struct TypedAccess {
        // Storage for various combinations of short, int and pointer.
        // Null: d_short = extended type
        // Boolean: d_int = value
        // Integer: d_int = value
        // String, longer than 6: d_cvp = allocated memory containing copy of
        //   string preceded by length
        // StringRef, length < USHORT_MAX: d_cvp = value, d_ushort = length
        // StringRef, length >= USHORT_MAX:
        //   d_cvp points to value preceded by length,
        //   d_short = extended type
        // Date: d_int = value
        // Time: d_int = value
        // Datetime - near: d_int = time part, d_short = days from now
        // Datetime - far: d_short = extended typed_cvp = allocated value
        // DatetimeInterval - short: d_short = upper 16 bits, d_int = lower 32
        // DatetimeInterval - long: allocated value, d_short = extended type
        // Error - code: d_short = extended type, d_int = value
        // Error code+StringRef: d_short = extended type, d_cvp = pointer to
        //   allocated memory containing: code, length, string
        // Udt: d_cvp = value, d_ushort = type
        // ArrayReference, length < USHORT_MAX:
        //   d_cvp = pointer to array, d_ushort = length
        // ArrayReference, length >= USHORT_MAX: d_short = extended type,
        //   d_cvp = pointer to allocated memory: pointer to array, length
        // Map: d_cvp = pointer to length, d_short = extended type
        // Binary: d_cvp = value, d_short = extended type

#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
        union {
            int             d_int;      // either an integer
            const void     *d_cvp;      // or a const void*
        };
        union {
            short           d_short;    // either a short
            unsigned short  d_ushort;   // or un insigned short
        };
        unsigned short      d_exponent; // the exponent inside the double
#else
        unsigned short      d_exponent; // the exponent inside the double
        union {
            short           d_short;    // either a short
            unsigned short  d_ushort;   // or un insigned short
        };
        union {
            int             d_int;      // either an integer
            const void     *d_cvp;      // or a const void*
        };
#endif
    };

    struct ExponentAccess {
        // For accessing exponent as a word, for better performance.
#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
        unsigned int        d_dummy;
        unsigned int        d_value;    // the exponent as a 32 bit word
#else
        unsigned int        d_value;    // the exponent as a 32 bit word
        unsigned int        d_dummy;
#endif
    };

    union {
        double            d_double;    // either a double value
        ShortString5      d_string5;   // or a string shorter than 5 chars
        ShortString6      d_string6;   // or a string of exactly 6 chars
        TypedAccess       d_as;        // or a combinbation of pointer, int
                                       // and short
        ExponentAccess    d_exp;       // or the exponent as a 32 bit word
        char              d_data[8];   // or a raw array of bytes
    };

    // PRIVATE CLASS METHODS
    // 32-bit variation
    static Datum createExtendedDataObject(ExtendedInternalDataType  type,
                                          void                     *data);
    static Datum createExtendedDataObject(ExtendedInternalDataType  type,
                                          int                       data);
        // Return a datum by copying the specified 'data' of the
        // specified 'type'.  Note that the pointer value in 'data' is copied
        // and the pointed object is not cloned.

    // PRIVATE ACCESSORS
    // 32-bit variation
    ExtendedInternalDataType extendedInternalType() const;
        // Return the type of value stored inside this object (which cannot be
        // represented by the 4-bit discriminator 'InternalDataType') as one of
        // the enumeration values defined in 'ExtendedInternalDataType'.

    DataType typeFromExtendedInternalType() const;
        // Return the type of the value stored in this object as one of the
        // enumeration values defined in 'DataType' (mapped from the
        // stored 'ExtendedInternalDataType' value.

    bsls::Types::Int64 theSmallInteger64() const;
        // Return the 64-bit integer value stored inline in this object.

    bsls::Types::Int64 theLargeInteger64() const;
        // Return the 64-bit integer value stored in allocated storage.

    bslstl::StringRef theLongestShortString() const;
        // Return the short string value stored in this object as a
        // 'bslstl::StringRef' object.  The behavior is undefined unless this
        // object actually stores a short string value.

    bslstl::StringRef theLongStringReference() const;
        // Return the string referenced by this object.  The behavior is
        // undefined unless this object holds a reference to a string whose
        // length is too long to store inline; used only on 32-bit platforms.

    ConstDatumArrayRef theLongArrayReference() const;
        // Return the array referenced by this object.  The behavior is
        // undefined unless this object references an array with a length to
        // long to store inline; used only on 32-bit platforms.

#else // defined(BSLS_PLATFORM_CPU_32_BIT)

  private:
    // PRIVATE TYPES
    // 64-bit variation
    enum InternalDataType {
        // Enumeration used to discriminate among the different types of
        // values that can be stored inside 'Datum'.

          e_INTERNAL_INF               =  0  // +/- infinity value
        , e_INTERNAL_NIL               =  1  // null value
        , e_INTERNAL_BOOLEAN           =  2  // boolean value
        , e_INTERNAL_SHORTSTRING       =  3  // short string value
        , e_INTERNAL_STRING            =  4  // string value
        , e_INTERNAL_DATE              =  5  // date value
        , e_INTERNAL_TIME              =  6  // time value
        , e_INTERNAL_DATETIME          =  7  // date+time value
        , e_INTERNAL_DATETIME_INTERVAL =  8  // date+time interval value
        , e_INTERNAL_INTEGER           =  9  // integer value
        , e_INTERNAL_INTEGER64         = 10  // 64-bit integer value
        , e_INTERNAL_USERDEFINED       = 11  // pointer to a user-defined
                                                // object
                                                // datums
        , e_INTERNAL_ARRAY             = 12  // array of datums
        , e_INTERNAL_STRING_REFERENCE  = 13  // unowned string
        , e_INTERNAL_ARRAY_REFERENCE   = 14  // unowned array of
        , e_INTERNAL_REAL              = 15  // double value
        , e_INTERNAL_MAP               = 16  // map of datums
                                                // keyed by string values
                                                // that are not owned
        , e_INTERNAL_OWNED_MAP         = 17  // map of datums
                                                // keyed by string values
                                                // that are owned
        , e_INTERNAL_ERROR             = 18  // just error code
        , e_INTERNAL_ERROR_ALLOC       = 19  // error code and allocated
        , e_INTERNAL_BINARY            = 20  // binary data
        , e_INTERNAL_BINARY_ALLOC      = 21  // binary data and allocated
        , e_INTERNAL_DECIMAL64         = 22  // Decimal64
        , k_NUM_INTERNAL_TYPES         = 23  // number of distinct
                                                // enumerated types
    };

    // CLASS DATA
    // 64-bit variation
    static const int TYPE_OFFSET  = 14;             // offset where type
                                                    // is store
    static const int SHORTSTRING_SIZE_OFFSET = 0;   // offset of count byte
    static const int SHORTSTRING_OFFSET  = 1;       // offset of string,
                                                    // one past count byte
    static const int SHORTSTRING_SIZE    = 13;      // maximum size of
                                                    // short-length
                                                    // strings that are
                                                    // stored "inline"
    static const int SMALLBINARY_SIZE_OFFSET = 13;  // offset where the size of
                                                    // small- size binaries are
                                                    // stored in the storage
                                                    // array
    static const int SMALLBINARY_SIZE    = 13;      // maximum size of
                                                    // small-size binaries that
                                                    // are stored "inline"

    // DATA
    // 64-bit variation
    struct TypedAccess {
        // Typed access to the bits of the 'Datum' internal representation
        union {                                          // +0
            bsls::Types::Int64  d_int64;
            void               *d_ptr;
            double              d_double;
        };
        int                     d_int32;                 // +8
        short                   d_filler;                // +12
        short                   d_type;                  // +14
    };

    union {
        // Ensures proper alignment (16 byte) and provides 2 types of access to
        // the 64bit 'Datum' internal representation. The 'd_storage' array
        // allows us raw access to the bytes; while 'd_as' provides typed
        // access to the individual "data compartments".
        bsls::AlignedBuffer<16> d_storage;
        TypedAccess             d_as;
    };

    // PRIVATE CLASS METHODS
    // 64-bit variation
    static Datum createDatum(InternalDataType type, void *data);
    static Datum createDatum(InternalDataType type, int data);

    // PRIVATE ACCESSORS
    // 64-bit variation
    void* theInlineStorage();
        // Return a pointer to the data storage available to inlined values.

    const void* theInlineStorage() const;
        // Return a pointer to the (non-modifiable) data storage available to
        // inlined values.

#endif // defined(BSLS_PLATFORM_CPU_32_BIT)

  private:
    // FRIENDS
    friend bool operator==(const Datum& lhs, const Datum& rhs);
    friend bool operator!=(const Datum& lhs, const Datum& rhs);
    friend bsl::ostream& operator<<(bsl::ostream& output,
                                    const Datum&  rhs);
    // PRIVATE CLASS METHODS
    static void destroyMemory(const Datum&      value,
                              bslma::Allocator *basicAllocator);
        // Deallocate and return any memory (calling destructor if it is not
        // trivial) that was previously allocated within the specified
        // 'value' using the specified 'basicAllocator'.

    // PRIVATE ACCESSORS
    InternalDataType internalType() const;
        // Return the type of value stored inside this object as one of the
        // enumeration values defined in 'InternalDataType'.

    bslstl::StringRef theShortString() const;
        // Return the short string value stored in this object as a
        // 'bslstl::StringRef' object.  The behavior is undefined unless this
        // object actually stores a short string value.

    bslstl::StringRef theInternalString() const;
        // Return the internal string stored in this object.  The behavior is
        // undefined unless this object holds an internal (non-reference,
        // non-short) string.

    bslstl::StringRef theStringReference() const;
        // Return the string referenced by this object.  The behavior is
        // undefined unless this object holds an reference to a string with a
        // short size.  Note that all strings have short sizes on 64-bit
        // platforms.

    ConstDatumArrayRef theInternalArray() const;
        // Return a reference to the internal array stored in this object.  The
        // behavior is undefined unless this object holds an owned array.

    ConstDatumArrayRef theArrayReference() const;
        // Return the array referenced by this object.  The behavior is
        // undefined unless this object holds a (short) array reference.  Note
        // that all array references are "short" on 64-bit platforms.

  public:
    // TYPES
    typedef bsls::Types::size_type size_type;
        // 'size_type' is an alias for an integer value, representing the
        // capacity or a datum array, the capacity of a datum map or the
        // capacity or the *keys-capacity* of a datum-key-owning map.  It can
        // also represent the length of a string.

    // CLASS METHODS
    static Datum createNull();
        // Return a datum having no value.

    static Datum createBoolean(bool value);
        // Return a datum having the specified 'value'.

    static Datum createInteger(int value);
        // Return a datum having the specified 'value'.

    static Datum createInteger64(bsls::Types::Int64  value,
                                 bslma::Allocator   *basicAllocator);
        // Return a datum having the specified 'value', using the
        // specified 'basicAllocator' to allocate memory if needed.

    static Datum createDouble(double value);
        // Return a datum having the specified 'value'.  Note that
        // this method normalizes any NaN or IND value to a Quiet NaN value for
        // the particular platform.

    static Datum createDecimal64(bdldfp::Decimal64  value,
                                 bslma::Allocator  *basicAllocator);
        // Return a datum having the specified 'value', using the
        // specified 'basicAllocator' to allocate memory if needed.  Note that
        // the argument is passed by value because it is assumed to be a
        // fundamental type.

    static Datum copyString(const char       *value,
                            size_type         length,
                            bslma::Allocator *basicAllocator);
        // Return a datum having a copy of the UTF-8 encoded string
        // pointed to by the specified 'value', and having the specified
        // 'length'.


    static Datum copyString(const bslstl::StringRef&  value,
                            bslma::Allocator         *basicAllocator);
        // Return a datum having the specified 'value', using the
        // specified 'basicAllocator' to allocate memory if needed. The
        // behavior is undefined unless 'value' refers to a UTF-8 encoded
        // string.

    static Datum createStringRef(const char       *value,
                                 bslma::Allocator *basicAllocator);
        // Return a datum having the specified 'value', using the
        // specified 'basicAllocator' to allocate memory if needed.  Note that
        // 'value' is held, not owned.

    static Datum createStringRef(const char       *value,
                                 size_type         length,
                                 bslma::Allocator *basicAllocator);
        // Return a datum having the specified 'value' and 'length',
        // using the specified 'basicAllocator' to allocate memory if needed.
        // The behavior is undefined unless 'value' points to a UTF-8 encoded
        // string. The behavior is also undefined unless '0 != value' or
        // '0 == length', that is, 'value' may be zero if 'length' is also
        // zero. Note that 'value' is held, not owned.

    static Datum createStringRef(const bslstl::StringRef&  value,
                                 bslma::Allocator         *basicAllocator);
        // Return a datum having the specified 'value', using the
        // specified 'basicAllocator' to allocate memory if needed. The
        // behavior is undefined unless 'value' refers to a UTF-8 encoded
        // string. Note that 'value' is held, not copied.

#ifndef BDLD_DATUM_OMIT_DEPRECATED

    BDLD_DATUM_DEPRECATED(
        static Datum createStringReference(const char       *value,
                                           bslma::Allocator *basicAllocator));
        // Deprecated, use createStringRef instead.

    BDLD_DATUM_DEPRECATED(
         static Datum createStringReference(
             const char       *value,
             size_type         length,
             bslma::Allocator *basicAllocator));
        // Deprecated, use createStringRef instead.

    BDLD_DATUM_DEPRECATED(
        static Datum createStringReference(
            const bslstl::StringRef&  value,
            bslma::Allocator         *basicAllocator));
        // Deprecated, use createStringRef instead.

#endif

    static char *createUninitializedString(Datum            *result,
                                           size_type         length,
                                           bslma::Allocator *basicAllocator);
        // Load the specified 'result' with a 'Datum' that holds the address of
        // a memory buffer of the specified 'length' and return the address,
        // using the specified 'basicAllocator' to allocate memory.  The
        // behavior is undefined unless '0 != result'.  Note that the caller is
        // responsible for initializing the returned buffer with a UTF-8
        // encoded string.

    static Datum createDate(const bdlt::Date& value);
        // Return a datum having the specified 'value'.

    static Datum createTime(const bdlt::Time& value);
        // Return a datum having the specified 'value'.

    static Datum createDatetime(const bdlt::Datetime&  value,
                                bslma::Allocator      *basicAllocator);
        // Return a datum having the specified 'value'.

    static Datum createDatetimeInterval(
                                const bdlt::DatetimeInterval&  value,
                                bslma::Allocator              *basicAllocator);
        // Return a datum having the specified 'value', using the
        // specified 'basicAllocator' to allocate memory if needed.

    static Datum createError(int code);
        // Return a datum holding an 'Error' value having the
        // specified 'code'.

    static Datum createError(int                       code,
                             const bslstl::StringRef&  message,
                             bslma::Allocator         *basicAllocator);
        // Return a datum holding an 'Error' value having the
        // specified 'code' and 'message', using the specified 'basicAllocator'
        // to supply memory if needed. The behavior is undefined unless
        // 'message' refers to a UTF-8 encoded string.

    static Datum createUdt(void *data, int type);
        // Return a datum holding the specified user defined 'data'
        // of the specified 'type'.  The behavior is undefined unless
        // '0 <= type <= 65535'.  Note that 'data' is held, not
        // owned. Also note that the content pointed to by 'data'
        // object is not copied.

    static Datum copyBinary(const void       *value,
                            size_type         size,
                            bslma::Allocator *basicAllocator);
        // Return a datum that refers to a copy of the content of the
        // specified 'size' bytes pointed to by the specified 'value', using
        // the specified 'basicAllocator' to allocate memory as needed.  The
        // copy is owned by the datum and will be freed when the datum is
        // destroyed.

    static Datum adoptArray(const DatumArrayRef& array);
        // Return a datum holding the datum array referenced by the
        // specified 'array'.  The behavior is undefined unless 'array' was set
        // using 'createUninitializedArray' method.  The behavior is also
        // undefined unless each element in the held datum array has been
        // assigned a value and the array's length has been set accordingly.

    static Datum createArrayReference(const Datum      *array,
                                      size_type         length,
                                      bslma::Allocator *basicAllocator);
        // Return a datum having the specified 'array' and 'length',
        // using the specified 'basicAllocator' to allocate memory if needed.
        // Note that 'value' is held, not copied, and is not freed when the
        // returned object is destroyed.

    static Datum createArrayReference(
                                    const ConstDatumArrayRef&  value,
                                    bslma::Allocator          *basicAllocator);
        // Return a datum having the specified 'value',  using the
        // specified 'basicAllocator' to allocate memory if needed.  Note that
        // 'value' is held, not copied, and is not freed when the returned
        // object is destroyed.

    static void createUninitializedArray(DatumArrayRef    *result,
                                         size_type         capacity,
                                         bslma::Allocator *basicAllocator);
        // Load the specified 'result' with a reference to a newly created
        // datum array having the specified 'capacity', using the specified
        // 'basicAllocator' to allocate memory.  The behavior is undefined
        // unless '0 != result'.  Note that the caller is responsible for
        // filling in elements into the datum array and setting its length
        // accordingly.  The number of elements in the datum array cannot
        // exceed 'capacity'.  Also note that any elements in the datum array
        // that need dynamic memory must be allocated with 'basicAllocator'.

    static void disposeUninitializedArray(
                                         const DatumArrayRef&  array,
                                         bslma::Allocator     *basicAllocator);
        // Dispose the memory allocated for the datum array referenced by the
        // specified 'array' using the specified 'basicAllocator'.  The
        // behavior is undefined unless memory for the datum array was
        // allocated through 'createUninitializedArray' using 'basicAllocator'.
        // Note that the memory allocated for the individual elements within
        // the datum array must be deallocated separately before calling this
        // method.

    static Datum adoptMap(const DatumMapRef& mapping);
        // Return a datum holding the datum map referenced by the
        // specified 'mapping'.  The behavior is undefined unless 'mapping' was
        // set using 'createUninitializedMap' method.  The behavior is also
        // undefined unless each element in the held map has been assigned a
        // value and the size of the map has been set accordingly.

    static Datum adoptMapOwningKeys(const DatumMapOwningKeysRef& mapping);
        // Return a datum holding the datum-key-owning map referenced
        // by the specified 'mapping'.  The behavior is undefined unless
        // 'mapping' was created using 'createUninitializedMapOwningKeys'
        // method.  The behavior is also undefined unless each element in the
        // held map has been assigned a value and the size of the map has been
        // set accordingly.  The behavior is also undefined unless keys have
        // been copied into the map.

    static void createUninitializedMap(DatumMapRef      *result,
                                       size_type         capacity,
                                       bslma::Allocator *basicAllocator);
        // Load the specified 'result' with a reference to a newly created
        // datum map having the specified 'capacity',  using the specified
        // 'basicAllocator' to allocate memory.  The behavior is undefined
        // unless '0 != result'.  Note that the caller is responsible for
        // filling in elements into the datum map and setting its size
        // accordingly.  The number of elements in the datum map cannot exceed
        // 'capacity'.  Also note that any elements in the datum map that need
        // dynamic memory, should also be allocated with 'basicAllocator'.

    static void disposeUninitializedMap(const DatumMapRef&  mapping,
                                        bslma::Allocator   *basicAllocator);
        // Dispose the memory allocated for the datum map referenced by the
        // specified 'mapping' using the specified 'basicAllocator'.  The
        // behavior is undefined unless memory for the datum map was allocated
        // through 'createUninitializedMap' using 'basicAllocator'.  Note that
        // the memory allocated for the individual elements within the datum
        // map must be deallocated separately before calling this method.

    static void createUninitializedMapOwningKeys(
                                       DatumMapOwningKeysRef *result,
                                       size_type              capacity,
                                       size_type              keysCapacity,
                                       bslma::Allocator      *basicAllocator);
        // Load the specified 'result' with a reference to a newly created
        // datum-key-owning map having the specified 'capacity' and
        // 'keysCapacity', using the specified 'basicAllocator' to allocate
        // memory.  The behavior is undefined unless '0 != result'.  Note that
        // the caller is responsible for filling in elements into the
        // datum-key-owning map, copying the keys into it, and setting its
        // size accordingly.  The number of elements in the datum-key-owning
        // map cannot exceed 'capacity' and total size of all the keys cannot
        // exceed 'keysCapacity'.  Also note that any elements in the
        // datum-key-owning map that need dynamic memory, should also be
        // allocated with 'basicAllocator'.

    static void disposeUninitializedMapOwningKeys(
                                 const DatumMapOwningKeysRef&  mapping,
                                 bslma::Allocator             *basicAllocator);
        // Dispose the memory allocated for the datum-key-owning map referenced
        // by the specified 'mapping' using the specified 'basicAllocator'.
        // The behavior is undefined unless memory for the datum-key-owning was
        // allocated through 'createUninitializeMapOwningKeys' using
        // 'basicAllocator'.  Note that the memory allocated for the individual
        // elements within the datum-key-owning map must be deallocated
        // separately before calling this method.

    static void destroy(const Datum&      value,
                        bslma::Allocator *basicAllocator);
        // Deallocate and return any memory that was previously allocated
        // within the specified 'value' using the specified 'basicAllocator'.
        // If the specified 'value' contains an array of datums,
        // 'destroy' is called on each datum inside the array to
        // deallocate any allocated memory within the objects.  If the
        // specified 'value' contains a map of datums, 'destroy' is
        // called on each datum inside the map to deallocate any
        // allocated memory within the objects.  Note that datums are
        // bitwise copyable.  Only one of the copies of the same datum
        // can be destroyed.  The rest of those copies then become invalid and
        // should not be used or destroyed.  It is undefined behavior to call
        // any accessors or 'destroy' on these copies.  It is also undefined
        // behavior to pass a 'Datum' holding an uninitialized or partially
        // initialized array to this function (when the array was created using
        // 'createUninitializedArray').  It is also undefined behavior to pass
        // a 'Datum' holding an uninitialized or partially initialized map to
        // this function (when the map was created using
        // 'createUninitializedMap' or 'createUninitializeMapOwningKeys').

    static const char *dataTypeToAscii(DataType type);
        // Return the non-modifiable string representation corresponding to the
        // specified 'type', if it exists, and a unique (error) string
        // otherwise.  The string representation of 'type' matches its
        // corresponding enumerator name with the 'BDLD_' prefix elided.
        // For example:
        //..
        //  bsl::cout << bdld::Datum::dataTypeToAscii(bdld::Datum::e_NIL);
        //..
        // will print the following on standard output:
        //..
        //  NIL
        //..
        // Note that specifying a 'type' that does not match any of the
        // enumerators will result in a string representation that is distinct
        // from any of those corresponding to the enumerators

    // CREATORS
    //! Datum() = default;
        // Create a datum having the default value.  Note that this
        // method's definition is compiler generated.

    //! Datum(const Datum& original) = default;
        // Create a datum having the value of the specified 'original'
        // object. Note that this method's definition is compiler generated.

    //! ~Datum() = default;
        // Destroy this object. Note that this method's definition is compiler
        // generated.

    // MANIPULATORS
    //! Datum& operator=(const Datum& rhs);
        // Assign to this object the value of the specified 'rhs' object. Note
        // that this method's definition is compiler generated.

    // ACCESSORS
    DataType type() const;
        // Return the type of value stored inside this object as one of the
        // enumeration values defined in 'DataType'.

    bool isNull() const;
        // Return 'true' if this object has no value and 'false' otherwise.

    bool isBoolean() const;
        // Return 'true' if this object holds a 'bool' value and 'false'
        // otherwise.

    bool theBoolean() const;
        // Return the boolean value stored in this object as a 'bool' value.
        // The behavior is undefined unless this object actually stores a
        // 'bool' value.

    bool isInteger() const;
        // Return 'true' if this object holds a 'int' value and 'false'
        // otherwise.

    int theInteger() const;
        // Return the integer value stored in this object as a 'int' value.
        // The behavior is undefined unless this object actually stores an
        // 'int' value.

    bool isInteger64() const;
        // Return 'true' if this object holds a 'Int64' value and 'false'
        // otherwise.

    bsls::Types::Int64 theInteger64() const;
        // Return the 64-bit integer value stored in this object as a 'Int64'
        // value.  The behavior is undefined unless this object actually stores
        // a 'Int64' value.

    bool isDouble() const;
        // Return 'true' if this object holds a 'double' value and 'false'
        // otherwise.

    double theDouble() const;
        // Return the floating point value stored inside this object as a
        // 'double' value.  The behavior is undefined unless this object
        // actually stores a 'double' value. Note that a zero-initialized
        // datum holds a 'double' with the value '0.0'.

    bool isDecimal64() const;
        // Return 'true' if this object holds a 'Decimal64' value and 'false'
        // otherwise.

    bdldfp::Decimal64 theDecimal64() const;
        // Return the decimal floating point value stored inside this object as
        // a 'Decimal64' value.  The behavior is undefined unless this object
        // actually stores a 'Decimal64' value.

    bool isString() const;
        // Return 'true' if this object holds a string value and 'false'
        // otherwise.

    bslstl::StringRef theString() const;
        // Return the string value stored in this object as a
        // 'bslstl::StringRef' object.  The behavior is undefined unless this
        // object actually stores a string value.

    bool isDate() const;
        // Return 'true' if this object holds a 'bdlt::Date' value and 'false'
        // otherwise.

    bdlt::Date theDate() const;
        // Return the date value stored in this object as a 'bdlt::Date' object.
        // The behavior is undefined unless this object actually stores a
        // 'bdlt::Date' value.

    bool isTime() const;
        // Return 'true' if this object holds a 'bdlt::Time' value and 'false'
        // otherwise.

    bdlt::Time theTime() const;
        // Return the time value stored in this object as a 'bdlt::Time' object.
        // The behavior is undefined unless this object actually stores a
        // 'bdlt::Time' value.

    bool isDatetime() const;
        // Return 'true' if this object holds a 'bdlt::Datetime' value and
        // 'false' otherwise.

    bdlt::Datetime theDatetime() const;
        // Return the date+time value stored in this object as a
        // 'bdlt::Datetime' object.  The behavior is undefined unless this
        // object actually stores a 'bdlt::Datetime' value.

    bool isDatetimeInterval() const;
        // Return 'true' if this object holds a 'bdlt::DatetimeInterval' value
        // and 'false' otherwise.

    bdlt::DatetimeInterval theDatetimeInterval() const;
        // Return the date+time interval value stored in this object as a
        // 'bdlt::DatetimeInterval'.  The behavior is undefined unless this
        // object actually stores a 'bdlt::DatetimeInterval' value.

    bool isError() const;
        // Return 'true' if this object holds a 'Error' value and 'false'
        // otherwise.

    Error theError() const;
        // Return the error value stored in this object as a 'Error' value.
        // The behavior is undefined unless this object actually stores a
        // 'Error' value.

    bool isUdt() const;
        // Return 'true' if this object holds a pointer to an externally
        // allocated object and 'false' otherwise.

    Udt theUdt() const;
        // Return the externally allocated object pointed to by the 'void *'
        // pointer stored inside this object along with a type value, as a
        // 'Udt' object.  The behavior is undefined unless this object actually
        // stores a 'void *' pointer and a type value.  Note that we hold a
        // pointer to a 'void' object, because it is compatible with most other
        // pointers.  Thus, it is easy to cast to and from a 'void *' to a
        // pointer to the actual object.

    bool isBinary() const;
        // Return 'true' if this object holds a binary value and 'false'
        // otherwise.

    ConstDatumBinaryRef theBinary() const;
        // Return the binary stored in this object as a
        // 'ConstDatumBinaryRef' object.  The behavior is undefined unless this
        // object actually stores a binary value.

    bool isArray() const;
        // Return 'true' if this object holds an array of 'Datum' values and
        // 'false' otherwise.

    ConstDatumArrayRef theArray() const;
        // Return the array value stored in this object as a
        // 'ConstDatumArrayRef' object.  The behavior is undefined unless this
        // object actually stores an array of datums.

    bool isMap() const;
        // Return 'true' if this object holds a map of datums that
        // are keyed by string values and 'false' otherwise.

    ConstDatumMapRef theMap() const;
        // Return the map value stored in this object as a 'ConstDatumMapRef'
        // object.  The behavior is undefined unless this object actually
        // stores a map of datums.

    template <typename BDLD_VISITOR>
    void apply(BDLD_VISITOR& visitor) const;
        // Apply the specified 'visitor' on the current value held by this
        // object by passing stored the value to the 'visitor' object's
        // 'operator()' overload.

    Datum clone(bslma::Allocator *basicAllocator) const;
        // Return a datum by "deep-copying" (clone any dynamically
        // allocated memory and not share it) this object, using the specified
        // 'basicAllocator' to allocate memory if needed.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // (absolute value of) the optionally specified indentation 'level'
        // and return a reference to 'stream'.  If 'level' is specified,
        // optionally specify 'spacesPerLevel', the number of spaces per
        // indentation level for this and all of its nested objects.  If
        // 'level' is negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, format the entire output on one
        // line, suppressing all but the initial indentation (as governed by
        // 'level').  If 'stream' is not valid on entry, this operation has no
        // effect.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS3(
                                Datum,
                                bslalg::TypeTraitBitwiseMoveable,
                                bslalg::TypeTraitBitwiseCopyable,
                                bslalg::TypeTraitHasTrivialDefaultConstructor);
        // datums are bitwise movable, bitwise copyable and have a
        // trivial constructor
};

// FREE OPERATORS
bool operator==(const Datum& lhs, const Datum& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.  Two datums (not holding strings and user-
    // defined objects) have the same value if they have the same type of value
    // stored inside them and invoking '==' operator on the stored values
    // returns 'true'.  Two datums holding strings are equal if the
    // strings have the same length and and values at each respective character
    // position are also same.  Two datums holding user-defined
    // objects are equal if the user-defined objects have the same pointer and
    // type values.  Two 'nil' datums are always equal.  Two 'Datum'
    // objects holding 'NaN' values are never equal.  Two datums that
    // hold array of datums have the same value if the underlying
    // arrays have the same length and invoking '==' operator on each
    // corresponding element returns 'true'.  Two datums that hold map
    // of datums have the same value if the underlying maps have the
    // same size and each corresponding pair of elements in the maps have the
    // same keys and invoking '==' operator on the values returns 'true'.

bool operator!=(const Datum& lhs, const Datum& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' datums do not
    // have the same value, and 'false' otherwise.  Two datums do not
    // have the same value if they do not hold values of the same type, or they
    // hold values of the same type but invoking '==' operator on the stored
    // values returns 'false'.  Two strings do not have the same value if they
    // have different lengths or values at one of the respective character
    // position are not the same.  Two 'Udt' objects are not equal if they have
    // different pointer or type values.  Two 'bdemf_Nil' values are always
    // equal.  Two datums with 'NaN' values are never equal.  Two
    // datums that hold array of datums have different values
    // if the the underlying arrays have different lengths or invoking '=='
    // operator on at least one of the corresponding pair of contained elements
    // returns 'false'.  Two datums that hold map of datums
    // have different values if the underlying maps have different sizes or at
    // least one of the corresponding pair of elements in the maps have
    // different keys or invoking '==' operator on the values returns 'false'.

bsl::ostream& operator<<(bsl::ostream& output, const Datum& rhs);
    // Write the specified 'rhs' value to the specified output 'stream' in
    // the format shown in the second column in the table below (based on the
    // type of value stored, indicated by the first column):
    //..
    //  null                  - [nil]
    //  bool                  - true
    //  bool                  - false
    //  Error                 - error(n) (where n is the integer error code
    //                                    value)
    //  int                   - nnn (where nnn is the integer number)
    //  Int64                 - nnnnnn (where nnnnnn is the Int64 number)
    //  double                - mm.nn (where mm.nn represents the double value)
    //  string                - abcd (where "abcd" is some string)
    //  array                 - [nn,true,abcd] (where nn is an integer
    //                                          number and "abcd" is some
    //                                          string)
    //  map                   - [(abc,nn),(pqr,true)] (where nn is an integer
    //                                                 number and "abc" and
    //                                                 "pqr" are some strings)
    //  bdlt::Date             - ddMONyyyy
    //  bdlt::Time             - hh:mm:ss.sss
    //  bdlt::Datetime         - ddMONyyyy_hh:mm:ss.sss
    //  bdlt::DatetimeInterval - sDD_HH:MM:SS.SSS (where s is the sign(+/-))
    //  void *                - user-defined(0xffffffff,type) (where 0xffffffff
    //                                       the address of the object and type
    //                                       its a numeric value corresponding
    //                                       to the actual type of the object)
    //..
    // and return a reference to the modifiable 'stream'.  The function will
    // have no effect if the specified 'stream' is not valid.

bsl::ostream& operator<<(bsl::ostream& stream, Datum::DataType rhs);
    // Write the string representation of the specified 'rhs' enumeration value
    // to the specified 'stream', and return a reference to 'stream'.  See
    // 'dataTypeToAscii' for what constitutes the string representation of a
    // 'Datum::DataType' value.

                            // ===================
                            // class DatumArrayRef
                            // ===================

class DatumArrayRef {
    // This 'class' provides mutable access to a datum array.  The users of
    // this class can read from and assign to the individual elements as well
    // as change the length of the array.

  private:
    // DATA
    Datum                  *d_data_p;    // pointer to an array of 'Datum'
                                         // objects (not owned)
    bsls::Types::size_type *d_length_p;  // pointer to location for the length
                                         // of the array

  public:
    // CREATORS
    DatumArrayRef();
        // Create a 'DatumArrayRef' object that refers to no array.

    DatumArrayRef(Datum *data, bsls::Types::size_type *length);
        // Create a 'DatumArrayRef' object having the specified 'data' and
        // 'length'.

    // ACCESSORS
    Datum *data() const;
        // Return pointer to the first element of the held array.

    bsls::Types::size_type *length() const;
        // Return pointer to the location where the (held) array's length is
        // stored.
};

//@PURPOSE: Provide type for mutable access to 'Datum' map that owns its keys.
//
//@CLASSES:
//   DatumMapOwningKeysRef: type for mutable 'Datum' map access
//
//@AUTHOR: Rishi Wani (pwani)
//
//@SEE ALSO: bdld_datum, bdld_datummapowningkeysbuilder
//
//@DESCRIPTION: This component defines a 'class' that provides mutable access
// to a datum key-owning map.  The caller of
// 'Datum::createUninitializedMapOwningKeys' passes a modifiable reference of a
// 'DatumMapOwningKeysRef' object, a capacity and a keys-capacity to the
// method, which populates the 'DatumMapOwningKeysRef' object with the
// addresses of the allocated memory locations for the map, its size, keys
// storage, and a *sorted* flag to indicate whether the map is sorted or not.
// The user is then responsible for filling in elements into the map,
// copying the keys, and setting its size accordingly.  The user can set the
// *sorted* flag if the elements are inserted in a sorted order in the map.
// Note that the user of this class is responsible to keep track of the size of
// the map exceeding its capacity as well as the keys-size exceeding the
// keys-capacity and has to request a new larger map by calling
// 'Datum::createUninitializedMapOwningKeys'.
//
///Usage
///-----
// The following snippets of code illustrate how to create and use a
// 'DatumMapOwnkingKeysRef' object.
//..
//  DatumMapEntry *data = reinterpret_cast<DatumMapEntry *>(0xDEADBEEF);
//  bsls::Types::size_type *size =
//                      reinterpret_cast<bsls::Types::size_type *>(0xBEEFDEAD);
//  char *keys = reinterpret_cast<char *>(0xDEADBEEF);
//  bool *sorted = reinterpret_cast<bool *>(0xBEEFDEAD);
//  DatumMapOwningKeysRef obj(data, size, keys, sorted);
//  ASSERT(obj.data() == data);
//  ASSERT(obj.size() == size);
//  ASSERT(obj.keys() == keys);
//  ASSERT(obj.sorted() == sorted);
//..

                        // ===========================
                        // class DatumMapOwningKeysRef
                        // ===========================

class DatumMapOwningKeysRef {
    // This 'class' provides mutable access to a datum key-owning map.  The
    // users of this class can assign to the individual elements, copy keys and
    // change the size of the map.

  private:
    // DATA
    DatumMapEntry          *d_data_p;    // pointer to a map of datums
                                         // (not owned)
    bsls::Types::size_type *d_size_p;    // pointer to location for the size of
                                         // the map
    char                   *d_keys_p;    // pointer to a location for storing
                                         // all the keys
    bool                   *d_sorted_p;  // pointer to location to indicate
                                         // if the map is sorted or not

  public:
    // CREATORS
    DatumMapOwningKeysRef();
        // Create a 'DatumMapOwningKeysRef' object.

    DatumMapOwningKeysRef(DatumMapEntry          *data,
                          bsls::Types::size_type *size,
                          char                   *keys,
                          bool                   *sorted);
        // Create a 'DatumMapRef' object having the specified 'data', 'size',
        // 'keys', and 'sorted'.

    // ACCESSORS
    DatumMapEntry *data() const;
        // Return pointer to the first element in the held map.

    bsls::Types::size_type *size() const;
        // Return pointer to the location where the (held) map's size is
        // stored.

    char *keys() const;
        // Return pointer to the start of the buffer where keys are stored.

    bool *sorted() const;
        // Return pointer to the location where the (held) map's *sorted* flag
        // is stored.
};

                             // =================
                             // class DatumMapRef
                             // =================

class DatumMapRef {
    // This 'class' provides a mutable access to a datum map.  The users of
    // this class can assign to the individual elements and also change the
    // size of the map.

  private:
    // DATA
    DatumMapEntry          *d_data_p;    // pointer to a map of datums
                                         // (not owned)
    bsls::Types::size_type *d_size_p;    // pointer to location for the size of
                                         // the map
    bool                   *d_sorted_p;  // pointer to location to indicate if
                                         // the map is sorted or not

  public:
    // CREATORS
   DatumMapRef();
        // Create a 'DatumMapRef' object.

    DatumMapRef(DatumMapEntry          *data,
                bsls::Types::size_type *size,
                bool                   *sorted);
        // Create a 'DatumMapRef' object having the specified 'data', 'size',
        // and 'sorted'.

    // ACCESSORS
    DatumMapEntry *data() const;
        // Return pointer to the first element in the (held) map.

    bsls::Types::size_type *size() const;
        // Return pointer to the location where the (held) map's size is
        // stored.

    bool *sorted() const;
        // Return pointer to the location where the (held) map's *sorted* flag
        // is stored.
};

//@PURPOSE: Provide a type for an error code with an optional error message.
//@CLASSES:
//  bdld::Error: type for an error code with an optional error message
//
//@SEE_ALSO: bdld_datum
//
//@AUTHOR: Rishi Wani (pwani) Paul Sader (psader)
//
//@DESCRIPTION: This component defines a *complex-constrained* *value-semantic*
// *attribute* class 'Error' representing a descriptive error message with an
// error code.  This component does not make a copy of the error message that
// was provided to it during construction, but it holds a reference to it.
// Accessors inside 'Datum' class that need to return an error value, return an
// instance of 'Error'.
//
///Usage
///-----
// The following snippets of code illustrate how to create and use an 'Error'
// object.
//..
//  const int code = 3;
//  const bsl::string message = "generic error";
//  const Error obj(code, message);
//  ASSERT(obj.code() == code);
//  ASSERT(obj.message() == message);
//..

                               // ===========
                               // class Error
                               // ===========
class Error {
    // This class provides a *complex constrained* attribute type that
    // represents a descriptive error message with an error code.  Notice that
    // this class has an implicitly defined (trivial) destructor.  Also notice
    // that this class holds a reference to the error message and does not make
    // a copy of it.

  private:
    // DATA
    int               d_code;     // error code
    bslstl::StringRef d_message;  // error message (ref)

  public:
    // CREATORS
    Error();
        // Create a 'Error' object having the default error code of 0 and an
        // empty error message.

    explicit Error(int code);
        // Create a 'Error' object having the specified error 'code' value and
        // and empty error message.

    Error(int code, const bslstl::StringRef& message);
        // Create a 'Error' object having the specified error 'code' value and
        // the specified error 'message' value.  Note that this class does not
        // make a copy of 'message', but holds a reference to it.  The string
        // referred to by 'message' should outlive this object.

    // ACCESSORS
    int code() const;
        // Return the error code.

    bslstl::StringRef message() const;
        // Return the error message.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // (absolute value of) the optionally specified indentation 'level'
        // and return a reference to 'stream'.  If 'level' is specified,
        // optionally specify 'spacesPerLevel', the number of spaces per
        // indentation level for this and all of its nested objects.  If
        // 'level' is negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, format the entire output on one
        // line, suppressing all but the initial indentation (as governed by
        // 'level').  If 'stream' is not valid on entry, this operation has no
        // effect.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(Error, bslalg::TypeTraitBitwiseCopyable);
        // 'Error' values are bitwise copyable.
};

// FREE OPERATORS
bool operator==(const Error& lhs, const Error& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have the same value and
    // 'false' otherwise.  Two 'Error' objects have the same value if they have
    // same error code and message values.

bool operator!=(const Error& lhs, const Error& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have different values and
    // 'false' otherwise.  Two 'Error' objects have different values if they
    // have different error code or message values.

bool operator<(const Error& lhs, const Error& rhs);
    // Return 'true' if value of the specified 'lhs' is less than value of the
    // specified 'rhs' and 'false' otherwise.  Value of 'lhs' is less than
    // value of 'rhs' if error code value of 'lhs' is less than error code
    // value of 'rhs', or they have the same error code value and error message
    // value of 'lhs' is less than error message value of 'rhs'.

bool operator<=(const Error& lhs, const Error& rhs);
    // Return 'true' if value of the specified 'lhs' is less than or equal to
    // value of the specified 'rhs' and 'false' otherwise.  Value of 'lhs' is
    // less than or equal to value of 'rhs' if error code value of 'lhs' is
    // less than or equal to error code value of 'rhs', or they have the same
    // error code value and error message value of 'lhs' is less than or equal
    // to error message value of 'rhs'.

bool operator>(const Error& lhs, const Error& rhs);
    // Return 'true' if value of the specified 'lhs' is greater than value of
    // the specified 'rhs' and 'false' otherwise.  Value of 'lhs' is greater
    // than value of 'rhs' if error code value of 'lhs' is greater than error
    // code value of 'rhs', or they have the same error code value and error
    // message value of 'lhs' is greater than error message value of 'rhs'.

bool operator>=(const Error& lhs, const Error& rhs);
    // Return 'true' if value of the specified 'lhs' is greater than or equal
    // to value of the specified 'rhs' and 'false' otherwise.  Value of 'lhs'
    // is greater than or equal to value of 'rhs' if error code value of 'lhs'
    // is greater than or equal to error code value of 'rhs', or they have the
    // same error code value and error message value of 'lhs' is greater than
    // or equal to error message value of 'rhs'.

bsl::ostream& operator<<(bsl::ostream& output, const Error& rhs);
    // Write the specified 'rhs' value to the specified output 'stream' in
    // the format shown below:
    //..
    //  error(n)
    //  error(n,'msg')
    //..
    // (where n is the integer error code value and 'msg' is the error message
    // value in single quotes) and return a reference to the modifiable
    // 'stream'.  The function will have no effect if the specified 'stream' is
    // not valid.  Note that the first version will be output if there is no
    // error message string.

//@PURPOSE: Provide a type to represent a user-defined type.
//@CLASSES:
//  bdld::Udt: *value-semantic* type representing a user-defined type
//
//@SEE_ALSO: bdld_datum
//
//@AUTHOR: Rishi Wani (pwani)
//
//@DESCRIPTION:  This component defines an *in-core* simply-constrained
// *value-semantic* class 'Udt' that represents a pair of pointer to a
// user-defined object (as a 'void *') and an integer value denoting what
// actual type is represented.  Note that the class defined by this component
// does not manage any memory.  Also note that the integer 'type' value is for
// use by some external class that casts this object back to its actual type
// using the type information.  Accessors inside 'Datum' class that need to
// return a user-defined object, return an instance of 'Udt'.
//
///Usage
///-----
// The following snippets of code illustrate how to create and use a 'Udt'
// object.  For sake of brevity we use a fake address in this example.  Note
// that in real code you would be using the address of some object.  Also note
// that since the 'Udt' type makes no assumptions about the pointer to the data
// and it makes no access to the pointed data; the following code is valid.
//..
//  void *ptr = reinterpret_cast<void *>(0xDEADBEEF);
//  const int type = 3;
//  const Udt obj(ptr, type);
//  ASSERT(obj.data() == ptr && obj.type() == type);
//..
                                 // =========
                                 // class Udt
                                 // =========
class Udt {
    // This class provides a type to represent a pointer to an object of some
    // user-defined type and an integer value denoting what actual type is
    // represented.  This class has an implicitly defined (trivial) destructor.

  private:
    // DATA
    void *d_data_p;  // pointer to an object of some type (not owned)
    int   d_type;    // which (user-defined) type is represented

  public:
    // CREATORS
    Udt(void *data, int type);
        // Create a 'Udt' object having the specified 'data' and 'type'.  Note
        // that the pointer to the 'data' is just copied bitwise; the memory
        // pointed by 'data' is never accessed by a 'Udt' object.

    // ACCESSORS
    void *data() const;
        // Return pointer to the user-defined object.  Note that the pointer to
        // the 'data' is just copied bitwise; the memory pointed by 'data' is
        // never accessed by a 'Udt' object.

    int type() const;
        // Return the type of the user-defined object.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // (absolute value of) the optionally specified indentation 'level'
        // and return a reference to 'stream'.  If 'level' is specified,
        // optionally specify 'spacesPerLevel', the number of spaces per
        // indentation level for this and all of its nested objects.  If
        // 'level' is negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, format the entire output on one
        // line, suppressing all but the initial indentation (as governed by
        // 'level').  If 'stream' is not valid on entry, this operation has no
        // effect.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(
                           Udt, BloombergLP::bslalg::TypeTraitBitwiseCopyable);
        // 'Udt' values are bitwise copyable.
};

// FREE OPERATORS
bool operator==(const Udt& lhs, const Udt& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.  Two 'Udt' objects have the same value if they have
    // the same data and type values.

bool operator!=(const Udt& lhs, const Udt& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have different values,
    // and 'false' otherwise.  Two 'Udt' objects have different values if they
    // have different data or type values.

bool operator<(const Udt& lhs, const Udt& rhs);
    // Return 'true' if value of the specified 'lhs' is less than value of the
    // specified 'rhs' and 'false' otherwise.  Value of 'lhs' is less than
    // value of 'rhs', if data value of 'lhs' is less than data value of 'rhs',
    // or they have the same data value and type value of 'lhs' is less than
    // type value of 'rhs'.

bool operator<=(const Udt& lhs, const Udt& rhs);
    // Return 'true' if value of the specified 'lhs' is less than or equal to
    // value of the specified 'rhs' and 'false' otherwise.  Value of 'lhs' is
    // less than or equal to value of 'rhs', if data value of 'lhs' is less
    // than or equal to data value of 'rhs', or they have the same data value
    // and type value of 'lhs' is less than or equal to type value of 'rhs'.

bool operator>(const Udt& lhs, const Udt& rhs);
    // Return 'true' if value of the specified 'lhs' is greater than value of
    // the specified 'rhs' and 'false' otherwise.  Value of 'lhs' is greater
    // than value of 'rhs', if data value of 'lhs' is greater than data value
    // of 'rhs', or they have the same data value and type value of 'lhs' is
    // greater than type value of 'rhs'.

bool operator>=(const Udt& lhs, const Udt& rhs);
    // Return 'true' if value of the specified 'lhs' is greater than or equal
    // to value of the specified 'rhs' and 'false' otherwise.  Value of 'lhs'
    // is greater than or equal to value of 'rhs', if data value of 'lhs' is
    // greater than or equal to data value of 'rhs', or they have the same data
    // value and type value of 'lhs' is greater than or equal to type value of
    // 'rhs'.

bsl::ostream& operator<<(bsl::ostream& output, const Udt& rhs);
    // Write the specified 'rhs' value to the specified output 'stream' in
    // the format shown below:
    //..
    //  user-defined(0xhhhhhhhh,type) - 0xhhhhhhhh is the address held in this
    //                                  object and type is the integer type
    //                                  value
    //..
    // and return a reference to the modifiable 'stream'.  The function will
    // have no effect if the specified 'stream' is not valid.

                          // ========================
                          // class ConstDatumArrayRef
                          // ========================

class ConstDatumArrayRef {
    // This class provides a read-only view to an array of datums.  It
    // It holds the array by a 'const' pointer and an integral length value. It
    // acts as return value for accessors inside the 'Datum' class which return
    // an array of datums.  Note that zero-length arrays are valid.
    // Note that this class has an implicitly defined (trivial) destructor.

  private:
    // DATA
    const Datum            *d_data_p;  // address of the first datum
                                       // within an array of datums
                                       // (not owned)
    bsls::Types::size_type  d_length;  // length of the array of 'Datum'
                                       // objects

  public:
    // CREATORS
    ConstDatumArrayRef();
        // Create a 'ConstDatumArrayRef' object representing an empty array.

    ConstDatumArrayRef(const Datum            *data,
                       bsls::Types::size_type  length);
        // Create a 'ConstDatumArrayRef' object having the specified 'data' and
        // 'length'.  The behavior is undefined unless
        // '0 != data' or '0 == length'.  Note that the pointer to the array is
        // just copied.

    // ACCESSORS
    const Datum& operator[](bsls::Types::size_type index) const;
        // Return the element stored at the specified 'index' position in this
        // array.

    const Datum *data() const;
        // Return pointer to the first element in the array.

    bsls::Types::size_type length() const;
        // Return the length of the array.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // (absolute value of) the optionally specified indentation 'level'
        // and return a reference to 'stream'.  If 'level' is specified,
        // optionally specify 'spacesPerLevel', the number of spaces per
        // indentation level for this and all of its nested objects.  If
        // 'level' is negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, format the entire output on one
        // line, suppressing all but the initial indentation (as governed by
        // 'level').  If 'stream' is not valid on entry, this operation has no
        // effect.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(ConstDatumArrayRef,
                                 bslalg::TypeTraitBitwiseCopyable);
        // 'ConstDatumArrayRef' values are bitwise copyable.
};

// FREE OPERATORS
bool operator==(const ConstDatumArrayRef& lhs,
                const ConstDatumArrayRef& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.  Two 'ConstDatumArrayRef' objects have the same value
    // if they hold arrays of the same length and all the corresponding 'Datum'
    // objects in the two arrays also compare equal.

bool operator!=(const ConstDatumArrayRef& lhs,
                const ConstDatumArrayRef& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have different values,
    // and 'false' otherwise.  Two 'ConstDatumArrayRef' objects have different
    // values if they hold arrays of different lengths or invoking operator
    // '==' returns false for at least one of the corresponding elements in the
    // arrays.

bsl::ostream& operator<<(bsl::ostream&             output,
                         const ConstDatumArrayRef& rhs);
    // Write the specified 'rhs' value to the specified output 'stream' in
    // the format shown below:
    //..
    //  [aa,bb,cc] - aa, bb and cc are the result of invoking operator '<<'
    //               on the individual elements in the array
    //..
    // and return a reference to the modifiable 'stream'.  The function will
    // have no effect if the specified 'stream' is not valid.

                          // =========================
                          // class ConstDatumBinaryRef
                          // =========================

class ConstDatumBinaryRef {
    // This class provides a read-only view to an array of bytes.  It
    // It holds the array by a 'const' pointer and an integral size value. It
    // acts as return value for accessors inside the 'Datum' class which return
    // a binary datum.  Note that zero-size arrays are valid.
    // Note that this class has an implicitly defined (trivial) destructor.

  private:
    // DATA
    const void             *d_data_p; // address of the binary data (not owned)
    bsls::Types::size_type  d_size;   // size of the binary data

  public:
    // CREATORS
    ConstDatumBinaryRef();
        // Create a 'ConstDatumBinaryRef' object representing an empty array.

    ConstDatumBinaryRef(const void             *data,
                       bsls::Types::size_type  size);
        // Create a 'ConstDatumBinaryRef' object having the specified 'data'
        // and 'size'.  The behavior is undefined unless '0 != data' or
        // '0 == size'. Note that the pointer to the array is just copied.

    // ACCESSORS
    const void *data() const;
        // Return pointer to the binary data.

    bsls::Types::size_type size() const;
        // Return the size of the binary data.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // (absolute value of) the optionally specified indentation 'level'
        // and return a reference to 'stream'.  If 'level' is specified,
        // optionally specify 'spacesPerLevel', the number of spaces per
        // indentation level for this and all of its nested objects.  If
        // 'level' is negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, format the entire output on one
        // line, suppressing all but the initial indentation (as governed by
        // 'level').  If 'stream' is not valid on entry, this operation has no
        // effect.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(ConstDatumBinaryRef,
                                 bslalg::TypeTraitBitwiseCopyable);
        // 'ConstDatumBinaryRef' values are bitwise copyable.
};

// FREE OPERATORS
bool operator==(const ConstDatumBinaryRef& lhs,
                const ConstDatumBinaryRef& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.  Two 'ConstDatumBinaryRef' objects have the same
    // value if they refer to arrays of bytes of the same size and having the
    // same content.

bool operator!=(const ConstDatumBinaryRef& lhs,
                const ConstDatumBinaryRef& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have different values,
    // and 'true' otherwise.  Two 'ConstDatumBinaryRef' objects have different
    // values if they refer to arrays of bytes of different sizes or having a
    // different content.

bool operator<(const ConstDatumBinaryRef& lhs,
               const ConstDatumBinaryRef& rhs);
    // Return 'true' if the specified 'lhs' compares smaller than the specified
    // 'rhs' have the same value, and 'false' otherwise. A byte comparision is
    // performed using memcpy.

bool operator>(const ConstDatumBinaryRef& lhs,
               const ConstDatumBinaryRef& rhs);
    // Return 'true' if the specified 'lhs' compares larger than the specified
    // 'rhs' have the same value, and 'false' otherwise. A byte comparision is
    // performed using memcpy.

bool operator<=(const ConstDatumBinaryRef& lhs,
                const ConstDatumBinaryRef& rhs);
    // Return 'true' if the specified 'lhs' compares smaller than or equal to
    // the specified 'rhs', and 'false' otherwise. A byte comparision is
    // performed using memcpy.

bool operator>=(const ConstDatumBinaryRef& lhs,
                const ConstDatumBinaryRef& rhs);
    // Return 'true' if the specified 'lhs' compares larger than or equal to
    // the specified 'rhs', and 'false' otherwise. A byte comparision is
    // performed using memcpy.

bsl::ostream& operator<<(bsl::ostream&             output,
                         const ConstDatumBinaryRef& rhs);
    // Write the specified 'rhs' value to the specified output 'stream' as a
    // hexadecimal dump.

                            // ===================
                            // class DatumMapEntry
                            // ===================

class DatumMapEntry {
    // This class represents an entry inside a map of datums keyed by
    // string values.  Note that this class has an implicitly defined (trivial)
    // destructor.

  private:
    // DATA
    bslstl::StringRef d_key_p;  // key for this entry (not owned)
    Datum             d_value;  // value for this entry

  public:
    // CREATORS
    DatumMapEntry();
        // Create a 'DatumMapEntry' object.

    DatumMapEntry(const bslstl::StringRef& key, const Datum& value);
        // Create a 'DatumMapEntry' object using the specified 'key' and
        // 'value'.

    // MANIPULATORS
    void setKey(const bslstl::StringRef& key);
        // Set the key for this entry to the specified 'key'.

    void setValue(const Datum& value);
        // Set the value for this entry to the specified 'value'.

    // ACCESSORS
    const bslstl::StringRef& key() const;
        // Return the key for this entry.

    const Datum& value() const;
        // Return the value for this entry.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // (absolute value of) the optionally specified indentation 'level'
        // and return a reference to 'stream'.  If 'level' is specified,
        // optionally specify 'spacesPerLevel', the number of spaces per
        // indentation level for this and all of its nested objects.  If
        // 'level' is negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, format the entire output on one
        // line, suppressing all but the initial indentation (as governed by
        // 'level').  If 'stream' is not valid on entry, this operation has no
        // effect.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(DatumMapEntry,
                                 bslalg::TypeTraitBitwiseCopyable);
        // 'DatumMapEntry' values are bitwise copyable.
};

// FREE OPERATORS
bool operator==(const DatumMapEntry& lhs,
                const DatumMapEntry& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have the same value,
    // and 'false' otherwise.  Two 'DatumMapEntry' objects have the same value
    // if their keys and values compare equal.

bool operator!=(const DatumMapEntry& lhs,
                const DatumMapEntry& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have different values,
    // and 'false' otherwise.  Two 'DatumMapEntry' objects have different
    // values if either the keys or values are not equal.

bsl::ostream& operator<<(bsl::ostream&        output,
                         const DatumMapEntry& rhs);
    // Write the specified 'rhs' value to the specified output 'stream' in
    // the format shown below:
    //..
    //  (abc,aa) - abc is key string, while aa is the result of invoking
    //             operator '<<' on the value
    //..
    // and return a reference to the modifiable 'stream'.  The function will
    // have no effect if the specified 'stream' is not valid.

                            // ======================
                            // class ConstDatumMapRef
                            // ======================

class ConstDatumMapRef {
    // This class provides a read-only view to a map of datums (an
    // array of 'DatumMapEntry' objects).  It holds the array by a 'const'
    // pointer and an integral size value.  It acts as return value for
    // accessors inside the 'Datum' class which return a map of 'Datum'
    // objects.  Note that zero-size maps are valid.  Note that this class has
    // an implicitly defined (trivial) destructor.

  private:
    // DATA
    const DatumMapEntry    *d_data_p;  // address of the first 'DatumMapEntry'
                                       // object within an array of
                                       // 'DatumMapEntry' objects (not owned)
    bsls::Types::size_type  d_size;    // length of the array of
                                       // 'DatumMapEntry' objects
    bool                    d_sorted;  // if the map is sorted or not

  public:
    // CREATORS
    ConstDatumMapRef(const DatumMapEntry    *data,
                     bsls::Types::size_type  size,
                     bool                    sorted);
        // Create a 'ConstDatumMapRef' object having the specified 'data',
        // 'size' and 'sorted'.  The behavior is undefined unless
        // '0 != data' or '0 == size'.  Note that the pointer to the array is
        // just copied.

    // ACCESSORS
    const DatumMapEntry& operator[](bsls::Types::size_type index) const;
        // Return the element stored at the specified 'index' position in this
        // map.

    const DatumMapEntry *data() const;
        // Return pointer to the first element in the map.

    bool isSorted() const;
        // Return 'true' if underlying map is sorted and 'false' otherwise.

    bsls::Types::size_type size() const;
        // Return the size of the map.

    const Datum *find(const bslstl::StringRef& key) const;
        // Return a const pointer to the datum having the specified
        // 'key', if it exists and 0 otherwise.  Note that the 'find' has order
        // of 'O(n)' if the data is not sorted based on the keys.  If the data
        // is sorted, it has order of 'O(log(n))'.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(ConstDatumMapRef,
                                 bslalg::TypeTraitBitwiseCopyable);
        // 'ConstDatumMapRef' values are bitwise copyable.
};

// FREE OPERATORS
bool operator==(const ConstDatumMapRef& lhs,
                const ConstDatumMapRef& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.  Two 'ConstDatumMapRef' objects have the same value
    // if they hold maps of the same size and all the corresponding
    // 'DatumMapEntry' elements in the two maps also compare equal.

bool operator!=(const ConstDatumMapRef& lhs,
                const ConstDatumMapRef& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have different values,
    // and 'false' otherwise.  Two 'ConstDatumMapRef' objects have different
    // values if they hold maps of different sizes or operator '==' returns
    // false for at least one of the corresponding elements in the maps.

bsl::ostream& operator<<(bsl::ostream&           output,
                         const ConstDatumMapRef& rhs);
    // Write the specified 'rhs' value to the specified output 'stream' in the
    // format shown below:
    //..
    //  [(abc,aa),(pqr,bb)] - abc and pqr are key strings, while aa and bb are
    //                        the result of invoking operator '<<' on the
    //                        individual value elements in the map
    //..
    // and return a reference to the modifiable 'stream'.  The function will
    // have no effect if the specified 'stream' is not valid.

// ============================================================================
//                               INLINE DEFINITIONS
// ============================================================================

#if defined(BSLS_PLATFORM_CPU_32_BIT)

                           // ----------------------
                           // struct Datum_Helpers32
                           // ----------------------

struct Datum_Helpers32 {
    // This struct contains helper functions used in the 32-bit variation.  The
    // functions are for internal use only and may change or disappear without
    // notice.

    // CLASS DATA
#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
    static const int b00          = 0; // Bits 0 to 32.
    static const int b32          = 4; // Bits 32 to 48.
    static const int b48          = 6; // Bits 48 to 64.
#else
    static const int b00          = 4;
    static const int b32          = 2;
    static const int b48          = 0;
#endif

    // CLASS METHODS
    static bsls::Types::Int64 loadSmallInt64(short s, int i);
        // Load an Int64 from short 's' and int 'i' values created by
        // storeSmallInt64.  This method is public for testing purpose only. It
        // may change or be removed without notice.
    static bool storeSmallInt64(bsls::Types::Int64  value,
                                short              *ps,
                                int                *pl);
        // Store an Int64 in short at 'ps' and int at 'pi'. Return true if it
        // fits.  This method is public for testing purpose only. It may change
        // or be removed without notice.
};

// CLASS METHODS
BDLD_DATUM_FORCE_INLINE
bsls::Types::Int64 Datum_Helpers32::loadSmallInt64(short up16, int lo32)
{
    bsls::Types::Int64 value;
    unsigned char *pv = reinterpret_cast<unsigned char*>(&value);

    if ((*reinterpret_cast<short*>(pv + b32) = up16) < 0) {
        *reinterpret_cast<short*>(pv + b48) = -1;
    } else {
        *reinterpret_cast<short*>(pv + b48) = 0;
    }

    *reinterpret_cast<long*>(pv + b00) = lo32;

    return value;
}

BDLD_DATUM_FORCE_INLINE
bool Datum_Helpers32::storeSmallInt64(bsls::Types::Int64  value,
                                      short              *ps,
                                      int                *pl)
{
    const unsigned char *pv = reinterpret_cast<const unsigned char*>(&value);

    // Check that the sign can be infered from the compressed 6-byte integer.
    // It is the case if the upper 16 bits are the same as the 17th bit.

    if ((*reinterpret_cast<const short*>(pv + b48) == 0
         && *reinterpret_cast<const short*>(pv + b32) >= 0)
        || (*reinterpret_cast<const short*>(pv + b48) == -1
            && *reinterpret_cast<const short*>(pv + b32) < 0)) {
        *pl = *reinterpret_cast<const long*>(pv + b00);
        *ps = *reinterpret_cast<const short*>(pv + b32);
        return true;
    } else {
        return false;
    }
}

                                // -----------
                                // class Datum
                                // -----------

                             // 32-bits variation

// PRIVATE CLASS METHODS
BDLD_DATUM_FORCE_INLINE
Datum Datum::createExtendedDataObject(ExtendedInternalDataType  type,
                                      void                     *data)
{
    Datum result;
    result.d_exp.d_value =
        (DOUBLE_MASK | e_INTERNAL_EXTENDED) << BDLD_TYPE_MASK_BITS | type;
    result.d_as.d_cvp = data;
    return result;
}

BDLD_DATUM_FORCE_INLINE
Datum Datum::createExtendedDataObject(ExtendedInternalDataType  type,
                                      int                       data)
{
    Datum result;
    result.d_exp.d_value =
        (DOUBLE_MASK | e_INTERNAL_EXTENDED) << BDLD_TYPE_MASK_BITS | type;
    result.d_as.d_int = data;
    return result;
}

BDLD_DATUM_FORCE_INLINE
void Datum::destroyMemory(const Datum&  value,
                          bslma::Allocator *basicAllocator)
{
    // NOTE: Call destructor of a type, if the destructor is not trivial.
    basicAllocator->deallocate(const_cast<void*>(value.d_as.d_cvp));
}

// CLASS METHODS
BDLD_DATUM_FORCE_INLINE
Datum Datum::createNull()
{
    Datum result;

    // Setting exponent using half-word is faster, maybe the compiler folds the
    // two statements into one?

    result.d_as.d_exponent = DOUBLE_MASK | Datum::e_INTERNAL_EXTENDED;
    result.d_as.d_ushort = e_EXTENDED_INTERNAL_NIL;
    return result;
}

BDLD_DATUM_FORCE_INLINE
Datum Datum::createBoolean(bool value)
{
    Datum result;
    result.d_exp.d_value = (DOUBLE_MASK | e_INTERNAL_BOOLEAN)
        << BDLD_TYPE_MASK_BITS;
    result.d_as.d_int = value;
    return result;
}

BDLD_DATUM_FORCE_INLINE
Datum Datum::createInteger(int value)
{
    Datum result;
    result.d_exp.d_value = (DOUBLE_MASK | e_INTERNAL_INTEGER)
        << BDLD_TYPE_MASK_BITS;
    result.d_as.d_int = value;
    return result;
}

BDLD_DATUM_FORCE_INLINE
Datum Datum::createDouble(double value)
{
    Datum result;
    if (!(value == value)) {
        return createExtendedDataObject(e_EXTENDED_INTERNAL_NAN2, 0); // RETURN
    }
    else {
        result.d_double = value;
    }
    return result;
}

BDLD_DATUM_FORCE_INLINE
Datum Datum::createStringRef(const char       *value,
                             size_type         length,
                             bslma::Allocator *basicAllocator)
{
    BSLS_ASSERT_SAFE(value || 0 == length);
    BSLS_ASSERT_SAFE(basicAllocator);


    // If the length will fix in the 'SHORT_OFFSET' area, store everything
    // inline; otherwise, must allocate space.

    if (bsl::numeric_limits<unsigned short>::max() >= length) {
        Datum result;
        // result.d_as.d_exponent = DOUBLE_MASK | e_INTERNAL_STRING_REFERENCE;
        // result.d_as.d_ushort = length;
        result.d_exp.d_value =
            (DOUBLE_MASK | e_INTERNAL_STRING_REFERENCE) << BDLD_TYPE_MASK_BITS
            | length;
        result.d_as.d_cvp = value;
        return result;                                                // RETURN
    }

    void *mem = basicAllocator->allocate(sizeof(length) + sizeof(value));
    *reinterpret_cast<const char **>(mem) = value;
    *reinterpret_cast<size_type *>(static_cast<char *>(mem) + sizeof(value))
        = length;

    return createExtendedDataObject(e_EXTENDED_INTERNAL_SREF_ALLOC, mem);
}

BDLD_DATUM_FORCE_INLINE
Datum Datum::createDate(const bdlt::Date& value)
{
    Datum result;
    BSLMF_ASSERT(sizeof(value) == sizeof(result.d_as.d_int));
    BSLMF_ASSERT((bslalg::HasTrait<bdlt::Date,
                  bslalg::TypeTraitBitwiseCopyable>::VALUE));
    result.d_exp.d_value =
        (DOUBLE_MASK | e_INTERNAL_DATE) << BDLD_TYPE_MASK_BITS;
    *reinterpret_cast<bdlt::Date*>(&result.d_as.d_int) = value;
    return result;
}

BDLD_DATUM_FORCE_INLINE
Datum Datum::createTime(const bdlt::Time& value)
{
    BSLMF_ASSERT((bslalg::HasTrait<bdlt::Time,
                  bslalg::TypeTraitBitwiseCopyable>::VALUE));
    Datum result;
    result.d_exp.d_value =
        (DOUBLE_MASK | e_INTERNAL_TIME) << BDLD_TYPE_MASK_BITS;
    *reinterpret_cast<bdlt::Time*>(&result.d_as.d_int) = value;
    return result;
}

inline // MSVC2013: forceinline degrades performance
Datum Datum::createDatetime(const bdlt::Datetime&  value,
                            bslma::Allocator     *basicAllocator)
{
    BSLS_ASSERT(basicAllocator);

    Datum result;

    // Check if number of days from now fits in two bytes.

    int dateOffsetFromNow = value.date() - s_dateTimeBase;
    short shortDateOffsetFromNow = static_cast<short>(dateOffsetFromNow);

    if (static_cast<int>(shortDateOffsetFromNow) == dateOffsetFromNow) {
        result.d_exp.d_value =
            (DOUBLE_MASK | e_INTERNAL_DATETIME) << BDLD_TYPE_MASK_BITS
            | (0xffff & shortDateOffsetFromNow);
        *reinterpret_cast<bdlt::Time*>(&result.d_as.d_int) = value.time();
    } else {
        void *mem = new (*basicAllocator) bdlt::Datetime(value);
        result = createExtendedDataObject(e_EXTENDED_INTERNAL_DATETIME_ALLOC,
                                          mem);
    }

    return result;
}

BDLD_DATUM_FORCE_INLINE
Datum Datum::createDatetimeInterval(
                                  const bdlt::DatetimeInterval&  value,
                                  bslma::Allocator             *basicAllocator)
{
    BSLS_ASSERT(basicAllocator);

    bsls::Types::Int64 msValue = value.totalMilliseconds();
    Datum result;

    if (Datum_Helpers32::storeSmallInt64(msValue,
                                         &result.d_as.d_short,
                                         &result.d_as.d_int)) {
        result.d_as.d_exponent = DOUBLE_MASK | e_INTERNAL_DATETIME_INTERVAL;
    } else {
        void *mem = new (*basicAllocator) bdlt::DatetimeInterval(value);
        result = createExtendedDataObject(
                                   e_EXTENDED_INTERNAL_DATETIME_INTERVAL_ALLOC,
                                   mem);
    }

    return result;
}

BDLD_DATUM_FORCE_INLINE
Datum Datum::createError(int code)
{
    return createExtendedDataObject(e_EXTENDED_INTERNAL_ERROR, code);
}

inline
Datum Datum::createUdt(void *data, int type)
{
    BSLS_ASSERT_SAFE(0 <= type && type <= 65535);
    Datum result;
    result.d_as.d_exponent = DOUBLE_MASK | e_INTERNAL_USERDEFINED;
    result.d_as.d_ushort = type;
    result.d_as.d_cvp = data;
    return result;
}

BDLD_DATUM_FORCE_INLINE
Datum Datum::createArrayReference(const Datum      *array,
                                  size_type         length,
                                  bslma::Allocator *basicAllocator)
{
    BSLS_ASSERT_SAFE(array || 0 == length);
    BSLS_ASSERT_SAFE(basicAllocator);

    // If the length will fit in the 'd_ushort' area, store everything inline;
    // otherwise, must allocate space.

    if (bsl::numeric_limits<unsigned short>::max() >= length) {
        Datum result;
        result.d_as.d_exponent = DOUBLE_MASK | e_INTERNAL_ARRAY_REFERENCE;
        result.d_as.d_ushort = length;
        result.d_as.d_cvp = array;
        return result;                                                // RETURN
    }

    char *mem = static_cast<char *>(
                     basicAllocator->allocate(sizeof(length) + sizeof(array)));
    *reinterpret_cast<const Datum **>(mem) = array;
    *reinterpret_cast<size_type *>(mem + sizeof(array)) = length;

    return createExtendedDataObject(e_EXTENDED_INTERNAL_AREF_ALLOC, mem);
}

BDLD_DATUM_FORCE_INLINE
Datum Datum::adoptMap(const DatumMapRef& mapping)
{
    return createExtendedDataObject(e_EXTENDED_INTERNAL_MAP, mapping.size());
}

BDLD_DATUM_FORCE_INLINE
Datum Datum::adoptMapOwningKeys(const DatumMapOwningKeysRef& mapping)
{
    return createExtendedDataObject(e_EXTENDED_INTERNAL_OWNED_MAP,
                                    mapping.size());
}

// PRIVATE ACCESSORS
BDLD_DATUM_FORCE_INLINE
Datum::InternalDataType Datum::internalType() const
{
    if (0x7f == d_data[EXPONENT_MSB] &&
        0xf0 == (d_data[EXPONENT_LSB] & 0xf0)) {
        return static_cast<InternalDataType>(                         // RETURN
            d_data[EXPONENT_LSB] & 0x0f);
    }
    return e_INTERNAL_REAL;
}

BDLD_DATUM_FORCE_INLINE
Datum::ExtendedInternalDataType Datum::extendedInternalType() const
{
    BSLS_ASSERT_SAFE(e_INTERNAL_EXTENDED == internalType());
    return static_cast<ExtendedInternalDataType>(d_as.d_short);
}

BDLD_DATUM_FORCE_INLINE
Datum::DataType Datum::typeFromExtendedInternalType() const
{
    BSLS_ASSERT_SAFE(e_INTERNAL_EXTENDED == internalType());

    static const DataType convert[] = {
        e_MAP         // e_EXTENDED_INTERNAL_MAP                     = 0
      , e_MAP         // e_EXTENDED_INTERNAL_OWNED_MAP               = 1
      , e_REAL        // e_EXTENDED_INTERNAL_NAN2                    = 2
      , e_ERROR_VALUE // e_EXTENDED_INTERNAL_ERROR                   = 3
      , e_ERROR_VALUE // e_EXTENDED_INTERNAL_ERROR_ALLOC             = 4
      , e_STRING      // e_EXTENDED_INTERNAL_SREF_ALLOC              = 5
      , e_ARRAY       // e_EXTENDED_INTERNAL_AREF_ALLOC              = 6
      , e_DATETIME    // e_EXTENDED_INTERNAL_DATETIME_ALLOC          = 7
      , e_DATETIME_INTERVAL
                      // e_EXTENDED_INTERNAL_DATETIME_INTERVAL_ALLOC = 8
      , e_INTEGER64   // e_EXTENDED_INTERNAL_INTEGER64_ALLOC         = 9
      , e_BINARY      // e_EXTENDED_INTERNAL_BINARY_ALLOC            = 10
      , e_DECIMAL64   // e_EXTENDED_INTERNAL_DECIMAL64               = 11
      , e_DECIMAL64   // e_EXTENDED_INTERNAL_DECIMAL64_SPECIAL       = 12
      , e_DECIMAL64   // e_EXTENDED_INTERNAL_DECIMAL64_ALLOC         = 13
      , e_NIL         // e_EXTENDED_INTERNAL_NIL                     = 14
    };

    const ExtendedInternalDataType type = extendedInternalType();
    const int convertLength = sizeof(convert) / sizeof(convert[0]);
    BSLS_ASSERT_OPT(type < convertLength);

    return convert[type];
}

BDLD_DATUM_FORCE_INLINE
bsls::Types::Int64 Datum::theSmallInteger64() const
{
    BSLS_ASSERT_SAFE(internalType() == e_INTERNAL_INTEGER64);
    return Datum_Helpers32::loadSmallInt64(d_as.d_short, d_as.d_int);
}

BDLD_DATUM_FORCE_INLINE
bsls::Types::Int64 Datum::theLargeInteger64() const
{
    BSLS_ASSERT_SAFE(internalType() == e_INTERNAL_EXTENDED);
    BSLS_ASSERT_SAFE(
        extendedInternalType() == e_EXTENDED_INTERNAL_INTEGER64_ALLOC);
    return *static_cast<const bsls::Types::Int64*>(d_as.d_cvp);
}

BDLD_DATUM_FORCE_INLINE
bslstl::StringRef Datum::theShortString() const
{
    return bslstl::StringRef(d_string5.d_chars, d_string5.d_length);
}

BDLD_DATUM_FORCE_INLINE
bslstl::StringRef Datum::theLongestShortString() const
{
    return bslstl::StringRef(d_string6.d_chars, sizeof d_string6.d_chars);
}

BDLD_DATUM_FORCE_INLINE
bslstl::StringRef Datum::theInternalString() const
{
    const char *data = static_cast<const char *>(d_as.d_cvp);
    return bslstl::StringRef(data + sizeof(int),
                             *reinterpret_cast<const int *>(data));
}

BDLD_DATUM_FORCE_INLINE
bslstl::StringRef Datum::theLongStringReference() const
{
    const char *data = static_cast<const char*>(d_as.d_cvp);
    return bslstl::StringRef(
                    *reinterpret_cast<char *const *>(data),
                    *reinterpret_cast<const size_type *>(data + sizeof(data)));
}

BDLD_DATUM_FORCE_INLINE
ConstDatumArrayRef Datum::theInternalArray() const
{
    const Datum *arr = static_cast<const Datum *>(d_as.d_cvp);
    if (arr) {
        return ConstDatumArrayRef(
            arr + 1,
            *reinterpret_cast<const size_type *>(arr));               // RETURN
    }
    return ConstDatumArrayRef(0, 0);
}

BDLD_DATUM_FORCE_INLINE
ConstDatumArrayRef Datum::theLongArrayReference() const
{
    const char *data = static_cast<const char *>(d_as.d_cvp);
    return ConstDatumArrayRef(
           *reinterpret_cast<Datum *const *>(data),
           *reinterpret_cast<const size_type *>(data + sizeof(const Datum *)));
}

BDLD_DATUM_FORCE_INLINE
ConstDatumArrayRef Datum::theArrayReference() const
{
    return ConstDatumArrayRef(static_cast<Datum const *>(d_as.d_cvp),
                              d_as.d_ushort);
}

// CLASS METHODS
BDLD_DATUM_FORCE_INLINE
Datum Datum::adoptArray(const DatumArrayRef& array)
{
    Datum result;
    result.d_as.d_exponent = DOUBLE_MASK | e_INTERNAL_ARRAY;
    result.d_as.d_cvp = array.length();
    return result;
}

// ACCESSORS
BDLD_DATUM_FORCE_INLINE
Datum::DataType Datum::type() const
{
    static const DataType convert[] = {
          e_REAL                  // e_INTERNAL_INF                 = 0x00
        , e_STRING                // e_INTERNAL_LONGEST_SHORTSTRING = 0x01
        , e_BOOLEAN               // e_INTERNAL_BOOLEAN             = 0x02
        , e_STRING                // e_INTERNAL_SHORTSTRING         = 0x03
        , e_STRING                // e_INTERNAL_STRING              = 0x04
        , e_DATE                  // e_INTERNAL_DATE                = 0x05
        , e_TIME                  // e_INTERNAL_TIME                = 0x06
        , e_DATETIME              // e_INTERNAL_DATETIME            = 0x07
        , e_DATETIME_INTERVAL     // e_INTERNAL_DATETIME_INTERVAL   = 0x08
        , e_INTEGER               // e_INTERNAL_INTEGER             = 0x09
        , e_INTEGER64             // e_INTERNAL_INTEGER64           = 0x0a
        , e_USERDEFINED           // e_INTERNAL_USERDEFINED         = 0x0b
        , e_ARRAY                 // e_INTERNAL_ARRAY               = 0x0c
        , e_STRING                // e_INTERNAL_STRING_REFERENCE    = 0x0d
        , e_ARRAY                 // e_INTERNAL_ARRAY_REFERENCE     = 0x0e
        , k_NUM_TYPES             // ---------------------------    = 0x0f
        , e_REAL                  // e_INTERNAL_REAL                = 0x10
    };

    const InternalDataType type = internalType();
    if (e_INTERNAL_EXTENDED == type) {
        return typeFromExtendedInternalType();                        // RETURN
    }
    return convert[type];
}

BDLD_DATUM_FORCE_INLINE
bool Datum::theBoolean() const
{
    BSLS_ASSERT_SAFE(isBoolean());
    return static_cast<bool>(d_as.d_int);
}

BDLD_DATUM_FORCE_INLINE
int Datum::theInteger() const
{
    BSLS_ASSERT_SAFE(isInteger());
    return d_as.d_int;
}

BDLD_DATUM_FORCE_INLINE
bsls::Types::Int64 Datum::theInteger64() const
{
    BSLS_ASSERT_SAFE(isInteger64());

    const InternalDataType type = internalType();

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(e_INTERNAL_INTEGER64 == type)) {
        return theSmallInteger64();                                   // RETURN
    }
    else {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BSLS_ASSERT_SAFE(e_INTERNAL_EXTENDED == type);
        BSLS_ASSERT_SAFE(
                e_EXTENDED_INTERNAL_INTEGER64_ALLOC == extendedInternalType());
        return theLargeInteger64();                                   // RETURN
    }
}

BDLD_DATUM_FORCE_INLINE
double Datum::theDouble() const
{
    BSLS_ASSERT_SAFE(isDouble());

    if (0x7f != d_data[EXPONENT_MSB] ||      // exponent is not the
        0xf0 != (d_data[EXPONENT_LSB] & 0xf0) || // special '7ff' value
        e_INTERNAL_INF == (d_data[EXPONENT_LSB] & 0x0f)) { // or infinity
        return d_double;                                              // RETURN
    }
    return bsl::numeric_limits<double>::quiet_NaN();
}

BDLD_DATUM_FORCE_INLINE
bslstl::StringRef Datum::theString() const
{
    BSLS_ASSERT_SAFE(isString());
    const InternalDataType type = internalType();
    switch(type) {
      case e_INTERNAL_SHORTSTRING:
          return theShortString();                                    // RETURN
      case e_INTERNAL_LONGEST_SHORTSTRING:
          return theLongestShortString();                             // RETURN
      case e_INTERNAL_STRING:
          return theInternalString();                                 // RETURN
      case e_INTERNAL_STRING_REFERENCE:
          return theStringReference();                                // RETURN
        // This case is not used with the 64-bit implementation.  Here, we can
        // assume that we have an allocated string reference.
      default:
          return theLongStringReference();                            // RETURN
    }
}

BDLD_DATUM_FORCE_INLINE
bslstl::StringRef Datum::theStringReference() const
{
    return bslstl::StringRef(static_cast<const char *>(d_as.d_cvp),
                             d_as.d_ushort);
}

BDLD_DATUM_FORCE_INLINE
bdlt::Date Datum::theDate() const
{
    BSLS_ASSERT_SAFE(isDate());
    return *reinterpret_cast<const bdlt::Date *>(&d_as.d_int);
}

BDLD_DATUM_FORCE_INLINE
bdlt::Time Datum::theTime() const
{
    BSLS_ASSERT_SAFE(isTime());
    return *reinterpret_cast<const bdlt::Time *>(&d_as.d_int);
}

inline // BDLD_DATUM_FORCE_INLINE
bdlt::Datetime Datum::theDatetime() const
{
    const InternalDataType type = internalType();

    if (type == e_INTERNAL_DATETIME) {
        return bdlt::Datetime(
            s_dateTimeBase
            + d_as.d_short,
            *reinterpret_cast<const bdlt::Time*>(&d_as.d_int));        // RETURN
    }

    if (type == e_INTERNAL_EXTENDED) {
        BSLS_ASSERT_SAFE(
                 extendedInternalType() == e_EXTENDED_INTERNAL_DATETIME_ALLOC);
        return *static_cast<const bdlt::Datetime*>(d_as.d_cvp);        // RETURN
    }

    BSLS_ASSERT_SAFE(!"UNKNOWN TYPE");
    return bdlt::Datetime(); // avoid warning on Solaris
}

inline // BDLD_DATUM_FORCE_INLINE
bdlt::DatetimeInterval Datum::theDatetimeInterval() const
{
    const InternalDataType type = internalType();

    if (type == e_INTERNAL_DATETIME_INTERVAL) {
        bdlt::DatetimeInterval result;
        result.setTotalMilliseconds(
            Datum_Helpers32::loadSmallInt64(d_as.d_short, d_as.d_int));
        return result;                                                // RETURN
    }

    if (type == e_INTERNAL_EXTENDED) {
        BSLS_ASSERT(
        extendedInternalType() == e_EXTENDED_INTERNAL_DATETIME_INTERVAL_ALLOC);
        return *static_cast<const bdlt::DatetimeInterval *>(d_as.d_cvp);
                                                                      // RETURN
    }

    BSLS_ASSERT_SAFE(!"UNKNOWN TYPE");
    return bdlt::DatetimeInterval(); // avoid warning on Solaris
}

BDLD_DATUM_FORCE_INLINE
Error Datum::theError() const
{
    BSLS_ASSERT_SAFE(isError());

    // If the extended type is 'e_EXTENDED_INTERNAL_ERROR', we are storing
    // just a code, at the data offset.  Otherwise, we're storing an allocated
    // object.

    if (e_EXTENDED_INTERNAL_ERROR == extendedInternalType()) {
        return Error(d_as.d_int);                                     // RETURN
    }

    const char *data = static_cast<const char *>(d_as.d_cvp);

    return Error(
                *reinterpret_cast<const int *>(data),
                bslstl::StringRef(data + 2 * sizeof(int),
                                  *(reinterpret_cast<const int *>(data) + 1)));
}

BDLD_DATUM_FORCE_INLINE
Udt Datum::theUdt() const
{
    BSLS_ASSERT_SAFE(isUdt());
    return Udt(const_cast<void*>(d_as.d_cvp), d_as.d_ushort);
}

BDLD_DATUM_FORCE_INLINE
ConstDatumBinaryRef Datum::theBinary() const
{
    BSLS_ASSERT_SAFE(isBinary());
    return ConstDatumBinaryRef(static_cast<const double *>(d_as.d_cvp) + 1,
                               *static_cast<const size_type *>(d_as.d_cvp));
}

BDLD_DATUM_FORCE_INLINE
ConstDatumArrayRef Datum::theArray() const
{
    BSLS_ASSERT_SAFE(isArray());

    const InternalDataType type = internalType();
    if (e_INTERNAL_ARRAY == type) {
        return theInternalArray();                                    // RETURN
    }

    // The external case is an option when on 32-bit platform only.

    if (e_INTERNAL_EXTENDED == type) {
        return theLongArrayReference();                               // RETURN
    }
    return theArrayReference();
}

BDLD_DATUM_FORCE_INLINE
ConstDatumMapRef Datum::theMap() const
{
    BSLS_ASSERT_SAFE(isMap());
    const DatumMapEntry *mapping =
        static_cast<const DatumMapEntry *>(d_as.d_cvp);
    if (mapping) {
        const DatumMapEntry *data = mapping + 1;
        const size_type size = *reinterpret_cast<const size_type *>(mapping);
        const bool sorted = *reinterpret_cast<const bool *>(data);
        return ConstDatumMapRef(data + 1, size, sorted);              // RETURN
    }
    return ConstDatumMapRef(0, 0, false);
}

template <typename BDLD_VISITOR>
void Datum::apply(BDLD_VISITOR& visitor) const
{
    switch (internalType()) {
      case e_INTERNAL_INF:
        visitor(bsl::numeric_limits<double>::infinity());
        break;
      case e_INTERNAL_BOOLEAN:
        visitor(theBoolean());
        break;
      case e_INTERNAL_SHORTSTRING:
        visitor(theShortString());
        break;
      case e_INTERNAL_LONGEST_SHORTSTRING:
        visitor(theLongestShortString());
        break;
      case e_INTERNAL_STRING:
        visitor(theInternalString());
        break;
      case e_INTERNAL_DATE:
        visitor(theDate());
        break;
      case e_INTERNAL_TIME:
        visitor(theTime());
        break;
      case e_INTERNAL_DATETIME:
        visitor(theDatetime());
        break;
      case e_INTERNAL_DATETIME_INTERVAL:
        visitor(theDatetimeInterval());
        break;
      case e_INTERNAL_INTEGER:
        visitor(theInteger());
        break;
      case e_INTERNAL_INTEGER64:
        visitor(theInteger64());
        break;
      case e_INTERNAL_USERDEFINED:
        visitor(theUdt());
        break;
      case e_INTERNAL_ARRAY:
        visitor(theInternalArray());
        break;
      case e_INTERNAL_STRING_REFERENCE:
        visitor(theStringReference());
        break;
      case e_INTERNAL_ARRAY_REFERENCE:
        visitor(theArrayReference());
        break;
      case e_INTERNAL_EXTENDED:
        switch (extendedInternalType()) {
          case e_EXTENDED_INTERNAL_MAP:
          case e_EXTENDED_INTERNAL_OWNED_MAP:  // fall through
            visitor(theMap());
            break;
          case e_EXTENDED_INTERNAL_NAN2:
            visitor(theDouble());
            break;
          case e_EXTENDED_INTERNAL_ERROR:
          case e_EXTENDED_INTERNAL_ERROR_ALLOC: // fall through
            visitor(theError());
            break;
          case e_EXTENDED_INTERNAL_SREF_ALLOC:
            visitor(theStringReference());
            break;
          case e_EXTENDED_INTERNAL_AREF_ALLOC:
            visitor(theLongArrayReference());
            break;
          case e_EXTENDED_INTERNAL_DATETIME_ALLOC:
            visitor(theDatetime());
            break;
          case e_EXTENDED_INTERNAL_DATETIME_INTERVAL_ALLOC:
            visitor(theDatetimeInterval());
            break;
          case e_EXTENDED_INTERNAL_INTEGER64_ALLOC:
            visitor(theInteger64());
            break;
          case e_EXTENDED_INTERNAL_BINARY_ALLOC:
            visitor(theBinary());
            break;
          case e_EXTENDED_INTERNAL_DECIMAL64:
          case e_EXTENDED_INTERNAL_DECIMAL64_SPECIAL:
          case e_EXTENDED_INTERNAL_DECIMAL64_ALLOC:
            visitor(theDecimal64());
            break;
          case e_EXTENDED_INTERNAL_NIL:
            visitor(bslmf::Nil());
            break;
          default:
            BSLS_ASSERT_SAFE(!"UNKNOWN TYPE");
        }
        break;
      case e_INTERNAL_REAL:
        visitor(d_double);
        break;
      default:
        BSLS_ASSERT_SAFE(!"Unknown type!!");
    }
}

#else // defined(BSLS_PLATFORM_CPU_32_BIT)

                                // -----------
                                // class Datum
                                // -----------

                              // 64-bit variation

// PRIVATE ACCESSORS
// 64-bit variation
inline
void* Datum::theInlineStorage()
{
    return d_storage.buffer();
}

inline
const void* Datum::theInlineStorage() const
{
    return d_storage.buffer();
}

inline
Datum::InternalDataType Datum::internalType() const
{
    return static_cast<InternalDataType>(d_as.d_type);
}

inline
bslstl::StringRef Datum::theShortString() const
{
    const char* str = reinterpret_cast<const char *>(theInlineStorage());
    const bsl::size_t len = *str++;
    return bslstl::StringRef(str, len);
}

// PRIVATE CLASS METHODS
// 64-bit variation
inline
Datum Datum::createDatum(InternalDataType type, void *data)
{
    Datum result;
    result.d_as.d_type = type;
    result.d_as.d_ptr = data;
    return result;
}

inline
Datum Datum::createDatum(InternalDataType type, int data)
{
    Datum result;
    result.d_as.d_type = type;
    result.d_as.d_int64 = data;
    return result;
}

inline
void Datum::destroyMemory(const Datum&  value,
                          bslma::Allocator *basicAllocator)
{
    basicAllocator->deallocate(value.d_as.d_ptr);
}

inline
bslstl::StringRef Datum::theInternalString() const
{
    return bslstl::StringRef(static_cast<const char *>(d_as.d_ptr),
                             d_as.d_int32);
}

inline
bslstl::StringRef Datum::theStringReference() const
{
    return bslstl::StringRef(static_cast<const char *>(d_as.d_ptr),
                             d_as.d_int32);
}

inline
ConstDatumArrayRef Datum::theInternalArray() const
{
    const Datum *arr = reinterpret_cast<Datum const *>(d_as.d_ptr);
    if (arr) {
        return ConstDatumArrayRef(
            arr + 1,
            *reinterpret_cast<const size_type *>(arr));               // RETURN
    }
    return ConstDatumArrayRef(0, 0);
}

inline
ConstDatumArrayRef Datum::theArrayReference() const
{
    return ConstDatumArrayRef(static_cast<const Datum *>(d_as.d_ptr),
                              d_as.d_int32);
}

// CLASS METHODS
// 64-bit variation
inline
Datum Datum::createNull()
{
    Datum result;
    result.d_as.d_type = e_INTERNAL_NIL;
    return result;
}

inline
Datum Datum::createBoolean(bool value)
{
    Datum result;
    result.d_as.d_type = e_INTERNAL_BOOLEAN;
    result.d_as.d_int32 = value;
    return result;
}

inline
Datum Datum::createInteger(int value)
{
    Datum result;
    result.d_as.d_type = e_INTERNAL_INTEGER;
    result.d_as.d_int32 = value;
    return result;
}

inline
Datum Datum::createInteger64(bsls::Types::Int64  value,
                             bslma::Allocator   *basicAllocator)
{
    BSLS_ASSERT(basicAllocator);
    Datum result;
    result.d_as.d_type = e_INTERNAL_INTEGER64;
    result.d_as.d_int64 = value;
    return result;
}

inline
Datum Datum::createDouble(double value)
{
    Datum result;
    result.d_as.d_type = e_INTERNAL_REAL;
    result.d_as.d_double = value;
    return result;
}

inline
Datum Datum::createStringRef(const char       *value,
                             size_type         length,
                             bslma::Allocator *basicAllocator)
{
    BSLS_ASSERT_SAFE(value || 0 == length);
    BSLS_ASSERT_SAFE(basicAllocator);

    (void)basicAllocator;

    Datum result;
    result.d_as.d_type = e_INTERNAL_STRING_REFERENCE;
    result.d_as.d_int32 = length;
    result.d_as.d_ptr = const_cast<char*>(value);
    return result;
}

inline
Datum Datum::createDate(const bdlt::Date& value)
{
    Datum result;
    result.d_as.d_type = e_INTERNAL_DATE;
    new (result.theInlineStorage()) bdlt::Date(value);
    return result;
}

inline
Datum Datum::createTime(const bdlt::Time& value)
{
    Datum result;
    result.d_as.d_type = e_INTERNAL_TIME;
    new (result.theInlineStorage()) bdlt::Time(value);
    return result;
}

inline
Datum Datum::createDatetime(const bdlt::Datetime&  value,
                            bslma::Allocator     *basicAllocator)
{
    BSLS_ASSERT(basicAllocator);

    Datum result;
    result.d_as.d_type = e_INTERNAL_DATETIME;
    new (result.theInlineStorage()) bdlt::Datetime(value);
    return result;
}

inline
Datum Datum::createDatetimeInterval(
                                  const bdlt::DatetimeInterval&  value,
                                  bslma::Allocator             *basicAllocator)
{
    BSLS_ASSERT(basicAllocator);

    Datum result;
    result.d_as.d_type = e_INTERNAL_DATETIME_INTERVAL;
    new (result.theInlineStorage()) bdlt::DatetimeInterval(value);
    return result;
}

inline
Datum Datum::createError(int code)
{
    return createDatum(e_INTERNAL_ERROR, code);
}

inline
Datum Datum::createUdt(void *data, int type)
{
    BSLS_ASSERT_SAFE(0 <= type && type <= 65535);

    Datum result;
    result.d_as.d_type = e_INTERNAL_USERDEFINED;
    result.d_as.d_int32 = type;
    result.d_as.d_ptr = data;
    return result;
}

inline
Datum Datum::adoptArray(const DatumArrayRef& array)
{
    Datum result;
    result.d_as.d_type = e_INTERNAL_ARRAY;
    result.d_as.d_ptr = array.length(); // array.length() returns a pointer
    return result;
}

inline
Datum Datum::createArrayReference(const Datum      *array,
                                  size_type         length,
                                  bslma::Allocator *basicAllocator)
{
    BSLS_ASSERT_SAFE(array || 0 == length);
    BSLS_ASSERT_SAFE(basicAllocator);

    (void)basicAllocator;

    Datum result;
    result.d_as.d_type = e_INTERNAL_ARRAY_REFERENCE;
    result.d_as.d_int32 = length;
    result.d_as.d_ptr = reinterpret_cast<void*>(const_cast<Datum*>(array));
    return result;
}

inline
Datum Datum::adoptMap(const DatumMapRef& mapping)
{
    return createDatum(e_INTERNAL_MAP, mapping.size());
}

inline
Datum Datum::adoptMapOwningKeys(const DatumMapOwningKeysRef& mapping)
{
    return createDatum(e_INTERNAL_OWNED_MAP,
                                    mapping.size());
}

// ACCESSORS
// 64-bit variation
inline
Datum::DataType Datum::type() const
{
    static const DataType convert[] = {
        e_REAL                       // e_INTERNAL_INF               = 0
      , e_NIL                        // e_INTERNAL_NIL               = 1
      , e_BOOLEAN                    // e_INTERNAL_BOOLEAN           = 2
      , e_STRING                     // e_INTERNAL_SHORTSTRING       = 3
      , e_STRING                     // e_INTERNAL_STRING            = 4
      , e_DATE                       // e_INTERNAL_DATE              = 5
      , e_TIME                       // e_INTERNAL_TIME              = 6
      , e_DATETIME                   // e_INTERNAL_DATETIME          = 7
      , e_DATETIME_INTERVAL          // e_INTERNAL_DATETIME_INTERVAL = 8
      , e_INTEGER                    // e_INTERNAL_INTEGER           = 9
      , e_INTEGER64                  // e_INTERNAL_INTEGER64         = 10
      , e_USERDEFINED                // e_INTERNAL_USERDEFINED       = 11
      , e_ARRAY                      // e_INTERNAL_ARRAY             = 12
      , e_STRING                     // e_INTERNAL_STRING_REFERENCE  = 13
      , e_ARRAY                      // e_INTERNAL_ARRAY_REFERENCE   = 14
      , e_REAL                       // e_INTERNAL_REAL              = 15
      , e_MAP                        // e_INTERNAL_MAP               = 16
      , e_MAP                        // e_INTERNAL_OWNED_MAP         = 17
      , e_ERROR_VALUE                // e_INTERNAL_ERROR             = 18
      , e_ERROR_VALUE                // e_INTERNAL_ERROR_ALLOC       = 19
      , e_BINARY                     // e_INTERNAL_BINARY            = 20
      , e_BINARY                     // e_INTERNAL_BINARY_ALLOC      = 21
      , e_DECIMAL64                  // e_INTERNAL_DECIMAL64         = 22
    };

    return convert[internalType()];
}

inline
bool Datum::theBoolean() const
{
    BSLS_ASSERT_SAFE(isBoolean());
    return d_as.d_int32;
}

inline
int Datum::theInteger() const
{
    BSLS_ASSERT_SAFE(isInteger());
    return d_as.d_int32;
}

inline
bsls::Types::Int64 Datum::theInteger64() const
{
    BSLS_ASSERT_SAFE(isInteger64());
    return d_as.d_int64;
}

inline
double Datum::theDouble() const
{
    BSLS_ASSERT_SAFE(isDouble());
    return d_as.d_double;
}

inline
bdldfp::Decimal64 Datum::theDecimal64() const
{
    BSLS_ASSERT_SAFE(isDecimal64());
    return *reinterpret_cast<const bdldfp::Decimal64 *>(theInlineStorage());
}

inline
bslstl::StringRef Datum::theString() const
{
    BSLS_ASSERT_SAFE(isString());
    const InternalDataType type = internalType();
    switch(type) {
      case e_INTERNAL_SHORTSTRING:
          return theShortString();                                    // RETURN
      case e_INTERNAL_STRING:
          return theInternalString();                                 // RETURN
      case e_INTERNAL_STRING_REFERENCE:
          return theStringReference();                                // RETURN
        default: {

            // Silence compiler warnings about unhandled enumeration values,
            // when building in 64bit mode.

            BSLS_ASSERT(false);
            return bslstl::StringRef();                               // RETURN
        } break;
    }
}

inline
bdlt::Date Datum::theDate() const
{
    BSLS_ASSERT_SAFE(isDate());
    return *reinterpret_cast<const bdlt::Date *>(theInlineStorage());
}

inline
bdlt::Time Datum::theTime() const
{
    BSLS_ASSERT_SAFE(isTime());
    return *reinterpret_cast<const bdlt::Time *>(theInlineStorage());
}

inline
bdlt::Datetime Datum::theDatetime() const
{
    BSLS_ASSERT(isDatetime());
    return *reinterpret_cast<const bdlt::Datetime *>(theInlineStorage());
}

inline
bdlt::DatetimeInterval Datum::theDatetimeInterval() const
{
    BSLS_ASSERT(isDatetimeInterval());
    return *reinterpret_cast<const bdlt::DatetimeInterval *>(
                                                           theInlineStorage());
}

inline
Error Datum::theError() const
{
    BSLS_ASSERT_SAFE(isError());

    // If the extended type is 'e_INTERNAL_ERROR', we are storing
    // just a code, at the data offset.  Otherwise, we're storing an allocated
    // object.

    if (e_INTERNAL_ERROR == internalType()) {
        return Error(static_cast<int>(d_as.d_int64));                 // RETURN
    }
    const char *data = reinterpret_cast<const char*>(d_as.d_ptr);
    return Error(
                *reinterpret_cast<const int *>(data),
                bslstl::StringRef(data + 2 * sizeof(int),
                                  *(reinterpret_cast<const int *>(data) + 1)));
}

inline
Udt Datum::theUdt() const
{
    BSLS_ASSERT_SAFE(isUdt());
    return Udt(d_as.d_ptr, d_as.d_int32);
}

inline
ConstDatumBinaryRef Datum::theBinary() const
{
    BSLS_ASSERT_SAFE(isBinary());
    switch(d_as.d_type) {
      case e_INTERNAL_BINARY:
          return ConstDatumBinaryRef(
              d_storage.buffer(),
              d_storage.buffer()[SMALLBINARY_SIZE_OFFSET]);           // RETURN
      case e_INTERNAL_BINARY_ALLOC:
          return ConstDatumBinaryRef(d_as.d_ptr, d_as.d_int32);       // RETURN
      default:
        BSLS_ASSERT_SAFE(!"NOT A BINARY");
    }
    return ConstDatumBinaryRef();
}

inline
ConstDatumArrayRef Datum::theArray() const
{
    BSLS_ASSERT_SAFE(isArray());

    if (e_INTERNAL_ARRAY == internalType()) {
        return theInternalArray();                                    // RETURN
    }

    // The external case is an option when on 32-bit platform only.

    return theArrayReference();
}

inline
ConstDatumMapRef Datum::theMap() const
{
    BSLS_ASSERT_SAFE(isMap());
    const DatumMapEntry *mapping =
                static_cast<DatumMapEntry *>(d_as.d_ptr);
    if (mapping) {
        const DatumMapEntry *data = mapping + 1;
        const size_type size = *reinterpret_cast<const size_type *>(mapping);
        const bool sorted = *reinterpret_cast<const bool *>(data);
        return ConstDatumMapRef(data + 1,                             // RETURN
                                size,
                                sorted);
    }
    return ConstDatumMapRef(0, 0, false);
}

template <typename BDLD_VISITOR>
void Datum::apply(BDLD_VISITOR& visitor) const
{
    switch (internalType()) {
      case e_INTERNAL_INF:
        visitor(bsl::numeric_limits<double>::infinity());
        break;
      case e_INTERNAL_NIL:
        visitor(bslmf::Nil());
        break;
      case e_INTERNAL_BOOLEAN:
        visitor(theBoolean());
        break;
      case e_INTERNAL_SHORTSTRING:
        visitor(theShortString());
        break;
      case e_INTERNAL_STRING:
        visitor(theInternalString());
        break;
      case e_INTERNAL_DATE:
        visitor(theDate());
        break;
      case e_INTERNAL_TIME:
        visitor(theTime());
        break;
      case e_INTERNAL_DATETIME:
        visitor(theDatetime());
        break;
      case e_INTERNAL_DATETIME_INTERVAL:
        visitor(theDatetimeInterval());
        break;
      case e_INTERNAL_INTEGER:
        visitor(theInteger());
        break;
      case e_INTERNAL_INTEGER64:
        visitor(theInteger64());
        break;
      case e_INTERNAL_USERDEFINED:
        visitor(theUdt());
        break;
      case e_INTERNAL_ARRAY:
        visitor(theInternalArray());
        break;
      case e_INTERNAL_STRING_REFERENCE:
        visitor(theStringReference());
        break;
      case e_INTERNAL_ARRAY_REFERENCE:
        visitor(theArrayReference());
        break;
      case e_INTERNAL_MAP:
      case e_INTERNAL_OWNED_MAP:  // fall through
        visitor(theMap());
        break;
      case e_INTERNAL_ERROR:
      case e_INTERNAL_ERROR_ALLOC: // fall through
        visitor(theError());
        break;
      case e_INTERNAL_REAL:
        visitor(d_as.d_double);
        break;
      case e_INTERNAL_BINARY:
      case e_INTERNAL_BINARY_ALLOC: // fall through
        visitor(theBinary());
        break;
      case e_INTERNAL_DECIMAL64:
        visitor(theDecimal64());
        break;
      default:
        BSLS_ASSERT_SAFE(!"Unknown type!!");
    }
}

#endif // defined(BSLS_PLATFORM_CPU_32_BIT)

                                // -----------
                                // class Datum
                                // -----------

                                // bit neutral

// CLASS METHODS
BDLD_DATUM_FORCE_INLINE
Datum Datum::copyString(const bslstl::StringRef&  value,
                        bslma::Allocator         *basicAllocator)
{
    return copyString(value.data(), value.length(), basicAllocator);
}

BDLD_DATUM_FORCE_INLINE
Datum Datum::createStringRef(const char       *value,
                             bslma::Allocator *basicAllocator)
{
    BSLS_ASSERT_SAFE(value);
    BSLS_ASSERT_SAFE(basicAllocator);

    return createStringRef(value, bsl::strlen(value), basicAllocator);
}

BDLD_DATUM_FORCE_INLINE
Datum Datum::createStringRef(const bslstl::StringRef&  value,
                                    bslma::Allocator  *basicAllocator)
{
    BSLS_ASSERT_SAFE(basicAllocator);
    return createStringRef(value.data(), value.length(), basicAllocator);
}

BDLD_DATUM_FORCE_INLINE
Datum Datum::createArrayReference(const ConstDatumArrayRef&  value,
                                  bslma::Allocator          *basicAllocator)
{
    BSLS_ASSERT_SAFE(basicAllocator);
    return createArrayReference(value.data(), value.length(), basicAllocator);
}

BDLD_DATUM_FORCE_INLINE
void Datum::disposeUninitializedArray(const DatumArrayRef&  array,
                                      bslma::Allocator     *basicAllocator)
{
    BSLS_ASSERT_SAFE(basicAllocator);
    basicAllocator->deallocate(array.length());
}

BDLD_DATUM_FORCE_INLINE
void Datum::disposeUninitializedMap(const DatumMapRef&  mapping,
                                    bslma::Allocator   *basicAllocator)
{
    BSLS_ASSERT_SAFE(basicAllocator);
    basicAllocator->deallocate(mapping.size());
}

BDLD_DATUM_FORCE_INLINE
void Datum::disposeUninitializedMapOwningKeys(
                                 const DatumMapOwningKeysRef&  mapping,
                                 bslma::Allocator             *basicAllocator)
{
    BSLS_ASSERT_SAFE(basicAllocator);
    basicAllocator->deallocate(mapping.size());
}

// ACCESSORS
BDLD_DATUM_FORCE_INLINE
bool Datum::isNull() const
{
    return (e_NIL == type());
}

BDLD_DATUM_FORCE_INLINE
bool Datum::isBoolean() const
{
    return (e_BOOLEAN == type());
}

BDLD_DATUM_FORCE_INLINE
bool Datum::isInteger() const
{
    return (e_INTEGER == type());
}

BDLD_DATUM_FORCE_INLINE
bool Datum::isInteger64() const
{
    return (e_INTEGER64 == type());
}

BDLD_DATUM_FORCE_INLINE
bool Datum::isDouble() const
{
    return (e_REAL == type());
}

BDLD_DATUM_FORCE_INLINE
bool Datum::isDecimal64() const
{
    return (e_DECIMAL64 == type());
}

BDLD_DATUM_FORCE_INLINE
bool Datum::isString() const
{
    return (e_STRING == type());
}

BDLD_DATUM_FORCE_INLINE
bool Datum::isDate() const
{
    return (e_DATE == type());
}

BDLD_DATUM_FORCE_INLINE
bool Datum::isTime() const
{
    return (e_TIME == type());
}

BDLD_DATUM_FORCE_INLINE
bool Datum::isDatetime() const
{
    return (e_DATETIME == type());
}

BDLD_DATUM_FORCE_INLINE
bool Datum::isDatetimeInterval() const
{
    return (e_DATETIME_INTERVAL == type());
}

BDLD_DATUM_FORCE_INLINE
bool Datum::isError() const
{
    return (e_ERROR_VALUE == type());
}

BDLD_DATUM_FORCE_INLINE
bool Datum::isUdt() const
{
    return (e_USERDEFINED == type());
}

BDLD_DATUM_FORCE_INLINE
bool Datum::isBinary() const
{
    return (e_BINARY == type());
}

BDLD_DATUM_FORCE_INLINE
bool Datum::isArray() const
{
    return (e_ARRAY == type());
}

BDLD_DATUM_FORCE_INLINE
bool Datum::isMap() const
{
    return (e_MAP == type());
}

#ifndef BDLD_DATUM_OMIT_DEPRECATED

inline
Datum Datum::createStringReference(const char       *value,
                                   size_type         length,
                                   bslma::Allocator *basicAllocator)
{
    return createStringRef(value, length, basicAllocator);
}


inline
Datum Datum::createStringReference(const char       *value,
                                   bslma::Allocator *basicAllocator)
{
    return createStringRef(value, basicAllocator);
}

inline Datum Datum::createStringReference(
                                      const bslstl::StringRef&  value,
                                      bslma::Allocator         *basicAllocator)
{
    return createStringRef(value, basicAllocator);
}

#endif

                            // -------------------
                            // class DatumArrayRef
                            // -------------------

// CREATORS
inline
DatumArrayRef::DatumArrayRef()
: d_data_p(0)
, d_length_p(0)
{
}

inline
DatumArrayRef::DatumArrayRef(Datum                  *data,
                             bsls::Types::size_type *length)
: d_data_p(data)
, d_length_p(length)
{
}

// ACCESSORS
inline
Datum *DatumArrayRef::data() const
{
    return d_data_p;
}

inline
bsls::Types::size_type *DatumArrayRef::length() const
{
    return d_length_p;
}

                        // ---------------------------
                        // class DatumMapOwningKeysRef
                        // ---------------------------

// CREATORS
inline
DatumMapOwningKeysRef::DatumMapOwningKeysRef()
: d_data_p(0)
, d_size_p(0)
, d_keys_p(0)
, d_sorted_p(0)
{
}

inline
DatumMapOwningKeysRef::DatumMapOwningKeysRef(DatumMapEntry          *data,
                                             bsls::Types::size_type *size,
                                             char                   *keys,
                                             bool                   *sorted)
: d_data_p(data)
, d_size_p(size)
, d_keys_p(keys)
, d_sorted_p(sorted)
{
}

// ACCESSORS
inline
DatumMapEntry *DatumMapOwningKeysRef::data() const
{
    return d_data_p;
}

inline
bsls::Types::size_type *DatumMapOwningKeysRef::size() const
{
    return d_size_p;
}

inline
char *DatumMapOwningKeysRef::keys() const
{
    return d_keys_p;
}

inline
bool *DatumMapOwningKeysRef::sorted() const
{
    return d_sorted_p;
}

                             // -----------------
                             // class DatumMapRef
                             // -----------------

// CREATORS
inline
DatumMapRef::DatumMapRef()
: d_data_p(0)
, d_size_p(0)
, d_sorted_p(0)
{
}

inline
DatumMapRef::DatumMapRef(DatumMapEntry          *data,
                         bsls::Types::size_type *size,
                         bool                   *sorted)
: d_data_p(data)
, d_size_p(size)
, d_sorted_p(sorted)
{
}

// ACCESSORS
inline
DatumMapEntry *DatumMapRef::data() const
{
    return d_data_p;
}

inline
bsls::Types::size_type *DatumMapRef::size() const
{
    return d_size_p;
}

inline
bool *DatumMapRef::sorted() const
{
    return d_sorted_p;
}

                                // -----------
                                // class Error
                                // -----------
// CREATORS
inline Error::Error()
: d_code(0)
{
}

inline Error::Error(int code)
: d_code(code)
{
}

inline Error::Error(int code, const bslstl::StringRef& message)
: d_code(code)
, d_message(message)
{
}

// ACCESSORS
inline int Error::code() const
{
    return d_code;
}

inline bslstl::StringRef Error::message() const
{
    return d_message;
}

// FREE OPERATORS
inline bool operator==(const Error& lhs, const Error& rhs)
{
    return (lhs.code() == rhs.code() && lhs.message() == rhs.message());
}

inline bool operator!=(const Error& lhs, const Error& rhs)
{
    return !(lhs == rhs);
}

inline bool operator<(const Error& lhs, const Error& rhs)
{
    return (lhs.code() < rhs.code() ||
            (lhs.code() == rhs.code() && lhs.message() < rhs.message()));
}

inline bool operator<=(const Error& lhs, const Error& rhs)
{
    return (lhs == rhs || lhs < rhs);
}

inline bool operator>(const Error& lhs, const Error& rhs)
{
    return !(lhs <= rhs);
}

inline bool operator>=(const Error& lhs, const Error& rhs)
{
    return !(lhs < rhs);
}

inline bsl::ostream& operator<<(bsl::ostream& output, const Error& rhs)
{
    return rhs.print(output, 0, -1);
}

                                 // ---------
                                 // class Udt
                                 // ---------

// CREATORS
inline Udt::Udt(void *data, int type)
: d_data_p(data)
, d_type(type)
{
}

// ACCESSORS
inline void *Udt::data() const
{
    return d_data_p;
}

inline int Udt::type() const
{
    return d_type;
}

// FREE OPERATORS
inline bool operator==(const Udt& lhs, const Udt& rhs)
{
    return (lhs.data() == rhs.data() &&
            lhs.type() == rhs.type());
}

inline bool operator!=(const Udt& lhs, const Udt& rhs)
{
    return (lhs.data() != rhs.data() ||
            lhs.type() != rhs.type());
}

inline bool operator<(const Udt& lhs, const Udt& rhs)
{
    return (lhs.data() < rhs.data() ||
           (lhs.data() == rhs.data() && lhs.type() < rhs.type()));
}

inline bool operator<=(const Udt& lhs, const Udt& rhs)
{
    return (lhs == rhs || lhs < rhs);
}

inline bool operator>(const Udt& lhs, const Udt& rhs)
{
    return !(lhs <= rhs);
}

inline bool operator>=(const Udt& lhs, const Udt& rhs)
{
    return !(lhs < rhs);
}

inline
bsl::ostream& operator <<(bsl::ostream& output, const Udt& rhs)
{
    return rhs.print(output, 0 , -1);
}

                         // ------------------------
                         // class ConstDatumArrayRef
                         // ------------------------

// CREATORS
inline
ConstDatumArrayRef::ConstDatumArrayRef()
: d_data_p(0)
, d_length(0)
{
}

inline
ConstDatumArrayRef::ConstDatumArrayRef(const Datum            *data,
                                       bsls::Types::size_type  length)
: d_data_p(data)
, d_length(length)
{
    BSLS_ASSERT_SAFE((length && data) || !length);
}

// ACCESSORS
inline
const Datum& ConstDatumArrayRef::operator[](bsls::Types::size_type index) const
{
    BSLS_ASSERT_SAFE(index < d_length);
    return d_data_p[index];
}

inline
const Datum *ConstDatumArrayRef::data() const
{
    return d_data_p;
}

inline
bsls::Types::size_type ConstDatumArrayRef::length() const
{
    return d_length;
}

                         // -------------------------
                         // class ConstDatumBinaryRef
                         // -------------------------

// CREATORS
inline
ConstDatumBinaryRef::ConstDatumBinaryRef()
: d_data_p(0)
, d_size(0)
{
}

inline
ConstDatumBinaryRef::ConstDatumBinaryRef(const void             *data,
                                         bsls::Types::size_type  size)
: d_data_p(data)
, d_size(size)
{
    BSLS_ASSERT_SAFE((size && data) || !size);
}

// ACCESSORS
inline
const void *ConstDatumBinaryRef::data() const
{
    return d_data_p;
}

inline
bsls::Types::size_type ConstDatumBinaryRef::size() const
{
    return d_size;
}

                            // -------------------
                            // class DatumMapEntry
                            // -------------------
// CREATORS
inline
DatumMapEntry::DatumMapEntry()
{
}

inline
DatumMapEntry::DatumMapEntry(const bslstl::StringRef& key,
                             const Datum&  value)
: d_key_p(key)
, d_value(value)
{
}

// MANIPULATORS
inline
void DatumMapEntry::setKey(const bslstl::StringRef& key)
{
    d_key_p = key;
}

inline
void DatumMapEntry::setValue(const Datum& value)
{
    d_value = value;
}

// ACCESSORS
inline
const bslstl::StringRef& DatumMapEntry::key() const
{
    return d_key_p;
}

inline
const Datum& DatumMapEntry::value() const
{
    return d_value;
}

                          // ----------------------
                          // class ConstDatumMapRef
                          // ----------------------
// CREATORS
inline
ConstDatumMapRef::ConstDatumMapRef(const DatumMapEntry    *data,
                                   bsls::Types::size_type  size,
                                   bool                    sorted)
: d_data_p(data)
, d_size(size)
, d_sorted(sorted)
{
    BSLS_ASSERT_SAFE((size && data) || !size);
}

// ACCESSORS
inline
const DatumMapEntry&
ConstDatumMapRef::operator[](bsls::Types::size_type index) const
{
    return d_data_p[index];
}

inline
const DatumMapEntry *ConstDatumMapRef::data() const
{
    return d_data_p;
}

inline
bool ConstDatumMapRef::isSorted() const
{
    return d_sorted;
}

inline
bsls::Types::size_type ConstDatumMapRef::size() const
{
    return d_size;
}

// FREE OPERATORS
inline
bool operator!=(const Datum& lhs, const Datum& rhs)
{
    return !(lhs == rhs);
}

inline
bool operator!=(const ConstDatumArrayRef& lhs,
                      const ConstDatumArrayRef& rhs)
{
    return !(lhs == rhs);
}

inline
bool operator==(const ConstDatumBinaryRef& lhs,
                      const ConstDatumBinaryRef& rhs)
{
    return (lhs.size() == rhs.size())
        && !bsl::memcmp(lhs.data(), rhs.data(), rhs.size());
}

inline
bool operator==(const DatumMapEntry& lhs,
                      const DatumMapEntry& rhs)
{
    return (lhs.key() == rhs.key()) && (lhs.value() == rhs.value());
}

inline
bool operator!=(const DatumMapEntry& lhs,
                      const DatumMapEntry& rhs)
{
    return !(lhs == rhs);
}

inline
bool operator!=(const ConstDatumMapRef& lhs,
                      const ConstDatumMapRef& rhs)
{
    return !(lhs == rhs);
}

inline
bool operator!=(const ConstDatumBinaryRef& lhs,
                      const ConstDatumBinaryRef& rhs)
{
    return !(lhs == rhs);
}

inline
bool operator<(const ConstDatumBinaryRef& lhs,
                     const ConstDatumBinaryRef& rhs)
{
    int cmp = bsl::memcmp(lhs.data(),
                          rhs.data(),
                          bsl::min(lhs.size(), rhs.size()));
    return cmp == 0 ? lhs.size() < rhs.size() : cmp < 0;
}

inline
bool operator>(const ConstDatumBinaryRef& lhs,
                     const ConstDatumBinaryRef& rhs)
{
    int cmp = bsl::memcmp(lhs.data(),
                          rhs.data(),
                          bsl::min(lhs.size(), rhs.size()));
    return cmp == 0 ? lhs.size() > rhs.size() : cmp > 0;
}

inline
bool operator<=(const ConstDatumBinaryRef& lhs,
                      const ConstDatumBinaryRef& rhs)
{
    return !(lhs > rhs);
}

inline
bool operator>=(const ConstDatumBinaryRef& lhs,
                      const ConstDatumBinaryRef& rhs)
{
    return !(lhs < rhs);
}

} // close bdld namespace
} // close BloombergLP namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
