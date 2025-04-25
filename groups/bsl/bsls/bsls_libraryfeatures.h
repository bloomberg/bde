// bsls_libraryfeatures.h                                             -*-C++-*-
#ifndef INCLUDED_BSLS_LIBRARYFEATURES
#define INCLUDED_BSLS_LIBRARYFEATURES

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide macros to identify native standard library features.
//
//@CLASSES:
//
//@MACROS:
//  BSLS_LIBRARYFEATURES_HAS_C90_GETS: C90 `gets` provided
//  BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY: fpclassify et al. provided in std
//  BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY: C99 library provided
//  BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF: C99 `snprintf` provided
//  BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR: `auto_ptr` provided
//  BSLS_LIBRARYFEATURES_HAS_CPP98_BINDERS_API: adaptable function API provided
//  BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY: C++11 base lib provided
//  BSLS_LIBRARYFEATURES_HAS_CPP11_DYNAMIC_EXCEPTION_SPECS: [deprecated]
//  BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING: except handling provided
//  BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API: GC support provided
//  BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES: misc utils provided
//  BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR: use "piecewise"
//  BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION: "program exit" provided
//  BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS: `begin` and `end` provided
//  BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE: basic_stream move operations
//  BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE: `tuple` provided
//  BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR: `unique_ptr` provided
//  BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY: C++14 base lib provided
//  BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE: `integer_sequence` defined
//  BSLS_LIBRARYFEATURES_HAS_CPP14_RANGE_FUNCTIONS: range functions extension
//  BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY: C++17 base lib provided
//  BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT: !NOT DEFINED! see below
//  BSLS_LIBRARYFEATURES_HAS_CPP17_EXCEPTION_HANDLING: except handling provided
//  BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS: optional atomics
//  BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS: searcher function objects
//  BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD: searcher object overload
//  BSLS_LIBRARYFEATURES_HAS_CPP17_DEPRECATED_REMOVED: `ptr_fun` et al. gone
//  BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV: `<charconv>` for integers only
//  BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV: full `<charconv`> support w floats
//  BSLS_LIBRARYFEATURES_HAS_CPP17_FILESYSTEM: `<filesystem>`
//  BSLS_LIBRARYFEATURES_HAS_CPP17_PARALLEL_ALGORITHMS: `<execution>`
//  BSLS_LIBRARYFEATURES_HAS_CPP17_PMR: `<memory_resource>`
//  BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING: `pmr::string`
//  BSLS_LIBRARYFEATURES_HAS_CPP17_TIMESPEC_GET: `<ctime>`
//  BSLS_LIBRARYFEATURES_HAS_CPP17_ALIGNED_ALLOC: `<cstdlib>`
//  BSLS_LIBRARYFEATURES_HAS_CPP20_VERSION: `<version>`
//  BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY: C++20 base lib provided
//  BSLS_LIBRARYFEATURES_HAS_CPP20_DEPRECATED_REMOVED: `result_of` et al. gone
//  BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS: `<concepts>`
//  BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES: `<ranges>`
//  BSLS_LIBRARYFEATURES_HAS_CPP20_SOURCE_LOCATION: `<source_location>`
//  BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_REF: `atomic_ref`
//  BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_LOCK_FREE_TYPE_ALIASES:
//                                                `atomic_[un]signed_lock_free`
//  BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_WAIT_FREE_FUNCTIONS:
//                                           `bsl::atomic_flag_wait[_explicit]`
//  BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_FLAG_TEST_FREE_FUNCTIONS:
//                                           `bsl::atomic_flag_test[_explicit]`
//  BSLS_LIBRARYFEATURES_HAS_CPP20_MAKE_UNIQUE_FOR_OVERWRITE: `*_for_overwrite`
//  BSLS_LIBRARYFEATURES_HAS_CPP20_CALENDAR: `<chrono>` calendar additions
//  BSLS_LIBRARYFEATURES_HAS_CPP20_CHAR8_MB_CONV: `mbrtoc8` & `c8rtomb`
//  BSLS_LIBRARYFEATURES_HAS_CPP20_IS_LAYOUT_COMPATIBLE: type trait
//  BSLS_LIBRARYFEATURES_HAS_CPP20_IS_CORRESPONDING_MEMBER: type trait
//  BSLS_LIBRARYFEATURES_HAS_CPP20_IS_POINTER_INTERCONVERTIBLE: type traits
//  BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT: `<format>`
//  BSLS_LIBRARYFEATURES_HAS_CPP20_JTHREAD: `std::jthread`
//  BSLS_LIBRARYFEATURES_HAS_CPP20_TIMEZONE: `<chrono>` timezone additions
//  BSLS_LIBRARYFEATURES_HAS_CPP23_BASELINE_LIBRARY: C++23 base lib provided
//  BSLS_LIBRARYFEATURES_STDCPP_GNU: implementation is GNU libstdc++
//  BSLS_LIBRARYFEATURES_STDCPP_IBM: implementation is IBM
//  BSLS_LIBRARYFEATURES_STDCPP_INTELLISENSE: Intellisense is running
//  BSLS_LIBRARYFEATURES_STDCPP_LLVM: implementation is LLVM libc++
//  BSLS_LIBRARYFEATURES_STDCPP_MSVC: implementation is MSVC
//  BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD: implementation is Sun's (RogueWave)
//  BSLS_LIBRARYFEATURES_STDCPP_STLPORT: implementation is STLPort
//
//@SEE_ALSO: bsls_platform, bsls_compilerfeatures
//
//@DESCRIPTION: This component provides a suite of preprocessor macros that
// indicate the availability of library-specific support of features that may
// not be available in all standard library versions in use across an
// organization.  None of these macros will be defined unless the compiler is
// building with a tool-chain targeting at least experimental support for a
// more recent standard than C++03.  For example, the C++11 standard library
// introduced several functions and types that were not available in libraries
// for earlier standards.  The macro
// `BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY` can be used to
// conditionally compile BDE code depending on those functions when they are
// available, and exclude that code when those functions are not present,
// thereby allowing compilation by earlier tool-chains.
//
// The standard library version described by these macros always corresponds to
// that used to build this component and is the source of the symbols found in
// the `std` namespace.
//
///Guarding Against Mixing C++ Versions
///------------------------------------
// This component defines a "link-coercion" symbol that prevents linking a
// translation unit (that includes this header, possibly indirectly) built
// against one version of the C++ Standard with a translation unit (also
// including this header) built against another version of the Standard.  For
// example, attempting to link objects built with C++14 with those built with
// C++17 will result in a link-time failure.  Because BDE supports a variety of
// features that are enabled depending on the C++ version for which code is
// built, it is generally not safe to link code built with one version of C++
// with code built with another version.  For example, in C++11 there are move
// constructor signatures, whereas in C++03 there are not, and linking code
// that views the set of constructors for a type differently is an ODR
// violation.  The link-coercion symbol that enforces this is meant to provide
// users a single, easy-to-comprehend link-time error, rather than having bugs
// potentially manifest at runtime in ways that are difficult to diagnose.
//
///Forcing Language ABI Compatibility
///----------------------------------
// For open-source users, configuration macros are provided to enable ABI
// compatibility when compiling BDE between different language standards.
//
// !WARNING! These configuration macros should **NOT** be used outside of
// recipes for open-source package managers like VCPKG.  Code compiled with
// these macros will **NOT** be linkable against code compiled without them
//
// BDE has it's own implementations for a number of types, like
// `bsl::string_view`,  that when building with a newer language standard
// (e.g., C++20) will be an alias to the platform standard library (these are
// sometimes referred to as "polyfill" types).  By default, BDE will make a
// selection between the platform implementation and the BDE implementation
// based on the language standard being used.  However, that means that
// translation units (using BDE) that are compiled with different language
// standards are binary incompatible.
//
// BDE provides the following macros for forcing ABI compatibility, to allow
// translation units compiled with different language standards to be linked
// together.  These macros follow a pattern
// `BSLS_LIBRARYFEATURES_FORCE_ABI_CPP##`, where `CPP##` is a language standard
// (e.g., `CPP11`).  Using `*_ABI_CPP11`, for example, forces ABI compatibility
// where the minimum supported compiler is C++11, so BDE implementations will
// be used for any standard library features that were introduced after C++11.
//
//  - `BSLS_LIBRARYFEATURES_FORCE_ABI_CPP11` -
//     Build the BDE libraries so that they are ABI compatible with a maximum
//     language standard of C++11.
//
//  - `BSLS_LIBRARYFEATURES_FORCE_ABI_CPP17` -
//     Build the BDE libraries so that they are ABI compatible with a maximum
//     language standard of C++17.
//
//  - `BSLS_LIBRARYFEATURES_FORCE_ABI_CPP20` -
//     Build the BDE libraries so that they are ABI compatible with a maximum
//     language standard of C++20.
//
// Attempts to combine these macros, or force ABI compatibility with a language
// standard newer than the current compiler will fail.  Attempts to link
// translation units built with different force-ABI macros defined should fail
// (on platforms where `bsls_linkcoercion` is supported).
//
// Also note that builds with C++03 are always binary incompatible with builds
// with other language standards because of the treatment of rvalue-references,
// and variadic functions, and variadic templates.
//
// Finally, note that the current list of "polyfill" types BDE provides are:
//
//  +--------------------+------------------------------------+
//  | Feature            | Minimum Language Version For Alias |
//  +====================+====================================+
//  | span               | C++20                              |
//  +--------------------+------------------------------------+
//  | string_view        | C++20 [1]                          |
//  +--------------------+------------------------------------+
//  | array              | C++17 [1]                          |
//  +--------------------+------------------------------------+
//  | optional           | C++17                              |
//  +--------------------+------------------------------------+
//  | uncaught_exception | C++17                              |
//  +--------------------+------------------------------------+
//  | reference_wrapper  | C++11 [2]                          |
//  +--------------------+------------------------------------+
//  | system_error       | C++11 [2]                          |
//  +--------------------+------------------------------------+
//
//  1. BDE uses its own implementations for some of the standard library
//     features even when a (possibly incomplete) platform implementation is
//     available to ensure that features are available when building with older
//     language standards and/or compilers.  E.g., we use the BDE
//     implementation of `string_view` in C++17 to allow C++17 users to use the
//     new methods introduced in C++20.
//
//  2. These types are aliases to the matching platform standard library types,
//     on all platforms where the forced-ABI compatibility macros are relevant.
//
///Converse Logic Is Not Symmetric
///-------------------------------
// The macros defined by this component describe features empirically observed
// in typical platform/compiler/library combinations used in BDE distributions.
// The definition of any of these macros implies that a resource is available,
// however, the converse is *not* guaranteed.  If a macro is not defined, the
// associated resource may or may not exist in the library.  For example, the
// resource may exist but may be of too early an implementation to be of use to
// clients of BDE, so in that case the associated macro would *not* be defined.
//
///`BSLS_LIBRARYFEATURES_HAS_C90_GETS`
///-----------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_C90_GETS` macro is defined if the `gets`
// function (defined in `<cstdio>`) is provided in namespace `std` by the
// native standard library.  This dangerous function is removed from the C++14
// standard library, and its use with earlier dialects is strongly discouraged.
//
///`BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY`
///------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY` macro is defined if *all* of
// the listed floating-point classification functions, defined in the headers
// named below, are implemented by the native standard library in namespace
// `std`:
//
//   - Functions defined in `<cmath>`
//     - `fpclassify`
//     - `isfinite`
//     - `isinf`
//     - `isnan`
//     - `isnormal`
//     - `signbit`
//     - `isgreater`
//     - `isgreaterequal`
//     - `isless`
//     - `islessequal`
//     - `islessgreater`
//     - `isunordered`
//
///`BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY`
///--------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY` macro is defined if *all* of the
// listed functions and types, defined in the headers named below, are
// implemented by the native standard library in namespace `std`:
//
// * Functions defined in `<cctype>`
//   - `isblank`
// * Types defined in     `<cmath>`
//   - `double_t`
//   - `float_t`
// * Functions defined in `<cmath>`
//   - `acosh`
//   - `asinh`
//   - `atanh`
//   - `cbrt`
//   - `copysign`
//   - `erf`
//   - `erfc`
//   - `exp2`
//   - `expm1`
//   - `fdim`
//   - `fma`
//   - `fmax`
//   - `fmin`
//   - `hypot`
//   - `ilogb`
//   - `lgamma`
//   - `llrint`
//   - `log1p`
//   - `log2`
//   - `logb`
//   - `lrint`
//   - `lround`
//   - `llround`
//   - `nan`
//   - `nanl`
//   - `nanf`
//   - `nearbyint`
//   - `nextafter`
//   - `nexttoward`
//   - `remainder`
//   - `remquo`
//   - `rint`
//   - `round`
//   - `scalbln`
//   - `scalbn`
//   - `tgamma`
//   - `trunc`
// * Functions defined in `<cstdlib>`
//   - `atoll`
//   - `llabs`
//   - `lldiv`
//   - `lldiv_t`
//   - `strtof`
//   - `strtold`
//   - `strtoll`
//   - `strtoull`
// * Functions defined in `<cstdio>`
//   - `vfscanf`
//   - `vsscanf`
//   - `vsnprintf`
//   - `vsscanf`
// * Functions defined in `<cwchar>`
//   - `vfwscanf`
//   - `vswscanf`
//   - `vwscanf`
//   - `wcstof`
//   - `wcstold`
//   - `wcstoll`
//   - `wcstoull`
// * Functions defined in `<cwctype>`
//   - `iswblank`
//
// This macro is defined first for the following compiler versions:
//
// * GCC 4.8
// * clang 3.0 using at least GCC 4.8 GNU C++ library
// * Microsoft Visual Studio 2015 / MSVC 19.00
//
// Notice that the above list does *not* include `snprintf` as that is included
// in the separate `BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF` macro.  Also note
// that AIX and Solaris compilers generally have implementations of these
// functions in the global namespace when including the corresponding C header,
// but do not provide the standard C++ mapping.
//
///`BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF`
///---------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF` macro is defined if `snprintf`
// function (defined in `<cstdio>`) is implemented by the native standard
// library in namespace `std`.
//
// * Function defined in `<cstdio>`
//   - `snprintf`
//
// This macro is defined first for the following compiler versions:
//
// * GCC 4.8
// * clang 3.0 using at least GCC 4.8 GNU C++ Library
// * Microsoft Visual Studio 2015 / MSVC 19.00
//
///`BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR`
///-----------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR` macro is defined if the
// `auto_ptr` class template (defined in `<memory>`) is provided by the native
// standard library.  This macro is expected to be defined for all
// libraries/platforms at least until the introduction of C++17 to our build
// systems.
//
///`BSLS_LIBRARYFEATURES_HAS_CPP98_BINDERS_API`
///--------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP98_BINDERS_API` macro is defined if the
// `result_type`, `argument_type`, `first_argument_type` and
// `second_argument_type` typedefs (defined in `<functional>`) are provided by
// the native standard library.  This macro is expected to be defined for all
// libraries/platforms at least until the introduction of C++20 to our build
// systems.
//
///`BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY`
///-------------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY` macro is used to
// identify whether the current platform's standard library supports a baseline
// set of C++11 library features (which are defined below).  This is especially
// important in BSL when importing standard library functions and types into
// the `bsl::` namespace, as is done in `bsl+bslhdrs`.  In particular this
// macro is used where C++11 has introduced new functions and types into
// headers available in older versions of the C++ standard.  For example, C++11
// introduced the type `move_iterator` to the existing `<iterator>` header, so
// `bsl_iterator.h` uses this macro to determine whether to import
// `std::move_iterator` into the `bsl` namespace.
//
// This macro is defined if *both* of the listed conditions are true:
//
// * The compiler supports C++11 language features.
// * The following functions and types are provided by the native standard
//   library:
//   - Functions defined in `<algorithm>`
//     - `all_of`
//     - `any_of`
//     - `copy_if`
//     - `copy_n`
//     - `find_if_not`
//     - `is_heap`
//     - `is_heap_until`
//     - `is_partitioned`
//     - `is_permutation`
//     - `is_sorted`
//     - `is_sorted_until`
//     - `minmax`
//     - `minmax_element`
//     - `move`
//     - `move_backward`
//     - `none_of`
//     - `partition_copy`
//     - `partition_point`
//     - `shuffle`
//   - Types defined in `<atomic>`
//     - atomic class template and specializations for integral types
//   - Functions defined in `<complex>`
//     - `acosh`
//     - `asinh`
//     - `atanh`
//     - `proj`
//   - Functions and types defined in `<exception>`
//     - `current_exception`
//     - `exception_ptr`
//     - `rethrow_exception`
//   - Functions and types defined in `<functional>`
//     - `bind`
//     - `bit_and`
//     - `bit_or`
//     - `bit_xor`
//     - `cref`
//     - `is_bind_expression`
//     - `is_placeholder`
//     - `mem_fn`
//     - `ref`
//     - `reference_wrapper`
//   - Functions defined in `<iomanip>`
//     - `get_money`
//     - `put_money`
//   - Functions and types defined in `<ios>`
//     - `io_errc`
//     - `iostream_category`
//     - `is_error_code_enum`
//     - `make_error_code`
//     - `make_error_condition`
//     - `hexfloat`
//     - `defaultfloat`
//   - Functions defined in `<iterator>`
//     - `begin`
//     - `end`
//     - `move_iterator`
//     - `make_move_iterator`
//     - `next`
//     - `prev`
//   - Function defined in `<locale>`
//     - `isblank`
//   - Functions defined in `<memory>`
//     - `addressof`
//     - `allocator_traits`
//     - `pointer_traits`
//     - `uninitialized_copy_n`
//   - Function defined in `<numeric>`
//     - `iota`
//   - Member function defined in `<typeinfo>`
//     - `type_info::hash_code`
//   - Functions defined in `<utility>`
//     - `declval`
//     - `forward`
//     - `move`
//     - `move_if_noexcept`
//     - `swap`
// * The following headers can be included:
//     - `<array>`
//     - `<atomic>`
//     - `<chrono>`
//     - `<codecvt>`
//     - `<condition_variable>`
//     - `<forward_list>`
//     - `<future>`
//     - `<mutex>`
//     - `<random>`
//     - `<regex>`
//     - `<scoped_allocator>`
//     - `<system_error>`
//     - `<thread>`
//     - `<typeindex>`
//     - `<unordered_map>`
//     - `<unordered_set>`
//
// This macro is defined first for the following compiler versions:
//
// * GCC 4.8
// * clang 3.0 using at least GCC 4.8 GNU C++ Library
// * Microsoft Visual Studio 2015 / MSVC 19.00
//
// Notice that the above list does *not* include `random_shuffle` as that is
// deprecated in C++14 and will be removed in C++17.
//
///`BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY`
///-------------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY` macro is used to
// identify whether the current platform's standard library supports a baseline
// set of C++14 library features (which are defined below).  This is especially
// important in BSL when importing standard library functions and types into
// the `bsl::` namespace, as is done in `bsl+bslhdrs`.
//
// This macro is defined if *all* of the listed conditions are true:
//
// * The compiler supports C++14 language features.
// * The following functions and types are provided by the native standard
//   library:
//   - UDLs support for `<chrono>`
//   - UDLs support for `<complex>`
//   - Type defined in `<functional>`
//     - bit_not
//   - Function defined in `<iterator>`
//     - make_reverse_iterator
//   - Function defined in `<iomanip>`
//     - quoted
//   - Function defined in `<memory>`
//     - `make_unique`
//   - Function defined in `<type_traits>`
//     - `is_null_pointer`
//     - `is_final`
//   - Function defined in `<utility>`
//     - `exchange`
// * The header `<shared_mutex>` is available providing the standard
//   components.
//
// This macro is defined first for the following compiler versions:
//
// * GCC 4.9
// * clang 3.4
// * Microsoft Visual Studio 2015 / MSVC 19.00
//
///`BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE`
///-------------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE` macro is defined if
// *all* of the listed conditions are true:
//
// * The `BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY` macro is defined.
// * The `BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES` macro is also
//   defined.
//
// * The `BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES` macro is also
//   defined.
//
// Note that though `struct` `integer_sequence` was introduced in the C++14
// standard, `bsl::integer_sequence` is supported by BDE when compiling in
// C++11 mode.
//
// This macro is defined first for the following compiler versions:
//
// * GCC 4.8
// * clang 3.0 using at least GCC 4.8 GNU C++ Library
// * Microsoft Visual Studio 2015 / MSVC 19.00
//
///`BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY`
///-------------------------------------------------
// This macro is used to identify whether the current platform's standard
// library supports a baseline set of C++17 library features (which are defined
// below).  This is especially important in BSL when importing standard library
// functions and types into the `bsl::` namespace, as is done in `bsl+bslhdrs`.
//
// This macro is defined if all of the listed conditions are true:
//
// * The compiler supports C++17 language features.
// * The following headers can be included:
//   - `<any>`
//   - `<optional>`
//   - `<variant>`
//   - `<string_view>`
//
// This macro is defined first for the following compiler versions:
//
// * GCC 7
// * clang 7
// * Microsoft Visual Studio 2019 / MSVC 19.20
//
///`BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY`
///-------------------------------------------------
// This macro is used to identify whether the current platform's standard
// library supports a baseline set of C++20 library features (which are defined
// below).  This is especially important in BSL when importing standard library
// functions and types into the `bsl::` namespace, as is done in `bsl+bslhdrs`.
//
// This macro is defined if all of the listed conditions are true:
//
// * The compiler supports C++20 language features.
// * The following headers can be included:
//   - `<barrier>`
//   - `<bit>`
//   - `<latch>`
//   - `<numbers>`
//   - `<semaphore>`
//   - `<span>`
//   - `<stop_token>`
// * The `<type_traits>` header defines:
//   - `std::remove_cvref`
//   - `std::type_identify`
//
// This macro is defined first for the following compiler versions:
//
//   - GCC 11.1
//   - clang 15 when compiling against either:
//       - libc++ version 15, or
//       - libstdc++ version 11
//   - Microsoft Visual Studio 2022 / MSVC 19.30
//
///`BSLS_LIBRARYFEATURES_HAS_CPP23_BASELINE_LIBRARY`
///-------------------------------------------------
// Experimental support! Do not use
// TODO: Complete C++23 baseline functionality validation.
//
// The information under this macro is subject to change!
//
// This macro is used to identify whether the current platform's standard
// library supports a baseline set of C++23 library features (which are defined
// below).  This is especially important in BSL when importing standard library
// functions and types into the `bsl::` namespace, as is done in `bsl+bslhdrs`.
//
// This macro is defined if all of the listed conditions are true:
//
// * The compiler supports C++23 language features.
// * The following headers can be included:
//   - <expected>  TODO
//   - TBD
//
// This macro is defined first for the following compiler versions:
//
//   - GCC 14.0
//   - clang 17 when compiling against either:
//       - libc++ version 17, or
//       - libstdc++ version 14
//   - Microsoft Visual Studio N/A
//
///`BSLS_LIBRARYFEATURES_HAS_CPP11_DYNAMIC_EXCEPTION_SPECS`
///--------------------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP11_DYNAMIC_EXCEPTION_SPECS` macro is
// defined if *both* of the listed conditions are true:
//
// * The compiler support is less or equal to C++20.
// * The following functions and types are provided by the native standard
//   library in `<exception>`:
//   - `unexpected`
//   - `unexpected_handler`
//   - `set_unexpected`
//   - `get_unexpected`
//
// **DEPRECATED**: Do not use the API listed above.
//
// This macro is defined first for the following compiler versions:
//
// * GCC 5.0
// * clang 3.0 using at least GCC 5.0 GNU C++ Library
// * Microsoft Visual Studio 2015 / MSVC 19.00
//
///`BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING`
///---------------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING` macro is defined if
// *both* of the listed conditions are true:
//
// * The `BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY` macro is defined.
// * The following functions and types are provided by the native standard
//   library in `<exception>`:
//   - `exception_ptr`
//   - `make_exception_ptr`
//   - `nested_exception`
//   - `rethrow_if_nested`
//   - `throw_with_nested`
//
// This macro is defined first for the following compiler versions:
//
// * GCC 4.8
// * clang 3.0 using at least GCC 4.8 GNU C++ Library
// * Microsoft Visual Studio 2015 / MSVC 19.00
//
///`BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API`
///-------------------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API` macro is defined
// if *both* of the listed conditions are true:
//
// * The `BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY` macro is defined.
// * The following functions and types are provided by the native standard
//   library in `<memory>`:
//   - `declare_no_pointers`
//   - `declare_reachable`
//   - `get_pointer_safety`
//   - `pointer_safety`
//   - `undeclare_no_pointers`
//   - `undeclare_reachable`
//
// This macro is defined first for the following compiler versions:
//
// * GCC 6.0
// * clang 3.0 using at least GCC 6.0 GNU C++ Library
// * Microsoft Visual Studio 2015 / MSVC 19.00
//
///`BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES`
///--------------------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES` macro is
// defined if *both* of the listed conditions are true:
//
// * The `BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY` macro is defined.
// * The following functions and types are provided by the native standard
//   library:
//   - Type defined in `<cstddef>`
//     - `max_align_t`
//   - Functions defined in `<exception>`
//     - `get_terminate`
//   - Functions defined in `<iomanip>`
//     - `get_time`
//     - `put_time`
//   - Functions defined in `<locale>`
//     - `isblank`
//     - `wstring_convert`
//     - `wbuffer_convert`
//   - Function defined in `<memory>`
//     - `align`
//   - Class and function defined in `<new>`
//     - `bad_array_new_length`
//     - `get_new_handler`
//
// This macro is defined first for the following compiler versions:
//
// * GCC 5.0
// * clang 3.0 using at least GCC 5.0 GNU C++ Library
// * Microsoft Visual Studio 2015 / MSVC 19.00
//
// Notice that the function `get_unexpected` will be removed in C++17.
//
///`BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR`
///-----------------------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR` macro is
// defined if the `pair` class template (defined in `<utility>`) provided by
// the native library has a constructor with the signature:
// ```
//  template <class... Args1, class... Args2>
//  pair(piecewise_construct_t ,
//       tuple<Args1...>       first_args,
//       tuple<Args2...>       second_args);
// ```
// Notice that the existence of the above constructor implies that each of the
// following conditions are also true:
//
// * The `piecewise_construct_t` (tag) type (defined in `<utility>`) is
//   provided by the native standard library.
// * The `BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY` macro is defined.
// * The `BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES` macro is defined.
// * The `BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE` macro is defined.
//
// This macro is defined first for the following compiler versions:
//
// * GCC 4.8
// * clang 3.0 using at least GCC 4.8 GNU C++ Library
// * Microsoft Visual Studio 2015 / MSVC 19.00
//
///`BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS`
///---------------------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS` macro is
// defined if *both* of the listed conditions are true: defined if the
// `<atomic>` header provided by the native standard library provides type
// aliases for all of the following precise bit-width atomic types:
//
// * The `BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY` macro is defined.
// * The following type aliases are provided by the native standard library in
//   the `<atomic>` header:
//   - `atomic_int8_t`
//   - `atomic_int16_t`
//   - `atomic_int32_t`
//   - `atomic_int64_t`
//   - `atomic_uint8_t`
//   - `atomic_uint16_t`
//   - `atomic_uint32_t`
//   - `atomic_uint64_t`
//   - `atomic_intptr_t`
//   - `atomic_uintptr_t`
//
// This macro is defined first for the following compiler versions:
//
// * GCC 7.0
// * clang 3.0 using at least GCC 7.0 GNU C++ Library
// * Microsoft Visual Studio 2015 / MSVC 19.00
//
///`BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS`
///------------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS` macro is defined if the
// native library supplies the following class templates in `<functional>`:
//
// * `default_searcher`
// * `boyer_moore_searcher`
// * `boyer_moore_horspool_searcher`
//
// Currently the following compilers define this function template in C++17 or
// later modes:
//
// * GCC 8.3.0
// * Microsoft Visual Studio 2019 / MSVC 19.20
// * clang 3.0 with GNU library
//
///`BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD`
///------------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD` macro is defined if
// `<algorithm>` defines an overload for the `search` function template that
// accepts instances of the searcher classes introduced in C++17.  See
// [alg.search]:
// ```
// template<class ForwardIterator, class Searcher>
// constexpr ForwardIterator std::search(ForwardIterator first,
//                                       ForwardIterator last,
//                                       const Searcher& searcher);
// ```
//
// Currently the following compilers define this function template in C++17 or
// later modes:
//
// * GCC 8.3.0
// * Microsoft Visual Studio 2017 / MSVC 19.10
// * clang above 3.0 with GNU library
// * Apple clang all supported versions
//
///`BSLS_LIBRARYFEATURES_HAS_CPP17_DEPRECATED_REMOVED`
///---------------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP17_DEPRECATED_REMOVED` macro is defined for
// libraries that do not export names removed in C++17, such as `std::ptr_fun`.
// `BSLS_LIBRARYFEATURES_HAS_CPP17_DEPRECATED_REMOVED` is generally the
// negation of `BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR`.  Although the removal
// of deprecated C++17 types is conceptually equivalent to
// `__cplusplus >= 201703L`, standard library implementations often provide
// configuration flags to expose the deprecated library features.
//
///`BSLS_LIBRARYFEATURES_HAS_CPP20_DEPRECATED_REMOVED`
///---------------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP20_DEPRECATED_REMOVED` macro is defined for
// libraries that do not export names removed in C++20, such as
// `std::uncaught_exception`.  Although the removal of deprecated C++20 types
// is conceptually equivalent to `__cplusplus >= 202002L`, some standard
// library implementations may continue to expose the features, and some may
// provide configuration flags to expose the deprecated library features.
//
///`BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION`
///----------------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION` macro is defined if
// of the listed conditions are true:
//
// * The `BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY` macro is defined.
// * The following functions are provided by the native standard library in
//   the `<cstdlib>` header:
// * `_Exit`
// * `quick_exit`
// * `at_quick_exit`
//
// This macro is defined first for the following compiler versions:
//
// * GCC 4.8
// * clang 3.0 using at least GCC 4.8 GNU C++ Library
// * Microsoft Visual Studio 2015 / MSVC 19.00
//
///`BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS`
///------------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS` is defined if the
// following functions are provided by the native standard library in the
// `<iterator>` header:
//
// ```
// template <class C> auto begin(C& c) -> decltype(c.begin());
// template <class C> auto begin(const C& c) -> decltype(c.begin());
// template <class C> auto end(C& c) -> decltype(c.end());
// template <class C> auto end(const C& c) -> decltype(c.end());
// template <class T, size_t N> T* begin(T (&array)[N]);
// template <class T, size_t N> T* end(T (&array)[N]);
// ```
//
// This macro is defined first for the following compiler versions:
//
// * GCC 4.6
// * clang 3.0
// * Microsoft Visual Studio 2012 / MSVC 17.00
//
///`BSLS_LIBRARYFEATURES_HAS_CPP14_RANGE_FUNCTIONS`
///------------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP14_RANGE_FUNCTIONS` macro is defined if the
// following functions are provided by the native standard library in the
// `<iterator>` header:
//
// ```
// template <class C> auto cbegin(const C& c) -> decltype(std::begin(c));
// template <class C> auto cend(  const C& c) -> decltype(std::end(c));
//
// template <class C> auto rbegin(      C& c) -> decltype(c.rbegin());
// template <class C> auto rbegin(const C& c) -> decltype(c.rbegin());
//
// template <class C> auto rend(      C& c)   -> decltype(c.rend());
// template <class C> auto rend(const C& c)   -> decltype(c.rend());
//
// template <class T, size_t N> reverse_iterator<T*> rbegin(T (&array)[N]);
// template <class T, size_t N> reverse_iterator<T*> rend(  T (&array)[N]);
//
// template <class E> reverse_iterator<const E*> rbegin(initializer_list<E>);
// template <class E> reverse_iterator<const E*> rend(  initializer_list<E>);
//
// template <class C> auto crbegin(const C& c) -> decltype(std::rbegin(c));
// template <class C> auto crend(  const C& c) -> decltype(std::rend(c));
// ```
//
// This macro is defined first for the following compiler versions:
//
// * GCC 4.9
// * clang 3.4
// * Microsoft Visual Studio 2015 / MSVC 19.00
//
///`BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE`
///--------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE` is defined if the standard
// stream classes (`basic_istream`, `basic_ostream`, `basic_stream`) all have
// the C++11 mandated move operations (constructor and assignment operator)
// defined.  Notice that the this macro is separate from the macro
// `BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY` as standard library
// implementations exist that implement almost all of C++11 (which is detected
// by the `BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY` macro) but not this
// feature.  Note that when `BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE` is
// defined `BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES` will also be
// defined, as the standard feature cannot be implemented without rvalue
// reference support from the compiler.
//
// * Supported by the compiler vendor's STL implementation
//   - GCC 5.0
//   - clang 3.0
//   - Microsoft Visual Studio 2015 / MSVC 19.00
//
///`BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE`
///--------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE` macro is defined if *both* of the
// listed conditions are true:
//
// * The `BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY` macro is defined.
// * The `tuple` type template (defined in `<tuple>`) is provided by the
//   native standard library.
// * The `BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES` macro is also
//   defined.
//
// This macro is defined first for the following compiler versions:
//
// * GCC 4.8
// * clang 3.0 using at least GCC 4.8 GNU C++ Library
// * Microsoft Visual Studio 2015 / MSVC 19.00
//
///`BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR`
///-------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR` macro is defined if *both*
// of the listed conditions are true:
//
// * The `BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY` macro is defined.
// * The `unique_ptr` class template (defined in `<memory>`) is provided by
//   the native standard library.
// * The `BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES` macro is also
//   defined.
//
// This macro is defined first for the following compiler versions:
//
// * GCC 4.8
// * clang 3.0 using at least GCC 4.8 GNU C++ Library
// * Microsoft Visual Studio 2015 / MSVC 19.00
//
///`BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT`: !NOT DEFINED!
///-------------------------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT` macro is never defined
// and is mentioned in this component as documentation until its appearance in
// several BDE test drivers is removed.
//
// The `BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT` macro was intended to
// indicate whether or not the `bool_constant` alias template (defined in
// `<type_traits>`) is provided by the native standard library.  It has since
// been discovered that, as `bsl::integral_constant` is actually a distinct
// class template derived from `std::integral_constant`, there must always be a
// distinct `bsl` alias template, which will be provided in a future BDE
// release.  Given this new direction, the need for a macro for the native type
// is obviated so `BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT` is being
// removed.
//
///`BSLS_LIBRARYFEATURES_HAS_CPP17_RANGE_FUNCTIONS`
///------------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP17_RANGE_FUNCTIONS` macro is defined if the
// following functions are provided by the native standard library in the
// `<iterator>` header:
//
// ```
// template <class C> auto empty(const C& c) -> decltype(c.empty());
// template <class C> auto data( const C& c) -> decltype(c.data());
// template <class C> auto size( const C& c) -> decltype(c.size());
// ```
//
///`BSLS_LIBRARYFEATURES_HAS_CPP17_ALIGNED_ALLOC`
///----------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP17_ALIGNED_ALLOC` macro is defined if the
// native standard library provides `std::aligned_alloc`.
//
// Both libstdc++ and libc++ will provide this call if the underlying C library
// provides the global call '::aligned_alloc.  This call is available on RedHat
// version 7, for example, but not on RedHat 6.  Microsoft does not provide
// this call.  See https://github.com/microsoft/STL/issues/2176
//
///`BSLS_LIBRARYFEATURES_HAS_CPP17_TIMESPEC_GET`
///---------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP17_TIMESPEC_GET` macro is defined if the
// native standard library provides `std::timespec_get`.
//
// Both libstdc++ and libc++ will provide this call if the underlying C library
// provides the global call '::timespec_get.  This call is available on RedHat
// version 7, for example, but not on RedHat 6.
//
///`BSLS_LIBRARYFEATURES_HAS_CPP17_SPECIAL_MATH_FUNCTIONS`
///-------------------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP17_SPECIAL_MATH_FUNCTIONS` is defined if
// the special math functions such as `std::assoc_laguerre`, `std::beta` and
// others are provided by the native standard library in the `<cmath>` header.
//
///`BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV`
///---------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV` macro is defined if the
// native standard library provides the `<charconv>` header and implements both
// `std::from_chars` and `std::to_chars` for all standard signed and unsigned
// integer types and the `char` type, as well as the `std::to_chars_result` and
// `std::from_chars_result` types.
//
// This macro is defined first for the following compiler versions:
//
//   - GCC 8
//   - clang using at least GCC 8 GNU C++ Library
//   - Microsoft Visual Studio 2017 / MSVC 19.10
//
///`BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV`
///-----------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV` macro is defined if the native
// standard library provides the `<charconv>` header and implements all
// required content with no major issues.  This macro is defined in addition to
// the `BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV` macro when the native
// standard library also implements the `std::chars_format` enumeration, and
// both `std::from_chars` and `std::to_chars` functions for all 3 standard
// floating point types (`float`, `double`, `long double`).
//
// This macro is defined first for the following compiler versions:
//
//   - GCC 12
//   - clang using at least GCC 12 GNU C++ Library
//   - Microsoft Visual Studio 2019 / MSVC 19.20
//
///`BSLS_LIBRARYFEATURES_HAS_CPP17_FILESYSTEM`
///-------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP17_FILESYSTEM` macro is defined if the
// native standard library provides the `<filesystem>` header and implements
// all required content with no major issues.
//
// This macro is defined first for the following compiler versions:
//
//   - GCC 9
//   - clang 14, or clang using at least GCC 9 GNU C++ Library
//   - Microsoft Visual Studio 2017 15.7 / MSVC 19.14
//
///`BSLS_LIBRARYFEATURES_HAS_CPP17_PMR`
///------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP17_PMR` macro is defined if the
// native standard library provides the `<memory_resource>` header and
// implements all required content with no major issues.
//
// This macro is defined first for the following stdlib versions:
//
//   - GNU  libstdc++ v 9
//   - LLVM libc++    v 16
//   - MSVC++ STL     v 15.6 (VS 2017)
//
///`BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING`
///-------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING` macro is defined if the
// native standard library provides the `pmr::basic_string` class template with
// no major issues.
//
// Usually `pmr::string` is available if the PMR feature is available.  But GNU
// libstdc++ doesn't provide it for the obsolete pre-C++11 CoW string
// implemetation (`_GLIBCXX_USE_CXX11_ABI` == 0).
//
///`BSLS_LIBRARYFEATURES_HAS_CPP20_VERSION`
///----------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP20_VERSION` macro is defined if the native
// standard library provides the `<version>` header and implements all required
// content with no major issues.
//
// A new header, `<version>`, that defines library feature-test macros has been
// added in C++20.  For better compatibility with the standard library
// `BSLS_LIBRARYFEATURES` macros should be based on the standard feature test
// macro when it is appropriate.  See the "STANDARD FEATURE-DETECTION MACROS"
// section below.
//
// This macro is defined first for the following compiler versions:
//
//   - GCC 9
//   - clang 7, or clang using at least GCC 9 GNU C++ Library
//   - Microsoft Visual Studio 2019 16.2 / MSVC 19.22
//
///`BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS`
///-----------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS` macro is defined if the native
// standard library provides the `<concepts>` header and implements all
// required content with no major issues.
//
// This macro is defined if the standard `__cpp_lib_concepts` feature-test
// macro is defined and `__cpp_lib_concepts >= 202002L`.
//
// This macro is defined first for the following compiler versions:
//
//   - GCC 11.1
//   - clang 15 when compiling against either:
//       - libc++ version 15, or
//       - libstdc++ version 11
//   - Microsoft Visual Studio 2022 17.2 / MSVC 19.32
//
///`BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES`
///---------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES` macro is defined if the native
// standard library provides the `<ranges>` header and implements all required
// C++20 content with no major issues.
//
// This macro is defined if the standard `__cpp_lib_ranges` feature-test macro
// is defined and `__cpp_lib_ranges >= 202110L`.
//
// This macro is defined first for the following compiler versions:
//
//   - GCC 12.1
//   - clang 15 when compiling against libc++ version 15
//   - clang 16 when compiling against
//       - libc++ version 16, or
//       - libstdc++ version 12
//   - Microsoft Visual Studio 2022 17.2 / MSVC 19.32
//
// Note that GCC 11 ranges support is incomplete, as it is missing a number of
// key Defect Report fixes, notably P2259R1, P2210R2, P2432R1, and P2415R2,
// the latter being a breaking change between GCC 11 and GCC 12.
//
///`BSLS_LIBRARYFEATURES_HAS_CPP20_SOURCE_LOCATION`
///------------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP20_SOURCE_LOCATION` macro is defined if the
// native standard library provides the `<source_location>` header and
// implements all required C++20 content with no major issues.
//
// This macro is defined if the standard `__cpp_lib_source_location`
// feature-test macro is defined and `__cpp_lib_source_location >= 201907L`.
//
// This macro is defined first for the following compiler versions:
//
//   - GCC 11.1
//   - clang 15 when compiling against either:
//       - libc++ version 16, or
//       - libstdc++ version 11
//   - Microsoft Visual Studio 2022 / MSVC 19.30
//
///`BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_REF`
///--------------------------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_REF` macro is defined if the
// `bsl::atomic_ref` type is available with C++20 semantics.
//
// This macro is defined if the standard `__cpp_lib_atomic_ref`
// feature-test macro is defined and `__cpp_lib_atomic_ref >= 201806L`.
//
// This macro is defined first for the following compiler versions:
//
//   - GCC 11.1
//   - clang 15 when compiling against libstdc++ version 11
//   - Microsoft Visual Studio 2022 / MSVC 19.30
//
// (no curent version of libc++ supports this feature)
//
///`BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_LOCK_FREE_TYPE_ALIASES`
///--------------------------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_LOCK_FREE_TYPE_ALIASES` macro is
// defined if `bsl::atomic_signed_lock_free` and
// `bsl:atomic_unsigned_lock_free` types are available with C++20 semantics.
//
// This macro is defined if the standard
// `__cpp_lib_atomic_lock_free_type_aliases` feature-test macro is defined.
//
// This macro is defined first for the following compiler versions:
//
//   - GCC 13.1
//   - Microsoft Visual Studio 2022 / MSVC 19.30
//
// (no curent version of clang supports this feature)
//
///`BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_WAIT_FREE_FUNCTIONS`
///-----------------------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_WAIT_FREE_FUNCTIONS` is defined
// if the following free functions are available: `bsl::atomic_flag_wait`,
// `bsl::atomic_flag_wait_explicit`, `bsl::atomic_flag_notify_one`,
// `bsl::atomic_flag_notify_all` with C++20 semantics.
//
// This macro is defined if the standard `__cpp_lib_atomic_wait` feature-test
// macro is defined and has a value of at least 201907L.
//
// This macro is defined first for the following compiler versions:
//
//   - clang 15 when compiling against libc++ version 15, or
//   - Microsoft Visual Studio 2022 / MSVC 19.30
//
// (no current version of GCC supports this feature)
//
///`BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_FLAG_TEST_FREE_FUNCTIONS`
///----------------------------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_FLAG_TEST_FREE_FUNCTIONS` is
// defined if the `bsl::atomic_flag_test` and `bsl::atomic_flag_test_explicit`
// functions are available with C++20 semantics.
//
// This macro is defined if the standard `__cpp_lib_atomic_flag_test`
// feature-test macro is defined and has a value of at least 201907L.
//
// This macro is defined first for the following compiler versions:
//
//   - clang 15 when compiling against libc++ version 15, or
//   - Microsoft Visual Studio 2022 / MSVC 19.30
//
// (no current version of GCC supports this feature)
//
///`BSLS_LIBRARYFEATURES_HAS_CPP20_MAKE_UNIQUE_FOR_OVERWRITE`
///----------------------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP20_MAKE_UNIQUE_FOR_OVERWRITE` macro is
// defined if the `bsl::make_unique_for_overwrite` function is available with
// C++20 semantics.
//
// This macro is defined first for the following compiler versions:
//
//   - GCC 11.1
//   - clang 15 when compiling against either:
//       - libc++ version 15, or
//       - libstdc++ version 11
//   - Microsoft Visual Studio 2022 / MSVC 19.30
//
///`BSLS_LIBRARYFEATURES_HAS_CPP20_CALENDAR`
///-----------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP20_CALENDAR` is defined if the C++20
// calendar features are available in `bsl::chrono` namespace.
//
// This macro is defined if the standard `__cpp_lib_chrono` feature-test macro
// has at least `201907L` value.
//
// This feature has been provided by MSVC++ compiler since VS 2019 16.10.  But
// that release was shipped with the following important note:  "While the STL
// generally provides all features on all supported versions of Windows, leap
// seconds and time zones (which change over time) require OS support that was
// added to Windows 10.  Specifically, updating the leap second database
// requires Windows 10 version 1809 or later, and time zones require `icu.dll`
// which is provided by Windows 10 version 1903/19H1 or later.  This applies to
// both client and server OSes; note that Windows Server 2019 is based on
// Windows 10 version 1809."  If the feature is used on a host that doesn't
// provide `icu.dll`, an exception with "The specified module could not be
// found." message will be thrown.  For this reason those features are disabled
// by default on Windows.
//
// This macro is defined first for the following compiler versions:
//
//   - Microsoft Visual Studio 2022 / MSVC 19.30
//   - GCC 14+
//
///`BSLS_LIBRARYFEATURES_HAS_CPP20_CHAR8_MB_CONV`
///----------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP20_CHAR8_MB_CONV` macro is defined if the
// `bsl::mbrtoc8` and `bsl::c8rtomb` functions are available with C++20
// semantics.
//
// (no current version of GCC, clang or MSVC supports this feature
//
///`BSLS_LIBRARYFEATURES_HAS_CPP20_IS_LAYOUT_COMPATIBLE`
///-----------------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP20_IS_LAYOUT_COMPATIBLE` is defined if the
// `bsl::is_layout_compatible` meta function is available.
//
// Note that the standard feature test macro `__cpp_lib_is_layout_compatible`
// is also defined and has a value of at least `201907L`.
//
// This macro is defined first for the following compiler versions:
//
//   - GCC 12.1
//   - Microsoft Visual Studio 2022 / MSVC 19.30
//
// (no current version of clang supports this feature)
//
///`BSLS_LIBRARYFEATURES_HAS_CPP20_IS_CORRESPONDING_MEMBER`
///--------------------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP20_IS_CORRESPONDING_MEMBER` is defined if
// the `bsl::is_corresponding_member` meta function is available.
//
// Note that the standard feature test macro `__cpp_lib_is_layout_compatible`,
// the macro that informs
// `BSLS_LIBRARYFEATURES_HAS_CPP20_IS_LAYOUT_COMPATIBLE`, is also defined.  See
// the last sentence of `P0466R5`.
//
// This macro is defined first for the following compiler versions:
//
//   - GCC 12.1
//   - Microsoft Visual Studio 2022 / MSVC 19.30
//
// (no current version of clang supports this feature)
//
///`BSLS_LIBRARYFEATURES_HAS_CPP20_IS_POINTER_INTERCONVERTIBLE`
///------------------------------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP20_IS_POINTER_INTERCONVERTIBLE` is defined
// if the `bsl::is_pointer_interconvertible_base_of` and
// `bsl::is_pointer_interconvertible_with_class` meta functions are available.
//
// Note that the standard feature test macro
// `__cpp_lib_is_pointer_interconvertible` is also defined and has a value of
// at least `201907L`.
//
// This macro is defined first for the following compiler versions:
//
//   - GCC 12.1
//   - Microsoft Visual Studio 2022 / MSVC 19.30
//
// (no current version of clang supports this feature)
//
///`BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT`
///---------------------------------------
// `BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT` is defined if the `<format>` header
// is available and supports P2216 and P2372 (which were defect reports against
// C++20).
//
// This macro is defined first for the following compiler versions:
//   - GCC 13.1
//   - Clang 17.0 with either:
//     - libc++ 17.0, or
//     - libstdc++ 13.1
//   - VS 16.11 / MSVC 19.29
//
///`BSLS_LIBRARYFEATURES_HAS_CPP20_JTHREAD`
///----------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP20_JTHREAD` is defined if the C++20
// `bsl::jthread` class in the `<bsl_thread.h>` header available.
//
// Note that the standard feature test macro `__cpp_lib_jthread` is also
// defined and has a value of at least `201911L`.
//
// This macro is defined first for the following compiler versions:
//
//   - GCC 10.1
//   - Microsoft Visual Studio 2019 Update 9 / _MSC_FULL_VER 192829913
//   - clang 18.0 with -fexperimental-library
//
///`BSLS_LIBRARYFEATURES_HAS_CPP20_TIMEZONE`
///----------------------------------------
// The `BSLS_LIBRARYFEATURES_HAS_CPP20_TIMEZONE` is defined if the C++20
// timezone features are available in `bsl::chrono` namespace.
//
// This macro is defined if the standard `__cpp_lib_chrono` feature-test macro
// has at least `201907L` value, and on GCC the compiler is allowed to use the
// C++11 (or later) ABI.
//
// This feature has been provided by MSVC++ compiler since VS 2019 16.10.  But
// that release was shipped with the following important note:  "While the STL
// generally provides all features on all supported versions of Windows, leap
// seconds and time zones (which change over time) require OS support that was
// added to Windows 10.  Specifically, updating the leap second database
// requires Windows 10 version 1809 or later, and time zones require `icu.dll`
// which is provided by Windows 10 version 1903/19H1 or later.  This applies to
// both client and server OSes; note that Windows Server 2019 is based on
// Windows 10 version 1809."  If the feature is used on a host that doesn't
// provide `icu.dll`, an exception with "The specified module could not be
// found." message will be thrown.  For this reason those features are disabled
// by default on Windows.
//
// This macro is defined first for the following compiler versions:
//
//   - Microsoft Visual Studio 2022 / MSVC 19.30
//   - GCC 14+
//
///`BSLS_LIBRARYFEATURES_STDCPP_GNU`
///---------------------------------
// The `BSLS_LIBRARYFEATURES_STDCPP_GNU` macro is defined if the C++ standard
// library implementation is GNU libstdc++.
//
///`BSLS_LIBRARYFEATURES_STDCPP_IBM`
///---------------------------------
// The `BSLS_LIBRARYFEATURES_STDCPP_IBM` macro is defined if the C++ standard
// library implementation is IBM's VACPP.
//
///`BSLS_LIBRARYFEATURES_STDCPP_INTELLISENSE`
///------------------------------------------
// The `BSLS_LIBRARYFEATURES_STDCPP_INTELLISENSE` macro is defined if
// Microsoft's Intellisense is running (instead of a production compiler).
// Note that detecting Intellisense does *not* mean that the MSVC compiler is
// used.  Visual Studio 2017 is capable of cross platform development using
// clang and Visual Studio Code (also featuring Intellisense) may be used with
// virtually any compiler.
//
///`BSLS_LIBRARYFEATURES_STDCPP_LLVM`
///----------------------------------
// The `BSLS_LIBRARYFEATURES_STDCPP_LLVM_LIBCPP` macro is defined if the C++
// standard library implementation is LLVM's libc++.
//
///`BSLS_LIBRARYFEATURES_STDCPP_MSVC`
///----------------------------------
// The `BSLS_LIBRARYFEATURES_STDCPP_MSVC` macro is defined if the C++ standard
// library implementation is Microsoft's (Dinkumware based) implementation.
//
///`BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD`
///-------------------------------------
// The `BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD` macro is defined if the C++
// standard library implementation is a RogueWave implementation.
//
///`BSLS_LIBRARYFEATURES_STDCPP_STLPORT`
///-------------------------------------
// The `BSLS_LIBRARYFEATURES_STDCPP_STLPORT` macro is defined if the C++
// standard library implementation is an STLPort implementation.
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Managing Library-Dependent Interfaces
/// - - - - - - - - - - - - - - - - - - - - - - - -
// When building software across multiple platforms a programmer may have to
// deal with different versions of the native standard library, some providing
// features that the others do not.  The macros defined in this component can
// be used make features visible only if the required native standard library
// features are present.
//
// For example, the `tuple`-type is not available in older versions of the
// native standard library.  Suppose we have a utility component that returns
// an instance of a `tuple` of values *if* the underlying version of the
// standard library provides that type, and yet remain compilable otherwise.
//
// First, we conditionally include the header file we will need if we define an
// interface that returns a `std::tuple`.
// ```
// #if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE)
//   #ifndef INCLUDED_TUPLE
//     #include <tuple>
//     #define INCLUDED_TUPLE
//   #endif
// #endif // BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
// ```
// Then, we declare the methods that will be unconditionally provided by our
// utility component:
// ```
// struct MyStatisticalUtil
// {
//     static double     mean(const int *begin, const int *end);
//     static int      median(const int *begin, const int *end);
//     static double variance(const int *begin, const int *end);
//         // Return the median (mean, variance) of the sequence of values in
//         // the specified non-empty, semi-open range '[begin, end)'.  The
//         // behavior is undefined unless 'begin < end'.
// ```
// Now, we conditionally define an interface that returns a `bsl::type`, if
// that type is available.  Note that, if all three values are needed, calling
// this interface is more efficient than calling the earlier three individually
// because the input need be traversed one time, not three.
// ```
// #ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
//     static std::tuple<int, double, double> getMedianMeanVariance(
//                                                           const int *begin,
//                                                           const int *end);
//         // Return the median, mean, and variance (in that order) of the
//         // sequence of values in the specified non-empty, semi-open range
//         // '[begin, end)'.  The behavior is undefined unless 'begin < end'.
//
// #endif // BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
//
// };
// ```
// Finally, we find that our code compiles when we build our code against
// versions of the native standard library that provide a `tuple` type, *and*
// those that do not.  Of course, in the later case the interface that returns

#include <bsls_compilerfeatures.h>
#include <bsls_platform.h>
#include <bsls_linkcoercion.h>

// ============================================================================
//                        STANDARD LIBRARY DETECTION
// ----------------------------------------------------------------------------

#define BSLS_LIBRARYFEATURES_DETECTION_IN_PROGRESS
// We are including a *standard* header to get the macros defined that allow us
// to determine what standard library implementation we have.  Do *not* change
// these to any other header.  These headers were selected because they do not
// include (m)any other standard headers *and* they do not define any dangerous
// (non-uppercase) macros.  '<ciso646>' would define dangerous macros on
// Microsoft platform, so we use the standard header '<cstddef>' to get to the
// platform-determining macros.  '<ciso646>' is effectively empty on older GNU
// implementations, so we also include '<cstddef>'.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE
// On platforms that support it we use '__has_include' to give a better error
// message if/when our assumptions about the platforms and its supported header
// files break.

// First we determine if we have clang that uses the GNU library.  We need this
// info because for the GNU library we need to include a different header, not
// '<ciso646>', because '<ciso646>' does not define the required macros prior
// to version 6.1.  See http://en.cppreference.com/w/cpp/header/ciso646

    #if defined(BSLS_PLATFORM_CMP_CLANG)
        #if __has_include(<bits/c++config.h>)
            #define BSLS_LIBRARYFEATURES_SUSPECT_CLANG_WITH_GLIBCPP           1
        #endif
    #endif

    #ifdef BSLS_PLATFORM_CMP_MSVC
        #if !__has_include(<yvals.h>)
            #error Standard library implementation detection is broken.
            // The implementation specific header file <yvals.h> we assumed
            // exists is not found.  This may be the result of either a new
            // major library version from Microsoft (that breaks with
            // tradition), or an unexpected standard library implementation
            // being used with the Microsoft compiler instead of the usual
            // Microsoft implementation.  This detection logic needs to be
            // updated to also support this new situation.
        #endif

    #elif defined(BSLS_PLATFORM_CMP_GNU) || \
          defined(BSLS_LIBRARYFEATURES_SUSPECT_CLANG_WITH_GLIBCPP)
        #if !__has_include(<bits/c++config.h>)
            #error Standard library implementation detection is broken.
            // The implementation specific header file <bits/c++config.h> we
            // assumed exists is not found.  This may be the result of either
            // a new major C++ library version from GNU (that breaks with
            // tradition), or an unexpected standard library implementation
            // being used with the GNU compiler instead of the usual libstdc++.
            // This detection logic needs to be updated to also support this
            // new situation.
        #endif

    #elif defined(BSLS_PLATFORM_CMP_SUN)
        #if !__has_include(<valarray>)
            #error Standard library implementation detection has failed.
            BSLS_PLATFORM_COMPILER_ERROR;  // Sun ignores '#error'
            // The implementation specific header file <valarray> we assumed
            // exists is not found.  Since '<valarray>' is in every ISO C++
            // standard (library) it is very likely that this error is caused
            // by a broken build configuration, such as trying to compile C++
            // source code files as C, or some other reason why the compiler
            // cannot see the C++ standard library header files.
        #endif

    #elif BSLS_COMPILERFEATURES_CPLUSPLUS >= 202002L
        #if !__has_include(<version>)
            #error Standard library implementation detection is broken.
            // The implementation specific header file <version> we assumed
            // exists is not found.  Since '<version>' is part of ISO C++2020
            // standard (library) it is very likely that this error is caused
            // by either a broken build configuration, or this untested
            // compiler reporting C++20 compatibility that it does not have.
            // Build configuration issues may be trying to compile a C++
            // source code files as C, or some other reason why the compiler
            // cannot see the C++ standard library header files.  If that is
            // not the case this detection logic has to be extended to support
            // this failing build platform.
        #endif
    #else
        #if !__has_include(<ciso646>)
            #error Standard library implementation detection is broken.
            // The implementation specific header file <ciso646> we assumed
            // exists is not found.  Since '<ciso646>' part of every ISO C++
            // standard (library) before C++20 it is very likely that this
            // error is caused by a broken build configuration, such as trying
            // to compile C++ source code files as C, or some other reason why
            // the compiler cannot see the C++ standard library header files.
            // If that is not the case this detection logic has to be extended
            // to support this failing build platform.
        #endif
    #endif

#endif  // '__has_include' is supported

// Include a header from the standard library implementation that defines the
// macros that we use for detection, and is "cheap" either because most headers
// of the implementation will '#include' it, or because it is a relatively
// small header.

#ifdef BSLS_PLATFORM_CMP_MSVC
    #include <cstddef>
#elif defined(BSLS_PLATFORM_CMP_GNU) || \
      defined(BSLS_LIBRARYFEATURES_SUSPECT_CLANG_WITH_GLIBCPP)
    #include <cstddef>
    #if defined(BSLS_LIBRARYFEATURES_SUSPECT_CLANG_WITH_GLIBCPP)
        // Prevent macro pollution
        #undef BSLS_LIBRARYFEATURES_SUSPECT_CLANG_WITH_GLIBCPP
    #endif
#elif defined(BSLS_PLATFORM_CMP_SUN)
    // Unfortunately, SUN does *nothing* in its '<ciso646>', or its
    // '<exception>' headers.  Its '<new>', and '<typeinfo>' headers are
    // shared between the RogueWave and the STLPort implementations, so they
    // do not define anything standard  library specific (see
    // https://docs.oracle.com/cd/E19205-01/819-5267/6n7c46e4p/index.html
    // 12.7.1), therefore we have to include something that is different
    // between RogueWave and STLPort implementations.  All such headers are
    // large, not cheap.  :(

    #include <valarray>
#else
    #include <ciso646>
#endif
#undef BSLS_LIBRARYFEATURES_DETECTION_IN_PROGRESS

// The implementation detection logic itself.  Note that on IBM we just detect
// the compiler, because the IBM library does not define a specific version
// macro.

#if defined (__GLIBCPP__) || defined(__GLIBCXX__)
    #define BSLS_LIBRARYFEATURES_STDCPP_GNU                                   1
#elif defined(_CPPLIB_VER) || (defined(_YVALS) && !defined(__IBMCPP__))
    #define BSLS_LIBRARYFEATURES_STDCPP_MSVC                                  1
#elif defined(_LIBCPP_VERSION)
    #define BSLS_LIBRARYFEATURES_STDCPP_LLVM                                  1
#elif defined(__STD_RWCOMPILER_H__) || defined(_RWSTD_VER)
    #define BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD                               1
#elif defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION)
    #define BSLS_LIBRARYFEATURES_STDCPP_STLPORT                               1
#elif defined(__IBMCPP__)
    #define BSLS_LIBRARYFEATURES_STDCPP_IBM                                   1
#elif defined(__INTELLISENSE__)
    #define BSLS_LIBRARYFEATURES_STDCPP_INTELLISENSE                          1
#else
    #error Could not determine standard library platform.
#endif

// ============================================================================
//                  UNIVERSAL FEATURE SUPPORT DEFINITIONS
// ----------------------------------------------------------------------------

// Define macros for C++98 features that may be detected as absent from later
// standards.  All removed libraries will have their macros undefined in a
// consistent manner at the end of this header

/// Set unconditionally for compilers supporting an earlier standard than
/// C++14; this feature macro will be undefined for those platforms with
/// partial support for C++14, implementing the removal of this dangerous
/// function.
#define BSLS_LIBRARYFEATURES_HAS_C90_GETS                                     1

/// Set unconditionally.  This features is found on each compiler
/// version/platform combination tested.  Assume universally available until
/// the day tool chains start removing this deprecated class template.
#define BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR                               1

#if BSLS_COMPILERFEATURES_CPLUSPLUS < 202002L
    /// These APIs should be removed in C++20.  Undefined conditionally
    /// later in this file for implementations that still have it.
    #define BSLS_LIBRARYFEATURES_HAS_CPP98_BINDERS_API                        1
#endif

#if BSLS_COMPILERFEATURES_CPLUSPLUS >= 201703L
    /// These APIs should be removed in C++17.  Undefined conditionally
    /// later in this file for implementations that still have it.
    #define BSLS_LIBRARYFEATURES_HAS_CPP17_DEPRECATED_REMOVED                 1
#endif

#if BSLS_COMPILERFEATURES_CPLUSPLUS >= 202002L
    /// These APIs should be removed in C++20.  Undefined conditionally
    /// later in this file for implementations that still have it.
    #define BSLS_LIBRARYFEATURES_HAS_CPP20_DEPRECATED_REMOVED                 1
#endif

// ============================================================================
//                   PLATFORM SPECIFIC FEATURE DETECTION
// ----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_GNU)
    #define BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY                          1
    #if (BSLS_COMPILERFEATURES_CPLUSPLUS >= 201103L) ||                \
           (defined(__GXX_EXPERIMENTAL_CXX0X__) && \
            BSLS_PLATFORM_CMP_VERSION >= 40800)
        // C99 functions are available in C++11 builds.
        #define BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY                          1
    #endif
    #if (BSLS_COMPILERFEATURES_CPLUSPLUS >= 201103L) ||                \
           (defined(__GXX_EXPERIMENTAL_CXX0X__) && \
            BSLS_PLATFORM_CMP_VERSION >= 40800) || \
            (defined(_GLIBCXX_USE_C99) && _GLIBCXX_USE_C99 == 1)
        // snprintf is also available in C++03 builds with new gcc versions

        #define BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF                         1
    #endif
    #if defined(__GXX_EXPERIMENTAL_CXX0X__) && \
        (BSLS_COMPILERFEATURES_CPLUSPLUS >= 201103L)
        #if BSLS_PLATFORM_CMP_VERSION >= 40600
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS            1
        #endif

        #if BSLS_PLATFORM_CMP_VERSION >= 40800
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY           1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_DYNAMIC_EXCEPTION_SPECS    1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING         1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR 1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE                      1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR                 1
            #if defined(_GLIBCXX_HAVE_AT_QUICK_EXIT) && \
                defined(_GLIBCXX_HAVE_QUICK_EXIT)
                // This feature depends on GLIBC exposing support by defining
                // the above two pre-processor definitions.
                #define BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION    1
            #endif
        #endif

        #if BSLS_PLATFORM_CMP_VERSION >= 50000
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES    1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE                1
        #endif

        #if BSLS_PLATFORM_CMP_VERSION >= 60000
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API     1
        #endif
    #endif
    #if BSLS_COMPILERFEATURES_CPLUSPLUS > 201103L
        #define BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY               1
        #define BSLS_LIBRARYFEATURES_HAS_CPP14_RANGE_FUNCTIONS                1
    #endif
    #if BSLS_COMPILERFEATURES_CPLUSPLUS > 201402L  // > C++14
        #if BSLS_PLATFORM_CMP_VERSION >= 70301
            #define BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD            1
            #define BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS            1
        #endif
    #endif
    #if BSLS_COMPILERFEATURES_CPLUSPLUS >= 201703L  // At least C++17
        #if BSLS_PLATFORM_CMP_VERSION >= 70000
            #define BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY           1
            #define BSLS_LIBRARYFEATURES_HAS_CPP17_RANGE_FUNCTIONS            1
        #endif
        #if BSLS_PLATFORM_CMP_VERSION >= 80000
            #define BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV               1
        #endif
        #if BSLS_PLATFORM_CMP_VERSION >= 90000
            #define BSLS_LIBRARYFEATURES_HAS_CPP17_FILESYSTEM                 1
            #ifdef BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE
              #if __has_include(<tbb/blocked-range.h>)
                // GCC 9 needs at least 2018 Intel Thread Building Blocks (TBB)
                // installed for full C++17 parallel algorithm support.
                #define BSLS_LIBRARYFEATURES_HAS_CPP17_PARALLEL_ALGORITHMS    1
              #endif
            #endif
        #endif
        #if _GLIBCXX_RELEASE >= 9
            #define BSLS_LIBRARYFEATURES_HAS_CPP17_PMR                        1
            // Using production flag to temporary disable generic PMR support.
            #if _GLIBCXX_USE_CXX11_ABI
                // GNU libstdc++: no 'pmr::string' when pre-C++11 ABI is used
                #define BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING             1
            #endif
        #endif
        #if BSLS_PLATFORM_CMP_VERSION >= 120000
            // GCC 11 ships FP <charconv> but slow and very buggy
            #define BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV                   1
        #endif
        #ifdef _GLIBCXX_HAVE_TIMESPEC_GET
            #define BSLS_LIBRARYFEATURES_HAS_CPP17_TIMESPEC_GET               1
        #endif
        #ifdef _GLIBCXX_HAVE_ALIGNED_ALLOC
            #define BSLS_LIBRARYFEATURES_HAS_CPP17__ALIGNED_ALLOC             1
        #endif
        #define BSLS_LIBRARYFEATURES_HAS_CPP17_SPECIAL_MATH_FUNCTIONS         1
    #endif
    #if BSLS_COMPILERFEATURES_CPLUSPLUS >= 202002L  // At least C++20
        #if BSLS_PLATFORM_CMP_VERSION >= 90000
            #define BSLS_LIBRARYFEATURES_HAS_CPP20_VERSION                    1
        #endif
        #if BSLS_PLATFORM_CMP_VERSION >= 110100
            #define BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY           1
        #endif
    #endif
    #if BSLS_COMPILERFEATURES_CPLUSPLUS >= 202302L  // At least C++23;
        #if BSLS_PLATFORM_CMP_VERSION >= 140000
            #define BSLS_LIBRARYFEATURES_HAS_CPP23_BASELINE_LIBRARY           1
        #endif
    #endif

    // '__cpp_lib_atomic_is_always_lock_free' is defined in the '<version>'
    // header, and the '<atomic>' header.  We prefer to include the '<version>'
    // header if it exists as it's not only a lot smaller and less complicated,
    // but also very likely to (already) be included anyway.
    #if __has_include(<version>)
      #include <version>
    #elif __has_include(<atomic>)
      #include <atomic>
    #endif
    #if defined(__cpp_lib_atomic_is_always_lock_free)
        // There is no pre-processor define declared in libstdc++ to indicate
        // that precise bitwidth atomics exist, but the libstdc++ shipping with
        // GCC 7 also includes lock-free support in C++17 mode.  That feature
        // does include a pre-processor definition, so use it as a stand-in for
        // detecting precise bitwidth atomics.  This pre-processor definition
        // will already only be defined when compiling in at least C++17
        // standard mode, so there is no need for an additional check.
        #define BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS       1
    #endif
    #if _GLIBCXX_USE_DEPRECATED
        // Note: always set to 1 for all known gcc versions.
        #undef BSLS_LIBRARYFEATURES_HAS_CPP20_DEPRECATED_REMOVED
        #undef BSLS_LIBRARYFEATURES_HAS_CPP17_DEPRECATED_REMOVED
    #endif
#endif

#if defined(BSLS_PLATFORM_CMP_IBM)
    // #define BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY
    // #define BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY
    // #define BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR
    // #define BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
    // #define BSLS_LIBRARYFEATURES_HAS_CPP14_RANGE_FUNCTIONS
    // #define BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS
    // #define BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD
    // #define BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS
#endif

#if defined(BSLS_PLATFORM_CMP_SUN)
    #if __cplusplus >= 201103L
        // It would be simpler if we could simply identify as the corresponding
        // gcc library version: CC CMP_VERSION libstdc++ version
        // 12.4     0x5130          4.8.4
        // 12.5     0x5140          5.1.0
        // 12.6     0x5150          5.4.0
        #if BSLS_PLATFORM_CMP_VERSION >= 0x5130
            #define BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY                  1
            #define BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY                      1
            #define BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF                     1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY           1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING         1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR 1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS            1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE                1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE                      1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR                 1
        #endif

        #if BSLS_PLATFORM_CMP_VERSION >= 0x5140
             #define BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES   1
        #endif

        #if BSLS_PLATFORM_CMP_VERSION >= 0x5150
            // Currently have an issue with rvalue-references on the CC 12.6
            // (beta) compiler, so undefining library feature macros that rely
            // on that language feature being marked as available
            #undef BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR
            #undef BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
            #undef BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR
        #endif
    #endif
    #if __cplusplus > 201103L
        #if BSLS_PLATFORM_CMP_VERSION >= 0x5140
            #define BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY           1
            #define BSLS_LIBRARYFEATURES_HAS_CPP14_RANGE_FUNCTIONS            1
        #endif
    #endif

    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_DYNAMIC_EXCEPTION_SPECS
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION
    // #define BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS
    // #define BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD
    // #define BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS
#endif

#if defined(BSLS_LIBRARYFEATURES_STDCPP_STLPORT)
    // #define BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY
    // #define BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY
    // #define BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_DYNAMIC_EXCEPTION_SPECS
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR
    // #define BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
    // #define BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS
    // #define BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD
    // #define BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS
#endif

#if defined(BSLS_PLATFORM_CMP_CLANG)

    #define BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY                          1

    #if defined(__APPLE_CC__) && (__APPLE_CC__ >= 6000)

        #define BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS                1
            // libc++ provides this C++11 feature as a C++98 extension.

        #if defined(__GXX_EXPERIMENTAL_CXX0X__) || (_LIBCPP_STD_VER >= 11)
            #define BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY                      1
            #define BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF                     1
        #endif

        #if (defined(__GXX_EXPERIMENTAL_CXX0X__) || (_LIBCPP_STD_VER >= 11)) \
          && (BSLS_COMPILERFEATURES_CPLUSPLUS >= 201103L)
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY           1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_DYNAMIC_EXCEPTION_SPECS    1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING         1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR 1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE                      1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR                 1
        #endif

        #if BSLS_COMPILERFEATURES_CPLUSPLUS > 201103L
            #define BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY           1
        #endif

        #if BSLS_COMPILERFEATURES_CPLUSPLUS >= 201402L
            #define BSLS_LIBRARYFEATURES_HAS_CPP14_RANGE_FUNCTIONS            1
        #endif

        #if BSLS_COMPILERFEATURES_CPLUSPLUS >= 201703L
            #define BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD            1

            #if BSLS_PLATFORM_CMP_VERSION >= 140000
                #define BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY       1
                #define BSLS_LIBRARYFEATURES_HAS_CPP17_RANGE_FUNCTIONS        1
                #define BSLS_LIBRARYFEATURES_HAS_CPP17_FILESYSTEM             1
            #endif

            //  #define BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV
            //  #define BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV
            //  #define BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS
            //  #define BSLS_LIBRARYFEATURES_HAS_CPP17_PARALLEL_ALGORITHMS
            //  #define BSLS_LIBRARYFEATURES_HAS_CPP17_SPECIAL_MATH_FUNCTIONS
        #endif

        #if BSLS_COMPILERFEATURES_CPLUSPLUS >= 202002L
            #if BSLS_PLATFORM_CMP_VERSION >= 100000
                #define BSLS_LIBRARYFEATURES_HAS_CPP20_VERSION                1
            #endif

            #if BSLS_PLATFORM_CMP_VERSION >= 150000
                #define BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY       1
            #endif
        #endif

        #if BSLS_COMPILERFEATURES_CPLUSPLUS >= 202302L
            #if BSLS_PLATFORM_CMP_VERSION >= 170000
                #define BSLS_LIBRARYFEATURES_HAS_CPP23_BASELINE_LIBRARY       1
            #endif
        #endif

    #elif BSLS_PLATFORM_CMP_VERSION >= 30000

        #if defined(__GXX_EXPERIMENTAL_CXX0X__)
            #define BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY                      1
            #define BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF                     1
        #endif

        #if defined(__GXX_EXPERIMENTAL_CXX0X__) && \
            (BSLS_COMPILERFEATURES_CPLUSPLUS >= 201103L)
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY           1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_DYNAMIC_EXCEPTION_SPECS    1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING         1
            #if BSLS_COMPILERFEATURES_CPLUSPLUS >= 201703L
                #define BSLS_LIBRARYFEATURES_HAS_CPP17_FILESYSTEM             1
            #endif
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR 1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS            1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE                1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE                      1
            #define BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR                 1

            #if defined(_GLIBCXX_HAVE_AT_QUICK_EXIT) && \
                defined(_GLIBCXX_HAVE_QUICK_EXIT)
                // Assume a minimum of a GCC 4.8 toolchain and check for GLIBC
                // support for the feature.
                #define BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION    1
            #endif
        #endif


        // '__cpp_lib_atomic_is_always_lock_free' is defined in the '<version>'
        // header, and the '<atomic>' header.  We prefer to include the
        // '<version>' header if it exists as it is not just a lot smaller and
        // less complicated, but also very likely to be included anyway.
        #if __has_include(<version>)
          #include <version>
        #elif __has_include(<atomic>)
          #include <atomic>
        #endif
        #if defined(__cpp_lib_atomic_is_always_lock_free)
            // There is no pre-processor define declared in libstdc++ to
            // indicate that precise bitwidth atomics exist, but the libstdc++
            // shipping with GCC 7 also includes lock-free support in C++17
            // mode.  That feature does include a pre-processor definition, so
            // use it as a stand-in for detecting precise bitwidth atomics.
            // This pre-processor definition will already only be defined when
            // compiling in at least C++17 standard mode, so there is no need
            // for an additional check.
            #define BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS   1
        #endif

        #if BSLS_COMPILERFEATURES_CPLUSPLUS > 201103L
            #if BSLS_PLATFORM_CMP_VERSION >= 30400
                #define BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY       1
                #define BSLS_LIBRARYFEATURES_HAS_CPP14_RANGE_FUNCTIONS        1
            #endif
        #endif

        #if BSLS_COMPILERFEATURES_CPLUSPLUS >= 201703L
            #define BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY           1
            #define BSLS_LIBRARYFEATURES_HAS_CPP17_RANGE_FUNCTIONS            1
            #if defined(BSLS_LIBRARYFEATURES_STDCPP_GNU)
                #define BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD        1
                #define BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS        1

                #if defined(_GLIBCXX_RELEASE) && _GLIBCXX_RELEASE >= 8
                    #define BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV       1
                #endif
                #if defined(_GLIBCXX_RELEASE) && _GLIBCXX_RELEASE >= 12
                    // The FP <charconv> in GCC 11 is slow and buggy
                    #define BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV           1
                #endif
            #elif defined(BSLS_LIBRARYFEATURES_STDCPP_LLVM)
                #define BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD        1
                #ifdef _LIBCPP_HAS_TIMESPEC_GET
                    #define BSLS_LIBRARYFEATURES_HAS_CPP17_TIMESPEC_GET       1
                #endif
                #ifdef _LIBCPP_HAS_ALIGNED_ALLOC
                    #define BSLS_LIBRARYFEATURES_HAS_CPP17_ALIGNED_ALLOC      1
                #endif
                //  #define BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV
                //  #define BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV
            #else
                #error Unsupported standard library for g++
            #endif

            //  #define BSLS_LIBRARYFEATURES_HAS_CPP17_PARALLEL_ALGORITHMS
        #endif

        #if BSLS_COMPILERFEATURES_CPLUSPLUS >= 202002L
            #if BSLS_PLATFORM_CMP_VERSION >= 90000
                #define BSLS_LIBRARYFEATURES_HAS_CPP20_VERSION                1
            #endif

            #if BSLS_PLATFORM_CMP_VERSION >= 150000
                #if defined(BSLS_LIBRARYFEATURES_STDCPP_GNU) &&               \
                    defined(_GLIBCXX_RELEASE) && _GLIBCXX_RELEASE >= 11
                    #define BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY   1
                #elif defined(BSLS_LIBRARYFEATURES_STDCPP_LLVM) &&            \
                      defined(_LIBCPP_VERSION) && _LIBCPP_VERSION >= 150000
                    #define BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY   1
                #endif
            #endif
        #endif

        #if BSLS_COMPILERFEATURES_CPLUSPLUS >= 202302L
            #if BSLS_PLATFORM_CMP_VERSION >= 170000
                #if defined(BSLS_LIBRARYFEATURES_STDCPP_GNU) &&               \
                    defined(_GLIBCXX_RELEASE) && _GLIBCXX_RELEASE >= 14
                    #define BSLS_LIBRARYFEATURES_HAS_CPP23_BASELINE_LIBRARY   1
                #elif defined(BSLS_LIBRARYFEATURES_STDCPP_LLVM) &&            \
                      defined(_LIBCPP_VERSION) && _LIBCPP_VERSION >= 170000
                    #define BSLS_LIBRARYFEATURES_HAS_CPP23_BASELINE_LIBRARY   1
                #endif
            #endif
        #endif
    #endif

    #if BSLS_COMPILERFEATURES_CPLUSPLUS >= 201703L &&                         \
        (_GLIBCXX_RELEASE >= 9 || _LIBCPP_VERSION >= 170000)
        #define BSLS_LIBRARYFEATURES_HAS_CPP17_PMR                            1
        #if defined(_GLIBCXX_RELEASE)
        // GLIB logic as above for GNU
            #if _GLIBCXX_USE_CXX11_ABI
                // GNU libstdc++: no 'pmr::string' when pre-C++11 ABI is used
                #define BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING             1
            #endif
        #else
            #define BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING                 1
        #endif
    #endif

    // Do not define these until there is a pre-processor definition given by
    // the build system to indicate at least the major version number of the
    // GCC toolchain used by Clang.
    //
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES
    // #define BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API

    #if _GLIBCXX_USE_DEPRECATED
        #undef BSLS_LIBRARYFEATURES_HAS_CPP20_DEPRECATED_REMOVED
        #undef BSLS_LIBRARYFEATURES_HAS_CPP17_DEPRECATED_REMOVED
    #endif
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC)

    // Visual Studio feature detection code is based on
    // https://docs.microsoft.com/en-us/cpp/overview/ ->
    // visual-cpp-language-conformance

    // We assume at least Visual Studio 2015
    #define BSLS_LIBRARYFEATURES_HAS_C99_FP_CLASSIFY                          1
    #define BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY                              1
    #define BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF                             1
    #define BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY                   1
    #define BSLS_LIBRARYFEATURES_HAS_CPP11_DYNAMIC_EXCEPTION_SPECS            1
    #define BSLS_LIBRARYFEATURES_HAS_CPP11_EXCEPTION_HANDLING                 1
    #define BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API             1
    #define BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES            1
    #define BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR         1
    #define BSLS_LIBRARYFEATURES_HAS_CPP11_PROGRAM_TERMINATION                1
    #define BSLS_LIBRARYFEATURES_HAS_CPP11_RANGE_FUNCTIONS                    1
    #define BSLS_LIBRARYFEATURES_HAS_CPP11_STREAM_MOVE                        1
    #define BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR                         1
    #define BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY                   1
    #define BSLS_LIBRARYFEATURES_HAS_CPP14_RANGE_FUNCTIONS                    1
    #define BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS           1
    // Note that earlier versions have 'tuple' but this macro also
    // requires the definition of the
    // 'BSLS_COMPILER_FEATURES_HAS_VARIADIC_TEMPLATES' macro.
    #define BSLS_LIBRARYFEATURES_HAS_CPP11_TUPLE                              1

    // Early access to C++17 features
    #define BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT                      1

    #undef BSLS_LIBRARYFEATURES_HAS_C90_GETS

    // C++14 support reported
    #if BSLS_COMPILERFEATURES_CPLUSPLUS >= 201402L

        // C++14 library features introduced in Visual Studio 2017

        // VS 2017 15.0
        #if BSLS_PLATFORM_CMP_VERSION >= 1910
            #define BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD            1
        #endif
    #endif  //  At least C++14

    // Possible C++17 support reported
    #if BSLS_COMPILERFEATURES_CPLUSPLUS > 201402L

        // C++17 library features introduced in Visual Studio 2017

        // VS 2017 15.6
        #if BSLS_PLATFORM_CMP_VERSION >= 1913
            #define BSLS_LIBRARYFEATURES_HAS_CPP17_PMR                        1
            #define BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING                 1
            #define BSLS_LIBRARYFEATURES_HAS_CPP17_RANGE_FUNCTIONS            1
        #endif

        // VS 2017 15.7
        #if BSLS_PLATFORM_CMP_VERSION >= 1914
            #define BSLS_LIBRARYFEATURES_HAS_CPP17_INT_CHARCONV               1
            #define BSLS_LIBRARYFEATURES_HAS_CPP17_FILESYSTEM                 1
            #define BSLS_LIBRARYFEATURES_HAS_CPP17_PARALLEL_ALGORITHMS        1
        #endif

        // VS 2017 15.8
        #if BSLS_PLATFORM_CMP_VERSION >= 1915
            // MSVC 19.15 has the floating point <charconv>' functions but they
            // do not report under and overflow properly.

            //  #define BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV
        #endif

        // C++17 library features introduced in Visual Studio 2019

        // VS 2019 16.0
        #if BSLS_PLATFORM_CMP_VERSION >= 1920
            #define BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY           1
            #define BSLS_LIBRARYFEATURES_HAS_CPP17_CHARCONV                   1
            #define BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS            1
            #define BSLS_LIBRARYFEATURES_HAS_CPP17_SPECIAL_MATH_FUNCTIONS     1
            #define BSLS_LIBRARYFEATURES_HAS_CPP17_TIMESPEC_GET               1
        #endif
    #endif  // More than C++14 (there are C++17 features)

    // C++20 support reported

    #if BSLS_COMPILERFEATURES_CPLUSPLUS >= 202002L

        // C++20 library features introduced in Visual Studio 2019

        // VS 2019 16.2
        #if BSLS_PLATFORM_CMP_VERSION >= 1922
            #define BSLS_LIBRARYFEATURES_HAS_CPP20_VERSION                    1
        #endif

        // C++20 library features introduced in Visual Studio 2022

        // VS 2022 17.0.1
        #if BSLS_PLATFORM_CMP_VERSION >= 1930
            #define BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY           1
        #endif
    #endif  // At least C++20

    // If _HAS_AUTO_PTR_ETC is defined, use its value as the deciding one for
    // whether the C++17 deprecated names are gone.
    #if defined _HAS_AUTO_PTR_ETC
        #if _HAS_AUTO_PTR_ETC
          #undef BSLS_LIBRARYFEATURES_HAS_CPP17_DEPRECATED_REMOVED
        #else
          #define BSLS_LIBRARYFEATURES_HAS_CPP17_DEPRECATED_REMOVED           1
        #endif
    #endif

    // If _HAS_FEATURES_REMOVED_IN_CXX20 is defined, use its value as the
    // deciding one for whether the C++20 deprecated names are gone.
    #if defined _HAS_FEATURES_REMOVED_IN_CXX20
        #if _HAS_FEATURES_REMOVED_IN_CXX20
          #undef BSLS_LIBRARYFEATURES_HAS_CPP20_DEPRECATED_REMOVED
        #else
          #define BSLS_LIBRARYFEATURES_HAS_CPP20_DEPRECATED_REMOVED           1
        #endif
    #endif
#endif

// ============================================================================
//                          POST-DETECTION FIXUPS
// ----------------------------------------------------------------------------

// Now, after detecting support, unconditionally undefine macros for features
// that have been removed from later standards.

#if BSLS_COMPILERFEATURES_CPLUSPLUS > 201103L
    // 'gets' is removed immediately from C++14, so undefine for any standard
    // version identifier greater than that of C++11.
    #undef BSLS_LIBRARYFEATURES_HAS_C90_GETS
#endif

#if defined BSLS_LIBRARYFEATURES_HAS_CPP17_DEPRECATED_REMOVED
    // 'auto_ptr' is removed from C++17, so undefine for any standard version
    // identifier greater than that of C++14.
    #undef BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR
#endif

#if BSLS_COMPILERFEATURES_CPLUSPLUS > 202002L
    // Dynamic exception specifications were removed from most compiler before
    // C++23.
    #undef BSLS_LIBRARYFEATURES_HAS_CPP11_DYNAMIC_EXCEPTION_SPECS

    // The garbage collection support API is removed from C++23, so undefine
    // for any standard version identifier greater than that of C++20.  Note
    // that some C++23 preview toolchains removed this API before the final
    // __cplusplus macro value for that standard was known, so we need to test
    // for "later standard than 2020".
    #undef BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API
#endif

// ============================================================================
//                           CONJUNCTION FEATURES
// ----------------------------------------------------------------------------

// Now that we have detected and fixed up all features we may determine the
// presence of those that we know exist when their required features exist.

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)  && \
    defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES) && \
    defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES)

    #define BSLS_LIBRARYFEATURES_HAS_CPP14_INTEGER_SEQUENCE                   1
#endif

// ============================================================================
//             STANDARD FEATURE-DETECTION-MACRO-BASED PROCESSING
// ----------------------------------------------------------------------------

// Starting with C++20 '__cpp_lib_*' macros indicating the presence of C++
// Standard library features is guaranteed by the ISO standard.  All such
// macros for the supported library features are guaranteed to be defined in
// the '<version>' standard header, and in additional headers related to the
// feature.  This section defines our C++20 or later feature macros that are
// based on the standard feature macros.  Note that one-to-one mapping is not
// guaranteed as we do not report any C++20 library functionality supported
// unless a minimum baseline of features are present.  It may also be necessary
// for us to consider a feature not supported, despite the standard macro being
// set, if there are issues in a certain implementation that prevent effective
// and/or safe use within the BDE libraries.

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_VERSION
  #include <version>
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_VERSION) &&                       \
    defined(BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY)
  // Feature macros are defined by the '<version>' header, we do not test for
  // their existence unless the header exists.  We make individual, additional
  // features available only when we claim to support the baseline c++20
  // standard library functionality.  We have separated the actual inclusion of
  // '<version>' into its own, preceding, '#ifdef' block (and not embedded the
  // 'BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY' conditional block in it)
  // to avoid adding yet another level of indentation.  The c++20 feature macro
  // names are long to be descriptive, which can result in crowded and hard to
  // read code even just with 2 spaces indentation.

  #if defined(__cpp_lib_concepts) && __cpp_lib_concepts >= 202002L
    #define BSLS_LIBRARYFEATURES_HAS_CPP20_CONCEPTS                           1
  #endif

  #if defined(__cpp_lib_ranges) && __cpp_lib_ranges >= 202110L &&             \
      !(defined(BSLS_PLATFORM_CMP_CLANG) &&                                   \
        BSLS_PLATFORM_CMP_VERSION < 160000 &&                                 \
        defined(BSLS_LIBRARYFEATURES_STDCPP_GNU))
      // Clang 15 doesn't implement deferred instantiation of 'requires'
      // clauses, which is necessary to build libstdc++ ranges.
    #define BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES                             1
  #endif

  #if defined(__cpp_lib_source_location) &&                                   \
      __cpp_lib_source_location >= 201907L
    #define BSLS_LIBRARYFEATURES_HAS_CPP20_SOURCE_LOCATION                    1
  #endif

  #if defined(__cpp_lib_atomic_ref) &&                                        \
      __cpp_lib_atomic_ref >= 201806L
    #define BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_REF                         1
  #endif

  #if defined(__cpp_lib_atomic_lock_free_type_aliases) &&                     \
      __cpp_lib_atomic_lock_free_type_aliases >= 201907L
    #define BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_LOCK_FREE_TYPE_ALIASES      1
  #endif


  #if defined(__cpp_lib_atomic_wait) && __cpp_lib_atomic_wait >= 201907L &&   \
      !defined(BSLS_LIBRARYFEATURES_STDCPP_GNU)
    #define BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_WAIT_FREE_FUNCTIONS         1
  #endif

  #if defined(__cpp_lib_atomic_flag_test) &&                                  \
      __cpp_lib_atomic_flag_test >= 201907L &&                                \
      !defined(BSLS_LIBRARYFEATURES_STDCPP_GNU)
    #define BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_FLAG_TEST_FREE_FUNCTIONS    1
  #endif

  #if (defined(__cpp_lib_smart_ptr_for_overwrite) &&                          \
               __cpp_lib_smart_ptr_for_overwrite >= 202002L) ||               \
      (defined(BSLS_LIBRARYFEATURES_STDCPP_GNU) &&                            \
              (_GLIBCXX_RELEASE == 11 || _GLIBCXX_RELEASE == 12))
    // GNU libstdc++ 11 & 12 don't define the macro but define the functions.
    #define BSLS_LIBRARYFEATURES_HAS_CPP20_MAKE_UNIQUE_FOR_OVERWRITE          1
  #endif

  #if defined(__cpp_lib_chrono) && __cpp_lib_chrono >= 201907L
    #define BSLS_LIBRARYFEATURES_HAS_CPP20_CALENDAR                           1

    #if !defined(BSLS_LIBRARYFEATURES_STDCPP_GNU) || _GLIBCXX_USE_CXX11_ABI   \
        || !_GLIBCXX_USE_DUAL_ABI
      // GCC's standard library only supports the timezone features if the
      // C++11 ABI, or dual ABI is enabled.
      #define BSLS_LIBRARYFEATURES_HAS_CPP20_TIMEZONE                         1
    #endif
  #endif

  // The following macro is not defined as it is not covered by any C++20
  // standard feature test macro and the feature it represents not supported by
  // any current compilers.
  //#define BSLS_LIBRARYFEATURES_HAS_CPP20_CHAR8_MB_CONV

  #if defined(__cpp_lib_is_layout_compatible) &&                              \
              __cpp_lib_is_layout_compatible >= 201907L
    #define BSLS_LIBRARYFEATURES_HAS_CPP20_IS_LAYOUT_COMPATIBLE               1
  #endif

  // Per the final sentence of 'P0466R5', the availability of
  // 'std::is_corresponding_member' is linked to the availability of
  // 'std::is_layout_compatible.
  #if defined(__cpp_lib_is_layout_compatible) &&                              \
              __cpp_lib_is_layout_compatible >= 201907L
    #define BSLS_LIBRARYFEATURES_HAS_CPP20_IS_CORRESPONDING_MEMBER            1
  #endif

  #if defined(__cpp_lib_is_pointer_interconvertible) &&                       \
              __cpp_lib_is_pointer_interconvertible >= 201907L
    #define BSLS_LIBRARYFEATURES_HAS_CPP20_IS_POINTER_INTERCONVERTIBLE        1
  #endif

  #if defined(__cpp_lib_jthread) && __cpp_lib_jthread >= 201911L
    #define BSLS_LIBRARYFEATURES_HAS_CPP20_JTHREAD                            1
  #endif

  // This one is complicated:
  //  - libstdc++ 13.1 defines the feature test macro to be 202106L even though
  //    they *do* implement P2372 and P2418, which bumped the feature test
  //    macro to 202110L.  This is GCC bug 111826.
  //
  //: o libc++ doesn't define the feature test macro as of version 17 because
  //:   they haven't implemented chrono time zones yet.  We should still use
  //:   libc++'s `<format>` despite the missing functionality, because we won't
  //:   be backporting the formatters for chrono time zones anyway.
  //
  //  - MSVC uses the correct value.
  #if (defined(__cpp_lib_format) && __cpp_lib_format >= 202110L) ||           \
      (defined(BSLS_LIBRARYFEATURES_STDCPP_GNU) && _GLIBCXX_RELEASE >= 13) || \
      (defined(BSLS_LIBRARYFEATURES_STDCPP_LLVM) && _LIBCPP_VERSION >= 17)
    #define BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT                             1
  #endif
  // But also, an older version of Clang might be used with a newer version of
  // libstdc++.  In that case, the use of `std::format` may result in bugs
  // because Clang `consteval` support was buggy prior to version 17 (see
  // https://github.com/llvm/llvm-project/commit/e328d68).
  #if (defined(BSLS_PLATFORM_CMP_CLANG) && BSLS_PLATFORM_CMP_CLANG < 170000L)
    #undef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
  #endif
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_VERSION && _CPP20_BASELINE_LIBRARY

// ============================================================================
//                  DEFINE ABI COMPATIBILITY AND LINK-SYMBOL
// ----------------------------------------------------------------------------

// Here determine whether the ABI compatibility is the default for the current
// compiler, or a forced ABI compatibility configuration flag has been
// supplied.  We create a coecion symbol to prevent linking against a binary
// incompatible translation unit.

#undef BSLS_LIBRARYFEATURES_FORCE_ABI_ENABLED
#if defined(BSLS_LIBRARYFEATURES_FORCE_ABI_CPP11)
#ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
#error Cannot force ABI compatibility with C++11 without at least C++11 support
#endif
#define BSLS_LIBRARYFEATURES_FORCE_ABI_ENABLED 11
#define BSLS_LIBRARYFEATURES_LINKER_CHECK_NAME   \
        bsls_libraryfeatures_forced_CPP11_ABI
#endif
#if defined(BSLS_LIBRARYFEATURES_FORCE_ABI_CPP17)
#ifndef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
#error Cannot force ABI compatibility with C++17 without at least C++17 support
#endif

#define BSLS_LIBRARYFEATURES_FORCE_ABI_ENABLED 17
#define BSLS_LIBRARYFEATURES_LINKER_CHECK_NAME   \
        bsls_libraryfeatures_forced_CPP17_ABI
#endif
#if defined(BSLS_LIBRARYFEATURES_FORCE_ABI_CPP20)
#ifndef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
#error Cannot force ABI compatibility with C++20 without at least C++20 support
#endif
#define BSLS_LIBRARYFEATURES_FORCE_ABI_ENABLED 20
#define BSLS_LIBRARYFEATURES_LINKER_CHECK_NAME   \
        bsls_libraryfeatures_forced_CPP20_ABI
#endif

// ============================================================================
//                       DEFINE LINK-COERCION SYMBOL
// ----------------------------------------------------------------------------

#ifndef BSLS_LIBRARYFEATURES_FORCE_ABI_ENABLED

// Catch attempts to link C++14 objects with C++17 objects (for example).

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP23_BASELINE_LIBRARY)
#define BSLS_LIBRARYFEATURES_LINKER_CHECK_NAME bsls_libraryfeatures_CPP23_ABI
#elif defined(BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY)
#define BSLS_LIBRARYFEATURES_LINKER_CHECK_NAME bsls_libraryfeatures_CPP20_ABI
#elif defined(BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY)
#define BSLS_LIBRARYFEATURES_LINKER_CHECK_NAME bsls_libraryfeatures_CPP17_ABI
#elif defined(BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY)
#define BSLS_LIBRARYFEATURES_LINKER_CHECK_NAME bsls_libraryfeatures_CPP14_ABI
#elif defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
#define BSLS_LIBRARYFEATURES_LINKER_CHECK_NAME bsls_libraryfeatures_CPP11_ABI
#else
#define BSLS_LIBRARYFEATURES_LINKER_CHECK_NAME bsls_libraryfeatures_CPP03_ABI
#endif

namespace BloombergLP {

extern const char *BSLS_LIBRARYFEATURES_LINKER_CHECK_NAME;
BSLS_LINKCOERCION_FORCE_SYMBOL_DEPENDENCY(
                           const char *,
                           bsls_libraryfeatures_assertion,
                           BloombergLP::BSLS_LIBRARYFEATURES_LINKER_CHECK_NAME)

}  // close enterprise namespace
#endif // BSLS_LIBRARYFEATURES_FORCE_ABI_ENABLED

#endif // INCLUDED_BSLS_LIBRARYFEATURES

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
