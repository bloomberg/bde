// bdem_aggregateimp.h           -*-C++-*-
#ifndef INCLUDED_BDEM_AGGREGATEIMP
#define INCLUDED_BDEM_AGGREGATEIMP

//@PURPOSE: Abstract the commonality of memory management for list and table.
//
//@CLASSES:
//    bdem_AggregateImp: combined row and row-memory-allocator information
//
//@SEE_ALSO: bdem_AggregateOption, bdem_RowDef
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component provides a class, 'bdem_AggregateImp', that
// couples the primary 'bdem_Row' mechanism (i.e., a 'bdem_RowDef') together
// with the allocators that will provide both the row's own memory, and memory
// for the elements contained within the row.
//
// In particular, 'bdem_AggregateImp' constructs and manages a 'bdem_RowDef'
// object in memory obtained from an allocator provided at construction; the
// row definition is configured with auxiliary allocators as indicated by the
// allocation strategy, which is also suppied by the user at construction.
// Modifiable and non-modifiable access to this row definition object is
// provided via the overloaded 'def' methods; accessor methods also grant
// non-modifiable access to the allocators and allocation strategies that were
// bound to the row definition at its construction.
//
///USAGE:
///------

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEM_AGGREGATEOPTION
#include <bdem_aggregateoption.h>
#endif

#ifndef INCLUDED_BDEM_ROWDEF
#include <bdem_rowdef.h>
#endif

#ifndef INCLUDED_BDEMF_ISBITWISE
#include <bdemf_isbitwise.h>
#endif

#ifndef INCLUDED_BDES_PLATFORMUTIL
#include <bdes_platformutil.h>
#endif

namespace BloombergLP {



class bdema_Allocator;
struct bdem_Descriptor;

                        // =======================
                        // class bdem_AggregateImp
                        // =======================

class bdem_AggregateImp {
    // This class implements the aspects of memory management (i.e., allocator
    // usage) that are common to all containers based on 'bdem_RowDef'.  In
    // particular, this class constructs a 'bdem_RowDef' object in memory
    // obtained from an allocator indicated at construction, and provides
    // modifiable and non-modifiable access to this row definition object via
    // the overloaded 'def' methods.
    //
    // Note that 'bdem_AggregateImp' is *not* value-semantic.

    bdem_AggregateOption::AllocationStrategy  d_allocMode;
    bdema_Allocator                          *d_originalAllocator;
    bdema_Allocator                          *d_internalAllocator;
    bdem_RowDef                              *d_rowDef;

  private:
    // NOT IMPLEMENTED
    bdem_AggregateImp(const bdem_AggregateImp& original);
    bdem_AggregateImp& operator=(const bdem_AggregateImp& rhs);

    // PRIVATE INITIALISERS
    void init(bdem_AggregateOption::AllocationStrategy  allocMode,
              int                                       initialMemory,
              bdema_Allocator                          *basicAllocator);
    void init(const bdem_ElemType::Type                 elementTypes[],
              int                                       numElements,
              const bdem_Descriptor *const              attrLookupTbl[],
              bdem_AggregateOption::AllocationStrategy  allocMode,
              int                                       initialMemory,
              bdema_Allocator                          *basicAllocator);
    void init(const bdem_RowDef&                        def,
              bdem_AggregateOption::AllocationStrategy  allocMode,
              int                                       initialMemory,
              bdema_Allocator                          *basicAllocator);
    void init(const bdem_RowHeader&                     row,
              bdem_AggregateOption::AllocationStrategy  allocMode,
              int                                       initialMemory,
              bdema_Allocator                          *basicAllocator);

  public:
    // TYPES
    class InitialMemory {
        // PRIVATE DATA MEMBERS
        int d_i;

      public:
        // CREATORS
        explicit InitialMemory(int i) : d_i(i) { }
        ~InitialMemory() { }

        // ACCESSORS
        operator int() const { return d_i; }
    };

    // PUBLIC CLASS FUNCTIONS
    static bdema_Allocator *makeInternalAllocator(
        bdem_AggregateOption::AllocationStrategy  allocMode,
        int                                       initialMemory,
        bdema_Allocator                          *originalAllocator);
       // Given an allocator and allocation mode, return a pointer to the
       // internal allocator that should be used for all future allocations.
       // The internal allocator may be hte same as the original allocator or
       // it may be a managed allocator allocated from the original allocator.

    // CREATORS
    bdem_AggregateImp(
                 bdem_AggregateOption::AllocationStrategy  allocMode,
                 bdema_Allocator                          *basicAllocator = 0);
    bdem_AggregateImp(
                 bdem_AggregateOption::AllocationStrategy  allocMode,
                 const InitialMemory&                      initialMemory,
                 bdema_Allocator                          *basicAllocator = 0);
        // Create an aggregate implementation object that will use the
        // specified 'allocMode' and the optionally specified 'basicAllocator'
        // to construct a held and managed 'bdem_RowDef' object.  If
        // 'basicAllocator' is 0,the currently installed default allocator is
        // used.  Optionally specify the 'initialMemory' that will be used to
        // satisfy allocation requests without replenishment (i.e., without
        // internal allocation).  Note that 'initialMemory' has no effect
        // unless 'allocMode' is 'WRITE_ONCE' or 'WRITE_MANY'.

    bdem_AggregateImp(
                 const bdem_ElemType::Type                 elementTypes[],
                 int                                       numElements,
                 const bdem_Descriptor *const              attrLookupTbl[],
                 bdem_AggregateOption::AllocationStrategy  allocMode,
                 bdema_Allocator                          *basicAllocator = 0);
    bdem_AggregateImp(
                 const bdem_ElemType::Type                 elementTypes[],
                 int                                       numElements,
                 const bdem_Descriptor *const              attrLookupTbl[],
                 bdem_AggregateOption::AllocationStrategy  allocMode,
                 const InitialMemory&                      initialMemory,
                 bdema_Allocator                          *basicAllocator = 0);
        // Create an aggregate implementation object that will use the
        // specified 'allocMode' and the optionally specified 'basicAllocator'
        // to construct a held and managed 'bdem_RowDef' object containing the
        // specified 'numElements' entries whose element types match the
        // specified 'elementTypes' (and are described by the specified
        // 'attrLookupTbl') at each index position.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  Optionally
        // specify the 'initialMemory' that will be used to satisfy allocation
        // requests without replenishment (i.e., without internal allocation).
        // Note that 'initialMemory' has no effect unless 'allocMode' is
        // 'WRITE_ONCE' or 'WRITE_MANY'.

    bdem_AggregateImp(
                 const bdem_RowDef&                        def,
                 bdem_AggregateOption::AllocationStrategy  allocMode,
                 bdema_Allocator                          *basicAllocator = 0);
    bdem_AggregateImp(
                 const bdem_RowDef&                        def,
                 bdem_AggregateOption::AllocationStrategy  allocMode,
                 const InitialMemory&                      initialMemory,
                 bdema_Allocator                          *basicAllocator = 0);
        // Create an aggregate implementation object that will use the
        // specified 'allocMode' and the optionally specified 'basicAllocator'
        // to construct a held and managed 'bdem_RowDef' object containing
        // elements matching the sequence of elements in the specified 'def'
        // row definition.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  Optionally specify the 'initialMemory'
        // that will be used to satisfy allocation requests without
        // replenishment (i.e., without internal allocation).  Note that
        // 'initialMemory' has no effect unless 'allocMode' is 'WRITE_ONCE' or
        // 'WRITE_MANY'.

    bdem_AggregateImp(
                 const bdem_RowHeader&                     row,
                 bdem_AggregateOption::AllocationStrategy  allocMode,
                 bdema_Allocator                          *basicAllocator = 0);
    bdem_AggregateImp(
                 const bdem_RowHeader&                     row,
                 bdem_AggregateOption::AllocationStrategy  allocMode,
                 const InitialMemory&                      initialMemory,
                 bdema_Allocator                          *basicAllocator = 0);
        // Create an aggregate implementation object that will use the
        // specified 'allocMode' and the optionally specified 'basicAllocator'
        // to construct a held and managed 'bdem_RowDef' object containing
        // elements matching the sequence of elements in the row definition
        // held by the specified 'row' header object.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.  Optionally
        // specify the 'initialMemory' that will be used to satisfy allocation
        // requests without replenishment (i.e., without internal allocation).
        // Note that 'initialMemory' has no effect unless 'allocMode' is
        // 'WRITE_ONCE' or 'WRITE_MANY'.

    ~bdem_AggregateImp();

    // MANIPULATORS
    bdem_RowDef& def();
        // Return a reference to the modifiable row definition owned by this
        // aggregate implementation.

    template <class STREAM>
    STREAM&
    bdexStreamIn(STREAM&                      stream,
                 int                          version,
                 const bdem_Descriptor *const attrLookup[]);
        // Extract the row definitition for this aggregate from the specified
        // 'stream' using the format specified by 'version'. 'attrLookup' maps
        // each known element type to a descriptor (see bdem_descriptor).  Any
        // elements that were stored in the held row definition will be
        // discarded.  If the 'stream' is in an invalid state or becomes
        // invalid during the operation, then the row definition remains
        // unchanged.

    void reserveMemory(int numBytes);
        // Reserve sufficient memory to satisfy allocation requests for at
        // least the specified 'numBytes' without replenishment (i.e., without
        // internal allocation).  The behavior is undefined unless
        // 0 <= numBytes.  Note that this method has no effect unless the
        // internal allocation mode is 'WRITE_ONCE' or 'WRITE_MANY'.

    template <class STREAM>
    STREAM&
    streamIn(STREAM&                      stream,
             int                          version,
             const bdem_Descriptor *const attrLookup[]);
        // Extract the row definitition for this aggregate from the specified
        // 'stream' using the format specified by 'version'.
        //
        // DEPRECATED: Replaced by 'bdexStreamIn(stream, version, attrLookup)'

    // ACCESSORS
    const bdem_RowDef& def() const;
        // Return a reference to the non-modifiable row definition object owned
        // by this aggregate implementation.

    bdem_AggregateOption::AllocationStrategy originalAllocationMode() const;
        // Return the allocation strategy specified by the client upon
        // construction of this 'bdem_AggregateImp'.  Note that that strategy
        // has been used to construct the 'bdem_RowDef' object that is held and
        // managed by this object.

    bdem_AggregateOption::AllocationStrategy internalAllocationMode() const;
        // Return 'bdem_AggregateOption::NODESTRUCT_FLAG' if the original
        // allocator to this 'bdem_AggregateImp' had that flag set, and 0
        // otherwise.  Note that that allocator has been used to construct the
        // 'bdem_RowDef' object that is held and managed by this object.

    bdema_Allocator *originalAllocator() const;
        // Return the address of the allocator created from the allocator
        // passed to the 'bdem_AggregateImp' upon construction.  Note that that
        // allocator has been used to construct the 'bdem_RowDef' object that
        // is held and managed by this object.

    bdema_Allocator *internalAllocator() const;
        // Return the address of the internal allocator created by this
        // 'bdem_AggregateImp' for use by the held 'bdem_RowDef' and by the
        // row's associated entries.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // If the specified 'version' is 1, insert the elements of the
        // 'bdem_RowDef' object held by this aggregate implementation into the
        // specified 'stream'.  The behavior is undefined unless 'version'
        // equals 1.

    template <class STREAM>
    STREAM& streamOutRaw(STREAM& stream, int version) const;
        // If the specified 'version' is 1, insert the elements of the
        // 'bdem_RowDef' object held by this aggregate implementation into the
        // specified 'stream'.  The behavior is undefined unless 'version'
        // equals 1.
        //
        // DEPRECATED: replaced by 'bdexStreamOut(stream, version)'
};

// NON-MEMBER TRAITS
template <>
struct bdemf_IsBitwiseMoveable<bdem_AggregateImp> { enum {VALUE = 1}; };

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

inline
bdem_RowDef& bdem_AggregateImp::def()
{
    return *d_rowDef;
}

template <class STREAM>
STREAM&
bdem_AggregateImp::bdexStreamIn(STREAM&                      stream,
                                int                          version,
                                const bdem_Descriptor *const attrLookup[])
{
    switch (version) {  // Switch on the schema version (starting with 1).
      case 1: {
          bdem_RowDef rowDef(0, internalAllocationMode(), internalAllocator());
          rowDef.bdexStreamIn(stream, 1, attrLookup);
          bdeimp_BitwiseCopy<bdem_RowDef>::swap(d_rowDef, &rowDef);
      } break;
      default: {
          stream.invalidate();
      }
    }

    return stream;
}

template <class STREAM>
inline STREAM&
bdem_AggregateImp::streamIn(STREAM&                      stream,
                            int                          version,
                            const bdem_Descriptor *const attrLookup[])
{
    return bdexStreamIn(stream, version, attrLookup);
}

// ACCESSORS
inline
const bdem_RowDef& bdem_AggregateImp::def() const
{
    return *d_rowDef;
}

inline
bdem_AggregateOption::AllocationStrategy
                              bdem_AggregateImp::originalAllocationMode() const
{
    return d_allocMode;
}

inline
bdem_AggregateOption::AllocationStrategy
                              bdem_AggregateImp::internalAllocationMode() const
{
    return (bdem_AggregateOption::AllocationStrategy)
                         (d_allocMode & bdem_AggregateOption::NODESTRUCT_FLAG);
}

inline
bdema_Allocator *bdem_AggregateImp::originalAllocator() const
{
    return d_originalAllocator;
}

inline
bdema_Allocator *bdem_AggregateImp::internalAllocator() const
{
    return d_internalAllocator;
}

template <class STREAM>
inline
STREAM& bdem_AggregateImp::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
          d_rowDef->bdexStreamOut(stream, 1);
      } break;
    }

    return stream;
}
template <class STREAM>
inline
STREAM& bdem_AggregateImp::streamOutRaw(STREAM& stream, int version) const
{
    return bdexStreamOut(stream, version);
}



}  // close namespace BloombergLP


#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
