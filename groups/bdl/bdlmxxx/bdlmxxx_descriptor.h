// bdlmxxx_descriptor.h                                               -*-C++-*-
#ifndef INCLUDED_BDLMXXX_DESCRIPTOR
#define INCLUDED_BDLMXXX_DESCRIPTOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide type-specific operations for type-agnostic clients.
//
//@CLASSES:
//           bdlmxxx::Descriptor: encapsulation of type-dependent operations
//   bdlmxxx::DescriptorStreamIn: namespace for a 'streamIn' function pointer
//  bdlmxxx::DescriptorStreamOut: namespace for a 'streamOut' function pointer
//
//@SEE_ALSO:
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component defines a container for aggregating function
// pointers that are particularly useful in implementing heterogeneous
// collections.  The 'bdlmxxx::Descriptor' 'struct' encapsulates type-dependent
// operations so that client code can manipulate values of arbitrary types
// without compile-time knowledge of those types.  Each instance of
// 'bdlmxxx::Descriptor' describes attributes of a specific data type and contains
// pointers to functions that operate on that type, ideally concentrating all
// knowledge of a type in one place.
//
// Operations provided by a 'bdlmxxx::Descriptor' include:
//..
//     1. 'unsetConstruct': This pointer stores the effective equivalent of
//        the default constructor for items of the described type.
//     2. 'copyConstruct': This pointer stores the effective equivalent of the
//        copy constructor for items of the described type.
//     3. 'destroy': This pointer stores the effective equivalent of the
//         destructor for items of the described type.
//     4. 'assign': This pointer stores the effective equivalent of 'operator='
//        for items of the described type.
//     5. 'move': This function relocates an item to a new memory location.
//     6. 'makeUnset': This functions sets an item of the described type to the
//        canonical "unset" value for that type.
//     7. 'isUnset': This function determines whether the current value of some
//        item equals the canonical "unset" value for that type.
//     8. 'areEqual': This pointer stores the effective equivalent of
//        'operator==' for items of the described type.
//..
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMXXX_AGGREGATEOPTION
#include <bdlmxxx_aggregateoption.h>
#endif

#ifndef INCLUDED_BDLIMPXXX_BITWISECOPY
#include <bdlimpxxx_bitwisecopy.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

namespace bdlmxxx {
                        // ======================
                        // struct Descriptor
                        // ======================

struct Descriptor {
    // This 'struct' encapsulates type-dependent operations.  Each instance of
    // this 'struct' describes attributes of a specific data type and contains
    // pointer(s) to functions that operate on that type, ideally concentrating
    // all knowledge of a type in one place.
    //
    // This type is implemented as a 'struct' with all public data and no
    // member functions rather than a 'class' because it is desirable to
    // statically construct instances of this type.

    // PUBLIC DATA
    int d_elemEnum;   // enumeration constant value for this type

    int d_size;       // size of objects of type being described
    int d_alignment;  // alignment requirement for type being described

    // Note: The following pointer-to-function variables intentionally do not
    // have the standard 'd_' prefix because they are used as if they were
    // member functions, e.g.:
    //..
    //  attr->move(lhs, rhs);
    //..

    void (*unsetConstruct)(
                      void                                     *obj,
                      AggregateOption::AllocationStrategy  allocationMode,
                      bslma::Allocator                         *alloc);

    void (*copyConstruct)(
                      void                                     *obj,
                      const void                               *rhs,
                      AggregateOption::AllocationStrategy  allocationMode,
                      bslma::Allocator                         *alloc);

    void (*destroy)(void *obj);

    void (*assign)(void *lhs, const void *rhs);

    void (*move)(void *lhs, void *rhs);
        // Pointer to function used to move an object from rhs location to lhs
        // location.  The 'rhs' is *not* 'const' because it is altered
        // (effectively destroyed) by the move operation.

    void (*makeUnset)(void *obj);

    bool (*isUnset)(const void *obj);

    bool (*areEqual)(const void *lhs, const void *rhs);

    bsl::ostream& (*print)(const void    *obj,
                           bsl::ostream&  stream,
                           int            level,
                           int            spacesPerLevel);
};

template <class STREAM>
struct DescriptorStreamIn {
    // This 'struct' provides a namespace for the 'streamIn' function pointer
    // data member.

    void (*streamIn)(
                    void                                  *obj,
                    STREAM&                                stream,
                    int                                    version,
                    const DescriptorStreamIn<STREAM> *strmAttrLookup,
                    const Descriptor                 *const attrLookup[]);
};

template <class STREAM>
struct DescriptorStreamOut {
    // This 'struct' provides a namespace for the 'streamOut' function pointer
    // data member.

    void (*streamOut)(const void                             *obj,
                      STREAM&                                 stream,
                      int                                     version,
                      const DescriptorStreamOut<STREAM> *strmAttrLookup);
};
}  // close package namespace

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
