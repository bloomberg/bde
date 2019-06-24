// bsltf_streamutil.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLTF_STREAMUTIL
#define INCLUDED_BSLTF_STREAMUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide streaming output operators for bsltf-defined types.
//
//@CLASSES:
//
//@SEE_ALSO: bslim_testutil, bslmt_testutil
//
//@AUTHOR: Bill Chapman (bchapman2)

//@DESCRIPTION: BDE components that define types normally define the
// 'operator<<' used to stream the type out to standard library streams.
// 'bsltf' components cannot do this for two reasons:
//
//: 1 Output should be based on the 'getIdentifier' method, and those methods
//:   are all static members of 'TemplateTestFacility', so a streaming operator
//:   cannot be defined in the components where the 'bsltf' types are defined,
//:   since those components are below 'TemplateTestFacility'.
//:
//: 2 It is our policy that all output in 'bsl' be done with 'printf'-style
//:   output until above 'bsl+bslhdrs', where 'bsl::ostream' is defined.  The
//:   'bsltf' package is far below 'bsl+bslhdrs'.
//
// We put the streaming operators here, above 'TemplateTestFacility', solving
// concern 1, and make the 'stream' parameter of these 'operator<<' functions
// be a template parameter, so that the functions can be defined without
// needing 'bsl::ostream' to be declared, solving concern 2.
//
///Usage
///-----
// We have defined a 'cout' object that can stream 'int's and 'const char *'
// null-terminated strings.  Note that the operators defined by this component
// require only that the left-hand argument be able to stream 'int's, so our
// 'cout' more than satisfies this requirement.
//
// Then, in 'main', we define a 'bsltf::SimpleTestType' object 'st':
//..
//  BloombergLP::bsltf::SimpleTestType st;
//..
// Now, we do some output, assigning 'st' several different values and
// streaming them out:
//..
//  cout << "Several values of 'st': ";
//
//  for (int ii = 0; ii <= 100; ii += 20) {
//      st.setData(ii);
//
//      cout << (ii ? ", " : "") << st;
//  }
//  cout << "\n";
//..
// Finally, we observe the output, which is:
//..
//  Several values of 'st': 0, 20, 40, 60, 80, 100
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLTF_TEMPLATETESTFACILITY
#include <bsltf_templatetestfacility.h>
#endif

namespace BloombergLP {
namespace bsltf {

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

// FREE OPERATORS
template <class STREAM>
inline
STREAM& operator<<(STREAM&                                stream,
                   const TemplateTestFacility::MethodPtr& object)
    // Stream the integer representing the value of the specified 'object' to
    // the specified 'stream'.  Return 'stream'.
{
    stream << TemplateTestFacility::getIdentifier(object);
    return stream;
}

template <class STREAM>
inline
STREAM& operator<<(STREAM&                             stream,
                   const AllocBitwiseMoveableTestType& object)
    // Stream the integer representing the value of the specified 'object' to
    // the specified 'stream'.  Return 'stream'.
{
    stream << TemplateTestFacility::getIdentifier(object);
    return stream;
}

template <class STREAM>
inline
STREAM& operator<<(STREAM&                        stream,
                   const AllocEmplacableTestType& object)
    // Stream the integer representing the value of the specified 'object' to
    // the specified 'stream'.  Return 'stream'.
{
    stream << TemplateTestFacility::getIdentifier(object);
    return stream;
}

template <class STREAM>
inline
STREAM& operator<<(STREAM&              stream,
                   const AllocTestType& object)
    // Stream the integer representing the value of the specified 'object' to
    // the specified 'stream'.  Return 'stream'.
{
    stream << TemplateTestFacility::getIdentifier(object);
    return stream;
}

template <class STREAM>
inline
STREAM& operator<<(STREAM&                        stream,
                   const BitwiseCopyableTestType& object)
    // Stream the integer representing the value of the specified 'object' to
    // the specified 'stream'.  Return 'stream'.
{
    stream << TemplateTestFacility::getIdentifier(object);
    return stream;
}

template <class STREAM>
inline
STREAM& operator<<(STREAM&                        stream,
                   const BitwiseMoveableTestType& object)
    // Stream the integer representing the value of the specified 'object' to
    // the specified 'stream'.  Return 'stream'.
{
    stream << TemplateTestFacility::getIdentifier(object);
    return stream;
}

template <class STREAM>
inline
STREAM& operator<<(STREAM&                   stream,
                   const EmplacableTestType& object)
    // Stream the integer representing the value of the specified 'object' to
    // the specified 'stream'.  Return 'stream'.
{
    stream << TemplateTestFacility::getIdentifier(object);
    return stream;
}

template <class STREAM>
inline
STREAM& operator<<(STREAM&                         stream,
                   const EnumeratedTestType::Enum& object)
    // Stream the integer representing the value of the specified 'object' to
    // the specified 'stream'.  Return 'stream'.
{
    stream << TemplateTestFacility::getIdentifier(object);
    return stream;
}

template <class STREAM>
inline
STREAM& operator<<(STREAM&                     stream,
                   const MovableAllocTestType& object)
    // Stream the integer representing the value of the specified 'object' to
    // the specified 'stream'.  Return 'stream'.
{
    stream << TemplateTestFacility::getIdentifier(object);
    return stream;
}

template <class STREAM>
inline
STREAM& operator<<(STREAM&                stream,
                   const MovableTestType& object)
    // Stream the integer representing the value of the specified 'object' to
    // the specified 'stream'.  Return 'stream'.
{
    stream << TemplateTestFacility::getIdentifier(object);
    return stream;
}

template <class STREAM>
inline
STREAM& operator<<(STREAM&                      stream,
                   const MoveOnlyAllocTestType& object)
    // Stream the integer representing the value of the specified 'object' to
    // the specified 'stream'.  Return 'stream'.
{
    stream << TemplateTestFacility::getIdentifier(object);
    return stream;
}

template <class STREAM>
inline
STREAM& operator<<(STREAM&                      stream,
                   const NonAssignableTestType& object)
    // Stream the integer representing the value of the specified 'object' to
    // the specified 'stream'.  Return 'stream'.
{
    stream << TemplateTestFacility::getIdentifier(object);
    return stream;
}

template <class STREAM>
inline
STREAM& operator<<(STREAM&                             stream,
                   const NonCopyConstructibleTestType& object)
    // Stream the integer representing the value of the specified 'object' to
    // the specified 'stream'.  Return 'stream'.
{
    stream << TemplateTestFacility::getIdentifier(object);
    return stream;
}

template <class STREAM>
inline
STREAM& operator<<(STREAM&                                stream,
                   const NonDefaultConstructibleTestType& object)
    // Stream the integer representing the value of the specified 'object' to
    // the specified 'stream'.  Return 'stream'.
{
    stream << TemplateTestFacility::getIdentifier(object);
    return stream;
}

template <class STREAM>
inline
STREAM& operator<<(STREAM&                           stream,
                   const NonEqualComparableTestType& object)
    // Stream the integer representing the value of the specified 'object' to
    // the specified 'stream'.  Return 'stream'.
{
    stream << TemplateTestFacility::getIdentifier(object);
    return stream;
}

template <class STREAM>
inline
STREAM& operator<<(STREAM&                         stream,
                   const NonOptionalAllocTestType& object)
    // Stream the integer representing the value of the specified 'object' to
    // the specified 'stream'.  Return 'stream'.
{
    stream << TemplateTestFacility::getIdentifier(object);
    return stream;
}

template <class STREAM>
inline
STREAM& operator<<(STREAM&                            stream,
                   const NonTypicalOverloadsTestType& object)
    // Stream the integer representing the value of the specified 'object' to
    // the specified 'stream'.  Return 'stream'.
{
    stream << TemplateTestFacility::getIdentifier(object);
    return stream;
}

template <class STREAM>
inline
STREAM& operator<<(STREAM&               stream,
                   const SimpleTestType& object)
    // Stream the integer representing the value of the specified 'object' to
    // the specified 'stream'.  Return 'stream'.
{
    stream << TemplateTestFacility::getIdentifier(object);
    return stream;
}

template <class STREAM, class ALLOC>
inline
STREAM& operator<<(STREAM&                        stream,
                   const StdAllocTestType<ALLOC>& object)
    // Stream the integer representing the value of the specified 'object' to
    // the specified 'stream'.  Return 'stream'.
{
    stream << TemplateTestFacility::getIdentifier(object);
    return stream;
}

template <class STREAM>
inline
STREAM& operator<<(STREAM&              stream,
                   const UnionTestType& object)
    // Stream the integer representing the value of the specified 'object' to
    // the specified 'stream'.  Return 'stream'.
{
    stream << TemplateTestFacility::getIdentifier(object);
    return stream;
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
