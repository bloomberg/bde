// bslstp_exfunctional.h                                              -*-C++-*-
#ifndef INCLUDED_BSLSTP_EXFUNCTIONAL
#define INCLUDED_BSLSTP_EXFUNCTIONAL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a namespace for functional extensions.
//
//@INTERNAL_DEPRECATED: Do not use directly.
//
//@CLASSES:
//  bsl::compose1: create an unary function object adaptor
//  bsl::compose2: create a binary function object adaptor
//  bsl::select1st: select 'first' value of a pair
//  bsl::select2nd: select 'second' value of a pair
//
//@CANONICAL_HEADER: bsl_functional.h
//
//@SEE_ALSO: bsl_functional
//
//@DESCRIPTION: This component is for internal use only.  Please include
// '<bsl_functional.h>' directly.  This component provides a namespace for
// functional extensions that are not specified in the C++ standard.  This
// provides backward compatibility for code using extensions that STLPort
// provides.
//
// Note that the classes in this component are based on STLPort's
// implementation, with copyright notice as follows:
//..
//-----------------------------------------------------------------------------
//
// Copyright (c) 1994
// Hewlett-Packard Company
//
// Copyright (c) 1996-1998
// Silicon Graphics Computer Systems, Inc.
//
// Copyright (c) 1997
// Moscow Center for SPARC Technology
//
// Copyright (c) 1999
// Boris Fomitchev
//
// This material is provided "as is", with absolutely no warranty expressed
// or implied.  Any use is at your own risk.
//
// Permission to use or copy this software for any purpose is hereby granted
// without fee, provided the above notices are retained on all copies.
// Permission to modify the code and to distribute modified code is granted,
// provided the above notices are retained, and a notice that the code was
// modified is included with the above copyright notice.
//-----------------------------------------------------------------------------
//..
//
///Usage
///-----
// This component is for internal use only.

#ifdef BDE_OPENSOURCE_PUBLICATION // STP
#error "bslstp_exfunctional is not for publication"
#endif

#include <bslscm_version.h>

#include <bslmf_istriviallycopyable.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bslstl_equalto.h>

#include <bsls_compilerfeatures.h>
#include <bsls_deprecatefeature.h>

#include <cstring>     // for 'std::strcmp'
#include <functional>  // for 'std::unary_function'

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
# include <bslalg_typetraits.h>
# include <bsls_nativestd.h>
# include <functional>
#endif

namespace bsl {

                    // ========================
                    // class bsl::unary_compose
                    // ========================

template <class OPERATION1, class OPERATION2>
class unary_compose
    // This class provides a function object adaptor for two other functors.
    // When given two adaptable unary functions 'f' and 'g', and when 'g's
    // return type is convertible to 'f's argument type, this class can be used
    // to create a function object 'h' that is equivalent to 'f(g(x))'.
{
  public:
    // public type names
    typedef typename OPERATION2::argument_type  argument_type;
    typedef typename OPERATION1::result_type    result_type;

  protected:
    // DATA
    OPERATION1 d_fn1;  // function object 'f', in 'f(g(x))'
    OPERATION2 d_fn2;  // function object 'g', in 'f(g(x))'

  public:
    // ACCESSORS
    unary_compose(const OPERATION1& x, const OPERATION2& y)
    : d_fn1(x)
    , d_fn2(y) {}

    result_type operator()(const argument_type& x) const {
        return d_fn1(d_fn2(x));
    }

    result_type operator()(argument_type& x) const {
        return d_fn1(d_fn2(x));
    }
};

                    // =========================
                    // class bsl::binary_compose
                    // =========================

template <class OPERATION1, class OPERATION2, class OPERATION3>
class binary_compose
    // This class provides a function object adaptor for three other functors.
    // When given two adaptable unary functions 'g1' and 'g2', and a third
    // adaptable binary function 'f', and if 'g1' and 'g2's argument type is
    // convertible to 'f's argument type, this class can be used to create a
    // function object 'h' that is equivalent to 'f(g1(x), g2(x))'.
{
  public:
    // public type names
    typedef typename OPERATION2::argument_type  argument_type;
    typedef typename OPERATION1::result_type    result_type;

  protected:
    // DATA
    OPERATION1 d_fn1;  // function object 'f',  in 'f(g1(x), g2(x))'
    OPERATION2 d_fn2;  // function object 'g1', in 'f(g1(x), g2(x))'
    OPERATION3 d_fn3;  // function object 'g2', in 'f(g1(x), g2(x))'

  public:
    // CREATORS
    binary_compose(const OPERATION1& fn1,
                   const OPERATION2& fn2,
                   const OPERATION3& fn3)
    : d_fn1(fn1)
    , d_fn2(fn2)
    , d_fn3(fn3) {}

    // ACCESSORS
    result_type operator()(const argument_type& x) const
    {
        return d_fn1(d_fn2(x), d_fn3(x));
    }

    result_type operator()(argument_type& x) const
    {
        return d_fn1(d_fn2(x), d_fn3(x));
    }
};


                    // ====================
                    // class bsl::select1st
                    // ====================

template <class PAIR>
struct select1st
{
    // public type names
    typedef          PAIR             argument_type;
    typedef typename PAIR::first_type result_type;

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(select1st, bsl::is_trivially_copyable);

    // ACCESSORS
    const typename PAIR::first_type& operator()(const PAIR& x) const
    {
        return x.first;
    }
};

                    // ====================
                    // class bsl::select2nd
                    // ====================

template <class PAIR>
struct select2nd
{
    // public type names
    typedef          PAIR              argument_type;
    typedef typename PAIR::second_type result_type;

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(select2nd, bsl::is_trivially_copyable);

    // ACCESSORS
    const typename PAIR::second_type& operator()(const PAIR& x) const
    {
        return x.second;
    }
};

// FREE FUNCTIONS
template <class OPERATION1, class OPERATION2>
inline
unary_compose<OPERATION1, OPERATION2>
compose1(const OPERATION1& fn1, const OPERATION2& fn2)
    // Return an 'unary_compose' function object constructed using the
    // specified 'fn1' and 'fn2' unary functions.  The returned function object
    // is equivalent to 'fn1(fn2(x))'.
{
    return unary_compose<OPERATION1, OPERATION2>(fn1, fn2);
}

template <class OPERATION1, class OPERATION2, class OPERATION3>
inline
binary_compose<OPERATION1, OPERATION2, OPERATION3>
compose2(const OPERATION1& fn1,
         const OPERATION2& fn2,
         const OPERATION3& fn3)
    // Return a 'binary_compose' function object constructed using the
    // specified 'fn1' binary function, and the specified 'fn2' and 'fn3' unary
    // functions.  The returned function object is equivalent to
    // 'fn1(fn2(x), fn3(x))'.
{
    return binary_compose<OPERATION1, OPERATION2, OPERATION3>(fn1, fn2, fn3);
}

                    // ===========================
                    // class bsl::StringComparator
                    // ===========================

struct StringComparator
    // This class is a functor that provides comparison between two
    // 'const char *' strings.
{
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(StringComparator,
                                   bsl::is_trivially_copyable);

    // ACCESSORS
    bool operator()(const char *a, const char *b) const
        // Return 'true' if the specified strings 'a' and 'b' have the same
        // value, and return 'false' otherwise.  Two strings have the same
        // value if the result of 'std::strcmp' returns 0.
    {

        return 0 == std::strcmp(a, b);
    }
};

                    // =============================
                    // class bsl::ComparatorSelector
                    // =============================

template <class HASH_KEY>
struct ComparatorSelector
    // This meta-function selects the appropriate implementation for comparing
    // the parameterized 'TYPE'.  This generic template uses the
    // 'std::equal_to' functor.
{
    // TYPES
    typedef bsl::equal_to<HASH_KEY> Type;
};

template <>
struct ComparatorSelector<const char *>
    // This meta-function is specialized for 'const char *', and uses the
    // 'StringComparator' functor to compare the string values.
{
    // TYPES
    typedef StringComparator Type;
};

}  // close namespace bsl

namespace BloombergLP {
namespace bslstp {

                    // ======================
                    // class bslstp::Identity
                    // ======================

template <class TYPE>
struct Identity {
    // An identity function.

    // TYPES
    typedef TYPE argument_type;
    typedef TYPE result_type;

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Identity, bsl::is_trivially_copyable);

    // ACCESSORS
    const TYPE& operator()(const TYPE& x) const;
        // Return a const reference to the specified 'x'.
};

//=============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
//=============================================================================

template <class TYPE>
inline
const TYPE& Identity<TYPE>::operator()(const TYPE& x) const
{
    return x;
}

} // close package namespace
} // close enterprise namespace

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
