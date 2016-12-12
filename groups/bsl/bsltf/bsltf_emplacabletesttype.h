// bsltf_emplacabletesttype.h                                         -*-C++-*-
#ifndef INCLUDED_BSLTF_EMPLACABLETESTTYPE
#define INCLUDED_BSLTF_EMPLACABLETESTTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a non-allocating test class used to test 'emplace' methods.
//
//@CLASSES:
//   bsltf::EmplacableTestType: non-allocating test class with 0..14 arguments
//
//@SEE_ALSO: bsltf_argumenttype, bsltf_templatetestfacility
//
//@DESCRIPTION: This component provides a (value-semantic) attribute class,
// 'bsltf::EmplacableTestType', that is used to ensure that arguments are
// forwarded correctly to a type's constructor.  This component is similar to
// 'bsltf_allocemplacabletesttype', but provides a type that does not allocate
// on construction.
//
///Attributes
///----------
//..
//  Name     Type                      Default
//  -------  ------------------------  -------
//  arg01    bsltf::ArgumentType< 1>   -1
//  arg02    bsltf::ArgumentType< 2>   -1
//  arg03    bsltf::ArgumentType< 3>   -1
//  arg04    bsltf::ArgumentType< 4>   -1
//  arg05    bsltf::ArgumentType< 5>   -1
//  arg06    bsltf::ArgumentType< 6>   -1
//  arg07    bsltf::ArgumentType< 7>   -1
//  arg08    bsltf::ArgumentType< 8>   -1
//  arg09    bsltf::ArgumentType< 9>   -1
//  arg10    bsltf::ArgumentType<10>   -1
//  arg11    bsltf::ArgumentType<11>   -1
//  arg12    bsltf::ArgumentType<12>   -1
//  arg13    bsltf::ArgumentType<13>   -1
//  arg14    bsltf::ArgumentType<14>   -1
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
// See 'bsltf_allocemplacabletesttype' for usage example.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLTF_ARGUMENTTYPE
#include <bsltf_argumenttype.h>
#endif

namespace BloombergLP {
namespace bsltf {

                         // ========================
                         // class EmplacableTestType
                         // ========================

class EmplacableTestType {
    // This class provides a test object used to check that the arguments
    // passed for creating an object with an in-place representation are of the
    // correct type and value.

  public:
    // PUBLIC TYPES
    typedef bsltf::ArgumentType< 1> ArgType01;
    typedef bsltf::ArgumentType< 2> ArgType02;
    typedef bsltf::ArgumentType< 3> ArgType03;
    typedef bsltf::ArgumentType< 4> ArgType04;
    typedef bsltf::ArgumentType< 5> ArgType05;
    typedef bsltf::ArgumentType< 6> ArgType06;
    typedef bsltf::ArgumentType< 7> ArgType07;
    typedef bsltf::ArgumentType< 8> ArgType08;
    typedef bsltf::ArgumentType< 9> ArgType09;
    typedef bsltf::ArgumentType<10> ArgType10;
    typedef bsltf::ArgumentType<11> ArgType11;
    typedef bsltf::ArgumentType<12> ArgType12;
    typedef bsltf::ArgumentType<13> ArgType13;
    typedef bsltf::ArgumentType<14> ArgType14;

  private:
    // DATA
    ArgType01 d_arg01;
    ArgType02 d_arg02;
    ArgType03 d_arg03;
    ArgType04 d_arg04;
    ArgType05 d_arg05;
    ArgType06 d_arg06;
    ArgType07 d_arg07;
    ArgType08 d_arg08;
    ArgType09 d_arg09;
    ArgType10 d_arg10;
    ArgType11 d_arg11;
    ArgType12 d_arg12;
    ArgType13 d_arg13;
    ArgType14 d_arg14;

    // CLASS DATA
    static int s_numDeletes;
        // Track number of times the destructor is called.

  public:
    // CLASS METHODS
    static int getNumDeletes();
        // Return the number of times an object of this type has been
        // destroyed.

    // CREATORS
    EmplacableTestType();
    explicit EmplacableTestType(ArgType01 arg01);
    EmplacableTestType(ArgType01 arg01, ArgType02 arg02);
    EmplacableTestType(ArgType01 arg01, ArgType02 arg02, ArgType03 arg03);
    EmplacableTestType(ArgType01 arg01,
                       ArgType02 arg02,
                       ArgType03 arg03,
                       ArgType04 arg04);
    EmplacableTestType(ArgType01 arg01,
                       ArgType02 arg02,
                       ArgType03 arg03,
                       ArgType04 arg04,
                       ArgType05 arg05);
    EmplacableTestType(ArgType01 arg01,
                       ArgType02 arg02,
                       ArgType03 arg03,
                       ArgType04 arg04,
                       ArgType05 arg05,
                       ArgType06 arg06);
    EmplacableTestType(ArgType01 arg01,
                       ArgType02 arg02,
                       ArgType03 arg03,
                       ArgType04 arg04,
                       ArgType05 arg05,
                       ArgType06 arg06,
                       ArgType07 arg07);
    EmplacableTestType(ArgType01 arg01,
                       ArgType02 arg02,
                       ArgType03 arg03,
                       ArgType04 arg04,
                       ArgType05 arg05,
                       ArgType06 arg06,
                       ArgType07 arg07,
                       ArgType08 arg08);
    EmplacableTestType(ArgType01 arg01,
                       ArgType02 arg02,
                       ArgType03 arg03,
                       ArgType04 arg04,
                       ArgType05 arg05,
                       ArgType06 arg06,
                       ArgType07 arg07,
                       ArgType08 arg08,
                       ArgType09 arg09);
    EmplacableTestType(ArgType01 arg01,
                       ArgType02 arg02,
                       ArgType03 arg03,
                       ArgType04 arg04,
                       ArgType05 arg05,
                       ArgType06 arg06,
                       ArgType07 arg07,
                       ArgType08 arg08,
                       ArgType09 arg09,
                       ArgType10 arg10);
    EmplacableTestType(ArgType01 arg01,
                       ArgType02 arg02,
                       ArgType03 arg03,
                       ArgType04 arg04,
                       ArgType05 arg05,
                       ArgType06 arg06,
                       ArgType07 arg07,
                       ArgType08 arg08,
                       ArgType09 arg09,
                       ArgType10 arg10,
                       ArgType11 arg11);
    EmplacableTestType(ArgType01 arg01,
                       ArgType02 arg02,
                       ArgType03 arg03,
                       ArgType04 arg04,
                       ArgType05 arg05,
                       ArgType06 arg06,
                       ArgType07 arg07,
                       ArgType08 arg08,
                       ArgType09 arg09,
                       ArgType10 arg10,
                       ArgType11 arg11,
                       ArgType12 arg12);
    EmplacableTestType(ArgType01 arg01,
                       ArgType02 arg02,
                       ArgType03 arg03,
                       ArgType04 arg04,
                       ArgType05 arg05,
                       ArgType06 arg06,
                       ArgType07 arg07,
                       ArgType08 arg08,
                       ArgType09 arg09,
                       ArgType10 arg10,
                       ArgType11 arg11,
                       ArgType12 arg12,
                       ArgType13 arg13);
    EmplacableTestType(ArgType01 arg01,
                       ArgType02 arg02,
                       ArgType03 arg03,
                       ArgType04 arg04,
                       ArgType05 arg05,
                       ArgType06 arg06,
                       ArgType07 arg07,
                       ArgType08 arg08,
                       ArgType09 arg09,
                       ArgType10 arg10,
                       ArgType11 arg11,
                       ArgType12 arg12,
                       ArgType13 arg13,
                       ArgType14 arg14);
        // Create an 'EmplacableTestType' object by initializing corresponding
        // attributes with the specified 'arg01'..'arg14', and initializing any
        // remaining attributes with their default value (-1).

    EmplacableTestType(const EmplacableTestType& original);
        // Create an in-place test object having the same value as the
        // specified 'original'.

    ~EmplacableTestType();
        // Increment the count of calls to this destructor, and destroy this
        // object.

    // MANIPULATORS
    //! EmplacableTestType& operator=(const EmplacableTestType& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    // ACCESSORS
    const ArgType01& arg01() const;
    const ArgType02& arg02() const;
    const ArgType03& arg03() const;
    const ArgType04& arg04() const;
    const ArgType05& arg05() const;
    const ArgType06& arg06() const;
    const ArgType07& arg07() const;
    const ArgType08& arg08() const;
    const ArgType09& arg09() const;
    const ArgType10& arg10() const;
    const ArgType11& arg11() const;
    const ArgType12& arg12() const;
    const ArgType13& arg13() const;
    const ArgType14& arg14() const;
        // Return the value of the correspondingly numbered argument that was
        // passed to the constructor of this object.

    bool isEqual(const EmplacableTestType& other) const;
        // Return 'true' if the specified 'other' object has the same value as
        // this object, and 'false' otherwise.  Two 'EmplacableTestType'
        // objects have the same value if each of their corresponding
        // attributes have the same value.
};

// FREE OPERATORS
bool operator==(const EmplacableTestType& lhs, const EmplacableTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'EmplacableTestType' objects have the
    // same value if each of their corresponding attributes have the same
    // value.

bool operator!=(const EmplacableTestType& lhs, const EmplacableTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'EmplacableTestType' objects do
    // not have the same value if any of their corresponding attributes do not
    // have the same value.

// ============================================================================
//                      INLINE DEFINITIONS
// ============================================================================

                        // ------------------------
                        // class EmplacableTestType
                        // ------------------------

// ACCESSORS
inline
const EmplacableTestType::ArgType01& EmplacableTestType::arg01() const
{
    return d_arg01;
}

inline
const EmplacableTestType::ArgType02& EmplacableTestType::arg02() const
{
    return d_arg02;
}

inline
const EmplacableTestType::ArgType03& EmplacableTestType::arg03() const
{
    return d_arg03;
}

inline
const EmplacableTestType::ArgType04& EmplacableTestType::arg04() const
{
    return d_arg04;
}

inline
const EmplacableTestType::ArgType05& EmplacableTestType::arg05() const
{
    return d_arg05;
}

inline
const EmplacableTestType::ArgType06& EmplacableTestType::arg06() const
{
    return d_arg06;
}

inline
const EmplacableTestType::ArgType07& EmplacableTestType::arg07() const
{
    return d_arg07;
}

inline
const EmplacableTestType::ArgType08& EmplacableTestType::arg08() const
{
    return d_arg08;
}

inline
const EmplacableTestType::ArgType09& EmplacableTestType::arg09() const
{
    return d_arg09;
}

inline
const EmplacableTestType::ArgType10& EmplacableTestType::arg10() const
{
    return d_arg10;
}

inline
const EmplacableTestType::ArgType11& EmplacableTestType::arg11() const
{
    return d_arg11;
}

inline
const EmplacableTestType::ArgType12& EmplacableTestType::arg12() const
{
    return d_arg12;
}

inline
const EmplacableTestType::ArgType13& EmplacableTestType::arg13() const
{
    return d_arg13;
}

inline
const EmplacableTestType::ArgType14& EmplacableTestType::arg14() const
{
    return d_arg14;
}

inline
bool EmplacableTestType::isEqual(const EmplacableTestType& other) const
{
    return d_arg01 == other.d_arg01
        && d_arg02 == other.d_arg02
        && d_arg03 == other.d_arg03
        && d_arg04 == other.d_arg04
        && d_arg05 == other.d_arg05
        && d_arg06 == other.d_arg06
        && d_arg07 == other.d_arg07
        && d_arg08 == other.d_arg08
        && d_arg09 == other.d_arg09
        && d_arg10 == other.d_arg10
        && d_arg11 == other.d_arg11
        && d_arg12 == other.d_arg12
        && d_arg13 == other.d_arg13
        && d_arg14 == other.d_arg14;
}

// FREE OPERATORS
inline
bool operator==(const EmplacableTestType& lhs, const EmplacableTestType& rhs)
{
    return lhs.isEqual(rhs);
}

inline
bool operator!=(const EmplacableTestType& lhs, const EmplacableTestType& rhs)
{
    return !lhs.isEqual(rhs);
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
