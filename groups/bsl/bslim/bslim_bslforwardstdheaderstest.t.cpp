// bslim_bslforwardstdheaderstest.t.cpp                                 -*-C++-*-
#include <bslim_bslforwardstdheaderstest.h>

#include <bslmf_voidtype.h>
#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>

#include <bsl_iosfwd.h>

#include <stdio.h>     // `printf`
#include <stdlib.h>    // `atoi`

#if defined BSLS_LIBRARYFEATURES_HAS_CPP20_VERSION
# include <version>    // feature test macros
#endif

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// <bsl_iosfwd.h> is a lightweight header that provides declarations without
// definitions for a variety of types and templates specified by standard
// library and that cannot be declared by the user.
//
// Globsl concerns:
// Do not transitively include a heder that might define one of the names that
// we ar e testing is only forward declared.  Me mitigate this risk by strictly
// including the minimum set of headers to provide the necessry testing support
// by implementing simple test machinery inside the test driver rather than
// including similar functionality from `bsl`.  Note that `<bslmf_voidtype.h>`
// has been audited to show no dependencies not otherwise in this test driver.
//
// Class templates can have an unspecified number of template parameters as
// library vendors are allowed to add additional template parameters with
// default arguments.  In order to propertly test the properties of templates
// we need variadic template parameters in order to match templates with any
// set of (type) parameters.  Hence, we disable the tests for templates when
// building with C++03.
//-----------------------------------------------------------------------------
// [5] FORWARD DECLARATIONS FOR C++23 SPANSTREAM
// [4] FORWARD DECLARATIONS FOR BDE SYNCSTREAM
// [3] BSL FORWARD DECLARATIONS FOR BSL IMPLEMENTATIONS
// [2] BSL ALIASES FOR STD DECLARATIONS
// ----------------------------------------------------------------------------
// [1] TEST MACHINERY

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);
        fflush(stdout);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

//=============================================================================
//                  FEATURE TEST MACROS FOR THIS TEST DRIVER
//-----------------------------------------------------------------------------

#if defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
# define BSLIM_BSLSTANDARDHEADERTEST_CAN_TEST_TEMPLATES     1
#endif

//=============================================================================
//                       GLOBAL HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

namespace {
                        // =======================
                        // trait aliases_same_type
                        // =======================

/// This `struct` template provides a meta-function to determine whether the
/// (template) parameter `t_FIRST_TYPE` and the (template) parameter
/// `t_SECOND_TYPE` are the same.  The primary template provides a `value` that
/// is `false`.  A template specialization is provided (below) is selected when
/// the template arguments are the same and provides a `value` that is `false`.
template <class t_FIRST_TYPE, class t_SECOND_TYPE>
struct aliases_same_type {
    enum { value = false };
};

template <class t_FIRST_TYPE>
struct aliases_same_type<t_FIRST_TYPE, t_FIRST_TYPE> {
    enum { value = true };
};

                        // ============================
                        // trait template_is_incomplete
                        // ============================

/// This `struct` template provides a meta-function to determine whether the
/// (template) parameter `t_TYPE` is an incomplete type.  The primary template
//  provides a `value` that is `true`.  A template specialization is provided
/// (below) is selected when the template argument is not a complete type in
/// a supplied SFINAE context.  Note that this trait would be dangerous to
/// promote into a supported component as it would invite violations of the
/// ODR when different instantions produce different results for the same
/// type according to whether the complete type is defined in that scope.
template <class t_TYPE, class = void>
struct type_is_incomplete {
    enum { value = true };
};

template <class t_TYPE>
struct type_is_incomplete<t_TYPE, BSLMF_VOIDTYPE(int[sizeof(t_TYPE)])> {
    enum { value = false };
};

#if defined(BSLIM_BSLSTANDARDHEADERTEST_CAN_TEST_TEMPLATES)

                        // ===========================
                        // trait aliases_same_template
                        // ===========================

/// This `struct` template provides a meta-function to determine whether the
/// (template template) parameter `t_TEMPLATE1` and the (template template)
/// parameter `t_TEMPLATE2` are the same.   The primary template provides a
///  `value` that is `false`.  A template partial specialization is provided
/// (below) is selected when the template arguments are the same and provides
/// a `value` that is `false`.
template <template<class ...> class, template<class ...> class>
struct aliases_same_template {
    enum { value = false };
};
template <template<class ...> class t_TEST_TEMPLATE>
struct aliases_same_template<t_TEST_TEMPLATE, t_TEST_TEMPLATE> {
    enum { value = true };
};

                        // ============================
                        // trait template_is_incomplete
                        // ============================

/// This `struct` template provides a meta-function to determine whether the
/// (template template) parameter `t_TEMPLATE` is a template for an incomplete
/// class.  The primary template provides a `value` that is `true`.  A template
/// partial specialization provided (below) is selected when the template
/// argument would not be a complete type when instantiated in a supplied
/// SFINAE context.  While it is not possible to implement this trait for
/// arbitrary templates without reflection in C++26, for the purposes of this
/// test driver we can assume that all tested templates support `char` as the
/// first and only argument, even if the full class template definition were
/// available.  Note that this trait would be dangerous to promote into a
/// supported component as it would invite violations of the ODR when different
/// instantions produce different results for the same template according to
/// whether the complete instantiated type is defined in that scope.
template <template<class ...> class t_TEMPLATE, class = void>
struct template_is_incomplete {
    enum { value = true };
};

template <template <class...> class t_TEMPLATE>
struct template_is_incomplete<t_TEMPLATE,
                              BSLMF_VOIDTYPE(int[sizeof(t_TEMPLATE<char>)])> {
    enum { value = false };
};
#endif

//=============================================================================
//                 VOCABULARY TEST TYPES AND TEMPLATES
//-----------------------------------------------------------------------------

class Incomplete;
union Uncomplete;

#if defined(BSLIM_BSLSTANDARDHEADERTEST_CAN_TEST_TEMPLATES)
template <class>
class SimpleTemplate;

template <class>
class AnotherTemplate;

template <class, class = void, class = int, class = char *, class...>
class ComplexTemplate;

template <class, class = void, class = int, class = char *>
class CompleteTemplate {};
#endif
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int test                 = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose             = argc > 2;
    const bool veryVerbose         = argc > 3;
    const bool veryVeryVerbose     = argc > 4;
    const bool veryVeryVeryVerbose = argc > 4;

    (void)veryVeryVerbose;
    (void)veryVeryVeryVerbose;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
        // -------------------------------------------------------------------
        // FORWARD DECLARATIONS FOR C++23 SPANSTREAM
        //
        // Concerns:
        // 1. The forward declarations from <spanstream> specified for <iosfwd>
        //    in the C++23 Standard are present in both the `bsl` and `std`
        //    namespaces.
        //
        // 2. The type and template names all refer to incomplete entities.
        //
        // 3. The `bsl` names alias the `std` names.
        //
        // Plan:
        // 1. Using type traits specific to this test driver, verify the
        //    declarations specified for <iosfwd> in C++23 are present in the
        //    `bsl` and `std` namespaces and do not refer to complete types or
        //    defined class templates.
        //
        // 2. Verify every name in the `bsl` namespace is an alias for the
        //    same entity with the corresponding name in the `std` namespace.
        //
        // Testing
        //   basic_ispanstream;
        //   basic_ospanstream;
        //   basic_spanbuf;
        //   basic_spanstream;
        //   ispanstream;
        //   ospanstream;
        //   spanbuf;
        //   spanstream;
        //   wspanbuf;
        //   wispanstream;
        //   wospanstream;
        //   wspanstream;
        // -------------------------------------------------------------------

        if (verbose)
            puts("\nFORWARD DECLARATIONS FOR C++23 SPANSTREAM"
                 "\n=========================================");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_SPANSTREAM
        if (veryVerbose) puts("Testing types are incomplete");
        {
            ASSERT(template_is_incomplete<bsl::basic_ispanstream>::value);
            ASSERT(template_is_incomplete<bsl::basic_ospanstream>::value);
            ASSERT(template_is_incomplete<bsl::basic_spanbuf>::value);
            ASSERT(template_is_incomplete<bsl::basic_spanstream>::value);
            ASSERT(type_is_incomplete<bsl::ispanstream>::value);
            ASSERT(type_is_incomplete<bsl::ospanstream>::value);
            ASSERT(type_is_incomplete<bsl::spanbuf>::value);
            ASSERT(type_is_incomplete<bsl::spanstream>::value);
            ASSERT(type_is_incomplete<bsl::wispanstream>::value);
            ASSERT(type_is_incomplete<bsl::wospanstream>::value);
            ASSERT(type_is_incomplete<bsl::wspanbuf>::value);
            ASSERT(type_is_incomplete<bsl::wspanstream>::value);
        }

        if (veryVerbose) puts("Testing bsl types alias std");
        {
            ASSERT((aliases_same_template<bsl::basic_ispanstream,
                                          std::basic_ispanstream>::value));
            ASSERT((aliases_same_template<bsl::basic_ospanstream,
                                          std::basic_ospanstream>::value));
            ASSERT((aliases_same_template<bsl::basic_spanbuf,
                                          std::basic_spanbuf>::value));
            ASSERT((aliases_same_template<bsl::basic_spanstream,
                                          std::basic_spanstream>::value));

            ASSERT((aliases_same_type<bsl::ispanstream,
                                      std::ispanstream>::value));
            ASSERT((aliases_same_type<bsl::ospanstream,
                                      std::ospanstream>::value));
            ASSERT((aliases_same_type<bsl::spanbuf, std::spanbuf>::value));
            ASSERT((aliases_same_type<bsl::spanstream,
                                      std::spanstream>::value));
            ASSERT((aliases_same_type<bsl::wspanbuf, std::wspanbuf>::value));
            ASSERT((aliases_same_type<bsl::wispanstream,
                                      std::wispanstream>::value));
            ASSERT((aliases_same_type<bsl::wospanstream,
                                      std::wospanstream>::value));
            ASSERT((aliases_same_type<bsl::wspanstream,
                                      std::wspanstream>::value));
        }
#endif
      } break;
      case 4: {
        // -------------------------------------------------------------------
        // FORWARD DECLARATIONS FOR BDE SYNCSTREAM
        //   BDE provides its own implementations of the stanrdard syncstreams
        //   facility. Note that there is no BDE library feature test macro
        //   for this facility as it is always present.
        //
        // Concerns:
        // 1. The declarations specified for <iosfwd> in C++20 are present in
        //    the `bsl` namespace, even in a C++03 build.
        //
        // 2. The type and template names all refer to incomplete entities.
        //
        // 3. Where the native library provides the syncstream facilities,
        //    the `bsl` names do not alias the `std` names.
        //
        // Plan:
        // 1. Verify every name in the `bsl` namespace denotes an incomplete
        //    type or template using a type trait created for just this
        //    test driver.
        //
        // 2. Where the `__cpp_lib_syncbuf` macro indicates that the native
        //    standard libray provides a complete <syncstream> implementation,
        //    verify no name in the `bsl` namespace is an alias for the
        //    same entity that is visible in the `std` namespace.
        //
        // Testing
        //   basic_osyncstream
        //   basic_syncbuf
        //   osyncstream
        //   syncbuf
        //   wosyncstream
        //   wsyncbuf
        // -------------------------------------------------------------------

        if (verbose)
            puts("\nFORWARD DECLARATIONS FOR BDE SYNCSTREAM"
                 "\n=======================================");

        if (veryVerbose) puts("Testing types are incomplete");
        {
#if defined(BSLIM_BSLSTANDARDHEADERTEST_CAN_TEST_TEMPLATES)
            ASSERT(template_is_incomplete<bsl::basic_osyncstream>::value);
            ASSERT(template_is_incomplete<bsl::basic_syncbuf>::value);
#endif
            ASSERT(type_is_incomplete<bsl::osyncstream>::value);
            ASSERT(type_is_incomplete<bsl::syncbuf>::value);
            ASSERT(type_is_incomplete<bsl::wosyncstream>::value);
            ASSERT(type_is_incomplete<bsl::wsyncbuf>::value);
        }

#if __cpp_lib_syncbuf >= 201803L
        if (veryVerbose) puts("Testing bsl types do not alias std");
        {
            ASSERT((!aliases_same_template<bsl::basic_osyncstream,
                                           std::basic_osyncstream>::value));
            ASSERT((!aliases_same_template<bsl::basic_syncbuf,
                                           std::basic_syncbuf>::value));

            ASSERT((!aliases_same_type<bsl::osyncstream,
                                       std::osyncstream>::value));
            ASSERT((!aliases_same_type<bsl::syncbuf, std::syncbuf>::value));
            ASSERT((!aliases_same_type<bsl::wosyncstream,
                                       std::wosyncstream>::value));
            ASSERT((!aliases_same_type<bsl::wosyncstream,
                                       std::wosyncstream>::value));
        }
#endif
      } break;
      case 3: {
        // -------------------------------------------------------------------
        // BSL FORWARD DECLARATIONS FOR BSL IMPLEMENTATIONS
        //   The following names denote types with BDE implementations that
        //    are not aliases of the Standard Library implementations.
        //
        // Concerns:
        // 1. The declarations of types and template with implementations
        //    suplied by BDE and declated in `<iosfwd>` according to the C++23
        //    Standard are available in C++23 mode in both the `bsl` and
        //    `std` namespaces.
        //
        // 2. The named entities in namespaces `bsl` and `std` do not alias
        //    each other.
        //
        // Plan:
        // 1. Verify every name in the `bsl` namespace denotes an incomplete
        //    type or template using a type trait created for just this
        //    test driver.
        //
        // 2. Verify no name in the `bsl` namespace is an alias for the
        //    same entity that is visible in the `std` namespace.
        //
        // Testing
        //   allocator
        //   basic_istringstream
        //   basic_ostringstream
        //   basic_stringbuf
        //   basic_stringstream
        //   istringstream
        //   ostringstream
        //   stringbuf
        //   stringstream
        //   wistringstream
        //   wostringstream
        //   wstringbuf
        //   wstringstream
        // -------------------------------------------------------------------

        if (verbose)
            puts("\nBSL FORWARD DECLARATIONS FOR BSL IMPLEMENTATIONS"
                 "\n================================================");

        if (veryVerbose) puts("Testing that types are incomplete");
        {
#if defined(BSLIM_BSLSTANDARDHEADERTEST_CAN_TEST_TEMPLATES)
            ASSERT(template_is_incomplete<bsl::allocator>::value);
            ASSERT(template_is_incomplete<bsl::basic_istringstream>::value);
            ASSERT(template_is_incomplete<bsl::basic_ostringstream>::value);
            ASSERT(template_is_incomplete<bsl::basic_stringbuf>::value);
            ASSERT(template_is_incomplete<bsl::basic_stringstream>::value);
#endif
            ASSERT(type_is_incomplete<bsl::istringstream>::value);
            ASSERT(type_is_incomplete<bsl::ostringstream>::value);
            ASSERT(type_is_incomplete<bsl::stringbuf>::value);
            ASSERT(type_is_incomplete<bsl::stringstream>::value);
            ASSERT(type_is_incomplete<bsl::wistringstream>::value);
            ASSERT(type_is_incomplete<bsl::wostringstream>::value);
            ASSERT(type_is_incomplete<bsl::wstringbuf>::value);
            ASSERT(type_is_incomplete<bsl::wstringstream>::value);
        }

        if (veryVerbose) puts("Testing bsl types do not alias std");
        {
#if defined(BSLIM_BSLSTANDARDHEADERTEST_CAN_TEST_TEMPLATES)
            ASSERT((!aliases_same_template<bsl::allocator,
                                           std::allocator>::value));
            ASSERT((!aliases_same_template<bsl::basic_istringstream,
                                           std::basic_istringstream
                                          >::value));
            ASSERT((!aliases_same_template<bsl::basic_ostringstream,
                                           std::basic_ostringstream
                                          >::value));
            ASSERT((!aliases_same_template<bsl::basic_stringbuf,
                                           std::basic_stringbuf>::value));
            ASSERT((!aliases_same_template<bsl::basic_stringstream,
                                           std::basic_stringstream
                                          >::value));
#endif
            ASSERT((!aliases_same_type<bsl::istringstream,
                                       std::istringstream>::value));
            ASSERT((!aliases_same_type<bsl::ostringstream,
                                       std::ostringstream>::value));
            ASSERT((!aliases_same_type<bsl::stringbuf,
                                       std::stringbuf>::value));
            ASSERT((!aliases_same_type<bsl::stringstream,
                                       std::stringstream>::value));
            ASSERT((!aliases_same_type<bsl::wistringstream,
                                       std::wistringstream>::value));
            ASSERT((!aliases_same_type<bsl::wostringstream,
                                       std::wostringstream>::value));
            ASSERT((!aliases_same_type<bsl::wstringbuf,
                                       std::wstringbuf>::value));
            ASSERT((!aliases_same_type<bsl::wstringstream,
                                       std::wstringstream>::value));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // BSL ALIASES FOR STD DECLARATIONS
        //   The following names denote aliases for the corresponding entities
        //   in the supplied Standard Library implementation.
        //
        // Concerns:
        // 1. The forward declarations specified for <iosfwd> and not supplied
        //    by a BDE implementation are present in both the `bsl` and `std`
        //    namespaces.
        //
        // 2. The type and template names all refer to incomplete entities.
        //
        // 3. The `bsl` names alias the `std` names.
        //
        // Plan:
        // 1. Using type traits specific to this test driver, verify the
        //    declarations specified for <iosfwd> in C++23 are present in the
        //    `bsl` and `std` namespaces and do not refer to complete types or
        //    defined class templates.
        //
        // 2. Verify every name in the `bsl` namespace is an alias for the
        //    same entity with the corresponding name in the `std` namespace.
        //
        // Testing
        //   basic_filebuf
        //   basic_fstream
        //   basic_ifstream
        //   basic_ios
        //   basic_iostream
        //   basic_istream
        //   basic_ofstream
        //   basic_ostream
        //   basic_streambuf
        //   char_traits
        //   filebuf
        //   fpos
        //   fstream
        //   ifstream
        //   ios
        //   iostream
        //   istream
        //   istreambuf_iterator
        //   ofstream
        //   ostream
        //   ostreambuf_iterator
        //   streambuf
        //   streamoff
        //   streampos
        //   u16streampos   (C++11)
        //   u32streampos   (C++11)
        //   u8streampos    (C++20)
        //   wfilebuf
        //   wfstream
        //   wifstream
        //   wios
        //   wiostream
        //   wistream
        //   wofstream
        //   wostream
        //   wstreambuf
        //   wstreampos
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING BSL ALIASES FOR STD DECLARATIONS"
                          "\n=======================================");

        if (veryVerbose) puts("Testing types are incomplete");
        {
            // First test the incomplete types
            ASSERT(type_is_incomplete<bsl::filebuf>::value);
            ASSERT(type_is_incomplete<bsl::fstream>::value);
            ASSERT(type_is_incomplete<bsl::ifstream>::value);
            ASSERT(type_is_incomplete<bsl::ios>::value);
            ASSERT(type_is_incomplete<bsl::iostream>::value);
            ASSERT(type_is_incomplete<bsl::istream>::value);
            ASSERT(type_is_incomplete<bsl::ofstream>::value);
            ASSERT(type_is_incomplete<bsl::ostream>::value);
            ASSERT(type_is_incomplete<bsl::streambuf>::value);
            ASSERT(type_is_incomplete<bsl::wfilebuf>::value);
            ASSERT(type_is_incomplete<bsl::wfstream>::value);
            ASSERT(type_is_incomplete<bsl::wifstream>::value);
            ASSERT(type_is_incomplete<bsl::wios>::value);
            ASSERT(type_is_incomplete<bsl::wiostream>::value);
            ASSERT(type_is_incomplete<bsl::wistream>::value);
            ASSERT(type_is_incomplete<bsl::wofstream>::value);
            ASSERT(type_is_incomplete<bsl::wostream>::value);
            ASSERT(type_is_incomplete<bsl::wstreambuf>::value);

            // The Standard Library implementation happens to define
            // several complete types in practice, apart from LLVM libc++.
#if defined(BSLS_LIBRARYFEATURES_STDCPP_LLVM)
            ASSERT(!type_is_incomplete<bsl::streamoff>::value);
            ASSERT( type_is_incomplete<bsl::streampos>::value);
            ASSERT( type_is_incomplete<bsl::wstreampos>::value);
# if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
            ASSERT( type_is_incomplete<bsl::u16streampos>::value);
            ASSERT( type_is_incomplete<bsl::u32streampos>::value);
# endif
# if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY)
            ASSERT( type_is_incomplete<bsl::u8streampos>::value);
# endif
#else
            ASSERT(!type_is_incomplete<bsl::streamoff>::value);
            ASSERT(!type_is_incomplete<bsl::streampos>::value);
            ASSERT(!type_is_incomplete<bsl::wstreampos>::value);
# if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
            ASSERT(!type_is_incomplete<bsl::u16streampos>::value);
            ASSERT(!type_is_incomplete<bsl::u32streampos>::value);
# endif
# if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY)
            ASSERT(!type_is_incomplete<bsl::u8streampos>::value);
# endif
#endif

            // Finally test the templates.
#if defined(BSLIM_BSLSTANDARDHEADERTEST_CAN_TEST_TEMPLATES)
            ASSERT(template_is_incomplete<bsl::basic_filebuf>::value);
            ASSERT(template_is_incomplete<bsl::basic_fstream>::value);
            ASSERT(template_is_incomplete<bsl::basic_ifstream>::value);
            ASSERT(template_is_incomplete<bsl::basic_ios>::value);
            ASSERT(template_is_incomplete<bsl::basic_iostream>::value);
            ASSERT(template_is_incomplete<bsl::basic_istream>::value);
            ASSERT(template_is_incomplete<bsl::basic_ofstream>::value);
            ASSERT(template_is_incomplete<bsl::basic_ostream>::value);
            ASSERT(template_is_incomplete<bsl::basic_streambuf>::value);
            ASSERT(template_is_incomplete<bsl::char_traits>::value);
            ASSERT(template_is_incomplete<bsl::istreambuf_iterator>::value);
            ASSERT(template_is_incomplete<bsl::ostreambuf_iterator>::value);

            // `fpos` must be complete so that the aliases for narrow and wide
            // types can be declared to alias `fpos<T>::state`,
#if !defined(BSLS_LIBRARYFEATURES_STDCPP_LLVM)
            ASSERT(!template_is_incomplete<bsl::fpos>::value);
# else      // but apparently not an issue for LLVM libc++.
            ASSERT(template_is_incomplete<bsl::fpos>::value);
# endif
#endif
        }

        if (veryVerbose) puts("Testing bsl types alias std");
        {
#if defined(BSLIM_BSLSTANDARDHEADERTEST_CAN_TEST_TEMPLATES)
            ASSERT((aliases_same_template<bsl::basic_filebuf,
                                          std::basic_filebuf>::value));
            ASSERT((aliases_same_template<bsl::basic_fstream,
                                          std::basic_fstream>::value));
            ASSERT((aliases_same_template<bsl::basic_ifstream,
                                          std::basic_ifstream>::value));
            ASSERT((aliases_same_template<bsl::basic_ios,
                                          std::basic_ios>::value));
            ASSERT((aliases_same_template<bsl::basic_iostream,
                                          std::basic_iostream>::value));
            ASSERT((aliases_same_template<bsl::basic_istream,
                                          std::basic_istream>::value));
            ASSERT((aliases_same_template<bsl::basic_ofstream,
                                          std::basic_ofstream>::value));
            ASSERT((aliases_same_template<bsl::basic_ostream,
                                          std::basic_ostream>::value));
            ASSERT((aliases_same_template<bsl::basic_streambuf,
                                          std::basic_streambuf>::value));
            ASSERT((aliases_same_template<bsl::char_traits,
                                          std::char_traits>::value));
            ASSERT((aliases_same_template<bsl::fpos, std::fpos>::value));
            ASSERT((aliases_same_template<bsl::istreambuf_iterator,
                                          std::istreambuf_iterator>::value));
            ASSERT((aliases_same_template<bsl::ostreambuf_iterator,
                                          std::ostreambuf_iterator>::value));
#endif
            ASSERT((aliases_same_type<bsl::filebuf, std::filebuf>::value));
            ASSERT((aliases_same_type<bsl::fstream, std::fstream>::value));
            ASSERT((aliases_same_type<bsl::ifstream, std::ifstream>::value));
            ASSERT((aliases_same_type<bsl::ios, std::ios>::value));
            ASSERT((aliases_same_type<bsl::iostream, std::iostream>::value));
            ASSERT((aliases_same_type<bsl::istream, std::istream>::value));
            ASSERT((aliases_same_type<bsl::ofstream, std::ofstream>::value));
            ASSERT((aliases_same_type<bsl::ostream, std::ostream>::value));
            ASSERT((aliases_same_type<bsl::streambuf, std::streambuf>::value));
            ASSERT((aliases_same_type<bsl::streamoff, std::streamoff>::value));
            ASSERT((aliases_same_type<bsl::streampos, std::streampos>::value));
            ASSERT((aliases_same_type<bsl::wfilebuf, std::wfilebuf>::value));
            ASSERT((aliases_same_type<bsl::wfstream, std::wfstream>::value));
            ASSERT((aliases_same_type<bsl::wifstream, std::wifstream>::value));
            ASSERT((aliases_same_type<bsl::wios, std::wios>::value));
            ASSERT((aliases_same_type<bsl::wiostream, std::wiostream>::value));
            ASSERT((aliases_same_type<bsl::wistream, std::wistream>::value));
            ASSERT((aliases_same_type<bsl::wofstream, std::wofstream>::value));
            ASSERT((aliases_same_type<bsl::wofstream, std::wofstream>::value));
            ASSERT((aliases_same_type<bsl::wostream, std::wostream>::value));
            ASSERT((aliases_same_type<bsl::wostream, std::wostream>::value));
            ASSERT((aliases_same_type<bsl::wstreambuf,
                                      std::wstreambuf>::value));
            ASSERT((aliases_same_type<bsl::wstreampos,
                                      std::wstreampos>::value));
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
            ASSERT((aliases_same_type<bsl::u16streampos,
                                      std::u16streampos>::value));
            ASSERT((aliases_same_type<bsl::u32streampos,
                                      std::u32streampos>::value));
#endif
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY)
            ASSERT((aliases_same_type<bsl::u8streampos,
                                      std::u8streampos>::value));
#endif
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TEST MACHINERY
        //
        // Concerns:
        // 1. `aliases_same_type` has the value `true` if both template
        //    arguments are the same type.
        //
        // 2. `aliases_same_type` has the value `false` if both template
        //    arguments are different type.
        //
        // 3. `aliases_same_template` has the value `true` if both template
        //    arguments are the same templates.
        //
        // 4. `aliases_same_template` has the value `false` if both template
        //    arguments are different templates.
        //
        // Plan:
        //  1. Instentiate each template in turn with the same and different
        //     arguments.
        //
        // Testing:
        //   TEST MACHINERY
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING THE TEST MACHINERY"
                         "\n==========================");

        if (veryVerbose) puts("Validating type_is_incomplete");
        {
            ASSERT( type_is_incomplete<void>::value);
            ASSERT(!type_is_incomplete<char>::value);
            ASSERT( type_is_incomplete<Incomplete>::value);
            ASSERT( type_is_incomplete<Uncomplete>::value);
        }

        if (veryVerbose) puts("Validating aliases_same_type");
        {
            ASSERT(( aliases_same_type<void, void>::value));
            ASSERT((!aliases_same_type<void, char>::value));
            ASSERT((!aliases_same_type<void, Incomplete>::value));
            ASSERT((!aliases_same_type<Uncomplete, Incomplete>::value));
            ASSERT(( aliases_same_type<Uncomplete, Uncomplete>::value));
            ASSERT(( aliases_same_type<Incomplete, Incomplete>::value));
        }

#if defined(BSLIM_BSLSTANDARDHEADERTEST_CAN_TEST_TEMPLATES)
        if (veryVerbose) puts("Validating template_is_incomplete");
        {
            ASSERT( template_is_incomplete<SimpleTemplate>::value);
            ASSERT( template_is_incomplete<AnotherTemplate>::value);
            ASSERT( template_is_incomplete<ComplexTemplate>::value);
            ASSERT(!template_is_incomplete<CompleteTemplate>::value);
        }

        if (veryVerbose) puts("Validating aliases_same_template");
        {
            ASSERT(( aliases_same_template<SimpleTemplate,
                                           SimpleTemplate>::value));
            ASSERT((!aliases_same_template<SimpleTemplate,
                                           AnotherTemplate>::value));
            ASSERT((!aliases_same_template<SimpleTemplate,
                                           ComplexTemplate>::value));
            ASSERT((aliases_same_template<ComplexTemplate,
                                          ComplexTemplate>::value));
            ASSERT((!aliases_same_template<ComplexTemplate,
                                           CompleteTemplate>::value));
            ASSERT((aliases_same_template<CompleteTemplate,
                                          CompleteTemplate>::value));
        }
#endif
      } break;

      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
