// bsltf_emplacabletesttype.h                                         -*-C++-*-
#ifndef INCLUDED_BSLTF_EMPLACABLETESTTYPE
#define INCLUDED_BSLTF_EMPLACABLETESTTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a non-allocating test class used to test 'emplace' methods.
//
//@REVIEW_FOR_MASTER:
//
//@CLASSES:
//   EmplacableTestType: non-allocating test class taking up to 14 arguments
//
//@SEE_ALSO: bsltf_templatetestfacility
//
//@DESCRIPTION: This component provides a (value-semantic) attribute class,
// 'EmplacableTestType', that is used to ensure that arguments are forwarded
// correctly to functions and methods taking an arbitrary number of arguments.
//
//
// TBD: fix this up
///Attributes
///----------
//..
//  Name                Type         Default
//  ------------------  -----------  -------
//  data                int          0
//..
//: o 'data': representation of the class value
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: TBD
/// - - - - - - -
// Suppose we wanted to ...

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
    // passed for creating a shared pointer with an in-place representation are
    // of the correct types and values.

  public:
    // TYPEDEFS
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
    ArgType01 d_a01;
    ArgType02 d_a02;
    ArgType03 d_a03;
    ArgType04 d_a04;
    ArgType05 d_a05;
    ArgType06 d_a06;
    ArgType07 d_a07;
    ArgType08 d_a08;
    ArgType09 d_a09;
    ArgType10 d_a10;
    ArgType11 d_a11;
    ArgType12 d_a12;
    ArgType13 d_a13;
    ArgType14 d_a14;

    // DATA
    static int s_numDeletes;
        // Track number of times the destructor is called.

  public:
    // CLASS METHODS
    static int getNumDeletes();
        // Return the number of times an object of this type has been
        // destroyed.

    // CREATORS
    EmplacableTestType();
    explicit EmplacableTestType(ArgType01 a01);
    EmplacableTestType(ArgType01 a01, ArgType02 a02);
    EmplacableTestType(ArgType01 a01, ArgType02 a02, ArgType03 a03);
    EmplacableTestType(ArgType01 a01,
                       ArgType02 a02,
                       ArgType03 a03,
                       ArgType04 a04);
    EmplacableTestType(ArgType01 a01,
                       ArgType02 a02,
                       ArgType03 a03,
                       ArgType04 a04,
                       ArgType05 a05);
    EmplacableTestType(ArgType01 a01,
                       ArgType02 a02,
                       ArgType03 a03,
                       ArgType04 a04,
                       ArgType05 a05,
                       ArgType06 a06);
    EmplacableTestType(ArgType01 a01,
                       ArgType02 a02,
                       ArgType03 a03,
                       ArgType04 a04,
                       ArgType05 a05,
                       ArgType06 a06,
                       ArgType07 a07);
    EmplacableTestType(ArgType01 a01,
                       ArgType02 a02,
                       ArgType03 a03,
                       ArgType04 a04,
                       ArgType05 a05,
                       ArgType06 a06,
                       ArgType07 a07,
                       ArgType08 a08);
    EmplacableTestType(ArgType01 a01,
                       ArgType02 a02,
                       ArgType03 a03,
                       ArgType04 a04,
                       ArgType05 a05,
                       ArgType06 a06,
                       ArgType07 a07,
                       ArgType08 a08,
                       ArgType09 a09);
    EmplacableTestType(ArgType01 a01,
                       ArgType02 a02,
                       ArgType03 a03,
                       ArgType04 a04,
                       ArgType05 a05,
                       ArgType06 a06,
                       ArgType07 a07,
                       ArgType08 a08,
                       ArgType09 a09,
                       ArgType10 a10);
    EmplacableTestType(ArgType01 a01,
                       ArgType02 a02,
                       ArgType03 a03,
                       ArgType04 a04,
                       ArgType05 a05,
                       ArgType06 a06,
                       ArgType07 a07,
                       ArgType08 a08,
                       ArgType09 a09,
                       ArgType10 a10,
                       ArgType11 a11);
    EmplacableTestType(ArgType01 a01,
                       ArgType02 a02,
                       ArgType03 a03,
                       ArgType04 a04,
                       ArgType05 a05,
                       ArgType06 a06,
                       ArgType07 a07,
                       ArgType08 a08,
                       ArgType09 a09,
                       ArgType10 a10,
                       ArgType11 a11,
                       ArgType12 a12);
    EmplacableTestType(ArgType01 a01,
                       ArgType02 a02,
                       ArgType03 a03,
                       ArgType04 a04,
                       ArgType05 a05,
                       ArgType06 a06,
                       ArgType07 a07,
                       ArgType08 a08,
                       ArgType09 a09,
                       ArgType10 a10,
                       ArgType11 a11,
                       ArgType12 a12,
                       ArgType13 a13);
    EmplacableTestType(ArgType01 a01,
                       ArgType02 a02,
                       ArgType03 a03,
                       ArgType04 a04,
                       ArgType05 a05,
                       ArgType06 a06,
                       ArgType07 a07,
                       ArgType08 a08,
                       ArgType09 a09,
                       ArgType10 a10,
                       ArgType11 a11,
                       ArgType12 a12,
                       ArgType13 a13,
                       ArgType14 a14);
        // Create an 'EmplacableTestType' by initializing the data members
        // 'd_a1'..'d_a14' with the specified 'a1'..'a14', and initializing any
        // remaining data members with their default value (-1).

    EmplacableTestType(const EmplacableTestType& original);
        // Create an in-place test object having the same value as the
        // specified 'original'.

    ~EmplacableTestType();
        // Increment the count of calls to this destructor, and destroy this
        // object.

    // ACCESSORS
    bool isEqual(const EmplacableTestType& rhs) const;
        // Return 'true' if the specified 'rhs' has the same value as this
        // object, and 'false' otherwise.  Two 'EmplacableTestType' objects have
        // the same value if each of their corresponding data members
        // 'd1'..'d14' have the same value.

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

};

// FREE OPERATORS
bool operator==(const EmplacableTestType& lhs, const EmplacableTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects has the same
    // value, and 'false' otherwise.  Two 'EmplacableTestType' objects have the
    // same value if each of their corresponding data members 'd1'..'d14' have
    // the same value.

bool operator!=(const EmplacableTestType& lhs, const EmplacableTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'EmplacableTestType' objects have
    // the same value if each of their corresponding data members 'd1'..'d14'
    // have the same value.

// ============================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

                        // ------------------------
                        // class EmplacableTestType
                        // ------------------------

// CREATORS
// MANIPULATORS


// ACCESSORS
inline
const EmplacableTestType::ArgType01& EmplacableTestType::arg01() const
{
    return d_a01;
}

inline
const EmplacableTestType::ArgType02& EmplacableTestType::arg02() const
{
    return d_a02;
}

inline
const EmplacableTestType::ArgType03& EmplacableTestType::arg03() const
{
    return d_a03;
}

inline
const EmplacableTestType::ArgType04& EmplacableTestType::arg04() const
{
    return d_a04;
}

inline
const EmplacableTestType::ArgType05& EmplacableTestType::arg05() const
{
    return d_a05;
}

inline
const EmplacableTestType::ArgType06& EmplacableTestType::arg06() const
{
    return d_a06;
}

inline
const EmplacableTestType::ArgType07& EmplacableTestType::arg07() const
{
    return d_a07;
}

inline
const EmplacableTestType::ArgType08& EmplacableTestType::arg08() const
{
    return d_a08;
}

inline
const EmplacableTestType::ArgType09& EmplacableTestType::arg09() const
{
    return d_a09;
}

inline
const EmplacableTestType::ArgType10& EmplacableTestType::arg10() const
{
    return d_a10;
}

inline
const EmplacableTestType::ArgType11& EmplacableTestType::arg11() const
{
    return d_a11;
}

inline
const EmplacableTestType::ArgType12& EmplacableTestType::arg12() const
{
    return d_a12;
}

inline
const EmplacableTestType::ArgType13& EmplacableTestType::arg13() const
{
    return d_a13;
}

inline
const EmplacableTestType::ArgType14& EmplacableTestType::arg14() const
{
    return d_a14;
}

inline
bool EmplacableTestType::isEqual(const EmplacableTestType& rhs) const
{
    return d_a01 == rhs.d_a01
        && d_a02 == rhs.d_a02
        && d_a03 == rhs.d_a03
        && d_a04 == rhs.d_a04
        && d_a05 == rhs.d_a05
        && d_a06 == rhs.d_a06
        && d_a07 == rhs.d_a07
        && d_a08 == rhs.d_a08
        && d_a09 == rhs.d_a09
        && d_a10 == rhs.d_a10
        && d_a11 == rhs.d_a11
        && d_a12 == rhs.d_a12
        && d_a13 == rhs.d_a13
        && d_a14 == rhs.d_a14;
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
