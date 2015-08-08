// balj_journalpageheader.h                                          -*-C++-*-
#ifndef INCLUDED_BALJ_JOURNALPAGEHEADER
#define INCLUDED_BALJ_JOURNALPAGEHEADER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a container for the header information of a journal page.
//
//@CLASSES:
//  balj::JournalPageHeader: header information for a page in a journal
//
//@SEE_ALSO: balj_journalheader, balj_journalparameters, balj_journal
//
//@AUTHOR: Oleg Semenov (osemenov), Andrei Basov (abasov)
//
//@DESCRIPTION: This component defines a class, 'balj::JournalPageHeader', that
// contains header information for a journal page.
//
///Usage
///-----
// This component is meant to used by higher level components in the 'baecs'
// package and should not be used directly by client code.  Therefore, a usage
// example is not provided.

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BDLB_BIGENDIAN
#include <bdlb_bigendian.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

namespace BloombergLP {

namespace balj {
                   // ===========================================
                   // class JournalPageHeader_TransactionId
                   // ===========================================

class JournalPageHeader_TransactionId {
    // TBD class-level doc

    // DATA
    bdlb::BigEndianInt64 d_data[4];
    // Four copies of the transaction id are stored to make the update atomic.
    // Assuming the data in a block on disk is updated linearly (but not
    // necessarily in any particular direction), and assuming all four
    // values are kept in the same block, we can be sure that either the
    // first or the last pair will be consistent regardless of what part of
    // the update has completed.

    // FRIENDS
    friend bsl::ostream& operator<<(
                                 bsl::ostream&,
                                 const JournalPageHeader_TransactionId&);

  public:
    // MANIPULATORS
    void setValue(const bsls::Types::Int64& rhs)
    {
        // TBD: Should we be concerned with the reordering of writes?
        /*
        *(volatile bdlb::BigEndianInt64*)(d_data+0) = rhs;
        *(volatile bdlb::BigEndianInt64*)(d_data+1) = rhs;
        *(volatile bdlb::BigEndianInt64*)(d_data+2) = rhs;
        *(volatile bdlb::BigEndianInt64*)(d_data+3) = rhs;
        */
        d_data[0] = rhs;
        d_data[1] = rhs;
        d_data[2] = rhs;
        d_data[3] = rhs;
        // full barrier - provided by d_workIndexLock
    }

    // ACCESSORS
    int getValue(bsls::Types::Int64 *outValue) const
    {
        if (d_data[0] == d_data[1]) {
            *outValue = d_data[0];
            return 0;
        } else if (d_data[2] == d_data[3]) {
            *outValue = d_data[2];
            return 0;
        } else {
            return 1;
        }
    }
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&                                stream,
                         const JournalPageHeader_TransactionId& rhs);

                         // =============================
                         // class JournalPageHeader
                         // =============================

class JournalPageHeader {
    // TBD doc

  public:
    // TYPES
    enum {
        k_INVALID_RECORD_HANDLE = 0xFFFFFFFF
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , BAECS_INVALID_RECORD_HANDLE = k_INVALID_RECORD_HANDLE
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
    };

    enum {
        k_ATTRIBUTES_SHIFT    = 16
      , k_ATTRIBUTES_MASK     = 0xFFFF0000
      , k_NEXT_BLOCK_SHIFT    = 3
      , k_NEXT_BLOCK_MASK     = 0x0000FFF8
      , k_CONFIRMED_SHIFT     = 2
      , k_CONFIRMED_MASK      = 0x00000004
      , k_EXTENDED_SHIFT      = 1
      , k_EXTENDED_MASK       = 0x00000002
      , k_HEAD_SHIFT          = 0
      , k_HEAD_MASK           = 0x00000001
      , k_INDEX_NONE          = k_NEXT_BLOCK_MASK >> k_NEXT_BLOCK_SHIFT
      , k_MAX_BLOCKS_PER_PAGE = k_INDEX_NONE
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , BAECS_ATTRIBUTES_SHIFT    = k_ATTRIBUTES_SHIFT
      , BAECS_ATTRIBUTES_MASK     = k_ATTRIBUTES_MASK
      , BAECS_NEXT_BLOCK_SHIFT    = k_NEXT_BLOCK_SHIFT
      , BAECS_NEXT_BLOCK_MASK     = k_NEXT_BLOCK_MASK
      , BAECS_CONFIRMED_SHIFT     = k_CONFIRMED_SHIFT
      , BAECS_CONFIRMED_MASK      = k_CONFIRMED_MASK
      , BAECS_EXTENDED_SHIFT      = k_EXTENDED_SHIFT
      , BAECS_EXTENDED_MASK       = k_EXTENDED_MASK
      , BAECS_HEAD_SHIFT          = k_HEAD_SHIFT
      , BAECS_HEAD_MASK           = k_HEAD_MASK
      , BAECS_INDEX_NONE          = k_INDEX_NONE
      , BAECS_MAX_BLOCKS_PER_PAGE = k_MAX_BLOCKS_PER_PAGE
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
    };

    class BlockHeader {
        // TBD doc

        // DATA
        bdlb::BigEndianUint32 d_nextRecord;
        bdlb::BigEndianUint32 d_prevRecord;
        bdlb::BigEndianUint32 d_extraData;

      public:
        // MANIPULATORS
        void init();
        void setNextRecord(unsigned nextRecord);
        void setPrevRecord(unsigned prevRecord);
        void setAttributes(unsigned attributes);
        void setNextBlock(unsigned nextBlock);
        void setConfirmed(bool isConfirmed);
        void setExtended(bool isExtended);
        void setHead(bool isHead);

        // ACCESSORS
        unsigned nextRecord() const;
        unsigned prevRecord() const;
        unsigned attributes() const;
        unsigned nextBlock() const;
        bool isConfirmed() const;
        bool isExtended() const;
        bool isHead() const;
    };

  private:
    // DATA
    JournalPageHeader_TransactionId d_transactionId;
    bdlb::BigEndianUint32                 d_numBlocks;
    bdlb::BigEndianUint32                 d_numAvailableBlocks;
    bdlb::BigEndianUint32                 d_numReleasedBlocks;
    bdlb::BigEndianUint32                 d_firstAvailableBlock;
    bdlb::BigEndianUint32                 d_firstReleasedBlock;
    bdlb::BigEndianUint32                 d_lastReleasedBlock;
    bdlb::BigEndianUint32                 d_isOnFillList;
    bdlb::BigEndianUint32                 d_nextPage;
    BlockHeader                           d_blocks[4];

  public:
    // CLASS METHODS
    static unsigned getSize(unsigned numBlocks);
        // Return the number of bytes required to store a
        // 'JournalPageHeader' with the specified 'numBlocks' in memory.

    // MANIPULATORS
    void copyFrom(const JournalPageHeader& other,
                  const bsls::Types::Int64&      transactionId);

    void init(unsigned                  numBlocks,
              const bsls::Types::Int64& transactionId);
        // Initialize this 'JournalPageHeader' structure having the
        // specified 'numBlocks', placing all blocks on the available list,
        // and setting the current transaction id to the specified
        // 'transactionId'.

    void setIsOnFillList(bool isOnFillList)
    {
        d_isOnFillList = (unsigned)isOnFillList;
    }

    unsigned allocate(unsigned numBlocks, unsigned attributes);
        // Allocate a list of the specified 'numBlocks' from the available
        // list of this 'JournalPageHeader', assigning the specified
        // 'attributes' to the head of the list, and setting flags to zero.
        // Returns the index of the head.  The behavior is undefined unless
        // 'numBlocks' is less or equal than the the number of available
        // blocks.

    int release(unsigned first);
        // Free the list of blocks (previously allocated with 'allocate'),
        // placing the blocks on the released list.  The behavior is undefined
        // unless 'first' is a valid block index previously returned from
        // 'allocate'.  Note that the blocks on the released list do not
        // become available for allocation until a call to 'purge'.

    int deallocate(unsigned first);
        // Free the list of blocks previously allocated with 'allocate',
        // placing the blocks directly on an available list.  The behavior is
        // undefined unless 'first' is a valid block index previously returned
        // from 'allocate'.

    int purge();
        // Moves the contents of the released list to the available list,
        // making the blocks available for allocation.

    int markFreeListsForValidation(unsigned *map,
                                   bool      verbose,
                                   unsigned  offset) const;
    int markBlockListForValidation(unsigned  index,
                                   unsigned  numBlocks,
                                   unsigned  blockType,
                                   unsigned *map,
                                   bool      verbose,
                                   unsigned  offset) const;

    void setNextPage(unsigned nextPage)
    {
        d_nextPage = nextPage;
    }

    void setTransactionId(const bsls::Types::Int64& id);

    // TBD
    BlockHeader *block(int n)
    {
        return d_blocks + n;
    }

    const BlockHeader *block(int n) const
    {
        return d_blocks + n;
    }

    bool isOnFillList() const;

    unsigned nextPage() const;
    unsigned numBlocks() const;
    unsigned numFreeBlocks() const;
    unsigned numAvailableBlocks() const;
    int getTransactionId(bsls::Types::Int64 *outValue) const;

    void print(bsl::ostream& stream) const;
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&                               stream,
                         const JournalPageHeader::BlockHeader& rhs);
}  // close package namespace
    // TBD doc

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                   // -------------------------------------------
                   // class balj::JournalPageHeader_TransactionId
                   // -------------------------------------------

// FREE OPERATORS
inline
bsl::ostream& balj::operator<<(bsl::ostream&                                stream,
                         const JournalPageHeader_TransactionId& rhs)
{
   return stream << "["
                 << rhs.d_data[0] << ","
                 << rhs.d_data[1] << ","
                 << rhs.d_data[2] << ","
                 << rhs.d_data[3] << "]";
}

namespace balj {
                         // -----------------------------
                         // class JournalPageHeader
                         // -----------------------------

// ACCESSORS
inline
bool JournalPageHeader::isOnFillList() const
{
    return d_isOnFillList;
}

inline
unsigned JournalPageHeader::nextPage() const
{
    return d_nextPage;
}

inline
unsigned JournalPageHeader::numBlocks() const
{
    return d_numBlocks;
}

inline
unsigned JournalPageHeader::numFreeBlocks() const
{
    return d_numAvailableBlocks + d_numReleasedBlocks;
}

inline
unsigned JournalPageHeader::numAvailableBlocks() const
{
    return d_numAvailableBlocks;
}

inline
int JournalPageHeader::getTransactionId(
                                            bsls::Types::Int64 *outValue) const
{
    return d_transactionId.getValue(outValue);
}

inline
void  JournalPageHeader::setTransactionId(const bsls::Types::Int64& id)
{
    d_transactionId.setValue(id);
}
}  // close package namespace

                     // ------------------------------------------
                     // class balj::JournalPageHeader::BlockHeader
                     // ------------------------------------------

// MANIPULATORS
inline
void balj::JournalPageHeader::BlockHeader::init()
{
    d_nextRecord = 0xBEEFDEAD;
    d_prevRecord = 0xBEEDDEAD;
    d_extraData  = 0;
}

inline
void balj::JournalPageHeader::BlockHeader::setNextRecord(unsigned nextRecord)
{
    d_nextRecord = nextRecord;
}

inline
void balj::JournalPageHeader::BlockHeader::setPrevRecord(unsigned prevRecord)
{
    d_prevRecord = prevRecord;
}

inline
void balj::JournalPageHeader::BlockHeader::setAttributes(unsigned attributes)
{
    d_extraData = (d_extraData & ~k_ATTRIBUTES_MASK)
            | ((attributes << k_ATTRIBUTES_SHIFT) & k_ATTRIBUTES_MASK);
}

inline
void balj::JournalPageHeader::BlockHeader::setNextBlock(unsigned nextBlock)
{
    d_extraData = (d_extraData & ~k_NEXT_BLOCK_MASK)
             | ((nextBlock << k_NEXT_BLOCK_SHIFT) & k_NEXT_BLOCK_MASK);
}

inline
void balj::JournalPageHeader::BlockHeader::setConfirmed(bool isConfirmed)
{
    d_extraData = (d_extraData & ~k_CONFIRMED_MASK)
   | (((unsigned)isConfirmed << k_CONFIRMED_SHIFT) & k_CONFIRMED_MASK);
}

inline
void balj::JournalPageHeader::BlockHeader::setExtended(bool isExtended)
{
    d_extraData = (d_extraData & ~k_EXTENDED_MASK)
      | (((unsigned)isExtended << k_EXTENDED_SHIFT) & k_EXTENDED_MASK);
}

inline
void balj::JournalPageHeader::BlockHeader::setHead(bool isHead)
{
    d_extraData = (d_extraData & ~k_HEAD_MASK)
                  | (((unsigned)isHead << k_HEAD_SHIFT) & k_HEAD_MASK);
}

// ACCESSORS
inline
unsigned balj::JournalPageHeader::BlockHeader::nextRecord() const
{
    return d_nextRecord;
}

inline
unsigned balj::JournalPageHeader::BlockHeader::prevRecord() const
{
    return d_prevRecord;
}

inline
unsigned balj::JournalPageHeader::BlockHeader::attributes() const
{
    return (d_extraData & k_ATTRIBUTES_MASK) >> k_ATTRIBUTES_SHIFT;
}

inline
unsigned balj::JournalPageHeader::BlockHeader::nextBlock() const
{
    return (d_extraData & k_NEXT_BLOCK_MASK) >> k_NEXT_BLOCK_SHIFT;
}

inline
bool balj::JournalPageHeader::BlockHeader::isConfirmed() const
{
    return 0 != (d_extraData & k_CONFIRMED_MASK);
}

inline
bool balj::JournalPageHeader::BlockHeader::isExtended() const
{
    return 0 != (d_extraData & k_EXTENDED_MASK);
}

inline
bool balj::JournalPageHeader::BlockHeader::isHead() const
{
    return 0 != (d_extraData & k_HEAD_MASK);
}

// FREE OPERATORS
inline
bsl::ostream& balj::operator<<(bsl::ostream&                               stream,
                         const JournalPageHeader::BlockHeader& rhs)
{
    return stream << "{"
                  << rhs.nextRecord() << ","
                  << rhs.prevRecord() << ","
                  << rhs.nextBlock()  << ","
                  << "A=" << rhs.attributes() << ","
                  << (rhs.isHead() ?      "H":"")
                  << (rhs.isExtended() ?  "E":"")
                  << (rhs.isConfirmed() ? "C":"")
                  << "}";
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
