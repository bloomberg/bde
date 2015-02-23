// bsl_type_traits.h                                                  -*-C++-*-
#ifndef INCLUDE_BSL_NATIVE_TYPE_TRAITS
#define INCLUDE_BSL_NATIVE_TYPE_TRAITS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif


#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#include<type_traits>

namespace bsl
{
    // Import selected symbols into bsl namespace
    using native_std::integral_constant;
    using native_std::true_type;
    using native_std::false_type;
    using native_std::is_array;
    using native_std::is_class;
    using native_std::is_enum;
    using native_std::is_floating_point;
    using native_std::is_integral;
    using native_std::is_lvalue_reference;
    using native_std::is_member_function_pointer;
    using native_std::is_member_object_pointer;
    using native_std::is_pointer;
    using native_std::is_rvalue_reference;
    using native_std::is_union;
    using native_std::is_void;
    using native_std::is_arithmetic;
    using native_std::is_compound;
    using native_std::is_fundamental;
    using native_std::is_member_pointer;
    using native_std::is_object;
    using native_std::is_reference;
    using native_std::is_scalar;
    using native_std::is_abstract;
    using native_std::is_const;
    using native_std::is_empty;
    using native_std::is_literal_type;
    using native_std::is_pod;
    using native_std::is_polymorphic;
    using native_std::is_signed;
    using native_std::is_standard_layout;
    using native_std::is_trivial;
    using native_std::is_trivially_copyable;
    using native_std::is_unsigned;
    using native_std::is_volatile;
    using native_std::has_virtual_destructor;
    using native_std::is_assignable;
    using native_std::is_constructible;
    using native_std::is_copy_assignable;
    using native_std::is_copy_constructible;
    using native_std::is_destructible;
    using native_std::is_default_constructible;
    using native_std::is_move_assignable;
    using native_std::is_move_constructible;
    using native_std::is_trivially_assignable;
    using native_std::is_trivially_constructible;
    using native_std::is_trivially_copy_assignable;
    using native_std::is_trivially_copy_constructible;
    using native_std::is_trivially_destructible;
    using native_std::is_trivially_default_constructible;
    using native_std::is_trivially_move_assignable;
    using native_std::is_trivially_move_constructible;
    using native_std::is_nothrow_assignable;
    using native_std::is_nothrow_constructible;
    using native_std::is_nothrow_copy_assignable;
    using native_std::is_nothrow_copy_constructible;
    using native_std::is_nothrow_destructible;
    using native_std::is_nothrow_default_constructible;
    using native_std::is_nothrow_move_assignable;
    using native_std::is_nothrow_move_constructible;
    using native_std::is_base_of;
    using native_std::is_convertible;
    using native_std::is_same;
    using native_std::alignment_of;
    using native_std::extent;
    using native_std::rank;
    using native_std::add_const;
    using native_std::add_cv;
    using native_std::add_volatile;
    using native_std::remove_const;
    using native_std::remove_cv;
    using native_std::remove_volatile;
    using native_std::add_pointer;
    using native_std::add_lvalue_referenceca;
    using native_std::add_rvalue_reference;
    using native_std::decay;
    using native_std::make_signed;
    using native_std::make_unsigned;
    using native_std::remove_all_extents;
    using native_std::remove_extent;
    using native_std::remove_pointer;
    using native_std::remove_reference;
    using native_std::underlying_type;
    using native_std::aligned_storage;
    using native_std::aligned_union;
    using native_std::common_type;
    using native_std::conditional;
    using native_std::enable_if;
    using native_std::result_of;
    

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    // Import additional names expected by existing code, but not mandated by
    // the standard header.

#endif  // BDE_OMIT_INTERNAL_DEPRECATED
}

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
