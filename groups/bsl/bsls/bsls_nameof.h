// bsls_nameof.h                                                      -*-C++-*-
#ifndef INCLUDED_BSLS_NAMEOF
#define INCLUDED_BSLS_NAMEOF

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a 'NameOf' type for displaying template type at run-time.
//
//@CLASSES:
//  bsls::NameOf: template class to return name of template parameter
//
//@FREE FUNCTIONS
//  bsls::nameOfType(const TYPE&): template function to return name of 'TYPE'
//
//@DESCRIPTION: This component provides a template class,
// 'bsls::NameOf<TYPE>', which can implicitly cast to a 'const char *' which
// will point to a description of 'TYPE'.
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1:
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// First, your test driver must have the following 'using' statements so that
// the template class 'NameOf' and the template function 'nameOfType' can be
// referred to concisely, without having to qualify them with namespaces on
// each call.  Note that if you've already said 'using namespace BloombergLP'
// you don't have to give the 'BloombergLP::' qualifiers here:
//..
//  using BloombergLP::bsls::NameOf;
//  using BloombergLP::bsls::nameOfType;
//..
// Next, we define some types in the unnamed namespace:
//..
//  namespace {
//
//  struct MyType {
//      int  d_i;
//      char d_c;
//  };
//
//  union MyUnion {
//      int  d_i;
//      char d_buffer[100];
//  };
//
//  }  // close unnamed namespace
//..
// Next, we see that the 'NameOf' template class, when created with a type, can
// be implicitly cast to a 'const char *' which points to a description of the
// type.
//..
//  assert(!std::strcmp("double", NameOf<double>()));
//  assert(!std::strcmp("int",    NameOf<int>()));
//..
// Then, we see that when 'NameOf' is passed a 'typedef' or template parameter,
// it resolves it to the original type:
//..
//  typedef int Woof;
//
//  assert(!std::strcmp("int",    NameOf<Woof>()));
//..
// Next, we introduce the 'nameOfType' template function, which takes as any
// variable as an argument, and returns a 'const char *' pointing to a
// description of the type of the variable.
//..
//  int ii = 2;
//
//  assert(!std::strcmp("int",    nameOfType(ii)));
//..
// Then, we see that 'NameOf' and 'nameOfType' will strip 'BloombergLP::'
// namespace qualifiers, as well as anonymous namespace qualifiers.
//..
//  typedef BloombergLP::bsls::Stopwatch SW;
//
//  const SW      sw;
//  const MyType  mt = { 2, 'a' };
//  MyUnion       mu;
//  mu.d_i = 7;
//
//  assert(!std::strcmp("bsls::Stopwatch", NameOf<SW>()));
//  assert(!std::strcmp("bsls::Stopwatch",
//                                    NameOf<BloombergLP::bsls::Stopwatch>()));
//  assert(!std::strcmp("bsls::Stopwatch", nameOfType(sw)));
//
//  assert(!std::strcmp("MyType",          NameOf<MyType>()));
//  assert(!std::strcmp("MyType",          nameOfType(mt)));
//
//  assert(!std::strcmp("MyUnion",         NameOf<MyUnion>()));
//  assert(!std::strcmp("MyUnion",         nameOfType(mu)));
//..
// There is a problem with template code not knowing how to implicitly cast the
// 'NameOf' type to 'const char *' for initializing or comparing with
// 'std::string's.  To facilitate, 'NameOf' provides a 'const char *' 'name'
// accessor, to avoid the user having to do a more verbose 'static cast'.
//..
//  const std::string swName = "bsls::Stopwatch";
//  assert(swName == static_cast<const char *>(NameOf<SW>()));
//  assert(swName == NameOf<SW>().name());
//
//  const std::string swNameB = NameOf<SW>().name();
//  assert(swNameB == swName);
//
//  printf("NameOf<SW>() = \"%s\"\n", NameOf<SW>().name());
//  printf("NameOfType(4 + 3) = \"%s\"\n", nameOfType(4 + 3));
//..
// Note that 'nameOfType' naturally returns a 'const char *' and needs no help
// casting.  Note also that 'bsls::debugprint' is able to figure out how to
// cast 'NameOf' directly to 'const char *' with no problems, as can iostreams,
// so there is no problem with putting a 'NameOf' in a 'LOOP_ASSERT' or
// 'ASSERTV'.  It is anticipated that displaying by the BDE 'ASSERTV',
// 'LOOP_ASSERT, and 'P' macros will be the primary use of this component.
//..
//  printf("NameOf<double>() = ");
//  BloombergLP::bsls::debugprint(NameOf<double>());
//  printf("\n");
//
//  typedef double DTYPE;
//  DTYPE x = 7.3;
//
//  LOOP_ASSERT(NameOf<DTYPE>(), x > 7);
//
//  std::string myStr;              // Assign, not init, of string doesn't need
//  myStr = NameOf<DTYPE>();        // '.name()'.
//  assert("double" == myStr);
//..
// Which produces:
//..
//  NameOf<SW>() = "bsls::Stopwatch"
//..
// Finally, we see that 'NameOf' and 'nameOfType' will simplifiy
// 'std::basic_string<...>' declarations to 'std::string'.
//..
//  const std::string s = "std::string";
//
//  assert(s == NameOf<std::basic_string<char> >().name());
//  assert(s == NameOf<std::string>().name());
//  assert(s == nameOfType(s));
//
//  typedef NameOf<std::string> Nos;
//
//  const std::string s2 = "bsls::NameOf<std::string>";
//
//  assert(s2 == NameOf<NameOf<std::basic_string<char> > >().name());
//  assert(s2 == NameOf<NameOf<std::string> >().name());
//  assert(s2 == NameOf<Nos>().name());
//  assert(s2 == nameOfType(Nos()));
//..

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_ATOMIC
#include <bsls_atomic.h>
#endif

#ifndef INCLUDED_BSLS_BSLONCE
#include <bsls_bslonce.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

namespace BloombergLP {
namespace bsls {

                        // =======================
                        // class bsls::NameOf_Base
                        // =======================

class NameOf_Base {
    // This 'class' provide non-template implementation code for the 'NameOf'
    // template class.

  protected:
    // PROTECTED TYPES
#if defined(BSLS_PLATFORM_CMP_SUN)
    enum { k_BUF_SIZE_SOLARIS_CC = 256 };
#endif

    // On all platforms, the 'functionName' passed to 'initBuffer' will start
    // with the contents of local buffer 'uselessPreamble', which will not make
    // it into the final buffer, so we know that the final buffer can be
    // trimmed of this length.

#if defined(BSLS_PLATFORM_CMP_MSVC)
# if defined(BSLS_PLATFORM_CPU_64_BIT)
    enum { k_USELESS_PREAMBLE_LEN = 34 };
# else
    enum { k_USELESS_PREAMBLE_LEN = 37 };
# endif
#else
    enum { k_USELESS_PREAMBLE_LEN = 26 };
#endif

    // PROTECTED CLASS METHOD
    static const char *initBuffer(char       *buffer,
                                  const char *functionName);
        // Initialize the specified 'buffer' with the type name contained in
        // the specified 'functionName', where 'functionName' is the function
        // name of the 'NameOf' constructor.  Return either a pointer to
        // 'buffer', or if 'buffer' couldn't be properly initialized,
        // 'functionName'.
};

                            // ==================
                            // class bsls::NameOf
                            // ==================

template <class TYPE>
class NameOf : public NameOf_Base {
    // This 'class' provides a means to display the type name of its template
    // parameter 'TYPE'.  An instance of this 'class' can be implicitly (or
    // explicitly via the 'name' accessor) cast to a 'const char *' which will
    // point to a buffer containing the description of the type.  Note that all
    // instances of a given type will refer to the same character buffer
    // containing the name.

    // CLASS DATA
    static bsls::AtomicPointer<const char> s_buffer_p;

  public:
    // CREATOR
    NameOf();
        // Initialize the base class of this object to the name of 'TYPE'.

    // ACCESSOR
    operator const char *() const;
        // Return a pointer to the a string containing the name of 'TYPE'.

    const char *name() const;
        // Return a pointer to the a string containing the name of 'TYPE', this
        // serves as a convenient way to explicitly cast the return value to a
        // 'const char *'.
};

// ============================================================================
//                        TEMPLATE FUNCTION DEFINITIONS
// ----------------------------------------------------------------------------

                              // ------------------
                              // class bsls::NameOf
                              // ------------------

// CLASS DATA
template <class TYPE>
bsls::AtomicPointer<const char> NameOf<TYPE>::s_buffer_p;

// CREATOR
template <class TYPE>
NameOf<TYPE>::NameOf()
    // Initialize the base class of this object to name of 'TYPE'.
{
    // It is important to ensure that no two threads are initializing the same
    // buffer at the same time.

    static BslOnce once = BSLS_BSLONCE_INITIALIZER;
    BslOnceGuard   onceGuard;

    if (!s_buffer_p && onceGuard.enter(&once)) {
#if   defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
        static char buffer[sizeof(__PRETTY_FUNCTION__) -
                                                       k_USELESS_PREAMBLE_LEN];
        s_buffer_p = initBuffer(buffer, __PRETTY_FUNCTION__);
#elif defined(BSLS_PLATFORM_CMP_SUN)
# if  BSLS_PLATFORM_CMP_VERSION >= 20768
        // The Solaris CC compiler doesn't understand
        // 'sizeof(__PRETTY_FUNCTION__)'.

        static char buffer[k_BUF_SIZE_SOLARIS_CC];
        s_buffer_p = initBuffer(buffer, __PRETTY_FUNCTION__);
# else
        // '__PRETTY_FUNCTION__' not supported, only '__FUNCTION__', which
        // doesn't mention 'TYPE', so we can't deduce the name of 'TYPE'

        s_buffer_p = "unknown_type";
# endif
#elif defined(BSLS_PLATFORM_CMP_IBM)
        static char buffer[sizeof(__FUNCTION__) - k_USELESS_PREAMBLE_LEN];
        s_buffer_p = initBuffer(buffer, __FUNCTION__);
#elif defined(BSLS_PLATFORM_CMP_MSVC)
        static char buffer[sizeof(__FUNCSIG__) - k_USELESS_PREAMBLE_LEN];
        s_buffer_p = initBuffer(buffer, __FUNCSIG__);
#else
# error No function signature macro defined.
#endif

        BSLS_ASSERT_SAFE(s_buffer_p);
    }
}

// ACCESSOR
template <class TYPE>
inline
NameOf<TYPE>::operator const char *() const
{
    return s_buffer_p;
}

template <class TYPE>
inline
const char *NameOf<TYPE>::name() const
{
    return s_buffer_p;
}

// FREE FUNCTIONS
template <class TYPE>
const char *nameOfType(const TYPE&)
    // Return the name of the type of the object passed to this function.
{
    return NameOf<TYPE>();
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
